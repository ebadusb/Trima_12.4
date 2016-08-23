/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MssRbc_Monitor.cpp
 *   This Monitor watchs the RG ratio at PLT addition for contamination
 *   it also watches for the second SS bag connection.
 *
 */

#include "MssRbc_Monitor.h"
#include "cobeconfig.h"
#include "trima_datalog.h"
#include "procdata.h"


// Local constants
static const float VERY_LARGE_RATIO = 1e6;
static const float ratioLimit       = CobeConfig::data().MssRGratio;
static const int   triggerSamples   = (int)CobeConfig::data().MssTriggerSamples;
static const int   samplestoCheck   = (int)CobeConfig::data().MssRbcDetectorZsamples;
static const int   samplestoCollect = (int)CobeConfig::data().MssRbcDetectorZsamples + 3;
static const float CSamples         = CobeConfig::data().mssConnectionSamples;
static const float Zo               = 2.058 * pow(samplestoCheck, 0.1014);
static const int   CheckLimit       = CobeConfig::data().MssCheckLimit;



DEFINE_OBJ(MssRbcMonitor);

MssRbcMonitor::MssRbcMonitor()
   : _VerifyClosureAlarm(CHANNEL_CLAMP_NOT_CLOSED_ALERT),
     _misconnectedPasBag(MSS_MISCONNECTED_2ND_PAS_BAG),
     _UpdateCounter(0),
     _Detected(0),
     _avgRatio(0.0),
     _sumRatios(0.0),
     _numRatios(0.0),
     _avgComputed(false),
     _highRGratio(-1.0), // start low so we populate
     _bagwatchNumRatios(0),
     _bagwatchSumRatios(0.0),
     _bagwatchAvgRatios(0.0),
     _bagwatchrunnig50(),
     _runningAvgFilled(false),
     _accruedPls_Vol(0.0),
     _bagAlarmCount(0),
     _sampleContainer((samplestoCheck + 5), 0.0), // space for samples + 5 extra for luck
     _sampleCount(0),
     _badSampleCount(0),
     _samplingDone(false),
     _analysisDone(false),
     _sumOfSamples(0.0),
     _avgGREEN(0.0),
     _sumGREEN(0.0),
     _numGREEN(0.0),
     _greenLowSkipPurges(false),
     _pauseToCheckCounter(0)
{}

MssRbcMonitor::~MssRbcMonitor()
{}

void MssRbcMonitor::Monitor ()
{
   ProcData pd;

   float    red;
   float    green;
   float    ratio;

   red   = (float)pd.Status().RedValue();
   green = (float)pd.Status().GreenValue();

   // Ensure green value is not zero.
   if (green != 0.0f)
   {
      ratio = red / green;
   }
   // If both green and red are zero then ratio is one.
   else if (red == 0.0f)
   {
      ratio = 1.0f;
   }
   // Other wise the ratio is very large.
   else
   {
      ratio = VERY_LARGE_RATIO;
   }

   // do this only after the vac purge state
   if (
      (pd.Run().currentPLTMSSSubState ==  SS_MSSVACPURGEFINAL)
      || (pd.Run().currentPLTMSSSubState ==  SS_MSSPLTADDITIONPAUSE)
      )
   {

      if ((red > 0.0) && (green > 0.0))
      {
         _sumGREEN += green;
         _numGREEN++;

         average(ratio);
      }

   }

   // do this only after the vac purge state
   if (
      ( pd.Run().currentPLTMSSSubState ==  SS_MSSPLTADDITION )
      &&   pd.Status().CollectValve.IsCollecting()
      &&   pd.TrimaSet().RBCDetectorCalibrated.Get()
      )
   {

      if ((red > 0.0) && (green > 0.0))
      {
         watchSecondConnection(ratio);

         if (_pauseToCheckCounter < CheckLimit)  // IT 10341
         {
            check(ratio);
         }
      }
   }

}

void MssRbcMonitor::check (const float ratio)
{
   ProcData pd;

   // keep a running high water mark for logs
   if ( pd.TrimaSet().RBCDetectorCalibrated.Get() )
      if (ratio > _highRGratio)
         _highRGratio = ratio;


   // compute the average
   if ( (_numRatios > 0) && (!_avgComputed) )

   {

      if (_numGREEN > 0) _avgGREEN = _sumGREEN / _numGREEN;
      DataLog(log_level_proc_info) << "Number of Samples of Green Ratio for avg  -> "  << _numGREEN  << endmsg;
      DataLog(log_level_proc_info) << "SUM of Green Ratio for avg      -> "  << _sumGREEN  << endmsg;
      DataLog(log_level_proc_info) << "Average Green Ratio             -> "  << _avgGREEN   << endmsg;
      if (_avgGREEN < 400.0f)
         DataLog(log_level_proc_info) << "Purge disabled for Green value avg less than 400.0"  << endmsg;

      _avgRatio    =  _sumRatios / _numRatios;
      _avgComputed = true;
      DataLog(log_level_proc_info) << "Number of Samples of RG Ratio for avg  -> "  << _numRatios  << endmsg;
      DataLog(log_level_proc_info) << "SUM of RG Ratio for avg      -> "  << _sumRatios  << endmsg;
      DataLog(log_level_proc_info) << "Average RG Ratio             -> "  << _avgRatio   << endmsg;
      DataLog(log_level_proc_info) << "Trigger threshold            -> "  << ratioLimit + _avgRatio   << endmsg;
   }


   if (_avgGREEN < 400.0f)
   {
      _greenLowSkipPurges = true;
      // return;
   }


   // we failed the clam check or priming... now check this for a full failure.
   // IT 8007
   static bool FullFailureWBC_check = false;
   if (!FullFailureWBC_check)
   {
      if (pd.PQI().ClampClosureFailedPartFailure.Get())
      {
         if (_avgRatio > ratioLimit)
         {
            // flag as possible WBC product
            pd.MakePQIWritable();
            pd.PQI().ClampClosureFailedFullFailure.Set(true);
            pd.MakePQIReadable();
            DataLog(log_level_proc_info) << "Trigger WBC flag for clamp closure, AVG-> "  << _avgRatio   << endmsg;
         }
      }
      FullFailureWBC_check = true;
   }



   if (  (pd.Run().cassettePurgeSetup.Get()
          || pd.Run().redoCassettePurge.Get())
         )
   {
      return;   // if we're doing the purge dont continue to flag and alarm
   }



   //////  pause the pumps if one violation and do the sample check
   if ( (ratio > (ratioLimit + _avgRatio) ) && !_Detected)
   {
      _UpdateCounter++;

      if ( _UpdateCounter == triggerSamples )
      {
         _Detected = true;

         if ( !pd.Run().pauseCassettePurge.Get() )
         {
            // IT 10341 .................
            if (_greenLowSkipPurges)
               _pauseToCheckCounter++;

            if (_pauseToCheckCounter >= CheckLimit)
            {
               _Detected = false;
               pd.MakeRunWritable();
               pd.Run().pauseCassettePurge.Set(false);
               pd.MakeRunReadable();
               DataLog(log_level_proc_info) << "(green < 400) Limit for pausing and flag checking reached, monitor will no longer check. " << _pauseToCheckCounter  << endmsg;

            }
            else
            {

               DataLog(log_level_proc_info) << "Detected a high RG ratio , Pausing pumps for analysis (" << ratio << ")" << endmsg;
               pd.MakeRunWritable();
               pd.Run().pauseCassettePurge.Set(true);
               pd.MakeRunReadable();
            }
            // ............................



         }

      }

   }

   //////////////
   if (_Detected)
   {
      collectSamples(ratio);
      analyzeSamples();
      matchAverage();
   }


}


/////////////////////////////////////////////////////////////////////////////////////////////////////
void MssRbcMonitor::collectSamples (const float ratio)
{

   if (_samplingDone)
   {
      return;
   }

   DataLog(log_level_proc_info) << "collectSamples( " <<  ratio << " )" << endmsg;

   if (_sampleCount < samplestoCollect)
   {
      _sampleContainer[_sampleCount] = ratio;
      _sampleCount++;

      DataLog(log_level_proc_info) << "Sample# " <<  _sampleCount  << endmsg;
   }
   else
   {

      DataLog(log_level_proc_info) << "Sampleing done "   << endmsg;
      _samplingDone = true;
   }


}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void MssRbcMonitor::analyzeSamples ()
{
   if (!_samplingDone || (samplestoCheck <= 0))
      return;

   if (_analysisDone)
      return;


   ProcData pd;
   DataLog(log_level_proc_info) << "analyzeSamples()" << endmsg;

   float STDDEV = sigma();


   float sum = 0.0f;
   for (int Tally = 0; Tally < samplestoCheck; Tally++)
      sum += _sampleContainer[Tally];

   float currentAvg = sum / samplestoCheck;

   for (int t = 0; t < samplestoCheck; t++)
   {
      float samp    = _sampleContainer[t];
      bool  TooHigh = ( samp > (currentAvg + Zo * STDDEV) );
      bool  TooLow  = ( samp < (currentAvg - Zo * STDDEV) );

      DataLog(log_level_proc_info) << "Sample -> " << samp << ", Zo -> " << Zo << ", STDEV -> " << STDDEV << ", _avgRatio -> " << _avgRatio << ", Current Avg -> " << currentAvg << endmsg;
      if (TooHigh || TooLow)
      {
         if (_badSampleCount < 3)
         {

            _badSampleCount++;
            DataLog(log_level_proc_info) << "a BAD SAMPLE FOUND -> " << samp << endmsg;
            DataLog(log_level_proc_info) << "Bad Sample #" << _badSampleCount  << endmsg;
            _sampleContainer[t] = _sampleContainer[samplestoCheck + _badSampleCount - 1];

            DataLog(log_level_proc_info) << "BAD SAMPLE replaced with -> " << _sampleContainer[t] << endmsg;



            sum = 0.0f;
            for (int Tally2 = 0; Tally2 < samplestoCheck; Tally2++)
               sum += _sampleContainer[Tally2];

            currentAvg = sum / samplestoCheck;
            STDDEV     = sigma();

            DataLog(log_level_proc_info) << "New Sigma -> " << STDDEV << endmsg;
            t = 0;

         }
         else
         {

            DataLog(log_level_proc_info) << "Too many Bad Samples " << _badSampleCount  << endmsg;
            _analysisDone = true;
            pd.MakeRunWritable();
            pd.Run().pauseCassettePurge.Set(false);
            pd.MakeRunReadable();
            _Detected = false;
            break;
         }
      }
   }


   for (int finalTally = 0; finalTally < samplestoCheck; finalTally++)
      _sumOfSamples += _sampleContainer[finalTally];

   DataLog(log_level_proc_info) << "_sumOfSamples =" << _sumOfSamples   << endmsg;

   _analysisDone = true;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void MssRbcMonitor::matchAverage ()
{


   ProcData pd;

   if (!_samplingDone && !_analysisDone)
      return;

   DataLog(log_level_proc_info) << "matchAverage()" << endmsg;
   if ( ( (_sumOfSamples / samplestoCheck) > (ratioLimit + _avgRatio) )
        && (_badSampleCount < 3)
        && !(pd.Run().cassettePurgeSetup.Get() || pd.Run().redoCassettePurge.Get())
        )
   {

      DataLog(log_level_proc_info) << "Contamination Detected by RBC detector,  Flagging plts for WBCs " << endmsg;

      pd.MakePQIWritable();
      pd.PQI().ClampClosureFailedFullFailure.Set(true);
      pd.MakePQIReadable();

      if (_greenLowSkipPurges)
      {
         DataLog(log_level_proc_info) << "Green too low, no PURGE allowed (disable monitor now)" << endmsg;
         pd.MakeRunWritable();
         pd.Run().cassettePurgeSetup.Set(false);
         pd.MakeRunReadable();

         pd.MakeRunWritable();
         pd.Run().pauseCassettePurge.Set(false);
         pd.MakeRunReadable();

         // doing this once is enough, we've flagged and cannot purge
         disable();

      }
      else
      {

         DataLog(log_level_proc_info) << "MssRbc threshold exceeded" <<  endmsg;
         pd.MakeRunWritable();
         pd.Run().cassettePurgeSetup.Set(true);
         pd.MakeRunReadable();
      }



   }
   else
   {

      DataLog(log_level_proc_info) << "no PURGE needed " << endmsg;
      pd.MakeRunWritable();
      pd.Run().pauseCassettePurge.Set(false);
      pd.MakeRunReadable();

   }

   _analysisDone   = false;
   _badSampleCount = 0;
   _sumOfSamples   = 0.0f;
   _sampleCount    = 0;
   _Detected       = false;
   _samplingDone   = false;
   _UpdateCounter  = 0;



}
/////////////////////////////////////////////////////////////////////////////////////////////////////
float MssRbcMonitor::sigma ()
{

   float sum   = 0.0;
   float sumSq = 0.0;
   float sigma = 0.0f;

   if (_sampleContainer.size() > samplestoCheck)
   {
      // data loop
      for (int i = 0; i<samplestoCheck; i++)
      {
         sum   += _sampleContainer[i];
         sumSq += _sampleContainer[i] * _sampleContainer[i];
      }

      if (samplestoCheck <= 1)
      {
         sigma =  0.0f;
      }
      else if (sumSq <= ((sum * sum) / samplestoCheck))
      {
         sigma =  0.0f;
      }
      else
      {
         sigma = (float)sqrt( (sumSq - (sum * sum) / samplestoCheck) / (samplestoCheck - 1) );
      }

   }
   else      // no data no sigma  Ha!
   {
      sigma =  0.0f;
   }

   DataLog(log_level_proc_info) << "Sigma -> " << sigma << endmsg;
   return sigma;


}
/////////////////////////////////////////////////////////////////
void MssRbcMonitor::average (const float ratio)
{
   // do the average
   _sumRatios = _sumRatios + ratio;
   _numRatios++;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void MssRbcMonitor::watchSecondConnection (const float ratio)
{


   ProcData pd;

   if (pd.Run().SecondPLTSSBag.Get()
       && (_bagwatchNumRatios < CSamples))
   {
      return;
   }

   if (pd.Run().SecondPLTSSBag.Get())
   {

      _accruedPls_Vol += pd.Status().PlasmaPump.dV();

      // do the check vs avg
      if ( (_accruedPls_Vol <= CobeConfig::data().mssConnectionRGmaxVol) &&
           (_bagAlarmCount < 1) // alarm count is so we dont retrigger after second bag hook up
           )
      {
         if ( ratio > (_bagwatchAvgRatios + CobeConfig::data().mssConnectionDeltaRGmax) )
         {
            DataLog(log_level_proc_info)
               << "Second Bag Checking alarm triggered; current RG Ratio-> " << ratio
               << ", Threshold for Trigger -> " << _bagwatchAvgRatios + CobeConfig::data().mssConnectionDeltaRGmax
               << ", Running Avg -> " << _bagwatchAvgRatios << endmsg;

            // not initially needed per L. F. 4-2-07
            // set low SS
            // pd.MakeRunWritable();
            //  pd.Run().flagMSSVolumeError.Set(true);
            // pd.MakeRunReadable();

            // alarm
            _misconnectedPasBag.setAlarm();

            _bagAlarmCount++;
         }
      }

   }
   else // no second bag yet so do running avg
   {
      _bagwatchAvgRatios = FindRunningAvg(ratio);
      if ( (_bagwatchNumRatios % 20) == 0)
      {
         DataLog(log_level_proc_info) << "Current running ave for RG -> "
                                      <<  _bagwatchAvgRatios
                                      << "; samples -> "
                                      <<  _bagwatchNumRatios << endmsg;
      }
   }


}

/////////////////////////////////////////////////////////////////////////////////////////////////////
float MssRbcMonitor::FindRunningAvg (const float ratio)
{
   if (!_runningAvgFilled)
   {
      _bagwatchrunnig50.push_back(ratio);
      if (_bagwatchNumRatios >= (CSamples - 1.0))  // 0..49 slots filled == 50
         _runningAvgFilled = true;

   }
   else
   {


      int loc = (_bagwatchNumRatios % (int)CSamples);
      _bagwatchrunnig50[loc] = ratio;

   }
   _bagwatchNumRatios++;

   if (_runningAvgFilled)
   {
      float tot = 0.0;
      for (int i = 0; i < _bagwatchrunnig50.size(); i++)
      {
         tot = tot + _bagwatchrunnig50[i] ;
      }
      return tot / CSamples;
   }
   return 1.0f;

}



/////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void MssRbcMonitor::enable ()
{
   MonitorBase::enable();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void MssRbcMonitor::disable ()
{
   DataLog(log_level_proc_info) << "MssRBCMonitor RG ratio High water mark during PLT addition -> "  << _highRGratio << endmsg;

   DataLog(log_level_proc_info) << "Second Bag Monitor RG avg -> "  <<  _bagwatchAvgRatios << endmsg;
   DataLog(log_level_proc_info) << "Second Bag Monitor RG samples -> "  <<  _bagwatchNumRatios << endmsg;

   MonitorBase::disable();
}

/* FORMAT HASH b2b0a8d6baa62b770eaec1eef36455c5 */
