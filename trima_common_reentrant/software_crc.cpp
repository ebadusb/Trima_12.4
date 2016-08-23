/*
 * Copyright (C) 2012 Gambro BCT, Inc.  All rights reserved.
 *
 * This file encapsulates the calculating and comparing of the various system CRC
 * files on Trima.
 *
 * $Header: //depot/main/embedded/Trima/Main/trima_common_reentrant/software_crc.cpp#1 $
 *
 */
#include "alarms.h"
#include "an_alarm.h"
#include "software_crc.h"
#include "crcgen.h"
#include "trima_datalog_levels.h"
#include "filenames.h"


void doFileCrcCheck (const char* fileList, const char* crcFile, const char* updateFile, const char* alarmMsg)
{
   char softCrcString[200];
   int  count = 0;

   if (updateFile == NULL)
      count = sprintf(softCrcString, "-filelist %s -verify %s", fileList, crcFile);
   else
      count = sprintf(softCrcString, "-filelist %s -verify %s -update %s", fileList, crcFile, updateFile);

   if (count > 200)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "doFileCrcCheck just overwrote memory");
   }

   const int status = softcrc(softCrcString);

   if (status != 0)
   {
      DataLog(log_level_startup_error) << "Software CRC check failed for " << softCrcString << endmsg;

      anAlarmMsg alarm(SOFTWARE_CRC_CHECK_FAILURE);
      alarm.setAlarm(alarmMsg);
   }
}



void doSafetyFilesCrcCheck (void)
{
   DataLog(log_level_startup_info) << "Starting Safety Files CRC check" << endmsg;

   doFileCrcCheck(SAFETY_CRC_LIST_FILES,
                  SAFETY_CRC_FILE,
                  TEMP_SAFETY_CRC_FILE,
                  "Safety software CRC check failed");

   DataLog(log_level_startup_info) << "Finished Safety Files CRC check" << endmsg;
}



void doTrimaFileCrcCheck (void)
{
   DataLog(log_level_startup_info) << "Starting Trima Files CRC check" << endmsg;

   doFileCrcCheck(TRIMA_CRC_LIST_FILES,
                  TRIMA_CRC_FILE,
                  TEMP_TRIMA_CRC_FILE,
                  "Trima software CRC check failed");

   DataLog(log_level_startup_info) << "Finished Trima Files CRC check" << endmsg;
}


void doGuiFileCrcCheck (void)
{
   DataLog(log_level_startup_info) << "Starting GUI Files CRC check" << endmsg;

   doFileCrcCheck(GRAPHICS_CRC_LIST_FILES, PNAME_GUI_GRAPHICS_CRC, NULL, "graphics file CRC check failed");
   doFileCrcCheck(STRINGS_CRC_LIST_FILES,  PNAME_STRING_CRC,       NULL, "strings file CRC check failed");
   doFileCrcCheck(DATA_CRC_LIST_FILES,     PNAME_DATA_CRC,         NULL, "data file CRC check failed");
   doFileCrcCheck(FONTS_CRC_LIST_FILES,    PNAME_FONT_CRC,         NULL, "Fonts file CRC check failed");

   DataLog(log_level_startup_info) << "Finished GUI Files CRC check" << endmsg;
}


void doMachineFileCrcCheck (void)
{
   DataLog(log_level_startup_info) << "Starting Machine Files CRC check" << endmsg;

   doFileCrcCheck(MACHINE_CRC_LIST_FILES,   PNAME_MACHINE_CRC,     TEMP_MACHINE_CRC_FILE,   "machine file CRC check failed");

   DataLog(log_level_startup_info) << "Finished Machine Files CRC check" << endmsg;
}


void doProcFileCrcCheck (void)
{
   DataLog(log_level_startup_info) << "Starting Proc Files CRC check" << endmsg;

   doFileCrcCheck(CALDAT_CRC_LIST_FILES,    PNAME_CALDAT_CRC,      TEMP_CALDAT_CRC_FILE,    "cal.dat file CRC check failed");
   doFileCrcCheck(CONFIG_CRC_LIST_FILES,    PNAME_CONFIGDAT_CRC,   TEMP_CONFIGDAT_CRC_FILE, "config.dat file CRC check failed");
   doFileCrcCheck(HWDAT_CRC_LIST_FILES,     PNAME_HWDAT_CRC,       TEMP_HWDAT_CRC_FILE,     "hw.dat file CRC check failed");
   doFileCrcCheck(RBCDAT_CRC_LIST_FILES,    PNAME_RBCDAT_CRC,      TEMP_RBCDAT_CRC_FILE,    "rbcdat file CRC check failed");
   doFileCrcCheck(CASSETTE_CRC_LIST_FILES,  PNAME_CASSETTEDAT_CRC, NULL,                    "cassette file CRC check failed");
   doFileCrcCheck(SETCONFIG_CRC_LIST_FILES, PNAME_SETCONFIG_CRC,   NULL,                    "setconfig file CRC check failed");
   doFileCrcCheck(REGIONALIZATION_CRC_LIST_FILES, PNAME_REGIONALIZATION_CRC,   NULL,        "regionalization file CRC check failed");

   DataLog(log_level_startup_info) << "Finished Proc Files CRC check" << endmsg;
}

/* FORMAT HASH 0c1c02be00c321e6ca12f2d1becf7114 */
