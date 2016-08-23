/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pause_alarm.h
 *
 * ABSTRACT:  This is the base class for pumps paused events
 *
 */

#ifndef _PAUSE_ALARM_H_
#define _PAUSE_ALARM_H_

#include "monitorbase.h"
#include "timermessage.h"
#include "objdictionary.h"
#include "an_alarm.h"

class PauseAlarm
   : public MonitorBase
{
   DECLARE_OBJ(PauseAlarm);
public:

   PauseAlarm();
   virtual ~PauseAlarm();

   virtual void Monitor ();

   virtual void enable ();
   virtual void disable ();

protected:

   void pumpsPausedOneMinute ();
   void pumpsPausedThreeMinutes ();
   void pumpsPausedTenMinutes ();

   void alarmEnabled ();

   void clearTimers ();

protected:

   anAlarmMsg   _OneMinuteAlert;
   anAlarmMsg   _ThreeMinuteAlert;
   anAlarmMsg   _TenMinuteAlert;
   anAlarmMsg   _TenMinuteAlarm;

   TimerMessage _OneMinuteTimer;
   TimerMessage _ThreeMinuteTimer;
   TimerMessage _TenMinuteTimer;
   TimerMessage _EnableTimer;

   int          _ACBloodAtNeedle;

   int          _TimersStarted;

   int          _OneMinutePassed;
   int          _ThreeMinutesPassed;
   int          _TenMinutesPassed;

};

#endif

/* FORMAT HASH ad21ccf56e6593d4c26c9907a0b42adf */
