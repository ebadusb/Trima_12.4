/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      an_alarm.hpp
 *             Focused System master controller
 *
 * AUTHOR:     Tate Moore
 *
 * ABSTRACT:   This file contains the definitions for anAlarmMsg.
 *                       All tasks wishing to generate an Alarm message should
 *                       include this file and then define the particular alarm
 *                       message in there file.
 *
 *   The following example would create an alarm message instance
 *   which would be the access pressure alarm.
 *      anAlarmMsg * access_alarm_msg = new anAlarmMsg( ACCESS_PRESSURE );
 *
 ****** ACCESS_PRESSURE is from the ALARM_VALUES enumration.
 *
 *
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 */

#ifndef AN_ALARM_HPP
#define AN_ALARM_HPP

#include "alarms.h"
#include "trima_datalog.h"
#include "message.h"

class anAlarmMsg
{
protected:

   // These methods will not be implemented.
   anAlarmMsg();
   anAlarmMsg(const anAlarmMsg&);
   anAlarmMsg& operator = (const anAlarmMsg&);

protected:
   // The name and the state of the alarm instance are stored here.
   Alarm_struct          the_alarm;
   Message<Alarm_struct> alarmmsg;

public:
   // Constructor
   // Each instance must be created with a specific alarm in mind.
   anAlarmMsg(ALARM_VALUES name)
   {
      the_alarm.alarm_name  = name;
      the_alarm.alarm_state = CLEARED;
      alarmmsg.init(Callback<anAlarmMsg>(this, &anAlarmMsg::notify), MessageBase::SNDRCV_GLOBAL);
   }

   // Destructor
   virtual ~anAlarmMsg()
   {}

   virtual void inline setAlarm (const char* str = NULL)
   {
      // make ACTIVE and send to alarm handler.
      the_alarm.alarm_state =  ACTIVE;

      if ( str )
      {
         DataLog(log_level_trima_library_info) << str << endmsg;

      }

      DataLog(log_level_trima_library_info) << "setting alarm " << the_alarm.alarm_name << endmsg;
      alarmmsg.send(the_alarm);
   }

   virtual void inline latchAlarm ()
   {
      // latch alarm and send to alarm handler.
      the_alarm.alarm_state =  LATCHED;

      DataLog(log_level_trima_library_info) << "latching alarm " << the_alarm.alarm_name << endmsg;
      alarmmsg.send(the_alarm);
   }

   virtual void inline unlatchAlarm ()
   {
      // make ACTIVE and send to alarm handler.
      the_alarm.alarm_state = ACTIVE;

      DataLog(log_level_trima_library_info) << "unlatching alarm " << the_alarm.alarm_name << endmsg;
      alarmmsg.send(the_alarm);
   }

   virtual inline ALARM_STATES getState ()
   {
      return the_alarm.alarm_state;
   }

   virtual void notify ()
   {
      Alarm_struct temp_alarm;
      temp_alarm = alarmmsg.getData();

      // If the alarm is the same as the one used in this instance.
      if ( temp_alarm.alarm_name == the_alarm.alarm_name )
      {
         the_alarm.alarm_state = temp_alarm.alarm_state;
      }

   }

};

#endif

/* FORMAT HASH 8fa9eaca3a8a47a8f50c6315b3c17010 */
