/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackrecirculate.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef _PLSRINSEBACKRECIRCULATE_H_  //  Prevents multiple inclusions.
#define _PLSRINSEBACKRECIRCULATE_H_

#include <stateabs.h>

class PlsRinsebackRecirculate
   : public StateAbs
{
   DECLARE_STATE(PlsRinsebackRecirculate);
public:

   // Default constructor
   PlsRinsebackRecirculate();

   // Default destructor
   virtual ~PlsRinsebackRecirculate();

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

/* FORMAT HASH 6155b4e47129aa8e9b68230b229bd991 */
