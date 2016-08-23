/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      lowercassette.cpp
 *
 */

#include <vxWorks.h>

#include "cobeconfig.h"
#include "filenames.h"
#include "lowercassette.h"
#include "periodic_log.h"
#include "procdata.h"
#include "caldat.h"
#include "autocaldat.h"


DEFINE_STATE(LowerCassette);

LowerCassette::LowerCassette()
   : StateAbs(),
     _LowerTooLongTimer(0, Callback<LowerCassette>(this, &LowerCassette::lowerTooLong), TimerMessage::DISARMED),
     _CompletelyLoweredTimer(0, Callback<LowerCassette>(this, &LowerCassette::completelyLowered), TimerMessage::DISARMED),
     _LowerTooLong(false),
     _CassetteLowered(false),
     _CassetteCompletelyLowered(false),
     _TimerStarted(false),
     _CassetteLowerFailure(false),
     _apsAverageCount(0),
     _apsAverageTotal(0),
     _apsAutoZeroComplete (false),
     _apsAlarm(NULL_ALARM),
     _apsStartReadDelay(0),
     _apsMin(0.0),
     _apsMax(0.0),
     _apsZeroOffset(0.0),
     _apsLastZeroOffset(0.0)
{}

LowerCassette::~LowerCassette()
{}

int LowerCassette::transitionStatus ()
{
   if (_CassetteLowerFailure != 0)
   {
      DataLog(log_level_proc_load_cassette_error) << "Cassette lower failure" << endmsg;
      return 2;
   }

   if (_CassetteCompletelyLowered && _apsAutoZeroComplete)
   {
      PeriodicLog::enablePeriodicity(6.0);
      DataLog(log_level_proc_load_cassette_info) << "Cassette lowered" << endmsg;
      return 1;
   }

   return NO_TRANSITION;
}

void LowerCassette::lowerTooLong ()
{
   //
   // Cassette took too long to lower ...
   //
   _LowerTooLong = true;
   DataLog(log_level_proc_load_cassette_error) << "Cassette lower took too long" << endmsg;
}

void LowerCassette::completelyLowered ()
{
   //
   // Cassette can be considered completely lowered ...
   //
   _CassetteCompletelyLowered = true;
   DataLog(log_level_proc_load_cassette_info) << "Cassette completely lowered" << endmsg;
}

int LowerCassette::preProcess ()
{
   ProcData pd;

   //
   // Send the command ...
   //
   if (_apsAlarm == NULL_ALARM && !_CassetteLowerFailure)
   {
      startCommand();
   }

   //
   // Average readings for the APS auto-zero
   //
   if (_apsStartReadDelay > 0)
      _apsStartReadDelay -= 1;

   if (!_apsStartReadDelay &&
       _apsAverageCount < CobeConfig::data().APSAutoZeroAverage)
   {
      _apsAverageCount += 1;
      _apsAverageTotal +=  pd.Status().APS();

      if (_apsAverageCount == 1)
      {
         _apsMin = _apsMax = pd.Status().APS();
      }
      else if (pd.Status().APS() < _apsMin)
      {
         _apsMin = pd.Status().APS();
      }
      else if (pd.Status().APS() > _apsMax)
      {
         _apsMax = pd.Status().APS();
      }

      if (_apsAverageCount >= CobeConfig::data().APSAutoZeroAverage)
      {
         _apsZeroOffset = _apsAverageTotal / _apsAverageCount;

         if (_apsMax - _apsMin > CobeConfig::data().APSAutoZeroRange)
         {
            DataLog(log_level_proc_load_cassette_error) << "APS auto-zero reading range exceeded: count=" << _apsAverageCount << " min=" << _apsMin << " max=" << _apsMax << endmsg;
            _apsAlarm = APS_AUTO_ZERO_FAILED_RANGE;
         }
         else if (_apsMax - _apsMin > CobeConfig::data().APSAutoZeroRange / 2.0)
         {
            DataLog(log_level_proc_load_cassette_error) << "APS auto-zero reading range warning: count=" << _apsAverageCount << " min=" << _apsMin << " max=" << _apsMax << endmsg;
         }

         if (_apsAlarm == NULL_ALARM &&
             ( _apsZeroOffset > CobeConfig::data().APSAutoZeroMax ||
               _apsZeroOffset > _apsLastZeroOffset + CobeConfig::data().APSAutoZeroMaxDiff ))
         {
            DataLog(log_level_proc_load_cassette_error) << "APS auto-zero offset out of range: " << _apsZeroOffset << endmsg;
            _apsAlarm = APS_AUTO_ZERO_FAILED_MAX;
         }

         if (_apsAlarm == NULL_ALARM &&
             ( _apsZeroOffset < CobeConfig::data().APSAutoZeroMin ||
               _apsZeroOffset < _apsLastZeroOffset - CobeConfig::data().APSAutoZeroMaxDiff ))
         {
            DataLog(log_level_proc_load_cassette_error) << "APS auto-zero offset out of range: " << _apsZeroOffset << endmsg;
            _apsAlarm = APS_AUTO_ZERO_FAILED_MIN;
         }

         if (_apsAlarm == NULL_ALARM)
         {
            DataLog(log_level_proc_load_cassette_info) << "APS auto-zero data: count=" << _apsAverageCount
                                                       << " avg=" << _apsZeroOffset << " min=" << _apsMin << " max=" << _apsMax << endmsg;

            // Inform HAL and Safety of new zero point for APS
            APSZeroMsg apsZeroMsg(MessageBase::SEND_GLOBAL);
            apsZeroMsg.send((int)(_apsZeroOffset + ((_apsZeroOffset >= 0) ? 0.5 : -0.5)));

            pd.MakeTrimaSetWritable();
            pd.TrimaSet().APSZero.Set(_apsZeroOffset);
            pd.MakeTrimaSetReadable();

            _apsAutoZeroComplete = true;
         }
      }
   }

   //
   // This state fails if the system
   //  becomes paused ...
   //
   if (pd.AlarmActive())
   {
      //
      // Set the failed flag ...
      //
      _LowerTooLong = true;
   }

   if (_LowerTooLong || (_apsAlarm != NULL_ALARM))
   {
      //
      // Set the alarm ...
      //
      if (_apsAlarm != NULL_ALARM)
      {
         //
         // Set alarm
         //
         PendingCassetteAlarmMsg delayedAlarm(MessageBase::SEND_LOCAL);
         delayedAlarm.send(_apsAlarm);
      }

      if (_LowerTooLong)
      {
         PendingCassetteAlarmMsg delayedAlarm(MessageBase::SEND_LOCAL);
         delayedAlarm.send(CASSETTE_FAILURE);
         DataLog(log_level_proc_debug) << "Set cassette lower alarm" << CASSETTE_FAILURE << endmsg;
      }

      //
      // Reset the flags ...
      //
      reset();

      _CassetteLowerFailure = 1;
   }

   return NORMAL;
}



int LowerCassette::postProcess ()
{
   ProcData pd;

   if (_CassetteLowered == 0 &&
       pd.Status().CassIsDown())
   {
      //
      // Set the cassette lowered flag ...
      //
      _CassetteLowered = true;
      DataLog(log_level_proc_load_cassette_info) << "Cassette reached the down position" << endmsg;

      //
      // Stop the too long timer ...
      //
      _LowerTooLongTimer.interval(0);
      _TimerStarted = 0;
      DataLog(log_level_proc_debug) << "Stop the lower too long timer" << endmsg;

      //
      // Start the completely lowered timer ...
      //
      _CompletelyLoweredTimer.interval(2000);    // 2 seconds (2000 msecs)
      DataLog(log_level_proc_debug) << "Start the completely lowered timer" << endmsg;

   }

   return NORMAL;
}


int LowerCassette::preEnter ()
{

   // just some sanity logging to see if AF is on at set load....
   ProcData pd;
   if (pd.Config().Procedure.Get().key_autoflow)
   {
      DataLog(log_level_proc_info) << "AutoFlow ON" << endmsg;
   }
   else
   {
      DataLog(log_level_proc_info) << "AutoFlow OFF" << endmsg;
   }



   // Reset any previous auto-zero data
   APSZeroMsg apsZeroMsg(MessageBase::SEND_GLOBAL);
   apsZeroMsg.send(0);

   if (!AutoCalDat::dataOK())
   {
      //
      // No previous autocal data - use default offset
      //
      _apsLastZeroOffset = 0.0;
   }
   else
   {
      _apsLastZeroOffset = AutoCalDat::lastZeroOffset();
   }

   DataLog(log_level_proc_load_cassette_info) << "Last autocal offset: " << _apsLastZeroOffset << endmsg;

   _apsStartReadDelay   = 3;
   _apsAutoZeroComplete = false;

   return NORMAL;
}

int LowerCassette::postExit ()
{
   //
   // Stop the timers ...
   //
   _LowerTooLongTimer.interval(0);
   _TimerStarted = 0;
   _CompletelyLoweredTimer.interval(0);

   //
   // Save auto cal offset if calibration was OK
   //
   if (_apsAutoZeroComplete)
   {
      if (!AutoCalDat::update(CalDat::data().APSRawReference, _apsZeroOffset))
      {
         DataLog(log_level_proc_load_cassette_error) << "Unable to process autocal file: " << AUTOCAL_FILE << endmsg;

         anAlarmMsg alrm(APS_AUTO_ZERO_FILE_ERROR);
         alrm.setAlarm("Unable to process autocal file");

         return PROCESSING_ERROR;
      }
   }

   return NORMAL;
}

void LowerCassette::startCommand ()
{
   ProcData pd;

   //
   // Command the cassette to lower ...
   //
   pd.Orders().CassetteCmd(HAL_CASSETTEDOWN);

   if (!_TimerStarted)
   {
      //
      // Start the cassette lower timer ...
      //
      _LowerTooLongTimer.interval(15000);   // 15 seconds (15000 msecs)
      DataLog(log_level_proc_debug) << "Lower timer started" << endmsg;
      _TimerStarted = true;
   }
}

void LowerCassette::reset ()
{
   //
   // Reset the flags ...
   //
   _LowerTooLong              = false;
   _CassetteLowered           = false;
   _CassetteCompletelyLowered = false;
   _TimerStarted              = false;
   _CassetteLowerFailure      = false;
   _apsAverageCount           = 0;
   _apsAverageTotal           = 0;
   _apsAlarm                  = NULL_ALARM;
}

/* FORMAT HASH 227962de7a916a6ad706eca2ea8cb507 */
