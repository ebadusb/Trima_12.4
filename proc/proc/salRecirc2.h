/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      SalRecirc2.h
 *
 */


#ifndef _SALRECIRC2_H_  //  Prevents multiple inclusions.
#define _SALRECIRC2_H_

#include <stateabs.h>

class SalRecirc2
   : public StateAbs
{
   DECLARE_STATE(SalRecirc2);
public:

   // Default constructor
   SalRecirc2();

   // Default destructor
   virtual ~SalRecirc2();

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
   bool  _transitionNow;
   float _inletVolume;



};


#endif

/* FORMAT HASH e27124dc06efb8d696618ea456d7d938 */
