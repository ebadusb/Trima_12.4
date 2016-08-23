/*****************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      overridealr.h
 *             Message structure of override information
 *
 * AUTHOR:    Hari Rajagopal
 *
 *
 ****************************************************************************/


#if !defined( INCLUDE_OVERRIDEALR )
#define INCLUDE_OVERRIDEALR

#include "alarms.h"
#include "haldefs.h"

typedef struct
{

   //  Pump flow rates
   float Qin;
   float Qac;
   float Qrp;
   float Qplasma;
   float Qcollect;

   //  Cent on/off
   bool centSpeed;

   //  Cent at paused speed
   bool centPausedSpeed;

   //  Valve position
   HAL_VALVESTATE Vplt;
   HAL_VALVESTATE Vrbc;
   HAL_VALVESTATE Vpls;

   //  Lights on/off
   HAL_ALARMLIGHTORDERS lightsOn;

   //  System has an alarm active
   bool alarmActive;

   //  Last received alarm
   ALARM_VALUES rcvdAlarmID;
   //  Last alarm which had a response
   ALARM_VALUES respAlarmID;
   //  Current displayed alarm
   ALARM_VALUES dispAlarmID;

} override_alr;

#endif

/* FORMAT HASH f87aa80e7bd749585b252b984a54a1f9 */
