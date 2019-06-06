/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      an_alarm.hpp
 *             Focused System master controller
 *
 * AUTHOR:     Tate Moore
 *
 * ABSTRACT:   This file contains the definitions for anAlarmMsg.
 *             All tasks wishing to generate an Alarm message should
 *             include this file and then define the particular alarm
 *             message in there file.
 *
 *   The following example would create an alarm message instance
 *   which would be the access pressure alarm, and then assert it
 *   (send it to the AlarmHandler):
 *
 *      anAlarmMsg * access_alarm_msg = new anAlarmMsg( APS_TOO_LOW );
 *      access_alarm_msg.set( __FILE__ ": Vampires suck blood" );
 *
 *   where APS_TOO_LOW is from the ALARM_VALUES enumeration.
 */

#ifndef AN_ALARM_HPP
#define AN_ALARM_HPP

#include "alarms.h"
#include "message.h"

class anAlarmMsg
{
public:

   anAlarmMsg(ALARM_VALUES name);

   virtual ~anAlarmMsg();

   virtual void init (ALARM_VALUES name); // resets alarm name

   virtual void setAlarm (const char* str = NULL);

   virtual void latchAlarm ();

   virtual void unlatchAlarm ();

   virtual ALARM_STATES getState ();

protected:
   void sendMessage (ALARM_STATES state);

   // Message handler to update the current state.  The implementation assumes that
   // there is really only one of each alarm - meaning, if you set an APS_PAUSE in
   // one task then it can be queried as set in another task.  This matches the
   // historical implementation.
   static void globalNotify ();

protected:

   ALARM_VALUES                 _alarm;
   static Message<Alarm_struct>*_globalNotify;

private:

   // These methods will not be implemented.
   anAlarmMsg();
   anAlarmMsg(const anAlarmMsg&);
   anAlarmMsg& operator = (const anAlarmMsg&);
};

#endif

/* FORMAT HASH 87a53e917f1bfec4cb061b3cf394130c */
