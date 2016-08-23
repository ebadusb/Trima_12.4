/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centpres_alarm.h
 *
 * ABSTRACT:  This alarm checks for centrifuge pressure problems
 *             during the run
 *
 */

#ifndef CENTPRES_ALARM_H
#define CENTPRES_ALARM_H

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class CentPresAlarm
   : public MonitorBase
{
   DECLARE_OBJ(CentPresAlarm);
public:
   CentPresAlarm();
   virtual ~CentPresAlarm();

   virtual void Monitor ();

protected:

   anAlarmMsg
      _centPresAlertNoClamp,
      _centPresAlertOneClamp,
      _centPresAlertThreeClamp,
      _centPresAlertThreeClampPtf;

};

#endif

/* FORMAT HASH 796c83a376ef22ff321491d7a6251e29 */
