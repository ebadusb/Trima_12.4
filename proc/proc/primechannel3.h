/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeChannel3.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef PRIMECHANNEL3_H  //  Prevents multiple inclusions.
#define PRIMECHANNEL3_H

#include "stateabs.h"

class PrimeChannel3
   : public StateAbs
{
   DECLARE_STATE(PrimeChannel3);
public:

   // Default constructor
   PrimeChannel3();

   // Copy constructor
   PrimeChannel3(const PrimeChannel3& state);

   // Default destructor
   virtual ~PrimeChannel3();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PrimeChannel3(*this)); }

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
   virtual void copyOver (const PrimeChannel3& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

};


#endif

/* FORMAT HASH f7da30a73acde0e117d25b8ba91c1375 */
