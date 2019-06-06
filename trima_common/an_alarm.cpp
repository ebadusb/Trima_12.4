/**
 * Copyright (C) 2015 Terumo BCT, Inc.  All rights reserved.
 *
 * @file   an_alarm.cpp
 *
 * @author mra1usb
 * @date   Jul 1, 2015
 *
 * @brief  Implementation of anAlarmMsg.
 *
 * Moved the inlined-implementation out of an_alarm.hpp.
 * It's now part of the libcontrol_reentrant.a library.
 *
 */

#include "an_alarm.h"
#include "alarmcontainer.h"
#include "trima_datalog.h"

Message<Alarm_struct>*anAlarmMsg::_globalNotify = 0;  // pointer used so we can construct when first used;
                                                      // since this class is in trima_common it gets loaded
                                                      // before the message system is initialized

anAlarmMsg::anAlarmMsg (ALARM_VALUES name)
   : _alarm(name)
{
   //
   //  Initialize our global callback at the time of first usage.  Note never deleted (it's really a static function masquerading as a pointer)
   if (!_globalNotify)
   {
      _globalNotify = new Message<Alarm_struct>(Callback<anAlarmMsg>(&anAlarmMsg::globalNotify), MessageBase::SNDRCV_RECEIVE_ONLY);
   }
}

anAlarmMsg::~anAlarmMsg ()
{}

void anAlarmMsg::init (ALARM_VALUES name)
{
   _alarm = name;
}

ALARM_STATES anAlarmMsg::getState ()
{
   static AlarmTable alarmTable; // declared here instead of at global scope since this is in the trima_common
                                 // library; avoids collisions with construction before alarm container class.
                                 // The alarmTable class is all static (size zero) so no real problem having
                                 // multiple static instances scattered in this class.
   return alarmTable[_alarm].getState();
}

void anAlarmMsg::sendMessage (ALARM_STATES state)
{
   static Message<Alarm_struct> alarmMsg(MessageBase::SEND_GLOBAL); // see IT17821: one message is fine for outgoing
   static AlarmTable            alarmTable;
   alarmTable[_alarm].setState(state); // this is a little redundant because it will get updated via the notify
                                       // anyway, but avoids latency if external classes call getState().

   Alarm_struct sendAlarm = {_alarm, state};
   alarmMsg.send(sendAlarm);
}

void anAlarmMsg::setAlarm (const char* str)
{
   if (str)
   {
      DataLog(log_level_trima_library_info) << str << endmsg;
   }

   DataLog(log_level_qa_external) << "setting alarm " // this logging used by QA for verification
                                  << _alarm << endmsg;
   sendMessage(ACTIVE);
}

void anAlarmMsg::latchAlarm ()
{
   DataLog(log_level_trima_library_info) << "latching alarm " << _alarm << endmsg;
   sendMessage(LATCHED);
}

void anAlarmMsg::unlatchAlarm ()
{
   DataLog(log_level_trima_library_info) << "unlatching alarm " << _alarm << endmsg;
   sendMessage(ACTIVE);
}

//
//
//   We listen to all Alarm_struct messages and update the alarmTable state for each one.
//
void anAlarmMsg::globalNotify ()
{
   static AlarmTable alarmTable;
   Alarm_struct      msgData(_globalNotify->getData());

   alarmTable[msgData.alarm_name].setState(msgData.alarm_state);
}

/* FORMAT HASH bde23886b7bbe5df5b459e53dcd4f9ea */
