/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfinit.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  power fail recovery initialization state.
 *
 */


#ifndef PFINIT_H  //  Prevents multiple compilations.
#define PFINIT_H

#include "stateabs.h"

class PFInit
   : public StateAbs
{
   DECLARE_STATE(PFInit);
public:

   // Default Constructor
   PFInit();

   // Copy Constructor
   PFInit(const PFInit& state);

   // Destructor
   virtual ~PFInit();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PFInit(*this)); }

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
   // Delete any dynamic memory here
   //
   virtual void cleanup ();

   bool _alreadySentCobeConfigValues;

};

#endif // ifndef PFINIT_H

/* FORMAT HASH 5ca31b5e5fb9531e5a711006eab8c272 */
