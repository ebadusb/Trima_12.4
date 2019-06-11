/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      donordisconnect.cpp
 *
 */

#include "donordisconnect.h"

#include "safproc.h"
#include "states.h"
#include "cobeconfig.h"
#include "hypovolemia.h"
#include "procdata.h"
#include "volumeacc.h"
#include "guiproc.h"
#include "MssCommon.h"   // common MSS functions

// for developer testing
// #define MY_DEBUG_LOG_LEVEL log_level_proc_donor_disconnect_info
#define MY_DEBUG_LOG_LEVEL log_level_proc_debug

static const float MINIMUM_PRESSURE    = -350.0f;        // fail if below this limit
static const float INITIAL_PRESSURE    = -150.0f;        // 1st pass desired target
static const float REPEAT_DELTA        =   20.0f;        // mmHg
static const float MARGIN              =   17.0f;        // mmHg
static const int   STABILIZATION_TIME  =    3000;        // milliseconds ( 3 seconds )
static const int   HOLD_TIME           =    6000;        // milliseconds ( 6 seconds )
static const int   TEST_FAILED_COUNT   =       3;        // failure of at least 3 times to pass
static const int   TEST_FAILURE_TIME   =   30000;        // 30 seconds to fail the test
static const int   FAILURE_NOTIFY_TIME =   15000;        // 15 seconds to notify of failure

DEFINE_STATE(DonorDisconnect);

DonorDisconnect::DonorDisconnect()
   : StateAbs(),
     _PermissionGrantedMsg(),
     _PowerRequestResponseMsg(),
     _ReadyToTransitionMsg(Callback<DonorDisconnect>(this, &DonorDisconnect::readyToTransitionNotify)),
     _APSMsg(),
     _SetRunType(),
     _TestTimer(0, Callback<DonorDisconnect>(this, &DonorDisconnect::testTooSlow), TimerMessage::DISARMED),
     _NotificationTimer(0, Callback<DonorDisconnect>(this, &DonorDisconnect::testFailed), TimerMessage::DISARMED),
     _StabilizationTimer(0, Callback<DonorDisconnect>(this, &DonorDisconnect::pressureStabilized), TimerMessage::DISARMED),
     _PressureHoldTimer(0, Callback<DonorDisconnect>(this, &DonorDisconnect::pressureCheck), TimerMessage::DISARMED),
     _BadCassettePositionAlarm(UNLOADING_BAD_CASSETTE_POSITION),
     _DisconnectTestAlert(DISCONNECT_TEST_ALERT),
     _DisconnectTestAlarm(DISCONNECT_TEST_ALARM),
     _DisconnectTestMssAlert(DISCONNECT_TEST_MSS_ALERT),
     _DisconnectTestMssAlarm(DISCONNECT_TEST_MSS_ALARM),
     _PerformTest(0),
     _PowerOn(0),
     _TestPassed(0),
     _TestFailed(0),
     _FailPaused(0),
     _CassettePositionError(0),
     _AlarmDisplayCount(0),
     _TestStep(),
     _TargetPressure(0.0f),
     _MaximumPressure(0.0f),
     _CurrentAPS(0.0f),
     _AchievedPressureGoal(0),
     _HypovolemiaDistance(0.0f),
     _VinAtStart(0.0f),
     _PowerRespRegistered(false),
     _SafetyDiscoStatus(Callback<DonorDisconnect>(this, &DonorDisconnect::safetyStatus) ),
     _safetyConfirmed(false),
     _TestHasFailed(false),
     _ReadyForTransition(false)
{}

DonorDisconnect::~DonorDisconnect()
{}

int DonorDisconnect::transitionStatus ()
{
   ProcData pd;

   if (_safetyConfirmed && _TestPassed)
   {
      bool aasToFollow = readyForPLTMSS() || (readyForRBCMSS() && (!RASBlockedDueToCPSalarms()) );

      if (aasToFollow)
      {
         if (!_PowerOn)
         {
            DataLog(log_level_proc_donor_disconnect_info) << "Request power on transition" << endmsg;
            request24VPower();
         }
         else
         {
            DataLog(log_level_proc_donor_disconnect_info) << "Donor disconnect test passed, Metered Storage to follow" << endmsg;
            pd.Orders().LockDoor();
            return 3;
         }
      }
      else
      {
         static bool loggedThis = false;

         if (!loggedThis)
         {
            loggedThis = true;
            DataLog(log_level_proc_donor_disconnect_info) << "Donor disconnect test passed, no mss. ";
         }

         // IT8248
         // if (pd.Run().rinsebackVolumeSuccessMss.Get() == false)
         //    DataLog( log_level_proc_donor_disconnect_info ) << "RAS Metering Forbidden due to rinseback incomplete. ";

         if (RASBlockedDueToCPSalarms())
            DataLog(log_level_proc_donor_disconnect_info) << "CPS alarms on both products. ";

         DataLog(log_level_proc_donor_disconnect_info) << endmsg;

         // Tell GUI that we've confirmed disconnection.
         TellGuiThatDisconnectPassed tellGui(MessageBase::SEND_LOCAL);
         tellGui.send(aasToFollow);
      }
   }

   if (_ReadyForTransition)
   {
      if (!_PowerOn)
      {
         DataLog(log_level_proc_donor_disconnect_info) << "Request power on transition" << endmsg;
         request24VPower();
      }
      else
      {
         DataLog(log_level_proc_donor_disconnect_info) << "Ready for transition. " << endmsg;
         pd.Orders().UnlockDoor();
         return 2;
      }
   }

   //
   // The cassette is already up, the disconnect
   //  test need not be ran ...
   //
   if ( pd.Status().CassIsUp() )
   {
      DataLog(log_level_proc_donor_disconnect_info) << "Donor disconnect test passed, cassette is up" << endmsg;
      return 1;
   }

   return NO_TRANSITION;
}

int DonorDisconnect::preProcess ()
{
   ProcData pd;

   //
   // Update the volumes ...
   //
   pd.Update(ProcData::DISCONNECT);

   //
   // Signify the power was turned on or not ...
   //
   _PowerOn = ( pd.Status().PS24Vsw.Acceptable() ? 1 : 0 );

   //
   // No need to do the test if it has failed ...
   //
   if ( !_TestFailed )
   {
      if ( !pd.AlarmActive() && _PerformTest )
      {
         performTest();
      }
   }
   return NORMAL;
}

int DonorDisconnect::postProcess ()
{
   ProcData pd;

   //
   // Signify the power was turned on or not ...
   //
   _PowerOn = ( pd.Status().PS24Vsw.Acceptable() ? 1 : 0 );

   if ( _TestFailed && !_FailPaused && !pd.AlarmActive() )
   {
      //
      // Stop any timers
      //
      stopTimers();

      //
      // Stop the pumps
      //
      stopPumps();

      _TestHasFailed = true;

      if ( _CassettePositionError )
      {
         //
         // Set the donor disconnect alert for the 2nd and subsequent failures
         // (the 1st failure raises a BadCassette alarm)
         //
         if (_PerformTest == 0 && _TestPassed == 0) testFailed();

         //
         // The test is finished ...
         //
         _PerformTest = 0;
         _TestFailed  = 1; // override the reset done by testFailed()
      }
      else
      {
         //
         // Start the notification timer ...
         //
         _FailPaused = 1;
         _NotificationTimer.interval(FAILURE_NOTIFY_TIME);
         DataLog(log_level_proc_donor_disconnect_info) << "Started the notification timer " << FAILURE_NOTIFY_TIME << endmsg;
      }
   }

   if ( _PowerOn && ( _PerformTest || _TestHasFailed ) )
   {
      //
      // Command all valves to return ...
      //
      closeValves();
   }

   return NORMAL;
}


int DonorDisconnect::preEnter ()
{
   ProcData pd;

   pd.PFRHandler().EnablePFSave();

   if ( pd.Status().CassIsUp() )
   {
      //
      // We are done ...
      //
      return NORMAL;
   }

   // Set the system state
   pd.SystemState(DONOR_DISCONNECT);

   // stop the procedure clocks (if rinseback skipped)  IT7132
   pd.SysRunTime().inactivate();
   pd.ProcRunTime().inactivate();
   pd.ProcTime().inactivate();

   //
   // The cassette has now been used and should be considered
   //  in a used state ...
   //
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::USED);
   pd.MakeTrimaSetReadable();

   //
   // Create and register for the GUI message that will allow
   //  the test to be attempted ...
   //
   _PermissionGrantedMsg.init(Callback<DonorDisconnect>(this, &DonorDisconnect::shouldStartTest) );
   DataLog(MY_DEBUG_LOG_LEVEL) << "Create donor disconnected message " << endmsg;

   //
   // Create the control driver pressure message that arrives when
   //  the pressure has violated a limit ...
   //
   _APSMsg.init(Callback<DonorDisconnect>(this, &DonorDisconnect::reachedPressure) );
   DataLog(MY_DEBUG_LOG_LEVEL) << "Create APS fast filter message " << endmsg;

   // Initialize message from Proc to handle the run type
   _SetRunType.init(Callback<DonorDisconnect>(this, &DonorDisconnect::setDisconMssStatus) );

   //
   // Command all the pumps to stop ...
   //
   stopPumps();

   //
   // Disable Pressure monitoring ...
   //
   pd.Orders().APSHigh(0.0f);
   pd.Orders().APSLow(0.0f);

   //
   //
   //   Initialize the hypovolemia object.  Calculate the current distance from
   //   hypovolemia and give credit for any rinseback volume.
   //
   Hypovolemia hypo;
   hypo.initialize(pd.TrimaSet().Cassette.Get() );

   _HypovolemiaDistance = hypo.VolumeRemainingToHypovolemia(1, // always current procedure
                                                            pd.Cycle().VAccumOverdraw.Get(),
                                                            pd.Volumes().PlateletVolume.Get(),
                                                            pd.Volumes().VacPlateletBag.Get(),
                                                            pd.Volumes().PlasmaVolume.Get(),
                                                            pd.Volumes().VacPlasmaBag.Get(),
                                                            pd.Volumes().RBCVolume.Get(),
                                                            pd.Volumes().VacRBCBag.Get(),
                                                            pd.Volumes().VReplacement.Get() + pd.Volumes().VSalineBolus.Get(),
                                                            pd.Volumes().Vac.Get());

   if ( !pd.Run().RinsebackStarted.Get() )
   {
      pd.MakeVolumesWritable();
      pd.Volumes().Vreturn_for_residuals_calc.Set(pd.Volumes().Vreturn.Get() );  // need this for residual calc before we zero it below....
      pd.MakeVolumesReadable();

      VolumeAcc::ClearReturn();
   }
   _HypovolemiaDistance += pd.Volumes().Vreturn.Get();
   _VinAtStart           = pd.Volumes().VinTotal.Get();

   _PowerRespRegistered = false;

   return NORMAL;
}

int DonorDisconnect::postExit ()
{
   ProcData pd;

   //
   // Turn off my timer ...
   //
   stopTimers();

   //
   // Delete my messages ...
   //
   cleanup();

   //
   // Disable Pressure monitoring ...
   //
   pd.Orders().APSHigh(0.0f);
   pd.Orders().APSLow(0.0f);


   pd.MakeRunWritable();
   pd.Run().DonorDisconnectTime.Set(pd.GetAbsTimeNowinMinutes() );

   // END OF: safety and control must both see fluid to run the return pump...
   pd.Orders().safeReturnCommands(false);

   return NORMAL;
}

void DonorDisconnect::readyToTransitionNotify ()
{
   DataLog(log_level_proc_donor_disconnect_error) << "GUI says we're ready for transition." << endmsg;
   _ReadyForTransition =  true;
}


void DonorDisconnect::testFailed ()
{
   _AlarmDisplayCount++;

   DataLog(MY_DEBUG_LOG_LEVEL) << "testFailed(): AlarmDispCnt=" << _AlarmDisplayCount
                               << " SafetyConf=" << (int)_safetyConfirmed << " TestPassed=" << _TestPassed
                               << " TestHasFailed=" << (int)_TestHasFailed << " FailPaused=" << _FailPaused
                               << " PowerOn=" << _PowerOn << endmsg;
   //
   // Stop any timers ...
   //
   stopTimers();

   if ( _AlarmDisplayCount < TEST_FAILED_COUNT )
   {
      //
      // Reset the test failed flags ...
      //
      _TestFailed    = 0;
      _FailPaused    = 0;
      _TestHasFailed = true;

      //
      // The test failed. Set the *alert*
      //
      if ( readyForPLTMSS() || (readyForRBCMSS() && !RASBlockedDueToCPSalarms()) )
      {
         DataLog(log_level_proc_donor_disconnect_error) << "Setting disconnect test alert (mss version): " << DISCONNECT_TEST_MSS_ALERT << endmsg;
         _DisconnectTestMssAlert.setAlarm();
      }
      else
      {
         DataLog(log_level_proc_donor_disconnect_error) << "Setting disconnect test alert: " << DISCONNECT_TEST_ALERT << endmsg;
         _DisconnectTestAlert.setAlarm();
      }

      //
      // Let's prepare the test again ...
      //
      _TestStep = Callback<DonorDisconnect>(this, &DonorDisconnect::setUpTest);
   }
   else if ( _AlarmDisplayCount >= TEST_FAILED_COUNT &&
             _TestPassed == 0 )
   {
      //
      // The test passes because it failed too many times. Set the *alarm*
      //
      if ( readyForPLTMSS() || (readyForRBCMSS() && !RASBlockedDueToCPSalarms()) )
      {
         DataLog(log_level_proc_donor_disconnect_error) << "Setting disconnect test alarm (mss version): " << DISCONNECT_TEST_MSS_ALARM << endmsg;
         _DisconnectTestMssAlarm.setAlarm();
      }
      else
      {
         DataLog(log_level_proc_donor_disconnect_error) << "Setting disconnect test alarm: " << DISCONNECT_TEST_ALARM << endmsg;
         _DisconnectTestAlarm.setAlarm();
      }

      _TestPassed = 1;
      DataLog(log_level_proc_donor_disconnect_error) << "Donor disconnect test passed, after " << TEST_FAILED_COUNT << " tries" << endmsg;

      //
      // Request power from Safety ... it could be waiting
      //
      request24VPower();
   }
   else // _TestPassed == 1
   {
      // Already displayed alarm and waiting for GUI to say we can transition;
      // however, Safety may be waiting for 24V power request to finalize its test.
      request24VPower();
   }
}

void DonorDisconnect::performTest ()
{
   ProcData pd;

   //
   // Check to see if the test should be performed at all.
   // If the cassette is not in the DOWN position the test cannot
   // be properly performed.
   //
   if ( pd.Status().CassPosIsUnknown() )
   {
      //
      // Cassette is out of position
      //
      _CassettePositionError = 1;

      if ( !_PowerOn )
      {
         //
         // Request power from Safety ...
         //
         request24VPower();
      }

      DataLog(log_level_proc_donor_disconnect_error) << "Setting bad cassette position alarm: " << UNLOADING_BAD_CASSETTE_POSITION << endmsg;
      _BadCassettePositionAlarm.setAlarm();
      _TestFailed = 1;

      //
      // This alarm counts as the first failure ...
      //
      _AlarmDisplayCount++;
      _TestHasFailed = true;

      // we need to short circuit this because we'll never
      // get a message from safety in this case. IT 8303
      // (see 4839 for background on the _safetyConfirmed message)
      _safetyConfirmed = true;
   }
   //
   //  If we are below the absolute minimum pressure then simply fail
   //  the test.  This is to avoid APS decoupling.
   //
   else if ( pd.Status().APS() < MINIMUM_PRESSURE)
   {
      DataLog(log_level_proc_donor_disconnect_error) << "disconnect test failed absolute min pressure at " << pd.Status().APS() << endmsg;
      //
      // Stop pumps and fail the test now, but defer setting notification timer (FailPaused=1);
      // testTooSlow() will set it if need be. This prevents Control's test from
      // getting too far ahead of Safety when donor remains connected.
      //
      stopPumps();
      _TestFailed = 1;
      _FailPaused = 1;
      if ( !_PowerOn || _TestHasFailed )
      {
         //
         // When TestHasFailed, this is a re-test. Request power from Safety
         // (which also triggers it to start its disconnect test)
         //
         request24VPower();
      }
   }
   //
   //  If we are in hypovolemia then it's bad to run the pump away from the
   //  donor (IT3704).  In cases where we know that we're hypovolemic or
   //  we can't determine that it's safe just fail the test.
   //
   else if ( Hypovolemic() )
   {
      DataLog(log_level_proc_donor_disconnect_error) << "disconnect test failed due to hypovolemia" << endmsg;
      //
      // Stop pumps and fail the test now, but defer setting notification timer (FailPaused=1);
      // testTooSlow() will set it if need be. This prevents Control's test from
      // getting too far ahead of Safety when donor remains connected.
      //
      stopPumps();
      _TestFailed = 1;
      _FailPaused = 1;
      if ( !_PowerOn || _TestHasFailed )
      {
         //
         // When TestHasFailed, this is a re-test. Request power from Safety
         // (which also triggers it to start its disconnect test)
         //
         request24VPower();
      }
   }
   else if ( pd.Status().CassIsDown() )
   {
      //
      // Perform current step in the test ...
      //
      _TestStep();
   }
}

void DonorDisconnect::setUpTest ()
{
   ProcData pd;

   //
   // Request power from Safety ...
   //
   request24VPower();

   //
   // This step is complete ...
   //
   _TestStep = CallbackBase();

   // Reset pressurized flag
   _AchievedPressureGoal = 0;

   //
   // If this is the first time through ensure that the "last
   //  target pressure" is set to the proper value (the repeat
   //  delta will be subtracted out below).
   //
   if ( _TargetPressure == 0.0f)
      _TargetPressure = INITIAL_PRESSURE + REPEAT_DELTA;

   //
   // Set the desired APS limit to the lesser of the current APS
   //  minus a skosh or, at least, to the desired pressure.  We
   //  inform the low-level driver of our intentions via the low
   //  limit so the driver will stop the pumps for us.  Hopefully
   //  the pumps stop AFTER we have achieved the pressure.  Note
   //  that this code assumes that the sign of the desired pressure
   //  is negative.
   //
   float temp = ( pd.Status().APS() < _TargetPressure ) ? pd.Status().APS() : _TargetPressure;
   temp           -= REPEAT_DELTA;
   _TargetPressure = temp;
   pd.Orders().APSLow(_TargetPressure);

   //
   // We enforce a maximum (most positive) pressure which will fail
   //  the test if it is exceeded.
   //
   _MaximumPressure = pd.Status().APS() - 10.0f;
   if ( _MaximumPressure > -100.0f )
      _MaximumPressure = -100.0f;

   //
   // Log pertinent data to both trace log and run_data file
   //
   DataLog(log_level_proc_donor_disconnect_info) << "disconnect start APS= " << pd.Status().APS() <<
      " target= " << _TargetPressure <<
      " max= " << _MaximumPressure << endmsg;
}

void DonorDisconnect::pressurize ()
{
   ProcData pd;

   //
   // Pressurize the set to the configured limit.  This state continues
   //  until the pressure is met or we are killed from the timeout timer.
   //  In case we started out below our desired pressure we ensure that
   //  the APS is at least changing in the proper direction (to catch
   //  cases where the APS is decoupled or failed).
   //
   if ( _AchievedPressureGoal )
   {
      stopPumps();
      DataLog(log_level_proc_donor_disconnect_info) << "disconnect test pressurized to " << pd.Status().APS() << endmsg;
   }
   else
   {
      pd.Orders().Qin( ( ( pd.Status().APS()<INITIAL_PRESSURE ) ? 20.0f : 50.0f ) );
      DataLog(MY_DEBUG_LOG_LEVEL) << "Commanded Inlet pump to " << pd.Orders().Qin() << endmsg;
   }

   //
   // Once the set has been pressurized we wait for the pumps to
   //  come to a complete stop.
   //
   if ( pd.Orders().Qin() == 0.0f &&               // Pumps have been commanded to stop and they have stopped
        pd.Status().InletPump.ActFlow() < 0.002f ) // approximately 40 encoder ticks
   {
      //
      // Now that the pumps have stopped we wait for the system to
      //  stabilize.  This allows a good first reading for the hold.
      //  We use absolute time for this phase since we are just waiting
      //  for pressure variances to even out.
      //

      //
      // Stop this timer to allow for stabilization ...
      //
      _TestTimer.interval(0);
      DataLog(log_level_proc_donor_disconnect_info) << "Stopped test timer" << endmsg;

      //
      // Start the stabilization and pressure hold timers ...
      //
      _StabilizationTimer.interval(STABILIZATION_TIME);
      _PressureHoldTimer.interval(HOLD_TIME + STABILIZATION_TIME);
      DataLog(log_level_proc_donor_disconnect_info) << "Started stabilization timer " << STABILIZATION_TIME << endmsg;
      DataLog(log_level_proc_donor_disconnect_info) << "Started pressure hold timer " << HOLD_TIME + STABILIZATION_TIME << endmsg;

      DataLog(log_level_proc_donor_disconnect_info) << "disconnect test APS after pumps stop = " << pd.Status().APS() << endmsg;

      //
      // We are complete with this step ...
      //
      _TestStep = CallbackBase();
   }
}

void DonorDisconnect::pressureStabilized ()
{
   ProcData pd;

   //
   // Stop the timer ...
   //
   _StabilizationTimer.interval(0);
   DataLog(log_level_proc_donor_disconnect_info) << "Stop the stabilization timer" << endmsg;

   //
   // Save the stabilized pressure ...
   //
   _CurrentAPS = pd.Status().APS();
   DataLog(log_level_proc_donor_disconnect_info) << "disconnect test APS after stabilization = " << _CurrentAPS << endmsg;


   //
   //  If we've gone above our local maximum pressure then we fail.
   //
   if ( _CurrentAPS > _MaximumPressure)
   {
      _TestFailed = 1;
      DataLog(log_level_proc_donor_disconnect_error) << "disconnect test failed max pressure at " << _CurrentAPS << endmsg;
   }

   //
   // Hold the pressure until the desired hold time has elapsed.
   //
}

void DonorDisconnect::pressureCheck ()
{
   ProcData pd;

   //
   // Stop the timer ...
   //
   _PressureHoldTimer.interval(0);
   DataLog(log_level_proc_donor_disconnect_info) << "Stop the pressure hold timer" << endmsg;

   //
   // Note that hold_aps is assumed to be negative, so a decay would cause
   //  it to go above (more positive than) the starting pressure + margin.
   //

   //
   // After the hold time has expired the APS must be within the
   //  configured margin.
   //
   DataLog(log_level_proc_donor_disconnect_info) << "disconnect test APS after hold= " << pd.Status().APS() <<
      " margin= " << MARGIN << endmsg;


   if ( pd.Status().APS() <= ( _CurrentAPS + MARGIN ) )
   {
      //
      // Stop the notification timer ...
      //
      _NotificationTimer.interval(0);
      DataLog(log_level_proc_donor_disconnect_info) << "Stop the notification timer" << endmsg;

      _TestPassed = 1;
      _TestFailed = 0;
      DataLog(log_level_proc_donor_disconnect_info) << "Test passed because pressure held" << endmsg;
   }
   else
   {
      _TestFailed = 1;
      DataLog(log_level_proc_donor_disconnect_error) << "disconnect test failed hold margin test" << endmsg;
   }
}

void DonorDisconnect::shouldStartTest ()
{
   //
   // We have been given permission to start the test
   //
   _PerformTest = 1;

   //
   // Let's prepare the test ...
   //
   _TestStep = Callback<DonorDisconnect>(this, &DonorDisconnect::setUpTest);

   DataLog(log_level_proc_donor_disconnect_info) << "Received disconnect test confirmation message" << endmsg;
}

void DonorDisconnect::testTooSlow ()
{
   //
   // Test failed because the test wasn't fast enough
   //
   _TestFailed = 1;
   _FailPaused = 0;
   DataLog(log_level_proc_donor_disconnect_error) << "disconnect test failed timeout" << endmsg;
}

void DonorDisconnect::powerRequestResponse ()
{
   const long status = _PowerRequestResponseMsg.getData();

   DataLog(log_level_proc_donor_disconnect_info) << "Received power request response: " << status << endmsg;

   ProcData pd;

   if ( pd.Status().PS24Vsw.Acceptable() )
   {
      //
      // If power was turned on, then proceed with the
      //  next step in the test ...
      //
      _TestStep = Callback<DonorDisconnect>(this, &DonorDisconnect::pressurize);
      DataLog(log_level_proc_donor_disconnect_info) << "Power acceptable" << endmsg;
   }
   else if ( (status > 0)
             && ( (status & PWR_CONTROL_24V_NOMINAL) ||
                  (status & PWR_CONTROL_BOTH_NOMINAL) ) ) // may be both now
   {
      //
      // Check again ...
      //
      _TestStep = Callback<DonorDisconnect>(this, &DonorDisconnect::powerRequestResponse);
      DataLog(log_level_proc_donor_disconnect_info) << "Power out of range -- waiting for power" << endmsg;
   }
   else
   {
      //
      // Test failed - safety refused to turn on the power
      //
      _TestFailed = 1;
      _FailPaused = 0;
      DataLog(log_level_proc_donor_disconnect_error) << "power request refused: " << status << endmsg;
   }
}

void DonorDisconnect::reachedPressure ()
{
   //
   // The control driver stopped the pumps because we reached the
   //  pressure goal ...
   //
   _AchievedPressureGoal = 1;
   DataLog(log_level_proc_donor_disconnect_info) << "disconnect test - low level driver stopped pumps" << endmsg;
}

void DonorDisconnect::request24VPower ()
{
   if ( !_PowerRespRegistered )
   {
      //
      // Register for the response for the power request ...
      // (can't do it in preEnter() because hypovolemia scenario may get the
      // premature/unsolicited response requested during previous state)
      //
      _PowerRequestResponseMsg.init(Callback<DonorDisconnect>(this, &DonorDisconnect::powerRequestResponse) );
      DataLog(log_level_proc_info) << "Create power request response message " << endmsg;

      _PowerRespRegistered = true;
   }

   //
   // Request safety power ...
   //
   RequestSafetyPowerMsg powerRequestMsg(MessageBase::SEND_GLOBAL);
   powerRequestMsg.send(ENABLE_24V_POWER);
   DataLog(log_level_proc_donor_disconnect_info) << "Sent the request to enable 24V Power " << endmsg;

   //
   // Start the request timer
   //
   _TestTimer.interval(TEST_FAILURE_TIME);
   DataLog(log_level_proc_donor_disconnect_info) << "Started the test timer " << TEST_FAILURE_TIME << endmsg;
}


void DonorDisconnect::stopPumps ()
{
   DataLog(log_level_proc_donor_disconnect_info) << "Stop all pumps" << endmsg;

   ProcData pd;
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().RPM(0.0f);
}

void DonorDisconnect::closeValves ()
{
   ProcData pd;

   //
   // Command all valves to the return position
   //
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
}

void DonorDisconnect::stopTimers ()
{
   //
   // Turn off my timer ...
   //
   _TestTimer.interval(0);
   _NotificationTimer.interval(0);
   _StabilizationTimer.interval(0);
   _PressureHoldTimer.interval(0);
   DataLog(log_level_proc_donor_disconnect_info) << "Stop all timers" << endmsg;
}

void DonorDisconnect::reset ()
{
   //
   // Reset the flags ...
   //
   _PerformTest           = 0;
   _PowerOn               = 0;
   _TestPassed            = 0;
   _TestFailed            = 0;
   _FailPaused            = 0;
   _CassettePositionError = 0;
   _TestStep              = CallbackBase();
   _AchievedPressureGoal  = 0;
   _AlarmDisplayCount     = 0;

   _TestHasFailed      = false;
   _ReadyForTransition = false;
   _safetyConfirmed    = false;
}

int DonorDisconnect::Hypovolemic ()
{
   ProcData pd;

   //
   //
   //   If we don't have a donor or disposable then we cannot assume hypovolemia
   //
   if ( pd.TrimaSet().Cassette.Get() != cassette::NOT_LOADED
        && pd.TrimaSet().Cassette.Get() != cassette::UNKNOWN
        && pd.Donor().TBV.Get() > 0.0f )
   {
      //
      //  Subtract off the amount of inlet volume processed so far
      //
      float remaining = _HypovolemiaDistance - pd.Volumes().VinTotal.Get() + _VinAtStart;
      if (remaining < 0.0f)
      {
         return 1;
      }
   }
   return 0;
}

void DonorDisconnect::cleanup ()
{
   //
   // Deregister for my messages ...
   //
   _PermissionGrantedMsg.deregisterMsg();
   _PowerRequestResponseMsg.deregisterMsg();
   _PowerRespRegistered = false;
   _APSMsg.deregisterMsg();
   _SetRunType.deregisterMsg();
}


void DonorDisconnect::setDisconMssStatus ()
{
   bool readyForRas = (readyForRBCMSS() && !RASBlockedDueToCPSalarms());
   bool readyForPas = readyForPLTMSS();

   // Don't worry about sending this info to GUI in a message anymore.
   // GUI will read from CDS to be more robust to PFR.
   ProcData pd;
   pd.MakeRunWritable();
   pd.Run().MeterRbcs_OK.Set(readyForRas);
   pd.Run().MeterPlatelets_OK.Set(readyForPas);
   pd.MakeRunReadable();
}


void DonorDisconnect::safetyStatus ()
{
   ProcData pd;

   // disconnect Successful =1;  disconnect Unsuccessful =0;
   const long status = _SafetyDiscoStatus.getData();

   if (status == 1)
   {
      _safetyConfirmed = true;
      DataLog(log_level_proc_donor_disconnect_info) << "Proc hears Safety say disconnect PASSED!! FailPaused=" << _FailPaused << endmsg;
   }
   else
   {
      _safetyConfirmed = false;
      DataLog(log_level_proc_donor_disconnect_info) << "Proc hears Safety say disconnect UNSUCCESSFUL!! status=" << status << endmsg;
   }

   // If Safety says failure, or Control has deferred its failure ...
   if ( (!_safetyConfirmed || _FailPaused) && !pd.AlarmActive() )
   {
      // No need to wait any longer; raise alert now vs. waiting for notification timeout
      testFailed();
   }
}

/* FORMAT HASH 6b59cd9dd0bc1314bdaf6b43ed685335 */
