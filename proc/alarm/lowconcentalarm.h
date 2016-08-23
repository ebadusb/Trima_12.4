/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      lowconcentalarm.h
 *
 * ABSTRACT:  This alarm checks the concentration of the platelet line
 *             at the configured point in the procedure.  If the concentration
 *             is seen as too low, the alert will be generated.
 *
 */

#ifndef _LOW_CONCENT_ALARM_H_
#define _LOW_CONCENT_ALARM_H_

#include "monitorbase.h"
#include "objdictionary.h"
#include "an_alarm.h"

class LowConcentrationAlarm
   : public MonitorBase
{
   DECLARE_OBJ(LowConcentrationAlarm);
public:
   LowConcentrationAlarm ();
   virtual ~LowConcentrationAlarm ();

   virtual void Monitor ();



private:

   anAlarmMsg _LowConcentrationAlert;
   int        _NotAlreadyGiven;

};


#endif

/* FORMAT HASH 7088b5ccbd732f6e337798b048ea7c40 */
