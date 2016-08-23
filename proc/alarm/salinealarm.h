/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acl_alarm.h
 *
 * ABSTRACT:  This alarm checks for ac seen at the sensor during the run.
 *             If the sensor detects air, the alarm is set.
 *
 */


#ifndef SALINE_ALARM_H
#define SALINE_ALARM_H

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"
#include "trimamessages.h"

class SalineAlarm
   : public MonitorBase
{
   DECLARE_OBJ(SalineAlarm);
public:
   SalineAlarm();
   virtual ~SalineAlarm();

   virtual void Monitor ();

   virtual void enable ();
   virtual void disable ();

   void repFluidConnected ();
   void acConnected ();
   void salineBolusRequest ();

private:

   anAlarmMsg               _SalineAlarmMsg;
   anAlarmMsg               _SalineBolusAlarmMsg;

   RepFluidConnectStatusMsg _RepFluidConMsg;
   ACConnectedMsg           _ACConMsg;
   SalineBolusMsg           _SalineBolusMsg;

   int _ACConnected;

};


#endif

/* FORMAT HASH 9022564b0fee057cbad758a37944dda9 */
