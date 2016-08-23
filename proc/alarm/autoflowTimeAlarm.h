#ifndef _AUTOFLOW_TIME_ALARM_H_
#define _AUTOFLOW_TIME_ALARM_H_


#include "monitorbase.h"
#include "an_alarm.h"
#include "shw.hpp"
#include "timermessage.h"
#include "objdictionary.h"
#include "trimamessages.h"


class AutoFlowTimeAlarm
   : public MonitorBase
{
   DECLARE_OBJ(AutoFlowTimeAlarm);
public:

   AutoFlowTimeAlarm();
   virtual ~AutoFlowTimeAlarm();

   virtual void Monitor ();

   virtual void enable ();
   virtual void disable ();
   void         showPredictScreen ();


protected:
   void HandleUpdatetimerMsg ();

   float _lastProcTime;
   int   _lastProcNumber;

protected:

   TimerMessage           _predictPopupTimer;
   anAlarmMsg             _timeAlert;
   UpdateAutoFlowTimerMsg _updateTimerMsg;         // Substate change message
};

#endif

/* FORMAT HASH 856ad99552d8b45c60ccc3c79e9e28f4 */
