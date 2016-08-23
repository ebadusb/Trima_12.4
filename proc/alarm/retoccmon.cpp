/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/alarm/rcs/retoccmon.cpp 1.13 2003/05/02 19:58:46Z jl11312 Exp rm70006 $
 * $Log: retoccmon.cpp $
 * Revision 1.13  2003/05/02 19:58:46Z  jl11312
 * - allow retry on initial retoccmon task response
 * - additional logging of retoccmon status
 * Revision 1.12  2003/04/24 17:32:47Z  jl11312
 * - increased queue size (see IT 5700, 6022)
 * Revision 1.11  2003/04/04 21:03:46Z  ms10234
 * 5818 - Updated logging for new standard log levels
 * Revision 1.10  2002/10/28 23:30:41Z  ms10234
 * Changed proc files to use interface class - ProcData
 * Revision 1.9  2002/06/21 21:22:43  sb07663
 * Mods to remove dynamic data and implement CDS
 * Revision 1.8  2002/06/05 14:18:36  sb07663
 * Modifications for compatibility with new Vx messaging system
 * Revision 1.7  2002/04/09 17:11:58  sb07663
 * Changes to make compatible with VxWorks port
 * Revision 1.6  2002/03/29 21:55:58  sb07663
 * added template declarations to callback for new callback.h
 * Revision 1.5  2002/03/22 19:16:04  sb07663
 * Initial cut at logging replacement for VxWorks port
 * Revision 1.4  2002/03/05 20:35:54  jl11312
 * - additional logging for pump RPM ranges during data collection
 * Revision 1.3  2002/01/09 21:22:04  jl11312
 * - disable occlusion alarm for very close RPM match (within 1.5 RPM)
 * Revision 1.2  2002/01/09 15:38:40  jl11312
 * - added status check for return line occlusion monitor task
 * - changed handling of APS data to allow for monitoring of return cycles < 12 seconds
 * Revision 1.1  2001/12/21 19:04:56  jl11312
 * Initial revision
 *
 * TITLE:      Return Line Occlusion Monitoring
 *
 * ABSTRACT:   This program uses the high speed APS data to monitor for the
 *             pressure signature of a return line occlusion.
 */

#include "cobeconfig.h"
#include "retoccmon.h"
#include "stmain.h"
#include <math.h>
#include <stdlib.h>

static const double PI = 3.1415926535897932384626433832795;

//
// Minimum pump RPM to consider a pump running
//
static const double MinPumpRPM = 10.0;

//
// Maximum pump RPM that can be monitored.  This is a function
// of the sample rate for APS data (1/0.020 samples/second =
// 60.0/0.020 samples/minute).  The maximum monitor frequency is
// then 1/2 of this value, but since the pumps have two rotors, the
// maximum RPM is reduced by an additional factor of 2.
//
static const double APSMonitorInterval = 0.020;
static const double MaxMonitorRPM      = 60.0 / APSMonitorInterval / 4.0;

//
// The difference in pump RPM that we can distinguish with frequency
// sampling is determined by the maximum monitor RPM (see above)
// and the number of samples.
//
static const double DiffPumpRPM = 3.0 * MaxMonitorRPM / ReturnOccMon::SampleSize;

//
// Main function declaration
//
MAIN2(ReturnOccMon, 200);

ReturnOccMon::ReturnOccMon()
   : CStdAppl(),
     _APSFastDataMsg(0),
     _HalStatus(ROLE_RO),
     _ReturnOcclusionAlarm(RETURN_LINE_OCCLUSION),
     _ReturnOcclusionAlarmA1(RETURN_LINE_OCCLUSION_A1),
     _MonitorEnabled(0),
     _MonitorState(OCCMON_IDLE),
     _ThresholdCount(0),
     _SampleBufferIndex(0),
     _ReturnRPM(0.0),
     _InletRPM(0.0),
     _StateStartTime(0.0),
     _SampleInletRPMTotal(0.0),
     _MinSampleInletRPM(0.0),
     _MaxSampleInletRPM(0.0),
     _SampleReturnRPMTotal(0.0),
     _MinSampleReturnRPM(0.0),
     _MaxSampleReturnRPM(0.0),
     _SampleRPMCount(0),
     _AnalyzeCount(0),
     _AlarmHandlerStatusCheck(Callback<ReturnOccMon>(this, &ReturnOccMon::alarmHandlerStatusCheck), MessageBase::SNDRCV_LOCAL),
     _trimasetcds(ROLE_RO),
     _timecds(ROLE_RO)
{}

ReturnOccMon::~ReturnOccMon()
{}

ALARM_VALUES ReturnOccMon::Init (void)
{
   int i;

   //
   // Pre-compute sine and cosine values that will be needed later
   //
   _CosTable[0] = _SinTable[0] = NULL;
   for (i = 1; i<TrigTableEntries; i++)
   {
      _CosTable[i] = new double[TrigTableSize];
      _SinTable[i] = new double[TrigTableSize];

      for (int j = 0; j<TrigTableSize; j++)
      {
         _CosTable[i][j] = cos(2 * PI * i * j / SampleSize);
         _SinTable[i][j] = sin(2 * PI * i * j / SampleSize);
      }
   }

   _SmallCosTable[0] = _SmallSinTable[0] = NULL;
   for (i = 1; i<SmallTrigTableEntries; i++)
   {
      _SmallCosTable[i] = new double[SmallTrigTableSize];
      _SmallSinTable[i] = new double[SmallTrigTableSize];

      for (int j = 0; j<SmallTrigTableSize; j++)
      {
         _SmallCosTable[i][j] = cos(2 * PI * i * j / SmallSampleSize);
         _SmallSinTable[i][j] = sin(2 * PI * i * j / SmallSampleSize);
      }
   }

   //
   // Register for status check message from alarm handler
   //
   _AlarmHandlerStatusCheck.send(1);
   DataLog(log_level_proc_info) << "retoccmon initial status sent" << endmsg;

   return NULL_ALARM;
}

void ReturnOccMon::UpdateMonitorState ()
{
   if ( _trimasetcds.CassetteState.Get() >= ProcTrimaSet_CDS::PRIMED &&
        _trimasetcds.CassetteState.Get() < ProcTrimaSet_CDS::RINSEBACK )
   {
      if (!_MonitorEnabled)
      {
         Enable();
      }
   }
   else
   {
      if (_MonitorEnabled)
      {
         Disable();
      }
   }

   if (!_MonitorEnabled) return;

   double newReturnRPM = _HalStatus.ReturnPump.ActRPM();
   double newInletRPM  = _HalStatus.InletPump.ActRPM();

   switch ( _MonitorState )
   {
      case OCCMON_IDLE :
         //
         // Wait for indication that either a draw or return cycle is
         // starting, then switch to appropriate state
         //
         if ( newInletRPM >= MinPumpRPM &&
              newInletRPM <= MaxMonitorRPM &&
              newReturnRPM == 0.0f )
         {
            DataLog(log_level_proc_alarm_monitor_info) << "switching to OCCMON_DRAW_START " << (int)_MonitorState << endmsg;
            _MonitorState   = OCCMON_DRAW_START;
            _ReturnRPM      = 0.0;
            _InletRPM       = newInletRPM;
            _LastInletCount = 0;

            _StateStartTime = _timecds.AbsTime();
         }
         else if ( newReturnRPM >= MinPumpRPM &&
                   newReturnRPM <= MaxMonitorRPM &&
                   newInletRPM <= MaxMonitorRPM )
         {
            DataLog(log_level_proc_alarm_monitor_info) << "switching to OCCMON_RETURN_START " << (int)_MonitorState << endmsg;

            _MonitorState   = OCCMON_RETURN_START;
            _ReturnRPM      = newReturnRPM;
            _InletRPM       = newInletRPM;
            _StateStartTime = _timecds.AbsTime();
         }

         break;

      case OCCMON_DRAW_START :
      case OCCMON_DRAW_DATA :
         //
         // If the pump RPM has changed, or if it looks like a return
         // cycle has started, then give up monitoring this draw cycle.
         // If we are still in the draw cycle, OCCMON_IDLE will send us
         // back here with the new pump RPM values.
         //
         if ( fabs(newInletRPM - _InletRPM) > DiffPumpRPM ||
              newReturnRPM > MinPumpRPM )
         {
            DataLog(log_level_proc_alarm_monitor_info) << "switching to OCCMON_IDLE " << (int)_MonitorState << endmsg;
            _MonitorState = OCCMON_IDLE;
         }
         else if ( _MonitorState == OCCMON_DRAW_START &&
                   _timecds.AbsTime() - _StateStartTime > CobeConfig::data().RetOccStabilizationDelay / 60.0 )
         {
            DataLog(log_level_proc_alarm_monitor_info) << "switching to OCCMON_DRAW_DATA " << (int)_MonitorState << endmsg;

            _SampleBufferIndex    = 0;
            _SampleSum            = 0;
            _SampleRPMCount       = 1;
            _InletRPM             = newInletRPM;
            _ReturnRPM            = 0.0;
            _SampleInletRPMTotal  = _MinSampleInletRPM = _MaxSampleInletRPM = _InletRPM;
            _SampleReturnRPMTotal = _MinSampleReturnRPM = _MaxSampleReturnRPM = _ReturnRPM;
            _AnalyzeCount         = 0;
            _MonitorState         = OCCMON_DRAW_DATA;
         }
         else
         {
            _InletRPM  = newInletRPM;
            _ReturnRPM = 0.0;
         }

         break;

      case OCCMON_RETURN_START :
      case OCCMON_RETURN_DATA :
         //
         // If the pump RPM has changed, or if it looks like a draw
         // cycle has started, then give up monitoring this return cycle.
         // If we are still in the return cycle, OCCMON_IDLE will send us
         // back here with the new pump RPM values.
         //
         if ( fabs(newInletRPM - _InletRPM) > DiffPumpRPM ||
              fabs(newReturnRPM - _ReturnRPM) > DiffPumpRPM )
         {
            if ( _MonitorState == OCCMON_RETURN_DATA &&
                 _AnalyzeCount == 0 &&
                 _ReturnRPM >= 120.0 &&
                 _SampleBufferIndex >= SmallSampleSize )
            {
               DataLog(log_level_proc_alarm_monitor_info) << "using small sample buffer index=" << _SampleBufferIndex << endmsg;
               CheckForOcclusion(SmallSampleSize);
            }

            DataLog(log_level_proc_alarm_monitor_info) << "switching to OCCMON_IDLE " << (int)_MonitorState << endmsg;
            _MonitorState = OCCMON_IDLE;
         }
         else if ( _MonitorState == OCCMON_RETURN_START &&
                   _timecds.AbsTime() - _StateStartTime > CobeConfig::data().RetOccStabilizationDelay / 60.0 )
         {
            DataLog(log_level_proc_alarm_monitor_info) << "switching to OCCMON_RETURN_DATA " << (int)_MonitorState << endmsg;

            _SampleBufferIndex    = 0;
            _SampleSum            = 0;
            _SampleRPMCount       = 1;
            _InletRPM             = newInletRPM;
            _ReturnRPM            = newReturnRPM;
            _SampleInletRPMTotal  = _MinSampleInletRPM = _MaxSampleInletRPM = _InletRPM;
            _SampleReturnRPMTotal = _MinSampleReturnRPM = _MaxSampleReturnRPM = _ReturnRPM;
            _AnalyzeCount         = 0;
            _MonitorState         = OCCMON_RETURN_DATA;
         }
         else
         {
            _InletRPM  = newInletRPM;
            _ReturnRPM = newReturnRPM;
         }

         break;
   }
}

void ReturnOccMon::Enable (void)
{
   DataLog(log_level_proc_alarm_monitor_info) << "enable alarm monitoring " << (int)_MonitorState << endmsg;

   _MonitorEnabled = 1;
   if ( !_APSFastDataMsg )
   {
      //
      // Register for the message ...
      //
      _APSFastDataMsg = new Message< FASTDATA >(Callback<ReturnOccMon>(this, &ReturnOccMon::newAPSData) );
   }
}

void ReturnOccMon::Disable (void)
{
   DataLog(log_level_proc_alarm_monitor_info) << "disable alarm monitoring " << (int)_MonitorState << endmsg;

   _MonitorEnabled = 0;
   if ( _APSFastDataMsg )
   {
      delete _APSFastDataMsg;
      _APSFastDataMsg = NULL;
   }
}

void ReturnOccMon::newAPSData ()
{
   FASTDATA fastdata;

   //
   // Get the message data
   //
   fastdata = _APSFastDataMsg->getData();

   //
   // Check the message source to see if this
   // data comes from the Access Pressure Sensor
   //
   if ( fastdata.source == CONTROL_ACCESS_PRESSURE &&
        ( _MonitorState == OCCMON_DRAW_DATA ||
          _MonitorState == OCCMON_RETURN_DATA ) )
   {
      int copyCount = FASTDATALENGTH;
      if ( _SampleBufferIndex + copyCount > SampleSize )
      {
         copyCount = SampleSize - _SampleBufferIndex;
      }

      _SampleRPMCount       += 1;
      _SampleInletRPMTotal  += _InletRPM;
      _SampleReturnRPMTotal += _ReturnRPM;
      if ( _InletRPM < _MinSampleInletRPM) _MinSampleInletRPM = _InletRPM;
      if ( _InletRPM > _MaxSampleInletRPM) _MaxSampleInletRPM = _InletRPM;
      if ( _ReturnRPM < _MinSampleReturnRPM) _MinSampleReturnRPM = _ReturnRPM;
      if ( _ReturnRPM > _MaxSampleReturnRPM) _MaxSampleReturnRPM = _ReturnRPM;

      memcpy(&_SampleBuffer[_SampleBufferIndex], fastdata.raw, sizeof(short) * copyCount);
      _SampleBufferIndex += copyCount;
      for ( int i = 0; i<copyCount; i++ )
      {
         _SampleSum += fastdata.raw[i];
      }

      if ( _SampleBufferIndex >= SampleSize )
      {
         CheckForOcclusion(SampleSize);

         _SampleBufferIndex    = 0;
         _SampleSum            = 0;
         _SampleRPMCount       = 1;
         _SampleInletRPMTotal  = _MinSampleInletRPM = _MaxSampleInletRPM = _InletRPM;
         _SampleReturnRPMTotal = _MinSampleReturnRPM = _MaxSampleReturnRPM = _ReturnRPM;
         _AnalyzeCount        += 1;
      }
   }
}

void ReturnOccMon::CheckForOcclusion (int currentSampleSize)
{
   int    inletBin[3], returnBin[2];
   double inletPower[3], returnPower, totalPower, totalHarmonicPower;
   double avgInletRPM  = _SampleInletRPMTotal / _SampleRPMCount;
   double avgReturnRPM = _SampleReturnRPMTotal / _SampleRPMCount;

   _SampleAverage = (short)(_SampleSum / (double)_SampleBufferIndex + 0.5);
   DataLog(log_level_proc_alarm_monitor_info).flags(ios::showpoint | ios::fixed);
   DataLog(log_level_proc_alarm_monitor_info).precision(1);
   DataLog(log_level_proc_alarm_monitor_info) << "CheckForOcclusion: inlet RPM(" << _SampleRPMCount
                                              << ") = " << avgInletRPM << "(" << _MinSampleInletRPM << " " << _MaxSampleInletRPM
                                              << ") return RPM = " << avgReturnRPM << "(" << _MinSampleReturnRPM << " " << _MaxSampleReturnRPM
                                              << ") sample count = " << _SampleRPMCount
                                              << endmsg;

   inletBin[0]   = (int)(avgInletRPM * APSMonitorInterval * currentSampleSize / 30.0 + 0.5);
   inletBin[1]   = (int)(2 * avgInletRPM * APSMonitorInterval * currentSampleSize / 30.0 + 0.5);
   inletBin[2]   = (int)(3 * avgInletRPM * APSMonitorInterval * currentSampleSize / 30.0 + 0.5);
   inletPower[0] = inletPower[1] = inletPower[2] = 0.0;
   returnBin[0]  = (int)(avgReturnRPM * APSMonitorInterval * currentSampleSize / 30.0 + 0.5);
   returnBin[1]  = (int)(2 * avgReturnRPM * APSMonitorInterval * currentSampleSize / 30.0 + 0.5);
   returnPower   = 0.0;
   totalPower    = totalHarmonicPower = 0.0;

   if ( _MonitorState == OCCMON_DRAW_DATA )
   {
      //
      // Handle draw cycle computations.  This consists of computing the
      // inlet power at the fundamental and first and second harmonics for
      // logging purposes.  The power at the fundamental is saved for possible
      // use in the subsequent return cycle.
      //
      for (int i = 1; i<TrigTableEntries; i++)
      {
         if ( i>=inletBin[0] - 2 && i<=inletBin[0] + 2 )
         {
            //
            // Fundamental frequency
            //
            inletPower[0] += GetSqrBinPower(i, currentSampleSize);
         }
         else if ( i>=inletBin[1] - 2 && i<=inletBin[1] + 2 )
         {
            //
            // First harmonic frequency
            //
            inletPower[1] += GetSqrBinPower(i, currentSampleSize);
         }
         else if ( i>=inletBin[2] - 2 && i<=inletBin[2] + 2 )
         {
            //
            // Second harmonic frequency
            //
            inletPower[2] += GetSqrBinPower(i, currentSampleSize);
         }
      }

      _LastInletRPM = (int)avgInletRPM;
      if ( _LastInletCount == 0 )
      {
         _LastInletCount = 1;
         _LastInletPower = inletPower[0];
      }
      else
      {
         _LastInletCount += 1;
         _LastInletPower += inletPower[0];
      }

      DataLog(log_level_proc_alarm_monitor_info) << "CheckForOcclusion: draw power " << sqrt(inletPower[0]) << "(" << inletBin[0] << ") "
                                                 << sqrt(inletPower[1]) << "(" << inletBin[1] << ") "
                                                 << sqrt(inletPower[2]) << "(" << inletBin[2] << ")"
                                                 << endmsg;
   }
   else if ( _MonitorState == OCCMON_RETURN_DATA )
   {
      enum OverlapType { OT_NONE, OT_FUNDAMENTAL, OT_FIRST, OT_SECOND };
      OverlapType oType = OT_NONE;

      //
      // Determine overlap status.  If the return frequency overlaps with the inlet frequency or
      // one of its harmonics, the inlet power needs to be accounted for to determine the
      // power from the return pump.
      //
      if ( inletBin[0] >= returnBin[0] - 4 &&
           inletBin[0] <= returnBin[0] + 4 )
      {
         oType = OT_FUNDAMENTAL;
      }
      else if ( inletBin[1] >= returnBin[0] - 4 &&
                inletBin[1] <= returnBin[0] + 4 )
      {
         oType = OT_FIRST;
      }
      else if ( inletBin[2] >= returnBin[0] - 4 &&
                inletBin[2] <= returnBin[0] + 4 )
      {
         oType = OT_SECOND;
      }

      //
      // Compute return pump power.  Initially this will be summed with inlet power
      // which might overlap frequency bands with the return pump power.
      //
      for (int i = 1; i<TrigTableEntries; i++)
      {
         if ( i>=returnBin[0] - 2 && i<=returnBin[0] + 2 )
         {
            totalPower += GetSqrBinPower(i, currentSampleSize);
         }
         else if ( oType==OT_FUNDAMENTAL &&
                   i>=inletBin[0] - 2 && i<=inletBin[0] + 2 )
         {
            //
            // Fundamental frequency
            //
            totalPower += GetSqrBinPower(i, currentSampleSize);
         }
         else if ( oType==OT_FIRST &&
                   i>=inletBin[1] - 2 && i<=inletBin[1] + 2 )
         {
            //
            // First harmonic frequency
            //
            totalPower += GetSqrBinPower(i, currentSampleSize);
         }
         else if ( oType==OT_SECOND &&
                   i>=inletBin[2] - 2 && i<=inletBin[2] + 2 )
         {
            //
            // Second harmonic frequency
            //
            totalPower += GetSqrBinPower(i, currentSampleSize);
         }

         if ( i>=returnBin[1] - 2 && i<=returnBin[1] + 2 )
         {
            totalHarmonicPower += GetSqrBinPower(i, currentSampleSize);
         }
      }

      //
      // We can only proceed if either overlap is not present, or we have inlet
      // pump data from the previous pump cycle to use in computing the inlet
      // contribution to the computed power.
      //
      if ( oType == OT_NONE ||
           _LastInletCount != 0 )
      {
         if ( _LastInletCount >= 1 )
         {
            _LastInletPower /= (double)_LastInletCount;
            _LastInletCount  = 1;
         }

         switch (oType)
         {
            case OT_FUNDAMENTAL :
               returnPower = totalPower - _LastInletPower * FreqRespSqrInletScale(_LastInletRPM, avgInletRPM);
               break;

            case OT_FIRST :
               returnPower = totalPower - _LastInletPower * FreqRespSqrInletScale(_LastInletRPM, avgInletRPM) * CobeConfig::data().RetOccInletFirstHarmonicScale;
               break;

            case OT_SECOND :
               returnPower = totalPower - _LastInletPower * FreqRespSqrInletScale(_LastInletRPM, avgInletRPM) * CobeConfig::data().RetOccInletSecondHarmonicScale;
               break;

            default :
               returnPower = totalPower;
               break;
         }

         //
         // Make sure computed power isn't negative (since we take the sqrt later)
         //
         if ( returnPower < 0.0 )
         {
            returnPower = 0.0;
         }

         //
         // Scale returnPower to a nominal 100 RPM
         //
         returnPower = sqrt(returnPower * FreqRespSqrReturnScale(avgReturnRPM, 100.0));
         DataLog(log_level_proc_alarm_monitor_info) << "CheckForOcclusion(" << currentSampleSize << "): return power " << sqrt(totalPower)
                                                    << "(" << returnBin[0] << "," << inletBin[0] << "," << inletBin[1] << "," << inletBin[2] << ") "
                                                    << sqrt(totalHarmonicPower) << "(" << returnBin[1] << ") " << returnPower << " "
                                                    << (int)oType << endmsg;

         //
         // Check for alarm
         //
         if ( returnPower < CobeConfig::data().RetOccAlarmPowerLimit )
         {
            int alarmShouldBeSet = 0;

            if ( fabs(avgInletRPM - avgReturnRPM) < 1.5 )
            {
               //
               // With very small difference in RPM, we don't get reliable power data.  Just log
               // that we detected a possible occlusion but don't generate the alarm.
               //
               DataLog(log_level_proc_alarm_monitor_info) << _ThresholdCount << " CheckForOcclusion: possible occlusion, alarm disable by RPM match" << endmsg;
            }
            else if ( fabs(avgInletRPM - avgReturnRPM) < 3.0 )
            {
               //
               // With only small differences in RPM, phase becomes important and can
               // skew the results.  To reduce false alarms under these conditions, two
               // consecutive observations must indicate occlusion
               //
               _ThresholdCount += 1;
               DataLog(log_level_proc_alarm_monitor_info) << _ThresholdCount << " CheckForOcclusion: possible occlusion, RPM overlap" << endmsg;
               if ( _ThresholdCount >= 2 )
               {
                  alarmShouldBeSet = 1;
               }
            }
            else
            {
               //
               // Occlusion detected with significant RPM difference - set the alarm
               // immediately.
               //
               alarmShouldBeSet = 1;
            }

            if ( alarmShouldBeSet )
            {
               DataLog(log_level_proc_alarm_monitor_info) << "CheckForOcclusion: occlusion detected" << endmsg;
               _ThresholdCount = 0;

               if ( CobeConfig::data().RetOccAlarmType == 1 )
               {
                  _ReturnOcclusionAlarm.setAlarm();
               }
               else if ( CobeConfig::data().RetOccAlarmType == 2 )
               {
                  _ReturnOcclusionAlarmA1.setAlarm();
               }
            }
         }
         else
         {
            //
            // No occlusion detected - reset threshold count
            //
            _ThresholdCount = 0;
         }
      }
   }
}

double ReturnOccMon::GetSqrBinPower (int bin, int currentSampleSize)
{
   double* cosData  = (currentSampleSize == SmallSampleSize) ? _SmallCosTable[bin] : _CosTable[bin];
   double* sinData  = (currentSampleSize == SmallSampleSize) ? _SmallSinTable[bin] : _SinTable[bin];
   short*  dataPtr1 = &_SampleBuffer[0];
   short*  dataPtr2 = &_SampleBuffer[currentSampleSize - 1];

   double  acc1     = 0.0;
   double  acc2     = 0.0;
   for ( int i = 0; i<currentSampleSize / 2; i++ )
   {
      acc1 += cosData[i] * (*dataPtr1 + *dataPtr2 - 2 * _SampleAverage);
      acc2 += sinData[i] * (*dataPtr1 - *dataPtr2);

      dataPtr1++;
      dataPtr2--;
   }

   double power = (acc1 * acc1 + acc2 * acc2) / (currentSampleSize * currentSampleSize);
   return power;
}

double ReturnOccMon::FreqRespSqrInletScale (double oldRPM, double newRPM)
{
   return FreqRespSqrInletMag(newRPM) / FreqRespSqrInletMag(oldRPM);
}

double ReturnOccMon::FreqRespSqrInletMag (double rpm)
{
   double value;

   if ( rpm < 10.0 )
   {
      value = 0.0;
   }
   else if ( rpm < 85.0 )
   {
      value = 0.4 * (rpm - 10.0) / 75.0;
   }
   else if ( rpm < 130.0 )
   {
      value = 0.4 + 0.6 * (rpm - 85.0) / (130.0 - 85.0);
   }
   else if ( rpm < 200.0 )
   {
      value = 1.0 - 0.1 * (rpm - 130.0) / (170.0 - 130.0);
   }
   else
   {
      value = 0.85;
   }

   return value * value;
}

double ReturnOccMon::FreqRespSqrReturnScale (double oldRPM, double newRPM)
{
   return FreqRespSqrReturnMag(newRPM) / FreqRespSqrReturnMag(oldRPM);
}

double ReturnOccMon::FreqRespSqrReturnMag (double rpm)
{
   double value;

   if ( rpm < 10.0 )
   {
      value = 0.0;
   }
   else if ( rpm < 65.0 )
   {
      value = 0.25 * (rpm - 10.0) / 55.0;
   }
   else if ( rpm < 130.0 )
   {
      value = 0.25 + 0.75 * (rpm - 65.0) / (130.0 - 65.0);
   }
   else
   {
      value = 1.0;
   }

   return value * value;
}

void ReturnOccMon::alarmHandlerStatusCheck ()
{
   if (!_AlarmHandlerStatusCheck.getData())
   {
      _AlarmHandlerStatusCheck.send(1);
   }
}

/* FORMAT HASH 0831736e8620af530b53453c0877180f */
