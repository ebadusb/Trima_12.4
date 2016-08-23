/*
 * Copyright (c) 2011 by TerumoBCT, Inc.  All rights reserved.
 *
 * File: datfile_reader.cpp
 *
 * The purpose of this module is to pre-load all of the Trima data files before the
 * application tasks start up.  The intent of the entry point readDataFiles is to
 * run in the context of sysinit.  This will prevent any priority inversion issues
 * during start-up and remove the need for inserting task delays in the file reading
 * code.
 *
 */

#include "messagesystem.h"
#include "an_alarm.h"
#include "filenames.h"

#include "autocaldat.h"
#include "cassette_dat.h"
#include "cobeconfig.h"
#include "config_cds.h"
#include "caldat.h"
#include "filehelp.h"
#include "hour_meter_dat.h"
#include "hwdat.h"
#include "language_loader.h"
#include "pin_file.h"
#include "rbcdat.h"
#include "software_cds.h"
#include "soundsdat.h"
#include "trap_dat.h"
#include "trima_info.h"
#include "trima_variable_database.h"
// #include "donor_image.h"



#define GENERATE_ALARM_IF_FAILED(msg, status)   \
   {                                               \
      if (status != NULL_ALARM)                    \
      {                                            \
         anAlarmMsg alarm(status);                 \
         alarm.setAlarm("dat file load failed");   \
                                                \
         DataLog(log_level_datfile_error)          \
            << msg << " read failed with status "  \
            << status << endmsg;                   \
      }                                            \
      else                                         \
      {                                            \
         DataLog(log_level_datfile_info)           \
            << msg << " file read" << endmsg;      \
      }                                            \
   }                                               \

//
// Executed during system initialization to read the configuration data file
//

// attempts to fix a backup file with a good file including crc
bool fixFile (CFileHelper& goodFile, const char* fname, const char* crcfname)
{
   bool bSuccess = true;
   char crcTxt[15];

   // obtain text copy of the crc
   CFileHelper fCRC;
   goodFile.CRC(crcTxt);

   // put it into the empty crc file
   fCRC.Append (crcTxt);
   fCRC.Append ("\n");

   // write file to backup location and crc to backup file
   bSuccess = goodFile.FastWrite (fname) &&
              fCRC.FastWrite (crcfname, false);

   return bSuccess;
}



// verifies that the main / backup file match
bool checkFile (void)
{
   bool bMainOk  = false;
   bool bBackOk  = false;
   bool bSuccess = false;

   // read in the main file and check CRC
   CFileHelper fhelper;
   bMainOk = fhelper.Read (PNAME_CONFIGDAT);

   if (bMainOk)
   {
      bMainOk = fhelper.CompareCRC (PNAME_CONFIGDAT_CRC);
   }

   // read in the backup file and check CRC
   CFileHelper fhelperBk;
   bBackOk = fhelperBk.Read (PNAME_CONFIGDAT_BK);

   if (bBackOk)
   {
      bBackOk = fhelperBk.CompareCRC (PNAME_CONFIGDAT_CRC_BK);
   }

   // There are four cases to deal with
   const int nCase = (bMainOk ? 1 : 0) + (bBackOk ? 2 : 0);

   switch (nCase)
   {
      case 1 : // Main file Ok, backup is bad.
      {
         // we need to update the backup file with the main one.
         DataLog(log_level_datfile_info) << "Attempting to fix backup config file." << endmsg;
         bSuccess = fixFile(fhelper, PNAME_CONFIGDAT_BK, PNAME_CONFIGDAT_CRC_BK);
         break;
      }

      case 2 : // Backup file is good, main file is bad.
      {
         // we need to update the main file with the backup one.
         DataLog(log_level_datfile_info) << "Attempting to fix main config file." << endmsg;
         bSuccess = fixFile(fhelperBk, PNAME_CONFIGDAT, PNAME_CONFIGDAT_CRC);
         break;
      }

      case 3 : // Both files are good, make sure they are the same.
      {
         if (fhelper.CRC() != fhelperBk.CRC())
         {
            // assume the main one is the correct one.
            DataLog(log_level_datfile_info) << "Config files ok, but don't match." << endmsg;
            bSuccess = fixFile(fhelper, PNAME_CONFIGDAT_BK, PNAME_CONFIGDAT_CRC_BK);
         }
         else
         {
            // both match and are fine
            DataLog(log_level_datfile_info) << "Config.dat, backup and crc ok." << endmsg;
            bSuccess = true;
            break;
         }
         break;
      }

      default : // neither file is good, $%#*$#@!
      {
         DataLog(log_level_datfile_error) << "Neither config or backup is ok." << endmsg;
         bSuccess = false;
         break;
      }
   }

   return bSuccess;
}


void loadConfiguredLanguage ()
{
   Config_CDS configDat(ROLE_RO);

   //
   // Load the configured language
   //
   const unsigned int    languageIndex       = configDat.LangUnit.Get().key_lang;
   const LanguageHeader* langHeader          = LanguageList::getLanguageHeader(languageIndex);
   bool                  loadDefaultLanguage = false;

   if (langHeader == NULL)
   {
      DataLog(log_level_datfile_error) << "ERROR: failed to load language file for language id "
                                       << languageIndex << ".  Attempting to load default language." << endmsg;

      loadDefaultLanguage = true;
   }
   else
   {
      // Try to load the language
      LanguageList::setCurrentLanguage(langHeader);

      if (!LanguageList::loadLanguage(langHeader))
      {
         DataLog(log_level_datfile_error) << "ERROR: failed to load a valid language file for language id "
                                          << languageIndex << endmsg;

         loadDefaultLanguage = true;
      }
   }

   if (loadDefaultLanguage)
   {
      // Attempt to load the "default language", English
      const LanguageHeader* defaultLang = LanguageList::getLanguageHeader(0);
      LanguageList::setCurrentLanguage(defaultLang);

      if ( (defaultLang == NULL) || !LanguageList::loadLanguage(defaultLang))
      {
         DataLog(log_level_datfile_error) << "ERROR: Can't load default language [English]. Aborting startup." << endmsg;

         _FATAL_ERROR(__FILE__, __LINE__, "No valid string file located.");
      }
   }

}



#if CPU==SIMNT
extern "C" void readSoftwareCDS (void)
{
   // Initialize the message system so that alarm messages
   // can be sent if config.dat can't be read correctly.
   MessageSystem msgSystem;
   msgSystem.initNonBlock("datfile_reader");

   Software_CDS::Read();
}



extern "C" void readLanguageStuff (void)
{
   DataLog(log_level_datfile_info) << "Reading variable database" << endmsg;
   TrimaVariableDatabase::read();
   DataLog(log_level_datfile_info) << "variable database file read." << endmsg;

   DataLog (log_level_datfile_info) << "Analyzing language files on disk..." << endmsg;
   LanguageList::read();
   DataLog (log_level_datfile_info) << "language files analyzed." << endmsg;

   DataLog (log_level_datfile_info) << "Loading the configured language." << endmsg;
   loadConfiguredLanguage();
   DataLog (log_level_datfile_info) << "Configured language loaded." << endmsg;
}

extern "C"
#endif // #if CPU==SIMNT
void readConfigDat (void)
{
#if CPU==SIMNT
   MessageSystem msgSystem;
   msgSystem.initNonBlock();
#else #endif // #if CPU!=SIMNT

   //
   // Main entry point for reading and validating the config.dat file.  This function also populates the
   // Config CDS.  This only needs to be called once.
   //
   if (!checkFile())
   {
      anAlarmMsg alarm(CONFIGDAT_BAD_FILE);
      alarm.setAlarm("CONFIG.DAT CRC Check Failed");
   }
#endif // #if CPU!=SIMNT

   Config_CDS* cfgreader = new Config_CDS(ROLE_RW);
   cfgreader->Read();
   delete cfgreader;

   DataLog(log_level_datfile_info) << "config.dat file read" << endmsg;
}

extern "C" void readCassette (void)
{
   MasterCassetteDat::read();
   AdminCassetteDat::read();
}

extern "C" void readDatFiles (void)
{
   //
   // Initialize the message system so that alarm messages
   // can be sent if config.dat can't be read correctly.
   //
   ALARM_VALUES datStatus;

#if 0  // Message system for the sysinit thread is started in router sync now.  Can't have two of them
   DataLog(log_level_datfile_info) << "Starting Message system" << endmsg;
   MessageSystem msgSystem;
   msgSystem.initNonBlock();   // Use non-blocking because we are not planing on receiving any messages
#endif

   DataLog(log_level_datfile_info) << "Reading Trima data files" << endmsg;

   DataLog(log_level_datfile_info) << "Reading cal.dat" << endmsg;
   datStatus = CalDat::initialize(true);
   GENERATE_ALARM_IF_FAILED("cal.dat", datStatus);

   DataLog(log_level_datfile_info) << "Reading hw.dat" << endmsg;
   datStatus = HWDat::initialize(true);
   GENERATE_ALARM_IF_FAILED("hw.dat", datStatus);

   DataLog(log_level_datfile_info) << "Reading rbc.dat" << endmsg;
   datStatus = RbcDat::initialize(true);
   GENERATE_ALARM_IF_FAILED("rbc.dat", datStatus);

   DataLog(log_level_datfile_info) << "Reading sounds.dat" << endmsg;
   datStatus = SoundsDat::initialize(true);
   GENERATE_ALARM_IF_FAILED("sounds.dat", datStatus);

   DataLog(log_level_datfile_info) << "Reading cobeconfig.dat" << endmsg;
   datStatus = CobeConfig::initialize(true);
   GENERATE_ALARM_IF_FAILED("cobeconfig.dat", datStatus);

   DataLog(log_level_datfile_info) << "Reading build info" << endmsg;
   TrimaInfo::read();
   DataLog(log_level_datfile_info) << "Build files read" << endmsg;

   DataLog(log_level_datfile_info) << "Reading features.bin" << endmsg;
   Software_CDS::Read();

   DataLog(log_level_datfile_info) << "Reading config.dat" << endmsg;
   readConfigDat();

   DataLog(log_level_datfile_info) << "Reading autocal.dat" << endmsg;
   AutoCalDat::read();
   DataLog(log_level_datfile_info) << "autocal.dat file read" << endmsg;

   DataLog(log_level_datfile_info) << "Reading cassette.dat" << endmsg;
   MasterCassetteDat::read();
   DataLog(log_level_datfile_info) << "cassette.dat file read" << endmsg;

   DataLog(log_level_datfile_info) << "Reading setconfig.dat" << endmsg;
   AdminCassetteDat::read();
   DataLog(log_level_datfile_info) << "setconfig.dat file read" << endmsg;

   DataLog(log_level_datfile_info) << "Reading pin.bio" << endmsg;
   const bool logPins = Software_CDS::GetInstance().getFeature(EnableVistaCfr11PinLogging);
   PinDat::setLoggingLevel(logPins);
   PinDat::read();
   DataLog(log_level_datfile_info) << "pin.bio file read" << endmsg;

   DataLog(log_level_datfile_info) << "Reading meter.dat" << endmsg;
   HourMeterDat::read();
   DataLog(log_level_datfile_info) << "meter.dat file read" << endmsg;

   DataLog(log_level_datfile_info) << "Reading trap_override.dat" << endmsg;
   OverrideTrapDat::read();
   DataLog(log_level_datfile_info) << "trap_override.dat file read" << endmsg;

   DataLog(log_level_datfile_info) << "Reading trap_defaults.dat" << endmsg;
   DefaultTrapDat::read();
   DataLog(log_level_datfile_info) << "trap_defaults.dat file read" << endmsg;

   DataLog(log_level_datfile_info) << "Reading variable database" << endmsg;
   TrimaVariableDatabase::read();
   DataLog(log_level_datfile_info) << "variable database file read" << endmsg;

   // DonorImage::read();
   // DataLog(log_level_datfile_info) << "Donor Image file read" << endmsg;

   DataLog (log_level_datfile_info) << "Analyzing language files on disk..." << endmsg;
   LanguageList::read();

   DataLog (log_level_datfile_info) << "Loading the configured language..." << endmsg;
   loadConfiguredLanguage();

   DataLog(log_level_datfile_info) << "Trima data file reading complete" << endmsg;
}

/* FORMAT HASH 18dc3146715de1c59a9b26f271264bae */
