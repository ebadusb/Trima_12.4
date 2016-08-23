/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      startprotocol.cpp
 *
 */

#include "startprotocol.h"
#include "states.h"
#include "procdata.h"
#include "task_start.h"
#include "trima_info.h"


DEFINE_STATE(StartProtocol);

StartProtocol::StartProtocol()
   : StateAbs(),
     _GUIHandshakeTimer(0, Callback<StartProtocol>(this, &StartProtocol::sendGUIHandshake), TimerMessage::DISARMED),
     _SafetyHandshakeTimer(0, Callback<StartProtocol>(this, &StartProtocol::sendSafetyHandshake), TimerMessage::DISARMED),
     _QueryGUIMessage(MessageBase::SEND_GLOBAL),
     _QuerySafetyMessage(MessageBase::SEND_GLOBAL),
     _GUIResponseMessage(Callback<StartProtocol>(this, &StartProtocol::guiResponded) ),
     _SafetyResponseMessage(Callback<StartProtocol>(this, &StartProtocol::safetyResponded) ),
     _NonResponsiveTimer(0, Callback<StartProtocol>(this, &StartProtocol::systemNotResponding), TimerMessage::DISARMED),
     _GUIResponded(0),
     _SafetyResponded(0)
{
   maintainHistory(1);
}

StartProtocol::StartProtocol(const StartProtocol& state)
   : StateAbs(state),
     _GUIHandshakeTimer(0, Callback<StartProtocol>(this, &StartProtocol::sendGUIHandshake), TimerMessage::DISARMED),
     _SafetyHandshakeTimer(0, Callback<StartProtocol>(this, &StartProtocol::sendSafetyHandshake), TimerMessage::DISARMED),
     _QueryGUIMessage(MessageBase::SEND_GLOBAL),
     _QuerySafetyMessage(MessageBase::SEND_GLOBAL),
     _GUIResponseMessage(Callback<StartProtocol>(this, &StartProtocol::guiResponded) ),
     _SafetyResponseMessage(Callback<StartProtocol>(this, &StartProtocol::safetyResponded) ),
     _GUIResponded(0),
     _SafetyResponded(0)
{
   copyOver(state);
}

StartProtocol::~StartProtocol()
{
   cleanup();
}

int StartProtocol::transitionStatus ()
{


   // if we're in service mode we just stay looping in StartProtocol
   // all we want to do is log stuff that happens........
   if (TrimaInfo::isServiceMode())
      return NO_TRANSITION;

   ProcData pd;

   if ( !pd.AlarmActive() &&
        _GUIResponded &&
        _SafetyResponded )
   {
      DataLog(log_level_proc_info) << "GUI and Safety both responded" << endmsg;
      return 1;
   }

   return NO_TRANSITION;
}

void StartProtocol::guiResponded ()
{
   _GUIResponded = 1;
   DataLog(log_level_proc_info) << "GUI responded" << endmsg;
   //
   // Stop the GUI timer ...
   //
   _GUIHandshakeTimer.interval(0);
   DataLog(log_level_proc_debug) << "GUI Response timer stopped" << endmsg;
}

void StartProtocol::safetyResponded ()
{
   _SafetyResponded = 1;
   DataLog(log_level_proc_info) << "Safety responded" << endmsg;
   //
   // Stop the GUI timer ...
   //
   _SafetyHandshakeTimer.interval(0);
   DataLog(log_level_proc_debug) << "Safety Response timer stopped" << endmsg;

   ProcData pd;

   pd.xIf().safetyStarted.Set(true);
}

void StartProtocol::sendGUIHandshake ()
{
   //
   // Send the handshake message queries ...
   //
   _QueryGUIMessage.send(1);
   DataLog(log_level_proc_info) << "GUI Handshake sent " << endmsg;

}

void StartProtocol::sendSafetyHandshake ()
{
   //
   // Send the handshake message queries ...
   //
   _QuerySafetyMessage.send(1);
   DataLog(log_level_proc_info) << "Safety Handshake sent" << endmsg;
}

void StartProtocol::systemNotResponding ()
{
   // Turn off the timers to stop the logging.
   _NonResponsiveTimer.interval(0);
   _GUIHandshakeTimer.interval(0);
   _SafetyHandshakeTimer.interval(0);

   if ( _GUIResponded )
   {
      //
      // Set an A2 alarm ...
      anAlarmMsg notRespondingAlarm(SAFETY_NOT_RESPONDING);
      notRespondingAlarm.setAlarm();
   }
   else
   {
      _FATAL_ERROR(__FILE__, __LINE__, "GUI has failed to respond");
   }
}

int StartProtocol::preProcess ()
{
   return NORMAL;
}

int StartProtocol::postProcess ()
{
   return NORMAL;
}

int StartProtocol::preEnter ()
{



   //                                                                //          \||/              |
   // Set Startup Test State message ...                             //          _____             |
   //                                                                //         /     \            |
   ProcData pd;                                                      //        /\    / \           |
   pd.SystemState(SELF_TEST);                                        //      (|   0 0   |)         |
                                                                     //       |   ___   |          |
   // Running in normal mode                                         //        \       /           |
   if ( !TrimaInfo::isServiceMode() )                                //         \_____/            |
   {                                                                 //                            |
      //                                                            //                            |
      // Send the handshake message queries ...                     //    Have an ordinary day    |
      //
      sendGUIHandshake();
      sendSafetyHandshake();

      //
      // Set the timer to retry until a response is
      //  received from both GUI and Safety ...
      //
      _GUIHandshakeTimer.interval(500);
      DataLog(log_level_proc_debug) << "GUI Response timer created" << endmsg;
      _SafetyHandshakeTimer.interval(500);
      DataLog(log_level_proc_debug) << "Safety Response timer created" << endmsg;

      _NonResponsiveTimer.interval(120000);    // 2 minutes = 120000 milliseconds
      DataLog(log_level_proc_debug) << "Not responding timer created" << endmsg;

   }
   else
   {  // in SERVICE Mode

      // if we're in service mode we just stay looping in StartProtocol
      // all we want to do is log stuff that happens........
      DataLog(log_level_proc_info) << "Proc starting in service mode for Dlog" << endmsg;
   }

   return NORMAL;
}

int StartProtocol::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   //
   // Stop the timers ...
   //
   _GUIHandshakeTimer.interval(0);
   DataLog(log_level_proc_debug) << "GUI Response timer stopped" << endmsg;
   _SafetyHandshakeTimer.interval(0);
   DataLog(log_level_proc_debug) << "Safety Response timer stopped" << endmsg;
   _NonResponsiveTimer.interval(0);
   DataLog(log_level_proc_debug) << "Not responding timer stopped" << endmsg;


   return NORMAL;
}

void StartProtocol::copyOver (const StartProtocol&)
{}

void StartProtocol::cleanup ()
{
   //
   // Delete any dynamic memory ...
   //
}

/* FORMAT HASH 931f8514c13858a7a5e52eec62ee82fc */
