/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  no classes, just structure allocations

*     File name:   cfg_kstr.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/
#include <vxworks.h>

/** include files **/

#include "guiglobs.hpp"
#include "cfg_ids.h"
#include "cfg_kstr.hpp"
#include "run_defs.h"


//
///////////////////////////////////////////////////////////////////////////////
//



THE_CONFIG_LIST_STRUC config_list_table[] =
{
   {KEY_HEIGHT, CENTI_HEIGHT, textListboxItemCfgHeightMetric},
   {KEY_HEIGHT, FT_HEIGHT,    textListboxItemCfgHeightEnglish},

   {KEY_WEIGHT, KG_WEIGHT, textListboxItemCfgWeightKg},
   {KEY_WEIGHT, LB_WEIGHT, textListboxItemCfgWeightLb},

   {KEY_DATE_FORMAT, MMDDYY_DATE, textListboxItemCfgDateMonth},
   {KEY_DATE_FORMAT, DDMMYY_DATE, textListboxItemCfgDateYear},
   {KEY_DATE_FORMAT, YYMMDD_DATE, textListboxItemCfgDateDay},

   {KEY_TIME_FORMAT, TWENTYFOUR_TIME_FORMAT, textListboxItemCfgTimeFmt24},
   {KEY_TIME_FORMAT, STANDARD_TIME_FORMAT,   textListboxItemCfgTimeFmt12},

   {KEY_DECIMAL_DELIMITER, COMMA_DELIMIT,   textListboxItemCfgDecDelimComma},
   {KEY_DECIMAL_DELIMITER, DECIMAL_DELIMIT, textListboxItemCfgDecDelimDec},

   {KEY_CRIT_OR_GLOB, RBC_UNIT_CRIT,       textListboxItemCfgRbcUnitsCrit},
   {KEY_CRIT_OR_GLOB, RBC_UNIT_HEMOGLOBIN, textListboxItemCfgRbcUnitsHemoglobin},

   // Donor Screen
   {KEY_AC_RATE,         LIST_SELECT_1,      textButtonKeypad1},   // textListboxItemCfgAcRate1
   {KEY_AC_RATE,         LIST_SELECT_2,      textButtonKeypad2},   // textListboxItemCfgAcRate2
   {KEY_AC_RATE,         LIST_SELECT_3,      textButtonKeypad3},   // textListboxItemCfgAcRate3
   {KEY_AC_RATE,         LIST_SELECT_4,      textButtonKeypad4},   // textListboxItemCfgAcRate4
   {KEY_AC_RATE,         LIST_SELECT_5,      textButtonKeypad5},   // textListboxItemCfgAcRate5
   {KEY_AC_RATE,         LIST_SELECT_6,      textButtonKeypad6},   // textListboxItemCfgAcRate6
   //
   //   Reuse of 1-6 for inlet / return flow management buttons
   {KEY_INLET_MANAGEMENT,         LIST_SELECT_1,      textButtonKeypad1},
   {KEY_INLET_MANAGEMENT,         LIST_SELECT_2,      textButtonKeypad2},
   {KEY_INLET_MANAGEMENT,         LIST_SELECT_3,      textButtonKeypad3},
   {KEY_INLET_MANAGEMENT,         LIST_SELECT_4,      textButtonKeypad4},
   {KEY_INLET_MANAGEMENT,         LIST_SELECT_5,      textButtonKeypad5},
   {KEY_INLET_MANAGEMENT,         LIST_SELECT_6,      textButtonKeypad6},

   {KEY_RETURN_MANAGEMENT,        LIST_SELECT_1,      textButtonKeypad1},
   {KEY_RETURN_MANAGEMENT,        LIST_SELECT_2,      textButtonKeypad2},
   {KEY_RETURN_MANAGEMENT,        LIST_SELECT_3,      textButtonKeypad3},
   {KEY_RETURN_MANAGEMENT,        LIST_SELECT_4,      textButtonKeypad4},
   {KEY_RETURN_MANAGEMENT,        LIST_SELECT_5,      textButtonKeypad5},
   {KEY_RETURN_MANAGEMENT,        LIST_SELECT_6,      textButtonKeypad6},

   {KEY_INLET_FLOW_RAMP, CONFIG_YES, textListboxItemCfgSalineYes},
   {KEY_INLET_FLOW_RAMP, CONFIG_NO,  textListboxItemCfgSalineNo},

   // Procedure priority screen // key name, list name, text ID
   {KEY_PRI_PLATELET_LIST, PLATELET_NONE, textListboxItemCfgAdjNoPlatelets},
   {KEY_PRI_PLATELET_LIST, PLATELET_1,    textListboxItemCfgPltProc1 },
   {KEY_PRI_PLATELET_LIST, PLATELET_2,    textListboxItemCfgPltProc2 },
   {KEY_PRI_PLATELET_LIST, PLATELET_3,    textListboxItemCfgPltProc3 },
   {KEY_PRI_PLATELET_LIST, PLATELET_4,    textListboxItemCfgPltProc4 },
   {KEY_PRI_PLATELET_LIST, PLATELET_5,    textListboxItemCfgPltProc5 },
   {KEY_PRI_PLATELET_LIST, PLATELET_6,    textListboxItemCfgPltProc6 },
   {KEY_PRI_PLATELET_LIST, PLATELET_7,    textListboxItemCfgPltProc7 },
   {KEY_PRI_PLATELET_LIST, PLATELET_8,    textListboxItemCfgPltProc8 },
   {KEY_PRI_PLATELET_LIST, PLATELET_9,    textListboxItemCfgPltProc9 },
   {KEY_PRI_PLATELET_LIST, PLATELET_10,   textListboxItemCfgPltProc10},

   // jph -- needs 6.0 string:
   {KEY_PRI_PLATELET_AMAP_LIST, PLATELET_NONE, textListboxItemCfgAdjNoPlatelets},
   {KEY_PRI_PLATELET_AMAP_LIST, PLATELET_1,    TEXT_PROD_NAME_AMAP_PLT1},
   {KEY_PRI_PLATELET_AMAP_LIST, PLATELET_2,    TEXT_PROD_NAME_AMAP_PLT2},
   {KEY_PRI_PLATELET_AMAP_LIST, PLATELET_3,    TEXT_PROD_NAME_AMAP_PLT3},

   {KEY_PRI_PLATELET_AMAP_PAS_LIST, PLATELET_NONE, textListboxItemCfgAdjNoPlatelets},
   {KEY_PRI_PLATELET_AMAP_PAS_LIST, PLATELET_1,    TEXT_PROD_NAME_AMAP_PLT1},
   {KEY_PRI_PLATELET_AMAP_PAS_LIST, PLATELET_2,    TEXT_PROD_NAME_AMAP_PLT2},
   {KEY_PRI_PLATELET_AMAP_PAS_LIST, PLATELET_3,    TEXT_PROD_NAME_AMAP_PLT3},
   {KEY_PRI_PLATELET_AMAP_PAS_LIST, PLATELET_4,    TEXT_PROD_NAME_AMAP_PLT1},
   {KEY_PRI_PLATELET_AMAP_PAS_LIST, PLATELET_5,    TEXT_PROD_NAME_AMAP_PLT2},
   {KEY_PRI_PLATELET_AMAP_PAS_LIST, PLATELET_6,    TEXT_PROD_NAME_AMAP_PLT3},


   {KEY_PRI_PLASMA_LIST, PLASMA_NONE,     textListboxItemCfgAdjNoPlama},
   {KEY_PRI_PLASMA_LIST, PLASMA_1,        textListboxItemCfgPlsProc1},
   {KEY_PRI_PLASMA_LIST, PLASMA_2,        textListboxItemCfgPlsProc2},
   {KEY_PRI_PLASMA_LIST, PLASMA_3,        textListboxItemCfgPlsProc3},
   {KEY_PRI_PLASMA_LIST, PLASMA_4,        textListboxItemCfgPlsProc4},
   {KEY_PRI_PLASMA_LIST, PLASMA_5,        textListboxItemCfgPlsProc5},
   {KEY_PRI_PLASMA_LIST, PLASMA_6,        textListboxItemCfgPlsProc6},
   {KEY_PRI_PLASMA_LIST, PLASMA_AMAP,     textListboxItemCfgAdjPossible},
   {KEY_PRI_PLASMA_LIST, PLASMA_MAX_CONC, textListboxItemCfgPlsProcAmap},
   {KEY_PRI_PLASMA_LIST, PLASMA_FFP,      textListboxItemCfgPlsProcFfp},

   {KEY_PRI_RBC_LIST, RBC_NONE, textListboxItemCfgAdjNoRbcs},
   {KEY_PRI_RBC_LIST, RBC_1,    textListboxItemCfgRbcProc1},
   {KEY_PRI_RBC_LIST, RBC_2,    textListboxItemCfgRbcProc2},
   {KEY_PRI_RBC_LIST, RBC_3,    textListboxItemCfgRbcProc3},
   {KEY_PRI_RBC_LIST, RBC_4,    textListboxItemCfgRbcProc4},
   {KEY_PRI_RBC_LIST, RBC_5,    textListboxItemCfgRbcProc5},
   {KEY_PRI_RBC_LIST, RBC_6,    textListboxItemCfgRbcProc6},

   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_ALL,                 textListboxItemCfgBloodTypeAll},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_OP_ON,               textListboxItemCfgBloodTypeOnOp},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_ON,                  textListboxItemCfgBloodTypeOn},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN,         textListboxItemCfgBloodTypeOpOnApAn},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_OP_ON_AN,            textListboxItemCfgBloodTypeOpOnAn},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_ON_AN,               textListboxItemCfgBloodTypeOnAn},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_OP_ON_BP_BN,         textListboxItemCfgBloodTypeOpOnBpBn},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN_BP_BN,   textListboxItemCfgBloodTypeOpOnApAnBpBn},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_OP_ON_AP_AN_BN,      textListboxItemCfgBloodTypeOpOnApAnBn},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_AP_AN,               textListboxItemCfgBloodTypeAnAp},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_AP_AN_BP_BN_ABP_ABN, textListboxItemCfgBloodTypeApAnBpBnAbpAbn},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_BP_BN,               textListboxItemCfgBloodTypeBnBp},
   {KEY_PRI_BLOOD_TYPE_LIST,  CONFIG_INDEX_BLOOD_TYPE_ABP_ABN,             textListboxItemCfgBloodTypeAbpAbn},

   // MACHINE SCREEN
   {KEY_SET_AUDIO, AUDIO_LOW,  textListboxItemCfgMachLow},
   {KEY_SET_AUDIO, AUDIO_MED,  textListboxItemCfgMachMed},
   {KEY_SET_AUDIO, AUDIO_HIGH, textListboxItemCfgMachHigh},

   // RBC SCREEN
   {KEY_RBC_RATIO, RBC_814,  textListboxItemCfgRbcRatio814},
   {KEY_RBC_RATIO, RBC_11,   textListboxItemCfgRbcRatio11},
   {KEY_RBC_REPLACE_FLUID, CONFIG_YES, textListboxItemCfgSalineYes},
   {KEY_RBC_REPLACE_FLUID, CONFIG_NO,  textListboxItemCfgSalineNo},
   {KEY_DRBC_ALERT, CONFIG_YES, textListboxItemCfgOn},
   {KEY_DRBC_ALERT, CONFIG_NO,  textListboxItemCfgOff},

   {KEY_MAX_DRAW_FLOW,   DRAW_FLOW_SLOW,     textListboxItemCfgAcRateSlow},
   {KEY_MAX_DRAW_FLOW,   DRAW_FLOW_MODERATE, textListboxItemCfgAcRateModerate},
   {KEY_MAX_DRAW_FLOW,   DRAW_FLOW_FAST,     textListboxItemCfgAcRateFast},

   {KEY_PLASMA_RINSEBACK, CONFIG_YES, textListboxItemCfgOn},
   {KEY_PLASMA_RINSEBACK, CONFIG_NO, textListboxItemCfgOff},

   {KEY_SALINE_RINSEBACK, CONFIG_YES, textListboxItemCfgOn},
   {KEY_SALINE_RINSEBACK, CONFIG_NO, textListboxItemCfgOff},

   {KEY_AIR_REMOVAL,   CONFIG_YES,     textListboxItemCfgOn},
   {KEY_AIR_REMOVAL,   CONFIG_NO,      textListboxItemCfgOff},

   {KEY_AUTOFLOW,   CONFIG_YES,     textListboxItemCfgOn},
   {KEY_AUTOFLOW,   CONFIG_NO,      textListboxItemCfgOff},

   {KEY_MSS_PLT_ON, CONFIG_YES, textListboxItemCfgOn},
   {KEY_MSS_PLT_ON, CONFIG_NO,  textListboxItemCfgOff},

   {KEY_MSS_RBC_ON, CONFIG_YES, textListboxItemCfgOn},
   {KEY_MSS_RBC_ON, CONFIG_NO,  textListboxItemCfgOff},

   {KEY_SHOW_PRE_AAS_FLAGS, CONFIG_YES, textListboxItemCfgOn},
   {KEY_SHOW_PRE_AAS_FLAGS, CONFIG_NO,  textListboxItemCfgOff},

   {KEY_PLT_MSS_SPLIT_NOTIF, CONFIG_YES, textListboxItemCfgOn},
   {KEY_PLT_MSS_SPLIT_NOTIF, CONFIG_NO,     textListboxItemCfgOff},

   {KEY_OVERRIDE_PAS_BAG_VOL, CONFIG_YES, textListboxItemCfgOn},
   {KEY_OVERRIDE_PAS_BAG_VOL, CONFIG_NO,    textListboxItemCfgOff},

   {KEY_BLOOD_DIVERSION, CONFIG_YES, textListboxItemCfgOn},
   {KEY_BLOOD_DIVERSION, CONFIG_NO,  textListboxItemCfgOff},

   {KEY_COMPLEMENTARY_PLASMA, CONFIG_YES, textListboxItemCfgOn},
   {KEY_COMPLEMENTARY_PLASMA, CONFIG_NO,  textListboxItemCfgOff},

   {KEY_MALE_ONLY_PLASMA, MALE_ONLY_OPTION_MALE_ONLY,              textListboxItemCfgTraliMaleOnly},
   {KEY_MALE_ONLY_PLASMA, MALE_ONLY_OPTION_MALES_AND_AB_FEMALES,   textListboxItemCfgTraliMaleAndAbFemale},
   {KEY_MALE_ONLY_PLASMA, MALE_ONLY_OPTION_ALL,                    textListboxItemCfgTraliAll},

   {KEY_MALE_ONLY_PLT, MALE_ONLY_OPTION_MALE_ONLY,             textListboxItemCfgTraliMaleOnly},
   {KEY_MALE_ONLY_PLT, MALE_ONLY_OPTION_MALES_AND_AB_FEMALES,  textListboxItemCfgTraliMaleAndAbFemale},
   {KEY_MALE_ONLY_PLT, MALE_ONLY_OPTION_ALL,                   textListboxItemCfgTraliAll},

   {LAST_CONFIG_INDEX_KEY, LAST_STRING_INDEX, TEXT_NULL}
}; // end of config_list_table
///////////////////////////////////////////////////////////////////////////////
// end of cfg_kstr.cpp file

/* FORMAT HASH 52fe9ecbeda4625fc195db2229ff6693 */
