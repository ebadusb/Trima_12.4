/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      leakalarm.h
 *
 * ABSTRACT:  This alarm checks the leak detector sensor
 *             during the run
 *
 */

#ifndef _LEAK_ALARM_H_
#define _LEAK_ALARM_H_

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class LeakAlarm
   : public MonitorBase
{
   DECLARE_OBJ(LeakAlarm);
public:
   LeakAlarm();
   virtual ~LeakAlarm();

   virtual void Monitor ();

protected:

   anAlarmMsg _LeakAlarmMsg;
   anAlarmMsg _LeakErrorAlarm;

   int        _DetectorInitialized;
};

#endif

/* FORMAT HASH 3c609ab01b48e930844d3baf182b5d24 */
