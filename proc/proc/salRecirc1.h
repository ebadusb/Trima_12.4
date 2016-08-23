/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salrinseback.h
 *
 */


#ifndef _SALRECIRC1_H_  //  Prevents multiple inclusions.
#define _SALRECIRC1_H_

#include <stateabs.h>

class SalRecirc1
   : public StateAbs
{
   DECLARE_STATE(SalRecirc1);
public:

   // Default constructor
   SalRecirc1();

   // Default destructor
   virtual ~SalRecirc1();

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

   bool  _ulsSpeedUp;
   float _returnVolume;

};


#endif

/* FORMAT HASH d0fdfa412fbfe10e68315bb0c322bc49 */
