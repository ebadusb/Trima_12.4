/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimeChannel.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef PRIMECHANNEL_H  //  Prevents multiple inclusions.
#define PRIMECHANNEL_H

#include "stateabs.h"

class PrimeChannel
   : public StateAbs
{
   DECLARE_STATE(PrimeChannel);
public:

   // Default constructor
   PrimeChannel();

   // Copy constructor
   PrimeChannel(const PrimeChannel& state);

   // Default destructor
   virtual ~PrimeChannel();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PrimeChannel(*this)); }

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
   virtual void copyOver (const PrimeChannel& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   float _CustomRatio;

};


#endif

/* FORMAT HASH ad168adfd94ee6814f90286d79be307c */
