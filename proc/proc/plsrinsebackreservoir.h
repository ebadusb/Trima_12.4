/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackreservoir.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef _PLSRINSEBACKRESERVOIR_H_  //  Prevents multiple inclusions.
#define _PLSRINSEBACKRESERVOIR_H_

#include <stateabs.h>

class PlsRinsebackReservoir
   : public StateAbs
{
   DECLARE_STATE(PlsRinsebackReservoir);
public:

   // Default constructor
   PlsRinsebackReservoir();

   // Default destructor
   virtual ~PlsRinsebackReservoir();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

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

protected:

};


#endif

/* FORMAT HASH fcdbc2099382b00e707e80913df400d9 */
