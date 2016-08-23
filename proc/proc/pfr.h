/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfr.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  Power Fail Recovery state.
 *
 */


#ifndef PFR_H  //  Prevents multiple compilations.
#define PFR_H

#include <stateabs.h>

class PFR
   : public StateAbs
{
   DECLARE_STATE(PFR);
public:

   // Default Constructor
   PFR();

   // Copy Constructor
   PFR(const PFR& state);

   // Destructor
   virtual ~PFR();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new PFR(*this)); }

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

protected:
   //
   // State specific data members
   //
};

#endif

/* FORMAT HASH 32f784f3ec4c4c50cd689694c7ce5935 */
