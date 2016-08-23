/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      doorlatchtest.cpp
 *
 */

#include "doorlatchtest.h"
#include "procdata.h"

DEFINE_STATE(DoorLatchTest)

const int POWER_REQUEST_TIME = 8000;      // 8000  milliseconds ( 8 seconds )
const int SOLENOID_MOVE_TIME = 6000;      // 6000  milliseconds ( 6 seconds )
const int MAX_TIME_TO_MOVE   = 10000;     // 10000 milliseconds ( 10 seconds )

//
// These times are used for the unlock without power test
//
const int MOVE_TIME_FUDGE_FACTOR      = 2000;   // additional time to move beyond what the original move took (msec)
const int MAX_MOVE_TIME_WITHOUT_POWER = 3500;   // maximum allowed delay for unlock without power test

DoorLatchTest::DoorLatchTest()
   : StateAbs(),
     _PowerResponseMsg(),
     _DoorLatchTimer(),
     _DoorLatchAlarm(DOOR_LATCH_FAILURE),
     _CurrentStage(),
     _TimerStarted(0),
     _PowerRequestGranted(0),
     _MoveTimer(),
     _TestPassed(0)
{}

DoorLatchTest::DoorLatchTest(const DoorLatchTest& state)
   : StateAbs(state),
     _PowerResponseMsg(),
     _DoorLatchTimer(),
     _DoorLatchAlarm(DOOR_LATCH_FAILURE),
     _CurrentStage(),
     _TimerStarted(0),
     _PowerRequestGranted(0),
     _MoveTimer(),
     _TestPassed(0)
{}

DoorLatchTest::~DoorLatchTest()
{
   cleanup();
}

int DoorLatchTest::transitionStatus ()
{
   if ( _TestPassed )
   {
      //
      // Test passed ...
      //
      DataLog(log_level_proc_startup_test_info) << "Door latch test passed" << endmsg;
      return 1;
   }
   return NO_TRANSITION;
}

void DoorLatchTest::powerResponse ()
{
   if ( _PowerResponseMsg.getData() == PWR_CONTROL_SOLENOID_NOMINAL )
   {
      //
      // Power Request granted, set the flag ...
      //
      _PowerRequestGranted = 1;
      DataLog(log_level_proc_debug) << "Power request granted " << PWR_CONTROL_SOLENOID_NOMINAL << endmsg;
   }
   else if ( _PowerResponseMsg.getData() == PWR_CONTROL_SOLENOID_IGNORED )
   {
      //
      // Reset the flags ...
      //
      reset();

      DataLog(log_level_proc_error) << "Power request denied" << endmsg;

      //
      // The request was denied, set the alarm ...
      //
      setDoorAlarm();
   }
}

void DoorLatchTest::setDoorAlarm ()
{
   //
   // Send alarm message indicating door latch test failed
   //  the current stage ...
   //
   _DoorLatchAlarm.setAlarm();
   DataLog(log_level_proc_debug) << "Set Door Latch failure alarm " << DOOR_LATCH_FAILURE << endmsg;

}

void DoorLatchTest::stage1Timeout ()
{
   DataLog(log_level_proc_error) << "Stage1Timeout" << endmsg;
   //
   // Set the alarm ...
   //
   setDoorAlarm();

   //
   // Set the current stage ...
   //
   stage1Callback();
}

void DoorLatchTest::stage1Callback ()
{
   //
   // Reset the flags ...
   //
   reset();

   //
   // Reset the current stage to stage 1 ...
   //
   _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::stage1);

}

void DoorLatchTest::stage2Timeout ()
{
   DataLog(log_level_proc_error) << "Stage2Timeout" << endmsg;
   //
   // Set the alarm ...
   //
   setDoorAlarm();

   //
   // Set the current stage ...
   //
   stage2Callback();
}

void DoorLatchTest::stage2Callback ()
{
   //
   // Reset the flags ...
   //
   reset();

   //
   // Reset the current stage to stage 1 ...
   //
   _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::stage2);

}

void DoorLatchTest::stage3Timeout ()
{
   DataLog(log_level_proc_error) << "Stage3Timeout" << endmsg;
   //
   // Set the alarm ...
   //
   setDoorAlarm();

   //
   // Set the current stage ...
   //
   stage3Callback();
}

void DoorLatchTest::stage3Callback ()
{
   //
   // Reset the flags ...
   //
   reset();

   //
   // Reset the current stage to stage 1 ...
   //
   _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::stage3);

}

int DoorLatchTest::preProcess ()
{
   ProcData pd;
   if ( !pd.AlarmActive() )
   {
      //
      // Perform the processing for the current stage of the system ...
      //
      _CurrentStage();
   }

   return NO_TRANSITION;
}

int DoorLatchTest::postProcess ()
{
   return NO_TRANSITION;
}

int DoorLatchTest::preEnter ()
{
   //
   // Create the power response message.
   //
   _PowerResponseMsg.init(Callback<DoorLatchTest>(this, &DoorLatchTest::powerResponse) );
   DataLog(log_level_proc_debug) << "Create power response message" << endmsg;

   //
   // Set the current stage to the first stage ...
   //
   _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::stage1);

   //
   // Reset everything ...
   //
   _DoorLatchTimer.init(0, Callback<DoorLatchTest>(this, &DoorLatchTest::stage1Timeout), TimerMessage::DISARMED);
   reset();

   return NORMAL;
}

int DoorLatchTest::postExit ()
{
   ProcData pd;

   //
   // Unlock the door ...
   //
   pd.Orders().UnlockDoor();

   //
   // Cleanup the dynamic data ...
   //
   cleanup();

   return NORMAL;
}

void DoorLatchTest::stage1 ()
{
   DataLog(log_level_proc_debug) << "Current Stage -> stage1" << endmsg;

   //
   // Order the solenoid power to the on position
   //
   powerOn(POWER_REQUEST_TIME);

   //
   // If the power is on ...
   //
   if ( _PowerRequestGranted )
   {
      DataLog(log_level_proc_debug) << "Solenoid Power is On" << endmsg;
      //
      // Reset the flags ...
      //
      reset();

      //
      // If the door is locked, go down the
      //  alternate test path ...
      //
      ProcData pd;
      if ( pd.Status().DoorIsLocked() )
      {
         _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::alternatePath);
      }
      //
      // ... else go down the main test path ...
      //
      else
      {
         _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::mainPath);
      }

   }
}

void DoorLatchTest::alternatePath ()
{
   ProcData pd;
   DataLog(log_level_proc_debug) << "Current Stage -> alternatePath" << endmsg;
   //
   // Unlock the door ...
   //
   _DoorLatchTimer.init(0, Callback<DoorLatchTest>(this, &DoorLatchTest::stage1Timeout), TimerMessage::DISARMED);
   unlockDoor(SOLENOID_MOVE_TIME);

   //
   // When the door is unlocked ...
   //
   if ( !pd.Status().DoorIsLocked() )
   {
      DataLog(log_level_proc_debug) << "Door is Unlocked" << endmsg;
      //
      // Reset the flags ...
      //
      reset();

      //
      // Go down the main test path ...
      //
      _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::mainPath);
   }
}

void DoorLatchTest::mainPath ()
{
   ProcData pd;
   DataLog(log_level_proc_debug) << "Current Stage -> mainPath" << endmsg;
   //
   // Lock the door ...
   //
   _DoorLatchTimer.init(0, Callback<DoorLatchTest>(this, &DoorLatchTest::stage1Timeout), TimerMessage::DISARMED);
   lockDoor(SOLENOID_MOVE_TIME);

   //
   // Store the time it takes to move the door solenoid
   //  from unlocked to locked ...
   //
   _MoveTimer.activate();

   //
   // When the door is locked ...
   //
   if ( pd.Status().DoorIsLocked() )
   {
      DataLog(log_level_proc_debug) << "Door is Locked" << endmsg;
      //
      // Reset the flags ...
      //
      reset();

      //
      // The door solenoid moved, stop accumulating time
      //
      _MoveTimer.inactivate();
      DataLog(log_level_proc_debug) << "Move Timer inactivated" << _MoveTimer.getMSecs() << endmsg;

      //
      // Go to stage 2 of the test ...
      //
      _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::stage2);
   }

}

void DoorLatchTest::stage2 ()
{
   DataLog(log_level_proc_debug) << "Current Stage -> stage2" << endmsg;
   //
   // Order the power to off ...
   //
   _DoorLatchTimer.init(0, Callback<DoorLatchTest>(this, &DoorLatchTest::stage2Timeout), TimerMessage::DISARMED);
   powerOff(POWER_REQUEST_TIME);

   //
   // If the power is off ...
   //
   if ( _PowerRequestGranted )
   {
      DataLog(log_level_proc_debug) << "Solenoid Power is Off" << endmsg;
      //
      // Reset the flags ...
      //
      reset();

      //
      // Test the door solenoid with the power off ...
      //
      _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::unlockWithoutPower);
   }

}

void DoorLatchTest::unlockWithoutPower ()
{
   ProcData pd;
   DataLog(log_level_proc_debug) << "Current Stage -> unlockWithoutPower" << endmsg;
   //
   // Attempt to unlock the door ...
   //
   _DoorLatchTimer.init(0, Callback<DoorLatchTest>(this, &DoorLatchTest::stage3Callback), TimerMessage::DISARMED);

   int timeout = _MoveTimer.getMSecs() + MOVE_TIME_FUDGE_FACTOR;
   if ( timeout > MAX_MOVE_TIME_WITHOUT_POWER )
   {
      timeout = MAX_MOVE_TIME_WITHOUT_POWER;
   }

   unlockDoor(timeout);

   //
   // If the door is unlocked ...
   //
   if ( !pd.Status().DoorIsLocked() )
   {
      DataLog(log_level_proc_error) << "Door is Unlocked" << endmsg;

      //
      // Reset the flags ...
      //
      reset();

      //
      // Set the alarm ...
      //
      setDoorAlarm();

      //
      // Go back to stage 1 ...
      //
      _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::stage1);
   }

}

void DoorLatchTest::stage3 ()
{
   DataLog(log_level_proc_debug) << "Current Stage -> stage3" << endmsg;

   //
   // Order the power to on ...
   //
   _DoorLatchTimer.init(0, Callback<DoorLatchTest>(this, &DoorLatchTest::stage3Timeout), TimerMessage::DISARMED);
   powerOn(POWER_REQUEST_TIME);

   //
   // The door should already be locked, so we will just synchronize
   //  the orders with the current state ( the timer will not be active
   //  for this command ) ...
   //
   lockDoor(0);

   //
   // If the power is on ...
   //
   if ( _PowerRequestGranted )
   {
      DataLog(log_level_proc_debug) << "Solenoid Power is On" << endmsg;
      //
      // Reset the flags ...
      //
      reset();

      //
      // Finish the test ...
      //
      _CurrentStage = Callback<DoorLatchTest>(this, &DoorLatchTest::unlockWithPower);
   }

}

void DoorLatchTest::unlockWithPower ()
{
   ProcData pd;
   DataLog(log_level_proc_debug) << "Current Stage -> unlockWithPower" << endmsg;

   //
   // Attempt to unlock the door ...
   //
   _DoorLatchTimer.init(0, Callback<DoorLatchTest>(this, &DoorLatchTest::stage3Timeout), TimerMessage::DISARMED);
   unlockDoor(SOLENOID_MOVE_TIME);

   //
   // If the door is unlocked ...
   //
   if ( !pd.Status().DoorIsLocked() )
   {
      DataLog(log_level_proc_debug) << "Door is Unlocked" << endmsg;
      //
      // Reset the flags ...
      //
      reset();

      //
      // We are finished ...
      //
      _TestPassed   = 1;
      _CurrentStage = CallbackBase();
   }

}

void DoorLatchTest::powerOn (const int time)
{
   if ( !_TimerStarted )
   {
      //
      // Request power ...
      //
      RequestSafetyPowerMsg powerRequestMsg(MessageBase::SEND_GLOBAL);
      powerRequestMsg.send(ENABLE_SOLENOID_POWER);
      DataLog(log_level_proc_debug) << "Power Request message sent " << ENABLE_SOLENOID_POWER << endmsg;

      //
      // Start the timer ...
      //
      _DoorLatchTimer.interval(time);
      DataLog(log_level_proc_debug) << "Door latch timer started " << time << endmsg;
      _TimerStarted = 1;
   }

}

void DoorLatchTest::powerOff (const int time)
{
   if ( !_TimerStarted )
   {
      //
      // Request power ...
      //
      RequestSafetyPowerMsg powerRequestMsg(MessageBase::SEND_GLOBAL);
      powerRequestMsg.send(DISABLE_SOLENOID_POWER);
      DataLog(log_level_proc_debug) << "Power Request message sent " << DISABLE_SOLENOID_POWER << endmsg;

      //
      // Start the timer ...
      //
      _DoorLatchTimer.interval(time);
      DataLog(log_level_proc_debug) << "Door latch timer started" << time << endmsg;
      _TimerStarted = 1;
   }

}

void DoorLatchTest::unlockDoor (const int time)
{
   ProcData pd;
   //
   // Unlock the door ...
   //
   pd.Orders().UnlockDoor();
   DataLog(log_level_proc_debug) << "Unlock Door" << endmsg;

   if ( !_TimerStarted )
   {
      int timeout = time;
      if ( timeout > MAX_TIME_TO_MOVE )
      {
         timeout = MAX_TIME_TO_MOVE;
      }

      //
      // Start the timer ...
      //
      _DoorLatchTimer.interval(timeout);
      DataLog(log_level_proc_debug) << "Door latch timer started " << timeout << endmsg;
      _TimerStarted = 1;
   }
}

void DoorLatchTest::lockDoor (const int time)
{
   ProcData pd;
   //
   // Lock the door ...
   //
   pd.Orders().LockDoor();
   DataLog(log_level_proc_debug) << "Lock Door" << endmsg;

   if ( !_TimerStarted )
   {
      int timeout = time;
      if ( timeout > MAX_TIME_TO_MOVE )
      {
         timeout = MAX_TIME_TO_MOVE;
      }

      //
      // Start the timer ...
      //
      _DoorLatchTimer.interval(timeout);
      DataLog(log_level_proc_debug) << "Door latch timer started " << timeout << endmsg;
      _TimerStarted = 1;
   }
}


void DoorLatchTest::reset ()
{
   ProcData pd;
   //
   // Reset the door command ...
   //
   if ( pd.Status().DoorIsLocked() )
      pd.Orders().LockDoor();
   else
      pd.Orders().UnlockDoor();

   //
   // Reset the flags ...
   //
   _TimerStarted        = 0;
   _PowerRequestGranted = 0;
   _TestPassed          = 0;
   DataLog(log_level_proc_debug) << "Flags reset" << endmsg;

   //
   // Stop the timer ...
   //
   _DoorLatchTimer.interval(0);
   DataLog(log_level_proc_debug) << "Door latch timer stopped" << endmsg;
}

void DoorLatchTest::cleanup ()
{
   //
   // Delete the messages ...
   //
   _PowerResponseMsg.deregisterMsg();
}

/* FORMAT HASH 08a0217f4a7afb1729b1b6ba306c803f */
