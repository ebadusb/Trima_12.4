/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * File: chambersaturation.cpp
 *
 */

#include <vxworks.h>
#include <math.h>
#include <float.h>

#include "chambersaturation.h"
#include "cobeconfig.h"
#include "datalog_periodic.h"
#include "procdata.h"
#include "rbcdat.h"
#include "software_cds.h"

int*              ChamberSaturation::_IntArray   = 0;
float*            ChamberSaturation::_FloatArray = 0;

DataLog_SetHandle ChamberSaturation::_DataLogSet = DATALOG_NULL_SET_HANDLE;
ProcData*         ChamberSaturation::_pd         = 0;


#define SUFFICIENTLY_SMALL_FLOAT       0.0001f


const float CALCULATION_INTERVAL        = CobeConfig::data().CalculationInterval      ;
const float ACTIVATION_TIME             = CobeConfig::data().ActivationTime           ;
const float MIN_DELTA_CHAMBER_FLOW      = CobeConfig::data().MinDeltaChamberFlow      ;
const float MIN_CHAMBER_FLOW            = CobeConfig::data().MinChamberFlow           ;
const float MIN_RED                     = CobeConfig::data().MinRed                   ;
const float MAX_RED                     = CobeConfig::data().MaxRed                   ;
const float MIN_UPDATE_INTERVAL         = CobeConfig::data().MinUpdateInterval        ;
const float ONE_MIN_MSECS               = CobeConfig::data().OneMin_Msecs             ;
const float THREE_MIN_MSECS             = CobeConfig::data().ThreeMinMsecs            ;
const float POST_PURGE_WAIT             = CobeConfig::data().PostPurgeWait            ;
const float MIN_PLT_CONCENTRATION       = CobeConfig::data().MinPltConcentration      ;
const float MIN_PLT_CONCENTRATION_JAPAN = CobeConfig::data().MinPltConcentrationJapan;
const float MIN_DELTA_CENT_SPEED        = CobeConfig::data().MinDeltaCentSpeed        ;
const float MIN_CENT_SPEED              = CobeConfig::data().MinCentSpeed             ;
const float VIN_START_C4A               = CobeConfig::data().VinStartc4a              ;
const float VIN_END_C4B                 = CobeConfig::data().VinEndc4b                ;
const float MIN_BASELINE                = CobeConfig::data().MinBaseline              ;






DEFINE_OBJ(ChamberSaturation);

ChamberSaturation::ChamberSaturation()
   : _clCompleted(Callback<ChamberSaturation>(this, &ChamberSaturation::NotifyLineCleared) ),
     _PltCollectionStatus(PrePltCollection),
     _IntervalCheck(),
     _ProcessNotifier(),
     _Crit3aPeakToPeakStartTime(0),
     _Crit3aPeakToPeakTriggerTime(0),
     _Crit3aSlopeStartTime(0),
     _Crit3aSlopeTriggerTime(0),
     _Crit3aTriggered(false),
     _Crit3bPeakToPeakStartTime(0),
     _Crit3bPeakToPeakTriggerTime(0),
     _Crit3bSlopeStartTime(0),
     _Crit3bSlopeTriggerTime(0),
     _Crit3bTriggered(false),
     _Crit4aStartTime(0),
     _Crit4aTriggered(false),
     _Crit4bStartTime(0),
     _Crit4bTriggered(false),
     _SampleIndex(0),
     _RGrAvgPeakIndex(0),
     _RGrAvgMdIndex(0),
     _RGrMdIndex(0),
     _CrAvgIndex(0),
     _RGpArrayFull(false),
     _RGpIndex(0),
     _OneMinOfRGpMdData(false),
     _RGpMdArrayFull(false),
     _RGpMdIndex(0),
     _BaselineCalculated(false),
     _BaselineCalculatedMidRun(false),
     _BaselineResetAtMidRun(false),
     _BaselineACalculated(false),
     _BaselineACount(0),
     _BaselineASum(0),
     _VinBaselineACalc(0.0f),
     _BaselineBCount(0),
     _BaselineBSum(0),
     _BaselineBTime(0.0f),
     _QchIndex(0),
     _QchLastTimeTooLow(0),
     _CentSpeedIndex(0),
     _BaselineBDelayStartTime(0),
     _APSAlarmTime(0.0f),
     _SatFlagDisabled(false),
     _postCLWaitTimer(0),
     _CurrentQch(0.0f)
{
   memset(_RedArray, 0, sizeof( _RedArray ) );
   memset(_GreenArray, 0, sizeof( _GreenArray ) );
   memset(_RGrArray, 0, sizeof( _RGrArray ) );
   memset(_RGrAvgPeakArray, 0, sizeof( _RGrAvgPeakArray ) );
   memset(_RGrAvgMdArray, 0, sizeof( _RGrAvgMdArray ) );
   memset(_RGrMdArray, 0, sizeof( _RGrMdArray ) );
   memset(_CrAvgArray, 0, sizeof( _CrAvgArray ) );
   memset(_RGpArray, 0, sizeof( _RGpArray ) );
   memset(_RGpMdArray, 0, sizeof( _RGpMdArray ) );
   memset(_QchArray, 0, sizeof( _QchArray ) );
   memset(_CentSpeedArray, 0, sizeof( _CentSpeedArray ) );

   if ( !_pd )
      _pd = new ProcData;
}

ChamberSaturation::~ChamberSaturation()
{
   if (_postCLWaitTimer)
   {
      _postCLWaitTimer->armTimer(TimerMessage::DISARMED);
      delete _postCLWaitTimer;
      _postCLWaitTimer = NULL;
   }
}


void ChamberSaturation::Initialize ()
{
   if ( _DataLogSet == DATALOG_NULL_SET_HANDLE )
   {
      datalog_CreatePeriodicSet("Proc_ChamberSaturation", &_DataLogSet);

      int (BaseElement<int>::* int_func) (void) const       = &BaseElement<int>::Get;
      float (BaseElement<float>::* float_func) (void) const = &BaseElement<float>::Get;

      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().Red,            int_func,   "Red", "average red", "%d");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().RedPl,          int_func,   "RedPl", "red plasma baseline", "%d");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().Green,          int_func,   "Green", "average green", "%d");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().RGr,            float_func, "RGr", "average red/green ratio", "%.5lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().RGrMd,          float_func, "RGrMd", "red/green ratio median", "%.5lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().dRGrMd,         float_func, "dRGrMd", "delta red/green ratio median", "%.5lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().Cr,             float_func, "Cr", "detected platelet concentration", "%.5lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().CrAv,           float_func, "CrAv", "average detected platelet concentration", "%.5lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().QchChamSat,     float_func, "Qch", "chamber flow used in algorithm", "%.5lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().RGp,            float_func, "RGp", "red/green ration peak-to-peak", "%.7lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().RGpMd,          float_func, "RGpMd", "median red/green ratio peak-to-peak", "%.7lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().AvRGpMd,        float_func, "AvRGpMd", "average red/green ratio peak-to-peak", "%.7lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().RGpMdBaseline,  float_func, "RGpMdBaseline", "baseine", "%.7lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().RGpMdBaselineA, float_func, "RGpMdBaselineA", "baseline A", "%.7lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().RGpMdBaselineB, float_func, "RGpMdBaselineB", "baseline B", "%.7lf");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().Crit3Status,    int_func,   "Crit3Status", "status of criteria 3 in chamber saturation algo", "%d");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().Crit3aStatus,   int_func,   "Crit3aStatus", "status of criteria 3a in chamber saturation algo", "%d");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().Crit3bStatus,   int_func,   "Crit3bStatus", "status of criteria 3b in chamber saturation algo", "%d");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().Crit4Status,    int_func,   "Crit4Status", "status of criteria 4 in chamber saturation algo", "%d");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().Crit4aStatus,   int_func,   "Crit4aStatus", "status of criteria 4a in chamber saturation algo", "%d");
      datalog_AddMemberFunc(_DataLogSet, &_pd->Run().Crit4bStatus,   int_func,   "Crit4bStatus", "status of criteria 4b in chamber saturation algo", "%d");

      datalog_ForcePeriodicOutput(_DataLogSet);
   }
}

void ChamberSaturation::Monitor ()
{
   //
   // Save the substate ...
   //
   State_names currentSubstate = ( _pd->Run().Substate.Get() != INVALID_SUBSTATE
                                   ? _pd->Run().Substate.Get()
                                   : SS_CHANNEL_SETUP );
   if (    _PltCollectionStatus == PrePltCollection
           && currentSubstate >= SS_CHANNEL_SETUP
           && currentSubstate <= SS_EXTENDED_PLASMA )
      _PltCollectionStatus = PltCollection;
   if (    _PltCollectionStatus == PltCollection
           && currentSubstate > SS_EXTENDED_PLASMA )
      _PltCollectionStatus = PostPltCollection;

   //
   // We are finished if we enter PostPltCollection state ...
   //
   if ( _PltCollectionStatus == PostPltCollection )
   {
      _ProcessNotifier.inactivate();
      return;
   }

   //
   // Only do this processing at an interval of about 500 milliseconds
   //
   if (    _IntervalCheck.activated()
           && _IntervalCheck.getMSecs() > MIN_UPDATE_INTERVAL )
   {
      //
      // Reset our interval check ...
      //
      _IntervalCheck.init();

      //
      // Check for APS alarm conditions ...
      //
      if (    _pd->AlarmQ().isActive(APS_TOO_HIGH_ALERT)
              || _pd->AlarmQ().isActive(APS_TOO_LOW) )
      {
         _APSAlarmTime = _pd->ProcRunTime().getMins();
      }

      //
      // Check our hardware on every update ...
      //
      calculateHardwareValues();

      if ( _pd->TrimaSet().CmdQch.Get() > MIN_DELTA_CHAMBER_FLOW )
      {
         //
         // Calculate new values ...
         calculateSampleValues();
         calculateRGr();
      }

   }

   //
   // Check for a saturation ...
   //
   if (    crit3aTriggered()
           || crit3bTriggered()
           || crit4aTriggered()
           || crit4bTriggered() )
   {
      _sout << "COUNT PLATELET PRODUCT because WBC contamination detected";
      LogIt(PLATELET_CONTAMINANT_DETECTION);
      _pqi.PutReason(PLATELET_CONTAMINANT_DETECTION);

      _pd->RequestWbcAlarm(__FILE__, __LINE__, "air block recovery");

      //
      // Only perform purge once.
      //
      if ( _pd->Run().PurgeDueToChamberSat.Get() == false)
      {
         //
         // Set the flag first to prevent a race condition
         //
         _pd->MakeRunWritable();
         _pd->Run().PurgeDueToChamberSat.Set(true);
         _pd->MakeRunReadable();

         //
         // Perform a chamber purge recovery ...
         //
         SpilloverRecoveryMsg requestSpillover(MessageBase::SEND_LOCAL);
         requestSpillover.send(1);
      }
   }
}

void ChamberSaturation::enable ()
{
   if (    _pd->Status().CassIsPltPlsRBC()
           && _pd->TrimaSet().RBCDetectorCalibrated.Get() )
   {
      //
      // Start our timers ...
      _IntervalCheck.activate();

      _ProcessNotifier.notifier(Callback<ChamberSaturation>(this, &ChamberSaturation::Process6SecData) );
      _ProcessNotifier.interval(CALCULATION_INTERVAL);
      _ProcessNotifier.activate();

      MonitorBase::enableMonitoring(1);
   }
   else
   {
      _IntervalCheck.inactivate();
      _ProcessNotifier.inactivate();

      MonitorBase::enableMonitoring(0);
   }

}

void ChamberSaturation::disable ()
{
   //
   // Turn off the 500 msec timer interval check ...
   //
   _IntervalCheck.inactivate();

   //
   // Turn off the 6 second timer ...
   //
   _ProcessNotifier.inactivate();

   MonitorBase::enableMonitoring(0);
}

void ChamberSaturation::NotifyLineCleared ()
{
   // No need to do this if we're not on a white stamp set
   if ( _pd->Status().CassIsPltPlsRBC() )
   {
      // reset the timer to tell us when to re-enable
      // the saturation flag
      if ( _postCLWaitTimer != NULL )
      {
         _postCLWaitTimer->armTimer(TimerMessage::DISARMED);
         delete _postCLWaitTimer;
         _postCLWaitTimer = NULL;
      }

      // disable flag here, if needed
      if ( !_SatFlagDisabled )
      {
         _SatFlagDisabled = true;

         DataLog (log_level_proc_pqi_info)
            << "ChamberSaturation Notification: Saturation flag temporarily disabled after Clear Line (valve opening)"
            << endmsg;
      }

      _postCLWaitTimer = new TimerMessage(POST_PURGE_WAIT, Callback< ChamberSaturation >(this, &ChamberSaturation::NotifyLineClearedWait), TimerMessage::ARMED);
   }
}

void ChamberSaturation::NotifyLineClearedWait ()
{
   if ( _SatFlagDisabled )
   {
      _SatFlagDisabled = false;

      DataLog (log_level_proc_pqi_info)
         << "ChamberSaturation Notification: Saturation flag re-enabled"
         << endmsg;
   }
}

void ChamberSaturation::Process6SecData ()
{
   //
   // Reset our timer to get notified in 6 seconds again...
   _ProcessNotifier.init();

   if ( _pd->TrimaSet().CmdQch.Get() > MIN_DELTA_CHAMBER_FLOW )
   {
      //
      // Calculate our 6 second data ...
      //
      calculateRGrMd();
      calculateCrAv();
      calculateRGp();
      calculateRGpMd();
      calculateBaselineA();
      calculateBaselineB();

      //
      // Calculate our criterium ...
      //
      calculateCrit3a();
      calculateCrit3b();
      calculateCrit4a();
      calculateCrit4b();
   }

   //
   // Log my values ...
   //
   datalog_ForcePeriodicOutput(_DataLogSet);
}

bool ChamberSaturation::crit3aTriggered ()
{
   if ( crit3aEnabled() )
   {
      unsigned long duration = (unsigned long)CobeConfig::data().ChamberSatDur_3a /*min*/
                               * 60 /*seconds/min*/ * 1000 /*millisecs/second*/;
      if (    _pd->ProcRunTime().getMSecs() - _Crit3aPeakToPeakTriggerTime < duration
              && _pd->ProcRunTime().getMSecs() - _Crit3aSlopeTriggerTime < duration
              )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3a triggered" << endmsg;
         _Crit3aTriggered = true;
         return true;
      }
   }
   else
   {
      _Crit3aPeakToPeakTriggerTime = 0;
      _Crit3aSlopeTriggerTime      = 0;
   }
   return false;
}

bool ChamberSaturation::crit3bTriggered ()
{
   if ( crit3bEnabled() )
   {
      unsigned long duration = (unsigned long)CobeConfig::data().ChamberSatDur_3b /*min*/
                               * 60 /*seconds/min*/ * 1000 /*millisecs/second*/;
      if (    _pd->ProcRunTime().getMSecs() - _Crit3bPeakToPeakTriggerTime < duration
              && _pd->ProcRunTime().getMSecs() - _Crit3bSlopeTriggerTime < duration
              )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3b triggered" << endmsg;
         _Crit3bTriggered = true;
         return true;
      }
   }
   else
   {
      _Crit3bPeakToPeakTriggerTime = 0;
      _Crit3bSlopeTriggerTime      = 0;
   }
   return false;
}

bool ChamberSaturation::crit4aTriggered ()
{
   if ( crit4aEnabled() )
   {
      if (    _Crit4aStartTime > 0
              && _pd->ProcRunTime().getMSecs() - _Crit4aStartTime > ACTIVATION_TIME )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4a triggered" << endmsg;
         _Crit4aTriggered = true;
         return true;
      }
   }
   else
   {
      _Crit4aStartTime = 0;
   }
   return false;
}

bool ChamberSaturation::crit4bTriggered ()
{
   if ( crit4bEnabled() )
   {
      if (    ( _Crit4bStartTime > 0 )
              && ( _pd->ProcRunTime().getMSecs() - _Crit4bStartTime > ACTIVATION_TIME )
              && ( _pd->Status().CollectValve.IsCollecting() )
              )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4b triggered" << endmsg;
         _Crit4bTriggered = true;
         return true;
      }
   }
   else
   {
      _Crit4bStartTime = 0;
   }
   return false;
}




bool ChamberSaturation::crit3Enabled ()
{
   int crit3Status = ChamberSaturation::Enabled;

   // Fix for IT8415: latch to delay flagging should actually be applied to the criteria.
   if ( _SatFlagDisabled )
   {
      // disable criteria (including a & b)
      crit3Status |= ChamberSaturation::FlagDisabled;

      if ( !(_pd->Run().Crit3Status.Get() & ChamberSaturation::FlagDisabled ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 disabled: delayed flagging after purge" << endmsg;
      }
   }
   else
   {
      // enable criteria
      crit3Status &= ~(ChamberSaturation::FlagDisabled);

      if ( _pd->Run().Crit3Status.Get() & ChamberSaturation::FlagDisabled )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 re-enabled after delay" << endmsg;
      }
   }

   //
   // Check for a valid baseline ...
   //
   if (    !_BaselineCalculated
           || _pd->Run().RGpMdBaseline < MIN_BASELINE )
   {
      crit3Status |= ChamberSaturation::ZeroBaseline;
      if ( !( _pd->Run().Crit3Status.Get() & ChamberSaturation::ZeroBaseline ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 disabled: No Baseline" << endmsg;
      }
   }
   else if ( _pd->Run().Crit3Status.Get() & ChamberSaturation::ZeroBaseline )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 re-enabled after new baseline calculation" << endmsg;
   }

   //
   // Check the state ...
   //
   if (    _pd->Run().Substate.Get() == SS_PCA
           || _pd->Run().Substate.Get() > SS_MIDRUN
           || _PltCollectionStatus == PostPltCollection )
   {
      crit3Status |= ChamberSaturation::WrongState;
      if ( !( _pd->Run().Crit3Status.Get() & ChamberSaturation::WrongState ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 disabled: Wrong State" << endmsg;
      }
   }
   else if ( _pd->Run().Crit3Status.Get() & ChamberSaturation::WrongState )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 re-enabled from Wrong State" << endmsg;
   }

   //
   // Check the concentration ...
   //
   float minconc = Software_CDS::GetInstance().getFeature(JapanFeatures) ? MIN_PLT_CONCENTRATION_JAPAN : MIN_PLT_CONCENTRATION;
   if ( _pd->Run().CrAv.Get() <= minconc )
   {
      crit3Status |= ChamberSaturation::PltConcTooLow;
      if ( !( _pd->Run().Crit3Status.Get() & ChamberSaturation::PltConcTooLow ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 disabled: Platelet Concentration detected too low , threshold=" << minconc << endmsg;
      }
   }
   else if ( _pd->Run().Crit3Status.Get() & ChamberSaturation::PltConcTooLow )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 re-enabled from Platelet Concentration detected too low , threshold=" << minconc << endmsg;
   }

   //
   // Check the change in Qch in the previous 3 minutes ...
   //
   if (    _QchLastTimeTooLow > 0
           && _pd->ProcRunTime().getMSecs() - _QchLastTimeTooLow <= THREE_MIN_MSECS )
   {
      crit3Status |= ChamberSaturation::QchTooLow;
      if ( !( _pd->Run().Crit3Status.Get() & ChamberSaturation::QchTooLow ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 disabled: Qch too slow" << endmsg;
      }
   }
   else if ( _pd->Run().Crit3Status.Get() & ChamberSaturation::QchTooLow )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 re-enabled from Qch too slow" << endmsg;
   }

   //
   // Check the change in centrifuge speed in the previous 3 minutes ...
   //
   if ( getChangeInCentSpeed() > MIN_DELTA_CENT_SPEED )
   {
      crit3Status |= ChamberSaturation::CentSpeedChange;
      if ( !( _pd->Run().Crit3Status.Get() & ChamberSaturation::CentSpeedChange ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 disabled: Centrifuge speed change" << endmsg;
      }
   }
   else if ( _pd->Run().Crit3Status.Get() & ChamberSaturation::CentSpeedChange )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 re-enabled from Centrifuge speed change" << endmsg;
   }

   //
   // Check the minimun centrifuge speed speed in the previous 3 minutes ...
   //
   /*if ( getMinCentSpeed() < MIN_CENT_SPEED )
   {
      crit3Status |= ChamberSaturation::CentSpeedTooLow;
      if ( !( _pd->Run().Crit3Status.Get() & ChamberSaturation::CentSpeedTooLow ) )
      {
         DataLog( log_level_proc_pqi_info ) << "ChamberSaturation algorithm c#3 disabled: Centrifuge speed" << endmsg;
      }
   }
   else if ( _pd->Run().Crit3Status.Get() & ChamberSaturation::CentSpeedTooLow )
   {
      DataLog( log_level_proc_pqi_info ) << "ChamberSaturation algorithm c#3 re-enabled from Centrifuge speed" << endmsg;
   }*/

   if (    _pd->Run().Crit3Status.Get() != crit3Status
           && crit3Status == ChamberSaturation::Enabled )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3 enabled" << endmsg;
   }

   _pd->MakeRunWritable();
   _pd->Run().Crit3Status = crit3Status;
   _pd->MakeRunReadable();
   return ( _pd->Run().Crit3Status.Get() == ChamberSaturation::Enabled );
}

bool ChamberSaturation::crit3aEnabled ()
{
   if ( _Crit3aTriggered )
   {
      _pd->MakeRunWritable();
      _pd->Run().Crit3aStatus = ChamberSaturation::Triggered;
      _pd->MakeRunReadable();
      return false;
   }

   int crit3aStatus = ChamberSaturation::Enabled;

   //
   // Standard Criteria 3 stuff ...
   //
   crit3Enabled();
   crit3aStatus |= _pd->Run().Crit3Status.Get();

   //
   // Disabled in config ...
   //
   if ( !RbcDat::data().ChamberSatSWs3a )
   {
      crit3aStatus |= ChamberSaturation::ConfigedOut;
      if ( !( _pd->Run().Crit3aStatus.Get() & ChamberSaturation::ConfigedOut ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3a disabled:  Configuration" << endmsg;
      }
   }
   else if ( _pd->Run().Crit3aStatus.Get() & ChamberSaturation::ConfigedOut )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3a re-enabled from Configuration" << endmsg;
   }

   //
   // APS alarm check ...
   //
   if (    _APSAlarmTime > SUFFICIENTLY_SMALL_FLOAT // small delta to avoid float problem
           && _pd->ProcRunTime().getMins() - _APSAlarmTime <= CobeConfig::data().ChamberSatTs3aAPS
           )
   {
      crit3aStatus |= ChamberSaturation::APSAlarm;
      if ( !( _pd->Run().Crit3aStatus.Get() & ChamberSaturation::APSAlarm ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3a disabled: APS alarm" << endmsg;
      }
   }
   else if ( _pd->Run().Crit3aStatus.Get() & ChamberSaturation::APSAlarm )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3a re-enabled from APS alarm" << endmsg;
   }

   if (    _pd->Run().Crit3aStatus.Get() != crit3aStatus
           && crit3aStatus == ChamberSaturation::Enabled )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3a enabled" << endmsg;
   }

   _pd->MakeRunWritable();
   _pd->Run().Crit3aStatus = crit3aStatus;
   _pd->MakeRunReadable();
   return ( _pd->Run().Crit3aStatus.Get() == ChamberSaturation::Enabled );
}

bool ChamberSaturation::crit3bEnabled ()
{
   if ( _Crit3bTriggered )
   {
      _pd->MakeRunWritable();
      _pd->Run().Crit3bStatus = ChamberSaturation::Triggered;
      _pd->MakeRunReadable();
      return false;
   }

   int crit3bStatus = ChamberSaturation::Enabled;

   //
   // Standard Criteria 3 stuff ...
   //
   crit3Enabled();
   crit3bStatus |= _pd->Run().Crit3Status.Get();

   //
   // Disabled in config ...
   //
   if ( !RbcDat::data().ChamberSatSWs3b )
   {
      crit3bStatus |= ChamberSaturation::ConfigedOut;
      if ( !( _pd->Run().Crit3bStatus.Get() & ChamberSaturation::ConfigedOut ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3b disabled:  Configuration" << endmsg;
      }
   }
   else if ( _pd->Run().Crit3bStatus.Get() & ChamberSaturation::ConfigedOut )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3b re-enabled from Configuration" << endmsg;
   }

   //
   // APS alarm check ...
   //
   if (    _APSAlarmTime > SUFFICIENTLY_SMALL_FLOAT // small delta to avoid float problem
           && _pd->ProcRunTime().getMins() - _APSAlarmTime <= CobeConfig::data().ChamberSatTs3bAPS
           )
   {
      crit3bStatus |= ChamberSaturation::APSAlarm;
      if ( !( _pd->Run().Crit3bStatus.Get() & ChamberSaturation::APSAlarm ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3b disabled: APS alarm" << endmsg;
      }
   }
   else if ( _pd->Run().Crit3bStatus.Get() & ChamberSaturation::APSAlarm )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3b re-enabled from APS alarm" << endmsg;
   }

   if (    _pd->Run().Crit3bStatus.Get() != crit3bStatus
           && crit3bStatus == ChamberSaturation::Enabled )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#3b enabled" << endmsg;
   }

   _pd->MakeRunWritable();
   _pd->Run().Crit3bStatus = crit3bStatus;
   _pd->MakeRunReadable();
   return ( _pd->Run().Crit3bStatus.Get() == ChamberSaturation::Enabled );
}

bool ChamberSaturation::crit4Enabled ()
{
   int crit4Status = ChamberSaturation::Enabled;

   // Fix for IT8415: latch to delay flagging should actually be applied to the criteria.
   if ( _SatFlagDisabled )
   {
      // disable criteria (including a & b)
      crit4Status |= ChamberSaturation::FlagDisabled;

      if ( !(_pd->Run().Crit4Status.Get() & ChamberSaturation::FlagDisabled ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4 disabled: delayed flagging after purge" << endmsg;
      }
   }
   else
   {
      // enable criteria
      crit4Status &= ~(ChamberSaturation::FlagDisabled);

      if ( _pd->Run().Crit4Status.Get() & ChamberSaturation::FlagDisabled )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4 re-enabled after delay" << endmsg;
      }
   }

   //
   // Check the state ...
   //
   if (    _pd->Run().Substate.Get() >= SS_EXTENDED_PCA
           || _PltCollectionStatus == PostPltCollection )
   {
      crit4Status |= ChamberSaturation::WrongState;
      if ( !( _pd->Run().Crit4Status.Get() & ChamberSaturation::WrongState ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4 disabled: Wrong State" << endmsg;
      }
   }
   else if ( _pd->Run().Crit4Status.Get() & ChamberSaturation::WrongState )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4 re-enabled from Wrong State" << endmsg;
   }

   //
   // Check the change in Qch in the previous 3 minutes ...
   //
   if (    _QchLastTimeTooLow > 0
           && _pd->ProcRunTime().getMSecs() - _QchLastTimeTooLow <= THREE_MIN_MSECS )
   {
      crit4Status |= ChamberSaturation::QchTooLow;
      if ( !( _pd->Run().Crit4Status.Get() & ChamberSaturation::QchTooLow ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4 disabled: Qch too slow" << endmsg;
      }
   }
   else if ( _pd->Run().Crit4Status.Get() & ChamberSaturation::QchTooLow )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4 re-enabled from Qch too slow" << endmsg;
   }

   //
   // APS alarm check ...
   //
   if (    _APSAlarmTime > SUFFICIENTLY_SMALL_FLOAT // small delta to avoid float problem
           && _pd->ProcRunTime().getMins() - _APSAlarmTime <= CobeConfig::data().ChamberSatTs4APS
           )
   {
      crit4Status |= ChamberSaturation::APSAlarm;
      if ( !( _pd->Run().Crit4Status.Get() & ChamberSaturation::APSAlarm ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4 disabled: APS alarm" << endmsg;
      }
   }
   else if ( _pd->Run().Crit4Status.Get() & ChamberSaturation::APSAlarm )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4 re-enabled from APS alarm" << endmsg;
   }

   if (    _pd->Run().Crit4Status.Get() != crit4Status
           && crit4Status == ChamberSaturation::Enabled )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4 enabled" << endmsg;
   }

   _pd->MakeRunWritable();
   _pd->Run().Crit4Status = crit4Status;
   _pd->MakeRunReadable();
   return ( _pd->Run().Crit4Status.Get() == ChamberSaturation::Enabled );
}

bool ChamberSaturation::crit4aEnabled ()
{
   if ( _Crit4aTriggered )
   {
      _pd->MakeRunWritable();
      _pd->Run().Crit4aStatus = ChamberSaturation::Triggered;
      _pd->MakeRunReadable();
      return false;
   }

   int crit4aStatus = ChamberSaturation::Enabled;

   //
   // Standard Criteria 4 stuff ...
   //
   crit4Enabled();
   crit4aStatus |= _pd->Run().Crit4Status.Get();

   //
   // Disabled in config ...
   //
   if ( !RbcDat::data().ChamberSatSWs4a )
   {
      crit4aStatus |= ChamberSaturation::ConfigedOut;
      if ( !( _pd->Run().Crit4aStatus.Get() & ChamberSaturation::ConfigedOut ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4a disabled:  Configuration" << endmsg;
      }
   }
   else if ( _pd->Run().Crit4aStatus.Get() & ChamberSaturation::ConfigedOut )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4a re-enabled from Configuration" << endmsg;
   }

   //
   // Check inlet volume ...
   //
   if ( _pd->Volumes().Vin <= VIN_START_C4A )
   {
      crit4aStatus |= ChamberSaturation::VinOutOfBound;
      if ( !( _pd->Run().Crit4aStatus.Get() & ChamberSaturation::VinOutOfBound ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4a disabled: Vin" << endmsg;
      }
   }
   else if ( _pd->Run().Crit4aStatus.Get() & ChamberSaturation::VinOutOfBound )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4a re-enabled from Vin" << endmsg;
   }

   if (    _pd->Run().Crit4aStatus.Get() != crit4aStatus
           && crit4aStatus == ChamberSaturation::Enabled )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4a enabled" << endmsg;
   }

   _pd->MakeRunWritable();
   _pd->Run().Crit4aStatus = crit4aStatus;
   _pd->MakeRunReadable();
   return ( _pd->Run().Crit4aStatus.Get() == ChamberSaturation::Enabled );
}

bool ChamberSaturation::crit4bEnabled ()
{
   if ( _Crit4bTriggered )
   {
      _pd->MakeRunWritable();
      _pd->Run().Crit4bStatus = ChamberSaturation::Triggered;
      _pd->MakeRunReadable();
      return false;
   }

   int crit4bStatus = ChamberSaturation::Enabled;

   //
   // Standard Criteria 4 stuff ...
   //
   crit4Enabled();
   crit4bStatus |= _pd->Run().Crit4Status.Get();

   //
   // Disabled in config ...
   //
   if ( !RbcDat::data().ChamberSatSWs4b )
   {
      crit4bStatus |= ChamberSaturation::ConfigedOut;
      if ( !( _pd->Run().Crit4bStatus.Get() & ChamberSaturation::ConfigedOut ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4b disabled:  Configuration" << endmsg;
      }
   }
   else if ( _pd->Run().Crit4bStatus.Get() & ChamberSaturation::ConfigedOut )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4b re-enabled from Configuration" << endmsg;
   }

   //
   // Check inlet volume ...
   //
   if ( _pd->Volumes().Vin > VIN_END_C4B )
   {
      crit4bStatus |= ChamberSaturation::VinOutOfBound;
      if ( !( _pd->Run().Crit4bStatus.Get() & ChamberSaturation::VinOutOfBound ) )
      {
         DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4b disabled: Vin" << endmsg;
      }
   }
   else if ( _pd->Run().Crit4bStatus.Get() & ChamberSaturation::VinOutOfBound )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4b re-enabled from Vin" << endmsg;
   }

   if (    _pd->Run().Crit4bStatus.Get() != crit4bStatus
           && crit4bStatus == ChamberSaturation::Enabled )
   {
      DataLog(log_level_proc_pqi_info) << "ChamberSaturation algorithm c#4b enabled" << endmsg;
   }

   _pd->MakeRunWritable();
   _pd->Run().Crit4bStatus = crit4bStatus;
   _pd->MakeRunReadable();
   return ( _pd->Run().Crit4bStatus.Get() == ChamberSaturation::Enabled );
}

void ChamberSaturation::calculateCrit3a ()
{
   if ( !_BaselineCalculated ) return;

   //
   // Check the peak-to-peak value
   //
   float avRGpMdCompValue = RbcDat::data().ChamberSatFsat *
                            CobeConfig::data().ChamberSatFs3a *
                            _pd->Run().RGpMdBaseline;
   if ( _pd->Run().AvRGpMd > avRGpMdCompValue )
   {
      if ( _Crit3aPeakToPeakStartTime == 0 ) _Crit3aPeakToPeakStartTime = _pd->ProcRunTime().getMSecs();
   }
   else
   {
      _Crit3aPeakToPeakStartTime = 0;
   }

   if (    _Crit3aPeakToPeakStartTime > 0
           && _pd->ProcRunTime().getMSecs() - _Crit3aPeakToPeakStartTime > ACTIVATION_TIME )
   {
      _Crit3aPeakToPeakTriggerTime = _pd->ProcRunTime().getMSecs();
   }

   //
   // Check the slope ...
   //
   if ( _pd->Run().dRGrMd > CobeConfig::data().ChamberSatdRGrMd_3a )
   {
      if ( _Crit3aSlopeStartTime == 0 ) _Crit3aSlopeStartTime = _pd->ProcRunTime().getMSecs();
   }
   else
   {
      _Crit3aSlopeStartTime = 0;
   }

   if (    _Crit3aSlopeStartTime > 0
           && _pd->ProcRunTime().getMSecs() - _Crit3aSlopeStartTime > ACTIVATION_TIME )
   {
      _Crit3aSlopeTriggerTime = _pd->ProcRunTime().getMSecs();
   }
}

void ChamberSaturation::calculateCrit3b ()
{
   if ( !_BaselineCalculated ) return;

   //
   // Check the peak-to-peak value
   //
   float avRGpMdCompValue = RbcDat::data().ChamberSatFsat *
                            CobeConfig::data().ChamberSatFs3b *
                            _pd->Run().RGpMdBaseline;
   if ( _pd->Run().AvRGpMd > avRGpMdCompValue )
   {
      if ( _Crit3bPeakToPeakStartTime == 0 ) _Crit3bPeakToPeakStartTime = _pd->ProcRunTime().getMSecs();
   }
   else
   {
      _Crit3bPeakToPeakStartTime = 0;
   }

   if (    _Crit3bPeakToPeakStartTime > 0
           && _pd->ProcRunTime().getMSecs() - _Crit3bPeakToPeakStartTime > ACTIVATION_TIME )
   {
      _Crit3bPeakToPeakTriggerTime = _pd->ProcRunTime().getMSecs();
   }

   //
   // Check the slope ...
   //
   if ( _pd->Run().dRGrMd > CobeConfig::data().ChamberSatdRGrMd_3b )
   {
      if ( _Crit3bSlopeStartTime == 0 ) _Crit3bSlopeStartTime = _pd->ProcRunTime().getMSecs();
   }
   else
   {
      _Crit3bSlopeStartTime = 0;
   }

   if (    _Crit3bSlopeStartTime > 0
           && _pd->ProcRunTime().getMSecs() - _Crit3bSlopeStartTime > ACTIVATION_TIME )
   {
      _Crit3bSlopeTriggerTime = _pd->ProcRunTime().getMSecs();
   }
}

void ChamberSaturation::calculateCrit4a ()
{
   float avRGpMdCompValue = RbcDat::data().ChamberSatFsat *
                            CobeConfig::data().ChamberSatRgpMd_4a;
   if ( _pd->Run().AvRGpMd > avRGpMdCompValue )
   {
      if ( _Crit4aStartTime == 0 ) _Crit4aStartTime = _pd->ProcRunTime().getMSecs();
   }
   else
   {
      _Crit4aStartTime = 0;
   }
}

void ChamberSaturation::calculateCrit4b ()
{
   float avRGpMdCompValue = RbcDat::data().ChamberSatFsat *
                            CobeConfig::data().ChamberSatRgpMd_4b;
   if (    _pd->Run().AvRGpMd > avRGpMdCompValue )
   {
      if ( _Crit4bStartTime == 0 ) _Crit4bStartTime = _pd->ProcRunTime().getMSecs();
   }
   else
   {
      _Crit4bStartTime = 0;
   }
}

void ChamberSaturation::calculateSampleValues ()
{
   _RedArray[ _SampleIndex ]   = _pd->Status().RedValue();
   _GreenArray[ _SampleIndex ] = _pd->Status().GreenValue();
   _pd->MakeRunWritable();
   _pd->Run().Red   = average(LEDSampleCount, _RedArray);
   _pd->Run().Green = average(LEDSampleCount, _GreenArray);
   _pd->MakeRunReadable();
   if ( _pd->Run().Green.Get() )
      _RGrArray[ _SampleIndex ] = (float)_pd->Run().Red.Get() / (float)_pd->Run().Green.Get();
   else
      _RGrArray[ _SampleIndex ] = 1.0f;
   _SampleIndex = ( _SampleIndex + 1 >= LEDSampleCount ? 0 : _SampleIndex + 1 );
}

void ChamberSaturation::calculateRGr ()
{
   _pd->MakeRunWritable();
   _pd->Run().RGr = average(LEDSampleCount, _RGrArray);
   _pd->MakeRunReadable();
   if ( _pd->Run().DrawCycle )
   {
      _RGrAvgPeakArray[ _RGrAvgPeakIndex ] = _pd->Run().RGr.Get();
      _RGrAvgPeakIndex                     = ( _RGrAvgPeakIndex + 1 >= RatioAveragePeakCount ? 0 : _RGrAvgPeakIndex + 1 );
   }
   _RGrAvgMdArray[ _RGrAvgMdIndex ] = _pd->Run().RGr.Get();
   _RGrAvgMdIndex                   = ( _RGrAvgMdIndex + 1 >= RatioAverageMedianCount ? 0 : _RGrAvgMdIndex + 1 );
}

void ChamberSaturation::calculateHardwareValues ()
{
   if ( _pd->TrimaSet().CmdQch.Get() >= MIN_DELTA_CHAMBER_FLOW )
   {
      _pd->MakeRunWritable();
      _pd->Run().QchChamSat = _pd->TrimaSet().CmdQch.Get();
      _pd->MakeRunReadable();
      if ( _pd->Run().QchChamSat.Get() < MIN_CHAMBER_FLOW * _pd->Predict().Csf(_pd->Run().Substate.Get() ) )
         _QchLastTimeTooLow = _pd->ProcRunTime().getMSecs();

      _QchArray[ _QchIndex ] = _pd->Run().QchChamSat.Get();
      _QchIndex              = ( _QchIndex + 1 >= QchFlowCount ? 0 : _QchIndex + 1 );
   }

   _CentSpeedArray[ _CentSpeedIndex ] = (int)_pd->Status().CentrifugeRPM();
   _CentSpeedIndex                    = ( _CentSpeedIndex + 1 >= CentSpeedCount ? 0 : _CentSpeedIndex + 1 );
}

void ChamberSaturation::calculateRGrMd ()
{
   _pd->MakeRunWritable();
   _pd->Run().RGrMd = median(RatioAverageMedianCount, _RGrAvgMdArray);
   if ( _RGrMdArray[ _RGrMdIndex ] != 0.0f )
      _pd->Run().dRGrMd = _pd->Run().RGrMd / _RGrMdArray[ _RGrMdIndex ];
   else
      _pd->Run().dRGrMd = 0.0f;
   _pd->MakeRunReadable();
   _RGrMdArray[ _RGrMdIndex ] = _pd->Run().RGrMd.Get();
   _RGrMdIndex                = ( _RGrMdIndex + 1 >= RatioMedianCount ? 0 : _RGrMdIndex + 1 );
}

void ChamberSaturation::calculateCrAv ()
{
   _pd->MakeRunWritable();
   _CrAvgArray[ _CrAvgIndex ] = _pd->Run().Cr.Get();
   _CrAvgIndex                = ( _CrAvgIndex + 1 >= CrAverageCount ? 0 : _CrAvgIndex + 1 );
   _pd->Run().CrAv = average(CrAverageCount, _CrAvgArray);
   _pd->MakeRunReadable();
}

void ChamberSaturation::calculateRGp ()
{
   float min = FLT_MAX; float max = FLT_MIN;
   for ( int i = 0 ; i<RatioAveragePeakCount ; i++ )
   {
      if ( min > _RGrAvgPeakArray[ i ] ) min = _RGrAvgPeakArray[ i ];
      if ( max < _RGrAvgPeakArray[ i ] ) max = _RGrAvgPeakArray[ i ];
   }

   _pd->MakeRunWritable();
   _pd->Run().RGp = max - min;
   _pd->MakeRunReadable();
   _RGpArray[ _RGpIndex ] = _pd->Run().RGp.Get();
   _RGpIndex              = ( _RGpIndex + 1 >= RGpSampleCount ? 0 : _RGpIndex + 1 );
   if ( !_RGpArrayFull && _RGpIndex==0 ) _RGpArrayFull = true;
}

void ChamberSaturation::calculateRGpMd ()
{
   //
   // Only calculate the median when the array is full ...
   //
   if ( !_RGpArrayFull ) return;

   _pd->MakeRunWritable();
   _pd->Run().RGpMd = median(RGpSampleCount, _RGpArray);
   _RGpMdArray[ _RGpMdIndex ] = _pd->Run().RGpMd.Get();
   _RGpMdIndex                = ( _RGpMdIndex + 1 >= RGpMedianCount ? 0 : _RGpMdIndex + 1 );
   if ( !_OneMinOfRGpMdData && _RGpMdIndex >= 10 ) _OneMinOfRGpMdData = true;
   if ( !_RGpMdArrayFull && _RGpMdIndex == 0 ) _RGpMdArrayFull = true;

   //
   // Only calculate the Average RGp median after 1 minutes worth of data ...
   //
   if ( _OneMinOfRGpMdData )
   {
      if ( _RGpMdArrayFull )
      {
         _pd->Run().AvRGpMd = average(RGpMedianCount, _RGpMdArray);
      }
      else
      {
         _pd->Run().AvRGpMd = average(_RGpMdIndex, _RGpMdArray);
      }
   }
   else
   {
      _pd->Run().AvRGpMd = 0.0f;
   }
   _pd->MakeRunReadable();
}

void ChamberSaturation::calculateBaselineA ()
{
   //
   // Baseline A ...
   //
   if ( _BaselineACalculated == false )
   {
      //
      // Snapshot Vin at the start of the calculation...
      if ( _BaselineACount == 0 )
         _VinBaselineACalc = _pd->Volumes().Vin;

      float minconc = Software_CDS::GetInstance().getFeature(JapanFeatures) ? MIN_PLT_CONCENTRATION_JAPAN : MIN_PLT_CONCENTRATION;
      if (    (    _BaselineACount > 0                  // we have started to calculate Baseline A
                   || _pd->Run().CrAv.Get() > minconc ) // the concentration is OK to start if we haven't already
              && ( ( _pd->Volumes().Vin - _VinBaselineACalc ) <= CobeConfig::data().ChamberSatVs3a )
              )
      {
         _BaselineASum += _pd->Run().RGpMd.Get();
         _BaselineACount++;
      }
      else if ( _BaselineACount > 0 /* divide by zero check */ )
      {
         _pd->MakeRunWritable();
         _pd->Run().RGpMdBaselineA = _BaselineASum / (float)_BaselineACount;
         _BaselineACalculated = true;
         _pd->MakeRunReadable();
      }
   }
}

void ChamberSaturation::calculateBaselineB ()
{

   // Fix for IT 8443: Reset Baseline B at MidRun if we haven't done so already
   if ( !_BaselineResetAtMidRun && _pd->Run().Substate.Get() == SS_MIDRUN )
   {
      DataLog (log_level_proc_pqi_info) << "ChamberSaturation Notification: Baseline B reset for MidRun" << endmsg;

      resetBaselineB();
      _BaselineResetAtMidRun = true;
   }

   //
   // Baseline B ...
   //
   if ( _BaselineACalculated ) // Only start after baseline A is complete
   {
      float minconc = Software_CDS::GetInstance().getFeature(JapanFeatures) ? MIN_PLT_CONCENTRATION_JAPAN : MIN_PLT_CONCENTRATION;
      if (    (    ( _pd->Run().CrAv.Get() > minconc && !_BaselineCalculated )
                   || _BaselineCalculated )
              && (_pd->Run().QchChamSat.Get() > MIN_CHAMBER_FLOW * _pd->Predict().Csf(_pd->Run().Substate.Get()))
              && getChangeInQch() < MIN_DELTA_CHAMBER_FLOW
              )
      {
         // Fix for IT 8149: We have an additional requirement to recalculate Baseline
         // at the start of MidRun in order to increase it, if need be due to extensive
         // plasma flow.
         if (  ( !_BaselineCalculated
                 && _pd->ProcRunTime().getMSecs() - _BaselineBDelayStartTime > ONE_MIN_MSECS )
               || ( !_BaselineCalculatedMidRun && _pd->Run().Substate.Get() == SS_MIDRUN ) )
         {
            //
            // Capture the time on the first trip through ...
            //
            if ( _BaselineBTime < SUFFICIENTLY_SMALL_FLOAT )
            {
               if ( _pd->Run().Substate.Get() == SS_MIDRUN )
                  DataLog (log_level_proc_pqi_info) << "ChamberSaturation Notification: Baseline recalculation for MidRun started , threshold=" << minconc << endmsg;

               _BaselineBTime = _pd->ProcRunTime().getMins();
            }

            if ( _pd->ProcRunTime().getMins() - _BaselineBTime <= CobeConfig::data().ChamberSatTs3b )
            {
               _BaselineBSum += _pd->Run().RGpMd.Get();
               _BaselineBCount++;

            }
            else if ( _BaselineBCount > 0 /* divide by zero check */ )
            {
               _pd->MakeRunWritable();
               //
               // Calculate baseline B ...
               //
               _pd->Run().RGpMdBaselineB = _BaselineBSum / (float)_BaselineBCount;

               //
               // Set the baseline equal to the larger of the two baseline values ...
               //
               _pd->Run().RGpMdBaseline = ( _pd->Run().RGpMdBaselineA.Get() > _pd->Run().RGpMdBaselineB.Get()
                                            ? _pd->Run().RGpMdBaselineA.Get()
                                            : _pd->Run().RGpMdBaselineB.Get() );

               _BaselineCalculated = true;

               if ( _pd->Run().Substate.Get() == SS_MIDRUN )
                  _BaselineCalculatedMidRun = true;

               _BaselineBSum   = 0.0f;
               _BaselineBCount = 0;
               _BaselineBTime  = 0.0f;

               _pd->MakeRunReadable();
            }
         }
      }
      //
      // Reset Baseline B ...
      //
      else
      {
         resetBaselineB();
      }

   }

}

// This new function was created as a refactor for IT 8443.  The goal is to
// break-out code to reset Baseline B for reuse purposes.
void ChamberSaturation::resetBaselineB ()
{
   //
   // Save the delay start time if we have been disabled during our calculation ...
   //
   _BaselineBDelayStartTime = _pd->ProcRunTime().getMSecs();

   //
   // Baseline B variables need to be reset ...
   //
   _pd->MakeRunWritable();
   _pd->Run().RGpMdBaselineB = 0.0f;
   _pd->MakeRunReadable();
   _BaselineBSum             = 0.0f;
   _BaselineBCount           = 0;
   _BaselineBTime            = 0.0f;
   _BaselineCalculated       = false;
   _BaselineCalculatedMidRun = false;
}

float ChamberSaturation::getChangeInQch ()
{
   float min = FLT_MAX; float max = -FLT_MAX;
   for ( int i = 0 ; i<QchFlowCount ; i++ )
   {
      if ( min > _QchArray[ i ] ) min = _QchArray[ i ];
      if ( max < _QchArray[ i ] ) max = _QchArray[ i ];
   }
   if ( max - min < 0.0f ) return FLT_MAX;
   return ( max - min );
}

int ChamberSaturation::getChangeInCentSpeed ()
{
   int min = INT_MAX; int max = 0;
   for ( int i = 0 ; i<CentSpeedCount ; i++ )
   {
      if ( min > _CentSpeedArray[ i ] ) min = _CentSpeedArray[ i ];
      if ( max < _CentSpeedArray[ i ] ) max = _CentSpeedArray[ i ];
   }
   if ( max - min < 0.0f ) return INT_MAX;
   return ( max - min );
}

int ChamberSaturation::getMinCentSpeed ()
{
   int min = INT_MAX;
   for ( int i = 0 ; i<CentSpeedCount ; i++ )
   {
      if ( min > _CentSpeedArray[ i ] ) min = _CentSpeedArray[ i ];
   }
   return min;
}

int ChamberSaturation::average (int arraySize, const int* array)
{
   if ( arraySize <= 0 ) return 0;
   int sum = 0;
   for ( int i = 0 ; i<arraySize ; i++ )
      sum += array[ i ];
   float avg = ( (float)sum / (float)arraySize ) + 0.5f;
   return ( (int)avg );
}

float ChamberSaturation::average (int arraySize, const float* array)
{
   if ( arraySize <= 0 ) return 0;
   float sum = 0.0f;
   for ( int i = 0 ; i<arraySize ; i++ )
      sum += array[ i ];
   return ( sum / (float)arraySize );
}

int ChamberSaturation::median (int arraySize, int* array)
{
   _IntArray = array; _FloatArray = 0;
   int* indexArray = createIndexArray(arraySize);
   qsort(indexArray, arraySize, sizeof( int ), ChamberSaturation::compare);
   if ( arraySize % 2 == 0 )
   {
      int   sum   = array[ indexArray[ arraySize / 2 - 1 ] ] + array[ indexArray[ arraySize / 2 ] ];
      float value = ( sum / 2.0f ) + 0.5f;
      delete [] indexArray;
      return ( (int)value );
   }
   else
   {
      int value = array[ indexArray[ arraySize / 2 ] ];
      delete [] indexArray;
      return value;
   }
}

float ChamberSaturation::median (int arraySize, float* array)
{
   _IntArray = 0; _FloatArray = array;
   int* indexArray = createIndexArray(arraySize);
   qsort(indexArray, arraySize, sizeof( float ), ChamberSaturation::compare);
   if ( arraySize % 2 == 0 )
   {
      float sum = array[ indexArray[ arraySize / 2 - 1 ] ] + array[ indexArray[ arraySize / 2 ] ];
      delete [] indexArray;
      return sum / 2;
   }
   else
   {
      float value = array[ indexArray[ arraySize / 2 ] ];
      delete [] indexArray;
      return value;
   }
}

int* ChamberSaturation::createIndexArray (int arraySize)
{
   int* indexArray = new int[ arraySize ];
   for ( int i = 0 ; i<arraySize ; i++ )
      indexArray[ i ] = i;
   return indexArray;
}

int ChamberSaturation::compare (const void* index1, const void* index2)
{
   if ( _IntArray )
   {
      int entry1 = _IntArray[ *((int*)index1) ];
      int entry2 = _IntArray[ *((int*)index2) ];

      return ( entry1 < entry2 ? -1 : ( entry1 == entry2 ? 0 : 1 ) );
   }
   else if ( _FloatArray )
   {
      float entry1 = _FloatArray[ *((int*)index1) ];
      float entry2 = _FloatArray[ *((int*)index2) ];

      return ( entry1 < entry2 ? -1 : ( entry1 == entry2 ? 0 : 1 ) );
   }
   return 0;
}

/* FORMAT HASH 2dc06dce62fd9658ea57304ec7a3a1fa */
