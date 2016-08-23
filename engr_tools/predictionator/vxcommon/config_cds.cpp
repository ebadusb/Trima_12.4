
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
 * HISTORY:    $Log: config_cds.cpp $
 * HISTORY:    Revision 1.2  2007/11/14 23:33:38Z  dslausb
 * HISTORY:    Take out trima_opsys (it no longer exists).
 * HISTORY:    Revision 1.1  2005/12/16 21:49:49Z  rm70006
 * HISTORY:    Initial revision
 * HISTORY:    Revision 1.49  2004/05/07 21:42:04Z  rm70006
 * HISTORY:    IT 6963.
 * HISTORY:    Fix problem in config.dat where the crc file was being set to read only and was interfering with EMS updates.
 * HISTORY:    Revision 1.48  2003/11/25 15:34:39Z  rm10919
 * HISTORY:    Make the weight-volumes from config donor volume screen reflect if there is a four way cut off or not.
 * HISTORY:    Revision 1.47  2003/10/16 16:06:44Z  rm70006
 * HISTORY:    IT 6518.  Fixed problem with logging.
 * HISTORY:    Revision 1.46  2003/09/10 23:33:17Z  jd11007
 * HISTORY:    Revision 1.45  2003/08/19 12:31:22Z  rm70006
 * HISTORY:    IT 6277.  Fix template syntax so it will compile on Windows.
 * HISTORY:    Revision 1.44  2003/08/11 15:47:20Z  ms10234
 * HISTORY:    6227 - changes to detect missing elements
 * HISTORY:    Revision 1.43  2003/08/06 15:41:58Z  rm70006
 * HISTORY:    IT 6277.  Added checking for audio level of 2.
 * HISTORY:    Revision 1.42  2003/07/09 16:26:30Z  sb07663
 * HISTORY:    Changes to allow Vista offline prediction build (under IT6119)
 * HISTORY:    Revision 1.41  2003/06/26 22:36:50Z  jl11312
 * HISTORY:    - added support for sw.dat
 * HISTORY:    Revision 1.40  2003/06/19 18:40:39Z  ms10234
 * HISTORY:    5829 - changes for PFR
 * HISTORY:    Revision 1.39  2003/04/21 18:38:23Z  rm70006
 * HISTORY:    IT 5953.
 * HISTORY:    
 * HISTORY:    default product designator to 1 for all offline procedures.
 * HISTORY:    Revision 1.38  2003/04/18 20:03:39Z  rm70006
 * HISTORY:    IT 5671.
 * HISTORY:    
 * HISTORY:    Fixed wrong column labels.
 * HISTORY:    Revision 1.37  2003/04/09 16:44:32Z  sb07663
 * HISTORY:    Minor ifdef mod to allow offline use (checked in under IT5808)
 * HISTORY:    Revision 1.36  2003/04/08 16:33:13Z  jl11312
 * HISTORY:    - changed Get() call to GetRef()
 * HISTORY:    Revision 1.35  2003/04/03 17:51:00Z  rm10919
 * HISTORY:    donor volume changes
 * HISTORY:    Revision 1.34  2003/04/03 14:59:46Z  jl11312
 * HISTORY:    - modified to use new log levels
 * HISTORY:    - moved config data to a periodic log table
 * HISTORY:    Revision 1.33  2003/04/01 22:13:13Z  rm10919
 * HISTORY:    Changes for audit tracking and donor volume configuration.
 * HISTORY:    Revision 1.32  2003/02/24 20:12:38Z  jl11312
 * HISTORY:    - corrected handling of config.dat related alarms
 * HISTORY:    Revision 1.31  2003/01/30 17:59:52  rm70006
 * HISTORY:    Add ability to get back error strings in valid calls.
 * HISTORY:    Revision 1.30  2003/01/23 18:15:21Z  ms10234
 * HISTORY:    added log config function
 * HISTORY:    Revision 1.29  2003/01/10 20:51:16Z  sb07663
 * HISTORY:    added max concurrent option back in
 * HISTORY:    Revision 1.28  2003/01/10 17:52:37Z  rm70006
 * HISTORY:    Make changes for 5.J config changes.
 * HISTORY:    Revision 1.27  2002/12/30 17:58:21Z  rm70006
 * HISTORY:    RBC Replacement Fluid field was mistakenly being validated against the range for MAX Return Pressure.
 * HISTORY:    Revision 1.26  2002/12/19 16:19:16Z  jl11312
 * HISTORY:    - added readConfig function
 * HISTORY:    Revision 1.25  2002/12/11 18:33:28  sb07663
 * HISTORY:    Changes for offline use - ability to read from a path + conditional win32 compilation
 * HISTORY:    Revision 1.23  2002/11/27 14:46:55Z  jl11312
 * HISTORY:    - removed unused Valid override in LangConfigElement
 * HISTORY:    Revision 1.22  2002/11/25 15:50:04  rm70006
 * HISTORY:    Use more strict syntax for templates to support T2.2 compiler.
 * HISTORY:    Revision 1.21  2002/11/19 19:09:29Z  jl11312
 * HISTORY:    - added missing Register calls
 * HISTORY:    Revision 1.20  2002/11/18 18:31:00  jl11312
 * HISTORY:    - modifications to improve compile speed, reduce inline function sizes
 * HISTORY:    Revision 1.19  2002/10/31 19:55:31  rm70006
 * HISTORY:    Make changes resulting from _debug and _fatal no longer being static member vars.
 * HISTORY:    Revision 1.18  2002/10/29 15:51:40Z  rm70006
 * HISTORY:    Added assignment operator to CDS.
 * HISTORY:    Revision 1.17  2002/10/18 20:01:46Z  rm70006
 * HISTORY:    Changed register call to remove role.
 * HISTORY:    Revision 1.16  2002/10/04 13:39:12Z  rm70006
 * HISTORY:    Fixed problem with blood type conversion with predict std::list.
 * HISTORY:    Revision 1.15  2002/10/01 19:20:15Z  rm70006
 * HISTORY:    Fixed bug in predict std::list copy.  Added logging of predict std::list.
 * HISTORY:    Revision 1.14  2002/09/26 21:35:20Z  rm70006
 * HISTORY:    Fix problems with updating the predict procedure std::list.
 * HISTORY:    Revision 1.13  2002/09/26 19:05:29Z  rm70006
 * HISTORY:    Fix bug in Write and Validate to use correct type of value.
 * HISTORY:    Make datafile var to be static so all instances can write after one of them reads.
 * HISTORY:    Revision 1.12  2002/09/25 16:02:35Z  rm70006
 * HISTORY:    Had to add a new call to DataStore to return a reference since the get call changed.
 * HISTORY:    Revision 1.11  2002/08/23 14:59:37Z  rm70006
 * HISTORY:    Fixed bug with read and unwanted float conversion.
 * HISTORY:    Revision 1.9  2002/07/18 21:26:12Z  rm70006
 * HISTORY:    Added update of procedure std::list for predict.
 * HISTORY:    Changed to be multiwriter.  Removed code to read config file on startup.
 * HISTORY:    Revision 1.8  2002/07/05 19:57:03Z  rm70006
 * HISTORY:    Added predict-style procedure std::list.
 * HISTORY:    Revision 1.7  2002/07/05 16:39:21Z  rm70006
 * HISTORY:    Added ConfigChanged message send call.  Write function automatically sends message.
 * HISTORY:    Revision 1.6  2002/07/02 20:44:37Z  sb07663
 * HISTORY:    typo fix for platelet priority B element
 * HISTORY:    Revision 1.5  2002/07/02 19:34:12  rm70006
 * HISTORY:    Move std::list init code to register function so that it occurs after elements have been bound.
 * HISTORY:    Revision 1.4  2002/06/26 21:30:20Z  rm70006
 * HISTORY:    Move template routines to .h file.
 * HISTORY:    Revision 1.3  2002/06/24 19:45:51Z  rm70006
 * HISTORY:    Add default constructor/destructor
 * HISTORY:    Revision 1.2  2002/06/21 21:29:59Z  sb07663
 * HISTORY:    Added error.h, needed for FATAL_ERROR calls
 * HISTORY:    Revision 1.1  2002/06/19 17:50:32  rm70006
 * HISTORY:    Initial revision
*******************************************************************************/

#include <vxWorks.h>

#ifndef WIN32
#pragma implementation "config_cds.h"
#include "../os/trima_options.h"
#else
#include <cstdio>
#endif

#include "config_cds.h"
#include "datalog_periodic.h"
#include "filenames.h"
#include "trimamessages.h"
#include "an_alarm.h"
#include "bloodtype.h"
#include "messagesystem.h"
#include "filehelp.h"

static CCfgVarList _Lst(1);

//
// Write
//
template <class dataType> bool ConfigElement<dataType>::Write(CDatFileReader &datfile)
{
   ConfigVarList item;

   // Don't write unless the section is valid.
   if (!Valid())
       return false;

   for (CONFIG_VAR_LIST::iterator i = _list.begin(); i != _list.end(); ++i)
   {
      item = *i;
      switch (item._storage_type)
      {
      case ConfigVarList::INT:
         if (!datfile.SetInt(item._info->Section() , item._info->Name() , *item._data_ptr.valuei))
         {
            DataLog(log_level_control_alarm_detail)
                            << "Couldn't write to Config.  Section(" << item._info->Section()
                            << "), Name(" << item._info->Name()
                            << "), Value(" << *item._data_ptr.valuei << ")." << endmsg;

            anAlarmMsg alarm(CONFIGDAT_BAD_FILE);
            alarm.setAlarm();
         }

         break;

      case ConfigVarList::FLOAT:
         if (!datfile.SetFloat(item._info->Section() , item._info->Name() , *item._data_ptr.valuef))
         {
            DataLog(log_level_control_alarm_detail)
                            << "Couldn't write to Config.  Section(" << item._info->Section()
                            << "), Name(" << item._info->Name()
                            << "), Value(" << *item._data_ptr.valuef << ")." << endmsg;

            anAlarmMsg alarm(CONFIGDAT_BAD_FILE);
            alarm.setAlarm();
         }

         break;

      case ConfigVarList::UNKNOWN:
      default:
         DataLog(log_level_control_alarm_detail)
                         << "Config Element not recognized.  Section(" << item._info->Section()
                         << "), storage_type(" << item._storage_type
                         << "), Name(" << item._info->Name()
                         << "Value(" << *item._data_ptr.valuef << "), Min("
                         << item._info->Min() << "), Max("
                         << item._info->Max() << ")." << endmsg;

         anAlarmMsg alarm(CONFIGDAT_WRITE_ERROR);
         alarm.setAlarm();


      }
   }

   return true;
}



//
// Read
//
template <class dataType> bool ConfigElement<dataType>::Read(CDatFileReader &datfile)
{
   ConfigVarList item;
   float value;

   for (CONFIG_VAR_LIST::iterator i = _list.begin(); i != _list.end(); ++i)
   {
      item = *i;
      
      value = datfile.GetFloat(item._info->Section(), item._info->Name());
      
      if (!item._info->Valid(value))
      {
         DataLog(log_level_control_alarm_detail)
                         << "Value out of Range: Section(" << item._info->Section()
                         << "), Name("                     << item._info->Name() 
                         << "), Value("                    << value
                         << "), Min("                      << item._info->Min()
                         << "), Max("                      << item._info->Max() 
                         << ")." << endmsg;
         
         // Generate S/W A2 Alarm
         anAlarmMsg alarm(CONFIGDAT_VALUE_OUT_OF_RANGE);
         alarm.setAlarm();

         switch (item._storage_type)
         {
         case ConfigVarList::INT:
            *item._data_ptr.valuei = (int)item._info->Min();
            break;

         case ConfigVarList::FLOAT:
            *item._data_ptr.valuef = item._info->Min();
            break;

         default:
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
         case ConfigVarList::INT:
            *item._data_ptr.valuei = (int)value;
            break;

         case ConfigVarList::FLOAT:
            *item._data_ptr.valuef = value;
            break;

         default:
            DataLog(log_level_config_error)
                            << "Wrong type found for config item " << item._info->Name()
                            << "(" << item._info->DisplayType() << ")." << endmsg;
            *item._data_ptr.valuef = item._info->Min();
         }
      }
   }

   return true;
}

template <class dataType> void ConfigElement<dataType>::SetDefault(void)
{
   ConfigVarList item;

   for (CONFIG_VAR_LIST::iterator i = _list.begin(); i != _list.end(); ++i)
   {
      item = *i;
      
      switch (item._storage_type)
      {
      case ConfigVarList::INT:
         *item._data_ptr.valuei = (int)item._info->Min();
         break;

      case ConfigVarList::FLOAT:
         *item._data_ptr.valuef = item._info->Min();
         break;

      default:
			break;
      }
   }
}

template <class dataType> ConfigElement<dataType>::ConfigElement()
{
}

template <class dataType> ConfigElement<dataType>::~ConfigElement()
{
}

//
// Config_CDS Class
//


CDatFileReader Config_CDS::_datfile;
DataLog_SetHandle Config_CDS::_configLogSet = DATALOG_NULL_SET_HANDLE;

Config_CDS::Config_CDS(Role role) :
   MultWriteDataStore("Config_CDS", role)
{
   LangUnit.Register   (this, PFR_RECOVER);
   Machine.Register    (this, PFR_RECOVER);
   Procedure.Register  (this, PFR_RECOVER);
   Predict.Register    (this, PFR_RECOVER);
   Templates.Register  (this, PFR_RECOVER);
   Definition.Register (this, PFR_RECOVER);

   ProcedureList.numberOfProcedures.Register(this, PFR_RECOVER, 0, NUM_VISTA_PROCEDURES);
   ProcedureList.Procedure.Register(this, PFR_RECOVER);

	if ( _configLogSet == DATALOG_NULL_SET_HANDLE )
	{
		datalog_CreatePeriodicSet("ConfigData", &_configLogSet);

		datalog_AddRef(_configLogSet, LangUnit.GetRef().key_lang,					"ConfigLang", 				"language", "%d");
		datalog_AddRef(_configLogSet, LangUnit.GetRef().key_height,					"ConfigHeight",				"height units", "%d");
		datalog_AddRef(_configLogSet, LangUnit.GetRef().key_weight,					"ConfigWeight",				"weight units", "%d");
		datalog_AddRef(_configLogSet, LangUnit.GetRef().key_decimal_delimiter,		"ConfigDecimal",			"decimal delimiter", "%d");

		datalog_AddRef(_configLogSet, LangUnit.GetRef().key_date_format,			"ConfigDateFormat",			"date format", "%d");
		datalog_AddRef(_configLogSet, LangUnit.GetRef().key_time_format,			"ConfigTimeFormat",			"time format", "%d");
		datalog_AddRef(_configLogSet, Machine.GetRef().key_set_audio,				"ConfigAudioLevel",			"audio level", "%d");

		datalog_AddRef(_configLogSet, Procedure.GetRef().key_proc_time,				"ConfigProcTime",			"maximum procedure time", "%d");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_return_press,			"ConfigReturnPressure",		"return pressure", "%.0f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_draw_press,	  		"ConfigDrawPressure",		"draw pressure", "%.0f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_custom_ratio,  		"ConfigCustomRatio",		"ratio", "%.1f");
		datalog_AddRef(_configLogSet, Predict.GetRef().key_rbc_ratio,  				"ConfigRBCRatio",			"RBC ratio", "%d");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_plasma_rinseback,  	"ConfigRinsebackProtocol",	"rinseback protocol", "%d");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_audit_tracking,  		"ConfigAuditTracking",		"audit tracking", "%d");

		char	keyName[32];
		char	keyDesc[32];
		int	i;

		for ( i=0; i<NUM_YIELD_VOLUME; i++ )
		{
			sprintf(keyName, "ConfigPltYield%d", i+1);
			sprintf(keyDesc, "platelet yield %d", i+1);
			datalog_AddRef(_configLogSet, Templates.GetRef().key_plt_yield[i],  keyName,	 keyDesc, "%.1f");

			sprintf(keyName, "ConfigPltVolume%d", i+1);
			sprintf(keyDesc, "platelet volume %d", i+1);
			datalog_AddRef(_configLogSet, Templates.GetRef().key_plt_volume[i],  keyName,	 keyDesc, "%.0f");
		}

		datalog_AddRef(_configLogSet, Predict.GetRef().key_ysf,  						"ConfigYSF",					"platelet yield scaling factor", "%.2f");

		for ( i=0; i<NUM_PLASMA_VOLUME; i++ )
		{
			sprintf(keyName, "ConfigPlasmaVolume%d", i+1);
			sprintf(keyDesc, "plasma volume %d", i+1);
			datalog_AddRef(_configLogSet, Templates.GetRef().key_pls_volume[i],  keyName,	 keyDesc, "%.0f");
		}

		datalog_AddRef(_configLogSet, Predict.GetRef().key_pls_amap_min,				"ConfigAMAPMin",				"plasma AMAP minimum volume", "%d");
		datalog_AddRef(_configLogSet, Predict.GetRef().key_pls_amap_max,				"ConfigAMAPMax",				"plasma AMAP maximum volume", "%d");

		for ( i=0; i<NUM_CRIT_DOSE; i++ )
		{
			sprintf(keyName, "ConfigRBCCrit%d", i+1);
			sprintf(keyDesc, "RBC crit %d", i+1);
			datalog_AddRef(_configLogSet, Templates.GetRef().key_rbc_crit[i],  keyName,	 keyDesc, "%.2f");

			sprintf(keyName, "ConfigRBCDose%d", i+1);
			sprintf(keyDesc, "RBC dose %d", i+1);
			datalog_AddRef(_configLogSet, Templates.GetRef().key_rbc_dose[i],  keyName,	 keyDesc, "%.0f");
		}

		datalog_AddRef(_configLogSet, Predict.GetRef().key_rsf,  						"ConfigRSF",					"RBC yield scaling factor", "%.2f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_drbc_alert, 				"ConfigDRBCAlert",			"DRBC alert", "%d");

		datalog_AddRef(_configLogSet, Procedure.GetRef().key_ac_rate, 					"ConfigACRate",				"AC infusion rate", "%d");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_post_crit,				"ConfigPostCrit",				"post crit", "%.2f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_post_plat,				"ConfigPostCount",			"post platelet count", "%.0f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_max_draw_flow, 			"ConfigMaxDrawFlow",			"maximum draw flow", "%d");
		datalog_AddRef(_configLogSet, Predict.GetRef().key_inlet_flow_ramp, 			"ConfigInletFlowRamp",		"inlet flow ramp", "%d");
		datalog_AddRef(_configLogSet, Predict.GetRef().key_inlet_management,			"ConfigInletManagement",	"inlet management", "%d");
		datalog_AddRef(_configLogSet, Predict.GetRef().key_return_management,		    "ConfigReturnManagement",	"return management", "%d");

		datalog_AddRef(_configLogSet, Procedure.GetRef().key_tbv_vol_setting,				"ConfigTBVSetting",					"TBV setting", "%d");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_tbv_percent,					"ConfigTBVPercent",					"TBV percent", "%.1f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_weight_1_setting,				"ConfigWeight1Setting",				"Weight 1 setting", "%.1f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_weight_2_setting,				"ConfigWeight2Setting",				"Weight 2 setting", "%.1f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_weight_3_setting,				"ConfigWeight3Setting",				"Weight 3 setting", "%.1f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_weight_1_less_than_vol,		"ConfigWeight1LessThanVol",		"Weight 1 less than volume", "%.1f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_weight_2_less_than_vol,		"ConfigWeight2LessThanVol",		"Weight 2 less than volume", "%.1f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_weight_3_less_than_vol,		"ConfigWeight3LessThanVol",		"Weight 3 less than volume", "%.1f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_weight_3_greater_than_vol,	"ConfigWeight3GreaterThanVol",	"Weight 3 less than volume", "%.1f");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_ml_per_kg,					"ConfigMlPerKg",						"ml/kg", "%.1f");

		datalog_AddRef(_configLogSet, Predict.GetRef().key_rbc_replace_fluid,	 		"ConfigRBCReplaceFluid",	"RBC replacement fluid", "%d");
		datalog_AddRef(_configLogSet, Procedure.GetRef().key_min_replacement_volume,	"ConfigMinReplaceVolume",	"minimum replacement volume", "%d");
		datalog_AddRef(_configLogSet, Predict.GetRef().key_rbc_fluid_percent,			"ConfigRBCFluidPercent",	"RBC fluid percent", "%.0f");

		for ( i=0; i<NUM_OFFLINE_PROCEDURES; i++ )
		{
			sprintf(keyName, "ConfigProcPlt%d", i+1);
			sprintf(keyDesc, "procedure %d platelet product", i+1);
			datalog_AddRef(_configLogSet, Definition.GetRef()[i].key_platelet, keyName, keyDesc, "%d");

			sprintf(keyName, "ConfigProcPls%d", i+1);
			sprintf(keyDesc, "procedure %d plasma product", i+1);
			datalog_AddRef(_configLogSet, Definition.GetRef()[i].key_plasma, keyName, keyDesc, "%d");

			sprintf(keyName, "ConfigProcRBC%d", i+1);
			sprintf(keyDesc, "procedure %d RBC product", i+1);
			datalog_AddRef(_configLogSet, Definition.GetRef()[i].key_rbc, keyName, keyDesc, "%d");

			sprintf(keyName, "ConfigProcBloodType%d", i+1);
			sprintf(keyDesc, "procedure %d blood type", i+1);
			datalog_AddRef(_configLogSet, Definition.GetRef()[i].key_blood_type, keyName, keyDesc, "%d");

			sprintf(keyName, "ConfigProcTime%d", i+1);
			sprintf(keyDesc, "procedure %d time", i+1);
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

//four_way_volume_cutoff is no longer a feature
#if 0
   bool four_way_volume_cutoff_config_allowed = SWOptionEnabled("four_way_volume_cutoff", LangUnit.GetRef().key_lang);
#endif  
   bool four_way_volume_cutoff_config_allowed = false; 

   if (!four_way_volume_cutoff_config_allowed)
   {
      *Procedure._weight_1_setting._data_ptr.valuef = *Procedure._weight_3_setting._data_ptr.valuef;
      *Procedure._weight_2_setting._data_ptr.valuef = *Procedure._weight_3_setting._data_ptr.valuef;
      
      *Procedure._weight_1_less_than_vol._data_ptr.valuef = *Procedure._weight_3_less_than_vol._data_ptr.valuef;
      *Procedure._weight_2_less_than_vol._data_ptr.valuef = *Procedure._weight_3_less_than_vol._data_ptr.valuef;
      
   }

   
   bool successful_write = true;

   // Write out the Config CDS to file.
   successful_write &= LangUnit.Write(_datfile);
   successful_write &= Machine.Write(_datfile);
   successful_write &= Procedure.Write(_datfile);
   successful_write &= Predict.Write(_datfile);
   successful_write &= Templates.Write(_datfile);
   successful_write &= Definition.Write(_datfile);

   if (!successful_write)
   {
      anAlarmMsg alarm (CONFIGDAT_WRITE_ERROR);
      alarm.setAlarm();

      DataLog(log_level_control_alarm_detail) << "Unable to write config.dat file." << endmsg;

      return false;
   }

   if (!_datfile.WriteCfgFile(FILE_CONFIG_DAT))
   {
      anAlarmMsg alarm (CONFIGDAT_WRITE_ERROR);
      alarm.setAlarm();

      DataLog(log_level_control_alarm_detail) << "Unable to write config.dat file." << endmsg;

      return false;
   }

   if (opt == COPY_OFFLINE_PROCEDURE_LIST)
   {
      // Copy the offline procedure std::list for predict
      CopyOfflineProcedureList();
   }
   
   // Send out an update message.
   SendUpdateMsg();

   return true;
}

Config_CDS::~Config_CDS()
{
}

//
// Read
//
bool Config_CDS::Read()
{
   return ReadConfig(PNAME_CONFIGDAT);
}

bool Config_CDS::ReadConfig(const char *pathname)
{
   // Only a RW Role can initialize the Config CDS.
   if (GetRole() != ROLE_RW)
   {
      DataLog(log_level_config_error) << "Config CDS read refused.  Incorrect role." << endmsg;
      return false;
   }

   bool successful_read = true;
   DFR_ERROR err;
   ALARM_VALUES alarmId = NULL_ALARM;
   
   err = _datfile.Initialize(pathname, true);

   if ( err == DFR_OK )
   {
      // Read in the config dat file into the CDS.
      successful_read &= LangUnit.Read(_datfile);
      successful_read &= Machine.Read(_datfile);
      successful_read &= Procedure.Read(_datfile);
      successful_read &= Predict.Read(_datfile);
      successful_read &= Templates.Read(_datfile);
      successful_read &= Definition.Read(_datfile);

      // get the error, if any ...
      err = _datfile.Error();
   }

   switch (err)
   {
   case DFR_FOPEN:
      alarmId = CONFIGDAT_BAD_FILE;
      
      DataLog(log_level_control_alarm_detail)
                << "Failed to open \"" << pathname << "\": " << errnoMsg << endmsg;
      break;

   case DFR_PARSE:
      alarmId = CONFIGDAT_PARSE_ERROR;

      DataLog(log_level_control_alarm_detail)
                << "Error parsing \"" << pathname << "\": " << errnoMsg << endmsg;
      break;

   case DFR_VALUE:
      alarmId = CONFIGDAT_VALUE_OUT_OF_RANGE;

      DataLog(log_level_control_alarm_detail)
                << "Value out of range in \"" << pathname << "\": " << errnoMsg << endmsg;
      break;

   case DFR_NOTFOUND:
      alarmId = CONFIGDAT_VALUE_NOT_FOUND;

      DataLog(log_level_control_alarm_detail)
                << "Value not found in \"" << pathname << "\": " << errnoMsg << endmsg;
      break;

   case DFR_OK:
      break;
   }

   if (alarmId != NULL_ALARM)
   {
      // Generate a Software A2 Alarm
      anAlarmMsg alarm(alarmId);
      alarm.setAlarm();

		LangUnit.SetDefault();
		Machine.SetDefault();
		Procedure.SetDefault();
		Predict.SetDefault();
		Templates.SetDefault();
		Definition.SetDefault();

      successful_read = false;
   }
   
   bool four_way_volume_cutoff_config_allowed = SWOptionEnabled("four_way_volume_cutoff", LangUnit.GetRef().key_lang);
   
   if (!four_way_volume_cutoff_config_allowed)
   {
      *Procedure._weight_1_setting._data_ptr.valuef = *Procedure._weight_3_setting._data_ptr.valuef;
      *Procedure._weight_2_setting._data_ptr.valuef = *Procedure._weight_3_setting._data_ptr.valuef;
      
      *Procedure._weight_1_less_than_vol._data_ptr.valuef = *Procedure._weight_3_less_than_vol._data_ptr.valuef;
      *Procedure._weight_2_less_than_vol._data_ptr.valuef = *Procedure._weight_3_less_than_vol._data_ptr.valuef;
   }

   // If successful read, initialize the offline procedure std::list for predict
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
bool Config_CDS::Valid()
{
   bool configuration_valid = true;

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
// SendUpdateMsg
//
void Config_CDS::SendUpdateMsg()
{
   ConfigChangedMsg newConfig(MessageBase::SEND_GLOBAL); 
   newConfig.send(1);

	datalog_ForcePeriodicOutput(_configLogSet);
}

//
// Check if the specified software option is enabled, given the setting
// from sw.dat and the current language selection.
//
bool Config_CDS::SWOptionEnabled(const char * optionName)
{
	return SWOptionEnabled(optionName, LangUnit.GetRef().key_lang);
}

bool Config_CDS::SWOptionEnabled(const char * optionName, int currentLanguage)
{
	bool result = false;
	unsigned long setting = trimaSysGetSoftwareSetting(optionName);

	if ( setting & 0x01 )
	{
		result = true;
	}
	else if ( setting != 0 )
	{
		unsigned long	mask = 1 << (currentLanguage + 1);
		result = ((setting & mask) != 0);
	}

	return result;
}

//
// Copy over the offline procedure std::list to the predict std::list
//
void Config_CDS::CopyOfflineProcedureList()
{
   const ProductDefinitionArray &offlineList = Definition.GetRef();
   const ProductTemplates       &templates   = Templates.GetRef();
   
   ProductListArray predictList;


   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      const int &crit_dose_index    = offlineList[i].key_rbc;
      const int &yield_volume_index = offlineList[i].key_platelet;
      
      if (yield_volume_index == 0)
      {
         predictList[i].yield           = 0.0f;
         predictList[i].platelet_volume = 0.0f;
      }
      else
      {
         predictList[i].yield           = templates.key_plt_yield[yield_volume_index - 1];
         predictList[i].platelet_volume = templates.key_plt_volume[yield_volume_index - 1];
      }

      if (crit_dose_index == 0)
      {
         predictList[i].rbc_dose = 0.0f;
         predictList[i].rbc_crit = 0.0f;
      }
      else
      {
         predictList[i].rbc_dose = templates.key_rbc_dose[crit_dose_index - 1];
         predictList[i].rbc_crit = templates.key_rbc_crit[crit_dose_index - 1];
      }

      predictList[i].max_procedure_time = offlineList[i].key_time;
      predictList[i].designator         = 1;   // All offline procedure default to primary

      // Convert the blood type.
      switch (offlineList[i].key_blood_type)
      {
      case CONFIG_INDEX_BLOOD_TYPE_ALL:
         predictList[i].blood_type = BT_ALL;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_OP_ON:
         predictList[i].blood_type = BT_OP_ON;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_ON:
         predictList[i].blood_type = BT_ON;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN:
         predictList[i].blood_type = BT_OP_ON_AP_AN;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_OP_ON_AN:
         predictList[i].blood_type = BT_OP_ON_AN;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_ON_AN:
         predictList[i].blood_type = BT_ON_AN;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_OP_ON_BP_BN:
         predictList[i].blood_type = BT_OP_ON_BP_BN;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN_BP_BN:
         predictList[i].blood_type = BT_OP_ON_AP_AN_BP_BN;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN_BN:
         predictList[i].blood_type = BT_OP_ON_AP_AN_BN;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_AP_AN:
         predictList[i].blood_type = BT_AP_AN;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_AP_AN_BP_BN_ABP_ABN:
         predictList[i].blood_type = BT_AP_AN_BP_BN_ABP_ABN;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_BP_BN:
         predictList[i].blood_type = BT_BP_BN;
         break;

      case CONFIG_INDEX_BLOOD_TYPE_ABP_ABN:
         predictList[i].blood_type = BT_ABP_ABN;
         break;

      default:
         // Generate a Software A2 Alarm
         anAlarmMsg alarm(CONFIGDAT_VALUE_OUT_OF_RANGE);
         alarm.setAlarm();

         DataLog(log_level_control_alarm_detail)
                          << "Unknown blood type (" 
                          << offlineList[i].key_blood_type
                          << "found during Predict conversion." << endmsg;
         break;
      }
      
      // Convert the plasma value
      switch (offlineList[i].key_plasma)
      {
      case PLASMA_NONE:
         predictList[i].plasma_volume = 0.0f;
         break;

      case PLASMA_1:
         predictList[i].plasma_volume = templates.key_pls_volume[0];
         break;

      case PLASMA_2:
         predictList[i].plasma_volume = templates.key_pls_volume[1];
         break;

      case PLASMA_3:
         predictList[i].plasma_volume = templates.key_pls_volume[2];
         break;

      case PLASMA_4:
         predictList[i].plasma_volume = templates.key_pls_volume[3];
         break;

      case PLASMA_5:
         predictList[i].plasma_volume = templates.key_pls_volume[4];
         break;

      case PLASMA_6:
         predictList[i].plasma_volume = templates.key_pls_volume[5];
         break;

      case PLASMA_AMAP:
         predictList[i].plasma_volume = -1.0f;  // AMAP flag for predict
         break;

      case PLASMA_MAX_CONC:
         predictList[i].plasma_volume = -2.0f;  // MAX Concurrent flag for predict
         break;

      default:
         // Generate a Software A2 Alarm
         anAlarmMsg alarm(CONFIGDAT_VALUE_OUT_OF_RANGE);
         alarm.setAlarm();

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
{
}

LangConfigElement::~LangConfigElement()
{
	_list.clear();
}

   
//
// Register
//
bool LangConfigElement::Register (DataStore *ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<LangUnitCfg>::Register(ds, pfr);

   // Next, fill in the std::list.
   _lang._info  = _Lst.pVar(KEY_LANG);
   _lang._data_ptr.valuei = (int *)&((this->GetRef()).key_lang);
   _lang._storage_type = ConfigVarList::INT;
   _list.push_back(_lang);

   _height._info = _Lst.pVar(KEY_HEIGHT);
   _height._data_ptr.valuei = (int *)&((this->GetRef()).key_height);
   _height._storage_type = ConfigVarList::INT;
   _list.push_back(_height);

   _weight._info = _Lst.pVar(KEY_WEIGHT);
   _weight._data_ptr.valuei = (int *)&((this->GetRef()).key_weight);
   _weight._storage_type = ConfigVarList::INT;
   _list.push_back(_weight);

   _date_format._info = _Lst.pVar(KEY_DATE_FORMAT);
   _date_format._data_ptr.valuei = (int *)&((this->GetRef()).key_date_format);
   _date_format._storage_type = ConfigVarList::INT;
   _list.push_back(_date_format);

   _time_format._info = _Lst.pVar(KEY_TIME_FORMAT);
   _time_format._data_ptr.valuei = (int *)&((this->GetRef()).key_time_format);
   _time_format._storage_type = ConfigVarList::INT;
   _list.push_back(_time_format);

   _decimal_delimiter._info = _Lst.pVar(KEY_DECIMAL_DELIMITER);
   _decimal_delimiter._data_ptr.valuei = (int *)&((this->GetRef()).key_decimal_delimiter);
   _decimal_delimiter._storage_type = ConfigVarList::INT;
   _list.push_back(_decimal_delimiter);
    
   return created;
}
   

//
// Assignment operator
//
LangConfigElement LangConfigElement::operator = (const LangUnitCfg &rhs)
{
   this->Set(rhs);

   return *this;
}



void LangConfigElement::Log()
{
}



//
// Basic Machine Config Element Class
//

//
// Base Constructor
//
BasicMachineElement::BasicMachineElement()
{
}

BasicMachineElement::~BasicMachineElement()
{
	_list.clear();
}


//
// Register
//
bool BasicMachineElement::Register (DataStore *ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<BasicMachineCfg>::Register(ds, pfr);

   // Next, fill in the std::list.
   _set_time._info  = _Lst.pVar(KEY_SET_TIME);
   _set_time._data_ptr.valuei = (int *)&((this->GetRef()).key_set_time);
   _set_time._storage_type = ConfigVarList::INT;
   _list.push_back(_set_time);
   
   _set_date._info  = _Lst.pVar(KEY_SET_DATE);
   _set_date._data_ptr.valuei = (int *)&((this->GetRef()).key_set_date);
   _set_date._storage_type = ConfigVarList::INT;
   _list.push_back(_set_date);
   
   _set_audio._info  = _Lst.pVar(KEY_SET_AUDIO);
   _set_audio._data_ptr.valuei = (int *)&((this->GetRef()).key_set_audio);
   _set_audio._storage_type = ConfigVarList::INT;
   _list.push_back(_set_audio);
   
   _pword._info  = _Lst.pVar(KEY_PWORD);
   _pword._data_ptr.valuei = (int *)&((this->GetRef()).key_pword);
   _pword._storage_type = ConfigVarList::INT;
   _list.push_back(_pword);
    
   return created;
}



//
//
//
bool BasicMachineElement::Valid()
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
BasicMachineElement BasicMachineElement::operator = (const BasicMachineCfg &rhs)
{
   this->Set(rhs);

   return *this;
}

void BasicMachineElement::Log()
{
}


//
// Procedure Element Class
//

//
// Base Constructor
//
ProcedureElement::ProcedureElement()
{
}

ProcedureElement::~ProcedureElement()
{
	_list.clear();
}


//
// Register
//
bool ProcedureElement::Register (DataStore *ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<ProcedureConfig>::Register(ds, pfr);

   // Next, fill in the std::list.
   _proc_time._info  = _Lst.pVar(KEY_PROC_TIME);
   _proc_time._data_ptr.valuei = (int *)&((this->GetRef()).key_proc_time);
   _proc_time._storage_type = ConfigVarList::INT;
   _list.push_back(_proc_time);
   
   _return_press._info  = _Lst.pVar(KEY_RETURN_PRESS);
   _return_press._data_ptr.valuef = (float *)&((this->GetRef()).key_return_press);
   _return_press._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_return_press);
   
   _draw_press._info  = _Lst.pVar(KEY_DRAW_PRESS);
   _draw_press._data_ptr.valuef = (float *)&((this->GetRef()).key_draw_press);
   _draw_press._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_draw_press);
   
   _custom_ratio._info  = _Lst.pVar(KEY_CUSTOM_RATIO);
   _custom_ratio._data_ptr.valuef = (float *)&((this->GetRef()).key_custom_ratio);
   _custom_ratio._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_custom_ratio);

   _ac_rate._info  = _Lst.pVar(KEY_AC_RATE);
   _ac_rate._data_ptr.valuei = (int *)&((this->GetRef()).key_ac_rate);
   _ac_rate._storage_type = ConfigVarList::INT;
   _list.push_back(_ac_rate);
   
   _post_crit._info  = _Lst.pVar(KEY_POST_CRIT);
   _post_crit._data_ptr.valuef = (float *)&((this->GetRef()).key_post_crit);
   _post_crit._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_post_crit);
   
   _tbv_vol_setting._info  = _Lst.pVar(KEY_TBV_VOL_SETTING);
   _tbv_vol_setting._data_ptr.valuei = (int *)&((this->GetRef()).key_tbv_vol_setting);
   _tbv_vol_setting._storage_type = ConfigVarList::INT;
   _list.push_back(_tbv_vol_setting);
   
   _tbv_percent._info  = _Lst.pVar(KEY_TBV_PERCENT);
   _tbv_percent._data_ptr.valuef = (float *)&((this->GetRef()).key_tbv_percent);
   _tbv_percent._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_tbv_percent);

   _weight_1_setting._info  = _Lst.pVar(KEY_WEIGHT_1_SETTING);
   _weight_1_setting._data_ptr.valuef = (float *)&((this->GetRef()).key_weight_1_setting);
   _weight_1_setting._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_weight_1_setting);
   
   _weight_1_less_than_vol._info  = _Lst.pVar(KEY_WEIGHT_1_LESS_THAN_VOL);
   _weight_1_less_than_vol._data_ptr.valuef = (float *)&((this->GetRef()).key_weight_1_less_than_vol);
   _weight_1_less_than_vol._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_weight_1_less_than_vol);
   
   _weight_2_setting._info  = _Lst.pVar(KEY_WEIGHT_2_SETTING);
   _weight_2_setting._data_ptr.valuef = (float *)&((this->GetRef()).key_weight_2_setting);
   _weight_2_setting._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_weight_2_setting);
   
   _weight_2_less_than_vol._info  = _Lst.pVar(KEY_WEIGHT_2_LESS_THAN_VOL);
   _weight_2_less_than_vol._data_ptr.valuef = (float *)&((this->GetRef()).key_weight_2_less_than_vol);
   _weight_2_less_than_vol._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_weight_2_less_than_vol);
   
   _weight_3_setting._info  = _Lst.pVar(KEY_WEIGHT_3_SETTING);
   _weight_3_setting._data_ptr.valuef = (float *)&((this->GetRef()).key_weight_3_setting);
   _weight_3_setting._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_weight_3_setting);
   
   _weight_3_less_than_vol._info  = _Lst.pVar(KEY_WEIGHT_3_LESS_THAN_VOL);
   _weight_3_less_than_vol._data_ptr.valuef = (float *)&((this->GetRef()).key_weight_3_less_than_vol);
   _weight_3_less_than_vol._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_weight_3_less_than_vol);
   
   _weight_3_greater_than_vol._info  = _Lst.pVar(KEY_WEIGHT_3_GREATER_THAN_VOL);
   _weight_3_greater_than_vol._data_ptr.valuef = (float *)&((this->GetRef()).key_weight_3_greater_than_vol);
   _weight_3_greater_than_vol._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_weight_3_greater_than_vol);
   
   _ml_per_kg._info  = _Lst.pVar(KEY_ML_PER_KG);
   _ml_per_kg._data_ptr.valuef = (float *)&((this->GetRef()).key_ml_per_kg);
   _ml_per_kg._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_ml_per_kg);

   _post_plat._info  = _Lst.pVar(KEY_POST_PLAT);
   _post_plat._data_ptr.valuef = (float *)&((this->GetRef()).key_post_plat);
   _post_plat._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_post_plat);
   
   _max_draw_flow._info  = _Lst.pVar(KEY_MAX_DRAW_FLOW);
   _max_draw_flow._data_ptr.valuei = (int *)&((this->GetRef()).key_max_draw_flow);
   _max_draw_flow._storage_type = ConfigVarList::INT;
   _list.push_back(_max_draw_flow);
   
   _min_replacement_volume._info  = _Lst.pVar(KEY_MIN_REPLACEMENT_VOLUME);
   _min_replacement_volume._data_ptr.valuei = (int *)&((this->GetRef()).key_min_replacement_volume);
   _min_replacement_volume._storage_type = ConfigVarList::INT;
   _list.push_back(_min_replacement_volume);
   
   _drbc_alert._info  = _Lst.pVar(KEY_DRBC_ALERT);
   _drbc_alert._data_ptr.valuei = (int *)&((this->GetRef()).key_drbc_alert);
   _drbc_alert._storage_type = ConfigVarList::INT;
   _list.push_back(_drbc_alert);

   _plasma_rinseback._info  = _Lst.pVar(KEY_PLASMA_RINSEBACK);
   _plasma_rinseback._data_ptr.valuei = (int *)&((this->GetRef()).key_plasma_rinseback);
   _plasma_rinseback._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_rinseback);

   _audit_tracking._info  = _Lst.pVar(KEY_AUDIT_TRACKING);
   _audit_tracking._data_ptr.valuei = (int *)&((this->GetRef()).key_audit_tracking);
   _audit_tracking._storage_type = ConfigVarList::INT;
   _list.push_back(_audit_tracking);
    
   return created;
}



//
// Assignment operator
//
ProcedureElement ProcedureElement::operator = (const ProcedureConfig &rhs)
{
   this->Set(rhs);

   return *this;
}

void ProcedureElement::Log()
{
}



//
// Prediction Element Class
//

//
// Base Constructor
//
PredictionElement::PredictionElement()
{
}

PredictionElement::~PredictionElement()
{
	_list.clear();
}


//
// Register
//
bool PredictionElement::Register (DataStore *ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<PredictionConfig>::Register(ds, pfr);

   // Next, fill in the std::list.
   _rbc_ratio._info  = _Lst.pVar(KEY_RBC_RATIO);
   _rbc_ratio._data_ptr.valuei = (int *)&((this->GetRef()).key_rbc_ratio);
   _rbc_ratio._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_ratio);
   
   _rbc_replace_fluid._info  = _Lst.pVar(KEY_RBC_REPLACE_FLUID);
   _rbc_replace_fluid._data_ptr.valuei = (int *)&((this->GetRef()).key_rbc_replace_fluid);
   _rbc_replace_fluid._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_replace_fluid);
   
   _rbc_fluid_percent._info  = _Lst.pVar(KEY_RBC_FLUID_PERCENT);
   _rbc_fluid_percent._data_ptr.valuef = (float *)&((this->GetRef()).key_rbc_fluid_percent);
   _rbc_fluid_percent._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_rbc_fluid_percent);

   _plasma_amap_min._info  = _Lst.pVar(KEY_PLASMA_AMAP_MINIMUM);
   _plasma_amap_min._data_ptr.valuei = (int *)&((this->GetRef()).key_pls_amap_min);
   _plasma_amap_min._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_amap_min);

   _plasma_amap_max._info  = _Lst.pVar(KEY_PLASMA_AMAP_MAXIMUM);
   _plasma_amap_max._data_ptr.valuei = (int *)&((this->GetRef()).key_pls_amap_max);
   _plasma_amap_max._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_amap_max);

   _ysf._info  = _Lst.pVar(KEY_YSF);
   _ysf._data_ptr.valuef = (float *)&((this->GetRef()).key_ysf);
   _ysf._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_ysf);

   _guaranteed_yield._info  = _Lst.pVar(KEY_GUARANTEED_YIELD);
   _guaranteed_yield._data_ptr.valuei = (int *)&((this->GetRef()).key_guaranteed_yield);
   _guaranteed_yield._storage_type = ConfigVarList::INT;
   _list.push_back(_guaranteed_yield);
   
   _confidence_level._info  = _Lst.pVar(KEY_GUARANTEED_YIELD_CL);
   _confidence_level._data_ptr.valuei = (int *)&((this->GetRef()).key_confidence_level);
   _confidence_level._storage_type = ConfigVarList::INT;
   _list.push_back(_confidence_level);
   
   _coefficient_of_variation._info  = _Lst.pVar(KEY_GUARANTEED_YIELD_CV);
   _coefficient_of_variation._data_ptr.valuef = (float *)&((this->GetRef()).key_coefficient_of_variation);
   _coefficient_of_variation._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_coefficient_of_variation);

   _inlet_management._info  = _Lst.pVar(KEY_INLET_MANAGEMENT);
   _inlet_management._data_ptr.valuei = (int *)&((this->GetRef()).key_inlet_management);
   _inlet_management._storage_type = ConfigVarList::INT;
   _list.push_back(_inlet_management);
   
   _inlet_flow_ramp._info  = _Lst.pVar(KEY_INLET_FLOW_RAMP);
   _inlet_flow_ramp._data_ptr.valuei = (int *)&((this->GetRef()).key_inlet_flow_ramp);
   _inlet_flow_ramp._storage_type = ConfigVarList::INT;
   _list.push_back(_inlet_flow_ramp);
   
   _return_management._info  = _Lst.pVar(KEY_RETURN_MANAGEMENT);
   _return_management._data_ptr.valuei = (int *)&((this->GetRef()).key_return_management);
   _return_management._storage_type = ConfigVarList::INT;
   _list.push_back(_return_management);
   
   _rsf._info  = _Lst.pVar(KEY_RSF);
   _rsf._data_ptr.valuef = (float *)&((this->GetRef()).key_rsf);
   _rsf._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_rsf);
    
   return created;
}



//
// Assignment operator
//
PredictionElement PredictionElement::operator = (const PredictionConfig &rhs)
{
   this->Set(rhs);

   return *this;
}

void PredictionElement::Log()
{
}



//
// ProductTemplates Element Class
//

//
// Base Constructor
//
ProductTemplatesElement::ProductTemplatesElement()
{
}

ProductTemplatesElement::~ProductTemplatesElement()
{
	_list.clear();
}


//
// Register
//
bool ProductTemplatesElement::Register (DataStore *ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<ProductTemplates>::Register(ds, pfr);

   // Next, fill in the std::list.
   _rbc_crit_1._info  = _Lst.pVar(KEY_RBC_CRIT_1);
   _rbc_crit_1._data_ptr.valuef = (float *)&((this->GetRef()).key_rbc_crit[0]);
   _rbc_crit_1._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_rbc_crit_1);

   _rbc_crit_2._info  = _Lst.pVar(KEY_RBC_CRIT_2);
   _rbc_crit_2._data_ptr.valuef = (float *)&((this->GetRef()).key_rbc_crit[1]);
   _rbc_crit_2._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_rbc_crit_2);

   _rbc_crit_3._info  = _Lst.pVar(KEY_RBC_CRIT_3);
   _rbc_crit_3._data_ptr.valuef = (float *)&((this->GetRef()).key_rbc_crit[2]);
   _rbc_crit_3._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_rbc_crit_3);

   _rbc_dose_1._info  = _Lst.pVar(KEY_RBC_DOSE_1);
   _rbc_dose_1._data_ptr.valuef = (float *)&((this->GetRef()).key_rbc_dose[0]);
   _rbc_dose_1._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_rbc_dose_1);

   _rbc_dose_2._info  = _Lst.pVar(KEY_RBC_DOSE_2);
   _rbc_dose_2._data_ptr.valuef = (float *)&((this->GetRef()).key_rbc_dose[1]);
   _rbc_dose_2._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_rbc_dose_2);

   _rbc_dose_3._info  = _Lst.pVar(KEY_RBC_DOSE_3);
   _rbc_dose_3._data_ptr.valuef = (float *)&((this->GetRef()).key_rbc_dose[2]);
   _rbc_dose_3._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_rbc_dose_3);

   _plt_yield_1._info  = _Lst.pVar(KEY_PLT_YIELD_1);
   _plt_yield_1._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_yield[0]);
   _plt_yield_1._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_1);
   
   _plt_yield_2._info  = _Lst.pVar(KEY_PLT_YIELD_2);
   _plt_yield_2._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_yield[1]);
   _plt_yield_2._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_2);
   
   _plt_yield_3._info  = _Lst.pVar(KEY_PLT_YIELD_3);
   _plt_yield_3._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_yield[2]);
   _plt_yield_3._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_3);
   
   _plt_yield_4._info  = _Lst.pVar(KEY_PLT_YIELD_4);
   _plt_yield_4._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_yield[3]);
   _plt_yield_4._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_4);
   
   _plt_yield_5._info  = _Lst.pVar(KEY_PLT_YIELD_5);
   _plt_yield_5._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_yield[4]);
   _plt_yield_5._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_5);
   
   _plt_yield_6._info  = _Lst.pVar(KEY_PLT_YIELD_6);
   _plt_yield_6._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_yield[5]);
   _plt_yield_6._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_yield_6);
   
   _plt_volume_1._info  = _Lst.pVar(KEY_PLT_VOLUME_1);
   _plt_volume_1._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_volume[0]);
   _plt_volume_1._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_1);

   _plt_volume_2._info  = _Lst.pVar(KEY_PLT_VOLUME_2);
   _plt_volume_2._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_volume[1]);
   _plt_volume_2._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_2);

   _plt_volume_3._info  = _Lst.pVar(KEY_PLT_VOLUME_3);
   _plt_volume_3._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_volume[2]);
   _plt_volume_3._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_3);

   _plt_volume_4._info  = _Lst.pVar(KEY_PLT_VOLUME_4);
   _plt_volume_4._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_volume[3]);
   _plt_volume_4._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_4);

   _plt_volume_5._info  = _Lst.pVar(KEY_PLT_VOLUME_5);
   _plt_volume_5._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_volume[4]);
   _plt_volume_5._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_5);

   _plt_volume_6._info  = _Lst.pVar(KEY_PLT_VOLUME_6);
   _plt_volume_6._data_ptr.valuef = (float *)&((this->GetRef()).key_plt_volume[5]);
   _plt_volume_6._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_plt_volume_6);

   _pls_volume_1._info  = _Lst.pVar(KEY_PLS_VOLUME_1);
   _pls_volume_1._data_ptr.valuef = (float *)&((this->GetRef()).key_pls_volume[0]);
   _pls_volume_1._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_1);

   _pls_volume_2._info  = _Lst.pVar(KEY_PLS_VOLUME_2);
   _pls_volume_2._data_ptr.valuef = (float *)&((this->GetRef()).key_pls_volume[1]);
   _pls_volume_2._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_2);

   _pls_volume_3._info  = _Lst.pVar(KEY_PLS_VOLUME_3);
   _pls_volume_3._data_ptr.valuef = (float *)&((this->GetRef()).key_pls_volume[2]);
   _pls_volume_3._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_3);

   _pls_volume_4._info  = _Lst.pVar(KEY_PLS_VOLUME_4);
   _pls_volume_4._data_ptr.valuef = (float *)&((this->GetRef()).key_pls_volume[3]);
   _pls_volume_4._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_4);

   _pls_volume_5._info  = _Lst.pVar(KEY_PLS_VOLUME_5);
   _pls_volume_5._data_ptr.valuef = (float *)&((this->GetRef()).key_pls_volume[4]);
   _pls_volume_5._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_5);

   _pls_volume_6._info  = _Lst.pVar(KEY_PLS_VOLUME_6);
   _pls_volume_6._data_ptr.valuef = (float *)&((this->GetRef()).key_pls_volume[5]);
   _pls_volume_6._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_pls_volume_6);
    
   return created;
}



//
// Assignment operator
//
ProductTemplatesElement ProductTemplatesElement::operator = (const ProductTemplates &rhs)
{
   this->Set(rhs);

   return *this;
}

void ProductTemplatesElement::Log()
{
}



//
// Product Definition Element Class
//

//
// Base Constructor
//
ProductDefinitionElement::ProductDefinitionElement()
{
}

ProductDefinitionElement::~ProductDefinitionElement()
{
	_list.clear();
}


//
// Register
//
bool ProductDefinitionElement::Register (DataStore *ds, PfrType pfr)
{
   // First register the element to bind it to a physical location.
   bool created = ConfigElement<ProductDefinitionArray>::Register(ds, pfr);

   // Next, fill in the std::list.
   const ProductDefinition *product = ((this)->GetRef()).as_array();


   // Platelet
   _platelet_a._info  = _Lst.pVar(KEY_PRI_PLATELET_A);
   _platelet_a._data_ptr.valuei = (int *)&product[0].key_platelet;
   _platelet_a._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_a);

   _platelet_b._info  = _Lst.pVar(KEY_PRI_PLATELET_B);
   _platelet_b._data_ptr.valuei = (int *)&product[1].key_platelet;
   _platelet_b._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_b);
   
   _platelet_c._info  = _Lst.pVar(KEY_PRI_PLATELET_C);
   _platelet_c._data_ptr.valuei = (int *)&product[2].key_platelet;
   _platelet_c._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_c);
   
   _platelet_d._info  = _Lst.pVar(KEY_PRI_PLATELET_D);
   _platelet_d._data_ptr.valuei = (int *)&product[3].key_platelet;
   _platelet_d._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_d);
   
   _platelet_e._info  = _Lst.pVar(KEY_PRI_PLATELET_E);
   _platelet_e._data_ptr.valuei = (int *)&product[4].key_platelet;
   _platelet_e._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_e);
   
   _platelet_f._info  = _Lst.pVar(KEY_PRI_PLATELET_F);
   _platelet_f._data_ptr.valuei = (int *)&product[5].key_platelet;
   _platelet_f._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_f);
   
   _platelet_g._info  = _Lst.pVar(KEY_PRI_PLATELET_G);
   _platelet_g._data_ptr.valuei = (int *)&product[6].key_platelet;
   _platelet_g._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_g);
   
   _platelet_h._info  = _Lst.pVar(KEY_PRI_PLATELET_H);
   _platelet_h._data_ptr.valuei = (int *)&product[7].key_platelet;
   _platelet_h._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_h);
   
   _platelet_i._info  = _Lst.pVar(KEY_PRI_PLATELET_I);
   _platelet_i._data_ptr.valuei = (int *)&product[8].key_platelet;
   _platelet_i._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_i);
   
   _platelet_j._info  = _Lst.pVar(KEY_PRI_PLATELET_J);
   _platelet_j._data_ptr.valuei = (int *)&product[9].key_platelet;
   _platelet_j._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_j);
   
   _platelet_k._info  = _Lst.pVar(KEY_PRI_PLATELET_K);
   _platelet_k._data_ptr.valuei = (int *)&product[10].key_platelet;
   _platelet_k._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_k);
   
   _platelet_l._info  = _Lst.pVar(KEY_PRI_PLATELET_L);
   _platelet_l._data_ptr.valuei = (int *)&product[11].key_platelet;
   _platelet_l._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_l);
   
   _platelet_m._info  = _Lst.pVar(KEY_PRI_PLATELET_M);
   _platelet_m._data_ptr.valuei = (int *)&product[12].key_platelet;
   _platelet_m._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_m);
   
   _platelet_n._info  = _Lst.pVar(KEY_PRI_PLATELET_N);
   _platelet_n._data_ptr.valuei = (int *)&product[13].key_platelet;
   _platelet_n._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_n);
   
   _platelet_o._info  = _Lst.pVar(KEY_PRI_PLATELET_O);
   _platelet_o._data_ptr.valuei = (int *)&product[14].key_platelet;
   _platelet_o._storage_type = ConfigVarList::INT;
   _list.push_back(_platelet_o);


   // Plasma
   _plasma_a._info  = _Lst.pVar(KEY_PRI_PLASMA_A);
   _plasma_a._data_ptr.valuei = (int *)&product[0].key_plasma;
   _plasma_a._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_a);
   
   _plasma_b._info  = _Lst.pVar(KEY_PRI_PLASMA_B);
   _plasma_b._data_ptr.valuei = (int *)&product[1].key_plasma;
   _plasma_b._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_b);
   
   _plasma_c._info  = _Lst.pVar(KEY_PRI_PLASMA_C);
   _plasma_c._data_ptr.valuei = (int *)&product[2].key_plasma;
   _plasma_c._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_c);
   
   _plasma_d._info  = _Lst.pVar(KEY_PRI_PLASMA_D);
   _plasma_d._data_ptr.valuei = (int *)&product[3].key_plasma;
   _plasma_d._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_d);
   
   _plasma_e._info  = _Lst.pVar(KEY_PRI_PLASMA_E);
   _plasma_e._data_ptr.valuei = (int *)&product[4].key_plasma;
   _plasma_e._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_e);
   
   _plasma_f._info  = _Lst.pVar(KEY_PRI_PLASMA_F);
   _plasma_f._data_ptr.valuei = (int *)&product[5].key_plasma;
   _plasma_f._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_f);
   
   _plasma_g._info  = _Lst.pVar(KEY_PRI_PLASMA_G);
   _plasma_g._data_ptr.valuei = (int *)&product[6].key_plasma;
   _plasma_g._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_g);
   
   _plasma_h._info  = _Lst.pVar(KEY_PRI_PLASMA_H);
   _plasma_h._data_ptr.valuei = (int *)&product[7].key_plasma;
   _plasma_h._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_h);
   
   _plasma_i._info  = _Lst.pVar(KEY_PRI_PLASMA_I);
   _plasma_i._data_ptr.valuei = (int *)&product[8].key_plasma;
   _plasma_i._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_i);
   
   _plasma_j._info  = _Lst.pVar(KEY_PRI_PLASMA_J);
   _plasma_j._data_ptr.valuei = (int *)&product[9].key_plasma;
   _plasma_j._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_j);
   
   _plasma_k._info  = _Lst.pVar(KEY_PRI_PLASMA_K);
   _plasma_k._data_ptr.valuei = (int *)&product[10].key_plasma;
   _plasma_k._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_k);
   
   _plasma_l._info  = _Lst.pVar(KEY_PRI_PLASMA_L);
   _plasma_l._data_ptr.valuei = (int *)&product[11].key_plasma;
   _plasma_l._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_l);
   
   _plasma_m._info  = _Lst.pVar(KEY_PRI_PLASMA_M);
   _plasma_m._data_ptr.valuei = (int *)&product[12].key_plasma;
   _plasma_m._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_m);
   
   _plasma_n._info  = _Lst.pVar(KEY_PRI_PLASMA_N);
   _plasma_n._data_ptr.valuei = (int *)&product[13].key_plasma;
   _plasma_n._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_n);
   
   _plasma_o._info  = _Lst.pVar(KEY_PRI_PLASMA_O);
   _plasma_o._data_ptr.valuei = (int *)&product[14].key_plasma;
   _plasma_o._storage_type = ConfigVarList::INT;
   _list.push_back(_plasma_o);


   // RBC
   _rbc_a._info = _Lst.pVar(KEY_PRI_RBC_A);
   _rbc_a._data_ptr.valuei = (int *)&(product[0].key_rbc);
   _rbc_a._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_a);
   
   _rbc_b._info = _Lst.pVar(KEY_PRI_RBC_B);
   _rbc_b._data_ptr.valuei = (int *)&(product[1].key_rbc);
   _rbc_b._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_b);
   
   _rbc_c._info = _Lst.pVar(KEY_PRI_RBC_C);
   _rbc_c._data_ptr.valuei = (int *)&(product[2].key_rbc);
   _rbc_c._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_c);
   
   _rbc_d._info = _Lst.pVar(KEY_PRI_RBC_D);
   _rbc_d._data_ptr.valuei = (int *)&(product[3].key_rbc);
   _rbc_d._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_d);
   
   _rbc_e._info = _Lst.pVar(KEY_PRI_RBC_E);
   _rbc_e._data_ptr.valuei = (int *)&(product[4].key_rbc);
   _rbc_e._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_e);
   
   _rbc_f._info = _Lst.pVar(KEY_PRI_RBC_F);
   _rbc_f._data_ptr.valuei = (int *)&(product[5].key_rbc);
   _rbc_f._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_f);
   
   _rbc_g._info = _Lst.pVar(KEY_PRI_RBC_G);
   _rbc_g._data_ptr.valuei = (int *)&(product[6].key_rbc);
   _rbc_g._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_g);
   
   _rbc_h._info = _Lst.pVar(KEY_PRI_RBC_H);
   _rbc_h._data_ptr.valuei = (int *)&(product[7].key_rbc);
   _rbc_h._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_h);
   
   _rbc_i._info = _Lst.pVar(KEY_PRI_RBC_I);
   _rbc_i._data_ptr.valuei = (int *)&(product[8].key_rbc);
   _rbc_i._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_i);
   
   _rbc_j._info = _Lst.pVar(KEY_PRI_RBC_J);
   _rbc_j._data_ptr.valuei = (int *)&(product[9].key_rbc);
   _rbc_j._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_j);
   
   _rbc_k._info = _Lst.pVar(KEY_PRI_RBC_K);
   _rbc_k._data_ptr.valuei = (int *)&(product[10].key_rbc);
   _rbc_k._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_k);
   
   _rbc_l._info = _Lst.pVar(KEY_PRI_RBC_L);
   _rbc_l._data_ptr.valuei = (int *)&(product[11].key_rbc);
   _rbc_l._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_l);
   
   _rbc_m._info = _Lst.pVar(KEY_PRI_RBC_M);
   _rbc_m._data_ptr.valuei = (int *)&(product[12].key_rbc);
   _rbc_m._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_m);
   
   _rbc_n._info = _Lst.pVar(KEY_PRI_RBC_N);
   _rbc_n._data_ptr.valuei = (int *)&(product[13].key_rbc);
   _rbc_n._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_n);
   
   _rbc_o._info = _Lst.pVar(KEY_PRI_RBC_O);
   _rbc_o._data_ptr.valuei = (int *)&(product[14].key_rbc);
   _rbc_o._storage_type = ConfigVarList::INT;
   _list.push_back(_rbc_o);


   // Blood Type
   _blood_type_a._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_A);
   _blood_type_a._data_ptr.valuei = (int *)&(product[0].key_blood_type);
   _blood_type_a._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_a);

   _blood_type_b._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_B);
   _blood_type_b._data_ptr.valuei = (int *)&(product[1].key_blood_type);
   _blood_type_b._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_b);

   _blood_type_c._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_C);
   _blood_type_c._data_ptr.valuei = (int *)&(product[2].key_blood_type);
   _blood_type_c._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_c);

   _blood_type_d._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_D);
   _blood_type_d._data_ptr.valuei = (int *)&(product[3].key_blood_type);
   _blood_type_d._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_d);

   _blood_type_e._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_E);
   _blood_type_e._data_ptr.valuei = (int *)&(product[4].key_blood_type);
   _blood_type_e._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_e);

   _blood_type_f._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_F);
   _blood_type_f._data_ptr.valuei = (int *)&(product[5].key_blood_type);
   _blood_type_f._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_f);

   _blood_type_g._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_G);
   _blood_type_g._data_ptr.valuei = (int *)&(product[6].key_blood_type);
   _blood_type_g._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_g);

   _blood_type_h._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_H);
   _blood_type_h._data_ptr.valuei = (int *)&(product[7].key_blood_type);
   _blood_type_h._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_h);

   _blood_type_i._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_I);
   _blood_type_i._data_ptr.valuei = (int *)&(product[8].key_blood_type);
   _blood_type_i._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_i);

   _blood_type_j._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_J);
   _blood_type_j._data_ptr.valuei = (int *)&(product[9].key_blood_type);
   _blood_type_j._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_j);

   _blood_type_k._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_K);
   _blood_type_k._data_ptr.valuei = (int *)&(product[10].key_blood_type);
   _blood_type_k._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_k);

   _blood_type_l._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_L);
   _blood_type_l._data_ptr.valuei = (int *)&(product[11].key_blood_type);
   _blood_type_l._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_l);

   _blood_type_m._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_M);
   _blood_type_m._data_ptr.valuei = (int *)&(product[12].key_blood_type);
   _blood_type_m._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_m);

   _blood_type_n._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_N);
   _blood_type_n._data_ptr.valuei = (int *)&(product[13].key_blood_type);
   _blood_type_n._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_n);

   _blood_type_o._info = _Lst.pVar(KEY_PRI_BLOOD_TYPE_O);
   _blood_type_o._data_ptr.valuei = (int *)&(product[14].key_blood_type);
   _blood_type_o._storage_type = ConfigVarList::INT;
   _list.push_back(_blood_type_o);

   
   // Time
   _time_a._info = _Lst.pVar(KEY_PRI_TIME_A);
   _time_a._data_ptr.valuef = (float *)&(product[0].key_time);
   _time_a._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_a);

   _time_b._info = _Lst.pVar(KEY_PRI_TIME_B);
   _time_b._data_ptr.valuef = (float *)&(product[1].key_time);
   _time_b._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_b);

   _time_c._info = _Lst.pVar(KEY_PRI_TIME_C);
   _time_c._data_ptr.valuef = (float *)&(product[2].key_time);
   _time_c._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_c);

   _time_d._info = _Lst.pVar(KEY_PRI_TIME_D);
   _time_d._data_ptr.valuef = (float *)&(product[3].key_time);
   _time_d._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_d);

   _time_e._info = _Lst.pVar(KEY_PRI_TIME_E);
   _time_e._data_ptr.valuef = (float *)&(product[4].key_time);
   _time_e._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_e);

   _time_f._info = _Lst.pVar(KEY_PRI_TIME_F);
   _time_f._data_ptr.valuef = (float *)&(product[5].key_time);
   _time_f._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_f);

   _time_g._info = _Lst.pVar(KEY_PRI_TIME_G);
   _time_g._data_ptr.valuef = (float *)&(product[6].key_time);
   _time_g._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_g);

   _time_h._info = _Lst.pVar(KEY_PRI_TIME_H);
   _time_h._data_ptr.valuef = (float *)&(product[7].key_time);
   _time_h._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_h);

   _time_i._info = _Lst.pVar(KEY_PRI_TIME_I);
   _time_i._data_ptr.valuef = (float *)&(product[8].key_time);
   _time_i._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_i);

   _time_j._info = _Lst.pVar(KEY_PRI_TIME_J);
   _time_j._data_ptr.valuef = (float *)&(product[9].key_time);
   _time_j._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_j);

   _time_k._info = _Lst.pVar(KEY_PRI_TIME_K);
   _time_k._data_ptr.valuef = (float *)&(product[10].key_time);
   _time_k._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_k);

   _time_l._info = _Lst.pVar(KEY_PRI_TIME_L);
   _time_l._data_ptr.valuef = (float *)&(product[11].key_time);
   _time_l._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_l);

   _time_m._info = _Lst.pVar(KEY_PRI_TIME_M);
   _time_m._data_ptr.valuef = (float *)&(product[12].key_time);
   _time_m._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_m);

   _time_n._info = _Lst.pVar(KEY_PRI_TIME_N);
   _time_n._data_ptr.valuef = (float *)&(product[13].key_time);
   _time_n._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_n);

   _time_o._info = _Lst.pVar(KEY_PRI_TIME_O);
   _time_o._data_ptr.valuef = (float *)&(product[14].key_time);
   _time_o._storage_type = ConfigVarList::FLOAT;
   _list.push_back(_time_o);
    
   return created;
}
   


//
// Assignment operator
//
ProductDefinitionElement ProductDefinitionElement::operator = (const ProductDefinitionArray &rhs)
{
   this->Set(rhs);

   return *this;
}

void ProductDefinitionElement::Log()
{
}


ProcedureListElement::ProcedureListElement()
{
}

ProcedureListElement::~ProcedureListElement()
{
}

void ProcedureListElement::Log()
{
}

//
// Executed during system initialization to read the configuration data file
// 

// attempts to fix a backup file with a good file including crc
bool fixFile(CFileHelper &goodFile, const char *fname, const char *crcfname)
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
bool checkFile(void)
{
    bool bMainOk = false;
    bool bBackOk = false;
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
    int nCase = (bMainOk ? 1 : 0) + (bBackOk ? 2 : 0);
    switch (nCase)
    {
    case 1: // Main file Ok, backup is bad.
        {
            // we need to update the backup file with the main one.
            DataLog(log_level_config_info) << "Attempting to fix backup config file." << endmsg;
            bSuccess = fixFile(fhelper, 
                               PNAME_CONFIGDAT_BK, 
                               PNAME_CONFIGDAT_CRC_BK);
            break;
        }
    case 2: // Backup file is good, main file is bad.
        {
            // we need to update the main file with the backup one.
            DataLog(log_level_config_info) << "Attempting to fix main config file." << endmsg;
            bSuccess = fixFile(fhelperBk, 
                               PNAME_CONFIGDAT,
                               PNAME_CONFIGDAT_CRC);
            break;
        }
    case 3: // Both files are good, make sure they are the same.
        {
            if (fhelper.CRC() != fhelperBk.CRC())
            {
                // assume the main one is the correct one.
                DataLog(log_level_config_info) << "Config files ok, but don't match." << endmsg;
                bSuccess = fixFile(fhelper,
                                   PNAME_CONFIGDAT_BK,
                                   PNAME_CONFIGDAT_CRC_BK);
            }
            else
            {
                // both match and are fine
                DataLog(log_level_config_info) << "Config.dat, backup and crc ok." << endmsg;
                bSuccess = true;
                break;
            }
            break;
        }
    default: // neither file is good, $%#*$#@!*
        {
            DataLog(log_level_config_info) << "Neither config or backup is ok." << endmsg;
            bSuccess = false;
            break;
        }
    }

    return bSuccess;
}

void readConfigCDS(void)
{
   //
   // Initialize the message system so that alarm messages
   // can be sent if config.dat can't be read correctly.
   //
   MessageSystem msgSystem;
   msgSystem.initNonBlock();
	
   if (!checkFile())
   {
      anAlarmMsg alarm(CONFIGDAT_BAD_FILE);
      alarm.setAlarm("CONFIG.DAT CRC Check Failed");
   }

   Config_CDS * cfgreader = new Config_CDS(ROLE_RW);
   cfgreader->Read();
   delete cfgreader;
}
