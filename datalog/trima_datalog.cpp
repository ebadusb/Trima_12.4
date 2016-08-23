/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include <ctype.h>
#include <dirent.h>
#include <envLib.h>
#include <time.h>
#include <sys/stat.h>

#include "dlogInitTable.h"
#include "filenames.h"
#include "portnumbers.h"
#include "projectrev.h"
#include "node_data.h"
#include "trima_tasks.h"

//
// Log file names are of the form (numbers added for index reference):
//
//            1         2         3
//	 0123456789012345678901234567890
//
//  MMMMMMM_DDDDDDDD_NNN_SSSSSS.log
//
// MMMMMMM - machine name (e.g. 1T00027)
// DDDDDDDD - date as year, month, day (e.g. 20011230)
// NNN - file number, restarts each day (e.g. 001)
// SSSSSS - sequence number, sequential across all log files (e.g. 0001093)
//
enum { MACHINE_NAME_LEN    = 7 };
enum { DATE_LEN            = 8 };
enum { FILE_NUMBER_LEN     = 3, MAX_FILE_NUMBER = 999 };
enum { SEQUENCE_NUMBER_LEN = 6, MAX_SEQUENCE_NUMBER = 999999 };
enum { EXTENSION_LEN       = 4 };
enum { FILE_NAME_LEN       = MACHINE_NAME_LEN + DATE_LEN + FILE_NUMBER_LEN + SEQUENCE_NUMBER_LEN + EXTENSION_LEN + 4 };

static DataLog_Result datalogInitLocal ();
static DataLog_Result datalogInitNet ();
static unsigned long  encodeLogDate (unsigned int year, unsigned int month, unsigned int day);
static DataLog_Result generateLogFileName (unsigned long fileNumber, unsigned long sequenceNumber, const DataLog_TimeStampStart& timeStart, char* logFileName);
static DataLog_Result scanExistingLogFileNames (unsigned long& nextFileNumber, unsigned long& nextSequenceNumber, const DataLog_TimeStampStart& timeStart);
static DataLog_Result datalogInitLevels ();
static void           overrideDatalogInitLevels (int levels, int nLevelsOff);
static int            getLevelIndex (char* buf, int levels);

DataLog_Result trima_datalogInit ()
{
   DataLog_Result result;

   //
   // Initialize based on node type.  Note that for any failures during
   // this initialization, we simply print an error message to stderr
   // and exit with a non-zero status code.  The rest of the Trima system
   // can't start without the data log subsystem initialized properly.
   //
   if ( sysGetLocalNodeID() == ControlNode )
   {
      result = datalogInitLocal();
   }
   else
   {
      result = datalogInitNet();
   }

   if ( result == DataLog_OK )
   {
      result = datalogInitLevels();
   }

   return result;
}

DataLog_NodeID datalog_NodeID ()
{
   SysNodeID   nodeID = sysGetLocalNodeID();
   SysNodeData nodeData;
   sysGetNodeData(LocalNode, &nodeData);
   return nodeData.ipAddress[nodeID];
}

static DataLog_Result datalogInitLocal ()
{
   DataLog_Result result = DataLog_OK;

   //
   // Get log start time (used to generate file name)
   //
   DataLog_TimeStampStart startTime;
   datalog_GetTimeStampStart(&startTime);

   //
   // Determine current values for file and sequence number
   //
   unsigned long fileNumber;
   unsigned long sequenceNumber;

   if ( result == DataLog_OK )
   {
      result = scanExistingLogFileNames(fileNumber, sequenceNumber, startTime);
   }

   //
   // Generate the log file name
   //
   char logFileName[FILE_NAME_LEN + 1];

   if ( result == DataLog_OK )
   {
      result = generateLogFileName(fileNumber, sequenceNumber, startTime, logFileName);
   }

   //
   // Initialize the data log subsystem
   //
   if (result == DataLog_OK )
   {
      const char* envVariable = "LOG_FILE_NAME=";
      char*       envString   = new char[strlen(envVariable) + strlen(logFileName) + 1];
      strcpy(envString, envVariable);
      strcat(envString, logFileName);

      // Remove the extension from the filename
      char* dot = strrchr(envString, '.');

      if (dot != 0)
      {
         *dot = 0;
      }
      else
      {
         printf("couldn't find extension in filename (%s).\n", envString);
         result = DataLog_Error;
      }

      putenv(envString);
      delete[] envString;

      char* logFileFullPath = new char[strlen(LOG_PATH) + FILE_NAME_LEN + 2];
      strcpy(logFileFullPath, LOG_PATH);
      strcat(logFileFullPath, "/");
      strcat(logFileFullPath, logFileName);
      result = datalog_Init(256, 128, logFileFullPath, "TRIMA", "CONTROL", getProjectRevision());
      delete[] logFileFullPath;
   }

   return result;
}

static DataLog_Result datalogInitNet ()
{
   SysNodeData safetyNodeData;
   sysGetNodeData(LocalNode, &safetyNodeData);

   struct in_addr iaddr;
   char           ipAddressStr[INET_ADDR_LEN];
   iaddr.s_addr = safetyNodeData.ipAddress[ControlNode];
   inet_ntoa_b(iaddr, ipAddressStr);

   return datalog_InitNet(128, 64, ipAddressStr, Trima_DataLogPort, 60, "SAFETY");
}

static unsigned long encodeLogDate (unsigned int year, unsigned int month, unsigned int day)
{
   //
   // A simple encoding for log date is used, which guarantees a unique encoding for
   // each date and that encodings are ordered (i.e. a smaller encoding value corresponds
   // to an earlier date).  Although computing something like days after Jan 1, 1970 would
   // work, the computations involved are more time consuming.
   //
   return (unsigned long)year * 12 * 31 + (unsigned long)month * 31 + (unsigned long)day;
}

static DataLog_Result generateLogFileName (unsigned long fileNumber, unsigned long sequenceNumber, const DataLog_TimeStampStart& timeStart, char* logFileName)
{
   DataLog_Result result = DataLog_OK;

   //
   // If DATE_LEN was changed, we must have changes to the date format, so
   // changes to this routine are necessary.  The following assert is
   // meant as a reminder to make sure this routine stay in-sync with
   // the DATE_LEN setting.
   //
   if ( result == DataLog_OK && DATE_LEN != 8 )
   {
      fprintf(stderr, "DATE_LEN definition (%d) is inconsistent\n", DATE_LEN);
      result = DataLog_Error;
   }

   //
   // Check for a valid machine name setting
   //
   const char* machineName = getenv("MACHINE");
   if ( result == DataLog_OK && !machineName )
   {
      fprintf(stderr, "The environment variable \"MACHINE\" has not been set\n");
      result = DataLog_Error;
   }
   else if ( result == DataLog_OK && strlen(machineName) != MACHINE_NAME_LEN )
   {
      fprintf(stderr, "The machine name is expected to be exactly %d characters in length.\n", MACHINE_NAME_LEN);
      fprintf(stderr, "The machine name environment variable setting shown below is invalid.\n");
      fprintf(stderr, "\"MACHINE=%s\"\n", machineName);
      result = DataLog_Error;
   }

   //
   // Generate the file name and make sure we get the correct length
   //
   if ( result == DataLog_OK )
   {
      sprintf(logFileName, "%s_%4u%02u%02u_%0*lu_%0*lu.dlog", machineName,
              (unsigned int)timeStart._year, (unsigned int)timeStart._month, (unsigned int)timeStart._day,
              FILE_NUMBER_LEN, fileNumber,
              SEQUENCE_NUMBER_LEN, sequenceNumber);

      if ( strlen(logFileName) != FILE_NAME_LEN )
      {
         fprintf(stderr, "Log file name \"%s\" does not have expected length of %d\n", logFileName, FILE_NAME_LEN);
         result = DataLog_Error;
      }
   }

   if ( result == DataLog_OK )
   {
      printf(" -----------  Current log file \"%s\"  -----------\n", logFileName);
   }

   return result;
}

static DataLog_Result scanExistingLogFileNames (unsigned long& nextFileNumber, unsigned long& nextSequenceNumber, const DataLog_TimeStampStart& timeStart)
{
   DataLog_Result result = DataLog_OK;

   nextFileNumber     = 0;
   nextSequenceNumber = 0;

   DIR* logDir = opendir(LOG_PATH);   // parasoft-suppress PB-27 "Ignore for OS functions"
   if ( logDir == NULL )
   {
      if ( mkdir(LOG_PATH) != OK )
      {
         perror("can't open or create log directory");
         fprintf(stderr, "log directory = \"%s\"\n", LOG_PATH);
         result = DataLog_Error;
      }
   }
   else
   {
      struct dirent* dirData = NULL;
      char           nameBuffer[FILE_NAME_LEN + 1];

      unsigned long  encodedLatestDateInDir = 0;
      unsigned long  encodedCurrentDate     = encodeLogDate(timeStart._year, timeStart._month, timeStart._day);

      while ( (dirData = readdir(logDir)) != NULL )
      {
         //
         // If the file can't be a log file, just skip it
         //
         size_t nameLen = strlen(dirData->d_name);

         if ( nameLen != FILE_NAME_LEN )
            continue;

         //
         // Copy the file name to a temporary buffer and convert to lower case
         //
         nameBuffer[nameLen] = '\0';

         for ( int i = 0; i<nameLen; i += 1 )
         {
            nameBuffer[i] = tolower(dirData->d_name[i]);
         }

         const char*   fileDateStr     = &nameBuffer[MACHINE_NAME_LEN + 1];
         const char*   fileNumberStr   = &nameBuffer[MACHINE_NAME_LEN + DATE_LEN + 2];
         const char*   fileSequenceStr = &nameBuffer[MACHINE_NAME_LEN + DATE_LEN + FILE_NUMBER_LEN + 3];
         const char*   fileSuffixStr   = &nameBuffer[FILE_NAME_LEN - EXTENSION_LEN - 1];

         unsigned int  fileYear, fileMonth, fileDay;
         unsigned int  encodedFileDate;
         unsigned long fileNumber, fileSequence;

         if ( sscanf(fileDateStr, "%4u%2u%2u", &fileYear, &fileMonth, &fileDay) == 3 &&
              sscanf(fileNumberStr, "%3lu", &fileNumber) == 1 &&
              sscanf(fileSequenceStr, "%6lu", &fileSequence) == 1 &&
              strcmp(fileSuffixStr, LOG_FILE_EXTENSION) == 0 )
         {
            encodedFileDate = encodeLogDate(fileYear, fileMonth, fileDay);

            if ( encodedFileDate == encodedCurrentDate &&
                 fileNumber >= nextFileNumber )
            {
               nextFileNumber = fileNumber + 1;
            }

            if ( fileSequence >= nextSequenceNumber )
            {
               nextSequenceNumber = fileSequence + 1;
            }
         }
      }

      closedir(logDir);
   }

   if ( result == DataLog_OK && nextSequenceNumber > MAX_SEQUENCE_NUMBER )
   {
      fprintf(stderr, "Next log sequence number (%lu) is greater than %lu.  Log\n", nextSequenceNumber, (unsigned long)MAX_SEQUENCE_NUMBER);
      fprintf(stderr, "directory should be archived and existing log files deleted\n");
      result = DataLog_Error;
   }

   if ( result == DataLog_OK && nextFileNumber > MAX_FILE_NUMBER )
   {
      fprintf(stderr, "Next log file number (%lu) is greater than %lu.  This generally\n", nextFileNumber, (unsigned long)MAX_FILE_NUMBER);
      fprintf(stderr, "indicates a corrupt log file directory or a problem which is\n");
      fprintf(stderr, "causing the machine to frequently reboot\n");
      result = DataLog_Error;
   }

   return result;
}

static int getLevelIndex (char* buf, int nLevels)
{
   for (int i = 0; i < nLevels; i++)
   {
      if (!strcmp(trimaInitTable[i].name, buf))
      {
         return i;
      }
   }

   return -1;
}

static void overrideDatalogInitLevels (int nLevels, int nLevelsOff)
{
   FILE* fp = NULL;

   fp = fopen(DLOG_OVERRIDE, "r");

   if (!fp)
   {
      return;
   }

   SysNodeID curNodeID = sysGetLocalNodeID();

   if (curNodeID == ControlNode)
   {
      DataLog(log_level_startup_info) << DLOG_OVERRIDE << " found. extra dlog levels can be turned on." << endmsg;
      DataLog(log_level_startup_info) << "The total number of dlog levels: " << nLevels
                                      << " The number of levels off by default: " << nLevelsOff << endmsg;
   }

   const int BUFFER_SIZE = 256;
   char      buffer[256];
   char*     name        = NULL;
   int       level       = -1;
   char      delimit[]   = {' ', '\t', '\n', '\r', '\0'};

   while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
   {
      if (strlen(buffer) && !strstr(buffer, "//"))
      {
         name = strtok(buffer, delimit);
         if (name)
         {
            level = getLevelIndex(name, nLevels);

            if (level > -1 && !trimaInitTable[level].enabled )
            {
               trimaInitTable[level].enabled = true;
               trimaInitTable[level].level->logOutput(DataLog_LogEnabled);

               if (curNodeID == ControlNode)
                  DataLog(log_level_startup_info) << "dlog level " << name << " is turned on by dlog override setting" << endmsg;
            }
         }
      }
      name = NULL;
   }

   fclose(fp);
}

static DataLog_Result datalogInitLevels ()
{
   int nLevels    = sizeof(trimaInitTable) / sizeof(trimaInitTable[0]);
   int nLevelsOff = 0;

   for (int level = 0; level < nLevels; level += 1)
   {
      // Check for log level name with embedded spaces (bad)
      //
      if (strchr(trimaInitTable[level].name, ' ') != NULL)
      {
         DataLog(log_level_critical) << "dlog level [" << trimaInitTable[level].name << "] has embedded spaces" << endmsg;
         return DataLog_Error;
      }

      datalog_CreateLevel(trimaInitTable[level].name, trimaInitTable[level].handle);
      trimaInitTable[level].level->setHandle(*trimaInitTable[level].handle);

      if ( !trimaInitTable[level].enabled )
      {
         trimaInitTable[level].level->logOutput(DataLog_LogDisabled);
         nLevelsOff++;
      }
   }

   overrideDatalogInitLevels(nLevels, nLevelsOff);

   return DataLog_OK;
}

/* FORMAT HASH d121b88c616af34a39fe9f0d5f12f4a0 */
