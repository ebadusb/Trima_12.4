/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mot_alarm.h
 *
 * ABSTRACT:  This alarm checks voltage levels for the pumps during the run
 *
 */

#ifndef _MOT_ALARM_H
#define _MOT_ALARM_H

#include "monitorbase.h"
#include "an_alarm.h"
#include "timermessage.h"
#include "objdictionary.h"
#include "halstatus_cds.h"

const int MotorFaultAlarmCount = 4;

class MotorAlarm
   : public MonitorBase
{
   DECLARE_OBJ(MotorAlarm);
public:

   MotorAlarm();
   virtual ~MotorAlarm();


   virtual void Monitor ();

   void pumpsNotUpToSpeed ();

protected:

   void checkActualVsCommand ();
   void checkActualVsCommand (const HalPump_CDS& pump,
                              TimerMessage& timer,
                              float& currentCommand);
   bool pumpOutOfTolerance (const HalPump_CDS& pump);
   bool plasmaMHFIgnore ();

protected:

   anAlarmMsg*  _MotorFaultAlarmP[MotorFaultAlarmCount];
   anAlarmMsg   _ValveFaultAlarm;
   anAlarmMsg   _CollectValveAlarm;
   anAlarmMsg   _PlasmaValveAlarm;
   anAlarmMsg   _RBCValveAlarm;

   unsigned int _RBCValveCount;
   unsigned int _PlasmaValveCount;
   unsigned int _CollectValveCount;

   float        _CurrentACCommand;
   float        _CurrentCollectCommand;
   float        _CurrentInletCommand;
   float        _CurrentPlasmaCommand;
   float        _CurrentReturnCommand;

   TimerMessage _ACPumpTimer;
   TimerMessage _InletPumpTimer;
   TimerMessage _PlasmaPumpTimer;
   TimerMessage _PlateletPumpTimer;
   TimerMessage _ReturnPumpTimer;

   short        _plasmaMHFCounter;
};

#endif

/* FORMAT HASH 76ec887d31452057c70f047161e85d1c */
