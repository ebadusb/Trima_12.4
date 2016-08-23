/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      drbcsplitalarm.h
 *
 * ABSTRACT:  This alarm notifies the operator when to switch rbc bags
 *             during a drbc run to properly filter the dual products.
 *
 */

#ifndef DRBC_SLPIT_ALARM_H
#define DRBC_SLPIT_ALARM_H

#include "monitorbase.h"
#include "objdictionary.h"
#include "an_alarm.h"

class DRBCSplitAlarm
   :  public MonitorBase
{
   DECLARE_OBJ(DRBCSplitAlarm);
public:

   DRBCSplitAlarm();
   virtual ~DRBCSplitAlarm();

   virtual void Monitor ();

protected:

   anAlarmMsg _DRBCSplitAlarm;

   int        _SplitNotificationSent;

};

#endif

/* FORMAT HASH 4400c1c1d9bc0270adc6e597cdace83c */
