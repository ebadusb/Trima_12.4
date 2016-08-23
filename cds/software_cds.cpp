/*
 * Copyright (C) 2010 Gambro BCT, Inc.  All rights reserved.
 *
 * Software CDS.  This class encapsulates the feature.bin file with added functionality.
 *
 * Author:  Johnathan Sylvain
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <usrLib.h>

#include "software_cds.h"
#include "cobeconfig.h"
#include "config_cds.h"
#include "trima_info.h"
#include "crcgen.h"
#include "crc16.h"

#include "regionalizationKeyManager.h"

#ifndef __SKIPMSGSYS__
#include "an_alarm.h"
#include "message.h"
#include "trimamessages.h"
#include "messagesystem.h"
#include <vector>
#endif
#include "filenames.h"
#ifndef WIN32

#include "trima_options.h"
#else
#include "zlib.h"
#include "fcntl.h"
#endif


#include "env_names.h"

#if CPU != SIMNT
#include "../tcpip/ted.h"
#include <sysSymTbl.h>
#endif

// #include "../gui/string_id.h"
// typedef unsigned int TEXT_STRING_ID;


// Begin static data and methods
OptionFileData* Software_CDS::_guiData    = NULL;
bool            Software_CDS::_dataLoaded = false;

// Hard coded value; used for determing R&D users
const int     TERUMO_RND_CUSTOMER_ID = 1;

Software_CDS& Software_CDS::GetInstance ()
{
   static Software_CDS readOnlyInstance(ROLE_RO);

   return readOnlyInstance;
}

Software_CDS& Software_CDS::getRWCds ()
{
   static Software_CDS cds(ROLE_RW);
   return cds;
}

OptionFileData* Software_CDS::ReadFeatures (const char* pathname)
{
   OptionFileData* swData  = NULL;

   const char*     newPath = GetFeatureFile(pathname);
   if ( newPath != NULL && strlen(newPath) > 0 )
   {
      readOptionFile(newPath, "FEATURES", &swData, 1);
   }

   return swData;
}

OptionFileData* Software_CDS::ReadGui (const char* pathname)
{
   OptionFileData* swData  = NULL;

   const char*     newPath = GetFeatureFile(pathname);
   if ( newPath != NULL && strlen(newPath) > 0 )
   {
      readOptionFile(newPath, "GUI", &swData, 1);
   }

   return swData;
}

void Software_CDS::ReleaseFileData (OptionFileData* data)
{
   while (data != NULL)
   {
      OptionFileData* tmp = data->next;

      free(data);
      data = tmp;
   }
}

#ifndef __WIN32__
bool Software_CDS::Read (void)
{
   return Software_CDS::Read(PNAME_FEATURES);
}
#endif

bool Software_CDS::Read (const char* pathname)
{
   // CDS classes all exist in the same shared memory, so this instance overlaps the memory for
   // the GetInstance() instance.  Therefore, setting member variables in this instance will set the
   // variables in the other instance.

   // Need to call for side effect of registering CDS variables
   Software_CDS cds = getRWCds();

   return cds.LoadFileData(pathname);
}

bool Software_CDS::LoadFileData (const char* pathname)
{
   OptionFileData* swData      = NULL;
   OptionFileData* machineData = NULL;

   bool            error       = false;

   // Need to decompress the features and machine id files to read them
   unzipFile(PNAME_FEATURES, PNAME_FEATURES_TMP);
   unzipFile(PNAME_MACHINE_ID, PNAME_MACHINE_ID_TMP);

   LogConfigFileContents(PNAME_FEATURES_TMP);
   LogConfigFileContents(PNAME_MACHINE_ID_TMP);

   swData   = ReadFeatures(pathname);
   _guiData = ReadGui(pathname);

   readOptionFile(PNAME_MACHINE_ID_TMP, "MACHINE_ID", &machineData, 1);

   bool loggedFirstError = false;

   if (swData)
   {
      for (int i = 0; i < FEATURE_DEF_END; i++)
      {
         if (_features[i].feature_key_id != INVALID_KEY_ID)
         {
            error = error || !GetFlagSetting(swData, (FEATURE_ID)i, _features[i].value);

            if (error && !loggedFirstError)
            {
               loggedFirstError = true;

               DataLog(log_level_config_error) << "Error reading feature " << i <<
                  " (" << getFeatureConfigFileLabel((FEATURE_ID)i) << ")" << endmsg;
            }
         }
      }

      // Deprecated options
      time_only_config_mode.Set(0);
      dont_connect_ac_msg_at_set_load.Set(1);
   }
   else
   {
      error            = 1;
      loggedFirstError = true;
      DataLog(log_level_config_error) << "swdata is null " << endmsg;
   }

   if (_guiData)
   {
      touchscreen_bounce_delay.Set(8);
      rbc_max_prod_vol.Set(1000);
      max_metering_duration.Set(30.0);

      for (int i = 0; i < VALUE_FEATURE_DEF_END; i++)
      {
         if (_valueFeatures[i].feature_key_id != INVALID_KEY_ID)
         {
            error = error || !GetValueSetting(_guiData, (VALUE_FEATURE_ID)i, _valueFeatures[i].value);
            if (error && !loggedFirstError)
            {
               loggedFirstError = true;

               DataLog(log_level_config_error) << "Error reading value feature " << i <<
                  " (" << getValueFeatureConfigFileLabel((VALUE_FEATURE_ID)i) << ")" << endmsg;
            }
         }
      }

   }
   else
   {
      error = 1;
      if (!loggedFirstError)
      {
         loggedFirstError = true;

         DataLog(log_level_config_error) << "guidata is null" << endmsg;
      }
   }

   if (error)
   {
      DataLog(log_level_config_error) << "The file 'features.bin' is invalid or incomplete." << endmsg;

#ifndef __SKIPMSGSYS__
      anAlarmMsg alarm(SW_CONFIG_VALUE_NOT_FOUND);
      alarm.setAlarm();
#endif
   }
   else
      DataLog(log_level_config_info) << "features.bin file read" << endmsg;

   if (machineData)
   {
      DataLog(log_level_config_info) << "Loading machine.id variables" << endmsg;
      bool writeMachineFile = false;
      error = !GetStringValueSetting(machineData, "serial_number", serial_number_machine_id);
      if (error || !serial_number_machine_id.Get().compare(""))
      {
         DataLog(log_level_config_info) << "machine id missing serial number" << endmsg;
         error = true;
      }
      else
      {
         const char* machineSerialNumber = getenv(MachineEnumName);

         if ( (machineSerialNumber == NULL) ||
              (serial_number_machine_id.Get().compare(machineSerialNumber) != 0) )
         {
            DataLog(log_level_config_info) << "machine id serial nums don't match: " <<
               serial_number_machine_id.Get() << "; " << machineSerialNumber << endmsg;
            error = true;
         }
      }

      error = error || !GetGuiValueSetting(machineData, "site_id", site_id);
      error = error || !GetStringValueSetting(machineData, "site_name", site_name);
      error = error || !GetGuiValueSetting(machineData, "country_id", country_id);
      error = error || !GetStringValueSetting(machineData, "country_name", country_name);
      error = error || !GetGuiValueSetting(machineData, "customer_id", customer_id);
      error = error || !GetStringValueSetting(machineData, "customer_name", customer_name);
      DataLog(log_level_config_external) << "customer_id = " << customer_id.Get() << endmsg;
      DataLog(log_level_config_external) << "customer_name = " << customer_name.Get() << endmsg;
      DataLog(log_level_config_external) << "country_id = " << country_id.Get() << endmsg;
      DataLog(log_level_config_external) << "country_name = " << country_name.Get() << endmsg;
      DataLog(log_level_config_external) << "site_id = " << site_id.Get() << endmsg;
      DataLog(log_level_config_external) << "site_name = " << site_name.Get() << endmsg;

   }
   else
   {
      DataLog(log_level_config_info) << "machine.id data is null" << endmsg;
   }

   if (error)
   {
      DataLog(log_level_config_error) << "The file 'machine.id' is invalid or incomplete." << endmsg;

#ifndef __SKIPMSGSYS__
      anAlarmMsg alarm(SW_CONFIG_VALUE_NOT_FOUND);
      alarm.setAlarm();
#endif
   }


   ReleaseFileData(swData);
   ReleaseFileData(machineData);

   _dataLoaded = true;

   DataLog(log_level_ted_info) << "Full confirmation code = " << getFullConfirmationCode() << endmsg;
   DataLog(log_level_ted_info) << "Short confirmation code = " << getShortConfirmationCode() << endmsg;

   return !error;
}

void Software_CDS::LogConfigFileContents (const char* file)
{
   FILE* fp = fopen(file, "r");
   if (fp)
   {
      DataLog(log_level_startup_detail) << "config file: " << file << endmsg;

      enum
      {
         BuffSize = 256
      };
      char buff[BuffSize];
      while (fgets(buff, BuffSize, fp))
      {
         char* token = buff + strspn(buff, " \t\r\n");
         token[strcspn(token, "\r\n")] = '\0';
         if (token[0] != '\0' && token[0] != '#')
         {
            DataLog(log_level_startup_detail) << "config data: " << token << endmsg;
         }
      }

      fclose(fp);
   }
   else
   {
      DataLog(log_level_startup_detail) << "Unable to open file: " << file << endmsg;
   }
}

// End static data and methods


Software_CDS::Software_CDS(Role role)
   : SingleWriteDataStore ("Software_CDS", role)
{
   // Ensure RW version has already been initialized
   if (role == ROLE_RO)
   {
      if (!_dataLoaded)
      {
         DataLog(log_level_config_error) <<
            "Read Only instance of Software_CDS created before Read/Write version was initialized.\n" << endmsg;
      }
   }

   _features.resize(FEATURE_DEF_END);

   for (vector<FeatureInfo>::iterator it = _features.begin(); it !=_features.end(); ++it)
   {
      (*it).value.Register(this, NO_PFR, 0);
   }

   _valueFeatures.resize(VALUE_FEATURE_DEF_END);

   for (vector<ValueFeatureInfo>::iterator it = _valueFeatures.begin(); it !=_valueFeatures.end(); ++it)
   {
      (*it).value.Register(this, NO_PFR, 0);
   }

   dont_connect_ac_msg_at_set_load.Register(this, NO_PFR, 0);

   time_only_config_mode.Register(this, NO_PFR, 0);

   touchscreen_bounce_delay.Register(this, NO_PFR, 0.0f);
   rbc_max_prod_vol.Register(this, NO_PFR, 0);
   max_metering_duration.Register(this, NO_PFR, 0.0f);


   serial_number_machine_id.Register(this, NO_PFR, "");
   site_id.Register(this, NO_PFR, -1);
   site_name.Register(this, NO_PFR, "");
   customer_id.Register(this, NO_PFR, -1);
   customer_name.Register(this, NO_PFR, "");
   country_id.Register(this, NO_PFR, -1);
   country_name.Register(this, NO_PFR, "");

   booleanFeatureInit();

   valueFeatureInit();
}
#ifndef __WIN32__
Software_CDS::~Software_CDS(void)
{}
#endif

const char* Software_CDS::GetGUISetting (const char* optionName)
{
   return GetSettingByName(_guiData, optionName);
}

const char* Software_CDS::getFeatureConfigFileLabel (FEATURE_ID item)
{
   if (item < 0 || item >= FEATURE_DEF_END)
      return "Invalid Feature ID";
   else
      return _features[item].feature_file_label;
}

const char* Software_CDS::getValueFeatureConfigFileLabel (VALUE_FEATURE_ID item)
{
   if (item < 0 || item >= VALUE_FEATURE_DEF_END)
      return "Invalid Feature ID";
   else
      return _valueFeatures[item].feature_file_label;
}

const char* Software_CDS::GetSettingByName (OptionFileData* data, const char* optionName)
{
   const char* result = NULL;

   STATUS      status;
   if (strlen(optionName)>0)
   {
      status = getOption(optionName, &result, data);
   }
   else
   {
      // Empty option string returns empty result, but not an error
      result = "";
      status = OK;
   }

   if (status != OK)
   {
      DataLog(log_level_config_error)
         << "Couldn't read the software option: "
         << optionName
         << endmsg;
      result = NULL;
   }
   else if (strspn(result, " \t") == strlen(result))
   {
      DataLog(log_level_config_error)
         << "Empty software option: "
         << optionName
         << endmsg;

      result = NULL;   // treat empty setting line as if not present
   }

   return result;
}

const char* Software_CDS::GetSetting (OptionFileData* data, FEATURE_ID optionId)
{
   const char* result     = NULL;

   const char* optionName = getFeatureConfigFileLabel(optionId);

   return GetSettingByName(data, optionName);
}

bool Software_CDS::GetFlagSetting (OptionFileData* swData, FEATURE_ID optionId, BaseElement<bool>& value)
{
   const char* setting = GetSetting(swData, optionId);
   bool        success = false;
   bool        result  = false;

   if (setting != NULL)
   {
      success = true;

      for (unsigned int i = 0; i < strlen(setting); ++i)
      {
         if (strlen(setting) == 0)
         {
            result  = false;
            success = false;
         }
         else if (setting[i] == '0')
         {
            result = false;
         }
         else if (setting[i] == '1')
         {
            result = true;
         }
         else
         {
            result  = false;
            success = false;

            DataLog(log_level_config_error)
               << "Illegal software setting: "
               << optionId << " - " << getFeatureConfigFileLabel(optionId)
               << "="
               << setting
               << endmsg;

            break;
         }

      }

      value.Set(result);
   }

   return success;
}

bool Software_CDS::GetValueSetting (OptionFileData* guiData, VALUE_FEATURE_ID optionId, BaseElement<uint32_t>& value)
{
   const char* data       = NULL;

   const char* optionName = getValueFeatureConfigFileLabel(optionId);

   data = GetSettingByName(guiData, optionName);

   bool success = false;

   if (data != NULL)
   {
      success = true;
      char* end = NULL;
      value.Set(strtoul(data, &end, 10));

      if (*end)
      {
         DataLog(log_level_config_error) << "Unable to convert value setting to long. [" <<
            optionName << ": " << data << "]" << endmsg;
      }
   }

   return success;
}

bool Software_CDS::GetGuiValueSetting (OptionFileData* guiData, const char* optionName, BaseElement<int>& value)
{
   const char* data    = GetSettingByName(guiData, optionName);
   bool        success = false;

   if (data != NULL)
   {
      success = true;
      char* end = NULL;
      value.Set(strtoul(data, &end, 10));

      if (*end)
      {
         DataLog(log_level_config_error) << "Unable to convert gui value setting to long. [" <<
            optionName << ": " << data << "]" << endmsg;
      }
   }

   return success;
}

bool Software_CDS::GetStringValueSetting (OptionFileData* guiData, const char* optionName, BaseElement<string>& value)
{
   const char* data    = GetSettingByName(guiData, optionName);
   bool        success = false;

   if (data != NULL)
   {
      success = true;
      value.Set(data);
   }

   return success;
}

bool Software_CDS::GetGuiValueSetting (OptionFileData* guiData, const char* optionName, BaseElement<float>& value)
{
   const char* data    = GetSettingByName(guiData, optionName);
   bool        success = false;

   if (data != NULL)
   {
      success = true;
      value.Set(atof(data));
   }

   return success;
}

bool Software_CDS::isModAllowed (FEATURE_PERMISSION permission, KEY_AUTH authorization)
{
   // Truth table for authorization function
   //          authorization
   //             R_N_D        SERVICE      USER
   // permission
   // RND_ONLY       x
   // SERVICE        x             x
   // ANYONE         x             x           x
   bool allowed = false;
   if (authorization == AUTH_R_N_D || permission == ANYONE)
      allowed = true;
   else if (authorization == AUTH_SERVICE && permission == SERVICE)
      allowed = true;

   return allowed;
}

KEY_AUTH Software_CDS::getCurrentAuthLevel ()
{
   KEY_AUTH auth = AUTH_R_N_D;
#if CPU != SIMNT
   SYM_TYPE symType;
   bool     (* isTedConnectionActive)(void) = NULL;
   TED*     ted = NULL;

   if (symFindByName(sysSymTbl, "IsTedConnectionActive", (char**)&isTedConnectionActive, &symType) != OK)
   {
      DataLog(log_level_config_error) << "Unable to find isTedConnectionActive symbol" << endmsg;
   }

   if (getCustomerId() == TERUMO_RND_CUSTOMER_ID)
      auth = AUTH_R_N_D;
   else if (isTedConnectionActive && (* isTedConnectionActive)())
      auth = AUTH_SERVICE;
   else
   {
      auth = AUTH_USER;
      if (!isTedConnectionActive)
         DataLog(log_level_config_error) << "isTedConnectionActive is null" << endmsg;
   }
#endif
   return auth;
}

bool Software_CDS::SetFeature (int featureKeyId, bool turnOn, KEY_AUTH auth, bool sendUpdate = true)
{
   FEATURE_ID featureId = FindFeatureId(featureKeyId);

   if (featureId != FEATURE_DEF_END)
   {

      if (isModAllowed(_features[(int)featureId].permission, auth))
      {
         getRWCds()._features[(int)featureId].value.Set(turnOn);
#ifndef __WIN32__
         if (sendUpdate)
         {
            SendUpdateNotification();
         }
#endif

      }
#if CPU != SIMNT
      else
      {
         DataLog(log_level_config_error) << "Attempt to set key without required permission.  Key Id = " << featureKeyId <<
            "; Permission level=" << auth << endmsg;

         return false;

      }
#endif
   }
   else
   {
      DataLog(log_level_config_error) << "Attempt to set invalid feature from key.  Key Id = " << featureKeyId << endmsg;
   }

   return true;
}

bool Software_CDS::SetValueFeature (int featureKeyId, uint32_t value, KEY_AUTH auth, bool sendUpdate = true)
{
   VALUE_FEATURE_ID featureId = FindValueFeatureId(featureKeyId);

   if (featureId != VALUE_FEATURE_DEF_END)
   {
      if (isModAllowed(_valueFeatures[(int)featureId].permission, auth))
      {
         getRWCds()._valueFeatures[(int)featureId].value.Set(value);
#ifndef __WIN32__
         if (sendUpdate)
         {
            SendUpdateNotification();
         }
#endif
      }
#if CPU != SIMNT
      else
      {
         DataLog(log_level_config_error) << "Attempt to set value key without required permission.  Key Id = " << featureKeyId <<
            "; Permission level=" << auth << endmsg;

         return false;

      }
#endif
   }
   else
   {
      DataLog(log_level_config_error) << "Attempt to set invalid value feature from key.  Key Id = " << featureKeyId << endmsg;
   }

   return true;
}

FEATURE_ID Software_CDS::FindFeatureId (int featureKeyId)
{
   FEATURE_ID id = FEATURE_DEF_END;

   for (int i = 0; i < FEATURE_DEF_END; i++)
   {
      if (_features[i].feature_key_id == featureKeyId)
      {
         id = (FEATURE_ID)i;
      }
   }

   return id;
}

FEATURE_PERMISSION Software_CDS::FindFeaturePermission (int featureKeyId)
{
   FEATURE_PERMISSION perm = RND_ONLY;

   for (int i = 0; i < FEATURE_DEF_END; i++)
   {
      if (_features[i].feature_key_id == featureKeyId)
      {
         perm = _features[i].permission;
      }
   }

   return perm;
}

VALUE_FEATURE_ID Software_CDS::FindValueFeatureId (int featureKeyId)
{
   VALUE_FEATURE_ID id = VALUE_FEATURE_DEF_END;

   for (int i = 0; i < VALUE_FEATURE_DEF_END; i++)
   {
      if (_valueFeatures[i].feature_key_id == featureKeyId)
      {
         id = (VALUE_FEATURE_ID)i;
      }
   }

   return id;
}

FEATURE_PERMISSION Software_CDS::FindValueFeaturePermission (int featureKeyId)
{
   FEATURE_PERMISSION perm = RND_ONLY;

   for (int i = 0; i < FEATURE_DEF_END; i++)
   {
      if (_valueFeatures[i].feature_key_id == featureKeyId)
      {
         perm = _features[i].permission;
      }
   }

   return perm;
}

void Software_CDS::SendUpdateNotification ()
{
#ifndef __WIN32__
   FeatureChanged _fc(MessageBase::SEND_LOCAL);
   _fc.send();
#endif
}

//
//
// ------------------------ methods to determine and test standard vs hyperconcentrate platelet products
//
bool Software_CDS::isPPC (float conc)
{
   return (conc > CobeConfig::data().PpcConcentrationThreshold);
}

bool Software_CDS::aboveStorageRange (const float target_concentration, const float actual_concentration)
{
   bool above = false;
   if (isPPC(target_concentration))
   {
      above = (actual_concentration > Software_CDS::GetInstance().getValueFeature(PPCConcHigh));
   }
   else
   {
      above = (actual_concentration > Software_CDS::GetInstance().getValueFeature(StandardConcHigh));
   }
   return above;
}

bool Software_CDS::belowStorageRange (const float target_concentration, const float actual_concentration)
{
   bool below = false;
   if (isPPC(target_concentration))
   {
      below = (actual_concentration < Software_CDS::GetInstance().getValueFeature(PPCConcLow));
   }
   else
   {
      below = (actual_concentration < Software_CDS::GetInstance().getValueFeature(StandardConcLow));
   }
   return below;
}


bool Software_CDS::withinStorageRange (const float target_concentration, const float actual_concentration)
{
   return (!outsideStorageRange(target_concentration, actual_concentration));
}


bool Software_CDS::outsideStorageRange (const float target_concentration, const float actual_concentration)
{
   return ( (aboveStorageRange(target_concentration, actual_concentration))  ||
            (belowStorageRange(target_concentration, actual_concentration)) );
}


float Software_CDS::getLowConcentrationRange (const float target_concentration)
{
   return isPPC(target_concentration) ? Software_CDS::GetInstance().getValueFeature(PPCConcLow) :
          Software_CDS::GetInstance().getValueFeature(StandardConcLow);
}
float Software_CDS::getHighConcentrationRange (const float target_concentration)
{
   return isPPC(target_concentration) ? Software_CDS::GetInstance().getValueFeature(PPCConcHigh) :
          Software_CDS::GetInstance().getValueFeature(StandardConcHigh);
}







int Software_CDS::unzipFile (const char* from, const char* to)
{
#ifdef WIN32
   gzFile fromFD = gzopen(from, "rb");
#else
   gzFile fromFD = gzopen(from, "r");
#endif
   int    toFD   = open(to, O_CREAT | O_RDWR, 0644);

   if ( fromFD >= 0 && toFD >= 0 )
   {
      int  bytesRead;
      int  bytesWritten  = 0;
      int  kBytesWritten = 0;
      char buffer[10 * 1024];

      while ( (bytesRead = gzread(fromFD, buffer, 10 * 1024)) > 0 )
      {
         bytesWritten += write(toFD, buffer, bytesRead);
         kBytesWritten = bytesWritten / 1024;
      }

      gzclose(fromFD);
      close(toFD);

      return 1;
   }
   else
   {
      if ( fromFD < 0 )
      {
         perror(from);
      }
      else
      {
         gzclose(fromFD);
      }

      if ( toFD < 0 )
      {
         perror(to);
      }
      else
      {
         close(toFD);
      }
   }

   return 0;
}


// Decompresses the feature file and returns the path to the dcompressed and validated features.bin file or NULL in case of error
const char* Software_CDS::GetFeatureFile (const char* pathname)
{
   OptionFileData* idData = NULL;
   int             error  =  0;

#ifndef WIN32
   const char* newPathName = PNAME_FEATURES_TMP;
#else
   const char* newPathName = FILE_FEATURES_TMP;
#endif

   // delete the temp file
   attrib(newPathName, "-R");
   remove(newPathName);

   unzipFile(pathname, newPathName);

#if (CPU!=SIMNT)
   readOptionFile(newPathName, "MACHINE_ID", &idData, 1);

   if (idData)
   {
      const char* result           = NULL;

      const char* fileSerialNumber = 0;
      STATUS      status           = getOption("serial_number", &fileSerialNumber, idData);
      if (status != OK)
      {
         error = 1;
      }
      else
      {
         const char* machineSerialNumber = getenv(MachineEnumName);
         DataLog(log_level_config_info) << "serial nums: " <<
            fileSerialNumber << "; " << machineSerialNumber << endmsg;

         if ( (machineSerialNumber == NULL) ||
              (strcmp(machineSerialNumber, fileSerialNumber) != 0) )
         {
            error = 1;
         }
      }
   }
   else
      error = 1;

   if (error)
   {
      newPathName = NULL;
      DataLog(log_level_config_error) <<
         "The file features.bin is not valid for this machine.\n" << endmsg;
   }

   ReleaseFileData(idData);
#endif
   return newPathName;
}



bool Software_CDS::getFeature (FEATURE_ID featureId)
{
   // DataLog(log_level_config_info) <<
   //       "Software CDS getting feature " << featureId << " (" << getFeatureConfigFileLabel(featureId) << ").\n" << endmsg;
   bool returnVal = false;

   if (featureId < FEATURE_DEF_END)
   {
      returnVal = _features[featureId].value.Get();
   }

   return returnVal;
}

uint32_t Software_CDS::getValueFeature (VALUE_FEATURE_ID featureId)
{
   return _valueFeatures[featureId].value.Get();
}

int Software_CDS::zipFile (const char* from, const char* to)
{
   int    fromFD = open(from, O_RDONLY, 0644);

   gzFile toFD   = gzopen(to, "w");

   if ( fromFD >= 0 && toFD >= 0 )
   {
      int  bytesRead;
      int  bytesWritten = 0;
      char buffer[10 * 1024];

      while ( (bytesRead = read(fromFD, buffer, 10 * 1024)) > 0 )
      {
         bytesWritten += gzwrite(toFD, buffer, bytesRead);
      }

      gzclose(toFD);
      close(fromFD);

      return 1;
   }
   else
   {
      if ( fromFD < 0 )
      {
         perror(from);
      }
      else
      {
         close(fromFD);
      }

      if ( toFD < 0 )
      {
         perror(to);
      }
      else
      {
         gzclose(toFD);
      }
   }

   return 0;
}

void Software_CDS::WriteFeatureFile ()
{
   // create the new file.
   attrib(PNAME_FEATURES_TMP, "-R");
   ofstream outFile;
   outFile.open(PNAME_FEATURES_TMP, ios::out | ios::trunc | ios::binary, 0777);

   if (!outFile.good())
   {
      DataLog(log_level_datfile_error) << "File " << PNAME_FEATURES_TMP << " couldn't be opened [" << outFile.rdstate() << "]." << endmsg;
      return;
   }
   int           numUpdated = 0;

   stringstream  feature;
   unsigned long crc = 0;
   feature << "serial_number=" << getenv(MachineEnumName);
   crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
   feature << ",";

   outFile << "[MACHINE_ID]" << endl;
   outFile << feature.str() << hex << crc << endl;
   outFile << "[OPTIONS]" << endl;
   outFile << "file_version=0.0.0.0" << endl;
   outFile << "[FileInfo]" << endl;
   outFile << "ReadOnly=true" << endl;
   outFile << "FileName=\"/config/features.bin\"" << endl;
   outFile << "ServiceChangeable=false" << endl;
   outFile << "[FEATURES]" << endl;

   taskDelay(0); // Make sure other tasks get a chance to interrupt (avoid priority inversion)




   for (int i = 0; i < FEATURE_DEF_END; i++, numUpdated++)
   {
      feature.str("");
      feature.clear();
      feature << getFeatureConfigFileLabel((FEATURE_ID)i) << "=" <<
         (_features[i].value ? "1" : "0");
      // DataLog(log_level_datfile_info) << "Generating CRC for '" << feature.str().c_str() << "'; Length: " << feature.str().length() << endmsg;
      crc = 0;
      crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
      // DataLog(log_level_datfile_info) << "CRC = " << hex << crc << dec << endmsg;
      feature << ",";

      outFile << feature.str() << hex << crc << endl;

      taskDelay(0);  // Make sure other tasks get a chance to interupt (avoid priority inversion)
   }

   outFile << "[GUI]" << endl;

   for (int i = 0; i < VALUE_FEATURE_DEF_END; i++, numUpdated++)
   {
      feature.str("");
      feature.clear();
      feature << getValueFeatureConfigFileLabel((VALUE_FEATURE_ID)i) << "=" << dec <<
         (_valueFeatures[i].value);
      // DataLog(log_level_datfile_info) << "Generating CRC for '" << feature.str().c_str() << "'; Length: " << feature.str().length() << endmsg;
      crc = 0;
      crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
      // DataLog(log_level_datfile_info) << "CRC = " << hex << crc << dec << endmsg;
      feature << ",";

      outFile << feature.str() << hex << crc << endl;

      taskDelay(0);     // Make sure other tasks get a chance to interrupt (avoid priority inversion)
   }

   outFile.close();
   attrib(PNAME_FEATURES_TMP, "+R");

   attrib(PNAME_FEATURES, "-R");
   zipFile(PNAME_FEATURES_TMP, PNAME_FEATURES);
   attrib(PNAME_FEATURES, "+R");

   updateCrc();

   DataLog(log_level_datfile_info) << "File " << PNAME_FEATURES << " updated successfully with " << numUpdated << " entries." << endmsg;
}

/*void Software_CDS::WriteMachineFile ()
{
   // create the new file.
   attrib(PNAME_MACHINE_ID_TMP, "-R");
   ofstream outFile;
   outFile.open(PNAME_MACHINE_ID_TMP, ios::out | ios::trunc | ios::binary, 0777);

   if (!outFile.good())
   {
      DataLog(log_level_datfile_error) << "File " << PNAME_MACHINE_ID_TMP << " couldn't be opened [" << outFile.rdstate() << "]." << endmsg;
      return;
   }

   stringstream  feature;
   unsigned long crc = 0;
   feature << "serial_number=" << getenv(MachineEnumName);
   crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
   feature << ",";

   outFile << "[MACHINE_ID]" << endl;
   outFile << feature.str() << hex << crc << endl;

   feature.str("");
   feature.clear();
   crc = 0;
   feature << "file_version=0.0.0.0";
   crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
   outFile << feature.str() << "," << hex << crc << endl;

   feature.str("");
   feature.clear();
   crc = 0;
   feature << "customer_id=" << customer_id.Get();
   crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
   outFile << feature.str() << "," << hex << crc << endl;

   feature.str("");
   feature.clear();
   crc = 0;
   feature << "customer_name=" << customer_name.Get();
   crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
   outFile << feature.str() << "," << hex << crc << endl;

   feature.str("");
   feature.clear();
   crc = 0;
   feature << "site_id=" << site_id.Get();
   crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
   outFile << feature.str() << "," << hex << crc << endl;

   feature.str("");
   feature.clear();
   crc = 0;
   feature << "site_name=" << site_name.Get();
   crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
   outFile << feature.str() << "," << hex << crc << endl;

   feature.str("");
   feature.clear();
   crc = 0;
   feature << "country_id=" << country_id.Get();
   crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
   outFile << feature.str() << "," << hex << crc << endl;

   feature.str("");
   feature.clear();
   crc = 0;
   feature << "country_name=" << country_name.Get();
   crcgen32(&crc, (const unsigned char*)feature.str().c_str(), feature.str().length());
   outFile << feature.str() << "," << hex << crc << endl;

   taskDelay(0); // Make sure other tasks get a chance to interrupt (avoid priority inversion)

   outFile.close();
   attrib(PNAME_MACHINE_ID_TMP, "+R");

   attrib(PNAME_MACHINE_ID, "-R");
   zipFile(PNAME_MACHINE_ID_TMP, PNAME_MACHINE_ID);
   attrib(PNAME_MACHINE_ID, "+R");

   updateCrc();

   DataLog(log_level_datfile_info) << "File " << PNAME_MACHINE_ID << " updated successfully." << endmsg;
}
*/

string Software_CDS::getShortConfirmationCode ()
{
   unsigned char bitField[128] = {0};

   // version id
   bitField[0] = 0;

   // machine info
   bitField[1] = (unsigned char)site_id;
   bitField[2] = (unsigned char)customer_id;
   bitField[3] = (unsigned char)country_id;


   // Feature information
   int currentBit  = 0;
   int startByte   = 4;
   int currentByte = 0;
   int keyId       = 0;
   int maxByte     = 0;

   for (int i = 0; i < FEATURE_DEF_END; i++)
   {
      keyId        = _features[i].feature_key_id;

      currentByte  = keyId / 8;
      currentBit   = keyId % 8;

      currentByte += startByte;

      if (currentByte > maxByte)
      {
         maxByte = currentByte;
      }

      bitField[currentByte] |= (((unsigned char)(_features[i].value.Get() ? 1 : 0)) << currentBit);
   }

   currentByte = maxByte + 1;

   // crc
   Crc16          crcCalc;
   unsigned short crcValue = crcCalc.doCRC16(bitField, currentByte);
   bitField[currentByte++] = crcValue >> 8;
   bitField[currentByte++] = (unsigned char)crcValue;

   string retVal;
   for (int i = 0; i < currentByte; i++)
   {
      // most significant byte
      if ((bitField[i] >> 4) < 10)
      {
         retVal += (char)(bitField[i] >> 4) + '0';
      }
      else
      {
         retVal += (char)(bitField[i] >> 4) + 'A' - 10;
      }

      // least significant byte
      if ((bitField[i] & 15) < 10)
      {
         retVal += (char)(bitField[i] & 15) + '0';
      }
      else
      {
         retVal += (char)(bitField[i] & 15) + 'A' - 10;
      }

      if (((i % 2) == 1) && (i + 1 < currentByte))
      {
         retVal += "-";
      }
   }

   return retVal;
}

union ValueBytes
{
   int  value;
   char bytes[4];
} ;

string Software_CDS::getFullConfirmationCode ()
{
   unsigned char bitField[128] = {0};

   // version id
   bitField[0] = 0;

   // machine info
   bitField[1] = (unsigned char)site_id;
   bitField[2] = (unsigned char)customer_id;
   bitField[3] = (unsigned char)country_id;




   // Feature information
   int currentBit  = 0;
   int startByte   = 5;
   int currentByte = 0;
   int keyId       = 0;
   int maxByte     = 0;

   int maxId       = 0;

   for (int i = 0; i < FEATURE_DEF_END; i++)
   {
      keyId = _features[i].feature_key_id;

      if (keyId > maxId)
      {
         maxId = keyId;
      }

      currentByte  = (keyId / 8);
      currentBit   = keyId % 8;

      currentByte += startByte;

      if (currentByte > maxByte)
      {
         maxByte = currentByte;
      }

      bitField[currentByte] |= (((unsigned char)(_features[i].value.Get() ? 1 : 0)) << currentBit);
   }

   bitField[4] = (unsigned char)maxId;

   currentByte = maxByte + 1;

   int numValueFeaturesByte = currentByte;
   currentByte++;

   unsigned int numValueFeatures = 0;
   ValueBytes   convertInt;

   for (int i = 0; i < VALUE_FEATURE_DEF_END; i++)
   {
      bitField[currentByte++] = _valueFeatures[i].feature_key_id;
      convertInt.value        = _valueFeatures[i].value.Get();
      bitField[currentByte++] = convertInt.bytes[0];
      bitField[currentByte++] = convertInt.bytes[1];
      bitField[currentByte++] = convertInt.bytes[2];
      bitField[currentByte++] = convertInt.bytes[3];

      numValueFeatures++;
   }

   bitField[numValueFeaturesByte] = numValueFeatures;

   // crc
   Crc16          crcCalc;
   unsigned short crcValue = crcCalc.doCRC16(bitField, currentByte);
   bitField[currentByte++] = crcValue >> 8;
   bitField[currentByte++] = (unsigned char)crcValue;

   string retVal;
   for (int i = 0; i < currentByte; i++)
   {
      // most significant byte
      if ((bitField[i] >> 4) < 10)
      {
         retVal += (char)(bitField[i] >> 4) + '0';
      }
      else
      {
         retVal += (char)(bitField[i] >> 4) + 'A' - 10;
      }

      // least significant byte
      if ((bitField[i] & 15) < 10)
      {
         retVal += (char)(bitField[i] & 15) + '0';
      }
      else
      {
         retVal += (char)(bitField[i] & 15) + 'A' - 10;
      }

      if (((i % 2) == 1) && (i + 1 < currentByte))
      {
         retVal += "-";
      }
   }

   return retVal;
}

int Software_CDS::getSiteId ()
{
   DataLog(log_level_config_info) << "Software CDS getting feature site_id=" << site_id.Get() << ").\n" << endmsg;

   return site_id.Get();
}

string Software_CDS::getSiteName ()
{
   DataLog(log_level_config_info) << "Software CDS getting feature site_name=" << site_name.Get() << ").\n" << endmsg;

   return site_name.Get();
}

int Software_CDS::getCustomerId ()
{
   DataLog(log_level_config_info) << "Software CDS getting feature customer_id=" << customer_id.Get() << ").\n" << endmsg;

   return customer_id.Get();
}

string Software_CDS::getCustomerName ()
{
   DataLog(log_level_config_info) << "Software CDS getting feature customer_name=" << customer_name.Get() << ").\n" << endmsg;

   return customer_name.Get();
}

int Software_CDS::getCountryId ()
{
   DataLog(log_level_config_info) << "Software CDS getting feature country_id=" << country_id.Get() << ").\n" << endmsg;

   return country_id.Get();
}

string Software_CDS::getCountryName ()
{
   DataLog(log_level_config_info) << "Software CDS getting feature country_name=" << country_name.Get() << ").\n" << endmsg;

   return country_name.Get();
}

// static
bool Software_CDS::isJapaneseWorkflow ()
{
   // The JapanFeatures feature is a proxy/alias indicating
   // that the machine/disposable setup is a Japanese-only configuration.
   //
   return Software_CDS::GetInstance().getFeature(JapanFeatures);
}

KEY_RESULT Software_CDS::decodeKey (string& key, KEY_AUTH auth)
{
   KEY_RESULT                result = FAILED_BAD_KEY;
   RegionalizationKeyManager keyMgr;
   // string key(m_text.getTextObject().getUTF8());

   KeyDefinition keyDef = keyMgr.decodeKey(key);

   if (!keyDef.error)
   {

      bool keyValid      = false;
      bool siteFound     = true;
      bool customerFound = true;
      bool countryFound  = true;
      bool serialFound   = true;

      int  serialNum     = atoi(getenv(MachineEnumName) + 2);
      for (vector<int>::iterator it = keyDef.groups.serialNumbers.begin(); it != keyDef.groups.serialNumbers.end(); ++it)
      {
         if (*it == serialNum)
         {
            serialFound = true;
            break;
         }
         serialFound = false;
      }

      for (vector<int>::iterator it = keyDef.groups.sites.begin(); it != keyDef.groups.sites.end(); ++it)
      {
         if (*it == getSiteId())
         {
            siteFound = true;
            break;
         }
         siteFound = false;
      }

      for (vector<int>::iterator it = keyDef.groups.customers.begin(); it != keyDef.groups.customers.end(); ++it)
      {
         if (*it == getCustomerId())
         {
            customerFound = true;
            break;
         }
         customerFound = false;
      }

      for (vector<int>::iterator it = keyDef.groups.countries.begin(); it != keyDef.groups.countries.end(); ++it)
      {
         if (*it == getCountryId())
         {
            countryFound = true;
            break;
         }
         countryFound = false;
      }

      if (!keyDef.error && countryFound && customerFound && siteFound && serialFound)
      {
         keyValid = true;
      }

      if (keyValid)
      {
         KEY_AUTH auth = getCurrentAuthLevel();
         if (keyDef.keyType == BOOLEAN_KEY)
         {

            // Once through list to verify permissions, then once to apply changes
            for (std::vector<FeatureList>::iterator featureListItem = keyDef.boolKey.opCodeTargetLists.begin();
                 featureListItem != keyDef.boolKey.opCodeTargetLists.end();
                 ++featureListItem)
            {
               for (std::vector<int>::iterator featureKeyIdItem = (*featureListItem).features.begin();
                    featureKeyIdItem != (*featureListItem).features.end();
                    ++featureKeyIdItem)
               {
                  if (isModAllowed(FindFeaturePermission(*featureKeyIdItem),
                                   auth))
                  {
                     result = SUCCESS;
                  }
                  else
                  {
                     result = FAILED_UNAUTHORIZED_USER;
                  }

               }
            }

            if (result == SUCCESS)
            {
               DataLog(log_level_gui_info) << "Valid key entered: " << key << endmsg;

               for (std::vector<FeatureList>::iterator featureListItem = keyDef.boolKey.opCodeTargetLists.begin();
                    featureListItem != keyDef.boolKey.opCodeTargetLists.end();
                    ++featureListItem)
               {
                  for (std::vector<int>::iterator featureKeyIdItem = (*featureListItem).features.begin();
                       featureKeyIdItem != (*featureListItem).features.end();
                       ++featureKeyIdItem)
                  {
                     SetFeature(*featureKeyIdItem, (*featureListItem).opCode == FEATURE_ON ? true : false, auth, false);
                     DataLog(log_level_gui_info) << "Feature has been set via key.  " << *featureKeyIdItem << ": " <<
                        getFeatureConfigFileLabel(FindFeatureId(*featureKeyIdItem)) <<
                        "=" << (getFeature(FindFeatureId(*featureKeyIdItem)) ? "1" : "0") << endmsg;
                  }
               }
            }
         }
         else   // Value key
         {
            // Once through list to verify permissions, then once to apply changes
            for (std::vector<ValueFeatureItem>::iterator valueFeature = keyDef.valueKey.valueTargetLists.begin();
                 valueFeature != keyDef.valueKey.valueTargetLists.end();
                 ++valueFeature)
            {

               if (isModAllowed(FindValueFeaturePermission((*valueFeature).feature),
                                auth))
               {
                  result = SUCCESS;
               }
               else
               {
                  result = FAILED_UNAUTHORIZED_USER;
               }


            }

            if (result == SUCCESS)
            {
               DataLog(log_level_gui_info) << "Valid key entered: " << key << endmsg;

               for (std::vector<ValueFeatureItem>::iterator valueFeature = keyDef.valueKey.valueTargetLists.begin();
                    valueFeature != keyDef.valueKey.valueTargetLists.end();
                    ++valueFeature)
               {
                  SetValueFeature((*valueFeature).feature, (*valueFeature).value, auth, false);

                  DataLog(log_level_gui_info) << "Value feature has been set via key.   " << (*valueFeature).feature << ": " <<
                     FindValueFeatureId((*valueFeature).feature) <<
                     "=" << getValueFeature(FindValueFeatureId((*valueFeature).feature)) << endmsg;
               }
            }
         }

         Software_CDS::GetInstance().SendUpdateNotification();

         Software_CDS::GetInstance().WriteFeatureFile();
      }
      else
      {
         if (!countryFound)
         {
            result = FAILED_UNAUTHORIZED_COUNTRY;
         }
         else if (!customerFound)
         {
            result = FAILED_UNAUTHORIZED_CUSTOMER;
         }
         else if (!siteFound)
         {
            result = FAILED_UNAUTHORIZED_SITE;
         }
         else
         {
            result = FAILED_UNAUTHORIZED_SERIAL_NUMBER;
         }
      }
   }
   else
   {
      result = FAILED_BAD_KEY;
   }

   return result;
}

typedef struct ip_address
{
   unsigned char b0;
   unsigned char b1;
   unsigned char b2;
   unsigned char b3;
} ip_address;

typedef union
{
   uint32_t   i;
   ip_address ip;
} ip_translate;

string Software_CDS::convertIntToIP (uint32_t ipValue)
{
   stringstream ipString;
   ip_translate val;
   val.i = ipValue;
   ipString << (int)val.ip.b3 << "." << (int)val.ip.b2 << "." << (int)val.ip.b1 << "." << (int)val.ip.b0;
   return ipString.str();

}

void Software_CDS::updateCrc ()
{
   char softCrcString[200];
   int  count = 0;


   count = sprintf(softCrcString,
                   "-filelist %s -update %s",
                   REGIONALIZATION_CRC_LIST_FILES,
                   PNAME_REGIONALIZATION_CRC);

   if (count > 200)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "updateCrc just overwrote memory");
   }

   attrib(PNAME_REGIONALIZATION_CRC, "-R");

   const int status = softcrc(softCrcString);

   attrib(PNAME_REGIONALIZATION_CRC, "+R");

   if (status != 0)
   {
      DataLog(log_level_startup_error) << "Software CRC check failed for " << softCrcString << endmsg;

      anAlarmMsg alarm(SOFTWARE_CRC_CHECK_FAILURE);
      alarm.setAlarm("regionalization file CRC check failed");
   }
}

/* FORMAT HASH 18271bb8b9ca50937b6907a82235e1b4 */
