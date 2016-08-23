/*
 * Copyright (C) 2010 Gambro BCT, Inc.  All rights reserved.
 *
 * Software CDS.  This class encapsulates the features.bin file with added functionality.
 *
 * Author:  Johnathan Sylvain
 *
 * $Header$
 *
 */
#ifndef __SOFTWARE_CDS
#define __SOFTWARE_CDS

#include "datastore.h"
#include <vector>
#include <string>
using namespace std;

#ifdef WIN32
#define uint32_t int
#else
#include "option_file.h"
#endif

#include "BooleanFeatures.h"

#include "ValueFeatures.h"

enum FEATURE_PERMISSION
{
   RND_ONLY,
   SERVICE,
   ANYONE
};

enum KEY_RESULT
{
   SUCCESS,
   FAILED_UNAUTHORIZED_USER,
   FAILED_UNAUTHORIZED_SERIAL_NUMBER,
   FAILED_UNAUTHORIZED_SITE,
   FAILED_UNAUTHORIZED_CUSTOMER,
   FAILED_UNAUTHORIZED_COUNTRY,
   FAILED_BAD_KEY
};

enum KEY_AUTH
{
   AUTH_R_N_D,
   AUTH_SERVICE,
   AUTH_USER
};

const int INVALID_KEY_ID = -1;

class Software_CDS
   : public SingleWriteDataStore
{
public:

   bool        getFeature (FEATURE_ID featureId);
   uint32_t    getValueFeature (VALUE_FEATURE_ID featureId);
   const char* getFeatureConfigFileLabel (FEATURE_ID item);
   const char* getValueFeatureConfigFileLabel (VALUE_FEATURE_ID item);
   const char* GetGUISetting (const char* optionName);

   KEY_RESULT decodeKey (string& key, KEY_AUTH auth);

   string convertIntToIP (uint32_t ipValue);

   //
   // Changes GUI behavior
   //

   // Japan
   long getDontConnectAcMsgAtSetLoad () {return dont_connect_ac_msg_at_set_load.Get(); }
   long getTimeOnlyConfigMode () {return time_only_config_mode.Get(); }

   // Is the feature set a Japanese-only configuration?
   static bool isJapaneseWorkflow ();

   float getTouchscreenBoundDelay () {return touchscreen_bounce_delay.Get(); }
   int   getRbcMaxProdVol () {return rbc_max_prod_vol.Get(); }
   float getMaxMeteringDuration () {return max_metering_duration.Get(); }

   int    getSiteId ();
   string getSiteName ();
   int    getCustomerId ();
   string getCustomerName ();
   int    getCountryId ();
   string getCountryName ();

   bool             SetFeature (int featureKeyId, bool turnOn, KEY_AUTH auth, bool sendUpdate /*=true*/);
   FEATURE_ID       FindFeatureId (int featureKeyId);
   VALUE_FEATURE_ID FindValueFeatureId (int featureKeyId);
   bool             SetValueFeature (int featureKeyId, uint32_t value, KEY_AUTH auth, bool sendUpdate /*=true*/);
   void             SendUpdateNotification ();

   void WriteFeatureFile ();

   string getShortConfirmationCode ();
   string getFullConfirmationCode ();

   KEY_AUTH getCurrentAuthLevel ();

   // Is it a hyperconcentrate?
   static bool isPPC (float conc);

   //
   //
   //  Use these to check if a platelet product is above or below storage range.  The target value
   //  is used to determine which set of ranges to use (standard or hyper), which are then compared
   //  against the actual concentration.  These are static for the convenience of the user.
   //
   static bool  aboveStorageRange (const float target_concentration, const float actual_concentration);
   static bool  belowStorageRange (const float target_concentration, const float actual_concentration);
   static bool  withinStorageRange (const float target_concentration, const float actual_concentration);
   static bool  outsideStorageRange (const float target_concentration, const float actual_concentration);
   static float getLowConcentrationRange (const float target_concentration);
   static float getHighConcentrationRange (const float target_concentration);

   // Vista needs it. Not advisable to create your own instances. This class is meant to be a singleton
   static Software_CDS& GetInstance ();
   static bool          Read (const char* pathname);
#ifndef __WIN32__
   static bool Read (void);
#endif

#ifndef __WIN32__
   virtual ~Software_CDS(void);
#endif

private:
   Software_CDS(Role role);
   static bool                _dataLoaded;

   BaseElement<unsigned long> dont_connect_ac_msg_at_set_load;

   // General
   BaseElement<unsigned long> time_only_config_mode;

   BaseElement<float>         touchscreen_bounce_delay;
   BaseElement<int>           rbc_max_prod_vol;
   BaseElement<float>         max_metering_duration;

   BaseElement<string>        serial_number_machine_id;
   BaseElement<int>           site_id;
   BaseElement<string>        site_name;
   BaseElement<int>           customer_id;
   BaseElement<string>        customer_name;
   BaseElement<int>           country_id;
   BaseElement<string>        country_name;

   BaseElement<unsigned long> _dont_connect_ac_msg_at_set_load;
   BaseElement<unsigned long> _alternate_next_procedure_button;
   BaseElement<unsigned long> _time_only_config_mode;
   BaseElement<unsigned long> _always_recover_platelet_inventory;

   struct FeatureInfo
   {
      FeatureInfo()
         : feature_file_label(NULL), feature_key_id(INVALID_KEY_ID), permission(RND_ONLY){}
      const char*        feature_file_label;
      int                feature_key_id;
      BaseElement<bool>  value;
      FEATURE_PERMISSION permission;
   };

   struct ValueFeatureInfo
   {
      ValueFeatureInfo()
         : feature_file_label(NULL), feature_key_id(INVALID_KEY_ID), permission(RND_ONLY){}
      const char*           feature_file_label;
      int                   feature_key_id;
      BaseElement<uint32_t> value;
      FEATURE_PERMISSION    permission;
   };

   vector<FeatureInfo>      _features;
   vector<ValueFeatureInfo> _valueFeatures;
   void        LogConfigFileContents (const char* file);
   int         zipFile (const char* from, const char* to);
   int         unzipFile (const char* from, const char* to);
   const char* GetFeatureFile (const char* pathname);

   static OptionFileData* _guiData;
#if CPU==SIMNT
public:
#endif
   static Software_CDS& getRWCds ();

   void            ReleaseFileData (OptionFileData* data);
   OptionFileData* ReadGui (const char* pathname);
   OptionFileData* ReadFeatures (const char* pathname);
   bool            LoadFileData (const char* pathname);

   const char* GetSettingByName (OptionFileData* data, const char* optionName);
   const char* GetSetting (OptionFileData* data, FEATURE_ID optionId);
   bool        GetFlagSetting (OptionFileData* swData, FEATURE_ID optionId, BaseElement<bool>& value);
   bool        GetGuiValueSetting (OptionFileData* guiData, const char* optionName, BaseElement<int>& value);
   bool        GetValueSetting (OptionFileData* guiData, VALUE_FEATURE_ID optionId, BaseElement<uint32_t>& value);
   bool        GetStringValueSetting (OptionFileData* guiData, const char* optionName, BaseElement<string>& value);
   bool        GetGuiValueSetting (OptionFileData* guiData, const char* optionName, BaseElement<float>& value);

   void booleanFeatureInit ();
   void valueFeatureInit ();

   bool isModAllowed (FEATURE_PERMISSION permission, KEY_AUTH authorization);

   FEATURE_PERMISSION FindFeaturePermission (int featureKeyId);
   FEATURE_PERMISSION FindValueFeaturePermission (int featureKeyId);

   void updateCrc ();

};

#endif // __SOFTWARE_CDS

/* FORMAT HASH 6fbe2367ffcbde035b19ba8a693ad070 */
