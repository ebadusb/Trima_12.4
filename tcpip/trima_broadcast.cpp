/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      trima_broadcast.h
 *             Trima Broadcast Message Class
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the Trima Broadcast Message function.  This is a stand-alone
 *             process that will turn on and off the external broadcast message when commanded.
 *
 ******************************************************************************/

#include "trima_broadcast.h"

#include "trima_info.h"    // Gets methods to get trima info
#include "trima_datalog.h" // Gets log levels
#include "messagesystem.h" // Gets message system stuff
#include "vipif.h"         // Gets the Vista Interface Version number
#include "trima_tasks.h"   // Gets task synchronization information
#include "software_cds.h"

#include <time.h>          // Gets clock_gettime function.



//
// Public Constructors
//


//
// Constructor used for getting broadcast info
//
TrimaBroadcast::TrimaBroadcast()
   : _xifCds(ROLE_RW),
     _messageCount(0),
     _udpBroadcastTimer(0, Callback<TrimaBroadcast>(this, &TrimaBroadcast::SendExternalBroadcast), TimerMessage::DISARMED),
     _broadcastSocket(AF_INET, sockbuf::sock_dgram),
     _vistaDirectSendSocket(AF_INET, sockbuf::sock_dgram),
     _ms(0),
     _vistaDirectSendAddressConfigured(false)
{}



//
// Constructor used for broadcast task.
//
TrimaBroadcast::TrimaBroadcast(MessageSystem* ms)
   : _xifCds(ROLE_RW),
     _messageCount(0),
     _udpBroadcastTimer(0, Callback<TrimaBroadcast>(this, &TrimaBroadcast::SendExternalBroadcast), TimerMessage::DISARMED),
     _broadcastSocket(AF_INET, sockbuf::sock_dgram),
     _vistaDirectSendSocket(AF_INET, sockbuf::sock_dgram),
     _ms(ms),
     _vistaDirectSendAddressConfigured(false)
{}



//
// Base Destructor
//
TrimaBroadcast::~TrimaBroadcast()
{}



//
// Initialize
//
void TrimaBroadcast::Initialize ()
{
   DataLog(log_level_trima_broadcast_info) << "Initializing Trima Broadcast" << endmsg;

   // Get Vista direct send info from Software_CDS first
   bool     foundDirectSendIp    = false;
   bool     foundDirectSendPort  = false;

   uint32_t vistaDirectSendIPNum = Software_CDS::GetInstance().getValueFeature(VistaIP);

   if (vistaDirectSendIPNum > 0)
      foundDirectSendIp = true;

   string   vistaDirectSendIpAddress = Software_CDS::GetInstance().convertIntToIP(vistaDirectSendIPNum);

   uint32_t vistaPortNumber          = Software_CDS::GetInstance().getValueFeature(VistaPort);

   if (vistaPortNumber > 0)
      foundDirectSendPort = true;

   bool vistaDirectSendConfigured = foundDirectSendIp && foundDirectSendPort;

   DataLog (log_level_trima_broadcast_info) << "Vista Direct Send IP Address("
                                            << vistaDirectSendIpAddress << ") Port(" << vistaPortNumber << ") Configured("
                                            << vistaDirectSendConfigured << ")" << endmsg;

   TrimaInfo::updateVistaDirectSendInfo(vistaDirectSendIpAddress, vistaPortNumber, vistaDirectSendConfigured);

   // Create Broadcast Socket
   _broadcastSocket.broadcast(1);

   _externalListener.setaddr(TrimaInfo::broadcastAddress());
   _externalListener.setport(TrimaInfo::broadcastPort());

   if (vistaDirectSendConfigured)
   {
      DataLog(log_level_trima_broadcast_info) << "Initializing Vista Direct Send..." << endmsg;

      _vistaDirectSendAddress.setaddr(vistaDirectSendIpAddress.c_str());
      _vistaDirectSendAddress.setport(vistaPortNumber);

      _vistaDirectSendAddressConfigured = true;
   }

   // Fill in the broadcast message contents
   PackageMessage(_broadcastMsg);

   // Set the original sequence number in the XIF CDS
   if (_xifCds.firstSequenceNumber.Get() == -1)  // -1 means no PFR
   {
      const unsigned int sequenceNumber = atoi(TrimaInfo::sequenceNumber());
      _xifCds.firstSequenceNumber.Set(sequenceNumber);
   }

   _vistaDirectSendSocket.turnOffLogging();
   TASK_DONE;
}



//
// StartBroadcast
//
void TrimaBroadcast::StartBroadcast (void)
{
   if (_udpBroadcastTimer.timerArmed() == TimerMessage::DISARMED)
   {
      DataLog(log_level_trima_broadcast_info) << "Starting Broadcast Message" << endmsg;

      _udpBroadcastTimer.interval(BROADCAST_INTERVAL);
   }
}



//
// StopBroadcast
//
void TrimaBroadcast::StopBroadcast (void)
{
   if (_udpBroadcastTimer.timerArmed() == TimerMessage::ARMED)
   {
      DataLog(log_level_trima_broadcast_info) << "Stopping Broadcast Message" << endmsg;

      _udpBroadcastTimer.interval(0);
   }

   DataLog(log_level_trima_broadcast_info) << "Stopping Messaging for Trima Broadcast Task" << endmsg;

   _ms->stopLoop();
}



//
// SendExternalBroadcast
//
void TrimaBroadcast::SendExternalBroadcast (void)
{
   int result;

   // Only log the first broadcast message.
   if (++_messageCount == 1)
   {
      DataLog(log_level_trima_broadcast_info) << "Starting Broadcast" << endmsg;

      // log broadcast
      DataLog(log_level_trima_broadcast_info) << "S/N:"         << _broadcastMsg.trimaSerialNumber  << endmsg;
      DataLog(log_level_trima_broadcast_info) << "Release:"     << _broadcastMsg.trimaRelease       << endmsg;
      DataLog(log_level_trima_broadcast_info) << "Build:"       << _broadcastMsg.trimaBuild         << endmsg;
      DataLog(log_level_trima_broadcast_info) << "CRC:"         << _broadcastMsg.trimaCRC           << endmsg;
      DataLog(log_level_trima_broadcast_info) << "HWRev:"       << _broadcastMsg.trimaHWRev         << endmsg;
      DataLog(log_level_trima_broadcast_info) << "IP:"          << _broadcastMsg.trimaIP            << endmsg;
      DataLog(log_level_trima_broadcast_info) << "Port:"        << _broadcastMsg.trimaPort          << endmsg;
      DataLog(log_level_trima_broadcast_info) << "LogFile:"     << _broadcastMsg.trimaLogFile       << endmsg;
      DataLog(log_level_trima_broadcast_info) << "EVR:"         << _broadcastMsg.sequenceNumber     << endmsg;
      DataLog(log_level_trima_broadcast_info) << "VIP Version:" << _broadcastMsg.vipifVersionNumber << endmsg;
      DataLog(log_level_trima_broadcast_info) << "TED Version:" << _broadcastMsg.tedifVersionNumber << endmsg;

      DataLog(log_level_trima_broadcast_info) << "Sending Broadcast to " << _externalListener << endmsg;

      if (_vistaDirectSendAddressConfigured)
      {
         DataLog(log_level_trima_broadcast_info) << "Sending Vista Direct Send message to " << _vistaDirectSendAddress << endmsg;
      }

      DataLog(log_level_trima_broadcast_info) << "Full Release:" << TrimaInfo::fullReleaseNumber() << endmsg;

      // Reset broadcast complete flag.
      _xifCds.broadcastComplete.Set(false);
   }

   if (_messageCount > MAX_NUMBER_OF_BROADCASTS)
   {
      // Stop the Broadcasts.
      StopBroadcast();

      // Set broadcast complete flag.
      _xifCds.broadcastComplete.Set(true);

      DataLog(log_level_trima_broadcast_info) << "Stopping Broadcast" << endmsg;
   }
   else
   {
      _broadcastMsg.broadcastCount = _messageCount;

      // Update the message header
      _tedif.prepareMsg(&_broadcastMsg, TED_BROADCAST, TED_BroadcastMsgSize);

      // Send Broadcast message.
      result = _broadcastSocket.sendto(_externalListener, &_broadcastMsg, TED_BroadcastMsgSize);

      if (result == ERROR)
      {
         DataLog(log_level_trima_broadcast_error) << "Broadcast socket send error number " << errnoMsg << endmsg;
      }
      else
      {
         DataLog(log_level_trima_broadcast_info) << "Broadcast sent (" << result << "/" << TED_BroadcastMsgSize << ") bytes." << endmsg;
      }

      if (_vistaDirectSendAddressConfigured)
      {
         result = _vistaDirectSendSocket.sendto(_vistaDirectSendAddress, &_broadcastMsg, TED_BroadcastMsgSize);

         if (result == ERROR)
            DataLog(log_level_trima_broadcast_error) << "Vista Direct Send socket send error number " << errnoMsg << endmsg;
         else
            DataLog(log_level_trima_broadcast_info) << "Vista Direct Send sent (" << result << "/" << TED_BroadcastMsgSize << ") bytes." << endmsg;
      }
   }
}



//
// PackageMessage
//
void TrimaBroadcast::PackageMessage (TED_BroadcastMsg& msg)
{
   string         temp;
   vector<string> biometricList;


   // Fill in the Broadcast Message.
   msg.broadcastCount = 0;

   char portNumber[TED_PORT_NUMBER_SIZE];

   sprintf(portNumber, "%d", TrimaInfo::broadcastPort());

   strncpy(msg.trimaSerialNumber, TrimaInfo::serialNumber(),     TED_SERIAL_NUMBER_SIZE);
   strncpy(msg.trimaRelease,      TrimaInfo::releaseNumber(),    TED_RELEASE_SIZE);
   strncpy(msg.trimaBuild,        TrimaInfo::buildNumber(),      TED_BUILD_SIZE);
   strncpy(msg.trimaCRC,          TrimaInfo::trimaCrc(),         TED_CRC_SIZE);
   strncpy(msg.trimaHWRev,        TrimaInfo::hwRevision(),       TED_HW_REV_SIZE);
   strncpy(msg.trimaIP,           TrimaInfo::ipAddress(),        TED_IP_ADDRESS_SIZE);
   strncpy(msg.trimaPort,         portNumber,                    TED_PORT_NUMBER_SIZE);
   strncpy(msg.trimaLogFile,      TrimaInfo::logFileNameShort(), TED_LOG_FILENAME_SIZE);
   strncpy(msg.sequenceNumber,    TrimaInfo::sequenceNumber(),   TED_SEQUENCE_NUMBER_SIZE);
   strncpy(msg.vipifVersionNumber, VISTA_IF_VERSION_NUMBER,      TED_VIPIF_VERSION_SIZE);

   strcpy(msg.tedifVersionNumber, TED_IF_VERSION_NUMBER);

   msg.biometricDeviceList = TED_BIO_SIGNATURE_PIN;

#if 0
   trima.GetBiometricDeviceList(biometricList);
   if (biometricList.size() != 0)
   {
      // Add biometric enums to list
      // TBD.  Add new ID's as developed.
   }
#endif

   // Initialize the header
   msg.hdr.som    = 0;
   msg.hdr.msgId  = TED_BROADCAST;
   msg.hdr.length = TED_BroadcastMsgSize;
}



//
// Trima Broadcast process
//
extern "C" void trima_broadcast (void)
{
   MessageSystem ms;
   ms.initBlocking("Trima Broadcast");

   TrimaBroadcast tb(&ms);

   tb.Initialize();

   tb.StartBroadcast();

   ms.dispatchMessages();
}

/* FORMAT HASH ee2f198fccf84f9824c0f71b44cd30f9 */
