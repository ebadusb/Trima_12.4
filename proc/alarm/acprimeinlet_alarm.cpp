/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acprimeinlet_alarm.cpp
 *
 */

#include "acprimeinlet_alarm.h"
#include "procdata.h"

DEFINE_OBJ(AcPrimeInletAlarm);

const float FIRST_STAGE_PRESSURE_LIMIT  = 20.0f;
const float SECOND_STAGE_PRESSURE_LIMIT = 40.0f;
const float THIRD_STAGE_PRESSURE_LIMIT  = 80.0f;

AcPrimeInletAlarm::AcPrimeInletAlarm()
   : _ACOcclusionAlarmMsg(AC_OCCLUSION_DETECTED),
     _AcPrimeInletAlarmMsg(AC_PRIME_INLET_ALARM),
     _FirstTime(1),
     _AcVolume(0.0f),
     _SecondStageThrown(0),
     _ApsSnapshot(0.0f),
     _ApsCurrent(0.0f)
{}

AcPrimeInletAlarm::~AcPrimeInletAlarm()
{}

void AcPrimeInletAlarm::Monitor ()
{
   ProcData pd;

   // Increment the volume...
   _AcVolume += pd.Status().ACPump.dV();

   // first time through we snapshot the APS
   if (_FirstTime)
   {
      _FirstTime   = 0;
      _ApsSnapshot = pd.Status()._aps;
   }

   _ApsCurrent = pd.Status()._aps;
   // if pressure drops > 80mmHg at any time during ac prime inlet we A1.
   // note that we check the THIRD_STAGE limit first so that if the limit
   // is hit we don't throw an R1 when we really want an A1.
   if ((_ApsSnapshot - _ApsCurrent) > THIRD_STAGE_PRESSURE_LIMIT)
   {
      DataLog(log_level_proc_alarm_monitor_info) << "AcPrimeInlet Monitoring:  Third Stage hit, showing A1" << endmsg;
      DataLog(log_level_proc_alarm_monitor_info) << "AcPrimeInlet Monitoring:  Snapshot(" << _ApsSnapshot
                                                 << "), Current(" << _ApsCurrent << ")" << endmsg;
      _AcPrimeInletAlarmMsg.setAlarm();
   }

   // if pressure drops > 20mmHg during ac prime inlet we reduce the
   // AC prime flow from 50ml/min to 30ml/min.  The thought is that this
   // reduced flow would reduce the % actual vs commanded flow differential due
   // to a temporary partial occlusion and so reduce the likelihood of
   // triggering the next stage of the protection cascade (R1's and A1's)
   if ((_ApsSnapshot - _ApsCurrent) > FIRST_STAGE_PRESSURE_LIMIT)
   {
      DataLog(log_level_proc_alarm_monitor_info) << "AcPrimeInlet Monitoring:  Slowing Qac and Qin to 30ml/min" << endmsg;
      DataLog(log_level_proc_alarm_monitor_info) << "AcPrimeInlet Monitoring:  Snapshot(" << _ApsSnapshot
                                                 << "), Current(" << _ApsCurrent << ")" << endmsg;
      pd.Orders().Qac(30.0f);
      pd.Orders().Qin(30.0f);
   }

   // if pressure drops > 40mmHg during 1st 10ml then we R1 and instruct the operator
   // to verify no AC or inlet occlusions before continueing.  Notice we only throw this
   // alarm once and then ignore it afterwards; otherwise once they get it they will
   // continue to get it over and over and over....
   if (_AcVolume <= 10.0f)
   {
      if (((_ApsSnapshot - _ApsCurrent) > SECOND_STAGE_PRESSURE_LIMIT) && !_SecondStageThrown)
      {
         _SecondStageThrown = 1;
         DataLog(log_level_proc_alarm_monitor_info) << "AcPrimeInlet Monitoring:  Second Stage hit, showing R1" << endmsg;
         DataLog(log_level_proc_alarm_monitor_info) << "AcPrimeInlet Monitoring:  Snapshot(" << _ApsSnapshot
                                                    << "), Current(" << _ApsCurrent << ")" << endmsg;
         _ACOcclusionAlarmMsg.setAlarm();
      }
   }
}

/* FORMAT HASH 6807ffd070c088320c0ee5e7b63af916 */
