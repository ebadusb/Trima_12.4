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
 *
 *******************************************************************************/


#ifndef __TED_MESSAGE__
#define __TED_MESSAGE__

#include <ioLib.h>
#include <algorithm>
#include <string>
#include <cctype>

#include "tedif.h"
#include "datagram_message_obj.h"

#include "sockstream.h"
#include "sockinet.h"

#include "trimamessages.h"
#include "tcorders.hpp"
#include "filenames.h"
#include "common_hw.h"
#include "autocaldat.h"
#include "discover_hw.h"

#include "software_cds.h"



template <class orderType> bool rangeCheck (orderType order, orderType min, orderType max, const char* orderName)
{
   if (order < min || order > max)
   {
      DataLog (log_level_ted_error) << orderName << " " << order << " out of range." << endmsg;
      return false;
   }
   else return true;
}


template <class MSGTYPE> class TedMessageObj
   : public DatagramMessageObject<MSGTYPE, TED_MessageId>
{
   // Prepare message for sending (must be defined by encapsulating protocol)
public:
   TedMessageObj(MSGTYPE& obj, const sockinetaddr& addr)
      : DatagramMessageObject<MSGTYPE, TED_MessageId>(obj, addr, log_level_ted_debug, log_level_ted_error) {}

   virtual ~TedMessageObj() {}

   void prepareMsg (TED_MessageId messageId)
   {
      tedIf.prepareMsg((char*)&_obj, messageId, MSG_SIZE);
      _prepared = true;
   }


/*
   virtual void Send(TED_MessageId messageId)
   {
      if (messageId == TED_CONTROL_HARDWARE_STATUS)
      {
         TED_ControlHwStatusMsg *msg = (TED_ControlHwStatusMsg *)&_obj;

         DataLog(log_level_ted_info) << "RKM: Control Status: " << *msg << endmsg;
      }

      DatagramMessageObject::Send(messageId);
   }
*/


private:
   TedIf tedIf;
};



//
// TedOneToOneStatusMsg
// This helper class encapsulates the necessary steps to receive an internal Trima message,
// format it, and send out a datagram message to an external client using UDP.
//
template <class CLASS, class TRIMA_IN_MSG, class TED_OUT_MSG> class TedOneToOneStatusMsg
{
public:
   typedef bool (CLASS::* FillFuncType)(const TRIMA_IN_MSG&, TED_OUT_MSG&);

// Methods
public:
   TedOneToOneStatusMsg(const sockinetaddr& addr, TED_MessageId msgId, CLASS* me, FillFuncType fn)
      : _trimaMsgObj (Callback<TedOneToOneStatusMsg>(this, &TedOneToOneStatusMsg::receive), MessageBase::SNDRCV_RECEIVE_ONLY),
        _tedMsgObj (_tedOutMsg, addr),
        _fn(fn),
        _msgId(msgId),
        _class(me)
   {}

   ~TedOneToOneStatusMsg() {}

   void deregisterMsg (void) { _trimaMsgObj.deregisterMsg(); }

   void setAddr (const sockinetaddr& addr) { _tedMsgObj.setAddr(addr); }

// Methods
private:
   void receive ()
   {
      bool send;

      // Receive the message
      TRIMA_IN_MSG trimaMsgIn = _trimaMsgObj.getData();

      // fill in the message
      send = (_class->*_fn)(trimaMsgIn, _tedOutMsg);

      if (send)
      {
         // Send the message
         _tedMsgObj.Send(_msgId);
      }
   }

// Data Members
private:
   const TED_MessageId        _msgId;
   const FillFuncType         _fn;
   Message<TRIMA_IN_MSG>      _trimaMsgObj;
   TedMessageObj<TED_OUT_MSG> _tedMsgObj;
   TED_OUT_MSG                _tedOutMsg;
   CLASS*                     _class;
};




//
// TedMessage
// This class is the skeleton for processing a TED message.  This class provide the routines
// that validate and process the message as well as send a reply status if needed.
//
template <class msgType> class TedMessage
   : public TedIf
{
public:
   TedMessage(const sockinetaddr& addr);
   TedMessage(const sockinetaddr& addr, const void* msg, int size);
   virtual ~TedMessage();

   virtual bool validBody (void) { return true; }    // Define for each message type.  Default for empty bodies.

   virtual bool process (void) { return false; }         // Define for each message type.

   virtual void SendStatus (TED_Status status);

   inline operator const msgType* const  () const { return _msg; }

private:
   TedMessage() {}

protected:
   sockinetaddr   _addr;

   const void*    _msgbuffer;
   int            _msgSize;

   const msgType* _msg;
};



//
// Constructor
//
template <class msgType> TedMessage<msgType>::TedMessage(const sockinetaddr& addr)
   : _msg(0),
     _msgbuffer(0),
     _msgSize(0),
     _addr(addr)
{}



//
// Message constructor
//
template <class msgType> TedMessage<msgType>::TedMessage(const sockinetaddr& addr, const void* msg, int size)
   : _msg((const msgType*)msg),
     _msgbuffer(msg),
     _msgSize(size),
     _addr(addr)
{}



//
// Destructor
//
template <class msgType>TedMessage<msgType>::~TedMessage()
{}



//
// SendStatus
// This function sends a TED_Reply message back to the connected client with the passed in status.
//
template <class msgType> void TedMessage<msgType >::SendStatus (TED_Status status)
{
   TED_ReplyMsg                reply;

   TedMessageObj<TED_ReplyMsg> tedMsg(reply, _addr);

   reply.status = status;
   tedMsg.Send(TED_REPLY);
}



#include "boot_mode.h"    // Get Trima mode calls
//
// TED_ConnectResponseMsg
// This method processes the TED_CONNECT_REQUEST message.
//
template <> bool TedMessage<TED_ConnectRequestMsg >::process (void)
{
   TED_Status status;

   DataLog(log_level_ted_info) << "Got TED Connect Request Msg" << endmsg;

   // TED_ConnectRequestMsg has no body so this is a null call.
   // Left for completeness.
   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
   }
   else if (bootNormalModeEnabled())
   {
      status = TED_CONNECTION_ALLOWED_OPERATIONAL;
   }
   else if (bootServiceModeEnabled())
   {
      status = TED_CONNECTION_ALLOWED_SERVICE;

      STSConnectedMsg stsConnected(MessageBase::SEND_LOCAL);
      stsConnected.send(1);
   }
   else if (bootSingleStepEnabled())
   {
      status = TED_CONNECTION_ALLOWED_SINGLE_STEP;
   }
   else
   {
      status = TED_CONNECTION_DENIED;
   }

   DataLog(log_level_ted_info) << "TED Connect Request Msg returned status " << status
                               << " to client " << _addr << endmsg;

   SendStatus(status);

   if (status == TED_BAD_MESSAGE_RECEIVED)
      return false;
   else
      return true;
}



//
// TED_HardwarePeriodicStatusRequestMsg
// This method validates the body of the message.
//
template <> bool TedMessage<TED_HwPeriodicStatusRequestMsg >::validBody (void)
{
   if (_msgSize != TED_HwPeriodicStatusRequestMsgSize)
   {
      DataLog(log_level_ted_error) << "HwPeriodicStatusRequestMsg size invalid.  Got " << _msgSize
                                   << ".  Expected " << TED_HwPeriodicStatusRequestMsgSize << endmsg;

      return false;
   }

   if ( (_msg->board != CONTROL) && (_msg->board != SAFETY) )
   {
      DataLog(log_level_ted_error) << "board value invalid.  Got " << _msg->board
                                   << ".  Expected (" << CONTROL << ", " << SAFETY << ")." << endmsg;

      return false;
   }

   return true;
}



//
// TED_HwPeriodicStatusRequestMsg
// This method processes the TED_HW_PERIODIC_STATUS_REQUEST message.
//
template <> bool TedMessage<TED_HwPeriodicStatusRequestMsg >::process (void)
{
   DataLog(log_level_ted_debug) << "Got TED Hw Periodic Status Request Msg" << endmsg;

   if (!validBody())
   {
      SendStatus(TED_BAD_MESSAGE_RECEIVED);

      DataLog(log_level_ted_debug) << "TED Hw Periodic Status Request Msg returned status " << TED_BAD_MESSAGE_RECEIVED << endmsg;

      return false;
   }
   else
   {
      SendStatus(TED_REQUEST_ALLOWED);

      // Application has to process this one.
      return true;
   }
}



//
// TED_ControlHwOrderMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_ControlHwOrderMsg >::validBody (void)
{
   if (_msgSize != TED_ControlHwOrderMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_ControlHwOrderMsg size invalid.  Got " << _msgSize
                                   << ".  Expected " << TED_ControlHwOrderMsgSize << endmsg;

      return false;
   }

   // Commented ones don't have a range that I'm aware of.
   if (!rangeCheck<CHW_CASSETTE_ORDERS>(_msg->cassettePosition, CHW_CASSETTE_FIRST_ORDER, CHW_CASSETTE_LAST_ORDER, "Cassette position") ||
       !rangeCheck<float>(_msg->centrifugeRPM, 0.0, 3100.0, "Centrifuge RPM") ||
       // !rangeCheck<float>(_msg->centrifugeRampUpRate, xxx, xxx, "Centrifuge ramp up rate") ||
       // !rangeCheck<float>(_msg->centrifugeRampDownRate, xxx, xxx, "Centrifuge ramp down rate") ||
       !rangeCheck<HW_ORDERS>(_msg->alarmLight, HW_ORDERS_FIRST, HW_ORDERS_LAST, "Alarm Light") ||
       !rangeCheck<HW_ORDERS>(_msg->doorLatches, HW_ORDERS_FIRST, HW_ORDERS_LAST, "Door latches") ||
       !rangeCheck<float>(_msg->inletRPM, 0.0, 250.0, "Inlet pump RPM") ||
       !rangeCheck<float>(_msg->plateletRPM, 0.0, 250.0, "Platelet pump RPM") ||
       !rangeCheck<float>(_msg->plasmaRPM, 0.0, 250.0, "Plasma pump RPM") ||
       !rangeCheck<float>(_msg->acRPM, 0.0, 250.0, "AC pump RPM") ||
       !rangeCheck<float>(_msg->returnRPM, -250, 250.0, "Return pump RPM") ||
       !rangeCheck<CHW_VALVE_ORDERS>(_msg->rbcValve, CHW_VALVE_FIRST, CHW_VALVE_LAST, "RBC valve") ||
       !rangeCheck<CHW_VALVE_ORDERS>(_msg->plasmaValve, CHW_VALVE_FIRST, CHW_VALVE_LAST, "Plasma valve") ||
       !rangeCheck<CHW_VALVE_ORDERS>(_msg->plateletValve, CHW_VALVE_FIRST, CHW_VALVE_LAST, "Platelet valve") ||
       !rangeCheck<unsigned char>(_msg->redDrive, 0, 255, "Red drive") ||
       !rangeCheck<unsigned char>(_msg->greenDrive, 0, 255, "Green drive") ||
       !rangeCheck<CHW_SOUND_LEVELS>(_msg->soundLevel, CHW_SOUND_LEVEL_FIRST, CHW_SOUND_LEVEL_LAST, "Green drive") ||
       !rangeCheck<long>(_msg->openLoop, CHW_SOUND_LEVEL_FIRST, CHW_SOUND_LEVEL_LAST, "Sound level") ||
       // !rangeCheck<short>(_msg->highAPSLimit, xxx, xxx, "APS high limit") ||
       // !rangeCheck<short>(_msg->lowAPSLimit, xxx, xxx, "APS low limit") ||
       !rangeCheck<HW_PS_OUTPUT_STATUS>(_msg->powerSupplyEnabled, HW_PS_OUTPUT_STATUS_FIRST, HW_PS_OUTPUT_STATUS_LAST, "Power supply enabled "))
   {
      DataLog (log_level_ted_error) << "TED Control Orders range check failure" << endmsg;
      return false;
   }

   // No validation required on HW Orders
   return true;
}



//
// TED_ControlHwOrderMsg
// This method processes the message
//
//
template <> bool TedMessage<TED_ControlHwOrderMsg >::process (void)
{
   TED_Status status = TED_REQUEST_ALLOWED;

   DataLog(log_level_ted_debug) << "Got TED Control Hw Order Msg" << endmsg;

   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
   }
   else if (!bootServiceModeEnabled())
   {
      status = TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE;
   }

   SendStatus(status);

   DataLog(log_level_ted_debug) << "TED Control Hw Order Msg returned status " << status << endmsg;

   if (status != TED_REQUEST_ALLOWED)
   {
      return false;
   }

   TedControlOrdersMsg orderMsg(MessageBase::SEND_GLOBAL);

   CHwOrders           controlHwOrder;

   // Copy Control HW Order
   controlHwOrder.cassettePosition       = _msg->cassettePosition;
   controlHwOrder.centrifugeRPM          = _msg->centrifugeRPM;
   controlHwOrder.centrifugeRampUpRate   = _msg->centrifugeRampUpRate;
   controlHwOrder.centrifugeRampDownRate = _msg->centrifugeRampDownRate;
   controlHwOrder.alarmLight             = _msg->alarmLight;
   controlHwOrder.doorLatches            = _msg->doorLatches;
   controlHwOrder.doorDirection          = _msg->doorDirection;
   controlHwOrder.inletRPM               = _msg->inletRPM;
   controlHwOrder.plateletRPM            = _msg->plateletRPM;
   controlHwOrder.plasmaRPM              = _msg->plasmaRPM;
   controlHwOrder.acRPM                  = _msg->acRPM;
   controlHwOrder.returnRPM              = _msg->returnRPM;
   controlHwOrder.rbcValve               = _msg->rbcValve;
   controlHwOrder.plasmaValve            = _msg->plasmaValve;
   controlHwOrder.plateletValve          = _msg->plateletValve;
   controlHwOrder.redDrive               = _msg->redDrive;
   controlHwOrder.greenDrive             = _msg->greenDrive;
   controlHwOrder.soundLevel             = _msg->soundLevel;
   controlHwOrder.openLoop               = _msg->openLoop;
   controlHwOrder.highAPSLimit           = _msg->highAPSLimit;
   controlHwOrder.lowAPSLimit            = _msg->lowAPSLimit;
   controlHwOrder.powerSupplyEnabled     = _msg->powerSupplyEnabled;

   DataLog(log_level_ted_debug) << "TED Cmd: "
                                << "CassettePos:"   << controlHwOrder.cassettePosition          << " "
                                << "Cent:("
                                << "RPM:"           << controlHwOrder.centrifugeRPM             << " "
                                << "RampUpRate:"    << controlHwOrder.centrifugeRampUpRate      << " "
                                << "RampDnRate:"    << controlHwOrder.centrifugeRampDownRate    << ") "
                                << "AlarmLight: "   << controlHwOrder.alarmLight                << " "
                                << "Door:("
                                << "Latch:"         << controlHwOrder.doorLatches               << " "
                                << "Direction:"     << controlHwOrder.doorDirection             << ") "
                                << "Pumps:("
                                << "Inlet:"         << controlHwOrder.inletRPM                  << " "
                                << "Plt:"           << controlHwOrder.plateletRPM               << " "
                                << "Plas:"          << controlHwOrder.plasmaRPM                 << " "
                                << "AC:"            << controlHwOrder.acRPM                     << " "
                                << "Return:"        << controlHwOrder.returnRPM                 << ") "
                                << "Valves:("
                                << "Rbc:"           << controlHwOrder.rbcValve                  << " "
                                << "Plas:"          << controlHwOrder.plasmaValve               << " "
                                << "Plt:"           << controlHwOrder.plateletValve             << ") "
                                << "RGDrives:("
                                << "R:"             << controlHwOrder.redDrive                  << " "
                                << "G:"             << controlHwOrder.greenDrive                << ") "
                                << "Sound:"         << controlHwOrder.soundLevel                << " "
                                << "OpenLoop:"      << controlHwOrder.openLoop                  << " "
                                << "APSLims:("
                                << "hi:"            << controlHwOrder.highAPSLimit              << " "
                                << "lo:"            << controlHwOrder.lowAPSLimit               << ") "
                                << "PsEnab:"        << controlHwOrder.powerSupplyEnabled        << endmsg;

   // Send message
   orderMsg.send(controlHwOrder);

   return true;
}



//
// TED_SafetyHwOrderMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_SafetyHwOrderMsg >::validBody (void)
{
   if (_msgSize != TED_SafetyHwOrderMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_SafetyHwOrderMsg size invalid.  Got " << _msgSize
                                   << ".  Expected " << TED_SafetyHwOrderMsgSize << endmsg;

      return false;
   }

   // Commented ones don't have a range that I'm aware of.
   if (!rangeCheck<HW_ORDERS>(_msg->pumpPower,          HW_ORDERS_FIRST, HW_ORDERS_LAST, "Pump power") ||
       !rangeCheck<HW_ORDERS>(_msg->centrifugePower,    HW_ORDERS_FIRST, HW_ORDERS_LAST, "Centrifuge power") ||
       !rangeCheck<HW_ORDERS>(_msg->valveLED,           HW_ORDERS_FIRST, HW_ORDERS_LAST, "Valve LED") ||
       !rangeCheck<HW_ORDERS>(_msg->doorSolenoidPower,  HW_ORDERS_FIRST, HW_ORDERS_LAST, "Door solenoid power") ||
       !rangeCheck<HW_ORDERS>(_msg->alarmLight,         HW_ORDERS_FIRST, HW_ORDERS_LAST, "Alarm light") ||
       !rangeCheck<HW_ORDERS>(_msg->serviceEnableMode,  HW_ORDERS_FIRST, HW_ORDERS_LAST, "Enable service mode") ||
       !rangeCheck<HW_ORDERS>(_msg->donorConnectMode,   HW_ORDERS_FIRST, HW_ORDERS_LAST, "Donor connect mode"))
   {
      DataLog (log_level_ted_error) << "TED Safety Orders range check failure" << endmsg;
      return false;
   }

   // No validation required on HW Orders
   return true;
}



//
// TED_SafetyHwOrderMsg
// This method processes the message
//
//
template <> bool TedMessage<TED_SafetyHwOrderMsg >::process (void)
{
   TED_Status status = TED_REQUEST_ALLOWED;

   DataLog(log_level_ted_debug) << "Got TED Safety Hw Order Msg" << endmsg;

   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
   }
   else if (!bootServiceModeEnabled())
   {
      status = TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE;
   }

   SendStatus(status);

   DataLog(log_level_ted_debug) << "TED Safety Hw Order Msg returned status " << status << endmsg;

   if (status != TED_REQUEST_ALLOWED)
   {
      return false;
   }

   TedSafetyOrdersMsg orderMsg(MessageBase::SEND_GLOBAL);

   SHwOrders          safetyHwOrder;

   // Copy Safety HW Order
   safetyHwOrder.pumpPower         = _msg->pumpPower;
   safetyHwOrder.centrifugePower   = _msg->centrifugePower;
   safetyHwOrder.valveLED          = _msg->valveLED;
   safetyHwOrder.doorSolenoidPower = _msg->doorSolenoidPower;
   safetyHwOrder.alarmLight        = _msg->alarmLight;
   safetyHwOrder.serviceEnableMode = _msg->serviceEnableMode;
   safetyHwOrder.donorConnectMode  = _msg->donorConnectMode;

   // Send message
   orderMsg.send(safetyHwOrder);

   return true;
}



//
// TED_EndServiceMode
// This method processes the message
//
template <> bool TedMessage<TED_EndServiceModeMsg >::process (void)
{
   TED_Status status = TED_REQUEST_ALLOWED;

   DataLog(log_level_ted_info) << "Got TED End Service Mode Msg" << endmsg;

   // TED_EndServiceModeMsg has no body so this is a null call.
   // Left for completeness.
   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
   }
   else if (!bootServiceModeEnabled())
   {
      status = TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE;
   }

   SendStatus(status);

   DataLog(log_level_ted_info) << "TED End Service Mode Msg returned status " << status << endmsg;

   if (status != TED_REQUEST_ALLOWED)
   {
      return false;
   }

   // Turn off GoodDog.
   StopGoodDogMsg stop(MessageBase::SEND_GLOBAL);
   stop.send(0);

   return true;
}



//
// TED_MoveCursorMsg
// This method validates the body of the message.
//
template <> bool TedMessage<TED_MoveCursorMsg >::validBody (void)
{
   if (_msgSize != TED_MoveCursorMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_MoveCursorMsg size invalid.  Got " << _msgSize
                                   << ".  Expected " << TED_MoveCursorMsgSize << endmsg;

      return false;
   }

   if ( (_msg->order > TED_CURSOR_DOWN) || (_msg->order < TED_CURSOR_LEFT) )
   {
      DataLog(log_level_ted_error) << "cursor order invalid.  Got " << _msg->order
                                   << ".  Expected (" << TED_CURSOR_LEFT << "-" << TED_CURSOR_DOWN << ")." << endmsg;

      return false;
   }

   return true;
}



//
// TED_MoveCursorMsg
// This method processes the message
//
template <> bool TedMessage<TED_MoveCursorMsg >::process (void)
{
   TED_Status status = TED_REQUEST_ALLOWED;

   DataLog(log_level_ted_info) << "Got TED Move Cursor Msg" << endmsg;

   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
   }
   else if (!bootServiceModeEnabled())
   {
      status = TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE;
   }

   SendStatus(status);

   DataLog(log_level_ted_info) << "TED Move Cursor Msg returned status " << status << endmsg;

   if (status != TED_REQUEST_ALLOWED)
   {
      return false;
   }

   // Send the command on to the Service Screen.
   MovePointerServiceMsg msg(MessageBase::SEND_LOCAL);
   msg.send(_msg->order);

   return true;
}



#include "trima_time.h"

//
// TED_SetTimeMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_SetTimeMsg >::validBody (void)
{
   if (_msgSize != TED_SetTimeMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_SetTimeMsg size invalid.  Got " << _msgSize
                                   << ".  Expected " << TED_SetTimeMsgSize << endmsg;

      return false;
   }

   // No way to validate time

   return true;
}



//
// TED_SetTimeMsg
// This method processes the message
//
//
template <> bool TedMessage<TED_SetTimeMsg >::process (void)
{
   TED_Status status = TED_REQUEST_ALLOWED;

   DataLog(log_level_ted_info) << "Got TED Set Time Msg" << endmsg;

   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
   }
   else if (!bootServiceModeEnabled())
   {
      status = TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE;
   }

   SendStatus(status);

   DataLog(log_level_ted_info) << "TED Set Time Msg returned status " << status << endmsg;

   if (status != TED_REQUEST_ALLOWED)
   {
      return false;
   }

   // Send on the Set Time message
   DataLog(log_level_ted_info) << "Setting Trima time to " << ctime ((const long unsigned int*)&_msg->time)
                               << "(" << _msg->time << ")" << endmsg;

   trima_time::set(_msg->time);

   return true;
}



//
// TED_EndConnectionMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_EndConnectionMsg >::validBody (void)
{
   if (_msgSize != TED_EndConnectionMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_EndConnectionMsg size invalid.  Got " << _msgSize
                                   << ".  Expected " << TED_EndConnectionMsgSize << endmsg;

      return false;
   }

   return true;
}



//
// TED_EndConnectionMsg
// This method processes the message
//
//
template <> bool TedMessage<TED_EndConnectionMsg >::process (void)
{
   TED_Status status = TED_REQUEST_ALLOWED;

   DataLog(log_level_ted_info) << "Got TED End Connection Msg" << endmsg;

   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
   }

   DataLog(log_level_ted_info) << "TED End Connection Msg returned status " << status << endmsg;

   SendStatus(status);

   if (status != TED_REQUEST_ALLOWED)
   {
      return false;
   }
   else
   {
      return true;
   }
}



//
// TED_RequestCursorPositionMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_RequestCursorPositionMsg >::validBody (void)
{
   if (_msgSize != TED_RequestCursorPositionMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_RequestCursorPositionMsg size invalid.  Got " << _msgSize
                                   << ".  Expected " << TED_RequestCursorPositionMsgSize << endmsg;

      return false;
   }

   return true;
}



//
// TED_RequestCursorPositionMsg
// This method processes the message
//
//
template <> bool TedMessage<TED_RequestCursorPositionMsg >::process (void)
{
   TED_Status status = TED_REQUEST_ALLOWED;

   DataLog(log_level_ted_info) << "Got TED Request Cursor Position Msg" << endmsg;

   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
   }
   else if (!bootServiceModeEnabled())
   {
      status = TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE;
   }

   if (status != TED_REQUEST_ALLOWED)
   {
      DataLog(log_level_ted_info) << "TED Request Cursor Position Msg returned status " << status << endmsg;

      SendStatus(status);

      return false;
   }

   // Send a message to GUI requesting the current cursor position
   GetPointerServiceMsg msg(MessageBase::SEND_LOCAL);
   msg.send(0);

   // GUI sends back a CURRENT_CURSOR_POSITION_MSG.
   DataLog(log_level_ted_info) << "TED waiting for GUI cursor position..." << endmsg;

   return true;
}



//
// TED_RequestTrimaBroadcastMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_RequestTrimaBroadcastMsg >::validBody (void)
{
   if (_msgSize != TED_RequestTrimaBroadcastMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_RequestTrimaBroadcastMsg size invalid.  Got " << _msgSize
                                   << ".  Expected " << TED_RequestTrimaBroadcastMsgSize << endmsg;

      return false;
   }

   return true;
}


#include "trima_broadcast.h"


//
// TED_RequestTrimaBroadcastMsg
// This method processes the message
//
//
template <> bool TedMessage<TED_RequestTrimaBroadcastMsg >::process (void)
{
   TED_Status status = TED_REQUEST_ALLOWED;

   DataLog(log_level_ted_debug) << "Got TED Request Trima Broadcast Msg" << endmsg;

   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
   }

   DataLog(log_level_ted_debug) << "TED Request Trima Broadcast Msg returned status " << status << endmsg;

   if (status != TED_REQUEST_ALLOWED)
   {
      SendStatus(status);

      return false;
   }

   // Send the broadcast message
   TrimaBroadcast   broadcastUtils;
   TED_BroadcastMsg broadcastMsg;

   broadcastUtils.PackageMessage(broadcastMsg);

   TedMessageObj<TED_BroadcastMsg> msg(broadcastMsg, _addr);

   DataLog(log_level_ted_debug) << "Sent Broadcast message to " << _addr << " with : "
                                << "Count " << broadcastMsg.broadcastCount
                                << ", HW REV " << broadcastMsg.trimaHWRev
                                << ", IP " << broadcastMsg.trimaIP
                                << ", Log File " << broadcastMsg.trimaLogFile
                                << ", SN " << broadcastMsg.trimaSerialNumber
                                << ", CRC " << broadcastMsg.trimaCRC
                                << ", Release " << broadcastMsg.trimaRelease
                                << ", Build " << broadcastMsg.trimaBuild
                                << ", Port " << broadcastMsg.trimaPort
                                << ", Seq " << broadcastMsg.sequenceNumber
                                << ", VIP " << broadcastMsg.vipifVersionNumber
                                << ", TED " << broadcastMsg.tedifVersionNumber
                                << ", Bio " << broadcastMsg.biometricDeviceList << endmsg;

   msg.Send(TED_BROADCAST);

   return true;
}



//
// TED_ApsAutoZeroRequestMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_ApsAutoZeroRequestMsg >::validBody (void)
{
   if (_msgSize != TED_ApsAutoZeroRequestMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_ApsAutoZeroRequestMsg size invalid.  Got "
                                   << _msgSize << ".  Expected " << TED_ApsAutoZeroRequestMsgSize << endmsg;

      return false;
   }

   return true;
}



//
// TED_ApsAutoZeroRequestMsg
// This method processes the message
//
template <> bool TedMessage<TED_ApsAutoZeroRequestMsg >::process (void)
{
   DataLog(log_level_ted_info) << "Got TED APS AutoZero Request Msg" << endmsg;

   if (!validBody())
   {
      SendStatus(TED_BAD_MESSAGE_RECEIVED);

      return false;
   }

   if (!AutoCalDat::dataOK())
   {
      DataLog(log_level_ted_error) << "APS calibration data invalid." << endmsg;

      SendStatus(TED_OPERATION_FAILED);

      return false;
   }

   // Send reply
   TED_ApsAutoZeroValueMsg                replyMsg;
   TedMessageObj<TED_ApsAutoZeroValueMsg> reply(replyMsg, _addr);
   replyMsg.rawValue       = AutoCalDat::rawReference();
   replyMsg.referenceValue = AutoCalDat::lastZeroOffset();
   reply.Send(TED_APS_AUTOZERO_VALUE);

   return true;
}


//
// TED_LoggingMsg
// This method validates the body of the TED logging message.
//
//
template <> bool TedMessage<TED_LoggingMsg >::validBody (void)
{
   if (_msgSize != TED_LoggingMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_LoggingMsg size invalid.  Got "
                                   << _msgSize << ".  Expected " << TED_LoggingMsgSize << endmsg;

      return false;
   }

   return true;
}



//
// TED_LoggingMsg
// This method processes the TED logging message
//
//
template <> bool TedMessage<TED_LoggingMsg >::process (void)
{
   TED_Status status = TED_REQUEST_ALLOWED;

   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
      SendStatus(status);
      return false;
   }

   SendStatus(status);
   DataLog (log_level_ted_info) << "Ted Logging Message [" << _msg->loggingMsg << "]" << endmsg;
   return true;
}




//
// TED_ScreenCalibrationRequestMsg
// This method validates the body of the TED screen calibration message.
//
//
template <> bool TedMessage<TED_ScreenCalibrationRequestMsg >::validBody (void)
{
   if (_msgSize != TED_ScreenCalibrationRequestMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_ScreenCalibrationRequestMsg size invalid.  Got "
                                   << _msgSize << ".  Expected " << TED_ScreenCalibrationRequestMsgSize << endmsg;

      return false;
   }

   return true;
}



//
// TED_ScreenCalibrationRequestMsg
// This method processes the TED Screen Cal request
//
//
template <> bool TedMessage<TED_ScreenCalibrationRequestMsg >::process (void)
{
   TED_Status status = TED_REQUEST_ALLOWED;

   DataLog (log_level_ted_info) << "Recieved TED Screen Calibration Request Message." << endmsg;

   if (!validBody())
   {
      status = TED_BAD_MESSAGE_RECEIVED;
      SendStatus(status);
      return false;
   }

   RequestScreenCal scrnCalMsg(MessageBase::SEND_LOCAL);
   scrnCalMsg.send();

   SendStatus(status);
   return true;
}

//
// TED_ControlOVPTestRequestMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_ControlOVPTestRequestMsg >::validBody (void)
{
   if (_msgSize != TED_ControlOVPTestRequestMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_ControlOVPTestRequestMsg size invalid.  Got "
                                   << _msgSize << ".  Expected " << TED_ControlOVPTestRequestMsgSize << endmsg;

      return false;
   }

   return true;
}


//
// TED_ControlOVPTestRequestMsg
// This method processes the message
//
//
template <> bool TedMessage<TED_ControlOVPTestRequestMsg >::process (void)
{
   DataLog(log_level_ted_error) << "Got TED Control OVP Test Request Msg" << endmsg;

   if (!validBody())
   {
      SendStatus(TED_BAD_MESSAGE_RECEIVED);
      return false;
   }

   SendStatus(TED_REQUEST_ALLOWED);
   hw_OVPTest();
   return true;
}

//
// TED_HardwareRequestMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_HardwareRequestMsg >::validBody (void)
{
   if (_msgSize != TED_HardwareRequestMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_HardwareRequestMsg size invalid.  Got "
                                   << _msgSize << ".  Expected " << TED_HardwareRequestMsgSize << endmsg;

      return false;
   }

   return true;
}


//
// TED_HardwareRequestMsg
// This method processes the message
//
//
template <> bool TedMessage<TED_HardwareRequestMsg >::process (void)
{
   DataLog(log_level_ted_error) << "Got TED Hardware Request Msg" << endmsg;

   if (!validBody())
   {
      DataLog(log_level_ted_error) << "Invalid body in TED Hardware Request Msg" << endmsg;
      SendStatus(TED_BAD_MESSAGE_RECEIVED);
      return false;
   }

   // Send the trima hardware information message
   TED_HardwareReplyMsg tedHardwareReplyMsg;

   memset(tedHardwareReplyMsg.deviceHardwareXML, 0, TED_DEVICE_HARDWARE_XML);

   string temp;
   string boardType;
   if (isVersalogic())
   {
      if (isVersalogicVSBC6())
      {
         boardType = "Versalogic VSBC 6";
      }
      else if (isVersalogicPython())
      {
         boardType = "Versalogic Python";
      }
      else
      {
         boardType = "Versalogic [Unknown subtype]";
      }
   }
   else
   {
      boardType = "Unknown";
   }
   temp  = "<?xml version=\"1.0\"?>";
   temp += "<Component name=\"" + boardType + "\" type=\"Processor Board\"/>";

   strncpy(tedHardwareReplyMsg.deviceHardwareXML, temp.c_str(), temp.size());

   TedMessageObj<TED_HardwareReplyMsg> msg(tedHardwareReplyMsg, _addr);
   msg.Send(TED_HARDWARE_REPLY);
   DataLog(log_level_ted_info) << "Sent Hardware Information message to " << _addr << endmsg;

   return true;
}

//
// TED_FullVersionRequestMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_FullVersionRequestMsg >::validBody (void)
{
   if (_msgSize != TED_FullVersionRequestMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_FullVersionRequestMsg size invalid.  Got "
                                   << _msgSize << ".  Expected " << TED_FullVersionRequestMsgSize << endmsg;

      return false;
   }

   return true;
}


//
// TED_FullVersionRequestMsg
// This method processes the message
//
//
template <> bool TedMessage<TED_FullVersionRequestMsg >::process (void)
{
   DataLog(log_level_ted_error) << "Got TED Full Version Request Msg" << endmsg;

   if (!validBody())
   {
      DataLog(log_level_ted_error) << "Invalid body in TED Full Version Request Msg" << endmsg;
      SendStatus(TED_BAD_MESSAGE_RECEIVED);
      return false;
   }

   // Send the trima version information message
   TED_FullVersionReplyMsg tedFullVersionReplyMsg;

   memset(tedFullVersionReplyMsg.trimaFullVersion, 0, TED_FULL_VERSION_STRING_LENGTH);

   strncpy(tedFullVersionReplyMsg.trimaFullVersion,
           TrimaInfo::fullReleaseNumber(),
           strlen(TrimaInfo::fullReleaseNumber()));

   TedMessageObj<TED_FullVersionReplyMsg> msg(tedFullVersionReplyMsg, _addr);
   msg.Send(TED_FULL_VERSION_REPLY);
   DataLog(log_level_ted_info) << "Sent TED Full Version Reply message to " << _addr << endmsg;

   return true;
}

//
// TED_InstallKeyMsg
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_InstallKeyMsg >::validBody (void)
{
   if (_msgSize != TED_InstallKeyMsgSize)
   {
      DataLog(log_level_ted_error) << "TED_InstallKeyMsg size invalid.  Got "
                                   << _msgSize << ".  Expected " << TED_InstallKeyMsgSize << endmsg;

      return false;
   }

   return true;
}


//
// TED_InstallKeyMsg
// This method processes the message
//
//
template <> bool TedMessage<TED_InstallKeyMsg >::process (void)
{
   DataLog(log_level_ted_info) << "Got TED Install Key Msg" << endmsg;

   if (!validBody())
   {
      DataLog(log_level_ted_error) << "Invalid body in TED Install Key Msg" << endmsg;
      SendStatus(TED_BAD_MESSAGE_RECEIVED);
      return false;
   }
   else if (!bootServiceModeEnabled())
   {
      DataLog(log_level_ted_error) << "TED Install Key Msg when not in service mode" << endmsg;
      SendStatus(TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE);
      return false;
   }

   string newKey(_msg->key);
   newKey.erase(std::remove_if(newKey.begin(), newKey.end(), std::not1(std::ptr_fun(std::isalnum))), newKey.end());
   std::transform(newKey.begin(), newKey.end(), newKey.begin(), ::toupper);

   DataLog(log_level_ted_info) << "TED Install Key Msg has key " << newKey
                               << endmsg;

   KEY_RESULT result = Software_CDS::GetInstance().decodeKey(newKey, AUTH_SERVICE);

   DataLog(log_level_ted_info) << "setRegionalizationFeature request with key " << newKey
                               << " returned value " << result << endmsg;
   TED_Status replyCode = TED_REQUEST_ALLOWED;
   switch (result)
   {
      case SUCCESS :
         replyCode = TED_REQUEST_ALLOWED;
         break;

      case FAILED_UNAUTHORIZED_USER :
         replyCode = TED_UNAUTHORIZED_USER;
         break;

      case FAILED_UNAUTHORIZED_COUNTRY :
      case FAILED_UNAUTHORIZED_CUSTOMER :
      case FAILED_UNAUTHORIZED_SERIAL_NUMBER :
      case FAILED_UNAUTHORIZED_SITE :
         replyCode = TED_UNAUTHORIZED_MACHINE;
         break;

      case FAILED_BAD_KEY :
      default :
         replyCode = TED_BAD_VALUE;
         break;
   }

   SendStatus(replyCode);

   return true;
}

//
// TED_ConfirmationCodeRequest
// This method validates the body of the message.
//
//
template <> bool TedMessage<TED_ConfirmationCodeRequest >::validBody (void)
{
   if (_msgSize != TED_ConfirmationCodeRequestSize)
   {
      DataLog(log_level_ted_error) << "TED_ConfirmationCodeRequest size invalid.  Got "
                                   << _msgSize << ".  Expected " << TED_ConfirmationCodeRequestSize << endmsg;

      return false;
   }

   return true;
}


//
// TED_ConfirmationCodeRequest
// This method processes the message
//
//
template <> bool TedMessage<TED_ConfirmationCodeRequest >::process (void)
{
   DataLog(log_level_ted_info) << "Got TED Confirmation Code Request Msg" << endmsg;

   if (!validBody())
   {
      DataLog(log_level_ted_error) << "Invalid body in TED Confirmation Code Request Msg" << endmsg;
      SendStatus(TED_BAD_MESSAGE_RECEIVED);
      return false;
   }

   // Send the trima version information message
   TED_ConfirmationCodeReply tedCodeReplyMsg;

   memset(tedCodeReplyMsg.code, 0, TED_CONFIRM_CODE_SIZE);
   string code = Software_CDS::GetInstance().getFullConfirmationCode();


   strncpy(tedCodeReplyMsg.code, code.c_str(), code.size());

   TedMessageObj<TED_ConfirmationCodeReply> msg(tedCodeReplyMsg, _addr);
   msg.Send(TED_CONFIRMATION_CODE_REPLY);
   DataLog(log_level_ted_info) << "Sent TED Confirmation Code Reply message to " << _addr << endmsg;

   return true;
}

#endif

/* FORMAT HASH 0d22a437d31bf810f92a21b546da43c2 */
