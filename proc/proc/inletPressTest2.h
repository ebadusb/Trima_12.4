/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:       inletPressTest2
 *
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/inletPressTest2.h 1.1 2007/02/20 22:10:55Z jheiusb Exp jheiusb $
$Log: inletPressTest2.h $
*/

#ifndef INLET_PRESSURE_TEST_2_H  //  Prevents multiple inclusions.
#define INLET_PRESSURE_TEST_2_H

#include "an_alarm.h"
#include "stateabs.h"
#include "timermessage.h"


// Class Declaration
class InletPressTest2
   : public StateAbs
{
   DECLARE_STATE(InletPressTest2);
public:

   // Default constructor
   InletPressTest2();

   // Copy constructor
   InletPressTest2(const InletPressTest2& state);

   // Default destructor
   virtual ~InletPressTest2();


   virtual void pressureCheck ();

   virtual StateAbs* clone () const { return ( new InletPressTest2(*this)); }

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
   virtual void copyOver (const InletPressTest2& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   virtual void reset ();

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

/* FORMAT HASH 0b60341f4f4f044fad87ce6f69987926 */
