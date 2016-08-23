/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      proc.h
 *
 */


#ifndef PROC_H  //  Prevents multiple compilations.
#define PROC_H

// Include files
#include <vxworks.h>

#include "donor.h"
#include "dvitals.h"
#include "hal.h"
#include "procpfr.h"
#include "procstate.h"
#include "stappl.h"
#include "timekeeper.h"
#include "alarmqueue.h"
#include "statecontrol.h"
#include "trimamessages.h"
#include "timermessage.h"
#include "ostime.hpp"


// Forward class declarations

class StateControl;

class Proc
   : public CStdAppl
{
public:
   // Default constructor
   Proc();
   // Destructor
   virtual ~Proc();

   // Initialize the application
   virtual ALARM_VALUES Init ();

protected:

   //
   // Message receiver callbacks
   //
   void debugMsgReceived ();
   void halStatusMsgReceived ();

   //
   // Callback for the send orders request message that
   //  will send the HAL orders off
   //
   void sendOrders ();

   //
   // Callbacks for the alarm handler responses
   //
   void alarmHandlerAlive ();
   void alarmHandlerCheck ();

   //
   // Callbacks for the safety status messagea checks
   //
   void safetyStatusRcvd ();
   void safetyStatusCheck ();

   //
   // Callbacks for the alarm handler queue messages
   //
   void alarmActive ();
   void alarmInactive ();

   //
   // Callback for the GUI alive message
   //
   void guiAlive ();


   void ConfigurationReceived ();

   //
   // Function to check important data to ensure that it is logged
   //  when it changes...
   void forceLogging ();

   // Cleanup all application memory
   virtual void Cleanup ();

protected:

   //
   // HAL interface
   CHal _HAL;

   //
   // Message to switch on debug logging
   ProcedureDebugMsg _DebugMsg;

   //
   // Main state that is active throughout all
   //  the lifetime of this process
   //
   StateControl _MainState;

   // To receive donor message
   CDVitals _DonorVitals;

   //
   // Message to trigger the orders be sent
   //
   SendOrdersMsg _SendOrdersRequest;

   //
   // Initialization status ...
   //
   bool _BadInitStatus;

   //
   // Message to make sure the alarm handler is alive ...
   AlarmHandlerStatusMsg _AlarmHandlerResponseMsg;
   //
   // Timer to make sure alarm handler is alive ...
   TimerMessage _AlarmHandlerResponseTimer;

   //
   // Flag to show the safety status has been received ...
   bool _AlarmHandlerAlive;

   //
   // Timer to make sure safety status is still coming ...
   TimerMessage _SafetyStatusCheckTimer;

   //
   // Flag to show the safety status has been received ...
   bool _SafetyStatusRcvd;


   //
   // Alarm Monitoring Queue
   AlarmActiveMessage   _AlarmActiveMsg;
   AlarmInactiveMessage _AlarmInactiveMsg;

   //
   // For logging purposes save the current state of the door
   //  and the cassette to log when they change state
   HAL_CASSETTESTATE _PrevCassettePos;
   HAL_DOORSTATE     _PrevDoorStatus;

   //
   // Message to broadcast current proc state
   Message<ProcState> _ProcStateMsg;

   QueryGUIAliveMsg   _QueryGUIAliveMsg;
   AnswerGUIAliveMsg  _GUIAliveResponsMsg;
   bool               _GUIResponded;


   ConfigChangedMsg _configChangeListnerMsg;

   rawTime          _halUpdateTime;
   bool             _halUpdateFirstTime;

   //
   // Timer to update all of Proc's TimeKeeper objects
   TimerMessage _TimeKeeperUpdateTimer;
};

#endif

/* FORMAT HASH 05000e70761f861fb8b3e54cad7acb22 */
