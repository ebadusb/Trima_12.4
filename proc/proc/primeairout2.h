/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeAirout2.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef PRIME_AIROUT2_H  //  Prevents multiple inclusions.
#define PRIME_AIROUT2_H

#include "stateabs.h"

class PrimeAirout2
   : public StateAbs
{
   DECLARE_STATE(PrimeAirout2);
public:

   // Default constructor
   PrimeAirout2();

   // Copy constructor
   PrimeAirout2(const PrimeAirout2& state);

   // Default destructor
   virtual ~PrimeAirout2();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PrimeAirout2(*this)); }

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
   virtual void copyOver (const PrimeAirout2& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Declare data members here ...
   //
   float _VinAtStart;

};


#endif

/* FORMAT HASH af9d470254686c67e879fc0817982948 */
