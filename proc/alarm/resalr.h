/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      resalr.h
 *             Performs a check on the reservoir volumes and alarms where
 *             appropriate.  Provides the switch to return sound.  May
 *             force a cycle switch.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

#if !defined( INCLUDE_RESALR )
#define INCLUDE_RESALR

#include "monitorbase.h"
#include "sounds.hpp"
#include "an_alarm.h"
#include "trima_datalog.h"
#include "objdictionary.h"
#include "trimamessages.h"
#include "clearablealarm.h"
#include "timermessage.h"


class ReservoirAlarm
   : public MonitorBase
{
   DECLARE_OBJ(ReservoirAlarm);

protected:
   typedef enum
   {
      DRAW,
      RETURN
   } DRAW_OR_RETURN;

   typedef enum
   {
      TOO_LONG,
      TOO_SHORT
   } LONG_OR_SHORT;

public:
   ReservoirAlarm();
   virtual ~ReservoirAlarm();

   virtual void Initialize ();

   virtual void Monitor ();

protected:

   void DoFirstDrawCheck ();
   void DoDrawCheck ();
   void DoReturnCheck ();
   void systemStateReceived ();

   void setResAlr (DRAW_OR_RETURN drawOrReturn, LONG_OR_SHORT longOrShort);

   void clearTimer ();
protected:

   // Message to inform safety that proc is switching
   //  to return after a 'return cycle too long'.  (Safety
   //  will shut proc down for running the AC pump during
   //  a return cycle.
   MidCycleSwitchMsg  _MsgMidCycleSwitch;

   StatLineRequestMsg _showStatLineMsg;

   // Reservoir Alarms
   anAlarmMsg _AlrReservoirSensor;
   anAlarmMsg _AlrReturnTooLong_A1;
   anAlarmMsg _AlrFirstCycleTooSoon;
   anAlarmMsg _AlrFirstCycleTooLate;

   // The following alarms need three different versions
   // depending on the disposable set.  The difference is
   // whether the graphic has one, three, or zero reservoir
   // clamps.
   anAlarmMsg _AlrDrawTooShortThreeClamp;
   anAlarmMsg _AlrDrawTooLongThreeClamp;
   anAlarmMsg _AlrReturnTooShortThreeClamp;
   anAlarmMsg _AlrReturnTooLongThreeClamp;

   anAlarmMsg _AlrDrawTooShortOneClamp;
   anAlarmMsg _AlrDrawTooLongOneClamp;
   anAlarmMsg _AlrReturnTooShortOneClamp;
   anAlarmMsg _AlrReturnTooLongOneClamp;
   anAlarmMsg _AlrDrawTooLongRbcPlOneClamp;

   anAlarmMsg _AlrDrawTooShortNoClamp;
   anAlarmMsg _AlrDrawTooLongNoClamp;
   anAlarmMsg _AlrReturnTooShortNoClamp;
   anAlarmMsg _AlrReturnTooLongNoClamp;
   anAlarmMsg _AlrDrawTooLongRbcPlNoClamp;


   // Unrecoverable alarms to stop the run if too many
   //   reservoir errors occur in a cycle.
   anAlarmMsg _AlrTooManyResDraw;
   anAlarmMsg _AlrTooManyResReturn;

   // Unrecoverable alarm to stop the run if the return
   // cycle did not evacuate the reservoir
   anAlarmMsg _AlrResFullAfterReturn;

   // Flag used to relay the condition that all
   //  valves are in the 'return' position in which
   // case we blind some reservoir alarms
   //
   int _BlindCondition;

   // flag for first volume limit reached
   int _InnerLimit;

   // flag based upon system state
   int                  _CheckingActive;
   SystemStateChangeMsg _StateChange;

   // flag to let us know if the cent slowed down during
   // a specific time for our new A1 alarm (IT5946)
   bool _CentSlowDown;

   // flag to let us know if we have had a 1st cycle too
   // late alarm for our new A1 alarm (IT5946 && IT6369)
   bool _firststCycleTooLate;

   // addjust for the bigger return size due to a bolus prime
   bool  _inBolusReturn;
   bool  _inWbcPurge;
   bool  _adjustLimits;
   float _theCleaningClycle;

   // IT 9161 compare the draw and return too shorts to see if flagging is needed.
   float          _drawTooShortVolume;
   float          _returnTooShortVolume;
   bool           _disableCycleMatchCheck;

   TimerMessage   _SilentPauseTimer;

   ClearableAlarm _SilentPauseForRTS;

   float          _RTSCompareVolume;
   bool           _QueueRTS;
   bool           _checkRTSVolm;

};
#endif

/* FORMAT HASH 9d6775dd04eb52ed1b9e745738fda071 */
