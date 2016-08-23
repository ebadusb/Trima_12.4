/*                                    -
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugetests.cpp
 *
 */


#include "an_alarm.h"
#include "centrifugetests.h"
#include "procdata.h"

DEFINE_STATE(CentrifugeTests) CentrifugeTests::CentrifugeTests()
   : StateAbs(),
     _DoorWasLocked(false)
{}

CentrifugeTests::~CentrifugeTests()
{}


int CentrifugeTests::preProcess ()
{
   return NORMAL;
}

int CentrifugeTests::postProcess ()
{
   return NORMAL;
}

int CentrifugeTests::preEnter ()
{

   ProcData pd;
   if ( pd.Status().DoorIsLocked() )
   {
      _DoorWasLocked = true;
   }

   return NORMAL;
}

int CentrifugeTests::postExit ()
{
   ProcData pd;
   if ( _DoorWasLocked && !pd.Status().DoorIsLocked() )
   {
      pd.Orders().LockDoor();
   }
   else if ( !_DoorWasLocked && pd.Status().DoorIsLocked() )
   {
      pd.Orders().UnlockDoor();
   }

   return NORMAL;
}

/* FORMAT HASH 163eb4c0af10710a03674cea11d98bc1 */
