/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salCheckConnect.h
 *
 */


#ifndef _SALCHECKCONNECT_H_  //  Prevents multiple inclusions.
#define _SALCHECKCONNECT_H_

#include <stateabs.h>
#include "an_alarm.h"

class SalCheckConnect
   : public StateAbs
{
   DECLARE_STATE(SalCheckConnect);
public:

   // Default constructor
   SalCheckConnect();

   // Default destructor
   virtual ~SalCheckConnect();

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
   anAlarmMsg _salineBagEmpty;


};


#endif

/* FORMAT HASH 33efe907a2dd9fef0a2ad0bacea96964 */
