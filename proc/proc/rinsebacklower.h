/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rinsebacklower.h
 *
 * ABSTRACT:   Lower the reservoir during rinseback to prevent blood from entering the
 *             vent bag.
 *
 */


#ifndef RINSEBACKLOWER_H  //  Prevents multiple inclusions.
#define RINSEBACKLOWER_H

#include "stateabs.h"

class RinsebackLower
   : public StateAbs
{
   DECLARE_STATE(RinsebackLower);
public:

   // Default constructor
   RinsebackLower();

   // Copy constructor
   RinsebackLower(const RinsebackLower& state);

   // Default destructor
   virtual ~RinsebackLower();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new RinsebackLower(*this)); }

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
   virtual void copyOver (const RinsebackLower& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   float _QreturnMax;
   float _IRMax;
   float _VacRes;
   float _VRes;
   float _VResNet;  // the accumulated volume of the res

};


#endif

/* FORMAT HASH 7fc161c4cae7c0c60a3a974636a2cfd7 */
