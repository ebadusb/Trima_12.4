/*******************************************************************
*    Copyright(c) 1995 - 1998 COBE BCT, Inc. All rights reserved
*
*    AUTHOR:     Barry Fulkerson
*
*    FILE NAME:  meter.cpp
*
*    DATE:       1/22/1998
*
*    DESCRIPTION: see meter.hpp
*
*******************************************************************/


// Includes
#include <vxWorks.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <usrLib.h>

// public data

#include "meter.hpp"

#include "messagesystem.h"
#include "units.h"
#include "chw.hpp"
#include "datfile.h"
#include "filenames.h"
#include "trima_datalog.h"
#include "../os/boot_mode.h"

#include "hour_meter_dat.h"
#include "trima_tasks.h"

//
//
//   Note that the meter.dat file stores the times in minutes
//   so we can't really update any faster than that.
const int updateRate  = MILLISEC_PER_SEC * SEC_PER_MIN;
const int minSpinRate = 100;


//
// Constructor
//
hourMeter::hourMeter()
   : TimerMessage(updateRate, Callback<hourMeter>(this, &hourMeter::updateMeterFile), DISARMED),
     _halStatusCds(0)
{
   _trima_on_time         = HourMeterDat::totalMachineTime();
   _centrifuge_on_time    = HourMeterDat::totalCentrifugeOnTime();
   _no_of_successful_runs = HourMeterDat::totalNumberOfRuns();

   logMeter();
}



//
// Destructor
//
hourMeter::~hourMeter()
{
   if (_halStatusCds)
   {
      delete _halStatusCds;
   }
}



void hourMeter::registerForMessages ()
{
   DataLog(log_level_meter_info) << "Registering for messages" << endmsg;

   //
   // Start listening for message notify requests
   //
   _acConnectedMsg.init(Callback<hourMeter>(this, &hourMeter::handleAcConnectedMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

   // If we are in service mode, we need to listen to the HwStates message,
   // otherwise, we can get the data from the HAL CDS.
   // By not listening to the message in normal mode, the meter task can run at the
   // lower priority that it deserves.
   if (bootServiceModeEnabled())
   {
      _stsConnected.init(Callback<hourMeter>(this, &hourMeter::handleSTSConnectedMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
      _controlDataMsg.init(Callback<hourMeter>(this, &hourMeter::handleHardwareStatesMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

      // Match the priority of the Control Message task as we now have to respond to 10 ms data
      TrimaTaskInfo info;
      trimaGetTaskInfo(TTID_CtlMsg, &info);

      taskPrioritySet(info.tid, info.priority);
   }
   else
   {
      DataLog(log_level_meter_info) << "Creating Hal Status CDS instance" << endmsg;

      _halStatusCds = new HalStatus_CDS(ROLE_RO);

      DataLog(log_level_meter_info) << "Created Hal Status CDS instance" << endmsg;
   }

   // Initialize messages.
   memset (&_controlData, 0, sizeof(_controlData));

   // Start Meter Timer
   armTimer(TimerMessage::ARMED);

   DataLog(log_level_meter_info) << "messages registered" << endmsg;
}



//
// UpdateMeterFile
//
void hourMeter::updateMeterFile ()
{
   bool centrifugeSpinning;

   // if in service mode, get data from message, else use CDS
   if (bootServiceModeEnabled())
   {
      centrifugeSpinning = (_controlData.centrifugeRPM > minSpinRate);
   }
   else
   {
      centrifugeSpinning = (_halStatusCds->CentrifugeRPM() > minSpinRate);
   }

   _trima_on_time++;

   if (centrifugeSpinning)
   {
      _centrifuge_on_time++;
   }

   HourMeterDat::update(_trima_on_time, _centrifuge_on_time, _no_of_successful_runs);
}



//
// handleHardwareStatesMsg
//
void hourMeter::handleHardwareStatesMsg ()
{
   _controlData = _controlDataMsg.getData();
}



//
// handleAcConnectedMsg
//
void hourMeter::handleAcConnectedMsg ()
{
   HourMeterDat::update(_trima_on_time, _centrifuge_on_time, ++_no_of_successful_runs);
}



//
// handleSTSConnectedMsg
//
void hourMeter::handleSTSConnectedMsg ()
{
   static bool doneIt = false;

   // Don't allow multiple registrations
   if (!doneIt)
   {
      _controlDataMsg.init(Callback<hourMeter>(this, &hourMeter::handleHardwareStatesMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
      DataLog(log_level_meter_info) << "STS Connected.  Registering for HW States Msg" << endmsg;
      doneIt = true;
   }
}



//
// logMeter
//
void hourMeter::logMeter ()
{
   char s_data[80];

   sprintf(s_data, "Current Machine Hour Meter = %.2f", (float)_trima_on_time / (float)MINUTES_PER_HOUR);
   DataLog(log_level_meter_info) << s_data << endmsg;

   sprintf(s_data, "Current Centrifuge Hour Meter = %.2f", (float)_centrifuge_on_time / (float)MINUTES_PER_HOUR);
   DataLog(log_level_meter_info) << s_data << endmsg;

   sprintf(s_data, "Current Number of Runs Meter = %ld", _no_of_successful_runs);
   DataLog(log_level_meter_info) << s_data << endmsg;
}



//
// meter executable
//
extern "C" void meter ()
{
   DataLog(log_level_meter_info) << "Meter process started" << endmsg;

   MessageSystem ms;
   ms.initBlocking("Meter");

   hourMeter meter;
   meter.registerForMessages();

   TASK_DONE;

   ms.dispatchMessages();

   DataLog(log_level_meter_info) << "Meter process terminating" << endmsg;
}

/* FORMAT HASH 038caed37de3b0438af620f805ef39de */
