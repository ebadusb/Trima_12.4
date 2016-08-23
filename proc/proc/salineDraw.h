/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salrinseback.h
 *
 */


#ifndef _SALDRAW_H_  //  Prevents multiple inclusions.
#define _SALDRAW_H_

#include <stateabs.h>
#include "an_alarm.h"

class SalineDraw
   : public StateAbs
{
   DECLARE_STATE(SalineDraw);
public:

   // Default constructor
   SalineDraw();

   // Default destructor
   virtual ~SalineDraw();

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

   bool       _transitionNow;
   float      _returnVolume;
   float      _totalReturnVolume;
   float      _collectVolume;

   anAlarmMsg _salineBagEmpty;

   anAlarmMsg _salineRinsebackError;

   int        _alarmCntr;
};


#endif

/* FORMAT HASH e8014d33cc97f115aeeac368234a5f97 */
