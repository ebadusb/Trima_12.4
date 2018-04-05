/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      alarm_h.cpp
 *
 */

#include "alarms.h"

// includes
#include "alarmqueue.h"
#include "alarm_h.h"
#include "cobeconfig.h"
#include "sounds.hpp"
#include "stmain.h"
#include "trima_datalog.h"

#include <algorithm>

#define RETOCC_PING_TIME 60000

MAIN(AlarmHandler);

AlarmHandler::AlarmHandler()
   : _AlarmTable(),
     _DisplayedAlarm(),
     _GUIsAlive(false),
     _SafetysAlive(false),
     _IncomingAlarmMsg(Callback<AlarmHandler>(this, &AlarmHandler::incomingAlarm), MessageBase::SNDRCV_LOCAL),
     _AlarmResponseMsg(Callback<AlarmHandler>(this, &AlarmHandler::alarmResponse), MessageBase::SNDRCV_RECEIVE_ONLY),
     _GUIsAliveMsg(Callback<AlarmHandler>(this, &AlarmHandler::guisAlive)),
     _SafetysAliveMsg(Callback<AlarmHandler>(this, &AlarmHandler::safetysAlive)),
     _systemState(DEFAULT_STATE),
     _systemStateMsg(Callback<AlarmHandler>(this, &AlarmHandler::HandleSystemStateChangeMsg), MessageBase::SNDRCV_LOCAL),
     _RetOccMonitorResponseMsg(Callback<AlarmHandler>(this, &AlarmHandler::retOccMonitorAlive), MessageBase::SNDRCV_LOCAL),
     _ProcedureStatusCheck(Callback<AlarmHandler>(this, &AlarmHandler::procStatusCheck), MessageBase::SNDRCV_LOCAL),
     _EndRunMsg(Callback<AlarmHandler>(this, &AlarmHandler::endRunRequested) ),
     _RetOccMonitorResponseTimer(RETOCC_PING_TIME, Callback<AlarmHandler>(this, &AlarmHandler::retOccMonitorCheck), TimerMessage::DISARMED),
     _DisplayRequestMsg(MessageBase::SEND_LOCAL),
     _OverrideRequestMsg(),
     _setCds(ROLE_RO),
     _runCds(ROLE_RW),
     _RetOccMonitorAlive(false),
     _RetOccMonitorCheckActive(false),
     _RetOccMonitorCheckRetry(0),
     _setStatusLineMsg(MessageBase::SEND_LOCAL),
     _clearStatusLineMsg(MessageBase::SEND_LOCAL),
     _RetOccMonitorAliveTime(0)
{}

AlarmHandler::~AlarmHandler()
{}

ALARM_VALUES AlarmHandler::Init (void)
{

   _DisplayedAlarm = _AlarmTable[NULL_ALARM];

   //
   // Start the alarm handler timer ...
   //
   _RetOccMonitorResponseMsg.send(0);

   DataLog(log_level_alarm_handler_info) << "Alarm Handler initialized OK" << endmsg;
   return NULL_ALARM;
}

void AlarmHandler::incomingAlarm ()
{
   //
   // Start off with a clean alarm slate ...
   //
   _OverrideRequestMsg.clear();

   Alarm_struct data  = _IncomingAlarmMsg.getData();

   ALARM_VALUES name  = data.alarm_name;
   ALARM_STATES state = data.alarm_state;

   //
   // Find the alarm in the alarm table.
   //
   AlarmStruct& alarm = _AlarmTable[name];


   if ( //
        // Only non-displayed alarms can be cleared without
        //  a response from GUI ...
        //
      ( (state == CLEARED) &&
        ( (alarm.getDisplayType() == DISP_NONE) ||
          (alarm.getDisplayType() == DISP_STATUS_LINE) )
      ) ||
      //
      // All other requests to set alarm state can be
      //  set here, assuming the alarm is activated
      //  at the current time ...
      //
      ( state != CLEARED &&
        alarm.activated() )
      )
   {
      //
      // Set the alarm state ...
      //
      alarm.setState(state);

      //
      // Process the alarm ...
      //
      processRequest(alarm);
   }
}

void AlarmHandler::alarmResponse ()
{
   //
   // Start off with a clean alarm slate ...
   //
   _OverrideRequestMsg.clear();

   bool error = true;

   _ResponseData = _AlarmResponseMsg.getData();

   ALARM_VALUES    name     = _ResponseData.alarm_name;
   ALARM_RESPONSES response = _ResponseData.alarm_response;


   //
   // Find the alarm in the alarm table.
   //
   AlarmStruct& alarm = _AlarmTable[name];

   // initialize the error flag to false now ...
   //
   error = false;

   if ( response == CONFIRM )
   {
      //
      // GUI has displayed an alarm.  Are we in synch???
      //
      if ( _DisplayedAlarm.getName() != name )
      {
         sendAlarmToGUI();
      }
   }
   else if ( alarm.getType() == A2 )
   {
      //
      // We don't receive any responses for A2s ...
      //
      error = true;
   }
   else
   {
      //
      // Process the alarm response ...
      //
      processResponse(alarm);
   }

   if (error)
   {
      DataLog(log_level_alarm_handler_error) << "Responded Alarm not found: name->" << name
                                             << ", response->" << response << endmsg;

      alarmHandlerError();
   }
}

void AlarmHandler::guisAlive ()
{
   AlarmTable::iterator foundAlarm = _AlarmTable.end();

   if (!_GUIsAlive)
   {
      DataLog(log_level_alarm_handler_info) << "GUI is alive." << endmsg;

      //
      // Find the highest display level alarm which is  NOT CLEARED.
      //
      foundAlarm = _AlarmTable.findFirstNotCleared();
   }

   //
   // Set the GUI is alive flag ...
   //
   _GUIsAlive = true;

   if ( foundAlarm != _AlarmTable.end() )
   {
      processRequest(foundAlarm->second);
   }
}

void AlarmHandler::safetysAlive ()
{
   AlarmTable::iterator foundAlarm = _AlarmTable.end();

   if (!_SafetysAlive)
   {
      DataLog(log_level_alarm_handler_info) << "Safety is alive." << endmsg;

      //
      // Find the highest display level alarm which is  NOT CLEARED.
      //
      foundAlarm = _AlarmTable.findFirstNotCleared();
   }

   //
   // Set the Safety is alive flag ...
   //
   _SafetysAlive = true;

   if ( foundAlarm != _AlarmTable.end() )
   {
      processRequest(foundAlarm->second);
   }

   _RetOccMonitorResponseTimer.armTimer();
}

void AlarmHandler::processRequest (AlarmStruct& alarm)
{
   if ( alarm.getName()  != _DisplayedAlarm.getName() ||
        alarm.getState() != _DisplayedAlarm.getState() )
   {
      DataLog(log_level_alarm_handler_info) << "Process Request:  alarm->" << alarm.getName()
                                            << ", state->" << alarm.getState() << endmsg;
   }

   //
   // Set the incoming alarm ID ...
   //
   _OverrideRequestMsg.rcvdAlarmID(alarm.getName() );

   //
   // Stop any AKO in progress (but not if we're clearing an already set alarm state)
   //
   if ( (alarm.getAction() < NOTHING) && (alarm.getState() != CLEARED)  )
   {
      DataLog(log_level_alarm_handler_info) << "Process Request: alarm has an action, and is not being cleared: "
                                            << "stop any in-progress AKOs" << endmsg;
      AKOMsg notifyProcToAKO(MessageBase::SEND_LOCAL);
      notifyProcToAKO.send(0);
   }

   //
   // Save the new alarm in PFR land ...
   saveAlarmForPFR(alarm);

   //
   // Clear alarms if appropriate ...
   //
   clearPressureAlarms(alarm);

   // Clear AF alert if AF alarm occurs on top of it
   //
   if ( alarm.getName() == AUTOFLOW_TIME_ALARM )
   {
      clearAfAlert(alarm.getName());
   }

   //
   // Determine which alarm should be displayed ...
   //
   AlarmStruct* dispAlarm = determineTopAlarm(alarm);

   //
   // Display the alarm ...
   //
   displayAlarm(*dispAlarm);
}

void AlarmHandler::processResponse (AlarmStruct& alarm)
{
   DataLog(log_level_alarm_handler_info) << "Process Response:  alarm->" << _ResponseData.alarm_name << ", response->" << _ResponseData.alarm_response << endmsg;

   //
   // Set the alarm ID that was responded to ...
   //
   _OverrideRequestMsg.respAlarmID(alarm.getName() );

   //
   // Determine the alarm response.
   //
   if ( _ResponseData.alarm_response == ABORT )
   {
      //
      // Inactivate the alarm ...
      //
      alarm.inactivate(/* for */ 1000 /*milliseconds = 1 second */);

      // If this is PFR from metered storage, we go to MSS disconnect
      // instead of donor disconnect (donor is already disconnected).
      if ( (_setCds.CassetteState.Get() == ProcTrimaSet_CDS::STORAGE ||
            _setCds.CassetteState.Get() == ProcTrimaSet_CDS::STORAGE_COMPLETE))
      {
         DataLog (log_level_alarm_handler_info) << "Send abortMssMsg" << endmsg;

         //
         // Notify proc to end automated additive solutions ...
         //
         MSSEndRunMsg notifyProcToAbortMss(MessageBase::SEND_LOCAL);
         notifyProcToAbortMss.send(1);
      }

      // For every other alarm, just abort to donor disconnect.
      else
      {
         //
         // Notify proc to end the run ...
         //
         CancelProcedureMsg notifyProcToAbort(MessageBase::SEND_LOCAL);
         notifyProcToAbort.send(1);
      }

      clearAlarms(A1, LATCHED);
   }
   else if ( _ResponseData.alarm_response == ABORT_THRU_RINSEBACK )
   {
      //
      // Inactivate the alarm ...
      //
      alarm.inactivate(/* for */ 1000 /*milliseconds = 1 second */);

      //
      // Notify proc to go to rinseback ...
      //
      RinsebackRequestMsg notifyProcOfState(MessageBase::SEND_LOCAL);
      notifyProcOfState.send(1);

      clearAlarms(A1, LATCHED);
   }
   else if ( _ResponseData.alarm_response == UNLOAD )
   {
      //
      // Notify proc to raise the set ...
      //
      GUIRequestRaiseCassetteMsg notifyProcToRaise(MessageBase::SEND_LOCAL);
      notifyProcToRaise.send(1);

      //
      // Set the alarm to the CLEARED "by operator" state.
      //
      alarm.setState(CLEARED);
   }
   else if ( _ResponseData.alarm_response == CONTINUE )
   {
      //
      // Clear only alarms which are NOT LATCHED.
      //
      if ( alarm.getState() != LATCHED )
      {
         //
         // Set the alarm to the CLEARED "by operator" state.
         //
         alarm.setState(CLEARED);
      }
      //
      // Notify proc to perform an Access Keep Open procedure ...
      //
      if ( alarm.getName() == MAINTAIN_DONOR_ACCESS_ALERT )
      {
         AKOMsg notifyProcToAKO(MessageBase::SEND_LOCAL);
         notifyProcToAKO.send(1);
      }
   }

   //
   // Remove the alarm from PFR land ...
   removeAlarmFromPFR(alarm);

   //
   // Send the responded alarm back to the original sender ...
   //
   Alarm_struct as;
   as.alarm_name  = alarm.getName();
   as.alarm_state = alarm.getState();
   Message< Alarm_struct > ar(MessageBase::SEND_LOCAL);
   ar.send(as);

   //
   // Call the display function to determine who should be displayed.
   //
   AlarmStruct* dispAlarm = determineTopAlarm(alarm);

   //
   // Display the alarm ...
   //
   displayAlarm(*dispAlarm);
}

void AlarmHandler::displayAlarm (AlarmStruct& alarm)
{
   //
   // We don't display any alarm until GUI and safety arealive, so
   //  we just safe state the system until they are ready ...
   //
   if (    _GUIsAlive                         // GUI is ready
           && (    _SafetysAlive              //  and, Safety is also ready,
                   || alarm.getType() == A2 ) //       or safety is not ready, but the alarm is not continuable,
                                              //          so we don't need safety
           )
   {     //   so let's send the alarm ...
      bool playSounds = false;
      bool sendAlarm  = false;

      //
      // If this alarm is the different than the displayed alarm
      //
      if ( alarm.getName() != _DisplayedAlarm.getName() )
      {
         // Turn off the sound for the currently displayed alarm before turning on the new one
         SoundOrders            alarmSound;
         Message< SoundOrders > soundRequest(MessageBase::SEND_LOCAL);
         alarmSound.soundCode = _DisplayedAlarm.getSound();
         alarmSound.mode      = SOUND_CLEAR;
         soundRequest.send(alarmSound);

         //
         // This is the new displayed alarm ...
         //
         _DisplayedAlarm = alarm;

         //
         // Set the sounds ...
         //
         playSounds = true;

         //
         // Send the alarm ..
         //
         sendAlarm = true;
      }
      else
      {
         //
         // If the state is different ...
         //
         if ( alarm.getState() != _DisplayedAlarm.getState() )
         {
            //
            // Set the new state ...
            //
            _DisplayedAlarm.setState(alarm.getState() );

            //
            // Send the alarm ...
            //
            sendAlarm = true;
         }
      }

      //
      // Notify safety of A2 alarms ...
      //
      if ( _DisplayedAlarm.getType() == A2 )
      {
         NotifySafetyOfA2Msg notifySafety(MessageBase::SEND_GLOBAL);
         notifySafety.send(1);
      }

      //
      // Set the lights ...
      //
      if ( _DisplayedAlarm.getType() != OPERATOR_ATTENTION )
      {
         _OverrideRequestMsg.alarmLight(HAL_ALARMLIGHTON);
      }

      if ( sendAlarm &&
           _DisplayedAlarm.getState() != CLEARED &&
           _DisplayedAlarm.getDisplayType() != DISP_NONE &&
           _DisplayedAlarm.getDisplayType() != DISP_STATUS_LINE)
      {
         //
         // Send GUI the alarm ...
         //
         sendAlarmToGUI();
      }

      if ( sendAlarm &&
           _DisplayedAlarm.getState() != CLEARED &&
           _DisplayedAlarm.getDisplayType() == DISP_STATUS_LINE )
      {
         //
         // Send GUI the command to change the status line...
         // This should be made generic to send the ID of the text string
         // However, there is no need at this time and the include file is not
         // common.
         //
         _setStatusLineMsg.send(0);

         DataLog(log_level_alarm_handler_info) << "setting status line" << endmsg;
      }

      if ( playSounds )
      {
         //
         // Play the sounds ...
         //
         setSounds(_DisplayedAlarm);
      }
   }

   //
   // Set the top displayed alarm
   //
   _OverrideRequestMsg.dispAlarmID(_DisplayedAlarm.getName() );

   //
   // Send the override message ...
   //
   _OverrideRequestMsg.SendMsg();
}

void AlarmHandler::takeAction (AlarmStruct& alarm)
{

   // Do nothing for PREDICTION_PAUSE alarm when system state is in AC_PRIME
   if ( (_systemState == AC_PRIME) && (alarm.getName() == PREDICTION_PAUSE) )
   {
      DataLog (log_level_alarm_handler_info) << "Don't stop pumps for PREDICTION_PAUSE alarm when system state is AC_PRIME" << endmsg;
      return;
   }
   else
   {
      DataLog (log_level_alarm_handler_info) << "Taking action " << alarm.getAction() << endmsg;
   }

   switch ( alarm.getAction() )
   {
      case STOP_ALL :
         _OverrideRequestMsg.centSpeed(false);
         _OverrideRequestMsg.Vrbc(HAL_VALVE_RETURN);

      case SLOW_CENTRIFUGE :
         _OverrideRequestMsg.centPausedSpeed(true);
         _OverrideRequestMsg.Vrbc(HAL_VALVE_RETURN);

      case STOP_PUMPS :
         _OverrideRequestMsg.Qcollect(0.0);

      case STOP_DONOR_ACCESS :
         _OverrideRequestMsg.Qac(0.0);
         _OverrideRequestMsg.Qin(0.0);
         _OverrideRequestMsg.Qrp(0.0);
         _OverrideRequestMsg.Qplasma(0.0);
         _OverrideRequestMsg.AlarmActive(true);
         break;

      case HALVE_PUMP_SPEEDS :
         _OverrideRequestMsg.Qac(CobeConfig::data().ApsFlowReductionFactor);
         _OverrideRequestMsg.Qin(CobeConfig::data().ApsFlowReductionFactor);
         _OverrideRequestMsg.Qrp(CobeConfig::data().ApsFlowReductionFactor);
         _OverrideRequestMsg.Qplasma(0.0);
         _OverrideRequestMsg.AlarmActive(true);
         break;

      case STOP_MSS_RBC :
         _OverrideRequestMsg.Qac(0.0f);
         _OverrideRequestMsg.AlarmActive(true);
         break;

      case STOP_MSS_PLT :
         _OverrideRequestMsg.Qcollect(0.0f);
         _OverrideRequestMsg.Qplasma(0.0f);
         _OverrideRequestMsg.AlarmActive(true);
         break;

      case STOP_PUMPS_CLOSE_RBC : // CPS PTF alarms need the valve to close
         _OverrideRequestMsg.Vrbc(HAL_VALVE_RETURN);
         _OverrideRequestMsg.Qcollect(0.0);
         _OverrideRequestMsg.Qac(0.0);
         _OverrideRequestMsg.Qin(0.0);
         _OverrideRequestMsg.Qrp(0.0);
         _OverrideRequestMsg.Qplasma(0.0);
         _OverrideRequestMsg.AlarmActive(true);
         break;

      default :
         break;
   }
}

AlarmStruct* AlarmHandler::determineTopAlarm (AlarmStruct& newestAlarm)
{
   AlarmStruct* highestAlarm = &_AlarmTable[NULL_ALARM];

   //
   // Determine if the newest alarm will get displayed ...
   //
   if ( newestAlarm.getState() != CLEARED &&
        newestAlarm.getDisplayType() != DISP_NONE )
   {
      highestAlarm = &newestAlarm;
   }

   //
   // Find the highest display level alarm which is NOT CLEARED.
   //
   AlarmTable::iterator foundAlarm = _AlarmTable.begin();

   for (; foundAlarm != _AlarmTable.end(); ++foundAlarm)
   {
      if (foundAlarm->second.getState() != CLEARED)
      {
         //
         // 'OR' all of the actions for all non-cleared alarms together
         //   to be sent at the end of processing ...
         //
         takeAction(foundAlarm->second);

         //
         // If the found alarm is of higher display type than the
         //  highest display type in the table ...
         //
         if ( foundAlarm->second.getDisplayType() < highestAlarm->getDisplayType() )
         {
            //
            // Newly found alarm has a higher display priority....
            //
            highestAlarm = &foundAlarm->second;

         }
      }
   }

   //
   // Make the last alarm displayed, come up again if it is
   //  still the highest display type and not CLEARED ...
   //
   if ( highestAlarm->getDisplayType() != DISP_FLOATING &&
        highestAlarm->getDisplayType() == _DisplayedAlarm.getDisplayType() )
   {
      AlarmStruct& dispAlarm = _AlarmTable[_DisplayedAlarm.getName() ];

      if (dispAlarm.getState() != CLEARED )
      {
         highestAlarm = &dispAlarm;
      }
   }

   //
   // Look for a DISP_STATUS_LINE display type that is being cleared.  Before it goes
   // away, make sure to tell GUI.
   //
   if ( (newestAlarm.getDisplayType() == DISP_STATUS_LINE) &&
        (newestAlarm.getState() == CLEARED) &&
        (highestAlarm->getName() == NULL_ALARM)
        )
   {
      _clearStatusLineMsg.send(0);
      DataLog(log_level_alarm_handler_info) << "clearing status line" << endmsg;
   }

   //
   // If the alarm is different than the currently displayed alarm or
   //  the state is different ...
   if ( highestAlarm->getName() != _DisplayedAlarm.getName() ||
        highestAlarm->getState() != _DisplayedAlarm.getState() )
   {
      //
      // Log the highest alarm ...
      //
      DataLog(log_level_alarm_handler_info) << "Determine Top Alarm:  highest alarm->"
                                            << highestAlarm->getName() << ", state->"
                                            << highestAlarm->getState() << endmsg;
   }

   if (highestAlarm->getName() == NULL_ALARM)
   {
      DataLog (log_level_alarm_handler_info) << "Alarm queue is empty." << endmsg;
   }
   else if (highestAlarm->getName() == GENERIC_PLT_WBC_CONTAMINATION_ALERT ||
            highestAlarm->getName() == GENERIC_PLT_WBC_CONTAMINATION_OBTRUSIVE_ALERT)
   {
      _runCds.wbcAlarmPending.Set(false);
      _runCds.ShowedPltWbcContamAlarm.Set(true);
   }

   //
   // Return what we think is the highest display priority alarm ...
   //
   return highestAlarm;
}

void AlarmHandler::clearAlarms (ALARM_TYPES type, ALARM_STATES state)
{

   AlarmTable::iterator iter = _AlarmTable.begin();

   for (; iter != _AlarmTable.end(); ++iter)
   {

      AlarmStruct& alarm = iter->second;

      //
      // If the type of the alarm is greater then or equal to
      // the type specified for removal check next condition..
      //
      // .......Lower ALARM_TYPES have higher priority.....
      //
      if ( alarm.getType() >= type )
      {
         if ( ((state == LATCHED) && (alarm.getState() != CLEARED)) ||
              ((state == ACTIVE) && (alarm.getState() == ACTIVE)) )
         {
            //
            // Inactivate the alarm temporarily ...
            //
            alarm.inactivate(1000 /*milliseconds = 1 second*/);

            //
            // Clear the internal state.
            //
            alarm.setState(CLEARED);
            removeAlarmFromPFR(alarm);

            //
            // Send to the alarm itself the resulting new alarm state.
            //
            Alarm_struct data;
            data.alarm_name  = alarm.getName();
            data.alarm_state = alarm.getState();

            //
            // This is really the outgoing response for the alarm that
            // was responded to.
            //
            _IncomingAlarmMsg.send(data);
         }
      }
   }
}

void AlarmHandler::clearTimeAlarms (AlarmStruct& alarm)
{
   if ( alarm.getState() != CLEARED )
   {
      bool clearOneMin   = false;
      bool clearThreeMin = false;

      //
      // If the alarm is the ten minute alarm ...
      //
      if ( alarm.getName() == TEN_MINUTE_PAUSE )
      {
         clearOneMin = true;
      }
      else if ( alarm.getName() == MAINTAIN_DONOR_ACCESS_ALERT )
      {
         clearOneMin = true;
      }

      if ( clearOneMin)
      {
         AlarmStruct& oma = _AlarmTable[ONE_MINUTE_PAUSE];
         oma.setState(CLEARED);
         removeAlarmFromPFR(oma);
      }
      if ( clearThreeMin)
      {
         AlarmStruct& tma = _AlarmTable[MAINTAIN_DONOR_ACCESS_ALERT];
         tma.setState(CLEARED);
         removeAlarmFromPFR(tma);
      }
   }
}

void AlarmHandler::clearPressureAlarms (AlarmStruct& alarm)
{
   if ( alarm.getState() != CLEARED )
   {
      bool clearAutoPause = false;
      bool clearHalfSpeed = false;

      //
      // If the alarm stops the pumps and is displayed to
      //  the operator ...
      //
      if (    alarm.getAction() <= STOP_DONOR_ACCESS
              && alarm.getDisplayType() != DISP_NONE
              )
      {
         clearHalfSpeed = 1;
      }

      if (    clearAutoPause
              || clearHalfSpeed )
      {
         Message< Alarm_struct > ar(MessageBase::SEND_LOCAL);
         Alarm_struct            as;

         if ( clearAutoPause )
         {
            AlarmStruct& apa = _AlarmTable[APS_PAUSE];
            apa.setState(CLEARED);
            removeAlarmFromPFR(apa);

            as.alarm_name  = apa.getName();
            as.alarm_state = apa.getState();

            //
            // Respond to the sender ...
            //
            ar.send(as);
         }

         if ( clearHalfSpeed )
         {
            AlarmStruct& hsa = _AlarmTable[APS_PUMPS_SLOW];
            hsa.setState(CLEARED);
            removeAlarmFromPFR(hsa);

            as.alarm_name  = hsa.getName();
            as.alarm_state = hsa.getState();

            //
            // Respond to the sender ...
            //
            ar.send(as);
         }
      }
   }
}

void AlarmHandler::sendAlarmToGUI ()
{
   GUI_Alarm_struct data;
   data.alarm_name  = _DisplayedAlarm.getName();
   data.alarm_state = _DisplayedAlarm.getState();
   data.alarm_type  = _DisplayedAlarm.getType();

   if (_DisplayedAlarm.getName() == NULL_ALARM)
   {
      // protect agains sending NULL alarm to GUI and crashing system
      DataLog(log_level_alarm_handler_info)
         << "Alarm Manager attempting to send NULL_ALARM (SEE IT6995) -- message send aborted"
         << endmsg;

      DataLog(log_level_alarm_handler_info) << "Displayed Alarm:  alarm->" << _DisplayedAlarm.getName()
                                            << ", state->" << _DisplayedAlarm.getState()
                                            << ", type->" << _DisplayedAlarm.getType()
                                            << ", disp->" << _DisplayedAlarm.getDisplayType()
                                            << endmsg;
   }
   else
   {
      _DisplayRequestMsg.send(data);

      DataLog(log_level_alarm_handler_info) << "Displayed Alarm:  alarm->" << _DisplayedAlarm.getName()
                                            << ", state->" << _DisplayedAlarm.getState()
                                            << ", type->" << _DisplayedAlarm.getType()
                                            << ", disp->" << _DisplayedAlarm.getDisplayType()
                                            << endmsg;
   }


}



void AlarmHandler::setSounds (AlarmStruct& alarm)
{
   SoundOrders            alarmSound;
   Message< SoundOrders > soundRequest(MessageBase::SEND_LOCAL);

   if ( alarm.getState() == CLEARED )
   {
      // Terminate all sounds
      alarmSound.mode      = SOUND_CLEAR_ALL;
      alarmSound.soundCode = NO_SOUND;
      soundRequest.send(alarmSound);

      DataLog(log_level_alarm_handler_info) << "All Alarm or Alert sounds are cleared." << endmsg;
   }
   else if ( alarm.getSound() != NO_SOUND )
   {
      // Start the appropriate sound continuously
      alarmSound.soundCode = alarm.getSound();
      alarmSound.mode      = SOUND_CONTINUOUS;
      soundRequest.send(alarmSound);

      DataLog(log_level_alarm_handler_info) << "Sound is played, Sound type: "
                                            << SoundCodeStrings[alarmSound.soundCode]
                                            << " sound mode: " << SoundModeStrings[alarmSound.mode] << endmsg;
   }

}

void AlarmHandler::alarmHandlerError ()
{
   //
   // Send the A2 Invalid alarm action to GUI ...
   //
   AlarmStruct& alarm = _AlarmTable[INVALID_ALARM_FAULT];

   alarm.setState(ACTIVE);
   processRequest(alarm);

}

void AlarmHandler::procStatusCheck ()
{
   //
   //
   //  If we received a 1 it's from us so no action required.  A
   //  reception of zero indicates a request to verify that we
   //  are still alive.
   //
   if (!_ProcedureStatusCheck.getData())
      _ProcedureStatusCheck.send(1);
}

void AlarmHandler::endRunRequested ()
{
   //
   // Clear any active alarms ...
   //
   clearAlarms(A1, LATCHED);

   //
   // Call the display function to determine who should be displayed.
   //
   AlarmStruct& alarm     = _AlarmTable[NULL_ALARM];
   AlarmStruct* dispAlarm = determineTopAlarm(alarm);

   //
   // Display the alarm ...
   //
   displayAlarm(*dispAlarm);
}

void AlarmHandler::saveAlarmForPFR (AlarmStruct& alarm)
{
   if ( alarm.getDisplayType() != DISP_NONE )
   {
      AlarmQueueEntry e;
      e.name       (alarm.getName() );
      e.type       (alarm.getType() );
      e.displayType(alarm.getDisplayType() );
      e.action     (alarm.getAction() );

      AlarmActiveMessage pfrMessage(MessageBase::SEND_LOCAL);
      pfrMessage.send(e);
   }
}

void AlarmHandler::removeAlarmFromPFR (AlarmStruct& alarm)
{
   AlarmQueueEntry e;
   e.name       (alarm.getName() );
   e.type       (alarm.getType() );
   e.displayType(alarm.getDisplayType() );
   e.action     (alarm.getAction() );

   AlarmInactiveMessage pfrMessage(MessageBase::SEND_LOCAL);
   pfrMessage.send(e);
}

void AlarmHandler::retOccMonitorAlive ()
{
   if ( _RetOccMonitorResponseMsg.getData() != 0 )
   {
      _RetOccMonitorAlive     = true;
      _RetOccMonitorAliveTime = time(0);     // Capture the time of the message
   }
}

void AlarmHandler::retOccMonitorCheck ()
{
   if ( !_RetOccMonitorAlive )
   {
      //
      // The monitor checked is not enforced unless we have already received the first
      // status from the task, or we have exhausted the retry limit before receiving the
      // first status
      //
      if ( !_RetOccMonitorCheckActive )
      {
         _RetOccMonitorCheckRetry += 1;

         if ( _RetOccMonitorCheckRetry > 5 )
         {
            _RetOccMonitorCheckActive = true;
            DataLog(log_level_alarm_handler_info) << "no first response from retoccmon - forcing monitoring to be enabled" << endmsg;
         }
         else
         {
            DataLog(log_level_alarm_handler_info) << "waiting for first response from retoccmon " << (int)_RetOccMonitorCheckRetry << endmsg;
         }
      }

      if (_RetOccMonitorCheckActive)
      {
         //
         // Return line occlusion monitor is not responding
         //
         AlarmStruct& alarm = _AlarmTable[RET_OCC_MONITOR_FAILURE];

         const double delta = difftime(time(0), _RetOccMonitorAliveTime);

         DataLog(log_level_proc_info) << "Return Occlusion Monitor handshake not seen in time"
                                      << "Actual= " << (int)delta << " seconds.  Expected= " << (RETOCC_PING_TIME / 1000)
                                      << " seconds." << endmsg;

         alarm.setState(ACTIVE);
         processRequest(alarm);

      }
   }
   else
   {
      if ( !_RetOccMonitorCheckActive )
      {
         _RetOccMonitorCheckActive = true;
         DataLog(log_level_alarm_handler_info) << "got first response from retoccmon" << endmsg;
      }

      _RetOccMonitorAlive = false;
      _RetOccMonitorResponseMsg.send(0);
   }
}

void AlarmHandler::Cleanup ()
{
   DataLog(log_level_alarm_handler_info) << "Cleanup" << endmsg;
}

void AlarmHandler::HandleSystemStateChangeMsg ()
{
   _systemState = (State_names)_systemStateMsg.getData();
   DataLog(log_level_alarm_handler_info) << "_systemState: " << _systemState << endmsg;
}

void AlarmHandler::clearAfAlert (ALARM_VALUES activeAlarm)
{
   AlarmStruct& afAlert     = _AlarmTable[AUTOFLOW_TIME_ALERT];
   AlarmStruct& afAlarm     = _AlarmTable[AUTOFLOW_TIME_ALARM];

   // Autoflow alarm clears autoflow alert
   specialClearAlarm(afAlert);
}

void AlarmHandler::specialClearAlarm( AlarmStruct& alarm )
{
   //
   // Special cases where one alarm being instantiated clears another
   //
   DataLog(log_level_alarm_handler_info) << "Special clearing of alarm " << alarm.getName() << endmsg;
   //
   // Clear the internal state.
   //
   alarm.setState(CLEARED);
   removeAlarmFromPFR(alarm);

   //
   // Send to the alarm itself the resulting new alarm state.
   //
   Alarm_struct data;
   data.alarm_name  = alarm.getName();
   data.alarm_state = alarm.getState();

   //
   // This is really the outgoing response for the alarm that
   // was responded to.
   //
   _IncomingAlarmMsg.send(data);
}

/* FORMAT HASH ea9fbd7e10cf9c9e4c0caa342e0c7ab0 */
