/*
 * Copyright (c) 2010 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      unlockdoor.h
 *
 */


#ifndef UNLOCKDOOR_H  //  Prevents multiple inclusions.
#define UNLOCKDOOR_H

#include "stateabs.h"


// Class Declaration
class UnlockDoor
   : public StateAbs
{
   DECLARE_STATE(UnlockDoor);
public:

   // Default constructor
   UnlockDoor();

   // Copy constructor
   UnlockDoor(const UnlockDoor& state);

   // Default destructor
   virtual ~UnlockDoor();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new UnlockDoor(*this)); }

protected:

   //
   // Functions to do this states processing ...
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
   // Command the centrifuge to stop
   //
   void commandToStop ();

   //
   // Function to reset the flags
   //
   virtual void reset ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const UnlockDoor& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Flag to signify the centrifuge is stopped
   //
   int _CentrifugeStopped;
};


#endif

/* FORMAT HASH afb5b59cac7406679230b4ac9449cdf4 */
