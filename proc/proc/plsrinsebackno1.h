/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackno1.h
 *
 */


#ifndef _PLSRINSEBACKNO1_H_  //  Prevents multiple inclusions.
#define _PLSRINSEBACKNO1_H_

#include <stateabs.h>

class PlsRinsebackNo1
   : public StateAbs
{
   DECLARE_STATE(PlsRinsebackNo1);
public:

   // Default constructor
   PlsRinsebackNo1();

   // Default destructor
   virtual ~PlsRinsebackNo1();

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

   //
   // Flag used to log the settings calculated by this state once
   bool _LogFlag;

   //
   // Actual AC ratio in the fluid remaining in the set ...
   //
   float _Ratio;

};


#endif

/* FORMAT HASH 3c0941685b2d595ab6bcb24e77ac0b98 */
