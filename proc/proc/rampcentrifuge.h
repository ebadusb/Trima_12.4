/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      RampCentrifuge.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef RAMPCENTRIFUGE_H  //  Prevents multiple inclusions.
#define RAMPCENTRIFUGE_H

#include "stateabs.h"

class RampCentrifuge
   : public StateAbs
{
   DECLARE_STATE(RampCentrifuge);
public:

   // Default constructor
   RampCentrifuge();

   // Copy constructor
   RampCentrifuge(const RampCentrifuge& state);

   // Default destructor
   virtual ~RampCentrifuge();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new RampCentrifuge(*this)); }

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
   virtual void copyOver (const RampCentrifuge& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

};


#endif

/* FORMAT HASH ceff0522f8c4e28442313aee4a54b2eb */
