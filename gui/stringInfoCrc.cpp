/*
* Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
*
* TITLE:      stringInfoCrc.cpp
*
* AUTHOR:     Dan Slauson
*
* DESCRIPTION: This file contains a function that generates a file
*       CRC for string.info file.  It goes through CRC-ing only the
*	quoted text, and returns an unsigned long CRC value.  You can
*	stick this value in the string.info file without affecting the
*	CRC value, which is the reason that only the quoted text is CRC-ed.
*	This is to prevent tampering, so this CRC will not be updated by
*	STS/EMS or update scripts.  The CRC in config/crc/strings.crc is
*	for detecting transmission errors or file corruption, and will
*	by update scripts.
*
*	Note that there's a Win32 console application in engr_tools that
*	compiles in this source code, which is why there's all the fugly
*	#ifdef WIN32 stuff up front.
*
*
*	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/stringInfoCrc.cpp 1.7 2009/05/12 16:45:25Z dslausb Exp dslausb $
*	$Log: stringInfoCrc.cpp $
*	Revision 1.7  2009/05/12 16:45:25Z  dslausb
*	Avoid priority inversion
*	Revision 1.6  2009/03/23 19:25:17Z  dslausb
*	Fix string.info CRCs
*	Revision 1.5  2009/02/17 16:50:56Z  dslausb
*	Make sure string.info file gets closed
*	Revision 1.4  2009/02/12 23:44:04Z  dslausb
*	CRC whole lines rather than just quoted text.
*	Revision 1.3  2009/02/12 23:06:03Z  dslausb
*	A little refactoring
*	Revision 1.2  2009/02/12 00:32:40Z  dslausb
*	A few tweaks for the VxWorks runtime version
*	Revision 1.1  2009/02/11 22:08:18Z  dslausb
*	Initial revision
*
*/

// Includes and definitions dependent on platform (Win32 or VxWorks)
#ifdef WIN32
    #include <iostream>
using namespace std;
    #define ERROR_LOG    cerr
    #define END_LOG      endl
    #define TOKENIZE(__A__, __B__, __C__) strtok(__A__, __B__)
    #define taskDelay(__DELAY_TIME__)

#else // WIN32 UNDEFINED
    #include <vxWorks.h>
    #include "trima_datalog.h"
    #include "an_alarm.h"
    #define ERROR_LOG    DataLog (log_level_gui_error)
    #define END_LOG      endmsg
    #define TOKENIZE(__A__, __B__, __C__) strtok_r(__A__, __B__, __C__)
#endif // #ifdef WIN32

#include "crcgen.h"

#define LINE_BUFFER_SIZE 4096

void stripJunk (char* stripStr)
{
   char copyBuffer[LINE_BUFFER_SIZE];
   strcpy(copyBuffer, stripStr);

   int stripStrIndex = 0;
   for (int copyStrIndex = 0; ((copyStrIndex < LINE_BUFFER_SIZE) && (copyBuffer[copyStrIndex] != '\0')); copyStrIndex++)
   {
      // Don't copy over any of these.
      if (copyBuffer[copyStrIndex] == '\n' ||
          copyBuffer[copyStrIndex] == '\t' ||
          copyBuffer[copyStrIndex] == '\v' ||
          copyBuffer[copyStrIndex] == '\b' ||
          copyBuffer[copyStrIndex] == '\r' ||
          copyBuffer[copyStrIndex] == '\f' ||
          copyBuffer[copyStrIndex] == '\a') continue;

      stripStr[stripStrIndex++] = copyBuffer[copyStrIndex];
   }

   stripStr[stripStrIndex] = '\0';
}

// main get routine called by all other gets.
unsigned long getStringInfoCrc (const char* pathName, const char* logPath = NULL)
{
   int           lineNum = 0;
   unsigned long crcVal  = INITCRC_DEFAULT;

   char          lineBuffer[LINE_BUFFER_SIZE];

   FILE*         stringInfoFile = fopen(pathName, "r");

   if (!stringInfoFile)
   {
      ERROR_LOG << "Unable to open string file " << pathName << "." << END_LOG;

        #ifndef WIN32 // We'll only have an alarm handler for VxWorks.
      anAlarmMsg badFileAlarm(STRING_INFO_FILE_ERROR);
      badFileAlarm.setAlarm();
        #endif // #ifndef WIN32

      return 0;
   }

   FILE* logFile = NULL;

   bool  logIt   = (logPath != NULL);

   if (logIt)
   {
      logFile = fopen(logPath, "w");

      if (!logFile)
      {
         ERROR_LOG << "Unable to open log file " << logPath << ". Skip logging." << END_LOG;
         logIt = false;
      }
   }

   if (logIt) fprintf(logFile, "Calculating CRC for %s...\n", pathName);
   if (logIt) fprintf(logFile, "Initial CRC val = %lx\n", crcVal);

   while (fgets(lineBuffer, LINE_BUFFER_SIZE, stringInfoFile) != NULL)
   {
      lineNum++;

      char  tokenizedBuffer[LINE_BUFFER_SIZE];
      strcpy(tokenizedBuffer, lineBuffer);
      char* nextToken  = 0;
      char* firstToken = TOKENIZE(tokenizedBuffer, " \t\n\r", &nextToken);

      // Do CRC on any line starting with 'text'.
      if (firstToken &&
          strlen(firstToken) > strlen("text") &&
          firstToken[0] == 't' &&
          firstToken[1] == 'e' &&
          firstToken[2] == 'x' &&
          firstToken[3] == 't')
      {
         stripJunk(lineBuffer);
         int lineBufferLen = strlen(lineBuffer);
         crcgen32 (&crcVal, (unsigned char*)lineBuffer, lineBufferLen - 2);

         if (logIt) fprintf(logFile, "Updating CRC to %lx for buffer [%d] %s\n", crcVal, lineBufferLen, lineBuffer);
         taskDelay(0);
      }
   }

   if (logIt) fprintf(logFile, "CRC is %lx for file %s\n", crcVal, pathName);

   fclose(stringInfoFile);
   if (logIt) fclose(logFile);

   if (crcVal == 0)
   {
      ERROR_LOG << pathName << "(" << lineNum << "): Generated invalid CRC value of "
                << crcVal << "." << END_LOG;

       #ifndef WIN32 // We'll only have an alarm handler for VxWorks.
      anAlarmMsg badFileAlarm(STRING_INFO_FILE_ERROR);
      badFileAlarm.setAlarm();
       #endif // #ifndef WIN32

      return 0;
   }

   return crcVal;
}

/* FORMAT HASH 43ad1fd283bdabd96dd274d2274e2098 */
