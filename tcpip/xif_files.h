/*******************************************************************************
 *
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      xif_files.h
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file implements common file handling routines used by external
 *             interfaces
 *
 ******************************************************************************/



#ifndef __XIF_FILE_IO__
#define __XIF_FILE_IO__

#include <netinet/if_ether.h>   // Get Network MTU size

#include "xif_cds.h"            // External IF CDS Class (Donor State, etc.)
#include "trima_datalog.h"      // ID for error logging

class XIF_FileIo
{

public:  // Type defn's
   enum STATUS
   {
      SUCCESS,
      BAD_SEARCH_PATTERN,
      BAD_DIR_NAME,
      BAD_TEMP_FILE
   };


public:  // Public methods

   //
   // Use this constructor for:
   //    getting a file list that will be sent back on a socket.  File name is a temp file.
   //    sending a file contents on a socket.  File name is file to send.
   //
   //
   XIF_FileIo(const char* fileName, int sendSocket, DataLog_Level& info, DataLog_Level& error, bool checkForDonor = false);

   //
   // Use this constructor for just doing file operations (no sockets).  File name is temp file.
   //
   XIF_FileIo(const char* fileName, DataLog_Level& info, DataLog_Level& error);


   virtual ~XIF_FileIo();


   //
   // Gets a list of files given a directory name and a regular expression.
   // Result is stored in the temp file passed in to the constructor.
   // From here, you can call countFileList or any of the send routines
   //
   STATUS getFileList (const char* directoryName, const char* pattern, int& fileSizeInBytes, unsigned long& crc);

   //
   // File Iterators
   // Note:  These iterators do not work in conjunction with other methods.
   //

   // Open temp file
   bool openFile ();

   // Close temp file
   void closeFile ();

   // Gets the next file from a successful getFileList call. Returns 0 on EOF
   const char* getNextFile (int& fileSize);


   //
   // This routine counts the number of matching files in the retrieved file list.
   // Returns either a -1 if operation failed or else the size of the buffer that
   // will be used in the reply message
   //
   int countFileList (int sequenceNumber, unsigned long& crcValue);

   //
   // Sends the list of matching files from the getFileList call on the supplied socket
   //
   bool sendFileList ();

   //
   // Sends the list of files greater than the sequence number from the getFileList call on
   // the supplied socket.
   //
   bool sendFileList (int sequenceNumber);

   //
   // Send the contents of a file on the supplied socket.
   //    First get the filesize and file crc, then send the header.
   //    Next, call this function to transmit the contents of the file.
   //
   bool sendFile (const char* filename);

   //
   // Set delay mode (on for Cadence, off for Vista)
   //
   void turnOnDelay (void){ _delayOn = true; }
   void turnOffDelay (void) { _delayOn = false; }
   bool delayOn (void) const { return _delayOn; }


private:  // Class methods
   XIF_FileIo()
      : _xifCds(ROLE_RO),
        _buffer(NULL),
        _sendSocket(0),
        _checkRestrictedAccess(false),
        _info(log_level_vip_info),
        _error(log_level_vip_error),
        _delayOn(false),
        _fileHandle(NULL)
   {}

   XIF_FileIo(const XIF_FileIo&)
      : _xifCds(ROLE_RO),
        _buffer(NULL),
        _sendSocket(0),
        _checkRestrictedAccess(false),
        _info(log_level_vip_info),
        _error(log_level_vip_error),
        _delayOn(false),
        _fileHandle(NULL)
   { }

   XIF_FileIo& operator = (const XIF_FileIo& rhs);

   bool TerminateImmediately () const;

   void delayTransfer (int ticks) { if (_delayOn) taskDelay (ticks); }


private:  // Class members
   External_IF_CDS _xifCds;
   string          _tempFileName;

   char*           _buffer;
   int             _sendSocket;

   bool            _checkRestrictedAccess;

   DataLog_Level&  _info;
   DataLog_Level&  _error;

   bool            _delayOn;

   FILE*           _fileHandle;
};

#endif

/* FORMAT HASH 12e50893327b41861793d82c40e3954c */
