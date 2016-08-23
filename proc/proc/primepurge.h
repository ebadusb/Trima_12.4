/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PrimePurge.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef PRIMEPURGE_H  //  Prevents multiple inclusions.
#define PRIMEPURGE_H

#include <stateabs.h>

class PrimePurge
   : public StateAbs
{
   DECLARE_STATE(PrimePurge);
public:

   // Default constructor
   PrimePurge();

   // Copy constructor
   PrimePurge(const PrimePurge& state);

   // Default destructor
   virtual ~PrimePurge();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PrimePurge(*this)); }

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
   virtual void copyOver (const PrimePurge& s);

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

/* FORMAT HASH be7c8b9ebc02e5622630c88d1df17a8f */
