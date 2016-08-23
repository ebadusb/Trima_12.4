/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      statecontrol.cpp
 *
 */

#include "statecontrol.h"
#include "override_alr_msg.h"
#include "recoveryman.h"
#include "recoverytypes.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "procdata.h"
#include "predict_msgs.h"

StateControl::StateControl()
   : StateAbs(),
     _AlarmOverrideMsg(Callback<StateControl>(this, &StateControl::receivedOverride)),
     _AKORequestMsg(Callback<StateControl>(this, &StateControl::akoRequested) ),
     _RBCUncalibratedMsg(Callback<StateControl>(this, &StateControl::rbcDetectorUncalibrated) ),
     _PauseAlarm(PAUSE_BUTTON_ALARM),
     _StopAlarm(STOP_BUTTON_ALARM),
     _CentStopTimer(0, Callback<StateControl>(this, &StateControl::systemOutOfControl), TimerMessage::DISARMED),
     _CentStopTimerStarted(false),
     _PumpsStopTimer(0, Callback<StateControl>(this, &StateControl::systemOutOfControl), TimerMessage::DISARMED),
     _PumpsStopTimerStarted(false),
     _UnlockDoor(false),
     _RBCCalibrator(),
     _AKORequested(false),
     _AKOCompleted(true),
     _AKOVolume(0.0f),
     _LastSeenCentSpeed(0.0f),
     _PredictTimer(0, Callback<StateControl>(this, &StateControl::predictTimeout), TimerMessage::DISARMED),
     _PowerRequestMsg(Callback<StateControl>(this, &StateControl::powerRequestCb) ),
     _PerformSalineBolus(false),
     _SalineBolusRequestMsg(Callback<StateControl>(this, &StateControl::salineBolusRequest)),
     _predictTime(),
     _inHalfSpeedAPS(false),
     _overrideChanged(false),
     _lastQin(0.0f),
     _lastQac(0.0f),
     _PPRcheck (false)
{}

StateControl::~StateControl()
{}

int StateControl::init ()
{
   ProcData pd;

   //
   // Initialize the RBC Calibrator ...
   //
   _RBCCalibrator.init();
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().RBCDetectorCalibrated.Set(false);
   pd.MakeTrimaSetReadable();

   //
   // Save the current alarm overrides ...
   //
   _PreviousAlarmOverrides = _AlarmOverrideMsg.getOverrides();

   return StateAbs::init();

}

int StateControl::preProcess ()
{
   ProcData pd;

   //
   // Set the paused flag immediately if it
   //  has been set via the alarm override message ...
   //
   pd.AlarmActive(_AlarmOverrideMsg.AlarmActive());

   //
   // Activate/Inactivate the timers ...
   //
   if (    !pd.AlarmActive()
           && (    RecoveryMan::inRecovery() == RecoveryTypes::NoRecovery
                   || RecoveryMan::inRecovery() == RecoveryTypes::CountAll
                   || RecoveryMan::inRecovery() == RecoveryTypes::CountTime )
           )
      pd.ProcTime().activate();
   else
      pd.ProcTime().inactivate();

   //
   // Check for RBC Calibration ...
   //
   calibrateRBCDetector();

   //
   // Set the centrifuge pressure limit ...
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CentPresLimit.Set(CobeConfig::data().CentPresLimitLow);
   pd.MakeTrimaSetReadable();

   return NORMAL;
}

int StateControl::postProcess ()
{
   ProcData pd;

   //
   // Set alarm overrides ...
   //
   alarmOverrides();

   //
   // Check for Stop or Pause button presses ...
   //
   if ( pd.Status().StopSwitch() )
   {
      //
      // Send the stop alarm ...
      //
      _StopAlarm.setAlarm();
      DataLog(log_level_proc_info) << "Stop button pressed" << endmsg;

      //
      // Request a centrifuge stop recovery now ...
      //
      CentrifugeStopMsg centStop(MessageBase::SEND_LOCAL);
      centStop.send(1);

      //
      // Unlock the door ...
      //
      pd.Orders().UnlockDoor();

      //
      // Stop the centrifuge ...
      //
      pd.Orders().RPM(0.0f);

      pd.DoorLockTestFinished();

   }
   else if ( pd.Status().PauseSwitch() )
   {
      //
      // Send the Pause Alarm ...
      //
      _PauseAlarm.setAlarm();
      DataLog(log_level_proc_info) << "Pause button pressed" << endmsg;

   }

   //
   // Check door commands ...
   //
   doDoorCommands();

   //
   // Do Access Keep Open procedures last ...
   //
   doAKO();

   //
   // Determine proper centrifuge speed ...
   //
   doCentrifugeCommands();

   //
   // Check commands vs. actual ...
   //
   checkCommands();

   //
   //  Perform predict communication
   //
   predictCommunication();

   return NORMAL;
}

void StateControl::receivedOverride ()
{
   //
   // Stop any SalineBolus activity...
   //
   if ( _PerformSalineBolus )
   {
      if (    _AlarmOverrideMsg.dispAlarmID() != REPLACEMENT_FLUID_ALARM
              && _AlarmOverrideMsg.dispAlarmID() != REPLACEMENT_FLUID_BOLUS_ALARM
              && _AlarmOverrideMsg.dispAlarmID() != REPLACEMENT_FLUID_NEW_BAG_ALARM
              && _AlarmOverrideMsg.dispAlarmID() != REPLACEMENT_FLUID_ALARM_RINSEBACK
              && _AlarmOverrideMsg.respAlarmID() != REPLACEMENT_FLUID_ALARM
              && _AlarmOverrideMsg.respAlarmID() != REPLACEMENT_FLUID_BOLUS_ALARM
              && _AlarmOverrideMsg.respAlarmID() != REPLACEMENT_FLUID_NEW_BAG_ALARM
              && _AlarmOverrideMsg.respAlarmID() != REPLACEMENT_FLUID_ALARM_RINSEBACK )
      {
         SalineBolusMsg stopBolusMsg(MessageBase::SEND_LOCAL);
         stopBolusMsg.send(0);
         _PerformSalineBolus = false;
      }
   }

   //
   //
   //  Act immediately . . .
   //
   alarmOverrides();

   //
   // Check door commands ...
   //
   doDoorCommands();

   SendOrdersMsg alarmaction(MessageBase::SEND_LOCAL);
   alarmaction.send(1);
}

void StateControl::alarmOverrides ()
{
   static bool loggedOnce = false;

   ProcData    pd;

   //
   // Log it if the overrides change ...
   //
   if ( _AlarmOverrideMsg.changed(_PreviousAlarmOverrides) )
   {
      _overrideChanged = true;

      DataLog(log_level_proc_info) << "Current alarm overrides-> "
                                   << " Qac: " << _AlarmOverrideMsg.Qac()
                                   << " Qin: " << _AlarmOverrideMsg.Qin()
                                   << " Qrp: " << _AlarmOverrideMsg.Qrp()
                                   << " Qpls " << _AlarmOverrideMsg.Qplasma()
                                   << " Qplt: " << _AlarmOverrideMsg.Qcollect()
                                   << " cent: " << _AlarmOverrideMsg.centSpeed()
                                   << " centP: " << _AlarmOverrideMsg.centPausedSpeed()
                                   << " Vplt: " << _AlarmOverrideMsg.Vplt()
                                   << " Vpls: " << _AlarmOverrideMsg.Vpls()
                                   << " Vrbc: " << _AlarmOverrideMsg.Vrbc()
                                   << " lights: " << _AlarmOverrideMsg.alarmLight()
                                   << " active: " << _AlarmOverrideMsg.AlarmActive()
                                   << " rcvdAlr: " << _AlarmOverrideMsg.rcvdAlarmID()
                                   << " respAlr: " << _AlarmOverrideMsg.respAlarmID()
                                   << " dispAlr: " << _AlarmOverrideMsg.dispAlarmID()
                                   << endmsg;

   }
   else
   {

      _overrideChanged = false;

   }




   _PreviousAlarmOverrides = _AlarmOverrideMsg.getOverrides();


   // IT10175 allow metering to go ahead if A2 Donor air alarm in rinseback.
   if (_AlarmOverrideMsg.dispAlarmID()  == DONOR_AIR_ALARM)
   {
      if ( (pd.SystemState() == BLOOD_RINSEBACK) && !loggedOnce)
      {
         pd.MakeRunWritable();
         DataLog(log_level_proc_pfr_info) << "set flag to continue with metering after this A2 alarm. (IT10175)" << endmsg;
         pd.Run().AllowMSSAfterA2SpecialCase.Set(true);
         pd.MakeRunReadable();

         loggedOnce = true;
      }
   }

   //
   // If we are considered in a paused state ...
   //
   if ( _AlarmOverrideMsg.AlarmActive() )
   {
      //
      // Stop the cassette ...
      //
      pd.Orders().CassetteCmd(HAL_NOCHANGE);
   }

   float chamberFlow;

   if ( pd.Status().PlasmaValve.IsReturning() )
      chamberFlow = pd.Orders().Qcollect() - pd.Orders().Qplasma();
   else
      chamberFlow = pd.Orders().Qcollect();

   //
   // Move the valves if specific positions are requested ...
   //
   if ( _AlarmOverrideMsg.Vrbc() != HAL_VALVE_UNKNOWN )
   {
      pd.Orders().ValveCmd(HAL_VRBC, _AlarmOverrideMsg.Vrbc() );
   }
   if ( _AlarmOverrideMsg.Vpls() != HAL_VALVE_UNKNOWN )
   {
      pd.Orders().ValveCmd(HAL_VPLASMA, _AlarmOverrideMsg.Vpls() );
   }
   if ( _AlarmOverrideMsg.Vplt() != HAL_VALVE_UNKNOWN )
   {
      pd.Orders().ValveCmd(HAL_VCOLLECT, _AlarmOverrideMsg.Vplt() );
   }


   /// centrifuge alarm overrides //////
   if ( !_AlarmOverrideMsg.centSpeed())
   {
      pd.Orders().UnlockDoor();
      pd.Orders().RPM(0.0f);
   }
   else if (    _AlarmOverrideMsg.centPausedSpeed()
                && (    pd.Status().CentrifugeRPM() > CobeConfig::data().CentrifugePumpsPausedSpeed
                        && pd.Orders().RPM() > CobeConfig::data().CentrifugePumpsPausedSpeed )
                )
   {
      //
      // Slow the centrifuge to the pause speed ...
      //
      pd.Orders().RPM(CobeConfig::data().CentrifugePumpsPausedSpeed);
   }

   //
   // Turn on the lights ...
   //
   pd.Orders().AlarmLight(_AlarmOverrideMsg.alarmLight() );

   //
   // Save the currently displayed alarm for logging purposes
   //
   pd.DisplayedAlarm(_AlarmOverrideMsg.dispAlarmID() );




   ///////////  PPR  overrides 1/2 speed alarm /////////////////////////
   if (   (_AlarmOverrideMsg.Qin()  < 0.9f)
          && ( _AlarmOverrideMsg.Qin() > 0.1f)
          )
   {
      if (pd.Run().inPPR.Get())
      {
         _PPRcheck = true;
         DataLog (log_level_proc_info) << "Auto APS Slowing alarm is Wait for PPR to finish..." << endmsg;
         return;
      }
   }
   /////////////////////////////////////////////////////////////////////


   //
   // Save the return and collect pump flows in case we
   //  are trying to perform a saline bolus during an alarm condition
   //
   float returnFlow  = pd.Orders().Qrp();
   float collectFlow = pd.Orders().Qcollect();

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   //  alarm Pump MASK:
   //       AC
   //      INLET
   //////////////////////////////////////////////////////////////////////////////
   // Qin special overide processing
   //   we must maintain the 20mL/min (QinLimitMin) minimum inlet speed, so....
   //
   //

   ///////////////////////////////////////////////////////////////////////////////
   if (((   _AlarmOverrideMsg.Qin() < 0.9f) && ( _AlarmOverrideMsg.Qin() > 0.1f)))
      _inHalfSpeedAPS = true;
   else
      _inHalfSpeedAPS = false;
   ///////////////////////////////////////////////////////////////////////////////

   if (_inHalfSpeedAPS)
   {
      const float  customRatio = pd.Config().Procedure.Get().key_custom_ratio;
      const float  MIN_Q_IN    = CobeConfig::data().QinLimitMin;
      static float Qin, Qac = 0.0f;

      if (_overrideChanged || _PPRcheck)  // on entry to the half speed store the original speeds.
      {
         Qin       =  _lastQin ;
         Qac       =  _lastQac ;
         _PPRcheck = false;
         DataLog (log_level_proc_info) << " Slowing based on last speed of Qin/Qac =" << Qin << "/" << Qac << endmsg;
      }

      float reducedQin = _AlarmOverrideMsg.Qin() * Qin;

      // if this would drive the Qin below 20 then....
      if (reducedQin < MIN_Q_IN)
      {
         float Qac_adj = MIN_Q_IN / customRatio;

         pd.Orders().Qin(MIN_Q_IN);
         if (pd.Run().DrawCycle.Get())
            pd.Orders().Qac(Qac_adj);
         else
            pd.Orders().Qac(0.0f);


         // otherwise, we havent driven the Qin below the limit so just adjust normal
      }
      else
      {

         pd.Orders().Qin(_AlarmOverrideMsg.Qin() * Qin);
         if (pd.Run().DrawCycle.Get())
            pd.Orders().Qac(_AlarmOverrideMsg.Qac() * Qac);
         else
            pd.Orders().Qac(0.0f);
      }

   }
   else       ////// not half speed....  //////////////////////////////////////////

   {
      if (_overrideChanged)
      {

         if ((_AlarmOverrideMsg.Qac() == 0.0f) && (_AlarmOverrideMsg.Qin() == 0.0f))
         {
            //  if (!pd.Run().inPPR.Get())
            {
               _lastQin =  pd.Orders().Qin();
               _lastQac =  pd.Orders().Qac();
               DataLog (log_level_proc_info) << "Snapshot of Qin/Qac =" << _lastQin << "/" << _lastQac << endmsg;
            }
         }


      }
      pd.Orders().Qin(_AlarmOverrideMsg.Qin() * pd.Orders().Qin()  );
      pd.Orders().Qac(_AlarmOverrideMsg.Qac() * pd.Orders().Qac()  );

   }

   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   //  alarm pump MASK:
   //     RETURN
   //     PLASMA
   //     COLLECT
   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////
   pd.Orders().Qrp(_AlarmOverrideMsg.Qrp()      * pd.Orders().Qrp()      );
   pd.Orders().Qplasma(_AlarmOverrideMsg.Qplasma()  * pd.Orders().Qplasma()  );
   pd.Orders().Qcollect(_AlarmOverrideMsg.Qcollect() * pd.Orders().Qcollect() );


   //////////////////////////////////////////////////////////////////////////////
   //
   //
   //
   // Check to see if this is a possible chamber flow change ...
   //
   if ( _AlarmOverrideMsg.Qcollect() > 0.0f &&
        _AlarmOverrideMsg.Qplasma() == 0.0f )
   {
      //
      // ... if so, try to maintain the chamber flow ...
      //
      if ( pd.Status().CassIsPltPlsRBC() )
      {
         if ( chamberFlow < 0.0f )
            chamberFlow = 0.0f;

         pd.Orders().Qcollect(chamberFlow);
      }
      else
      {
         pd.Orders().Qcollect(0.0f);
      }
   }

   //
   // Recommand the return and collect pumps for saline bolus related
   //  features during the stop and pause alarms ...
   //
   if (    _PerformSalineBolus
           && (    RecoveryMan::current() == RecoveryTypes::SalinePrime
                   || RecoveryMan::current() == RecoveryTypes::SalineBolus )
           && (    _AlarmOverrideMsg.dispAlarmID() == PAUSE_BUTTON_ALARM
                   || _AlarmOverrideMsg.dispAlarmID() == STOP_BUTTON_ALARM )
           )
   {
      pd.Orders().Qrp(returnFlow);
      pd.Orders().Qcollect(collectFlow);
   }



}

void StateControl::salineBolusRequest ()
{
   if ( _SalineBolusRequestMsg.getData() )
   {
      if (    _AlarmOverrideMsg.dispAlarmID() == PAUSE_BUTTON_ALARM
              || _AlarmOverrideMsg.dispAlarmID() == STOP_BUTTON_ALARM )
      {
         _PerformSalineBolus = true;
      }
   }
   else
   {
      _PerformSalineBolus = false;
   }
}

void StateControl::doDoorCommands ()
{
   ProcData pd;
   //
   // Save the fact that we wanted the door unlocked,
   //  but don't do it until the centrifuge stops ...
   //
   if ( !pd.Orders().DoorCommandedToLock() )
   {
      _UnlockDoor = true;
      if ( pd.Status().DoorIsLocked() && !pd.Status().DoorIsOpen() )
      {
         pd.Orders().LockDoor();
      }
   }

   //
   // When the centrifuge is at zero RPMs, let the door unlock ...
   //
   if ( _UnlockDoor )
   {
      if ( pd.Status().CentrifugeRPM() < CobeConfig::data().CentrifugeOpenSpeed )
      {
         if ( pd.Status().PS64Vsw.InRange() )
         {
            RequestSafetyPowerMsg request(MessageBase::SEND_GLOBAL);
            request.send(DISABLE_64V_POWER);
         }
         else if ( pd.Orders().RPM() < 1.0f &&
                   pd.Status().PS64Vsw.TooLow()
                   )
         {
            pd.Orders().UnlockDoor();
            _UnlockDoor = false;
         }
      }

      if ( pd.Orders().RPM() > 0.0f )
      {
         _UnlockDoor = false;
      }
   }

   //
   // Keep the door locked if someone is trying to order the
   //  centrifuge to run ...
   //
   if (   ( pd.Orders().RPM() >= 1.0f || pd.Status().PS64Vsw.InRange() )
          && !pd.Status().DoorIsOpen()
          && !pd.AlarmActive()
          )
   {
      pd.Orders().LockDoor();
   }

}

void StateControl::powerRequestCb ()
{
   int request = _PowerRequestMsg.getData();

   if (    request == TEST_BOTH_POWER
           || request == ENABLE_64V_POWER )
   {
      _UnlockDoor = false;
   }
}

void StateControl::doAKO ()
{
   ProcData pd;

   //
   // If we are not paused any more, then the AKO is
   //  no longer valid ...
   //
   if ( !pd.AlarmActive() )
   {
      //
      // AKO is complete ...
      _AKOCompleted = true;
      _AKORequested = false;
      return;
   }

   //
   // Find out if we can do the AKO ...
   //
   if ( _AKORequested )
   {
      //
      // Accumulate volume...
      //
      _AKOVolume += pd.Status().ReturnPump.dV();
      DataLog (log_level_proc_info) << "_AKOVolume = " << _AKOVolume << endmsg;

      if ( _AKOVolume > CobeConfig::data().AKOVolume &&
           pd.Volumes().VnonACBloodAtNeedle.Get() <= 0.0f )
      {
         //
         // AKO is complete ...
         DataLog (log_level_proc_info) << "_AKOVolume reached, setting AKO to complete." << endmsg;
         _AKOCompleted = true;
         _AKORequested = false;
      }
      else if ( pd.Status().ReservoirIsEmpty() )
      {
         //
         // AKO is complete if the reservoir is empty ...
         DataLog (log_level_proc_info) << "Reservoir empty, setting AKO to complete." << endmsg;
         _AKOCompleted = true;
         _AKORequested = false;
      }
      else
      {
         //
         // Perform the Access Keep Open ...
         //
         pd.Orders().Qrp(CobeConfig::data().QrpAKO);
      }
   }
}

void StateControl::doCentrifugeCommands ()
{
   ProcData pd;

   //
   // If the inlet pump is paused ...
   //
   if (    (    pd.TrimaSet().CassetteState.Get() > ProcTrimaSet_CDS::BLOOD
                && pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::RINSEBACK )
           && (    pd.Orders().Qin() == 0.0
                   || pd.Status().InletPump.ActRPM() < CobeConfig::data().MinimumPumpRPM )
           && pd.Orders().RPM() > CobeConfig::data().CentrifugeMinCommandedRunSpeed )
   {
      if ( _LastSeenCentSpeed < pd.Orders().RPM() )
      {
         //
         // Hold the centrifuge at the current speed, or non-resonant speed ...
         //
         if ( pd.Status().CentrifugeRPM() <= CobeConfig::data().CentrifugeMinCommandedRunSpeed )
         {
            // Enable the door latch alarms while stopping the centrifuge ...
            pd.Orders().RPM(1.0f);
         }
         else if ( pd.Status().CentrifugeRPM() < CobeConfig::data().CentResonantRPM )
         {
            pd.Orders().RPM(CobeConfig::data().AirBlockCentrifugeRPM);
         }
         else
         {
            pd.Orders().RPM(_LastSeenCentSpeed);
         }
      }
      //
      // If the centrifuge is commanded to a speed that is less
      // than the last recorded centrifuge speed, then let the
      // centrifuge ramp down to that speed, even if the pumps
      // are paused.  Store this speed as the last recorded
      // centrifuge speed.
      //
      else
      {
         _LastSeenCentSpeed = pd.Orders().RPM();
      }
   }
   else if (pd.Status().CentrifugeRPM() > pd.Orders().RPM())
   {
      // This is for fixing small deviations in commanded centrifuge speed
      // during recoveries.
      _LastSeenCentSpeed = pd.Orders().RPM();
   }
   else
   {
      _LastSeenCentSpeed = pd.Status().CentrifugeRPM();
   }

}

void StateControl::calibrateRBCDetector ()
{
   ProcData pd;

   //
   // If the detector needs calibration ...
   //
   if ( !_RBCCalibrator.calibrated() )
   {
      pd.MakeTrimaSetWritable();
      pd.TrimaSet().RBCDetectorCalibrated.Set(false);
      pd.MakeTrimaSetReadable();
      _RBCCalibrator.calibrate();
      if ( _RBCCalibrator.calibrated() )
      {
         pd.MakeTrimaSetWritable();
         pd.TrimaSet().RBCDetectorCalibrated.Set(true);
         pd.MakeTrimaSetReadable();
      }
   }
   //
   // ... Check the calibration ...
   //
   else
   {
      //
      // Make sure the data is synchronized
      pd.MakeTrimaSetWritable();
      pd.TrimaSet().RBCDetectorCalibrated.Set(true);
      pd.MakeTrimaSetReadable();

      RBCDetectorInit::CalibrateState calCheckDisabled = RBCDetectorInit::CheckCalibration;

      if ( pd.TrimaSet().CassetteState.Get() > ProcTrimaSet_CDS::BLOODPRIME )
      {
         calCheckDisabled = RBCDetectorInit::DisableGreenCheck;
      }

      if (    // ... In spillover ...
         ( RecoveryMan::inRecovery() != RecoveryTypes::NoRecovery &&
           ( RecoveryTypes::RBCSpillover    == RecoveryTypes::RecoveryId(pd.RecoveryName().c_str() ) ||
             RecoveryTypes::WBCChamberPurge == RecoveryTypes::RecoveryId(pd.RecoveryName().c_str() ) )
         )
         // ... RBC valve is collecting ...
         || ( pd.Status().RBCValve.IsCollecting()
              )
         )
      {
         calCheckDisabled = RBCDetectorInit::DisableCalCheck;
      }
      _RBCCalibrator.checkCalibration(calCheckDisabled);
   }
}

void StateControl::akoRequested ()
{
   ProcData pd;
   if ( _AKORequestMsg.getData() == 1 )
   {
      DataLog (log_level_proc_info) << "akoRequested" << endmsg;
      _AKORequested = true;
      _AKOCompleted = false;

      if (pd.NewAKO())
      {
         DataLog (log_level_proc_info) << "akoRequested:  NewAKO, resetting _AKOVolume." << endmsg;
         _AKOVolume = 0.0f;
      }
   }
   //
   // Stop the current AKO if the AKO is in progress ...
   //
   else if ( _AKORequested )
   {
      if ( _AKOCompleted == false )
      {
         //
         // Reassert the alarm ...
         //
         DataLog (log_level_proc_info) << "AKO reasserting myself." << endmsg;
         pd.NewAKO(false);
         //
         anAlarmMsg threeMinuteAlert(MAINTAIN_DONOR_ACCESS_ALERT);
         threeMinuteAlert.setAlarm();
      }
      _AKORequested = false;
   }
}

void StateControl::rbcDetectorUncalibrated ()
{
   _RBCCalibrator.calibrated(0);
}

void StateControl::checkCommands ()
{
   ProcData pd;
   if ( ( ( pd.Orders().RPM() == 0 &&
            pd.Status().CentrifugeRPM() > 0 )
          )
        )
   {
      if ( !_CentStopTimerStarted )
      {
         //
         // Start a timer and ensure it stops within the
         //  specified time limit ...
         //
         _CentStopTimer.interval(40000 /*milliseconds*/);   // 40 seconds
         _CentStopTimerStarted = true;
      }
   }
   else
   {
      //
      // Stop the timer ...
      _CentStopTimer.interval(0);
      _CentStopTimerStarted = false;
   }

   //
   // The 0.6 mls/min is the minimum speed we can be running at
   //  to pass the power on T1 test (The test which checks the
   //  ability of proc to control the pumps with the power on)
   //
   if ( ( ( pd.Orders().Qac() == 0 &&
            pd.Status().ACPump.ActFlow() > 0.6f /*mls/min*/ ) ||
          ( pd.Orders().Qcollect() == 0 &&
            pd.Status().CollectPump.ActFlow() > 0.6f /*mls/min*/ ) ||
          ( pd.Orders().Qin() == 0 &&
            pd.Status().InletPump.ActFlow() > 0.6f /*mls/min*/ ) ||
          ( pd.Orders().Qplasma() == 0 &&
            pd.Status().PlasmaPump.ActFlow() > 0.6f /*mls/min*/ ) ||
          ( pd.Orders().Qrp() == 0 &&
            pd.Status().ReturnPump.ActFlow() > 0.6f /*mls/min*/ )
          )

        )
   {
      if ( !_PumpsStopTimerStarted )
      {
         //
         // Start a timer and ensure it stops within the
         //  specified time limit ...
         //
         _PumpsStopTimer.interval(5000 /*milliseconds*/);   // 5 seconds
         _PumpsStopTimerStarted = true;
      }
   }
   else
   {
      //
      // Stop the timer ...
      _PumpsStopTimer.interval(0);
      _PumpsStopTimerStarted = false;
   }
}

void StateControl::systemOutOfControl ()
{
   DataLog(log_level_control_alarm_detail) << "State machine cannot control hardware" << endmsg;

   anAlarmMsg outOfControlAlarm(HARDWARE_FAULT);
   outOfControlAlarm.setAlarm();
   _CentStopTimer.interval(0);
   _CentStopTimerStarted  = false;
   _PumpsStopTimer.interval(0);
   _PumpsStopTimerStarted = false;
   _PredictTimer.interval(0);
}

void StateControl::predictCommunication ()
{
   ProcData   pd;
   int        request     = pd.Run().PredictRequest.Get();

   static int lastRequest = -1;

   if (request != REPREDICT_ONLY)
   {
      switch (request)
      {

         case (CLEAR_ALL) :
         {
            if (lastRequest != request)
               DataLog(log_level_proc_info) << "recieving predict request CLEAR_ALL" << endmsg;
         }
            // DataLog( log_level_proc_info ) << "recieving predict request CLEAR_ALL" << endmsg;
         break;
         case (DONOR_INFO_CHANGED) :
            DataLog(log_level_proc_info) << "recieving predict request DONOR_INFO_CHANGED" << endmsg;
            break;
         case (DONOR_INFO_UNCHANGED) :
            DataLog(log_level_proc_info) << "recieving predict request DONOR_INFO_UNCHANGED" << endmsg;
            break;
         case (DONOR_INFO_PRECONNECT) :
            DataLog(log_level_proc_info) << "recieving predict request DONOR_INFO_PRECONNECT" << endmsg;
            break;
         case (ADJUSTMENT) :
            DataLog(log_level_proc_info) << "recieving predict request ADJUSTMENT" << endmsg;
            break;
         case (PTF_FILTER_RECALC) :
            DataLog(log_level_proc_info) << "recieving predict request PTF_FILTER_RECALC" << endmsg;
            break;
         default :
            DataLog(log_level_proc_info) << "recieving predict request ????? Oh, oh now what?" << endmsg;
            break;
      }


   }
   else
   {

      if (lastRequest != request)
      {
         DataLog(log_level_proc_info) << "recieving predict request REPREDICT_ONLY" << endmsg;
      }

   }
   lastRequest = request;


   //
   //
   //   For each request ensure predict responds within desired time
   //
   if (request)
   {
      if ( _PredictTimer.timerArmed() == TimerMessage::DISARMED )
      {
         //
         //
         //   use cobe_config timeout, modified by the number of procedures that we
         //   expect to optimize.
         //
         int ms = (int)(CobeConfig::data().PredictTimeout * 1000.0f);
         DataLog(log_level_proc_info) << "predict timeout set to " << ms << endmsg;
         if (ms > 0)
         {                                 // not disabled
            if (request != REPREDICT_ONLY) // some other elements are set
            {
               ms *= pd.Config().ProcedureList.numberOfProcedures;
               ms /= 2; // since we don't expect worse case for every procedure
            }

            _PredictTimer.interval(ms);

            _predictTime.init();
            _predictTime.activate();

            DataLog(log_level_proc_info) << " (again) predict timeout set to " << ms << endmsg;
         }
      }
   }

   //
   //
   //   Send out latest request if predict is ready for it
   //
   if ( !pd.PredictBlocked() && request )
   {
      DataLog(log_level_proc_info) << "sending predict request " << request << endmsg;

      pd.PredictBlocked(true);
      DoPredictionMsg req(MessageBase::SEND_LOCAL);
      req.send(request);
      pd.MakeRunWritable();
      pd.Run().PredictRequest.Set(0);
      pd.MakeRunReadable();

      lastRequest = -1;
   }

   //
   //
   //   At this point if predict isn't blocked then a response has been received
   //
   if (!pd.PredictBlocked() && _PredictTimer.timerArmed() == TimerMessage::ARMED )
   {
      DataLog(log_level_proc_info) << "Predict responded, removing timeout.  Predict took " <<  _predictTime.getSecs() << " seconds." << endmsg;
      _PredictTimer.interval(0);
      _predictTime.inactivate();
   }

}

void StateControl::predictTimeout ()
{
   DataLog(log_level_proc_error) << "Predict timeout exceeded" << endmsg;
   anAlarmMsg predicterr(PREDICT_NOT_RESPONDING);
   _predictTime.inactivate();
   predicterr.setAlarm();
}

/* FORMAT HASH a4480e0454c9bcea9d1e5cc0481ce088 */
