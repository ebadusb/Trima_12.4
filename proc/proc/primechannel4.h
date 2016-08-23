/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeChannel4.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef PRIMECHANNEL4_H  //  Prevents multiple inclusions.
#define PRIMECHANNEL4_H

#include "stateabs.h"

class PrimeChannel4
   : public StateAbs
{
   DECLARE_STATE(PrimeChannel4);
public:

   // Default constructor
   PrimeChannel4();

   // Copy constructor
   PrimeChannel4(const PrimeChannel4& state);

   // Default destructor
   virtual ~PrimeChannel4();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PrimeChannel4(*this)); }

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
   virtual void copyOver (const PrimeChannel4& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

};


#endif

/* FORMAT HASH 1ec2e7325803b9b62703d3bab16016ef */
