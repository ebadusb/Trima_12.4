/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PreRinseback.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef PRERINSEBACK_H  //  Prevents multiple inclusions.
#define PRERINSEBACK_H

#include "stateabs.h"
#include "timermessage.h"

class PreRinseback
   : public StateAbs
{
   DECLARE_STATE(PreRinseback);
public:

   // Default constructor
   PreRinseback();

   // Copy constructor
   PreRinseback(const PreRinseback& state);

   // Default destructor
   virtual ~PreRinseback();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PreRinseback(*this)); }

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
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const PreRinseback& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   virtual void timeout ();

protected:
   TimerMessage _Timer;
   int          _TimerExpired;


};


#endif

/* FORMAT HASH 24126d2d3d571c20689821802bd74b9b */
