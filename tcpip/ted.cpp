/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ted.cpp
 *             Trima External Diagnostic Interface
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface between Trima and EMS.
 *
 * HISTORY:    $Log: ted.cpp $
 * HISTORY:    Revision 1.19  2009/07/08 17:00:33Z  dslausb
 * HISTORY:    Update TED interface for 6.0
 * HISTORY:    Revision 1.18  2008/11/17 17:52:28Z  dslausb
 * HISTORY:    Fix timing issue with TED and gooddog, add some more logging.
 * HISTORY:    Revision 1.17  2008/01/23 17:34:17Z  dslausb
 * HISTORY:    IT 8435 - VxWorks 5.5 Checkin
 * HISTORY:    Revision 1.16  2006/11/08 21:45:20Z  rm70006
 * HISTORY:    IT 7232.  Added new status bits to Artesyn power supply
 * HISTORY:    Revision 1.15  2004/01/28 17:14:33Z  rm70006
 * HISTORY:    IT 6777.  Fix bug where response is sent to wrong port if EMS doesn't disconnect proplerly.
 * HISTORY:    Revision 1.14  2004/01/19 22:06:43Z  rm70006
 * HISTORY:    IT 6795.  Improve logging.
 * HISTORY:    Revision 1.13  2003/11/07 16:39:32Z  rm70006
 * HISTORY:    IT 6593.  Improve logging of invalid message ID.
 * HISTORY:    Revision 1.12  2003/11/05 16:37:12Z  rm70006
 * HISTORY:    IT 5830.  Fix bug with end connection message.
 * HISTORY:    Revision 1.11  2003/10/17 16:51:58Z  rm70006
 * HISTORY:    IT 6522.  Make some changes so TED doesn't reboot trima.
 * HISTORY:    Revision 1.10  2003/10/16 16:09:57Z  rm70006
 * HISTORY:    IT 6522.  Make temp fix to increase queue size.
 * HISTORY:    Revision 1.9  2003/10/15 16:55:14Z  rm70006
 * HISTORY:    IT 6492.  Added both raw and calibrated values to interface.
 * HISTORY:    Revision 1.8  2003/10/14 21:54:31Z  rm70006
 * HISTORY:    IT 5830.  Add current cursor position message.  Fix bug with buffer logic.
 * HISTORY:    Revision 1.7  2003/10/08 18:31:38Z  rm70006
 * HISTORY:    IT 5830.  Put more fixes in TED.
 * HISTORY:    Revision 1.6  2003/07/01 15:56:55Z  rm70006
 * HISTORY:    IT 5830.  Interim checkin.  Broadcast msg still not working.
 * HISTORY:    Revision 1.5  2003/06/16 21:58:17Z  rm70006
 * HISTORY:    IT 5830.
 * HISTORY:    Add more functionality to TED.
 * HISTORY:    Revision 1.4  2003/05/16 13:34:59Z  rm70006
 * HISTORY:    IT 5830.
 * HISTORY:
 * HISTORY:    Add more functionality.
 * HISTORY:    Revision 1.3  2003/04/07 23:09:36Z  rm70006
 * HISTORY:    IT 5818.
 * HISTORY:
 * HISTORY:    Change logging.
 * HISTORY:    Revision 1.2  2002/12/20 21:49:54Z  rm70006
 * HISTORY:    Incremental checkin.
 * HISTORY:    Revision 1.1  2002/10/29 16:12:31Z  rm70006
 * HISTORY:    Initial revision
 *
 *******************************************************************************/

#include <netinet/if_ether.h>       // Get Network MTU size

#include "ted.h"
#include "tedif.h"                  // Get TED External Message Set

#include "portnumbers.h"
#include "trima_datalog.h"          // Get Datalogging stuff
#include "units.h"                  // Get Unit conversions.



const int            NOMINAL_CTL_DRIVER_UPDATE_TIME = 250; // Nominal Driver update rate in ms

const struct timeval waitForever                    = {SECONDS_PER_HOUR* HOURS_PER_DAY, 0};


//
// Default Constructor
//
TED::TED()
   : _listenAddress((unsigned short)Trima_ServiceCmdPort),
     _commandSocket(sockinetbuf::af_inet, sockbuf::sock_dgram),
     _connectedHost(),
     _connectionActive(false),
     _controlPeriodicStatus(0),
     _safetyPeriodicStatus(0),
     _xifCds(ROLE_RO),
     _currentCursorPositionMsg(NULL)
{}



//
// Default Destructor
//
TED::~TED()
{
   if (_currentCursorPositionMsg)
      delete _currentCursorPositionMsg;

   if (_controlPeriodicStatus)
      delete _controlPeriodicStatus;

   if (_safetyPeriodicStatus)
      delete _safetyPeriodicStatus;
}



//
// Initialize
//
bool TED::Initialize ()
{
   // Since TED has to handle both socket data and message queue data, double the queue size to be able to handle
   // delays in servicing it.
   if (!_ms.initNonBlock("TED", MessageSystemConstant::DEFAULT_Q_SIZE * 2))
   {
      DataLog(log_level_ted_error) << "Message System failed to initialize" << endmsg;
      return false;
   }

   if (_commandSocket.bind(_listenAddress) != 0)
   {
      DataLog(log_level_ted_error) << "Bind failed.  Error = " << errnoMsg << endmsg;
      return false;
   }

   // Create the Periodic Status Messages.
   // Do this after the message system has been started.
   _controlPeriodicStatus = new TedHwPeriodicStatus (CONTROL);
   _safetyPeriodicStatus  = new TedHwPeriodicStatus (SAFETY);

   DataLog(log_level_ted_debug) << "TED_Header size is " << TED_HEADER_SIZE << endmsg;
   DataLog(log_level_ted_debug) << "TED_ReplyMsg size is " << TED_ReplyMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_ConnectRequestMsg size is " << TED_ConnectRequestMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_HwPeriodicStatusRequestMsg size is " << TED_HwPeriodicStatusRequestMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_ControlHwStatusMsg size is " << TED_ControlHwStatusMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_SafetyHwStatusMsg size is " << TED_SafetyHwStatusMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_ControlHwOrderMsg size is " << TED_ControlHwOrderMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_SafetyHwOrderMsg size is " << TED_SafetyHwOrderMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_EndServiceModeMsg size is " << TED_EndServiceModeMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_MoveCursorMsg size is " << TED_MoveCursorMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_SetTimeMsg size is " << TED_SetTimeMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_EndConnectionMsg size is " << TED_EndConnectionMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_RequestCursorPositionMsg size is " << TED_RequestCursorPositionMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_CurrentCursorPositionMsg size is " << TED_CurrentCursorPositionMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_RequestTrimaBroadcastMsg size is " << TED_RequestTrimaBroadcastMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_BroadcastMsg size is " << TED_BroadcastMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_ApsAutoZeroRequestMsg size is " << TED_ApsAutoZeroRequestMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_ApsAutoZeroValueMsg size is " << TED_ApsAutoZeroValueMsgSize << endmsg;
   DataLog(log_level_ted_debug) << "TED_LoggingMsg size is " << TED_LoggingMsgSize << endmsg;

   return true;
}



//
// ProcessMessages
//
void TED::ProcessMessages ()
{
   const int numberOfSockets = 2; // Actual number of sockets used + 1
   const int bufferSize      = ETHERMTU;

   char      buffer[bufferSize];
   int       bufferIndex = 0;

   fd_set    readSockets;

   // Default timeout value to large number.  Until we have a connection,
   // there is no need to wake up.
   _timeoutValue = waitForever;

   while (true)
   {
      // Initialize the socket list
      FD_ZERO(&readSockets);
      FD_SET (_commandSocket, &readSockets);

      int status = select(FD_SETSIZE, &readSockets, 0, 0, &_timeoutValue);

      if (status == ERROR)
      {
         // Error occurred.
         DataLog(log_level_ted_error) << "Select Error " << errnoMsg << endmsg;
      }
      else if (status == 0)   // Request timed out.
      {
         // Process Trima messages.
         ProcessInternalMessages();
      }
      else  // Got socket data, process it.
      {
         // Process Trima messages.
         ProcessInternalMessages();

         sockinetaddr client;

         // Data received
         int packetSize = _commandSocket.recvfrom(client, &buffer[bufferIndex], bufferSize - bufferIndex);

         DataLog(log_level_ted_debug) << "Rx " << packetSize << " bytes from " << client
                                      << ".  BufferIndex is " << bufferIndex << ".  Header is " << *(TED_Header*)buffer << endmsg;

         if (_xifCds.safetyStarted.Get() == false)
         {
            DataLog(log_level_ted_info) << "Safety not started yet." << endmsg;

            TED_ReplyMsg                reply;
            TedMessageObj<TED_ReplyMsg> tedMsg(reply, client);
            reply.status = TED_SAFETY_NOT_STARTED;
            tedMsg.Send(TED_REPLY);

            // go back and try again.
            continue;
         }

         bool bufferNotEmpty = true;

         while (bufferNotEmpty)
         {
            // Validate Header
            const TED_BUFFER_STATUS msgStatus = _tedIf.validHeader(buffer, bufferIndex + packetSize);

            bufferNotEmpty = (msgStatus == TED_BUFFER_OVERRUN);

            DataLog(log_level_ted_debug) << "Header status is " << msgStatus << ".  BufferIndex is "
                                         << bufferIndex << ".  Header is " << *(TED_Header*)buffer << endmsg;

            void* msg = GetMessageFromBuffer(client, msgStatus, buffer, bufferIndex, packetSize);

            // If return message is 0, then message was bad
            if (msg == 0)
            {
               continue;   // Go back to select
            }

            // Check to see if we have an active connection
            if (_connectionActive)
            {
               if (client.sin_addr.s_addr != _connectedHost.sin_addr.s_addr)
               {
                  // Can't serve 2 masters.  Reject requestor
                  DataLog(log_level_ted_info) << "Second Host Request(" << client << ") received."
                                              << "  Currently communicating with (" << _connectedHost << ").  Request denied!" << endmsg;

                  TED_ReplyMsg                reply;
                  TedMessageObj<TED_ReplyMsg> tedMsg(reply, client);
                  reply.status = TED_REQUEST_DENIED_NOT_CLIENT;
                  tedMsg.Send(TED_REPLY);

                  // If new message was malloced (buffer overrun case), delete it.
                  if (msg != buffer)
                  {
                     delete (char*)msg;
                  }

                  continue;
               }
               else if (client.sin_port != _connectedHost.sin_port)
               {
                  // Same IP, different port.  EMS must have gone down and restarted with a new port
                  DataLog(log_level_ted_info) << "Same Host with different port request(" << client << ") received."
                                              << "  Was communicating with (" << _connectedHost << ").  Changing ports." << endmsg;

                  _connectedHost = client;
               }
            }
            else
            {
               TED_MessageId msgId = ((TED_MessageId)((TED_Header*)msg)->msgId);

               // Remember host name
               _connectedHost = client;

               if ( msgId == TED_CONNECT_REQUEST )
               {
                  DataLog(log_level_ted_info) << "New connection from Host (" << client << ") received." << endmsg;

                  _connectionActive = true;
               }
               else if (msgId == TED_BROADCAST_REQUEST)
               {
                  DataLog(log_level_ted_info) << "Broadcast Request message received from non-connected Host (" << client << ") received." << endmsg;
               }
               else
               {
                  DataLog(log_level_ted_info) << "Message from host (" << client << ") not a connection message. (ID=" << msgId << ")" << endmsg;
               }
            }

            // Process the message
            ProcessOneMessage(msg);

            // If new message was malloced (buffer overrun case), delete it.
            if (msg != buffer)
            {
               delete (char*)msg;
            }

            // Manage things depending on the connection state
            ManageConnection();
         }
      }
   }
}



//
// ProcessOneMessage
//
void TED::ProcessOneMessage (const void* msg)
{
   const TED_Header* hdr        = (TED_Header*)msg;
   const int         packetSize = hdr->length + TED_HEADER_SIZE;

   // Process the message.
   DataLog(log_level_ted_debug) << "Message " << *hdr << " received from host " << _connectedHost << "." << endmsg;

   switch (hdr->msgId)
   {
      case TED_CONNECT_REQUEST :
      {
         TedMessage<TED_ConnectRequestMsg> connectMsg(_connectedHost);
         connectMsg.process();
      }
      break;

      case TED_HW_PERIODIC_STATUS_REQUEST :
      {
         TedMessage<TED_HwPeriodicStatusRequestMsg> periodicStatusRequestMsg(_connectedHost, msg, packetSize);

         // If message was processed, Set the timeout value.
         if (periodicStatusRequestMsg.process())
         {
            const TED_HwPeriodicStatusRequestMsg* hwMsg = periodicStatusRequestMsg;
            TED_Status status;
            int        period = hwMsg->period;

            // Silently enforce 500 ms min.
            if ( (period > 0) && (period < 500) )
            {
               period = 500;

               DataLog(log_level_ted_error) << "period value invalid.  Got " << hwMsg->period
                                            << ".  Expected 500+" << endmsg;
            }

            if (hwMsg->board == CONTROL)
            {
               if (_controlPeriodicStatus->isSet())
                  status = TED_REQUEST_ALLOWED_OVERRIDE;
               else
                  status = TED_REQUEST_ALLOWED;

               sockinetaddr addr = _connectedHost;
               addr.setport(hwMsg->port);
               _controlPeriodicStatus->set(addr, period);
            }
            else
            {
               if (_safetyPeriodicStatus->isSet())
                  status = TED_REQUEST_ALLOWED_OVERRIDE;
               else
                  status = TED_REQUEST_ALLOWED;

               sockinetaddr addr = _connectedHost;
               addr.setport(hwMsg->port);
               _safetyPeriodicStatus->set(addr, period);
            }

            DataLog(log_level_ted_info) << "TED Hw Periodic Status Request Msg returned status " << status << endmsg;

            periodicStatusRequestMsg.SendStatus(status);

            DataLog(log_level_ted_info) << "Setting timeout value for " << hwMsg->board << " to " << period << " ms." << endmsg;

            // Make sure that we at least service the expected driver update rate.
            if (period < NOMINAL_CTL_DRIVER_UPDATE_TIME)
            {
               _timeoutValue.tv_sec  = 0;
               _timeoutValue.tv_usec = hwMsg->period * MICROSEC_PER_MILLISEC;
            }
            else
            {
               _timeoutValue.tv_sec  = 0;
               _timeoutValue.tv_usec = NOMINAL_CTL_DRIVER_UPDATE_TIME * MICROSEC_PER_MILLISEC;
            }
         }
      }
      break;

      case TED_CONTROL_HARDWARE_CMD :
      {
         TedMessage<TED_ControlHwOrderMsg> controlHwCmdMsg(_connectedHost, msg, packetSize);
         controlHwCmdMsg.process();
      }
      break;

      case TED_SAFETY_HARDWARE_CMD :
      {
         TedMessage<TED_SafetyHwOrderMsg> safetyHwCmdMsg(_connectedHost, msg, packetSize);
         safetyHwCmdMsg.process();
      }
      break;

      case TED_END_SERVICE_MODE :
      {
         TedMessage<TED_EndServiceModeMsg> endServiceModeMsg(_connectedHost);
         endServiceModeMsg.process();
      }
      break;

      case TED_MOVE_CURSOR_CMD :
      {
         TedMessage<TED_MoveCursorMsg> moveCursorMsg(_connectedHost, msg, packetSize);
         moveCursorMsg.process();
      }
      break;

      case TED_SET_TIME :
      {
         TedMessage<TED_SetTimeMsg> setTimeMsg(_connectedHost, msg, packetSize);
         setTimeMsg.process();
      }
      break;

      case TED_END_CONNECTION :
      {
         TedMessage<TED_EndConnectionMsg> endConnection(_connectedHost, msg, packetSize);

         if (endConnection.process())
         {
            _connectionActive = false;

            // DeRegister for all messages
            DeRegister();


            // Go back to wait forever on the socket for the next client
            _timeoutValue = waitForever;
         }
      }
      break;

      case TED_REQUEST_CURSOR_POSITION :
      {
         TedMessage<TED_RequestCursorPositionMsg> requestCursorPositionMsg(_connectedHost, msg, packetSize);
         requestCursorPositionMsg.process();
      }
      break;

      case TED_BROADCAST_REQUEST :
      {
         TedMessage<TED_RequestTrimaBroadcastMsg> broadcastRequestMsg(_connectedHost, msg, packetSize);
         broadcastRequestMsg.process();
      }
      break;

      case TED_REQUEST_APS_AUTOZERO_VALUE :
      {
         TedMessage<TED_ApsAutoZeroRequestMsg> apsAutoZeroRequestMsg(_connectedHost, msg, packetSize);
         apsAutoZeroRequestMsg.process();
      }
      break;

      case TED_LOGGING_MESSAGE :
      {
         TedMessage<TED_LoggingMsg> loggingMsg(_connectedHost, msg, packetSize);
         loggingMsg.process();
      }
      break;

      case TED_SCREEN_CALIBRATION_MESSAGE :
      {
         DataLog (log_level_ted_info) << "Got a screen calibration message." << endmsg;
         TedMessage<TED_ScreenCalibrationRequestMsg> scrnCalMsg(_connectedHost, msg, packetSize);
         scrnCalMsg.process();
      }
      break;


      case TED_CONTROL_OVP_TEST_REQUEST :
      {
         DataLog (log_level_ted_info) << "Got an OVP test message." << endmsg;
         TedMessage<TED_ControlOVPTestRequestMsg> ovpRequestMsg(_connectedHost, msg, packetSize);
         ovpRequestMsg.process();
      }
      break;

      case TED_HARDWARE_REQUEST :
      {
         DataLog (log_level_ted_info) << "Got a hardware request message." << endmsg;
         TedMessage<TED_HardwareRequestMsg> hardwareRequestMsg(_connectedHost, msg, packetSize);
         hardwareRequestMsg.process();
      }
      break;

      case TED_FULL_VERSION_REQUEST :
      {
         DataLog (log_level_ted_info) << "Got a full version request message." << endmsg;
         TedMessage<TED_FullVersionRequestMsg> fullVersionRequestMsg(_connectedHost, msg, packetSize);
         fullVersionRequestMsg.process();
      }
      break;

      case TED_INSTALL_REGIONALIZATION_KEY :
      {
         DataLog (log_level_ted_info) << "Got a key installation message." << endmsg;
         TedMessage<TED_InstallKeyMsg> installKeyMsg(_connectedHost, msg, packetSize);
         installKeyMsg.process();
      }
      break;

      case TED_CONFIRMATION_CODE_REQUEST :
      {
         DataLog (log_level_ted_info) << "Got a confirmation code request message." << endmsg;
         TedMessage<TED_ConfirmationCodeRequest> confirmationCodeRequestMsg(_connectedHost, msg, packetSize);
         confirmationCodeRequestMsg.process();
      }
      break;

      default :
         DataLog(log_level_ted_error) << "Unexpected message identifier.  Header is " << *hdr << endmsg;
   }
}



//
// ProcessInternalMessages
//
void TED::ProcessInternalMessages ()
{
   // Process Status Messages
   int numberOfMessagesLeft = _ms.dispatchMessages();

   if (numberOfMessagesLeft > 6)
   {
      DataLog(log_level_ted_info) << "TED has " << numberOfMessagesLeft << " TRIMA messages to process." << endmsg;
   }

   while (numberOfMessagesLeft > 0)
   {
      numberOfMessagesLeft = _ms.dispatchMessages();
   }
}



void* TED::GetMessageFromBuffer (const sockinetaddr& client, TED_BUFFER_STATUS msgStatus, char* buffer, int& bufferIndex, int packetSize)
{
   void* msg = buffer;

   switch (msgStatus)
   {
      case TED_BUFFER_UNDERRUN :
         // Not enough data to process a message.  Save it.
         bufferIndex += packetSize;
         DataLog(log_level_ted_debug) << _tedIf.errorString() << "  bufferIndex = " << bufferIndex << ", packetSize = "
                                      << packetSize << endmsg;
         break;

      case TED_BUFFER_OVERRUN :
         // Got a message plus some.
         // Copy off the whole message and save off the partial one
      {
         // Copy off the complete message
         const int msgSize = ((TED_Header*)buffer)->length + TED_HEADER_SIZE;
         msg = new char[msgSize];
         memmove(msg, buffer, msgSize);

         const int leftOver = packetSize - msgSize;
         // Move the left over to the front of the buffer
         memmove(buffer, &buffer[bufferIndex + msgSize], leftOver);
         bufferIndex = leftOver;

         DataLog(log_level_ted_debug) << _tedIf.errorString() << "  bufferIndex = " << bufferIndex << endmsg;
      }
      break;

      case TED_OK :
         // Got just enough.  Process the message.
         bufferIndex = 0;
         break;

      case TED_BAD_SOM :
      {
         DataLog(log_level_ted_error) << _tedIf.errorString() << "  Bad Header is " << *(TED_Header*)&buffer[bufferIndex] << endmsg;

         const int bufferOffset = _tedIf.findSOM(&buffer[bufferIndex], packetSize);

         if (bufferOffset == 0)
         {
            // Don't know what we got, flush the buffer.
            bufferIndex = 0;
            DataLog(log_level_ted_error) << "Couldn't find SOM in rest of buffer.  Flushing buffer..." << endmsg;

            TED_ReplyMsg                reply;
            TedMessageObj<TED_ReplyMsg> tedMsg(reply, client);
            reply.status = TED_BAD_MESSAGE_RECEIVED;
            tedMsg.Send(TED_REPLY);

            return 0;
         }
         else
         {
            bufferIndex += bufferOffset;

            DataLog(log_level_ted_error) << "Found the header " << bufferOffset << " bytes into the slop.  Buffer Index is " << bufferIndex;
            DataLog(log_level_ted_error) << "Header is " << *(TED_Header*)&buffer[bufferIndex] << endmsg;
         }
      }
      break;

      case TED_HEADER_CRC_INVALID :
      case TED_BODY_CRC_INVALID :
      case TED_MESSAGE_ID_INVALID :
      {
         DataLog(log_level_ted_error) << _tedIf.errorString() << "  Bad Header is " << *(TED_Header*)&buffer[bufferIndex] << endmsg;

         TED_ReplyMsg                reply;
         TedMessageObj<TED_ReplyMsg> tedMsg(reply, client);
         reply.status = TED_BAD_MESSAGE_RECEIVED;
         tedMsg.Send(TED_REPLY);

         // Don't know what we got, flush the buffer.
         bufferIndex = 0;

         return 0;
      }

      break;
   }

   return msg;
}



//
// HandleCursorCurrentPostionMsg
//
bool TED::HandleCurrentCursorPositionMsg (const CURRENT_CURSOR_POSITION_MSG& in, TED_CurrentCursorPositionMsg& out)
{
   out.x = in.x;
   out.y = in.y;

   return true;
}



//
// ManageConnection
//
void TED::ManageConnection ()
{
   static bool connectionWasActive = false;

   // Only enable this in service mode.
   if (!bootServiceModeEnabled())
      return;

   if ( (_connectionActive && connectionWasActive) ||
        (!_connectionActive && !connectionWasActive)
        )
   {
      // Do nothing.  No change
   }
   else if (_connectionActive && !connectionWasActive)
   {
      // New connection.  Turn on stuff
      connectionWasActive       = true;
      _currentCursorPositionMsg = new TedOneToOneStatusMsg <TED, CURRENT_CURSOR_POSITION_MSG, TED_CurrentCursorPositionMsg> (_connectedHost, TED_CURRENT_CURSOR_POSITION, this, &TED::HandleCurrentCursorPositionMsg);
      DataLog(log_level_ted_info) << "Creating Cursor Position Msg" << endmsg;
   }
   else if (!_connectionActive && connectionWasActive)
   {
      // Connection Lost.  Turn off stuff
      connectionWasActive       = false;
      delete _currentCursorPositionMsg;
      _currentCursorPositionMsg = 0;
      DataLog(log_level_ted_info) << "Deleting Cursor Position Msg" << endmsg;
   }
}



//
// UnRegister
//
void TED::DeRegister ()
{
   if ( _controlPeriodicStatus && _controlPeriodicStatus->isSet() )
   {
      _controlPeriodicStatus->deactivate();
   }

   if ( _safetyPeriodicStatus && _safetyPeriodicStatus->isSet() )
   {
      _safetyPeriodicStatus->deactivate();
   }

   if (_currentCursorPositionMsg)
   {
      _currentCursorPositionMsg->deregisterMsg();
   }
}


TED* _ted = NULL;

extern "C" bool IsTedConnectionActive ()
{
   bool ret = false;

   if (_ted)
      ret = _ted->IsConnectionActive();

   return ret;
}


//
// main routine for TED
//
extern "C" void ted ()
{
   DataLog (log_level_ted_info) << "TED process started" << endmsg;

   _ted = new TED();

   DataLog (log_level_ted_info) << "Initializing TED..." << endmsg;

   if (!_ted->Initialize())
   {
      // TBD.  Generate Alert.
      DataLog (log_level_ted_error) << "TED failed to initialize.  Exiting... " << endmsg;

      delete _ted;
      return;
   }

   DataLog(log_level_ted_info) << "TED is ready to process messages." << endmsg;

   _ted->ProcessMessages();

   DataLog(log_level_ted_info) << "TED process exiting..." << endmsg;

   delete _ted;
}

/* FORMAT HASH e53a92a6c2262379e0e2986057918215 */
