/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salEvac2.h
 *
 */


#ifndef _SALEVAC2_H_  //  Prevents multiple inclusions.
#define _SALEVAC2_H_

#include <stateabs.h>

class SalEvac2
   : public StateAbs
{
   DECLARE_STATE(SalEvac2);
public:

   // Default constructor
   SalEvac2();

   // Default destructor
   virtual ~SalEvac2();

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
   float _plasmaVolume;
   float _returnVolume;

   float _QreturnMax;
   float _IRMax;
   float _VacRes;
   float _VRes;
   float _totalPlasmaVolume;


   bool  _justEmptyNow;

   bool  _primed;
   float _qp_prime_volm;
};


#endif

/* FORMAT HASH b984a25c171c4fb233f3b0e4edac4856 */
