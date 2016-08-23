/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      proc.cpp
 *
 */

#include <taskLib.h>

#include "proc.h"
#include "procdata.h"
#include "filenames.h"
#include "filereader.h"
#include "recoveryman.h"
#include "stmain.h"
#include "cobeconfig.h"
#include "rbcdat.h"
#include "hwdat.h"
#include "periodic_log.h"
#include "substatemap.h"
#include "statedictionary.h"
#include "trima_datalog.h"
#include "../os/discover_hw.h"
#include "trima_tasks.h"


#include <signal.h>
#include <stdio.h>
#include <tasklib.h>

const int           PROC_ORDERS_TIMEOUT         =  800;
const unsigned long TIME_KEEPER_UPDATE_INTERVAL = 500; /* milliseconds */

// main function declaration
MAIN2(Proc, 90);

Proc::Proc()
   : _HAL(Callback<Proc>(this, &Proc::halStatusMsgReceived) ),
     _DebugMsg(Callback<Proc>(this, &Proc::debugMsgReceived) ),
     _MainState(),
     _DonorVitals(),
     _SendOrdersRequest(),
     _BadInitStatus(false),
     _AlarmHandlerResponseMsg(Callback<Proc>(this, &Proc::alarmHandlerAlive), MessageBase::SNDRCV_GLOBAL),
     _AlarmHandlerResponseTimer(0, Callback<Proc>(this, &Proc::alarmHandlerCheck), TimerMessage::DISARMED),
     _AlarmHandlerAlive(true), // Assume the alarm handler is alive to start ...
     _SafetyStatusCheckTimer(0, Callback<Proc>(this, &Proc::safetyStatusCheck), TimerMessage::DISARMED),
     _SafetyStatusRcvd(true), // Assume everything is good to start ...
     _AlarmActiveMsg(Callback<Proc>(this, &Proc::alarmActive)),
     _AlarmInactiveMsg(Callback<Proc>(this, &Proc::alarmInactive)),
     _PrevCassettePos(HAL_CASSETTE_UNKNOWN),
     _PrevDoorStatus(HAL_DOOR_OPEN),
     _ProcStateMsg(MessageBase::SEND_GLOBAL),
     _QueryGUIAliveMsg(MessageBase::SEND_LOCAL),
     _GUIAliveResponsMsg(Callback<Proc>(this, &Proc::guiAlive), MessageBase::SNDRCV_RECEIVE_ONLY),
     _GUIResponded(false),
     _configChangeListnerMsg(Callback<Proc>(this, &Proc::ConfigurationReceived)),
     _halUpdateFirstTime(true),
     _TimeKeeperUpdateTimer(0, CallbackBase(&TimeKeeper::updateTimers), TimerMessage::DISARMED)
{}

Proc::~Proc()
{
   Proc::Cleanup();
}


ALARM_VALUES Proc::Init ()
{
   ALARM_VALUES status = NULL_ALARM;
   ProcData     pd;

   //
   // Object which opens and reads the state
   //  machine initialization file.
   //
   FileReader statesReader;

   if ( statesReader.init(PROC_STATES_FILE, &_MainState) == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_error) << "States Reader Error " << PROC_STATES_FILE_NOT_FOUND << endmsg;
      status = PROC_STATES_FILE_NOT_FOUND;
   }

   DataLog(log_level_proc_info) << "Created the PROC_STATES_FILE reader" << endmsg;

   //
   // Create the state and substate list
   //
   if ( statesReader.readFile() == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_error) << "State machine initialization error " << PROC_STATES_FILE_READ_ERROR << endmsg;
      status = PROC_STATES_FILE_READ_ERROR;
   }

   DataLog(log_level_proc_info) << "Read in the PROC_STATES_FILE" << endmsg;

   //
   // Initialize the main state
   //
   if ( _MainState.init() == PROCESSING_ERROR )
   {
      DataLog(log_level_proc_error) << "Main State initialization error" << endmsg;
      status = PROC_STATES_INIT_ERROR;
   }

   DataLog(log_level_proc_info) << "Initialized the main state" << endmsg;

   //
   // Initialize the donor vitals object ...
   //
   _DonorVitals.Initialize();
   DataLog(log_level_proc_info) << "Registered for the donor vitals message" << endmsg;

   //
   // Initialize the adjust control object
   //
   pd.AdjustControl().Initialize();
   DataLog(log_level_proc_info) << "Initialized adjust control object" << endmsg;

   //
   // Register for the send orders request message ...
   //
   _SendOrdersRequest.init(Callback<Proc>(this, &Proc::sendOrders) );
   DataLog(log_level_proc_info) << "Created the orders message" << endmsg;

   //
   // Start the 'always on' time keeper ...
   //
   pd.AbsTime().activate();
   DataLog(log_level_proc_info) << "Activated the absolute timer" << endmsg;

   pd.MSSRunTime().inactivate(); // dont increment this  time until we enter metering!

   //
   // Call the enter function for the main state get us started
   //  off correctly before we enter the processing loop ...
   //
   _MainState.enter();
   DataLog(log_level_proc_info) << "Called the main state enter function" << endmsg;

   //
   // Initialize HAL ...
   ALARM_VALUES datStatus = _HAL.Init();

   if (datStatus != NULL_ALARM)
   {
      DataLog(log_level_proc_error) << "HAL Initialization error" << datStatus << endmsg;
      status = datStatus;
   }

   DataLog(log_level_proc_info) << "Initialized HAL" << endmsg;

   //
   // Save our bad initialization status ...
   //
   if ( status != NULL_ALARM )
   {
      _BadInitStatus = true;
   }

   //
   // Start periodic logging
   //
   PeriodicLog::init(&pd, 30.0);
   DataLog(log_level_proc_info) << "Started the periodic logging" << endmsg;

   //
   // Start the alarm handler timer ...
   //
   _AlarmHandlerResponseTimer.interval(5000 /*milliseconds*/);
   _AlarmHandlerResponseMsg.send(0);
   DataLog(log_level_proc_info) << "Ask alarm handler for status" << endmsg;

   //
   // Start the safety status check timer ...
   //
   _SafetyStatusCheckTimer.interval(10000 /*milliseconds*/);
   pd.SafetyStatusCb(Callback<Proc>(this, &Proc::safetyStatusRcvd) );
   DataLog(log_level_proc_info) << "Set up the safety status check timer" << endmsg;

   //
   // Start TimeKeeper heartbeat to service all of Proc's TimeKeeper objects
   //
   TimeKeeper::initUpdateTimersInterval(TIME_KEEPER_UPDATE_INTERVAL);
   _TimeKeeperUpdateTimer.interval(TIME_KEEPER_UPDATE_INTERVAL);

   // set initial sound level.
   pd.Orders().SoundLevel(pd.Config().Machine.Get().key_set_audio);
   DataLog(log_level_proc_info) << "Setting initial sound level" << endmsg;

   return status;
}



void Proc::debugMsgReceived ()
{
   log_level_proc_debug.logOutput( ( _DebugMsg.getData() == 1 ? DataLog_LogEnabled : DataLog_LogDisabled ) );
}



//
// This function is called when we receive the CHWStates message from the ctl_msg task.
// This message (status data) is generated by the ctl_drv at a 500 ms rate.
// If this function takes too long to run, then we can generate a soft watchdog event
//
void Proc::halStatusMsgReceived ()
{
   if (_halUpdateFirstTime)
   {
      osTime::snapshotRawTime(_halUpdateTime);
      _halUpdateFirstTime = false;
   }
   else
   {
      const int   deltaMs    = osTime::howLongMilliSecAndUpdate(_halUpdateTime);

      Dispatcher& dispatcher = MessageSystem::MsgSystem()->dispatcher();

      if (deltaMs > PROC_ORDERS_TIMEOUT)
      {
         DataLog(log_level_proc_info) << "WARNING: HAL CHWStates message delay of " << deltaMs
                                      << " ms.  " << dispatcher.messageRemaining() << " messages remaining." << endmsg;
      }
   }

   //
   // Call the main state's process function ...
   //
   if ( _MainState.process() == PROCESSING_ERROR )
   {
      //
      // Set an alarm ...
      InitAlarm(PROC_STATES_PROCESSING_ERROR);
   }

   //
   // Send the new orders ...
   //
   sendOrders();

   //
   // Force a periodic log output if we need to ...
   forceLogging();

   //
   // Send the new proc state ...
   //
   ProcData  pd;
   ProcState ps;
   ps.stateName(_MainState.currentState() );
   pd.SubstateName(_MainState.currentState() );
   ps.paused(pd.AlarmActive() );
   ps.recoveryState(pd.RecoveryName().c_str() );

   DataLog(log_level_proc_debug) << "ProcState -> state name: " << ps.stateName()
                                 << " paused: " << ps.paused()
                                 << " recovery name: " << ps.recoveryState() << endmsg;

   _ProcStateMsg.send(ps);

   // Update the Proc Time
   if (    !pd.AlarmActive()
           && (    RecoveryMan::inRecovery() == RecoveryTypes::NoRecovery
                   || RecoveryMan::inRecovery() == RecoveryTypes::CountAll
                   || RecoveryMan::inRecovery() == RecoveryTypes::CountTime )
           )
      pd.ProcTime().activate();
   else
      pd.ProcTime().inactivate();

   if ( _BadInitStatus && !_GUIResponded )
   {
      //
      // Ask gui if it's ready yet ...
      //
      _QueryGUIAliveMsg.send(1);
   }
}



void Proc::sendOrders ()
{
   //
   // Send the orders off now ...
   //
   ProcData pd;
   _HAL.SendOrders(&(pd.Orders()) );

   // log the pump changes for Metered storage only... Ehh
   if (pd.LogMSSPumps())
   {
      PeriodicLog::forceOutput();
      pd.LogMSSPumps(false);  // turn it off now
   }

   DataLog(log_level_proc_debug) << "Sent HAL Orders Message" << endmsg;
}



void Proc::alarmHandlerAlive ()
{
   if ( _AlarmHandlerResponseMsg.getData() != 0 )
      _AlarmHandlerAlive = true;
}



void Proc::alarmHandlerCheck ()
{
   ProcData pd;

   if (    !_AlarmHandlerAlive
           && pd.SystemState() > SELF_TEST )
   {
      //
      // We can't stay up if the alarm handler is inactive ...
      //
      _FATAL_ERROR(__FILE__, __LINE__, "alarm handler stopped responding.");
   }
   else
   {
      _AlarmHandlerAlive = false;
      _AlarmHandlerResponseMsg.send(0);
   }
}



void Proc::safetyStatusRcvd ()
{
   _SafetyStatusRcvd = true;
}



void Proc::safetyStatusCheck ()
{
   ProcData pd;

   if (    !_SafetyStatusRcvd
           && pd.SystemState() > SELF_TEST )
   {
      //
      // We should alarm if safety is inactive ...
      //
      anAlarmMsg safetyInactiveAlarm(SAFETY_STATUS_INACTIVE);
      safetyInactiveAlarm.setAlarm();
      DataLog(log_level_proc_error) << "Safety status periodicity check failed" << endmsg;
   }
   else
   {
      _SafetyStatusRcvd = false;
   }
}



void Proc::alarmActive ()
{
   ProcData        pd;

   AlarmQueueEntry e = _AlarmActiveMsg.getData();
   pd.AlarmQ().add(e);

   if ( e.type() <= A1 )
   {
      //
      // Once an unrecoverable alarm has occurred, make sure that
      // we are not allowed to continue any run which may be in
      // progress.
      //
      pd.PFRHandler().MarkUnrecoverable();
   }
}



void Proc::alarmInactive ()
{
   ProcData        pd;
   AlarmQueueEntry e = _AlarmInactiveMsg.getData();
   pd.AlarmQ().remove(e.name() );
}



void Proc::guiAlive ()
{
   _GUIResponded = true;
}



void Proc::forceLogging ()
{
   ProcData pd;

   if (
      //
      // Check for cassette movement ...
      ( _PrevCassettePos != pd.Status()._cassettePos )
      //
      // Check for door movement ...
      || ( _PrevDoorStatus != pd.Status()._doorStatus )
      //
      // Check for cycle change
      || ( pd.Run().CycleSwitch.Get())
      )
   {
      _PrevCassettePos = pd.Status()._cassettePos;
      _PrevDoorStatus  = pd.Status()._doorStatus;

      PeriodicLog::forceOutput();
   }

}



void Proc::Cleanup ()
{
   if ( MessageSystem::MsgSystem()->dispatcher().receivedSignal() == SIGHUP )
   {
      DataLog(log_level_proc_info) << "Proc attempting PF save" << endmsg;

      //
      // Allow PFR handler to save power fail data to disk if necessary, then
      // restore task priority to the original value.
      //
      ProcData pd;
      pd.PFRHandler().DoPFSave();

      DataLog(log_level_proc_pfr_info) << "PF save complete" << endmsg;

      //
      // Sleep for a while.  Since the cleanup is due to a power fail, this makes sure
      // that proc won't use CPU time which could be used for other power down tasks.
      //
      int iTotalDelay = 0;
      int iRetryCount = 0;

      while ( iRetryCount < 5000 && iTotalDelay < 5000 )
      {
         iTotalDelay += 1000 - taskDelay(1000);
         iRetryCount += 1;
      }
   }
}



void Proc::ConfigurationReceived ()
{
   // needed to set the sound level changes but can be used for any other
   // config changes in the future.

   ProcData pd;
   pd.Orders().SoundLevel(pd.Config().Machine.Get().key_set_audio);
}

/* FORMAT HASH 71ba3d63ebfabd3f705c8c692129e567 */
