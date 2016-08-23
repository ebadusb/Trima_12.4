#include "autoflowTimeAlarm.h"
#include "trima_datalog.h"
#include "safproc.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "predict_msgs.h"


DEFINE_OBJ(AutoFlowTimeAlarm);

AutoFlowTimeAlarm::AutoFlowTimeAlarm()
   : _timeAlert(AUTOFLOW_TIME_ALERT),
     _lastProcTime(0.0f),
     _lastProcNumber(0),
     _updateTimerMsg(Callback<AutoFlowTimeAlarm>(this, &AutoFlowTimeAlarm::HandleUpdatetimerMsg), MessageBase::SNDRCV_RECEIVE_ONLY),
     _predictPopupTimer(0, Callback<AutoFlowTimeAlarm>(this, &AutoFlowTimeAlarm::showPredictScreen), TimerMessage::DISARMED)
{}

AutoFlowTimeAlarm::~AutoFlowTimeAlarm()
{}

void AutoFlowTimeAlarm::showPredictScreen ()
{

   ProcData pd;

   DataLog(log_level_proc_alarm_monitor_info)
      << "Procedure Time Exceeds Max Proc Time,"
      << (float)pd.Config().Procedure.Get().key_proc_time
      << " minutes.  Predict screen coming up as alert.  "
      << endmsg;

   _predictPopupTimer.interval(0.0f);

   DataLog(log_level_proc_alarm_monitor_info) << "send  ShowTimeoutPredictScreen"   << endmsg;
   ShowTimeoutPredictScreen bringUpPredictGui(MessageBase::SEND_LOCAL);
   bringUpPredictGui.send(1);

}

void AutoFlowTimeAlarm::Monitor ()
{

   ProcData   pd;
   const bool AUTOFLOW_ON = (bool)(pd.Config().Procedure.Get().key_autoflow);
   if (AUTOFLOW_ON == false)
   {
      return;
   }

   float       max_time    = (float)pd.Config().Procedure.Get().key_proc_time;
   float       time        =  pd.RunTargets().ProcTimeTarget.Get();
   int         pnum        =  pd.RunTargets().ProcNumber.Get();

   static bool oneTimeOnly = true;
   if (time > max_time)
   {
      if (oneTimeOnly)
      {
         oneTimeOnly = false;
         DataLog(log_level_proc_alarm_monitor_info) << "Force the Predict screen " << time << ";  Max Proc Time config'ed = " << max_time << endmsg;

         _predictPopupTimer.interval(1000.0f * 1.0f);
      }

      return;
   }
   else
   {
      oneTimeOnly = true;
   }

   // initial selection
   if ( (_lastProcNumber == 0) && (pnum != 0) )
   {
      _lastProcNumber = pnum;
      _lastProcTime   = time;
      DataLog(log_level_proc_alarm_monitor_info) << "initial selection time=" << _lastProcTime << endmsg;
   }
   // new product
   else if ( _lastProcNumber != pnum )
   {
      _lastProcNumber = pnum;
      _lastProcTime   = time;
      DataLog(log_level_proc_alarm_monitor_info) << "new product new time=" << _lastProcTime  << endmsg;
   }
   else
   {
      if (_lastProcTime < (time - 10.0f) )
      {
         _timeAlert.setAlarm();
         DataLog(log_level_proc_alarm_monitor_info) << "AutoFlowTimeAlarm set; lastProcTime="
                                                    << _lastProcTime
                                                    << ";  current proc time=" << time <<  endmsg;

         // reset for another ten...
         _lastProcNumber = pnum;
         _lastProcTime   = time;
      }

   }

}

void AutoFlowTimeAlarm::HandleUpdatetimerMsg ()
{

   ProcData pd;
   int      type =  _updateTimerMsg.getData();

   if (_lastProcNumber == pd.RunTargets().ProcNumber.Get())
   {
      DataLog(log_level_proc_alarm_monitor_info) << "update selection proc time = "
                                                 << _lastProcTime
                                                 << "; current = "
                                                 << pd.RunTargets().ProcNumber.Get()
                                                 << ";  type=" << type
                                                 << endmsg;

      _lastProcTime = pd.RunTargets().ProcTimeTarget.Get();
   }

}


void AutoFlowTimeAlarm::enable ()
{}

void AutoFlowTimeAlarm::disable ()
{}

/* FORMAT HASH 74551353623303487c5eaec7ccdd01de */
