/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/HOME/BCT_Development/vxWorks/Trima/proc/alarm/rcs/retoccmon.h 1.9 2002/08/06 18:55:44 ms10234 Exp $
 * $Log: retoccmon.h $
 * Revision 1.9  2002/08/06 18:55:44  ms10234
 * Changed all int message names to end with "Msg"
 * Revision 1.8  2002/06/21 21:22:44  sb07663
 * Mods to remove dynamic data and implement CDS
 * Revision 1.7  2002/06/05 14:18:36  sb07663
 * Modifications for compatibility with new Vx messaging system
 * Revision 1.6  2002/04/09 17:11:58  sb07663
 * Changes to make compatible with VxWorks port
 * Revision 1.5  2002/03/29 20:10:31  sb07663
 * Revision 1.4  2002/03/22 19:16:04  sb07663
 * Initial cut at logging replacement for VxWorks port
 * Revision 1.3  2002/03/05 20:36:08  jl11312
 * - additional logging for pump RPM ranges during data collection
 * Revision 1.2  2002/01/09 15:38:43  jl11312
 * - added status check for return line occlusion monitor task
 * - changed handling of APS data to allow for monitoring of return cycles < 12 seconds
 * Revision 1.1  2001/12/21 19:04:54  jl11312
 * Initial revision
 *
 * TITLE:      retoccmon.h
 *
 * ABSTRACT:   This program uses the high speed APS data to monitor for the
 *             pressure signature of a return line occlusion.  Due to the
 *             relatively time consuming calculations involved, it runs as
 *             a stand-alone low-priority task.
 */


#ifndef _RET_OCC_MON_H_
#define _RET_OCC_MON_H_

#include <stddef.h>

#include "an_alarm.h"
#include "cobeconfig.h"
#include "fastdata.h"
#include "stappl.h"
#include "trimamessages.h"
#include "halstatus_cds.h"
#include "proctrimaset_cds.h"
#include "proctime_cds.h"

class ReturnOccMon
   : public CStdAppl
{

public:
   ReturnOccMon(void);
   virtual ~ReturnOccMon();

   //
   // functions required by standard application class
   //
   virtual ALARM_VALUES Init (void);

   //
   // Callback function when APS data is received from fastdata interface
   //
   void newAPSData ();

   //
   // Callback for status check from alarm handler
   //
   void alarmHandlerStatusCheck ();

   //
   // Number of samples of APS data collected during each draw and
   // return cycle.  This must be evenly divisible by 16 and at least 64.
   // If this is changed, alarm limits in cobe_config.dat will need to
   // be scaled accordingly.
   //
   enum { SampleSize      = 384 };
   enum { SmallSampleSize = 192 };

protected:
   virtual void UpdateMonitorState (void);
   virtual void Enable (void);
   virtual void Disable (void);

   void   CheckForOcclusion (int currentSampleSize);
   double GetSqrBinPower (int bin, int currentSampleSize);
   double FreqRespSqrInletScale (double oldRPM, double newRPM);
   double FreqRespSqrInletMag (double rpm);
   double FreqRespSqrReturnScale (double oldRPM, double newRPM);
   double FreqRespSqrReturnMag (double rpm);

protected:
   //
   // Data required for alarm detection
   //
   Message< FASTDATA >* _APSFastDataMsg;
   HalStatus_CDS        _HalStatus;

   //
   // Alarm message
   //
   anAlarmMsg _ReturnOcclusionAlarm;
   anAlarmMsg _ReturnOcclusionAlarmA1;

   //
   // Monitor state for alarm
   //
   enum MonitorState
   {
      OCCMON_IDLE,            // monitor has not yet started
      OCCMON_DRAW_START,      // draw cycle in progress, waiting to start collecting data
      OCCMON_DRAW_DATA,       // draw cycle in progress, collecting data
      OCCMON_RETURN_START,    // return cycle in progress, waiting to start collecting data
      OCCMON_RETURN_DATA      // return cycle in progress, collecting data
   };
   int          _MonitorEnabled;
   MonitorState _MonitorState;
   double       _StateStartTime;
   int          _ThresholdCount;

   //
   // Inlet pump data from most recent draw cycle
   //
   int    _LastInletRPM;
   int    _LastInletCount;
   double _LastInletPower;

   //
   // Pump RPM values for current cycle
   //
   double _ReturnRPM;
   double _InletRPM;

   //
   // Data buffer for sine/cosine computations.  Since these are expensive in terms
   // of CPU time, all of the values that will be needed during operation are
   // pre-computed.  Memory cost is a function of SampleSize (the number of APS
   // samples that are analyzed during each draw and return cycle).  For a sample
   // size of 256, the sine/cosine buffer requires approximately 64K bytes of memory.
   //
   enum { TrigTableSize    = SampleSize / 2 };
   enum { TrigTableEntries = SampleSize / 4 };
   double* _CosTable[TrigTableEntries];
   double* _SinTable[TrigTableEntries];

   enum { SmallTrigTableSize    = SmallSampleSize / 2 };
   enum { SmallTrigTableEntries = SmallSampleSize / 4 };
   double* _SmallCosTable[SmallTrigTableEntries];
   double* _SmallSinTable[SmallTrigTableEntries];

   //
   // Data buffer for APS samples - filled during each draw and return cycle
   //
   short  _SampleBuffer[SampleSize];
   int    _SampleBufferIndex;
   double _SampleSum;
   short  _SampleAverage;

   double _SampleInletRPMTotal, _MinSampleInletRPM, _MaxSampleInletRPM;
   double _SampleReturnRPMTotal, _MinSampleReturnRPM, _MaxSampleReturnRPM;
   int    _SampleRPMCount;

   int    _AnalyzeCount;

   //
   // Status check message from alarm handler
   //
   RetOccMonitorStatusMsg _AlarmHandlerStatusCheck;

   //
   // Common data store access
   //
   ProcTrimaSet_CDS _trimasetcds;
   ProcTime_CDS     _timecds;
};


#endif

/* FORMAT HASH 962d9941cd6a90d8e4e039cae63097b4 */
