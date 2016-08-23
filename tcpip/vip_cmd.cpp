/****************************************************************************
*
* Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
*
* TITLE:      vip.cpp
*
* AUTHOR:     Regis McGarry
*
*
***************************************************************************/
#include <vxworks.h>          // Needed for some system include files
#include <stat.h>             // File stat routines
#include <errnoLib.h>
#include <unistd.h>           // needed for close
#include <iolib.h>            // needed for select



#include "vip_cmd.h"
#include "vip_common.h"       // VIP Helper Routines

#include "trima_datalog.h"    // Datalog Levels
#include "messagesystem.h"
#include "trimamessages.h"    // Gets Common Int messages

#include "portnumbers.h"      // To get Vista listen port number.
#include "trima_time.h"       // Contains set_time command.
#include "predict_cds.h"      // For Donor Time
#include "filenames.h"        // System file names
#include "scan_structure.h"   // Contains Barcode Feedback Msg.
#include "bio_signatures.h"   // Biometric stuff
#include "tedif.h"            // Biometric ID's.
#include "xif_files.h"        // Utility File IO
#include "CatalogChecker.h"   // Catalog list message
#include "procstate.h"        // Get proc and recovery state defns
#include "trima_assert.h"

#include "software_cds.h"     // Get Optional sw.dat settings



// Default socket timeout value of 30 seconds.
#define DEFAULT_TIMEOUT 30

#define VIP_MAX_REPLY_BUFFER_SIZE 4095  // 4K



VipCmdProcess::VipCmdProcess()
   : _vipshm(ROLE_RW),
     _addr((unsigned short)Trima_VistaListenPort),     // Bind to AF_INET,INADDR_ANY,7010
     _vistaSocket(0),
     _replyBuffer(new char[VIP_MAX_REPLY_BUFFER_SIZE]),
     _shutdown(false)
{
   _timeoutValue.tv_sec  = DEFAULT_TIMEOUT;
   _timeoutValue.tv_usec = 0;

   _listenSocket         = socket(AF_INET, SOCK_STREAM, 0);
}



//
// Class Destructor
//
VipCmdProcess::~VipCmdProcess (void)
{
   delete[] _replyBuffer;
}



//
// This routine initializes the class instance.
//
ALARM_VALUES VipCmdProcess::Init ()
{
   // Bind socket to the Vista listen address
   int result = bind(_listenSocket, _addr.addr(), _addr.size());

   if (result < 0)
   {
      DataLog(log_level_vip_error) << "Error on bind" << endmsg;

      return VIP_CANT_BIND_SOCKET;
   }

   // Set the socket to the Listen state (accept new connections).
   // Only accept one socket connection at a time.
   result = listen(_listenSocket, 1);

   if (result == -1)
   {
      DataLog(log_level_vip_error) << "Listen Failed (" << errnoMsg << ")" << endmsg;

      return VIP_CANT_LISTEN_ON_SOCKET;
   }

   //
   // Sanity tests to ensure that external interface and internal interface constants
   // are consistent
   //
   TRIMA_STATIC_ASSERT(VIP_CATALOG_LIST_SIZE         == MAX_CATALOG_ITEMS);
   TRIMA_STATIC_ASSERT(VIP_MAX_VISTA_PROCEDURES      == NUM_VISTA_PROCEDURES);
   TRIMA_STATIC_ASSERT(VIP_CATALOG_BARCODE_SIZE      == TUBING_SET_BARCODE_SIZE);
   TRIMA_STATIC_ASSERT(VIP_BARCODE_MAX_REASON_LENGTH == MAX_RESPONSE_LENGTH);
   TRIMA_STATIC_ASSERT(VIP_MAX_SUBSTATE_LENGTH       == NAME_SIZE);
   TRIMA_STATIC_ASSERT(VIP_MAX_RECOVERYSTATE_LENGTH  == NAME_SIZE);

   // TRIMA_STATIC_ASSERT(VIP_BIO_MAX_SIGNATURES   == BIO_MAX_PIN_LIST);  // This is not required anymore.  See IT 10670
   DataLog_Level log_level = log_level_vip_info;

   DataLog(log_level) << "Vista Command Messages" << endmsg;
   DataLog(log_level) << "Sizeof setLocalTimeMsg is " << setLocalTimeMsgSize << endmsg;
   DataLog(log_level) << "Sizeof startPeriodicStatusMsg is " << startPeriodicStatusMsgSize << endmsg;
   DataLog(log_level) << "Sizeof setOfflineConfigurationMsg is " << setOfflineConfigurationMsgSize << endmsg;
   DataLog(log_level) << "Sizeof requestFileListMsg is " << requestFileListMsgSize << endmsg;
   DataLog(log_level) << "Sizeof requestFileListBySeqNumMsg is " << requestFileListBySeqNumMsgSize << endmsg;
   DataLog(log_level) << "Sizeof uploadFileMsg is " << uploadFileMsgSize << endmsg;
   DataLog(log_level) << "Sizeof donorInfoMsg is " << donorInfoMsgSize << endmsg;
   DataLog(log_level) << "Sizeof barcodeFeedbackMsg is " << barcodeFeedbackMsgSize << endmsg;
   DataLog(log_level) << "Sizeof getinterfaceStateMsg is " << getInterfaceStateMsgSize << endmsg;
   DataLog(log_level) << "Sizeof InterfaceStateMsg is " << interfaceStateMsgSize << endmsg;
   DataLog(log_level) << "Sizeof startMachineStatusMsg is " << startMachineStatusMsgSize << endmsg;
   DataLog(log_level) << "Sizeof downloadBiometricSignaturesMsg is " << downloadBiometricSignaturesMsgSize << endmsg;
   DataLog(log_level) << "Sizeof biometricSignatureResponseMsg is " << biometricSignatureResponseMsgSize << endmsg;
   DataLog(log_level) << "Sizeof downloadCatalogListMsg is " << downloadCatalogListMsgSize << endmsg;
   DataLog(log_level) << "Sizeof getRegionalizationFeatureMsg is " << getRegionalizationFeatureMsgSize << endmsg;
   DataLog(log_level) << "Sizeof getRegionalizationFeatureResponseMsg is " << getRegionalizationFeatureResponseMsgSize << endmsg;
   DataLog(log_level) << "Sizeof getRegionalizationConstantMsg is " << getRegionalizationConstantMsgSize << endmsg;
   DataLog(log_level) << "Sizeof getRegionalizationConstantResponseMsg is " << getRegionalizationConstantResponseMsgSize << endmsg;
   DataLog(log_level) << "Sizeof getRegionalizationConfirmationCodeMsg is " << getRegionalizationConfirmationCodeMsgSize << endmsg;
   DataLog(log_level) << "Sizeof getRegionalizationConfirmationCodeResponseMsg is " << getRegionalizationConfirmationCodeResponseMsgSize << endmsg;

   DataLog(log_level) << "Vista Status Messages" << endmsg;
   DataLog(log_level) << "Sizeof barcodeContentsMsg is " << barcodeContentsMsgLength << endmsg;
   DataLog(log_level) << "Sizeof powerFailRecoverIndicationMsg is " << powerFailRecoveryIndicationMsgLength << endmsg;
   DataLog(log_level) << "Sizeof procedureParameterAdjustmentMsg is " << procedureParameterAdjMsgLength << endmsg;
   DataLog(log_level) << "Sizeof donorVitalsUpdatedMsg is " << donorVitalsUpdatedMsgLength << endmsg;
   DataLog(log_level) << "Sizeof endRunSummaryMsg is " << endRunSummaryMsgLength << endmsg;
   DataLog(log_level) << "Sizeof alarmMsg is " << alarmMsgLength << endmsg;
   DataLog(log_level) << "Sizeof alarmAckMsg is " << alarmAckMsgLength << endmsg;
   DataLog(log_level) << "Sizeof trimaPeriodicStatusMsg is " << trimaPeriodicStatusMsgLength << endmsg;
   DataLog(log_level) << "Sizeof donorCommittedMsg is " << donorCommittedMsgLength << endmsg;
   DataLog(log_level) << "Sizeof donorRejectedMsg is " << donorRejectedMsgLength << endmsg;
   DataLog(log_level) << "Sizeof operatorButtonPush is " << operatorButtonPushLength << endmsg;
   DataLog(log_level) << "Sizeof trimaMachineStatusMsg is " << trimaMachineStatusMsgLength << endmsg;
   DataLog(log_level) << "Sizeof biometricAuthorizationEventMsg is " << biometricAuthorizationEventMsgSize << endmsg;
   DataLog(log_level) << "Sizeof PinAuthorizationEventMsg is " << pinAuthorizationEventMsgSize << endmsg;
   DataLog(log_level) << "Sizeof biometricLogEventMsg is " << biometricLogEventMsgSize << endmsg;

   DataLog(log_level_vip_info) << "VIP CMD initialized.  Interface rev is: " << VISTA_IF_VERSION_NUMBER << endmsg;

   return NULL_ALARM;
}



//
// Cleanup routine for process.
//
void VipCmdProcess::Cleanup (int sig)
{
   // Set the shutdown flag
   _shutdown = true;

   // Block receives
   shutdown(_listenSocket, 0);
   close(_listenSocket);
   _listenSocket = 0;

   shutdown(_vistaSocket, 0);  // Block receives
   closeVistaCmdSocket(false);
}



//
// Main loop for handling socket connection to Vista
//
void VipCmdProcess::Process (void)
{
   static char RxBuffer[VIP_MAX_TRIMA_MSG_SIZE];
   const int   RxBufferSize = sizeof (RxBuffer);

   fd_set      readSockets;

   int         nbytes;
   int         bytesProcessed;
   bool        reconnect = true;

   //
   // Need a guard to ensure that the Vista Status process is ready to receive messages
   // before allowing Vista to connect.
   //
   // The rationale for keeping this close-ended is that a correct system should have
   // vip_sts started by now so this should only delay once, or twice at max.  If we
   // wait too long, then we will miss the broadcast message window.
   // Also, it will be very noticable to the operator that a Vista controlled Trima
   // didn't start up in Vista mode triggering a service call.
   //
   const struct timespec sleepTime = {1, 0};      // One second delay

   while (!_vipshm.vistaStatusReady.Get())
   {
      DataLog(log_level_vip_error) << "Waiting for Vista Status Process" << endmsg;
      nanosleep(&sleepTime, 0);
   }

   // Main Message Processing Loop.
   for (;; )
   {
      if (reconnect)
      {
         // Accept a new connection to Vista.
         // This call will block until successful.
         acceptConnection();

         if (_shutdown)
         {
            // Exit processing loop
            break;
         }
         else if (_vistaSocket == 0)
         {
            nanosleep(&sleepTime, 0);

            continue;
         }

         reconnect = false;

         // If connection happens before broadcast, set privilege.
         if (!_vipshm.broadcastComplete.Get())
         {
            _vipshm.vistaInControl.Set(true);

            DataLog(log_level_vip_info) << "Connection Made on socket " << _vistaSocket << ".  Vista is in control" << endmsg;
         }
      }

      // Add the command socket to the list.
      FD_ZERO(&readSockets);
      FD_SET (_vistaSocket, &readSockets);

      int status = select(FD_SETSIZE, &readSockets, 0, 0, &_timeoutValue);

      if (status == ERROR)  // select error
      {
         DataLog(log_level_vip_info) << "select error!  Closing socket.  Errno = " << errnoMsg << endmsg;

         closeVistaCmdSocket(true);

         reconnect = true;
      }
      else if (status == 0)  // Socket Timeout
      {
         DataLog(log_level_vip_info) << "Recv timeout!  Closing socket " << _vistaSocket << ".  Errno = " << errnoMsg << endmsg;

         int bytesAvailable;
         ioctl(_vistaSocket, FIONREAD, (int)&bytesAvailable);
         DataLog(log_level_vip_info) << "Number of bytes available = " << bytesAvailable << endmsg;

         closeVistaCmdSocket(true);

         reconnect = true;
      }
      else  // Socket Data
      {
         nbytes = recv(_vistaSocket, RxBuffer, RxBufferSize, 0);

         DataLog(log_level_vip_debug) << "Rx " << nbytes << " bytes" << endmsg;

         if (nbytes == 0)
         {
            closeVistaCmdSocket(true);

            reconnect = true;
            DataLog(log_level_vip_info) << "Vista initiated Socket Close.  Closing socket" << endmsg;
         }
         else if (nbytes < 0)
         {
            switch (errnoGet())
            {
               // Connection was reset.  Vista disconnected.
               case ECONNRESET :
                  DataLog(log_level_vip_info) << "Socket Connection to Vista lost!  Closing socket" << endmsg;

                  closeVistaCmdSocket(true);

                  reconnect = true;
                  break;

               case EINTR :
                  DataLog(log_level_vip_info) << "Recv interrupted by signal!" << endmsg;
                  break;

               case EWOULDBLOCK :
                  DataLog(log_level_vip_info) << "Recv timeout!  Closing socket" << endmsg;

                  closeVistaCmdSocket(true);
                  reconnect = true;
                  break;

               default :
                  DataLog(log_level_vip_error) << "Bad byte count from recv(" << nbytes
                                               << ").  Closing socket.  Errno = " << errnoMsg << endmsg;

                  closeVistaCmdSocket(true);

                  reconnect = true;
                  break;
            }
         }
         else
         {
            // Message received.  Parse it and process it.
            bytesProcessed = ProcessMessage (RxBuffer, nbytes);

            while (bytesProcessed > 0)
            {
               bytesProcessed = ProcessMessage (NULL, 0);
            }
         }
      }
   }
}



//
// Sets up the listening socket to Vista.
// Returns the socket handle of the first connection.
//
void VipCmdProcess::acceptConnection (void)
{
   char      socketOption     = 1;
   const int socketOptionSize = sizeof(socketOption);


   // const struct linger lingerOption = {1, 0};
   // const int lingerOptionSize = sizeof(lingerOption);

   sockinetaddr newAddr;
   int          sockAddrSize = sizeof(struct sockaddr);


   // Mark Vista Disconneced in the VIP Shared Memory
   _vipshm.vistaConnected.Set(false);

   DataLog(log_level_vip_info) << "Waiting for Vista client connection" << endmsg;

   // Block waiting for a connection.
   // newSocket will contain the handle to Vista.
   _vistaSocket = accept(_listenSocket, newAddr.addr(), &sockAddrSize);

   if (_shutdown)
   {
      closeVistaCmdSocket(false);
   }

   // Set the value in the shared memory for the VIP STS process to read.
   _vipshm.replyToAddress.Set(newAddr.sin_addr.s_addr);

   // Set up the Socket options.
   if (!setsockopt(_vistaSocket, SOL_SOCKET, SO_KEEPALIVE, &socketOption, socketOptionSize))
   {
      DataLog(log_level_vip_error) << "Failed to set socket option SO_RCVTIMEO " << errnoMsg << endmsg;
   }

   // _vistaSocket.setopt(sockbuf::so_linger,    &lingerOption, lingerOptionSize);

   if (!setsockopt(_vistaSocket, SOL_SOCKET, SO_REUSEADDR, &socketOption, socketOptionSize))
   {
      DataLog(log_level_vip_error) << "Failed to set socket option SO_RCVTIMEO " << errnoMsg << endmsg;
   }

   DataLog(log_level_vip_info) << "Client IP address " << newAddr << endmsg;

   DataLog(log_level_vip_info) << "Connection Valid" << endmsg;

   // Mark Vista Connected in the VIP Shared Memory
   _vipshm.vistaConnected.Set(true);
   _vipshm.vistaHasConnected.Set(true);

   // Send message to GUI to update Vista connection icon
   VistaConnectionStatusMsg vistaConnectionStatusMsg(MessageBase::SEND_LOCAL);
   vistaConnectionStatusMsg.send(1);
}



// Close Vista Command soket (in error or shutdown conditions)
void VipCmdProcess::closeVistaCmdSocket (bool updatGUI)
{
   close(_vistaSocket);
   _vistaSocket = 0;

   if (updatGUI)     // Send a message to GUI

   {
      VistaConnectionStatusMsg vistaConnectionStatusMsg(MessageBase::SEND_LOCAL);
      vistaConnectionStatusMsg.send(0);
   }
}


//
// This routine handles the validation, execution and response
// of all the Commands sent by Vista.
//
// This routine returns the number of bytes processed.
//
int VipCmdProcess::ProcessMessage (char* msg, int sizeInBytes)
{
   static char*      saveBuffer        = NULL;
   static int        bufferSizeInBytes = 0;

   int               currentSizeInBytes;
   int               processedSizeInBytes;

   trimaVistaHeader* hdr = 0;

   if (sizeInBytes == 0)
   {
      if (bufferSizeInBytes == 0)
      {
         DataLog(log_level_vip_debug) << "Buffer Empty" << endmsg;

         return 0;
      }
      else
      {
         // Try to get a whole message out of the saved buffer.
         hdr                = (trimaVistaHeader*)saveBuffer;
         currentSizeInBytes = bufferSizeInBytes;
      }
   }
   else if (bufferSizeInBytes > 0)
   {
      DataLog(log_level_vip_debug) << "Adding (" << sizeInBytes
                                   << ") bytes to Current Buffer (" << bufferSizeInBytes
                                   << ")" << endmsg;

      // Reallocate the new buffer with a larger size.
      saveBuffer = (char*)realloc ((void*)saveBuffer, bufferSizeInBytes + sizeInBytes);

      if (saveBuffer == NULL)
      {
         DataLog(log_level_vip_error) << "realloc failed!" << endmsg;
         bufferSizeInBytes = 0;
         return 0;
      }

      // Holding a saved partial buffer.  Glue them together.
      memcpy (&saveBuffer[bufferSizeInBytes], msg, sizeInBytes);
      bufferSizeInBytes = bufferSizeInBytes + sizeInBytes;

      // Attach the hdr var to the saved buffer
      hdr                = (trimaVistaHeader*)saveBuffer;
      currentSizeInBytes = bufferSizeInBytes;
   }
   else
   {
      // Attach the hdr var to the rx stream buffer
      hdr                = (trimaVistaHeader*)msg;
      currentSizeInBytes = sizeInBytes;
   }

   int result = _vipif.validHeader (*hdr, currentSizeInBytes);

   // A non-zero return value indicates an error.
   switch (result)
   {
      case 0 :
         // Valid Message.  Process it.
         DataLog(log_level_vip_debug) << "Valid Msg" << endmsg;
         break;

      case VIP_BUFFER_UNDERRUN :
         DataLog(log_level_vip_debug) << "Buffer Underrun (" << sizeInBytes
                                      << ", " << bufferSizeInBytes << ")" << endmsg;

         if (bufferSizeInBytes == 0 && saveBuffer == NULL)
         {
            // Create new Save buffer for existing packet.
            saveBuffer = (char*)malloc (sizeInBytes);
            if (saveBuffer != NULL)
            {
               memcpy (saveBuffer, msg, sizeInBytes);
               bufferSizeInBytes = sizeInBytes;
            }
            else
            {
               DataLog(log_level_vip_error) << "malloc() failed!" << endmsg;
            }
         }

         return 0;

      case VIP_BUFFER_OVERRUN :
         // Process existing message and save left over for next time.
         DataLog(log_level_vip_debug) << "Buffer Overrun (" << sizeInBytes << ", "
                                      << bufferSizeInBytes << ")" << endmsg;

         DataLog(log_level_vip_debug) << _vipif.errorString() << endmsg;
         break;

      case VIP_HEADER_CRC_INVALID :
      {
         // Add parse loop that scans buffer looking for SOM.
         DataLog(log_level_vip_error) << "Bad CRC (" << sizeInBytes << ", " << bufferSizeInBytes << ")" << endmsg;
         DataLog(log_level_vip_error) << _vipif.errorString() << endmsg;
         DataLog(log_level_vip_error) << *hdr << endmsg;

         trimaVistaHeader reply = *hdr;

         _vipif.prepareMsg(&reply, -1, -1, result);
         Send (&reply, trimaVistaHeaderLength);

         // Clear the Saved Buffer
         bufferSizeInBytes = 0;

         if (saveBuffer != 0)
         {
            free (saveBuffer);
            saveBuffer = 0;
         }

         return 0;
      }

      case VIP_MESSAGE_ID_INVALID :
      {
         DataLog(log_level_vip_error) << _vipif.errorString() << endmsg;
         DataLog(log_level_vip_error) << *hdr << endmsg;

         trimaVistaHeader reply = *hdr;
         _vipif.prepareMsg(&reply, -1, hdr->sequenceNumber, result);
         Send (&reply, trimaVistaHeaderLength);

         // Clear the Saved Buffer
         bufferSizeInBytes = 0;

         if (saveBuffer != 0)
         {
            free (saveBuffer);
            saveBuffer = 0;
         }

         return 0;
      }

      default :
         DataLog(log_level_vip_error) << _vipif.errorString() << endmsg;
         DataLog(log_level_vip_error) << *hdr << endmsg;

         SendAck (*hdr, result);

         // Clear the Saved Buffer
         bufferSizeInBytes = 0;

         if (saveBuffer != 0)
         {
            free (saveBuffer);
            saveBuffer = 0;
         }

         return 0;
   }

   switch (hdr->messageType)
   {
      case VIP_SET_LOCAL_TIME :
         processedSizeInBytes = SetTrimaTime (*hdr, currentSizeInBytes);
         break;

      case VIP_START_PERIODIC_STATUS :
         processedSizeInBytes = StartPeriodicStatus (*hdr, currentSizeInBytes);
         break;

      case VIP_SET_OFFLINE_CONFIG :
         processedSizeInBytes = SetOfflineConfig (*hdr, currentSizeInBytes);
         break;

      case VIP_REQUEST_FILE_LIST :
         processedSizeInBytes = RequestFileList (*hdr, currentSizeInBytes);
         break;

      case VIP_REQUEST_FILE_LIST_BY_SN :
         processedSizeInBytes = RequestFileListBySequenceNumber (*hdr, currentSizeInBytes);
         break;

      case VIP_UPLOAD_FILE :
         processedSizeInBytes = UploadFile (*hdr, currentSizeInBytes);
         break;

      case VIP_DOWNLOAD_DONOR_INFO :
         processedSizeInBytes = DownloadDonorInfo (*hdr, currentSizeInBytes);
         break;

      case VIP_PING :
         processedSizeInBytes = Ping (*hdr, currentSizeInBytes);
         break;

      case VIP_BARCODE_RESPONSE :
         processedSizeInBytes = BarcodeResponse (*hdr, currentSizeInBytes);
         break;

      case VIP_INTERFACE_STATE :
         processedSizeInBytes = GetInterfaceState (*hdr, currentSizeInBytes);
         break;

      case VIP_START_MACHINE_STATUS :
         processedSizeInBytes = StartMachineStatus (*hdr, currentSizeInBytes);
         break;

      case VIP_DOWNLOAD_BIOMETRIC_SIGNATURES :
         processedSizeInBytes = DownloadBiometricSignatures(*hdr, currentSizeInBytes);
         break;

      case VIP_BIOMETRIC_SIGNATURE_RESPONSE :
         processedSizeInBytes = BiometricSignatureResponse(*hdr, currentSizeInBytes);
         break;

      case VIP_DOWNLOAD_CATALOG_INFO :
         processedSizeInBytes = DownloadCatalogList(*hdr, currentSizeInBytes);
         break;

      case VIP_REQUEST_OPT_SETTING_VALUES :
         processedSizeInBytes = GetOptionalSettingsValues(*hdr, currentSizeInBytes);
         break;

      case VIP_GET_REGIONALIZATION_FEATURE :
         processedSizeInBytes = GetRegionalizationFeature(*hdr, currentSizeInBytes);
         break;

      case VIP_GET_REGIONALIZATION_CONSTANT :
         processedSizeInBytes = GetRegionalizationConstant(*hdr, currentSizeInBytes);
         break;

      case VIP_GET_REGIONALIZATION_CONFIRMATION_CODE :
         processedSizeInBytes = GetRegionalizationConfirmationCode(*hdr, currentSizeInBytes);
         break;

      case VIP_SET_REGIONALIZATION_FEATURE :
         processedSizeInBytes = SetRegionalizationFeature(*hdr, currentSizeInBytes);
         break;

      default :
         DataLog(log_level_vip_error) << "Unsupported Command ID(" << hdr->messageType << ")" << endmsg;

         processedSizeInBytes = currentSizeInBytes;
         break;
   }

   if (processedSizeInBytes < currentSizeInBytes)
   {
      // If buffersize is 0, then move over the current buffer
      // otherwise, move down the current one.
      if (bufferSizeInBytes == 0)
      {
         int bytesToMove = currentSizeInBytes - processedSizeInBytes;

         DataLog(log_level_vip_debug) << "saving (" << bytesToMove << ") Bytes" << endmsg;

         // Create new Save buffer for remaining data.
         saveBuffer = (char*)malloc (bytesToMove);
         if (saveBuffer != NULL)
         {
            memcpy (saveBuffer, &msg[processedSizeInBytes], bytesToMove);
            bufferSizeInBytes = bytesToMove;
         }
         else
         {
            DataLog(log_level_vip_error) << "malloc() failed!" << endmsg;
         }
      }
      else
      {
         int bytesToMove = bufferSizeInBytes - processedSizeInBytes;
         DataLog(log_level_vip_debug) << "moving (" << bytesToMove << ") Bytes" << endmsg;

         // Left over data.  Store it.
         memcpy (&saveBuffer[0], &saveBuffer[processedSizeInBytes], bytesToMove);
         bufferSizeInBytes -= processedSizeInBytes;
      }
   }
   else if (bufferSizeInBytes)
   {
      // In this case, complete message has been processed.
      // If buffer is hanging around, remove it.
      DataLog(log_level_vip_debug) << "freeing buffer" << endmsg;

      bufferSizeInBytes = 0;
      free (saveBuffer);
      saveBuffer        = NULL;
   }

   return processedSizeInBytes;
}



//
// This routine processes the Set Trima Time command.
//
int VipCmdProcess::SetTrimaTime (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const setLocalTimeMsg& newTime = (setLocalTimeMsg&)hdr;

   // Validate message size.
   if ( (sizeInBytes        < setLocalTimeMsgSize) ||
        (hdr.lengthInBytes != setLocalTimeMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid SetTrimaTime Msg size. value=" << sizeInBytes
                                   << ". expected " << setLocalTimeMsgSize << endmsg;
      DataLog(log_level_vip_error) << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return setLocalTimeMsgSize;
   }

   // This command is privileged.
   if (!_vipshm.vistaInControl.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Set Time allowed, Vista not in control" << endmsg;

      SendAck (hdr, VIP_NOT_PRIVILEGED);

      return setLocalTimeMsgSize;
   }

   // Verify that PFR has completed.
   if (!_vipshm.pfrComplete.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Set Time allowed, PFR not complete" << endmsg;

      SendAck (hdr, VIP_PFR_NOT_COMPLETE);

      return setLocalTimeMsgSize;
   }

   // Check if messaging is restricted
   if (_vipshm.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_COMMITTED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Set Time allowed, Messages Restricted" << endmsg;

      SendAck (hdr, VIP_DONOR_DOWNLOADED);

      return setLocalTimeMsgSize;
   }

   // Send on the Set Time message
   DataLog(log_level_vip_info) << "Setting Trima time to " << ctime ((const long unsigned int*)&newTime.localTime)
                               << "(" << newTime.localTime << ")" << endmsg;

   trima_time::set(newTime.localTime);

   // Send Response packet back to Vista.
   SendAck (hdr, VIP_OK);

   return setLocalTimeMsgSize;
}



//
// This routine processes the Start Periodic Status command.
//
int VipCmdProcess::StartPeriodicStatus (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const startPeriodicStatusMsg& startStatus = (startPeriodicStatusMsg&)hdr;

   // Validate message size.
   if ( (sizeInBytes        <  startPeriodicStatusMsgSize) ||
        (hdr.lengthInBytes != startPeriodicStatusMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid StartPeriodicStatus Msg size. value=" << sizeInBytes
                                   << ". expected " << startPeriodicStatusMsgSize << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return startPeriodicStatusMsgSize;
   }


   // Validate the message contents
   if (startStatus.repeatPeriod < 500)
   {
      // Send a failed status back to Vista
      SendAck (hdr, VIP_BAD_VALUE);

      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid repeat period " << startStatus.repeatPeriod << endmsg;

      return startPeriodicStatusMsgSize;
   }

   if (startStatus.portNumber == 0)
   {
      // Send a failed status back to Vista
      SendAck (hdr, VIP_BAD_VALUE);

      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid port number " << startStatus.portNumber << endmsg;

      return startPeriodicStatusMsgSize;
   }

   // TBD.  Make constant out of magic number
   if ( (startStatus.receiveTimeout > 30) || (startStatus.receiveTimeout < 5) )
   {
      // Send a failed status back to Vista
      SendAck (hdr, VIP_BAD_VALUE);

      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid receive timeout, value=" << startStatus.receiveTimeout
                                   << ". expected 5-30" << endmsg;

      return startPeriodicStatusMsgSize;
   }

   // Set the socket timeout value.
   _timeoutValue.tv_sec  = startStatus.receiveTimeout;
   _timeoutValue.tv_usec = 0;

   // Message validated.  Forward Request on to the vip_sts process
   Message<startPeriodicStatusMsg> statusRequestMsg (MessageBase::SEND_LOCAL);
   statusRequestMsg.send(startStatus);

   DataLog(log_level_vip_info) << "Forwarding Trima Status Request Msg, Rpt interval " << startStatus.repeatPeriod
                               << ", Port # " << startStatus.portNumber
                               << ", timeout " << startStatus.receiveTimeout << endmsg;

   // Send Response packet back to Vista.
   SendAck (hdr, VIP_OK);

   return startPeriodicStatusMsgSize;
}



//
// This routine processes the Start Machine Status command.
//
int VipCmdProcess::StartMachineStatus (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const startMachineStatusMsg& startStatus = (startMachineStatusMsg&)hdr;


   // Validate message size.
   if ( (sizeInBytes        < startMachineStatusMsgSize) ||
        (hdr.lengthInBytes != startMachineStatusMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid StartMachineStatus Msg size. value=" << sizeInBytes
                                   << ". expected " << startMachineStatusMsgSize << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return startMachineStatusMsgSize;
   }


   // Validate the message contents
   if (startStatus.repeatPeriod < 500)
   {
      // Send a failed status back to Vista
      SendAck (hdr, VIP_BAD_VALUE);

      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid repeat period " << startStatus.repeatPeriod << endmsg;

      return startMachineStatusMsgSize;
   }

   if (startStatus.portNumber == 0)
   {
      // Send a failed status back to Vista
      SendAck (hdr, VIP_BAD_VALUE);

      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid port number " << startStatus.portNumber << endmsg;

      return startMachineStatusMsgSize;
   }

   // Message validated.  Forward Request on to the vip_sts process
   Message<startMachineStatusMsg> statusRequestMsg (MessageBase::SEND_LOCAL);
   statusRequestMsg.send(startStatus);

   DataLog(log_level_vip_info) << "Forwarding Machine Status Request Msg. Rpt " << startStatus.repeatPeriod
                               << ", Port " << startStatus.portNumber << endmsg;

   // Send Response packet back to Vista.
   SendAck (hdr, VIP_OK);

   return startMachineStatusMsgSize;
}



//
// This routine processes the Set Offline Configuration command.
//
int VipCmdProcess::SetOfflineConfig (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const setOfflineConfigurationMsg& newOfflineConfig = (setOfflineConfigurationMsg&)hdr;


   // Validate message size.
   if ( (sizeInBytes        < setOfflineConfigurationMsgSize) ||
        (hdr.lengthInBytes != setOfflineConfigurationMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid SetOfflineConfiguration Msg size. value=" << sizeInBytes
                                   << ". expected " << setOfflineConfigurationMsgSize << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return setOfflineConfigurationMsgSize;
   }

   // Verify that Vista is privileged.
   if (!_vipshm.vistaInControl.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Offline Config allowed, Vista not in control" << endmsg;

      SendAck (hdr, VIP_NOT_PRIVILEGED);

      return setOfflineConfigurationMsgSize;
   }

   // Verify that PFR has completed.
   if (!_vipshm.pfrComplete.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Offline Config allowed, PFR not complete" << endmsg;

      SendAck (hdr, VIP_PFR_NOT_COMPLETE);

      return setOfflineConfigurationMsgSize;
   }

   // Check if messaging is restricted
   if (_vipshm.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_COMMITTED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Offline Config allowed, Messages Restricted" << endmsg;

      SendAck (hdr, VIP_DONOR_DOWNLOADED);

      return setOfflineConfigurationMsgSize;
   }


   Config_CDS  _config(ROLE_RW);
   VistaConfig _vistaConfig(&_config);

   // Save a copy of the config in-case it is bad.
   _vistaConfig.Snapshot();


   // Change the Language Config.
   _vistaConfig.CopyLanguageSection(newOfflineConfig);

   // Change the Machine Config.
   _vistaConfig.CopyMachineSection(newOfflineConfig);

   // Change the Procedure Config.
   _vistaConfig.CopyProcedureSection(newOfflineConfig);

   // Change the Predict Config
   _vistaConfig.CopyPredictSection(newOfflineConfig);

   // Change the Product Templates Config
   if (!_vistaConfig.CopyProcedureTemplates(newOfflineConfig))
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "New Offline Configuration could not be validated.  Reason="
                                   << _vistaConfig.errorMsg() << endmsg;

      string str = _vistaConfig.errorMsg();

      SendAck (hdr, VIP_BAD_VALUE, str.c_str());

      // Roll back config
      _vistaConfig.Rollback();

      return setOfflineConfigurationMsgSize;
   }

   if (!_config.Valid())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "New Offline Configuration could not be validated" << endmsg;

      SendAck (hdr, VIP_BAD_VALUE, _config.errorMsg());

      // Roll back config
      _vistaConfig.Rollback();

      return setOfflineConfigurationMsgSize;
   }

   // If the new Offline Coniguration could not be committed.
   if (!_config.Write())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Could not commit Offline Configuration" << endmsg;

      SendAck (hdr, VIP_FAILED_TO_COMMIT);

      // Roll back config
      _vistaConfig.Rollback();

      return setOfflineConfigurationMsgSize;
   }

   // Send Response packet back to Vista.
   SendAck (hdr, VIP_OK);

   DataLog(log_level_vip_info) << "New Offline Configuration Stored" << endmsg;

   return setOfflineConfigurationMsgSize;
}



//
// This routine processes the Request File List command.
//
int VipCmdProcess::RequestFileList (const trimaVistaHeader& hdr, int sizeInBytes)
{
   requestFileListMsg& getFileList = (requestFileListMsg&)hdr;

   XIF_FileIo          fileIo(VIP_TEMPORARY_FILE, _vistaSocket, log_level_vip_info, log_level_vip_error);
   fileIo.turnOffDelay();

   unsigned long fileCrc = INITCRC_DEFAULT;
   int           fileSize;


   // Validate message size.
   if ( (sizeInBytes       <  requestFileListMsgSize) ||
        (hdr.lengthInBytes != requestFileListMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid RequestFileList Msg size. value=" << sizeInBytes
                                   << ". expected " << requestFileListMsgSize << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return requestFileListMsgSize;
   }

   int size = strlen(getFileList.pathName);

   if ( (size == 0) || (size > VIP_MAX_PATHNAME_LENGTH - 1) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid pathname length, value=" << size
                                   << ". expected range 1-" << VIP_MAX_PATHNAME_LENGTH - 1 << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return requestFileListMsgSize;
   }

   size = strlen(getFileList.pattern);

   if ( (size == 0) || (size > VIP_MAX_PATTERN_LENGTH - 1) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid pattern length, value=" << size
                                   << ". expected range 1-" << VIP_MAX_PATTERN_LENGTH - 1 << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return requestFileListMsgSize;
   }

   DataLog(log_level_vip_info) << "Requesting Files for directory " << getFileList.pathName <<
      ", pattern " << getFileList.pattern << endmsg;

   // Collect the list of files.
   XIF_FileIo::STATUS status = fileIo.getFileList(getFileList.pathName, getFileList.pattern, fileSize, fileCrc);

   switch (status)
   {
      case XIF_FileIo::SUCCESS :
         break;

      case XIF_FileIo::BAD_SEARCH_PATTERN :
         SendAck(hdr, VIP_ILLEGAL_FORMAT);
         return requestFileListMsgSize;
         break;

      case XIF_FileIo::BAD_DIR_NAME :
         SendAck(hdr, VIP_BAD_VALUE);
         return requestFileListMsgSize;
         break;

      case XIF_FileIo::BAD_TEMP_FILE :
         SendAck(hdr, VIP_FAILED_TO_COMMIT);
         return requestFileListMsgSize;
         break;

      default :
         DataLog(log_level_vip_error) << "Unexpected choice " << status << endmsg;

         SendAck(hdr, VIP_BAD_VALUE);
         return requestFileListMsgSize;
         break;
   }

   // If the file size is 0, return a blank reply
   if (fileSize == 0)
   {
      DataLog(log_level_vip_info) << "No files found in " << getFileList.pathName
                                  << ", pattern " << getFileList.pattern << ")" << endmsg;

      // Send Response packet back to the TRAP Server.
      SendAck (hdr, VIP_OK);

      return requestFileListMsgSize;
   }

   // Prepare the reply message header.
   // Since this reply will possibly be split over multiple socket sends, the file crc
   // needs to be computed before any individual packet is sent.

   trimaVistaHeader reply = hdr;

   _vipif.prepareMsg(&reply, reply.messageType, reply.sequenceNumber, VIP_OK);
   reply.lengthInBytes = fileSize;

   // Set the Body CRC value.
   reply.bodyCrc = fileCrc ^ INITCRC_DEFAULT;

   // Reset the CRC value.
   fileCrc = INITCRC_DEFAULT;

   // Recompute the Header CRC.
   if (crcgen32(&fileCrc, (unsigned char*)&reply, trimaVistaHeaderLength - sizeof(unsigned long)) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Could not generate crc for message header" << endmsg;

      SendAck (hdr, VIP_FAILED_TO_COMMIT);

      return requestFileListMsgSize;
   }

   reply.headerCrc = fileCrc ^ INITCRC_DEFAULT;

   // Send the header first.  Body follows. Easier that way.
   Send (&reply, trimaVistaHeaderLength);

   fileIo.sendFileList();

   DataLog(log_level_vip_info) << "File List Retrieved from " << getFileList.pathName << ", pattern " << getFileList.pattern << endmsg;

   return requestFileListMsgSize;
}



//
// This routine processes the Request File List By Sequence Number command.
//
int VipCmdProcess::RequestFileListBySequenceNumber (const trimaVistaHeader& hdr, int sizeInBytes)
{
   //  1T       DDDDD             _                 YYYYMMDD               _   DDD         _    DDDDDD                      .extension
   const char* logFilePattern = "^^1T[0-9][0-9][0-9][0-9][0-9]_[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]_[0-9][0-9][0-9]_[0-9][0-9][0-9][0-9][0-9][0-9]" LOG_FILE_EXTENSION;

   const requestFileListBySeqNumMsg& getFileListBySeqNum = (requestFileListBySeqNumMsg&)hdr;

   XIF_FileIo fileIo(VIP_TEMPORARY_FILE, _vistaSocket, log_level_vip_info, log_level_vip_error);
   fileIo.turnOffDelay();

   unsigned long fileCrc;
   int           fileSize;


   // Validate message size.
   if ( (sizeInBytes        <  requestFileListBySeqNumMsgSize) ||
        (hdr.lengthInBytes  != requestFileListBySeqNumMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid RequestFileListBySN Msg size. value=" << sizeInBytes
                                   << ". expected " << requestFileListBySeqNumMsgSize << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return requestFileListBySeqNumMsgSize;
   }

   // Check directory name size
   int size = strlen(getFileListBySeqNum.directoryName);

   if ( (size == 0) || (size > VIP_MAX_PATHNAME_LENGTH - 1) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid pathname length, value=" << size
                                   << ". expected range 1-" << VIP_MAX_PATHNAME_LENGTH - 1 << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return requestFileListBySeqNumMsgSize;
   }

   // Check sequence number range
   if ( (getFileListBySeqNum.sequenceNumber < 0) || (getFileListBySeqNum.sequenceNumber > VIP_MAX_SEQUENCE_NUMBER) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid sequence number, value=" << getFileListBySeqNum.sequenceNumber
                                   << ". expected range 0-" << VIP_MAX_SEQUENCE_NUMBER << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return requestFileListBySeqNumMsgSize;
   }

   DataLog(log_level_vip_info) << "Requesting Files for directory " << getFileListBySeqNum.directoryName <<
      ", Sequence Number " << getFileListBySeqNum.sequenceNumber << endmsg;

   // Get list of files
   // int status = RetrieveFileList (getFileListBySeqNum.directoryName, logFilePattern);
   XIF_FileIo::STATUS status = fileIo.getFileList(getFileListBySeqNum.directoryName, logFilePattern, fileSize, fileCrc);

   switch (status)
   {
      case XIF_FileIo::SUCCESS :
         break;

      case XIF_FileIo::BAD_SEARCH_PATTERN :
         SendAck(hdr, VIP_ILLEGAL_FORMAT);
         return requestFileListBySeqNumMsgSize;
         break;

      case XIF_FileIo::BAD_DIR_NAME :
         SendAck(hdr, VIP_BAD_VALUE);
         return requestFileListBySeqNumMsgSize;
         break;

      case XIF_FileIo::BAD_TEMP_FILE :
         SendAck(hdr, VIP_FAILED_TO_COMMIT);
         return requestFileListBySeqNumMsgSize;
         break;

      default :
         DataLog(log_level_vip_error) << "Unexpected choice " << status << endmsg;

         SendAck(hdr, VIP_BAD_VALUE);
         return requestFileListBySeqNumMsgSize;
         break;
   }

   // File size of 0 means no files found
   if (fileSize == 0)
   {
      DataLog(log_level_vip_info) << "No files found!" << endmsg;

      SendAck (hdr, VIP_OK);

      return requestFileListBySeqNumMsgSize;
   }

   // Initialize the File CRC
   fileCrc = INITCRC_DEFAULT;

   // Count the size of the number of matches in the returned stream.
   const int countSize = fileIo.countFileList(getFileListBySeqNum.sequenceNumber, fileCrc);

   // Check for failure.
   if (countSize == -1)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Could not open file " << VIP_TEMPORARY_FILE << endmsg;

      SendAck (hdr, VIP_FAILED_TO_COMMIT);

      return requestFileListBySeqNumMsgSize;
   }

   // Prepare the reply message header.
   // Since this reply will possibly be split over multiple socket sends, the file crc
   // needs to be computed before any individual packet is sent.
   trimaVistaHeader reply = hdr;

   _vipif.prepareMsg(&reply, reply.messageType, reply.sequenceNumber, VIP_OK);
   reply.lengthInBytes = countSize;

   // Set the Body CRC value.
   reply.bodyCrc = fileCrc ^ INITCRC_DEFAULT;

   // Reset the CRC value.
   fileCrc = INITCRC_DEFAULT;

   // Recompute the Header CRC.
   if (crcgen32(&fileCrc, (unsigned char*)&reply, trimaVistaHeaderLength - sizeof(unsigned long)) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Could not generate crc for message header" << endmsg;

      SendAck (hdr, VIP_FAILED_TO_COMMIT);

      return requestFileListBySeqNumMsgSize;
   }

   reply.headerCrc = fileCrc ^ INITCRC_DEFAULT;

   // Send the header first.  Body follows. Easier that way.
   Send (&reply, trimaVistaHeaderLength);

   // Send the body of the message
   fileIo.sendFileList(getFileListBySeqNum.sequenceNumber);

   DataLog(log_level_vip_info) << "File List By Seq Num " << getFileListBySeqNum.sequenceNumber
                               << " retrieved from " << getFileListBySeqNum.directoryName << endmsg;

   return requestFileListBySeqNumMsgSize;
}



//
// This routine processes the UploadFile command.
//
int VipCmdProcess::UploadFile (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const uploadFileMsg& uploadFile = (uploadFileMsg&)hdr;

   XIF_FileIo           fileIo(VIP_TEMPORARY_FILE, _vistaSocket, log_level_vip_info, log_level_vip_error);
   fileIo.turnOffDelay();

   unsigned long fileCrc = INITCRC_DEFAULT;
   struct stat   fileStat;
   int           nbytes;


   // Validate message size.
   if ( (sizeInBytes        <  uploadFileMsgSize) ||
        (hdr.lengthInBytes  != uploadFileMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid uploadFile Msg size. value=" << sizeInBytes
                                   << ". expected " << uploadFileMsgSize << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return uploadFileMsgSize;
   }

   // Get the file size
   if (stat ((char*)uploadFile.fileName, &fileStat) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Could not stat file " << uploadFile.fileName << endmsg;

      SendAck (hdr, VIP_ILLEGAL_FORMAT);

      return uploadFileMsgSize;
   }

   // If the file size is 0, return a blank reply
   if (fileStat.st_size == 0)
   {
      // Send Response packet back to Vista.
      SendAck (hdr, VIP_OK);

      return uploadFileMsgSize;
   }

   // Get the CRC of the file.
   // Note.  This should be OK if the file exists.
   if (file_crcgen32 (uploadFile.fileName, &fileCrc) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Could not generate crc for file " << uploadFile.fileName << endmsg;

      SendAck (hdr, VIP_ILLEGAL_FORMAT);

      return uploadFileMsgSize;
   }

   DataLog(log_level_vip_info) << "file " << uploadFile.fileName << " CRC " << fileCrc
                               << ", size " << fileStat.st_size << endmsg;

   // Prepare the reply message header.
   // Since this reply will possibly be split over multiple socket sends, the file crc
   // needs to be computed before any individual packet is sent.
   trimaVistaHeader reply = hdr;

   _vipif.prepareMsg(&reply, reply.messageType, reply.sequenceNumber, VIP_OK);
   reply.lengthInBytes = fileStat.st_size;

   // Set the Body CRC value.
   reply.bodyCrc = fileCrc ^ INITCRC_DEFAULT;

   // Reset the CRC value.
   fileCrc = INITCRC_DEFAULT;

   // Recompute the Header CRC.
   if (crcgen32(&fileCrc, (unsigned char*)&reply, trimaVistaHeaderLength - sizeof(unsigned long)) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Could not generate crc for file " << uploadFile.fileName << endmsg;

      SendAck (hdr, VIP_ILLEGAL_FORMAT);

      return uploadFileMsgSize;
   }

   reply.headerCrc = fileCrc ^ INITCRC_DEFAULT;

   // Send the header first.  Body follows. Easier that way.
   Send (&reply, trimaVistaHeaderLength);

   fileIo.sendFile(uploadFile.fileName);

   DataLog(log_level_vip_info) << "File Uploaded from " << uploadFile.fileName << endmsg;

   return uploadFileMsgSize;
}



//
// This routine processes the Download Donor Info command.
//
int VipCmdProcess::DownloadDonorInfo (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const donorInfoMsg& donorInfo = (donorInfoMsg&)hdr;

   DataLog(log_level_vip_info) << "Received Donor Info Msg" << endmsg;

   // Validate message size.
   if ( (sizeInBytes        <  donorInfoMsgSize) ||
        (hdr.lengthInBytes  != donorInfoMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid donorInfo Msg size. value=" << sizeInBytes
                                   << ". expected " << donorInfoMsgSize << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      trimaVistaHeader reply = hdr;

      SendAck (reply, VIP_BAD_LENGTH);

      return donorInfoMsgSize;
   }

   // Verify that Vista is privileged to execute this command.
   if (!_vipshm.vistaInControl.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Donor Download allowed, Vista not in control" << endmsg;

      SendAck (hdr, VIP_NOT_PRIVILEGED);

      return donorInfoMsgSize;
   }

   // Verify that PFR has completed.
   if (!_vipshm.pfrComplete.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Donor Download allowed, PFR not complete" << endmsg;

      SendAck (hdr, VIP_PFR_NOT_COMPLETE);

      return donorInfoMsgSize;
   }

   // Check if messaging is restricted
   if (_vipshm.restrictedAccess(XIF_DONOR_COMMITTED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Donor Download allowed, Messages Restricted" << endmsg;

      SendAck (hdr, VIP_DONOR_DOWNLOADED);

      return donorInfoMsgSize;
   }

   // Create a new donor message
   DVitalsFromVista donorMsg(MessageBase::SEND_LOCAL);

   // Create a donor instance
   CDonor donor;

   // Return a reference to the encapsulated donor vitals
   SDonorVitals& donorVitals = donor.Data();

   // Config.dat write/update message (on/off line).
   Config_CDS  _config(ROLE_RW);
   VistaConfig _vistaConfig(&_config);

   // Save a copy of the config in-case it is bad.
   _vistaConfig.Snapshot();

   // Process the Donor Vitals section.
   int status = _vistaConfig.ProcessDonorVitals (donor, donorInfo);

   DataLog(log_level_vip_info) << "Donor vitals received: " << donor.cData() << endmsg;

   if (status != 0)
   {
      DataLog(log_level_vip_error) << _vistaConfig.errorMsg() << endmsg;

      string str = _vistaConfig.errorMsg();

      SendAck (hdr, status, str.c_str());

      return donorInfoMsgSize;
   }

   DataLog(log_level_vip_info) << "Donor Vitals processed successfully" << endmsg;

   // Write the donor picture
   if (!writeDonorPicture(donorInfo.donorPicture, donorVitals.DonorDataCRC))
   {
      SendAck (hdr, VIP_FAILED_TO_COMMIT);

      return donorInfoMsgSize;
   }

   //
   // Validate the Max Donor Time
   //
   if (!_vistaConfig.ValidMaxDonorTime(donorInfo.maximumDonorTime))
   {
      DataLog(log_level_vip_error) << _vistaConfig.errorMsg() << endmsg;

      string str = _vistaConfig.errorMsg();

      SendAck (hdr, VIP_BAD_VALUE, str.c_str());

      return donorInfoMsgSize;
   }

   //
   // Validate the Vista Procedures Section
   //
   status = _vistaConfig.ValidateVistaProcedureSection(donorInfo);

   if (status != 0)
   {
      DataLog(log_level_vip_error) << _vistaConfig.errorMsg() << endmsg;

      string str = _vistaConfig.errorMsg();

      SendAck (hdr, status, str.c_str());

      return donorInfoMsgSize;
   }

   DataLog(log_level_vip_info) << "Donor Procedure List processed successfully" << endmsg;

   //
   // Validation is complete.  Copy the Trima Config sections
   //
   _vistaConfig.CopyLanguageSection (donorInfo);
   _vistaConfig.CopyMachineSection (donorInfo);
   _vistaConfig.CopyProcedureSection (donorInfo);
   _vistaConfig.CopyPredictSection (donorInfo);
   _vistaConfig.CopyProcedureList (donorInfo);

   // Validate the config.dat section
   if (!_config.Valid())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "config.dat section of donorInfo could not be validated.  Error = "
                                   << _config.errorMsg() << endmsg;

      SendAck (hdr, VIP_BAD_VALUE, _config.errorMsg());

      // Roll back config
      _vistaConfig.Rollback();

      return donorInfoMsgSize;
   }

   DataLog(log_level_vip_info) << "Config Valid" << endmsg;

   //
   // Everything has been validated (finally), commit changes and send ack.
   //

   // Send the Donor Update Msg
   donorMsg.send(donorVitals);

   DataLog(log_level_vip_info) << "Donor Updated" << endmsg;

   // Update the Config.dat changes
   _config.SendUpdateMsg();

   DataLog(log_level_vip_info) << "Config written" << endmsg;

   // Send Predict Donor Time value.
   Predict_CDS _PredictCDS(ROLE_RW);
   _PredictCDS.maxDonorTime.Set(donorInfo.maximumDonorTime);

   // Mark the donor downloaded so another donor
   // cannot be written over the current one.
   _vipshm.donorDownloaded.Set(true);

   // Send Response packet back to Vista.
   SendAck (hdr, VIP_OK);

   return donorInfoMsgSize;
}



//
//
//
int VipCmdProcess::Ping (const trimaVistaHeader& hdr, int sizeInBytes)
{
   // Validate message size.  Message has no body and Header size already checked
   if (hdr.lengthInBytes != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid Ping Msg size. value=" << trimaVistaHeaderLength
                                   << ". expected " << sizeInBytes << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return trimaVistaHeaderLength;
   }

   // Send Response packet back to Vista.
   SendAck (hdr, VIP_OK);

   DataLog(log_level_vip_debug) << "Received Ping Msg" << endmsg;

   return trimaVistaHeaderLength;
}



//
// This routine processes the Barcode Feedback command.
//
int VipCmdProcess::BarcodeResponse (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const barcodeFeedbackMsg& barcodeFeedback = (barcodeFeedbackMsg&)hdr;

   struct BarcodeResponseBinaryRecord
   {
      DataLog_UINT32 scanCategory;
      DataLog_UINT32 scanSequence;
      DataLog_UINT32 reasonCode;
      DataLog_UINT32 reasonStringLength;
      DataLog_UINT8  reasonString[VIP_BARCODE_MAX_REASON_LENGTH];
   };

   BarcodeResponseBinaryRecord binaryRecord;


   // Validate the feedback string length first
   if ( (barcodeFeedback.reasonStringLength < 0) ||
        (barcodeFeedback.reasonStringLength > VIP_BARCODE_MAX_REASON_LENGTH) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid Barcode Response Reason String length, value="
                                   << barcodeFeedback.reasonStringLength << ". expected 0.."
                                   << VIP_BARCODE_MAX_REASON_LENGTH << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      // No idea what right is.  Flush everything
      return sizeInBytes;
   }

   //
   // Compute the actual message size to be either the message plus the string size plus the
   // null terminator or just the base message size and no string.
   //
   const int msgSize = (barcodeFeedback.reasonStringLength > 0) ?
                       (barcodeFeedbackMsgSize + barcodeFeedback.reasonStringLength) :
                       barcodeFeedbackMsgSize;

   const int binaryRecordSize = (barcodeFeedback.reasonStringLength > 0) ?
                                (sizeof(DataLog_UINT32) * 4) + barcodeFeedback.reasonStringLength :
                                (sizeof(DataLog_UINT32) * 4);


   // Validate message size.
   if (sizeInBytes < msgSize)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid Barcode Response Msg size. value=" << sizeInBytes
                                   << ". expected " << msgSize << ".  String length is " << barcodeFeedback.reasonStringLength
                                   << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return msgSize;
   }

   // Verify that PFR has completed.
   if (!_vipshm.pfrComplete.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Barcode Feedback allowed, PFR not complete" << endmsg;

      SendAck (hdr, VIP_PFR_NOT_COMPLETE);

      return msgSize;
   }

   if ( (barcodeFeedback.scanCategory < VIP_SCAN_CATEGORY_FIRST) ||
        (barcodeFeedback.scanCategory > VIP_SCAN_CATEGORY_LAST) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid Barcode Scan Category, value=" << barcodeFeedback.scanCategory
                                   << ". expected " << VIP_SCAN_CATEGORY_FIRST << "-" << VIP_SCAN_CATEGORY_LAST << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return msgSize;
   }

   if ( (barcodeFeedback.reasonCode < VIP_BARCODE_FIRST) ||
        (barcodeFeedback.reasonCode > VIP_BARCODE_LAST) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid Barcode Reason Code, value=" << barcodeFeedback.reasonCode
                                   << ". expected " << VIP_BARCODE_FIRST << "-" << VIP_BARCODE_LAST << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return msgSize;
   }

   //
   // Translate External message into internal message
   //
   VISTA_BARCODE_FEEDBACK newFeedback;

   newFeedback.scan_category         = (SCAN_CATEGORY)barcodeFeedback.scanCategory;
   newFeedback.scan_sequence         = barcodeFeedback.scanSequence;
   newFeedback.vista_response_length = 0;
   newFeedback.vista_response[0]     = 0;

   // Stuff the binary record
   binaryRecord.scanCategory       = barcodeFeedback.scanCategory;
   binaryRecord.scanSequence       = barcodeFeedback.scanSequence;
   binaryRecord.reasonCode         = barcodeFeedback.reasonCode;
   binaryRecord.reasonStringLength = 0;
   binaryRecord.reasonString[0]    = 0;


   // Translate the external reason code into the internal one
   switch (barcodeFeedback.reasonCode)
   {
      case VIP_BARCODE_VALID :
         newFeedback.vista_status = BC_VISTA_OK;
         break;

      case VIP_BARCODE_INVALID :
         newFeedback.vista_status = BC_VISTA_INVALID;
         break;

      case VIP_BARCODE_WRONG_CATEGORY :
         newFeedback.vista_status = BC_VISTA_INCORRECT_CATEGORY;
         break;

      case VIP_BARCODE_PROCEDURE_DATA_CONSISTENCY :
         newFeedback.vista_status = BC_VISTA_INVALID_DATA;
         break;

      default :
         // Log event for later debugging.
         DataLog(log_level_vip_error) << "Invalid Barcode Reason Code, value=" << barcodeFeedback.reasonCode
                                      << ". expected " << VIP_BARCODE_VALID << "-"
                                      << VIP_BARCODE_PROCEDURE_DATA_CONSISTENCY << endmsg;

         SendAck (hdr, VIP_BAD_VALUE);

         return msgSize;
   }

   // If the barcode feedback is an error and there is a string, then copy it.
   if ( (barcodeFeedback.reasonCode != VIP_BARCODE_VALID) &&
        (barcodeFeedback.reasonStringLength > 0) )
   {
      newFeedback.vista_response_length = barcodeFeedback.reasonStringLength;
      strncpy(newFeedback.vista_response, barcodeFeedback.reasonString, VIP_BARCODE_MAX_REASON_LENGTH);

      binaryRecord.reasonStringLength = barcodeFeedback.reasonStringLength;
      strncpy((char*)binaryRecord.reasonString, barcodeFeedback.reasonString, VIP_BARCODE_MAX_REASON_LENGTH);
   }

   DataLog(log_level_vip_info) << "Barcode Response Sent Cat(" << newFeedback.scan_category
                               << ") Seq(" << newFeedback.scan_sequence << ") St("
                               << newFeedback.vista_status << ") Sz(" << newFeedback.vista_response_length << ")"
                               << endmsg;

   Message<VISTA_BARCODE_FEEDBACK> vistaBarcodeFeedback (MessageBase::SEND_LOCAL);
   vistaBarcodeFeedback.send(newFeedback);

   // Send Response packet back to Vista.
   SendAck (hdr, VIP_OK);

   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_BarcodeFeedbackSubType, &binaryRecord, binaryRecordSize);

   return msgSize;
}



//
// This routine processes the Get Interface State Msg
//
int VipCmdProcess::GetInterfaceState (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const getInterfaceStateMsg& getInterfaceState = (getInterfaceStateMsg&)hdr;


   // Validate message size.
   if ( (sizeInBytes       <  getInterfaceStateMsgSize) ||
        (hdr.lengthInBytes != getInterfaceStateMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid Get Interface State Msg size. value=" << sizeInBytes
                                   << ". expected " << getInterfaceStateMsgSize << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return getInterfaceStateMsgSize;
   }

   interfaceStateMsg interfaceState;

   interfaceState.hdr                    = hdr;
   interfaceState.privilegedState        = _vipshm.vistaInControl.Get();
   interfaceState.donorDownloaded        = _vipshm.donorDownloaded.Get();
   interfaceState.pfrStatus              = _vipshm.pfrComplete.Get();
   interfaceState.originalSequenceNumber = _vipshm.firstSequenceNumber.Get();

   // TBD.  Call yet unknown TED routine to generate this list when we support more than one.
   interfaceState.biometricConfig = TED_BIO_SIGNATURE_PIN;

   interfaceState.predictVersion  = TrimaInfo::predictVersion();

   // Send Response packet back to Vista.
   SendReply (&interfaceState, interfaceStateMsgSize);

   return getInterfaceStateMsgSize;
}



//
// This routine processes the Get Optional Settings Values Msg (which is now deprecated)
//
int VipCmdProcess::GetOptionalSettingsValues (const trimaVistaHeader& hdr, int sizeInBytes)
{
   // Validate message size.  Message has no body and header size has already been checked
   if (hdr.lengthInBytes < trimaVistaHeaderLength)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid Get Optional Settings Values Msg size. value=" << sizeInBytes << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      // Clear the buffer because we have no idea what the right size should be.
      return sizeInBytes;
   }

   // Send Response packet back to Vista.
   SendAck (hdr, VIP_DEPRECATED);

   // Not ideal as this may include part of another message but ....
   return sizeInBytes;
}



//
// This routine processes the Download Biometric Signatures file message
//
int VipCmdProcess::DownloadBiometricSignatures (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const downloadBiometricSignaturesMsg& biometricSignaturesMsg = (downloadBiometricSignaturesMsg&)hdr;

   int bytesProcessed = downloadBiometricSignaturesMsgSize - sizeof(int);   // Take away a byte for the char placeholder

   DataLog(log_level_vip_info) << "Got BIO Sig Msg.  Num(" << biometricSignaturesMsg.numberOfSignatures
                               << "), BIO ID(" << biometricSignaturesMsg.biometricDeviceId << ")" << endmsg;

   // Verify that Vista is privileged to execute this command.
   if (!_vipshm.vistaInControl.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Donor Download allowed, Vista not in control" << endmsg;

      SendAck (hdr, VIP_NOT_PRIVILEGED);

      return downloadBiometricSignaturesMsgSize;
   }

   // Verify that PFR has completed.
   if (!_vipshm.pfrComplete.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Donor Download allowed, PFR not complete" << endmsg;

      SendAck (hdr, VIP_PFR_NOT_COMPLETE);

      return downloadBiometricSignaturesMsgSize;
   }

   // Verify number of signatures doesn't exceed maximum
   if (biometricSignaturesMsg.numberOfSignatures > VIP_BIO_MAX_SIGNATURES)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Max number of signatures exceeded.  Received "
                                   << biometricSignaturesMsg.numberOfSignatures << ". expected 0-" << VIP_BIO_MAX_SIGNATURES
                                   << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return downloadBiometricSignaturesMsgSize;
   }

   switch (biometricSignaturesMsg.biometricDeviceId)
   {
      case TED_BIO_SIGNATURE_PIN :
      {
         unsigned int numberOfPins    = biometricSignaturesMsg.numberOfSignatures;
         bool         maxPinsExceeded = false;

         if (numberOfPins == 0)
         {
            // Erase biometric signatures file.
            ErasePinFileMsg msg(MessageBase::SEND_LOCAL);
            DataLog(log_level_vip_info) << "Received erase PIN barcode file msg" << endmsg;
            msg.send(1);
            break;
         }
         else
         {
            const char*         ptr          = (const char*)biometricSignaturesMsg.signatureSection;
            const char*         badgeBarcode = NULL;
            const char*         pin          = NULL;

            PinSignatureListMsg pinList;
            pinList.numberOfSignatures = 0;

            Message<PinSignatureListMsg> msg(MessageBase::SEND_LOCAL);
            VistaWritePinFileMsg         writePinFile(MessageBase::SEND_LOCAL);
            VistaNumberOfNewPinsMsg      numberOfNewPins(MessageBase::SEND_LOCAL);

            // IT 10670.  If number of signatures is greater than PIN file size, then issue warning
            if (numberOfPins > BIO_MAX_PIN_LIST)
            {
               DataLog(log_level_vip_info) << "number of signatures exceeds PIN file size.  Received "
                                           << numberOfPins << ". expected 0-" << BIO_MAX_PIN_LIST << endmsg;

               numberOfPins    = BIO_MAX_PIN_LIST;
               maxPinsExceeded = true;
            }

            // Inform GUI about the number of PINS to add
            numberOfNewPins.send(numberOfPins);

            for (int i = 0; i < numberOfPins; ++i)
            {
               badgeBarcode = ptr;
               int badgeBarcodeLength = strlen(badgeBarcode);

               if ( (badgeBarcodeLength == 0) || (badgeBarcodeLength > VIP_BIO_MAX_BADGE_LENGTH) )
               {
                  // Generate error.
                  DataLog(log_level_vip_error) << "Invalid barcode string length " << badgeBarcodeLength << " at entry " << i
                                               << ".  Expected 0-" << VIP_BIO_MAX_BADGE_LENGTH << endmsg;
                  DataLog(log_level_vip_error) << "Offending barcode was: " << badgeBarcode << endmsg;

                  SendAck (hdr, VIP_ILLEGAL_FORMAT);

                  writePinFile.send(0);
                  return downloadBiometricSignaturesMsgSize;
               }

               pin = ptr + badgeBarcodeLength + 1;
               int pinLength = strlen(pin);

               if ( (pinLength == 0) || (pinLength > VIP_BIO_MAX_PIN_LENGTH) )
               {
                  // Generate error.
                  DataLog(log_level_vip_error) << "Invalid pin string length " << pinLength << " at entry " << i
                                               << ".  Expected 0-" << VIP_BIO_MAX_PIN_LENGTH << endmsg;

                  SendAck (hdr, VIP_ILLEGAL_FORMAT);
                  writePinFile.send(0);
                  return downloadBiometricSignaturesMsgSize;
               }

               // Copy barcode and pin
               strcpy(pinList.signatureList[pinList.numberOfSignatures].badge, badgeBarcode);
               strcpy(pinList.signatureList[pinList.numberOfSignatures].pin, pin);

               // DataLog(log_level_vip_info) << "Just added [" << pinList.numberOfSignatures << "] '" << pinList.signatureList[pinList.numberOfSignatures].badge << "'("
               //   << badgeBarcodeLength << "), '" << pinList.signatureList[pinList.numberOfSignatures].pin << "'(" << pinLength << ")" << endmsg;

               pinList.numberOfSignatures++;
               bytesProcessed += badgeBarcodeLength + pinLength + 2;

               // DataLog(log_level_vip_info) << "Bytes processed bumped to " << bytesProcessed << endmsg;

               // Send message if msg array is full
               if (pinList.numberOfSignatures == BIO_MAX_PIN_SIGNATURES)
               {
                  msg.send(pinList);

                  DataLog(log_level_vip_info) << "Sent " << pinList.numberOfSignatures << " to the GUI BioMgr" << endmsg;
                  pinList.numberOfSignatures = 0;
                  taskDelay (sysClkRateGet () / 2);   /* pause a 1/2 second */
               }

               // Increment the string ptr.
               ptr += badgeBarcodeLength + pinLength + 2;
            }

            // Send residual msg if any left over
            if (pinList.numberOfSignatures != 0)
            {
               msg.send(pinList);

               DataLog(log_level_vip_info) << "Sent " << pinList.numberOfSignatures << " to the GUI BioMgr" << endmsg;
            }

            // If we are shortening the number of PINs, then don't error out on final message check.
            if (maxPinsExceeded)
            {
               bytesProcessed = hdr.lengthInBytes + trimaVistaHeaderLength;
            }

            DataLog(log_level_vip_info) << "Sent " << numberOfPins << " PIN Biometric entries" << endmsg;

            writePinFile.send(0);
         }
      }
      break;

      default :
         // Log event for later debugging.
         DataLog(log_level_vip_error) << "Invalid BIO Device ID (" << biometricSignaturesMsg.biometricDeviceId
                                      << ") in Download Biometric Signatures Msg" << endmsg;

         SendAck (hdr, VIP_BAD_VALUE);

         return downloadBiometricSignaturesMsgSize;
   }

   if (bytesProcessed != hdr.lengthInBytes + trimaVistaHeaderLength)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid Biometric Signatures file Msg size. value=" << hdr.lengthInBytes + trimaVistaHeaderLength
                                   << ". expected " << bytesProcessed << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      const char* ptr = (const char*)biometricSignaturesMsg.signatureSection;

      for (int i = 0; i < hdr.lengthInBytes; i++)
      {
         DataLog(log_level_vip_error) << ptr[i];
      }

      DataLog(log_level_vip_error) << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return bytesProcessed;
   }

   DataLog(log_level_vip_info) << "Procesed  " << bytesProcessed << " bytes.  Message size is "
                               << sizeInBytes << endmsg;

   // Send Response packet back to Vista.
   SendAck (hdr, VIP_OK);

   return bytesProcessed;
}



//
// This routine processes the Biometric Signature Response message
//
int VipCmdProcess::BiometricSignatureResponse (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const biometricSignatureResponseMsg& signatureResponse = (biometricSignatureResponseMsg&)hdr;
   const int msgSize = hdr.lengthInBytes + trimaVistaHeaderLength;

   DataLog(log_level_vip_info) << "biometricSignatureResponseMsgSize is " << biometricSignatureResponseMsgSize << endmsg;

   // Verify that PFR has completed.
   if (!_vipshm.pfrComplete.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Biometric Signature Response allowed, PFR not complete" << endmsg;

      SendAck (hdr, VIP_PFR_NOT_COMPLETE);

      return msgSize;
   }

   int returnBufferSize;

   //
   // Handle the message based on the signature type.
   //
   switch (signatureResponse.biometricDeviceId)
   {
      case VIP_BIO_SIGNATURE_PIN :
         returnBufferSize = SendPinResponse(sizeInBytes, signatureResponse);
         break;

      default :
         // Log event for later debugging.
         DataLog(log_level_vip_error) << "Bad Bio Dev ID ("
                                      << signatureResponse.biometricDeviceId
                                      << ") in Biometric Signature Response msg.  RN = "
                                      << signatureResponse.scanSequenceNumber
                                      << ", Response = " << signatureResponse.response << endmsg;

         SendAck (hdr, VIP_BAD_VALUE);

         return msgSize;
   }

   return returnBufferSize;
}



//
// This routine processes the Download Catalog List command
//
int VipCmdProcess::DownloadCatalogList (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const downloadCatalogListMsg& downloadCatalogList = (downloadCatalogListMsg&)hdr;

   // Validate message size.
   if ( (sizeInBytes        <  downloadCatalogListMsgSize) ||
        (hdr.lengthInBytes  != downloadCatalogListMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid downloadCatalogList Msg size. value=" << sizeInBytes
                                   << ". expected " << downloadCatalogListMsgSize << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return downloadCatalogListMsgSize;
   }

   // Verify that Vista is privileged.
   if (!_vipshm.vistaInControl.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Catalog Lists allowed, Vista not in control" << endmsg;

      SendAck (hdr, VIP_NOT_PRIVILEGED);

      return downloadCatalogListMsgSize;
   }

   // Verify that PFR has completed.
   if (!_vipshm.pfrComplete.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Catalog Lists allowed, PFR not complete" << endmsg;

      SendAck (hdr, VIP_PFR_NOT_COMPLETE);

      return downloadCatalogListMsgSize;
   }

   // Check if messaging is restricted.
   if (_vipshm.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_COMMITTED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Catalog Lists allowed, Messages Restricted" << endmsg;

      SendAck (hdr, VIP_DONOR_DOWNLOADED);

      return downloadCatalogListMsgSize;
   }

   if ( (downloadCatalogList.numberOfItems < 1) ||
        (downloadCatalogList.numberOfItems > VIP_CATALOG_LIST_SIZE)
        )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid number of items.  Expected 1-"
                                   << VIP_CATALOG_LIST_SIZE << ".  Got " << downloadCatalogList.numberOfItems << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return downloadCatalogListMsgSize;
   }

   Message<VistaCatalogList> msg(MessageBase::SEND_LOCAL);
   VistaCatalogList          list;


   // Send the catalog list
   for (int i = 0; i < downloadCatalogList.numberOfItems; i++)
   {
      const int barcodeSize = strlen(downloadCatalogList.itemList[i].barcode);

      // Check for barcodes that are incorrect size
      if ( (barcodeSize <= 0) ||
           (barcodeSize > VIP_CATALOG_BARCODE_SIZE)
           )
      {
         // Log event for later debugging.
         DataLog(log_level_vip_error) << "Catalog entry " << i << " has invalid barcode size.  Got "
                                      << downloadCatalogList.numberOfItems << ".  Expected 1-" << VIP_CATALOG_BARCODE_SIZE
                                      << endmsg;

         SendAck (hdr, VIP_ILLEGAL_FORMAT);

         return downloadCatalogListMsgSize;
      }

      CatalogChecker catalog(downloadCatalogList.itemList[i].catalogNumber, downloadCatalogList.itemList[i].tubingSetCode);

      // Check for incorrect catalog numbers and tubing set codes
      if (!catalog.isValid() ||
          !catalog.isBarcodeValid(downloadCatalogList.itemList[i].barcode))
      {
         // Log event for later debugging.
         DataLog(log_level_vip_error) << "Catalog entry " << i << " has invalid entry.  "
                                      << "catalog number is " << downloadCatalogList.itemList[i].catalogNumber
                                      << ". tubing set code is " << downloadCatalogList.itemList[i].tubingSetCode
                                      << ". barcode code is " << downloadCatalogList.itemList[i].barcode
                                      << ".  Catalog number is " << (catalog.isValid() ? "Valid" : "Invalid")
                                      << ".  Barcode is " << (catalog.isBarcodeValid(downloadCatalogList.itemList[i].barcode) ? "Valid" : "Invalid")
                                      << endmsg;

         SendAck (hdr, VIP_BAD_CATALOG_ENTRY);

         return downloadCatalogListMsgSize;
      }

      // Process valid entry
      list.catalogList[i].catalogNumber = downloadCatalogList.itemList[i].catalogNumber;
      list.catalogList[i].tubingSetCode = downloadCatalogList.itemList[i].tubingSetCode;
      strcpy(list.catalogList[i].barcode, downloadCatalogList.itemList[i].barcode);

      DataLog(log_level_vip_info) << "processed catalog entry "
                                  << list.catalogList[i].catalogNumber << ", "
                                  << list.catalogList[i].tubingSetCode << ", "
                                  << list.catalogList[i].barcode << ", "
                                  << endmsg;
   }

   list.permanent     = downloadCatalogList.permanent;
   list.numberOfItems = downloadCatalogList.numberOfItems;
   msg.send(list);

   // Only write out what is in the list instead of the entire struct.
   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_CatalogListSubType, &list.numberOfItems, (sizeof(VistaCatalogItem) * list.numberOfItems + sizeof(int)));

   DataLog(log_level_vip_info) << "Catalog Number list successfully processed "
                               << list.numberOfItems << " items.  List is "
                               << (list.permanent ? "permanent" : "not permanent") << endmsg;

   SendAck (hdr, VIP_OK);

   return downloadCatalogListMsgSize;
}





//
// This helper routine sends a PIN response to GUI
//
int VipCmdProcess::SendPinResponse (int sizeInBytes, const biometricSignatureResponseMsg& msg)
{
   const int                        msgSize = msg.hdr.lengthInBytes + trimaVistaHeaderLength;

   PinSignatureResponseMsg          pinResponse;
   Message<PinSignatureResponseMsg> responseMsg(MessageBase::SEND_LOCAL);


   //
   // Range check values
   //
   switch (msg.response)
   {
      case VIP_BIO_SIGNATURE_INVALID :
         pinResponse.action             = (BioSignatureResponse)msg.response;
         pinResponse.scanSequenceNumber = msg.scanSequenceNumber;
         pinResponse.signature.badge[0] = 0;
         pinResponse.signature.pin[0]   = 0;

         // Forward this message on to the Biometric Mgr on GUI.
         responseMsg.send(pinResponse);

         // Send Response packet back to Vista.
         SendAck (msg.hdr, VIP_OK);

         DataLog(log_level_vip_info) << "Sent Bio Sig Pin Response Invalid(" << pinResponse.scanSequenceNumber
                                     << ", " << pinResponse.action << ")" << endmsg;
         break;


      case VIP_BIO_SIGNATURE_VALID :
      case VIP_BIO_SIGNATURE_VALID_ADD :
      {
         pinResponse.action             = (BioSignatureResponse)msg.response;
         pinResponse.scanSequenceNumber = msg.scanSequenceNumber;

         const char* ptr                = msg.signatureSection;
         const char* badgeBarcode       = ptr;
         const char* pin                = ptr;

         const int   badgeBarcodeLength = strlen(badgeBarcode);

         if ( (badgeBarcodeLength == 0) || (badgeBarcodeLength > VIP_BIO_MAX_BADGE_LENGTH) )
         {
            // Generate error.
            DataLog(log_level_vip_error) << "Invalid barcode string length " << badgeBarcodeLength
                                         << ".  Expected 0-" << VIP_BIO_MAX_BADGE_LENGTH << endmsg;

            SendAck (msg.hdr, VIP_ILLEGAL_FORMAT);

            return msgSize;
         }

         pin += badgeBarcodeLength + 1;  // Get past the badge null
         int pinLength = strlen(pin);

         if ( (pinLength == 0) || (pinLength > VIP_BIO_MAX_PIN_LENGTH) )
         {
            // Generate error.
            DataLog(log_level_vip_error) << "Invalid pin string length " << pinLength
                                         << ".  Expected 0-" << VIP_BIO_MAX_PIN_LENGTH << endmsg;

            SendAck (msg.hdr, VIP_ILLEGAL_FORMAT);

            return msgSize;
         }

         // Copy barcode and pin
         strcpy(pinResponse.signature.badge, badgeBarcode);
         strcpy(pinResponse.signature.pin, pin);

         // Forward this message on to the Biometric Mgr on GUI.
         responseMsg.send(pinResponse);

         // Send Response packet back to Vista.
         SendAck (msg.hdr, VIP_OK);

         DataLog(log_level_vip_info) << "Sent Bio Sig Pin Response(" << pinResponse.scanSequenceNumber
                                     << ", " << pinResponse.action << ", " << badgeBarcode << ", " << pin << ")" << endmsg;
         break;
      }

      default :
         // Log event for later debugging.
         DataLog(log_level_vip_error) << "Bad resonse in Biometric Signature Response msg "
                                      << msg.response << endmsg;

         SendAck (msg.hdr, VIP_BAD_VALUE);

         return msgSize;
   }

   return msgSize;
}



//
// This routine processes the Get Regionalization Feature command
//
int VipCmdProcess::GetRegionalizationFeature (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const getRegionalizationFeatureMsg getFeatureMsg = (getRegionalizationFeatureMsg&)hdr;

   // Validate message size.
   if ( (sizeInBytes        <  getRegionalizationFeatureMsgSize) ||
        (hdr.lengthInBytes  != getRegionalizationFeatureMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid getRegionalizationFeature Msg size. value=" << sizeInBytes
                                   << ". expected " << getRegionalizationFeatureMsgSize << endmsg;
      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return getRegionalizationFeatureMsgSize;
   }


   if ( (getFeatureMsg.featureId < 0) ||
        (getFeatureMsg.featureId >= FEATURE_DEF_END) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid Feature ID. Expected 0-"
                                   << FEATURE_DEF_END - 1 << ".  Got "
                                   << getFeatureMsg.featureId << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return getRegionalizationFeatureMsgSize;
   }

   getRegionalizationFeatureResponseMsg response;

   response.hdr          = getFeatureMsg.hdr;
   response.featureId    = getFeatureMsg.featureId;
   response.featureValue = Software_CDS::GetInstance().getFeature((FEATURE_ID)getFeatureMsg.featureId);

   DataLog(log_level_vip_info) << "getRegionalizationFeature request for feature " << getFeatureMsg.featureId
                               << " returned value " << response.featureValue << endmsg;

   // Send Response packet back to Vista.
   SendReply (&response, getRegionalizationFeatureResponseMsgSize);

   return getRegionalizationFeatureMsgSize;
}



//
// This routine processes the Get Regionalization Constant command
//
int VipCmdProcess::GetRegionalizationConstant (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const getRegionalizationConstantMsg getConstantMsg = (getRegionalizationConstantMsg&)hdr;

   // Validate message size.
   if ( (sizeInBytes        <  getRegionalizationConstantMsgSize) ||
        (hdr.lengthInBytes  != getRegionalizationConstantMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid getRegionalizationConstant Msg size. value=" << sizeInBytes
                                   << ". expected " << getRegionalizationConstantMsgSize << endmsg;

      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return getRegionalizationConstantMsgSize;
   }


   if ( (getConstantMsg.constantId < 0) ||
        (getConstantMsg.constantId >= VALUE_FEATURE_DEF_END) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid Feature Constant ID. Expected 0-"
                                   << VALUE_FEATURE_DEF_END - 1 << ".  Got "
                                   << getConstantMsg.constantId << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return getRegionalizationConstantMsgSize;
   }

   getRegionalizationConstantResponseMsg response;

   response.hdr           = getConstantMsg.hdr;
   response.constantId    = getConstantMsg.constantId;
   response.constantValue = Software_CDS::GetInstance().getValueFeature((VALUE_FEATURE_ID)getConstantMsg.constantId);

   DataLog(log_level_vip_info) << "getRegionalizationConstant request for constant " << getConstantMsg.constantId
                               << " returned value " << response.constantValue << endmsg;

   // Send Response packet back to Vista.
   SendReply (&response, getRegionalizationConstantResponseMsgSize);

   return getRegionalizationConstantMsgSize;
}



//
// This routine processes the Get Regionalization Confirmation Code command
//
int VipCmdProcess::GetRegionalizationConfirmationCode (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const getRegionalizationConfirmationCodeMsg getConfirmationCodeMsg = (getRegionalizationConfirmationCodeMsg&)hdr;

   // Validate message size.
   if ( (sizeInBytes        <  getRegionalizationConfirmationCodeMsgSize) ||
        (hdr.lengthInBytes  != getRegionalizationConfirmationCodeMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid getRegionalizationConfirmationCode Msg size. value=" << sizeInBytes
                                   << ". expected " << getRegionalizationConfirmationCodeMsgSize << endmsg;

      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return getRegionalizationConfirmationCodeMsgSize;
   }


   getRegionalizationConfirmationCodeResponseMsg response;

   response.hdr = getConfirmationCodeMsg.hdr;

   string code = Software_CDS::GetInstance().getFullConfirmationCode();

   if (code.size() > VIP_REGIONALIZATION_KEY_SIZE)
   {
      DataLog(log_level_vip_error) << "Size of Confirmation code " << code.size()
                                   << "exceeds max interface value of " << VIP_REGIONALIZATION_KEY_SIZE << endmsg;

      SendAck (hdr, VIP_BAD_VALUE);

      return getRegionalizationConfirmationCodeMsgSize;
   }

   strncpy(response.confirmationCode, code.c_str(), code.size());
   response.confirmationCode[code.size()] = 0;

   DataLog(log_level_vip_info) << "getConfirmationCode request returned value " << code.c_str() << endmsg;

   // Send Response packet back to Vista.
   SendReply (&response, getRegionalizationConfirmationCodeResponseMsgSize);

   return getRegionalizationConfirmationCodeMsgSize;
}



//
// This routine processes the Set Regionalization Feature command
//
int VipCmdProcess::SetRegionalizationFeature (const trimaVistaHeader& hdr, int sizeInBytes)
{
   const setRegionalizationFeatureMsg setRegionalizationFeature = (setRegionalizationFeatureMsg&)hdr;

   // Validate message size.
   if ( (sizeInBytes        <  setRegionalizationFeatureMsgSize) ||
        (hdr.lengthInBytes  != setRegionalizationFeatureMsgSize - trimaVistaHeaderLength) )
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "Invalid setRegionalizationFeature Msg size. value=" << sizeInBytes
                                   << ". expected " << setRegionalizationFeatureMsgSize << endmsg;

      DataLog(log_level_vip_error) << "Header: " << hdr << endmsg;

      SendAck (hdr, VIP_BAD_LENGTH);

      return setRegionalizationFeatureMsgSize;
   }

   // Verify that Vista is privileged.
   if (!_vipshm.vistaInControl.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Regionalization Setting allowed, Vista not in control" << endmsg;

      SendAck (hdr, VIP_NOT_PRIVILEGED);

      return downloadCatalogListMsgSize;
   }

   // Verify that PFR has completed.
   if (!_vipshm.pfrComplete.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Regionalization Setting allowed, PFR not complete" << endmsg;

      SendAck (hdr, VIP_PFR_NOT_COMPLETE);

      return downloadCatalogListMsgSize;
   }

   // Check if messaging is restricted.
   if (_vipshm.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_COMMITTED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "No Catalog Lists allowed, Messages Restricted" << endmsg;

      SendAck (hdr, VIP_DONOR_DOWNLOADED);

      return downloadCatalogListMsgSize;
   }


   string           code(setRegionalizationFeature.featureCode);

   const KEY_RESULT result = Software_CDS::GetInstance().decodeKey(code, AUTH_SERVICE);      // Vista and Service have the same level of permission

   DataLog(log_level_vip_info) << "setRegionalizationFeature request with key " << setRegionalizationFeature.featureCode
                               << " returned value " << result << endmsg;

   switch (result)
   {
      case SUCCESS :
         SendAck (hdr, VIP_OK);
         break;

      case FAILED_UNAUTHORIZED_USER :
         SendAck (hdr, VIP_UNAUTHORIZED_USER);
         break;

      case FAILED_UNAUTHORIZED_COUNTRY :
      case FAILED_UNAUTHORIZED_CUSTOMER :
      case FAILED_UNAUTHORIZED_SERIAL_NUMBER :
      case FAILED_UNAUTHORIZED_SITE :
         SendAck (hdr, VIP_UNAUTHORIZED_MACHINE);
         break;

      case FAILED_BAD_KEY :
      default :
         SendAck (hdr, VIP_BAD_VALUE);
         break;
   }

   return setRegionalizationFeatureMsgSize;
}



//
// Send
//
void VipCmdProcess::Send (void* msg, int size)
{
   int nbytes;

   if ( (nbytes = send(_vistaSocket, (char*)msg, size, 0)) != size)
   {
      trimaVistaHeader* hdr = (trimaVistaHeader*)msg;

      DataLog(log_level_vip_error) << "Msg Id (" << hdr->messageType
                                   << ") Failed to send!(" << nbytes << ", " << size
                                   << ")" << endmsg;
   }
#if 0
   else
   {
      trimaVistaHeader* hdr = (trimaVistaHeader*)msg;

      DataLog(log_level_vip_debug) << *hdr << " send!(" << nbytes << ", " << size << ")" << endmsg;
   }
#endif
}



//
//  SendAck
//
void VipCmdProcess::SendAck (const trimaVistaHeader& hdr, int status, const char* msg)
{
   // If no error string to report, just send the header.
   if (msg == 0)
   {
      trimaVistaHeader reply = hdr;

      _vipif.prepareMsg (&reply, reply.messageType, reply.sequenceNumber, status);
      VipCmdProcess::Send (&reply, trimaVistaHeaderLength);
   }
   else
   {
      const unsigned int stringsize = strlen(msg) + 1;
      const unsigned int msgsize    = trimaVistaHeaderLength + stringsize;
      char*              buffer     = new char[msgsize];
      trimaVistaHeader*  reply      = (trimaVistaHeader*)buffer;

      *reply = hdr;
      memcpy(&buffer[trimaVistaHeaderLength], msg, stringsize);

      _vipif.prepareMsg (reply, reply->messageType, reply->sequenceNumber, status, msgsize);
      VipCmdProcess::Send (buffer, msgsize);

      delete[] buffer;
   }

}



//
//  SendReply
//
void VipCmdProcess::SendReply (void* hdr, int size)
{
   trimaVistaHeader* reply = (trimaVistaHeader*)hdr;

   _vipif.prepareMsg (hdr, reply->messageType, reply->sequenceNumber, VIP_OK, size);
   VipCmdProcess::Send (reply, size);
}



//
// This routine writes the donor bitmap to a file.
//
bool VipCmdProcess::writeDonorPicture (const unsigned char* bitmap, const unsigned long& crc)
{
   char donorFileName[80];


   // Store the Donor Picture
   // 1.  Extract the donor picture from the message.
   // 2.  Calculate a CRC from the picture.
   // 3.  Write picture to a file called <CRC Value>.BMP in the update dir.

   sprintf (donorFileName, "%s/%08lX.BMP", UPDATE_PATH, crc);

   FILE* outFile = fopen (donorFileName, "wb");

   if (outFile == 0)
   {
      DataLog(log_level_vip_error) << "Error creating donor image file " << donorFileName << ".  "
                                   << errnoMsg << endmsg;

      return false;
   }

   bool isOkay = true; // return value

   int  size   = fwrite (bitmap, 1, VIP_BITMAP_SIZE, outFile);

   if (size != VIP_BITMAP_SIZE)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "donorInfo: Error writing bitmap file " << donorFileName << "(" << size
                                   << ")" << endmsg;

      isOkay = false;
   }
   else if (fflush (outFile) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "donorInfo: Error flushing file " << donorFileName << ".  "
                                   << errnoMsg << endmsg;

      isOkay = false;
   }

   if (fclose (outFile) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_vip_error) << "donorInfo: Error closing file " << donorFileName << ".  "
                                   << errnoMsg << endmsg;

      isOkay = false;
   }

   if ( isOkay )
      DataLog(log_level_vip_info) << "Donor bitmap " << donorFileName << " written" << endmsg;

   return isOkay;
}



// define the main function
extern "C" void vip_cmd ()
{
   MessageSystem ms;

   ms.initNonBlock("vip_cmd");

   DataLog(log_level_vip_info) << "Creating vip_cmd process" << endmsg;

   VipCmdProcess vip_cmd;

   ALARM_VALUES  status = vip_cmd.Init();

   if (status == NULL_ALARM)
   {
      vip_cmd.Process();
   }
   else
   {
      anAlarmMsg initAlarm(status);
      initAlarm.setAlarm();
      DataLog(log_level_vip_error) << "S/W Alarm Set!" << endmsg;
   }

   // Cleanup the program.
   vip_cmd.Cleanup ();
}

/* FORMAT HASH 99de6cc98a531c12c646fd9e83745481 */
