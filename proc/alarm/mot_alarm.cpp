/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mot_alarm.cpp
 *
 */

#include "mot_alarm.h"
#include <math.h>
#include "trima_datalog.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "halstatus_cds.h"
#include <periodic_log.h>

const int MAX_OUT_OF_POSITION_COUNT = 20;

DEFINE_OBJ(MotorAlarm);

MotorAlarm::MotorAlarm()
   : _ValveFaultAlarm(VALVE_HARDWARE_FAILURE),
     _CollectValveAlarm(COLLECT_VALVE_FAILURE),
     _PlasmaValveAlarm(PLASMA_VALVE_FAILURE),
     _RBCValveAlarm(RBC_VALVE_FAILURE),
     _RBCValveCount(0),
     _CollectValveCount(0),
     _PlasmaValveCount(0),
     _CurrentACCommand(0.0f),
     _CurrentCollectCommand(0.0f),
     _CurrentInletCommand(0.0f),
     _CurrentPlasmaCommand(0.0f),
     _CurrentReturnCommand(0.0f),
     _ACPumpTimer(0, Callback<MotorAlarm>(this, &MotorAlarm::pumpsNotUpToSpeed), TimerMessage::DISARMED),
     _InletPumpTimer(0, Callback<MotorAlarm>(this, &MotorAlarm::pumpsNotUpToSpeed), TimerMessage::DISARMED),
     _PlasmaPumpTimer(0, Callback<MotorAlarm>(this, &MotorAlarm::pumpsNotUpToSpeed), TimerMessage::DISARMED),
     _PlateletPumpTimer(0, Callback<MotorAlarm>(this, &MotorAlarm::pumpsNotUpToSpeed), TimerMessage::DISARMED),
     _ReturnPumpTimer(0, Callback<MotorAlarm>(this, &MotorAlarm::pumpsNotUpToSpeed), TimerMessage::DISARMED),
     _plasmaMHFCounter(4)
{

   for (int i = 0; i < MotorFaultAlarmCount; i++)
   {
      _MotorFaultAlarmP[i] = NULL;
   }
   _MotorFaultAlarmP[0] = new anAlarmMsg(MOTOR_FAILED_TO_MAINTAIN_SPEED);
   _MotorFaultAlarmP[1] = new anAlarmMsg(MOTOR_INTEGRATION_ERROR);
   _MotorFaultAlarmP[2] = new anAlarmMsg(MOTOR_VOLUME_ERROR);
   _MotorFaultAlarmP[3] = new anAlarmMsg(MOTOR_DISABLED);
}


MotorAlarm::~MotorAlarm()
{
   for (int i = 0; i < MotorFaultAlarmCount; i++)
   {
      delete _MotorFaultAlarmP[i];
      _MotorFaultAlarmP[i] = NULL;
   }
}

void MotorAlarm::Monitor ()
{
   ProcData pd;

   if ( pd.Status().PS24Vsw.Acceptable() )
   {
      if ( ( pd.Status().RBCValve.State() != pd.Status().RBCValve.Command() ) &&
           ( pd.Status().RBCValve.Command() != HAL_VALVE_UNKNOWN) )
      {
         _RBCValveCount++;
         if ( _RBCValveCount > MAX_OUT_OF_POSITION_COUNT )
         {

            if (_RBCValveAlarm.getState() != ACTIVE)
            {
               _RBCValveAlarm.setAlarm();
               DataLog(log_level_proc_alarm_monitor_info) << "Error: RBC valve out of pos count - " << _RBCValveCount << endmsg;
            }
         }
      }
      else
      {
         _RBCValveCount = 0;
      }

      if ( ( pd.Status().CollectValve.State() != pd.Status().CollectValve.Command() ) &&
           ( pd.Status().CollectValve.Command() != HAL_VALVE_UNKNOWN ) )
      {
         _CollectValveCount++;
         if ( _CollectValveCount > MAX_OUT_OF_POSITION_COUNT )
         {
            if (_CollectValveAlarm.getState() != ACTIVE)
            {
               _CollectValveAlarm.setAlarm();
               DataLog(log_level_proc_alarm_monitor_info) << "Error: Collect valve out of pos count - " << _CollectValveCount << endmsg;
            }
         }
      }
      else
      {
         _CollectValveCount = 0;
      }

      if ( ( pd.Status().PlasmaValve.State() != pd.Status().PlasmaValve.Command() ) &&
           ( pd.Status().PlasmaValve.Command() != HAL_VALVE_UNKNOWN ) )
      {
         _PlasmaValveCount++;
         if ( _PlasmaValveCount > MAX_OUT_OF_POSITION_COUNT)
         {

            if (_PlasmaValveAlarm.getState() != ACTIVE)
            {
               _PlasmaValveAlarm.setAlarm();
               DataLog(log_level_proc_alarm_monitor_info) << "Error: Plasma valve out of pos count - " << _PlasmaValveCount << endmsg;
            }
         }
      }
      else
      {
         _PlasmaValveCount = 0;
      }
   }

   int index = 0;
   enum HalPump_CDS::PumpError pumpError = pd.Status().InletPump.Error();

   if ( pumpError != HalPump_CDS::PumpError_NoFault)
   {
      index = (int)(log2((int)pumpError));
      _MotorFaultAlarmP[index]->setAlarm();

      DataLog(log_level_proc_alarm_monitor_info) << "Inlet Pump FAILURE error = " << pumpError << endmsg;
   }
   pumpError = pd.Status().ACPump.Error();
   if ( pumpError != HalPump_CDS::PumpError_NoFault )
   {
      index = (int)(log2((int)pumpError));
      _MotorFaultAlarmP[index]->setAlarm();

      DataLog(log_level_proc_alarm_monitor_info) << "AC Pump FAILURE error = " << pumpError << endmsg;
   }
   pumpError = pd.Status().ReturnPump.Error();
   if ( pumpError != HalPump_CDS::PumpError_NoFault )
   {
      index = (int)(log2((int)pumpError));
      _MotorFaultAlarmP[index]->setAlarm();

      DataLog(log_level_proc_alarm_monitor_info) << "Return Pump FAILURE error = " << pumpError << endmsg;
   }
   pumpError = pd.Status().PlasmaPump.Error();
   if ( (pumpError != HalPump_CDS::PumpError_NoFault) && !plasmaMHFIgnore())
   {
      index = (int)(log2((int)pumpError));
      _MotorFaultAlarmP[index]->setAlarm();

      DataLog(log_level_proc_alarm_monitor_info) << "Plasma Pump FAILURE error = " << pumpError << endmsg;
   }
   pumpError = pd.Status().CollectPump.Error();
   if ( pumpError != HalPump_CDS::PumpError_NoFault )
   {
      index = (int)(log2((int)pumpError));
      _MotorFaultAlarmP[index]->setAlarm();

      DataLog(log_level_proc_alarm_monitor_info) << "Collect Pump FAILURE error = " << pumpError << endmsg;
   }

   if ( pd.Status().ValveFault() )
   {
      _ValveFaultAlarm.setAlarm();
   }

   //
   // Check actual vs. commanded cent speed ...
   //
   checkActualVsCommand();

}

void MotorAlarm::pumpsNotUpToSpeed ()
{
   if ( _ACPumpTimer.timerArmed() )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "AC - ";
      _ACPumpTimer.interval(0);
   }
   if ( _InletPumpTimer.timerArmed() )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Inlet - ";
      _InletPumpTimer.interval(0);
   }
   if ( _PlasmaPumpTimer.timerArmed() )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Plasma - ";
      _PlasmaPumpTimer.interval(0);
   }
   if ( _PlateletPumpTimer.timerArmed() )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Platelet - ";
      _PlateletPumpTimer.interval(0);
   }
   if ( _ReturnPumpTimer.timerArmed() )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Return - ";
      _ReturnPumpTimer.interval(0);
   }
   DataLog(log_level_proc_alarm_monitor_info) << "not maintaining speed" << endmsg;

   PeriodicLog::forceOutput();

   int index = (int)( log2((int)(HalPump_CDS::PumpError_FailureToMaintainSpeed)) );
   _MotorFaultAlarmP[index]->setAlarm();
}

void MotorAlarm::checkActualVsCommand ()
{
   ProcData pd;

   checkActualVsCommand(pd.Status().ACPump,      _ACPumpTimer,       _CurrentACCommand);
   checkActualVsCommand(pd.Status().InletPump,   _InletPumpTimer,    _CurrentInletCommand);
   checkActualVsCommand(pd.Status().PlasmaPump,  _PlasmaPumpTimer,   _CurrentPlasmaCommand);
   checkActualVsCommand(pd.Status().CollectPump, _PlateletPumpTimer, _CurrentCollectCommand);
   checkActualVsCommand(pd.Status().ReturnPump,  _ReturnPumpTimer,   _CurrentReturnCommand);
}

void MotorAlarm::checkActualVsCommand (const HalPump_CDS& pump,
                                       TimerMessage& timer,
                                       float& currentCommand)
{
   //
   // If any of the commands change ...
   //
   if ( currentCommand != pump.CmdFlow() )
   {
      //
      // Save the changed command...
      currentCommand = pump.CmdFlow();

      //
      // Reset and turn off the timer ...
      //
      timer.interval(0);
   }
   else
   {
      //
      // Check the commanded vs. the actual pump speeds ...
      //
      bool outOfBounds = pumpOutOfTolerance(pump);

      //
      // If the pump was out of tolerance, but is now in tolerance,
      //  then reset the associated timer ...
      //
      if ( timer.timerArmed() && !outOfBounds )
      {
         timer.interval(0);
      }
      //
      // If any pump is out-of-tolerance, start the timer ...
      //
      else if ( outOfBounds )
      {
         if ( !timer.timerArmed() )
         {
            //
            // Start the timer if we are not up to speed on all the pumps ...
            //
            timer.interval(10000 /*milliseconds*/);   // 10 seconds
         }
      }
   }
}

bool MotorAlarm::pumpOutOfTolerance (const HalPump_CDS& pump)
{
   float cmd        = fabs(pump.CmdFlow() );
   float difference = fabs(pump.CmdFlow() - pump.ActFlow() );
   if (    cmd >= CobeConfig::data().MinimumPumpRPM
           && (    difference > ( cmd * 0.06 )
                   && difference > CobeConfig::data().MinimumPumpRPM
                   )
           )
   {
      return true;
   }

   return false;
}

/* IT 7720 - Verify MHF - When the plasma pump is commanded from roughly 20-30ml/min to < 1.0ml/min, we get this
   error. This is due to a very large decrease in the commanded speed. This happens when we transition from PCA to
   Midrun. Under those conditions, it is a false positive alarm. The fix at the driver level did not work as exp-
   ected. Hence this workaround. See IT 8999 for context.
*/
bool MotorAlarm::plasmaMHFIgnore ()
{
   ProcData pd;
   if (_plasmaMHFCounter > 0)
   {
      float cmdFlow  = pd.Status().PlasmaPump.CmdFlow();
      float cmdRatio = 1.0f;
      if (cmdFlow > 0.0f)
         cmdRatio = pd.Run().LastCommandedQp.Get() / cmdFlow;
      bool largeCmdRatio = ( cmdRatio > CobeConfig::data().PlasmaLargeCmdRatio) /* from dlogs > 30 is more conservative*/;
      bool inMidrun      = (pd.Run().Substate.Get() == SS_MIDRUN);
      bool failSpeed     = (pd.Status().PlasmaPump.Error() == HalPump_CDS::PumpError_FailureToMaintainSpeed);
      if (largeCmdRatio & inMidrun & failSpeed)
      {
         _plasmaMHFCounter--;
         DataLog(log_level_proc_alarm_monitor_info) << " Ignored Plasma MHF Alarm - "
                                                    << pd.Status().PlasmaPump.Error()
                                                    << " Counter - " << _plasmaMHFCounter << endmsg;
         return true;
      }
   }
   return false;
}

/* FORMAT HASH 9d32312303c68b0bc3778d5bb09ff047 */
