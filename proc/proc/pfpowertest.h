/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfpowertest.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *             power fail specific power tests state.
 *
 */


#ifndef PFPOWERTEST_H
#define PFPOWERTEST_H

#include <stateabs.h>
#include "trimamessages.h"

class PFPowerTest
   : public StateAbs
{
   DECLARE_STATE(PFPowerTest);
public:

   PFPowerTest();

   virtual ~PFPowerTest();

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
   // Callback for the pending alarm message ...
   //
   void pendingAlarm ();

protected:

   //
   // Message to set a delayed reaction alarm ...
   //
   PendingCassetteAlarmMsg _PendingAlarmMsg;

};

#endif // ifndef PFPOWERTEST_H

/* FORMAT HASH 8cc5ff246f16d8a18eea0faf95053024 */
