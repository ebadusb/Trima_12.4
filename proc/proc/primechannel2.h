/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeChannel2.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef PRIMECHANNEL2_H  //  Prevents multiple inclusions.
#define PRIMECHANNEL2_H

#include "stateabs.h"

class PrimeChannel2
   : public StateAbs
{
   DECLARE_STATE(PrimeChannel2);
public:

   // Default constructor
   PrimeChannel2();

   // Copy constructor
   PrimeChannel2(const PrimeChannel2& state);

   // Default destructor
   virtual ~PrimeChannel2();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PrimeChannel2(*this)); }

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
   virtual void copyOver (const PrimeChannel2& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

};


#endif

/* FORMAT HASH fad99a6d65c1d68d9f85e11f20510f40 */
