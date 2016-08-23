/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      dooralarm.cpp
 *
 */

#include "dooralarm.h"
#include "trima_datalog.h"
#include "safproc.h"
#include "procdata.h"
#include "cobeconfig.h"

const int TOO_MANY_UNLOCK_COUNTS = 5;

DEFINE_OBJ(DoorAlarm);

DoorAlarm::DoorAlarm()
   : _DoorLockTimer(0, Callback<DoorAlarm>(this, &DoorAlarm::doorNotLocked), TimerMessage::DISARMED),
     _DoorLockTimerStarted(false),
     _SafetyPowerResponseMsg(),
     _DoorLockAlarm(DOOR_LOCK_ALARM),
     _DoorOpenAlarm(DOOR_OPEN_ALARM)
{}

DoorAlarm::~DoorAlarm()
{
   _SafetyPowerResponseMsg.deregisterMsg();

   if ( _DoorLockTimerStarted )
   {
      _DoorLockTimer.interval(0);
      _DoorLockTimerStarted = false;
   }
}


void DoorAlarm::Monitor ()
{
   ProcData   pd;
   bool       stopDoorLockTimer = true;

   const bool doorIsOpen        = pd.Status().DoorIsOpen();
   const bool doorIsLocked      = pd.Status().DoorIsLocked();

   //
   // If the centrifuge is powered-up
   //
   if (!pd.Status().PS64Vsw.TooLow() ||
       pd.Status().CmdCentrifugeRPM() > 0.0f)
   {
      const HW_DOOR_STATES safetyDoorStatus = pd.SafetyStatus().doorLocks;

      //
      // If the door is open ...
      //
      if (doorIsOpen ||
          (safetyDoorStatus == HW_DOOR_OPEN ||
           safetyDoorStatus == HW_DOOR_LOCKED_STATE)
          )
      {
         DataLog(log_level_proc_alarm_monitor_info) << "Door is open.  DO=" << doorIsOpen
                                                    << "SDO=" << safetyDoorStatus << endmsg;

         disable64VPower();
         _DoorOpenAlarm.setAlarm();
      }
      //
      // ... If the door is closed but unlocked ...
      //
      else if (
         (
            (!doorIsOpen && !doorIsLocked)
            || safetyDoorStatus != HW_DOOR_CLOSED_AND_LOCKED
         )
         &&  !pd.AlarmActive()
         )
      {
         // Announce that we are running the door lock test to the 'others'
         pd.DoorLockTestStarted();

         DataLog(log_level_proc_alarm_monitor_info) << "Door lock test started.  DO=" << doorIsOpen
                                                    << "SDO=" << safetyDoorStatus << endmsg;

         disable64VPower();
         stopDoorLockTimer = false;

         if ( !_DoorLockTimerStarted )
         {
            _DoorLockTimer.interval(6000);
            _DoorLockTimerStarted = true;
         }
      }
   }

   if ( stopDoorLockTimer && _DoorLockTimerStarted )
   {
      _DoorLockTimer.interval(0);
      _DoorLockTimerStarted = false;
   }

   // If we were the ones to turn off 64V, then when the door gets locked, turn it back on.
   if ( pd.DoorLockTestInProgress() &&
        !doorIsOpen &&
        doorIsLocked &&                       // Closed and Locked
        pd.Status().PS64Vsw.TooLow() &&       //
        !pd.AlarmActive() &&                  // alarms on either we never stop the fuge or we did in both cases we dont want to get 64 volts
        (pd.SystemState() < DONOR_DISCONNECT) // no need after this.
        )
   {
      pd.DoorLockTestFinished();
      enable64VPower();
   }
}

void DoorAlarm::enable ()
{
   //
   // Create the safety power messages.
   //
   _SafetyPowerResponseMsg.init(Callback<DoorAlarm>(this, &DoorAlarm::safetyPowerResponse) );
   DataLog(log_level_proc_alarm_monitor_info) << "Create safety power response message" << endmsg;
}

void DoorAlarm::disable ()
{
   _SafetyPowerResponseMsg.deregisterMsg();

   if ( _DoorLockTimerStarted )
   {
      _DoorLockTimer.interval(0);
      _DoorLockTimerStarted = false;
   }
}

void DoorAlarm::safetyPowerResponse ()
{
   int response = _SafetyPowerResponseMsg.getData();

   if ( response == PWR_CONTROL_64V_IGNORED )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "64V Power Request ignored " << response << endmsg;
   }
}

void DoorAlarm::doorNotLocked ()
{
   ProcData             pd;

   const bool           doorIsOpen       = pd.Status().DoorIsOpen();
   const bool           doorIsLocked     = pd.Status().DoorIsLocked();
   const HW_DOOR_STATES safetyDoorStatus = pd.SafetyStatus().doorLocks;

   if ( (!doorIsOpen && !doorIsLocked) ||
        (safetyDoorStatus != HW_DOOR_CLOSED_AND_LOCKED)
        )
   {
      DataLog(log_level_proc_alarm_monitor_info) << "Door lock test started.  DO=" << doorIsOpen
                                                 << "SDO=" << safetyDoorStatus << " DL=" << doorIsLocked << endmsg;

      disable64VPower();
      _DoorLockAlarm.setAlarm();
   }

   _DoorLockTimer.interval(0);
   _DoorLockTimerStarted = false;
}

void DoorAlarm::disable64VPower ()
{
   RequestSafetyPowerMsg request(MessageBase::SEND_GLOBAL);
   request.send(DISABLE_64V_POWER);
   DataLog(log_level_proc_alarm_monitor_info) << "Request 64V Power Off " << DISABLE_64V_POWER << endmsg;
}

void DoorAlarm::enable64VPower ()
{
   RequestSafetyPowerMsg request(MessageBase::SEND_GLOBAL);
   request.send(ENABLE_64V_POWER);
   DataLog(log_level_proc_alarm_monitor_info) << "Request 64V Power On " << ENABLE_64V_POWER << endmsg;
}

/* FORMAT HASH ea1a784f63aeaf0f2bf4e64bf42d8c30 */
