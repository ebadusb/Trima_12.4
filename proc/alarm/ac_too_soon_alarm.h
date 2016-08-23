/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ac_too_soon_alarm.h
 *
 * ABSTRACT:  This alarm checks for ac seen at the sensor before the
 *             AC_PRIME state ...
 *
 */

#ifndef AC_TOO_SOON_ALARM_H
#define AC_TOO_SOON_ALARM_H

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class ACTooSoonAlarm
   : public MonitorBase
{
   DECLARE_OBJ(ACTooSoonAlarm);
public:

   ACTooSoonAlarm();
   virtual ~ACTooSoonAlarm();

   virtual void Monitor ();

protected:
   anAlarmMsg _ACTooSoonAlarm;

};


#endif

/* FORMAT HASH a70b9e52a020ac7deabb3845bfa436bb */
