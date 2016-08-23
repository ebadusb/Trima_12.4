/*******************************************************************************
 *
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      xif_files.cpp
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file implements common file handling routines used by external
 *             interfaces
 *
 ******************************************************************************/


#include <vxworks.h>          // Needed for some system include files
#include <stat.h>             // File stat routines
#include <dirent.h>           // Directory fiile parse routines
#include <stdio.h>            // File IO
#include <fstream.h>          // Stream file IO
#include <ioLib.h>            // File IO
#include <time.h>

#include "readdir_stat.h"     // Directory utilities
#include "trima_info.h"       // Current log file name

extern "C" {
#include <regex.h>            // Regular Expression routines
}


#include "crcgen.h"           // CRC Gen tools

#include "xif_files.h"


// 1TNNNNN_YYYYMMDD_XXX_SSSSSS.dlog
// Note:  the Log File Size is comprised of 33 character file name (including null) and 4 byte file size.
const int _LogFileBlockSize    = 37;

const int _LogFileSizeIndex    = 33;
const int _SequenceNumberIndex = 21;

// Sequence number component of the file
const int _SequenceNumberSize = 6;

const int BUFFERSIZE          = ETHERMTU;


//
// Constructor
//
XIF_FileIo::XIF_FileIo(const char* fileName, int sendSocket, DataLog_Level& info, DataLog_Level& error, bool checkRestrictedAccess)
   : _tempFileName(fileName),
     _xifCds(ROLE_RO),
     _buffer(new char[BUFFERSIZE]),
     _sendSocket(sendSocket),
     _checkRestrictedAccess(checkRestrictedAccess),
     _info(info),
     _error(error),
     _delayOn(false),
     _fileHandle(0)
{}



//
// Constructor
//
XIF_FileIo::XIF_FileIo(const char* fileName, DataLog_Level& info, DataLog_Level& error)
   : _tempFileName(fileName),
     _xifCds(ROLE_RO),
     _buffer(new char[BUFFERSIZE]),
     _sendSocket(-1),
     _checkRestrictedAccess(false),
     _info(info),
     _error(error),
     _delayOn(false),
     _fileHandle(0)
{}



//
// Destructor
//
XIF_FileIo::~XIF_FileIo()
{
   delete [] _buffer;
   _buffer = 0;

   if (_fileHandle)
   {
      fclose(_fileHandle);
   }
}


XIF_FileIo& XIF_FileIo::operator = (const XIF_FileIo& rhs)
{
   if (this == &rhs)
   {
      return *this;
   }

   return *this;
}

//
// Check if restricted access
//
// Used for TRAP and Vista.  For Ampro machines, any File IO can overload the
// CPU.  Be conservative.
//
bool XIF_FileIo::TerminateImmediately () const
{
   if (_checkRestrictedAccess)
      return _xifCds.restrictedAccess (XIF_CASSETTE_LOWERED | XIF_DONOR_COMMITTED | XIF_DONOR_DOWNLOADED);
   else
      return false;
}



//
// Gets a list of files given a directory name and a regular expression
//
XIF_FileIo::STATUS XIF_FileIo::getFileList (const char* directoryName, const char* pattern, int& fileSizeInBytes, unsigned long& crc)

{
   DIR*           directory = NULL;
   struct dirent* dirEntry  = NULL;

   regex_t        expressionTable;

   struct stat    fileStat;

   int            fileSize;
   int            errorCode;
   int            numFilesFound = 0;

   FILE*          outFile       = NULL;


   errorCode = regcomp (&expressionTable, pattern, REG_NOSUB | REG_EXTENDED);

   // Compute the regular expression table.
   if (errorCode != 0)
   {
      char msg[128];
      regerror (errorCode, &expressionTable, msg, 128);
      DataLog(_error) << "regcomp failed on " << pattern << ", " << msg << endmsg;

      regfree (&expressionTable);

      return BAD_SEARCH_PATTERN;
   }

   // Open the requested direcory
   directory = opendir ((char*)directoryName);

   if (directory == NULL)
   {
      DataLog(_error) << "opendir failed on " << directoryName << endmsg;

      // Don't call closedir here.  QNX will core!
      regfree (&expressionTable);

      return BAD_DIR_NAME;
   }

   // Open the file for write.
   outFile = fopen (_tempFileName.c_str(), "w");

   if (outFile == NULL)
   {
      DataLog(_error) << "File " << _tempFileName.c_str() << " could not be opened" << endmsg;

      // Don't call closedir here.  QNX will core!
      regfree (&expressionTable);
      closedir (directory);

      return BAD_TEMP_FILE;
   }

   fileSizeInBytes = 0;

   // Match each entry against the search pattern
   for (;; )
   {
      // Get the next directory entry.
      dirEntry = readdir_stat(directory, &fileStat);

      if (dirEntry == NULL)
      {
         // List empty, exit
         break;
      }

      // If one of the entries is the current file, ignore it.
      // Trima log name call doesn't include seq num so substring search for it.
      if (strstr(dirEntry->d_name, TrimaInfo::logFileNameFull()) != 0)
      {
         continue;
      }

      const int fileNameSize = strlen (dirEntry->d_name) + 1;

      // Did the file name match the regular expression?
      if (regexec (&expressionTable, dirEntry->d_name, 0, NULL, 0) == 0)
      {
         fileSize = fileStat.st_size;

         // Add both to the stream
         fprintf(outFile, "%s\n%d\n", dirEntry->d_name, fileSize);

         const int fileNameSize = strlen(dirEntry->d_name) + 1;  // include the null char

         // Compute the CRC's.
         crcgen32(&crc, (unsigned char*)dirEntry->d_name, fileNameSize);
         crcgen32(&crc, (unsigned char*)&fileSize, sizeof(fileSize));

         numFilesFound++;
         fileSizeInBytes += fileNameSize + sizeof(int);

         delayTransfer(1);
      }
   }

   regfree (&expressionTable);
   closedir (directory);

   fclose (outFile);

   DataLog(_info) << "found " << dec << numFilesFound << " files in " << directoryName
                  << "/" << pattern << ".  Temp file size is " << fileSizeInBytes << " bytes.  CRC is "
                  << hex << crc << dec << endmsg;

   return SUCCESS;
}



//
// countFileList
//
int XIF_FileIo::countFileList (int sequenceNumber, unsigned long& crcValue)
{
   char        temp[_SequenceNumberSize + 1];

   const char* filename = NULL;

   int         fileSize;
   int         fileSequenceNumber;

   // Counters
   int bufferCount = 0;
   int fileCount   = 0;

   //
   // First, scan the file and figure out how big the return packet will be.
   // Create the reply message
   //

   // open the file
   if (!openFile())
      return 0;

   // Scan the returned list of files and count the number of bytes that will be in the returned buffer.
   for (;; )
   {
      // Get the file name and size fields.
      filename = getNextFile(fileSize);

      if (filename == 0)
      {
         break;
      }

      // Grab the 6 digit sequence number from the file name.
      // Note that these operations are assumed to be safe since the earlier search should
      // only return filenames that match the expected pattern.
      strncpy (temp, &filename[_SequenceNumberIndex], _SequenceNumberSize);
      temp[_SequenceNumberSize] = 0;
      fileSequenceNumber        = atoi(temp);

      if (fileSequenceNumber >= sequenceNumber)
      {
         // DataLog(_info) << "RKM:  Found file " << filename << ", SN " << fileSequenceNumber << endmsg;

         // Count it.
         const int fileNameSize = strlen(filename) + 1;   // Include the null char

         bufferCount += fileNameSize + sizeof(fileSize);
         fileCount++;

         // Compute the CRC's.
         crcgen32(&crcValue, (unsigned char*)filename, fileNameSize);
         crcgen32(&crcValue, (unsigned char*)&fileSize, sizeof(fileSize));
      }

      delayTransfer(1);
   }

   closeFile();

   DataLog(_info) << "Counted " << fileCount << " files, byte count " << bufferCount << ", CRC " << hex << (unsigned long)(crcValue ^ INITCRC_DEFAULT) << dec << endmsg;

   return bufferCount;
}



//
// SendFileList
//
bool XIF_FileIo::sendFileList ()
{
   const char* filename = NULL;
   int         fileSize;

   // Counters
   int nbytes;
   int totalBytesSent = 0;


   //
   // First, scan the file and figure out how big the return packet will be.
   // Create the reply message
   //

   // Open the dir list file
   if (!openFile())
   {
      return false;
   }

   // Scan the returned list of files and count the number of bytes that will be in the returned buffer.
   for (;; )
   {
      // If Donor Connected, terminate link
      if (TerminateImmediately())
      {
         closeFile();

         // Log event for later debugging.
         DataLog(_info) << "Donor Connected.  Exiting" << endmsg;

         return false;
      }

      // Get the file name and size fields.
      filename = getNextFile(fileSize);

      if (filename == 0)
      {
         break;
      }
      else
      {
         // DataLog(_info) << "RKM: Adding file" << filename << ", size " << fileSize << endmsg;

         const int filenameSize = strlen(filename) + 1;

         // Send the file name
         if ( (nbytes = send(_sendSocket, filename, filenameSize, 0)) != filenameSize)
         {
            // If one fails, they will all probably fail
            DataLog(_error) << "sendFileList failed to send!(" << nbytes << " != " << filenameSize << ").  Exiting." << endmsg;

            closeFile();
            return false;
         }

         totalBytesSent += nbytes;

         // Send the file size
         if ( (nbytes = send(_sendSocket, (char*)&fileSize, sizeof(int), 0)) != sizeof(int))
         {
            // If one fails, they will all probably fail
            DataLog(_error) << "sendFileList failed to send!(" << nbytes << " != " << sizeof(int) << ").  Exiting." << endmsg;

            closeFile();
            return false;
         }

         totalBytesSent += nbytes;
      }

      delayTransfer(1);
   }

   closeFile();

   DataLog(_info) << "Sent " << totalBytesSent << " bytes of message data." << endmsg;

   return true;
}



//
// SendFileList
//
bool XIF_FileIo::sendFileList (int sequenceNumber)
{
   char        buffer[_LogFileBlockSize];
   char        temp[_SequenceNumberSize + 1];

   const char* filename = NULL;

   int         size;
   int         fileSequenceNumber;

   // Counters
   int bufferCount = 0;
   int fileCount   = 0;
   int nbytes;

   //
   // First, scan the file and figure out how big the return packet will be.
   // Create the reply message
   //

   if (!openFile())
   {
      return false;
   }

   // Scan the returned list of files and count the number of bytes that will be in the returned buffer.
   for (;; )
   {
      // If Donor Connected, terminate link
      if (TerminateImmediately())
      {
         closeFile();

         // Log event for later debugging.
         DataLog(_info) << "Donor Connected.  Exiting" << endmsg;

         return false;
      }

      // Get the file name and size fields.
      filename = getNextFile(size);

      if (filename == 0)
         break;

      // Grab the 6 digit sequence number from the file name.
      // Note that these operations are assumed to be safe since the earlier search should
      // only return filenames that match the expected pattern.
      strncpy (temp, &filename[_SequenceNumberIndex], _SequenceNumberSize);
      temp[_SequenceNumberSize] = 0;
      fileSequenceNumber        = atoi(temp);

      if (fileSequenceNumber >= sequenceNumber)
      {
         strncpy(buffer, filename, _LogFileSizeIndex);
         memcpy(&buffer[_LogFileSizeIndex], &size, sizeof(int));

         // DataLog(_info) << "RKM: Sending file " << filename << ", SN " << fileSequenceNumber << endmsg;

         // Count it.
         bufferCount += strlen(filename) + 1 + sizeof(size);  // include null char
         fileCount++;

         // Send it.
         if ( (nbytes = send(_sendSocket, buffer, _LogFileBlockSize, 0)) != _LogFileBlockSize)
         {
            // If one fails, they will all probably fail
            DataLog(_error) << filename << " failed to send!(" << nbytes << "!=" << _LogFileBlockSize << ").  Exiting." << endmsg;

            closeFile();
            return false;
         }
      }

      delayTransfer(1);
   }

   closeFile();

   DataLog(_info) << "Sent " << fileCount << " files, size " << bufferCount << endmsg;

   return true;
}



//
// sendFile
//
bool XIF_FileIo::sendFile (const char* filename)
{
   static const timespec timeDelay  = {1, 0};
   static const int      freqDelay  = 1000000 / BUFFERSIZE;

   int                   readSize   = 0;
   int                   nbytes     = 0;
   int                   numPackets = 0;

   // Create the reply message
   const int fileHandle = open(filename, O_RDONLY,  DEFAULT_FILE_PERM);

   // This should not happen.
   // Put here for completeness and paranoia.
   if (fileHandle == ERROR)
   {
      // Log event for later debugging.
      DataLog(_error) << "Could not open file " << filename << ".  Errno = " << errnoMsg << endmsg;

      return false;
   }

   // Process the file stream one block at a time.
   do
   {
      if (TerminateImmediately())
      {
         close(fileHandle);

         // Log event for later debugging.
         DataLog(_info) << "Donor Connected.  Exiting" << endmsg;

         return false;
      }

      readSize = read(fileHandle, _buffer, BUFFERSIZE);

      // DataLog(_info) << "RKM: File " << filename << " sent " << readSize << " bytes." << endmsg;

      if (readSize > 0)
      {
         // Send Response packet back to Vista.
         if ( (nbytes = send(_sendSocket, _buffer, readSize, 0)) != readSize)
         {
            DataLog(_error) << "file " << filename << ") Failed to send!("
                            << nbytes << " " << readSize << ") - " << errnoMsg << endmsg;

            if (nbytes == -1)
            {
               DataLog(_error) << "socket closed.  Aborting file " << filename << endmsg;
               close(fileHandle);
               return false;
            }
         }

         if (_delayOn && ( ( (++numPackets) % freqDelay) == 0) )
         {
            nanosleep(&timeDelay, NULL);
         }
      }
   } while (readSize == BUFFERSIZE);

   close(fileHandle);

   return true;
}



//
// openFile
//
bool XIF_FileIo::openFile ()
{
   _fileHandle = fopen(_tempFileName.c_str(), "r");

   if (!_fileHandle)
   {
      DataLog(_error) << "Can't open file " << _tempFileName.c_str() << endmsg;
      return false;
   }

   return true;
}



//
// CloseFile
//
void XIF_FileIo::closeFile ()
{
   if (_fileHandle != 0)
   {
      fclose(_fileHandle);
      _fileHandle = 0;
   }
}



//
// getNextFile
//
const char* XIF_FileIo::getNextFile (int& fileSize)
{
   const char* filename = _buffer;
   int         size     = 0;

   // This should not happen. Put here for completeness and paranoia.
   if (_fileHandle == 0)
   {
      // Log event for later debugging.
      DataLog(_error) << "Invalid file handle" << endmsg;

      return 0;
   }

   // A null return is assumed to be EOF
   fscanf(_fileHandle, "%s", _buffer);
   const int numItems = fscanf(_fileHandle, "%d", &size);

   if (numItems <= 0)
   {
      // End of file
      fileSize = 0;
      return 0;
   }

   fileSize = size;

   return filename;
}

/* FORMAT HASH b14da8d4f1a7ff2eaabf2940726bdfe7 */
