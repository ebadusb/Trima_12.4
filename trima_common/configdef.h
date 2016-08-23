/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      configdef.h
 *             Config.dat section structure definitions
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

#include "carray.h"
#include "bloodtype.h"
#include "datalog.h"


#if !defined( INCLUDE_CONFIGDEF )
#define INCLUDE_CONFIGDEF

#define NUM_OFFLINE_PROCEDURES 20
#define NUM_VISTA_PROCEDURES   50
#define NUM_CRIT_DOSE           6
#define NUM_YIELD_VOLUME       10
#define NUM_PLASMA_VOLUME       6
#define NUM_PLT_AMAP            6
#define NUM_PLT_AMAP_PAS        3


struct LangUnitCfg
{
   int key_lang;
   int key_height;
   int key_weight;
   int key_date_format;
   int key_time_format;
   int key_decimal_delimiter;
   int key_crit_or_glob;

public: LangUnitCfg::LangUnitCfg()
      : key_lang(0), key_height(0), key_weight(0), key_date_format(0), key_time_format(0), key_decimal_delimiter(0), key_crit_or_glob(0) {}
};

struct ProcedureConfig
{
   float key_return_press;
   float key_draw_press;
   float key_custom_ratio;
   float key_plasma_ratio;
   float key_post_crit;
   float key_tbv_percent;
   float key_weight_setting;
   float key_weight_less_than_vol;
   float key_weight_greater_than_vol;
   float key_ml_per_kg;
   float key_drbc_body_vol;
   float key_max_plasma_vol_during_plasma_collection;
   float key_max_plasma_vol_during_platelet_collection;
   float key_post_plat;
   int   key_proc_time;
   int   key_ac_rate;
   int   key_tbv_vol_setting;
   int   key_max_draw_flow;
   int   key_min_replacement_volume;
   int   key_plt_mss_split_notif;
   int   key_override_pas_bag_vol;
   int   key_blood_diversion;
   int   key_drbc_alert;
   int   key_drbc_threshold;
   int   key_plasma_rinseback;
   int   key_saline_rinseback;
   int   key_audit_tracking;
   int   key_air_removal;
   int   key_mss_plt_on;
   int   key_mss_rbc_on;
   int   key_show_pre_aas_flags;  // See IT 7960 for implementation details.
   int   key_plt_def_bag_vol;
   int   key_rbc_def_bag_vol;
   int   key_autoflow;

public: ProcedureConfig::ProcedureConfig()
      : key_return_press(0.0f), key_draw_press(0.0f), key_custom_ratio(0.0f), key_plasma_ratio(0.0f), key_post_crit(0.0f), key_tbv_percent(0.0f),
        key_weight_setting(0.0f), key_weight_less_than_vol(0.0f), key_weight_greater_than_vol(0.0f), key_ml_per_kg(0.0f), key_drbc_body_vol(0.0f),
        key_max_plasma_vol_during_plasma_collection(0.0f), key_max_plasma_vol_during_platelet_collection(0.0f), key_post_plat(0.0f),
        key_proc_time(0), key_ac_rate(0), key_tbv_vol_setting(0), key_max_draw_flow(0), key_min_replacement_volume(0), key_plt_mss_split_notif(0), key_override_pas_bag_vol(0), key_blood_diversion(0), key_drbc_alert(0), key_drbc_threshold(0), key_plasma_rinseback(0), key_saline_rinseback(0), key_audit_tracking(0), key_air_removal(0), key_mss_plt_on(0), key_mss_rbc_on(0), key_show_pre_aas_flags(0), key_plt_def_bag_vol(0), key_rbc_def_bag_vol(0), key_autoflow(0) {}
};


// different AMAP PLT size ranges available
enum AMAPPLTTYPES
{
   AMAP_PLT_NONE       = 0, // setting if doing old-school PLT collects
   AMAP_PLT_SINGLE     = 1,
   AMAP_PLT_DOUBLE     = 2,
   AMAP_PLT_TRIPLE     = 3,
   AMAP_PLT_PAS_SINGLE = 4,
   AMAP_PLT_PAS_DOUBLE = 5,
   AMAP_PLT_PAS_TRIPLE = 6,
};


struct PredictionConfig
{
   int   key_rbc_ratio;
   int   key_rbc_replace_fluid;
   float key_rbc_fluid_percent;
   int   key_pls_amap_min;
   int   key_pls_amap_max;
   float key_ysf;
   int   key_guaranteed_yield;
   int   key_confidence_level;
   float key_coefficient_of_variation;
   int   key_inlet_management;
   int   key_inlet_flow_ramp;
   int   key_return_management;
   float key_rsf;
   int   key_male_only_plt;
   int   key_male_only_plasma;
   int   key_ffp_volume;
   int   key_complementary_plasma;

public: PredictionConfig::PredictionConfig()
      : key_rbc_ratio(0), key_rbc_replace_fluid(0), key_rbc_fluid_percent(0.0f), key_pls_amap_min(0), key_pls_amap_max(0), key_ysf(0.0f), key_guaranteed_yield(0), key_confidence_level(0), key_coefficient_of_variation(0.0f), key_inlet_management(0), key_inlet_flow_ramp(0), key_return_management(0), key_rsf(0.0f), key_male_only_plt(0), key_male_only_plasma(0), key_ffp_volume(0), key_complementary_plasma(0) {}
};

struct ProductTemplates
{
   float key_rbc_crit[NUM_CRIT_DOSE];
   float key_rbc_dose[NUM_CRIT_DOSE];
   int   key_rbc_mss[NUM_CRIT_DOSE];
   int   key_rbc_mss_volume[NUM_CRIT_DOSE];

   float key_plt_amap_min_yield[NUM_PLT_AMAP];
   float key_plt_amap_max_yield[NUM_PLT_AMAP];
   float key_plt_amap_conc[NUM_PLT_AMAP];
   int   key_plt_amap_pas_pct_carryover[NUM_PLT_AMAP];

   float key_plt_yield[NUM_YIELD_VOLUME];
   float key_plt_volume[NUM_YIELD_VOLUME];
   int   key_plt_mss[NUM_YIELD_VOLUME];
   int   key_plt_pct_carryover[NUM_YIELD_VOLUME];

   float key_pls_volume[NUM_PLASMA_VOLUME];

public: ProductTemplates::ProductTemplates()
   {
      memset(key_rbc_crit,       0, sizeof(key_rbc_crit));
      memset(key_rbc_dose,       0, sizeof(key_rbc_dose));
      memset(key_rbc_mss,        0, sizeof(key_rbc_mss));
      memset(key_rbc_mss_volume, 0, sizeof(key_rbc_mss_volume));

      memset(key_plt_amap_min_yield,         0, sizeof(key_plt_amap_min_yield));
      memset(key_plt_amap_max_yield,         0, sizeof(key_plt_amap_max_yield));
      memset(key_plt_amap_conc,              0, sizeof(key_plt_amap_conc));
      memset(key_plt_amap_pas_pct_carryover, 0, sizeof(key_plt_amap_pas_pct_carryover));

      memset(key_plt_yield,         0, sizeof(key_plt_yield));
      memset(key_plt_volume,        0, sizeof(key_plt_volume));
      memset(key_plt_mss,           0, sizeof(key_plt_mss));
      memset(key_plt_pct_carryover, 0, sizeof(key_plt_pct_carryover));

      memset(key_pls_volume, 0, sizeof(key_pls_volume));
   }
};

// #
// #
// #   The product definitions shown are indices into the product templates:
// #      key_platelet_   : 0=none; 1=plt1, 2=plt2, ... 6=plt6
// #      key_platelet_amap_  : 0=none; 1=single,    2=double,     3=triple
// #                                    4=single&PAS 5=double&PAS  6=triple&PAS
// #      key_plasma      : 0=none; 1=pls1; 2=pls2; ... 6=pls6 7=AMAP 8= MAX CONCURRENT
// #      key_rbc_        : 0=none; 1=rbc1; 2=rbc2; 3=rbc3
// #      key_blood_type_ : 0=all; 1=O+/O-/A+/A-; 2=O+/O-; 3=O-; 4=O-/A-; 5=A-/A+; 6=B-/B+; 7=AB-/AB+
// #      key_time_       : minutes, range 10-150
// #


struct ProductDefinition
{
   int   key_platelet;
   int   key_platelet_amap;
   int   key_plasma;
   int   key_rbc;
   int   key_blood_type;    // 0 - 12.  See above.
   float key_time;

public: ProductDefinition::ProductDefinition()
      : key_platelet(0), key_platelet_amap(0), key_plasma(0), key_rbc(0), key_blood_type(0), key_time(0) {}
};


struct BasicMachineCfg
{
   int key_set_time;
   int key_set_date;
   int key_set_audio;
   int key_pword;

public: BasicMachineCfg::BasicMachineCfg()
      : key_set_time(0), key_set_date(0), key_set_audio(0), key_pword(0) { }
};


typedef carray<ProductDefinition, NUM_OFFLINE_PROCEDURES> ProductDefinitionArray;

DataLog_Stream& operator << (DataLog_Stream& os, const AMAPPLTTYPES& type);


struct ProcedureDescription
{
   float        yield;              // x 100,000,000,000 platelets
   float        platelet_volume;    // in ml
   float        plasma_volume;      // in ml or -1 for AMAP, -2 for MC
   float        rbc_dose;           // volume of packed rbc in ml
   float        rbc_crit;           // Fraction hematocrit of rbc product
   float        max_procedure_time; // maximum proc time in min
   BLOODTYPE    blood_type;         // see bloodtype enum
   bool         plt_mss;            // metered storage on=1/off=0
   int          pct_carryover;      // percent plasma carryover
   bool         rbc_mss;            // metered storage on=1/off=0
   int          rbc_mss_volume;     // metered rbc volume
   int          designator;         // 1 = primary procedure, 2 = secondary procedure.
   AMAPPLTTYPES amap_platelet_size; // none, single, double, or triple as defined by Plt Config
   float        amap_platelet_min_yield;
   float        amap_platelet_max_yield;
   float        amap_platelet_conc;

   ProcedureDescription()
      : yield(0), platelet_volume(0), plasma_volume(0), rbc_dose(0),
        rbc_crit(0), max_procedure_time(0), blood_type(BT_UNKNOWN), plt_mss(0), pct_carryover(0),
        rbc_mss(0), rbc_mss_volume(0), designator(0), amap_platelet_size(AMAP_PLT_NONE),
        amap_platelet_min_yield(0), amap_platelet_max_yield(0), amap_platelet_conc(0) {}
};

DataLog_Stream& operator << (DataLog_Stream& os, const ProcedureDescription& type);

typedef carray<ProcedureDescription, NUM_VISTA_PROCEDURES> ProductListArray;
#ifdef __WIN32__
struct ConfigStruct
{
   LangUnitCfg            langCfg;
   BasicMachineCfg        machineCfg;
   ProcedureConfig        procedureCfg;
   PredictionConfig       predictionCfg;
   ProductTemplates       prodTemplates;
   ProductDefinitionArray prodDefArray;
};
#endif

#endif

/* FORMAT HASH ce103172d75d6314051e42538da5d0ee */
