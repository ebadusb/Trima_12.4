/*
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      volt_alarm.h
 *
 * ABSTRACT:   This object checks the voltage levels for the power supplies
 *              during the run
 *
 */

#ifndef _VOLT_ALARM_H_
#define _VOLT_ALARM_H_

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class VoltageAlarm
   : public MonitorBase
{
   DECLARE_OBJ(VoltageAlarm);
public:

   VoltageAlarm();
   virtual ~VoltageAlarm();

   virtual void Monitor ();


protected:

   // various alarm messages used
   anAlarmMsg _Minus12VLowAlarm;
   anAlarmMsg _Minus12VHighAlarm;
   anAlarmMsg _plus64VHighAlarm;
   anAlarmMsg _plus64VLowAlarm;
   anAlarmMsg _plus24VLowAlarm;
   anAlarmMsg _plus24VHighAlarm;
   anAlarmMsg _plus12VLowAlarm;
   anAlarmMsg _plus12VHighAlarm;

};

#endif

/* FORMAT HASH 97077f40395938f7f479074393bf69f2 */
