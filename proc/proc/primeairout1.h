/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeAirout1.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef PRIME_AIROUT_H  //  Prevents multiple inclusions.
#define PRIME_AIROUT_H

#include "stateabs.h"

class PrimeAirout1
   : public StateAbs
{
   DECLARE_STATE(PrimeAirout1);
public:

   // Default constructor
   PrimeAirout1();

   // Copy constructor
   PrimeAirout1(const PrimeAirout1& state);

   // Default destructor
   virtual ~PrimeAirout1();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PrimeAirout1(*this)); }

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
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const PrimeAirout1& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

};


#endif

/* FORMAT HASH 2513875758d3cabf990ff59f99345bf4 */
