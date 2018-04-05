/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      alarm_h.cpp
 *
 * ABSTRACT:   Focussed System alarm handler.
 *             Handles incoming alarm messages by going to alarm  mode
 *             and preforming the actions specified by the alarm action.
 *
 */

#ifndef _ALARM_H_H_
#define _ALARM_H_H_

#include "stappl.h"
#include "alarms.h"
#include "alarmcontainer.h"
#include "an_alarm.h"
#include "override_alr_msg.h"
#include "timermessage.h"
#include "trimamessages.h"
#include "states.h"
#include "proctrimaset_cds.h"
#include "procrun_cds.h"

class AlarmHandler
   : public CStdAppl
{

public:

   //
   // Default constructor
   AlarmHandler();

   //
   // Default destructor
   virtual ~AlarmHandler();

   //
   // Initialize the application, and create and set up all messages
   //  (i.e. do any initialization that needs to happen after the dispatcher
   //        is created )
   //
   // return NULL_ALARM -- Initialized properly
   // A2 or FATAL_ERROR -- Initialization failure
   //
   virtual ALARM_VALUES Init ();

   //
   // Callback for the incoming alarm message
   //
   void incomingAlarm ();

   //
   // Callback for the incoming response message
   //
   void alarmResponse ();

   //
   // Callback for the GUI is alive message
   //
   void guisAlive ();

   //
   // Callback for the Safety is alive message
   //
   void safetysAlive ();

   //
   // Callback for the system state update message
   //
   void HandleSystemStateChangeMsg ();

protected:

   //
   // This method will determine which alarm to display and
   //  perform all actions required to satisfy all non cleared
   //  alarms
   //
   void processRequest (AlarmStruct& alarm);

   //
   // This method will determine which actions need to be done
   //  to satisfy the received GUI response for the displayed
   //  alarm
   //
   void processResponse (AlarmStruct& alarm);

   //
   // Function used to send the messages and literally get
   //  the alarm displayed
   //
   void displayAlarm (AlarmStruct& alarm);

   //
   // Function which sets the override message based
   //  on the action in a specific alarm ...
   //
   void takeAction (AlarmStruct& alarm);

   //
   // Function which finds the alarm which should be on
   //  top and sets that alarm as the currently displayed
   //  alarm.  If all alarms are cleared, the displayed
   //  alarm will be the NULL_ALARM ...
   //
   AlarmStruct* determineTopAlarm (AlarmStruct& newestAlarm);

   //
   // This method clears the alarms of "type" or lower
   //  if they are currently in the "state" passed in.
   //  Clearing LATCHED alarms of any level will also clear
   //  alarms ACTIVE as well.
   //
   void clearAlarms (ALARM_TYPES type, ALARM_STATES state);

   // Clear AF alert, used when AF ALARM occurs on top of AF alert
   //
   void clearAfAlert (ALARM_VALUES activeAlarm);

   //
   // Used in special cases where proc can clear an alarm
   //
   void specialClearAlarm (AlarmStruct& alarm);

   //
   // This function will clear the one minute
   //  alarm if the three or ten minute alarm get set
   //
   void clearTimeAlarms (AlarmStruct& alarm);

   //
   // This function will clear the other pressure alarms
   //  if a higher order pressure alarm is given
   //
   void clearPressureAlarms (AlarmStruct& alarm);

   //
   // Send the display message to GUI
   //
   void sendAlarmToGUI ();

   //
   // Function which sends the sound command based on the
   //  alarm's requirements
   //
   void setSounds (AlarmStruct& alarm);

   //
   // This function will send a software error out and
   //  effectively stop the system
   //
   void alarmHandlerError ();

   //
   // Callback function for the proc status check message
   //
   void procStatusCheck ();

   //
   // Callback function for the end run message
   //
   void endRunRequested ();

   //
   // PFR messages
   //
   void saveAlarmForPFR (AlarmStruct& alarm);
   void removeAlarmFromPFR (AlarmStruct& alarm);

   //
   // Callbacks for the return line occlusion monitor responses
   //
   void retOccMonitorAlive ();
   void retOccMonitorCheck ();

   //
   // Cleanup any memory used by the object
   //
   virtual void Cleanup (void);

protected:

   //
   // The actual alarm table which wraps the alarm array
   //
   AlarmTable _AlarmTable;

   // The alarm which is currently displayed, should default to NULL_ALARM.
   AlarmStruct _DisplayedAlarm;

   //
   // Flag to signify that gui and safety are alive
   //
   bool _GUIsAlive;
   bool _SafetysAlive;

   //
   // Incoming messages.
   //
   //  New system alarm
   Message<Alarm_struct> _IncomingAlarmMsg;
   //
   //  GUI's alarm response
   Message<Alarm_response_struct> _AlarmResponseMsg;
   Alarm_response_struct          _ResponseData;

   //
   //  GUI's alive and responding
   AnswerGUIAliveMsg    _GUIsAliveMsg;
   AnswerSafetyAliveMsg _SafetysAliveMsg;

   // System state message
   State_names          _systemState;
   SystemStateChangeMsg _systemStateMsg;

   //
   // Outgoing messages.
   //
   //  Alarm display request to GUI
   Message<GUI_Alarm_struct> _DisplayRequestMsg;

   //
   //  Alarm action request to Proc
   override_alr_msg _OverrideRequestMsg;

   //
   // Status check message ...
   //
   AlarmHandlerStatusMsg _ProcedureStatusCheck;

   //
   // End Run message ...
   //
   CancelProcedureMsg _EndRunMsg;

   ProcTrimaSet_CDS   _setCds;
   ProcRun_CDS        _runCds;

   //
   // set/clear status line messages to GUI
   //
   SetStatusLineMsg   _setStatusLineMsg;
   ClearStatusLineMsg _clearStatusLineMsg;


   //
   // Internal data needed to check return line occlusion monitor status
   //
   RetOccMonitorStatusMsg _RetOccMonitorResponseMsg;
   TimerMessage           _RetOccMonitorResponseTimer;
   bool                   _RetOccMonitorAlive;
   bool                   _RetOccMonitorCheckActive;
   unsigned char          _RetOccMonitorCheckRetry;
   time_t                 _RetOccMonitorAliveTime;
};

#endif

/* FORMAT HASH d4171ec3bff6ce635437d8b611a673b5 */
