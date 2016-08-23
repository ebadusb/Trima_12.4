/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */
#include "messagesystem.h"

#include "needle.hpp"
#include "safproc.h"
#include "s_cont.hpp"
#include "s_dev.hpp"
#include "s_disc.hpp"
#include "s_donor.hpp"
#include "s_door.hpp"
#include "s_exec.hpp"
#include "s_pfr.hpp"
#include "s_pmsg.hpp"
#include "s_power.hpp"
#include "s_res.hpp"
#include "s_state.hpp"

//
//
//   This class creates most of the global objects needed for safety processing.
//   The order of creation is important as different objects depend upon each other.
//   Note especially the objects that are created before power failure recovery is attempted.
//
extern "C" void saf_exec (void)
{
   SafetyExec* pSafetyExec = NULL;

   // Initialize Data Log Level class
   datalog_SetDefaultLevel(log_handle_safe_exec_info);

   // APS cal data
   initialize_APS_cal();

   // FS messaging
   MessageSystem ms;
   ms.initBlocking("saf_exec", 75);

   // instantiate objects for safety commands messaging
   // THIS OBJECT POINTER IS GLOBAL.
   // other objects use this object to effect safe state
   // or command safety h/w as in door solenoid power and power
   // request/test messaging
   paSafetyHardwareCommandsExec
      = new aSafetyHardwareCommandsExec();

   //
   //        ------ start of object creation needed for power fail recovery
   //

   //  Create the safety executive
   pSafetyExec = SafetyExec::instance();
   pSafetyExec->init();
   DataLog(log_level_safe_exec_info) << "SafetyExec started" << endmsg;

   // instantiate objects related to donor monitoring.
   aDonor* paDonor = new aDonor();

   // instantiate containers
   // inlet and return accumulate/contribute volumes so we can treat them as containers
   ACContainer*       pACContainer       = new ACContainer();
   PlateletContainer* pPlateletContainer = new PlateletContainer();
   PlasmaContainer*   pPlasmaContainer   = new PlasmaContainer();
   InletContainer*    pInletContainer    = new InletContainer();
   ReturnContainer*   pReturnContainer   = new ReturnContainer();
   RBCContainer*      pRBCContainer      = new RBCContainer();

   // instantiate cassette
   aCassette_Safety* paCassette = new aCassette_Safety(paDonor);

   // instantiate needle coagulation monitor
   needleMonitor* pNeedleMonitor = new needleMonitor(pInletContainer
                                                     , pReturnContainer);

   // instantiate reservoir monitor
   // this monitor owns all the physiological monitors
   aReservoir_Safety* paReservoir = new aReservoir_Safety(pInletContainer
                                                          , pReturnContainer
                                                          , pACContainer
                                                          , pPlasmaContainer
                                                          , pPlateletContainer
                                                          , pRBCContainer
                                                          , pNeedleMonitor
                                                          , paDonor
                                                          , paCassette);

   // instantiate the Power Fail Recovery object to see if we are in PF
   aPFR_Safety* paPFR_Safety = new aPFR_Safety;
   paPFR_Safety->init(paDonor, paReservoir);

   //
   //        ------ end of object creation needed for power fail recovery
   //


   // instantiate object that watches the commanded state of the machine
   aProcOpMonitor_Safety* paProcOpMonitor = new aProcOpMonitor_Safety();

   // instantiate switches.
   aPauseSwitch_Safety* paPauseSwitch = new aPauseSwitch_Safety(paProcOpMonitor);
   aStopSwitch_Safety*  paStopSwitch  = new aStopSwitch_Safety(paProcOpMonitor);

   // instantiate CRC timer
   aCRC_Timer* paCRC = new aCRC_Timer();

   // instantiate power test objects
   aSafety24vPowerTest* paSafety24vPowerTest = new aSafety24vPowerTest();
   aSafety64vPowerTest* paSafety64vPowerTest = new aSafety64vPowerTest();

   // instantiate centrifuge monitor
   aCentrifuge_Safety* paCentrifuge = new aCentrifuge_Safety();

   // instantiate door monitor
   aDoor_Safety* paDoor = new aDoor_Safety(paCentrifuge, paSafety24vPowerTest);

   // instantiate the blink test
   aBlinkTest_Safety* pBlinkTest = new aBlinkTest_Safety();

   // instantiate donor disconnect test object
   aDonorDisconnectTest* paDonorDisconnectTest
      = new aDonorDisconnectTest(0, paDonor, paCassette, paReservoir);

   // instantiate message receiver for disposable ID type
   aDisposableIDMsg_rcvr* paDisposableIDMsg_rcvr = new aDisposableIDMsg_rcvr(paReservoir);

   // instantiate message receiver to allow Procedure to instruct Safety to make a cycle switch
   aMidCycleSwitchMsg_rcvr* paMidCycleSwitchMsg_rcvr = new aMidCycleSwitchMsg_rcvr(paReservoir);

   // instantiate message receiver to allow Procedure to inform Safety that it has initiated an A2 alarm
   aNotifySafetyOfA2Msg_rcvr* paNotifySafetyOfA2Msg_rcvr = new aNotifySafetyOfA2Msg_rcvr();

   // instantiate message receiver for processing donor information
   aProcToSafetyDonorVitalsMsg_rcvr* paProcToSafetyDonorVitalsMsg_rcvr
      = new aProcToSafetyDonorVitalsMsg_rcvr(paDonor);

   // instantiate message receiver for power requests
   aRequestSafetyPowerMsg_rcvr* paRequestSafetyPowerMsg_rcvr
      = new aRequestSafetyPowerMsg_rcvr(paDoor
                                        , paCentrifuge
                                        , paSafety24vPowerTest
                                        , paSafety64vPowerTest
                                        , paDonorDisconnectTest);

   // instantiate message receiver to update for constrol status
   aControlStatusMsg_rcvr* paControlStatusMsg_rcvr
      = new aControlStatusMsg_rcvr(paDoor
                                   , paReservoir
                                   , paSafety24vPowerTest
                                   , paSafety64vPowerTest
                                   , paDonorDisconnectTest
                                   , pSafetyExec->AppData());

   // instantiate message receiver to update for control commands
   aControlCommandsMsg_rcvr* paControlCommandsMsg_rcvr
      = new aControlCommandsMsg_rcvr(paProcOpMonitor
                                     , paCentrifuge
                                     , pSafetyExec->AppData());

   // instantiate message receiver to update for safety status
   aSafetyStatusMsg_rcvr* paSafetyStatusMsg_rcvr
      = new aSafetyStatusMsg_rcvr(paPauseSwitch
                                  , paStopSwitch
                                  , paCassette
                                  , paDoor
                                  , paCentrifuge
                                  , paReservoir
                                  , paDonor
                                  , pNeedleMonitor
                                  , pBlinkTest
                                  , pSafetyExec->AppData());

   // instantiate message receiver to update state transition
   aSafetySystemStateChangeMsg_rcvr* paSafetySystemStateChangeMsg_rcvr
      = new aSafetySystemStateChangeMsg_rcvr(paCassette
                                             , paReservoir
                                             , paDoor
                                             , paDonorDisconnectTest
                                             , paCentrifuge
                                             , paCRC
                                             , paRequestSafetyPowerMsg_rcvr
                                             , paMidCycleSwitchMsg_rcvr
                                             , paDonor
                                             , pNeedleMonitor
                                             , pBlinkTest);

   // instantiate message receiver for safety alive messaging
   aQuerySafetyAliveMsg_rcvr* paQuerySafetyAliveMsg_rcvr
      = new aQuerySafetyAliveMsg_rcvr(paControlCommandsMsg_rcvr
                                      , paControlStatusMsg_rcvr
                                      , paSafetyStatusMsg_rcvr
                                      , paPFR_Safety
                                      , pSafetyExec->AppData());

   // instantiate message receiver to test for safety command messages that
   // did not originate from saf_exec
   aSafetyCommandsMsg_rcvr* paSafetyCommandsMsg_rcvr =
      new aSafetyCommandsMsg_rcvr(pSafetyExec->AppData());

   // if this is a possible power fail recovery run, restore the data
   paSafetySystemStateChangeMsg_rcvr->ifPFRThenRestore();

   // if we need to time until the pump starts, start a timer.
   // put this after the restore so we don't destroy any timer
   // which was restored because it was already running
   paPFR_Safety->ifPFRTimerNeeded(pNeedleMonitor);

   //
   // Ensure the CRCs are correct
   //
   paCRC->CheckCRCs();

   // FS messaging
   DataLog(log_level_safe_exec_info) << "entering dispatchMessages()" << endmsg;

   ms.dispatchMessages();

   // Power Fail Data Save
   delete paPFR_Safety;
}

/* FORMAT HASH 4148ef4a317c459a262b9c7947afbc9a */
