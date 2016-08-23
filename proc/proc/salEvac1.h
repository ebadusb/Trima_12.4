/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salEvac1.h
 *
 */


#ifndef _SALEVAC1_H_  //  Prevents multiple inclusions.
#define _SALEVAC1_H_

#include <stateabs.h>

class SalEvac1
   : public StateAbs
{
   DECLARE_STATE(SalEvac1);
public:

   // Default constructor
   SalEvac1();

   // Default destructor
   virtual ~SalEvac1();

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

   // checks the cetrifuge RPMs
   bool centrifugeStopped ();


protected:
   bool  _transitionNow;
   float _plasmaVolume;
   float _returnVolume;

   float _returnVolumeForULSCutout;

   float _QreturnMax;
   float _Qr;
   float _IRMax;
   float _VacRes;
   float _VRes;

   float _totalPlasmaVolume;

   bool  _ulsCutout;
   bool  _justEmptyNow;
   bool  _primed;
   float _qp_prime_volm;

};


#endif

/* FORMAT HASH 8bbc72c50b62e18965bf50f99c3851f1 */
