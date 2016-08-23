/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfpowertest.cpp
 *
 */

#include <pfpowertest.h>
#include "an_alarm.h"

DEFINE_STATE(PFPowerTest);

PFPowerTest::PFPowerTest()
   : StateAbs(),
     _PendingAlarmMsg()
{}

PFPowerTest::~PFPowerTest()
{
   PFPowerTest::cleanup();
}

int PFPowerTest::preProcess ()
{
   return NORMAL;
}

int PFPowerTest::postProcess ()
{
   return NORMAL;
}

int PFPowerTest::preEnter ()
{

   //
   // Set up for the delayed alarm message ...
   //
   _PendingAlarmMsg.init(Callback<PFPowerTest>(this, &PFPowerTest::pendingAlarm) );

   return NORMAL;
}

int PFPowerTest::postExit ()
{

   //
   // Stop listening for the pending alarm message ...
   //
   _PendingAlarmMsg.deregisterMsg();

   return NORMAL;
}

void PFPowerTest::pendingAlarm ()
{
   anAlarmMsg alrm( (ALARM_VALUES)_PendingAlarmMsg.getData() );
   alrm.setAlarm();
}

/* FORMAT HASH d5ea1a1bc8ecb7df9ac0c3fad7101c5d */
