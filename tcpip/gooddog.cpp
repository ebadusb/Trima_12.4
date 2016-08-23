/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      gooddog.cpp
 *             Service Mode Watch Dog task
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   Gooddog is a program that runs in service mode that performs the functions
 *             of safe_exec and proc.  It sends the messages expected by control and safety
 *             drivers needed to keep the watchdogs from going off.
 *
 *             The Hardware Orders are received from TED on the control CPU with in turn receives them
 *             from EMS.  The orders sent out are initially 0's (Safe states).  They are replaced with
 *             actual commanded values when they are received from EMS.  They are repeated every second
 *             by gooddog so that EMS doesn't have to fulfill the responsibility of keeping the Trima
 *             watchdogs alive.
 *
*/



#include "trima_datalog.h"      // Gets Datalog Handles
#include "messagesystem.h"      // Gets Message System
#include "trimamessages.h"      // Gets Trima Int messages


#include "gooddog.h"


static bool serviceModeActive = false;

//
// main routine for GoodDog
//
extern "C" void gooddog ()
{
   DataLog (log_level_gooddog_info) << "GoodDog process started" << endmsg;

   MessageSystem ms;

   // Blocking loop that receives messages.
   ms.initBlocking("GoodDog");

   GoodDog goodDog;

   DataLog (log_level_gooddog_info) << "Initializing GoodDog..." << endmsg;

   if (!goodDog.Initialize())
   {
      DataLog (log_level_gooddog_error) << "GoodDog failed to initialize.  Exiting... " << endmsg;
      return;
   }

   DataLog(log_level_gooddog_info) << "GoodDog is ready to process messages." << endmsg;

   ms.dispatchMessages();

   DataLog(log_level_gooddog_info) << "GoodDog process exiting..." << endmsg;
}



const int TIMER_INTERVAL    = 500; // 1/2 second

const int LATENCY_TIMEOUT   = 1250;

const int MINIMUM_SEND_TIME = 100;


//
// Default constructor
//
GoodDog::GoodDog()
   : _outgoingControlOrdersMsg(MessageBase::SEND_GLOBAL),
     _outgoingSafetyOrdersMsg(MessageBase::SEND_GLOBAL),
     _controlOrdersSequenceNumber(0),
     _safetyOrdersSequenceNumber(0)
{}



//
// Default Destructor
//
GoodDog::~GoodDog()
{}



//
// Initialize
//
bool GoodDog::Initialize ()
{
   // Zero out the member data
   memset(&_pumpDeltaRevs, 0, sizeof(_pumpDeltaRevs));
   memset(&_safetyOrders,  0, sizeof(_safetyOrders));

   _safetyServiceModeMsg.init(Callback<GoodDog>(this, &GoodDog::handleSafetyServiceModeMsg), MessageBase::SNDRCV_GLOBAL);

   // Create the Status Message Handlers
   _controlHwStatusMsg.init(Callback<GoodDog>(this, &GoodDog::HandleControlHwStatusMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   _safetyHwStatusMsg.init (Callback<GoodDog>(this, &GoodDog::HandleSafetyHwStatusMsg),  MessageBase::SNDRCV_RECEIVE_ONLY);
   _controlOrdersFromTed.init(Callback<GoodDog>(this, &GoodDog::HandleControlOrdersMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   _safetyOrdersFromTed.init(Callback<GoodDog>(this, &GoodDog::HandleSafetyOrdersMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Create the Stop GoodDog Message Handler
   _stopGoodDogMsg.init(Callback<GoodDog>(this, &GoodDog::HandleStopGoodDogMsg), MessageBase::SNDRCV_GLOBAL);

   // Create the Orders Timer
   _ordersTimer.init(TIMER_INTERVAL, Callback<GoodDog>(this, &GoodDog::HandleOrdersTimerMsg), TimerMessage::ARMED);

   osTime::snapshotRawTime(_lastOrdersSendTime);

   return true;
}

void GoodDog::handleSafetyServiceModeMsg ()
{
   const int msgData = _safetyServiceModeMsg.getData();

   if ( msgData != 0 )
   {
      DataLog(log_level_service_info) << "Response to safety service request received" << endmsg;
      serviceModeActive = true;
   }
}


//
// HandleControlHwStatesMsg
//
void GoodDog::HandleControlHwStatusMsg ()
{
   // get control status data
   const CHwStates controlStatusData = _controlHwStatusMsg.getData();

   _pumpDeltaRevs.inletRevsControl    += controlStatusData.inletPump.Revs;
   _pumpDeltaRevs.acRevsControl       += controlStatusData.acPump.Revs;
   _pumpDeltaRevs.plateletRevsControl += controlStatusData.plateletPump.Revs;
   _pumpDeltaRevs.plasmaRevsControl   += controlStatusData.plasmaPump.Revs;
   _pumpDeltaRevs.returnRevsControl   += controlStatusData.returnPump.Revs;

   // ask for safety messages
   RequestSafetyDriverStatusMsg safetyDriverStatusMsg(MessageBase::SEND_GLOBAL);
   safetyDriverStatusMsg.send(1);
}



//
// HandleControlHwStatesMsg
//
void GoodDog::HandleSafetyHwStatusMsg ()
{
   // get safety status data
   const SHwStates safetyStatusData = _safetyHwStatusMsg.getData();

   _pumpDeltaRevs.inletRevsSafety    += safetyStatusData.inletRevs;
   _pumpDeltaRevs.acRevsSafety       += safetyStatusData.acRevs;
   _pumpDeltaRevs.plateletRevsSafety += safetyStatusData.plateletRevs;
   _pumpDeltaRevs.plasmaRevsSafety   += safetyStatusData.plasmaRevs;
   _pumpDeltaRevs.returnRevsSafety   += safetyStatusData.returnRevs;
}



//
// HandleOrdersTimerMsg
// This function is called from multiple places.  It can happen that both control and safety orders
// trigger it (sending a message) and the timer going off in a short interval.  Metering the data seems
// reasonable.
//
void GoodDog::HandleOrdersTimerMsg ()
{
   if ( serviceModeActive )
   {
      // Monitor the lag time for sending messages and report gross latencies
      const int   deltaMs    = osTime::howLongMilliSecAndUpdate(_lastOrdersSendTime);

      Dispatcher& dispatcher = MessageSystem::MsgSystem()->dispatcher();

      if (deltaMs > LATENCY_TIMEOUT)
      {
         DataLog(log_level_proc_info) << "WARNING: GoodDog message delay of " << deltaMs
                                      << " ms.  " << dispatcher.messageRemaining() << " messages remaining." << endmsg;
      }

      // Don't hammer system with back to back messages
      if (deltaMs > MINIMUM_SEND_TIME)
      {
         _safetyOrders.sequenceNumber  = ++_safetyOrdersSequenceNumber;
         _controlOrders.sequenceNumber = ++_controlOrdersSequenceNumber;

         _outgoingControlOrdersMsg.send(_controlOrders);
         _outgoingSafetyOrdersMsg.send(_safetyOrders);
      }
   }
   else
   {
      DataLog(log_level_service_info) << "Good Dog sending safety service mode message." << endmsg;
      _safetyServiceModeMsg.send(0);
   }
}



//
// HandleSafetyOrdersMsg
//
void GoodDog::HandleSafetyOrdersMsg ()
{
   setSafetyOrders(_safetyOrdersFromTed.getData());

   // Make sure we don't repeat this. Safety can't order itself into service mode
   // That request has to come from control (EMS via TED)
   _safetyOrders.serviceEnableMode = HW_DISABLE;

   HandleOrdersTimerMsg();
}


//
// HandleControlOrdersMsg
//
void GoodDog::HandleControlOrdersMsg ()
{
   setControlOrders(_controlOrdersFromTed.getData());
   HandleOrdersTimerMsg();
}



//
// HandleStopGoodDogMsg
//
void GoodDog::HandleStopGoodDogMsg ()
{
   // Deregister the Status Message Handlers
   _controlHwStatusMsg.deregisterMsg();
   _safetyHwStatusMsg.deregisterMsg();

   // Deregister the TED Orders Message Handler
   _controlOrdersFromTed.deregisterMsg();
   _safetyOrdersFromTed.deregisterMsg();

   // Deregister the Stop GoodDog Message Handler
   _stopGoodDogMsg.deregisterMsg();

   // Deregister the Orders Timer
   _ordersTimer.interval(0);
}

// Diff macros - If something changed, log it.
#define CONTROL_HW_ORDERS_DIFF(_ORDERS_NAME_)                                               \
   {                                                                                           \
      if (_controlOrders._ORDERS_NAME_ != newControlHwOrders._ORDERS_NAME_)                   \
      {                                                                                       \
         if (!changeCount) DataLog (log_level_service_info) << "Control order change: ";     \
                                                                                            \
         DataLog (log_level_service_info) << "[" << #_ORDERS_NAME_                           \
                                          << ":" << _controlOrders._ORDERS_NAME_ << "->"                                  \
                                          << newControlHwOrders._ORDERS_NAME_ << "] " << endmsg;                          \
                                                                                            \
         changeCount++;                                                                      \
      }                                                                                       \
   }

#define SAFETY_HW_ORDERS_DIFF(_ORDERS_NAME_)                                                \
   {                                                                                           \
      if (_safetyOrders._ORDERS_NAME_ != newSafetyHwOrders._ORDERS_NAME_)                     \
      {                                                                                       \
         if (!changeCount) DataLog (log_level_service_info) << "Safety order change: ";      \
                                                                                            \
         DataLog (log_level_service_info) << "[" << #_ORDERS_NAME_                           \
                                          << ":" << _safetyOrders._ORDERS_NAME_ << "->"                                   \
                                          << newSafetyHwOrders._ORDERS_NAME_ << "] " << endmsg;                           \
                                                                                            \
         changeCount++;                                                                      \
      }                                                                                       \
   }


void GoodDog::setControlOrders (const CHwOrders& newControlHwOrders)
{
   int changeCount = 0;

   CONTROL_HW_ORDERS_DIFF(cassettePosition);
   CONTROL_HW_ORDERS_DIFF(centrifugeRPM);
   CONTROL_HW_ORDERS_DIFF(centrifugeRampUpRate);
   CONTROL_HW_ORDERS_DIFF(centrifugeRampDownRate);
   CONTROL_HW_ORDERS_DIFF(alarmLight);
   CONTROL_HW_ORDERS_DIFF(doorLatches);
   CONTROL_HW_ORDERS_DIFF(doorDirection);
   CONTROL_HW_ORDERS_DIFF(inletRPM);
   CONTROL_HW_ORDERS_DIFF(plateletRPM);
   CONTROL_HW_ORDERS_DIFF(plasmaRPM);
   CONTROL_HW_ORDERS_DIFF(acRPM);
   CONTROL_HW_ORDERS_DIFF(returnRPM);
   CONTROL_HW_ORDERS_DIFF(rbcValve);
   CONTROL_HW_ORDERS_DIFF(plasmaValve);
   CONTROL_HW_ORDERS_DIFF(plateletValve);
   CONTROL_HW_ORDERS_DIFF(redDrive);
   CONTROL_HW_ORDERS_DIFF(greenDrive);
   CONTROL_HW_ORDERS_DIFF(soundLevel);
   CONTROL_HW_ORDERS_DIFF(openLoop);
   CONTROL_HW_ORDERS_DIFF(highAPSLimit);
   CONTROL_HW_ORDERS_DIFF(lowAPSLimit);
   CONTROL_HW_ORDERS_DIFF(powerSupplyEnabled);

   if (changeCount)
   {
      DataLog (log_level_service_info) << endmsg;
      _controlOrders = newControlHwOrders;
   }
}

void GoodDog::setSafetyOrders (const SHwOrders& newSafetyHwOrders)
{
   int changeCount = 0;

   SAFETY_HW_ORDERS_DIFF(pumpPower);
   SAFETY_HW_ORDERS_DIFF(centrifugePower);
   SAFETY_HW_ORDERS_DIFF(valveLED);
   SAFETY_HW_ORDERS_DIFF(doorSolenoidPower);
   SAFETY_HW_ORDERS_DIFF(alarmLight);
   SAFETY_HW_ORDERS_DIFF(serviceEnableMode);
   SAFETY_HW_ORDERS_DIFF(donorConnectMode);
   SAFETY_HW_ORDERS_DIFF(test1);

   if (changeCount > 0)
   {
      DataLog (log_level_service_info) << endmsg;
      _safetyOrders = newSafetyHwOrders;
   }
}

// Undefine the diff macros.
#undef CONTROL_HW_ORDERS_DIFF
#undef SAFETY_HW_ORDERS_DIFF

/* FORMAT HASH f3f34e454a6b214ba68bb08d80fbee3d */
