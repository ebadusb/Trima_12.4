/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      doorlatchtest.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  Startup Tests - DOOR LATCH TEST.
 *
 */


#ifndef DOORLATCHTEST_H  //  Prevents multiple compilations.
#define DOORLATCHTEST_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timekeeper.h"
#include "timermessage.h"
#include "trimamessages.h"

class DoorLatchTest
   : public StateAbs
{
   DECLARE_STATE(DoorLatchTest);
public:

   // Constructor
   DoorLatchTest();

   // Copy Constructor
   DoorLatchTest(const DoorLatchTest& state);

   // Destructor
   virtual ~DoorLatchTest();

   //
   // transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the power response message ...
   //
   void powerResponse ();

   //
   // Callbacks for the door latch timer ...
   //  Each one set the next stage to the inferred stage.  The
   //  Timeout variations set the door alarm, while the
   //  Callback functions only set the next stage.
   //
   void stage1Timeout ();
   void stage1Callback ();
   void stage2Timeout ();
   void stage2Callback ();
   void stage3Timeout ();
   void stage3Callback ();

   //
   // Create a duplicate of myself
   //
   virtual StateAbs* clone () const { return ( new DoorLatchTest(*this)); }

protected:

   //
   // Main processing function
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // One time per entry and exit functions
   //
   virtual int preEnter ();
   virtual int postExit ();

   //
   // Stage 1:  Whether the solenoid power is on or off, order solenoid
   //  power on.  When the solenoid power is on, determine whether the
   //  door is locked or unlocked.  Transition to the next function based
   //  on this locked state.
   //
   //  If door power is not seen before timeout, the alarm is set and power
   //  is requested again (restart stage 1)
   //
   void stage1 ();

   //
   // Alternate path:  When the test is started with the door locked, this
   //  path is traversed.  This function unlocks the door and then continues
   //  through the normal path.
   //
   //  If the door does not unlock by the timeout, the alarm is set and
   //  power is requested again (restart stage 1).
   //
   void alternatePath ();

   //
   // Main path:  This stage is entered with the door unlocked and solenoid
   //  power on.  Order door locked.  The test continues to the next stage
   //  when the door is seen locked.  The time to move from locked to unlocked
   //  is captured and saved.
   //
   //  If the door does not reach the locked state in time, the alarm is set
   //  and the test return to request power again (returns to stage 1).
   //
   void mainPath ();

   //
   // Stage 2:  This stage is entered with the solenoid power on, the door in
   //  the locked state.  The stage orders the power off.  When the power is off,
   //  the next stage tries to unlock the door.
   //
   //  If the power doesn't go off in the specified time, the alarm is set and
   //  the test requests power off again (returns to stage 2).
   //
   void stage2 ();

   //
   // This function will attempt to unlock the door without power.  The door is
   //  commanded unlocked.  If the door remains unlocked, the test proceeds.
   //
   //  If the door unlocks, the alarm is set and the test returns to order power
   //  on (return to stage 1).
   //
   void unlockWithoutPower ();

   //
   // Stage 3:  The last stage.  This stage is entered with the power off, and the
   //  door locked.  The power is first requested on.  When the power is seen on, the
   //  test proceeds to the unlock door state.
   //
   //  If the power doesn't come on in time, the alarm is set and the power is requested
   //  again (return to stage 3).
   //
   void stage3 ();

   //
   // This function will unlock the door.  The door is commanded unlocked.  If the
   //  door unlocks, the test finishes.
   //
   //  If the door doesn't move to the unlocked position in time, the pwer is requested
   //  on again (return to stage 3).
   //
   void unlockWithPower ();

   //
   //
   // Function to order the solenoid power on
   //
   void powerOn (const int time);  // time in millisecs

   //
   // Function to order the solenoid power off
   //
   void powerOff (const int time);  // time in millisecs

   //
   // Function to order the door to unlock
   //
   void unlockDoor (const int time);  // time in millisecs

   //
   // Function to order the door to lock
   //
   void lockDoor (const int time);  // time in millisecs

   //
   // Function to set the alarm
   //
   void setDoorAlarm ();

   //
   // Reset the state upon termination
   //
   virtual void reset ();

   //
   // Delete any dynamically allocated memory here
   //
   virtual void cleanup ();

protected:

   //
   // Safety power request response message
   //
   SafetyTestDoneMsg _PowerResponseMsg;

   //
   // Timer to check door latch movement
   //
   TimerMessage _DoorLatchTimer;

   //
   // Alarm for door latch failure
   //
   anAlarmMsg _DoorLatchAlarm;

   //
   // Current stage function for the test
   //
   CallbackBase _CurrentStage;

   //
   // Flag to signify the timer has been started
   //
   int _TimerStarted;

   //
   // Flag to signify the door latch solenoid power on state
   //
   int _PowerRequestGranted;

   //
   // Timer to accumulate the time it takes to move the door
   //  from the unlocked to locked state.
   //
   TimeKeeper _MoveTimer;

   //
   // Flag to signify the test has been ran and has passed
   //
   int _TestPassed;

};

#endif

/* FORMAT HASH fa08ed17097d96ea0a4ab850de54cdbd */
