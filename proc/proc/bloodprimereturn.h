/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodprimereturn.h
 *
 * ABSTRACT:   This state controls the priming of the disposable set.
 *
 */


#ifndef BLOODPRIMERETURN_H  //  Prevents multiple inclusions.
#define BLOODPRIMERETURN_H

#include "stateabs.h"
#include "an_alarm.h"

class BloodPrimeReturn
   : public StateAbs
{
   DECLARE_STATE(BloodPrimeReturn);
public:

   // Default constructor
   BloodPrimeReturn();

   // Copy constructor
   BloodPrimeReturn(const BloodPrimeReturn& state);

   // Default destructor
   virtual ~BloodPrimeReturn();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new BloodPrimeReturn(*this)); }

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
   virtual void copyOver (const BloodPrimeReturn& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Alarms
   //
   anAlarmMsg _TooSoonAlarm;
   anAlarmMsg _TooLateAlarm;

   //
   // Return volume accumulated after the low level sensor
   //  sees fluid
   //
   float _VolumeAfterSensor;

};


#endif

/* FORMAT HASH cd68a2223d487dcec38f3fa20465bd3b */
