/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MssRbc_Monitor.h
 *
 */


#ifndef __MSS_RBC_MONITIOR_H_
#define __MSS_RBC_MONITIOR_H_

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"
#include <vector>

class MssRbcMonitor
   : public MonitorBase
{
   DECLARE_OBJ(MssRbcMonitor);
public:

   MssRbcMonitor();
   virtual ~MssRbcMonitor();

   virtual void Monitor ();
   virtual void enable ();
   virtual void disable ();

protected:

   void check (const float ratio);
   void average (const float ratio);
   void watchSecondConnection (const float ratio);

   float FindRunningAvg (const float ratio) ;

protected:

   //
   //  alarm
   //
   anAlarmMsg _VerifyClosureAlarm;

   anAlarmMsg _misconnectedPasBag;
   //
   // Counter used to measure alarm duration
   //
   int _UpdateCounter;

   //
   // Flag used to save the detection status
   //
   int _Detected;



   float _avgRatio;
   float _sumRatios;
   float _numRatios;
   bool  _avgComputed; // the average is ready
   float _highRGratio; // the highest ratio seen


   int           _bagwatchNumRatios;
   float         _bagwatchSumRatios;
   float         _bagwatchAvgRatios;
   vector<float> _bagwatchrunnig50;
   bool          _runningAvgFilled;


   float _accruedPls_Vol;
   int   _bagAlarmCount;



   /// the new RG stuff
   void  collectSamples (const float ratio);
   void  analyzeSamples ();
   void  matchAverage ();
   float sigma ();


   vector<float> _sampleContainer;
   int           _sampleCount;
   int           _badSampleCount;
   bool          _samplingDone;
   bool          _analysisDone;
   float         _sumOfSamples;

   float         _avgGREEN;           // IT9818
   float         _sumGREEN;           // IT9818
   float         _numGREEN;           // IT9818
   bool          _greenLowSkipPurges; // IT9818

   int           _pauseToCheckCounter; // IT10341


};

#endif

/* FORMAT HASH dec3a96898680a3d2c6c68cc56c739a0 */
