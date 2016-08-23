/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cent_alarm.cpp
 *
 */

#include "cent_alarm.h"
#include <math.h>
#include "procdata.h"
#include "cobeconfig.h"

const int CENTRIFUGE_COMMAND_VOLT_FILTER = 20;

DEFINE_OBJ(CentAlarm);

CentAlarm::CentAlarm()
   : _CentSpeedTimer(0, Callback<CentAlarm>(this, &CentAlarm::centNotUpToSpeed), TimerMessage::DISARMED),
     _CentErrAlarm(CENTRIFUGE_HARDWARE_FAILURE),
     _CentIntegralAlarm(CENTRIFUGE_INTEGRAL_FAILURE),
     _CentOverSpeedAlarm(HIGH_CENTRIFUGE_COMMAND),
     _CentVoltOffError(0.0),
     _IntegrationErrorReported(0),
     _CurrentCentCommand(0.0f),
     _CentSpeedTimerStarted(0)
{}

CentAlarm::~CentAlarm()
{}

void CentAlarm::Monitor ()
{
   ProcData pd;

   //
   // Someone commanded to centrifuge to a value out
   //  of range ...
   //
   if ( pd.Status().CmdCentrifugeRPM() > CobeConfig::data().CentSpeedMax + 1.0f )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Cent Cmd Speed: " << pd.Status().CmdCentrifugeRPM()
                                                 << ", Max: " << CobeConfig::data().CentSpeedMax << endmsg;
      _CentOverSpeedAlarm.setAlarm();
   }

   //
   // If we are trying to command the centrifuge, but have no power ...
   //
   if ( pd.Status().CmdCentrifugeRPM() >= CobeConfig::data().CentrifugeMinCommandedRunSpeed &&
        !pd.Status().PS64Vsw.Acceptable() )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Power Problem" << endmsg;
      _CentVoltOffError++;
   }
   else
   {
      _CentVoltOffError = 0;
   }

   //
   // We have been without power for too many samples ...
   //
   if ( _CentVoltOffError > CENTRIFUGE_COMMAND_VOLT_FILTER)
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Centrifuge ERROR set by power off too long." << endmsg;
      _CentErrAlarm.setAlarm();
      _CentVoltOffError = 0;
   }

   //
   // Hardware error ...
   //
   if ( pd.Status().CentrifugeError() )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Centrifuge ERROR detected." << endmsg;
      _CentErrAlarm.setAlarm();
   }

   //
   // Centrifuge can't keep up with the command ...
   //
   if ( pd.Status().CentrifugeIntError() &&
        !_IntegrationErrorReported )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Centrifuge Integration ERROR detected." << endmsg;
      _IntegrationErrorReported = 1;
   }

   //
   // Check actual vs. commanded cent speed ...
   //
   checkActualVsCommand();

}

void CentAlarm::centNotUpToSpeed ()
{
   _CentIntegralAlarm.setAlarm();
   _CentSpeedTimerStarted = 0;
   _CentSpeedTimer.interval(0);
}

void CentAlarm::checkActualVsCommand ()
{
   ProcData pd;
   if ( _CurrentCentCommand != pd.Status().CmdCentrifugeRPM() )
   {
      //
      // Reset the timer, this is a new command ...
      //
      _CurrentCentCommand    = pd.Status().CmdCentrifugeRPM();
      _CentSpeedTimerStarted = 0;
      _CentSpeedTimer.interval(0);
   }

   if ( //
        // The centrifuge has been commanded to a runable speed ...
      pd.Status().CmdCentrifugeRPM() >= CobeConfig::data().CentrifugeMinCommandedRunSpeed &&
      //
      // The centrifuge speed is out of range on either side ...
      ( pd.Status().CmdCentrifugeRPM() - CobeConfig::data().CentrifugeSpeedError >
        pd.Status().CentrifugeRPM() ||
        pd.Status().CmdCentrifugeRPM() + CobeConfig::data().CentrifugeSpeedError <
        pd.Status().CentrifugeRPM() )
      )
   {
      if ( !_CentSpeedTimerStarted )
      {
         //
         // Start the timer ...
         //  ( Commanded RPMs * 1 sec/10 RPMs * 1000 msec/sec )
         _CentSpeedTimer.interval( (long)pd.Status().CmdCentrifugeRPM() * 100);
         _CentSpeedTimerStarted = 1;
      }
   }
   else
   {
      //
      // We are up to speed ...
      _CentSpeedTimerStarted = 0;
      _CentSpeedTimer.interval(0);
   }
}

/* FORMAT HASH 2d9f75063bac688955d91f3380a0786c */
