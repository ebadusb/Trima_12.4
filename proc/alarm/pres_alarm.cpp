/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pres_alarm.cpp
 *
 */


#include <periodic_log.h>
#include "pres_alarm.h"
#include "trima_datalog.h"
#include "cobeconfig.h"
#include "recoverytypes.h"
#include "software_cds.h"


DEFINE_OBJ(PressureAlarm);


#define TIMESTAMP    " ( " << _pd.GetAbsTimeNowinMinutes() << " )/ ( " << _pd.Status().APS() <<  "; p_cnt-> " << _Pauses.size()   << " )"


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////  CONTROL DATA  ////////////////////////////////////////////////////////////////////////

// Once we have paused, the pumps will not resume until the pressure reaches
// both the nominal range and we have paused for the minimum amount of time.
const float MinPauseTime = CobeConfig::data().ApsMinimumPauseTime;            // 1.0 sec

// Once we have paused, the pumps will not resume until the pressure reaches
// both the nominal range and we have paused for the minimum amount of time.
const float APSNominalPressure = (-1.0 * CobeConfig::data().ApsNominalPressure);   // 50.0 mmhg in CC but neg will be applied

//  A pause is only allowed to last this long before the alarm is
//  generated.  This quantity is in SECONDS.  This value may be set to
//  zero, in which case the APS pause function is inactive (the alarm
//  will occur immediately when the APS is detected out of range).
const float TimeBeforeAlarm = CobeConfig::data().ApsPauseTimeBeforeAlarm;       // 6.0 sec

// check if pause can stop due to slight positive pressure indicating squeezing
const float APSPositivePressure = CobeConfig::data().ApsPositivePressureCheck;   // 10.0 mmhg

// Define the period during which we keep track of pauses.  This
// quantity is in MINUTES.  For example, if it is set to 1.5 then
// the APS alarm will occur if MORE than aps_max_pauses_in_period
// occur within any 90-second timeframe.
const float PausePeriod       = CobeConfig::data().ApsPausePeriod;           // 3.0 sec
const float MaxPausesInPeriod = CobeConfig::data().ApsMaxPausesInPeriod;     // 3 count


const int VEIN_TIMER          = int(1000.0f * CobeConfig::data().ApsAdditionalDelay); // in millisecs of course (

const int AUTOFLOW_INCR_TIMER = int(1000.0f * 60.0f * CobeConfig::data().AutoflowIncreaseTimer); // in millisecs of course



//
//
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

PressureAlarm::PressureAlarm()
   : _APSMsg(Callback<PressureAlarm>(this, &PressureAlarm::pumpsStopped)),
     _APSLowAlarm(APS_TOO_LOW),
     _APSHighAlarm(APS_TOO_HIGH_ALERT),

     _APSDuringPauseAlarm_highpres(APS_DURING_PAUSE_HI),
     _APSDuringPauseAlarm_lowpres(APS_DURING_PAUSE_LOW),

     _APSPumpsSlowAlarm(APS_PUMPS_SLOW),
     _APSPauseAlarm(APS_PAUSE),
     _OutOfBoundValue(0.0f),
     _LocallyLatched(false),
     _Pauses(),
     _LastPause(),
     _disarmTimer(false),
     _slowAlarmScheduled(false),
     _pConfigCds(new Config_CDS(ROLE_RO)),
     _AdditionalPauseTimer(0, Callback<PressureAlarm>(this, &PressureAlarm::clearAutoPauseAlarm), TimerMessage::DISARMED),
     _QincreaseTimer(0, Callback<PressureAlarm>(this, &PressureAlarm::autoFlowIncrease), TimerMessage::DISARMED),
     _allowAutoIncreases(false),
     _allowAutoDecreases(false),
     _autoFlow_On(false),
     _shouldStopInletRamp(false),
     _adjustMsg(0),
     _apsLowRecovery(false),
     _isAFDecreaseScheduled(false),
     _isSystemInRecovery(false),
     _isAutoFlowEnabled(false)
{}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
PressureAlarm::~PressureAlarm()
{
   delete _pConfigCds;
   _pConfigCds = 0;

   delete _adjustMsg;
   _adjustMsg = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::Monitor ()
{
   bool  high       = false;
   bool  low        = false;
   float currentAPS = 0.0f;

   if ( _OutOfBoundValue != 0.0f )
   {
      DataLog(log_level_proc_alarm_monitor_info) <<  "_OutOfBoundValue = "   << _OutOfBoundValue << endmsg;

      currentAPS       = _OutOfBoundValue; // from HAL -- APS reading that stopped the pumps
      _OutOfBoundValue = 0.0f;

      high             = ( currentAPS > 0.0f );
      low              = ( currentAPS <= 0.0f );
   }
   else
   {
      currentAPS = _pd.Status().APS();
      high       = ( currentAPS >= _pd.Status().APSHigh() );

      // IT 13082 - if the aps low alarm is latched, the aps has to rise above a set minimum
      // for 0.5 sec (1 cycle) before it's considered recovered
      if ( _APSLowAlarm.getState() != LATCHED )
      {
         // the alarm isn't latched so do the usual
         low = ( currentAPS <= _pd.Status().APSLow()  );
      }
      else
      {
         // if it is latched, check if the pressure has stayed recovered for 1 cycle
         if (currentAPS > APSNominalPressure)
         {
            if (_apsLowRecovery)
            {
               // the pressure has been over the minimum for at least 1 full cycle
               low = false;
            }
            else
            {
               // the pressure just went above the minimum so it hasn't officially recovered
               low = true;
            }

            _apsLowRecovery = true;
         }
         else
         {
            // pressure is below the minimum for a recovery
            // if it had been high enough but dropped, the counter will have to start over
            low             = true;
            _apsLowRecovery = false;
         }
      }
   }

   // Set the Recovery Flag
   setRecoveryFlag();

   // special timer only if VEIN_TIMER > 0.0f
   endVeinRecoveryTimer();

   // Set the flags for AF increase/decrease only if AF is enabled
   if (_isAutoFlowEnabled)
   {
      updateAutoflowAdjustFlags();
   }

   updateAPS(currentAPS, high, low);

   if ( _autoFlow_On == true )
   {
      startQinTimerAfterRamp();
   }

}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::updateAPS (const float aps, const bool high, const bool low)
{
   ///  see if we can do the slowing yet.... ///////////
   if (!_isAutoFlowEnabled)
   {
      if ( _slowAlarmScheduled
           && (_pd.Status().APS() >= APSNominalPressure)
           && _LastPause.getSecs() > MinPauseTime
           )
      {
         setSlowingAlarm(aps);
      }
   }
   else
   {
      
     // Decrease flow only if:
      // - System is not in recovery
      // - AF decrease is true (This when set to true has already satisfied inCorrectSubstates()) 
      if (_isAFDecreaseScheduled && !_isSystemInRecovery)
      {
         autoFlowDecrease();
         _isAFDecreaseScheduled = false;
      }
   }

   unlatchAlarms(aps, high, low);

   // Check and remove pauses if they are older than 3 mins
   removeOldPauses();

   //  if it is in AKO or saline bolus
   bool IsSalineOrAKO = (  _pd.AlarmActive()  &&                            // AKO is in progress or
                              ( _pd.Status().ReturnPump.CmdRPM() == CobeConfig::data().QrpAKO
                              || ( _pd.Status().ReturnPump.CmdRPM() > 0.0f
                                  && RecoveryTypes::RecoveryId(_pd.RecoveryName().c_str() ) == RecoveryTypes::SalineBolus )
                              )
                             );

   bool outOfRange           = ( high || low );

   bool okTolookForAPSAlarms = ( !_pd.Run().DrawCycle.Get() || IsSalineOrAKO || isSystemInAPSAlarm() );

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //

   if ( (outOfRange && okTolookForAPSAlarms) || pauseCondition(aps, high, low)  )
   {
      bool isPumpsSlowAlarmClear = true;

      if (!_isAutoFlowEnabled)
      {
         isPumpsSlowAlarmClear = (_APSPumpsSlowAlarm.getState() == CLEARED);
      }

      if (      low
                && isPumpsSlowAlarmClear
                && inAutoPause()
                && _Pauses.size() >= MaxPausesInPeriod
                && inCorrectSubstates()
                )
      {
         if (!_isAutoFlowEnabled)
         {
            if (!_slowAlarmScheduled)        // log it the first time...
            {
               DataLog(log_level_proc_alarm_monitor_info)
                  << "schedule a slowing alarm process for when the aps recovers to -50 : "
                  << TIMESTAMP <<  endmsg;

               PeriodicLog::forceOutput();
            }
            _slowAlarmScheduled = true;

         }
         else
         {
            // Schedule a decrease if not in recovery
            if (!_isSystemInRecovery)
            {
               DataLog(log_level_proc_alarm_monitor_info)
                  << "schedule a AF decrease for when the aps recovers to -50 : "
                  << TIMESTAMP <<  endmsg;

               PeriodicLog::forceOutput();

               _isAFDecreaseScheduled = true;
            }
         }

      }
      else
      {
         // DataLog(log_level_proc_alarm_monitor_info) << "Latch Check: " << TIMESTAMP <<  endmsg;
         //
         //  ... then check the current value and set the appropriate alarm
         //  for operator intervention.
         //
         if (    _pd.AlarmActive()
                 && _APSHighAlarm.getState() != LATCHED
                 && _APSLowAlarm.getState()  != LATCHED
                 && (
                    (_APSDuringPauseAlarm_highpres.getState() != CLEARED &&  _APSDuringPauseAlarm_lowpres.getState() != CLEARED)
                    || _pd.AlarmQ().highestDisplay() <= DISP_FLOATING ) // floating, R1, Operator, CFR11, None
                 )
         {
            if (high)
            {
               DataLog(log_level_proc_alarm_monitor_info) << "Latching APS During Pause Alarm highpres" << TIMESTAMP <<  endmsg;
               _APSDuringPauseAlarm_highpres.latchAlarm();
            }
            else if (low)
            {
               DataLog(log_level_proc_alarm_monitor_info) << "Latching APS During Pause Alarm lowpres" << TIMESTAMP <<  endmsg;
               _APSDuringPauseAlarm_lowpres.latchAlarm();
            }
         }
         else if ( high )
         {
            // Only latch when not latched
            if (_APSHighAlarm.getState() != LATCHED)
            {
               _APSHighAlarm.latchAlarm();
            }
         }
         else
         {
            // Only latch when not latched
            if (_APSLowAlarm.getState() != LATCHED)
            {
               DataLog(log_level_proc_alarm_monitor_info) << "Latching LOCAL APS " << TIMESTAMP <<  endmsg;
               _APSLowAlarm.latchAlarm();
               _LocallyLatched = true;
            }
         }

         clearAutoPauseAlarm();

         // special timer only if VEIN_TIMER > 0.0f
         endVeinRecoveryTimer();
      }

      // If AutoFlow is disabled, clear all pauses as manual intervention required
      if (!_isAutoFlowEnabled)
      {
         removeAllPauses();
      }
      else if (!inCorrectSubstates() && !high)
      {
         // If not in correct substate no need to maintain count
         // But APS High cannot reset counter that could be set
         // in previous substate
         removeAllPauses();
      }

      DataLog(log_level_proc_alarm_monitor_info) << "APS=" << aps
                                                 << " limits=" << _pd.Status().APSHigh()
                                                 << " " << _pd.Status().APSLow()
                                                 << TIMESTAMP << endmsg;
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

int PressureAlarm::pauseCondition (const float aps, const bool high, const bool low)
{

   int doAlarm = 0;

   if ( low )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: APS Low: " << "aps->" << aps << TIMESTAMP << endmsg;
      if ( !inAutoPause() )
      {
         setAutoPauseAlarm(aps);
      }

   }
   else if ( high )
   {

      DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: APS Hi: " << "aps->" << aps << TIMESTAMP << endmsg;
      doAlarm = 1;

   }

   if ( _Pauses.size() >= MaxPausesInPeriod )
   {

      DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: more than ApsMaxPausesInPeriod: " << MaxPausesInPeriod << " ."  << TIMESTAMP << endmsg;
      doAlarm = 1;

   }
   else if ( inAutoPause() && ( _LastPause.getSecs() > TimeBeforeAlarm ) )

   {


      DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: Six Seconds is up: "
                                                 << "aps = "
                                                 << aps
                                                 << ", Last Pause = "
                                                 << _LastPause.getSecs()
                                                 << TIMESTAMP << endmsg;

      /////////////////////// IT 11181 ////////////////////////////////////////////////
      // if aps GT -50mmHg
      if ( aps > APSNominalPressure )
      {

         DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: Pressure recovered to " << TIMESTAMP << endmsg;
         doAlarm = 0;
         clearAutoPauseAlarm();

      }
      else
      {

         DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: Pressure NOT recovered alarm to follow.  APS:  " << TIMESTAMP << endmsg;
         doAlarm = 1;
      }


      if (_autoFlow_On == false)
      {
         _slowAlarmScheduled = false;
      }

      /////////////////////// IT 11181 ////////////////////////////////////////////////
   }
   else if (   inAutoPause()
               && ( aps >= APSPositivePressure )
               && (_LastPause.getSecs() > MinPauseTime) )
   {

      DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: Pressure recovered, pause is up: :"
                                                 << "aps = "
                                                 << aps
                                                 << ", Last Pause = "
                                                 << _LastPause.getSecs()
                                                 << TIMESTAMP << endmsg;
      doAlarm = 0;

      if (_autoFlow_On == false) _slowAlarmScheduled = false;  // disable the slow if it doesnt recover in time
      //
      clearAutoPauseAlarm();


   }
   ////////////////////////////////////////////////////////////////////////////////////
   //
   // Return the pause condition
   //
   return doAlarm ;

}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::unlatchAlarms (const float,
                                   const bool high,
                                   const bool low)
{
   //
   // Check to see if the high or low pressure alarms are currently
   //  active.  If they are then we only need to check on whether to
   //  unlatch the given alarm.  The operator can clear the alarm once
   //  the pressure goes inside the alarm limits.
   //
   if ( _APSHighAlarm.getState() == LATCHED )
      if ( !high)
         _APSHighAlarm.unlatchAlarm();


   if ( _APSLowAlarm.getState() == LATCHED )
      if ( _LocallyLatched && !low )
      {
         _APSLowAlarm.unlatchAlarm();
         _LocallyLatched = false;
      }


   if ( ( _APSDuringPauseAlarm_highpres.getState() == LATCHED ) && !high)
   {
      DataLog(log_level_proc_alarm_monitor_info) << "UnLatching  APS APS During Pause Alarm highpres" << TIMESTAMP <<  endmsg;
      _APSDuringPauseAlarm_highpres.unlatchAlarm();
   }

   if ( ( _APSDuringPauseAlarm_lowpres.getState() == LATCHED ) && !low)
   {
      DataLog(log_level_proc_alarm_monitor_info) << "UnLatching  APS APS During Pause Alarm lowpres" << TIMESTAMP <<  endmsg;
      _APSDuringPauseAlarm_lowpres.unlatchAlarm();
   }


}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::removeAllPauses ()
{
   // DataLog(log_level_proc_alarm_monitor_info) << "Remove All Pause history: number removed=" << _Pauses.size() << "; "  << TIMESTAMP << endmsg;
   list< TimeKeeper* >::iterator pause;

   for ( pause = _Pauses.begin() ; pause != _Pauses.end() ; pause++ )
      delete *pause;

   _Pauses.erase(_Pauses.begin(), _Pauses.end() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
void PressureAlarm::removeOldPauses ()
{

   //  Parse my list of any pauses outside of the defined maximum
   //  limit.  Since new entries are added to the end of the list,
   //  the first entry is the oldest.
   //
   if ( _Pauses.size() > 0 )
   {
      list< TimeKeeper* >::iterator oldestTime;
      oldestTime = _Pauses.begin();
      if ( ( ((TimeKeeper*)*oldestTime)->getMins() ) > PausePeriod )
      {
         _Pauses.remove(*oldestTime);   // removes entry from list

         DataLog(log_level_proc_alarm_monitor_info) << "Remove Old Pauses from history:"  << ((TimeKeeper*)*oldestTime)->getMins()
                                                    << ", "  << TIMESTAMP << endmsg;

         delete *oldestTime;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
void PressureAlarm::setAutoPauseAlarm (const float aps)
{

   DataLog(log_level_proc_alarm_monitor_info) << "*** Setting AutoPause Alarm: " << aps  << TIMESTAMP << endmsg;
   _APSPauseAlarm.setAlarm();
   _LastPause.init();
   _LastPause.activate();

   // Add the pause ...
   //
   TimeKeeper* newPause = new TimeKeeper;
   newPause->activate();
   _Pauses.push_back(newPause);

   // Log the limits ...
   //
   DataLog(log_level_proc_alarm_monitor_info) << "APS=" << aps
                                              << " limits=" << _pd.Status().APSHigh()
                                              << " " << _pd.Status().APSLow() << TIMESTAMP
                                              << endmsg;

   PeriodicLog::forceOutput();
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::clearAutoPauseAlarm ()
{
   // DataLog(log_level_proc_alarm_monitor_info) << "*** Clearing AutoPause Alarm: " << TIMESTAMP  << endmsg;
   _APSPauseAlarm.clearAlarm();
   _LastPause.inactivate();
   _LastPause.init();
   _disarmTimer = true;

   PeriodicLog::forceOutput();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

bool PressureAlarm::inAutoPause ()
{
   if (_APSPauseAlarm.getState() == CLEARED)
      return false;
   else
      return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::setSlowingAlarm (const float aps)
{
   if (_autoFlow_On == false)
   {
      DataLog(log_level_proc_alarm_monitor_info) << " entering the slowing alarm process: " << TIMESTAMP <<  endmsg;
      // Set the pumps slow alarm ...
      //
      clearAutoPauseAlarm();
      removeAllPauses();
      _APSPumpsSlowAlarm.setAlarm();
      _slowAlarmScheduled = false;
   }
   else
   {
      DataLog(log_level_proc_alarm_monitor_info) << " ERROR!! AUTOFLOW should not entering the slowing alarm process: " << TIMESTAMP <<  endmsg;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::disable ()
{

   DataLog(log_level_proc_alarm_monitor_info) << " PressureAlarm monitor disabled: " << TIMESTAMP  << endmsg;
   MonitorBase::disable();

   unlatchAlarms(0.0f, 0, 0);
   clearAutoPauseAlarm();

   _AdditionalPauseTimer.interval(0.0f);
   _QincreaseTimer.interval(0.0f);


}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::enable ()
{
   // refresh my feature flag
   _autoFlow_On = (bool)(_pd.Config().Procedure.Get().key_autoflow);
   // Set the AutoFlow feature flag
   setAutoFlow();

   DataLog(log_level_proc_alarm_monitor_info) << " PressureAlarm monitor enabled: " << TIMESTAMP  << endmsg;
   DataLog(log_level_proc_alarm_monitor_info) << "  is AutoFlow enabled? " << _isAutoFlowEnabled  << endmsg;

   MonitorBase::enable();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::pumpsStopped ()
{
   _OutOfBoundValue = (float)_APSMsg.getData();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::startVeinRecovery ()
{
   // Additional Timer Code that is disabled in 6.1
   // start timer... but only once!
   if (_AdditionalPauseTimer.timerArmed() != TimerMessage::ARMED && VEIN_TIMER > 0.0f)
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Start the Vein recovery timer. Timer set to "
                                                 << (VEIN_TIMER / 1000.0f)
                                                 << " sec.  "
                                                 << TIMESTAMP
                                                 << endmsg;

      _AdditionalPauseTimer.interval(VEIN_TIMER);
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::endVeinRecoveryTimer ()
{

   // Additional Timer Code that is disabled in 6.1
   if (_disarmTimer && VEIN_TIMER > 0.0f)
   {
      _disarmTimer = false;

      // disable the additional pause timer
      DataLog(log_level_proc_alarm_monitor_info) << "End the Vein recovery timer " << TIMESTAMP << endmsg;
      _AdditionalPauseTimer.interval(0.0f);
   }

}



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::autoFlowDecrease ()
{
   DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Auto Decrease Qin: " << TIMESTAMP <<  endmsg;

   stopInletRamp();

   clearAutoPauseAlarm();
   removeAllPauses();

   if (!_adjustMsg)
   {
      DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: Creating the procedure adjustment message" << endmsg;
      _adjustMsg = new ProcedureAdjustmentMsg(MessageBase::SEND_LOCAL);
   }

   DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: Sending ProcedureAdjustmentMsg " << endmsg;
   _adjustMsg->send(AUTOFLOW_QIN_DECREASE);

   // reset the timer for increase.
   _QincreaseTimer.interval(AUTOFLOW_INCR_TIMER);
   DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer reset. (post Qin decrement) " << TIMESTAMP <<  endmsg;

   DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: Current Proc Time =  " << _pd.RunTargets().ProcTimeTarget.Get() << endmsg;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::autoFlowIncrease ()
{
   if (!_isAutoFlowEnabled)
      return;

   if (!_allowAutoIncreases)
      return;

   DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Auto Increase Qin: " << TIMESTAMP <<  endmsg;
   if (!_adjustMsg)
   {
      DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: Creating the procedure adjustment message" << endmsg;
      _adjustMsg = new ProcedureAdjustmentMsg(MessageBase::SEND_LOCAL);
   }

   DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: Sending ProcedureAdjustmentMsg " << endmsg;
   _adjustMsg->send(AUTOFLOW_QIN_INCREASE);

   // reset timer to ten minutes
   _QincreaseTimer.interval(AUTOFLOW_INCR_TIMER);
   DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer reset. (post Qin increase)" << TIMESTAMP <<  endmsg;

   DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: Current Proc Time =  " << _pd.RunTargets().ProcTimeTarget.Get() << endmsg;

}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
void PressureAlarm::stopInletRamp ()
{

   if (!_shouldStopInletRamp )
      return;

   State_names ss     =   _pd.Run().Substate.Get();

   static bool _block = false;

   if (  !_block  &&
         ( (ss == SS_PREPLATELET_PLASMA) ||  (ss == SS_PREPLATELET_NOPLASMA) )
         )
   {
      float qin = _pd.Predict().Qin(1, ss);

      _pd.MakeRunWritable();
      _pd.Run().stopRamp.Set(true);
      _pd.Run().deadRampMaxQin.Set(qin - 5.0f);      // get actual Qin draw -5ml/Min
      _pd.MakeRunReadable();

      _block = true;

      if (!_QincreaseTimer.timerArmed())
      {
         _QincreaseTimer.interval(AUTOFLOW_INCR_TIMER);
         DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer started. (ramp stopped by AF) " << TIMESTAMP <<  endmsg;
      }

      DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Stop inlet ramping. Qin to cap at " << _pd.Run().deadRampMaxQin.Get() <<  endmsg;
   }

}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::updateAutoflowAdjustFlags ()
{
   if (!(_pd.Config().Procedure.Get().key_autoflow))
      return;

   State_names ss =  _pd.Run().Substate.Get();

   switch (ss)
   {
      case SS_CHANNEL_SETUP :
         _allowAutoIncreases  = false;
         _allowAutoDecreases  = false;
         _shouldStopInletRamp = false;
         break;

      case SS_PREPLATELET_PLASMA :
      case SS_PREPLATELET_NOPLASMA :
         _allowAutoIncreases  = false;
         _allowAutoDecreases  = true;
         _shouldStopInletRamp = true;
         if ( !(_pConfigCds->Predict.Get().key_inlet_flow_ramp) )
         {
            if (!_QincreaseTimer.timerArmed())
            {
               _QincreaseTimer.interval(AUTOFLOW_INCR_TIMER);
               DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer started. (post prime, ramp off-1)" << TIMESTAMP <<  endmsg;
            }
         }
         break;

      case SS_PLATELET_PLASMA :
      case SS_PCA :
      case SS_MIDRUN :
      case SS_PIR_PLASMA :
      case SS_PIR_NOPLASMA :
      case SS_EXTENDED_PCA :
      case SS_EXTENDED_PLASMA :
         _allowAutoIncreases  = true;
         _allowAutoDecreases  = true;
         _shouldStopInletRamp = false;
         if ( !(_pConfigCds->Predict.Get().key_inlet_flow_ramp) )
         {
            if (!_QincreaseTimer.timerArmed())
            {
               _QincreaseTimer.interval(AUTOFLOW_INCR_TIMER);
               DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer started. (post prime, ramp off)" << TIMESTAMP <<  endmsg;
            }
         }
         break;

      default :
         _allowAutoIncreases  = false;
         _allowAutoDecreases  = false;
         _shouldStopInletRamp = false;
         break;
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::rinsebackState ()
{
   if (_pd.Run().RinsebackStarted.Get())
   {
      _allowAutoIncreases = false;
      _allowAutoDecreases = false;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::startQinTimerAfterRamp ()
{

   if (!(_pd.Config().Procedure.Get().key_autoflow))
      return;

   float Vincv = CobeConfig::data().MinVinCollectStart;
   float Vinr  = (Vincv < CobeConfig::data().VinEndOfQinRamp ? Vincv : CobeConfig::data().VinEndOfQinRamp);
   float Vin   = _pd.Volumes().Vin.Get();

   if ( Vin >= Vinr)
   {

      static bool logItOnce = true;
      if (logItOnce && !_pd.Run().stopRamp.Get())
      {
         logItOnce = false;
         DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin ramping completed. " << TIMESTAMP <<  endmsg;
      }
      if (!_QincreaseTimer.timerArmed())
      {
         _QincreaseTimer.interval(AUTOFLOW_INCR_TIMER);
         DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer started. (post ramp) " << TIMESTAMP <<  endmsg;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   setRecoveryFlag ()
///
///   Set the recovery flag if system is in any of recovery modes
///   
///   @param None
///   @return void
////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::setRecoveryFlag ()
{
   if (RecoveryTypes::RecoveryId(_pd.RecoveryName().c_str()) == RecoveryTypes::None)
   {
      _isSystemInRecovery = false;
   }
   else
   {
      _isSystemInRecovery = true;
   }
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   setAutoFlow ()
///
///   Set the value of AutoFlow Feature flag from features.bin
///   This basically sets the _isAutoFlowEnabled flag which will be used 
///   throughout pressure alarm 
///   
///   @param None
///   @return void
////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::setAutoFlow ()
{
   _isAutoFlowEnabled = Software_CDS::GetInstance().getFeature(AutoFlowFlag);
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   isSystemInAPSAlarm ()
///
///   This function checks if there are any active alarms. The condition differs
///   if autoflow is enabled.
///   
///   @param None
///   @return bool - true if any of the alarms is active, false otherwise
////////////////////////////////////////////////////////////////////////////////
bool PressureAlarm::isSystemInAPSAlarm ()
{
   bool inAPSPresAlarm = false;

   inAPSPresAlarm = (
      _APSHighAlarm.getState() != CLEARED ||
      _APSLowAlarm.getState() != CLEARED  ||
      (_APSDuringPauseAlarm_highpres.getState() != CLEARED &&
       _APSDuringPauseAlarm_lowpres.getState() != CLEARED)
      );

   if (!_isAutoFlowEnabled)
   {
      return ((_APSPumpsSlowAlarm.getState() != CLEARED ) || inAPSPresAlarm);
   }
   else
   {
      return inAPSPresAlarm;
   }
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   inCorrectSubstates ()
///
///   This function verifies the system is in correct substate to schedule
///   a pump slow alarm or an AutoFlow decrease when AutoFlow feature is 
///   enabled.
///   
///   @param   None
///   @return  bool - true if system is in valid substate, false otherwise
////////////////////////////////////////////////////////////////////////////////   

bool PressureAlarm::inCorrectSubstates ()
{
   State_names subState =  _pd.Run().Substate.Get();

   if (_isAutoFlowEnabled)
   {
      return (_pd.Status().CassIsPltPlsRBC() &&
                 // Check if we are in valid sub-states
                 // Exception states are PIR states
                 (subState > SS_CHANNEL_SETUP &&
                  subState != SS_PIR_PLASMA &&
                  subState != SS_PIR_NOPLASMA &&
                  subState < SS_RBC_PTF_SETUP_1)
              );
   }
   else
   {
      return ( _pd.Status().CassIsPltPlsRBC() &&
                 subState < SS_RBC_PTF_SETUP_1);
   }
}

/* FORMAT HASH 9fff9f1134959345225571be5747e6d3 */
