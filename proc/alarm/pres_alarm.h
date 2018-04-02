/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pres_alarm.h
 *
 * ABSTRACT:  This object monitors the APS sensor for high and
 *             low pressure ranges
 *
 */

#ifndef _PRES_ALARM_H_
#define _PRES_ALARM_H_

#include "monitorbase.h"
#include "an_alarm.h"
#include "procdata.h"
#include "clearablealarm.h"
#include <list>
#include "timekeeper.h"
#include "objdictionary.h"
#include "trimamessages.h"
#include "config_cds.h"
#include "configdef.h"


#include "timermessage.h"

class PressureAlarm
   : public MonitorBase
{
   DECLARE_OBJ(PressureAlarm);
public:
   PressureAlarm();
   virtual ~PressureAlarm();

   virtual void enable ();

   virtual void Monitor ();

   // Unlatch all latched alarms
   virtual void disable ();

   // global proc data
   ProcData _pd;

protected:

   // These methods will not be implemented.
   PressureAlarm(PressureAlarm&);
   PressureAlarm& operator = (const PressureAlarm&);

protected:

   //
   // Callback for the APS message
   //
   void pumpsStopped ();

   //
   // Function which determines how to set or clear the
   //  appropriate APS alarm
   //
   void updateAPS_old (const float aps, const bool high, const bool low);
   void updateAPS (const float aps, const bool high, const bool low);

   //
   // Unlatch alarms when the sensor reading is within range
   //
   void unlatchAlarms (const float aps, const bool high, const bool low);

   //
   // Function to perform pause functionality.
   //  1 returned if an alarm should occur
   //
   int checkPauseCondition (const float& aps, const bool& high, const bool& low);
   int pauseCondition (const float aps, const bool high, const bool low);
   int pauseConditionInRecovery (const float& aps, const bool& high, const bool& low);

   //
   // Remove all pauses in the list
   //
   void removeAllPauses ();
   void removeAllRecoveryPauses ();

   //
   // Remove pauses from the list which are older than the
   //  time defined to keep them archived
   //
   void removeOldPauses ();

   //
   // Set the pause alarm and add this pause the the list
   //
   void setAutoPauseAlarm (const float aps);
   void clearAutoPauseAlarm ();
   void setAutoPauseRecoveryAlarm (const float& aps);
   void clearAutoPauseRecoveryAlarm ();
   void setSlowingAlarm (const float aps);


   void autoFlowIncrease ();
   void autoFlowDecrease ();


   void rinsebackState ();
   void stopInletRamp ();


   bool inAutoPause ();

   void startVeinRecovery ();      // special recovery for IT-8487
   void endVeinRecoveryTimer () ;  // special recovery for IT-8487

   void startQinTimerAfterRamp ();

   bool isAutoFlowEnabled ();

   bool isSystemInAPSAlarm ();

   bool inCorrectSubstates ();

   void setRecoveryFlag ();

   void setAutoFlow ();

   void disableScheduledFlags ();

   bool isSystemPaused ();

   void toggleAfQinIncreaseTimer ();
   void resumeAutoFlowQinIncreaseTimer ();
   void pauseAutoFlowQinIncreaseTimer ();


protected:

   //
   // Message from HAL indicating the pumps have been
   //  stopped by the control driver.
   //
   APSOutOfBoundsMsg _APSMsg;

   //
   // APS High and low alarms
   //
   anAlarmMsg _APSLowAlarm;
   anAlarmMsg _APSHighAlarm;
   anAlarmMsg _APSDuringPauseAlarm_highpres;
   anAlarmMsg _APSDuringPauseAlarm_lowpres;

   //
   // APS Pause alarm
   //
   ClearableAlarm _APSPumpsSlowAlarm;
   ClearableAlarm _APSPauseAlarm;

   //
   // APS reading from the APS message
   //
   float _OutOfBoundValue;

   //
   // Variable to allow this object to unlatch the low alarm
   //  if it latches the low alarm
   //
   bool _LocallyLatched;

   //
   //   This list contains an entry for each of the pauses that
   //   have occurred in the last defined interval since an actual
   //   APS alarm has occurred.
   //
   list< TimeKeeper* > _Pauses;
   list< TimeKeeper* > _PausesInRecovery;

   //
   // Time of since current active pause alarm was initially set ...
   //
   TimeKeeper _LastPause;
   TimeKeeper _LastPauseInRecovery;

   // wait for vein to recover  after a pause is cleared
   TimerMessage _AdditionalPauseTimer;
   bool         _disarmTimer, _slowAlarmScheduled;


   TimeKeeper              _QincreaseTimer;

   ProcedureAdjustmentMsg* _adjustMsg;
   ProcSubstateChangeMsg   _substateChangeMsg;   // Substate change message

   bool                    _initialQinTimerStarted;

   bool                    _apsLowRecovery;
   bool                    _isAFDecreaseScheduled;
   bool                    _isSystemInRecovery;
   bool                    _isAutoFlowEnabled;
   bool                    _stoppedInletRamp;
   bool                    _isQinIncreaseTimerPaused;

private:

   const Config_CDS* _pConfigCds;




};

#endif

/* FORMAT HASH 7973efe2a79035d5552f7953b4a6d8b3 */
