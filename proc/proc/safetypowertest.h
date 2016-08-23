/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      safepowertest.h
 *
 * ABSTRACT:   This header file defines the door lock test.
 *
 */


#ifndef SAFETYPOWERTEST_H  //  Prevents multiple compilations.
#define SAFETYPOWERTEST_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"
#include "trimamessages.h"


class SafetyPowerTest
   : public StateAbs
{
   DECLARE_STATE(SafetyPowerTest);
public:

   // Constructor
   SafetyPowerTest();

   // Copy constructor
   SafetyPowerTest(const SafetyPowerTest& state);

   // Destructor
   virtual ~SafetyPowerTest();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback function when the safety test completes ...
   //
   virtual void safetyTestDone ();

   //
   // Callback function when safety fail to complete in time ...
   //
   virtual void safetyTooSlow ();

   //
   // Function used to setup the conditions for the test
   //
   void startTest ();

   //
   // Make a copy of myself ...
   //
   virtual StateAbs* clone () const { return ( new SafetyPowerTest(*this)); }


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
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const SafetyPowerTest& s);

   //
   // Delete any dynamically allocated memory here
   //
   virtual void cleanup ();

protected:

   //
   // Flag used to signal the test has started
   //
   bool _TestStarted;

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
   // Alarms ...
   //
   anAlarmMsg _PowerTestFailAlarm;

};


#endif

/* FORMAT HASH e79f7908a860addc1e5f119d916ad3ed */
