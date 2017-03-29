/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      trapif.h
 *             TRIMA REMOTE ACCESS PROTOCOL
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface for the TRAP protocol
 *
 *
 * HISTORY:    $Log: trapif.h $
 * HISTORY:    Revision 1.3  2003/10/08 18:32:58Z  rm70006
 * HISTORY:    IT 6463.  Add better logging.
 * HISTORY:    Revision 1.2  2002/04/09 16:50:07Z  rm70006
 * HISTORY:    Initial port to VxWorks comlete.  Code compiles.  Changed SOM to be unsigned.
 * HISTORY:    Revision 1.1  2002/01/08 23:57:05Z  rm70006
 * HISTORY:    Initial revision
 * HISTORY:    Revision 1.2  2001/12/05 22:28:33  rm70006
 * HISTORY:    IT 5466.
 * HISTORY:    Added delete file message to interface.
 * HISTORY:    reduced max buffer size.
 * HISTORY:    Revision 1.1  2001/12/04 22:47:09  rm70006
 * HISTORY:    Initial revision
 *
 ******************************************************************************/
#ifndef __TRAPIF__
#define __TRAPIF__



#include <sys/types.h>
#include <time.h>

#ifndef WIN32
#include <netinet/in.h>
#include "datalog.h"
#endif


// Define the SOM value
#define TRAP_SOM_ID (0xCAFEBABEL)

typedef struct
{
   unsigned int  som;               // Start of Message identifier
   int           messageType;       // Message Type
   int           sequenceNumber;    // Number generated by Vista to correlate commands to responses
   int           status;            // Used by Trima only.  Indicates success or failure of commands
   int           lengthInBytes;     // Length of body message in bytes (excludes header)
   int           timestamp;         // Time message was sent (used for recording / playback only).
   unsigned long bodyCrc;           // CRC computed against entire message (header + body)
   unsigned long headerCrc;         // CRC computed against message header.
} trapHeader;


const int trapHeaderLength = sizeof (trapHeader);


const int TRAP_OK                 =  1;
const int TRAP_BAD_START_OF_MSG   =  2;
const int TRAP_HEADER_CRC_INVALID =  3;
const int TRAP_MSG_CRC_INVALID    =  4;
const int TRAP_BAD_LENGTH         =  5;
const int TRAP_NOT_PRIVILEGED     =  6;
const int TRAP_CASSETTE_DOWN      =  7;
const int TRAP_MESSAGE_ID_INVALID =  8;
const int TRAP_BUFFER_UNDERRUN    =  9;
const int TRAP_BUFFER_OVERRUN     = 10;
const int TRAP_BAD_VALUE          = 11;
const int TRAP_ILLEGAL_FORMAT     = 12;
const int TRAP_FAILED_TO_COMMIT   = 13;
const int TRAP_PFR_NOT_COMPLETE   = 14;


#define TRAP_MAX_TRIMA_MSG_SIZE 1024

const int trapMsgLength = TRAP_MAX_TRIMA_MSG_SIZE - sizeof(trapHeader);

typedef char trapBuffer[trapMsgLength];

//
// Define the standard message header
//
typedef struct
{
   trapHeader hdr;
   trapBuffer buf;
} trapMsg;



//
// Command Messages
//
const int TRAP_FIRST_MSG               = 501;
const int TRAP_SET_LOCAL_TIME          = TRAP_FIRST_MSG;
const int TRAP_REQUEST_FILE_LIST       = 502;
const int TRAP_REQUEST_FILE_LIST_BY_SN = 503;
const int TRAP_UPLOAD_FILE             = 504;
const int TRAP_SET_ALT_CONTROL_SERVER  = 505;
const int TRAP_SET_CONNECT_TIME        = 506;
const int TRAP_GET_TRIMA_INFO          = 507;
const int TRAP_DISCONNECT              = 508;
const int TRAP_DELETE_LOG_FILE         = 509;
const int TRAP_SET_REQUEST_DELAY       = 510;
const int TRAP_LAST_MSG                = TRAP_SET_REQUEST_DELAY;


//
// Common Message definitions.
//

#define IP_ADDRESS_SIZE 16



#define TRAP_MIN_PORT     1
#define TRAP_MAX_PORT 65535

//
// Set Alternate Control Server IP
//
typedef struct
{
   trapHeader hdr;
   char           serverIp[IP_ADDRESS_SIZE]; // null terminated.
   unsigned short portId;                    // Range 1 - 65535
} setAltControlServerMsg;

const int setAltControlServerMsgSize = sizeof (setAltControlServerMsg);



// Days of Week
enum TRAP_DAYS_OF_WEEK
{
   TRAP_NO_DAYS   =   0,
   TRAP_MONDAY    =   1,
   TRAP_TUESDAY   =   2,
   TRAP_WEDNESDAY =   4,
   TRAP_THURSDAY  =   8,
   TRAP_FRIDAY    =  16,
   TRAP_SATURDAY  =  32,
   TRAP_SUNDAY    =  64,
   TRAP_ALL_DAYS  = 127
};




#define TRAP_MIN_WINDOW  0
#define TRAP_MAX_WINDOW 23

#define TRAP_MIN_NUMBER_OF_ATTEMPTS  1
#define TRAP_MAX_NUMBER_OF_ATTEMPTS 10


#define TRAP_MIN_NUMBER_OF_TIMEOUTS   1
#define TRAP_MAX_NUMBER_OF_TIMEOUTS 100

#define TRAP_FEATURE_OFF 0
#define TRAP_FEATURE_ON  1

//
// Set Connect Time Message
//
typedef struct
{
   trapHeader        hdr;
   TRAP_DAYS_OF_WEEK daysOfWeek;         // Which day(s) the Trima will try to connect to the TRAP server.
   int               startWindow;        // Beginning of connection window range (0 - 23 hours)
   int               endWindow;          // End of connection window range (0 - 23 hours)
   int               numberOfAttempts;   // Number of attempts to be made during the connect window per day (1 - 10)
   int               numberOfTimeouts;   // Number of 30 second socket timeouts to be allowed before link is terminated (0 - 100).
   int               featureOnOff;       // 0 = OFF, 1 = ON
} setConnectTimeMsg;

const int setConnectTimeMsgSize = sizeof (setConnectTimeMsg);



//
// Set Local Time Msg
//
typedef struct
{
   trapHeader hdr;
   time_t     localTime;    // Local Time in UTC.
} setLocalTimeMsg;

const int setLocalTimeMsgSize = sizeof (setLocalTimeMsg);


//
// Request File List Msg
//
#define TRAP_MAX_REPLY_BUFFER_SIZE 4096  // 4K

// Max defined by QNX
#define TRAP_MAX_PATHNAME_LENGTH 256
#define TRAP_MAX_PATTERN_LENGTH   49

typedef struct
{
   trapHeader hdr;
   char       pathName[TRAP_MAX_PATHNAME_LENGTH];
   char       pattern[TRAP_MAX_PATTERN_LENGTH];
} requestFileListMsg;

const int requestFileListMsgSize = sizeof (requestFileListMsg);


#define TRAP_MAX_SEQUENCE_NUMBER 999999

//
// Request File List Msg By Sequence Number
//
typedef struct
{
   trapHeader hdr;
   char       directoryName[TRAP_MAX_PATHNAME_LENGTH];
   int        sequenceNumber;
} requestFileListBySeqNumMsg;

const int requestFileListBySeqNumMsgSize = sizeof (requestFileListBySeqNumMsg);


//
// Upload Log File Msg
//

#define TRAP_MAX_FILENAME_LENGTH 81

typedef struct
{
   trapHeader hdr;
   char       fileName[TRAP_MAX_FILENAME_LENGTH];
} uploadFileMsg;

const int uploadFileMsgSize = sizeof (uploadFileMsg);



//
// Disconnect Message
//
typedef struct
{
   trapHeader hdr;
} disconnectMsg;

const int disconnectMsgSize = sizeof (disconnectMsg);



//
// Get Trima Info Message
//
typedef struct
{
   trapHeader hdr;
} getTrimaInfoMsg;

const int getTrimaInfoMsgSize = sizeof (getTrimaInfoMsg);

#define TRAP_MAX_LOG_FILE_SIZE        32
#define TRAP_MAX_CRC_SIZE             16
#define TRAP_MAX_RELEASE_SIZE         16
#define TRAP_MAX_BUILD_SIZE           16
#define TRAP_MAX_SEQUENCE_NUMBER_SIZE  7

typedef struct
{
   trapHeader hdr;
   char       trimaLogFile[TRAP_MAX_LOG_FILE_SIZE];          // T00015_0504_0033
   char       trimaCRC[TRAP_MAX_CRC_SIZE];                   // 0xadcef73
   char       trimaRelease[TRAP_MAX_RELEASE_SIZE];           // Trima 3.3a
   char       trimaBuild[TRAP_MAX_BUILD_SIZE];               // 1.191
   char       sequenceNumber[TRAP_MAX_SEQUENCE_NUMBER_SIZE]; // 000000
} getTrimaInfoReplyMsg;

const int getTrimaInfoReplyMsgSize = sizeof (getTrimaInfoReplyMsg);



//
// Delete File Msg
//

typedef struct
{
   trapHeader hdr;
   char       fileName[TRAP_MAX_FILENAME_LENGTH];
} deleteFileMsg;

const int deleteFileMsgSize = sizeof (deleteFileMsg);


//
// SetRequestDelay

#define TRAP_MIN_REQUEST_DELAY 0
#define TRAP_MAX_REQUEST_DELAY 30


typedef struct
{
   trapHeader hdr;
   int        requestDelay;  // Number of seconds to delay between Cadence commands (0 - 5)
} setRequestDelayMsg;

const int setRequestDelayMsgSize = sizeof (setRequestDelayMsg);





class CTrapIf
{
public:

   CTrapIf();

   virtual ~CTrapIf();

   //
   //
   // This routine checks to see that the message is valid.  The following
   // Checks are made:
   // 1.  Check that the message header CRC is correct
   // 2.  Check that the SOM is correct
   // 3.  Check that the message ID is a valid range
   // 4.  Check that the message length is valid
   // 5.  Check that the message body checksum is correct
   //
   // Usage:
   //    (1) pass in pointer to message buffer and the size received.
   //    (2) routine will return 0 if message passed validation.  Otherwise
   //        routine will pass back the appropriate response status code.  Also,
   //        routine will set a descriptive text string that can be gotten by calling
   //        errorString;
   //    (3) In the case that the message passed in does not have enough bytes to satisfy the
   //        message header, the value of TRAP_BUFFER_INCOMPLETE will be returned.  Assuming
   //        that the message length is valid, the message should be stored and glued with the
   //        next received piece and revalidated.
   //
   static int validHeader (void const* msg, int sizeInBytes);


   //
   // This routine prepares the TRAP message for transmission.
   // CRC's are calculated and all of the message fields are properly filled in.
   //
   static void prepareMsg (trapHeader* hdr, int messageId, int sequenceNumber, int status = 0, int sizeInBytes = trapHeaderLength);


   //
   // Returns the last generated error string.
   //
   static char* errorString (void);


   //
   // Returns the current sequence number.
   //
   static int sequenceNumber (void);

#ifndef WIN32
   friend DataLog_Stream& operator << (DataLog_Stream& os, const TRAP_DAYS_OF_WEEK day);
#endif

private:
   static char m_ErrorString[256];
};

#endif

/* FORMAT HASH 525688dcdf08c88c0d874f274663f09e */
