/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      config_cds.cpp
 *             Config Common Data Store
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file declares the body of the Config CDS
 *
*******************************************************************************/

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "config_cds.h"
#include "../os/trima_options.h"
#else
#include <cstdio>
#endif

#include "config_cds.h"
#include "software_cds.h"
#include "datalog_periodic.h"
#include "filenames.h"
#include "bloodtype.h"
#include "alarms.h"

#ifndef __SKIPMSGSYS__
#include "messagesystem.h"
#include "trimamessages.h"
#include "an_alarm.h"
#endif


static CCfgVarList _Lst(1);

//
// Write
//
template <class dataType> bool ConfigElement<dataType >::Write (CDatFileReader& datfile)
{
   ConfigVarList item;

#ifdef __WIN32__
   for (CONFIG_VAR_LIST::iterator j = _list.begin(); j != _list.end(); ++j)
   {
      item = *j;
      if (!datfile.HasSection(item._info->Section()))
      {
         std::string section = "["; section += item._info->Section(); section += "]";
         datfile.AddSection(section.c_str());
      }
      datfile.AddLine(item._info->Section(), item._info->Name(), "0");
   }
#endif

   // Don't write unless the section is valid.
   if (!Valid())
      return false;

   for (CONFIG_VAR_LIST::iterator i = _list.begin(); i != _list.end(); ++i)
   {
      item = *i;
      switch (item._storage_type)
      {
         case ConfigVarList::INT :
            if (!datfile.SetInt(item._info->Section(), item._info->Name(), *item._data_ptr.valuei))
            {
               DataLog(log_level_control_alarm_detail)
                  << "Couldn't write to Config.  Section(" << item._info->Section()
                  << "), Name(" << item._info->Name()
                  << "), Value(" << *item._data_ptr.valuei << ")." << endmsg;
#ifndef __SKIPMSGSYS__
               anAlarmMsg alarm(CONFIGDAT_BAD_FILE);
               alarm.setAlarm();
#endif
            }

            break;

         case ConfigVarList::FLOAT :
            if (!datfile.SetFloat(item._info->Section(), item._info->Name(), *item._data_ptr.valuef))
            {
               DataLog(log_level_control_alarm_detail)
                  << "Couldn't write to Config.  Section(" << item._info->Section()
                  << "), Name(" << item._info->Name()
                  << "), Value(" << *item._data_ptr.valuef << ")." << endmsg;

#ifndef __SKIPMSGSYS__
               anAlarmMsg alarm(CONFIGDAT_BAD_FILE);
               alarm.setAlarm();
#endif
            }

            break;

         case ConfigVarList::UNKNOWN :
         default :
            DataLog(log_level_control_alarm_detail)
               << "Config Element not recognized.  Section(" << item._info->Section()
               << "), storage_type(" << item._storage_type
               << "), Name(" << item._info->Name()
               << "Value(" << *item._data_ptr.valuef << "), Min("
               << item._info->Min() << "), Max("
               << item._info->Max() << ")." << endmsg;

#ifndef __SKIPMSGSYS__
            anAlarmMsg alarm(CONFIGDAT_WRITE_ERROR);
            alarm.setAlarm();
#endif
      }
   }

   return true;
}



//
// Read
//
template <class dataType> bool ConfigElement<dataType >::Read (CDatFileReader& datfile)
{
   ConfigVarList item;
   float         value;
   bool          returnValue = true;

   for (CONFIG_VAR_LIST::iterator i = _list.begin(); i != _list.end(); ++i)
   {
      item  = *i;

      value = datfile.GetFloat(item._info->Section(), item._info->Name());

      if (!item._info->Valid(value))
      {
         returnValue = false;

         DataLog(log_level_control_alarm_detail)
            << "Value out of Range: Section(" << item._info->Section()
            << "), Name("                     << item._info->Name()
            << "), Value("                    << value
            << "), Min("                      << item._info->Min()
            << "), Max("                      << item._info->Max()
            << ")." << endmsg;

         // Generate S/W A2 Alarm
#ifndef __SKIPMSGSYS__
         anAlarmMsg alarm(CONFIGDAT_VALUE_OUT_OF_RANGE);
         alarm.setAlarm();
#endif
         switch (item._storage_type)
         {
            case ConfigVarList::INT :
               *item._data_ptr.valuei = (int)item._info->Min();
               break;

            case ConfigVarList::FLOAT :
               *item._data_ptr.valuef = item._info->Min();
               break;

            default :
               DataLog(log_level_config_error)
                  << "Wrong type found for config item " << item._info->Name()
                  << "(" << item._info->DisplayType() << ")." << endmsg;
               *item._data_ptr.valuef = item._info->Min();
         }
      }
      else
      {
         switch (item._storage_type)
         {
            case ConfigVarList::INT :
               *item._data_ptr.valuei = (int)value;
               break;

            case ConfigVarList::FLOAT :
               *item._data_ptr.valuef = value;
               break;

            default :
               DataLog(log_level_config_error)
                  << "Wrong type found for config item " << item._info->Name()
                  << "(" << item._info->DisplayType() << ")." << endmsg;
               *item._data_ptr.valuef = item._info->Min();
         }
         if (!strcmp(item._info->Name(), "key_air_removal"))
         {
            if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
            {
               if (*item._data_ptr.valuei != 0)
               {
                  *item._data_ptr.valuei = 0;
                  DataLog(log_level_config_info) << "Turn off air_removal if AiroutMitigation is off" << endmsg;
               }
            }
         }
      }
   }

   return returnValue;
}

template <class dataType> void ConfigElement<dataType >::SetDefault (void)
{
   ConfigVarList item;

   for (CONFIG_VAR_LIST::iterator i = _list.begin(); i != _list.end(); ++i)
   {
      item = *i;

      switch (item._storage_type)
      {
         case ConfigVarList::INT :
            *item._data_ptr.valuei = (int)item._info->Min();
            break;

         case ConfigVarList::FLOAT :
            *item._data_ptr.valuef = item._info->Min();
            break;

         default :
            break;
      }
   }
}

template <class dataType> ConfigElement<dataType>::ConfigElement()
{                                   }

template <class dataType> ConfigElement<dataType>::~ConfigElement()
{}

//
// Config_CDS Class
//


CDatFileReader    Config_CDS::_datfile;
DataLog_SetHandle Config_CDS::_configLogSet = DATALOG_NULL_SET_HANDLE;

Config_CDS::Config_CDS(Role role)
   : MultWriteDataStore("Config_CDS", role)
{
   LangUnit.Register        (this, PFR_RECOVER);
   Machine.Register         (this, PFR_RECOVER);
   Procedure.Register       (this, PFR_RECOVER);
   Predict.Register         (this, PFR_RECOVER);
   Templates.Register       (this, PFR_RECOVER);
   Definition.Register      (this, PFR_RECOVER);

   ProcedureList.numberOfProcedures.Register(this, PFR_RECOVER, 0, NUM_VISTA_PROCEDURES);
   ProcedureList.Procedure.Register(this, PFR_RECOVER);

   if ( _configLogSet == DATALOG_NULL_SET_HANDLE )
   {
      datalog_CreatePeriodicSet("ConfigData", &_configLogSet);

      datalog_AddRef(_configLogSet, LangUnit.GetRef().key_lang,                   "ConfigLang",               "language", "%d");
      datalog_AddRef(_configLogSet, LangUnit.GetRef().key_height,                 "ConfigHeight",             "height units", "%d");
      datalog_AddRef(_configLogSet, LangUnit.GetRef().key_weight,                 "ConfigWeight",             "weight units", "%d");
      datalog_AddRef(_configLogSet, LangUnit.GetRef().key_decimal_delimiter,      "ConfigDecimal",            "decimal delimiter", "%d");
      datalog_AddRef(_configLogSet, LangUnit.GetRef().key_crit_or_glob,           "ConfigCritOrHemoglobin",   "crit or hemoglobin", "%d");

      datalog_AddRef(_configLogSet, LangUnit.GetRef().key_date_format,            "ConfigDateFormat",         "date format", "%d");
      datalog_AddRef(_configLogSet, LangUnit.GetRef().key_time_format,            "ConfigTimeFormat",         "time format", "%d");

      datalog_AddRef(_configLogSet, Machine.GetRef().key_set_audio,               "ConfigAudioLevel",         "set audio level", "%d");

      datalog_AddRef(_configLogSet, Procedure.GetRef().key_proc_time,             "ConfigProcTime",           "maximum procedure time", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_return_press,          "ConfigReturnPressure",     "return pressure", "%.0f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_draw_press,            "ConfigDrawPressure",       "draw pressure", "%.0f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_custom_ratio,          "ConfigCustomRatio",        "ratio", "%.1f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_plasma_ratio,          "ConfigPlasmaRatio",        "ratio during plasma collection", "%.1f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_plasma_rinseback,      "ConfigPlasmaRinseback",    "plasma assisted rinseback", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_saline_rinseback,      "ConfigSalineRinseback",    "saline assisted rinseback", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_audit_tracking,        "ConfigAuditTracking",      "audit tracking", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_air_removal,           "ConfigAirRemoval",         "air removal", "%d");

      datalog_AddRef(_configLogSet, Predict.GetRef().key_rbc_ratio,               "ConfigRBCRatio",           "RBC ratio", "%d");

      // Metered Storage Config
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_mss_rbc_on,       "ConfigRBCMeteredSolution", "rbc metered solution",         "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_mss_plt_on,         "ConfigPLTMeteredSolution", "plt metered solution",         "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_rbc_def_bag_vol, "ConfigRBCDefBagVol",       "rbc default storage bag volm", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_plt_def_bag_vol,   "ConfigPLTDefBagVol",       "plt default storage bag volm", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_show_pre_aas_flags, "ConfigShowPreAasFlags", "show flags pre aas addition",     "%d");

      char keyName[32];
      char keyDesc[64];
      int  i;

      for ( i = 0; i<NUM_YIELD_VOLUME; i++ )
      {
         sprintf(keyName, "ConfigPltYield%d", i + 1);
         sprintf(keyDesc, "platelet yield %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_plt_yield[i],  keyName,     keyDesc, "%.2f");

         sprintf(keyName, "ConfigPltVolume%d", i + 1);
         sprintf(keyDesc, "platelet volume %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_plt_volume[i], keyName, keyDesc, "%.2f");

         sprintf(keyName, "ConfigPLTMSS%d", i + 1);
         sprintf(keyDesc, "PLT MSS %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_plt_mss[i], keyName, keyDesc, "%d");

         sprintf(keyName, "ConfigPctCarryoverVal%d", i + 1);
         sprintf(keyDesc, "PLT Percent Carryover Value %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_plt_pct_carryover[i], keyName, keyDesc, "%d");
      }


      for ( i = 0; i<NUM_PLT_AMAP; i++ )
      {
         sprintf(keyName, "ConfigPltAmapConc%d", i + 1);
         sprintf(keyDesc, "platelet amap conc %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_plt_amap_conc[i],  keyName,     keyDesc, "%.0f");
      }


      for ( i = 0; i<NUM_PLT_AMAP_PAS; i++ )
      {
         sprintf(keyName, "ConfigPltAmapMinYield%d", i + 1);
         sprintf(keyDesc, "platelet amap min yield %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_plt_amap_min_yield[i],  keyName,    keyDesc, "%.1f");

         sprintf(keyName, "ConfigPltAmapMaxYield%d", i + 1);
         sprintf(keyDesc, "platelet amap max yield %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_plt_amap_max_yield[i],  keyName,    keyDesc, "%.1f");

         sprintf(keyName, "ConfigPltAmapPasPctCarryover%d", i + 1);
         sprintf(keyDesc, "platelet amap pas pct carryover %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_plt_amap_pas_pct_carryover[i],  keyName,    keyDesc, "%d");
      }


      datalog_AddRef(_configLogSet, Predict.GetRef().key_ysf,                         "ConfigYSF",                    "platelet yield scaling factor", "%.2f");

      datalog_AddRef(_configLogSet, Predict.GetRef().key_male_only_plt,               "ConfigMaleOnlyPlt",            "male-only platelets", "%d");

      for ( i = 0; i<NUM_PLASMA_VOLUME; i++ )
      {
         sprintf(keyName, "ConfigPlasmaVolume%d", i + 1);
         sprintf(keyDesc, "plasma volume %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_pls_volume[i],  keyName,    keyDesc, "%.0f");
      }

      datalog_AddRef(_configLogSet, Predict.GetRef().key_pls_amap_min,                "ConfigAMAPMin",                "plasma AMAP minimum volume", "%d");
      datalog_AddRef(_configLogSet, Predict.GetRef().key_pls_amap_max,                "ConfigAMAPMax",                "plasma AMAP maximum volume", "%d");
      datalog_AddRef(_configLogSet, Predict.GetRef().key_male_only_plasma,            "ConfigMaleOnlyPlasma",         "male-only plasma", "%d");
      datalog_AddRef(_configLogSet, Predict.GetRef().key_ffp_volume,                  "ConfigFFPVolume",              "FFP volume to collect", "%d");
      datalog_AddRef(_configLogSet, Predict.GetRef().key_complementary_plasma,        "ConfigComplementaryPlasma",    "Complementary plasma", "%d");

      for ( i = 0; i<NUM_CRIT_DOSE; i++ )
      {
         sprintf(keyName, "ConfigRBCCrit%d", i + 1);
         sprintf(keyDesc, "RBC crit %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_rbc_crit[i],  keyName,  keyDesc, "%.2f");

         sprintf(keyName, "ConfigRBCDose%d", i + 1);
         sprintf(keyDesc, "RBC dose %d", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_rbc_dose[i],  keyName,  keyDesc, "%.0f");

         sprintf(keyName, "ConfigRBCMss%d", i + 1);
         sprintf(keyDesc, "RBC Mss Procedure %d toggle", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_rbc_mss[i],  keyName,   keyDesc, "%d");

         sprintf(keyName, "ConfigRBCMssVolume%d", i + 1);
         sprintf(keyDesc, "RBC Mss Procedure %d volume", i + 1);
         datalog_AddRef(_configLogSet, Templates.GetRef().key_rbc_mss_volume[i],  keyName,    keyDesc, "%d");

      }

      datalog_AddRef(_configLogSet, Predict.GetRef().key_rsf,                         "ConfigRSF",                    "RBC yield scaling factor", "%.2f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_drbc_alert,                "ConfigDRBCAlert",          "DRBC alert", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_drbc_threshold,            "ConfigDRBCThreshold",      "DRBC threshold", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_ac_rate,                   "ConfigACRate",             "AC infusion rate", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_post_crit,             "ConfigPostCrit",               "post crit", "%.2f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_post_plat,             "ConfigPostCount",          "post platelet count", "%.0f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_max_draw_flow,             "ConfigMaxDrawFlow",            "maximum draw flow", "%d");
      datalog_AddRef(_configLogSet, Predict.GetRef().key_inlet_flow_ramp,             "ConfigInletFlowRamp",      "inlet flow ramp", "%d");
      datalog_AddRef(_configLogSet, Predict.GetRef().key_inlet_management,            "ConfigInletManagement",    "inlet management", "%d");
      datalog_AddRef(_configLogSet, Predict.GetRef().key_return_management,           "ConfigReturnManagement",   "return management", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_autoflow,            "ConfigAutoFlow",   "AutoFlow", "%d");

      datalog_AddRef(_configLogSet, Procedure.GetRef().key_tbv_vol_setting,               "ConfigTBVSetting",                 "TBV setting", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_tbv_percent,                   "ConfigTBVPercent",                 "TBV percent", "%.1f");

      datalog_AddRef(_configLogSet, Procedure.GetRef().key_weight_setting,           "ConfigWeightSetting",          "Weight setting", "%.1f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_weight_less_than_vol,     "ConfigWeightLessThanVol",      "Weight less than volume", "%.1f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_weight_greater_than_vol,  "ConfigWeightGreaterThanVol",   "Weight greater than volume", "%.1f");

      datalog_AddRef(_configLogSet, Procedure.GetRef().key_ml_per_kg,                 "ConfigMlPerKg",                    "ml/kg", "%.1f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_drbc_body_vol,                 "ConfigDrbcBodyVol",            "DRBC body volume limit", "%.1f");

      datalog_AddRef(_configLogSet, Procedure.GetRef().key_max_plasma_vol_during_platelet_collection, "ConfigMaxPlasmaVolPltCollect",         "Max Plasma volume during platelet collection", "%.0f");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_max_plasma_vol_during_plasma_collection,   "ConfigMaxPlasmaVolPlsCollect",         "Max Plasma volume during plasma collection", "%.0f");

      datalog_AddRef(_configLogSet, Procedure.GetRef().key_blood_diversion,           "ConfigBloodDiversion",             "Blood Diversion", "%d");

      datalog_AddRef(_configLogSet, Predict.GetRef().key_rbc_replace_fluid,           "ConfigRBCReplaceFluid",    "RBC replacement fluid", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_min_replacement_volume,    "ConfigMinReplaceVolume",   "minimum replacement volume", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_plt_mss_split_notif,       "ConfigPLTMssSplitNotif",   "PLT MSS split notification", "%d");
      datalog_AddRef(_configLogSet, Procedure.GetRef().key_override_pas_bag_vol,      "ConfigMssOverrideBagVol", "MSS override bag volume", "%d");
      datalog_AddRef(_configLogSet, Predict.GetRef().key_rbc_fluid_percent,           "ConfigRBCFluidPercent",    "RBC fluid percent", "%.0f");


      for ( i = 0; i<NUM_OFFLINE_PROCEDURES; i++ )
      {
         sprintf(keyName, "ConfigProcPlt%d", i + 1);
         sprintf(keyDesc, "procedure %d platelet product", i + 1);
         datalog_AddRef(_configLogSet, Definition.GetRef()[i].key_platelet, keyName, keyDesc, "%d");

         sprintf(keyName, "ConfigProcPltAmap%d", i + 1);
         sprintf(keyDesc, "procedure %d amap platelet product", i + 1);
         datalog_AddRef(_configLogSet, Definition.GetRef()[i].key_platelet_amap, keyName, keyDesc, "%d");

         sprintf(keyName, "ConfigProcPls%d", i + 1);
         sprintf(keyDesc, "procedure %d plasma product", i + 1);
         datalog_AddRef(_configLogSet, Definition.GetRef()[i].key_plasma, keyName, keyDesc, "%d");

         sprintf(keyName, "ConfigProcRBC%d", i + 1);
         sprintf(keyDesc, "procedure %d RBC product", i + 1);
         datalog_AddRef(_configLogSet, Definition.GetRef()[i].key_rbc, keyName, keyDesc, "%d");

         sprintf(keyName, "ConfigProcBloodType%d", i + 1);
         sprintf(keyDesc, "procedure %d blood type", i + 1);
         datalog_AddRef(_configLogSet, Definition.GetRef()[i].key_blood_type, keyName, keyDesc, "%d");

         sprintf(keyName, "ConfigProcTime%d", i + 1);
         sprintf(keyDesc, "procedure %d time", i + 1);
         datalog_AddRef(_configLogSet, Definition.GetRef()[i].key_time, keyName, keyDesc, "%.0f");
      }
   }
}


//
// Write
//
bool Config_CDS::Write (WRITE_OPTIONS opt)
{
   if (GetRole() != ROLE_RW)
   {
      DataLog(log_level_config_error) << "Config CDS write refused.  Incorrect role." << endmsg;
      return false;
   }

   if (!Valid())
   {
      DataLog(log_level_config_error) << "Config CDS write refused.  Invalid datastore." << endmsg;
      return false;
   }

   bool successful_write = true;

   // Write out the Config CDS to file.
   successful_write = LangUnit.Write(_datfile);
   successful_write = successful_write && Machine.Write(_datfile);
   successful_write = successful_write && Procedure.Write(_datfile);
   successful_write = successful_write && Predict.Write(_datfile);
   successful_write = successful_write && Templates.Write(_datfile);
   successful_write = successful_write && Definition.Write(_datfile);

   if (!successful_write)
   {
#ifndef __SKIPMSGSYS__
      anAlarmMsg alarm (CONFIGDAT_WRITE_ERROR);
      alarm.setAlarm();
#endif
      DataLog(log_level_control_alarm_detail) << "Unable to write config.dat file." << endmsg;

      return false;
   }

#ifndef _NONTRIMA
   if (!_datfile.WriteCfgFile(FILE_CONFIG_DAT))
   {
      anAlarmMsg alarm (CONFIGDAT_WRITE_ERROR);
      alarm.setAlarm();

      DataLog(log_level_control_alarm_detail) << "Unable to write config.dat file." << endmsg;

      return false;
   }
#endif

   if (opt == COPY_OFFLINE_PROCEDURE_LIST)
   {
      // Copy the offline procedure list for predict
      CopyOfflineProcedureList();
   }

   // Send out an update message.
   SendUpdateMsg();

   return true;
}

Config_CDS::~Config_CDS()
{}

//
// Read
//
bool Config_CDS::Read ()
{
   return ReadConfig(PNAME_CONFIGDAT);
}

bool Config_CDS::ReadConfig (const char* pathname)
{
   // Only a RW Role can initialize the Config CDS.
   if (GetRole() != ROLE_RW)
   {
      DataLog(log_level_config_error) << "Config CDS read refused.  Incorrect role." << endmsg;
      return false;
   }

   bool         successful_read = true;
   DFR_ERROR    err;
   ALARM_VALUES alarmId         = NULL_ALARM;

   err = _datfile.Initialize(pathname, true);

   if ( err == DFR_OK )
   {
      // Read in the config dat file into the CDS.
      successful_read = LangUnit.Read(_datfile);
      successful_read = successful_read && Machine.Read(_datfile);
      successful_read = successful_read && Procedure.Read(_datfile);
      successful_read = successful_read && Predict.Read(_datfile);
      successful_read = successful_read && Templates.Read(_datfile);
      successful_read = successful_read && Definition.Read(_datfile);

      // get the error, if any ...
      err = _datfile.Error();
   }

   switch (err)
   {
      case DFR_FOPEN :
         alarmId = CONFIGDAT_BAD_FILE;

         DataLog(log_level_control_alarm_detail)
            << "Failed to open \"" << pathname << "\": " << errnoMsg << endmsg;
         break;

      case DFR_PARSE :
         alarmId = CONFIGDAT_PARSE_ERROR;

         DataLog(log_level_control_alarm_detail)
            << "Error parsing \"" << pathname << "\": " << errnoMsg << endmsg;
         break;

      case DFR_VALUE :
         alarmId = CONFIGDAT_VALUE_OUT_OF_RANGE;

         DataLog(log_level_control_alarm_detail)
            << "Value out of range in \"" << pathname << "\": " << errnoMsg << endmsg;
         break;

      case DFR_NOTFOUND :
         alarmId = CONFIGDAT_VALUE_NOT_FOUND;

         DataLog(log_level_control_alarm_detail)
            << "Value not found in \"" << pathname << "\": " << errnoMsg << endmsg;
         break;

      case DFR_OK :
         break;
   }

   if (alarmId != NULL_ALARM)
   {
      // Generate a Software A2 Alarm
#ifndef __SKIPMSGSYS__
      anAlarmMsg alarm(alarmId);
      alarm.setAlarm();
#endif
      LangUnit.SetDefault();
      Machine.SetDefault();
      Procedure.SetDefault();
      Predict.SetDefault();
      Templates.SetDefault();
      Definition.SetDefault();

      successful_read = false;
   }

   // If successful read, initialize the offline procedure list for predict
   if (successful_read)
   {
      // Initialize the number of procedures to the number of offline ones.
      ProcedureList.numberOfProcedures.Set(NUM_OFFLINE_PROCEDURES);

      CopyOfflineProcedureList();
   }

   datalog_ForcePeriodicOutput(_configLogSet);
   return successful_read;
}


//
// Valid
//
bool Config_CDS::Valid ()
{
   // Validate the sections.
   if (!LangUnit.Valid())
   {
      _errorMsg = LangUnit.lastError();
      return false;
   }

   if (!Machine.Valid())
   {
      _errorMsg = Machine.lastError();
      return false;
   }

   if (!Procedure.Valid())
   {
      _errorMsg = Procedure.lastError();
      return false;
   }

   if (!Predict.Valid())
   {
      _errorMsg = Predict.lastError();
      return false;
   }

   if (!Templates.Valid())
   {
      _errorMsg = Templates.lastError();
      return false;
   }

   if (!Definition.Valid())
   {
      _errorMsg = Definition.lastError();
      return false;
   }

   DataLog (log_level_config_info) << "Config Valid" << endmsg;

   _errorMsg = "";

   return true;
}

//
// isPromptedDrbcSplit
//
// warning: this does not and should not check stamp color.
// you need to check that independant of this.

// This function determines whether or not we are
// showing the DRBC split messages prompting a
// split.  This determination is always made based
// on configured dose without any adjustments.
// (See IT 8306)
const bool Config_CDS::isPromptedDrbcSplit (int procNum) const
{
   if (procNum < 1 || procNum > NUM_VISTA_PROCEDURES)
   {
      DataLog (log_level_config_error) << "No procedure selected (index=" << procNum << ")." << endmsg;
      return false;
   }
   else
   {
      bool  drbcAlertOn    = (bool)Procedure.Get().key_drbc_alert;
      float drbcThreshold  = (float)Procedure.Get().key_drbc_threshold;
      float configuredDose = ProcedureList.Procedure.Get()[procNum - 1].rbc_dose;  // Array indexed from 0, procedures from 1

      return drbcAlertOn && (configuredDose >= drbcThreshold);
   }
}

//
// SendUpdateMsg
//
void Config_CDS::SendUpdateMsg ()
{
#ifndef __SKIPMSGSYS__
   ConfigChangedMsg newConfig(MessageBase::SEND_LOCAL);
   newConfig.send(1);
#endif
   datalog_ForcePeriodicOutput(_configLogSet);
}



void Config_CDS::FillProcedureDescription (const ProductTemplates& pt, const ProductDefinition& pd, ProcedureDescription& pl, bool amapPltEnabled)
{
   if (amapPltEnabled)
   {
      const int& plt_amap_index = pd.key_platelet_amap;

      if (plt_amap_index == 0)
      {
         pl.yield                   = 0.0f;
         pl.platelet_volume         = 0.0f;
         pl.plt_mss                 = false;
         pl.pct_carryover           = 0;

         pl.amap_platelet_size      = AMAP_PLT_NONE;
         pl.amap_platelet_min_yield = 0.0f;
         pl.amap_platelet_max_yield = 0.0f;
         pl.amap_platelet_conc      = 0.0f;
      }
      else
      {
         pl.yield                   = 0.0f; // AMAP platelets are controlled differently
         pl.platelet_volume         = 0.0f;

         pl.plt_mss                 = (pt.key_plt_mss[plt_amap_index - 1] == 1);
         pl.pct_carryover           = pt.key_plt_pct_carryover[plt_amap_index - 1];

         pl.amap_platelet_size      = (AMAPPLTTYPES)plt_amap_index;
         pl.amap_platelet_min_yield = pt.key_plt_amap_min_yield[plt_amap_index - 1];
         pl.amap_platelet_max_yield = pt.key_plt_amap_max_yield[plt_amap_index - 1];
         pl.amap_platelet_conc      = pt.key_plt_amap_conc[plt_amap_index - 1];
      }
   }
   else
   {
      const int& yield_volume_index = pd.key_platelet;

      if (yield_volume_index == 0)
      {
         pl.yield                   = 0.0f;
         pl.platelet_volume         = 0.0f;
         pl.plt_mss                 = false;
         pl.pct_carryover           = 0;

         pl.amap_platelet_size      = AMAP_PLT_NONE;
         pl.amap_platelet_min_yield = 0.0f;
         pl.amap_platelet_max_yield = 0.0f;
         pl.amap_platelet_conc      = 0.0f;
      }
      else
      {
         pl.yield                   = pt.key_plt_yield[yield_volume_index - 1];
         pl.platelet_volume         = pt.key_plt_volume[yield_volume_index - 1];
         pl.plt_mss                 = (pt.key_plt_mss[yield_volume_index - 1] == 1);
         pl.pct_carryover           = pt.key_plt_pct_carryover[yield_volume_index - 1];

         pl.amap_platelet_size      = AMAP_PLT_NONE;
         pl.amap_platelet_min_yield = 0.0f;
         pl.amap_platelet_max_yield = 0.0f;
         pl.amap_platelet_conc      = 0.0f;
      }
   }
}


//
// Copy over the offline procedure list to the predict list
//
void Config_CDS::CopyOfflineProcedureList ()
{
   const bool                    amapPltEnabled = Software_CDS::GetInstance().getFeature(AmapPlateletsAvailable);
   const ProductDefinitionArray& offlineList    = Definition.GetRef();
   const ProductTemplates&       templates      = Templates.GetRef();

   ProductListArray              predictList;


   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++) // parasoft-suppress BD-PB-CC "Variable i will increment so this condition does not always evaluate to true"
   {
      const int& crit_dose_index = offlineList[i].key_rbc;

      if (amapPltEnabled)
      {
         const int& plt_amap_index = offlineList[i].key_platelet_amap;

         if (plt_amap_index == 0)
         {
            predictList[i].yield                   = 0.0f;
            predictList[i].platelet_volume         = 0.0f;
            predictList[i].plt_mss                 = false;
            predictList[i].pct_carryover           = 0;

            predictList[i].amap_platelet_size      = AMAP_PLT_NONE;
            predictList[i].amap_platelet_min_yield = 0.0f;
            predictList[i].amap_platelet_max_yield = 0.0f;
            predictList[i].amap_platelet_conc      = 0.0f;
         }
         else
         {
            predictList[i].platelet_volume         = 0.0f;
            predictList[i].plt_mss                 = (templates.key_plt_mss[plt_amap_index - 1] == 1);
            predictList[i].pct_carryover           = templates.key_plt_pct_carryover[plt_amap_index - 1];
            predictList[i].amap_platelet_size      = (AMAPPLTTYPES)plt_amap_index;
            predictList[i].amap_platelet_min_yield = templates.key_plt_amap_min_yield[plt_amap_index - 1];
            predictList[i].amap_platelet_max_yield = templates.key_plt_amap_max_yield[plt_amap_index - 1];
            predictList[i].amap_platelet_conc      = templates.key_plt_amap_conc[plt_amap_index - 1];
            predictList[i].yield                   = 0.0f;
            predictList[i].platelet_volume         = 0.0f;
         }
      }
      else
      {
         const int& yield_volume_index = offlineList[i].key_platelet;

         if (yield_volume_index == 0)
         {
            predictList[i].yield                   = 0.0f;
            predictList[i].platelet_volume         = 0.0f;
            predictList[i].plt_mss                 = false;
            predictList[i].pct_carryover           = 0;

            predictList[i].amap_platelet_size      = AMAP_PLT_NONE;
            predictList[i].amap_platelet_min_yield = 0.0f;
            predictList[i].amap_platelet_max_yield = 0.0f;
            predictList[i].amap_platelet_conc      = 0.0f;
         }
         else
         {
            predictList[i].yield                   = templates.key_plt_yield[yield_volume_index - 1];
            predictList[i].platelet_volume         = templates.key_plt_volume[yield_volume_index - 1];
            predictList[i].plt_mss                 = (templates.key_plt_mss[yield_volume_index - 1] == 1);
            predictList[i].pct_carryover           = templates.key_plt_pct_carryover[yield_volume_index - 1];

            predictList[i].amap_platelet_size      = AMAP_PLT_NONE;
            predictList[i].amap_platelet_min_yield = 0.0f;
            predictList[i].amap_platelet_max_yield = 0.0f;
            predictList[i].amap_platelet_conc      = 0.0f;
         }
      }

      // FillProcedureDescription(templates, offlineList[i], predictList[i], amapPltEnabled);

      if (crit_dose_index == 0)
      {
         predictList[i].rbc_dose       = 0.0f;
         predictList[i].rbc_crit       = 0.0f;
         predictList[i].rbc_mss        = false;
         predictList[i].rbc_mss_volume = 0;
      }
      else
      {
         predictList[i].rbc_dose       = templates.key_rbc_dose[crit_dose_index - 1];
         predictList[i].rbc_crit       = templates.key_rbc_crit[crit_dose_index - 1];
         predictList[i].rbc_mss        = (templates.key_rbc_mss[crit_dose_index - 1] == 1);
         predictList[i].rbc_mss_volume = templates.key_rbc_mss_volume[crit_dose_index - 1];
      }

      predictList[i].max_procedure_time = offlineList[i].key_time;
      predictList[i].designator         = 1;   // All offline procedure default to primary

      // Convert the blood type.
      switch (offlineList[i].key_blood_type)
      {
         case CONFIG_INDEX_BLOOD_TYPE_ALL :
            predictList[i].blood_type = BT_ALL;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_OP_ON :
            predictList[i].blood_type = BT_OP_ON;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_ON :
            predictList[i].blood_type = BT_ON;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN :
            predictList[i].blood_type = BT_OP_ON_AP_AN;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_OP_ON_AN :
            predictList[i].blood_type = BT_OP_ON_AN;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_ON_AN :
            predictList[i].blood_type = BT_ON_AN;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_OP_ON_BP_BN :
            predictList[i].blood_type = BT_OP_ON_BP_BN;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN_BP_BN :
            predictList[i].blood_type = BT_OP_ON_AP_AN_BP_BN;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN_BN :
            predictList[i].blood_type = BT_OP_ON_AP_AN_BN;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_AP_AN :
            predictList[i].blood_type = BT_AP_AN;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_AP_AN_BP_BN_ABP_ABN :
            predictList[i].blood_type = BT_AP_AN_BP_BN_ABP_ABN;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_BP_BN :
            predictList[i].blood_type = BT_BP_BN;
            break;

         case CONFIG_INDEX_BLOOD_TYPE_ABP_ABN :
            predictList[i].blood_type = BT_ABP_ABN;
            break;

         default :
#ifndef __SKIPMSGSYS__
            // Generate a Software A2 Alarm
            anAlarmMsg alarm(CONFIGDAT_VALUE_OUT_OF_RANGE);
            alarm.setAlarm();
#endif
            DataLog(log_level_control_alarm_detail)
               << "Unknown blood type ("
               << offlineList[i].key_blood_type
               << "found during Predict conversion." << endmsg;
            break;
      }

      // Convert the plasma value
      switch (offlineList[i].key_plasma)
      {
         case PLASMA_NONE :
            predictList[i].plasma_volume = 0.0f;
            break;

         case PLASMA_1 :
            predictList[i].plasma_volume = templates.key_pls_volume[0];
            break;

         case PLASMA_2 :
            predictList[i].plasma_volume = templates.key_pls_volume[1];
            break;

         case PLASMA_3 :
            predictList[i].plasma_volume = templates.key_pls_volume[2];
            break;

         case PLASMA_4 :
            predictList[i].plasma_volume = templates.key_pls_volume[3];
            break;

         case PLASMA_5 :
            predictList[i].plasma_volume = templates.key_pls_volume[4];
            break;

         case PLASMA_6 :
            predictList[i].plasma_volume = templates.key_pls_volume[5];
            break;

         case PLASMA_AMAP :
            predictList[i].plasma_volume = PLASMA_AMAP_FLAG;
            break;

         case PLASMA_MAX_CONC :
            predictList[i].plasma_volume = PLASMA_MAX_CONCURRENT_FLAG;
            break;

         case PLASMA_FFP :
            predictList[i].plasma_volume = PLASMA_FFP_FLAG;
            break;

         default :
#ifndef __SKIPMSGSYS__
            // Generate a Software A2 Alarm
            anAlarmMsg alarm(CONFIGDAT_VALUE_OUT_OF_RANGE);
            alarm.setAlarm();
#endif
            DataLog(log_level_control_alarm_detail)
               << "Unknown plasma volume ("
               << offlineList[i].key_plasma
               << "found during Predict conversion." << endmsg;
            break;
      }
   }

   ProcedureList.Procedure.Set(predictList);
}



//
// Lang Config Element Class
//


//
// Base Constructor
//
LangConfigElement::LangConfigElement()
{}

LangConfigElement::~LangConfigElement()
{
   _list.clear();
}


//
// Register
//
bool LangConfigElement::Register (DataStore* ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<LangUnitCfg>::Register(ds, pfr);

   // Next, fill in the list.
   _lang._info            = _Lst.pVar(KEY_LANG);
   _lang._data_ptr.valuei = (int*)&((this->GetRef()).key_lang);
   _lang._storage_type    = ConfigVarList::INT;
   _list.push_back(_lang);

   _height._info            = _Lst.pVar(KEY_HEIGHT);
   _height._data_ptr.valuei = (int*)&((this->GetRef()).key_height);
   _height._storage_type    = ConfigVarList::INT;
   _list.push_back(_height);

   _weight._info            = _Lst.pVar(KEY_WEIGHT);
   _weight._data_ptr.valuei = (int*)&((this->GetRef()).key_weight);
   _weight._storage_type    = ConfigVarList::INT;
   _list.push_back(_weight);

   _date_format._info            = _Lst.pVar(KEY_DATE_FORMAT);
   _date_format._data_ptr.valuei = (int*)&((this->GetRef()).key_date_format);
   _date_format._storage_type    = ConfigVarList::INT;
   _list.push_back(_date_format);

   _time_format._info            = _Lst.pVar(KEY_TIME_FORMAT);
   _time_format._data_ptr.valuei = (int*)&((this->GetRef()).key_time_format);
   _time_format._storage_type    = ConfigVarList::INT;
   _list.push_back(_time_format);

   _decimal_delimiter._info            = _Lst.pVar(KEY_DECIMAL_DELIMITER);
   _decimal_delimiter._data_ptr.valuei = (int*)&((this->GetRef()).key_decimal_delimiter);
   _decimal_delimiter._storage_type    = ConfigVarList::INT;
   _list.push_back(_decimal_delimiter);

   _crit_or_hemoglobin._info            = _Lst.pVar(KEY_CRIT_OR_GLOB);
   _crit_or_hemoglobin._data_ptr.valuei = (int*)&((this->GetRef()).key_crit_or_glob);
   _crit_or_hemoglobin._storage_type    = ConfigVarList::INT;
   _list.push_back(_crit_or_hemoglobin);

   return created;
}


//
// Assignment operator
//
LangConfigElement& LangConfigElement::operator = (const LangUnitCfg& rhs) // parasoft-suppress MRM-43 "It appears to be correct to not assign the base class members _list and _lastError in this case since we're just changing the value of the encapsulated variable."
{
   this->Set(rhs);

   return *this;
}



void LangConfigElement::Log ()
{}



//
// Basic Machine Config Element Class
//

//
// Base Constructor
//
BasicMachineElement::BasicMachineElement()
{}

BasicMachineElement::~BasicMachineElement()
{
   _list.clear();
}


//
// Register
//
bool BasicMachineElement::Register (DataStore* ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<BasicMachineCfg>::Register(ds, pfr);

   // Next, fill in the list.
   _set_time._info            = _Lst.pVar(KEY_SET_TIME);
   _set_time._data_ptr.valuei = (int*)&((this->GetRef()).key_set_time);
   _set_time._storage_type    = ConfigVarList::INT;
   _list.push_back(_set_time);

   _set_date._info            = _Lst.pVar(KEY_SET_DATE);
   _set_date._data_ptr.valuei = (int*)&((this->GetRef()).key_set_date);
   _set_date._storage_type    = ConfigVarList::INT;
   _list.push_back(_set_date);

   _set_audio._info            = _Lst.pVar(KEY_SET_AUDIO);
   _set_audio._data_ptr.valuei = (int*)&((this->GetRef()).key_set_audio);
   _set_audio._storage_type    = ConfigVarList::INT;
   _list.push_back(_set_audio);

   _pword._info            = _Lst.pVar(KEY_PWORD);
   _pword._data_ptr.valuei = (int*)&((this->GetRef()).key_pword);
   _pword._storage_type    = ConfigVarList::INT;
   _list.push_back(_pword);

   return created;
}



//
//
//
bool BasicMachineElement::Valid ()
{
   if (*_set_audio._data_ptr.valuei == 2)
   {
      std::ostringstream errorMsg;

      errorMsg << "Config Elememt " << _set_audio._info->Name() << " failed valid check.  "
               << "Value(" << *_set_audio._data_ptr.valuei << "), Min("
               << _set_audio._info->Min() << "), Max("
               << _set_audio._info->Max() << ")." << std::ends;

      _lastError = errorMsg.str();

      DataLog(log_level_control_alarm_detail) << errorMsg.str() << endmsg;

      return false;
   }
   else
      return ConfigElement<BasicMachineCfg>::Valid();
}


//
// Assignment operator
//
BasicMachineElement& BasicMachineElement::operator = (const BasicMachineCfg& rhs) // parasoft-suppress MRM-43 "It appears to be correct to not assign the base class members _list and _lastError in this case since we're just changing the value of the encapsulated variable."
{
   this->Set(rhs);

   return *this;
}

void BasicMachineElement::Log ()
{}


//
// Procedure Element Class
//

//
// Base Constructor
//
ProcedureElement::ProcedureElement()
   : _featureChanged(), _firstTimeRead(true)
{}

ProcedureElement::~ProcedureElement()
{
   _list.clear();
}


//
// Register
//
bool ProcedureElement::Register (DataStore* ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<ProcedureConfig>::Register(ds, pfr);

   // Next, fill in the list.
   _proc_time._info            = _Lst.pVar(KEY_PROC_TIME);
   _proc_time._data_ptr.valuei = (int*)&((this->GetRef()).key_proc_time);
   _proc_time._storage_type    = ConfigVarList::INT;
   _list.push_back(_proc_time);

   _return_press._info            = _Lst.pVar(KEY_RETURN_PRESS);
   _return_press._data_ptr.valuef = (float*)&((this->GetRef()).key_return_press);
   _return_press._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_return_press);

   _draw_press._info            = _Lst.pVar(KEY_DRAW_PRESS);
   _draw_press._data_ptr.valuef = (float*)&((this->GetRef()).key_draw_press);
   _draw_press._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_draw_press);

   _custom_ratio._info            = _Lst.pVar(KEY_CUSTOM_RATIO);
   _custom_ratio._data_ptr.valuef = (float*)&((this->GetRef()).key_custom_ratio);
   _custom_ratio._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_custom_ratio);

   _plasma_ratio._info            = _Lst.pVar(KEY_PLASMA_RATIO);
   _plasma_ratio._data_ptr.valuef = (float*)&((this->GetRef()).key_plasma_ratio);
   _plasma_ratio._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plasma_ratio);

   _ac_rate._info            = _Lst.pVar(KEY_AC_RATE);
   _ac_rate._data_ptr.valuei = (int*)&((this->GetRef()).key_ac_rate);
   _ac_rate._storage_type    = ConfigVarList::INT;
   _list.push_back(_ac_rate);

   _post_crit._info            = _Lst.pVar(KEY_POST_CRIT);
   _post_crit._data_ptr.valuef = (float*)&((this->GetRef()).key_post_crit);
   _post_crit._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_post_crit);

   _tbv_vol_setting._info            = _Lst.pVar(KEY_TBV_VOL_SETTING);
   _tbv_vol_setting._data_ptr.valuei = (int*)&((this->GetRef()).key_tbv_vol_setting);
   _tbv_vol_setting._storage_type    = ConfigVarList::INT;
   _list.push_back(_tbv_vol_setting);

   _tbv_percent._info            = _Lst.pVar(KEY_TBV_PERCENT);
   _tbv_percent._data_ptr.valuef = (float*)&((this->GetRef()).key_tbv_percent);
   _tbv_percent._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_tbv_percent);

   _weight_setting._info            = _Lst.pVar(KEY_WEIGHT_SETTING);
   _weight_setting._data_ptr.valuef = (float*)&((this->GetRef()).key_weight_setting);
   _weight_setting._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_weight_setting);

   _weight_less_than_vol._info            = _Lst.pVar(KEY_WEIGHT_LESS_THAN_VOL);
   _weight_less_than_vol._data_ptr.valuef = (float*)&((this->GetRef()).key_weight_less_than_vol);
   _weight_less_than_vol._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_weight_less_than_vol);

   _weight_greater_than_vol._info            = _Lst.pVar(KEY_WEIGHT_GREATER_THAN_VOL);
   _weight_greater_than_vol._data_ptr.valuef = (float*)&((this->GetRef()).key_weight_greater_than_vol);
   _weight_greater_than_vol._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_weight_greater_than_vol);

   _key_autoflow._info            = _Lst.pVar(KEY_AUTOFLOW);
   _key_autoflow._data_ptr.valuei = (int*)&((this->GetRef()).key_autoflow);
   _key_autoflow._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_autoflow);

   _ml_per_kg._info            = _Lst.pVar(KEY_ML_PER_KG);
   _ml_per_kg._data_ptr.valuef = (float*)&((this->GetRef()).key_ml_per_kg);
   _ml_per_kg._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_ml_per_kg);

   _drbc_body_volume._info            = _Lst.pVar(KEY_DRBC_BODY_VOL);
   _drbc_body_volume._data_ptr.valuef = (float*)&((this->GetRef()).key_drbc_body_vol);
   _drbc_body_volume._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_drbc_body_volume);

   _max_plasma_vol_during_platelet_collection._info            = _Lst.pVar(KEY_MAX_PLASMA_DURING_PLT_VOL);
   _max_plasma_vol_during_platelet_collection._data_ptr.valuef = (float*)&((this->GetRef()).key_max_plasma_vol_during_platelet_collection);
   _max_plasma_vol_during_platelet_collection._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_max_plasma_vol_during_platelet_collection);

   _max_plasma_vol_during_plasma_collection._info            = _Lst.pVar(KEY_MAX_PLASMA_DURING_PLS_VOL);
   _max_plasma_vol_during_plasma_collection._data_ptr.valuef = (float*)&((this->GetRef()).key_max_plasma_vol_during_plasma_collection);
   _max_plasma_vol_during_plasma_collection._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_max_plasma_vol_during_plasma_collection);

   _post_plat._info            = _Lst.pVar(KEY_POST_PLAT);
   _post_plat._data_ptr.valuef = (float*)&((this->GetRef()).key_post_plat);
   _post_plat._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_post_plat);

   _max_draw_flow._info            = _Lst.pVar(KEY_MAX_DRAW_FLOW);
   _max_draw_flow._data_ptr.valuei = (int*)&((this->GetRef()).key_max_draw_flow);
   _max_draw_flow._storage_type    = ConfigVarList::INT;
   _list.push_back(_max_draw_flow);

   _min_replacement_volume._info            = _Lst.pVar(KEY_MIN_REPLACEMENT_VOLUME);
   _min_replacement_volume._data_ptr.valuei = (int*)&((this->GetRef()).key_min_replacement_volume);
   _min_replacement_volume._storage_type    = ConfigVarList::INT;
   _list.push_back(_min_replacement_volume);

   _plt_mss_split_notif._info            = _Lst.pVar(KEY_PLT_MSS_SPLIT_NOTIF);
   _plt_mss_split_notif._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss_split_notif);
   _plt_mss_split_notif._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_split_notif);

   _key_override_pas_bag_vol._info            = _Lst.pVar(KEY_OVERRIDE_PAS_BAG_VOL);
   _key_override_pas_bag_vol._data_ptr.valuei = (int*)&((this->GetRef()).key_override_pas_bag_vol);
   _key_override_pas_bag_vol._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_override_pas_bag_vol);

   _blood_diversion._info            = _Lst.pVar(KEY_BLOOD_DIVERSION);
   _blood_diversion._data_ptr.valuei = (int*)&((this->GetRef()).key_blood_diversion);
   _blood_diversion._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_diversion);

   _drbc_alert._info            = _Lst.pVar(KEY_DRBC_ALERT);
   _drbc_alert._data_ptr.valuei = (int*)&((this->GetRef()).key_drbc_alert);
   _drbc_alert._storage_type    = ConfigVarList::INT;
   _list.push_back(_drbc_alert);

   _drbc_threshold._info            = _Lst.pVar(KEY_DRBC_THRESHOLD);
   _drbc_threshold._data_ptr.valuei = (int*)&((this->GetRef()).key_drbc_threshold);
   _drbc_threshold._storage_type    = ConfigVarList::INT;
   _list.push_back(_drbc_threshold);

   _plasma_rinseback._info            = _Lst.pVar(KEY_PLASMA_RINSEBACK);
   _plasma_rinseback._data_ptr.valuei = (int*)&((this->GetRef()).key_plasma_rinseback);
   _plasma_rinseback._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_rinseback);

   _saline_rinseback._info            = _Lst.pVar(KEY_SALINE_RINSEBACK);
   _saline_rinseback._data_ptr.valuei = (int*)&((this->GetRef()).key_saline_rinseback);
   _saline_rinseback._storage_type    = ConfigVarList::INT;
   _list.push_back(_saline_rinseback);

   _audit_tracking._info            = _Lst.pVar(KEY_AUDIT_TRACKING);
   _audit_tracking._data_ptr.valuei = (int*)&((this->GetRef()).key_audit_tracking);
   _audit_tracking._storage_type    = ConfigVarList::INT;
   _list.push_back(_audit_tracking);

   _air_removal._info            = _Lst.pVar(KEY_AIR_REMOVAL);
   _air_removal._data_ptr.valuei = (int*)&((this->GetRef()).key_air_removal);
   _air_removal._storage_type    = ConfigVarList::INT;
   _list.push_back(_air_removal);

   _mss_rbc_on._info            = _Lst.pVar(KEY_MSS_RBC_ON);
   _mss_rbc_on._data_ptr.valuei = (int*)&((this->GetRef()).key_mss_rbc_on);
   _mss_rbc_on._storage_type    = ConfigVarList::INT;
   _list.push_back(_mss_rbc_on);

   _mss_plt_on._info            = _Lst.pVar(KEY_MSS_PLT_ON);
   _mss_plt_on._data_ptr.valuei = (int*)&((this->GetRef()).key_mss_plt_on);
   _mss_plt_on._storage_type    = ConfigVarList::INT;
   _list.push_back(_mss_plt_on);

// Removed from 6.0 but needed for 6.1
   _show_pre_aas_flags._info            = _Lst.pVar(KEY_SHOW_PRE_AAS_FLAGS);
   _show_pre_aas_flags._data_ptr.valuei = (int*)&((this->GetRef()).key_show_pre_aas_flags);
   _show_pre_aas_flags._storage_type    = ConfigVarList::INT;
   _list.push_back(_show_pre_aas_flags);

   _rbc_def_bag_vol._info            = _Lst.pVar(KEY_RBC_DEF_BAG_VOL);
   _rbc_def_bag_vol._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_def_bag_vol);
   _rbc_def_bag_vol._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_def_bag_vol);

   _plt_def_bag_vol._info            = _Lst.pVar(KEY_PLT_DEF_BAG_VOL);
   _plt_def_bag_vol._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_def_bag_vol);
   _plt_def_bag_vol._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_def_bag_vol);

   return created;
}



//
// Assignment operator
//
ProcedureElement& ProcedureElement::operator = (const ProcedureConfig& rhs) // parasoft-suppress MRM-43 "It appears to be correct to not assign the base class members _list and _lastError in this case since we're just changing the value of the encapsulated variable."
{
   this->Set(rhs);

   return *this;
}

void ProcedureElement::Log ()
{}

// Do the default read behavior, and then make sure the pre-mss flag makes sense vis-a-vis the config file setting.
bool ProcedureElement::Read (CDatFileReader& datfile)
{
   // Make sure that the message handler is only registered once
   if (_firstTimeRead)
   {
      _firstTimeRead = false;
      _featureChanged.init(Callback<ProcedureElement>(this, &ProcedureElement::FeatureChangedNotification), MessageBase::SNDRCV_LOCAL);
   }

   if (ConfigElement<ProcedureConfig>::Read(datfile))
   {
      validatePreAasFlag();

      return true;
   }

   return false;
}

void ProcedureElement::FeatureChangedNotification (void* data)
{
   validatePreAasFlag();
}

void ProcedureElement::validatePreAasFlag ()
{
   // If the AllowExtraWbcMessages flag is not set in the Software CDS, set _show_pre_aas_flags to 0
   // regardless of config.dat setting.
   if (!Software_CDS::GetInstance().getFeature(AllowExtraWbcMessages))
   {
      DataLog(log_level_config_info) << "Setting _show_pre_aas_flags to 0 based on AllowExtraWbcMessages feature setting." << endmsg;
      *(_show_pre_aas_flags._data_ptr.valuei) = 0;
   }
}


//
// Prediction Element Class
//

//
// Base Constructor
//
PredictionElement::PredictionElement()
{}

PredictionElement::~PredictionElement()
{
   _list.clear();
}


//
// Register
//
bool PredictionElement::Register (DataStore* ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<PredictionConfig>::Register(ds, pfr);

   // Next, fill in the list.
   _rbc_ratio._info            = _Lst.pVar(KEY_RBC_RATIO);
   _rbc_ratio._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_ratio);
   _rbc_ratio._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_ratio);

   _rbc_replace_fluid._info            = _Lst.pVar(KEY_RBC_REPLACE_FLUID);
   _rbc_replace_fluid._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_replace_fluid);
   _rbc_replace_fluid._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_replace_fluid);

   _rbc_fluid_percent._info            = _Lst.pVar(KEY_RBC_FLUID_PERCENT);
   _rbc_fluid_percent._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_fluid_percent);
   _rbc_fluid_percent._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_fluid_percent);

   _plasma_amap_min._info            = _Lst.pVar(KEY_PLASMA_AMAP_MINIMUM);
   _plasma_amap_min._data_ptr.valuei = (int*)&((this->GetRef()).key_pls_amap_min);
   _plasma_amap_min._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_amap_min);

   _plasma_amap_max._info            = _Lst.pVar(KEY_PLASMA_AMAP_MAXIMUM);
   _plasma_amap_max._data_ptr.valuei = (int*)&((this->GetRef()).key_pls_amap_max);
   _plasma_amap_max._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_amap_max);

   _male_only_plasma._info            = _Lst.pVar(KEY_MALE_ONLY_PLASMA);
   _male_only_plasma._data_ptr.valuei = (int*)&((this->GetRef()).key_male_only_plasma);
   _male_only_plasma._storage_type    = ConfigVarList::INT;
   _list.push_back(_male_only_plasma);

   _ffp_volume._info            = _Lst.pVar(KEY_FFP_VOLUME);
   _ffp_volume._data_ptr.valuei = (int*)&((this->GetRef()).key_ffp_volume);
   _ffp_volume._storage_type    = ConfigVarList::INT;
   _list.push_back(_ffp_volume);

   _complementary_plasma._info            = _Lst.pVar(KEY_COMPLEMENTARY_PLASMA);
   _complementary_plasma._data_ptr.valuei = (int*)&((this->GetRef()).key_complementary_plasma);
   _complementary_plasma._storage_type    = ConfigVarList::INT;
   _list.push_back(_complementary_plasma);

   _ysf._info            = _Lst.pVar(KEY_YSF);
   _ysf._data_ptr.valuef = (float*)&((this->GetRef()).key_ysf);
   _ysf._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_ysf);

   _male_only_plt._info            = _Lst.pVar(KEY_MALE_ONLY_PLT);
   _male_only_plt._data_ptr.valuei = (int*)&((this->GetRef()).key_male_only_plt);
   _male_only_plt._storage_type    = ConfigVarList::INT;
   _list.push_back(_male_only_plt);

   _guaranteed_yield._info            = _Lst.pVar(KEY_GUARANTEED_YIELD);
   _guaranteed_yield._data_ptr.valuei = (int*)&((this->GetRef()).key_guaranteed_yield);
   _guaranteed_yield._storage_type    = ConfigVarList::INT;
   _list.push_back(_guaranteed_yield);

   _confidence_level._info            = _Lst.pVar(KEY_GUARANTEED_YIELD_CL);
   _confidence_level._data_ptr.valuei = (int*)&((this->GetRef()).key_confidence_level);
   _confidence_level._storage_type    = ConfigVarList::INT;
   _list.push_back(_confidence_level);

   _coefficient_of_variation._info            = _Lst.pVar(KEY_GUARANTEED_YIELD_CV);
   _coefficient_of_variation._data_ptr.valuef = (float*)&((this->GetRef()).key_coefficient_of_variation);
   _coefficient_of_variation._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_coefficient_of_variation);

   _inlet_management._info            = _Lst.pVar(KEY_INLET_MANAGEMENT);
   _inlet_management._data_ptr.valuei = (int*)&((this->GetRef()).key_inlet_management);
   _inlet_management._storage_type    = ConfigVarList::INT;
   _list.push_back(_inlet_management);

   _inlet_flow_ramp._info            = _Lst.pVar(KEY_INLET_FLOW_RAMP);
   _inlet_flow_ramp._data_ptr.valuei = (int*)&((this->GetRef()).key_inlet_flow_ramp);
   _inlet_flow_ramp._storage_type    = ConfigVarList::INT;
   _list.push_back(_inlet_flow_ramp);

   _return_management._info            = _Lst.pVar(KEY_RETURN_MANAGEMENT);
   _return_management._data_ptr.valuei = (int*)&((this->GetRef()).key_return_management);
   _return_management._storage_type    = ConfigVarList::INT;
   _list.push_back(_return_management);

   _rsf._info            = _Lst.pVar(KEY_RSF);
   _rsf._data_ptr.valuef = (float*)&((this->GetRef()).key_rsf);
   _rsf._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rsf);

   return created;
}



//
// Assignment operator
//
PredictionElement& PredictionElement::operator = (const PredictionConfig& rhs) // parasoft-suppress MRM-43 "It appears to be correct to not assign the base class members _list and _lastError in this case since we're just changing the value of the encapsulated variable."
{
   this->Set(rhs);

   return *this;
}

void PredictionElement::Log ()
{}



//
// ProductTemplates Element Class
//

//
// Base Constructor
//
ProductTemplatesElement::ProductTemplatesElement()
{}

ProductTemplatesElement::~ProductTemplatesElement()
{
   _list.clear();
}


//
// Register
//
bool ProductTemplatesElement::Register (DataStore* ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<ProductTemplates>::Register(ds, pfr);

   // Next, fill in the list.
   _rbc_crit_1._info            = _Lst.pVar(KEY_RBC_CRIT_1);
   _rbc_crit_1._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_crit[0]);
   _rbc_crit_1._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_crit_1);

   _rbc_crit_2._info            = _Lst.pVar(KEY_RBC_CRIT_2);
   _rbc_crit_2._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_crit[1]);
   _rbc_crit_2._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_crit_2);

   _rbc_crit_3._info            = _Lst.pVar(KEY_RBC_CRIT_3);
   _rbc_crit_3._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_crit[2]);
   _rbc_crit_3._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_crit_3);

   _rbc_crit_4._info            = _Lst.pVar(KEY_RBC_CRIT_4);
   _rbc_crit_4._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_crit[3]);
   _rbc_crit_4._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_crit_4);

   _rbc_crit_5._info            = _Lst.pVar(KEY_RBC_CRIT_5);
   _rbc_crit_5._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_crit[4]);
   _rbc_crit_5._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_crit_5);

   _rbc_crit_6._info            = _Lst.pVar(KEY_RBC_CRIT_6);
   _rbc_crit_6._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_crit[5]);
   _rbc_crit_6._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_crit_6);

   _rbc_dose_1._info            = _Lst.pVar(KEY_RBC_DOSE_1);
   _rbc_dose_1._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_dose[0]);
   _rbc_dose_1._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_dose_1);

   _rbc_dose_2._info            = _Lst.pVar(KEY_RBC_DOSE_2);
   _rbc_dose_2._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_dose[1]);
   _rbc_dose_2._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_dose_2);

   _rbc_dose_3._info            = _Lst.pVar(KEY_RBC_DOSE_3);
   _rbc_dose_3._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_dose[2]);
   _rbc_dose_3._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_dose_3);

   _rbc_dose_4._info            = _Lst.pVar(KEY_RBC_DOSE_4);
   _rbc_dose_4._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_dose[3]);
   _rbc_dose_4._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_dose_4);

   _rbc_dose_5._info            = _Lst.pVar(KEY_RBC_DOSE_5);
   _rbc_dose_5._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_dose[4]);
   _rbc_dose_5._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_dose_5);

   _rbc_dose_6._info            = _Lst.pVar(KEY_RBC_DOSE_6);
   _rbc_dose_6._data_ptr.valuef = (float*)&((this->GetRef()).key_rbc_dose[5]);
   _rbc_dose_6._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_rbc_dose_6);

   _rbc_mss_volume_1._info            = _Lst.pVar(KEY_RBC_MSS_VOLUME_1);
   _rbc_mss_volume_1._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss_volume[0]);
   _rbc_mss_volume_1._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_volume_1);

   _rbc_mss_volume_2._info            = _Lst.pVar(KEY_RBC_MSS_VOLUME_2);
   _rbc_mss_volume_2._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss_volume[1]);
   _rbc_mss_volume_2._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_volume_2);

   _rbc_mss_volume_3._info            = _Lst.pVar(KEY_RBC_MSS_VOLUME_3);
   _rbc_mss_volume_3._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss_volume[2]);
   _rbc_mss_volume_3._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_volume_3);

   _rbc_mss_volume_4._info            = _Lst.pVar(KEY_RBC_MSS_VOLUME_4);
   _rbc_mss_volume_4._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss_volume[3]);
   _rbc_mss_volume_4._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_volume_4);

   _rbc_mss_volume_5._info            = _Lst.pVar(KEY_RBC_MSS_VOLUME_5);
   _rbc_mss_volume_5._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss_volume[4]);
   _rbc_mss_volume_5._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_volume_5);

   _rbc_mss_volume_6._info            = _Lst.pVar(KEY_RBC_MSS_VOLUME_6);
   _rbc_mss_volume_6._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss_volume[5]);
   _rbc_mss_volume_6._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_volume_6);

   _rbc_mss_1._info            = _Lst.pVar(KEY_RBC_MSS_1);
   _rbc_mss_1._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss[0]);
   _rbc_mss_1._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_1);

   _rbc_mss_2._info            = _Lst.pVar(KEY_RBC_MSS_2);
   _rbc_mss_2._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss[1]);
   _rbc_mss_2._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_2);

   _rbc_mss_3._info            = _Lst.pVar(KEY_RBC_MSS_3);
   _rbc_mss_3._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss[2]);
   _rbc_mss_3._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_3);

   _rbc_mss_4._info            = _Lst.pVar(KEY_RBC_MSS_4);
   _rbc_mss_4._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss[3]);
   _rbc_mss_4._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_4);

   _rbc_mss_5._info            = _Lst.pVar(KEY_RBC_MSS_5);
   _rbc_mss_5._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss[4]);
   _rbc_mss_5._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_5);

   _rbc_mss_6._info            = _Lst.pVar(KEY_RBC_MSS_6);
   _rbc_mss_6._data_ptr.valuei = (int*)&((this->GetRef()).key_rbc_mss[5]);
   _rbc_mss_6._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_mss_6);

   _plt_yield_1._info            = _Lst.pVar(KEY_PLT_YIELD_1);
   _plt_yield_1._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_yield[0]);
   _plt_yield_1._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_1);

   _plt_yield_2._info            = _Lst.pVar(KEY_PLT_YIELD_2);
   _plt_yield_2._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_yield[1]);
   _plt_yield_2._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_2);

   _plt_yield_3._info            = _Lst.pVar(KEY_PLT_YIELD_3);
   _plt_yield_3._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_yield[2]);
   _plt_yield_3._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_3);

   _plt_yield_4._info            = _Lst.pVar(KEY_PLT_YIELD_4);
   _plt_yield_4._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_yield[3]);
   _plt_yield_4._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_4);

   _plt_yield_5._info            = _Lst.pVar(KEY_PLT_YIELD_5);
   _plt_yield_5._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_yield[4]);
   _plt_yield_5._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_5);

   _plt_yield_6._info            = _Lst.pVar(KEY_PLT_YIELD_6);
   _plt_yield_6._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_yield[5]);
   _plt_yield_6._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_6);

   _plt_yield_7._info            = _Lst.pVar(KEY_PLT_YIELD_7);
   _plt_yield_7._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_yield[6]);
   _plt_yield_7._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_7);

   _plt_yield_8._info            = _Lst.pVar(KEY_PLT_YIELD_8);
   _plt_yield_8._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_yield[7]);
   _plt_yield_8._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_8);

   _plt_yield_9._info            = _Lst.pVar(KEY_PLT_YIELD_9);
   _plt_yield_9._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_yield[8]);
   _plt_yield_9._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_9);

   _plt_yield_10._info            = _Lst.pVar(KEY_PLT_YIELD_10);
   _plt_yield_10._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_yield[9]);
   _plt_yield_10._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_10);

   _plt_volume_1._info            = _Lst.pVar(KEY_PLT_VOLUME_1);
   _plt_volume_1._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_volume[0]);
   _plt_volume_1._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_1);

   _plt_volume_2._info            = _Lst.pVar(KEY_PLT_VOLUME_2);
   _plt_volume_2._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_volume[1]);
   _plt_volume_2._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_2);

   _plt_volume_3._info            = _Lst.pVar(KEY_PLT_VOLUME_3);
   _plt_volume_3._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_volume[2]);
   _plt_volume_3._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_3);

   _plt_volume_4._info            = _Lst.pVar(KEY_PLT_VOLUME_4);
   _plt_volume_4._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_volume[3]);
   _plt_volume_4._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_4);

   _plt_volume_5._info            = _Lst.pVar(KEY_PLT_VOLUME_5);
   _plt_volume_5._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_volume[4]);
   _plt_volume_5._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_5);

   _plt_volume_6._info            = _Lst.pVar(KEY_PLT_VOLUME_6);
   _plt_volume_6._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_volume[5]);
   _plt_volume_6._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_6);

   _plt_volume_7._info            = _Lst.pVar(KEY_PLT_VOLUME_7);
   _plt_volume_7._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_volume[6]);
   _plt_volume_7._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_7);

   _plt_volume_8._info            = _Lst.pVar(KEY_PLT_VOLUME_8);
   _plt_volume_8._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_volume[7]);
   _plt_volume_8._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_8);

   _plt_volume_9._info            = _Lst.pVar(KEY_PLT_VOLUME_9);
   _plt_volume_9._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_volume[8]);
   _plt_volume_9._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_9);

   _plt_volume_10._info            = _Lst.pVar(KEY_PLT_VOLUME_10);
   _plt_volume_10._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_volume[9]);
   _plt_volume_10._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_10);



   _plt_amap_min_yield_1._info            = _Lst.pVar(KEY_PLT_AMAP_MIN_YIELD_1);
   _plt_amap_min_yield_1._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_min_yield[0]);
   _plt_amap_min_yield_1._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_min_yield_1);

   _plt_amap_min_yield_2._info            = _Lst.pVar(KEY_PLT_AMAP_MIN_YIELD_2);
   _plt_amap_min_yield_2._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_min_yield[1]);
   _plt_amap_min_yield_2._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_min_yield_2);

   _plt_amap_min_yield_3._info            = _Lst.pVar(KEY_PLT_AMAP_MIN_YIELD_3);
   _plt_amap_min_yield_3._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_min_yield[2]);
   _plt_amap_min_yield_3._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_min_yield_3);

   _plt_amap_max_yield_1._info            = _Lst.pVar(KEY_PLT_AMAP_MAX_YIELD_1);
   _plt_amap_max_yield_1._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_max_yield[0]);
   _plt_amap_max_yield_1._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_max_yield_1);

   _plt_amap_max_yield_2._info            = _Lst.pVar(KEY_PLT_AMAP_MAX_YIELD_2);
   _plt_amap_max_yield_2._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_max_yield[1]);
   _plt_amap_max_yield_2._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_max_yield_2);

   _plt_amap_max_yield_3._info            = _Lst.pVar(KEY_PLT_AMAP_MAX_YIELD_3);
   _plt_amap_max_yield_3._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_max_yield[2]);
   _plt_amap_max_yield_3._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_max_yield_3);

   _plt_amap_conc_1._info            = _Lst.pVar(KEY_PLT_AMAP_CONC_1);
   _plt_amap_conc_1._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_conc[0]);
   _plt_amap_conc_1._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_conc_1);

   _plt_amap_conc_2._info            = _Lst.pVar(KEY_PLT_AMAP_CONC_2);
   _plt_amap_conc_2._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_conc[1]);
   _plt_amap_conc_2._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_conc_2);

   _plt_amap_conc_3._info            = _Lst.pVar(KEY_PLT_AMAP_CONC_3);
   _plt_amap_conc_3._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_conc[2]);
   _plt_amap_conc_3._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_conc_3);


   _plt_amap_conc_4._info            = _Lst.pVar(KEY_PLT_AMAP_CONC_4);
   _plt_amap_conc_4._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_conc[3]);
   _plt_amap_conc_4._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_conc_4);

   _plt_amap_conc_5._info            = _Lst.pVar(KEY_PLT_AMAP_CONC_5);
   _plt_amap_conc_5._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_conc[4]);
   _plt_amap_conc_5._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_conc_5);

   _plt_amap_conc_6._info            = _Lst.pVar(KEY_PLT_AMAP_CONC_6);
   _plt_amap_conc_6._data_ptr.valuef = (float*)&((this->GetRef()).key_plt_amap_conc[5]);
   _plt_amap_conc_6._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_plt_amap_conc_6);

   _key_plt_amap_pas_pct_carryover_1._info            = _Lst.pVar(KEY_PLT_AMAP_PAS_CARRYOVER_1);
   _key_plt_amap_pas_pct_carryover_1._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_amap_pas_pct_carryover[0]);
   _key_plt_amap_pas_pct_carryover_1._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_amap_pas_pct_carryover_1);

   _key_plt_amap_pas_pct_carryover_2._info            = _Lst.pVar(KEY_PLT_AMAP_PAS_CARRYOVER_2);
   _key_plt_amap_pas_pct_carryover_2._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_amap_pas_pct_carryover[1]);
   _key_plt_amap_pas_pct_carryover_2._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_amap_pas_pct_carryover_2);

   _key_plt_amap_pas_pct_carryover_3._info            = _Lst.pVar(KEY_PLT_AMAP_PAS_CARRYOVER_3);
   _key_plt_amap_pas_pct_carryover_3._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_amap_pas_pct_carryover[2]);
   _key_plt_amap_pas_pct_carryover_3._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_amap_pas_pct_carryover_3);


   _key_plt_pct_carryover_1._info            = _Lst.pVar(KEY_PLT_PCT_CARRYOVER_1);
   _key_plt_pct_carryover_1._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_pct_carryover[0]);
   _key_plt_pct_carryover_1._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_pct_carryover_1);

   _key_plt_pct_carryover_2._info            = _Lst.pVar(KEY_PLT_PCT_CARRYOVER_2);
   _key_plt_pct_carryover_2._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_pct_carryover[1]);
   _key_plt_pct_carryover_2._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_pct_carryover_2);

   _key_plt_pct_carryover_3._info            = _Lst.pVar(KEY_PLT_PCT_CARRYOVER_3);
   _key_plt_pct_carryover_3._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_pct_carryover[2]);
   _key_plt_pct_carryover_3._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_pct_carryover_3);

   _key_plt_pct_carryover_4._info            = _Lst.pVar(KEY_PLT_PCT_CARRYOVER_4);
   _key_plt_pct_carryover_4._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_pct_carryover[3]);
   _key_plt_pct_carryover_4._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_pct_carryover_4);

   _key_plt_pct_carryover_5._info            = _Lst.pVar(KEY_PLT_PCT_CARRYOVER_5);
   _key_plt_pct_carryover_5._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_pct_carryover[4]);
   _key_plt_pct_carryover_5._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_pct_carryover_5);

   _key_plt_pct_carryover_6._info            = _Lst.pVar(KEY_PLT_PCT_CARRYOVER_6);
   _key_plt_pct_carryover_6._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_pct_carryover[5]);
   _key_plt_pct_carryover_6._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_pct_carryover_6);

   _key_plt_pct_carryover_7._info            = _Lst.pVar(KEY_PLT_PCT_CARRYOVER_7);
   _key_plt_pct_carryover_7._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_pct_carryover[6]);
   _key_plt_pct_carryover_7._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_pct_carryover_7);

   _key_plt_pct_carryover_8._info            = _Lst.pVar(KEY_PLT_PCT_CARRYOVER_8);
   _key_plt_pct_carryover_8._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_pct_carryover[7]);
   _key_plt_pct_carryover_8._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_pct_carryover_8);

   _key_plt_pct_carryover_9._info            = _Lst.pVar(KEY_PLT_PCT_CARRYOVER_9);
   _key_plt_pct_carryover_9._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_pct_carryover[8]);
   _key_plt_pct_carryover_9._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_pct_carryover_9);

   _key_plt_pct_carryover_10._info            = _Lst.pVar(KEY_PLT_PCT_CARRYOVER_10);
   _key_plt_pct_carryover_10._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_pct_carryover[9]);
   _key_plt_pct_carryover_10._storage_type    = ConfigVarList::INT;
   _list.push_back(_key_plt_pct_carryover_10);

   _plt_mss_1._info            = _Lst.pVar(KEY_PLT_MSS_1);
   _plt_mss_1._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss[0]);
   _plt_mss_1._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_1);

   _plt_mss_2._info            = _Lst.pVar(KEY_PLT_MSS_2);
   _plt_mss_2._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss[1]);
   _plt_mss_2._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_2);

   _plt_mss_3._info            = _Lst.pVar(KEY_PLT_MSS_3);
   _plt_mss_3._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss[2]);
   _plt_mss_3._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_3);

   _plt_mss_4._info            = _Lst.pVar(KEY_PLT_MSS_4);
   _plt_mss_4._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss[3]);
   _plt_mss_4._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_4);

   _plt_mss_5._info            = _Lst.pVar(KEY_PLT_MSS_5);
   _plt_mss_5._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss[4]);
   _plt_mss_5._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_5);

   _plt_mss_6._info            = _Lst.pVar(KEY_PLT_MSS_6);
   _plt_mss_6._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss[5]);
   _plt_mss_6._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_6);

   _plt_mss_7._info            = _Lst.pVar(KEY_PLT_MSS_7);
   _plt_mss_7._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss[6]);
   _plt_mss_7._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_7);

   _plt_mss_8._info            = _Lst.pVar(KEY_PLT_MSS_8);
   _plt_mss_8._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss[7]);
   _plt_mss_8._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_8);

   _plt_mss_9._info            = _Lst.pVar(KEY_PLT_MSS_9);
   _plt_mss_9._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss[8]);
   _plt_mss_9._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_9);

   _plt_mss_10._info            = _Lst.pVar(KEY_PLT_MSS_10);
   _plt_mss_10._data_ptr.valuei = (int*)&((this->GetRef()).key_plt_mss[9]);
   _plt_mss_10._storage_type    = ConfigVarList::INT;
   _list.push_back(_plt_mss_10);

   _pls_volume_1._info            = _Lst.pVar(KEY_PLS_VOLUME_1);
   _pls_volume_1._data_ptr.valuef = (float*)&((this->GetRef()).key_pls_volume[0]);
   _pls_volume_1._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_1);

   _pls_volume_2._info            = _Lst.pVar(KEY_PLS_VOLUME_2);
   _pls_volume_2._data_ptr.valuef = (float*)&((this->GetRef()).key_pls_volume[1]);
   _pls_volume_2._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_2);

   _pls_volume_3._info            = _Lst.pVar(KEY_PLS_VOLUME_3);
   _pls_volume_3._data_ptr.valuef = (float*)&((this->GetRef()).key_pls_volume[2]);
   _pls_volume_3._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_3);

   _pls_volume_4._info            = _Lst.pVar(KEY_PLS_VOLUME_4);
   _pls_volume_4._data_ptr.valuef = (float*)&((this->GetRef()).key_pls_volume[3]);
   _pls_volume_4._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_4);

   _pls_volume_5._info            = _Lst.pVar(KEY_PLS_VOLUME_5);
   _pls_volume_5._data_ptr.valuef = (float*)&((this->GetRef()).key_pls_volume[4]);
   _pls_volume_5._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_5);

   _pls_volume_6._info            = _Lst.pVar(KEY_PLS_VOLUME_6);
   _pls_volume_6._data_ptr.valuef = (float*)&((this->GetRef()).key_pls_volume[5]);
   _pls_volume_6._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_6);

   return created;
}



//
// Assignment operator
//
ProductTemplatesElement& ProductTemplatesElement::operator = (const ProductTemplates& rhs) // parasoft-suppress MRM-43 "It appears to be correct to not assign the base class members _list and _lastError in this case since we're just changing the value of the encapsulated variable."
{
   this->Set(rhs);

   return *this;
}

void ProductTemplatesElement::Log ()
{}



//
// Product Definition Element Class
//

//
// Base Constructor
//
ProductDefinitionElement::ProductDefinitionElement()
{}

ProductDefinitionElement::~ProductDefinitionElement()
{
   _list.clear();
}


//
// Register
//
bool ProductDefinitionElement::Register (DataStore* ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<ProductDefinitionArray>::Register(ds, pfr);

   // Next, fill in the list.
   const ProductDefinition* product = ((this)->GetRef()).as_array();


   // Platelet
   _platelet_a._info            = _Lst.pVar(KEY_PRI_PLATELET_A);
   _platelet_a._data_ptr.valuei = (int*)&product[0].key_platelet;
   _platelet_a._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_a);

   _platelet_b._info            = _Lst.pVar(KEY_PRI_PLATELET_B);
   _platelet_b._data_ptr.valuei = (int*)&product[1].key_platelet;
   _platelet_b._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_b);

   _platelet_c._info            = _Lst.pVar(KEY_PRI_PLATELET_C);
   _platelet_c._data_ptr.valuei = (int*)&product[2].key_platelet;
   _platelet_c._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_c);

   _platelet_d._info            = _Lst.pVar(KEY_PRI_PLATELET_D);
   _platelet_d._data_ptr.valuei = (int*)&product[3].key_platelet;
   _platelet_d._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_d);

   _platelet_e._info            = _Lst.pVar(KEY_PRI_PLATELET_E);
   _platelet_e._data_ptr.valuei = (int*)&product[4].key_platelet;
   _platelet_e._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_e);

   _platelet_f._info            = _Lst.pVar(KEY_PRI_PLATELET_F);
   _platelet_f._data_ptr.valuei = (int*)&product[5].key_platelet;
   _platelet_f._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_f);

   _platelet_g._info            = _Lst.pVar(KEY_PRI_PLATELET_G);
   _platelet_g._data_ptr.valuei = (int*)&product[6].key_platelet;
   _platelet_g._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_g);

   _platelet_h._info            = _Lst.pVar(KEY_PRI_PLATELET_H);
   _platelet_h._data_ptr.valuei = (int*)&product[7].key_platelet;
   _platelet_h._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_h);

   _platelet_i._info            = _Lst.pVar(KEY_PRI_PLATELET_I);
   _platelet_i._data_ptr.valuei = (int*)&product[8].key_platelet;
   _platelet_i._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_i);

   _platelet_j._info            = _Lst.pVar(KEY_PRI_PLATELET_J);
   _platelet_j._data_ptr.valuei = (int*)&product[9].key_platelet;
   _platelet_j._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_j);

   _platelet_k._info            = _Lst.pVar(KEY_PRI_PLATELET_K);
   _platelet_k._data_ptr.valuei = (int*)&product[10].key_platelet;
   _platelet_k._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_k);

   _platelet_l._info            = _Lst.pVar(KEY_PRI_PLATELET_L);
   _platelet_l._data_ptr.valuei = (int*)&product[11].key_platelet;
   _platelet_l._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_l);

   _platelet_m._info            = _Lst.pVar(KEY_PRI_PLATELET_M);
   _platelet_m._data_ptr.valuei = (int*)&product[12].key_platelet;
   _platelet_m._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_m);

   _platelet_n._info            = _Lst.pVar(KEY_PRI_PLATELET_N);
   _platelet_n._data_ptr.valuei = (int*)&product[13].key_platelet;
   _platelet_n._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_n);

   _platelet_o._info            = _Lst.pVar(KEY_PRI_PLATELET_O);
   _platelet_o._data_ptr.valuei = (int*)&product[14].key_platelet;
   _platelet_o._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_o);

   _platelet_p._info            = _Lst.pVar(KEY_PRI_PLATELET_P);
   _platelet_p._data_ptr.valuei = (int*)&product[15].key_platelet;
   _platelet_p._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_p);

   _platelet_q._info            = _Lst.pVar(KEY_PRI_PLATELET_Q);
   _platelet_q._data_ptr.valuei = (int*)&product[16].key_platelet;
   _platelet_q._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_q);

   _platelet_r._info            = _Lst.pVar(KEY_PRI_PLATELET_R);
   _platelet_r._data_ptr.valuei = (int*)&product[17].key_platelet;
   _platelet_r._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_r);

   _platelet_s._info            = _Lst.pVar(KEY_PRI_PLATELET_S);
   _platelet_s._data_ptr.valuei = (int*)&product[18].key_platelet;
   _platelet_s._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_s);

   _platelet_t._info            = _Lst.pVar(KEY_PRI_PLATELET_T);
   _platelet_t._data_ptr.valuei = (int*)&product[19].key_platelet;
   _platelet_t._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_t);


   // Platelet AMAP
   _platelet_amap_a._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_A);
   _platelet_amap_a._data_ptr.valuei = (int*)&product[0].key_platelet_amap;
   _platelet_amap_a._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_a);

   _platelet_amap_b._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_B);
   _platelet_amap_b._data_ptr.valuei = (int*)&product[1].key_platelet_amap;
   _platelet_amap_b._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_b);

   _platelet_amap_c._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_C);
   _platelet_amap_c._data_ptr.valuei = (int*)&product[2].key_platelet_amap;
   _platelet_amap_c._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_c);

   _platelet_amap_d._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_D);
   _platelet_amap_d._data_ptr.valuei = (int*)&product[3].key_platelet_amap;
   _platelet_amap_d._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_d);

   _platelet_amap_e._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_E);
   _platelet_amap_e._data_ptr.valuei = (int*)&product[4].key_platelet_amap;
   _platelet_amap_e._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_e);

   _platelet_amap_f._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_F);
   _platelet_amap_f._data_ptr.valuei = (int*)&product[5].key_platelet_amap;
   _platelet_amap_f._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_f);

   _platelet_amap_g._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_G);
   _platelet_amap_g._data_ptr.valuei = (int*)&product[6].key_platelet_amap;
   _platelet_amap_g._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_g);

   _platelet_amap_h._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_H);
   _platelet_amap_h._data_ptr.valuei = (int*)&product[7].key_platelet_amap;
   _platelet_amap_h._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_h);

   _platelet_amap_i._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_I);
   _platelet_amap_i._data_ptr.valuei = (int*)&product[8].key_platelet_amap;
   _platelet_amap_i._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_i);

   _platelet_amap_j._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_J);
   _platelet_amap_j._data_ptr.valuei = (int*)&product[9].key_platelet_amap;
   _platelet_amap_j._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_j);

   _platelet_amap_k._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_K);
   _platelet_amap_k._data_ptr.valuei = (int*)&product[10].key_platelet_amap;
   _platelet_amap_k._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_k);

   _platelet_amap_l._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_L);
   _platelet_amap_l._data_ptr.valuei = (int*)&product[11].key_platelet_amap;
   _platelet_amap_l._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_l);

   _platelet_amap_m._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_M);
   _platelet_amap_m._data_ptr.valuei = (int*)&product[12].key_platelet_amap;
   _platelet_amap_m._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_m);

   _platelet_amap_n._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_N);
   _platelet_amap_n._data_ptr.valuei = (int*)&product[13].key_platelet_amap;
   _platelet_amap_n._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_n);

   _platelet_amap_o._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_O);
   _platelet_amap_o._data_ptr.valuei = (int*)&product[14].key_platelet_amap;
   _platelet_amap_o._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_o);

   _platelet_amap_p._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_P);
   _platelet_amap_p._data_ptr.valuei = (int*)&product[15].key_platelet_amap;
   _platelet_amap_p._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_p);

   _platelet_amap_q._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_Q);
   _platelet_amap_q._data_ptr.valuei = (int*)&product[16].key_platelet_amap;
   _platelet_amap_q._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_q);

   _platelet_amap_r._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_R);
   _platelet_amap_r._data_ptr.valuei = (int*)&product[17].key_platelet_amap;
   _platelet_amap_r._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_r);

   _platelet_amap_s._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_S);
   _platelet_amap_s._data_ptr.valuei = (int*)&product[18].key_platelet_amap;
   _platelet_amap_s._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_s);

   _platelet_amap_t._info            = _Lst.pVar(KEY_PRI_PLATELET_AMAP_T);
   _platelet_amap_t._data_ptr.valuei = (int*)&product[19].key_platelet_amap;
   _platelet_amap_t._storage_type    = ConfigVarList::INT;
   _list.push_back(_platelet_amap_t);

   // Plasma
   _plasma_a._info            = _Lst.pVar(KEY_PRI_PLASMA_A);
   _plasma_a._data_ptr.valuei = (int*)&product[0].key_plasma;
   _plasma_a._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_a);

   _plasma_b._info            = _Lst.pVar(KEY_PRI_PLASMA_B);
   _plasma_b._data_ptr.valuei = (int*)&product[1].key_plasma;
   _plasma_b._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_b);

   _plasma_c._info            = _Lst.pVar(KEY_PRI_PLASMA_C);
   _plasma_c._data_ptr.valuei = (int*)&product[2].key_plasma;
   _plasma_c._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_c);

   _plasma_d._info            = _Lst.pVar(KEY_PRI_PLASMA_D);
   _plasma_d._data_ptr.valuei = (int*)&product[3].key_plasma;
   _plasma_d._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_d);

   _plasma_e._info            = _Lst.pVar(KEY_PRI_PLASMA_E);
   _plasma_e._data_ptr.valuei = (int*)&product[4].key_plasma;
   _plasma_e._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_e);

   _plasma_f._info            = _Lst.pVar(KEY_PRI_PLASMA_F);
   _plasma_f._data_ptr.valuei = (int*)&product[5].key_plasma;
   _plasma_f._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_f);

   _plasma_g._info            = _Lst.pVar(KEY_PRI_PLASMA_G);
   _plasma_g._data_ptr.valuei = (int*)&product[6].key_plasma;
   _plasma_g._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_g);

   _plasma_h._info            = _Lst.pVar(KEY_PRI_PLASMA_H);
   _plasma_h._data_ptr.valuei = (int*)&product[7].key_plasma;
   _plasma_h._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_h);

   _plasma_i._info            = _Lst.pVar(KEY_PRI_PLASMA_I);
   _plasma_i._data_ptr.valuei = (int*)&product[8].key_plasma;
   _plasma_i._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_i);

   _plasma_j._info            = _Lst.pVar(KEY_PRI_PLASMA_J);
   _plasma_j._data_ptr.valuei = (int*)&product[9].key_plasma;
   _plasma_j._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_j);

   _plasma_k._info            = _Lst.pVar(KEY_PRI_PLASMA_K);
   _plasma_k._data_ptr.valuei = (int*)&product[10].key_plasma;
   _plasma_k._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_k);

   _plasma_l._info            = _Lst.pVar(KEY_PRI_PLASMA_L);
   _plasma_l._data_ptr.valuei = (int*)&product[11].key_plasma;
   _plasma_l._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_l);

   _plasma_m._info            = _Lst.pVar(KEY_PRI_PLASMA_M);
   _plasma_m._data_ptr.valuei = (int*)&product[12].key_plasma;
   _plasma_m._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_m);

   _plasma_n._info            = _Lst.pVar(KEY_PRI_PLASMA_N);
   _plasma_n._data_ptr.valuei = (int*)&product[13].key_plasma;
   _plasma_n._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_n);

   _plasma_o._info            = _Lst.pVar(KEY_PRI_PLASMA_O);
   _plasma_o._data_ptr.valuei = (int*)&product[14].key_plasma;
   _plasma_o._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_o);

   _plasma_p._info            = _Lst.pVar(KEY_PRI_PLASMA_P);
   _plasma_p._data_ptr.valuei = (int*)&product[15].key_plasma;
   _plasma_p._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_p);

   _plasma_q._info            = _Lst.pVar(KEY_PRI_PLASMA_Q);
   _plasma_q._data_ptr.valuei = (int*)&product[16].key_plasma;
   _plasma_q._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_q);

   _plasma_r._info            = _Lst.pVar(KEY_PRI_PLASMA_R);
   _plasma_r._data_ptr.valuei = (int*)&product[17].key_plasma;
   _plasma_r._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_r);

   _plasma_s._info            = _Lst.pVar(KEY_PRI_PLASMA_S);
   _plasma_s._data_ptr.valuei = (int*)&product[18].key_plasma;
   _plasma_s._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_s);

   _plasma_t._info            = _Lst.pVar(KEY_PRI_PLASMA_T);
   _plasma_t._data_ptr.valuei = (int*)&product[19].key_plasma;
   _plasma_t._storage_type    = ConfigVarList::INT;
   _list.push_back(_plasma_t);

   // RBC
   _rbc_a._info            = _Lst.pVar(KEY_PRI_RBC_A);
   _rbc_a._data_ptr.valuei = (int*)&(product[0].key_rbc);
   _rbc_a._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_a);

   _rbc_b._info            = _Lst.pVar(KEY_PRI_RBC_B);
   _rbc_b._data_ptr.valuei = (int*)&(product[1].key_rbc);
   _rbc_b._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_b);

   _rbc_c._info            = _Lst.pVar(KEY_PRI_RBC_C);
   _rbc_c._data_ptr.valuei = (int*)&(product[2].key_rbc);
   _rbc_c._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_c);

   _rbc_d._info            = _Lst.pVar(KEY_PRI_RBC_D);
   _rbc_d._data_ptr.valuei = (int*)&(product[3].key_rbc);
   _rbc_d._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_d);

   _rbc_e._info            = _Lst.pVar(KEY_PRI_RBC_E);
   _rbc_e._data_ptr.valuei = (int*)&(product[4].key_rbc);
   _rbc_e._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_e);

   _rbc_f._info            = _Lst.pVar(KEY_PRI_RBC_F);
   _rbc_f._data_ptr.valuei = (int*)&(product[5].key_rbc);
   _rbc_f._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_f);

   _rbc_g._info            = _Lst.pVar(KEY_PRI_RBC_G);
   _rbc_g._data_ptr.valuei = (int*)&(product[6].key_rbc);
   _rbc_g._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_g);

   _rbc_h._info            = _Lst.pVar(KEY_PRI_RBC_H);
   _rbc_h._data_ptr.valuei = (int*)&(product[7].key_rbc);
   _rbc_h._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_h);

   _rbc_i._info            = _Lst.pVar(KEY_PRI_RBC_I);
   _rbc_i._data_ptr.valuei = (int*)&(product[8].key_rbc);
   _rbc_i._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_i);

   _rbc_j._info            = _Lst.pVar(KEY_PRI_RBC_J);
   _rbc_j._data_ptr.valuei = (int*)&(product[9].key_rbc);
   _rbc_j._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_j);

   _rbc_k._info            = _Lst.pVar(KEY_PRI_RBC_K);
   _rbc_k._data_ptr.valuei = (int*)&(product[10].key_rbc);
   _rbc_k._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_k);

   _rbc_l._info            = _Lst.pVar(KEY_PRI_RBC_L);
   _rbc_l._data_ptr.valuei = (int*)&(product[11].key_rbc);
   _rbc_l._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_l);

   _rbc_m._info            = _Lst.pVar(KEY_PRI_RBC_M);
   _rbc_m._data_ptr.valuei = (int*)&(product[12].key_rbc);
   _rbc_m._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_m);

   _rbc_n._info            = _Lst.pVar(KEY_PRI_RBC_N);
   _rbc_n._data_ptr.valuei = (int*)&(product[13].key_rbc);
   _rbc_n._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_n);

   _rbc_o._info            = _Lst.pVar(KEY_PRI_RBC_O);
   _rbc_o._data_ptr.valuei = (int*)&(product[14].key_rbc);
   _rbc_o._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_o);

   _rbc_p._info            = _Lst.pVar(KEY_PRI_RBC_P);
   _rbc_p._data_ptr.valuei = (int*)&(product[15].key_rbc);
   _rbc_p._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_p);

   _rbc_q._info            = _Lst.pVar(KEY_PRI_RBC_Q);
   _rbc_q._data_ptr.valuei = (int*)&(product[16].key_rbc);
   _rbc_q._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_q);

   _rbc_r._info            = _Lst.pVar(KEY_PRI_RBC_R);
   _rbc_r._data_ptr.valuei = (int*)&(product[17].key_rbc);
   _rbc_r._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_r);

   _rbc_s._info            = _Lst.pVar(KEY_PRI_RBC_S);
   _rbc_s._data_ptr.valuei = (int*)&(product[18].key_rbc);
   _rbc_s._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_s);

   _rbc_t._info            = _Lst.pVar(KEY_PRI_RBC_T);
   _rbc_t._data_ptr.valuei = (int*)&(product[19].key_rbc);
   _rbc_t._storage_type    = ConfigVarList::INT;
   _list.push_back(_rbc_t);

   // Blood Type
   _blood_type_a._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_A);
   _blood_type_a._data_ptr.valuei = (int*)&(product[0].key_blood_type);
   _blood_type_a._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_a);

   _blood_type_b._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_B);
   _blood_type_b._data_ptr.valuei = (int*)&(product[1].key_blood_type);
   _blood_type_b._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_b);

   _blood_type_c._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_C);
   _blood_type_c._data_ptr.valuei = (int*)&(product[2].key_blood_type);
   _blood_type_c._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_c);

   _blood_type_d._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_D);
   _blood_type_d._data_ptr.valuei = (int*)&(product[3].key_blood_type);
   _blood_type_d._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_d);

   _blood_type_e._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_E);
   _blood_type_e._data_ptr.valuei = (int*)&(product[4].key_blood_type);
   _blood_type_e._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_e);

   _blood_type_f._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_F);
   _blood_type_f._data_ptr.valuei = (int*)&(product[5].key_blood_type);
   _blood_type_f._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_f);

   _blood_type_g._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_G);
   _blood_type_g._data_ptr.valuei = (int*)&(product[6].key_blood_type);
   _blood_type_g._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_g);

   _blood_type_h._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_H);
   _blood_type_h._data_ptr.valuei = (int*)&(product[7].key_blood_type);
   _blood_type_h._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_h);

   _blood_type_i._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_I);
   _blood_type_i._data_ptr.valuei = (int*)&(product[8].key_blood_type);
   _blood_type_i._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_i);

   _blood_type_j._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_J);
   _blood_type_j._data_ptr.valuei = (int*)&(product[9].key_blood_type);
   _blood_type_j._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_j);

   _blood_type_k._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_K);
   _blood_type_k._data_ptr.valuei = (int*)&(product[10].key_blood_type);
   _blood_type_k._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_k);

   _blood_type_l._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_L);
   _blood_type_l._data_ptr.valuei = (int*)&(product[11].key_blood_type);
   _blood_type_l._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_l);

   _blood_type_m._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_M);
   _blood_type_m._data_ptr.valuei = (int*)&(product[12].key_blood_type);
   _blood_type_m._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_m);

   _blood_type_n._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_N);
   _blood_type_n._data_ptr.valuei = (int*)&(product[13].key_blood_type);
   _blood_type_n._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_n);

   _blood_type_o._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_O);
   _blood_type_o._data_ptr.valuei = (int*)&(product[14].key_blood_type);
   _blood_type_o._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_o);

   _blood_type_p._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_P);
   _blood_type_p._data_ptr.valuei = (int*)&(product[15].key_blood_type);
   _blood_type_p._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_p);

   _blood_type_q._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_Q);
   _blood_type_q._data_ptr.valuei = (int*)&(product[16].key_blood_type);
   _blood_type_q._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_q);

   _blood_type_r._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_R);
   _blood_type_r._data_ptr.valuei = (int*)&(product[17].key_blood_type);
   _blood_type_r._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_r);

   _blood_type_s._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_S);
   _blood_type_s._data_ptr.valuei = (int*)&(product[18].key_blood_type);
   _blood_type_s._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_s);

   _blood_type_t._info            = _Lst.pVar(KEY_PRI_BLOOD_TYPE_T);
   _blood_type_t._data_ptr.valuei = (int*)&(product[19].key_blood_type);
   _blood_type_t._storage_type    = ConfigVarList::INT;
   _list.push_back(_blood_type_t);

   // Time
   _time_a._info            = _Lst.pVar(KEY_PRI_TIME_A);
   _time_a._data_ptr.valuef = (float*)&(product[0].key_time);
   _time_a._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_a);

   _time_b._info            = _Lst.pVar(KEY_PRI_TIME_B);
   _time_b._data_ptr.valuef = (float*)&(product[1].key_time);
   _time_b._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_b);

   _time_c._info            = _Lst.pVar(KEY_PRI_TIME_C);
   _time_c._data_ptr.valuef = (float*)&(product[2].key_time);
   _time_c._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_c);

   _time_d._info            = _Lst.pVar(KEY_PRI_TIME_D);
   _time_d._data_ptr.valuef = (float*)&(product[3].key_time);
   _time_d._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_d);

   _time_e._info            = _Lst.pVar(KEY_PRI_TIME_E);
   _time_e._data_ptr.valuef = (float*)&(product[4].key_time);
   _time_e._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_e);

   _time_f._info            = _Lst.pVar(KEY_PRI_TIME_F);
   _time_f._data_ptr.valuef = (float*)&(product[5].key_time);
   _time_f._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_f);

   _time_g._info            = _Lst.pVar(KEY_PRI_TIME_G);
   _time_g._data_ptr.valuef = (float*)&(product[6].key_time);
   _time_g._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_g);

   _time_h._info            = _Lst.pVar(KEY_PRI_TIME_H);
   _time_h._data_ptr.valuef = (float*)&(product[7].key_time);
   _time_h._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_h);

   _time_i._info            = _Lst.pVar(KEY_PRI_TIME_I);
   _time_i._data_ptr.valuef = (float*)&(product[8].key_time);
   _time_i._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_i);

   _time_j._info            = _Lst.pVar(KEY_PRI_TIME_J);
   _time_j._data_ptr.valuef = (float*)&(product[9].key_time);
   _time_j._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_j);

   _time_k._info            = _Lst.pVar(KEY_PRI_TIME_K);
   _time_k._data_ptr.valuef = (float*)&(product[10].key_time);
   _time_k._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_k);

   _time_l._info            = _Lst.pVar(KEY_PRI_TIME_L);
   _time_l._data_ptr.valuef = (float*)&(product[11].key_time);
   _time_l._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_l);

   _time_m._info            = _Lst.pVar(KEY_PRI_TIME_M);
   _time_m._data_ptr.valuef = (float*)&(product[12].key_time);
   _time_m._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_m);

   _time_n._info            = _Lst.pVar(KEY_PRI_TIME_N);
   _time_n._data_ptr.valuef = (float*)&(product[13].key_time);
   _time_n._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_n);

   _time_o._info            = _Lst.pVar(KEY_PRI_TIME_O);
   _time_o._data_ptr.valuef = (float*)&(product[14].key_time);
   _time_o._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_o);

   _time_p._info            = _Lst.pVar(KEY_PRI_TIME_P);
   _time_p._data_ptr.valuef = (float*)&(product[15].key_time);
   _time_p._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_p);

   _time_q._info            = _Lst.pVar(KEY_PRI_TIME_Q);
   _time_q._data_ptr.valuef = (float*)&(product[16].key_time);
   _time_q._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_q);

   _time_r._info            = _Lst.pVar(KEY_PRI_TIME_R);
   _time_r._data_ptr.valuef = (float*)&(product[17].key_time);
   _time_r._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_r);

   _time_s._info            = _Lst.pVar(KEY_PRI_TIME_S);
   _time_s._data_ptr.valuef = (float*)&(product[18].key_time);
   _time_s._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_s);

   _time_t._info            = _Lst.pVar(KEY_PRI_TIME_T);
   _time_t._data_ptr.valuef = (float*)&(product[19].key_time);
   _time_t._storage_type    = ConfigVarList::FLOAT;
   _list.push_back(_time_t);

   return created;
}



//
// Assignment operator
//
ProductDefinitionElement& ProductDefinitionElement::operator = (const ProductDefinitionArray& rhs) // parasoft-suppress MRM-43 "It appears to be correct to not assign the base class members _list and _lastError in this case since we're just changing the value of the encapsulated variable."
{
   this->Set(rhs);

   return *this;
}

void ProductDefinitionElement::Log ()
{}


ProcedureListElement::ProcedureListElement()
{}

ProcedureListElement::~ProcedureListElement()
{}

void ProcedureListElement::Log ()
{}

#ifdef __WIN32__
ConfigStruct Config_CDS::getConfigStruct ()
{
   ConfigStruct cs;
   cs.langCfg       = LangUnit.GetRef();
   cs.machineCfg    = Machine.GetRef();
   cs.predictionCfg = Predict.GetRef();
   cs.procedureCfg  = Procedure.GetRef();
   cs.prodDefArray  = Definition.GetRef();
   cs.prodTemplates = Templates.GetRef();
   return cs;
}
void Config_CDS::setConfig (ConfigStruct cs)
{
   LangUnit   = cs.langCfg;
   Machine    = cs.machineCfg;
   Procedure  = cs.procedureCfg;
   Templates  = cs.prodTemplates;
   Predict    = cs.predictionCfg;
   Definition = cs.prodDefArray;
}

bool Config_CDS::WriteConfig (std::string filepath)
{

   // flush file...
   std::ofstream outfile;
   outfile.open(filepath.c_str());
   outfile.close();


   bool successful_write = Write();
   if (successful_write)
   {
      _datfile.Write(filepath.c_str());
   }

   return successful_write;
}
#endif



Config_CDS& Config_CDS::GetInstance (void)
{
   static Config_CDS instance(ROLE_RO);

   return instance;
}



#if CPU==SIMNT
#if 0
unsigned long Config_CDS::SWOptionGet (const char* optionName)
{
   unsigned long setting = 0;

   if (trimaSysGetSoftwareSetting(optionName, &setting))
   {
      DataLog(log_level_control_alarm_detail)
         << "Couldn't read the software option: "
         << optionName
         << endmsg;
#ifndef __SKIPMSGSYS__
      anAlarmMsg alarm(SW_CONFIG_VALUE_NOT_FOUND);
      alarm.setAlarm();
#endif
   }
   return setting;
}


//
// Check if the specified software option is enabled, given the setting
// from features.bin.
//
bool Config_CDS::SWOptionEnabled (const char* optionName)
{
#if CPU==SIMNT
   if (optionOverrides.count(optionName) == 0)
   {

#endif /* if CPU==SIMNT */

   bool result           = false;
   unsigned long setting = this->SWOptionGet(optionName);

   if ( setting & 0x01 )
   {
      result = true;
   }

   return result;

#if CPU==SIMNT
}
else
{
   DataLog(log_level_gui_info) << "Returning Overridden Config CDS key: " << optionName << " - " << optionOverrides[optionName] << endmsg;
   return optionOverrides[optionName];
}
#endif /* CPU==SIMNT */
}
#endif

// Function that will "overwrite" a value in the CDS.  It really just puts it in an array that is checked first.
void Config_CDS::SWOptionSet (const std::string& optionName, bool value)
{
   optionOverrides[optionName] = value;
}

// Unsets our override value.
void Config_CDS::SWOptionUnset (const std::string& optionName)
{
   optionOverrides.erase(optionName);
}
#endif /* if CPU==SIMNT */

/* FORMAT HASH 5a6529102fdc3d4b827ce4ff7769ebca */
