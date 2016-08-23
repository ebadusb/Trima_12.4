/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salineFlush.h
 *
 */


#ifndef _SALINEFLUSH_H_  //  Prevents multiple inclusions.
#define _SALINEFLUSH_H_

#include <stateabs.h>

#include "an_alarm.h"

class SalFlush
   : public StateAbs
{
   DECLARE_STATE(SalFlush);
public:

   // Default constructor
   SalFlush();

   // Default destructor
   virtual ~SalFlush();

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
   float      _collectVolume;
   float      _CollectVolumeForInitPause;
   float      _returnVolume;
   float      _totalReturnVolume;

   anAlarmMsg _salineBagEmpty;

   anAlarmMsg _salineRinsebackError;

   int        _alarmCntr;

};


#endif

/* FORMAT HASH 13cc3395063ad98cac182d8ffe37250a */
