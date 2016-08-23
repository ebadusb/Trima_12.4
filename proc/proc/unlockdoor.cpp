/*
 * Copyright (c) 2010 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      unlockdoor.cpp
 *
 */

#include "unlockdoor.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(UnlockDoor);

UnlockDoor::UnlockDoor()
   : StateAbs(),
     _CentrifugeStopped(0)
{
   maintainHistory(0);
}

UnlockDoor::UnlockDoor(const UnlockDoor& state)
   : StateAbs(state),
     _CentrifugeStopped(0)
{
   UnlockDoor::copyOver(state);
}

UnlockDoor::~UnlockDoor()
{
   UnlockDoor::cleanup();
}

int UnlockDoor::transitionStatus ()
{
   //
   // Check and see if the centrifuge is stopped
   //
   if ( _CentrifugeStopped )
   {
      return 1;
   }
   return NO_TRANSITION;
}

int UnlockDoor::preProcess ()
{
   //
   // Recommand the centrifuge to stop ...
   //

   commandToStop();


   return NORMAL;
}

int UnlockDoor::postProcess ()
{
   ProcData pd;
   //
   // Check and see if the centrifuge is slow enough to open the door
   // and the power is safe
   //
   if ( !(pd.Status().CentrifugeRPM() > 1.0f)  )
   {
      _CentrifugeStopped = 1;
      if (pd.Status().DoorIsLocked() )
      {
         pd.Orders().UnlockDoor();
         DataLog(log_level_proc_info) << "The centrifuge has stopped and the door is unlocked" << endmsg;
      }

   }
   return NORMAL;
}

int UnlockDoor::preEnter ()
{
   return NORMAL;
}

int UnlockDoor::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void UnlockDoor::commandToStop ()
{
   ProcData pd;
   //
   // Command the centrifuge to stop
   //
   pd.Orders().RPM(0.0f);

   DataLog(log_level_proc_info) << "Command centrifuge to stop " << endmsg;
}

void UnlockDoor::reset ()
{
   //
   // Reset the flags ...
   //
   _CentrifugeStopped = 0;
}

void UnlockDoor::copyOver (const UnlockDoor&)
{}

void UnlockDoor::cleanup ()
{}

/* FORMAT HASH 7e9a2b2fbd36ecb8b93bf54eb745e6eb */
