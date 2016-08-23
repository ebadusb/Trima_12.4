/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      guistarted.cpp
 *
 */

#include "guistarted.h"
#include "states.h"
#include "trima_datalog.h"

DEFINE_STATE(GUIStarted);

GUIStarted::GUIStarted()
   : StateAbs(),
     _GUIHandshakeTimer(0, Callback<GUIStarted>(this, &GUIStarted::sendGUIHandshake), TimerMessage::DISARMED),
     _GUIResponseMessage(),
     _GUIResponded(0)
{}

GUIStarted::GUIStarted(const GUIStarted& state)
   : StateAbs(state),
     _GUIHandshakeTimer(0, Callback<GUIStarted>(this, &GUIStarted::sendGUIHandshake), TimerMessage::DISARMED),
     _GUIResponseMessage(),
     _GUIResponded(0)
{
   copyOver(state);
}

GUIStarted::~GUIStarted()
{
   cleanup();
}

int GUIStarted::transitionStatus ()
{
   if ( _GUIResponded )
   {
      DataLog(log_level_proc_info) << "GUI responded" << endmsg;
      return 1;
   }
   return NO_TRANSITION;
}

void GUIStarted::guiResponded ()
{
   _GUIResponded = 1;
   //
   // Stop the GUI timer ...
   //
   _GUIHandshakeTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop GUI handshake timer" << endmsg;
}

void GUIStarted::sendGUIHandshake ()
{
   //
   // Send the handshake message queries ...
   //
   EnableLoadSystemMsg queryGUIMsg(MessageBase::SEND_LOCAL);
   queryGUIMsg.send(1);
   DataLog(log_level_proc_info) << "GUI Message sent : Enable Load System " << endmsg;

}

int GUIStarted::preProcess ()
{
   return NORMAL;
}

int GUIStarted::postProcess ()
{
   return NORMAL;
}

int GUIStarted::preEnter ()
{
   //
   // Set the messages to receive the responses ...
   //
   _GUIResponseMessage.init(Callback<GUIStarted>(this, &GUIStarted::guiResponded) );
   DataLog(log_level_proc_debug) << "Create GUI response message" << endmsg;

   //
   // Send the handshake message queries ...
   //
   sendGUIHandshake();

   //
   // Set the timer to retry until a response is
   //  received from GUI ...
   //
   _GUIHandshakeTimer.interval(5000);
   DataLog(log_level_proc_debug) << "Start GUI handshake timer" << endmsg;

   return NORMAL;
}

int GUIStarted::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   //
   // Stop the timers ...
   //
   _GUIHandshakeTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop GUI handshake timer" << endmsg;

   return NORMAL;
}

void GUIStarted::copyOver (const GUIStarted&)
{}

void GUIStarted::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
   _GUIResponseMessage.deregisterMsg();
}

/* FORMAT HASH 470e779122188a9be7f1a0dbf7f3bf00 */
