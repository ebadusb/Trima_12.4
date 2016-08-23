/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      volt_alarm.cpp
 *
 */

#include "volt_alarm.h"
#include "trima_datalog.h"
#include "procdata.h"

DEFINE_OBJ(VoltageAlarm);

VoltageAlarm::VoltageAlarm()
   : _Minus12VLowAlarm(minus12V_TOO_LOW),
     _Minus12VHighAlarm(minus12V_TOO_HIGH),
     _plus64VHighAlarm(plus64V_TOO_HIGH),
     _plus64VLowAlarm(plus64V_TOO_LOW),
     _plus24VLowAlarm(plus24V_TOO_LOW),
     _plus24VHighAlarm(plus24V_TOO_HIGH),
     _plus12VLowAlarm(plus12V_TOO_LOW),
     _plus12VHighAlarm(plus12V_TOO_HIGH)
{}

VoltageAlarm::~VoltageAlarm ()
{}

void VoltageAlarm::Monitor ()
{
   ProcData pd;

   if ( pd.Status().PS64V.TooHigh() )
   {
      _plus64VHighAlarm.setAlarm();
      DataLog(log_level_proc_alarm_monitor_info) << "Voltage out of bounds: 64V reading -> " << pd.Status().PS64V.mV() << endmsg;
   }

   if ( pd.Status().PS64V.TooLow() )
   {
      _plus64VLowAlarm.setAlarm();
      DataLog(log_level_proc_alarm_monitor_info) << "Voltage out of bounds: 64V reading -> " << pd.Status().PS64V.mV() << endmsg;
   }

   if ( pd.Status().PS24V.TooHigh() )
   {
      _plus24VHighAlarm.setAlarm();
      DataLog(log_level_proc_alarm_monitor_info) << "Voltage out of bounds: 24V reading -> " << pd.Status().PS24V.mV() << endmsg;
   }

   if ( pd.Status().PS24V.TooLow() )
   {
      _plus24VLowAlarm.setAlarm();
      DataLog(log_level_proc_alarm_monitor_info) << "Voltage out of bounds: 24V reading -> " << pd.Status().PS24V.mV() << endmsg;
   }

   if ( pd.Status().PSp12V.TooHigh() )
   {
      _plus12VHighAlarm.setAlarm();
      DataLog(log_level_proc_alarm_monitor_info) << "Voltage out of bounds: 12V reading -> " << pd.Status().PSp12V.mV() << endmsg;
   }

   if ( pd.Status().PSp12V.TooLow() )
   {
      _plus12VLowAlarm.setAlarm();
      DataLog(log_level_proc_alarm_monitor_info) << "Voltage out of bounds: 12V reading -> " << pd.Status().PSp12V.mV() << endmsg;
   }

   if ( pd.Status().PSm12V.TooHigh() )
   {
      _Minus12VHighAlarm.setAlarm();
      DataLog(log_level_proc_alarm_monitor_info) << "Voltage out of bounds: neg 12V reading -> " << pd.Status().PSm12V.mV() << endmsg;
   }
   if ( pd.Status().PSm12V.TooLow() )
   {
      _Minus12VLowAlarm.setAlarm();
      DataLog(log_level_proc_alarm_monitor_info) << "Voltage out of bounds: neg 12V reading -> " << pd.Status().PSm12V.mV() << endmsg;
   }

}

/* FORMAT HASH ca16972c2aba87ceca0c5cdf0c10208e */
