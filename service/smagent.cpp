/*
 * Copyright (C) 2007 Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      smagent.cpp
 *             Service Mode Agent process
 *
 * AUTHOR:     Shawn Price
 *
 * ABSTRACT:   This file implements the class/task responsible for control-side processing
 *             under Service Mode.  Primary responsibilities for SMAgent are 1) instantiate
 *             and provide the process context for the hardware abstraction layer (HAL), and
 *             2) effect periodic logging.  In both regards, SMAgent acts as a lean
 *             surrogate for Proc.
 */

#include "messagesystem.h"      // Gets Message System
#include "trimamessages.h"      // Gets Trima Int messages

#include "smagent.hpp"
#include "cobeconfig.h"
#include "rbcdat.h"
#include "hwdat.h"
#include "periodic_log.h"   // for periodic logging
#include "software_cds.h"

//
// main routine for SMAgent
//
extern "C" void smagent ()
{
   DataLog (log_level_service_info) << "SMAgent process started" << endmsg;

   MessageSystem ms;

   // Blocking loop that receives messages
   ms.initBlocking("smagent", 100);

   SMAgent* agent = new SMAgent();

   DataLog (log_level_service_info) << "Initializing SMAgent..." << endmsg;

   if (!agent->Initialize())
   {
      DataLog (log_level_service_info) << "SMAgent failed to initialize.  Exiting... " << endmsg;

      delete agent;
      return;
   }

   DataLog(log_level_service_info) << "SMAgent is ready to process messages." << endmsg;

   ms.dispatchMessages();

   DataLog(log_level_service_info) << "SMAgent process exiting..." << endmsg;

   delete agent;
}

//
// Default constructor
//
SMAgent::SMAgent()
   : _HAL(Callback<SMAgent>(this, &SMAgent::HandleHalStatusMsg) ),
     _smControlOrdersMsg(Callback<SMAgent>(this, &SMAgent::HandleControlOrdersMsg), MessageBase::SNDRCV_RECEIVE_ONLY)
{}

//
// Default Destructor
//
SMAgent::~SMAgent()
{}

//
// Initializer
//
bool SMAgent::Initialize ()
{
   bool         status   = true;
   ALARM_VALUES a_status = NULL_ALARM;

   //
   // make periodic logging in service mode sw.dat configurable
   if (Software_CDS::GetInstance().getFeature(ServiceModePeriodicLog))
   {
      DataLog(log_level_service_info) << "periodic logging enabled" << endmsg;

      data.setPeriodicLogging(true);
   }
   else
   {
      DataLog(log_level_service_info) << "periodic logging disabled" << endmsg;

      data.setPeriodicLogging(false);
   }

   //
   // Initialize HAL ...
   DataLog(log_level_service_info) << "Initializing HAL..." << endmsg;

   if ((a_status = _HAL.Init()) != NULL_ALARM)
   {
      DataLog(log_level_service_error) << "HAL Initialization failed" << a_status << endmsg;
      a_status = NULL_ALARM;
      status   = false;
   }

   //
   // Start periodic logging
   //
   PeriodicLog::init(&data, 30.0);

   return status;
}

//
// Description:     Processes the Control hardware
//                  status sent by HAL.  While TED
//                  receives Control status directly
//                  in the form of a CHwStates object,
//                  We must process HAL status data
//                  in order to effect periodic logging.
//
// Error Handling:  None
//
void SMAgent::HandleHalStatusMsg ()
{
   PeriodicLog::forceOutput();
}

//
// Handle Service Mode Control hardware messages
//
void SMAgent::HandleControlOrdersMsg ()
{
   // save a test copy of this message for Control hardware keepalive
   CHwOrders orders = _smControlOrdersMsg.getData();

   // _HAL.SendOrders( orders, data.Orders().Data() );
   _HAL.MakeOrders(orders, data.Orders().Data());
}

/* FORMAT HASH 33cfd36e0559b38f4e7783a725f5f39c */
