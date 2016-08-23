/******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      vip_common.cpp
 *             Vista Interface Common Code
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This class provides helper functions for the management of donors
 *             and configuration data.
 *
 ******************************************************************************/


#include <math.h>

#include "vip_common.h"

#include "trima_datalog.h"  // Datalog Levels
#include "message.h"        // Message Stuff
#include "trimamessages.h"

#include "crcgen.h"       // for crcgen32 function.
#include "units.h"        // for metric/english conversion factors.

#include "cfg_vars.h"     // To get config ranges
#include "bloodtype.h"    // To get blood types

#include "cobeconfig.h"   // To get Cobe Config items
#include "software_cds.h" // To get feature settings

#include "trima_assert.h"  // To get assert function and macros


#define VIP_TBV_DELTA 25.0f


////////////////////////////////////////////////////////////////////////////////
// Compile-time consistency checks
////////////////////////////////////////////////////////////////////////////////

// Lang checks
TRIMA_STATIC_ASSERT(ENGLISH_LANG    == VIP_LANG_ENGLISH);
TRIMA_STATIC_ASSERT(FRENCH_LANG     == VIP_LANG_FRENCH);
TRIMA_STATIC_ASSERT(GERMAN_LANG     == VIP_LANG_GERMAN);
TRIMA_STATIC_ASSERT(ITALIAN_LANG    == VIP_LANG_ITALIAN);
TRIMA_STATIC_ASSERT(JAPAN_LANG      == VIP_LANG_JAPANESE);
TRIMA_STATIC_ASSERT(SPANISH_LANG    == VIP_LANG_SPANISH);
TRIMA_STATIC_ASSERT(DUTCH_LANG      == VIP_LANG_DUTCH);
TRIMA_STATIC_ASSERT(PORTUGUESE_LANG == VIP_LANG_PORTUGUESE);
TRIMA_STATIC_ASSERT(SWEDISH_LANG    == VIP_LANG_SWEDISH);
TRIMA_STATIC_ASSERT(KOREAN_LANG     == VIP_LANG_KOREAN);
TRIMA_STATIC_ASSERT(RUSSIAN_LANG    == VIP_LANG_RUSSIAN);
TRIMA_STATIC_ASSERT(CZECH_LANG      == VIP_LANG_CZECH);
TRIMA_STATIC_ASSERT(POLISH_LANG     == VIP_LANG_POLISH);
TRIMA_STATIC_ASSERT(CHINESE_LANG    == VIP_LANG_CHINESE);

// Units checks
TRIMA_STATIC_ASSERT(FT_HEIGHT       == VIP_UNITS_IN);
TRIMA_STATIC_ASSERT(CENTI_HEIGHT    == VIP_UNITS_CM);

TRIMA_STATIC_ASSERT(KG_WEIGHT       == VIP_UNITS_KG);
TRIMA_STATIC_ASSERT(LB_WEIGHT       == VIP_UNITS_LB);

TRIMA_STATIC_ASSERT(RBC_UNIT_HEMOGLOBIN == VIP_RBC_HEMOGLOBIN);
TRIMA_STATIC_ASSERT(RBC_UNIT_CRIT       == VIP_RBC_HEMATOCRIT);

// Date format checks
TRIMA_STATIC_ASSERT(MMDDYY_DATE == VIP_DATE_MMDDYY);
TRIMA_STATIC_ASSERT(DDMMYY_DATE == VIP_DATE_DDMMYY);
TRIMA_STATIC_ASSERT(YYMMDD_DATE == VIP_DATE_YYMMDD);

// Date and Time checks
TRIMA_STATIC_ASSERT(TWENTYFOUR_TIME_FORMAT == VIP_TIME_24H);
TRIMA_STATIC_ASSERT(STANDARD_TIME_FORMAT   == VIP_TIME_12H);

// Deliminator checks
TRIMA_STATIC_ASSERT(COMMA_DELIMIT   == VIP_DELIM_COMMA);
TRIMA_STATIC_ASSERT(DECIMAL_DELIMIT == VIP_DELIM_DECIMAL);

// Audio checks
TRIMA_STATIC_ASSERT(AUDIO_HIGH == VIP_AUDIO_HIGH);
TRIMA_STATIC_ASSERT(AUDIO_MED  == VIP_AUDIO_MED);
TRIMA_STATIC_ASSERT(AUDIO_LOW  == VIP_AUDIO_LOW);

// Blood Type
TRIMA_STATIC_ASSERT(BT_UNKNOWN == VIP_BLOOD_UNKNOWN);
TRIMA_STATIC_ASSERT(BT_O_NEG   == VIP_BLOOD_O_NEG);
TRIMA_STATIC_ASSERT(BT_O_POS   == VIP_BLOOD_O_POS);
TRIMA_STATIC_ASSERT(BT_AB_NEG  == VIP_BLOOD_AB_NEG);
TRIMA_STATIC_ASSERT(BT_AB_POS  == VIP_BLOOD_AB_POS);
TRIMA_STATIC_ASSERT(BT_A_NEG   == VIP_BLOOD_A_NEG);
TRIMA_STATIC_ASSERT(BT_A_POS   == VIP_BLOOD_A_POS);
TRIMA_STATIC_ASSERT(BT_B_NEG   == VIP_BLOOD_B_NEG);
TRIMA_STATIC_ASSERT(BT_B_POS   == VIP_BLOOD_B_POS);
TRIMA_STATIC_ASSERT(BT_ALL     == VIP_BLOOD_ALL);

// Volume Removal
TRIMA_STATIC_ASSERT(VOLUME_REMOVAL_TBV           == VIP_TBV_PERCENT);
TRIMA_STATIC_ASSERT(VOLUME_REMOVAL_WEIGHT        == VIP_TBV_GTLT);
TRIMA_STATIC_ASSERT(VOLUME_REMOVAL_ML_PER_KG     == VIP_TBV_MLKG);
TRIMA_STATIC_ASSERT(VOLUME_REMOVAL_DRBC_BODY_VOL == VIP_TBV_DRBC);

// Draw flow
TRIMA_STATIC_ASSERT(DRAW_FLOW_SLOW     == VIP_DRAW_SLOW);
TRIMA_STATIC_ASSERT(DRAW_FLOW_MODERATE == VIP_DRAW_MED);
TRIMA_STATIC_ASSERT(DRAW_FLOW_FAST     == VIP_DRAW_FAST);

// TRALI settings
TRIMA_STATIC_ASSERT(MALE_ONLY_OPTION_MALE_ONLY            == VIP_TRALI_MALE_ONLY);
TRIMA_STATIC_ASSERT(MALE_ONLY_OPTION_MALES_AND_AB_FEMALES == VIP_TRALI_MALE_AB_FEMALE);
TRIMA_STATIC_ASSERT(MALE_ONLY_OPTION_ALL                  == VIP_TRALI_ALL);

// Template Arrays
TRIMA_STATIC_ASSERT(NUM_CRIT_DOSE          == VIP_MAX_CRIT_DOSE);
TRIMA_STATIC_ASSERT(NUM_YIELD_VOLUME       == VIP_MAX_PLATELETYV);
TRIMA_STATIC_ASSERT(NUM_PLASMA_VOLUME      == VIP_MAX_PLASMA_VOLUME);
TRIMA_STATIC_ASSERT(NUM_OFFLINE_PROCEDURES == VIP_MAX_PROCEDURE_LIST);
TRIMA_STATIC_ASSERT(NUM_VISTA_PROCEDURES   == VIP_MAX_VISTA_PROCEDURES);
TRIMA_STATIC_ASSERT(NUM_PLT_AMAP_PAS       == VIP_MAX_AMAP_PLT_PRODUCTS);
TRIMA_STATIC_ASSERT(NUM_PLT_AMAP_PAS       == VIP_MAX_AMAP_PLT_PCO);
TRIMA_STATIC_ASSERT(NUM_PLT_AMAP           == VIP_MAX_AMAP_PLT_CONCENTRATIONS);

//
// Base Constructor
//
VistaConfig::VistaConfig()
   : _config(0),
     _pltYield(KEY_PLT_YIELD_1),
     _pltVolume(KEY_PLT_VOLUME_1),
     _meterPAS(KEY_PLT_MSS_1),
     _pco(KEY_PLT_PCT_CARRYOVER_1),
     _plsVolume(KEY_PLS_VOLUME_1),
     _rbcDose(KEY_RBC_DOSE_1),
     _rbcCrit(KEY_RBC_CRIT_1),
     _maxProcTime(KEY_PRI_TIME_A),
     _rbcMss(KEY_RBC_MSS_1),
     _rbcMssVolume(KEY_RBC_MSS_VOLUME_1),
     _amapPltIndex(KEY_PRI_PLATELET_AMAP_A),
     _amapPltYield(KEY_PLT_AMAP_MIN_YIELD_1),
     _amapPltConcentration(KEY_PLT_AMAP_CONC_1),
     _amapPltPasPco(KEY_PLT_AMAP_PAS_CARRYOVER_1),
     _numberOfProcedures(0),
     _complementaryPlasmaOn(false),
     _rbcMaxProductVolume(0)
{
   _FATAL_ERROR(__FILE__, __LINE__, "VistaConfig base constructor called");
}



//
//
//
VistaConfig::VistaConfig(Config_CDS* cds)
   : _config(cds),
     _pltYield(KEY_PLT_YIELD_1),
     _pltVolume(KEY_PLT_VOLUME_1),
     _meterPAS(KEY_PLT_MSS_1),
     _pco(KEY_PLT_PCT_CARRYOVER_1),
     _plsVolume(KEY_PLS_VOLUME_1),
     _rbcDose(KEY_RBC_DOSE_1),
     _rbcCrit(KEY_RBC_CRIT_1),
     _maxProcTime(KEY_PRI_TIME_A),
     _rbcMss(KEY_RBC_MSS_1),
     _rbcMssVolume(KEY_RBC_MSS_VOLUME_1),
     _amapPltIndex(KEY_PRI_PLATELET_AMAP_A),
     _amapPltYield(KEY_PLT_AMAP_MIN_YIELD_1),
     _amapPltConcentration(KEY_PLT_AMAP_CONC_1),
     _amapPltPasPco(KEY_PLT_AMAP_PAS_CARRYOVER_1),
     _numberOfProcedures(0),
     _complementaryPlasmaOn(Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable)),
     _rbcMaxProductVolume(Software_CDS::GetInstance().getRbcMaxProdVol())
{}



//
// Base Destructor
//
VistaConfig::~VistaConfig()
{}



//
//
//
int VistaConfig::ProcessDonorVitals (CDonor& donor, const donorInfoMsg& donorInfo)
{
   _errorMsg.str(string());  // Clear the string

   // Return a reference to the encapsulated donor vitals
   SDonorVitals& donorVitals = donor.Data();

   // Validate the string size of the donor name.  Note.  Passing a null
   // string to strcpy causes a segv!
   int size = strlen(donorInfo.donorName);

   if ( (size > 0) && (size < VIP_MAX_DONOR_NAME - 1))
   {
      strcpy (donorVitals.cDonorName, donorInfo.donorName);
   }
   else
   {
      // Log event for later debugging.
      _errorMsg << "donor name size = 0" << std::ends;

      return VIP_BAD_VALUE;
   }

   size = strlen(donorInfo.donorDOB);

   // Validate the string size of the donor DOB.  Note.  Passing a null
   // string to strcpy causes a segv!
   if (size != VIP_DATE_LENGTH)
   {
      // Log event for later debugging.
      _errorMsg << "invalid DOB size, value=" << size << ", expected " << VIP_DATE_LENGTH << std::ends;

      return VIP_BAD_VALUE;
   }

   // Validate the format of the DOB.
   if (strspn(donorInfo.donorDOB, "0123456789") != VIP_DATE_LENGTH)
   {
      // Log event for later debugging.
      _errorMsg << "invalid DOB character, value=" << donorInfo.donorDOB
                << ", expected [0123456789]" << std::ends;

      return VIP_BAD_VALUE;
   }

   // TBD.  Validate the individual YYYYMMDD ranges.
   strcpy (donorVitals.cDonorDateofBirth, donorInfo.donorDOB);


   // Update the donor vitals.
   donorVitals.cDonorSex          = donorInfo.donorGender;
   donorVitals.fDonorHeight       = donorInfo.donorHeight;
   donorVitals.fDonorWeight       = donorInfo.donorWeight;
   donorVitals.fDonorSampleVolume = donorInfo.donorSampleVolume;
   donorVitals.DonorBloodType     = BLOODTYPE(donorInfo.donorBloodType);
   donorVitals.fDonorHct          = donorInfo.donorHematocrit;
   donorVitals.fDonorPltPrecount  = donorInfo.donorPrecount;

   if (donorInfo.heightUnits == VIP_UNITS_IN)
   {
      donorVitals.cDonorHeightUnits = HEIGHT_UNITS_IN;

      // Interface default is metric.  Only change if units are different.
      donorVitals.fDonorHeight *= IN_PER_CM;
   }
   else if (donorInfo.heightUnits == VIP_UNITS_CM)
   {
      donorVitals.cDonorHeightUnits = HEIGHT_UNITS_CM;
   }
   else
   {
      // Log event for later debugging.
      _errorMsg << "invalid Height Units, value=" << donorInfo.heightUnits
                << ", expected " << VIP_UNITS_CM << "-" << VIP_UNITS_IN << std::ends;

      return VIP_BAD_VALUE;
   }

   if (donorInfo.weightUnits == VIP_UNITS_LB)
   {
      donorVitals.cDonorWeightUnits = WEIGHT_UNITS_LB;

      // Interface default is metric.  Only change if units are different.
      donorVitals.fDonorWeight *= LB_PER_KG;
   }
   else if (donorInfo.weightUnits == VIP_UNITS_KG)
   {
      donorVitals.cDonorWeightUnits = WEIGHT_UNITS_KG;
   }
   else
   {
      // Log event for later debugging.
      _errorMsg << "invalid Weight Units, value=" << donorInfo.weightUnits
                << ", expected " << VIP_UNITS_KG << "-" << VIP_UNITS_LB << std::ends;

      return VIP_BAD_VALUE;
   }

   if (donorInfo.rbcMeasurementType == VIP_RBC_HEMATOCRIT)
      donorVitals.cDonorRbcCount = RBC_COUNT_HEMATOCRIT;
   else if (donorInfo.rbcMeasurementType == VIP_RBC_HEMOGLOBIN)
      donorVitals.cDonorRbcCount = RBC_COUNT_HEMOGLOBIN;
   else
   {
      // Log event for later debugging.
      _errorMsg << "invalid RBC Measurement Units, value=" << donorInfo.rbcMeasurementType
                << ", expected " << VIP_RBC_HEMATOCRIT << "-" << VIP_RBC_HEMOGLOBIN << std::ends;

      return VIP_BAD_VALUE;
   }

   // Calculate the CRC for the donor picture.
   donorVitals.DonorDataCRC = 0;  // Both GUI and VIP need to use the same seed value.
   crcgen32 (&donorVitals.DonorDataCRC, &donorInfo.donorPicture[0], VIP_BITMAP_SIZE);

   //
   // Validate that the donor info entered is valid.
   //
   if (!donor.ValidSex())
   {
      // Log event for later debugging.
      _errorMsg << "donorInfo: Invalid Gender(" << (int)donorVitals.cDonorSex << ")." << std::ends;

      return VIP_BAD_VALUE;
   }

   if (!donor.ValidWeight())
   {
      // Log event for later debugging.
      _errorMsg << "donorInfo: Invalid Weight(" << donorVitals.fDonorWeight << ")." << std::ends;

      return VIP_BAD_VALUE;
   }

   if (!donor.ValidHeight())
   {
      // Log event for later debugging.
      _errorMsg << "donorInfo: Invalid Height(" << donorVitals.fDonorHeight << ")." << std::ends;

      return VIP_BAD_VALUE;
   }

   if (!donor.ValidHct())
   {
      // Log event for later debugging.
      _errorMsg << "donorInfo: Invalid Hematocrit(" << donorVitals.fDonorHct << ")." << std::ends;

      return VIP_BAD_VALUE;
   }

   if (!donor.ValidPreCnt())
   {
      // Log event for later debugging.
      _errorMsg << "donorInfo: Invalid Precount(" << donorVitals.fDonorPltPrecount << ")." << std::ends;

      return VIP_BAD_VALUE;
   }

   if (!donor.ValidUnits())
   {
      // Log event for later debugging.
      _errorMsg << "donorInfo: Invalid Units W(" << int(donorVitals.cDonorWeightUnits)
                << ") H(" << (int)donorVitals.cDonorHeightUnits << ")." << std::ends;

      return VIP_BAD_VALUE;
   }

   if (!donor.ValidBloodType())
   {
      // Log event for later debugging.
      _errorMsg << "donorInfo: Invalid BloodType(" << donorVitals.DonorBloodType << ")." << std::ends;

      return VIP_BAD_VALUE;
   }

   if (!donor.ValidSampleVolume())
   {
      // Log event for later debugging.
      _errorMsg << "donorInfo: Invalid Sample Volume(" << donorVitals.fDonorSampleVolume << ")." << std::ends;

      return VIP_BAD_VALUE;
   }

   if (!donor.ValidRBCUnits())
   {
      // Log event for later debugging.
      _errorMsg << "donorInfo: Invalid RBC Units(" << (int)donorVitals.cDonorRbcCount << ")." << std::ends;

      return VIP_BAD_VALUE;
   }

   // Validate that the TBV that Vista calculated is within 25 mls of the Trima calculated value.
   if (fabs(donor.CalTBV() - donorInfo.donorTBV) > VIP_TBV_DELTA)
   {
      _errorMsg << "donorInfo: Invalid Vista TBV.  Calculated (" << donor.CalTBV()
                << "), Received (" << donorInfo.donorTBV << ")." << std::ends;

      return VIP_BAD_VALUE;
   }

   // Extra sanity check to make sure nothing is missed.
   if (!donor.Valid())
   {
      // Log event for later debugging.
      _errorMsg << "donorInfo: Donor Invalid. Internal consistency error!." << std::ends;

      return VIP_BAD_VALUE;
   }

   _errorMsg << "" << std::ends;

   return 0;
}



//
// This routine copies over the Language section of the offline configuration
// (config.dat)
//
void VistaConfig::CopyLanguageSection (const donorInfoMsg& donorInfo)
{
   LangUnitCfg lang;

   // Change the Language Config.
   lang.key_lang              = donorInfo.languageOnTrima;
   lang.key_height            = donorInfo.heightUnits;
   lang.key_weight            = donorInfo.weightUnits;
   lang.key_date_format       = donorInfo.dateFormat;
   lang.key_time_format       = donorInfo.timeFormat;
   lang.key_decimal_delimiter = donorInfo.decimalDelimeter;
   lang.key_crit_or_glob      = donorInfo.rbcMeasurementType;

   // Set the lang unit section.
   _config->LangUnit.Set(lang);
}



//
// This routine copies over the Language section of the offline configuration
// (config.dat)
//
void VistaConfig::CopyLanguageSection (const setOfflineConfigurationMsg& newConfig)
{
   LangUnitCfg lang;

   // Change the Language Config.
   lang.key_lang              = newConfig.languageOnTrima;
   lang.key_height            = newConfig.heightUnits;
   lang.key_weight            = newConfig.weightUnits;
   lang.key_date_format       = newConfig.dateFormat;
   lang.key_time_format       = newConfig.timeFormat;
   lang.key_decimal_delimiter = newConfig.decimalDelimeter;
   lang.key_crit_or_glob      = newConfig.rbcMeasurementType;

   // Set the lang unit section.
   _config->LangUnit.Set(lang);
}



//
// This routine copies over the Machine section of the offline configuration
// (config.dat)
//
void VistaConfig::CopyMachineSection (const donorInfoMsg& donorInfo)
{
   BasicMachineCfg machine;

   // Get the machine section
   _config->Machine.Get(&machine);

   // Change the Machine Config.
   machine.key_set_audio = donorInfo.audioLevel;

   // NOTE:  date, time, and pword are not settable.

   // Set the machine section.
   _config->Machine.Set(machine);
}



//
// This routine copies over the Machine section of the offline configuration
// (config.dat)
//
void VistaConfig::CopyMachineSection (const setOfflineConfigurationMsg& newConfig)
{
   BasicMachineCfg machine;

   // Get the machine section
   _config->Machine.Get(&machine);

   // Change the Machine Config.
   machine.key_set_audio = newConfig.audioLevel;

   // NOTE:  date, time, and pword are not settable.

   // Set the machine section.
   _config->Machine.Set(machine);
}



//
// This routine copies over the Procedure section of the offline configuration
// (config.dat)
//
void VistaConfig::CopyProcedureSection (const donorInfoMsg& donorInfo)
{
   ProcedureConfig procedure;

   // Change the Procedure Config.
   procedure.key_proc_time       = donorInfo.maximumProcedureTime;
   procedure.key_return_press    = donorInfo.maximumReturnPressure;
   procedure.key_draw_press      = donorInfo.maximumDrawPressure;
   procedure.key_custom_ratio    = donorInfo.plateletAcRatio;
   procedure.key_ac_rate         = donorInfo.acRate;
   procedure.key_plasma_ratio    = donorInfo.plasmaAcRatio;
   procedure.key_post_crit       = donorInfo.postHematocrit;
   procedure.key_tbv_vol_setting = donorInfo.tbvSetting;
   procedure.key_tbv_percent     = donorInfo.tbvPercent;

   // Change the weight setting from Kg to Lb's.
   const float weightInLb = donorInfo.weightSetting * LB_PER_KG;

   procedure.key_weight_setting       = weightInLb;
   procedure.key_weight_less_than_vol = donorInfo.noLessThanVolume;
   procedure.key_weight_greater_than_vol                   = donorInfo.noGreaterThanVolume;
   procedure.key_ml_per_kg                                 = donorInfo.mlPerKg;
   procedure.key_drbc_body_vol                             = donorInfo.drbcTbvLimit;
   procedure.key_max_plasma_vol_during_plasma_collection   = donorInfo.maxPlasmaVolDuringPlasmaCollection;
   procedure.key_max_plasma_vol_during_platelet_collection = donorInfo.maxPlasmaVolDuringPlateletCollection;
   procedure.key_post_plat                                 = donorInfo.postProcedurePlateletCount;
   procedure.key_max_draw_flow                             = donorInfo.maxDrawFlow;
   procedure.key_min_replacement_volume                    = donorInfo.minReplacementVolume;
   procedure.key_plt_mss_split_notif                       = donorInfo.plateletDivertPrompts;
   procedure.key_override_pas_bag_vol                      = donorInfo.solutionsBagVolOverride;
   procedure.key_blood_diversion                           = donorInfo.bloodDivertPrompts;
   procedure.key_drbc_alert                                = donorInfo.drbcSplitNotification;
   procedure.key_drbc_threshold                            = donorInfo.drbcThreshold;
   procedure.key_plasma_rinseback                          = donorInfo.plasmaRinseback;
   procedure.key_saline_rinseback                          = donorInfo.salineRinseback;
   procedure.key_audit_tracking                            = donorInfo.auditTrailLevel;
   procedure.key_air_removal                               = donorInfo.airRemovalEnabled;
   procedure.key_mss_plt_on                                = donorInfo.pasEnabled;
   procedure.key_mss_rbc_on                                = donorInfo.rasEnabled;
   procedure.key_show_pre_aas_flags                        = donorInfo.showPreAasFlags;
   procedure.key_plt_def_bag_vol                           = donorInfo.pasDefaultBagVolume;
   procedure.key_rbc_def_bag_vol                           = donorInfo.rasDefaultBagVolume;
   procedure.key_autoflow                                  = 0;  // Disable for 6.4, probably turn on for 7.0

   // Set the procedure section.
   _config->Procedure.Set(procedure);
}



//
// This routine copies over the Procedure section of the offline configuration
// (config.dat)
//
void VistaConfig::CopyProcedureSection (const setOfflineConfigurationMsg& newConfig)
{
   ProcedureConfig procedure;

   // Change the Procedure Config.
   procedure.key_proc_time       = newConfig.maximumProcedureTime;
   procedure.key_return_press    = newConfig.maximumReturnPressure;
   procedure.key_draw_press      = newConfig.maximumDrawPressure;
   procedure.key_custom_ratio    = newConfig.plateletAcRatio;
   procedure.key_plasma_ratio    = newConfig.plasmaAcRatio;
   procedure.key_ac_rate         = newConfig.acRate;
   procedure.key_post_crit       = newConfig.postHematocrit;
   procedure.key_tbv_vol_setting = newConfig.tbvSetting;
   procedure.key_tbv_percent     = newConfig.tbvPercent;

   const float weightInLb = newConfig.weightSetting * LB_PER_KG;

   procedure.key_weight_setting       = weightInLb;
   procedure.key_weight_less_than_vol = newConfig.noLessThanVolume;
   procedure.key_weight_greater_than_vol                   = newConfig.noGreaterThanVolume;

   procedure.key_ml_per_kg                                 = newConfig.mlPerKg;
   procedure.key_drbc_body_vol                             = newConfig.drbcTbvLimit;
   procedure.key_max_plasma_vol_during_plasma_collection   = newConfig.maxPlasmaVolDuringPlasmaCollection;
   procedure.key_max_plasma_vol_during_platelet_collection = newConfig.maxPlasmaVolDuringPlateletCollection;
   procedure.key_post_plat                                 = newConfig.postProcedurePlateletCount;
   procedure.key_max_draw_flow                             = newConfig.maxDrawFlow;
   procedure.key_min_replacement_volume                    = newConfig.minReplacementVolume;
   procedure.key_plt_mss_split_notif                       = newConfig.plateletDivertPrompts;
   procedure.key_override_pas_bag_vol                      = newConfig.solutionsBagVolOverride;
   procedure.key_blood_diversion                           = newConfig.bloodDivertPrompts;
   procedure.key_drbc_alert                                = newConfig.drbcSplitNotification;
   procedure.key_drbc_threshold                            = newConfig.drbcThreshold;
   procedure.key_plasma_rinseback                          = newConfig.plasmaRinseback;
   procedure.key_saline_rinseback                          = newConfig.salineRinseback;
   procedure.key_audit_tracking                            = newConfig.auditTrailLevel;
   procedure.key_air_removal                               = newConfig.airRemovalEnabled;
   procedure.key_mss_plt_on                                = newConfig.pasEnabled;
   procedure.key_mss_rbc_on                                = newConfig.rasEnabled;
   procedure.key_show_pre_aas_flags                        = newConfig.showPreAasFlags;
   procedure.key_plt_def_bag_vol                           = newConfig.pasDefaultBagVolume;
   procedure.key_rbc_def_bag_vol                           = newConfig.rasDefaultBagVolume;
   procedure.key_autoflow                                  = 0;  // Disable for 6.4, probably turn on for 7.0

   // Set the procedure section.
   _config->Procedure.Set(procedure);
}




//
// This routine copies over the Predict section of the offline configuration
// (config.dat)
//
void VistaConfig::CopyPredictSection (const donorInfoMsg& donorInfo)
{
   PredictionConfig predict;

   // Change the Predict Config
   predict.key_rbc_ratio                = donorInfo.rbcRatio;
   predict.key_rbc_replace_fluid        = donorInfo.rbcReplacementFluid;
   predict.key_rbc_fluid_percent        = donorInfo.rbcFluidPercent;
   predict.key_pls_amap_min             = donorInfo.amapMin;
   predict.key_pls_amap_max             = donorInfo.amapMax;
   predict.key_ysf                      = donorInfo.ysf;
   predict.key_guaranteed_yield         = donorInfo.guaranteedYield;
   predict.key_confidence_level         = donorInfo.confidenceLevel;
   predict.key_coefficient_of_variation = donorInfo.coefficientOfVariation;
   predict.key_inlet_management         = donorInfo.inletManagement;
   predict.key_inlet_flow_ramp          = donorInfo.inletFlowRamp;
   predict.key_return_management        = donorInfo.returnManagement;
   predict.key_rsf                      = donorInfo.rsf;
   predict.key_male_only_plt            = donorInfo.maleOnlyPlatelet;
   predict.key_male_only_plasma         = donorInfo.maleOnlyPlasma;
   predict.key_ffp_volume               = donorInfo.ffpVolume;

   predict.key_complementary_plasma     = _complementaryPlasmaOn ? donorInfo.complementaryPlasma : 0;

   // Set the prediction section.
   _config->Predict.Set(predict);
}



//
// This routine copies over the Predict section of the offline configuration
// (config.dat)
//
void VistaConfig::CopyPredictSection (const setOfflineConfigurationMsg& newConfig)
{
   PredictionConfig predict;

   // Change the Predict Config
   predict.key_rbc_ratio                = newConfig.rbcRatio;
   predict.key_rbc_replace_fluid        = newConfig.rbcReplacementFluid;
   predict.key_rbc_fluid_percent        = newConfig.rbcFluidPercent;
   predict.key_pls_amap_min             = newConfig.amapMin;
   predict.key_pls_amap_max             = newConfig.amapMax;
   predict.key_ysf                      = newConfig.ysf;
   predict.key_guaranteed_yield         = newConfig.guaranteedYield;
   predict.key_confidence_level         = newConfig.confidenceLevel;
   predict.key_coefficient_of_variation = newConfig.coefficientOfVariation;
   predict.key_inlet_management         = newConfig.inletManagement;
   predict.key_inlet_flow_ramp          = newConfig.inletFlowRamp;
   predict.key_return_management        = newConfig.returnManagement;
   predict.key_rsf                      = newConfig.rsf;
   predict.key_male_only_plt            = newConfig.maleOnlyPlatelet;
   predict.key_male_only_plasma         = newConfig.maleOnlyPlasma;
   predict.key_ffp_volume               = newConfig.ffpVolume;
   predict.key_complementary_plasma     = _complementaryPlasmaOn ? newConfig.complementaryPlasma : 0;

   // Set the prediction section.
   _config->Predict.Set(predict);
}




//
// CopyProcedureList
// This routine copies the procedure list for predict.
//
void VistaConfig::CopyProcedureList (const donorInfoMsg& donorInfo)
{
   // Procedure Description Item
   ProductListArray procedureList;
   memset(&procedureList, 0, sizeof(procedureList));

   DataLog(log_level_vip_info) << "Creating " << donorInfo.numberOfProcedures << " Vista procedures" << endmsg;

   for (int i = 0; i < donorInfo.numberOfProcedures; i++)
   {
      procedureList[i].yield              = donorInfo.procedureList[i].plateletYield;
      procedureList[i].platelet_volume    = donorInfo.procedureList[i].plateletVolume;
      procedureList[i].plasma_volume      = donorInfo.procedureList[i].plasmaVolume;
      procedureList[i].rbc_dose           = donorInfo.procedureList[i].rbcDose;
      procedureList[i].rbc_crit           = donorInfo.procedureList[i].rbcHematocrit;
      procedureList[i].max_procedure_time = donorInfo.procedureList[i].maxProcedureTime;
      procedureList[i].blood_type         = (BLOODTYPE)donorInfo.procedureList[i].bloodType;
      procedureList[i].plt_mss            = donorInfo.procedureList[i].meterPAS;
      procedureList[i].pct_carryover      = donorInfo.procedureList[i].pco;
      procedureList[i].rbc_mss            = donorInfo.procedureList[i].meterRas;
      procedureList[i].rbc_mss_volume     = donorInfo.procedureList[i].rasVolume;
      procedureList[i].designator         = donorInfo.procedureList[i].designator;
   }

   // Set procedure list.
   _config->ProcedureList.Procedure.Set(procedureList);
   _config->ProcedureList.numberOfProcedures.Set(donorInfo.numberOfProcedures);
}



//
// CopyProcedureTemplates
//
bool VistaConfig::CopyProcedureTemplates (const setOfflineConfigurationMsg& newConfig)
{
   ProductTemplates       productTemplates;
   ProductDefinitionArray productDefinition;

   float                  min = _amapPltYield.Min();
   float                  max = _amapPltYield.Max();


   for (int i = 0; i < NUM_CRIT_DOSE; i++)
   {
      productTemplates.key_rbc_crit[i]       = newConfig.critDose[i].rbcHematocrit;
      productTemplates.key_rbc_dose[i]       = newConfig.critDose[i].rbcDose;
      productTemplates.key_rbc_mss[i]        = newConfig.critDose[i].meterRas;
      productTemplates.key_rbc_mss_volume[i] = newConfig.critDose[i].rasVolume;

      // Bail out of the array so you don't screw up the error string
      if (newConfig.rasEnabled)
         if (!ValidRasItems(newConfig.critDose[i].meterRas, newConfig.critDose[i].rbcDose, newConfig.critDose[i].rbcHematocrit, newConfig.critDose[i].rasVolume, newConfig.drbcThreshold, newConfig.drbcSplitNotification, i))
            return false;
   }

   // Make a index that goes from top to bottom
   int j = VIP_MAX_AMAP_PLT_PRODUCTS - 1;

   // Tests for AMAP Platelets.
   // Note that to get it all done in 1 pass, there is a going up (min) counter i and a going
   // down counter (max) j.  Don't confuse them!
   //
   // Also note that the internal structure is an array of 6 definitions.
   // In fact,
   //    only Plt 0-2 are used.
   //    only PCO 0-2 are used but they apply to the PPC definitions only.
   //
   for (int i = 0; i < VIP_MAX_AMAP_PLT_PRODUCTS; i++)
   {
      productTemplates.key_plt_amap_min_yield[i]         = newConfig.amapProduct[i].amapPltYieldMin;
      productTemplates.key_plt_amap_max_yield[j]         = newConfig.amapProduct[j].amapPltYieldMax;
      productTemplates.key_plt_amap_pas_pct_carryover[i] = newConfig.amapPltPco[i];

      DataLog(log_level_vip_info) << "Processing entry i=" << i << " j=" << j
                                  << ".  min " <<  newConfig.amapProduct[i].amapPltYieldMin
                                  << ".  max " <<  newConfig.amapProduct[j].amapPltYieldMax
                                  << ".  pco " <<  newConfig.amapPltPco[i]
                                  << ".  MIN " <<  min << ".  MAX " << max << endmsg;

      // Do the range validation on the entries and report if bad
      if (!ValidProcedure (productTemplates.key_plt_amap_min_yield[i], _amapPltYield.Min(), _amapPltYield.Max(), false, true, "AMAP Plt Yield Min", i))
         return false;

      if (!ValidProcedure (productTemplates.key_plt_amap_max_yield[j], _amapPltYield.Min(), _amapPltYield.Max(), false, true, "AMAP Plt Yield Max", j))
         return false;

      if (!ValidProcedure (productTemplates.key_plt_amap_pas_pct_carryover[i], _amapPltPasPco.Min(), _amapPltPasPco.Max(), false, true, "AMAP Plt PAS PCO", i))
         return false;

      // Ensure increasing mins
      if (productTemplates.key_plt_amap_min_yield[i] < min)
      {
         _errorMsg << "AMAP Plt Min Yield " << productTemplates.key_plt_amap_min_yield[i] << " ["
                   << i << "] is below the expected min of " << min << std::ends;
         return false;
      }
      else
      {
         // Make the min just a little bit bigger
         min = newConfig.amapProduct[i].amapPltYieldMax + 0.1;
      }

      // Ensure decreasing maxs
      if (productTemplates.key_plt_amap_max_yield[j] > max)
      {
         _errorMsg << "AMAP Plt Max Yield " << productTemplates.key_plt_amap_max_yield[j]
                   << " index [" << j << "] " << " is above the expected max of " << max << std::ends;
         return false;
      }
      else
      {
         // Make the max just a little bit smaller
         max = newConfig.amapProduct[j].amapPltYieldMin - 0.1;
      }

      // Ensure consistent item range
      if (newConfig.amapProduct[i].amapPltYieldMin >= newConfig.amapProduct[i].amapPltYieldMax)
      {
         _errorMsg << "AMAP Plt Yield [" << i << "] range is incorrrect.  Min " << newConfig.amapProduct[i].amapPltYieldMin
                   << ". Max " << newConfig.amapProduct[i].amapPltYieldMax << "." << std::ends;

         return false;
      }

      j--;
   }

   // The AMAP Plt concentrations are straight forward.  Fill them in accordingly
   for (int i = 0; i < VIP_MAX_AMAP_PLT_CONCENTRATIONS; i++)
   {
      productTemplates.key_plt_amap_conc[i] = newConfig.amapPltConcentration[i];

      if (!ValidProcedure (productTemplates.key_plt_amap_conc[i], _amapPltConcentration.Min(), _amapPltConcentration.Max(), false, true, "AMAP Plt Concentration", i))
         return false;
   }

   for (int i = 0; i < NUM_YIELD_VOLUME; i++)
   {
      productTemplates.key_plt_yield[i]         = newConfig.plateletYV[i].plateletYield;
      productTemplates.key_plt_volume[i]        = newConfig.plateletYV[i].plateletVolume;
      productTemplates.key_plt_mss[i]           = newConfig.plateletYV[i].meterPAS;
      productTemplates.key_plt_pct_carryover[i] = newConfig.plateletYV[i].pco;

      // Bail out of the array so you don't screw up the error string
      if (newConfig.pasEnabled)
         if (!ValidPasItems(newConfig.plateletYV[i].meterPAS, newConfig.plateletYV[i].plateletVolume, newConfig.plateletYV[i].pco, i))
            return false;
   }

   for (int i = 0; i < NUM_PLASMA_VOLUME; i++)
   {
      productTemplates.key_pls_volume[i] = newConfig.plasmaVolume[i];
   }

   // Change the Product Definition Config
   for (int i = 0; i < NUM_OFFLINE_PROCEDURES; i++)
   {
      productDefinition[i].key_platelet      = newConfig.procedureList[i].plateletIndex;
      productDefinition[i].key_platelet_amap = newConfig.procedureList[i].amapPlateletIndex;
      productDefinition[i].key_plasma        = newConfig.procedureList[i].plasmaIndex;
      productDefinition[i].key_rbc           = newConfig.procedureList[i].rbcIndex;
      productDefinition[i].key_blood_type    = newConfig.procedureList[i].bloodType;
      productDefinition[i].key_time          = newConfig.procedureList[i].maxProcedureTime;
   }

   _config->Templates.Set(productTemplates);
   _config->Definition.Set(productDefinition);

   return true;
}



//
// ValidateVistaProcedureSelection
//
int VistaConfig::ValidateVistaProcedureSection (const donorInfoMsg& donorInfo)
{
   _errorMsg.str(string());  // Clear the string

   if ( (donorInfo.numberOfProcedures < 1) || (donorInfo.numberOfProcedures > VIP_MAX_VISTA_PROCEDURES) )
   {
      // Log event for later debugging.
      _errorMsg << "Invalid number of procedures, value=" << donorInfo.numberOfProcedures
                << ", expected (1-" << NUM_VISTA_PROCEDURES << ")" << std::ends;

      return VIP_BAD_VALUE;
   }

   DataLog(log_level_vip_debug) << "Validating " << donorInfo.numberOfProcedures << " procedures." << endmsg;

   // Validate the list of procedures.
   for (int i = 0; i < donorInfo.numberOfProcedures; i++)
   {
      const procedureGoalType& proc = donorInfo.procedureList[i];

      DataLog(log_level_vip_debug) << "Testing procedure " << i << " = "
                                   << proc.plateletYield << "(" << proc.plateletVolume << ")(" << proc.pco << ")" << " "
                                   << proc.plasmaVolume << " "
                                   << proc.rbcDose << "(" << proc.rbcHematocrit << ")(" << proc.rasVolume << ") "
                                   << proc.meterPAS << " " << proc.meterRas << endmsg;

      if (!ValidProcedure (proc.plateletYield, _pltYield.Min(), _pltYield.Max(), false, true, "platelet yield", i))
         return VIP_BAD_VALUE;

      if (!ValidProcedure (proc.plateletVolume, _pltVolume.Min(), _pltVolume.Max(), true, true, "platelet volume", i))
         return VIP_BAD_VALUE;

      if (!ValidProcedure (proc.meterPAS, _meterPAS.Min(), _meterPAS.Max(), true, true, "meter PAS", i))
         return VIP_BAD_VALUE;

      if (!ValidProcedure (proc.pco, _pco.Min(), _pco.Max(), true, true, "PCO", i))
         return VIP_BAD_VALUE;

      // For Plasma, they can have AMAP or MC (-1 and -2) so allow them as values.
      if (!ValidProcedure (proc.plasmaVolume, -2.0, _plsVolume.Max(), false, true, "plasma volume", i))
         return VIP_BAD_VALUE;

      if (!ValidProcedure (proc.rbcDose, _rbcDose.Min(), _rbcDose.Max(), true, true, "rbc dose", i))
         return VIP_BAD_VALUE;

      if (!ValidProcedure (proc.rbcHematocrit, _rbcCrit.Min(), _rbcCrit.Max(), false, true, "rbc hematocrit", i))
         return VIP_BAD_VALUE;

      if (!ValidProcedure (proc.meterRas, _rbcMss.Min(), _rbcMss.Max(), true, true, "meter RAS", i))
         return VIP_BAD_VALUE;

      if (!ValidProcedure (proc.rasVolume, _rbcMssVolume.Min(), _rbcMssVolume.Max(), true, true, "RAS volume", i))
         return VIP_BAD_VALUE;

      if (!ValidProcedure (proc.bloodType, BT_UNKNOWN, BT_ALL, true, false, "blood type", i))
         return VIP_BAD_VALUE;

      if (!ValidProcedure (proc.maxProcedureTime, _maxProcTime.Min(), _maxProcTime.Max(), false, true, "max proc time", i))
         return VIP_BAD_VALUE;

      if (!ValidProcedure (proc.designator, 1, 2, false, true, "proc designator", i))
         return VIP_BAD_VALUE;

      // Special tests for RAS products
      if (donorInfo.rasEnabled)
         if (!ValidRasItems(proc.meterRas, proc.rbcDose, proc.rbcHematocrit, proc.rasVolume, donorInfo.drbcThreshold, donorInfo.drbcSplitNotification, i))
            return VIP_BAD_VALUE;

      // Special tests for PAS products
      if (donorInfo.pasEnabled)
         if (!ValidPasItems(proc.meterPAS, proc.plateletVolume, proc.pco, i))
            return VIP_BAD_VALUE;

   }

   return 0;
}



//
// Helper routine that validates some specific items for RAS
// Checks include:
//  * forcing 80 crit for RAS procedures
//  * checking 160 minimum for DRBC min RAS volume
//  * check for max bag limit
//  * check for RAS enabled and Split Notify off
//
bool VistaConfig::ValidRasItems (bool meteringRas, float rbcDose, float rbcCrit, int rasVolume, int drbcThreshold, bool splitAlert, int index)
{
   const int   minDRBCRasVol   = 2 * (int)_rbcMssVolume.Min();

   const float fixedRBCPtfCrit = CCfgVarInfo(KEY_RBC_CRIT_1).Max();
   const float totalRBCVolume  = rasVolume + (rbcDose / (rbcCrit / 100.0f));

   _errorMsg.str(string());  // Clear the string

   // Special check to make sure crit is forced to 80 if doing RAS.
   if (meteringRas && rbcCrit != fixedRBCPtfCrit)
   {
      _errorMsg << "Procedure " << index << " invalid.  RAS enabled and RBC target crit is not set to "
                << fixedRBCPtfCrit << std::ends;

      return false;
   }

   // Special test to enforce RBC bag limits
   if (meteringRas && (totalRBCVolume >= (float)_rbcMaxProductVolume))
   {
      _errorMsg << "Procedure " << index << " invalid.  RBC bag volume exceeded.  " << totalRBCVolume
                << " exceeds " << _rbcMaxProductVolume << std::ends;

      return false;
   }

   // Special test to enforce 160 mls for RAS if product is a DRBC
   if (meteringRas && (rbcDose > drbcThreshold) &&
       (rasVolume < minDRBCRasVol) )
   {
      _errorMsg << "Procedure " << index << " invalid.  RAS volume of " << rasVolume
                << " does not meet minimum of " << minDRBCRasVol << std::ends;

      return false;
   }

   // Test to enforce RAS on and Split On
   if (meteringRas && !splitAlert)
   {
      _errorMsg << "Procedure " << index << " invalid.  DRBC Split Notification alert must be activated" << std::ends;

      return false;
   }

   return true;
}


//
// Helper routine that validates some specific items for RAS
// Checks include:
//  * check for max bag limit
//
bool VistaConfig::ValidPasItems (bool meteringPas, float plateletVolume, int pco, int index)
{
   const float percentPlasma  = (float)pco / 100.0f;
   const float pasVol         = plateletVolume * (( 1 - percentPlasma ) / percentPlasma);
   const float totalPltVolume = plateletVolume + pasVol;

   _errorMsg.str(string());  // Clear the string

   // Special test to enforce Plt bag limits
   if (meteringPas && (totalPltVolume > CobeConfig::data().MaxPlateletBagVolume))
   {
      _errorMsg << "Procedure " << index << " invalid.  Plt bag volume " << totalPltVolume
                << " exceeds " << CobeConfig::data().MaxPlateletBagVolume
                << ".  Percent Plasma " << percentPlasma << ", PasVol "
                << pasVol << ", totalPltVol " << totalPltVolume << std::ends;

      return false;
   }

   DataLog(log_level_vip_debug) << "Procedure " << index << " Plt bag volume " << totalPltVolume
                                << ".  Percent Plasma " << percentPlasma << ", PasVol "
                                << pasVol << ", totalPltVol " << totalPltVolume << endmsg;

   return true;
}



//
// This routine validates a trima procedure
// TBD.  Fix this.  Improve the config class so it doesn't have to be done here.
//
bool VistaConfig::ValidProcedure (float value, float lowRange, float highRange, bool allowZero, bool allowOdd, const char* s, int index)
{
   _errorMsg.str(string());  // Clear the string

   if (notInRange (value, lowRange, highRange))
   {
      if (allowZero && (value == 0.0f))
      {
         return true;
      }
      else
      {
         _errorMsg << "Procedure " << index << " Invalid " << s << ", value=" << value
                   << ", Range " << lowRange << ".." << highRange << std::ends;

         return false;
      }
   }
   else
   {
      // Do odd number check
      if (!allowOdd && ((int)value & 0x001))
      {
         _errorMsg << "Procedure " << index << " contains an odd number " << value << std::ends;
         return false;
      }
      else
      {
         return true;
      }
   }
}



//
// Routine to validate the maximum donor time.
// Range is the same as max procedure time.
// TBD.  Fix this.  Improve the config class so it doesn't have to be done here.
//
bool VistaConfig::ValidMaxDonorTime (int value)
{
   _errorMsg.str(string());  // Clear the string

   if (notInRange (value, (int)_maxProcTime.Min(), (int)_maxProcTime.Max()))
   {
      _errorMsg << "Max Donor Time value invalid, value=" << value << ", Range "
                << _maxProcTime.Min() << ".." << _maxProcTime.Max() << std::ends;

      return false;
   }
   else
   {
      return true;
   }
}



//
// This helper routine does a range check on a value given the upper and lower bound constraints.
//
bool VistaConfig::notInRange (int value, int lowerRange, int upperRange)
{
   if (value < lowerRange)
   {
      return true;
   }
   else if (value > upperRange)
   {
      return true;
   }

   return false;
}



//
// This helper routine does a range check on a value given the upper and lower bound constraints.
//
bool VistaConfig::notInRange (float value, float lowerRange, float upperRange)
{
   if (value < lowerRange)
   {
      return true;
   }
   else if (value > upperRange)
   {
      return true;
   }

   return false;
}



//
// Snapshot
//
void VistaConfig::Snapshot ()
{
   // Store Snapshot data
   _config->LangUnit.Get(&_lang);
   _config->Machine.Get(&_machine);
   _config->Procedure.Get(&_procedure);
   _config->Predict.Get(&_predict);
   _config->Templates.Get(&_productTemplates);
   _config->Definition.Get(&_productDefinition);
   _config->ProcedureList.Procedure.Get(&_procedureList);
   _numberOfProcedures = _config->ProcedureList.numberOfProcedures.Get();
}



//
// Rollback
//
void VistaConfig::Rollback ()
{
   // Restore the config
   _config->LangUnit.Set(_lang);
   _config->Machine.Set(_machine);
   _config->Procedure.Set(_procedure);
   _config->Predict.Set(_predict);
   _config->Templates.Set(_productTemplates);
   _config->Definition.Set(_productDefinition);
   _config->ProcedureList.numberOfProcedures.Set(_numberOfProcedures);
   _config->ProcedureList.Procedure.Set(_procedureList);
}

/* FORMAT HASH 432c4568400352d2cc5e7401116f3596 */
