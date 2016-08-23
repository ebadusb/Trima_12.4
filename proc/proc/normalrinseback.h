/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      normalrinseback.h
 *
 * ABSTRACT:   This state controls the trima actions during the blood rinseback
 *             phases of the run.
 *
 */


#ifndef _NORMAL_RINSEBACK_H_  //  Prevents multiple inclusions.
#define _NORMAL_RINSEBACK_H_

#include "stateabs.h"
#include "timermessage.h"

class NormalRinseback
   : public StateAbs
{
   DECLARE_STATE(NormalRinseback);
public:

   // Default constructor
   NormalRinseback();

   // Copy constructor
   NormalRinseback(const NormalRinseback& state);

   // Default destructor
   virtual ~NormalRinseback();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new NormalRinseback(*this)); }

   //
   // Function to send of the state message
   //
   void rinsebackState ();

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

   //
   // Reset the state upon termination
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const NormalRinseback& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Declare data members here ...
   //
   TimerMessage _PeriodicStateMsg;
};


#endif

/* FORMAT HASH cb6ea2178fe81c2b8181eaa280348d3c */
