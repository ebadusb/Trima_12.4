/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      safetypowertest.cpp
 *
 */


#include "cobeconfig.h"
#include "safetypowertest_64v.h"
#include "safproc.h"
#include "procdata.h"


const float TIME_TO_WAIT_FOR_SAFETY    = 30000;    // Maximum time before power test timeout ( in msecs )
const float TIME_TO_WAIT_FOR_DOOR_LOCK = 10000;    // Maximum time before power test timeout ( in msecs )

DEFINE_STATE(SafetyPowerTest64);

SafetyPowerTest64::SafetyPowerTest64()
   : StateAbs(),
     _TestNotStarted(true),
     _TestComplete(false),
     _TestFailed(false),
     _RequestSafetyTestMsg(),
     _SafetyTestDoneMsg(),
     _SafetyTooSlowTimer(0, Callback<SafetyPowerTest64>(this, &SafetyPowerTest64::safetyTooSlow), TimerMessage::DISARMED),
     _DoorDidNotLockTimer(0, Callback<SafetyPowerTest64>(this, &SafetyPowerTest64::doorDidNotLock), TimerMessage::DISARMED)
{}

SafetyPowerTest64::~SafetyPowerTest64()
{
   SafetyPowerTest64::cleanup();
}

int SafetyPowerTest64::transitionStatus ()
{
   return _TransitionStatus;
}

//
// Safety Test Done message handler
//
void SafetyPowerTest64::safetyTestDone ()
{
   if ( !_TestNotStarted )
   {
      const int response = _SafetyTestDoneMsg.getData();

      if ( (response == PWR_CONTROL_64V_NOMINAL) ||
           (response == PWR_CONTROL_BOTH_NOMINAL)
           )
      {
         //
         // Stop the timer ...
         //
         _SafetyTooSlowTimer.interval(0);
         _DoorDidNotLockTimer.interval(0);
         DataLog(log_level_proc_debug) << "Stopped the safety power test (64v) and door lock timers" << endmsg;

         //
         // Log the safety test completed
         //
         DataLog(log_level_proc_startup_test_info) << "Safety power test (64v) complete" << endmsg;

         //
         // Set the flag to notify that the test is complete ...
         //
         _TestComplete = true;
      }
   }
}


//
// Power Test timeout event handler
//
void SafetyPowerTest64::safetyTooSlow ()
{
   //
   // Send alarm message indicating valve tests failed
   //
   PendingCassetteAlarmMsg delayedAlarm(MessageBase::SEND_LOCAL);
   delayedAlarm.send(POWER_TEST_FAILURE);

   //
   // Log the failure
   //
   DataLog(log_level_proc_startup_test_error) << "Power test (64v) fail: timeout occurred. Set the power test (64v) fail alarm " << POWER_TEST_FAILURE << endmsg;

   //
   // Set the flag to notify that the test failed ...
   //
   _TestFailed = true;
}


//
// Door lock command timeout event handler
//
void SafetyPowerTest64::doorDidNotLock ()
{
   ProcData   pd;

   const bool doorIsOpen     = pd.Status().DoorIsOpen();
   const bool doorIsUnlocked = !pd.Status().DoorIsLocked();

   if (doorIsOpen)
   {
      anAlarmMsg alarm(DOOR_OPEN_ALARM);
      alarm.setAlarm("SafetyPowerTest64::Door did not close");
   }
   else if (doorIsUnlocked)
   {
      anAlarmMsg alarm(DOOR_LOCK_ALARM);
      alarm.setAlarm("SafetyPowerTest64::Door did not lock");
   }

   if (pd.Orders().DoorCommandedToLock())
   {
      DataLog(log_level_proc_startup_test_info) << "Unlocking door..." << endmsg;
      pd.Orders().UnlockDoor();
   }

   //
   // Log the failure
   //
   DataLog(log_level_proc_startup_test_error) << "Power test (64v) fail: door lock timeout occurred" << endmsg;
   DataLog(log_level_proc_startup_test_error) << "Safety power test (64v) failed to complete " << endmsg;

   //
   // Set the flag to notify that the test failed ...
   //
   _TestFailed = true;
}


void SafetyPowerTest64::startPowerTest ()
{
   _TestNotStarted = false;

   //
   // Create the safety test done message.
   //
   _SafetyTestDoneMsg.init(Callback<SafetyPowerTest64>(this, &SafetyPowerTest64::safetyTestDone) );
   DataLog(log_level_proc_debug) << "Create safety power test (64v) complete message " << endmsg;

   //
   // Request that safety start the power tests NOW.
   //
   _RequestSafetyTestMsg.send(ENABLE_64V_POWER);
   DataLog(log_level_proc_startup_test_info) << "Request power test ..." << ENABLE_64V_POWER << endmsg;

   //
   // Start the timers ...
   //
   _SafetyTooSlowTimer.interval(TIME_TO_WAIT_FOR_SAFETY);
   DataLog(log_level_proc_debug) << "Create safety power test (64v) timer " << TIME_TO_WAIT_FOR_SAFETY << endmsg;

   //
   // stop the centrifuge if it was moving.
   // Set the centrifuge inhibit bit.
   //
   ProcData pd;
   pd.Orders().RPM(-1.0f);
   DataLog(log_level_proc_debug) << "Stop Centrifuge and set inhibit bit" << endmsg;
}


int SafetyPowerTest64::preProcess ()
{
   ProcData pd;

   // Only evaluate if there are no other active alarms and the centrifuge is not spinning
   if ( !pd.AlarmActive()
        && pd.Status().CentrifugeRPM() < CobeConfig::data().CentrifugeOpenSpeed )
   {
      //
      // Wahoo!! the test is finished ...
      //
      if ( _TestComplete )
      {
         DataLog(log_level_proc_startup_test_info) << "Safety power test (64v) passed" << endmsg;
         _TransitionStatus = 1;
      }
   }

   return NORMAL;
}

int SafetyPowerTest64::postProcess ()
{
   ProcData pd;

   if ( !pd.AlarmActive() &&
        (pd.Status().CentrifugeRPM() < CobeConfig::data().CentrifugeOpenSpeed)
        )
   {
      //
      // Start the test if we can and if we haven't already started ...
      if ( _TestNotStarted )
      {
         if ( !pd.Status().DoorIsOpen() )
         {
            pd.Orders().LockDoor();

            // Arm the Timer to check to make sure the door locked.
            if (_DoorDidNotLockTimer.timerArmed() == TimerMessage::DISARMED)
            {
               DataLog(log_level_proc_startup_test_info) << "Starting Door lock timer" << endmsg;
               _DoorDidNotLockTimer.interval(TIME_TO_WAIT_FOR_DOOR_LOCK);
            }

            if ( pd.Status().DoorIsLocked() )
               startPowerTest();
         }
         else
         {
            //
            // Enable the door alarms to alert the operator to close the door
            //  so we can perform the test ...
            //
            pd.Orders().RPM(1.0f);
         }
      }
   }

   return NORMAL;
}

int SafetyPowerTest64::preEnter ()
{
   //
   // Reset my flags ...
   //
   _TestFailed = false;

   //
   // Create the request safety test message
   //
   _RequestSafetyTestMsg.init(MessageBase::SEND_GLOBAL);
   DataLog(log_level_proc_debug) <<  "Create request safety power message" <<  endmsg;

   return NORMAL;
}

int SafetyPowerTest64::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   //
   // Stop the timers ...
   //
   _SafetyTooSlowTimer.interval(0);
   _DoorDidNotLockTimer.interval(0);

   DataLog(log_level_proc_debug) << "Stopped the safety power test (64v) and Door Lock timer" << endmsg;

   return NORMAL;
}

void SafetyPowerTest64::reset ()
{
   //
   // Reset the flags ...
   //
   _TestNotStarted   = true;
   _TestFailed       = false;
   _TransitionStatus = NO_TRANSITION;
}


void SafetyPowerTest64::cleanup ()
{
   //
   // Cleanup my messages
   //
   _RequestSafetyTestMsg.deregisterMsg();
   _SafetyTestDoneMsg.deregisterMsg();
}

/* FORMAT HASH 6801e557ed115685a92e7a79f665952d */
