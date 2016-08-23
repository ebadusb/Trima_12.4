/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      inletpresstest.h
 *
 */


#ifndef INLETPRESSTEST_H  //  Prevents multiple inclusions.
#define INLETPRESSTEST_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"

// Class Declaration
class InletPressTest
   : public StateAbs
{
   DECLARE_STATE(InletPressTest);
public:

   // Default constructor
   InletPressTest();

   // Copy constructor
   InletPressTest(const InletPressTest& state);

   // Default destructor
   virtual ~InletPressTest();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the pressure check timer
   //
   virtual void pressureCheck ();

   virtual StateAbs* clone () const { return ( new InletPressTest(*this)); }

protected:

   //
   // Functions to do this states processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Reset the state upon termination
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const InletPressTest& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Timer to determine when to check the pressure
   //
   TimerMessage _PressureCheckTimer;

   //
   // Pressure has decayed too much alarm
   //
   anAlarmMsg _DecayAlarm;

   //
   // The test's initial APS reading
   //
   float _InitialAPS;

   //
   // The pressure has decayed flag
   //
   int _PressureDecayed;

   //
   // Flag to signify the pressure has been checked
   //
   int _PressureChecked;

};


#endif

/* FORMAT HASH 68912830bfb64120c5a96cc6ee5ebbe0 */
