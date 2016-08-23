/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      lowernotification.cpp
 *
 */

#include "lowernotification.h"
#include "guiproc.h"
#include "procdata.h"
#include "trimamessages.h"

DEFINE_STATE(LowerNotification);

LowerNotification::LowerNotification()
   : StateAbs(),
     _NotificationSent(0)
{}

LowerNotification::LowerNotification(const LowerNotification& state)
   : StateAbs(state),
     _NotificationSent(state._NotificationSent)
{
   copyOver(state);
}

LowerNotification::~LowerNotification()
{
   cleanup();
}

int LowerNotification::transitionStatus ()
{
   if ( _NotificationSent )
   {
      return 1;
   }
   return NO_TRANSITION;
}

int LowerNotification::preProcess ()
{
   ProcData pd;
   if ( pd.Status().CassIsDown() )
   {
      //
      // Send the gui handshake message ...
      //
      DisposableLoweredMsg      guiMsg(MessageBase::SEND_LOCAL);
      guiMsg.send(1);
      CassettePositionStatusMsg posmsg(MessageBase::SEND_LOCAL);
      posmsg.send(CASSETTE_LOWERED);
      DataLog(log_level_proc_load_cassette_info) << "GUI Message sent : Disposable Lowered" << endmsg;

      _NotificationSent = 1;
   }

   return NORMAL;
}

int LowerNotification::postProcess ()
{
   return NORMAL;
}

int LowerNotification::preEnter ()
{
   return NORMAL;
}

int LowerNotification::postExit ()
{
   //
   // Reset the flag ...
   //
   _NotificationSent = 0;

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void LowerNotification::copyOver (const LowerNotification&)
{}

void LowerNotification::cleanup ()
{}

/* FORMAT HASH 78b4990a639145e1c3a7daddff8a7a4d */
