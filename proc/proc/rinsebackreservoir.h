/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      RinsebackReservoir.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef RINSEBACKRESERVOIR_H  //  Prevents multiple inclusions.
#define RINSEBACKRESERVOIR_H

#include "stateabs.h"

class RinsebackReservoir
   : public StateAbs
{
   DECLARE_STATE(RinsebackReservoir);
public:

   // Default constructor
   RinsebackReservoir();

   // Copy constructor
   RinsebackReservoir(const RinsebackReservoir& state);

   // Default destructor
   virtual ~RinsebackReservoir();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new RinsebackReservoir(*this)); }

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
   virtual void copyOver (const RinsebackReservoir& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

};


#endif

/* FORMAT HASH b564f87f38f475383e02c1a88a288632 */
