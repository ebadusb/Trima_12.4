/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      inletdecaytest.h
 *
 */


#ifndef INLETDECAYTEST_H  //  Prevents multiple inclusions.
#define INLETDECAYTEST_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class InletDecayTest
   : public StateAbs
{
   DECLARE_STATE(InletDecayTest);
public:

   // Default constructor
   InletDecayTest();

   // Copy constructor
   InletDecayTest(const InletDecayTest& state);

   // Default destructor
   virtual ~InletDecayTest();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback for the pressure timer
   //
   virtual void pressureCheck ();

   //
   // Callback for the pump timer
   //
   virtual void pumpProblem ();

   virtual StateAbs* clone () const { return ( new InletDecayTest(*this)); }

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
   // Function to set up all necessary commands and readings
   //  to start the test
   //
   void setUpTest ();
   //
   // Function to stop all pumps
   //
   void stopPumps ();

   //
   // Function to save current values
   //
   void saveValues ();

   //
   // Reset this state when it exits
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const InletDecayTest& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Timer to test for pressure decay
   //
   TimerMessage _PressureTimer;

   //
   // Timer to catch a moving or non-moving pump
   //
   TimerMessage _PumpTimer;

   //
   // Pressure decay alarm
   //
   anAlarmMsg _DecayAlarm;

   //
   // Initial APS reading
   //
   float _InitialAPS;

   //
   // Delta volume accumulations
   //
   float _InletVolume;
   float _ACVolume;
   float _ReturnVolume;

   //
   // Flag to signify the pumps have moved and
   //  we are currently testing for pressure decay
   //
   bool _TestStarted;

   //
   // Flag to keep whether the test passed or not
   //
   bool _TestPassed;

   //
   // Flag to signify the test failed
   //
   bool _TestFailed;

   //
   // Counter to keep track of how many times the test
   //  gets ran
   //
   int _TestPassedCount;
};


#endif

/* FORMAT HASH 17491f0ebb4bb8a921729b32f557049e */
