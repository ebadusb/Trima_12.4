/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      raisecassette.cpp
 *
 */

#include "raisecassette.h"
#include "periodic_log.h"
#include "procdata.h"

DEFINE_STATE(RaiseCassette);

RaiseCassette::RaiseCassette()
   : StateAbs(),
     _CassetteRaiseAlarm(CASSETTE_FAILURE),
     _RaiseTooLongTimer(0, Callback<RaiseCassette>(this, &RaiseCassette::raiseTooLong), TimerMessage::DISARMED),
     _CassetteUpTimer(0, Callback<RaiseCassette>(this, &RaiseCassette::cassetteClear), TimerMessage::DISARMED),
     _CassetteUpFlag(0),
     _CassetteClearFlag(0),
     _CassetteRaiseFailure(0),
     _TimerStarted(0)
{}

RaiseCassette::RaiseCassette(const RaiseCassette& state)
   : StateAbs(state),
     _CassetteRaiseAlarm(CASSETTE_FAILURE),
     _RaiseTooLongTimer(0, Callback<RaiseCassette>(this, &RaiseCassette::raiseTooLong), TimerMessage::DISARMED),
     _CassetteUpTimer(0, Callback<RaiseCassette>(this, &RaiseCassette::cassetteClear), TimerMessage::DISARMED),
     _CassetteUpFlag(state._CassetteUpFlag),
     _CassetteClearFlag(state._CassetteUpFlag),
     _CassetteRaiseFailure(state._CassetteRaiseFailure),
     _TimerStarted(state._TimerStarted)
{}

RaiseCassette::~RaiseCassette()
{}

int RaiseCassette::transitionStatus ()
{
   if ( _CassetteClearFlag == 1 )
   {
      PeriodicLog::enablePeriodicity(30.0);

      DataLog(log_level_proc_unload_cassette_info) << "Cassette raised" << endmsg;
      return 1;
   }
   if ( _CassetteRaiseFailure == 1 )
   {
      DataLog(log_level_proc_unload_cassette_error) << "Cassette raise failure" << endmsg;
      return 2;
   }
   return NO_TRANSITION;
}

void RaiseCassette::raiseTooLong ()
{
   //
   // Set the alarm ...
   //
   _CassetteRaiseAlarm.setAlarm();
   DataLog(log_level_proc_debug) << "Set cassette raise alarm" << CASSETTE_FAILURE << endmsg;

   //
   // Set the cassette raise failure flag ...
   //
   _CassetteRaiseFailure = 1;
}

void RaiseCassette::cassetteClear ()
{
   //
   // The cassette is clear, set the flag ...
   //
   _CassetteClearFlag = 1;
   DataLog(log_level_proc_debug) << "Cassette cleared" << endmsg;
}

int RaiseCassette::preProcess ()
{
   ProcData pd;
   //
   // Command the cassette to raise ...
   //
   startCommand();

   //
   // This state fails if the system
   //  becomes paused ...
   //
   if ( pd.AlarmActive() )
   {
      //
      // Set the failed flag ...
      //
      _CassetteRaiseFailure = 1;
   }

   return NORMAL;
}

int RaiseCassette::postProcess ()
{
   ProcData pd;
   if ( _CassetteUpFlag == 0 &&
        pd.Status().CassIsUp() )
   {
      //
      // Stop the raise timer ...
      //
      _RaiseTooLongTimer.interval(0);
      DataLog(log_level_proc_debug) << "Stop the raise too long timer" << endmsg;

      //
      // Set the flag to signify the cassette is raised ...
      //
      _CassetteUpFlag = 1;
      DataLog(log_level_proc_debug) << "Cassette seen up" << endmsg;

      //
      // Start the timer to allow the cassette to clear ...
      //
      _CassetteUpTimer.interval(2000);    // 2 seconds (2000 msecs)
      DataLog(log_level_proc_debug) << "Start the cassette up timer" << endmsg;
   }

   return NORMAL;
}

int RaiseCassette::preEnter ()
{
   return NORMAL;
}

int RaiseCassette::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   //
   // Stop the timers ...
   //
   _RaiseTooLongTimer.interval(0);
   _CassetteUpTimer.interval(0);
   DataLog(log_level_proc_debug) << "Stop the timers" << endmsg;

   ProcData pd;

   // if the cassette is coming up and we didnt do a run then reset RBCDetectorError flag for next try:
   // IT 8365
   if (  !(  pd.PQI().PlateletsWereCollected
             || pd.PQI().PlasmaWasCollected
             || pd.PQI().RbcWereCollected  )
         )
   {
      if  (pd.PQI().RBCDetectorError.Get() )
      {
         DataLog(log_level_proc_info) << "resetting the RBCDetectorError flag for next lowering" << endmsg;
         pd.MakePQIWritable();
         pd.PQI().RBCDetectorError.Set(false);
         pd.MakePQIReadable();
      }
   }

   return NORMAL;
}

void RaiseCassette::startCommand ()
{
   ProcData pd;

   //
   // Command the cassette to raise ...
   //
   pd.Orders().CassetteCmd(HAL_CASSETTEUP);
   DataLog(log_level_proc_debug) << "Command the cassette up " << HAL_CASSETTEUP << endmsg;

   if ( !_TimerStarted )
   {
      //
      // Start the cassette raise timer ...
      //
      _RaiseTooLongTimer.interval(15000);   // 15 seconds (15000 msecs)
      DataLog(log_level_proc_debug) << "Lower timer started" << endmsg;
      _TimerStarted = 1;
   }
}

void RaiseCassette::reset ()
{
   //
   // Reset the cassette clear and up flags
   //
   _CassetteUpFlag       = 0;
   _CassetteClearFlag    = 0;
   _CassetteRaiseFailure = 0;
   _TimerStarted         = 0;
}

void RaiseCassette::copyOver (const RaiseCassette&)
{}

void RaiseCassette::cleanup ()
{}

/* FORMAT HASH 803e3ea61f17db987464b14b875031ee */
