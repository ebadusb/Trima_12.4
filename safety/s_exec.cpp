/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      s_exec.cpp
 *             Safety Executive class implementations.
 *
 *
 * ABSTRACT:   This file contains the class methods for the message handlers
 *
 *
 ******************************************************************************/
#include "trima_datalog.h"
#include "s_exec.hpp"
#include "s_glob.hpp"
#include "s_disc.hpp"
#include "s_power.hpp"
#include "s_dev.hpp"
#include "state_objects.hpp"
#include "s_names.hpp"
#include "load_module.h"
#include "task_start.h"
#include "trima_tasks.h"

// monitors and processors
#include "pump_monitor.hpp"
#include "delta_vol_calc.hpp"
#include "cassette_monitor.hpp"

// For PF Recovery
#include "s_pfr.hpp"



#define MAX_SAFETY_MESSAGE_LATENCY  2190  // in ms.  max time allowed between send and
#define CHW_LATENCY_WARNING_TIME    1000  // in ms.  Messages should be every 500 ms.  Warn at 1000
#define LOG_SAFETY_MESSAGE_LATENCY  1000  // in ms.  log if delivery time exceeds for safety status messages.



// 07/29/96 false echo processing
// must see FALSE_ECHO_FILTER occurances of reservoir indicating middle
// following an empty event before another empty event will
// switch the cycle from draw to return for the purposes of infusion monitoring in safety.
//
#define FALSE_ECHO_FILTER                    5



// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataFromRestore;

SafetyExec*        SafetyExec::m_instance = NULL;

SafetyExec::SafetyExec()
   : groups(ModuleGroup::LAST_GROUP),
     execData(),
     _procState(FIRST_STATE),
     _setType(SET_UNKNOWN),
     _pump_stroke_volume_logging(false),
     _strokeCalculatorP(NULL),
     _appData(NULL)
{}

void SafetyExec::init ()
{
   // Get the point to the Safety App Storage Area
   _appData = safetyRamAppData();

   // Initialize the sentinel value
   _appData->sentinel = 0xCAFEBABE;

   safetyRamPrintData();

   // monitor group attached to Control status messages
   MonitorGroup* controlStatus_monitors = new MonitorGroup(ModuleGroup::CONTROL_STATUS);
   controlStatus_monitors->addPreProcessor(new DeltaVolCalculator());
   controlStatus_monitors->add(new PumpMonitor());

   addGroup(controlStatus_monitors);

   DataLog(log_level_safe_exec_debug) << "Control status group created" << endmsg;

   // monitor group attached to Safety status messages
   MonitorGroup* controlCmd_monitors = new MonitorGroup(ModuleGroup::CONTROL_COMMAND);
   _strokeCalculatorP = new StrokeCalculator();
   controlCmd_monitors->addPreProcessor(_strokeCalculatorP);

   addGroup(controlCmd_monitors);

   DataLog(log_level_safe_exec_debug) << "Control command group created" << endmsg;

   // monitor group attached to state change messages
   MonitorGroup* stateChange_monitors = new MonitorGroup(ModuleGroup::STATE_CHANGE);
   stateChange_monitors->add(new HallT1Monitor());

   addGroup(stateChange_monitors);

   DataLog(log_level_safe_exec_debug) << "State change group created" << endmsg;

   const int fd =  open(PUMP_STROKE_LOGGING_ON, O_RDONLY, DEFAULT_FILE_PERM);

   if (fd != ERROR)
   {
      _pump_stroke_volume_logging = true;
      close(fd);
   }
   else
   {
      _pump_stroke_volume_logging = false;
   }

   DataLog(log_level_safe_exec_info) << "pump_stroke_volume_logging: " << _pump_stroke_volume_logging << endmsg;
   DataLog(log_level_safe_exec_debug) << "Safety Executive created" << endmsg;

   //
   //
   //  Transition safety to self test state
   //
   update(SELF_TEST);
}

/**
 * Update callback for safety hardware
 * status messages.
 * */
void SafetyExec::update (const SHwStates& state)
{
   _appData->here = 1;               // bread crumb

   // in addition to transferring message data
   // we'll offset the halls using pre-PFR data
   execData.safety.pumps[RETURN_PUMP].rpm        = state.returnRPM;
   execData.safety.pumps[RETURN_PUMP].deltaHalls = state.returnRevs;
   execData.safety.pumps[RETURN_PUMP].accumHalls = state.returnAccumRevs
                                                   + execData.safety.pumps[RETURN_PUMP].offsetHalls;

   execData.safety.pumps[AC_PUMP].rpm        = state.acRPM;
   execData.safety.pumps[AC_PUMP].deltaHalls = state.acRevs;
   execData.safety.pumps[AC_PUMP].accumHalls = state.acAccumRevs
                                               + execData.safety.pumps[AC_PUMP].offsetHalls;

   execData.safety.pumps[INLET_PUMP].rpm        = state.inletRPM;
   execData.safety.pumps[INLET_PUMP].deltaHalls = state.inletRevs;
   execData.safety.pumps[INLET_PUMP].accumHalls = state.inletAccumRevs
                                                  + execData.safety.pumps[INLET_PUMP].offsetHalls;

   execData.safety.pumps[PLASMA_PUMP].rpm        = state.plasmaRPM;
   execData.safety.pumps[PLASMA_PUMP].deltaHalls = state.plasmaRevs;
   execData.safety.pumps[PLASMA_PUMP].accumHalls = state.plasmaAccumRevs
                                                   + execData.safety.pumps[PLASMA_PUMP].offsetHalls;

   execData.safety.pumps[PLATELET_PUMP].rpm        = state.plateletRPM;
   execData.safety.pumps[PLATELET_PUMP].deltaHalls = state.plateletRevs;
   execData.safety.pumps[PLATELET_PUMP].accumHalls = state.plateletAccumRevs
                                                     + execData.safety.pumps[PLATELET_PUMP].offsetHalls;

   _appData->here = 2;               // bread crumb

   process(ModuleGroup::SAFETY_STATUS);

   _appData->here = 3;               // bread crumb

   SaveToPFR();

   _appData->here = 4;               // bread crumb

   // Print out the data every once in a while ~10 minutes
   if (_appData->counter++ % 1200 == 0)
   {
      safetyRamPrintData();
   }
}

/**
 * Update callback for control hardware
 * status messages.
 * */
void SafetyExec::update (const CHwStates& state)
{
   _appData->here = 5;               // bread crumb

   // copy over pump data, including offsets
   execData.control.pumps[RETURN_PUMP].revs  = state.returnPump.Revs;
   execData.control.pumps[RETURN_PUMP].accum = state.returnPump.Accum
                                               + execData.control.pumps[RETURN_PUMP].offset;

   execData.control.pumps[AC_PUMP].revs  = state.acPump.Revs;
   execData.control.pumps[AC_PUMP].accum = state.acPump.Accum
                                           + execData.control.pumps[AC_PUMP].offset;

   execData.control.pumps[INLET_PUMP].revs  = state.inletPump.Revs;
   execData.control.pumps[INLET_PUMP].accum = state.inletPump.Accum
                                              + execData.control.pumps[INLET_PUMP].offset;

   execData.control.pumps[PLASMA_PUMP].revs  = state.plasmaPump.Revs;
   execData.control.pumps[PLASMA_PUMP].accum = state.plasmaPump.Accum
                                               + execData.control.pumps[PLASMA_PUMP].offset;

   execData.control.pumps[PLATELET_PUMP].revs  = state.plateletPump.Revs;
   execData.control.pumps[PLATELET_PUMP].accum = state.plateletPump.Accum
                                                 + execData.control.pumps[PLATELET_PUMP].offset;

   execData.control.valves[RBC_VALVE]      = state.rbcValve;
   execData.control.valves[PLASMA_VALVE]   = state.plasmaValve;
   execData.control.valves[PLATELET_VALVE] = state.plateletValve;

   // copy over access pressure
   execData.control.accessPressure = state.accessPressure;

   _appData->here                  = 6; // bread crumb

   if (_pump_stroke_volume_logging)
   {
      DataLog(log_level_safe_exec_info) << "safety_pump_stroke_volume Access Pressure: (raw)" << state.accessPressure << endmsg;
   }

   // copy over timestamp
   execData.control.timestamp = state.timestamp;

   _appData->here             = 7;   // bread crumb

   process(ModuleGroup::CONTROL_STATUS);

   _appData->here = 8;               // bread crumb

   SaveToPFR();

   _appData->here = 9;               // bread crumb
}

/**
 * Update callback for control orders
 * messages.
 * */
void SafetyExec::update (const CHwOrders& orders)
{
   _appData->here = 10;               // bread crumb

   // copy over pump orders
   execData.orders.pumpRpm[RETURN_PUMP]   = orders.returnRPM;
   execData.orders.pumpRpm[AC_PUMP]       = orders.acRPM;
   execData.orders.pumpRpm[INLET_PUMP]    = orders.inletRPM;
   execData.orders.pumpRpm[PLASMA_PUMP]   = orders.plasmaRPM;
   execData.orders.pumpRpm[PLATELET_PUMP] = orders.plateletRPM;

   _appData->here = 11;               // bread crumb

   if (_pump_stroke_volume_logging)
   {
      _appData->here = 12;               // bread crumb

      DataLog(log_level_safe_exec_info)
         << "safety_pump_stroke_volume s (Command Speed) sV (Stroke Volume), Return pump s: " << orders.returnRPM << " sV: " << _strokeCalculatorP->calcReturnPump()
         << " AC pump s: " << orders.acRPM << " sV: " << _strokeCalculatorP->calcACPump()
         << " Inlet pump s: " << orders.inletRPM << " sV: " << _strokeCalculatorP->calcInletPump()
         << " Plasma pump s: " << orders.plasmaRPM << " sV: " << _strokeCalculatorP->calcPlasmaPump()
         << " Platelet pump s: " << orders.plateletRPM << " sV: " << _strokeCalculatorP->calcPlateletPump()
         << endmsg;
   }
   _appData->here = 13;               // bread crumb

   process(ModuleGroup::CONTROL_COMMAND);

   _appData->here = 14;               // bread crumb
}

/**
 * Update callback for proc system
 * state messages.
 * */
void SafetyExec::update (State_names state)
{
   _appData->here = 15;               // bread crumb

   _procState     = state;
   DataLog(log_level_safe_exec_info) << "Safety setting state " << state << endmsg;

   // enable/disable monitors based on state
   switch (state)
   {
      case FIRST_STATE :
         break;

      case SELF_TEST :
         break;

      case STARTUP_TESTS :
         break;

      case DISPOSABLE_TEST :
      {
         _appData->here = 16;              // bread crumb

         MonitorGroup* group = getGroup(ModuleGroup::STATE_CHANGE);

         _appData->here = 17;             // bread crumb

         if (group)
            group->enable("HallT1Monitor");

         _appData->here = 18;             // bread crumb
      }

      break;

      case AC_CONNECTED :
         break;

      case AC_PRIME :
         break;

      case DONOR_CONNECTED :
         break;

      case BLOOD_PRIME :
         break;

      case BLOOD_RUN :
         break;

      case BLOOD_RINSEBACK :
         break;

      case DONOR_DISCONNECT :
      {
         _appData->here = 19;                 // bread crumb

         // If there is not a set type or if it is unknown, default to plt/pls/rbc because this is
         // the disconnect test only and plt/pls/rbc will be safe no matter
         // what the real set type is.
         //
         if ((_setType == SET_UNKNOWN) || (_setType == SET_NOT_LOADED))
         {
            _appData->here = 20;               // bread crumb

            update(SET_PLTPLSRBC);
         }
      }
      break;

      case POST_RUN :
         break;

      case METERED_STORAGE :
         break;

      case METERED_STORAGE_DISCONNECT :
         break;

      default :
         break;
   }

   _appData->here = 21;               // bread crumb

   process(ModuleGroup::STATE_CHANGE);

   _appData->here = 22;               // bread crumb

   SaveToPFR();

   _appData->here = 23;               // bread crumb
}

/**
 * Update callback for disposable set type
 * message sent by proc
 **/
void SafetyExec::update (SET_TYPE type)
{
   _appData->here = 24;               // bread crumb

   _setType       = type;

   process(ModuleGroup::SET_TYPE);

   _appData->here = 25;               // bread crumb

   SaveToPFR();

   _appData->here = 26;               // bread crumb
}


//
//
//   The safety executive is responsible for saving its own data (below).  It is
//   also responsible for restoring data for both itself and the monitor groups.
//   This is because the design for the monitors uses data that is encapsulated
//   by the safety exec data class.
//
void SafetyExec::SaveToPFR ()
{
   _appData->here = 27;               // bread crumb

   SafetyPFRDataToSave.PFR_SetType     = _setType;
   SafetyPFRDataToSave.PFR_SystemState = _procState;
}

void SafetyExec::RestoreFromPFR ()
{
   _appData->here = 28;               // bread crumb

   _setType       = SafetyPFRDataFromRestore.PFR_SetType;
   _procState     = SafetyPFRDataFromRestore.PFR_SystemState;

   //
   //
   //   Note that although there can be any number of groups, the PFR data
   //   is stored at the group level (i.e. not on a per-monitor basis).
   //   Therefore any group can be used to call the PFR recovery method.
   //
   _appData->here = 29;               // bread crumb

   MonitorGroup* mg = groups[ModuleGroup::CONTROL_STATUS];

   _appData->here = 30;               // bread crumb

   mg->RestoreFromPFR();

   _appData->here = 31;               // bread crumb
}

void SafetyExec::process (ModuleGroup::Group group)
{
   _appData->here = 32;               // bread crumb

   MonitorGroup* mg = groups[(int)group];

   _appData->here = 33;               // bread crumb

   if (mg)
      mg->check();
}



aSafetyStatusMsg_rcvr::aSafetyStatusMsg_rcvr(aPauseSwitch_Safety* paPauseSwitch
                                             , aStopSwitch_Safety* paStopSwitch
                                             , aCassette_Safety* paCassette
                                             , aDoor_Safety* paDoor
                                             , aCentrifuge_Safety* paCentrifuge
                                             , aReservoir_Safety* paRes
                                             , aDonor* paDonor
                                             , needleMonitor* needle
                                             , aBlinkTest_Safety* blink
                                             , safetyRamSafetyApplicationData* appData)
   : _theSHwStatesMsg(Callback<aSafetyStatusMsg_rcvr>(this, &aSafetyStatusMsg_rcvr::notify), MessageBase::SNDRCV_RECEIVE_ONLY),
     _paPauseSwitch(paPauseSwitch),
     _paStopSwitch(paStopSwitch),
     _paCassette(paCassette),
     _paDoor(paDoor),
     _paCentrifuge(paCentrifuge),
     _paRes(paRes),
     _paDonor(paDonor),
     _needle(needle),
     _blink(blink),
     _appData(appData),
     _cT1Status(T1_NOT_COMPLETE),
     _iSafetyStatusCount(0)
{
   trima_assert(paPauseSwitch);
   trima_assert(paStopSwitch);
   trima_assert(paCassette);
   trima_assert(paDoor);
   trima_assert(paCentrifuge);
   trima_assert(paRes);
   trima_assert(paDonor);
   trima_assert(needle);
   trima_assert(blink);
   trima_assert(appData);

   // 16Dec97 initialize the Safety Status Structure data area to 0
   memset(&SHwStateData, 0, sizeof(SHwStates));
}



aSafetyStatusMsg_rcvr::aSafetyStatusMsg_rcvr::~aSafetyStatusMsg_rcvr()
{
   _paPauseSwitch = NULL;
   _paStopSwitch  = NULL;
   _paCassette    = NULL;
   _paDoor        = NULL;
   _paCentrifuge  = NULL;
   _paRes         = NULL;
   _paDonor       = NULL;
   _needle        = NULL;
   _blink         = NULL;
   _appData       = NULL;
}



// alarms if someone tries to order service mode while this code is running
void aSafetyStatusMsg_rcvr::checkForbiddenMode (HW_ORDERS mode)
{
   _appData->here = 34;               // bread crumb

   if (mode == HW_ENABLE)
   {
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
      DataLog(log_level_safety_alarm_detail) << "Forbidden state serviceEnableMode = " << (int)mode << endmsg;
   }

   _appData->here = 35;              // bread crumb
}



// SPECIFICATION:    notify for safety status msg receiver
//
//
// ERROR HANDLING:   various alarms, fatal error.

void aSafetyStatusMsg_rcvr::notify ()
{
   SHwStateData   = _theSHwStatesMsg.getData();

   _appData->here = 37;              // bread crumb

   // if we are in donor connect mode, check the message delivery
   // latency on this message type. this message needs to be delivered
   // within 500ms of being sent.
   deliveryTime();

   _appData->here = 38;              // bread crumb

   // update centrifuge status
   _paCentrifuge->Update(SHwStateData.centrifugeRPM
                         , SHwStateData.centrifugeError);

   _appData->here = 39;              // bread crumb

   // update door status
   _paDoor->Update(SHwStateData.doorLocks);

   _appData->here = 40;              // bread crumb

   // 06/27/96 when spin centrifuge by hand, can exceed 60 RPM so also gate this alarm with
   // state vbl indicating if safety has enabled 64V power( based on last safety command ).
   if ( paSafetyHardwareCommandsExec->GetCentrifugePower() == HW_ENABLE )
      _paDoor->UpdateAndTestDoorFault();

   _appData->here = 41;              // bread crumb

   // update cassette status
   _paCassette->Update(SHwStateData.cassettePosition);

   _appData->here = 42;              // bread crumb

   // update reservoir status
   _paRes->UpdateReservoirSafety(SHwStateData.reservoir);

   _appData->here = 43;              // bread crumb

   // check blink test status
   _blink->Update(SHwStateData.valveLedTest);

   _appData->here = 44;              // bread crumb

   // check for service mode
   checkForbiddenMode(SHwStateData.serviceEnableMode);

   _appData->here = 45;              // bread crumb

   switch ( SHwStateData.event )
   {
      case SHW_NORMAL_UPDATE :

         SafetyExec::instance()->update(SHwStateData);

         // update the hall data and compare to encoders
         _paRes->UpdateSafety(SHwStateData);

         _appData->here = 46;           // bread crumb

         break;

      case SHW_PAUSE_EVENT :

         _paPauseSwitch->SetSwitchTimerStatus(TIMER_ARMED);

         _appData->here = 47;           // bread crumb

         // if there is not AC in the needle, start a pump power timer
         if (_paRes->shouldWeWorry())
            _needle->turnOnTimer(PUMP_MAX_DURATION);

         _appData->here = 48;           // bread crumb

         break;

      case SHW_STOP_EVENT :

         _paStopSwitch->SetSwitchTimerStatus(TIMER_ARMED);

         _appData->here = 49;           // bread crumb

         // if there is not AC in the needle, start a pump power timer
         if (_paRes->shouldWeWorry())
            _needle->turnOnTimer(PUMP_MAX_DURATION);

         _appData->here = 50;           // bread crumb

         break;

      case SHW_DOOR_EVENT :

         break;

      case SHW_RESERVOIR_EVENT :
         // check to make sure first fluid is during Blood Prime
         _paRes->firstFluid();

         _appData->here = 51;           // bread crumb

         break;

      case SHW_RETURN_EVENT :

         break;

      case SHW_AIR_EVENT :
         if (_paDonor->GetDonorConnectedState() == DONOR_IS_CONNECTED )
         {
            paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DONOR_AIR_ALARM);

            _appData->here = 52;             // bread crumb
         }
         else
         {
            DataLog(log_level_safe_exec_info) << "Safety saw SHW_AIR_EVENT with no donor connected" << endmsg;
         }
         break;

      case SHW_RETURN_PUMP_TOO_FAST_EVENT :

         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, RETURN_PUMP_HALL_TOO_BIG);

         _appData->here = 53;           // bread crumb

         break;

      case SHW_AC_PUMP_TOO_FAST_EVENT :

         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_PUMP_HALL_TOO_BIG);

         _appData->here = 54;           // bread crumb

         break;

      default :
      {
         DataLog(log_level_critical) << "Undefined SHwStateData.event: "
                                     << (int)SHwStateData.event
                                     << "  causes forbidden case switch default"
                                     << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "invalid event type");
      }

      break;

   }

   _appData->here = 55;              // bread crumb

   ++_iSafetyStatusCount;

} // END aSafetyStatusMsg_rcvr :: notify()



// SPECIFICATION:    calculates elapsed time between message sent
//                   and message delivery for safety status msg receiver.
//                   in-sensitive to setting of the real-time clock
//
//
// ERROR HANDLING:   SW Fault alarm

void aSafetyStatusMsg_rcvr::deliveryTime (void)
{
   // don't test if the donor is not connected.  During initialization
   // of tasks, especially during cal.dat file read, messages can be delayed.
   if ( SHwStateData.donorConnectMode == HW_DISABLE )
      return;

   // alarm if we exceed the maximum allowed latency.
   // log it if we get close.
#ifndef _NONTRIMA
   const unsigned int elapsedTime = _theSHwStatesMsg.latency();
#else
   const unsigned int elapsedTime = 0; // Magic!
#endif

   _appData->here = 56;              // bread crumb

   if ( elapsedTime > MAX_SAFETY_MESSAGE_LATENCY )
   {
      _appData->here = 57;              // bread crumb

      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);

      DataLog(log_level_safety_alarm_detail) << "SafetyStatusMsg delivery time of "
                                             << elapsedTime << " exceeds " << MAX_SAFETY_MESSAGE_LATENCY << " ms." << endmsg;
   }
   else if ( elapsedTime > LOG_SAFETY_MESSAGE_LATENCY )
   {
      DataLog(log_level_safe_exec_info) << "SafetyStatusMsg delivery time = " << elapsedTime << " ms." << endmsg;
   }
}



aQuerySafetyAliveMsg_rcvr::aQuerySafetyAliveMsg_rcvr(aControlCommandsMsg_rcvr* paControlCommandsMsg_rcvr
                                                     , aControlStatusMsg_rcvr* paControlStatusMsg_rcvr
                                                     , aSafetyStatusMsg_rcvr* paSafetyStatusMsg_rcvr
                                                     , aPFR_Safety* paPFR_Safety
                                                     , safetyRamSafetyApplicationData* appData)
   : _theQuerySafetyAliveMsg(Callback<aQuerySafetyAliveMsg_rcvr>(this, &aQuerySafetyAliveMsg_rcvr::notify), MessageBase::SNDRCV_RECEIVE_ONLY),
     _panAnswerSafetyAliveMsg_sndr(NULL),
     _paControlCommandsMsg_rcvr(paControlCommandsMsg_rcvr),
     _paControlStatusMsg_rcvr(paControlStatusMsg_rcvr),
     _paSafetyStatusMsg_rcvr(paSafetyStatusMsg_rcvr),
     _appData(appData),
     _paPFR_Safety(paPFR_Safety),
     _PFRStatusMsgSent(FALSE)
{
   trima_assert(paControlCommandsMsg_rcvr);
   trima_assert(paControlStatusMsg_rcvr);
   trima_assert(paSafetyStatusMsg_rcvr);
   trima_assert(appData);
   trima_assert(paPFR_Safety);

   _panAnswerSafetyAliveMsg_sndr = new AnswerSafetyAliveMsg(MessageBase::SEND_GLOBAL);
}



aQuerySafetyAliveMsg_rcvr::~aQuerySafetyAliveMsg_rcvr()
{
   delete _panAnswerSafetyAliveMsg_sndr;
   _panAnswerSafetyAliveMsg_sndr = NULL;

   _paControlCommandsMsg_rcvr    = NULL;
   _paControlStatusMsg_rcvr      = NULL;
   _paSafetyStatusMsg_rcvr       = NULL;
   _paPFR_Safety                 = NULL;
   _appData                      = NULL;
}



// SPECIFICATION:    notify for proc to safety query alive msg.
//
//
// ERROR HANDLING:   none.

void aQuerySafetyAliveMsg_rcvr::notify ()
{
   if ( (_paSafetyStatusMsg_rcvr->GetSafetyStatusCount() != 0)
        &&(_paControlCommandsMsg_rcvr->GetControlCommandsCount() != 0)
        &&(_paControlStatusMsg_rcvr->GetControlStatusCount() != 0) )
   {
      _appData->here = 58;              // bread crumb

      // OK to respond to ping
      DataLog(log_level_safe_exec_debug) << "aQuerySafetyAliveMsg_rcvr :: notify() sending AnswerSafetyAliveMsg(true)" << endmsg;
      _panAnswerSafetyAliveMsg_sndr->send( (long)true);

      _appData->here = 59;              // bread crumb

      // tell system alarm state is not asserted
      paSafetyHardwareCommandsExec->SetAlarmState(NO_SAFETY_ALARM);

      _appData->here = 60;              // bread crumb

      // turn alarm light off
      paSafetyHardwareCommandsExec->SetAlarmLight(HW_DISABLE);

      _appData->here = 61;              // bread crumb

      // 05/15/97 door power T1, init with disable
      paSafetyHardwareCommandsExec->SetDoorPower(HW_DISABLE);

      _appData->here = 62;              // bread crumb

      // Let proc know status of safety PFR file, since we know at this point
      // that proc must be running.
      if ( !_PFRStatusMsgSent )
      {
         _paPFR_Safety->SendPFRStatusMsg();
         _PFRStatusMsgSent = true;
      }

      _appData->here = 63;              // bread crumb
   }
   else
   {
      DataLog(log_level_safe_exec_info) << "Safety Status Message Count: "
                                        << _paSafetyStatusMsg_rcvr->GetSafetyStatusCount()
                                        << endmsg;
      DataLog(log_level_safe_exec_info) << "Control Commands Message Count: "
                                        << _paControlCommandsMsg_rcvr->GetControlCommandsCount()
                                        << endmsg;
      DataLog(log_level_safe_exec_info) << "Control Status Message Count: "
                                        << _paControlStatusMsg_rcvr->GetControlStatusCount()
                                        << endmsg;
   }

} // END void aQuerySafetyAliveMsg_rcvr :: notify()



aControlCommandsMsg_rcvr::aControlCommandsMsg_rcvr(aProcOpMonitor_Safety* paProcOpMonitor
                                                   , aCentrifuge_Safety* paCentrifuge
                                                   , safetyRamSafetyApplicationData* appData)
   : _theCHwOrdersMsg(Callback<aControlCommandsMsg_rcvr>(this, &aControlCommandsMsg_rcvr::notify), MessageBase::SNDRCV_RECEIVE_ONLY),
     _paProcOpMonitor(paProcOpMonitor),
     _paCentrifuge(paCentrifuge),
     _appData(appData),
     _iControlCommandsCount(0)
{
   trima_assert(paProcOpMonitor);
   trima_assert(paCentrifuge);
   trima_assert(appData);

   // 30Dec2002 initialize the Control Commands Structure data area to 0
   memset(&CHwOrderData, 0, sizeof(CHwOrders));
}



aControlCommandsMsg_rcvr::~aControlCommandsMsg_rcvr()
{
   _paProcOpMonitor       = NULL;
   _paCentrifuge          = NULL;
   _appData               = NULL;
   _iControlCommandsCount = 0;
}



// SPECIFICATION:    notify for control orders msg rcvr.
//
//
// ERROR HANDLING:   none.

void aControlCommandsMsg_rcvr::notify ()
{
   CHwOrderData   = _theCHwOrdersMsg.getData();

   _appData->here = 64;              // bread crumb

   SafetyExec::instance()->update(CHwOrderData); // RKM.  Add processing here

   _appData->here = 65;              // bread crumb

   // update data for command monitor
   _paProcOpMonitor->Update(CHwOrderData);

   _appData->here = 66;              // bread crumb

   // update order data for centrifuge
   _paCentrifuge->SetCommandRPM(CHwOrderData.centrifugeRPM);

   _appData->here = 67;              // bread crumb

   if (_iControlCommandsCount == 0)
   {
      _iControlCommandsCount = CHwOrderData.sequenceNumber;
   }
   else if (CHwOrderData.sequenceNumber - 1 != _iControlCommandsCount)
   {
      // For now, just log the event.  In the future, we may track the drops and do something with them.
      DataLog(log_level_safe_exec_error) << "Received out of sequence Control Order packet.  Expected "
                                         << _iControlCommandsCount << ", got " << CHwOrderData.sequenceNumber << endmsg;
      _iControlCommandsCount = CHwOrderData.sequenceNumber;
   }
   else
      _iControlCommandsCount = CHwOrderData.sequenceNumber;

} // END void aControlCommandsMsg_rcvr :: notify()



aControlStatusMsg_rcvr::aControlStatusMsg_rcvr(aDoor_Safety* paDoor,
                                               aReservoir_Safety* paReservoir,
                                               aSafety24vPowerTest* pa24vPowerTest,
                                               aSafety64vPowerTest* pa64vPowerTest,
                                               aDonorDisconnectTest* paDonorDisconnectTest,
                                               safetyRamSafetyApplicationData* appData)
   : _theCHwStatesMsg(Callback<aControlStatusMsg_rcvr>(this, &aControlStatusMsg_rcvr::notify), MessageBase::SNDRCV_RECEIVE_ONLY),
     _theAPSZeroMsg(Callback<aControlStatusMsg_rcvr>(this, &aControlStatusMsg_rcvr::newAPSZero), MessageBase::SNDRCV_RECEIVE_ONLY),
     _requestSafetyStatusMsg(MessageBase::SEND_LOCAL),
     _paDoor(paDoor),
     _paReservoir(paReservoir),
     _pa24vPowerTest(pa24vPowerTest),
     _pa64vPowerTest(pa64vPowerTest),
     _paDonorDisconnectTest(paDonorDisconnectTest),
     _appData(appData),
     _iControlStatusCount(0),
     _lowLevelIgnore(false),
     _lowLevelDebounceCount(0)
{
   trima_assert(paDoor);
   trima_assert(paReservoir);
   trima_assert(pa24vPowerTest);
   trima_assert(pa64vPowerTest);
   trima_assert(paDonorDisconnectTest);
   trima_assert(appData);
}



aControlStatusMsg_rcvr::~aControlStatusMsg_rcvr()
{
   _paDoor                = NULL;
   _paReservoir           = NULL;
   _pa24vPowerTest        = NULL;
   _pa64vPowerTest        = NULL;
   _paDonorDisconnectTest = NULL;
   _appData               = NULL;
}


// SPECIFICATION:    notify for control status msg rcvr.
//
//
// ERROR HANDLING:   none.

void aControlStatusMsg_rcvr::notify ()
{
   const CHwStates CHwStateData = _theCHwStatesMsg.getData();

   _appData->here = 68;              // bread crumb

   //
   // take the timestamp off the message header in order
   // to calculate delta time from the control side.
   // some calculations using encoder data need to use
   // control time to be correct (see return pump speed).
   //
   const struct timespec sendTime = CHwStateData.timestamp;

   // request safety status for matching halls and encoders
   if ( (CHwStateData.event == CHW_NORMAL_UPDATE) ||
        (CHwStateData.event == CHW_RESERVOIR_EVENT) )
   {
      if (_iControlStatusCount == 0)
      {
         // First time, just create the baseline timestamp
         osTime::snapshotRawTime(_lastCHWStatesUpdate);
         DataLog(log_level_safe_exec_info) << "Baselining the CHWStatus update time" << endmsg;
      }
      else
      {
         const int deltaTime = osTime::howLongMilliSecAndUpdate(_lastCHWStatesUpdate);

         if (deltaTime > CHW_LATENCY_WARNING_TIME)
         {
            DataLog(log_level_safe_exec_error) << "WARNING:  CHWStates message delay of " << deltaTime
                                               << " ms." << endmsg;
         }
      }

      _appData->here = 69;              // bread crumb

      DataLog(log_level_safe_exec_debug) << "aControlStatusMsg_rcvr :: notify() sending RequestSafetyStatusMsg(0x0ABCDEF0)" << endmsg;
      _requestSafetyStatusMsg.send(0x0ABCDEF0);   // Got to send SOMETHING, and safe_dvr doesn't care what.

      SafetyExec::instance()->update(CHwStateData);
   }

   _appData->here = 70;              // bread crumb

   if (CHwStateData.sixtyFourVoltSwitched > ACCEPT_64V_OFF_RAW_ADC /* ADC value in which we see the power as off */)
      _paDoor->UpdateAndTestDoorFault();

   _appData->here = 71;              // bread crumb

   // update reservoir level
   _paReservoir->UpdateControl(CHwStateData);

   _appData->here = 72;              // bread crumb

   // update power readings
   _pa24vPowerTest->SetCurrentVoltage(CHwStateData.twentyFourVoltSwitched);
   _pa64vPowerTest->SetCurrentVoltage(CHwStateData.sixtyFourVoltSwitched);

   _appData->here = 73;              // bread crumb

   // update info for disconnect tests
   _paDonorDisconnectTest->Update(calc_APS_mmhg_3_pt(CHwStateData.accessPressure)
                                  , CHwStateData.inletPump.Accum
                                  , CHwStateData.msgCount);

   _appData->here = 74;              // bread crumb

   // if we are debouncing the lower sensor, increment the counter
   if (_lowLevelIgnore && (CHwStateData.reservoir == CHW_RESERVOIR_MIDDLE) )
   {
      if ( ++_lowLevelDebounceCount >= FALSE_ECHO_FILTER )
      {
         _lowLevelIgnore        = false;
         _lowLevelDebounceCount = 0;
      }
   }

   _appData->here = 75;              // bread crumb

   switch ( CHwStateData.event )
   {
      case CHW_NORMAL_UPDATE :
         _appData->here = 76;           // bread crumb

         // this is the normal 500ms update.
         //
         // calculate the delta time between messages with pump data.
         _paReservoir->SetDeltaControlStatusTime(sendTime);

         // update containers and run all the physiological monitors
         _paReservoir->update_res_mon();

         break;

      case CHW_AC_EVENT :
      case CHW_DOOR_EVENT :
      case CHW_PAUSE_EVENT :
      case CHW_STOP_EVENT :
         break;

      case CHW_RESERVOIR_EVENT :
         _appData->here = 77;           // bread crumb

         // this is a reservoir sensor event

         // for reservoir events, we run the physiological monitors first
         // and then switch the cycle.

         // if we have a true reservoir switch event, update the monitors
         if ( (CHwStateData.reservoir == CHW_RESERVOIR_HIGH ) ||
              ( (CHwStateData.reservoir == CHW_RESERVOIR_EMPTY) && !_lowLevelIgnore )
              )
         {
            // calculate the delta time between messages with pump data.
            _paReservoir->SetDeltaControlStatusTime(sendTime);

            // update containers and run all the physiological monitors
            _paReservoir->update_res_mon();
         }

         _appData->here = 78;           // bread crumb

         // if we are normal operation with a high event,switch the cycle
         if ( CHwStateData.reservoir == CHW_RESERVOIR_HIGH )
         {
            _paReservoir->UpdateInfusionTimers(CHwStateData.reservoir);
            _paReservoir->switch_to_return_cycle();
         }

         _appData->here = 79;           // bread crumb

         // if we have a true low reservoir event, force a switch to draw
         if ( (CHwStateData.reservoir == CHW_RESERVOIR_EMPTY ) && !_lowLevelIgnore)
         {
            _paReservoir->UpdateInfusionTimers(CHwStateData.reservoir);
            _paReservoir->switch_to_draw_cycle();

            // set to ignore low level sensor bounce
            _lowLevelIgnore = true;
         }
         break;

      default :
      {
         DataLog(log_level_critical) << "Undefined CHwStateData.event: "
                                     << (int)CHwStateData.event << "  causes forbidden case switch default"
                                     << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "invalid event type");
      }
      break;

   }

   ++_iControlStatusCount;

} // end void  aControlStatusMsg_rcvr :: notify()


// SPECIFICATION:    notify for APS zero message from control
//
//
// ERROR HANDLING:   none.

void aControlStatusMsg_rcvr::newAPSZero (void)
{
   int zero = _theAPSZeroMsg.getData();
   set_APS_mmhg_offset(zero);
}



aSafetyCommandsMsg_rcvr::aSafetyCommandsMsg_rcvr(safetyRamSafetyApplicationData* appData)
   : _theSHwOrdersMsg(Callback<aSafetyCommandsMsg_rcvr>(this, &aSafetyCommandsMsg_rcvr::notify), MessageBase::SNDRCV_RECEIVE_ONLY),
     _appData(appData)
{
   _pidSafExec         = (unsigned long)taskIdSelf();
   _nidSafetyProcessor = 0UL;
}



// SPECIFICATION:    Receiver for SafetyCommands
//                   Alarms if it came from anyone other
//                   than itself
//
//
// ERROR HANDLING:   none.

void aSafetyCommandsMsg_rcvr::notify ()
{
   unsigned long   pid;                // sending pid
   unsigned long   nid;                // sending nid
   struct timespec timestamp;
   SHwOrders       structSHwOrderData;

   // get message data into local structure.
   structSHwOrderData = _theSHwOrdersMsg.getData();

   _appData->here     = 80;          // bread crumb

//   msgHeader( pid, nid, timestamp );
   pid            = _theSHwOrdersMsg.originTask();
   nid            = _theSHwOrdersMsg.originNode();

   _appData->here = 81;              // bread crumb

   if ( pid != _pidSafExec || nid != _nidSafetyProcessor )
   {
      // illegal msg
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
      DataLog(log_level_safety_alarm_detail) << "Forbidden safety command msg _pidSafExec="
                                             << _pidSafExec
                                             << ",sndrpid="
                                             << pid
                                             << ",_nidSafetyProcessor="
                                             << _nidSafetyProcessor
                                             << ",sndrnid="
                                             << nid
                                             << endmsg;
   }

}  // END void aSafetyCommandsMsg_rcvr :: notify()

/* FORMAT HASH bf98220d76b2c5761c8ec75bebfca1f8 */
