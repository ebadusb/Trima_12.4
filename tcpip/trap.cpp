/****************************************************************************
*
* Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
*
* TITLE:      trap.cpp
*
* AUTHOR:     Regis McGarry
*
***************************************************************************/

#include <unistd.h>     // Needed for close and unlink
#include <sys/stat.h>

// VxWorks specific libs
#include "sockLib.h"  // Replaces socket, et. al.
#include <errnoLib.h>

#include "trap.h"

#include "xif_files.h"        // Get common external i/f file access routines

#include "crcgen.h"
#include "trima_info.h"       // Accesses Trima System Info (build number, etc).
#include "trima_datalog.h"    // ID for error logging

#include "filenames.h"        // Contains trap file names
#include "trima_time.h"       // For setting time

#include "units.h"            // For common units

#include "trap_dat.h"         // For trap file management


// Default socket timeout value of 30 seconds.
#define DEFAULT_TIMEOUT 30



static const struct timespec ONE_HOUR = {SECONDS_PER_HOUR, 0};



// Main entry point into program
extern "C" void trap_client ()
{
   TrapProcess trapClient;

   trapClient.Execute();
}




//
// define the main function
//
void TrapProcess::Execute (void)
{
   DataLog(log_level_trap_info) << "Trap Process Started...." << endmsg;

   if (Init())
   {
      do
      {
         Process();
      } while (_tryAgainLater);
   }
   else
   {
      DataLog(log_level_trap_error) << "Trap Init Failed.  Exiting..." << endmsg;
   }

   Cleanup ();
}



//
// Class Public Constructor
//
TrapProcess::TrapProcess()
   : _txSequenceNumber(0),
     _connectSocket(0),

// Initialize the config variables.
     _daysOfWeek(-1),
     _beginWindow(-1),
     _endWindow(-1),
     _maxRetries(-1),
     _maxTimeouts(-1),
     _feature(-1),

// These will get overridden by the dat file, but can't hurt
     _requestDelay(20),
     _controlIp(NULL),
     _controlPort(-1),
     _requestDelayInMilliSeconds(0),

     _timeWindow(NULL),
     _xifCds(ROLE_RW),
     _procTrimaSetCds(ROLE_RO),
     _tryAgainLater(false)


{

   osTime::snapshotRawTime(_lastRequestTime);
}



//
// Class Destructor
//
TrapProcess::~TrapProcess (void)
{
   delete _timeWindow;
}



//
// This helper routine opens and checks the TRAP_OVERRIDE file.
//
bool TrapProcess::OpenOverrideFile ()
{
   // Dat file was read during system startup, check the status of the customized
   // config file (if exists)
   const bool success = OverrideTrapDat::fileOK();

   // If operation failed, Halt operation and return.
   if (!success)
   {
      // Can't find override config file.  Try default config file.
      DataLog(log_level_trap_error) << "Can't open override config file " << OverrideTrapDat::filename() << endmsg;

      return false;
   }

   // Validate the CRC of the Override file.
   if (OverrideTrapDat::calculatedCrc() != OverrideTrapDat::overrideFileCrc())
   {
      // File CRC Doesn't match.  Log it.
      DataLog(log_level_trap_error) << OverrideTrapDat::filename() << " computed CRC doesn't match stored value ("
                                    << OverrideTrapDat::calculatedCrc() << "[" << hex << OverrideTrapDat::calculatedCrc() << dec << "]"
                                    << " != " << OverrideTrapDat::overrideFileCrc() << "[" << hex << OverrideTrapDat::overrideFileCrc() << dec << "]"
                                    << ").  Using Defaults file." << endmsg;

      return false;
   }

   // Update member vars from file read.
   _daysOfWeek                 = OverrideTrapDat::daysOfWeek();
   _beginWindow                = OverrideTrapDat::beginWindow();
   _endWindow                  = OverrideTrapDat::endWindow();
   _maxRetries                 = OverrideTrapDat::maxRetries();
   _maxTimeouts                = OverrideTrapDat::maxTimeouts();
   _feature                    = OverrideTrapDat::feature();
   _requestDelay               = OverrideTrapDat::requestDelay();

   _controlIp                  = OverrideTrapDat::controlIp();
   _controlPort                = OverrideTrapDat::controlPort();

   _requestDelayInMilliSeconds = OverrideTrapDat::requestDelayInMilliSeconds();

   // Validate the config entries.
   if (ConfigurationInvalid())
   {
      DataLog(log_level_trap_error) << "Override File Configuration Invalid." << endmsg;

      return false;
   }

   LogFileContents(OverrideTrapDat::filename());

   return true;
}




//
// This routine opens and checks the TRAP_DEFAULT file.
//
bool TrapProcess::OpenDefaultsFile ()
{
   // Read the customized config file (if exists)
   const bool success = DefaultTrapDat::fileOK();

   // If operation failed, Halt operation and return.
   if (!success)
   {
      // Can't find default config file.  Exit.
      DataLog(log_level_trap_error) << "Can't open defaults config file " << DefaultTrapDat::filename()
                                    << ".  EXITING." << endmsg;

      return false;
   }

   // Update member vars from file read.
   _daysOfWeek                 = DefaultTrapDat::daysOfWeek();
   _beginWindow                = DefaultTrapDat::beginWindow();
   _endWindow                  = DefaultTrapDat::endWindow();
   _maxRetries                 = DefaultTrapDat::maxRetries();
   _maxTimeouts                = DefaultTrapDat::maxTimeouts();
   _feature                    = DefaultTrapDat::feature();
   _requestDelay               = DefaultTrapDat::requestDelay();

   _controlIp                  = DefaultTrapDat::controlIp();
   _controlPort                = DefaultTrapDat::controlPort();

   _requestDelayInMilliSeconds = DefaultTrapDat::requestDelayInMilliSeconds();

   // Validate the config entries.
   if (ConfigurationInvalid())
   {
      DataLog(log_level_trap_error) << "Defaults File Configuration Invalid." << endmsg;

      return false;
   }

   LogFileContents(DefaultTrapDat::filename());

   return true;
}




//
// This routine initializes the class instance.
//
bool TrapProcess::Init ()
{
   bool success = OpenOverrideFile();

   if (!success)
   {
      success = OpenDefaultsFile();

      if (!success)
      {
         // Can't validate the default file.  Exit
         DataLog(log_level_trap_error) << "Can't validate defaults config file " <<  DefaultTrapDat::filename()
                                       << ".  EXITING." << endmsg;

         Cleanup();

         return false;
      }
   }

   // Check to see if the feature is turned on.
   if (_feature == 0)
   {
      DataLog(log_level_trap_info) << "TRAP Client Feature DISABLED.  EXITING." << endmsg;

      Cleanup();

      return false;
   }

   // Create the Time Window object.
   _timeWindow       = new TimeWindow (_beginWindow, _endWindow, (TRAP_DAYS_OF_WEEK)_daysOfWeek);

   _tryAgainLater    = false;
   _txSequenceNumber = 0;

   DataLog(log_level_trap_info) << "TRAP initialized" << endmsg;

   return true;
}



//
// Cleanup routine for process.
//
void TrapProcess::Cleanup (int sig)
{
   DataLog(log_level_trap_info) << "INSIDE CLEANUP" << endmsg;

   if (_connectSocket)
   {
      shutdown (_connectSocket, 2);
      close (_connectSocket);
   }

   DataLog(log_level_trap_info) << "EXIT" << endmsg;

   exit (sig);
}



//
// Main loop for handling socket connection to the TRAP Server
//
void TrapProcess::Process (void)
{
   const struct timeval  timeoutValue = {DEFAULT_TIMEOUT, 0};

   static char           RxBuffer[TRAP_MAX_TRIMA_MSG_SIZE];
   const int             RxBufferSize    = sizeof (RxBuffer);
   const int             sockAddressSize = sizeof (sockaddr_in);
   const int             flags           = 0;

   bool                  processMessages = true;

   int                   nbytes;
   int                   result;
   int                   bytesProcessed;
   char                  socketOption    = 1;

   int                   currentRetries  = 0;
   int                   currentTimeouts = 0;

   fd_set                readSockets;

   const struct timespec waitTime = {10, 0};   // Ten second

   //
   // Wait for T1 tests to complete.  During boot and T1 testing, the system is very busy
   // and TRAP can cause problems sending over files.
   // After T1 is complete, things settle down.
   // It takes about 40 seconds for all of this to occur so waiting in 10 second intervals
   // makes a nice polling time.
   //
   while (!_xifCds.loadSetReady.Get())
   {
      DataLog(log_level_trap_info) << "Waiting for T1 test to complete." << endmsg;
      nanosleep (&waitTime, 0);
   }

   // Main Message Processing Loop.
   while (processMessages)
   {
      //
      // Must satisfy all gates that prevent connection.
      //    Time Window.
      //    Donor Connected.
      //    Max Retries.
      //

      // If outside the time window, exit processing loop.
      if (_timeWindow->PastWindow())
      {
         DataLog(log_level_trap_info) << "PAST TIME WINDOW" << endmsg;

         _tryAgainLater = false;

         break;
      }
      else if (_timeWindow->BeforeWindow())
      {
         DataLog(log_level_trap_info) << "BEFORE TIME WINDOW" << endmsg;
         // Wait until time to connect
         _timeWindow->Wait();

         // Force another evaluation in case the wait is not long enough
         continue;
      }
      else
      {
         // Inside Window
         DataLog(log_level_trap_info) << "INSIDE WINDOW" << endmsg;
      }

      // If max retries have been exceeded, exit processing loop.
      if (currentRetries >= _maxRetries)
      {
         DataLog(log_level_trap_info) << "MAX RETRIES EXCEEDED" << endmsg;

         _tryAgainLater = true;

         break;
      }

      // If cassette is lowered, exit processing loop.
      if (_xifCds.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_DOWNLOADED))
      {
         DataLog(log_level_trap_info) << "CASSETTE LOWERED | DONOR_DOWNLOADED" << endmsg;

         _tryAgainLater = false;

         break;
      }

      //
      // All gates have been passed, OK to connect to TRAP Server.
      //

      // Create the Recieve Socket
      _connectSocket = socket (AF_INET, SOCK_STREAM, 0);

      if (_connectSocket < 0)
      {
         currentRetries++;

         DataLog(log_level_trap_error) << "Error creating connect socket" << endmsg;

         continue;
      }

      // Connect to the TRAP Server.
      result = connect (_connectSocket, (sockaddr*)&_serverAddress, sockAddressSize);

      if (result == ERROR)
      {
         currentRetries++;

         DataLog(log_level_trap_info) << "Connect Failed (" << currentRetries << ")." << endmsg;

         shutdown (_connectSocket, 2);
         close (_connectSocket);

         continue;
      }

      //
      // Successful socket connection.
      //
      sockaddr     peerSocket;
      sockaddr_in* inetPeerSocket = (sockaddr_in*)&peerSocket;

      // Print out Server IP address
      result = getpeername (_connectSocket, (sockaddr*)&peerSocket, (int*)&sockAddressSize);
      DataLog(log_level_trap_info) << "Successful connection. Server IP address ("
                                   << inet_ntoa (inetPeerSocket->sin_addr) << ")" << endmsg;


      setsockopt (_connectSocket, SOL_SOCKET, SO_KEEPALIVE, &socketOption, sizeof(int));
      setsockopt (_connectSocket, SOL_SOCKET, SO_LINGER,    &socketOption, sizeof(int));
      setsockopt (_connectSocket, SOL_SOCKET, SO_REUSEADDR, &socketOption, sizeof(int));

      //
      // Message Processing Loop
      //

      for (;; )
      {
         // If max timeouts have been exceeded, exit processing loop.
         if (currentTimeouts >= _maxTimeouts)
         {
            DataLog(log_level_trap_info) << "MAX TIMEOUTS EXCEEDED.  Terminating link" << endmsg;

            processMessages = false;
            _tryAgainLater  = false;

            break;
         }

         // Setup select call
         FD_ZERO(&readSockets);
         FD_SET (_connectSocket, &readSockets);

         // Block on the select call waiting for data (or timeout)
         int status = select(FD_SETSIZE, &readSockets, 0, 0, (timeval*)&timeoutValue);

         if (status == ERROR)
         {
            // Error occurred.
            DataLog(log_level_trap_info) << "Select Error " << errnoMsg << ".  Timeouts = " << ++currentTimeouts << endmsg;

            continue;
         }
         else if (status == 0)   // Request timed out.  Process Trima messages.
         {
            DataLog(log_level_trap_info) << "Recv timeout number " << ++currentTimeouts << "." << endmsg;

            continue;
         }
         else  // Got socket data, process it.
         {
            nbytes = recv (_connectSocket, RxBuffer, RxBufferSize, flags);
         }

         if (nbytes <= 0)
         {
            DataLog(log_level_trap_info) << "Socket Connection to the TRAP Server lost!  Closing socket." << endmsg;
            DataLog(log_level_trap_info) << "Received " << nbytes << ".  Errno = " << errnoMsg << endmsg;

            processMessages = false;
            _tryAgainLater  = false;

            break;
         }
         else
         {
            // If cassette is lowered, Don't process command, Send Disconnect Msg, and close socket.
            if (_xifCds.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_DOWNLOADED))
            {
               trapHeader reply;

               // Assign the header pointer to the new message
               reply.messageType    = TRAP_DISCONNECT;
               reply.sequenceNumber = _txSequenceNumber++;

               // Send a message to TRAP Server telling him I am disconnecting (not an ack really).
               SendAck (&reply, 0);

               DataLog(log_level_trap_info) << "CASSETTE LOWERED.  Terminating link." << endmsg;

               processMessages = false;
               _tryAgainLater  = false;

               break;
            }

            // Message received.  Parse it and process it.
            bytesProcessed = ProcessMessage (RxBuffer, nbytes);


            while (bytesProcessed > 0)
            {
               bytesProcessed = ProcessMessage (NULL, 0);
            }
         }
      }

      // Close the socket and try again.
      shutdown (_connectSocket, 2);
      close (_connectSocket);
      currentRetries++;
   }

   if (_tryAgainLater)
   {
      DataLog(log_level_trap_info) << "Counter's exceeded. Waiting an hour and trying again...." << endmsg;

      // Reset the counters

      // delay 1 hour and try again.  Server may be having problems.
      nanosleep (&ONE_HOUR, NULL);
   }
}



//
// This routine handles the validation, execution and response
// of all the Commands sent by the TRAP Server.
//
// This routine returns the number of bytes processed.
//
int TrapProcess::ProcessMessage (const char* msg, int sizeInBytes)
{
   static char* saveBuffer        = NULL;
   static int   bufferSizeInBytes = 0;

   int          currentSizeInBytes;
   int          processedSizeInBytes;

   trapHeader*  hdr = NULL;

   if (sizeInBytes == 0)
   {
      if (bufferSizeInBytes == 0)
      {
         DataLog(log_level_trap_debug) << "Buffer Empty" << endmsg;
         return 0;
      }
      else
      {
         DataLog(log_level_trap_debug) << "Buffer NOT Empty" << endmsg;

         // Try to get a whole message out of the saved buffer.
         hdr                = (trapHeader*)saveBuffer;
         currentSizeInBytes = bufferSizeInBytes;
      }
   }
   else if (bufferSizeInBytes > 0)
   {
      DataLog(log_level_trap_debug) << "Adding (" << sizeInBytes << ") bytes to Current Buffer "
                                    << bufferSizeInBytes << ")" << endmsg;

      DataLog(log_level_trap_debug) << "saveBuffer before (" << hex << saveBuffer << dec << ")" << endmsg;

      // Reallocate the new buffer with a larger size.
      saveBuffer = (char*)realloc ((void*)saveBuffer, bufferSizeInBytes + sizeInBytes);

      if (saveBuffer == NULL)
      {
         DataLog(log_level_trap_error) << "realloc failed!" << endmsg;
      }

      DataLog(log_level_trap_debug) << "saveBuffer after (" << hex << saveBuffer << dec << ")" << endmsg;

      // Holding a saved partial buffer.  Glue them together.
      memcpy (&saveBuffer[bufferSizeInBytes], msg, sizeInBytes);
      bufferSizeInBytes += sizeInBytes;

      // Attach the hdr var to the saved buffer
      hdr                = (trapHeader*)saveBuffer;
      currentSizeInBytes = bufferSizeInBytes;
   }
   else
   {
      // Attach the hdr var to the rx stream buffer
      hdr                = (trapHeader*)msg;
      currentSizeInBytes = sizeInBytes;
   }

   // Sanity check
   if (hdr == NULL)
   {
      DataLog(log_level_trap_error) << "realloc failed!" << endmsg;
      return 0;
   }

   int result = CTrapIf::validHeader (hdr, currentSizeInBytes);

   // A non-zero return value indicates an error.
   switch (result)
   {
      case 0 :
         // Valid Message.  Process it.
         break;

      case TRAP_BUFFER_UNDERRUN :
         DataLog(log_level_trap_debug) << "Buffer Underrun (" << sizeInBytes << ", "
                                       << bufferSizeInBytes << ")" << endmsg;

         if (bufferSizeInBytes == 0)
         {
            // Create new Save buffer for existing packet.
            saveBuffer = (char*)malloc (sizeInBytes);

            // Sanity check
            if (saveBuffer == NULL)
            {
               DataLog(log_level_trap_error) << "realloc failed!" << endmsg;
               return 0;
            }

            memcpy (saveBuffer, msg, sizeInBytes);
            bufferSizeInBytes = sizeInBytes;
         }

         return 0;

      case TRAP_BUFFER_OVERRUN :
         // Process existing message and save left over for next time.
         DataLog(log_level_trap_debug) << "Buffer Overrun (" << sizeInBytes << ", "
                                       << bufferSizeInBytes << ")" << endmsg;
         break;

      case TRAP_HEADER_CRC_INVALID :
         // Add parse loop that scans buffer looking for SOM.

         PrintHeader (CTrapIf::errorString(), hdr);
         CTrapIf::prepareMsg(hdr, -1, -1, result);
         Send (hdr, trapHeaderLength);

         // Clear the Saved Buffer
         bufferSizeInBytes = 0;

         // Don't trust QNX here.
         if (saveBuffer)
         {
            free (saveBuffer);
            saveBuffer = NULL;
         }

         return 0;

      case TRAP_MESSAGE_ID_INVALID :
         PrintHeader (CTrapIf::errorString(), hdr);
         CTrapIf::prepareMsg(hdr, -1, hdr->sequenceNumber, result);
         Send (hdr, trapHeaderLength);

         // Clear the Saved Buffer
         bufferSizeInBytes = 0;

         // Don't trust QNX here.
         if (saveBuffer)
         {
            free (saveBuffer);
            saveBuffer = NULL;
         }

         return 0;

      default :
         PrintHeader (CTrapIf::errorString(), hdr);
         SendAck (hdr, result);

         // Clear the Saved Buffer
         bufferSizeInBytes = 0;

         // Don't trust QNX here.
         if (saveBuffer)
         {
            free (saveBuffer);
            saveBuffer = NULL;
         }

         return 0;
   }

   const int lastRequestDelta = osTime::howLongMilliSec(_lastRequestTime);

   DataLog(log_level_trap_info) << "Last message was " << lastRequestDelta << " milliseconds ago" << endmsg;

   // Meter Cadence so they don't overwhelm us with requests for files.
   // The other commands don't risk priority inversion issues so they don't need
   // to be metered
   if ( (lastRequestDelta < _requestDelayInMilliSeconds) &&
        ( (hdr->messageType == TRAP_REQUEST_FILE_LIST)       ||
          (hdr->messageType == TRAP_REQUEST_FILE_LIST_BY_SN) ||
          (hdr->messageType == TRAP_UPLOAD_FILE) )
        )
   {
      const struct timespec waitTime = {_requestDelay, 0};

      DataLog(log_level_trap_info) << "Request exceeded max time interval.  Waiting " << _requestDelay << " seconds..." << endmsg;
      nanosleep (&waitTime, 0);
   }

   osTime::snapshotRawTime(_lastRequestTime);

   switch (hdr->messageType)
   {
      case TRAP_SET_LOCAL_TIME :
         processedSizeInBytes = SetTrimaTime (hdr, currentSizeInBytes);
         break;

      case TRAP_REQUEST_FILE_LIST :
         processedSizeInBytes = RequestFileList (hdr, currentSizeInBytes);
         break;

      case TRAP_REQUEST_FILE_LIST_BY_SN :
         processedSizeInBytes = RequestFileListBySn (hdr, currentSizeInBytes);
         break;

      case TRAP_UPLOAD_FILE :
         processedSizeInBytes = UploadFile (hdr, currentSizeInBytes);
         break;

      case TRAP_SET_ALT_CONTROL_SERVER :
         processedSizeInBytes = SetAltControlServer (hdr, currentSizeInBytes);
         break;

      case TRAP_SET_CONNECT_TIME :
         processedSizeInBytes = SetConnectTime (hdr, currentSizeInBytes);
         break;

      case TRAP_GET_TRIMA_INFO :
         processedSizeInBytes = GetTrimaInfo (hdr, currentSizeInBytes);
         break;

      case TRAP_DISCONNECT :
         processedSizeInBytes = Disconnect (hdr, currentSizeInBytes);
         break;

      case TRAP_DELETE_LOG_FILE :
         processedSizeInBytes = DeleteFile (hdr, currentSizeInBytes);
         break;

      case TRAP_SET_REQUEST_DELAY :
         processedSizeInBytes = SetRequestDelay (hdr, currentSizeInBytes);
         break;

      default :
         DataLog(log_level_trap_error) << "Unsupported Command ID(" << hdr->messageType << ")." << endmsg;

         CTrapIf::prepareMsg(hdr, -1, hdr->sequenceNumber, result);
         Send (hdr, trapHeaderLength);

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

         DataLog(log_level_trap_debug) << "saving (" << bytesToMove << ") Bytes." << endmsg;

         // Create new Save buffer for remaining data.
         saveBuffer        = (char*)malloc (bytesToMove);
         memcpy (saveBuffer, &msg[processedSizeInBytes], bytesToMove);
         bufferSizeInBytes = bytesToMove;
      }
      else
      {
         int bytesToMove = bufferSizeInBytes - processedSizeInBytes;
         DataLog(log_level_trap_debug) << "moving (" << bytesToMove << ") Bytes." << endmsg;

         // Left over data.  Store it.
         memcpy (&saveBuffer[0], &saveBuffer[processedSizeInBytes], bytesToMove);
         bufferSizeInBytes -= processedSizeInBytes;
      }
   }
   else if (bufferSizeInBytes)
   {
      // In this case, complete message has been processed.
      // If buffer is hanging around, remove it.
      DataLog(log_level_trap_debug) << "freeing buffer." << endmsg;

      bufferSizeInBytes = 0;
      free (saveBuffer);
      saveBuffer        = NULL;
   }

   return processedSizeInBytes;
}



//
// This routine processes the Set Trima Time command.
//
int TrapProcess::SetTrimaTime (const void* msg, int sizeInBytes)
{
   const setLocalTimeMsg* newTime = (setLocalTimeMsg*)msg;

   trapHeader*            hdr     = (trapHeader*)msg;

   // Validate message size.
   if ( (sizeInBytes        < setLocalTimeMsgSize) ||
        (hdr->lengthInBytes != setLocalTimeMsgSize - trapHeaderLength) )
   {
      // Log event for later debugging.
      _logStr << "Invalid SetTrimaTime Msg size, value=" << sizeInBytes << ", expected "
              << setLocalTimeMsgSize << "." << ends;

      PrintHeader (_logStr.str(), hdr);

      SendAck (hdr, TRAP_BAD_LENGTH);

      return setLocalTimeMsgSize;
   }

   // Verify that PFR has completed.
   // if (!CVipShm::pfrComplete())
   if (!_xifCds.pfrComplete.Get())
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "No Set Time allowed, PFR not complete." << endmsg;

      SendAck (hdr, TRAP_PFR_NOT_COMPLETE);

      return setLocalTimeMsgSize;
   }

   // If cassette is lowered, terminate link
   if (_xifCds.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Cassette Lowered.  Exiting" << endmsg;

      SendAck (hdr, TRAP_CASSETTE_DOWN);

      Cleanup();

      return setLocalTimeMsgSize;
   }

   // Send on the Set Time message
   DataLog(log_level_trap_info) << "Setting Trima time to "
                                << ctime ((const long unsigned int*)&newTime->localTime) << "("
                                << newTime->localTime << ")" << endmsg;

   trima_time::set(newTime->localTime);

   // Send Response packet back to the TRAP Server.
   SendAck (hdr, TRAP_OK);

   return setLocalTimeMsgSize;
}



//
// This routine processes the Request File List command.
//
int TrapProcess::RequestFileList (const void* msg, int sizeInBytes)
{
   trapHeader*         hdr         = (trapHeader*)msg;

   requestFileListMsg* getFileList = (requestFileListMsg*)msg;

   XIF_FileIo          fileIo(TRAP_TEMPORARY_FILE, _connectSocket, log_level_trap_info, log_level_trap_error, true);
   fileIo.turnOnDelay();

   unsigned long fileCrc = INITCRC_DEFAULT;
   int           fileSize;


   // Validate message size.
   if ( (sizeInBytes        < requestFileListMsgSize) ||
        (hdr->lengthInBytes != requestFileListMsgSize - trapHeaderLength) )
   {
      // Log event for later debugging.
      _logStr << "Invalid RequestFileList Msg size, value=" << sizeInBytes << ", expected "
              << requestFileListMsgSize << "." << ends;

      PrintHeader (_logStr.str(), hdr);

      SendAck (hdr, TRAP_BAD_LENGTH);

      return requestFileListMsgSize;
   }

   // If Donor Connected, terminate link
   if (_xifCds.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Cassette Lowered.  Exiting" << endmsg;

      SendAck (hdr, TRAP_CASSETTE_DOWN);

      Cleanup();

      return requestFileListMsgSize;
   }

   int size = strlen(getFileList->pathName);
   if ( (size == 0) || (size > TRAP_MAX_PATHNAME_LENGTH - 1) )
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Invalid pathname length, value=" << size
                                   << ", expected range 1-" << TRAP_MAX_PATHNAME_LENGTH - 1 << endmsg;

      SendAck (hdr, TRAP_BAD_VALUE);

      return requestFileListMsgSize;
   }

   size = strlen(getFileList->pattern);
   if ( (size == 0) || (size > TRAP_MAX_PATTERN_LENGTH - 1) )
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Invalid pattern length, value=" << size
                                   << ", expected range 1-" << TRAP_MAX_PATTERN_LENGTH - 1 << endmsg;

      SendAck (hdr, TRAP_BAD_VALUE);

      return requestFileListMsgSize;
   }

   DataLog(log_level_trap_info) << "File List request: " << getFileList->pathName << ", "
                                << getFileList->pattern << endmsg;

   // Collect the list of files.
   XIF_FileIo::STATUS status = fileIo.getFileList(getFileList->pathName, getFileList->pattern, fileSize, fileCrc);

   switch (status)
   {
      case XIF_FileIo::SUCCESS :
         break;

      case XIF_FileIo::BAD_SEARCH_PATTERN :
         SendAck(hdr, TRAP_ILLEGAL_FORMAT);
         return requestFileListMsgSize;
         break;

      case XIF_FileIo::BAD_DIR_NAME :
         SendAck(hdr, TRAP_BAD_VALUE);
         return requestFileListMsgSize;
         break;

      case XIF_FileIo::BAD_TEMP_FILE :
         SendAck(hdr, TRAP_FAILED_TO_COMMIT);
         return requestFileListMsgSize;
         break;

      default :
         DataLog(log_level_trap_error) << "Unexpected choice " << status << endmsg;

         SendAck(hdr, TRAP_BAD_VALUE);
         return requestFileListMsgSize;
         break;
   }

   // If the file size is 0, return a blank reply
   if (fileSize == 0)
   {
      DataLog(log_level_trap_info) << "No files found for request (" << getFileList->pathName << "/"
                                   << getFileList->pattern << ")" << endmsg;

      // Send Response packet back to the TRAP Server.
      SendAck (hdr, TRAP_OK);

      return requestFileListMsgSize;
   }

   // Prepare the reply message header.
   // Since this reply will possibly be split over multiple socket sends, the file crc
   // needs to be computed before any individual packet is sent.
   CTrapIf::prepareMsg(hdr, hdr->messageType, hdr->sequenceNumber, TRAP_OK);
   hdr->lengthInBytes = fileSize;

   // Set the Body CRC value.
   hdr->bodyCrc = fileCrc ^ INITCRC_DEFAULT;

   // Reset the CRC value.
   fileCrc = INITCRC_DEFAULT;

   // Recompute the Header CRC.
   if (crcgen32(&fileCrc, (unsigned char*)hdr, trapHeaderLength - sizeof(unsigned long)) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_trap_error) << "Could not generate crc for message header" << endmsg;

      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return requestFileListMsgSize;
   }

   hdr->headerCrc = fileCrc ^ INITCRC_DEFAULT;

   // Send the header first.  Body follows. Easier that way.
   Send (hdr, trapHeaderLength);

   fileIo.sendFileList();

   DataLog(log_level_trap_info) << "File List Retrieved from " << getFileList->pathName << ", pattern " << getFileList->pattern << "." << endmsg;

   return requestFileListMsgSize;
}




//
// This routine processes the Request File List command.
//
int TrapProcess::RequestFileListBySn (const void* msg, int sizeInBytes)
{
   //  1T       DDDDD             _                 YYYYMMDD               _   DDD         _    DDDDDD                     .evr
   const char*                 logFilePattern      = "^^1T[0-9][0-9][0-9][0-9][0-9]_[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]_[0-9][0-9][0-9]_[0-9][0-9][0-9][0-9][0-9][0-9]" LOG_FILE_EXTENSION;

   trapHeader*                 hdr                 = (trapHeader*)msg;
   requestFileListBySeqNumMsg* getFileListBySeqNum = (requestFileListBySeqNumMsg*)msg;

   XIF_FileIo                  fileIo(TRAP_TEMPORARY_FILE, _connectSocket, log_level_trap_info, log_level_trap_error, true);
   fileIo.turnOnDelay();

   unsigned long fileCrc           = INITCRC_DEFAULT;
   int           bufferSizeInBytes = 0;
   int           fileSize;


   // Validate message size.
   if ( (sizeInBytes        <  requestFileListBySeqNumMsgSize) ||
        (hdr->lengthInBytes != requestFileListBySeqNumMsgSize - trapHeaderLength) )
   {
      // Log event for later debugging.
      _logStr << "Invalid RequestFileListBySN Msg size, value=" << sizeInBytes << ", expected "
              << requestFileListBySeqNumMsgSize << "." << ends;

      PrintHeader (_logStr.str(), hdr);

      SendAck (hdr, TRAP_BAD_LENGTH);

      return requestFileListBySeqNumMsgSize;
   }

   int size = strlen(getFileListBySeqNum->directoryName);
   if ( (size == 0) || (size > TRAP_MAX_PATHNAME_LENGTH - 1) )
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Invalid pathname length, value=" << size << ", expected range 1-"
                                   << TRAP_MAX_PATHNAME_LENGTH - 1 << endmsg;

      SendAck (hdr, TRAP_BAD_VALUE);

      return requestFileListBySeqNumMsgSize;
   }

   if ( (getFileListBySeqNum->sequenceNumber < 0) || (getFileListBySeqNum->sequenceNumber > TRAP_MAX_SEQUENCE_NUMBER) )
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Invalid sequence number, value="
                                   << getFileListBySeqNum->sequenceNumber << ", expected range 0-"
                                   << TRAP_MAX_SEQUENCE_NUMBER << endmsg;

      SendAck (hdr, TRAP_BAD_VALUE);

      return requestFileListBySeqNumMsgSize;
   }

   DataLog(log_level_trap_info) << "File List Request by SN " << getFileListBySeqNum->directoryName
                                << ", " << getFileListBySeqNum->sequenceNumber << "." << endmsg;

   // Collect the list of files.
   XIF_FileIo::STATUS status = fileIo.getFileList(getFileListBySeqNum->directoryName, logFilePattern, fileSize, fileCrc);

   switch (status)
   {
      case XIF_FileIo::SUCCESS :
         break;

      case XIF_FileIo::BAD_SEARCH_PATTERN :
         SendAck(hdr, TRAP_ILLEGAL_FORMAT);
         return requestFileListBySeqNumMsgSize;
         break;

      case XIF_FileIo::BAD_DIR_NAME :
         SendAck(hdr, TRAP_BAD_VALUE);
         return requestFileListBySeqNumMsgSize;
         break;

      case XIF_FileIo::BAD_TEMP_FILE :
         SendAck(hdr, TRAP_FAILED_TO_COMMIT);
         return requestFileListBySeqNumMsgSize;
         break;

      default :
         DataLog(log_level_trap_error) << "Unexpected choice " << status << endmsg;

         SendAck(hdr, TRAP_BAD_VALUE);
         return requestFileListBySeqNumMsgSize;
   }

   if (fileSize == 0)
   {
      DataLog(log_level_trap_info) << "No files found!" << endmsg;

      SendAck (hdr, TRAP_OK);

      return requestFileListBySeqNumMsgSize;
   }

   const int countSize = fileIo.countFileList(getFileListBySeqNum->sequenceNumber, fileCrc);

   // Check for failure.
   if (countSize == -1)
   {
      // Log event for later debugging.
      DataLog(log_level_trap_error) << "Could not open file " << TRAP_TEMPORARY_FILE << endmsg;

      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return requestFileListBySeqNumMsgSize;
   }

   // Prepare the reply message header.
   // Since this reply will possibly be split over multiple socket sends, the file crc
   // needs to be computed before any individual packet is sent.
   CTrapIf::prepareMsg(hdr, hdr->messageType, hdr->sequenceNumber, TRAP_OK);
   hdr->lengthInBytes = countSize;

   // Set the Body CRC value.
   hdr->bodyCrc = fileCrc ^ INITCRC_DEFAULT;

   // Reset the CRC value.
   fileCrc = INITCRC_DEFAULT;

   // Recompute the Header CRC.
   if (crcgen32(&fileCrc, (unsigned char*)hdr, trapHeaderLength - sizeof(unsigned long)) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_trap_error) << "Could not generate crc for message header" << endmsg;

      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return requestFileListBySeqNumMsgSize;
   }

   hdr->headerCrc = fileCrc ^ INITCRC_DEFAULT;

   // Send the header first.  Body follows. Easier that way.
   Send (hdr, trapHeaderLength);

   fileIo.sendFileList(getFileListBySeqNum->sequenceNumber);

   DataLog(log_level_trap_info) << "File List By Seq Num " << getFileListBySeqNum->sequenceNumber
                                << " retrieved from " << getFileListBySeqNum->directoryName << "." << endmsg;

   return requestFileListBySeqNumMsgSize;
}




//
// This routine processes the UploadFile command.
//
int TrapProcess::UploadFile (const void* msg, int sizeInBytes)
{
   trapHeader*    hdr        = (trapHeader*)msg;

   uploadFileMsg* uploadFile = (uploadFileMsg*)msg;

   XIF_FileIo     fileIo(TRAP_TEMPORARY_FILE, _connectSocket, log_level_trap_info, log_level_trap_error, true);
   fileIo.turnOnDelay();

   unsigned long fileCrc = INITCRC_DEFAULT;
   struct stat   fileStat;


   // Validate message size.
   if ( (sizeInBytes        < uploadFileMsgSize) ||
        (hdr->lengthInBytes != uploadFileMsgSize - trapHeaderLength) )
   {
      // Log event for later debugging.
      _logStr << "Invalid uploadFile Msg size, value=" << sizeInBytes << ", expected "
              << uploadFileMsgSize << "." << ends;

      PrintHeader (_logStr.str(), hdr);

      SendAck (hdr, TRAP_BAD_LENGTH);

      return uploadFileMsgSize;
   }

   // If cassette lowered, terminate link
   if (_xifCds.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Cassette Lowered.  Exiting" << endmsg;

      SendAck (hdr, TRAP_CASSETTE_DOWN);

      Cleanup();

      return uploadFileMsgSize;
   }

   // Get the file size
   if (stat (uploadFile->fileName, &fileStat) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Could not stat file " << uploadFile->fileName << endmsg;

      SendAck (hdr, TRAP_ILLEGAL_FORMAT);

      return uploadFileMsgSize;
   }

   // If the file size is 0, return a blank reply
   if (fileStat.st_size == 0)
   {
      // Send Response packet back to the TRAP Server.
      SendAck (hdr, TRAP_OK);

      return uploadFileMsgSize;
   }

   DataLog(log_level_trap_info) << "preparing CRC for file " << uploadFile->fileName << " CRC " << hex << fileCrc << dec
                                << ", size " << fileStat.st_size << endmsg;

   // Get the CRC of the file.
   // Note.  This should be OK if the file exists.
   if (file_crcgen32 (uploadFile->fileName, &fileCrc) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_trap_error) << "Could not generate crc for file " << uploadFile->fileName << endmsg;

      SendAck (hdr, TRAP_ILLEGAL_FORMAT);

      return uploadFileMsgSize;
   }

   DataLog(log_level_trap_info) << "uploading file " << uploadFile->fileName << " CRC " << hex << fileCrc << dec
                                << ", size " << fileStat.st_size << endmsg;

   // Prepare the reply message header.
   // Since this reply will possibly be split over multiple socket sends, the file crc
   // needs to be computed before any individual packet is sent.
   trapHeader reply = *hdr;

   CTrapIf::prepareMsg(&reply, reply.messageType, reply.sequenceNumber, TRAP_OK);
   reply.lengthInBytes = fileStat.st_size;

   // Set the Body CRC value.
   reply.bodyCrc = fileCrc ^ INITCRC_DEFAULT;

   // Reset the CRC value.
   fileCrc = INITCRC_DEFAULT;

   // Recompute the Header CRC.
   if (crcgen32(&fileCrc, (unsigned char*)&reply, trapHeaderLength - sizeof(unsigned long)) != 0)
   {
      // Log event for later debugging.
      DataLog(log_level_trap_error) << "Could not generate crc for file " << uploadFile->fileName << endmsg;

      SendAck (&reply, TRAP_ILLEGAL_FORMAT);

      return uploadFileMsgSize;
   }

   reply.headerCrc = fileCrc ^ INITCRC_DEFAULT;

   // Send the header first.  Body follows. Easier that way.
   Send (&reply, trapHeaderLength);

   fileIo.sendFile(uploadFile->fileName);

   DataLog(log_level_trap_info) << "File Uploaded from " << uploadFile->fileName << endmsg;

   return uploadFileMsgSize;
}



//
// This routine processes the Set Alt Contol Server command.
//
int TrapProcess::SetAltControlServer (const void* msg, int sizeInBytes)
{
   trapHeader*             hdr       = (trapHeader*)msg;
   setAltControlServerMsg* newServer = (setAltControlServerMsg*)msg;

   // Validate message size.
   if ( (sizeInBytes        < setAltControlServerMsgSize) ||
        (hdr->lengthInBytes != setAltControlServerMsgSize - trapHeaderLength) )
   {
      // Log event for later debugging.
      _logStr << "Invalid Set Alt Control Server Msg size, value=" << sizeInBytes << ", expected "
              << setAltControlServerMsgSize << ends;

      PrintHeader (_logStr.str(), hdr);

      SendAck (hdr, TRAP_BAD_LENGTH);

      return setAltControlServerMsgSize;
   }

   // If Cassette Lowered, terminate link
   if (_xifCds.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Cassette Lowered.  Exiting" << endmsg;

      SendAck (hdr, TRAP_CASSETTE_DOWN);

      Cleanup();

      return setAltControlServerMsgSize;
   }

   // Validate the Control IP entry.
   if (!ValidControlIp(newServer->serverIp))
   {
      SendAck (hdr, TRAP_BAD_VALUE);

      return setAltControlServerMsgSize;
   }

   // Validate the Control Port entry.
   if (!ValidControlPort(newServer->portId))
   {
      SendAck (hdr, TRAP_BAD_VALUE);

      return setAltControlServerMsgSize;
   }

   // Update the Server IP
   if (!OverrideTrapDat::updateConfigEntry(CONTROL_IP, newServer->serverIp))
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setAltControlServerMsgSize;
   }

   // Update the Server Port
   char buffer[10];
   sprintf(buffer, "%d", newServer->portId);

   if (!OverrideTrapDat::updateConfigEntry (CONTROL_PORT, buffer))
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setAltControlServerMsgSize;
   }

   // Update the Override file and CRC
   if (!OverrideTrapDat::write())
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setAltControlServerMsgSize;
   }

   DataLog(log_level_trap_info) << "New Control Server: CIP(" << newServer->serverIp
                                << "), CP(" << newServer->portId << ")" << endmsg;

   // Send Response packet back to the TRAP Server.
   SendAck (hdr, TRAP_OK);

   return setAltControlServerMsgSize;
}



//
// This routine processes the Set Connect Time command.
//
int TrapProcess::SetConnectTime (const void* msg, int sizeInBytes)
{
   trapHeader*        hdr            = (trapHeader*)msg;
   setConnectTimeMsg* newConnectTime = (setConnectTimeMsg*)msg;

   // Validate message size.
   if ( (sizeInBytes        <  setConnectTimeMsgSize) ||
        (hdr->lengthInBytes != setConnectTimeMsgSize - trapHeaderLength) )
   {
      // Log event for later debugging.
      _logStr << "Invalid Connect Time Msg size, value=" << sizeInBytes << ", expected "
              << setConnectTimeMsgSize << "." << ends;

      PrintHeader (_logStr.str(), hdr);

      SendAck (hdr, TRAP_BAD_LENGTH);

      return setConnectTimeMsgSize;
   }

   // If cassette lowered, terminate link
   if (_xifCds.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Cassette Lowered.  Exiting" << endmsg;

      SendAck (hdr, TRAP_CASSETTE_DOWN);

      Cleanup();

      return setConnectTimeMsgSize;
   }

   // Validate the Days of Week
   if (!ValidDaysOfWeek(newConnectTime->daysOfWeek))
   {
      SendAck (hdr, TRAP_BAD_VALUE);

      return setConnectTimeMsgSize;
   }

   // Validate the Window
   if (!ValidWindow(newConnectTime->startWindow, newConnectTime->endWindow))
   {
      SendAck (hdr, TRAP_BAD_VALUE);

      return setConnectTimeMsgSize;
   }

   // Validate the Number of Attempts
   if (!ValidMaxRetries(newConnectTime->numberOfAttempts))
   {
      SendAck (hdr, TRAP_BAD_VALUE);

      return setConnectTimeMsgSize;
   }

   // Validate the Number of Timeouts
   if (!ValidMaxTimeouts(newConnectTime->numberOfTimeouts))
   {
      SendAck (hdr, TRAP_BAD_VALUE);

      return setConnectTimeMsgSize;
   }

   // Validate the Feature
   if (!ValidFeature(newConnectTime->featureOnOff))
   {
      SendAck (hdr, TRAP_BAD_VALUE);

      return setConnectTimeMsgSize;
   }

   char buffer[20];

   // Update the Days of Week
   sprintf(buffer, "%d", newConnectTime->daysOfWeek);
   if (!OverrideTrapDat::updateConfigEntry (DAYS_OF_WEEK, buffer))
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setConnectTimeMsgSize;
   }

   // Update the Begin Window
   sprintf(buffer, "%d", newConnectTime->startWindow);
   if (!OverrideTrapDat::updateConfigEntry (BEGIN_WINDOW, buffer))
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setConnectTimeMsgSize;
   }

   // Update the End Window
   sprintf(buffer, "%d", newConnectTime->endWindow);
   if (!OverrideTrapDat::updateConfigEntry (END_WINDOW, buffer))
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setConnectTimeMsgSize;
   }

   // Update the Number of Attempts
   sprintf(buffer, "%d", newConnectTime->numberOfAttempts);
   if (!OverrideTrapDat::updateConfigEntry (NUMBER_OF_ATTEMPTS, buffer))
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setConnectTimeMsgSize;
   }

   // Update the Number of Timeouts
   sprintf(buffer, "%d", newConnectTime->numberOfTimeouts);
   if (!OverrideTrapDat::updateConfigEntry (NUMBER_OF_TIMEOUTS, buffer))
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setConnectTimeMsgSize;
   }

   // Update the Feature On/Off
   sprintf(buffer, "%d", newConnectTime->featureOnOff);
   if (!OverrideTrapDat::updateConfigEntry (FEATURE, buffer))
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setConnectTimeMsgSize;
   }

   // Update the Override file and CRC
   if (!OverrideTrapDat::write())
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setConnectTimeMsgSize;
   }

   DataLog(log_level_trap_info) << "New Config: DAY(" << newConnectTime->daysOfWeek
                                << "), BW(" << newConnectTime->startWindow
                                << "), EW(" << newConnectTime->endWindow
                                << "), MR(" << newConnectTime->numberOfAttempts
                                << "), MT(" << newConnectTime->numberOfTimeouts
                                << "), F("  << newConnectTime->featureOnOff << ")" << endmsg;

   // Send Response packet back to the TRAP Server.
   SendAck (hdr, TRAP_OK);

   return setConnectTimeMsgSize;
}



//
// This routine processes the Set Request Delay message
//
int TrapProcess::SetRequestDelay (const void* msg, int sizeInBytes)
{
   trapHeader*         hdr      = (trapHeader*)msg;
   setRequestDelayMsg* newDelay = (setRequestDelayMsg*)msg;

   // Validate message size.
   if ( (sizeInBytes        <  setRequestDelayMsgSize) ||
        (hdr->lengthInBytes != setRequestDelayMsgSize - trapHeaderLength) )
   {
      // Log event for later debugging.
      _logStr << "Invalid Request Delay Msg size, value=" << sizeInBytes << ", expected "
              << setRequestDelayMsgSize << "." << ends;

      PrintHeader (_logStr.str(), hdr);

      SendAck (hdr, TRAP_BAD_LENGTH);

      return setRequestDelayMsgSize;
   }

   // If Donor Connected, terminate link
   if (_xifCds.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Donor Connected.  Exiting" << endmsg;

      SendAck (hdr, TRAP_CASSETTE_DOWN);

      Cleanup();

      return setRequestDelayMsgSize;
   }

   // Validate the Request Delay
   if (!ValidRequestDelay(newDelay->requestDelay))
   {
      SendAck (hdr, TRAP_BAD_VALUE);

      return setRequestDelayMsgSize;
   }

   char buffer[20];

   // Update the Request Delay
   sprintf(buffer, "%d", newDelay->requestDelay);

   if (!OverrideTrapDat::updateConfigEntry (REQUEST_DELAY, buffer))
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setRequestDelayMsgSize;
   }

   // Update the Override file and CRC
   if (!OverrideTrapDat::write())
   {
      SendAck (hdr, TRAP_FAILED_TO_COMMIT);

      return setRequestDelayMsgSize;
   }

   DataLog(log_level_trap_info) << "New Request Delay (" << newDelay->requestDelay << ")" << endmsg;

   // Send Response packet back to the TRAP Server.
   SendAck (hdr, TRAP_OK);

   return setRequestDelayMsgSize;
}



//
// This routine processes the Get Trima Info command.
//
int TrapProcess::GetTrimaInfo (const void* msg, int sizeInBytes)
{
   trapHeader* hdr = (trapHeader*)msg;
   TrimaInfo   trima;


   // Validate message size.
   if ( (sizeInBytes        <  getTrimaInfoMsgSize) ||
        (hdr->lengthInBytes != getTrimaInfoMsgSize - trapHeaderLength) )
   {
      // Log event for later debugging.
      _logStr << "Invalid Trima Info Msg size, value=" << sizeInBytes << ", expected "
              << getTrimaInfoMsgSize << "." << ends;

      PrintHeader (_logStr.str(), hdr);

      SendAck (hdr, TRAP_BAD_LENGTH);

      return getTrimaInfoMsgSize;
   }

   // If Cassette Lowered, terminate link
   if (_xifCds.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Cassette Lowered.  Exiting" << endmsg;

      SendAck (hdr, TRAP_CASSETTE_DOWN);

      Cleanup();

      return getTrimaInfoMsgSize;
   }

   DataLog(log_level_trap_info) << "Requesting Trima Info" << endmsg;

   // Declare the reply message
   getTrimaInfoReplyMsg replyMsg;

   // Copy across the header.
   memcpy (&replyMsg.hdr, hdr, trapHeaderLength);

   string temp;

   strncpy(replyMsg.trimaLogFile, TrimaInfo::logFileNameShort(), TRAP_MAX_LOG_FILE_SIZE);
   strncpy(replyMsg.trimaCRC, TrimaInfo::trimaCrc(), TRAP_MAX_CRC_SIZE);
   strncpy(replyMsg.trimaRelease, TrimaInfo::releaseNumber(), TRAP_MAX_RELEASE_SIZE);
   strncpy(replyMsg.trimaBuild, TrimaInfo::buildNumber(), TRAP_MAX_BUILD_SIZE);
   strncpy(replyMsg.sequenceNumber, TrimaInfo::sequenceNumber(), TRAP_MAX_SEQUENCE_NUMBER_SIZE);

   hdr = (trapHeader*)&replyMsg;

   // Send Response packet back to the TRAP Server.
   SendReply (hdr, getTrimaInfoReplyMsgSize);

   DataLog(log_level_trap_info) << "Log Name is " << replyMsg.trimaLogFile << endmsg;
   DataLog(log_level_trap_info) << "Trima CRC is " << replyMsg.trimaCRC << endmsg;
   DataLog(log_level_trap_info) << "Release is " << replyMsg.trimaRelease << endmsg;
   DataLog(log_level_trap_info) << "Sequence Number is " << replyMsg.sequenceNumber << endmsg;

   return getTrimaInfoMsgSize;
}



//
// This routine processes the Disconnect command.
//
int TrapProcess::Disconnect (const void* msg, int sizeInBytes)
{
   trapHeader* hdr = (trapHeader*)msg;

   // Validate message size.
   if ( (sizeInBytes        <  disconnectMsgSize) ||
        (hdr->lengthInBytes != disconnectMsgSize - trapHeaderLength) )
   {
      // Log event for later debugging.
      _logStr << "Invalid Connect Time Msg size, value=" << sizeInBytes << ", expected "
              << disconnectMsgSize << "." << ends;

      PrintHeader (_logStr.str(), hdr);

      SendAck (hdr, TRAP_BAD_LENGTH);

      return disconnectMsgSize;
   }

   // Send Response packet back to the TRAP Server.
   SendAck (hdr, TRAP_OK);

   DataLog(log_level_trap_info) << "Got Disconnect Msg.  Exiting..." << endmsg;

   Cleanup();

   return disconnectMsgSize;
}



//
// This routine processes the Delete File command.
//
int TrapProcess::DeleteFile (const void* msg, int sizeInBytes)
{
   trapHeader*    hdr        = (trapHeader*)msg;
   deleteFileMsg* deleteFile = (deleteFileMsg*)msg;


   // Validate message size.
   if ( (sizeInBytes        < deleteFileMsgSize) ||
        (hdr->lengthInBytes != deleteFileMsgSize - trapHeaderLength) )
   {
      // Log event for later debugging.
      _logStr << "Invalid deleteFile Msg size, value=" << sizeInBytes << ", expected "
              << deleteFileMsgSize << ends;

      PrintHeader (_logStr.str(), hdr);

      SendAck (hdr, TRAP_BAD_LENGTH);

      return deleteFileMsgSize;
   }

   // If Cassette Lowered, terminate link
   if (_xifCds.restrictedAccess(XIF_CASSETTE_LOWERED | XIF_DONOR_DOWNLOADED))
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Cassette Lowered. Exiting" << endmsg;

      SendAck (hdr, TRAP_CASSETTE_DOWN);

      Cleanup();

      return deleteFileMsgSize;
   }

   int size = strlen(deleteFile->fileName);
   if ( (size == 0) || (size > TRAP_MAX_FILENAME_LENGTH - 1) )
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Invalid filename length, value=" << size
                                   << ", expected range 1-" << TRAP_MAX_FILENAME_LENGTH - 1 << endmsg;

      SendAck (hdr, TRAP_BAD_VALUE);

      return deleteFileMsgSize;
   }

   // Verify that the path is one of the valid choices.
   if ( !(strstr(deleteFile->fileName, LOG_PATH) ) )
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Invalid path, value=" << deleteFile->fileName
                                   << ", expected " << LOG_PATH << "." << endmsg;

      SendAck (hdr, TRAP_BAD_VALUE);

      return deleteFileMsgSize;
   }

   // Verify that the file isn't the current log file.
   if (strstr(TrimaInfo::logFileNameFull(), deleteFile->fileName) == 0)
   {
      // Log event for later debugging.
      DataLog(log_level_trap_info) << "Invalid filename. Can't delete current log file. ("
                                   << deleteFile->fileName << ", "
                                   << TrimaInfo::logFileNameFull() << ")." << endmsg;

      SendAck (hdr, TRAP_BAD_VALUE);

      return deleteFileMsgSize;
   }

   int status = unlink (deleteFile->fileName);

   if (status == ERROR)
   {
      // Log event for later debugging.
      DataLog(log_level_trap_error) << "File " << deleteFile->fileName
                                    << " could not be deleted." << endmsg;

      SendAck (hdr, TRAP_BAD_VALUE);

      return deleteFileMsgSize;
   }

   DataLog(log_level_trap_info) << "File " << deleteFile->fileName << " deleted." << endmsg;

   return deleteFileMsgSize;
}



//
// PrintHeader
//
void TrapProcess::PrintHeader (const char* msg, const trapHeader* hdr)
{
   if (hdr == NULL)
   {
      return;
   }

   datalog_Print(log_handle_trap_info, __FILE__, __LINE__, msg);
   datalog_Print(log_handle_trap_info, __FILE__, __LINE__, "hdr: SOM(%x) MID(%d) SEQ(%d) ST(%d), LEN(%d), T(%x), HC(%lx), BC(%lx)",
                 hdr->som,
                 hdr->messageType,
                 hdr->sequenceNumber,
                 hdr->status,
                 hdr->lengthInBytes,
                 hdr->timestamp,
                 hdr->headerCrc,
                 hdr->bodyCrc);
}



//
// This helper routine validates the read in configuration.
//
bool TrapProcess::ConfigurationInvalid (void)
{
   const int sockAddressSize = sizeof (sockaddr_in);

   // Check existence
   if (_daysOfWeek == -1)
   {
      DataLog(log_level_trap_info) << "No entry found for " << DAYS_OF_WEEK << endmsg;
      return true;
   }

   // Check Range
   if (!ValidDaysOfWeek (_daysOfWeek))
   {
      return true;
   }

   // Check existence
   if (_beginWindow == -1)
   {
      DataLog(log_level_trap_info) << "No entry found for " << BEGIN_WINDOW << endmsg;
      return true;
   }

   // Check existence
   if (_endWindow == -1)
   {
      DataLog(log_level_trap_info) << "No entry found for " << END_WINDOW << endmsg;
      return true;
   }

   // Check Range
   if (!ValidWindow(_beginWindow, _endWindow))
   {
      return true;
   }

   // Check existence
   if (_maxRetries == -1)
   {
      DataLog(log_level_trap_info) << "No entry found for " << NUMBER_OF_ATTEMPTS << endmsg;
      return true;
   }

   // Check Range
   if (!ValidMaxRetries(_maxRetries))
   {
      return true;
   }

   // Check existence
   if (_maxTimeouts == -1)
   {
      DataLog(log_level_trap_info) << "No entry found for " << NUMBER_OF_TIMEOUTS << endmsg;
      return true;
   }

   // Check Range
   if (!ValidMaxTimeouts(_maxTimeouts))
   {
      return true;
   }

   // Check existence
   if (_feature == -1)
   {
      DataLog(log_level_trap_info) << "No entry found for " << FEATURE << endmsg;
      return true;
   }

   // Check Range
   if (!ValidFeature(_feature))
   {
      return true;
   }

   // Check existence
   if (_controlPort == -1)
   {
      DataLog(log_level_trap_info) << "No entry found for " << CONTROL_PORT << endmsg;
      return true;
   }

   // Check Range
   if (!ValidControlPort(_controlPort))
   {
      return true;
   }

   // If Control IP is missing, then fatal out.
   if (!ValidControlIp(_controlIp))
   {
      return true;
   }

   // Check Range
   if (!ValidRequestDelay(_requestDelay))
   {
      return true;
   }


   memset (&_serverAddress, 0, sockAddressSize);

   _serverAddress.sin_family      = AF_INET;
   _serverAddress.sin_addr.s_addr = inet_addr((char*)_controlIp);
   _serverAddress.sin_port        = htons(_controlPort);

   if ( (_serverAddress.sin_addr.s_addr == INADDR_NONE) ||
        (_serverAddress.sin_addr.s_addr == INADDR_ANY) )
   {
      DataLog(log_level_trap_info) << "Invalid entry found for " << CONTROL_IP << ", " << _controlIp << endmsg;
      return true;
   }

   return false;
}



//
// ValidDaysOfWeek
//
bool TrapProcess::ValidDaysOfWeek (int value)
{
   if ( (value < TRAP_NO_DAYS) ||
        (value > TRAP_ALL_DAYS) )
   {
      DataLog(log_level_trap_info) << "Bad value found for " << DAYS_OF_WEEK << ", " << value
                                   << ", expected " << TRAP_NO_DAYS << " - " << TRAP_ALL_DAYS << endmsg;

      return false;
   }
   else
      return true;
}




//
// ValidWindow
//
bool TrapProcess::ValidWindow (int beginWindow, int endWindow)
{
   if ( (beginWindow < TRAP_MIN_WINDOW) ||
        (beginWindow > TRAP_MAX_WINDOW) )
   {
      DataLog(log_level_trap_info) << "Bad value found for " << BEGIN_WINDOW << ", " << _beginWindow
                                   << ", expected " << TRAP_MIN_WINDOW << " - " << TRAP_MAX_WINDOW << endmsg;
      return false;
   }

   if ( (endWindow < TRAP_MIN_WINDOW) ||
        (endWindow > TRAP_MAX_WINDOW) )
   {
      DataLog(log_level_trap_info) << "Bad value found for " << END_WINDOW << ", " << _beginWindow
                                   << ", expected " << TRAP_MIN_WINDOW << " - " << TRAP_MAX_WINDOW << endmsg;
      return false;
   }

   // Check consistency
   if ( (beginWindow > endWindow) || (beginWindow == endWindow) )
   {
      DataLog(log_level_trap_info) << "Bad Begin/End Window pair, " << beginWindow << "/"
                                   << endWindow << "." << endmsg;
      return false;
   }

   return true;
}



//
// ValidMaxRetries
//
bool TrapProcess::ValidMaxRetries (int value)
{
   if ( (value < TRAP_MIN_NUMBER_OF_ATTEMPTS) ||
        (value > TRAP_MAX_NUMBER_OF_ATTEMPTS) )
   {
      DataLog(log_level_trap_info) << "Bad value found for " << NUMBER_OF_ATTEMPTS << ", " << value
                                   << ", expected " << TRAP_MIN_NUMBER_OF_ATTEMPTS << " - "
                                   << TRAP_MAX_NUMBER_OF_ATTEMPTS << endmsg;
      return false;
   }
   else
      return true;
}


//
// ValidMaxTimeouts
//
bool TrapProcess::ValidMaxTimeouts (int value)
{
   if ( (value < TRAP_MIN_NUMBER_OF_TIMEOUTS) ||
        (value > TRAP_MAX_NUMBER_OF_TIMEOUTS) )
   {
      DataLog(log_level_trap_info) << "Bad value found for " << NUMBER_OF_TIMEOUTS << ", " << value
                                   << ", expected " << TRAP_MIN_NUMBER_OF_TIMEOUTS << " - "
                                   << TRAP_MAX_NUMBER_OF_TIMEOUTS << endmsg;
      return false;
   }
   else
      return true;
}


//
// ValidFeature
//
bool TrapProcess::ValidFeature (int value)
{
   if ( (value < TRAP_FEATURE_OFF) || (value > TRAP_FEATURE_ON) )
   {
      DataLog(log_level_trap_info) << "Bad value found for " << FEATURE << ", " << value
                                   << ", expected " << TRAP_FEATURE_OFF << " - "
                                   << TRAP_FEATURE_ON << endmsg;
      return false;
   }
   else
      return true;
}



//
// ValidControlPort
//
bool TrapProcess::ValidControlPort (int value)
{
   if ( (value < TRAP_MIN_PORT) || (value > TRAP_MAX_PORT) )
   {
      DataLog(log_level_trap_info) << "Bad value found for " << CONTROL_PORT << ", " << value
                                   << ", expected " << TRAP_MIN_PORT << " - "
                                   << TRAP_MAX_PORT << endmsg;
      return false;
   }
   else
      return true;
}



//
// ValidControlIp
//
bool TrapProcess::ValidControlIp (const char* value)
{
   if (value[0] == 0)
   {
      DataLog(log_level_trap_info) << "Null entry found for " << CONTROL_IP << endmsg;
      return false;
   }
   else
      return true;
}



//
// ValidRequestDelay
//
bool TrapProcess::ValidRequestDelay (int value)
{
   if ( (value < TRAP_MIN_REQUEST_DELAY) || (value > TRAP_MAX_REQUEST_DELAY) )
   {
      DataLog(log_level_trap_info) << "Bad value found for " << REQUEST_DELAY << ", " << value
                                   << ", expected " << TRAP_MIN_REQUEST_DELAY << " - "
                                   << TRAP_MAX_REQUEST_DELAY << endmsg;
      return false;
   }
   else
      return true;
}



//
// Send Method.
//
void TrapProcess::Send (const trapHeader* hdr, int size)
{
   int nbytes;

   if ( (nbytes = send (_connectSocket, (char*)hdr, size, 0) ) != size)
   {
      DataLog(log_level_trap_error) << "Msg Id (" << hdr->messageType << ") Failed to send!(" << nbytes
                                    << ", " << size << ")" << endmsg;
   }
}



//
// SendReply
//
void TrapProcess::SendReply (trapHeader* hdr, int size)
{
   CTrapIf::prepareMsg (hdr, hdr->messageType, hdr->sequenceNumber, TRAP_OK, size);
   TrapProcess::Send(hdr, size);
}



//
// SendAck
//
void TrapProcess::SendAck (const trapHeader* hdr, int status)
{
   if (hdr == NULL)
   {
      DataLog(log_level_trap_error) << "tried to send a null message header" << endmsg;
      return;
   }

   trapHeader reply = *hdr;
   CTrapIf::prepareMsg (&reply, reply.messageType, reply.sequenceNumber, status, trapHeaderLength);
   TrapProcess::Send(&reply, trapHeaderLength);
}



//
// LogFileContents
//
void TrapProcess::LogFileContents (const char* filename)
{
   DataLog (log_level_trap_info) << "File: " << filename << " Config read: DAY(" << _daysOfWeek << "), BW("
                                 << _beginWindow << "), EW(" << _endWindow << "), MR(" << _maxRetries << "), MT("
                                 << _maxTimeouts << "), F(" << _feature << "), CIP(" << _controlIp << "), CP("
                                 << _controlPort << "), RD(" << _requestDelay << ")" << endmsg;
}

/* FORMAT HASH e6c19631d4b1a4fe5ba5825b8d3a00c6 */
