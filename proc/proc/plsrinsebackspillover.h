/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackspillover.h
 *
 */


#ifndef _PLSRINSEBACKSPILLOVER_H_  //  Prevents multiple inclusions.
#define _PLSRINSEBACKSPILLOVER_H_

#include <stateabs.h>

class PlsRinsebackSpillover
   : public StateAbs
{
   DECLARE_STATE(PlsRinsebackSpillover);
public:

   // Default constructor
   PlsRinsebackSpillover();

   // Default destructor
   virtual ~PlsRinsebackSpillover();

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

/* FORMAT HASH c3a0e88c240899579f3412cccf01e561 */
