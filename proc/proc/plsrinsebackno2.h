/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackno2.h
 *
 */


#ifndef _PLSRINSEBACKNO2_H_  //  Prevents multiple inclusions.
#define _PLSRINSEBACKNO2_H_

#include <stateabs.h>

class PlsRinsebackNo2
   : public StateAbs
{
   DECLARE_STATE(PlsRinsebackNo2);
public:

   // Default constructor
   PlsRinsebackNo2();

   // Default destructor
   virtual ~PlsRinsebackNo2();

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
   // Actual ratio of AC in the remaining fluid
   //
   float _Ratio;
};


#endif

/* FORMAT HASH 7c212dc3558b58db22e379a23fdb97fa */
