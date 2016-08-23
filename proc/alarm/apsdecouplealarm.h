/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      apsdecouplealarm.h
 *
 * ABSTRACT:  This alarm checks for a decoupled APS sensor during
 *             a procedure
 *
 */


#ifndef _APSDECOUPLE_ALARM_H_
#define _APSDECOUPLE_ALARM_H_

#include "monitorbase.h"
#include "objdictionary.h"
#include "an_alarm.h"
#include "clearablealarm.h"
#include "fastdata.h"


class APSDecoupleAlarm
   : public MonitorBase
{
   DECLARE_OBJ(APSDecoupleAlarm);
public:
   APSDecoupleAlarm();
   virtual ~APSDecoupleAlarm();

   virtual void Monitor ();

protected:

   virtual void enable ();
   virtual void disable ();

   //
   // Callback for the FASTDATA message
   void fastAPSData ();


protected:

   //
   // APS data readings
   Message< FASTDATA > _APSFastDataMsg;

   //
   // Alarm message
   //
   ClearableAlarm _APSDecoupledPause;
   anAlarmMsg     _APSDecoupledAlarm;

   //
   // Current time
   //
   float _AbsoluteTime;

   //
   // Time since decoupling was detected (in minutes)
   //
   float _DecouplingTime;

   //
   // Time since pumps were last paused (in minutes)
   //
   float _PumpsPausedTime;

   //
   // Flag to show the alarm checking is enabled
   //
   int _DecouplingEnabled;

   //
   // Flag to show that the alarm was latched by this
   //  object
   //
   int _PauseCount;

};


#endif

/* FORMAT HASH 789bb6aaf1468ece86fdd36508525b05 */
