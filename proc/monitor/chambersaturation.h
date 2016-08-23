/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      chambersaturation.h
 *
 */


#ifndef _CHAMBERSATURATION_H_
#define _CHAMBERSATURATION_H_

// Include files
#include <vxworks.h>

#include "aPQMonitor.h"
#include "procdata.h"
#include "rbcdat.h"
#include "states.h"
#include "timekeeper.h"


class ChamberSaturation
   : public aPQMonitor
{
   DECLARE_OBJ(ChamberSaturation);
public:

   enum
   {
      LEDSampleCount         =7,
      RatioAveragePeakCount  =24,
      RatioAverageMedianCount=120,
      RatioMedianCount       =20,
      CrAverageCount         =10,
      RGpSampleCount         =30,
      RGpMedianCount         =30,
      QchFlowCount           =120,
      CentSpeedCount         =360
   };

   enum FilterStatus
   {
      Enabled        =0,
      WrongState     =1,
      PltConcTooLow  =2,
      QchTooLow      =4,
      CentSpeedChange=8,
      CentSpeedTooLow=16,
      APSAlarm       =32,
      VinOutOfBound  =64,
      ZeroBaseline   =128,
      ConfigedOut    =256,
      Triggered      =512,
      FlagDisabled   =1024
   };


   // Default constructor
   ChamberSaturation();

   // Destructor
   virtual ~ChamberSaturation();

   // Initialize the class
   virtual void Initialize ();

   virtual void enable ();
   virtual void disable ();

   //
   // Function called every status update ...
   virtual void Monitor ();
   //
   // Functions to manage enabling/disabling saturation flagging
   // during a brief period (4 mins) after collect valve
   // opens after purges
   void NotifyLineCleared ();
   void NotifyLineClearedWait ();

protected:

   //
   // Function called every 6 seconds ...
   //
   void Process6SecData ();

   //
   // Functions which return whether or not the flags should be set
   //
   bool crit3aTriggered ();
   bool crit3bTriggered ();
   bool crit4aTriggered ();
   bool crit4bTriggered ();

   //
   // Functions which return the active status of the flagging criteria
   //
   bool crit3Enabled ();
   bool crit3aEnabled ();
   bool crit3bEnabled ();
   bool crit4Enabled ();
   bool crit4aEnabled ();
   bool crit4bEnabled ();

   //
   // Calculate our new periodic values ...
   //
   void  calculateCrit3a ();
   void  calculateCrit3b ();
   void  calculateCrit4a ();
   void  calculateCrit4b ();
   void  calculateSampleValues ();
   void  calculateRGr ();
   void  calculateHardwareValues ();
   void  calculateRGrMd ();
   void  calculateCrAv ();
   void  calculateRGp ();
   void  calculateRGpMd ();
   void  calculateBaselineA ();
   void  calculateBaselineB ();
   void  resetBaselineB ();
   float getChangeInQch ();
   int   getChangeInCentSpeed ();
   int   getMinCentSpeed ();

   float average (int arraySize, const float* array);
   int   average (int arraySize, const int* array);

   int   median (int arraySize, int* array);
   float median (int arraySize, float* array);

   int* createIndexArray (int arraySize);

private:

   static int*   _IntArray;
   static float* _FloatArray;

   static int compare (const void* index1, const void* index2);

   static DataLog_SetHandle _DataLogSet;
   static ProcData*         _pd;

private:

   enum { PrePltCollection, PltCollection, PostPltCollection } _PltCollectionStatus;

   TimeKeeper _IntervalCheck;
   TimeKeeper _ProcessNotifier;

   //
   // Allows us to coordinate disabling the saturation flag after purge
   //
   TimerMessage*        _postCLWaitTimer;
   ClearLineCompleteMsg _clCompleted;

   int                  _Crit3aPeakToPeakStartTime;
   int                  _Crit3aPeakToPeakTriggerTime;
   int                  _Crit3aSlopeStartTime;
   int                  _Crit3aSlopeTriggerTime;
   bool                 _Crit3aTriggered;
   int                  _Crit3bPeakToPeakStartTime;
   int                  _Crit3bPeakToPeakTriggerTime;
   int                  _Crit3bSlopeStartTime;
   int                  _Crit3bSlopeTriggerTime;
   bool                 _Crit3bTriggered;
   int                  _Crit4aStartTime;
   bool                 _Crit4aTriggered;
   int                  _Crit4bStartTime;
   bool                 _Crit4bTriggered;

   float                _CurrentQch;

   int                  _SampleIndex;
   int                  _RedArray[ LEDSampleCount ];
   int                  _GreenArray[ LEDSampleCount ];
   float                _RGrArray[ LEDSampleCount ];

   int                  _RGrAvgPeakIndex;
   float                _RGrAvgPeakArray[ RatioAveragePeakCount ];

   int                  _RGrAvgMdIndex;
   float                _RGrAvgMdArray[ RatioAverageMedianCount ];

   int                  _RGrMdIndex;
   float                _RGrMdArray[ RatioMedianCount ];

   int                  _CrAvgIndex;
   float                _CrAvgArray[ CrAverageCount ];

   bool                 _RGpArrayFull;
   int                  _RGpIndex;
   float                _RGpArray[ RGpSampleCount ];

   bool                 _OneMinOfRGpMdData;
   bool                 _RGpMdArrayFull;
   int                  _RGpMdIndex;
   float                _RGpMdArray[ RGpMedianCount ];

   bool                 _BaselineCalculated;
   bool                 _BaselineCalculatedMidRun;
   bool                 _BaselineResetAtMidRun;
   bool                 _BaselineACalculated;
   int                  _BaselineACount;
   float                _BaselineASum;
   float                _VinBaselineACalc;

   int                  _BaselineBCount;
   float                _BaselineBSum;
   float                _BaselineBTime; // in minutes

   int                  _QchIndex;
   float                _QchArray[ QchFlowCount ];
   long                 _QchLastTimeTooLow; // in milliseconds

   int                  _CentSpeedIndex;
   int                  _CentSpeedArray[ CentSpeedCount ];

   long                 _BaselineBDelayStartTime; // in milliseconds

   float                _APSAlarmTime; // in minutes

   // Fix for IT 8149: This allows us to temporarily disable
   // saturation flagging after a purge to reduce likelihood
   bool _SatFlagDisabled;

   //
   //  Copy constructor, op= not implemented
private:
   ChamberSaturation(const ChamberSaturation&);
   ChamberSaturation& operator = (const ChamberSaturation&);
};


#endif

/* FORMAT HASH 34d46c26a02fffb61c6d90545540c536 */
