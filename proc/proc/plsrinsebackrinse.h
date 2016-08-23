/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackrinse.h
 *
 */


#ifndef _PLSRINSEBACKRINSE_H_  //  Prevents multiple inclusions.
#define _PLSRINSEBACKRINSE_H_

#include <stateabs.h>

class PlsRinsebackRinse
   : public StateAbs
{
   DECLARE_STATE(PlsRinsebackRinse);
public:

   // Default constructor
   PlsRinsebackRinse();

   // Default destructor
   virtual ~PlsRinsebackRinse();

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

/* FORMAT HASH 2392853a01a92170dd5636fa4b650000 */
