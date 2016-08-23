/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pause_alarm.h
 *
 */


#include "pause_alarm.h"
#include "proctime_cds.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_OBJ(PauseAlarm);

PauseAlarm::PauseAlarm()
   : _OneMinuteAlert(ONE_MINUTE_PAUSE),
     _ThreeMinuteAlert(MAINTAIN_DONOR_ACCESS_ALERT),
     _TenMinuteAlert(TEN_MINUTE_PAUSE),
     _TenMinuteAlarm(TEN_MINUTE_PAUSE_ALARM),
     _OneMinuteTimer(0, Callback<PauseAlarm>(this, &PauseAlarm::pumpsPausedOneMinute), TimerMessage::DISARMED),
     _ThreeMinuteTimer(0, Callback<PauseAlarm>(this, &PauseAlarm::pumpsPausedThreeMinutes), TimerMessage::DISARMED),
     _TenMinuteTimer(0, Callback<PauseAlarm>(this, &PauseAlarm::pumpsPausedTenMinutes), TimerMessage::DISARMED),
     _EnableTimer(0, Callback<PauseAlarm>(this, &PauseAlarm::alarmEnabled), TimerMessage::DISARMED),
     _ACBloodAtNeedle(0),
     _TimersStarted(0),
     _OneMinutePassed(0),
     _ThreeMinutesPassed(0),
     _TenMinutesPassed(0)
{}


PauseAlarm::~PauseAlarm()
{}

void PauseAlarm::Monitor ()
{
   if ( !enableMonitoring() )
      return;

   ProcData pd;

   //
   // If the set has already been changed to the used state (or later), we
   //  don't need to monitor for clotting anymore because rinseback
   //  has already completed ...
   //
   if ( pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::USED )
   {
      clearTimers();
      return;
   }

   //
   // If the pumps are paused ...
   //
   if ( ( pd.Status().InletPump.ActFlow() == 0.0f ||
          pd.Status().InletPump.CmdFlow() == 0.0f ) &&
        ( pd.Status().ReturnPump.ActFlow() == 0.0f ||
          pd.Status().ReturnPump.CmdFlow() == 0.0f )
        )

   {
      if ( !_TimersStarted )
      {
         unsigned elapsed = 0;
         if ( pd.Run().StartPauseTime.Get().tv_sec == 0 )
         {
            struct timespec temptime;
            clock_gettime(CLOCK_REALTIME, &temptime);
            pd.MakeRunWritable();
            pd.Run().StartPauseTime.Set(temptime);
            pd.MakeRunReadable();

            DataLog (log_level_proc_alarm_monitor_info) << "pd.Run().StartPauseTime.Get().tv_sec :"
                                                        << pd.Run().StartPauseTime.Get().tv_sec << endmsg;
         }
         else
         {
            struct timespec current;
            clock_gettime(CLOCK_REALTIME, &current);

            if ( current.tv_sec > pd.Run().StartPauseTime.Get().tv_sec )
            {
               elapsed = 1000 * ( current.tv_sec - pd.Run().StartPauseTime.Get().tv_sec );
            }

            DataLog (log_level_proc_alarm_monitor_info) << "current.tv_sec :" << current.tv_sec << endmsg;
            DataLog (log_level_proc_alarm_monitor_info) << "pd.Run().StartPauseTime.Get().tv_sec :"
                                                        << pd.Run().StartPauseTime.Get().tv_sec << endmsg;
         }

         if ( elapsed >= 60000 )
            _OneMinutePassed = 1;
         else
            _OneMinuteTimer.interval(60000 - elapsed);        //  60000 milliseconds = 1 minute

         if ( elapsed >= 180000 )
            _ThreeMinutesPassed = 1;
         else
            _ThreeMinuteTimer.interval(180000  - elapsed);    // 180000 milliseconds = 3 minutes

         if ( elapsed >= 570000 )
            _TenMinutesPassed = 1;
         else
            _TenMinuteTimer.interval(570000  - elapsed);      // 570000 milliseconds = 9.5 minutes

         _TimersStarted = 1;

         DataLog(log_level_proc_alarm_monitor_info) << " elapsed time " << elapsed << endmsg;
         DataLog(log_level_proc_alarm_monitor_info) << " Pumps found to be paused -- timers started" << endmsg;
         DataLog (log_level_proc_alarm_monitor_info) << " min times up :" << _OneMinutePassed << _ThreeMinutesPassed <<  _TenMinutesPassed << endmsg;
      }
   }
   //
   // We are not paused any more ...
   //
   else if ( ( pd.Status().InletPump.ActFlow() > 0.0f                                             //  ... Inlet pump is moving any at all
               && pd.Status().InletPump.CmdFlow() > 0.0f )                                        //       and is commanded to move ...
             || ( fabs(pd.Status().ReturnPump.ActFlow() ) > 0.0f                                  // ... Return pump is moving at all
                  && ( ( fabs(pd.Status().ReturnPump.CmdFlow() ) > CobeConfig::data().QrpAKO      // ... Return pump commanded greater than
                         && pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::RINSEBACK ) ) ) //      AKO speed during the run ...
             || ( pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::RINSEBACK                //      during rinseback ...
                  && !pd.AlarmActive() )                                                          //  ... and not in an alarm condition
             )
   {
      if ( _TimersStarted )
      {
         DataLog(log_level_proc_alarm_monitor_info) << "Pumps found to be running -- Timers stopped" << endmsg;
      }

      clearTimers();
   }

   //
   // Check to see if there is anti-coagulated blood at the needle ...
   //
   _ACBloodAtNeedle = 0;
   if ( pd.Volumes().VnonACBloodAtNeedle.Get() <= 0.0f )
   {
      _ACBloodAtNeedle = 1;

      DataLog(log_level_proc_debug) << "AC Blood at the needle" << endmsg;
   }

   //
   // Set the alarm if the timers have expired ...
   //
   if ( _OneMinutePassed )
   {
      _OneMinutePassed = 0;

      //
      // Only set the one minute alert if the centrifuge
      //  is commanded to run greater than the one minute pause speed ...
      //
      if ( pd.Status().CmdCentrifugeRPM() > CobeConfig::data().CentrifugePumpsPausedSpeed )
      {
         _OneMinuteAlert.setAlarm();
      }
   }

   //
   // Set the three minute alert whether or not
   //  un-anti-coagulated blood is at the needle ...
   //
   if (    _ThreeMinutesPassed
           && pd.Status().ReservoirLowSensorCovered()
           && (    pd.SystemState() == BLOOD_RUN
                   || pd.SystemState() == BLOOD_RINSEBACK )
           )
   {
      DataLog (log_level_proc_info) << "Sending new AKO alarm." << endmsg;
      pd.NewAKO(true);
      _ThreeMinutesPassed = 0;
      _ThreeMinuteAlert.setAlarm();
   }

   if ( _TenMinutesPassed
        && (    pd.SystemState() == BLOOD_RUN
                || pd.SystemState() == BLOOD_RINSEBACK )
        )
   {
      _TenMinutesPassed = 0;
      //
      // If anti-coagulated blood is in the needle ...
      //
      if ( _ACBloodAtNeedle )
      {
         DataLog(log_level_proc_info) << "_ACBloodAtNeedle  " << _ACBloodAtNeedle << endmsg;
         //
         // Only set the ten minute alert if the centrifuge
         //  is still running ...
         //
         if ( pd.Status().CentrifugeRPM() > 0.0f &&
              pd.Status().CmdCentrifugeRPM() > 0.0f )
         {
            _TenMinuteAlert.setAlarm();
         }
      }
      //
      // Possible clots could be forming in the needle,
      //  set the alarm ...
      //
      else
      {
         DataLog(log_level_proc_info) << "_ACBloodAtNeedle  " << _ACBloodAtNeedle << endmsg;
         _TenMinuteAlarm.setAlarm();
      }
   }
}

void PauseAlarm::enable ()
{
   enableMonitoring(0);
   _EnableTimer.interval(2000);   // 2000 millisec = 2 seconds
}

void PauseAlarm::disable ()
{
   MonitorBase::disable();
   clearTimers();
}

void PauseAlarm::pumpsPausedOneMinute ()
{
   _OneMinutePassed = 1;
   _OneMinuteTimer.interval(0);
}

void PauseAlarm::pumpsPausedThreeMinutes ()
{
   _ThreeMinutesPassed = 1;
   _ThreeMinuteTimer.interval(0);
}

void PauseAlarm::pumpsPausedTenMinutes ()
{
   _TenMinutesPassed = 1;
   _TenMinuteTimer.interval(0);
}

void PauseAlarm::alarmEnabled ()
{
   MonitorBase::enable();
}

void PauseAlarm::clearTimers ()
{
   const struct timespec zerotime = {0, 0};
   ProcData              pd;
   pd.MakeRunWritable();

   pd.Run().StartPauseTime.Set(zerotime);

   _OneMinutePassed    = 0;
   _OneMinuteTimer.interval(0);
   _ThreeMinutesPassed = 0;
   _ThreeMinuteTimer.interval(0);
   _TenMinutesPassed   = 0;
   _TenMinuteTimer.interval(0);
   _TimersStarted      = 0;

   _EnableTimer.interval(0);
}

/* FORMAT HASH a4a3fa02a55f36d980ad9d555b234748 */
