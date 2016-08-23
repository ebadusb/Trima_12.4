/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      reslowalarm.h
 *
 * ABSTRACT:  This object monitors the low level sensor for
 *             fluid seen before blood prime
 *
 */

#ifndef _RES_LOW_ALARM_H_
#define _RES_LOW_ALARM_H_


#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class ResLowLevelAlarm
   : public MonitorBase
{
   DECLARE_OBJ(ResLowLevelAlarm);
public:

   ResLowLevelAlarm();
   virtual ~ResLowLevelAlarm();

   virtual void Monitor ();

protected:

   anAlarmMsg _LowLevelAlarm;

};

#endif

/* FORMAT HASH 9abe8d25783f3d1af22d622e5f343489 */
