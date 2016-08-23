/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeChannel2A.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef PRIMECHANNEL2A_H  //  Prevents multiple inclusions.
#define PRIMECHANNEL2A_H

#include "stateabs.h"

class PrimeChannel2A
   : public StateAbs
{
   DECLARE_STATE(PrimeChannel2A);
public:

   // Default constructor
   PrimeChannel2A();

   // Copy constructor
   PrimeChannel2A(const PrimeChannel2A& state);

   // Default destructor
   virtual ~PrimeChannel2A();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PrimeChannel2A(*this)); }

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
   virtual void copyOver (const PrimeChannel2A& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

};


#endif

/* FORMAT HASH cf377e9b97ae0682f15dc5b8d101482d */
