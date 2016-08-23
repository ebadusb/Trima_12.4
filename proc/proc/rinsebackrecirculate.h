/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      RinsebackRecirculate.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef RINSEBACKRECIRCULATE_H  //  Prevents multiple inclusions.
#define RINSEBACKRECIRCULATE_H

#include "stateabs.h"

class RinsebackRecirculate
   : public StateAbs
{
   DECLARE_STATE(RinsebackRecirculate);
public:

   // Default constructor
   RinsebackRecirculate();

   // Copy constructor
   RinsebackRecirculate(const RinsebackRecirculate& state);

   // Default destructor
   virtual ~RinsebackRecirculate();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new RinsebackRecirculate(*this)); }

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
   virtual void copyOver (const RinsebackRecirculate& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Declare data members here ...
   //
   float _VpAtStart;
};


#endif

/* FORMAT HASH 86b205779fc0d7b76aff132c12367a07 */
