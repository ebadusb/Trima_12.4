/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acl_alarm.cpp
 *
 */


#include "acl_alarm.h"
#include "procdata.h"

const float VAC_ALARM_BLIND_VOLUME = 5.0f;
const int   NUMBER_ALERTS_ALLOWED  = 5; // before we A1 it8094

DEFINE_OBJ(ACLevelAlarm);

ACLevelAlarm::ACLevelAlarm()
   : _ACLevelAlarmMsg(AC_LEVEL_ALARM),
     _ACVolume(0.0f),
     _LastACVolume(-VAC_ALARM_BLIND_VOLUME),
     _acAlerts(0),
     _ACAlarm(AC_DETECTION_ALARM)
{}


ACLevelAlarm::~ACLevelAlarm()
{}


void ACLevelAlarm::Monitor ()
{
   ProcData          pd;
   const State_names myState = pd.SystemState();

   // Exit the monitor if it is turned off
   if (!_EnableMonitoring)
   {
      return;
   }

   // If we are in or past blood rinseback, disable the monitor
   if (myState >= BLOOD_RINSEBACK)
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Rinseback substate reached.  Deactivating AC Detected alarm" << endmsg;
      disable();

      return;
   }

   //
   // Incrememnt the volume ...
   _ACVolume += pd.Status().ACPump.dV();

   // Check to see if AC sensor is showing air
   if ( !pd.Status().ACDetectFluid() &&
        (_ACVolume >= _LastACVolume + VAC_ALARM_BLIND_VOLUME)
        )
   {
      //
      // Save the volume ...
      _LastACVolume = _ACVolume;

      if (_acAlerts < NUMBER_ALERTS_ALLOWED)
      {
         // Send alarm message indicating AC level sensor alarm
         _ACLevelAlarmMsg.setAlarm("AC Level alarm before the Max Limit");
      }
      else
      {
         DataLog(log_level_proc_alarm_monitor_info) << "AC Detector A1-ALARMING: Too many AC level alerts (IT8094) " << _acAlerts << endmsg;
         _ACAlarm.setAlarm();
         disable();
      }

      // only count the failures after AC prime
      if ( (myState > AC_PRIME) && (myState < BLOOD_RINSEBACK) )
      {
         _acAlerts++;
         DataLog(log_level_proc_alarm_monitor_info) << "Counting AC fluid mis-detections, thats " << _acAlerts << " AC alert and counting! " << endmsg;
      }
   }
}

/* FORMAT HASH 35777b400a2e710c2f937b31fe1c1281 */
