/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      donorconnected.cpp
 *
 */

#include "donorconnected.h"
#include "recoveryman.h"
#include "states.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "procdata.h"
#include "cassette.h"

#include "caldat.h"
#include "MssCommon.h"   // common MSS functions

DEFINE_STATE(DonorConnected);

DonorConnected::DonorConnected()
   : StateAbs(),
     _RequestPowerMsg(),
     _RunInfo(),
     _AccessPressureLimiter(),
     _safetyInAirStopMsg(Callback<DonorConnected>(this, &DonorConnected::SafetyInAirStopHandler)),
     _safetyClearsAirStopMsg(Callback<DonorConnected>(this, &DonorConnected::SafetyClearsAirStopHandler)),
     _silentWaitAlarm(SILENT_PAUSE_FOR_MICROAIR_CHECK)
{
   _microAirWaitTimer.init();
}

void DonorConnected::reset ()
{
   _quietPlease_airCheck = false;
}

DonorConnected::~DonorConnected()
{}

int DonorConnected::transitionStatus ()
{
   return NO_TRANSITION;
}

int DonorConnected::preProcess ()
{
   if (_quietPlease_airCheck)
   {  
      if (!_microAirWaitTimer.activated())
      {
         DataLog(log_level_proc_info) << "Control in safety microAir wait started"  << endmsg;
         _microAirWaitTimer.activate();
      } else {
         if (_microAirWaitTimer.getSecs() >= 20.0f)
         {
            DataLog(log_level_proc_info) << "Control in safety microAir timer expired with no call from safety!" << endmsg;
         } else {
            DataLog(log_level_proc_info) << "Control in safety microAir wait at t= " << _microAirWaitTimer.getSecs() << " seconds" << endmsg;
         }
      }
   }
   return NORMAL;
}

int DonorConnected::postProcess ()
{
   ProcData pd;
   //
   // Set the pressure limits ...
   //
   _AccessPressureLimiter.setLimits(currentState());

   //
   // Make sure we request power if we are sitting idle without
   //  when we are trying to do something without any alarm ...
   //
   if ( !pd.AlarmActive() )
   {
      if ( !pd.Status().PS24Vsw.Acceptable() &&
           //
           // Pumps are commanded to move ...
           ( pd.Status().ACPump.CmdFlow() > 0.0f ||
             pd.Status().InletPump.CmdFlow() > 0.0f ||
             pd.Status().PlasmaPump.CmdFlow() > 0.0f ||
             pd.Status().CollectPump.CmdFlow() > 0.0f ||
             fabs(pd.Status().ReturnPump.CmdFlow() ) > 0.0f ||
             //
             // or the valves are commanded to move ...
             pd.Status().RBCValve.Command() != pd.Status().RBCValve.State() ||
             pd.Status().PlasmaValve.Command() != pd.Status().PlasmaValve.State() ||
             pd.Status().CollectValve.Command() != pd.Status().CollectValve.State() )
           )
      {
         if (!_quietPlease_airCheck)
         {
            _RequestPowerMsg.send(ENABLE_24V_POWER);
            DataLog(log_level_proc_info) << "Request 24V Power On " << endmsg;
         }
      }

      // IT 10057.
      // I think the only purpose for this command is to turn on power after the door lock
      // test turned it off.  I added logic to the door lock test to turn the power back on
      // when it was the one that turned it off, making this unnecessary (I think).
      if ( !pd.Status().PS64Vsw.Acceptable() &&
           !pd.DoorLockTestInProgress() &&
           pd.Status().CmdCentrifugeRPM() >= 1.0f )
      {
         _RequestPowerMsg.send(ENABLE_64V_POWER);
         DataLog(log_level_proc_info) << "Request 64V Power On " << endmsg;
      }

      //
      // Always try to keep the door locked when we are in a non-alarm state...
      //
      //
      if (!pd.Status().DoorIsOpen()) // dont block open by locking please...
      {
         if (!pd.Status().DoorIsLocked())
         {
            DataLog(log_level_proc_info) << "door unlocked; commanding to lock " << endmsg;
         }
         pd.Orders().LockDoor();
      }
   }

   //
   // Set the centrifuge ramp rates ...
   //
   if (pd.Status().CentrifugeRPM() > CobeConfig::data().StandardCentrifugeRampTransition)
   {
      pd.Orders().RampUp(CobeConfig::data().StandardHighCentrifugeRamp);
   }
   else
   {
      pd.Orders().RampUp(CobeConfig::data().StandardLowCentrifugeRamp);
   }
   pd.Orders().RampDn(CobeConfig::data().CentrifugeRampDownRate);

   //
   // Set information in the run info message ...
   //
   //  The pressure meter should use the "first cycle" meter until
   //  the start of the first return cycle.
   //
   if (pd.Run().FirstCycleComplete.Get() || (!pd.Run().DrawCycle.Get()))
   {
      _RunInfo.Pressure(pd.Orders().APSLow(), pd.Orders().APSHigh(), pd.Status().APS(), 0);
   }
   else
   {
      _RunInfo.Pressure(CobeConfig::data().PressureMeterMinLimit, CobeConfig::data().PressureMeterMaxLimit, pd.Status().APS(), 1);
   }

   //
   // Set the Plasma Target to the correct value if using plasma rinseback
   //
   const float plasmaTarget = pd.PlasmaRinseback().PlasmaRinsebackTarget
                                 (pd.PlasmaRinseback().VPlasmaTarget.Get(),
                                 pd.RunTargets().PlasmaTarget.Get(),
                                 pd.Volumes().PlasmaVolume.Get(),
                                 pd.TrimaSet().CassetteState.Get());


   _RunInfo.SetTargets(pd.RunTargets().PlateletYieldTarget.Get(),
                       plasmaTarget,
                       pd.RunTargets().RbcVolumeTarget.Get() );

   _RunInfo.AlarmStatus(pd.AlarmActive() );
   _RunInfo.Update();

   DataLog(log_level_proc_debug) << "Run Info: Pressure Min->" << _RunInfo.Data().current_inlet_pressure_min
                                 << " Pressure->" << _RunInfo.Data().current_inlet_pressure
                                 << " Pressure Max->" << _RunInfo.Data().current_inlet_pressure_max << endmsg;
   DataLog(log_level_proc_debug) << "Run Info: Targets: Time->" <<  pd.RunTargets().ProcTimeTarget.Get() - pd.ProcRunTime().getMins()
                                 << " Platelet->" << _RunInfo.Data().estimated_platelets
                                 << " Plasma->" << _RunInfo.Data().estimated_plasma
                                 << " RBC->" << _RunInfo.Data().estimated_rbcs << endmsg;
   DataLog(log_level_proc_debug) << "Run Info: Status: DrawCycle-> " << (int)_RunInfo.Data().return_pump_cycle
                                 << " Time-> " << pd.ProcRunTime().getMins()
                                 << " Platelet-> " << _RunInfo.Data().current_platelets_collected
                                 << " Plasma-> " << _RunInfo.Data().current_plasma_collected
                                 << " RBC-> " << _RunInfo.Data().current_rbc_collected << endmsg;

   _RunInfo.SendMsg();

   //
   //
   //   Set new current adjustment values for meters
   //
   setButtonStatus();
   pd.AdjustControl().Update();

   return NORMAL;
}

int DonorConnected::preEnter ()
{
   ProcData pd;

   // from here on, safety and control must both see fluid to run the return pump... in the positive direction; ccw
   pd.Orders().safeReturnCommands(true);

   //
   // Set the cassette state ...
   //
   if ( pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::BLOODPRIME )
   {
      pd.MakeTrimaSetWritable();
      pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::BLOODPRIME);
      pd.MakeTrimaSetReadable();
   }

   //
   // Request Power ...
   //
   _RequestPowerMsg.init(MessageBase::SEND_GLOBAL);
   DataLog(log_level_proc_debug) << "Create Request Power Message " << endmsg;

   _RequestPowerMsg.send(ENABLE_BOTH_POWER);
   DataLog(log_level_proc_info) << "Request 24V and 64V Power On" << endmsg;

   //
   // Initialize the Run Info object and set DonorConnected latch ...
   //
   pd.MakeRunWritable();
   _RunInfo.Initialize();
   pd.MakeRunReadable();

   //
   // Initialize with APS limits ...
   //
   _AccessPressureLimiter.init(pd.Config().Procedure.Get().key_draw_press,
                               pd.Config().Procedure.Get().key_return_press);

   // black stamp uses the cal.dat values for red/green drive after cassette ID per IT 8389
   if (pd.TrimaSet().Cassette != cassette::PLTPLSRBC)
   {
      DataLog(log_level_proc_load_cassette_info) << "Blackstamp set using CAL.dat RBC data" << endmsg;
      pd.MakeTrimaSetWritable();
      pd.TrimaSet().RBCRedDriveValue.Set(CalDat::data().RBCInitRedDrive);
      pd.TrimaSet().RBCGreenDriveValue.Set(CalDat::data().RBCInitGreenDrive);
      pd.MakeTrimaSetReadable();
   }

   return NORMAL;
}

int DonorConnected::postExit ()
{
   ProcData pd;

   //
   // Request power off ...
   //
   _RequestPowerMsg.send(DISABLE_24V_POWER);
   DataLog(log_level_proc_info) << "Request 24V Power Off" << endmsg;

   //
   // The cassette can now been used and should be considered
   //  in a used state ...
   //
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::USED);
   pd.MakeTrimaSetReadable();

   if ( readyForPLTMSS() || readyForRBCMSS()  )
   {
      // IT 7892 .. leave door locked if going into MSS
      pd.Orders().LockDoor();
   }
   else
   {
      // Unlock the door, the procedure is finished ...
      // pd.Orders().UnlockDoor();   // removed via IT 11863
   }

   //
   // Stop everything ...
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().RPM(0.0f);

   //
   // Deregister the safety message ...
   _RequestPowerMsg.deregisterMsg();

   //
   // Save the end run time here ( if not already saved ) ...
   //
   if ( pd.Run().ProcEndRunTime.Get() == 0 )
   {
      time_t t;
      time(&t);
      pd.MakeRunWritable();
      pd.Run().ProcEndRunTime.Set(t);
      pd.MakeRunReadable();
      DataLog(log_level_proc_donor_disconnect_info) << "End of run time: " << t << endmsg;
   }

   return NORMAL;
}

void DonorConnected::setButtonStatus ()
{
   ProcData pd;
   ////////////////////////////////////////////////////////////////////
   // SPILLOVER
   //
   // If the transition to 'spillover' cannot happen ...
   if (    !RecoveryMan::recoveryAvailable("RBCSpillover")
           || pd.AlarmActive() )
   {
      //
      // The cassette type is right for 'spillovers'...
      //
      if ( !pd.Status().CassIsPltPlsRBC() )
         //
         // The button should not be shown ...
         pd.AdjustControl().BtnSpillover(BUTTON_INVISIBLE);
      else
         //
         // The button should only be greyed out ...
         pd.AdjustControl().BtnSpillover(BUTTON_GREY);

   }
   //
   // The transition to spillover is available ...
   //
   else
   {
      pd.AdjustControl().BtnSpillover(BUTTON_VISIBLE);
   }

   ////////////////////////////////////////////////////////////////////
   // AIR_BLOCK
   //
   // If the transition to 'air block' cannot happen ...
   if ( !RecoveryMan::recoveryAvailable("AirBlock") || pd.AlarmActive() )
   {
      //
      // The button should be greyed out ...
      pd.AdjustControl().BtnAirblock(BUTTON_GREY);

   }
   //
   // The transition to air block is available ...
   //
   else
   {
      pd.AdjustControl().BtnAirblock(BUTTON_VISIBLE);
   }

   ////////////////////////////////////////////////////////////////////
   // SALINE_BOLUS
   //
   // If the transition to 'saline bolus' cannot happen ...
   if (CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::NO_SALINE_BOLUS))
   {
      pd.AdjustControl().BtnSalinebolus(BUTTON_INVISIBLE); // IT11991
   }
   else if ( !RecoveryMan::recoveryAvailable("SalineBolus") ||
             ( pd.AlarmActive() &&
               (pd.DisplayedAlarm() != PAUSE_BUTTON_ALARM &&
                pd.DisplayedAlarm() != STOP_BUTTON_ALARM)
             )
             )
   {
      //
      // The cassette type is right for 'saline bolus'...
      //
      if (!pd.Status().CassIsPltPlsRBC())
         //
         // The button should only be greyed out ...
         pd.AdjustControl().BtnSalinebolus(BUTTON_GREY);
      else
         //
         // The button should not be shown ...
         pd.AdjustControl().BtnSalinebolus(BUTTON_INVISIBLE);
   }
   //
   // The transition to saline bolus is available ...
   //
   else
   {
      pd.AdjustControl().BtnSalinebolus(BUTTON_VISIBLE);
   }
}

void DonorConnected::SafetyInAirStopHandler ()
{
    ProcData pd;
    // 24 volt pulled by safety
    DataLog(log_level_proc_donor_disconnect_info) << "Control got stop event from safety:  Setting silent pause alarm "  << endmsg;
    _quietPlease_airCheck = true;
    //switch to draw if not in rinseback
    if ( BLOOD_RUN == pd.SystemState() )
    {
       if (!pd.Run().DrawCycle.Get())
       {
           DataLog(log_level_proc_donor_disconnect_info) << "Control switching to Draw cycle during microAir wait "  << endmsg;

           MidCycleSwitchMsg msg(MessageBase::SEND_GLOBAL);
           msg.send(SWITCH_TO_DRAW);

           pd.SwitchtoDraw();
       }
    }
   _silentWaitAlarm.setAlarm();

}

void DonorConnected::SafetyClearsAirStopHandler ()
{

    DataLog(log_level_proc_donor_disconnect_info) << "Control got start event from safety: Clearing silent pause alarm "  << endmsg;
    DataLog(log_level_proc_donor_disconnect_info) << "It took  " << _microAirWaitTimer.getSecs() << " seconds to clear the microAir (whew!)"  << endmsg;
    _microAirWaitTimer.inactivate();
    _microAirWaitTimer.init();
    // 24 volt on by safety
    _quietPlease_airCheck = false;
    _silentWaitAlarm.clearAlarm();
}
/* FORMAT HASH 51f3817adad6a667318cc000e2c7fe4a */
