/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      cfgids.h
 *             Enumerations to identify config.dat items
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

/**********************************************************************************
 NOTE: This file is now shared with Vista. Take care when adding new dependencies
 **********************************************************************************/

#if !defined( INCLUDE_CFGIDS )
#define INCLUDE_CFGIDS

/////March 18 1999 - AISchreiber////////////////////////////////////
// Global defines for flag values concerning the difference between
// AMAP and Optimal Plasma.  Used in Procedure (proc), modules:
// optimizer and run_init.  (RUN INIT switches off of PLASMA_AMAP enum
// defined in this header file).
#define PLASMA_AMAP_FLAG           -1.0f
#define PLASMA_MAX_CONCURRENT_FLAG -2.0f
#define PLASMA_FFP_FLAG            -3.0f


// Codes to map GUI listbox selections to product templates


// Variable types
// config.dat sections
// NB: Adjustments are included in the config data but are never written to
//     disk.

enum CONFIG_SECTION_NAMES
{
   LANGUAGE_UNIT_CONFIG,
   PROCEDURE_CONFIG,
   PREDICTION_CONFIG,
   PRODUCT_TEMPLATES,
   PRODUCT_DEFINITIONS,
   BASIC_MACHINE_CONFIG,
   NUM_SECTIONS
};

enum CONFIG_DATA_TYPE
{
   STRING_TYPE    =0,
   INT_TYPE       =1,
   FLOAT_0DEC_TYPE=2,
   FLOAT_1DEC_TYPE=3,
   FLOAT_2DEC_TYPE=4,
   TIME_TYPE      =5,
   DATE_TYPE      =6,
   INACTIVE_TYPE  =7,
   NULL_TYPE      = 99
};

enum VOLUME_REMOVAL_TYPE // for key_tbv_volume_setting
{
   VOLUME_REMOVAL_TBV           = 1,
   VOLUME_REMOVAL_WEIGHT        = 2,
   VOLUME_REMOVAL_ML_PER_KG     = 4,
   VOLUME_REMOVAL_DRBC_BODY_VOL = 8,
   VOLUME_REMOVAL_PLATELET_VOL  = 16,
   VOLUME_REMOVAL_PLASMA_VOL    = 32
};
// individual config data value ids

enum CONFIG_DATA_KEYS
{
   // LANGUAGE/UNITS SCREEN ENTRIES
   KEY_LANG,
   KEY_HEIGHT,
   KEY_WEIGHT,
   KEY_DATE_FORMAT,
   KEY_TIME_FORMAT,
   KEY_DECIMAL_DELIMITER,
   KEY_CRIT_OR_GLOB,

   // PROCEDURE SCREEN ENTRIES
   KEY_PROC_TIME,
   KEY_RETURN_PRESS,
   KEY_DRAW_PRESS,
   KEY_CUSTOM_RATIO,
   KEY_RBC_RATIO,
   KEY_PLASMA_RATIO,
   KEY_PLASMA_RINSEBACK,
   KEY_SALINE_RINSEBACK,
   KEY_AUDIT_TRACKING,
   KEY_AIR_REMOVAL,

   // DONOR SCREEN
   KEY_AC_RATE,
   KEY_POST_CRIT,
   KEY_POST_PLAT,
   KEY_MAX_DRAW_FLOW,
   KEY_MIN_REPLACEMENT_VOLUME,
   KEY_PLT_MSS_SPLIT_NOTIF,
   KEY_OVERRIDE_PAS_BAG_VOL,
   KEY_BLOOD_DIVERSION,
   KEY_RBC_REPLACE_FLUID,
   KEY_RBC_FLUID_PERCENT,
   KEY_DRBC_ALERT,
   KEY_DRBC_THRESHOLD,
   KEY_YSF,
   KEY_GUARANTEED_YIELD,
   KEY_GUARANTEED_YIELD_CL,
   KEY_GUARANTEED_YIELD_CV,
   KEY_INLET_MANAGEMENT,
   KEY_INLET_FLOW_RAMP,
   KEY_RETURN_MANAGEMENT,
   KEY_RSF,
   KEY_MALE_ONLY_PLT,
   KEY_COMPLEMENTARY_PLASMA,

   // DONOR VOLUME SCREEN
   KEY_TBV_VOL_SETTING,
   KEY_TBV_PERCENT,
   KEY_AUTOFLOW,


#if 1
   KEY_WEIGHT_SETTING,
   KEY_WEIGHT_LESS_THAN_VOL,
   KEY_WEIGHT_GREATER_THAN_VOL,
#else
   KEY_WEIGHT_1_SETTING,
   KEY_WEIGHT_2_SETTING,
   KEY_WEIGHT_3_SETTING,
   KEY_WEIGHT_1_LESS_THAN_VOL,
   KEY_WEIGHT_2_LESS_THAN_VOL,
   KEY_WEIGHT_3_LESS_THAN_VOL,
   KEY_WEIGHT_3_GREATER_THAN_VOL,
#endif

   KEY_ML_PER_KG,
   KEY_DRBC_BODY_VOL,

   KEY_MAX_PLASMA_DURING_PLT_VOL,
   KEY_MAX_PLASMA_DURING_PLS_VOL,

   // PLASMA SCREEN
   KEY_PLS_VOLUME_1,
   KEY_PLS_VOLUME_2,
   KEY_PLS_VOLUME_3,
   KEY_PLS_VOLUME_4,
   KEY_PLS_VOLUME_5,
   KEY_PLS_VOLUME_6,
   KEY_PLASMA_AMAP_MINIMUM,
   KEY_PLASMA_AMAP_MAXIMUM,
   KEY_MALE_ONLY_PLASMA,
   KEY_FFP_VOLUME,

   // RBC SCREEN
   KEY_RBC_CRIT_1,
   KEY_RBC_DOSE_1,
   KEY_RBC_MSS_VOLUME_1,
   KEY_RBC_MSS_1,

   KEY_RBC_CRIT_2,
   KEY_RBC_DOSE_2,
   KEY_RBC_MSS_VOLUME_2,
   KEY_RBC_MSS_2,

   KEY_RBC_CRIT_3,
   KEY_RBC_DOSE_3,
   KEY_RBC_MSS_VOLUME_3,
   KEY_RBC_MSS_3,

   KEY_RBC_CRIT_4,
   KEY_RBC_DOSE_4,
   KEY_RBC_MSS_VOLUME_4,
   KEY_RBC_MSS_4,

   KEY_RBC_CRIT_5,
   KEY_RBC_DOSE_5,
   KEY_RBC_MSS_VOLUME_5,
   KEY_RBC_MSS_5,

   KEY_RBC_CRIT_6,
   KEY_RBC_DOSE_6,
   KEY_RBC_MSS_VOLUME_6,
   KEY_RBC_MSS_6,

   // PLATELET SCREEN
   KEY_PLT_YIELD_1,
   KEY_PLT_VOLUME_1,
   KEY_PLT_MSS_1,
   KEY_PLT_PCT_CARRYOVER_1,

   KEY_PLT_YIELD_2,
   KEY_PLT_VOLUME_2,
   KEY_PLT_MSS_2,
   KEY_PLT_PCT_CARRYOVER_2,

   KEY_PLT_YIELD_3,
   KEY_PLT_VOLUME_3,
   KEY_PLT_MSS_3,
   KEY_PLT_PCT_CARRYOVER_3,

   KEY_PLT_YIELD_4,
   KEY_PLT_VOLUME_4,
   KEY_PLT_MSS_4,
   KEY_PLT_PCT_CARRYOVER_4,

   KEY_PLT_YIELD_5,
   KEY_PLT_VOLUME_5,
   KEY_PLT_MSS_5,
   KEY_PLT_PCT_CARRYOVER_5,

   KEY_PLT_YIELD_6,
   KEY_PLT_VOLUME_6,
   KEY_PLT_MSS_6,
   KEY_PLT_PCT_CARRYOVER_6,

   KEY_PLT_YIELD_7,
   KEY_PLT_VOLUME_7,
   KEY_PLT_MSS_7,
   KEY_PLT_PCT_CARRYOVER_7,

   KEY_PLT_YIELD_8,
   KEY_PLT_VOLUME_8,
   KEY_PLT_MSS_8,
   KEY_PLT_PCT_CARRYOVER_8,

   KEY_PLT_YIELD_9,
   KEY_PLT_VOLUME_9,
   KEY_PLT_MSS_9,
   KEY_PLT_PCT_CARRYOVER_9,

   KEY_PLT_YIELD_10,
   KEY_PLT_VOLUME_10,
   KEY_PLT_MSS_10,
   KEY_PLT_PCT_CARRYOVER_10,

   KEY_PLT_AMAP_MIN_YIELD_1,
   KEY_PLT_AMAP_MAX_YIELD_1,
   KEY_PLT_AMAP_CONC_1,
   KEY_PLT_AMAP_CONC_4,
   KEY_PLT_AMAP_PAS_CARRYOVER_1,

   KEY_PLT_AMAP_MIN_YIELD_2,
   KEY_PLT_AMAP_MAX_YIELD_2,
   KEY_PLT_AMAP_CONC_2,
   KEY_PLT_AMAP_CONC_5,
   KEY_PLT_AMAP_PAS_CARRYOVER_2,

   KEY_PLT_AMAP_MIN_YIELD_3,
   KEY_PLT_AMAP_MAX_YIELD_3,
   KEY_PLT_AMAP_CONC_3,
   KEY_PLT_AMAP_CONC_6,
   KEY_PLT_AMAP_PAS_CARRYOVER_3,




   // Product definitions
   KEY_PRI_PLATELET_A,
   KEY_PRI_PLATELET_AMAP_A,
   KEY_PRI_PLASMA_A,
   KEY_PRI_RBC_A,
   KEY_PRI_BLOOD_TYPE_A,
   KEY_PRI_TIME_A,

   KEY_PRI_PLATELET_B,
   KEY_PRI_PLATELET_AMAP_B,
   KEY_PRI_PLASMA_B,
   KEY_PRI_RBC_B,
   KEY_PRI_BLOOD_TYPE_B,
   KEY_PRI_TIME_B,

   KEY_PRI_PLATELET_C,
   KEY_PRI_PLATELET_AMAP_C,
   KEY_PRI_PLASMA_C,
   KEY_PRI_RBC_C,
   KEY_PRI_BLOOD_TYPE_C,
   KEY_PRI_TIME_C,

   KEY_PRI_PLATELET_D,
   KEY_PRI_PLATELET_AMAP_D,
   KEY_PRI_PLASMA_D,
   KEY_PRI_RBC_D,
   KEY_PRI_BLOOD_TYPE_D,
   KEY_PRI_TIME_D,

   KEY_PRI_PLATELET_E,
   KEY_PRI_PLATELET_AMAP_E,
   KEY_PRI_PLASMA_E,
   KEY_PRI_RBC_E,
   KEY_PRI_BLOOD_TYPE_E,
   KEY_PRI_TIME_E,

   KEY_PRI_PLATELET_F,
   KEY_PRI_PLATELET_AMAP_F,
   KEY_PRI_PLASMA_F,
   KEY_PRI_RBC_F,
   KEY_PRI_BLOOD_TYPE_F,
   KEY_PRI_TIME_F,

   KEY_PRI_PLATELET_G,
   KEY_PRI_PLATELET_AMAP_G,
   KEY_PRI_PLASMA_G,
   KEY_PRI_RBC_G,
   KEY_PRI_BLOOD_TYPE_G,
   KEY_PRI_TIME_G,

   KEY_PRI_PLATELET_H,
   KEY_PRI_PLATELET_AMAP_H,
   KEY_PRI_PLASMA_H,
   KEY_PRI_RBC_H,
   KEY_PRI_BLOOD_TYPE_H,
   KEY_PRI_TIME_H,

   KEY_PRI_PLATELET_I,
   KEY_PRI_PLATELET_AMAP_I,
   KEY_PRI_PLASMA_I,
   KEY_PRI_RBC_I,
   KEY_PRI_BLOOD_TYPE_I,
   KEY_PRI_TIME_I,

   KEY_PRI_PLATELET_J,
   KEY_PRI_PLATELET_AMAP_J,
   KEY_PRI_PLASMA_J,
   KEY_PRI_RBC_J,
   KEY_PRI_BLOOD_TYPE_J,
   KEY_PRI_TIME_J,

   KEY_PRI_PLATELET_K,
   KEY_PRI_PLATELET_AMAP_K,
   KEY_PRI_PLASMA_K,
   KEY_PRI_RBC_K,
   KEY_PRI_BLOOD_TYPE_K,
   KEY_PRI_TIME_K,

   KEY_PRI_PLATELET_L,
   KEY_PRI_PLATELET_AMAP_L,
   KEY_PRI_PLASMA_L,
   KEY_PRI_RBC_L,
   KEY_PRI_BLOOD_TYPE_L,
   KEY_PRI_TIME_L,

   KEY_PRI_PLATELET_M,
   KEY_PRI_PLATELET_AMAP_M,
   KEY_PRI_PLASMA_M,
   KEY_PRI_RBC_M,
   KEY_PRI_BLOOD_TYPE_M,
   KEY_PRI_TIME_M,

   KEY_PRI_PLATELET_N,
   KEY_PRI_PLATELET_AMAP_N,
   KEY_PRI_PLASMA_N,
   KEY_PRI_RBC_N,
   KEY_PRI_BLOOD_TYPE_N,
   KEY_PRI_TIME_N,

   KEY_PRI_PLATELET_O,
   KEY_PRI_PLATELET_AMAP_O,
   KEY_PRI_PLASMA_O,
   KEY_PRI_RBC_O,
   KEY_PRI_BLOOD_TYPE_O,
   KEY_PRI_TIME_O,

   KEY_PRI_PLATELET_P,
   KEY_PRI_PLATELET_AMAP_P,
   KEY_PRI_PLASMA_P,
   KEY_PRI_RBC_P,
   KEY_PRI_BLOOD_TYPE_P,
   KEY_PRI_TIME_P,

   KEY_PRI_PLATELET_Q,
   KEY_PRI_PLATELET_AMAP_Q,
   KEY_PRI_PLASMA_Q,
   KEY_PRI_RBC_Q,
   KEY_PRI_BLOOD_TYPE_Q,
   KEY_PRI_TIME_Q,

   KEY_PRI_PLATELET_R,
   KEY_PRI_PLATELET_AMAP_R,
   KEY_PRI_PLASMA_R,
   KEY_PRI_RBC_R,
   KEY_PRI_BLOOD_TYPE_R,
   KEY_PRI_TIME_R,

   KEY_PRI_PLATELET_S,
   KEY_PRI_PLATELET_AMAP_S,
   KEY_PRI_PLASMA_S,
   KEY_PRI_RBC_S,
   KEY_PRI_BLOOD_TYPE_S,
   KEY_PRI_TIME_S,

   KEY_PRI_PLATELET_T,
   KEY_PRI_PLATELET_AMAP_T,
   KEY_PRI_PLASMA_T,
   KEY_PRI_RBC_T,
   KEY_PRI_BLOOD_TYPE_T,
   KEY_PRI_TIME_T,

   // MACHINE SCREEN
   KEY_SET_TIME,
   KEY_SET_DATE,
   KEY_SET_AUDIO,
   KEY_MACH_CATALOG_NUM,
   KEY_MACH_ADMIN_CODE,
   KEY_MACH_CONFIRM_DELETE_ONE,
   KEY_MACH_CONFIRM_DELETE_ALL,

   KEY_RAS_CONNECT_CONFIRM,
   KEY_PAS_CONNECT_CONFIRM,
   KEY_SALINE_CONNECT_CONFIRM,
   KEY_AC_CONNECT_CONFIRM,
   KEY_REBOOT_CONFIRM,

   // PASSWORD
   KEY_PWORD,

   // Metered Solution configs
   KEY_MSS_PLT_ON,
   KEY_MSS_RBC_ON,

   KEY_SHOW_PRE_AAS_FLAGS,

   KEY_RBC_DEF_BAG_VOL,
   KEY_PLT_DEF_BAG_VOL,
   // KEY_PLT_VOL_TYPE,

   // User Cassette ID entry screen
   KEY_CASSETTE_LOT_ID,

   //  The following keys are not represented in the actual configuration file.
   KEY_PRI_PLATELET_LIST,          // dummy for listbox
   KEY_PRI_PLATELET_AMAP_LIST,     // dummy for listbox
   KEY_PRI_PLATELET_AMAP_PAS_LIST, // dummy for listbox
   KEY_PRI_PLASMA_LIST,            // dummy for listbox
   KEY_PRI_RBC_LIST,               // dummy for listbox
   KEY_PRI_BLOOD_TYPE_LIST,        // dummy for listbox

   KEY_RBC_VOLUME_1,
   KEY_RBC_VOLUME_2,
   KEY_RBC_VOLUME_3,
   KEY_PLT_CONC_1,
   KEY_PLT_CONC_2,
   KEY_PLT_CONC_3,
   KEY_PLT_CONC_4,
   KEY_PLT_CONC_5,
   KEY_PLT_CONC_6,

   KEY_INACTIVE,  // for configuration screen use

   LAST_CONFIG_INDEX_KEY

};

enum CONFIG_DATA_LISTBOX_STRINGS
{
   // LANGUAGE LIST
   ENGLISH_LANG   =0,
   FRENCH_LANG    =1,
   GERMAN_LANG    =2,
   ITALIAN_LANG   =3,
   JAPAN_LANG     =4,
   SPANISH_LANG   =5,
   DUTCH_LANG     =6,
   PORTUGUESE_LANG=7,
   SWEDISH_LANG   =8,
   KOREAN_LANG    =9,
   RUSSIAN_LANG   =10,
   CZECH_LANG     =11,
   POLISH_LANG    =12,
   CHINESE_LANG   =13,

   // HEIGHT LIST
   CENTI_HEIGHT=0,
   FT_HEIGHT   =1,

   // WEIGHT LIST
   KG_WEIGHT=0,
   LB_WEIGHT=1,

   // DATE LIST
   MMDDYY_DATE=0,
   DDMMYY_DATE=1,
   YYMMDD_DATE=2,

   // TIME FORMAT LIST
   TWENTYFOUR_TIME_FORMAT=0,
   STANDARD_TIME_FORMAT  =1,

   // DECIMAL DELIMITER LIST
   COMMA_DELIMIT  =0,
   DECIMAL_DELIMIT=1,

   // RBC UNIT (CRIT OR HEMOGLOBIN)
   RBC_UNIT_CRIT      =0,
   RBC_UNIT_HEMOGLOBIN=1,

   // Donor Screen 1 to 6 lists
   LIST_SELECT_1=1,
   LIST_SELECT_2=2,
   LIST_SELECT_3=3,
   LIST_SELECT_4=4,
   LIST_SELECT_5=5,
   LIST_SELECT_6=6,

   // Procedure Priority Screen lists
   PLATELET_NONE  =0,
   PLATELET_1     =1,
   PLATELET_2     =2,
   PLATELET_3     =3,
   PLATELET_4     =4,
   PLATELET_5     =5,
   PLATELET_6     =6,
   PLATELET_7     =7,
   PLATELET_8     =8,
   PLATELET_9     =9,
   PLATELET_10    =10,

   PLASMA_NONE    =0,
   PLASMA_1       =1,
   PLASMA_2       =2,
   PLASMA_3       =3,
   PLASMA_4       =4,
   PLASMA_5       =5,
   PLASMA_6       =6,
   PLASMA_AMAP    =7,
   PLASMA_MAX_CONC=8,
   PLASMA_FFP     =9,

   RBC_NONE       =0,
   RBC_1          =1,
   RBC_2          =2,
   RBC_3          =3,
   RBC_4          =4,
   RBC_5          =5,
   RBC_6          =6,

   // The constants below are to allow translation of blood types for
   // configuration use
   CONFIG_INDEX_BLOOD_TYPE_ALL                 = 0,
   CONFIG_INDEX_BLOOD_TYPE_OP_ON               = 1,
   CONFIG_INDEX_BLOOD_TYPE_ON                  = 2,
   CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN         = 3,
   CONFIG_INDEX_BLOOD_TYPE_OP_ON_AN            = 4,
   CONFIG_INDEX_BLOOD_TYPE_ON_AN               = 5,
   CONFIG_INDEX_BLOOD_TYPE_OP_ON_BP_BN         = 6,
   CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN_BP_BN   = 7,
   CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN_BN      = 8,
   CONFIG_INDEX_BLOOD_TYPE_AP_AN               = 9,
   CONFIG_INDEX_BLOOD_TYPE_AP_AN_BP_BN_ABP_ABN = 10,
   CONFIG_INDEX_BLOOD_TYPE_BP_BN               = 11,
   CONFIG_INDEX_BLOOD_TYPE_ABP_ABN             = 12,


   // These constants are used to allow translation for max draw flow
   DRAW_FLOW_SLOW     = 0,
   DRAW_FLOW_MODERATE = 1,
   DRAW_FLOW_FAST     = 2,

   // Machine Screen Audio List
   AUDIO_HIGH=0,
   AUDIO_MED =1,
   AUDIO_LOW =3,

   // RBC Screen RBC ratio list
   RBC_814   =0,
   RBC_11    =1,
   CONFIG_NO =0,
   CONFIG_YES=1,

   // Male-only plt/plasma options
   MALE_ONLY_OPTION_MALE_ONLY           =0,
   MALE_ONLY_OPTION_MALES_AND_AB_FEMALES=1,
   MALE_ONLY_OPTION_ALL                 =2,

   // Procedure Screen audit tracking level
   TRACKING_OFF                  =0,
   TRACKING_LEVEL_1              =1,
   TRACKING_LEVEL_2              =2,
   TRACKING_LEVEL_3              =3,

   PLT_MSS_CALC_PERCENT_CARRYOVER=0,
   PLT_MSS_CALC_FIXED_VOL        =1,

   LAST_STRING_INDEX             =999,
};

#endif

/* FORMAT HASH 23941d1e21ac4677cd956e3371f94112 */
