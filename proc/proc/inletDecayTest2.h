/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:       InletDecayTest2
 *
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/negPressTest2.h 1.1 2007/02/20 22:10:55Z jheiusb Exp jheiusb $
$Log: negPressTest2.h $
*/

#ifndef INLET_DECAY_TEST_2_H  //  Prevents multiple inclusions.
#define INLET_DECAY_TEST_2_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class InletDecayTest2
   : public StateAbs
{
   DECLARE_STATE(InletDecayTest2);
public:

   // Default constructor
   InletDecayTest2();

   // Copy constructor
   InletDecayTest2(const InletDecayTest2& state);

   // Default destructor
   virtual ~InletDecayTest2();

   virtual StateAbs* clone () const { return ( new InletDecayTest2(*this)); }

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


   virtual int transitionStatus ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const InletDecayTest2& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   //
   // Callback for the pressure timer
   //
   virtual void pressureCheck ();

   //
   // Callback for the pump timer
   //
   virtual void pumpProblem ();

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

private:
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
   int _TestStarted;

   //
   // Flag to keep whether the test passed or not
   //
   int _TestPassed;

   //
   // Flag to signify the test failed
   //
   int _TestFailed;

   //
   // Counter to keep track of how many times the test
   //  gets ran
   //
   int _TestPassedCount;


};





#endif

/* FORMAT HASH 428868c5824dea91407eae77fc616fe9 */
