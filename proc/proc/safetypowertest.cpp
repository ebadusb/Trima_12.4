/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      safetypowertest.cpp
 *
 */


#include "safetypowertest.h"
#include "safproc.h"
#include "procdata.h"

const float TIME_TO_WAIT_FOR_SAFETY = 30000;    // Maximum time before power test timeout ( in msecs )

DEFINE_STATE(SafetyPowerTest);

SafetyPowerTest::SafetyPowerTest()
   : StateAbs(),
     _TestStarted(false),
     _TestComplete(false),
     _TestFailed(false),
     _RequestSafetyTestMsg(),
     _SafetyTestDoneMsg(),
     _SafetyTooSlowTimer(0, Callback<SafetyPowerTest>(this, &SafetyPowerTest::safetyTooSlow), TimerMessage::DISARMED),
     _PowerTestFailAlarm(POWER_TEST_FAILURE)
{}

SafetyPowerTest::SafetyPowerTest(const SafetyPowerTest& state)
   : StateAbs(state),
     _TestStarted(state._TestStarted),
     _TestComplete(state._TestComplete),
     _TestFailed(state._TestFailed),
     _RequestSafetyTestMsg(),
     _SafetyTestDoneMsg(),
     _SafetyTooSlowTimer(0, Callback<SafetyPowerTest>(this, &SafetyPowerTest::safetyTooSlow), TimerMessage::DISARMED),
     _PowerTestFailAlarm(POWER_TEST_FAILURE)
{
   copyOver(state);
}

SafetyPowerTest::~SafetyPowerTest()
{
   cleanup();
}

int SafetyPowerTest::transitionStatus ()
{
   return _TransitionStatus;
}

void SafetyPowerTest::safetyTestDone ()
{
   int response = _SafetyTestDoneMsg.getData();

   if (    response == PWR_CONTROL_24V_NOMINAL
           || response == PWR_CONTROL_BOTH_NOMINAL )
   {
      //
      // Stop the timer ...
      //
      _SafetyTooSlowTimer.interval(0);
      DataLog(log_level_proc_debug) << "Stopped the safety power test (24v) timer" << endmsg;

      //
      // Log the safety test completed
      //
      DataLog(log_level_proc_startup_test_info) << "Safety power test (24v) complete" << endmsg;

      //
      // Set the flag to notify that the test is complete ...
      //
      _TestComplete = true;
   }
}

void SafetyPowerTest::safetyTooSlow ()
{
   //
   // Send alarm message indicating valve tests failed
   //
   _PowerTestFailAlarm.setAlarm();
   DataLog(log_level_proc_debug) << "Set the power test (24v) fail alarm " << POWER_TEST_FAILURE << endmsg;

   //
   // Log the failure
   //
   DataLog(log_level_proc_debug) << "Power test (24v) fail: timeout occurred" << endmsg;
   DataLog(log_level_proc_debug) << "Safety power test (24v) failed to complete " << TIME_TO_WAIT_FOR_SAFETY << endmsg;

   //
   // Set the flag to notify that the test failed ...
   //
   _TestFailed = true;

   //
   // Stop the timer ...
   //
   _SafetyTooSlowTimer.interval(0);
}

void SafetyPowerTest::startTest ()
{
   //
   // Request that safety start the power tests NOW.
   //
   _RequestSafetyTestMsg.send(ENABLE_24V_POWER);
   DataLog(log_level_proc_startup_test_info) << "Request power test ..." << ENABLE_24V_POWER << endmsg;

   //
   // Start the timer ...
   //
   _SafetyTooSlowTimer.interval(TIME_TO_WAIT_FOR_SAFETY);
   DataLog(log_level_proc_debug) << "Create safety power test (24v) timer " << TIME_TO_WAIT_FOR_SAFETY << endmsg;

   _TestStarted = true;
}

int SafetyPowerTest::preProcess ()
{
   ProcData pd;

   if ( !pd.AlarmActive() )
   {
      //
      // Wahoo!! the test is finished ...
      //
      if ( _TestComplete )
      {
         DataLog(log_level_proc_startup_test_info) << "Safety power test (24v) passed" << endmsg;
         _TransitionStatus = 1;
      }
      //
      // The test didn't finish before the timer expired ...
      //
      else if ( _TestFailed )
      {
         //
         // Reset the test failed flag to test again ...
         //
         _TestFailed  = false;
         _TestStarted = false;
      }
   }
   return NORMAL;
}

int SafetyPowerTest::postProcess ()
{
   ProcData pd;

   if ( !pd.AlarmActive() )
   {
      //
      // Check to see if we need to run the test ...
      //
      if ( !_TestStarted )
      {
         startTest();
      }
   }

   //
   // Check that the pumps don't run during the safety power test.  If they
   // are above a low threshold for 3 consecutive scans, or above a higher
   // threshold on a single scan, the system generates an A2 to shut down the
   // system.
   //
   if ( _TestStarted )
   {
      const float highLimit = 3.0;   /* ml/min */

      if ( pd.Status().ACPump.ActFlow() > highLimit ||
           pd.Status().CollectPump.ActFlow() > highLimit ||
           pd.Status().InletPump.ActFlow() > highLimit ||
           pd.Status().PlasmaPump.ActFlow() > highLimit ||
           fabs(pd.Status().ReturnPump.ActFlow()) > highLimit )
      {
         DataLog(log_level_control_alarm_detail) << "Safety power test failed.  " <<
            "AC=" << pd.Status().ACPump.ActFlow() << " " <<
            "COL=" << pd.Status().CollectPump.ActFlow() << " " <<
            "Inlet=" << pd.Status().InletPump.ActFlow() << " " <<
            "Plasma=" << pd.Status().PlasmaPump.ActFlow() << " " <<
            "Return=" << pd.Status().ReturnPump.ActFlow() << " " <<
            "Limit=" << highLimit << endmsg;

         _RequestSafetyTestMsg.send(DISABLE_24V_POWER);

         anAlarmMsg outOfControlAlarm(HARDWARE_FAULT);
         outOfControlAlarm.setAlarm();
      }

   }

   return NORMAL;
}

int SafetyPowerTest::preEnter ()
{
   ProcData pd;
   //
   // Stop the pumps and centrifuge if they were moving.
   // Set the centrifuge inhibit bit.
   //
   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);
   DataLog(log_level_proc_debug) << "Stop all pumps" << endmsg;

   //
   // Create the request safety test message
   //
   _RequestSafetyTestMsg.init(MessageBase::SEND_GLOBAL);
   DataLog(log_level_proc_debug) <<  "Create request safety power message" <<  endmsg;

   //
   // Create the safety test done message.
   //
   _SafetyTestDoneMsg.init(Callback<SafetyPowerTest>(this, &SafetyPowerTest::safetyTestDone) );
   DataLog(log_level_proc_debug) << "Create safety power test (24v) complete message " << endmsg;

   return NORMAL;
}

int SafetyPowerTest::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   //
   // Stop the timer ...
   //
   _SafetyTooSlowTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stopped the safety power test (24v) timer" << endmsg;

   return NORMAL;
}

void SafetyPowerTest::reset ()
{
   //
   // Reset my status ...
   _TestStarted      = false;
   _TransitionStatus = NO_TRANSITION;
}

void SafetyPowerTest::copyOver (const SafetyPowerTest&)
{}

void SafetyPowerTest::cleanup ()
{
   //
   // Cleanup my messages
   //
   _RequestSafetyTestMsg.deregisterMsg();
   _SafetyTestDoneMsg.deregisterMsg();
}

/* FORMAT HASH 64176c90cdd740dfac077d50c65b6fe3 */
