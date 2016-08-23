/*
 * Copyright (c) 2012 by TerumoBCT, Inc.  All rights reserved.
 *
 * File: trap_dat.h
 *
 * The purpose of this module is to pre-load all of the Trima data files before the
 * application tasks start up.  The intent of the entry point readDataFiles is to
 * run in the context of sysinit.  This will prevent any priority inversion issues
 * during start-up and remove the need for inserting task delays in the file reading
 * code.
 *
 */

#include <stdio.h>
#include <strstream.h>
#include <usrLib.h>

#include "trap_dat.h"

#include "units.h"
#include "trima_datalog.h"    // Datalog stuff
#include "an_alarm.h"         // Get proc alarm defn.
#include "filenames.h"        // Contains PF Procedures file name.


//
// Base class definitions
//
TrapDat::TrapDat()
   : _calculatedCrc(INITCRC_DEFAULT),
     _overrideFileCrc(0),
     _isOverrideFile(false),

// Initialize the config variables.
     _daysOfWeek(-1),
     _beginWindow(-1),
     _endWindow(-1),
     _maxRetries(-1),
     _maxTimeouts(-1),
     _feature(-1),
     _requestDelay(20), // This will get overridden by the dat file, but can't hurt
     _topOfFile(new LINE),
     _lineInRamFile(_topOfFile),
     _fileHasChanged(false),
     _isEOF(false)
{
   _requestDelayInMilliSeconds = _requestDelay * MILLISEC_PER_SEC;

   memset(_controlIp, 0, sizeof(_controlIp));
   _controlPort = -1;
}



TrapDat::~TrapDat()
{
   if (_topOfFile)
   {
      while (_topOfFile != NULL)
      {
         LINE* thisLine = _topOfFile;
         _topOfFile = _topOfFile->next;
         delete thisLine;
      }

      delete _topOfFile;
      _topOfFile = NULL;
   }
}



void TrapDat::BOF ()
{
   _lineInRamFile = _topOfFile;
   _isEOF         = false;
}



const char* TrapDat::nextLine ()
{
   // Prevent running off of the end of the list
   if (_lineInRamFile->next != NULL)
   {
      _lineInRamFile = _lineInRamFile->next;
   }
   else
   {
      _isEOF = true;
   }

   return _lineInRamFile->line;
}



void TrapDat::replaceLine (const char* s)
{
   if ( (_lineInRamFile != NULL) && (_lineInRamFile->line != NULL) )
   {
      strncpy(_lineInRamFile->line, s, BUFFER_SIZE);
      _fileHasChanged = true;
   }
}




//
// Get the items from the config file
//
bool TrapDat::read (const char* filename, bool ramCopy)
{
   LINE* ramFile = _topOfFile;
   FILE* infile  = NULL;
   char* value   = NULL;

   char  buffer[TrapDat::BUFFER_SIZE];
   int   lineCount = 1;


   // Open the file read only.
   infile = fopen (filename, "r");

   if (infile == NULL)
   {
      DataLog(log_level_datfile_error) << "File " << filename << " could not be opened.  Errno= " << errnoMsg << endmsg;
      return false;
   }

   while (fgets(buffer, TrapDat::BUFFER_SIZE, infile) != NULL)
   {
      if (ramCopy)
      {
         // Store the line in the RAM file.
         ramFile->next = new LINE(buffer, lineCount);
         ramFile       = ramFile->next;
      }

      // If comment line, skip line.
      if (strstr(buffer, COMMENT_LINE) != NULL)
      {
         // No Operation
         DataLog(log_level_datfile_debug) << "(" << lineCount << ") Found comment line " << buffer << endmsg;
      }
      else if (strstr(buffer, DAYS_OF_WEEK) != NULL)
      {
         // Calculate CRC Value
         crcgen32 (&_calculatedCrc, (unsigned char*)buffer, strlen(buffer));

         // Throw away label
         value = strtok(buffer, TOKENS);

         // Get value
         value = strtok(NULL, TOKENS);

         if (value == NULL)
         {
            DataLog(log_level_datfile_error) << DAYS_OF_WEEK << " config line malformed (" << buffer
                                             << ") in file " << filename << ", line " << lineCount << "."
                                             << endmsg;

            fclose (infile);
            return false;
         }

         _daysOfWeek = atoi(value);
      }
      else if (strstr(buffer, BEGIN_WINDOW) != NULL)
      {
         // Calculate CRC Value
         crcgen32 (&_calculatedCrc, (unsigned char*)buffer, strlen(buffer));

         // Throw away label
         value = strtok(buffer, TOKENS);

         // Get value
         value = strtok(NULL, TOKENS);

         if (value == NULL)
         {
            DataLog(log_level_datfile_error) << BEGIN_WINDOW << " config line malformed (" << buffer
                                             << ") in file " << filename << ", line " << lineCount << "."
                                             << endmsg;

            fclose (infile);
            return false;
         }

         _beginWindow = atoi(value);
      }
      else if (strstr(buffer, END_WINDOW) != NULL)
      {
         // Calculate CRC Value
         crcgen32 (&_calculatedCrc, (unsigned char*)buffer, strlen(buffer));

         // Throw away label
         value = strtok(buffer, TOKENS);

         // Get value
         value = strtok(NULL, TOKENS);

         if (value == NULL)
         {
            DataLog(log_level_datfile_error) << END_WINDOW << " config line malformed (" << buffer
                                             << ") in file " << filename << ", line " << lineCount << "."
                                             << endmsg;

            fclose (infile);
            return false;
         }

         _endWindow = atoi(value);
      }
      else if (strstr(buffer, NUMBER_OF_ATTEMPTS) != NULL)
      {
         // Calculate CRC Value
         crcgen32 (&_calculatedCrc, (unsigned char*)buffer, strlen(buffer));

         // Throw away label
         value = strtok(buffer, TOKENS);

         // Get value
         value = strtok(NULL, TOKENS);

         if (value == NULL)
         {
            DataLog(log_level_datfile_error) << NUMBER_OF_ATTEMPTS << " config line malformed (" << buffer
                                             << ") in file " << filename << ", line " << lineCount << "."
                                             << endmsg;

            fclose (infile);
            return false;
         }

         _maxRetries = atoi(value);
      }
      else if (strstr(buffer, NUMBER_OF_TIMEOUTS) != NULL)
      {
         // Calculate CRC Value
         crcgen32 (&_calculatedCrc, (unsigned char*)buffer, strlen(buffer));

         // Throw away label
         value = strtok(buffer, TOKENS);

         // Get value
         value = strtok(NULL, TOKENS);

         if (value == NULL)
         {
            DataLog(log_level_datfile_error) << NUMBER_OF_TIMEOUTS << " config line malformed (" << buffer
                                             << ") in file " << filename << ", line " << lineCount << "."
                                             << endmsg;

            fclose (infile);
            return false;
         }

         _maxTimeouts = atoi(value);
      }
      else if (strstr(buffer, FEATURE) != NULL)
      {
         // Calculate CRC Value
         crcgen32 (&_calculatedCrc, (unsigned char*)buffer, strlen(buffer));

         // Throw away label
         value = strtok(buffer, TOKENS);

         // Get value
         value = strtok(NULL, TOKENS);

         if (value == NULL)
         {
            DataLog(log_level_datfile_error) << FEATURE << " config line malformed (" << buffer
                                             << ") in file " << filename << ", line " << lineCount << "."
                                             << endmsg;

            fclose (infile);
            return false;
         }

         _feature = atoi(value);
      }
      else if (strstr(buffer, CONTROL_IP) != NULL)
      {
         // Calculate CRC Value
         crcgen32 (&_calculatedCrc, (unsigned char*)buffer, strlen(buffer));

         // Throw away label
         value = strtok(buffer, TOKENS);

         // Get value
         value = strtok(NULL, TOKENS);

         if (value == NULL)
         {
            DataLog(log_level_datfile_error) << CONTROL_IP << " config line malformed (" << buffer
                                             << ") in file " << filename << ", line " << lineCount << "."
                                             << endmsg;

            fclose (infile);
            return false;
         }

         strncpy(_controlIp, value, IP_ADDR_SIZE);
      }
      else if (strstr(buffer, CONTROL_PORT) != NULL)
      {
         // Calculate CRC Value
         crcgen32 (&_calculatedCrc, (unsigned char*)buffer, strlen(buffer));

         // Throw away label
         value = strtok(buffer, TOKENS);

         // Get value
         value = strtok(NULL, TOKENS);

         if (value == NULL)
         {
            DataLog(log_level_datfile_error) << CONTROL_PORT << " config line malformed (" << buffer
                                             << ") in file " << filename << ", line " << lineCount << "."
                                             << endmsg;

            fclose (infile);
            return false;
         }

         _controlPort = (short)atoi(value);
      }
      else if (strstr(buffer, REQUEST_DELAY) != NULL)
      {
         // Calculate CRC Value
         crcgen32 (&_calculatedCrc, (unsigned char*)buffer, strlen(buffer));

         // Throw away label
         value = strtok(buffer, TOKENS);

         // Get value
         value = strtok(NULL, TOKENS);

         if (value == NULL)
         {
            DataLog(log_level_datfile_error) << REQUEST_DELAY << " config line malformed (" << buffer
                                             << ") in file " << filename << ", line " << lineCount << "."
                                             << endmsg;

            fclose (infile);
            return false;
         }

         _requestDelay = atoi(value);
      }
      else if (strstr(buffer, CRC_VALUE) != NULL)
      {
         // Throw away label
         value = strtok(buffer, TOKENS);

         // Get value
         value = strtok(NULL, TOKENS);

         if (value == NULL)
         {
            DataLog(log_level_datfile_error) << CRC_VALUE << " config line malformed (" << buffer
                                             << ") in file " << filename << ", line " << lineCount << "."
                                             << endmsg;

            fclose (infile);
            return false;
         }

         _overrideFileCrc = strtoul(value, (char**)0, 10);
         _isOverrideFile  = true;
      }
      else
      {
         DataLog(log_level_datfile_debug) << "(" << lineCount << ") Found other: " << buffer << endmsg;
      }

      lineCount++;
   }


   fclose (infile);

   // Assign the delay value.
   _requestDelayInMilliSeconds = _requestDelay * MILLISEC_PER_SEC;

   if (ramCopy)
   {
      DataLog(log_level_trap_info) << "Read " << lineCount << " lines from the file " << filename << endmsg;
   }

   return true;
}



//
//
//
TrapDat     OverrideTrapDat::_trapDat;
bool        OverrideTrapDat::_fileOK   = false;
const char* OverrideTrapDat::_filename = TRAP_OVERRIDE_FILE;



OverrideTrapDat::OverrideTrapDat()
{}



OverrideTrapDat::~OverrideTrapDat()
{}



//
// read
//
void OverrideTrapDat::read ()
{
   if (_trapDat.read(_filename, true) && _trapDat.isOverrideFile())
   {
      _fileOK = true;
   }
   else
      _fileOK = false;
}


bool OverrideTrapDat::updateConfigEntry (const char* token, const char* value)
{
   const int   delayTime = (int)(0.1 * sysClkRateGet()); // 100 ms delay.

   char        buffer[TrapDat::BUFFER_SIZE];
   bool        foundItem = false;
   const char* line      = NULL;

   // Start at the top of the file
   _trapDat.BOF();
   line = _trapDat.nextLine();

   //
   // Scan through the RAM file for the string to change
   while (!_trapDat.isEOF())
   {
      int result = 0;

      if ( (line != NULL) && (strstr(line, token) != NULL) )  // Token match?
      {
         result = sprintf (buffer, "%s %s\n", token, value);

         if ( (result >= TrapDat::BUFFER_SIZE) || (result == ERROR) )
         {
            DataLog(log_level_trap_error) << "Operation failed. <" << _filename << " " << token
                                          << value << " >."
                                          << ".  Line=" << _trapDat.lineNumber()
                                          << ".  Result=" << result
                                          << endmsg;

            return false;
         }

         _trapDat.replaceLine(buffer);
         foundItem = true;

         DataLog(log_level_trap_info) << "Operation successful.  Line "
                                      << _trapDat.lineNumber() << ". "
                                      << _filename << " <" << buffer << ">." << endmsg;

         break;
      }

      line = _trapDat.nextLine();
   }

   if (!foundItem)
   {
      DataLog(log_level_trap_error) << "Could not find token. File not changed.  <" << _filename << " "
                                    << token << " " << value << ">." << endmsg;

      return false;
   }

   // Never get here
   return true;
}



bool OverrideTrapDat::write ()
{
   const int     delayTime       = (int)(0.1 * sysClkRateGet()); // 100 ms delay.
   const char*   line            = NULL;

   unsigned long runningCrcValue = INITCRC_DEFAULT;
   int           result          = 0;
   char          buffer[TrapDat::BUFFER_SIZE];

   if (!_trapDat.fileHasChanged())
   {
      DataLog(log_level_trap_error) << _filename << " has not changed.  Abandoning write." << endmsg;
      return false;
   }

   // Make file writable
   attrib(_filename, "-R");

   // Open the file for write.
   FILE* outFile = fopen (_filename, "w");

   if (outFile == NULL)
   {
      // Make file read only
      attrib(_filename, "+R");

      DataLog(log_level_trap_error) << "File " << _filename << " could not be written" << endmsg;

      return false;
   }

   // Start at the top of the file
   _trapDat.BOF();
   line = _trapDat.nextLine();

   //
   // Write out the file line by line
   while (!_trapDat.isEOF())
   {
      // initialize buffer to cover all paths
      buffer[0] = 0;

      // Meter writing the file.  The delay number was imperically driven to make the write take ~ 2 seconds.
      if (_trapDat.lineNumber() % 4 == 0)
      {
         taskDelay(delayTime);
      }

      // If comment line, skip line.
      if ( (line != NULL) && (strstr(line, COMMENT_LINE) != NULL) )
      {
         result = fputs (line, outFile);
      }
      else if ( (line != NULL) &&
                ( (strstr(line, DAYS_OF_WEEK)       != NULL) ||
                  (strstr(line, BEGIN_WINDOW)       != NULL) ||
                  (strstr(line, END_WINDOW)         != NULL) ||
                  (strstr(line, NUMBER_OF_ATTEMPTS) != NULL) ||
                  (strstr(line, NUMBER_OF_TIMEOUTS) != NULL) ||
                  (strstr(line, FEATURE)            != NULL) ||
                  (strstr(line, CONTROL_IP)         != NULL) ||
                  (strstr(line, CONTROL_PORT)       != NULL) ||
                  (strstr(line, REQUEST_DELAY)      != NULL) )
                )
      {
         // Compute the running CRC value
         crcgen32 (&runningCrcValue, (unsigned char*)line, strlen(line));

         result = fputs (line, outFile);
      }
      else if (strstr(line, CRC_VALUE) != NULL)
      {
         // Update the CRC value
         sprintf (buffer, "%s %lu\n", CRC_VALUE, runningCrcValue);

         // Write the line to the file.
         result = fputs (buffer, outFile);

         DataLog(log_level_trap_info) << "Update CRC to " << buffer << endmsg;
      }
      else
      {
         // Found white space.  Write it out.
         result = fputs (line, outFile);
      }

      // Check the result of the write
      if (result == EOF)
      {
         DataLog(log_level_trap_error) << "Operation failed. <" << _filename << " "
                                       << buffer << " >." << endmsg;

         fclose(outFile);

         // Guard against priority inversion
         taskDelay(delayTime);

         // Make file read only
         attrib(_filename, "+R");

         return false;
      }

      line = _trapDat.nextLine();
   }

   fclose(outFile);

   // Guard against priority inversion
   taskDelay(delayTime);

   attrib(_filename, "+R");

   DataLog(log_level_trap_info) << _filename << " successfully written" << endmsg;

   return true;
}



//
// Default Trap Dat methods
//
TrapDat     DefaultTrapDat::_trapDat;
bool        DefaultTrapDat::_fileOK   = false;
const char* DefaultTrapDat::_filename = TRAP_DEFAULTS_FILE;



DefaultTrapDat::DefaultTrapDat()
{}



DefaultTrapDat::~DefaultTrapDat()
{}

/* FORMAT HASH be1d9f583b77d5e8e30dbabed064b33f */
