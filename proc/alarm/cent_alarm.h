/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cent_alarm.h
 *
 * ABSTRACT:  This alarm checks for various centrifuge problems
 *             during the run
 *
 */

#ifndef CENT_ALARM_H
#define CENT_ALARM_H

#include "monitorbase.h"
#include "an_alarm.h"
#include "timermessage.h"
#include "objdictionary.h"

class CentAlarm
   :  public MonitorBase
{
   DECLARE_OBJ(CentAlarm);
public:

   CentAlarm();
   virtual ~CentAlarm();

   virtual void Monitor ();

   //
   // Callback for the cent speed timer
   //
   void centNotUpToSpeed ();

protected:

   //
   // Function for making sure the centrifuge is responding
   //  to the command
   //
   void checkActualVsCommand ();

protected:

   TimerMessage _CentSpeedTimer;

   anAlarmMsg   _CentErrAlarm;
   anAlarmMsg   _CentIntegralAlarm;
   anAlarmMsg   _CentOverSpeedAlarm;

   float        _CentVoltOffError;

   int          _IntegrationErrorReported;

   float        _CurrentCentCommand;
   int          _CentSpeedTimerStarted;

};

#endif

/* FORMAT HASH 23db2f0d3963e44bde60c877c771f110 */
