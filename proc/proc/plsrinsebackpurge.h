/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackpurge.h
 *
 */


#ifndef _PLSRINSEBACKPURGE_H_  //  Prevents multiple inclusions.
#define _PLSRINSEBACKPURGE_H_

#include <stateabs.h>

class PlsRinsebackPurge
   : public StateAbs
{
   DECLARE_STATE(PlsRinsebackPurge);
public:

   // Default constructor
   PlsRinsebackPurge();

   // Default destructor
   virtual ~PlsRinsebackPurge();

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

   bool _Finished;

};


#endif

/* FORMAT HASH a259ba3d9ea594faec1d70aa97e0bd8a */
