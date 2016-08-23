/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinseback.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef _PLSRINSEBACK_H_ //  Prevents multiple inclusions.
#define _PLSRINSEBACK_H_

#include <stateabs.h>

class PlsRinseback
   : public StateAbs
{
   DECLARE_STATE(PlsRinseback);
public:

   // Default constructor
   PlsRinseback();

   // Default destructor
   virtual ~PlsRinseback();

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
   // Plasma volume delta used as a trigger for moving
   //  the RBC valve.
   float _ValveVolume;

   //
   // Flag used to log the settings calculated by this state once
   bool _LogFlag;

   //
   // Actual ratio of the fluid remaining in the set
   //
   float _Ratio;
};


#endif

/* FORMAT HASH 25903306970d113fc17e04c6ab298e3e */
