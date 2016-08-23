// cfg_main_input.cpp : implementation file
//

#include "stdafx.h"
#include "cfg_main_input.h"
#include "cfg_ids.h"
#include "filehelp.h"
#include "datfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Config_CDS *cfg_main_input::userconfig = 0;

//
//
//   Filter for trima filenames
//
static const CString lpstrFilter = "Trima data files|*.dat|All files|*.*||";


/////////////////////////////////////////////////////////////////////////////
// cfg_main_input

cfg_main_input::cfg_main_input(CWnd * pParent) :
   CDialog(cfg_main_input::IDD, pParent),
   m_modified(0)
{
	EnableAutomation();
	//{{AFX_DATA_INIT(cfg_main_input)
	m_percent_tbv = 0.0f;
	m_remove_weight = 0.0f;
	m_remove_weight_less_than = 0.0f;
	m_remove_weight_greater_than = 0.0f;
	m_height_units = -1;
	m_infusion_rate = -1;
	m_language = -1;
	m_max_procedure_time = 0.0f;
	m_minimum_postcount = 0.0f;
	m_minimum_postcrit = 0.0f;
	m_plt1_concentration = 0.0f;
	m_plt1_volume = 0.0f;
	m_plt1_yield = 0.0f;
	m_plt2_concentration = 0.0f;
	m_plt2_volume = 0.0f;
	m_plt2_yield = 0.0f;
	m_plt3_concentration = 0.0f;
	m_plt3_volume = 0.0f;
	m_plt3_yield = 0.0f;
	m_weight_units = -1;
	m_ysf = 0.0f;
	m_rbc_ratio_index = -1;
	m_replacement_fluid_index = -1;
	m_replacement_fluid_percent = 0.0f;
	m_plt4_concentration = 0.0f;
	m_plt4_volume = 0.0f;
	m_plt4_yield = 0.0f;
	m_plt5_concentration = 0.0f;
	m_plt5_volume = 0.0f;
	m_plt5_yield = 0.0f;
	m_plt6_concentration = 0.0f;
	m_plt6_volume = 0.0f;
	m_plt6_yield = 0.0f;
	m_rbc_volume_1 = 0.0f;
	m_rbc_volume_2 = 0.0f;
	m_rbc_volume_3 = 0.0f;
	m_rbc_crit_1 = 0.0f;
	m_rbc_crit_2 = 0.0f;
	m_rbc_dose_1 = 0.0f;
	m_rbc_dose_2 = 0.0f;
	m_rbc_dose_3 = 0.0f;
	m_rbc_crit_3 = 0.0f;
	m_product_plasma_1 = -1;
	m_product_plasma_10 = -1;
	m_product_plasma_11 = -1;
	m_product_plasma_12 = -1;
	m_product_plasma_13 = -1;
	m_product_plasma_14 = -1;
	m_product_plasma_15 = -1;
	m_product_plasma_2 = -1;
	m_product_plasma_3 = -1;
	m_product_plasma_4 = -1;
	m_product_plasma_5 = -1;
	m_product_plasma_6 = -1;
	m_product_plasma_7 = -1;
	m_product_plasma_8 = -1;
	m_product_plasma_9 = -1;
	m_product_platelet_1 = -1;
	m_product_platelet_10 = -1;
	m_product_platelet_11 = -1;
	m_product_platelet_12 = -1;
	m_product_platelet_13 = -1;
	m_product_platelet_14 = -1;
	m_product_platelet_15 = -1;
	m_product_platelet_2 = -1;
	m_product_platelet_3 = -1;
	m_product_platelet_4 = -1;
	m_product_platelet_5 = -1;
	m_product_platelet_6 = -1;
	m_product_platelet_7 = -1;
	m_product_platelet_8 = -1;
	m_product_platelet_9 = -1;
	m_product_rbc_1 = -1;
	m_product_rbc_10 = -1;
	m_product_rbc_11 = -1;
	m_product_rbc_12 = -1;
	m_product_rbc_13 = -1;
	m_product_rbc_14 = -1;
	m_product_rbc_15 = -1;
	m_product_rbc_2 = -1;
	m_product_rbc_3 = -1;
	m_product_rbc_4 = -1;
	m_product_rbc_5 = -1;
	m_product_rbc_6 = -1;
	m_product_rbc_7 = -1;
	m_product_rbc_8 = -1;
	m_product_rbc_9 = -1;
	m_replace_volume_trigger = 0.0f;
	m_max_draw_flow = -1;
	m_product_blood_type_1 = -1;
	m_product_blood_type_10 = -1;
	m_product_blood_type_11 = -1;
	m_product_blood_type_12 = -1;
	m_product_blood_type_13 = -1;
	m_product_blood_type_14 = -1;
	m_product_blood_type_15 = -1;
	m_product_blood_type_2 = -1;
	m_product_blood_type_3 = -1;
	m_product_blood_type_4 = -1;
	m_product_blood_type_5 = -1;
	m_product_blood_type_6 = -1;
	m_product_blood_type_7 = -1;
	m_product_blood_type_8 = -1;
	m_product_blood_type_9 = -1;
	m_product_time_10 = 0.0f;
	m_product_time_1 = 0.0f;
	m_product_time_11 = 0.0f;
	m_product_time_12 = 0.0f;
	m_product_time_13 = 0.0f;
	m_product_time_14 = 0.0f;
	m_product_time_15 = 0.0f;
	m_product_time_2 = 0.0f;
	m_product_time_3 = 0.0f;
	m_product_time_4 = 0.0f;
	m_product_time_5 = 0.0f;
	m_product_time_6 = 0.0f;
	m_product_time_7 = 0.0f;
	m_product_time_8 = 0.0f;
	m_product_time_9 = 0.0f;
	m_custom_ratio = 0.0f;
	m_draw_management = -1;
	m_ml_per_kg_active = FALSE;
	m_ml_per_kg = 0.0f;
	m_rbc_ysf = 0.0f;
	m_tbv_limit_active = FALSE;
	m_volume_limit_active = FALSE;
	m_return_management = -1;
	m_inlet_ramp = -1;
	m_plasma_amap_max = 0;
	m_plasma_amap_min = 0;
	m_plasma1 = 0;
	m_plasma2 = 0;
	m_plasma3 = 0;
	m_plasma4 = 0;
	m_plasma5 = 0;
	m_plasma6 = 0;
	m_plasma_rinseback = -1;
	m_product_rbc_16 = -1;
	m_product_rbc_17 = -1;
	m_product_rbc_18 = -1;
	m_product_rbc_19 = -1;
	m_product_rbc_20 = -1;
	m_product_time_16 = 0.0f;
	m_product_time_17 = 0.0f;
	m_product_time_18 = 0.0f;
	m_product_time_19 = 0.0f;
	m_product_time_20 = 0.0f;
	m_product_blood_type_16 = -1;
	m_product_blood_type_17 = -1;
	m_product_blood_type_18 = -1;
	m_product_blood_type_19 = -1;
	m_product_blood_type_20 = -1;
	m_product_plasma_16 = -1;
	m_product_plasma_17 = -1;
	m_product_plasma_18 = -1;
	m_product_plasma_19 = -1;
	m_product_plasma_20 = -1;
	m_product_platelet_16 = -1;
	m_product_platelet_17 = -1;
	m_product_platelet_18 = -1;
	m_product_platelet_19 = -1;
	m_product_platelet_20 = -1;
	m_saline_rinseback = -1;
	m_plt_mss_1 = FALSE;
	m_plt_mss_2 = FALSE;
	m_plt_mss_3 = FALSE;
	m_plt_mss_4 = FALSE;
	m_plt_mss_5 = FALSE;
	m_plt_mss_6 = FALSE;
	m_rbc_mss_1 = FALSE;
	m_rbc_mss_2 = FALSE;
	m_rbc_mss_3 = FALSE;
	//}}AFX_DATA_INIT

   userconfig = new Config_CDS(ROLE_RW);
   configpath = "config.dat";
}

cfg_main_input::~cfg_main_input()
{
}

void cfg_main_input::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cfg_main_input)
	DDX_Text(pDX, IDC_PERCENT_TBV, m_percent_tbv);
	DDX_Text(pDX, IDC_REMOVE_WEIGHT, m_remove_weight);
	DDX_Text(pDX, IDC_VOLUME_LESS_THAN, m_remove_weight_less_than);
	DDX_Text(pDX, IDC_VOLUME_GREATER_THAN, m_remove_weight_greater_than);
	DDX_CBIndex(pDX, IDC_HEIGHT_UNITS, m_height_units);
	DDX_CBIndex(pDX, IDC_INFUSION_RATE, m_infusion_rate);
	DDX_CBIndex(pDX, IDC_LANGUAGE, m_language);
	DDX_Text(pDX, IDC_MAX_PROCEDURE_TIME, m_max_procedure_time);
	DDX_Text(pDX, IDC_MINIMUM_POSTCOUNT, m_minimum_postcount);
	DDX_Text(pDX, IDC_MINIMUM_POSTCRIT, m_minimum_postcrit);
	DDX_Text(pDX, IDC_PLT1_CONCENTRATION, m_plt1_concentration);
	DDX_Text(pDX, IDC_PLT1_VOLUME, m_plt1_volume);
	DDX_Text(pDX, IDC_PLT1_YIELD, m_plt1_yield);
	DDX_Text(pDX, IDC_PLT2_CONCENTRATION, m_plt2_concentration);
	DDX_Text(pDX, IDC_PLT2_VOLUME, m_plt2_volume);
	DDX_Text(pDX, IDC_PLT2_YIELD, m_plt2_yield);
	DDX_Text(pDX, IDC_PLT3_CONCENTRATION, m_plt3_concentration);
	DDX_Text(pDX, IDC_PLT3_VOLUME, m_plt3_volume);
	DDX_Text(pDX, IDC_PLT3_YIELD, m_plt3_yield);
	DDX_CBIndex(pDX, IDC_WEIGHT_UNITS, m_weight_units);
	DDX_Text(pDX, IDC_YSF, m_ysf);
	DDX_CBIndex(pDX, IDC_RBC_RATIO, m_rbc_ratio_index);
	DDX_CBIndex(pDX, IDC_REPLACEMENT_FLUID, m_replacement_fluid_index);
	DDX_Text(pDX, IDC_REPLACEMENT_FLUID_PERCENT, m_replacement_fluid_percent);
	DDX_Text(pDX, IDC_PLT4_CONCENTRATION, m_plt4_concentration);
	DDX_Text(pDX, IDC_PLT4_VOLUME, m_plt4_volume);
	DDX_Text(pDX, IDC_PLT4_YIELD, m_plt4_yield);
	DDX_Text(pDX, IDC_PLT5_CONCENTRATION, m_plt5_concentration);
	DDX_Text(pDX, IDC_PLT5_VOLUME, m_plt5_volume);
	DDX_Text(pDX, IDC_PLT5_YIELD, m_plt5_yield);
	DDX_Text(pDX, IDC_PLT6_CONCENTRATION, m_plt6_concentration);
	DDX_Text(pDX, IDC_PLT6_VOLUME, m_plt6_volume);
	DDX_Text(pDX, IDC_PLT6_YIELD, m_plt6_yield);
	DDX_Text(pDX, IDC_RBC_COLLECT_VOLUME_1, m_rbc_volume_1);
	DDX_Text(pDX, IDC_RBC_COLLECT_VOLUME_2, m_rbc_volume_2);
	DDX_Text(pDX, IDC_RBC_COLLECT_VOLUME_3, m_rbc_volume_3);
	DDX_Text(pDX, IDC_RBC_CRIT_1, m_rbc_crit_1);
	DDX_Text(pDX, IDC_RBC_CRIT_2, m_rbc_crit_2);
	DDX_Text(pDX, IDC_RBC_DOSE_1, m_rbc_dose_1);
	DDX_Text(pDX, IDC_RBC_DOSE_2, m_rbc_dose_2);
	DDX_Text(pDX, IDC_RBC_DOSE_3, m_rbc_dose_3);
	DDX_Text(pDX, IDC_RBC_CRIT_3, m_rbc_crit_3);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_1, m_product_plasma_1);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_10, m_product_plasma_10);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_11, m_product_plasma_11);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_12, m_product_plasma_12);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_13, m_product_plasma_13);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_14, m_product_plasma_14);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_15, m_product_plasma_15);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_2, m_product_plasma_2);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_3, m_product_plasma_3);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_4, m_product_plasma_4);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_5, m_product_plasma_5);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_6, m_product_plasma_6);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_7, m_product_plasma_7);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_8, m_product_plasma_8);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_9, m_product_plasma_9);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_1, m_product_platelet_1);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_10, m_product_platelet_10);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_11, m_product_platelet_11);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_12, m_product_platelet_12);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_13, m_product_platelet_13);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_14, m_product_platelet_14);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_15, m_product_platelet_15);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_2, m_product_platelet_2);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_3, m_product_platelet_3);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_4, m_product_platelet_4);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_5, m_product_platelet_5);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_6, m_product_platelet_6);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_7, m_product_platelet_7);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_8, m_product_platelet_8);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_9, m_product_platelet_9);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_1, m_product_rbc_1);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_10, m_product_rbc_10);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_11, m_product_rbc_11);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_12, m_product_rbc_12);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_13, m_product_rbc_13);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_14, m_product_rbc_14);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_15, m_product_rbc_15);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_2, m_product_rbc_2);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_3, m_product_rbc_3);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_4, m_product_rbc_4);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_5, m_product_rbc_5);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_6, m_product_rbc_6);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_7, m_product_rbc_7);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_8, m_product_rbc_8);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_9, m_product_rbc_9);
	DDX_Text(pDX, IDC_REPLACEMENT_VOLUME_TRIGGER, m_replace_volume_trigger);
	DDX_CBIndex(pDX, IDC_MAX_DRAW_FLOW, m_max_draw_flow);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_1, m_product_blood_type_1);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_10, m_product_blood_type_10);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_11, m_product_blood_type_11);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_12, m_product_blood_type_12);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_13, m_product_blood_type_13);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_14, m_product_blood_type_14);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_15, m_product_blood_type_15);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_2, m_product_blood_type_2);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_3, m_product_blood_type_3);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_4, m_product_blood_type_4);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_5, m_product_blood_type_5);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_6, m_product_blood_type_6);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_7, m_product_blood_type_7);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_8, m_product_blood_type_8);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_9, m_product_blood_type_9);
	DDX_Text(pDX, IDC_PRODUCT_TIME_10, m_product_time_10);
	DDX_Text(pDX, IDC_PRODUCT_TIME_1, m_product_time_1);
	DDX_Text(pDX, IDC_PRODUCT_TIME_11, m_product_time_11);
	DDX_Text(pDX, IDC_PRODUCT_TIME_12, m_product_time_12);
	DDX_Text(pDX, IDC_PRODUCT_TIME_13, m_product_time_13);
	DDX_Text(pDX, IDC_PRODUCT_TIME_14, m_product_time_14);
	DDX_Text(pDX, IDC_PRODUCT_TIME_15, m_product_time_15);
	DDX_Text(pDX, IDC_PRODUCT_TIME_2, m_product_time_2);
	DDX_Text(pDX, IDC_PRODUCT_TIME_3, m_product_time_3);
	DDX_Text(pDX, IDC_PRODUCT_TIME_4, m_product_time_4);
	DDX_Text(pDX, IDC_PRODUCT_TIME_5, m_product_time_5);
	DDX_Text(pDX, IDC_PRODUCT_TIME_6, m_product_time_6);
	DDX_Text(pDX, IDC_PRODUCT_TIME_7, m_product_time_7);
	DDX_Text(pDX, IDC_PRODUCT_TIME_8, m_product_time_8);
	DDX_Text(pDX, IDC_PRODUCT_TIME_9, m_product_time_9);
	DDX_Text(pDX, IDC_CUSTOM_RATIO, m_custom_ratio);
	DDX_CBIndex(pDX, IDC_DRAW_MANAGEMENT, m_draw_management);
	DDX_Check(pDX, IDC_ML_PER_KG_LIMIT_ACTIVE, m_ml_per_kg_active);
	DDX_Text(pDX, IDC_ML_PER_KG, m_ml_per_kg);
	DDX_Text(pDX, IDC_RBC_YSF, m_rbc_ysf);
	DDX_Check(pDX, IDC_TBV_LIMIT_ACTIVE, m_tbv_limit_active);
	DDX_Check(pDX, IDC_VOLUME_LIMIT_ACTIVE, m_volume_limit_active);
	DDX_CBIndex(pDX, IDC_RETURN_MANAGEMENT, m_return_management);
	DDX_CBIndex(pDX, IDC_INLET_RAMP, m_inlet_ramp);
	DDX_Text(pDX, IDC_PLASMA_AMAP_MAX, m_plasma_amap_max);
	DDX_Text(pDX, IDC_PLASMA_AMAP_MIN, m_plasma_amap_min);
	DDX_Text(pDX, IDC_PLASMA_1, m_plasma1);
	DDX_Text(pDX, IDC_PLASMA_2, m_plasma2);
	DDX_Text(pDX, IDC_PLASMA_3, m_plasma3);
	DDX_Text(pDX, IDC_PLASMA_4, m_plasma4);
	DDX_Text(pDX, IDC_PLASMA_5, m_plasma5);
	DDX_Text(pDX, IDC_PLASMA_6, m_plasma6);
	DDX_CBIndex(pDX, IDC_PLASMA_RINSEBACK, m_plasma_rinseback);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_16, m_product_rbc_16);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_17, m_product_rbc_17);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_18, m_product_rbc_18);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_19, m_product_rbc_19);
	DDX_CBIndex(pDX, IDC_PRODUCT_RBC_20, m_product_rbc_20);
	DDX_Text(pDX, IDC_PRODUCT_TIME_16, m_product_time_16);
	DDX_Text(pDX, IDC_PRODUCT_TIME_17, m_product_time_17);
	DDX_Text(pDX, IDC_PRODUCT_TIME_18, m_product_time_18);
	DDX_Text(pDX, IDC_PRODUCT_TIME_19, m_product_time_19);
	DDX_Text(pDX, IDC_PRODUCT_TIME_20, m_product_time_20);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_16, m_product_blood_type_16);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_17, m_product_blood_type_17);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_18, m_product_blood_type_18);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_19, m_product_blood_type_19);
	DDX_CBIndex(pDX, IDC_PRODUCT_BLOOD_TYPE_20, m_product_blood_type_20);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_16, m_product_plasma_16);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_17, m_product_plasma_17);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_18, m_product_plasma_18);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_19, m_product_plasma_19);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLASMA_20, m_product_plasma_20);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_16, m_product_platelet_16);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_17, m_product_platelet_17);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_18, m_product_platelet_18);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_19, m_product_platelet_19);
	DDX_CBIndex(pDX, IDC_PRODUCT_PLATELET_20, m_product_platelet_20);
	DDX_CBIndex(pDX, IDC_SALINE_RINSEBACK, m_saline_rinseback);
	DDX_Check(pDX, IDC_PLT_MSS_CHECK1, m_plt_mss_1);
	DDX_Check(pDX, IDC_PLT_MSS_CHECK2, m_plt_mss_2);
	DDX_Check(pDX, IDC_PLT_MSS_CHECK3, m_plt_mss_3);
	DDX_Check(pDX, IDC_PLT_MSS_CHECK4, m_plt_mss_4);
	DDX_Check(pDX, IDC_PLT_MSS_CHECK5, m_plt_mss_5);
	DDX_Check(pDX, IDC_PLT_MSS_CHECK6, m_plt_mss_6);
	DDX_Check(pDX, IDC_RBC_MSS_CHECK1, m_rbc_mss_1);
	DDX_Check(pDX, IDC_RBC_MSS_CHECK2, m_rbc_mss_2);
	DDX_Check(pDX, IDC_RBC_MSS_CHECK3, m_rbc_mss_3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cfg_main_input, CDialog)
	//{{AFX_MSG_MAP(cfg_main_input)
	ON_BN_CLICKED(IDC_LOAD_FILE, OnLoadFile)
	ON_BN_CLICKED(IDC_SAVE_FILE, OnSaveFile)
	ON_CBN_CLOSEUP(IDC_HEIGHT_UNITS, OnCloseupHeightUnits)
	ON_CBN_CLOSEUP(IDC_WEIGHT_UNITS, OnCloseupWeightUnits)
	ON_EN_KILLFOCUS(IDC_MAX_PROCEDURE_TIME, OnKillfocusMaxProcedureTime)
	ON_EN_KILLFOCUS(IDC_MINIMUM_POSTCOUNT, OnKillfocusMinimumPostcount)
	ON_EN_KILLFOCUS(IDC_MINIMUM_POSTCRIT, OnKillfocusMinimumPostcrit)
	ON_EN_KILLFOCUS(IDC_REMOVE_WEIGHT, OnKillfocusRemoveWeight)
	ON_CBN_CLOSEUP(IDC_INFUSION_RATE, OnCloseupInfusionRate)
	ON_EN_KILLFOCUS(IDC_YSF, OnKillfocusYsf)
	ON_EN_KILLFOCUS(IDC_PLT1_CONCENTRATION, OnKillfocusPlt1Concentration)
	ON_EN_KILLFOCUS(IDC_PLT1_VOLUME, OnKillfocusPlt1Volume)
	ON_EN_KILLFOCUS(IDC_PLT1_YIELD, OnKillfocusPlt1Yield)
	ON_EN_KILLFOCUS(IDC_PLT2_CONCENTRATION, OnKillfocusPlt2Concentration)
	ON_EN_KILLFOCUS(IDC_PLT2_VOLUME, OnKillfocusPlt2Volume)
	ON_EN_KILLFOCUS(IDC_PLT2_YIELD, OnKillfocusPlt2Yield)
	ON_EN_KILLFOCUS(IDC_PLT3_CONCENTRATION, OnKillfocusPlt3Concentration)
	ON_EN_KILLFOCUS(IDC_PLT3_VOLUME, OnKillfocusPlt3Volume)
	ON_EN_KILLFOCUS(IDC_PLT3_YIELD, OnKillfocusPlt3Yield)
	ON_EN_KILLFOCUS(IDC_VOLUME_LESS_THAN, OnKillfocusVolumeLessThan)
	ON_EN_KILLFOCUS(IDC_PERCENT_TBV, OnKillfocusPercentTbv)
	ON_EN_KILLFOCUS(IDC_VOLUME_GREATER_THAN, OnKillfocusVolumeGreaterThan)
	ON_CBN_CLOSEUP(IDC_REPLACEMENT_FLUID, OnCloseupReplacementFluid)
	ON_EN_KILLFOCUS(IDC_REPLACEMENT_FLUID_PERCENT, OnKillfocusReplacementFluidPercent)
	ON_CBN_CLOSEUP(IDC_RBC_RATIO, OnCloseupRbcRatio)
	ON_EN_KILLFOCUS(IDC_PLT4_CONCENTRATION, OnKillfocusPlt4Concentration)
	ON_EN_KILLFOCUS(IDC_PLT4_VOLUME, OnKillfocusPlt4Volume)
	ON_EN_KILLFOCUS(IDC_PLT4_YIELD, OnKillfocusPlt4Yield)
	ON_EN_KILLFOCUS(IDC_PLT5_CONCENTRATION, OnKillfocusPlt5Concentration)
	ON_EN_KILLFOCUS(IDC_PLT5_VOLUME, OnKillfocusPlt5Volume)
	ON_EN_KILLFOCUS(IDC_PLT5_YIELD, OnKillfocusPlt5Yield)
	ON_EN_KILLFOCUS(IDC_PLT6_CONCENTRATION, OnKillfocusPlt6Concentration)
	ON_EN_KILLFOCUS(IDC_PLT6_VOLUME, OnKillfocusPlt6Volume)
	ON_EN_KILLFOCUS(IDC_PLT6_YIELD, OnKillfocusPlt6Yield)
	ON_EN_KILLFOCUS(IDC_RBC_COLLECT_VOLUME_1, OnKillfocusRbcCollectVolume1)
	ON_EN_KILLFOCUS(IDC_RBC_COLLECT_VOLUME_2, OnKillfocusRbcCollectVolume2)
	ON_EN_KILLFOCUS(IDC_RBC_COLLECT_VOLUME_3, OnKillfocusRbcCollectVolume3)
	ON_EN_KILLFOCUS(IDC_RBC_CRIT_2, OnKillfocusRbcCrit2)
	ON_EN_KILLFOCUS(IDC_RBC_CRIT_3, OnKillfocusRbcCrit3)
	ON_EN_KILLFOCUS(IDC_RBC_DOSE_1, OnKillfocusRbcDose1)
	ON_EN_KILLFOCUS(IDC_RBC_DOSE_2, OnKillfocusRbcDose2)
	ON_EN_KILLFOCUS(IDC_RBC_DOSE_3, OnKillfocusRbcDose3)
	ON_EN_KILLFOCUS(IDC_RBC_CRIT_1, OnKillfocusRbcCrit1)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_10, OnUpdateProduct10)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_1, OnUpdateProduct1)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_11, OnUpdateProduct11)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_12, OnUpdateProduct12)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_13, OnUpdateProduct13)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_14, OnUpdateProduct14)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_15, OnUpdateProduct15)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_2, OnUpdateProduct2)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_3, OnUpdateProduct3)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_4, OnUpdateProduct4)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_5, OnUpdateProduct5)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_6, OnUpdateProduct6)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_7, OnUpdateProduct7)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_8, OnUpdateProduct8)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_9, OnUpdateProduct9)
	ON_CBN_CLOSEUP(IDC_MAX_DRAW_FLOW, OnCloseupMaxDrawFlow)
	ON_EN_KILLFOCUS(IDC_REPLACEMENT_VOLUME_TRIGGER, OnKillfocusReplacementVolumeTrigger)
	ON_WM_CLOSE()
	ON_CBN_CLOSEUP(IDC_RETURN_MANAGEMENT, OnCloseupReturnManagement)
	ON_EN_KILLFOCUS(IDC_RBC_YSF, OnKillfocusRbcYsf)
	ON_EN_KILLFOCUS(IDC_ML_PER_KG, OnKillfocusMlPerKg)
	ON_CBN_CLOSEUP(IDC_INLET_RAMP, OnCloseupInletRamp)
	ON_CBN_CLOSEUP(IDC_DRAW_MANAGEMENT, OnCloseupDrawManagement)
	ON_EN_KILLFOCUS(IDC_CUSTOM_RATIO, OnKillfocusCustomRatio)
	ON_BN_CLICKED(IDC_VOLUME_LIMIT_ACTIVE, OnUpdateRemovalSetting)
	ON_EN_CHANGE(IDC_PLASMA_1, OnChangePlasma1)
	ON_EN_CHANGE(IDC_PLASMA_2, OnChangePlasma2)
	ON_EN_CHANGE(IDC_PLASMA_3, OnChangePlasma3)
	ON_EN_CHANGE(IDC_PLASMA_4, OnChangePlasma4)
	ON_EN_CHANGE(IDC_PLASMA_5, OnChangePlasma5)
	ON_EN_CHANGE(IDC_PLASMA_6, OnChangePlasma6)
	ON_EN_CHANGE(IDC_PLASMA_AMAP_MAX, OnChangePlasmaAmapMax)
	ON_EN_CHANGE(IDC_PLASMA_AMAP_MIN, OnChangePlasmaAmapMin)
	ON_CBN_CLOSEUP(IDC_PLASMA_RINSEBACK, OnCloseupPlasmaRinseback)
	ON_CBN_EDITCHANGE(IDC_PRODUCT_PLATELET_16, OnUpdateProduct16)
	ON_CBN_EDITCHANGE(IDC_PRODUCT_PLATELET_17, OnUpdateProduct17)
	ON_CBN_EDITCHANGE(IDC_PRODUCT_PLATELET_18, OnUpdateProduct18)
	ON_CBN_EDITCHANGE(IDC_PRODUCT_PLATELET_19, OnUpdateProduct19)
	ON_CBN_EDITCHANGE(IDC_PRODUCT_PLATELET_20, OnUpdateProduct20)
	ON_BN_CLICKED(IDC_PLT_MSS_CHECK1, OnPltMssCheck1Click)
	ON_BN_CLICKED(IDC_PLT_MSS_CHECK2, OnPltMssCheck2Click)
	ON_BN_CLICKED(IDC_PLT_MSS_CHECK3, OnPltMssCheck3Click)
	ON_BN_CLICKED(IDC_PLT_MSS_CHECK4, OnPltMssCheck4Click)
	ON_BN_CLICKED(IDC_PLT_MSS_CHECK5, OnPltMssCheck5Click)
	ON_BN_CLICKED(IDC_PLT_MSS_CHECK6, OnPltMssCheck6Click)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_BLOOD_TYPE_10, OnUpdateProduct10)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_1, OnUpdateProduct1)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_10, OnUpdateProduct10)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_11, OnUpdateProduct11)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_12, OnUpdateProduct12)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_13, OnUpdateProduct13)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_14, OnUpdateProduct14)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_15, OnUpdateProduct15)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_2, OnUpdateProduct2)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_3, OnUpdateProduct3)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_4, OnUpdateProduct4)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_5, OnUpdateProduct5)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_6, OnUpdateProduct6)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_7, OnUpdateProduct7)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_8, OnUpdateProduct8)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLASMA_9, OnUpdateProduct9)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_1, OnUpdateProduct1)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_10, OnUpdateProduct10)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_11, OnUpdateProduct11)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_12, OnUpdateProduct12)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_13, OnUpdateProduct13)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_14, OnUpdateProduct14)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_15, OnUpdateProduct15)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_2, OnUpdateProduct2)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_3, OnUpdateProduct3)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_4, OnUpdateProduct4)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_5, OnUpdateProduct5)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_6, OnUpdateProduct6)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_7, OnUpdateProduct7)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_8, OnUpdateProduct8)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_PLATELET_9, OnUpdateProduct9)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_1, OnUpdateProduct1)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_10, OnUpdateProduct10)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_11, OnUpdateProduct11)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_12, OnUpdateProduct12)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_13, OnUpdateProduct13)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_14, OnUpdateProduct14)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_15, OnUpdateProduct15)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_2, OnUpdateProduct2)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_3, OnUpdateProduct3)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_4, OnUpdateProduct4)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_5, OnUpdateProduct5)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_6, OnUpdateProduct6)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_7, OnUpdateProduct7)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_8, OnUpdateProduct8)
	ON_CBN_KILLFOCUS(IDC_PRODUCT_RBC_9, OnUpdateProduct9)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_1, OnUpdateProduct1)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_11, OnUpdateProduct11)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_12, OnUpdateProduct12)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_13, OnUpdateProduct13)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_14, OnUpdateProduct14)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_15, OnUpdateProduct15)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_2, OnUpdateProduct2)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_3, OnUpdateProduct3)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_4, OnUpdateProduct4)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_5, OnUpdateProduct5)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_6, OnUpdateProduct6)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_7, OnUpdateProduct7)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_8, OnUpdateProduct8)
	ON_EN_KILLFOCUS(IDC_PRODUCT_TIME_9, OnUpdateProduct9)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_1, OnUpdateProduct1)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_1, OnUpdateProduct1)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_10, OnUpdateProduct10)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_11, OnUpdateProduct11)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_12, OnUpdateProduct12)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_13, OnUpdateProduct13)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_14, OnUpdateProduct14)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_15, OnUpdateProduct15)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_2, OnUpdateProduct2)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_3, OnUpdateProduct3)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_4, OnUpdateProduct4)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_5, OnUpdateProduct5)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_6, OnUpdateProduct6)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_7, OnUpdateProduct7)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_8, OnUpdateProduct8)
	ON_CBN_CLOSEUP(IDC_PRODUCT_BLOOD_TYPE_9, OnUpdateProduct9)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_1, OnUpdateProduct1)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_10, OnUpdateProduct10)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_11, OnUpdateProduct11)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_12, OnUpdateProduct12)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_13, OnUpdateProduct13)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_14, OnUpdateProduct14)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_15, OnUpdateProduct15)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_2, OnUpdateProduct2)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_3, OnUpdateProduct3)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_4, OnUpdateProduct4)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_5, OnUpdateProduct5)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_6, OnUpdateProduct6)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_7, OnUpdateProduct7)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_8, OnUpdateProduct8)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLASMA_9, OnUpdateProduct9)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_10, OnUpdateProduct10)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_11, OnUpdateProduct11)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_12, OnUpdateProduct12)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_13, OnUpdateProduct13)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_14, OnUpdateProduct14)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_15, OnUpdateProduct15)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_2, OnUpdateProduct2)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_3, OnUpdateProduct3)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_4, OnUpdateProduct4)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_5, OnUpdateProduct5)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_6, OnUpdateProduct6)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_7, OnUpdateProduct7)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_8, OnUpdateProduct8)
	ON_CBN_CLOSEUP(IDC_PRODUCT_PLATELET_9, OnUpdateProduct9)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_1, OnUpdateProduct1)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_10, OnUpdateProduct10)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_11, OnUpdateProduct11)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_12, OnUpdateProduct12)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_13, OnUpdateProduct13)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_14, OnUpdateProduct14)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_3, OnUpdateProduct3)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_15, OnUpdateProduct15)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_2, OnUpdateProduct2)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_4, OnUpdateProduct4)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_5, OnUpdateProduct5)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_6, OnUpdateProduct6)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_7, OnUpdateProduct7)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_8, OnUpdateProduct8)
	ON_CBN_CLOSEUP(IDC_PRODUCT_RBC_9, OnUpdateProduct9)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_1, OnUpdateProduct1)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_10, OnUpdateProduct10)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_11, OnUpdateProduct11)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_12, OnUpdateProduct12)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_13, OnUpdateProduct13)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_14, OnUpdateProduct14)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_15, OnUpdateProduct15)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_2, OnUpdateProduct2)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_3, OnUpdateProduct3)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_4, OnUpdateProduct4)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_5, OnUpdateProduct5)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_6, OnUpdateProduct6)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_7, OnUpdateProduct7)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_8, OnUpdateProduct8)
	ON_EN_UPDATE(IDC_PRODUCT_TIME_9, OnUpdateProduct9)
	ON_EN_UPDATE(IDC_MAX_PROCEDURE_TIME, OnKillfocusMaxProcedureTime)
	ON_EN_UPDATE(IDC_MINIMUM_POSTCOUNT, OnKillfocusMinimumPostcount)
	ON_EN_UPDATE(IDC_MINIMUM_POSTCRIT, OnKillfocusMinimumPostcrit)
	ON_EN_UPDATE(IDC_PERCENT_TBV, OnKillfocusPercentTbv)
	ON_EN_UPDATE(IDC_RBC_COLLECT_VOLUME_1, OnKillfocusRbcCollectVolume1)
	ON_EN_UPDATE(IDC_RBC_COLLECT_VOLUME_2, OnKillfocusRbcCollectVolume2)
	ON_EN_UPDATE(IDC_RBC_COLLECT_VOLUME_3, OnKillfocusRbcCollectVolume3)
	ON_EN_UPDATE(IDC_RBC_CRIT_1, OnKillfocusRbcCrit1)
	ON_EN_UPDATE(IDC_RBC_CRIT_2, OnKillfocusRbcCrit2)
	ON_EN_UPDATE(IDC_RBC_CRIT_3, OnKillfocusRbcCrit3)
	ON_EN_UPDATE(IDC_RBC_DOSE_1, OnKillfocusRbcDose1)
	ON_EN_UPDATE(IDC_RBC_DOSE_2, OnKillfocusRbcDose2)
	ON_EN_UPDATE(IDC_RBC_DOSE_3, OnKillfocusRbcDose3)
	ON_EN_UPDATE(IDC_REMOVE_WEIGHT, OnKillfocusRemoveWeight)
	ON_EN_CHANGE(IDC_YSF, OnKillfocusYsf)
	ON_EN_CHANGE(IDC_PLT6_YIELD, OnKillfocusPlt6Yield)
	ON_EN_CHANGE(IDC_PLT6_VOLUME, OnKillfocusPlt6Volume)
	ON_EN_CHANGE(IDC_PLT6_CONCENTRATION, OnKillfocusPlt6Concentration)
	ON_EN_CHANGE(IDC_PLT5_YIELD, OnKillfocusPlt5Yield)
	ON_EN_CHANGE(IDC_PLT5_VOLUME, OnKillfocusPlt5Volume)
	ON_EN_CHANGE(IDC_PLT5_CONCENTRATION, OnKillfocusPlt5Concentration)
	ON_EN_CHANGE(IDC_PLT4_YIELD, OnKillfocusPlt4Yield)
	ON_EN_CHANGE(IDC_PLT4_VOLUME, OnKillfocusPlt4Volume)
	ON_EN_CHANGE(IDC_PLT4_CONCENTRATION, OnKillfocusPlt4Concentration)
	ON_EN_CHANGE(IDC_PLT3_YIELD, OnKillfocusPlt3Yield)
	ON_EN_CHANGE(IDC_PLT3_VOLUME, OnKillfocusPlt3Volume)
	ON_EN_CHANGE(IDC_PLT3_CONCENTRATION, OnKillfocusPlt3Concentration)
	ON_EN_CHANGE(IDC_PLT2_YIELD, OnKillfocusPlt2Yield)
	ON_EN_CHANGE(IDC_PLT2_VOLUME, OnKillfocusPlt2Volume)
	ON_EN_CHANGE(IDC_PLT2_CONCENTRATION, OnKillfocusPlt2Concentration)
	ON_EN_CHANGE(IDC_PLT1_YIELD, OnKillfocusPlt1Yield)
	ON_EN_CHANGE(IDC_PLT1_VOLUME, OnKillfocusPlt1Volume)
	ON_EN_CHANGE(IDC_PLT1_CONCENTRATION, OnKillfocusPlt1Concentration)
	ON_EN_UPDATE(IDC_VOLUME_LESS_THAN, OnKillfocusVolumeLessThan)
	ON_EN_UPDATE(IDC_VOLUME_GREATER_THAN, OnKillfocusVolumeGreaterThan)
	ON_EN_UPDATE(IDC_REPLACEMENT_VOLUME_TRIGGER, OnKillfocusReplacementVolumeTrigger)
	ON_EN_UPDATE(IDC_REPLACEMENT_FLUID_PERCENT, OnKillfocusReplacementFluidPercent)
	ON_EN_UPDATE(IDC_YSF, OnKillfocusYsf)
	ON_EN_CHANGE(IDC_RBC_YSF, OnKillfocusRbcYsf)
	ON_EN_CHANGE(IDC_VOLUME_LESS_THAN, OnKillfocusVolumeLessThan)
	ON_EN_CHANGE(IDC_VOLUME_GREATER_THAN, OnKillfocusVolumeGreaterThan)
	ON_EN_CHANGE(IDC_REPLACEMENT_VOLUME_TRIGGER, OnKillfocusReplacementVolumeTrigger)
	ON_EN_CHANGE(IDC_REPLACEMENT_FLUID_PERCENT, OnKillfocusReplacementFluidPercent)
	ON_EN_CHANGE(IDC_REMOVE_WEIGHT, OnKillfocusRemoveWeight)
	ON_EN_CHANGE(IDC_RBC_DOSE_3, OnKillfocusRbcDose3)
	ON_EN_CHANGE(IDC_RBC_DOSE_2, OnKillfocusRbcDose2)
	ON_EN_CHANGE(IDC_RBC_DOSE_1, OnKillfocusRbcDose1)
	ON_EN_CHANGE(IDC_RBC_CRIT_3, OnKillfocusRbcCrit3)
	ON_EN_CHANGE(IDC_RBC_CRIT_2, OnKillfocusRbcCrit2)
	ON_EN_CHANGE(IDC_RBC_CRIT_1, OnKillfocusRbcCrit1)
	ON_EN_CHANGE(IDC_RBC_COLLECT_VOLUME_3, OnKillfocusRbcCollectVolume3)
	ON_EN_CHANGE(IDC_RBC_COLLECT_VOLUME_2, OnKillfocusRbcCollectVolume2)
	ON_EN_CHANGE(IDC_RBC_COLLECT_VOLUME_1, OnKillfocusRbcCollectVolume1)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_9, OnUpdateProduct9)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_8, OnUpdateProduct8)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_7, OnUpdateProduct7)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_6, OnUpdateProduct6)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_5, OnUpdateProduct5)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_4, OnUpdateProduct4)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_3, OnUpdateProduct3)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_2, OnUpdateProduct2)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_15, OnUpdateProduct15)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_14, OnUpdateProduct14)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_13, OnUpdateProduct13)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_12, OnUpdateProduct12)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_11, OnUpdateProduct11)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_10, OnUpdateProduct10)
	ON_EN_CHANGE(IDC_PRODUCT_TIME_1, OnUpdateProduct1)
	ON_CBN_KILLFOCUS(IDC_WEIGHT_UNITS, OnCloseupWeightUnits)
	ON_CBN_KILLFOCUS(IDC_RETURN_MANAGEMENT, OnCloseupReturnManagement)
	ON_CBN_KILLFOCUS(IDC_REPLACEMENT_FLUID, OnCloseupReplacementFluid)
	ON_CBN_KILLFOCUS(IDC_RBC_RATIO, OnCloseupRbcRatio)
	ON_EN_UPDATE(IDC_PLT6_YIELD, OnKillfocusPlt6Yield)
	ON_EN_UPDATE(IDC_PLT6_VOLUME, OnKillfocusPlt6Volume)
	ON_EN_UPDATE(IDC_PLT6_CONCENTRATION, OnKillfocusPlt6Concentration)
	ON_EN_UPDATE(IDC_PLT5_YIELD, OnKillfocusPlt5Yield)
	ON_EN_UPDATE(IDC_PLT5_VOLUME, OnKillfocusPlt5Volume)
	ON_EN_UPDATE(IDC_PLT5_CONCENTRATION, OnKillfocusPlt5Concentration)
	ON_EN_UPDATE(IDC_PLT4_YIELD, OnKillfocusPlt4Yield)
	ON_EN_UPDATE(IDC_PLT4_VOLUME, OnKillfocusPlt4Volume)
	ON_EN_UPDATE(IDC_PLT4_CONCENTRATION, OnKillfocusPlt4Concentration)
	ON_EN_UPDATE(IDC_PLT3_YIELD, OnKillfocusPlt3Yield)
	ON_EN_UPDATE(IDC_PLT3_VOLUME, OnKillfocusPlt3Volume)
	ON_EN_UPDATE(IDC_PLT3_CONCENTRATION, OnKillfocusPlt3Concentration)
	ON_EN_UPDATE(IDC_PLT2_YIELD, OnKillfocusPlt2Yield)
	ON_EN_UPDATE(IDC_PLT2_VOLUME, OnKillfocusPlt2Volume)
	ON_EN_UPDATE(IDC_PLT2_CONCENTRATION, OnKillfocusPlt2Concentration)
	ON_EN_UPDATE(IDC_PLT1_YIELD, OnKillfocusPlt1Yield)
	ON_EN_UPDATE(IDC_PLT1_VOLUME, OnKillfocusPlt1Volume)
	ON_EN_UPDATE(IDC_PLT1_CONCENTRATION, OnKillfocusPlt1Concentration)
	ON_EN_CHANGE(IDC_PERCENT_TBV, OnKillfocusPercentTbv)
	ON_EN_CHANGE(IDC_ML_PER_KG, OnKillfocusMlPerKg)
	ON_EN_UPDATE(IDC_ML_PER_KG, OnKillfocusMlPerKg)
	ON_EN_CHANGE(IDC_MINIMUM_POSTCRIT, OnKillfocusMinimumPostcrit)
	ON_EN_CHANGE(IDC_MINIMUM_POSTCOUNT, OnKillfocusMinimumPostcount)
	ON_EN_CHANGE(IDC_MAX_PROCEDURE_TIME, OnKillfocusMaxProcedureTime)
	ON_CBN_KILLFOCUS(IDC_MAX_DRAW_FLOW, OnCloseupMaxDrawFlow)
	ON_CBN_KILLFOCUS(IDC_INLET_RAMP, OnCloseupInletRamp)
	ON_CBN_KILLFOCUS(IDC_INFUSION_RATE, OnCloseupInfusionRate)
	ON_CBN_KILLFOCUS(IDC_HEIGHT_UNITS, OnCloseupHeightUnits)
	ON_CBN_KILLFOCUS(IDC_DRAW_MANAGEMENT, OnCloseupDrawManagement)
	ON_EN_CHANGE(IDC_CUSTOM_RATIO, OnKillfocusCustomRatio)
	ON_EN_UPDATE(IDC_CUSTOM_RATIO, OnKillfocusCustomRatio)
	ON_BN_CLICKED(IDC_TBV_LIMIT_ACTIVE, OnUpdateRemovalSetting)
	ON_BN_CLICKED(IDC_ML_PER_KG_LIMIT_ACTIVE, OnUpdateRemovalSetting)
	ON_EN_UPDATE(IDC_RBC_YSF, OnKillfocusRbcYsf)
	ON_EN_KILLFOCUS(IDC_PLASMA_1, OnChangePlasma1)
	ON_EN_UPDATE(IDC_PLASMA_1, OnChangePlasma1)
	ON_EN_KILLFOCUS(IDC_PLASMA_2, OnChangePlasma2)
	ON_EN_UPDATE(IDC_PLASMA_2, OnChangePlasma2)
	ON_EN_KILLFOCUS(IDC_PLASMA_3, OnChangePlasma3)
	ON_EN_UPDATE(IDC_PLASMA_3, OnChangePlasma3)
	ON_EN_KILLFOCUS(IDC_PLASMA_4, OnChangePlasma4)
	ON_EN_UPDATE(IDC_PLASMA_4, OnChangePlasma4)
	ON_EN_KILLFOCUS(IDC_PLASMA_5, OnChangePlasma5)
	ON_EN_UPDATE(IDC_PLASMA_5, OnChangePlasma5)
	ON_EN_KILLFOCUS(IDC_PLASMA_6, OnChangePlasma6)
	ON_EN_UPDATE(IDC_PLASMA_6, OnChangePlasma6)
	ON_EN_KILLFOCUS(IDC_PLASMA_AMAP_MAX, OnChangePlasmaAmapMax)
	ON_EN_UPDATE(IDC_PLASMA_AMAP_MAX, OnChangePlasmaAmapMax)
	ON_EN_KILLFOCUS(IDC_PLASMA_AMAP_MIN, OnChangePlasmaAmapMin)
	ON_EN_UPDATE(IDC_PLASMA_AMAP_MIN, OnChangePlasmaAmapMin)
	ON_CBN_KILLFOCUS(IDC_PLASMA_RINSEBACK, OnCloseupPlasmaRinseback)
	ON_BN_CLICKED(IDC_RBC_MSS_CHECK1, OnRbcMssCheck1Click)
	ON_BN_CLICKED(IDC_RBC_MSS_CHECK2, OnRbcMssCheck2Click)
	ON_BN_CLICKED(IDC_RBC_MSS_CHECK3, OnRbcMssCheck3Click)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(cfg_main_input, CDialog)
	//{{AFX_DISPATCH_MAP(cfg_main_input)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_Icfg_main_input to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {AD0CDA07-BBA2-11D1-B0F8-006097402E62}
static const IID IID_Icfg_main_input =
{ 0xad0cda07, 0xbba2, 0x11d1, { 0xb0, 0xf8, 0x0, 0x60, 0x97, 0x40, 0x2e, 0x62 } };

BEGIN_INTERFACE_MAP(cfg_main_input, CDialog)
	INTERFACE_PART(cfg_main_input, IID_Icfg_main_input, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cfg_main_input diagnostics

/////////////////////////////////////////////////////////////////////////////
// cfg_main_input message handlers

bool cfg_main_input::update_config(const char *path)
{
   configpath = path;
   return load();
}

bool cfg_main_input::load()
{
   const bool success = userconfig->ReadConfig(configpath);

   if (!success)
      return false;

   int tempint;

   m_language = 0; // m_language = (int)temp; force to english until language support added

   //
   //         The following code assumes that the combo box definitions
   //         match the Trima GUI configurations in cfg_keys.h
   //
   m_height_units = userconfig->LangUnit.Get().key_height;
   m_weight_units = userconfig->LangUnit.Get().key_weight;

   
   m_max_procedure_time = userconfig->Procedure.Get().key_proc_time;
   m_custom_ratio = userconfig->Procedure.Get().key_custom_ratio;
   m_infusion_rate = userconfig->Procedure.Get().key_ac_rate-1;
   m_minimum_postcrit = userconfig->Procedure.Get().key_post_crit;
   m_plasma_rinseback = userconfig->Procedure.Get().key_plasma_rinseback;

   tempint = userconfig->Procedure.Get().key_tbv_vol_setting;
      m_tbv_limit_active = ((tempint & VOLUME_REMOVAL_TBV) != 0);
      m_volume_limit_active = ((tempint & VOLUME_REMOVAL_WEIGHT) != 0);
      m_ml_per_kg_active = ((tempint & VOLUME_REMOVAL_ML_PER_KG) != 0);

   // weight setting always kept in lbs
   m_remove_weight = userconfig->Procedure.Get().key_weight_1_setting;
   m_percent_tbv = userconfig->Procedure.Get().key_tbv_percent;
   m_remove_weight_less_than = userconfig->Procedure.Get().key_weight_1_less_than_vol;
   m_remove_weight_greater_than = userconfig->Procedure.Get().key_weight_3_greater_than_vol;
   m_ml_per_kg = userconfig->Procedure.Get().key_ml_per_kg;
   m_minimum_postcount = userconfig->Procedure.Get().key_post_plat;
   m_max_draw_flow = userconfig->Procedure.Get().key_max_draw_flow;
   m_replace_volume_trigger = userconfig->Procedure.Get().key_min_replacement_volume;

   m_rbc_ratio_index = userconfig->Predict.Get().key_rbc_ratio;
   m_replacement_fluid_index = userconfig->Predict.Get().key_rbc_replace_fluid;
   m_replacement_fluid_percent = userconfig->Predict.Get().key_rbc_fluid_percent;
   m_plasma_amap_min = userconfig->Predict.Get().key_pls_amap_min;
   m_plasma_amap_max = userconfig->Predict.Get().key_pls_amap_max;
   m_ysf = userconfig->Predict.Get().key_ysf;
   m_draw_management = userconfig->Predict.Get().key_inlet_management -1;
   m_inlet_ramp = userconfig->Predict.Get().key_inlet_flow_ramp;
   m_return_management = userconfig->Predict.Get().key_return_management -1;
   m_rbc_ysf = userconfig->Predict.Get().key_rsf;

   m_plasma1 = userconfig->Templates.Get().key_pls_volume[0];
   m_plasma2 = userconfig->Templates.Get().key_pls_volume[1];
   m_plasma3 = userconfig->Templates.Get().key_pls_volume[2];
   m_plasma4 = userconfig->Templates.Get().key_pls_volume[3];
   m_plasma5 = userconfig->Templates.Get().key_pls_volume[4];
   m_plasma6 = userconfig->Templates.Get().key_pls_volume[5];
   m_rbc_crit_1 = userconfig->Templates.Get().key_rbc_crit[0];
   m_rbc_dose_1 = userconfig->Templates.Get().key_rbc_dose[0];
   m_rbc_crit_2 = userconfig->Templates.Get().key_rbc_crit[1];
   m_rbc_dose_2 = userconfig->Templates.Get().key_rbc_dose[1];
   m_rbc_crit_3 = userconfig->Templates.Get().key_rbc_crit[2];
   m_rbc_dose_3 = userconfig->Templates.Get().key_rbc_dose[2];
   m_plt1_yield = userconfig->Templates.Get().key_plt_yield[0];
   m_plt1_volume = userconfig->Templates.Get().key_plt_volume[0];;
   m_plt2_yield = userconfig->Templates.Get().key_plt_yield[1];
   m_plt2_volume = userconfig->Templates.Get().key_plt_volume[1];
   m_plt3_yield = userconfig->Templates.Get().key_plt_yield[2];
   m_plt3_volume = userconfig->Templates.Get().key_plt_volume[2];
   m_plt4_yield = userconfig->Templates.Get().key_plt_yield[3];
   m_plt4_volume = userconfig->Templates.Get().key_plt_volume[3];
   m_plt5_yield = userconfig->Templates.Get().key_plt_yield[4];
   m_plt5_volume = userconfig->Templates.Get().key_plt_volume[4];
   m_plt6_yield = userconfig->Templates.Get().key_plt_yield[5];
   m_plt6_volume = userconfig->Templates.Get().key_plt_volume[5];
   

   m_plt1_concentration = (m_plt1_yield / m_plt1_volume) * 100000.0f;
   m_plt2_concentration = (m_plt2_yield / m_plt2_volume) * 100000.0f;
   m_plt3_concentration = (m_plt3_yield / m_plt3_volume) * 100000.0f;
   m_plt4_concentration = (m_plt4_yield / m_plt4_volume) * 100000.0f;
   m_plt5_concentration = (m_plt5_yield / m_plt5_volume) * 100000.0f;
   m_plt6_concentration = (m_plt6_yield / m_plt6_volume) * 100000.0f;

   m_rbc_volume_1 = m_rbc_dose_1 / (m_rbc_crit_1/100.0f) / m_rbc_ysf;
   m_rbc_volume_2 = m_rbc_dose_2 / (m_rbc_crit_2/100.0f) / m_rbc_ysf;
   m_rbc_volume_3 = m_rbc_dose_3 / (m_rbc_crit_3/100.0f) / m_rbc_ysf;

   const ProductDefinitionArray &defs = userconfig->Definition.Get();
   m_product_platelet_1 = defs[0].key_platelet;
   m_product_platelet_2 = defs[1].key_platelet;
   m_product_platelet_3 = defs[2].key_platelet;
   m_product_platelet_4 = defs[3].key_platelet;
   m_product_platelet_5 = defs[4].key_platelet;
   m_product_platelet_6 = defs[5].key_platelet;
   m_product_platelet_7 = defs[6].key_platelet;
   m_product_platelet_8 = defs[7].key_platelet;
   m_product_platelet_9 = defs[8].key_platelet;
   m_product_platelet_10 = defs[9].key_platelet;
   m_product_platelet_11 = defs[10].key_platelet;
   m_product_platelet_12 = defs[11].key_platelet;
   m_product_platelet_13 = defs[12].key_platelet;
   m_product_platelet_14 = defs[13].key_platelet;
   m_product_platelet_15 = defs[14].key_platelet;
   m_product_platelet_16 = defs[15].key_platelet;
   m_product_platelet_17 = defs[16].key_platelet;
   m_product_platelet_18 = defs[17].key_platelet;
   m_product_platelet_19 = defs[18].key_platelet;
   m_product_platelet_20 = defs[19].key_platelet;

   m_product_plasma_1 = defs[0].key_plasma;
   m_product_plasma_2 = defs[1].key_plasma;
   m_product_plasma_3 = defs[2].key_plasma;
   m_product_plasma_4 = defs[3].key_plasma;
   m_product_plasma_5 = defs[4].key_plasma;
   m_product_plasma_6 = defs[5].key_plasma;
   m_product_plasma_7 = defs[6].key_plasma;
   m_product_plasma_8 = defs[7].key_plasma;
   m_product_plasma_9 = defs[8].key_plasma;
   m_product_plasma_10 = defs[9].key_plasma;
   m_product_plasma_11 = defs[10].key_plasma;
   m_product_plasma_12 = defs[11].key_plasma;
   m_product_plasma_13 = defs[12].key_plasma;
   m_product_plasma_14 = defs[13].key_plasma;
   m_product_plasma_15 = defs[14].key_plasma;
   m_product_plasma_16 = defs[15].key_plasma;
   m_product_plasma_17 = defs[16].key_plasma;
   m_product_plasma_18 = defs[17].key_plasma;
   m_product_plasma_19 = defs[18].key_plasma;
   m_product_plasma_20 = defs[19].key_plasma;
   

   m_product_rbc_1 = defs[0].key_rbc;
   m_product_rbc_2 = defs[1].key_rbc;
   m_product_rbc_3 = defs[2].key_rbc;
   m_product_rbc_4 = defs[3].key_rbc;
   m_product_rbc_5 = defs[4].key_rbc ;
   m_product_rbc_6 = defs[5].key_rbc;
   m_product_rbc_7 = defs[6].key_rbc;
   m_product_rbc_8 = defs[7].key_rbc;
   m_product_rbc_9 = defs[8].key_rbc;
   m_product_rbc_10 = defs[9].key_rbc ;
   m_product_rbc_11 = defs[10].key_rbc ;
   m_product_rbc_12 = defs[11].key_rbc;
   m_product_rbc_13 = defs[12].key_rbc ;
   m_product_rbc_14 = defs[13].key_rbc;
   m_product_rbc_15 = defs[14].key_rbc;
   m_product_rbc_16 = defs[15].key_rbc ;
   m_product_rbc_17 = defs[16].key_rbc;
   m_product_rbc_18 = defs[17].key_rbc ;
   m_product_rbc_19 = defs[18].key_rbc;
   m_product_rbc_20 = defs[19].key_rbc;

//  key_blood_type_ : 0=all; 1=O+/O-/A+/A-; 2=O+/O-; 3=O-; 4=O-/A-; 5=A-/A+; 6=B-/B+; 7=AB-/AB+
   m_product_blood_type_1 = defs[0].key_blood_type;
   m_product_blood_type_2 = defs[1].key_blood_type;
   m_product_blood_type_3 = defs[2].key_blood_type;
   m_product_blood_type_4 = defs[3].key_blood_type;
   m_product_blood_type_5 = defs[4].key_blood_type;
   m_product_blood_type_6 = defs[5].key_blood_type;
   m_product_blood_type_7 = defs[6].key_blood_type;
   m_product_blood_type_8 = defs[7].key_blood_type;
   m_product_blood_type_9 = defs[8].key_blood_type;
   m_product_blood_type_10 = defs[9].key_blood_type;
   m_product_blood_type_11 = defs[10].key_blood_type;
   m_product_blood_type_12 = defs[11].key_blood_type;
   m_product_blood_type_13 = defs[12].key_blood_type;
   m_product_blood_type_14 = defs[13].key_blood_type;
   m_product_blood_type_15 = defs[14].key_blood_type;
   m_product_blood_type_16 = defs[15].key_blood_type;
   m_product_blood_type_17 = defs[16].key_blood_type;
   m_product_blood_type_18 = defs[17].key_blood_type;
   m_product_blood_type_19 = defs[18].key_blood_type;
   m_product_blood_type_20 = defs[19].key_blood_type;

   m_product_time_1 = defs[0].key_time;
   m_product_time_2 = defs[1].key_time;
   m_product_time_3 = defs[2].key_time;
   m_product_time_4 = defs[3].key_time;
   m_product_time_5 = defs[4].key_time;
   m_product_time_6 = defs[5].key_time;
   m_product_time_7 = defs[6].key_time;
   m_product_time_8 = defs[7].key_time;
   m_product_time_9 = defs[8].key_time;
   m_product_time_10 = defs[9].key_time;
   m_product_time_11 = defs[10].key_time;
   m_product_time_12 = defs[11].key_time;
   m_product_time_13 = defs[12].key_time;
   m_product_time_14 = defs[13].key_time;
   m_product_time_15 = defs[14].key_time;
   m_product_time_16 = defs[15].key_time;
   m_product_time_17 = defs[16].key_time;
   m_product_time_18 = defs[17].key_time;
   m_product_time_19 = defs[18].key_time;
   m_product_time_20 = defs[19].key_time;
   
   m_plt_mss_1 = userconfig->Templates.Get().key_plt_mss[0];
   m_plt_mss_2 = userconfig->Templates.Get().key_plt_mss[1];
   m_plt_mss_3 = userconfig->Templates.Get().key_plt_mss[2];
   m_plt_mss_4 = userconfig->Templates.Get().key_plt_mss[3];
   m_plt_mss_5 = userconfig->Templates.Get().key_plt_mss[4];
   m_plt_mss_6 = userconfig->Templates.Get().key_plt_mss[5];

   m_rbc_mss_1 = userconfig->Templates.Get().key_rbc_mss[0];
   m_rbc_mss_2 = userconfig->Templates.Get().key_rbc_mss[1];
   m_rbc_mss_3 = userconfig->Templates.Get().key_rbc_mss[2];

   if (m_hWnd){
	   UpdateData(0);
   }
   return true;
}

//
//-----------------------------------------------------------------------------
//                       File load / save functionality
//
void cfg_main_input::OnLoadFile() 
{
   CFileDialog dlg(TRUE,0,configpath,
                   OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR,
                   lpstrFilter);
   int update = dlg.DoModal();
   if (update == IDOK) 
   {
      configpath = dlg.GetPathName();
      const bool success = load();

      if (!success)
      {
         MessageBox("config.dat failed to read", "ERROR", MB_OK);
      }
   }
}


void cfg_main_input::OnSaveFile() 
{
   CFileDialog dlg(FALSE,0,configpath,
      OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
      lpstrFilter);
   int save = dlg.DoModal();
   if (save == IDOK) {
      configpath = dlg.GetPathName();

      CDatFileReader writer;
      writer.Initialize("config.dat", false);   
      bool successful_write = true;
      
      // Write out the Config CDS to file.
      successful_write &= userconfig->LangUnit.Write(writer);
      successful_write &= userconfig->Machine.Write(writer);
      successful_write &= userconfig->Procedure.Write(writer);
      successful_write &= userconfig->Predict.Write(writer);
      successful_write &= userconfig->Templates.Write(writer);
      successful_write &= userconfig->Definition.Write(writer);
      writer.Write(configpath);
		
      m_modified = 0;
   }
}

const int cfg_main_input::modified() const
{
   return m_modified;
}

//
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                    Message handling
//
//
//
//         The following code assumes that the combo box definitions
//         match the Trima GUI configurations in cfg_keys.h
//
#define setconfig(type,section,key,var) \
   { UpdateData(1); \
   type tmpconfig = userconfig->section.Get(); \
   tmpconfig.key = var; \
   userconfig->section.Set(tmpconfig); \
   m_modified = 1;}


void cfg_main_input::OnCloseupHeightUnits() 
{
   setconfig(LangUnitCfg,LangUnit,key_height,m_height_units);
}

void cfg_main_input::OnCloseupWeightUnits() 
{
   setconfig(LangUnitCfg,LangUnit,key_weight,m_weight_units);
}

void cfg_main_input::OnChangePlasmaAmapMin() 
{
   setconfig(PredictionConfig,Predict,key_pls_amap_min,m_plasma_amap_min);  
}
void cfg_main_input::OnChangePlasmaAmapMax() 
{
   setconfig(PredictionConfig,Predict,key_pls_amap_max,m_plasma_amap_max);  
}


void cfg_main_input::OnKillfocusMaxProcedureTime() 
{
   setconfig(ProcedureConfig,Procedure,key_proc_time,m_max_procedure_time);
}

void cfg_main_input::OnKillfocusMinimumPostcount() 
{
   setconfig(ProcedureConfig,Procedure,key_post_plat,m_minimum_postcount);
}

void cfg_main_input::OnKillfocusMinimumPostcrit() 
{
   setconfig(ProcedureConfig,Procedure,key_post_crit,m_minimum_postcrit);
}

void cfg_main_input::OnKillfocusRemoveWeight() 
{
   setconfig(ProcedureConfig,Procedure,key_weight_1_setting,m_remove_weight);
   setconfig(ProcedureConfig,Procedure,key_weight_2_setting,m_remove_weight);
   setconfig(ProcedureConfig,Procedure,key_weight_3_setting,m_remove_weight);
}

void cfg_main_input::OnCloseupInfusionRate() 
{
   setconfig(ProcedureConfig,Procedure,key_ac_rate,m_infusion_rate+1);
}

void cfg_main_input::OnKillfocusYsf() 
{
   setconfig(PredictionConfig,Predict,key_ysf,m_ysf);
}

void cfg_main_input::OnKillfocusPlt1Concentration() 
{
   UpdateData(1);
   m_plt1_volume = m_plt1_yield*100000.0f / m_plt1_concentration;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[0],m_plt1_volume);
}

void cfg_main_input::OnKillfocusPlt1Volume() 
{
   UpdateData(1);
   m_plt1_concentration = m_plt1_yield*100000.0f / m_plt1_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[0],m_plt1_volume);
}

void cfg_main_input::OnKillfocusPlt1Yield() 
{
   UpdateData(1);
   m_plt1_concentration = m_plt1_yield*100000.0f / m_plt1_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_yield[0],m_plt1_yield);
}

void cfg_main_input::OnKillfocusPlt2Concentration() 
{
   UpdateData(1);
   m_plt2_volume = m_plt2_yield*100000.0f / m_plt2_concentration;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[1],m_plt2_volume);
}

void cfg_main_input::OnKillfocusPlt2Volume() 
{
   UpdateData(1);
   m_plt2_concentration = m_plt2_yield*100000.0f / m_plt2_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[1],m_plt2_volume);
}

void cfg_main_input::OnKillfocusPlt2Yield() 
{
   UpdateData(1);
   m_plt2_concentration = m_plt2_yield*100000.0f / m_plt2_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_yield[1],m_plt2_yield);
}


void cfg_main_input::OnKillfocusPlt3Concentration() 
{
   UpdateData(1);
   m_plt3_volume = m_plt3_yield*100000.0f / m_plt3_concentration;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[2],m_plt3_volume);
}

void cfg_main_input::OnKillfocusPlt3Volume() 
{
   UpdateData(1);
   m_plt3_concentration = m_plt3_yield*100000.0f / m_plt3_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[2],m_plt3_volume);
}

void cfg_main_input::OnKillfocusPlt3Yield() 
{
   UpdateData(1);
   m_plt3_concentration = m_plt3_yield*100000.0f / m_plt3_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_yield[2],m_plt3_yield);
}


void cfg_main_input::OnKillfocusPlt4Concentration() 
{
   UpdateData(1);
   m_plt4_volume = m_plt4_yield*100000.0f / m_plt4_concentration;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[3],m_plt4_volume);
}

void cfg_main_input::OnKillfocusPlt4Volume() 
{
   UpdateData(1);
   m_plt4_concentration = m_plt4_yield*100000.0f / m_plt4_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[3],m_plt4_volume);
}

void cfg_main_input::OnKillfocusPlt4Yield() 
{
   UpdateData(1);
   m_plt4_concentration = m_plt4_yield*100000.0f / m_plt4_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_yield[3],m_plt4_yield);
}


void cfg_main_input::OnKillfocusPlt5Concentration() 
{
   UpdateData(1);
   m_plt5_volume = m_plt5_yield*100000.0f / m_plt5_concentration;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[4],m_plt5_volume);
}

void cfg_main_input::OnKillfocusPlt5Volume() 
{
   UpdateData(1);
   m_plt5_concentration = m_plt5_yield*100000.0f / m_plt5_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[4],m_plt5_volume);
}

void cfg_main_input::OnKillfocusPlt5Yield() 
{
   UpdateData(1);
   m_plt5_concentration = m_plt5_yield*100000.0f / m_plt5_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_yield[4],m_plt5_yield);
}


void cfg_main_input::OnKillfocusPlt6Concentration() 
{
   UpdateData(1);
   m_plt6_volume = m_plt6_yield*100000.0f / m_plt6_concentration;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[5],m_plt6_volume);
}

void cfg_main_input::OnKillfocusPlt6Volume() 
{
   UpdateData(1);
   m_plt6_concentration = m_plt6_yield*100000.0f / m_plt6_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_volume[5],m_plt6_volume);
}

void cfg_main_input::OnKillfocusPlt6Yield() 
{
   UpdateData(1);
   m_plt6_concentration = m_plt6_yield*100000.0f / m_plt6_volume;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_plt_yield[5],m_plt6_yield);
}




//
//
//   The following methods are invoked when a product changes.  This is the
//   compromise between 1 function saving all product values and 75 functions
//   each saving 1 value.  15 functions saving 5 values apiece seemed somewhat
//   optimal in terms of maintenance.
//
void cfg_main_input::OnUpdateProduct1() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[0].key_platelet   = m_product_platelet_1;
   defs[0].key_plasma     = m_product_plasma_1;
   defs[0].key_rbc        = m_product_rbc_1;
   defs[0].key_blood_type = m_product_blood_type_1;
   defs[0].key_time       = m_product_time_1;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
   
}

void cfg_main_input::OnUpdateProduct2() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[1].key_platelet   = m_product_platelet_2;
   defs[1].key_plasma     = m_product_plasma_2;
   defs[1].key_rbc        = m_product_rbc_2;
   defs[1].key_blood_type = m_product_blood_type_2;
   defs[1].key_time       = m_product_time_2;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct3() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[2].key_platelet   = m_product_platelet_3;
   defs[2].key_plasma     = m_product_plasma_3;
   defs[2].key_rbc        = m_product_rbc_3;
   defs[2].key_blood_type = m_product_blood_type_3;
   defs[2].key_time       = m_product_time_3;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct4() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[3].key_platelet   = m_product_platelet_4;
   defs[3].key_plasma     = m_product_plasma_4;
   defs[3].key_rbc        = m_product_rbc_4;
   defs[3].key_blood_type = m_product_blood_type_4;
   defs[3].key_time       = m_product_time_4;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct5() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[4].key_platelet   = m_product_platelet_5;
   defs[4].key_plasma     = m_product_plasma_5;
   defs[4].key_rbc        = m_product_rbc_5;
   defs[4].key_blood_type = m_product_blood_type_5;
   defs[4].key_time       = m_product_time_5;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct6() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[5].key_platelet   = m_product_platelet_6;
   defs[5].key_plasma     = m_product_plasma_6;
   defs[5].key_rbc        = m_product_rbc_6;
   defs[5].key_blood_type = m_product_blood_type_6;
   defs[5].key_time       = m_product_time_6;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct7() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[6].key_platelet   = m_product_platelet_7;
   defs[6].key_plasma     = m_product_plasma_7;
   defs[6].key_rbc        = m_product_rbc_7;
   defs[6].key_blood_type = m_product_blood_type_7;
   defs[6].key_time       = m_product_time_7;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct8() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[7].key_platelet   = m_product_platelet_8;
   defs[7].key_plasma     = m_product_plasma_8;
   defs[7].key_rbc        = m_product_rbc_8;
   defs[7].key_blood_type = m_product_blood_type_8;
   defs[7].key_time       = m_product_time_8;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct9() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[8].key_platelet   = m_product_platelet_9;
   defs[8].key_plasma     = m_product_plasma_9;
   defs[8].key_rbc        = m_product_rbc_9;
   defs[8].key_blood_type = m_product_blood_type_9;
   defs[8].key_time       = m_product_time_9;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct10() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[9].key_platelet   = m_product_platelet_10;
   defs[9].key_plasma     = m_product_plasma_10;
   defs[9].key_rbc        = m_product_rbc_10;
   defs[9].key_blood_type = m_product_blood_type_10;
   defs[9].key_time       = m_product_time_10;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct11() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[10].key_platelet   = m_product_platelet_11;
   defs[10].key_plasma     = m_product_plasma_11;
   defs[10].key_rbc        = m_product_rbc_11;
   defs[10].key_blood_type = m_product_blood_type_11;
   defs[10].key_time       = m_product_time_11;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct12() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[11].key_platelet   = m_product_platelet_12;
   defs[11].key_plasma     = m_product_plasma_12;
   defs[11].key_rbc        = m_product_rbc_12;
   defs[11].key_blood_type = m_product_blood_type_12;
   defs[11].key_time       = m_product_time_12;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct13() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[12].key_platelet   = m_product_platelet_13;
   defs[12].key_plasma     = m_product_plasma_13;
   defs[12].key_rbc        = m_product_rbc_13;
   defs[12].key_blood_type = m_product_blood_type_13;
   defs[12].key_time       = m_product_time_13;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct14() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[13].key_platelet   = m_product_platelet_14;
   defs[13].key_plasma     = m_product_plasma_14;
   defs[13].key_rbc        = m_product_rbc_14;
   defs[13].key_blood_type = m_product_blood_type_14;
   defs[13].key_time       = m_product_time_14;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}

void cfg_main_input::OnUpdateProduct15() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[14].key_platelet   = m_product_platelet_15;
   defs[14].key_plasma     = m_product_plasma_15;
   defs[14].key_rbc        = m_product_rbc_15;
   defs[14].key_blood_type = m_product_blood_type_15;
   defs[14].key_time       = m_product_time_15;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
}


void cfg_main_input::OnUpdateProduct16() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[15].key_platelet   = m_product_platelet_16;
   defs[15].key_plasma     = m_product_plasma_16;
   defs[15].key_rbc        = m_product_rbc_16;
   defs[15].key_blood_type = m_product_blood_type_16;
   defs[15].key_time       = m_product_time_16;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
	
}



void cfg_main_input::OnUpdateProduct17() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[16].key_platelet   = m_product_platelet_17;
   defs[16].key_plasma     = m_product_plasma_17;
   defs[16].key_rbc        = m_product_rbc_17;
   defs[16].key_blood_type = m_product_blood_type_17;
   defs[16].key_time       = m_product_time_17;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
	
}

void cfg_main_input::OnUpdateProduct18() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[17].key_platelet   = m_product_platelet_18;
   defs[17].key_plasma     = m_product_plasma_18;
   defs[17].key_rbc        = m_product_rbc_18;
   defs[17].key_blood_type = m_product_blood_type_18;
   defs[17].key_time       = m_product_time_18;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
	
}

void cfg_main_input::OnUpdateProduct19() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[18].key_platelet   = m_product_platelet_19;
   defs[18].key_plasma     = m_product_plasma_19;
   defs[18].key_rbc        = m_product_rbc_19;
   defs[18].key_blood_type = m_product_blood_type_19;
   defs[18].key_time       = m_product_time_19;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
	
}
void cfg_main_input::OnUpdateProduct20() 
{
   UpdateData(1);
   ProductDefinitionArray defs = userconfig->Definition.Get();
   defs[19].key_platelet   = m_product_platelet_20;
   defs[19].key_plasma     = m_product_plasma_20;
   defs[19].key_rbc        = m_product_rbc_20;
   defs[19].key_blood_type = m_product_blood_type_20;
   defs[19].key_time       = m_product_time_20;
   userconfig->Definition.Set(defs);
   userconfig->CopyOfflineProcedureList(); // set predict equivalent
   m_modified = 1;
	
}
void cfg_main_input::OnKillfocusVolumeLessThan() 
{
   setconfig(ProcedureConfig,Procedure,key_weight_1_less_than_vol,m_remove_weight_less_than);
   setconfig(ProcedureConfig,Procedure,key_weight_2_less_than_vol,m_remove_weight_less_than);
   setconfig(ProcedureConfig,Procedure,key_weight_3_less_than_vol,m_remove_weight_less_than);
}

void cfg_main_input::OnKillfocusPercentTbv() 
{
   setconfig(ProcedureConfig,Procedure,key_tbv_percent,m_percent_tbv);
}

void cfg_main_input::OnKillfocusVolumeGreaterThan() 
{
   setconfig(ProcedureConfig,Procedure,key_weight_3_greater_than_vol,m_remove_weight_greater_than);
}


void cfg_main_input::OnCloseupReplacementFluid() 
{
   setconfig(PredictionConfig,Predict,key_rbc_replace_fluid,m_replacement_fluid_index);
}

void cfg_main_input::OnKillfocusReplacementFluidPercent() 
{
   setconfig(PredictionConfig,Predict,key_rbc_fluid_percent,m_replacement_fluid_percent);
}

void cfg_main_input::OnCloseupRbcRatio() 
{
   setconfig(PredictionConfig,Predict,key_rbc_ratio,m_rbc_ratio_index);
}


void cfg_main_input::OnKillfocusRbcCollectVolume1() 
{
   UpdateData(1);
   m_rbc_crit_1 = 100.0f * m_rbc_dose_1 / m_rbc_volume_1;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_rbc_crit[0],m_rbc_crit_1);
}

void cfg_main_input::OnKillfocusRbcCollectVolume2() 
{
   UpdateData(1);
   m_rbc_crit_2 = 100.0f * m_rbc_dose_2 / m_rbc_volume_2;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_rbc_crit[1],m_rbc_crit_2);
}

void cfg_main_input::OnKillfocusRbcCollectVolume3() 
{
   UpdateData(1);
   m_rbc_crit_3 = 100.0f * m_rbc_dose_3 / m_rbc_volume_3;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_rbc_crit[2],m_rbc_crit_3);
}

void cfg_main_input::OnKillfocusRbcCrit1() 
{
   UpdateData(1);
   m_rbc_volume_1 = 100.0f * m_rbc_dose_1 / m_rbc_crit_1 / m_rbc_ysf;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_rbc_crit[0],m_rbc_crit_1);
}

void cfg_main_input::OnKillfocusRbcCrit2() 
{
   UpdateData(1);
   m_rbc_volume_2 = 100.0f * m_rbc_dose_2 / m_rbc_crit_2 / m_rbc_ysf;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_rbc_crit[1],m_rbc_crit_2);
}

void cfg_main_input::OnKillfocusRbcCrit3() 
{
   UpdateData(1);
   m_rbc_volume_3 = 100.0f * m_rbc_dose_3 / m_rbc_crit_3 / m_rbc_ysf;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_rbc_crit[2],m_rbc_crit_3);
}

void cfg_main_input::OnKillfocusRbcDose1() 
{
   UpdateData(1);
   m_rbc_volume_1 = 100.0f * m_rbc_dose_1 / m_rbc_crit_1 / m_rbc_ysf;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_rbc_dose[0],m_rbc_dose_1);
}

void cfg_main_input::OnKillfocusRbcDose2() 
{
   UpdateData(1);
   m_rbc_volume_2 = 100.0f * m_rbc_dose_2 / m_rbc_crit_2 / m_rbc_ysf;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_rbc_dose[1],m_rbc_dose_2);
}

void cfg_main_input::OnKillfocusRbcDose3() 
{
   UpdateData(1);
   m_rbc_volume_3 = 100.0f * m_rbc_dose_3 / m_rbc_crit_3 / m_rbc_ysf;
   UpdateData(0);
   setconfig(ProductTemplates,Templates,key_rbc_dose[2],m_rbc_dose_3);
}


void cfg_main_input::OnCloseupMaxDrawFlow() 
{
   setconfig(ProcedureConfig,Procedure,key_max_draw_flow,m_max_draw_flow);
}

void cfg_main_input::OnKillfocusReplacementVolumeTrigger() 
{
   setconfig(ProcedureConfig,Procedure,key_min_replacement_volume,m_replace_volume_trigger);
}

void cfg_main_input::OnClose() 
{
   UpdateData(1);
//   load();	
	CDialog::OnClose();
}

void cfg_main_input::OnUpdateRemovalSetting() 
{
   UpdateData(1);
   int setting = 0;
   if (m_tbv_limit_active)     setting |= (int)VOLUME_REMOVAL_TBV;
   if (m_volume_limit_active)  setting |= (int)VOLUME_REMOVAL_WEIGHT;
   if (m_ml_per_kg_active)     setting |= (int)VOLUME_REMOVAL_ML_PER_KG;
   setconfig(ProcedureConfig,Procedure,key_tbv_vol_setting,setting);
}


void cfg_main_input::OnCloseupReturnManagement() 
{
   setconfig(PredictionConfig,Predict,key_return_management,m_return_management+1);
}

void cfg_main_input::OnKillfocusRbcYsf() 
{
   setconfig(PredictionConfig,Predict,key_rsf,m_rbc_ysf);
   if (m_rbc_ysf > 0.0f) {
      OnKillfocusRbcCrit1();
      OnKillfocusRbcCrit2();
      OnKillfocusRbcCrit3();
   }
}


void cfg_main_input::OnKillfocusMlPerKg() 
{
   setconfig(ProcedureConfig,Procedure,key_ml_per_kg,m_ml_per_kg);
}

void cfg_main_input::OnCloseupInletRamp() 
{
   setconfig(PredictionConfig,Predict,key_inlet_flow_ramp,m_inlet_ramp);
}

void cfg_main_input::OnCloseupDrawManagement() 
{
   setconfig(PredictionConfig,Predict,key_inlet_management,m_draw_management+1);
}

void cfg_main_input::OnKillfocusCustomRatio() 
{
   setconfig(ProcedureConfig,Procedure,key_custom_ratio,m_custom_ratio);
}

void cfg_main_input::OnChangePlasma1() 
{
   setconfig(ProductTemplates,Templates,key_pls_volume[0],m_plasma1);
}

void cfg_main_input::OnChangePlasma2() 
{
   setconfig(ProductTemplates,Templates,key_pls_volume[1],m_plasma2);
}

void cfg_main_input::OnChangePlasma3() 
{
   setconfig(ProductTemplates,Templates,key_pls_volume[2],m_plasma3);
}

void cfg_main_input::OnChangePlasma4() 
{
   setconfig(ProductTemplates,Templates,key_pls_volume[3],m_plasma4);
}

void cfg_main_input::OnChangePlasma5() 
{
   setconfig(ProductTemplates,Templates,key_pls_volume[4],m_plasma5);
}

void cfg_main_input::OnChangePlasma6() 
{
   setconfig(ProductTemplates,Templates,key_pls_volume[5],m_plasma6);
}

void cfg_main_input::OnCloseupPlasmaRinseback() 
{
   setconfig(ProcedureConfig,Procedure,key_plasma_rinseback,m_plasma_rinseback);
}



void cfg_main_input::OnPltMssCheck1Click() 
{
	setconfig(ProductTemplates, Templates, key_plt_mss[0], m_plt_mss_1);
}


void cfg_main_input::OnPltMssCheck2Click() 
{
	setconfig(ProductTemplates, Templates, key_plt_mss[1], m_plt_mss_2);
}

void cfg_main_input::OnPltMssCheck3Click() 
{
	setconfig(ProductTemplates, Templates, key_plt_mss[2], m_plt_mss_3);	
}

void cfg_main_input::OnPltMssCheck4Click() 
{
	setconfig(ProductTemplates, Templates, key_plt_mss[3], m_plt_mss_4);	
}

void cfg_main_input::OnPltMssCheck5Click() 
{
	setconfig(ProductTemplates, Templates, key_plt_mss[4], m_plt_mss_5);
}

void cfg_main_input::OnPltMssCheck6Click() 
{
	setconfig(ProductTemplates, Templates, key_plt_mss[5], m_plt_mss_6);
}


void cfg_main_input::OnRbcMssCheck1Click() 
{
	setconfig(ProductTemplates, Templates, key_rbc_mss[0], m_rbc_mss_1);
}

void cfg_main_input::OnRbcMssCheck2Click() 
{
	setconfig(ProductTemplates, Templates, key_rbc_mss[1], m_rbc_mss_2);
}

void cfg_main_input::OnRbcMssCheck3Click() 
{
	setconfig(ProductTemplates, Templates, key_rbc_mss[2], m_rbc_mss_3);
}
