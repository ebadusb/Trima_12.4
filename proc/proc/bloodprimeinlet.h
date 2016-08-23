/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodprimeinlet.h
 *
 * ABSTRACT:   This state controls the priming of the disposable set.
 *
 */


#ifndef BLOODPRIMEINLET_H  //  Prevents multiple inclusions.
#define BLOODPRIMEINLET_H

#include "stateabs.h"
#include "an_alarm.h"

class BloodPrimeInlet
   : public StateAbs
{
   DECLARE_STATE(BloodPrimeInlet);
public:

   // Default constructor
   BloodPrimeInlet();

   // Copy constructor
   BloodPrimeInlet(const BloodPrimeInlet& state);

   // Default destructor
   virtual ~BloodPrimeInlet();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new BloodPrimeInlet(*this)); }

protected:

   //
   // Functions to do this state's processing ...
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
   // Function to stop all pumps
   //
   void stopPumps ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const BloodPrimeInlet& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   float      _CustomRatio;
   anAlarmMsg _DoorOpenAlarm;

};


#endif

/* FORMAT HASH c42e02dd1dd8af891ed247d3440c10c9 */
