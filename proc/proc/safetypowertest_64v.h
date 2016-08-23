/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      safepowertest.h
 *
 * ABSTRACT:   This header file defines the door lock test.
 *
 */


#ifndef SAFETYPOWERTEST64_H  //  Prevents multiple compilations.
#define SAFETYPOWERTEST64_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"
#include "trimamessages.h"


class SafetyPowerTest64
   : public StateAbs
{
   DECLARE_STATE(SafetyPowerTest64);
public:

   // Constructor
   SafetyPowerTest64();

   // Destructor
   virtual ~SafetyPowerTest64();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback function when the safety test completes ...
   //
   void safetyTestDone ();

   //
   // Callback function when safety fail to complete in time ...
   //
   void safetyTooSlow ();

   //
   // Callback function when door lock did not to complete in time ...
   //
   void doorDidNotLock ();

   //
   // Function to start the test by sending messages to safety ...
   //
   void startPowerTest ();

protected:

   //
   // Main processing function
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // One time per entrance and exit functions ...
   //
   virtual int preEnter ();
   virtual int postExit ();

   //
   // Reset the state at termination
   //
   virtual void reset ();

   //
   // Delete any dynamically allocated memory here
   //
   virtual void cleanup ();

protected:

   //
   // Flag used to signal the test has not been started
   //
   bool _TestNotStarted;

   //
   // Flag used to signal the test has completed
   //
   bool _TestComplete;

   //
   // Flag used to signal the test failed to complete on time
   //
   bool _TestFailed;

   //
   // Message to request safety test power ...
   //
   RequestSafetyPowerMsg _RequestSafetyTestMsg;

   //
   // Message that informs me that safety has completed
   //  its power test ...
   //
   SafetyTestDoneMsg _SafetyTestDoneMsg;

   //
   // Timer that will produce an error if safety doesn't
   //  complete in the allotted time ...
   //
   TimerMessage _SafetyTooSlowTimer;

   //
   // Timer that will produce an error if the door doesn't
   //  lock in the allotted time ...
   //
   TimerMessage _DoorDidNotLockTimer;

};


#endif

/* FORMAT HASH fbd313ee9d08c52b12423f632c347a6e */
