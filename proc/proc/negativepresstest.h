/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      negativepresstest.h
 *
 */


#ifndef NEGATIVEPRESSTEST_H  //  Prevents multiple inclusions.
#define NEGATIVEPRESSTEST_H

#include <an_alarm.h>
#include <stateabs.h>


// Class Declaration
class NegativePressTest
   : public StateAbs
{
   DECLARE_STATE(NegativePressTest);
public:

   // Default constructor
   NegativePressTest();

   // Copy constructor
   NegativePressTest(const NegativePressTest& state);

   // Default destructor
   virtual ~NegativePressTest();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new NegativePressTest(*this)); }

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
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const NegativePressTest& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Negative Pressure Alarm
   //
   anAlarmMsg _NegativePressureAlarm;

   //
   // Delta Volume pumped through the inlet or return line
   //
   float _testVolume;
   bool  _airEvacMitigation;
   bool  _transition;

};


#endif

/* FORMAT HASH 811365c3077b34723a599d6b2816dc46 */
