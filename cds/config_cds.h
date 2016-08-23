/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      config_cds.h
 *             Config Common Data Store
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the Common Data Store for Trima Procedure Configuration
 *             data
 *
*******************************************************************************/

#ifndef __CONFIG_CDS
#define __CONFIG_CDS

#include <sstream>
#include <string>
#include <list>
#include <map>

#include "datastore.h"      // Common Data Store framework
#include "configdef.h"      // Config Structure definitions
#include "cfg_vars.h"       // Config Vars
#include "datfile.h"        // Datfile reader
#include "trima_datalog.h"  // Datalog
#include "trimamessages.h"

#ifndef __SKIPMSGSYS__
#include "an_alarm.h"       // Alarm messages
#include "message.h"        // Messaging
#endif

#ifndef WIN32
#pragma interface
#endif

class ConfigVarList
{
// Data Members
public:

   typedef enum STORAGE_TYPE
   {
      INT, FLOAT, UNKNOWN
   };

   typedef union
   {
      float* valuef;
      int*   valuei;
   } VARPTR;

   const CCfgVarInfo* _info;
   VARPTR             _data_ptr;
   STORAGE_TYPE       _storage_type;

   inline ConfigVarList()
      : _info(0), _storage_type(UNKNOWN) { _data_ptr.valuef = 0; }

   // ConfigVarList(const CCfgVarInfo *info, float &value) : _info(info), _value(value){};
   virtual ~ConfigVarList() { }
};


// Forward Reference
class Config_CDS;


// DataStore Specialization for Config Class
template <class dataType> class ConfigElement
   : public BaseElement <dataType>
{
// Member Functions
protected:
   ConfigElement();
   virtual ~ConfigElement();

   virtual bool Valid ();  // returns false if not valid
   virtual void SetDefault (void);

   virtual const char* lastError () { return _lastError.c_str(); }

   //
   //
   //  These methods are available offline
   //
#ifdef __WIN32__
public:
#endif
   virtual bool Write (CDatFileReader& datfile); // returns false if not valid
   virtual bool Read (CDatFileReader& datfile);  // returns false if not valid

// Data Members
protected:
   typedef std::list<ConfigVarList> CONFIG_VAR_LIST;

   CONFIG_VAR_LIST _list;

   std::string     _lastError;
};



//
// Valid
//
template <class dataType> bool ConfigElement<dataType >::Valid ()
{
   ConfigVarList      item;
   std::ostringstream errorMsg;


   for (CONFIG_VAR_LIST::iterator i = _list.begin(); i != _list.end(); ++i)
   {
      item = *i;

      switch (item._storage_type)
      {
         case ConfigVarList::INT :
            if (!item._info->Valid((float)*item._data_ptr.valuei))
            {
               errorMsg << "Config Elememt " << item._info->Name() << " failed valid check.  "
                        << "Value(" << *item._data_ptr.valuei << "), Min("
                        << item._info->Min() << "), Max("
                        << item._info->Max() << ").";

               _lastError = errorMsg.str();

               DataLog(log_level_control_alarm_detail) << errorMsg.str() << endmsg;

               return false;
            }

            break;

         case ConfigVarList::FLOAT :
            if (!item._info->Valid(*item._data_ptr.valuef))
            {
               errorMsg << "Config Elememt " << item._info->Name() << " failed valid check.  "
                        << "Value(" << *item._data_ptr.valuef << "), Min("
                        << item._info->Min() << "), Max("
                        << item._info->Max() << ").";

               _lastError = errorMsg.str();

               DataLog(log_level_control_alarm_detail) << errorMsg.str() << endmsg;

               return false;
            }

            break;

         case ConfigVarList::UNKNOWN :
         default :
            errorMsg << "Config Element not recognized.  Section(" << item._info->Section()
                     << "), storage_type(" << item._storage_type
                     << "), Name(" << item._info->Name()
                     << "Value(" << *item._data_ptr.valuef << "), Min("
                     << item._info->Min() << "), Max("
                     << item._info->Max() << ").";

            _lastError = errorMsg.str();

            DataLog(log_level_control_alarm_detail) << errorMsg.str() << endmsg;
#ifndef __SKIPMSGSYS__
            anAlarmMsg alarm(CONFIGDAT_WRITE_ERROR);
            alarm.setAlarm();
#endif
            return false;
      }
   }

   _lastError = "";

   return true;
}



//
// Language Config Element specialization
//
class LangConfigElement
   : public ConfigElement<LangUnitCfg>
{
   friend class Config_CDS;

// Member Functions
public:
   LangConfigElement();
   virtual ~LangConfigElement();

   LangConfigElement& operator = (const LangUnitCfg& rhs);

   void Log ();

// Member Functions
protected:
   virtual bool Register (DataStore* ds, PfrType pfr);

// Data Members
private:
   ConfigVarList _lang;
   ConfigVarList _height;
   ConfigVarList _weight;
   ConfigVarList _date_format;
   ConfigVarList _time_format;
   ConfigVarList _decimal_delimiter;
   ConfigVarList _crit_or_hemoglobin;
};



//
// Basic Machine Element specialization
//
class BasicMachineElement
   : public ConfigElement<BasicMachineCfg>
{
   friend class Config_CDS;

// Member Functions
public:
   BasicMachineElement();
   virtual ~BasicMachineElement();

   BasicMachineElement& operator = (const BasicMachineCfg& rhs);

   void Log ();

// Member Functions
protected:
   virtual bool Register (DataStore* ds, PfrType pfr);

   virtual bool Valid ();  // returns false if not valid


// Data Members
private:
   ConfigVarList _set_time;
   ConfigVarList _set_date;
   ConfigVarList _set_audio;
   ConfigVarList _pword;
};



//
// Procedure Element specialization
//
class ProcedureElement
   : public ConfigElement<ProcedureConfig>
{
   friend class Config_CDS;

// Member Functions
public:
   ProcedureElement();
   virtual ~ProcedureElement();

   ProcedureElement& operator = (const ProcedureConfig& rhs);

   void Log ();

   virtual bool Read (CDatFileReader& datfile);

// Member Functions
protected:
   virtual bool Register (DataStore* ds, PfrType pfr);

// Data Members
private:

   FeatureChanged _featureChanged;
   void FeatureChangedNotification (void* data);
   void validatePreAasFlag ();

   ConfigVarList _proc_time;
   ConfigVarList _return_press;
   ConfigVarList _draw_press;
   ConfigVarList _custom_ratio;
   ConfigVarList _plasma_ratio;
   ConfigVarList _ac_rate;
   ConfigVarList _post_crit;
   ConfigVarList _tbv_vol_setting;
   ConfigVarList _tbv_percent;

   ConfigVarList _weight_setting;
   ConfigVarList _weight_less_than_vol;
   ConfigVarList _weight_greater_than_vol;

   ConfigVarList _ml_per_kg;
   ConfigVarList _drbc_body_volume;
   ConfigVarList _max_plasma_vol_during_plasma_collection;
   ConfigVarList _max_plasma_vol_during_platelet_collection;
   ConfigVarList _post_plat;
   ConfigVarList _max_draw_flow;
   ConfigVarList _min_replacement_volume;
   ConfigVarList _plt_mss_split_notif;
   ConfigVarList _key_override_pas_bag_vol;
   ConfigVarList _blood_diversion;
   ConfigVarList _drbc_alert;
   ConfigVarList _drbc_threshold;
   ConfigVarList _plasma_rinseback;
   ConfigVarList _saline_rinseback;
   ConfigVarList _audit_tracking;
   ConfigVarList _air_removal;
   ConfigVarList _mss_rbc_on;
   ConfigVarList _mss_plt_on;
   ConfigVarList _show_pre_aas_flags;
   ConfigVarList _rbc_def_bag_vol;
   ConfigVarList _plt_def_bag_vol;
   ConfigVarList _plt_vol_type;
   ConfigVarList _key_autoflow;


   // Used to prevent registering the FeatureChanged event more than once;
   bool _firstTimeRead;
};



//
// Prediction Element specialization
//
class PredictionElement
   : public ConfigElement<PredictionConfig>
{
   friend class Config_CDS;

// Member Functions
public:
   PredictionElement();
   virtual ~PredictionElement();

   PredictionElement& operator = (const PredictionConfig& rhs);

   void Log ();

// Member Functions
protected:
   virtual bool Register (DataStore* ds, PfrType pfr);

// Data Members
private:
   ConfigVarList _rbc_ratio;
   ConfigVarList _rbc_replace_fluid;
   ConfigVarList _rbc_fluid_percent;
   ConfigVarList _plasma_amap_min;
   ConfigVarList _plasma_amap_max;
   ConfigVarList _ysf;
   ConfigVarList _guaranteed_yield;
   ConfigVarList _confidence_level;
   ConfigVarList _coefficient_of_variation;
   ConfigVarList _inlet_management;
   ConfigVarList _inlet_flow_ramp;
   ConfigVarList _return_management;
   ConfigVarList _rsf;
   ConfigVarList _plasma_rinseback;
   ConfigVarList _saline_rinseback;
   ConfigVarList _male_only_plt;
   ConfigVarList _male_only_plasma;
   ConfigVarList _ffp_volume;

   ConfigVarList _complementary_plasma;

};



//
// ProductTemplates Element specialization
//
class ProductTemplatesElement
   : public ConfigElement<ProductTemplates>
{
   friend class Config_CDS;

// Member Functions
public:
   ProductTemplatesElement();
   virtual ~ProductTemplatesElement();

   ProductTemplatesElement& operator = (const ProductTemplates& rhs);

   void Log ();

// Member Functions
protected:
   virtual bool Register (DataStore* ds, PfrType pfr);

// Data Members
private:
   ConfigVarList _rbc_crit_1;
   ConfigVarList _rbc_crit_2;
   ConfigVarList _rbc_crit_3;
   ConfigVarList _rbc_crit_4;
   ConfigVarList _rbc_crit_5;
   ConfigVarList _rbc_crit_6;

   ConfigVarList _rbc_dose_1;
   ConfigVarList _rbc_dose_2;
   ConfigVarList _rbc_dose_3;
   ConfigVarList _rbc_dose_4;
   ConfigVarList _rbc_dose_5;
   ConfigVarList _rbc_dose_6;

   ConfigVarList _rbc_mss_1;
   ConfigVarList _rbc_mss_2;
   ConfigVarList _rbc_mss_3;
   ConfigVarList _rbc_mss_4;
   ConfigVarList _rbc_mss_5;
   ConfigVarList _rbc_mss_6;

   ConfigVarList _rbc_mss_volume_1;
   ConfigVarList _rbc_mss_volume_2;
   ConfigVarList _rbc_mss_volume_3;
   ConfigVarList _rbc_mss_volume_4;
   ConfigVarList _rbc_mss_volume_5;
   ConfigVarList _rbc_mss_volume_6;

   ConfigVarList _plt_yield_1;
   ConfigVarList _plt_yield_2;
   ConfigVarList _plt_yield_3;
   ConfigVarList _plt_yield_4;
   ConfigVarList _plt_yield_5;
   ConfigVarList _plt_yield_6;
   ConfigVarList _plt_yield_7;
   ConfigVarList _plt_yield_8;
   ConfigVarList _plt_yield_9;
   ConfigVarList _plt_yield_10;

   ConfigVarList _plt_volume_1;
   ConfigVarList _plt_volume_2;
   ConfigVarList _plt_volume_3;
   ConfigVarList _plt_volume_4;
   ConfigVarList _plt_volume_5;
   ConfigVarList _plt_volume_6;
   ConfigVarList _plt_volume_7;
   ConfigVarList _plt_volume_8;
   ConfigVarList _plt_volume_9;
   ConfigVarList _plt_volume_10;

   ConfigVarList _plt_mss_1;
   ConfigVarList _plt_mss_2;
   ConfigVarList _plt_mss_3;
   ConfigVarList _plt_mss_4;
   ConfigVarList _plt_mss_5;
   ConfigVarList _plt_mss_6;
   ConfigVarList _plt_mss_7;
   ConfigVarList _plt_mss_8;
   ConfigVarList _plt_mss_9;
   ConfigVarList _plt_mss_10;

   ConfigVarList _plt_amap_min_yield_1;
   ConfigVarList _plt_amap_min_yield_2;
   ConfigVarList _plt_amap_min_yield_3;

   ConfigVarList _plt_amap_max_yield_1;
   ConfigVarList _plt_amap_max_yield_2;
   ConfigVarList _plt_amap_max_yield_3;

   ConfigVarList _plt_amap_conc_1;
   ConfigVarList _plt_amap_conc_2;
   ConfigVarList _plt_amap_conc_3;
   ConfigVarList _plt_amap_conc_4;  // single with PAS
   ConfigVarList _plt_amap_conc_5;  // double with PAS
   ConfigVarList _plt_amap_conc_6;  // triple with PAS

   ConfigVarList _key_plt_amap_pas_pct_carryover_1;
   ConfigVarList _key_plt_amap_pas_pct_carryover_2;
   ConfigVarList _key_plt_amap_pas_pct_carryover_3;


   ConfigVarList _key_plt_pct_carryover_1;
   ConfigVarList _key_plt_pct_carryover_2;
   ConfigVarList _key_plt_pct_carryover_3;
   ConfigVarList _key_plt_pct_carryover_4;
   ConfigVarList _key_plt_pct_carryover_5;
   ConfigVarList _key_plt_pct_carryover_6;
   ConfigVarList _key_plt_pct_carryover_7;
   ConfigVarList _key_plt_pct_carryover_8;
   ConfigVarList _key_plt_pct_carryover_9;
   ConfigVarList _key_plt_pct_carryover_10;

   ConfigVarList _pls_volume_1;
   ConfigVarList _pls_volume_2;
   ConfigVarList _pls_volume_3;
   ConfigVarList _pls_volume_4;
   ConfigVarList _pls_volume_5;
   ConfigVarList _pls_volume_6;
};



//
// Product Definition Element specialization
//
class ProductDefinitionElement
   : public ConfigElement<ProductDefinitionArray>
{
   friend class Config_CDS;

// Member Functions
public:
   ProductDefinitionElement();
   virtual ~ProductDefinitionElement();

   ProductDefinitionElement& operator = (const ProductDefinitionArray& rhs);

   void Log ();

// Member Functions
protected:
   virtual bool Register (DataStore* ds, PfrType pfr);

// Data Members
private:
   // A
   ConfigVarList _platelet_a;
   ConfigVarList _platelet_amap_a;
   ConfigVarList _plasma_a;
   ConfigVarList _rbc_a;
   ConfigVarList _blood_type_a;
   ConfigVarList _time_a;

   // B
   ConfigVarList _platelet_b;
   ConfigVarList _platelet_amap_b;
   ConfigVarList _plasma_b;
   ConfigVarList _rbc_b;
   ConfigVarList _blood_type_b;
   ConfigVarList _time_b;

   // C
   ConfigVarList _platelet_c;
   ConfigVarList _plasma_c;
   ConfigVarList _platelet_amap_c;
   ConfigVarList _rbc_c;
   ConfigVarList _blood_type_c;
   ConfigVarList _time_c;

   // D
   ConfigVarList _platelet_d;
   ConfigVarList _platelet_amap_d;
   ConfigVarList _plasma_d;
   ConfigVarList _rbc_d;
   ConfigVarList _blood_type_d;
   ConfigVarList _time_d;

   // E
   ConfigVarList _platelet_e;
   ConfigVarList _platelet_amap_e;
   ConfigVarList _plasma_e;
   ConfigVarList _rbc_e;
   ConfigVarList _blood_type_e;
   ConfigVarList _time_e;

   // F
   ConfigVarList _platelet_f;
   ConfigVarList _plasma_f;
   ConfigVarList _platelet_amap_f;
   ConfigVarList _rbc_f;
   ConfigVarList _blood_type_f;
   ConfigVarList _time_f;

   // G
   ConfigVarList _platelet_g;
   ConfigVarList _plasma_g;
   ConfigVarList _platelet_amap_g;
   ConfigVarList _rbc_g;
   ConfigVarList _blood_type_g;
   ConfigVarList _time_g;

   // H
   ConfigVarList _platelet_h;
   ConfigVarList _plasma_h;
   ConfigVarList _platelet_amap_h;
   ConfigVarList _rbc_h;
   ConfigVarList _blood_type_h;
   ConfigVarList _time_h;

   // I
   ConfigVarList _platelet_i;
   ConfigVarList _plasma_i;
   ConfigVarList _platelet_amap_i;
   ConfigVarList _rbc_i;
   ConfigVarList _blood_type_i;
   ConfigVarList _time_i;

   // J
   ConfigVarList _platelet_j;
   ConfigVarList _plasma_j;
   ConfigVarList _platelet_amap_j;
   ConfigVarList _rbc_j;
   ConfigVarList _blood_type_j;
   ConfigVarList _time_j;

   // K
   ConfigVarList _platelet_k;
   ConfigVarList _plasma_k;
   ConfigVarList _platelet_amap_k;
   ConfigVarList _rbc_k;
   ConfigVarList _blood_type_k;
   ConfigVarList _time_k;

   // L
   ConfigVarList _platelet_l;
   ConfigVarList _plasma_l;
   ConfigVarList _platelet_amap_l;
   ConfigVarList _rbc_l;
   ConfigVarList _blood_type_l;
   ConfigVarList _time_l;

   // M
   ConfigVarList _platelet_m;
   ConfigVarList _plasma_m;
   ConfigVarList _platelet_amap_m;
   ConfigVarList _rbc_m;
   ConfigVarList _blood_type_m;
   ConfigVarList _time_m;

   // N
   ConfigVarList _platelet_n;
   ConfigVarList _plasma_n;
   ConfigVarList _platelet_amap_n;
   ConfigVarList _rbc_n;
   ConfigVarList _blood_type_n;
   ConfigVarList _time_n;

   // O
   ConfigVarList _platelet_o;
   ConfigVarList _plasma_o;
   ConfigVarList _platelet_amap_o;
   ConfigVarList _rbc_o;
   ConfigVarList _blood_type_o;
   ConfigVarList _time_o;

   // P
   ConfigVarList _platelet_p;
   ConfigVarList _plasma_p;
   ConfigVarList _platelet_amap_p;
   ConfigVarList _rbc_p;
   ConfigVarList _blood_type_p;
   ConfigVarList _time_p;

   // Q
   ConfigVarList _platelet_q;
   ConfigVarList _plasma_q;
   ConfigVarList _platelet_amap_q;
   ConfigVarList _rbc_q;
   ConfigVarList _blood_type_q;
   ConfigVarList _time_q;

   // R
   ConfigVarList _platelet_r;
   ConfigVarList _plasma_r;
   ConfigVarList _platelet_amap_r;
   ConfigVarList _rbc_r;
   ConfigVarList _blood_type_r;
   ConfigVarList _time_r;

   // S
   ConfigVarList _platelet_s;
   ConfigVarList _plasma_s;
   ConfigVarList _platelet_amap_s;
   ConfigVarList _rbc_s;
   ConfigVarList _blood_type_s;
   ConfigVarList _time_s;

   // T
   ConfigVarList _platelet_t;
   ConfigVarList _plasma_t;
   ConfigVarList _platelet_amap_t;
   ConfigVarList _rbc_t;
   ConfigVarList _blood_type_t;
   ConfigVarList _time_t;

};



//
// Procedure List Element
//
class ProcedureListElement
{
// Data Members
public:
   RangedElement<int>            numberOfProcedures;
   BaseElement<ProductListArray> Procedure;

   ProcedureListElement& operator = (const ProductListArray& rhs);

   void Log ();

// Class Methods
public:
   ProcedureListElement();
   virtual ~ProcedureListElement();
};




//
// The Config CDS is the overall container for the Trima Procedure Configuration.
// The Configuration can be written by either GUI through the config screens or by Vista
// so this class must serve both clients.
//
// The biggest difference between how Vista and Trima define a configuration is in the
// procedure list.  On Trima Config screens, the procedure list is made up of a number
// of product template definitions, as defined in the Templates object.  On Vista Config,
// the procedure list is not bound by the templates, but has a free form list of products.
// Also, Vista can define more procedures than Trima.
//
// To accomodate the difference, The Procedure List contains the list of procedures for
// Predict.  This list is either created from Vista or from the Trima Config.  The Trima
// Config copies all of the template definitions into the Vista style list for simplicity.
// Doing that makes the Template and the Definition objects effectively obsolete.  These
// are kept as containers for the config structure ONLY and should not be used for other
// reasons as these are not compatible with the Vista side.  The Procedure List object
// should be used for examining individual procedures and products.
//
class Config_CDS
   : public MultWriteDataStore
{
// Definitions
public:
   enum WRITE_OPTIONS { COPY_OFFLINE_PROCEDURE_LIST, DONT_COPY_OFFLINE_PROCEDURE_LIST };

// Data Members
public:
   LangConfigElement        LangUnit;
   BasicMachineElement      Machine;
   ProcedureElement         Procedure;
   PredictionElement        Predict;
   ProductTemplatesElement  Templates;
   ProductDefinitionElement Definition;     // Contains the 20 Offline procedures read from config.dat
   ProcedureListElement     ProcedureList;  // Contains the procedure list used by proc/gui (up to 50).

// Class Methods
public:
   static Config_CDS& GetInstance (void);

   // Write (commit) the data store to disk.  Also sends out ConfigUpdate message.
   bool Write (WRITE_OPTIONS opt = COPY_OFFLINE_PROCEDURE_LIST);

   // Reads the data file from disk to the data store.
   bool Read ();                       // from default pathname
   bool ReadConfig (const char* path); // if you want to specify a nondefault path (eg offline use)

   // Validates the Config Datastore
   bool Valid ();

   // Sends out the ConfigUpdate message.
   void SendUpdateMsg ();

   virtual ~Config_CDS();

   Config_CDS(Role role);

   const char* errorMsg () const { return _errorMsg.c_str(); }

   // warning: this does not and should not check stamp color.
   // you need to check that independant of this.
   const bool isPromptedDrbcSplit (int procNum) const;

#ifdef __WIN32__
//  Public for offline use
public:
   // Following get/set for offline use only
   ConfigStruct getConfigStruct ();
   void         setConfig (ConfigStruct cs);
   bool         WriteConfig (std::string filename);
#endif
   void CopyOfflineProcedureList ();

// Class Methods
private:
   Config_CDS();  // Base Constructor not available

   // Helper function
   void FillProcedureDescription (const ProductTemplates& pt, const ProductDefinition& pd, ProcedureDescription& pl, bool amapPltEnabled);

// Data Members
private:
   static DataLog_SetHandle _configLogSet;

   static CDatFileReader    _datfile;
   std::string              _errorMsg;

#if CPU==SIMNT
public:
   void SWOptionSet (const std::string& optionName, bool value);
   void SWOptionUnset (const std::string& optionName);
   // bool SWOptionEnabled(const char * optionName);
   // unsigned long SWOptionGet(const char * optionName);
private:
   std::map<const std::string, bool> optionOverrides;

#endif /* if CPU==SIMNT */
};

#endif

/* FORMAT HASH a88c47fd98c4a21ff7db35667953f115 */
