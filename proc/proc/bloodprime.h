/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodprime.h
 *
 * ABSTRACT:   This state controls the priming of the disposable set.
 *
 */


#ifndef BLOODPRIME_H  //  Prevents multiple inclusions.
#define BLOODPRIME_H

#include "stateabs.h"
#include "timermessage.h"
#include "runinfomsg.h"

class BloodPrime
   : public StateAbs
{
   DECLARE_STATE(BloodPrime);
public:

   // Default constructor
   BloodPrime();

   // Copy constructor
   BloodPrime(const BloodPrime& state);

   // Default destructor
   virtual ~BloodPrime();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new BloodPrime(*this)); }

   //
   // Send the prime state message
   void primeState ();

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
   // Function to command all pumps to stop
   //
   void stopPumps ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const BloodPrime& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();


protected:

   //
   // Periodic state message sender
   //
   TimerMessage _PeriodicStateMsg;



   //
   // Run information for GUI ...
   //
   CRunInfoMsg _RunInfo;


};


#endif

/* FORMAT HASH 2c06b4695e642e29cfe8349cbc3806f0 */
