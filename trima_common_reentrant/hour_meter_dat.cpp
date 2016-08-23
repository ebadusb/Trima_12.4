/*
 * Copyright (C) 2012 Gambro BCT, Inc.  All rights reserved.
 *
 * This class encapsulates the meter.dat file.  The file is read during task initialization
 * and is a sentinel object.
 *
 * $Header: //depot/main/embedded/Trima/Main/trima_common_reentrant/hour_meter_dat.cpp#1 $
 *
 */
#include <fstream>
#include <usrLib.h>

#include "datalog.h"
#include "datalog_periodic.h"

#include "hour_meter_dat.h"


#include "filenames.h"        // Get Trima filename locations.
#include "trima_datalog.h"    // Datalog stuff

#include "datfile.h"



#define MACHINE_METER_KEY_STRING    "total_machine_on_time"
#define CENTRIFUGE_METER_KEY_STRING "total_centrifuge_on_time"
#define NO_OF_RUNS_KEY_STRING       "total_no_of_runs"

#define PNAME_METERDAT_BK PNAME_METERDAT ".bk"


DataLog_SetHandle HourMeterDat::_set                   = 0;

long              HourMeterDat::_totalMachineTime      = 0;
long              HourMeterDat::_totalCentrifugeOnTime = 0;
long              HourMeterDat::_totalNumberOfRuns     = 0;

//
// Base class definitions
//
HourMeterDat::HourMeterDat()
{}


HourMeterDat::~HourMeterDat()
{}



//
// read
//
void HourMeterDat::read ()
{
   // Create the hour meter dlog set used for writing the file to the drive
   datalog_CreatePeriodicSet("hour_meter", &_set);
   datalog_SetPeriodicOutputInterval(_set, 0);
   datalog_AddFunc(_set, writeDataToFile, "HOURwriteDataToFile", "Meter Data written", "%d");

   DFR_ERROR statusInit       = DFR_OK; // default
   DFR_ERROR statusRead       = DFR_OK; // default
   DFR_ERROR statusBackup     = DFR_OK; // default
   DFR_ERROR statusBackupRead = DFR_OK; // default

   /*
         DFR_OK ,       // file read OK
         DFR_FOPEN,     // error on file open
         DFR_PARSE,     // parse error on the file
         DFR_VALUE,     // Value out of specified range
         DFR_NOTFOUND   // Section/Name not found
   */

   // read meter.dat file
   CDatFileReader meter_data_file;

   statusInit = meter_data_file.Initialize(PNAME_METERDAT);

   if (statusInit == DFR_OK)
   {
      _totalMachineTime      = meter_data_file.get_int(MACHINE_METER_KEY_STRING,    "value", 0, 99999999);
      _totalCentrifugeOnTime = meter_data_file.get_int(CENTRIFUGE_METER_KEY_STRING, "value", 0, 99999999);
      _totalNumberOfRuns     = meter_data_file.get_int(NO_OF_RUNS_KEY_STRING,       "value", 0, 99999999);
      statusRead             = meter_data_file.Error();
   }
   else
   {
      if (statusInit==DFR_FOPEN)
      {
         DataLog(log_level_datfile_error) << "Initialize Error meter.dat-Error on file open" << endmsg;
      }
      else if (statusInit==DFR_PARSE)
      {
         DataLog(log_level_datfile_error) << "Initialize Error meter.dat-Parse error on the file" << endmsg;
      }
      else if (statusInit==DFR_VALUE)
      {
         DataLog(log_level_datfile_error) << "Initialize Error meter.dat-Value out of specified range" << endmsg;
      }
      else if (statusInit==DFR_NOTFOUND)
      {
         DataLog(log_level_datfile_error) << "Initialize Error meter.dat-Section/Name not found" << endmsg;
      }
      else
      {
         DataLog(log_level_datfile_error) << "Initialize Error meter.dat-Reason Unknown " << endmsg;
      }

   }

   if (statusRead != DFR_OK)
   {
      if (statusInit==DFR_FOPEN)
      {
         DataLog(log_level_datfile_error) << "get_int Error meter.dat-Error on file open" << endmsg;
      }
      else if (statusInit==DFR_PARSE)
      {
         DataLog(log_level_datfile_error) << "get_int Error meter.dat-Parse error on the file" << endmsg;
      }
      else if (statusInit==DFR_VALUE)
      {
         DataLog(log_level_datfile_error) << "get_int Error meter.dat-Value out of specified range" << endmsg;
      }
      else if (statusInit==DFR_NOTFOUND)
      {
         DataLog(log_level_datfile_error) << "get_int Error meter.dat-Section/Name not found" << endmsg;
      }
      else
      {
         DataLog(log_level_datfile_error) << "get_int Error meter.dat-Reason Unknown " << endmsg;
      }
   }

   // if you can't read the primary goto the backup file.
   if ((statusRead != DFR_OK) || (statusInit != DFR_OK))
   {
      DataLog(log_level_datfile_error) << "Could not open meter.dat, using backup file" << endmsg;

      statusBackup = meter_data_file.Initialize(PNAME_METERDAT_BK);

      if (statusBackup == DFR_OK)
      {
         _totalMachineTime      = meter_data_file.get_int(MACHINE_METER_KEY_STRING,    "value", 0, 99999999);
         _totalCentrifugeOnTime = meter_data_file.get_int(CENTRIFUGE_METER_KEY_STRING, "value", 0, 99999999);
         _totalNumberOfRuns     = meter_data_file.get_int(NO_OF_RUNS_KEY_STRING,       "value", 0, 99999999);
         statusBackupRead       = meter_data_file.Error();
      }
      else
      {
         if (statusBackup==DFR_FOPEN)
         {
            DataLog(log_level_datfile_error) << "Initialize Error meter.dat.bak-Error on file open" << endmsg;
         }
         else if (statusBackup==DFR_PARSE)
         {
            DataLog(log_level_datfile_error) << "Initialize Error meter.dat.bak-Parse error on the file" << endmsg;
         }
         else if (statusBackup==DFR_VALUE)
         {
            DataLog(log_level_datfile_error) << "Initialize Error meter.dat.bak-Value out of specified range" << endmsg;
         }
         else if (statusBackup==DFR_NOTFOUND)
         {
            DataLog(log_level_datfile_error) << "Initialize Error meter.dat.bak-Section/Name not found" << endmsg;
         }
         else
         {
            DataLog(log_level_datfile_error) << "Initialize Error meter.dat.bak-Reason Unknown " << endmsg;
         }
      }
   }

   //
   //
   //   Use defaults if neither the regular or backup file was good
   //
   if ((statusBackupRead != DFR_OK) || (statusBackup != DFR_OK))
   {
      _totalMachineTime      = 60;
      _totalCentrifugeOnTime = 60;
      _totalNumberOfRuns     = 1;
      DataLog(log_level_datfile_error) << "Could not open meter.dat.bak file!, setting meter to defaults." << endmsg;
   }

}



//
// update
//
void HourMeterDat::update (long totalMachineTime, long centrifugeOnTime, long totalNumberOfRuns)
{
   // Save off the updates
   _totalMachineTime      = totalMachineTime;
   _totalCentrifugeOnTime = centrifugeOnTime;
   _totalNumberOfRuns     = totalNumberOfRuns;

   // Update the dat files
   datalog_ForcePeriodicOutput(_set);
}



//
// writeDataToFile
//
bool HourMeterDat::writeDataToFile ()
{
   DataLog(log_level_datfile_info) << "Writing meter data..." << endmsg;

   updateFile(PNAME_METERDAT, _totalMachineTime, _totalCentrifugeOnTime, _totalNumberOfRuns);
   updateFile(PNAME_METERDAT_BK, _totalMachineTime, _totalCentrifugeOnTime, _totalNumberOfRuns);

   DataLog(log_level_datfile_info) << "meter data updated" << endmsg;

   return true;
}



//
//  updateMeterFile
//
void HourMeterDat::updateFile (const char* filename, long totalMachineTime, long centrifugeOnTime, long totalNumberOfRuns)
{
   char s_data[80];

   if (filename == 0)
   {
      DataLog(log_level_datfile_error) << "filename is null" << endmsg;
      return;
   }

   attrib(filename, "-R");

   FILE* file = fopen (filename, "w");      // open file

   if (file != NULL)
   {
      sprintf(s_data, "[FileInfo]\n");
      fputs (s_data, file);

      sprintf(s_data, "ReadOnly=false\n");
      fputs (s_data, file);

      sprintf(s_data, "FileName=\"/config/meter.dat\"\n");
      fputs (s_data, file);

      sprintf(s_data, "ServiceChangeable=true\n");
      fputs (s_data, file);


      // Trima On Time
      sprintf(s_data, "[%s]\n", MACHINE_METER_KEY_STRING);
      fputs (s_data, file);

      sprintf(s_data, "value=%ld\n", totalMachineTime);
      fputs (s_data, file);

      // Centrifuge On Time
      sprintf(s_data, "[%s]\n", CENTRIFUGE_METER_KEY_STRING);
      fputs (s_data, file);

      sprintf(s_data, "value=%ld\n", centrifugeOnTime);
      fputs (s_data, file);

      // no of runs
      sprintf (s_data, "[%s]\n", NO_OF_RUNS_KEY_STRING);
      fputs (s_data, file);

      sprintf(s_data, "value=%ld\n", totalNumberOfRuns);
      fputs (s_data, file);

      fflush(file);
      fclose(file);
   }
   else
   {
      DataLog(log_level_datfile_error) << "Could not write " << filename << " file.  Errno=" << errnoMsg << endmsg;
   }

   attrib(filename, "+R");
}

/* FORMAT HASH bb4f6ad314eba5300a5a9c68ff393387 */
