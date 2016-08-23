/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebacklower.h
 *
 * ABSTRACT:   Lower the reservoir during rinseback to prevent blood from entering the
 *             vent bag.
 *
 */


#ifndef _PLSRINSEBACKLOWER_H_  //  Prevents multiple inclusions.
#define _PLSRINSEBACKLOWER_H_

#include <stateabs.h>

class PlsRinsebackLower
   : public StateAbs
{
   DECLARE_STATE(PlsRinsebackLower);
public:

   // Default constructor
   PlsRinsebackLower();

   // Default destructor
   virtual ~PlsRinsebackLower();

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

};


#endif

/* FORMAT HASH 8b0ea90d7b7d2ff2917bc3624816b4e6 */
