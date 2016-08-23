/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acprimeinlet_alarm.h
 *
 * ABSTRACT:  These alarms check for AC occlusions during prime.
 *
 */


#ifndef AC_PRIME_INLET_ALARM_H
#define AC_PRIME_INLET_ALARM_H

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class AcPrimeInletAlarm
   : public MonitorBase
{
   DECLARE_OBJ(AcPrimeInletAlarm);
public:
   AcPrimeInletAlarm();
   virtual ~AcPrimeInletAlarm();

   virtual void Monitor ();

private:
   anAlarmMsg _ACOcclusionAlarmMsg;
   anAlarmMsg _AcPrimeInletAlarmMsg;
   int        _FirstTime;
   float      _ApsSnapshot;
   float      _ApsCurrent;
   float      _AcVolume;
   int        _SecondStageThrown;
};

#endif

/* FORMAT HASH 6c9e69fa5d0f30eab7b3ed796f7b72a2 */
