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

   //
   // constructors, destructor
   //
   ClearableAlarm(ALARM_VALUES name)
      : anAlarmMsg(name) {}

   // Destructor
   virtual ~ClearableAlarm() {}

   //
   //
   //   I allow you to clear me in addition to my base class functions.
   //
   virtual void clearAlarm ()
   {
      // Is the alarm currently cleared in this task.
      if ( the_alarm.alarm_state != CLEARED )
      {
         // No, clear and send to alarm handler.
         the_alarm.alarm_state = CLEARED;
         alarmmsg.send(the_alarm);
      }
   }

protected:

   // the default constructor, copy constructor, and op= are not implemented.
   ClearableAlarm();
   ClearableAlarm(const ClearableAlarm&);
   ClearableAlarm& operator = (const ClearableAlarm&);

};

#endif

/* FORMAT HASH ef34971145a252b11da54dae36506dcf */
