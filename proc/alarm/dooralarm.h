/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      dooralarm.h
 *
 * ABSTRACT:  This alarm checks for various door problems
 *             during the run
 *
 */


#ifndef _DOOR_ALARM_H_
#define _DOOR_ALARM_H_


#include "monitorbase.h"
#include "an_alarm.h"
#include "shw.hpp"
#include "timermessage.h"
#include "objdictionary.h"
#include "trimamessages.h"


class DoorAlarm
   : public MonitorBase
{
   DECLARE_OBJ(DoorAlarm);
public:

   DoorAlarm();
   virtual ~DoorAlarm();

   virtual void Monitor ();

   virtual void enable ();
   virtual void disable ();

   void doorNotLocked ();

protected:

   void safetyStatus ();
   void safetyPowerResponse ();
   void disable64VPower ();
   void enable64VPower ();

protected:

   //
   // Handle timing for door lock alarm
   //
   TimerMessage _DoorLockTimer;
   bool         _DoorLockTimerStarted;

   //
   // Message that informs me of the status of the power
   //  request ...
   //
   SafetyTestDoneMsg _SafetyPowerResponseMsg;

   // the alarm objects
   anAlarmMsg _DoorLockAlarm;
   anAlarmMsg _DoorOpenAlarm;

};

#endif

/* FORMAT HASH 72a3d39898113a30253596ad06ca6ac2 */
