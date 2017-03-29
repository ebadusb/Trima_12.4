/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      trap.h
 *             TRAP Process
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file implements the TRAP protocol.  It allows the TRAP server
 *             to request files from TRIMA remotely.
 *
 * REVISION HISTORY:
 * $Log: trap.h $
 * Revision 1.14  2004/01/06 23:02:12Z  rm70006
 * IT 6741.  Fixed bug where invalid IP address is not rejected.
 * Revision 1.13  2003/10/03 16:49:51Z  rm70006
 * IT 6463.  Fix problem where Trap reboots trima.
 * Revision 1.12  2003/09/30 22:01:28Z  rm70006
 * IT 6472.  Fixed problem where files were being downloaded after donor connected.
 * Revision 1.11  2003/09/22 22:10:11Z  rm70006
 * IT 6412.  Fixed problem where I was not handling a recv of 0 bytes well.
 * Revision 1.10  2003/09/04 20:40:15Z  rm70006
 * IT 5832.  Break out functions to create trap override crc file.
 * Revision 1.9  2003/08/27 20:30:09Z  rm70006
 * IT 5832.  Check in working version of Trap.
 * Revision 1.8  2003/08/26 20:27:25Z  rm70006
 * IT 5832.  Clean up functions to use consts more often.
 * Revision 1.7  2003/04/02 22:29:47Z  rm70006
 * IT 5832.
 *
 * Changed logging to match new spec.
 * Revision 1.6  2003/01/31 19:12:46Z  rm70006
 * Get trap to run.  Still need to test it.
 * Revision 1.5  2002/10/28 21:19:55Z  rm70006
 * Interim checkin.
 * Revision 1.4  2002/06/07 14:57:44Z  rm70006
 * First cut at port of Trap.
 * Revision 1.3  2002/01/16 20:08:43Z  rm70006
 * IT 5538.
 * Ported code from Vista.  Now directory requests are file based.
 * Revision 1.2  2002/01/14 20:01:59Z  rm70006
 * IT 5507.
 * Add code to keep TRAP alive if connection attempt fails.
 * Revision 1.1  2002/01/08 23:57:15Z  rm70006
 * Initial revision
 * Revision 1.4  2001/12/20 16:47:41  rm70006
 * IT 5508.
 * Remove Request File List by SN message.
 * Revision 1.3  2001/12/14 21:57:33  rm70006
 * IT 5498.
 * Add new routines for better handling of errors on config read.
 * Revision 1.2  2001/12/05 22:27:32  rm70006
 * It 5466.
 * Added delete file message to interface.
 * Revision 1.1  2001/12/04 19:38:57  rm70006
 * Initial revision
 *
 ******************************************************************************/

#ifndef INCLUDE_TRAP
#define INCLUDE_TRAP

#include <strstream.h>


#include "trapif.h"
#include "timewindow.h"
#include "time.h"
#include "xif_cds.h"
#include "proctrimaset_cds.h"
#include "trima_info.h"
#include "ostime.hpp"



class TrapProcess
{
// Member Functions
public:

   TrapProcess();
   virtual ~TrapProcess();

   // Execute the Trap Client process.
   void Execute (void);

private:
   TrapProcess& operator = (const TrapProcess& rhs);
   TrapProcess(const TrapProcess&);

// Member Functions
private:
   //
   // Initialize the class.
   //
   bool Init (void);


   //
   // Block on the receive socket and process the received messages.
   //
   void Process (void);


   //
   // Cleanup does a gracefull shutdown of the class.
   // Attached in the main program to handle SIGINT, SIGPWR
   //
   void Cleanup (int sig = 0);


   //
   // This routine handles the validation, execution and response
   // of all the Commands sent by Vista.
   //
   int ProcessMessage (const char* msg, int sizeInBytes);


   //
   // This routine processes the Set Trima Time command.
   //
   int SetTrimaTime (const void* msg, int sizeInBytes);

   //
   // This routine processes the Request File List command.
   //
   int RequestFileList (const void* msg, int sizeInBytes);

   //
   // This routine processes the Request File List By Seq Num command.
   //
   int RequestFileListBySn (const void* msg, int sizeInBytes);

   //
   // This routine processes the Upload File command.
   //
   int UploadFile (const void* msg, int sizeInBytes);

   //
   // This routine processes the Set Alt Contol Server command.
   //
   int SetAltControlServer (const void* msg, int sizeInBytes);

   //
   // This routine processes the Set Connect Time command.
   //
   int SetConnectTime (const void* msg, int sizeInBytes);

   //
   // This routine processes the Get Trima Info file command.
   //
   int GetTrimaInfo (const void* msg, int sizeInBytes);

   //
   // This routine processes the Disconnect command.
   //
   int Disconnect (const void* msg, int sizeInBytes);

   //
   // This routine processes the Delete File command.
   //
   int DeleteFile (const void* msg, int sizeInBytes);

   //
   // This routine processes the Set Connect Time command.
   //
   int SetRequestDelay (const void* msg, int sizeInBytes);

   //
   // This helper routine opens and checks the TRAP_OVERRIDE file.
   //
   bool OpenOverrideFile ();

   //
   // This helper routine opens and checks the TRAP_DEFAULT file.
   //
   bool OpenDefaultsFile ();

   //
   // This helper routine validates the read in configuration.
   //
   bool ConfigurationInvalid ();

   //
   // This helper routine prints out the message header.
   //
   void PrintHeader (const char* msg, const trapHeader* hdr);

   //
   // These helper routine validate the configurable items.
   //
   bool ValidDaysOfWeek  (int value);
   bool ValidWindow      (int beginWindow, int endWindow);
   bool ValidMaxRetries  (int value);
   bool ValidMaxTimeouts (int value);
   bool ValidFeature     (int value);
   bool ValidControlPort (int value);
   bool ValidControlIp   (const char* value);
   bool ValidRequestDelay (int value);


   void Send (const trapHeader* hdr, int size);
   void SendReply (trapHeader* hdr, int size);
   void SendAck (const trapHeader* hdr, int status);

   void LogFileContents (const char* filename);

// Data Members
private:
   bool _tryAgainLater;

   // logging string storage
   ostrstream _logStr;

   // Connect socket for TRAP I/F
   // The Connect socket is a TCP blocking client socket.  It establishes a connection to the TRAP Server.
   int _connectSocket;

   // Socket struct for server address.
   sockaddr_in _serverAddress;

   int         _txSequenceNumber;

   // Feature set from config file
   int              _daysOfWeek;                 // Day(s) which connection will be attempted.
   int              _beginWindow;                // Time of day to start
   int              _endWindow;                  // Time of day to end
   int              _maxRetries;                 // Number of retries per session.
   int              _maxTimeouts;                // Number of timeouts per session
   int              _feature;                    // Turn on or off interface.
   int              _requestDelay;               // Seconds to delay between Cadence commands
   const char*      _controlIp;                  // Control Server IP Address
   unsigned short   _controlPort;                // Control Server Port #
   int              _requestDelayInMilliSeconds; // milliseconds to delay between Cadence commands

   TimeWindow*      _timeWindow;

   External_IF_CDS  _xifCds;
   ProcTrimaSet_CDS _procTrimaSetCds;

   rawTime          _lastRequestTime; // Meter requests.  Do not allow more than 1 per second
};

#endif

/* FORMAT HASH d812f20dcf6d8e9f588206999b0f6467 */
