/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      clearablealarm.h
 *
 * ABSTRACT:   This file encapsulates an entry for an alarm which has the
 *             capability of being cleared.  Note that an_alr_h.cpp will
 *             only allow clearing of alarms that are designated with a
 *             display type of "NO_DISPLAY".
 *
 */
#ifndef _CLEARABLE_ALARM_H_
#define _CLEARABLE_ALARM_H_

#include "an_alarm.h"

class ClearableAlarm
   : public anAlarmMsg
{
public:

   ClearableAlarm(ALARM_VALUES name)
      : anAlarmMsg(name) {}

   virtual ~ClearableAlarm() {}

   // I allow you to clear me in addition to my base class functions.
   virtual bool clearAlarm ()
   {
      // Is the alarm currently cleared in this task.
      if ( getState() != CLEARED )
      {
         // No, clear and send to alarm handler.
         sendMessage(CLEARED);
         return true;
      }
      return false;
   }

protected:

   // the default constructor, copy constructor, and op= are not implemented.
   ClearableAlarm();
   ClearableAlarm(const ClearableAlarm&);
   ClearableAlarm& operator = (const ClearableAlarm&);

};

#endif

/* FORMAT HASH d977b961c5ee4eae27d442ad1494569a */
