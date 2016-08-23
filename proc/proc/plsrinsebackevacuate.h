/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackevacuate.h
 *
 */


#ifndef _PLSRINSEBACKEVACUATE_H_  //  Prevents multiple inclusions.
#define _PLSRINSEBACKEVACUATE_H_

#include <stateabs.h>

class PlsRinsebackEvacuate
   : public StateAbs
{
   DECLARE_STATE(PlsRinsebackEvacuate);
public:

   // Default constructor
   PlsRinsebackEvacuate();

   // Default destructor
   virtual ~PlsRinsebackEvacuate();

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

   bool  _Evacuating;
   float _EvacuationLimit;
   float _EvacuationMinVolume;

   //
   // Flag used to log the settings calculated by this state once
   bool _LogFlag;

   //
   // AC ratio of the remaining fluid in the set
   //
   float _Ratio;
};


#endif

/* FORMAT HASH f3c0fb17b4bc8573ffc59f79da46e9d4 */
