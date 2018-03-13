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
     _QincreaseTimer(),
     _autoFlow_On(false),
     _initialQinTimerStarted(false),
     _adjustMsg(0),
     _apsLowRecovery(false),
     _isAFDecreaseScheduled(false),
     _isSystemInRecovery(false),
     _isAutoFlowEnabled(false),
     _stoppedInletRamp(false),
     _isQinIncreaseTimerPaused(false)
{
   _QincreaseTimer.notifier(Callback<PressureAlarm>(this, &PressureAlarm::autoFlowIncrease) );
   _QincreaseTimer.interval(AUTOFLOW_INCR_TIMER);
   _QincreaseTimer.inactivate();
}

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

   updateAPS(currentAPS, high, low);

   if (_isAutoFlowEnabled)
   {
      startQinTimerAfterRamp();
      toggleAfQinIncreaseTimer();
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

      // Proceed for AF decrease only if:
      // - System is not in recovery
      // - AF decrease is true
      if (_isAFDecreaseScheduled &&
          !_isSystemInRecovery)
      {
         // From 1 sec to less than 6 secs
         if (_LastPause.getSecs() > MinPauseTime &&
             _LastPause.getSecs() < TimeBeforeAlarm)
         {
            // If we have positve pressure, proceed with Qin decrease
            if (_pd.Status().APS() >= APSPositivePressure)
            {
               DataLog(log_level_proc_alarm_monitor_info) << "Pressure recovered, Qin decrease " << endmsg;
               autoFlowDecrease();
               _isAFDecreaseScheduled = false;
            }
         }// If pressure does not recover above nominal in 6 sec
         else if (_LastPause.getSecs() >= TimeBeforeAlarm)
         {
            // If the pressure has recovered over nominal, proceed with Qin decrease
            if (_pd.Status().APS() > APSNominalPressure)
            {
               DataLog(log_level_proc_alarm_monitor_info) << "Pressure recovered to nominal, Qin decrease " << endmsg;
               autoFlowDecrease();
               _isAFDecreaseScheduled = false;
            }
            else
            {
               // Pressure has not recovered above nominal and time is up
               // Latch the low alarm
               if ( _APSLowAlarm.getState() != LATCHED )
               {
                  DataLog(log_level_proc_alarm_monitor_info) << "_APSLowAlarm latched" << endmsg;
                  _APSLowAlarm.latchAlarm();
               }
               // Set this to true when we latch the alarm
               _LocallyLatched = true;
            }
         }
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
            // Existing condition when AF is disabled
            if (!_isAutoFlowEnabled)
            {
               if (_APSLowAlarm.getState() != LATCHED)
               {
                  DataLog(log_level_proc_alarm_monitor_info) << "Latching LOCAL APS " << TIMESTAMP <<  endmsg;
                  _APSLowAlarm.latchAlarm();
                  _LocallyLatched = true;
               }
            }
            else
            {  // Autoflow is enabled here
               if (inAutoPause())
               {
                  // If we exceed 3 APS low and we are in
                  // substate where AF is not supposed to work
                  // latch the aps low alarm
                  if (_Pauses.size() >= MaxPausesInPeriod && !inCorrectSubstates())
                  {
                     if (_APSLowAlarm.getState() != LATCHED)
                     {
                        DataLog(log_level_proc_alarm_monitor_info) << "Latching LOCAL APS " << TIMESTAMP <<  endmsg;
                        _APSLowAlarm.latchAlarm();
                        _LocallyLatched = true;
                     }
                  }
                  // We exceed 6 seconds latch the APS low Alarm
                  else if (_LastPause.getSecs() >= TimeBeforeAlarm)
                  {
                     if (_APSLowAlarm.getState() != LATCHED)
                     {
                        DataLog(log_level_proc_alarm_monitor_info) << "Latching LOCAL APS " << TIMESTAMP <<  endmsg;
                        _APSLowAlarm.latchAlarm();
                        _LocallyLatched = true;
                     }
                  }
               }
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

   // If AF in enabled and decrease is scheduled then we could hit this
   // immediately in next monitor cycle.
   // We do not want to alarm again if we are going into Qin decrease.
   if (_Pauses.size() >= MaxPausesInPeriod &&
       !_isAFDecreaseScheduled)
   {
      DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: more than ApsMaxPausesInPeriod: " << MaxPausesInPeriod << " ."  << TIMESTAMP << endmsg;
      doAlarm = 1;
   }
   else if (inAutoPause())
   {
      // Case where low pause counter are less than 3
      // Check between 1 and 6 sec after AutoPause,
      // we are ok to check for other conditions
      if (_LastPause.getSecs() > MinPauseTime &&
          _LastPause.getSecs() < TimeBeforeAlarm)
      {
         // Check if pressure has recovered to positive
         if (aps >= APSPositivePressure)
         {
            DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: Pressure recovered to Positive " << TIMESTAMP << endmsg;

            // Since pressure has recovered with 1 sec do not alarm
            doAlarm = 0;
            clearAutoPauseAlarm();
            disableScheduledFlags();
         }
      }
      else if (_LastPause.getSecs() >= TimeBeforeAlarm)
      {
         // 6 Seconds have passed, check if pressure above nominal
         if ( aps > APSNominalPressure )
         {
            // The pressure has recovered to nominal of -50 mmHg so its ok to proceed
            // Clear the APS low
            DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: Pressure recovered to " << TIMESTAMP << endmsg;
            doAlarm = 0;
            clearAutoPauseAlarm();
         }
         else
         {
            // The pressure has not recovered to nominal after 6 secs
            // But we already incremented the counter, so remove the last pause
            // If Qin decrease scheduled, disable it
            DataLog(log_level_proc_alarm_monitor_info) << "pauseCondition :: Pressure NOT recovered alarm to follow.  APS:  " << TIMESTAMP << endmsg;
            doAlarm = 1;
            _Pauses.pop_back();
            _isAFDecreaseScheduled = false;
         }

         disableScheduledFlags();
      }
   }

   if (doAlarm)
   {
      DataLog(log_level_proc_alarm_monitor_info) << "auto-pause ConditionCheck returns alarm = " << doAlarm << " ::" << TIMESTAMP <<  endmsg;
   }

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
   {
      if ( _LocallyLatched && !low )
      {
         DataLog(log_level_proc_alarm_monitor_info) << "Unlatching _APSLowAlarm " <<  endmsg;
         _APSLowAlarm.unlatchAlarm();
         _LocallyLatched = false;
      }
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
   if (inAutoPause())
   {
      _APSPauseAlarm.clearAlarm();
      _LastPause.inactivate();
      _LastPause.init();
      _disarmTimer = true;
      if(_initialQinTimerStarted && !_isQinIncreaseTimerPaused)
      {
         _QincreaseTimer.init();
         DataLog(log_level_qa_external) << "AutoFlow: Qin increase timer reset. (at clear of APS AutoPause) " << TIMESTAMP <<  endmsg;
      }
   }

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
   _QincreaseTimer.init();


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

   // AF down should only be allowed in certain substates
   // This check is required to avoid potential race condition
   // when states transition after AF down is scheduled.
   if (!inCorrectSubstates())
   {
      return;
   }

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
   _QincreaseTimer.init();
   DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer reset. (post Qin decrement) " << TIMESTAMP <<  endmsg;

   DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: Current Proc Time =  " << _pd.RunTargets().ProcTimeTarget.Get() << endmsg;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::autoFlowIncrease ()
{
   // Function call is via a timer, check if AF enabled else return
   if (!_isAutoFlowEnabled)
   {
      return;
   }

   // AF up should only be allowed in certain substates
   if (!inCorrectSubstates())
   {
      return;
   }

   DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Auto Increase Qin: " << TIMESTAMP <<  endmsg;
   if (!_adjustMsg)
   {
      DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: Creating the procedure adjustment message" << endmsg;
      _adjustMsg = new ProcedureAdjustmentMsg(MessageBase::SEND_LOCAL);
   }

   if ((_pd.RunTargets().ProcTimeTarget.Get() - _pd.ProcRunTime().getMins()) > CobeConfig::data().AutoflowMinUpTime)
   {
      DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: Sending ProcedureAdjustmentMsg " << endmsg;
      _adjustMsg->send(AUTOFLOW_QIN_INCREASE);
   }
   else
   {
      DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: ignoring up adjust near end of procedure" << endmsg;
   }

   // reset timer to five minutes
   _QincreaseTimer.init();
   DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer reset. (post Qin increase)" << TIMESTAMP <<  endmsg;

   DataLog (log_level_proc_alarm_monitor_info) << "AutoFlow: Current Proc Time =  " << _pd.RunTargets().ProcTimeTarget.Get() << endmsg;

}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
void PressureAlarm::stopInletRamp ()
{

   State_names ss     =   _pd.Run().Substate.Get();

   if (  !_stoppedInletRamp  &&
         ( (ss == SS_PREPLATELET_PLASMA) ||  (ss == SS_PREPLATELET_NOPLASMA) )
         )
   {
      float qin = _pd.Predict().Qin(1, ss);

      _pd.MakeRunWritable();
      _pd.Run().stopRamp.Set(true);
      _pd.Run().deadRampMaxQin.Set(qin - 5.0f);      // get actual Qin draw -5ml/Min
      _pd.MakeRunReadable();

      _stoppedInletRamp = true;

      if (!_QincreaseTimer.activated() && !_isQinIncreaseTimerPaused)
      {
         _initialQinTimerStarted = true;
         _QincreaseTimer.init();
         _QincreaseTimer.activate();
         DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer started. (ramp stopped by AF) " << TIMESTAMP <<  endmsg;
      }

      DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Stop inlet ramping. Qin to cap at " << _pd.Run().deadRampMaxQin.Get() <<  endmsg;
   }

}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::rinsebackState ()
{
   if (_pd.Run().RinsebackStarted.Get())
   {
      // AF increase/decrease not allowed
   }
}
////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   startQinTimerAfterRamp ()
///
///   Start autoflow increase timer when inlet ramp completed or terminated early.
///
///   @param None
///   @return void
////////////////////////////////////////////////////////////////////////////////

void PressureAlarm::startQinTimerAfterRamp ()
{
   if(!_initialQinTimerStarted)
   {
      float Vincv = CobeConfig::data().MinVinCollectStart;
      float Vinr  = (Vincv < CobeConfig::data().VinEndOfQinRamp ? Vincv : CobeConfig::data().VinEndOfQinRamp);
      float Vin   = _pd.Volumes().Vin.Get();
      static bool logItOnce = true;

      if ( Vin >= Vinr)
      {
         if (logItOnce && !_pd.Run().stopRamp.Get())
         {
            logItOnce = false;
            DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin ramping completed. " << TIMESTAMP <<  endmsg;
         }
         if (!_QincreaseTimer.activated() && !_isQinIncreaseTimerPaused)
         {
            _initialQinTimerStarted = true;
            _QincreaseTimer.init();
            _QincreaseTimer.activate();
            DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer started. (post ramp) " << TIMESTAMP <<  endmsg;
         }
      }
      else
      {
         if(_pd.Run().stopRamp.Get())
         {
            if (logItOnce)
            {
               logItOnce = false;
               DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin ramping terminated early. " << TIMESTAMP <<  endmsg;
            }
            if (!_QincreaseTimer.activated() && !_isQinIncreaseTimerPaused)
            {
               _initialQinTimerStarted = true;
               _QincreaseTimer.init();
               _QincreaseTimer.activate();
               DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer started. (ramp terminated early) " << TIMESTAMP <<  endmsg;
            }
         }
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

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   disableScheduledFlags ()
///
///   This function:
///   For AF On - Disables _isAFDecreaseScheduled
///   For AF OFF - Disables _slowAlarmScheduled
///
///   @param   None
///   @return  void
////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::disableScheduledFlags ()
{
   if (_isAutoFlowEnabled)
   {
      _isAFDecreaseScheduled = false;
   }
   else
   {
      _slowAlarmScheduled = false;
   }
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   isSystemPaused ()
///
///   This function:
///   Checks for condition when all pumps are paused and system is in alarm
///
///   @param   None
///   @return  true - Above conditions are met
///            false - otherwise
////////////////////////////////////////////////////////////////////////////////
bool PressureAlarm::isSystemPaused ()
{
   if (_pd.AlarmActive() &&
      _pd.Status().ACPump.ActFlow()      == 0.0f &&
      _pd.Status().CollectPump.ActFlow() == 0.0f &&
      _pd.Status().InletPump.ActFlow()   == 0.0f &&
      _pd.Status().PlasmaPump.ActFlow()  == 0.0f &&
      _pd.Status().ReturnPump.ActFlow()  == 0.0f)
   {
      return true;
   }

   return false;
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   toggleAfQinIncreaseTimer ()
///
///   This function:
///   Toggles the Qin increase timer. If the system is paused, the timer is paused
///   Once the system resumes, the timer is started back from where it was paused.
///
///   @param   None
///   @return  void
////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::toggleAfQinIncreaseTimer ()
{
   if (isSystemPaused())
   {
      // Pause the timer
      if (!_isQinIncreaseTimerPaused)
      {
         _isQinIncreaseTimerPaused = true;
         pauseAutoFlowQinIncreaseTimer();
      }
   }
   else
   {
      // Resume the timer
      if (_isQinIncreaseTimerPaused)
      {
         _isQinIncreaseTimerPaused = false;
         resumeAutoFlowQinIncreaseTimer();
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   pauseAutoFlowQinIncreaseTimer ()
///
///   This function:
///   Paused the Qin Increase timer for AutoFlow
///
///   @param   None
///   @return  void
////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::pauseAutoFlowQinIncreaseTimer ()
{
   if (_initialQinTimerStarted)
   {
      _QincreaseTimer.inactivate();
      DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer pause " << TIMESTAMP <<  endmsg;
   }
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   resumeAutoFlowQinIncreaseTimer ()
///
///   This function:
///   Resumes the Qin Increase timer for AutoFlow
///
///   @param   None
///   @return  void
////////////////////////////////////////////////////////////////////////////////
void PressureAlarm::resumeAutoFlowQinIncreaseTimer ()
{
   if (_initialQinTimerStarted)
   {
      _QincreaseTimer.activate();
      DataLog(log_level_proc_alarm_monitor_info) << "AutoFlow: Qin increase timer resumed." << TIMESTAMP <<  endmsg;
   }
}

/* FORMAT HASH 9fff9f1134959345225571be5747e6d3 */
