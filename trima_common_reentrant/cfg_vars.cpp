/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      cfg_vars.h
 *             Individual config variable definitions and access
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/
#include <vxworks.h>

#include <stdio.h>

#include "cfg_vars.h"
#include "run_defs.h"
#include "an_alarm.h"
#include "trima_datalog.h"
#include "configdef.h"

#define MIN_TM MINIMUM_PROCEDURE_TIME
#define MAX_TM ABSOLUTE_MAXIMUM_PROCEDURE_TIME

#define MAX_KEY_PLT_AMAP    6
#define MAX_KEY_PLASMA      9
#define MAX_KEY_BLOOD_TYPE 12

// Master table for Config.Dat Data.
// defines section, data key, max and min

static CCfgVarInfo VarList[] =
{
   CCfgVarInfo(LANGUAGE_UNIT_CONFIG, KEY_LANG,              "key_lang",              0,  128, STRING_TYPE),
   CCfgVarInfo(LANGUAGE_UNIT_CONFIG, KEY_HEIGHT,            "key_height",            0,  1, STRING_TYPE),
   CCfgVarInfo(LANGUAGE_UNIT_CONFIG, KEY_WEIGHT,            "key_weight",            0,  1, STRING_TYPE),
   CCfgVarInfo(LANGUAGE_UNIT_CONFIG, KEY_DATE_FORMAT,       "key_date_format",       0,  2, STRING_TYPE),
   CCfgVarInfo(LANGUAGE_UNIT_CONFIG, KEY_TIME_FORMAT,       "key_time_format",       0,  1, STRING_TYPE),
   CCfgVarInfo(LANGUAGE_UNIT_CONFIG, KEY_DECIMAL_DELIMITER, "key_decimal_delimiter", 0,  1, STRING_TYPE),
   CCfgVarInfo(LANGUAGE_UNIT_CONFIG, KEY_CRIT_OR_GLOB,      "key_crit_or_glob",      0,  1, STRING_TYPE),

   CCfgVarInfo(PROCEDURE_CONFIG, KEY_PROC_TIME,              "key_proc_time",         MIN_TM, MAX_TM, INT_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_RETURN_PRESS,           "key_return_press",         100,    310, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_DRAW_PRESS,             "key_draw_press",          -250,   -100, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_CUSTOM_RATIO,           "key_custom_ratio",        6.0f,  13.7f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_PLASMA_RATIO,           "key_plasma_ratio",        6.0f,  13.7f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_AC_RATE,                "key_ac_rate",                1,      6, INT_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_POST_CRIT,              "key_post_crit",           30.0,   55.0, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_TBV_VOL_SETTING,        "key_tbv_vol_setting",        1,     63, NULL_TYPE), // no auto display in configuration
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_TBV_PERCENT,            "key_tbv_percent",            1,     15, FLOAT_0DEC_TYPE),

#if 1
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_WEIGHT_SETTING,          "key_weight_setting",           88,    500, NULL_TYPE), // no auto display in configuration
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_WEIGHT_LESS_THAN_VOL,    "key_weight_less_than_vol",      0,   1200, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_WEIGHT_GREATER_THAN_VOL, "key_weight_greater_than_vol",   0,   1200, FLOAT_0DEC_TYPE),
#else
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_WEIGHT_1_SETTING,       "key_weight_1_setting",      88,    500, NULL_TYPE), // no auto display in configuration
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_WEIGHT_2_SETTING,       "key_weight_2_setting",      88,    500, NULL_TYPE), // no auto display in configuration
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_WEIGHT_3_SETTING,       "key_weight_3_setting",      88,    500, NULL_TYPE), // no auto display in configuration
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_WEIGHT_1_LESS_THAN_VOL, "key_weight_1_less_than_vol", 0,   1200, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_WEIGHT_2_LESS_THAN_VOL, "key_weight_2_less_than_vol", 0,   1200, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_WEIGHT_3_LESS_THAN_VOL, "key_weight_3_less_than_vol", 0,   1200, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_WEIGHT_3_GREATER_THAN_VOL, "key_weight_3_greater_than_vol",      0,   1200, FLOAT_0DEC_TYPE),
#endif

   CCfgVarInfo(PROCEDURE_CONFIG, KEY_MAX_PLASMA_DURING_PLT_VOL,      "key_max_plasma_plt",               0,   1200, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_MAX_PLASMA_DURING_PLS_VOL,      "key_max_plasma_pls",               0,   1200, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PROCEDURE_CONFIG, KEY_ML_PER_KG,              "key_ml_per_kg",           2.0f,  10.5f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_DRBC_BODY_VOL,          "key_drbc_body_vol",       3.0f,  7.0f,  FLOAT_1DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_POST_PLAT,              "key_post_plat",          50000, 120000, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_MAX_DRAW_FLOW,          "key_max_draw_flow",          0,      2, STRING_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_MIN_REPLACEMENT_VOLUME, "key_min_replacement_volume", 0,   1000, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_PLT_MSS_SPLIT_NOTIF,    "key_plt_mss_split_notif",    0,      1, STRING_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_OVERRIDE_PAS_BAG_VOL,   "key_override_pas_bag_vol",   0,      1, STRING_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_BLOOD_DIVERSION,        "key_blood_diversion",        0,      1, STRING_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_DRBC_ALERT,             "key_drbc_alert",             0,      1, STRING_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_DRBC_THRESHOLD,         "key_drbc_threshold",         150,    450, INT_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_PLASMA_RINSEBACK,       "key_plasma_rinseback",       0,      1, STRING_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_SALINE_RINSEBACK,       "key_saline_rinseback",       0,      1, STRING_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_AUDIT_TRACKING,         "key_audit_tracking",         0,      3, STRING_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_AIR_REMOVAL,            "key_air_removal",            0,      1, STRING_TYPE),

   CCfgVarInfo(PROCEDURE_CONFIG, KEY_AUTOFLOW,              "key_autoflow",                0,      1, STRING_TYPE),


   CCfgVarInfo(PREDICTION_CONFIG, KEY_RBC_RATIO,           "key_rbc_ratio",                   0,    1, STRING_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_RBC_REPLACE_FLUID,   "key_rbc_replace_fluid",           0,    1, NULL_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_RBC_FLUID_PERCENT,   "key_rbc_fluid_percent",          80,  120, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_PLASMA_AMAP_MINIMUM, "key_pls_amap_minimum",            0, 1000, INT_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_PLASMA_AMAP_MAXIMUM, "key_pls_amap_maximum",            0, 1000, INT_TYPE),

   CCfgVarInfo(PREDICTION_CONFIG, KEY_MALE_ONLY_PLASMA,    "key_male_only_plasma",            0, 2,    STRING_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_MALE_ONLY_PLT,       "key_male_only_plt",               0, 2,    STRING_TYPE),

   CCfgVarInfo(PREDICTION_CONFIG, KEY_COMPLEMENTARY_PLASMA, "key_complementary_plasma",       0, 1,    STRING_TYPE),

   CCfgVarInfo(PREDICTION_CONFIG, KEY_FFP_VOLUME, "key_ffp_volume",       0, 1000,    INT_TYPE),

   CCfgVarInfo(PREDICTION_CONFIG, KEY_YSF,                 "key_ysf",                      0.5f, 1.5f, FLOAT_2DEC_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_GUARANTEED_YIELD,    "key_guaranteed_yield",            0,    1, NULL_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_GUARANTEED_YIELD_CL, "key_confidence_level",         75.0, 95.0, INT_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_GUARANTEED_YIELD_CV, "key_coefficient_of_variation",  0.0, 20.0, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_INLET_MANAGEMENT,    "key_inlet_management",            1,    6, INT_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_INLET_FLOW_RAMP,     "key_inlet_flow_ramp",           0.0,  1.0, STRING_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_RETURN_MANAGEMENT,   "key_return_management",           1,    6, INT_TYPE),
   CCfgVarInfo(PREDICTION_CONFIG, KEY_RSF,                 "key_rsf",                      0.9f, 1.1f, FLOAT_2DEC_TYPE),

   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_CRIT_1,   "key_rbc_crit_1",   55.0f,   80.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_DOSE_1,   "key_rbc_dose_1",   35.0f,  550.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_CRIT_2,   "key_rbc_crit_2",   55.0f,   80.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_DOSE_2,   "key_rbc_dose_2",   35.0f,  550.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_CRIT_3,   "key_rbc_crit_3",   55.0f,   80.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_DOSE_3,   "key_rbc_dose_3",   35.0f,  550.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_CRIT_4,   "key_rbc_crit_4",   55.0f,   80.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_DOSE_4,   "key_rbc_dose_4",   35.0f,  550.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_CRIT_5,   "key_rbc_crit_5",   55.0f,   80.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_DOSE_5,   "key_rbc_dose_5",   35.0f,  550.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_CRIT_6,   "key_rbc_crit_6",   55.0f,   80.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_DOSE_6,   "key_rbc_dose_6",   35.0f,  550.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_VOLUME_1, "key_rbc_mss_volume_1",   80, 1000, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_VOLUME_2, "key_rbc_mss_volume_2",   80, 1000, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_VOLUME_3, "key_rbc_mss_volume_3",   80, 1000, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_VOLUME_4, "key_rbc_mss_volume_4",   80, 1000, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_VOLUME_5, "key_rbc_mss_volume_5",   80, 1000, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_VOLUME_6, "key_rbc_mss_volume_6",   80, 1000, INT_TYPE),

   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_1,   "key_rbc_mss_1",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_2,   "key_rbc_mss_2",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_3,   "key_rbc_mss_3",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_4,   "key_rbc_mss_4",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_5,   "key_rbc_mss_5",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_RBC_MSS_6,   "key_rbc_mss_6",   0, 1, INT_TYPE),


   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_MIN_YIELD_1,  "key_plt_amap_single_yield_min",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_MIN_YIELD_2,  "key_plt_amap_double_yield_min",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_MIN_YIELD_3,  "key_plt_amap_triple_yield_min",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_MAX_YIELD_1,  "key_plt_amap_single_yield_max",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_MAX_YIELD_2,  "key_plt_amap_double_yield_max",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_MAX_YIELD_3,  "key_plt_amap_triple_yield_max",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_CONC_1,  "key_plt_amap_single_conc",   100.0f,   10000.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_CONC_2,  "key_plt_amap_double_conc",   100.0f,   10000.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_CONC_3,  "key_plt_amap_triple_conc",   100.0f,   10000.0f, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_CONC_4,  "key_plt_amap_pas_single_conc",   100.0f,   10000.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_CONC_5,  "key_plt_amap_pas_double_conc",   100.0f,   10000.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_CONC_6,  "key_plt_amap_pas_triple_conc",   100.0f,   10000.0f, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_PAS_CARRYOVER_1,  "key_plt_amap_pas_single_pct_carryover", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_PAS_CARRYOVER_2,  "key_plt_amap_pas_double_pct_carryover", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_AMAP_PAS_CARRYOVER_3,  "key_plt_amap_pas_triple_pct_carryover", 1, 99, INT_TYPE),



   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_YIELD_1,  "key_plt_yield_1",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_VOLUME_1, "key_plt_volume_1", 50.0f, 1200.0f, FLOAT_2DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_YIELD_2,  "key_plt_yield_2",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_VOLUME_2, "key_plt_volume_2", 50.0f, 1200.0f, FLOAT_2DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_YIELD_3,  "key_plt_yield_3",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_VOLUME_3, "key_plt_volume_3", 50.0f, 1200.0f, FLOAT_2DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_YIELD_4,  "key_plt_yield_4",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_VOLUME_4, "key_plt_volume_4", 50.0f, 1200.0f, FLOAT_2DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_YIELD_5,  "key_plt_yield_5",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_VOLUME_5, "key_plt_volume_5", 50.0f, 1200.0f, FLOAT_2DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_YIELD_6,  "key_plt_yield_6",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_VOLUME_6, "key_plt_volume_6", 50.0f, 1200.0f, FLOAT_2DEC_TYPE),

   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_YIELD_7,  "key_plt_yield_7",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_VOLUME_7, "key_plt_volume_7", 50.0f, 1200.0f, FLOAT_2DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_YIELD_8,  "key_plt_yield_8",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_VOLUME_8, "key_plt_volume_8", 50.0f, 1200.0f, FLOAT_2DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_YIELD_9,  "key_plt_yield_9",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_VOLUME_9, "key_plt_volume_9", 50.0f, 1200.0f, FLOAT_2DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_YIELD_10, "key_plt_yield_10",   1.0f,   15.0f, FLOAT_1DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_VOLUME_10, "key_plt_volume_10", 50.0f, 1200.0f, FLOAT_2DEC_TYPE),

   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_PCT_CARRYOVER_1, "key_plt_pct_carryover_1", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_PCT_CARRYOVER_2, "key_plt_pct_carryover_2", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_PCT_CARRYOVER_3, "key_plt_pct_carryover_3", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_PCT_CARRYOVER_4, "key_plt_pct_carryover_4", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_PCT_CARRYOVER_5, "key_plt_pct_carryover_5", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_PCT_CARRYOVER_6, "key_plt_pct_carryover_6", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_PCT_CARRYOVER_7, "key_plt_pct_carryover_7", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_PCT_CARRYOVER_8, "key_plt_pct_carryover_8", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_PCT_CARRYOVER_9, "key_plt_pct_carryover_9", 1, 99, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_PCT_CARRYOVER_10, "key_plt_pct_carryover_10", 1, 99, INT_TYPE),

   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_MSS_1,   "key_plt_mss_1",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_MSS_2,   "key_plt_mss_2",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_MSS_3,   "key_plt_mss_3",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_MSS_4,   "key_plt_mss_4",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_MSS_5,   "key_plt_mss_5",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_MSS_6,   "key_plt_mss_6",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_MSS_7,   "key_plt_mss_7",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_MSS_8,   "key_plt_mss_8",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_MSS_9,   "key_plt_mss_9",   0, 1, INT_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLT_MSS_10,  "key_plt_mss_10",  0, 1, INT_TYPE),

   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLS_VOLUME_1, "key_pls_volume_1",  0.0f, 1000.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLS_VOLUME_2, "key_pls_volume_2",  0.0f, 1000.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLS_VOLUME_3, "key_pls_volume_3",  0.0f, 1000.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLS_VOLUME_4, "key_pls_volume_4",  0.0f, 1000.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLS_VOLUME_5, "key_pls_volume_5",  0.0f, 1000.0f, FLOAT_0DEC_TYPE),
   CCfgVarInfo(PRODUCT_TEMPLATES, KEY_PLS_VOLUME_6, "key_pls_volume_6",  0.0f, 1000.0f, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_A,      "key_platelet_a",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_A, "key_platelet_amap_a",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_A,        "key_plasma_a",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_A,           "key_rbc_a",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_A,    "key_blood_type_a",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_A,          "key_time_a",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_B,      "key_platelet_b",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_B,        "key_plasma_b",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_B, "key_platelet_amap_b",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_B,           "key_rbc_b",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_B,    "key_blood_type_b",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_B,          "key_time_b",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_C,      "key_platelet_c",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_C,        "key_plasma_c",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_C, "key_platelet_amap_c",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_C,           "key_rbc_c",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_C,    "key_blood_type_c",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_C,          "key_time_c",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_D,      "key_platelet_d",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_D,        "key_plasma_d",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_D, "key_platelet_amap_d",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_D,           "key_rbc_d",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_D,    "key_blood_type_d",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_D,          "key_time_d",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_E,      "key_platelet_e",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_E,        "key_plasma_e",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_E,           "key_rbc_e",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_E, "key_platelet_amap_e",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_E,    "key_blood_type_e",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_E,          "key_time_e",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_F,      "key_platelet_f",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_F,        "key_plasma_f",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_F, "key_platelet_amap_f",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_F,           "key_rbc_f",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_F,    "key_blood_type_f",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_F,          "key_time_f",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_G,      "key_platelet_g",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_G,        "key_plasma_g",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_G, "key_platelet_amap_g",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_G,           "key_rbc_g",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_G,    "key_blood_type_g",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_G,          "key_time_g",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_H,      "key_platelet_h",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_H,        "key_plasma_h",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_H, "key_platelet_amap_h",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_H,           "key_rbc_h",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_H,    "key_blood_type_h",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_H,          "key_time_h",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_I,      "key_platelet_i",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_I,        "key_plasma_i",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_I, "key_platelet_amap_i",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_I,           "key_rbc_i",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_I,    "key_blood_type_i",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_I,          "key_time_i",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_J,      "key_platelet_j",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_J,        "key_plasma_j",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_J, "key_platelet_amap_j",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_J,           "key_rbc_j",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_J,    "key_blood_type_j",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_J,          "key_time_j",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_K,      "key_platelet_k",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_K,        "key_plasma_k",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_K, "key_platelet_amap_k",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_K,           "key_rbc_k",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_K,    "key_blood_type_k",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_K,          "key_time_k",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_L,      "key_platelet_l",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_L,        "key_plasma_l",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_L, "key_platelet_amap_l",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_L,           "key_rbc_l",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_L,    "key_blood_type_l",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_L,          "key_time_l",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_M,      "key_platelet_m",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_M,        "key_plasma_m",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_M, "key_platelet_amap_m",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_M,           "key_rbc_m",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_M,    "key_blood_type_m",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_M,          "key_time_m",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_N,      "key_platelet_n",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_N,        "key_plasma_n",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_N, "key_platelet_amap_n",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_N,           "key_rbc_n",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_N,    "key_blood_type_n",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_N,          "key_time_n",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_O,      "key_platelet_o",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_O,        "key_plasma_o",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_O, "key_platelet_amap_o",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_O,           "key_rbc_o",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_O,    "key_blood_type_o",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_O,          "key_time_o",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_P,      "key_platelet_p",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_P,        "key_plasma_p",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_P, "key_platelet_amap_p",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_P,           "key_rbc_p",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_P,    "key_blood_type_p",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_P,          "key_time_p",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_Q,      "key_platelet_q",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_Q,        "key_plasma_q",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_Q, "key_platelet_amap_q",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_Q,           "key_rbc_q",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_Q,    "key_blood_type_q",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_Q,          "key_time_q",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_R,      "key_platelet_r",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_R,        "key_plasma_r",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_R, "key_platelet_amap_r",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_R,           "key_rbc_r",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_R,    "key_blood_type_r",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_R,          "key_time_r",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_S,      "key_platelet_s",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_S,        "key_plasma_s",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_S, "key_platelet_amap_s",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_S,           "key_rbc_s",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_S,    "key_blood_type_s",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_S,          "key_time_s",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_T,      "key_platelet_t",       0, NUM_YIELD_VOLUME, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLASMA_T,        "key_plasma_t",         0, MAX_KEY_PLASMA, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_PLATELET_AMAP_T, "key_platelet_amap_t",  0, MAX_KEY_PLT_AMAP, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_RBC_T,           "key_rbc_t",            0, NUM_CRIT_DOSE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_BLOOD_TYPE_T,    "key_blood_type_t",     0, MAX_KEY_BLOOD_TYPE, STRING_TYPE),
   CCfgVarInfo(PRODUCT_DEFINITIONS, KEY_PRI_TIME_T,          "key_time_t",           MIN_TM, MAX_TM, FLOAT_0DEC_TYPE),

   CCfgVarInfo(BASIC_MACHINE_CONFIG, KEY_SET_TIME,    "key_set_time", 0, 24, TIME_TYPE),
   CCfgVarInfo(BASIC_MACHINE_CONFIG, KEY_SET_DATE,    "key_set_date", 1, 99, DATE_TYPE),
   CCfgVarInfo(BASIC_MACHINE_CONFIG, KEY_SET_AUDIO,   "key_set_audio",  0, 3, STRING_TYPE),
   CCfgVarInfo(BASIC_MACHINE_CONFIG, KEY_PWORD,       "key_pword",     0, 999999, INT_TYPE),

   // Metered Storage Stuff
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_MSS_RBC_ON,      "key_mss_rbc_on",         0,        1, STRING_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_MSS_PLT_ON,      "key_mss_plt_on",         0,        1, STRING_TYPE),

   CCfgVarInfo(PROCEDURE_CONFIG, KEY_SHOW_PRE_AAS_FLAGS, "key_show_pre_aas_flags", 0,     1, STRING_TYPE),

   CCfgVarInfo(PROCEDURE_CONFIG, KEY_RBC_DEF_BAG_VOL, "key_rbc_def_bag_vol", 30,  500, INT_TYPE),
   CCfgVarInfo(PROCEDURE_CONFIG, KEY_PLT_DEF_BAG_VOL, "key_plt_def_bag_vol", 30, 1000, INT_TYPE),

   CCfgVarInfo(NUM_SECTIONS, KEY_INACTIVE,  "", 0, 0, INACTIVE_TYPE),
   CCfgVarInfo(NUM_SECTIONS, LAST_CONFIG_INDEX_KEY,  "", 0, 0, NULL_TYPE)
};

typedef struct
{
   CONFIG_SECTION_NAMES sct;
   const char*          pName;
} SECTION_STRINGS;

static SECTION_STRINGS SectionNames[] = {
   { LANGUAGE_UNIT_CONFIG, "LANGUAGE_UNIT_CONFIG"},
   { PROCEDURE_CONFIG, "PROCEDURE_CONFIG"},
   { PREDICTION_CONFIG, "PREDICTION_CONFIG"},
   { PRODUCT_TEMPLATES, "PRODUCT_TEMPLATES"},
   { PRODUCT_DEFINITIONS, "PRODUCT_DEFINITIONS"},
   { BASIC_MACHINE_CONFIG, "BASIC_MACHINE_CONFIG"},
   { NUM_SECTIONS, ""}
};

static const char* SectionName (CONFIG_SECTION_NAMES sct)
{
   int i = 0;
   while (( SectionNames[i].sct != sct ) && ( SectionNames[i].sct != NUM_SECTIONS ))
   {
      i++;
   }
   return SectionNames[i].pName;
}

CCfgVarInfo::CCfgVarInfo(CONFIG_SECTION_NAMES sct,
                         CONFIG_DATA_KEYS id,
                         const char* name,
                         float min,
                         float max,
                         CONFIG_DATA_TYPE displayType
                         )
{
   m_nId         = id;
   m_nSct        = sct;
   m_fMin        = min;
   m_fMax        = max;
   m_pStrSct     = SectionName(sct);
   m_pStrName    = name;
   m_displayType = displayType;
   _LogErrors    = 0;
}

CCfgVarInfo::CCfgVarInfo(CONFIG_DATA_KEYS id)
{
   Init();
   CCfgVarList        List(_LogErrors);

   const CCfgVarInfo* pVar = List.pVar(id);
   if (pVar != 0)
   {
      Transfer(*pVar);
   }
}

CCfgVarInfo::CCfgVarInfo(const CCfgVarInfo& var)
{
   Transfer(var);
}


void CCfgVarInfo::Transfer (const CCfgVarInfo& var)
{
   m_nId         = var.DataId();
   m_nSct        = var.SectionId();
   m_fMin        = var.Min();
   m_fMax        = var.Max();
   m_pStrSct     = var.Section();
   m_pStrName    = var.Name();
   m_displayType = var.DisplayType();
   _LogErrors    = var._LogErrors;
}

const CCfgVarInfo& CCfgVarInfo::operator = (const CCfgVarInfo& var)
{
   if (this != &var)
   {
      Transfer(var);
   }
   return *this;
}

void CCfgVarInfo::Init (int logErrors)
{
   m_nId         = LAST_CONFIG_INDEX_KEY;
   m_nSct        = NUM_SECTIONS;
   m_fMin        = 0;
   m_fMax        = 0;
   m_pStrSct     = 0;
   m_pStrName    = 0;
   m_displayType = NULL_TYPE;
   _LogErrors    = logErrors;
}

CCfgVarInfo::~CCfgVarInfo()
{}

bool CCfgVarInfo::Valid (float v) const
{
   int flag = ( ( v >= m_fMin ) && ( v <= m_fMax ) );

   // Note:  For some reason, _LogErrors is not be set properly.  For now
   // remove logging check and print it out always.  This should be revisited later.
   if (!flag)
   {
      DataLog(log_level_config_error) << "config.dat CCfgVarInfo Failed Check: "
                                      << m_pStrSct << "," << m_pStrName << "="
                                      << v << " : [" << m_fMin << "," << m_fMax << "]"
                                      << endmsg;
   }

   return flag;
}

CCfgVarList::CCfgVarList(int logErrors)
   : _LogErrors(logErrors)
{}

CCfgVarList::~CCfgVarList()
{}

const CCfgVarInfo& CCfgVarList::Var (CONFIG_DATA_KEYS id)
{
   int              i     = 0;
   CONFIG_DATA_KEYS tmpId = VarList[0].DataId();

   while (tmpId != LAST_CONFIG_INDEX_KEY)
   {
      if (tmpId == id)
         return VarList[i];
      i++;
      tmpId = VarList[i].DataId();
   }

   if (_LogErrors)
   {
      DataLog(log_level_config_error) << "Config Var " << id << " not found" << endmsg;
   }

   VarList[i].logErrors(_LogErrors);

   return VarList[i];
}


const CCfgVarInfo* CCfgVarList::pVar (CONFIG_DATA_KEYS id)
{
   return &Var(id);
}

const CCfgVarInfo& CCfgVarList::IthVar (int index)
{
   if (( index < 0 ) || ( index >= NumberVars() ))
   {
      if (_LogErrors)
      {
         DataLog(log_level_config_error) << "config.dat invalid index " << index << endmsg;
      }
      return Var(LAST_CONFIG_INDEX_KEY);
   }

   return VarList[index];
}


const CCfgVarInfo* CCfgVarList::pIthVar (int index)
{
   return &IthVar(index);
}

int CCfgVarList::NumberVars () const
{
   return((sizeof( VarList ) / sizeof( CCfgVarInfo )));
}


///////////////////////////////////////////////////////////////
// CCfgValue
// object containing var info and the current value

CCfgValue::CCfgValue()
{
   Init();
}

CCfgValue::CCfgValue(const CCfgVarInfo* pVar, float* pVal)
{
   Initialize(pVar, pVal);
}

void CCfgValue::Initialize (const CCfgVarInfo* pVar, float* pVal)
{
   m_pVar = pVar;
   m_pVal = pVal;
}

CCfgValue::CCfgValue(const CCfgValue& val)
{
   Transfer(val);
}

CCfgValue::~CCfgValue()
{}

const CCfgValue& CCfgValue::operator = (const CCfgValue& val)
{
   if (this != &val)
   {
      Transfer(val);
   }
   return *this;
}


void CCfgValue::Init ()
{
   m_pVar = 0;
   m_pVal = 0;
}

void CCfgValue::Transfer (const CCfgValue& val)
{
   m_pVar = val.cpInfo();
   m_pVal = val.pValue();
}

float* CCfgValue::pValue () const
{
   return m_pVal;
}

const CCfgVarInfo* CCfgValue::cpInfo () const
{
   return m_pVar;
}

int CCfgValue::Valid (float v) const
{
   return m_pVar->Valid(v);
}

int CCfgValue::Valid () const
{
   return m_pVar->Valid(*m_pVal);
}

float CCfgValue::Value () const
{
   return *m_pVal;
}

float CCfgValue::Value (float v)
{
   if (Valid(v)) *m_pVal = v;
   return *m_pVal;
}

/* FORMAT HASH 6be0a4f8283b93f9e0c5b61cfb574bf6 */
