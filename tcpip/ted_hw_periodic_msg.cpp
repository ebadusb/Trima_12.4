/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      tedif.h
 *             Trima External Diagnostic Interface
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface between Trima and EMS.
 *
 * HISTORY:    $Log: ted_hw_periodic_msg.cpp $
 * HISTORY:    Revision 1.11  2006/11/08 21:45:27Z  rm70006
 * HISTORY:    IT 7232.  Added new status bits to Artesyn power supply
 * HISTORY:    Revision 1.10  2003/11/18 15:14:59Z  rm70006
 * HISTORY:    IT 5830.  forgot to clear register message flag.
 * HISTORY:    Revision 1.9  2003/11/18 13:05:45Z  rm70006
 * HISTORY:    IT 5830.  Clear socket variables on disconnect.
 * HISTORY:    Revision 1.8  2003/11/05 16:37:25Z  rm70006
 * HISTORY:    IT 5830.  Fix bug with end connection message.
 * HISTORY:    Revision 1.7  2003/10/17 16:52:02Z  rm70006
 * HISTORY:    IT 6522.  Make some changes so TED doesn't reboot trima.
 * HISTORY:    Revision 1.6  2003/10/14 22:07:52Z  rm70006
 * HISTORY:    IT 6492.  Add APS message.
 * HISTORY:    Revision 1.5  2003/10/08 18:31:47Z  rm70006
 * HISTORY:    IT 5830.  Put more fixes in TED.
 * HISTORY:    Revision 1.4  2003/07/01 15:59:09Z  rm70006
 * HISTORY:    IT 5830.  Interim checkin.  Got periodic status working.
 * HISTORY:    Revision 1.3  2003/06/16 21:58:24Z  rm70006
 * HISTORY:    IT 5830.
 * HISTORY:    Add more functionality to TED.
 * HISTORY:    Revision 1.2  2003/05/16 13:35:03Z  rm70006
 * HISTORY:    IT 5830.
 * HISTORY:
 * HISTORY:    Add more functionality.
 * HISTORY:    Revision 1.1  2002/12/20 21:48:44Z  rm70006
 * HISTORY:    Initial revision
 *
 *******************************************************************************/


#include "ted_hw_periodic_msg.h"



//
// TedHwPeriodicStatus constructor
//
TedHwPeriodicStatus::TedHwPeriodicStatus(TED_BoardType board)
   : _period(0),
     _board(board),
     _noControlData(true),
     _noSafetyData(true),
     _registeredForMessages(false),
     _controlStatusMsg(0),
     _safetyStatusMsg(0)
{
   if (board == CONTROL)
   {
      // Init the timer message with the class callback function.
      _tm.init(0, Callback<TedHwPeriodicStatus>(this, &TedHwPeriodicStatus::sendControlPeriodicStatus), TimerMessage::DISARMED);

      _controlStatusMsg = new TedMessageObj<TED_ControlHwStatusMsg> (_controlStatus, _client);
   }
   else
   {
      // Init the timer message with the class callback function.
      _tm.init(0, Callback<TedHwPeriodicStatus>(this, &TedHwPeriodicStatus::sendSafetyPeriodicStatus), TimerMessage::DISARMED);

      _safetyStatusMsg = new TedMessageObj<TED_SafetyHwStatusMsg> (_safetyStatus, _client);
   }

   memset(&_controlStatus, 0, sizeof(_controlStatus));
   memset(&_safetyStatus,  0, sizeof(_safetyStatus));
}



//
// TedHwPeriodic Status destructor
//
TedHwPeriodicStatus::~TedHwPeriodicStatus()
{
   // Turn off the messages
   deactivate();

   delete _controlStatusMsg;
   delete _safetyStatusMsg;
}



//
// Set
//
void TedHwPeriodicStatus::set (const sockinetaddr& addr, unsigned int period)
{
   DataLog(log_level_ted_info) << "Setting period for board " << _board << " to " << period
                               << " ms.  Client is " << addr << "." << endmsg;

   // Stop timer if period is 0
   if (period == 0)
   {
      // Turn off hw status messages
      deactivate();
   }
   else
   {
      // Set the client address
      _client = addr;

      // Set the address for the datagram message
      if (_board == CONTROL)
      {
         if (!_registeredForMessages)
         {
            // Register for the Control Status message
            _controlHwStatesMsg.init(Callback<TedHwPeriodicStatus>(this, &TedHwPeriodicStatus::handleControlHwStatusMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

            _registeredForMessages = true;
         }

         _controlStatusMsg->setAddr(_client);
      }
      else
      {
         if (!_registeredForMessages)
         {
            // Register for the Safety Status message
            _safetyHwStatesMsg.init(Callback<TedHwPeriodicStatus>(this, &TedHwPeriodicStatus::handleSafetyHwStatusMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

            _registeredForMessages = true;
         }

         _safetyStatusMsg->setAddr(_client);
      }

      // Set the interval for the timer message and arm the timer.
      _tm.interval(period);
   }
}



//
// Deactivate
//
void TedHwPeriodicStatus::deactivate ()
{
   // Turn off the timer
   _tm.interval(0);

   // Zero out the old client address
   _client.setaddr(0, sockinetaddr::host);
   _client.setport(0);

   // De-register for the hw status message(s)
   if (_board == CONTROL)
   {
      _controlHwStatesMsg.deregisterMsg();
   }
   else
   {
      _safetyHwStatesMsg.deregisterMsg();
   }

   // Reset registered for message flag
   _registeredForMessages = false;

   DataLog(log_level_ted_info) << "Deactivating " << _board << " messages." << endmsg;
}



//
// Callback for Control timer message.
//
void TedHwPeriodicStatus::sendControlPeriodicStatus (void)
{
   if (_noControlData)
   {
      DataLog(log_level_ted_info) << "No Control Data exists" << endmsg;
      return;
   }

   // Fill in message
   _controlStatus.accessPressure = _controlHwStates.accessPressure;

   // Power
   _controlStatus.emiTemperature         = _controlHwStates.emiTemperature;
   _controlStatus.centrifugeCurrent      = _controlHwStates.centrifugeCurrent;
   _controlStatus.centrifugePressure     = _controlHwStates.centrifugePressure;
   _controlStatus.fiveVoltSupply         = _controlHwStates.fiveVoltSupply;
   _controlStatus.leakDetector           = _controlHwStates.leakDetector;
   _controlStatus.minusTwelvePS          = _controlHwStates.minusTwelvePS;
   _controlStatus.sixtyFourVoltCurrent   = _controlHwStates.sixtyFourVoltCurrent;
   _controlStatus.sixtyFourVoltSupply    = _controlHwStates.sixtyFourVoltSupply;
   _controlStatus.sixtyFourVoltSwitched  = _controlHwStates.sixtyFourVoltSwitched;
   _controlStatus.twelveVoltSupply       = _controlHwStates.twelveVoltSupply;
   _controlStatus.twentyFourVoltCurrent  = _controlHwStates.twentyFourVoltCurrent;
   _controlStatus.twentyFourVoltSupply   = _controlHwStates.twentyFourVoltSupply;
   _controlStatus.twentyFourVoltSwitched = _controlHwStates.twentyFourVoltSwitched;

   // Cassette
   _controlStatus.cassettePosition = _controlHwStates.cassettePosition;
   _controlStatus.cassetteError    = _controlHwStates.cassetteError;

   // Centrifuge
   _controlStatus.centrifugeRPM   = _controlHwStates.centrifugeRPM;
   _controlStatus.centrifugeError = _controlHwStates.centrifugeError;

   // Misc
   _controlStatus.doorLocks        = _controlHwStates.doorLocks;
   _controlStatus.pauseSwitch      = _controlHwStates.pauseSwitch;
   _controlStatus.stopSwitch       = _controlHwStates.stopSwitch;
   _controlStatus.pauseSwitchLatch = _controlHwStates.pauseSwitch;
   _controlStatus.stopSwitchLatch  = _controlHwStates.stopSwitchLatch;
   _controlStatus.vibration        = _controlHwStates.vibration;
   _controlStatus.valveFault       = _controlHwStates.valveFault;
   _controlStatus.motorFault       = _controlHwStates.motorFault;
   _controlStatus.solenoidFault    = _controlHwStates.solenoidFault;

   // rbc detector
   _controlStatus.red          = _controlHwStates.red;
   _controlStatus.green        = _controlHwStates.green;
   _controlStatus.rbcCommError = _controlHwStates.rbcCommError;

   // Pumps
   _controlStatus.acPump       = _controlHwStates.acPump;
   _controlStatus.inletPump    = _controlHwStates.inletPump;
   _controlStatus.plasmaPump   = _controlHwStates.plasmaPump;
   _controlStatus.plateletPump = _controlHwStates.plateletPump;
   _controlStatus.returnPump   = _controlHwStates.returnPump;

   // Ultrasonics
   _controlStatus.acDetector    = _controlHwStates.acDetector;
   _controlStatus.reservoir     = _controlHwStates.reservoir;
   _controlStatus.plasmaValve   = _controlHwStates.plasmaValve;
   _controlStatus.plateletValve = _controlHwStates.plateletValve;
   _controlStatus.rbcValve      = _controlHwStates.rbcValve;

   // Fan Status
   _controlStatus.fanSense0 = _controlHwStates.fanSense0;
   _controlStatus.fanSense1 = _controlHwStates.fanSense1;
   _controlStatus.fanSense2 = _controlHwStates.fanSense2;
   _controlStatus.tempError = _controlHwStates.tempError;

   // Power Supply
   _controlStatus.psTempWarning = _controlHwStates.psTempWarning;

   // Sound Level
   _controlStatus.soundLevel = _controlHwStates.soundLevel;

   // Event
   _controlStatus.event             = _controlHwStates.event;

   _controlStatus.msgCount          = _controlHwStates.msgCount;
   _controlStatus.lowAGC            = _controlHwStates.lowAGC;
   _controlStatus.highAGC           = _controlHwStates.highAGC;

   _controlStatus.sealSafeOperating = _controlHwStates.sealSafeOperating;
   _controlStatus.sealSafeOvertemp  = _controlHwStates.sealSafeOvertemp;
   _controlStatus.ovpTestResult     = _controlHwStates.ovpTestResult;

   _controlStatusMsg->Send(TED_CONTROL_HARDWARE_STATUS);

   SetCtlStatusSentTimestamp timeStampMsg(MessageBase::SEND_GLOBAL);
   timeStampMsg.send();
}



//
// Callback for Safety timer message.
//
void TedHwPeriodicStatus::sendSafetyPeriodicStatus (void)
{
   if (_noSafetyData)
   {
      DataLog(log_level_ted_info) << "No Safety Data exists" << endmsg;
      return;
   }

   // Fill in message
   _safetyStatus.inletRPM             = _safetyHwStates.inletRPM;
   _safetyStatus.inletRevs            = _safetyHwStates.inletRevs;
   _safetyStatus.inletAccumRevs       = _safetyHwStates.inletAccumRevs;
   _safetyStatus.plateletRPM          = _safetyHwStates.plateletRPM;
   _safetyStatus.plateletRevs         = _safetyHwStates.plateletRevs;
   _safetyStatus.plateletAccumRevs    = _safetyHwStates.plateletAccumRevs;
   _safetyStatus.plasmaRPM            = _safetyHwStates.plasmaRPM;
   _safetyStatus.plasmaRevs           = _safetyHwStates.plasmaRevs;
   _safetyStatus.plasmaAccumRevs      = _safetyHwStates.plasmaAccumRevs;
   _safetyStatus.acRPM                = _safetyHwStates.acRPM;
   _safetyStatus.acRevs               = _safetyHwStates.acRevs;
   _safetyStatus.acAccumRevs          = _safetyHwStates.acAccumRevs;
   _safetyStatus.returnRPM            = _safetyHwStates.returnRPM;
   _safetyStatus.returnRevs           = _safetyHwStates.returnRevs;
   _safetyStatus.returnAccumRevs      = _safetyHwStates.returnAccumRevs;
   _safetyStatus.returnDirection      = _safetyHwStates.returnDirection;
   _safetyStatus.rbcValve             = _safetyHwStates.rbcValve;
   _safetyStatus.plasmaValve          = _safetyHwStates.plasmaValve;
   _safetyStatus.plateletValve        = _safetyHwStates.plateletValve;
   _safetyStatus.valveLedTest         = _safetyHwStates.valveLedTest;
   _safetyStatus.cassettePosition     = _safetyHwStates.cassettePosition;
   _safetyStatus.reservoir            = _safetyHwStates.reservoir;
   _safetyStatus.centrifugeRPM        = _safetyHwStates.centrifugeRPM;
   _safetyStatus.centrifugeError      = _safetyHwStates.centrifugeError;
   _safetyStatus.doorLocks            = _safetyHwStates.doorLocks;
   _safetyStatus.pauseSwitch          = _safetyHwStates.pauseSwitch;
   _safetyStatus.stopSwitch           = _safetyHwStates.stopSwitch;
   _safetyStatus.event                = _safetyHwStates.event;
   _safetyStatus.msgCount             = _safetyHwStates.msgCount;
   _safetyStatus.returnPumpDirChgTime = _safetyHwStates.returnPumpDirChgTime;
   _safetyStatus.test1                = _safetyHwStates.test1;
   _safetyStatus.test2                = _safetyHwStates.test2;
   _safetyStatus.serviceEnableMode    = _safetyHwStates.serviceEnableMode;
   _safetyStatus.donorConnectMode     = _safetyHwStates.donorConnectMode;

   _safetyStatusMsg->Send(TED_SAFETY_HARDWARE_STATUS);
}



//
// handleControlStatesMsg
//
void TedHwPeriodicStatus::handleControlHwStatusMsg ()
{
   _controlHwStates = _controlHwStatesMsg.getData();

   _noControlData   = false;
}



//
// handleSafetyStatesMsg
//
void TedHwPeriodicStatus::handleSafetyHwStatusMsg ()
{
   _safetyHwStates = _safetyHwStatesMsg.getData();

   _noSafetyData   = false;
}

/* FORMAT HASH 8e979fd4646f2de603db41c18b26196a */
