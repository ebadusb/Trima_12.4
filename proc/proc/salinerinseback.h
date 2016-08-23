/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salinerinseback.h
 *
 * ABSTRACT:   This state controls the trima actions during the saline rinseback
 *             phases of the run.
 *
 */


#ifndef _SALINE_RINSEBACK_H_  //  Prevents multiple inclusions.
#define _SALINE_RINSEBACK_H_

#include "stateabs.h"
#include "timekeeper.h"
#include "timermessage.h"

class SalineRinseback
   : public StateAbs
{
   DECLARE_STATE(SalineRinseback);
public:

   // Default constructor
   SalineRinseback();

   // Default destructor
   virtual ~SalineRinseback();

   //
   // Function to send of the state message
   //
   void rinsebackState ();



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

   TimerMessage _PeriodicStateMsg;
   float        _totalVolumeToDonor;

   float        _QreturnMax;
   float        _Qr;
   float        _IRMax;
   float        _VacRes;
   float        _VRes;

};


#endif

/* FORMAT HASH d6084797d10bfbe98c6f96668640e8d6 */
