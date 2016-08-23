/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acl_alarm.h
 *
 * ABSTRACT:  This alarm checks for ac seen at the sensor during the run.
 *             If the sensor detects air, the alarm is set.
 *
 */


#ifndef AC_LEVEL_ALARM_H
#define AC_LEVEL_ALARM_H

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class ACLevelAlarm
   : public MonitorBase
{
   DECLARE_OBJ(ACLevelAlarm);
public:
   ACLevelAlarm();
   virtual ~ACLevelAlarm();

   virtual void Monitor ();

private:

   anAlarmMsg _ACLevelAlarmMsg;

   float      _ACVolume;
   float      _LastACVolume;

   // IT 8094 make AC alarm A1 after n alerts
   int        _acAlerts;
   anAlarmMsg _ACAlarm;

};


#endif

/* FORMAT HASH 6edc30bcb790e38e83b7c8b432f823f6 */
