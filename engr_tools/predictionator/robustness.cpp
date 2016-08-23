// robustness.cpp : implementation file
//

#include "stdafx.h"

#include <math.h>
#include <fstream.h>

#include "ap2 predictionater.h"
#include "robustness.h"
#include "extendedresults.h"
#include "substatemap.h"
extern CAP2predictionaterApp theApp;

#include "predict.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const float __donorheight = 60.0f; // use these for a static BMI calculation
static const float __donorweight = 100.0f;
/////////////////////////////////////////////////////////////////////////////
// robustness

IMPLEMENT_DYNCREATE(robustness, CFormView)

robustness::robustness() :
	optimizer_form(robustness::IDD),
   m_popup(0),
   m_popup_repredict(0),
   m_last_number_updated(0)
{
	EnableAutomation();
	//{{AFX_DATA_INIT(robustness)
	m_hct_delta = 0.0f;
	m_hct_start = 0.0f;
	m_hct_stop = 0.0f;
	m_hct_tests = 0;
	m_plasma_delta = 0.0f;
	m_plasma_start = 0.0f;
	m_plasma_stop = 0.0f;
	m_plasma_tests = 0;
	m_precount_delta = 0.0f;
	m_precount_start = 0.0f;
	m_precount_stop = 0.0f;
	m_precount_tests = 0;
	m_rbccrit_delta = 0.0f;
	m_rbccrit_start = 0.0f;
	m_rbccrit_stop = 0.0f;
	m_rbccrit_tests = 0;
	m_rbcdose_delta = 0.0f;
	m_rbcdose_start = 0.0f;
	m_rbcdose_stop = 0.0f;
	m_rbcdose_tests = 0;
	m_tbv_delta = 0.0f;
	m_tbv_start = 0.0f;
	m_tbv_stop = 0.0f;
	m_tbv_tests = 0;
	m_yield_delta = 0.0f;
	m_yield_start = 0.0f;
	m_yield_stop = 0.0f;
	m_yield_tests = 0;
	m_percent_complete = 0;
	m_elapsed_time = _T("");
	m_remaining_time = _T("");
	m_total_tried = 0;
	m_total_products = 0;
	m_conc_delta = 0.0f;
	m_conc_start = 0.0f;
	m_conc_stop = 0.0f;
	m_conc_tests = 0;
	m_amap = FALSE;
	m_max_concurrent = FALSE;
	m_draw_flow_delta = 0;
	m_draw_flow_start = 0;
	m_draw_flow_stop = 0;
	m_draw_management_delta = 0;
	m_draw_management_start = 0;
	m_infusion_delta = 0;
	m_infusion_start = 0;
	m_infusion_stop = 0;
	m_inlet_ramp_delta = 0;
	m_inlet_ramp_start = 0;
	m_inlet_ramp_stop = 0;
	m_ratio_delta = 0.0f;
	m_ratio_start = 0.0f;
	m_ratio_stop = 0.0f;
	m_return_management_delta = 0;
	m_return_management_start = 0;
	m_return_management_stop = 0;
	m_draw_management_stop = 0;
	m_good = 0;
	m_good_lt_2000 = 0;
	m_good_lt_4000 = 0;
	m_percent_lt_2000 = 0.0f;
	m_percent_lt_4000 = 0.0f;
	m_percent_good = 0.0f;
	m_unconverged = 0;
	m_unconverged_lt_2000 = 0;
	m_unconverged_lt_4000 = 0;
	m_good_gt_4000 = 0;
	m_percent_gt_4000 = 0.0f;
	m_unconverged_gt_4000 = 0;
	m_inconc = 0;
	m_inconc_gt_4000 = 0;
	m_inconc_lt_2000 = 0;
	m_inconc_lt_4000 = 0;
	m_inconc_percent_good = 0.0f;
	m_inconc_percent_gt_4000 = 0.0f;
	m_inconc_percent_lt_4000 = 0.0f;
	m_inconc_percent_lt_2000 = 0.0f;
	m_display_concentration = TRUE;
	m_display_unconverged = TRUE;
	m_display_vc_errors = TRUE;
	m_display_vp_errors = TRUE;
	m_display_chamber_flow = TRUE;
	m_lockin_errors = 0;
	m_display_lockin = FALSE;
	m_debug = 0.0f;
	m_repredict = FALSE;
	m_listbox_save_path = _T("c:\\temp\\robustnessstrings");
	m_sequential = FALSE;
	m_use_normal = FALSE;
	//}}AFX_DATA_INIT
}

robustness::~robustness()
{
}

void robustness::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

   m_popup->DestroyWindow();
   delete m_popup;
   m_popup = 0;
   m_popup_repredict->DestroyWindow();
   delete m_popup_repredict;
   m_popup_repredict = 0;
	optimizer_form::OnFinalRelease();
}

void robustness::DoDataExchange(CDataExchange* pDX)
{
	optimizer_form::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(robustness)
	DDX_Control(pDX, IDC_TBV_STOP, m_tbv_stop_ctl);
	DDX_Control(pDX, IDC_TBV_START, m_tbv_start_ctl);
	DDX_Control(pDX, IDC_TBV_DELTA, m_tbv_delta_ctl);
	DDX_Control(pDX, IDC_PRECOUNT_STOP, m_precount_stop_ctl);
	DDX_Control(pDX, IDC_PRECOUNT_START, m_precount_start_ctl);
	DDX_Control(pDX, IDC_PRECOUNT_DELTA, m_precount_delta_ctl);
	DDX_Control(pDX, IDC_HCT_STOP, m_hct_stop_ctl);
	DDX_Control(pDX, IDC_HCT_START, m_hct_start_ctl);
	DDX_Control(pDX, IDC_HCT_DELTA, m_hct_delta_ctl);
	DDX_Control(pDX, IDC_REPREDICT, m_repredict_control);
	DDX_Control(pDX, IDC_TOTAL_PRODUCTS, m_total_products_control);
	DDX_Control(pDX, IDC_AMAP, m_amap_control);
	DDX_Control(pDX, IDC_MAX_CONCURRENT, m_max_concurrent_control);
	DDX_Control(pDX, IDC_RESULTS, m_results_control);
	DDX_Control(pDX, IDC_STOP_SIMULATION, m_stop_control);
	DDX_Control(pDX, IDC_START_SIMULATION, m_start_control);
	DDX_Control(pDX, IDC_INITIAL_CONDITIONS, m_initial_conditions_control);
	DDX_Control(pDX, IDC_EDIT_CONFIG, m_edit_config_control);
	DDX_Control(pDX, IDC_TEST_SUITE_PROGRESS, m_test_suite_progress);
	DDX_Text(pDX, IDC_HCT_DELTA, m_hct_delta);
	DDX_Text(pDX, IDC_HCT_START, m_hct_start);
	DDX_Text(pDX, IDC_HCT_STOP, m_hct_stop);
	DDX_Text(pDX, IDC_HCT_TESTS, m_hct_tests);
	DDX_Text(pDX, IDC_PLASMA_DELTA, m_plasma_delta);
	DDX_Text(pDX, IDC_PLASMA_START, m_plasma_start);
	DDX_Text(pDX, IDC_PLASMA_STOP, m_plasma_stop);
	DDX_Text(pDX, IDC_PLASMA_TESTS, m_plasma_tests);
	DDX_Text(pDX, IDC_PRECOUNT_DELTA, m_precount_delta);
	DDX_Text(pDX, IDC_PRECOUNT_START, m_precount_start);
	DDX_Text(pDX, IDC_PRECOUNT_STOP, m_precount_stop);
	DDX_Text(pDX, IDC_PRECOUNT_TESTS, m_precount_tests);
	DDX_Text(pDX, IDC_RBCCRIT_DELTA, m_rbccrit_delta);
	DDX_Text(pDX, IDC_RBCCRIT_START, m_rbccrit_start);
	DDX_Text(pDX, IDC_RBCCRIT_STOP, m_rbccrit_stop);
	DDX_Text(pDX, IDC_RBCCRIT_TESTS, m_rbccrit_tests);
	DDX_Text(pDX, IDC_RBCDOSE_DELTA, m_rbcdose_delta);
	DDX_Text(pDX, IDC_RBCDOSE_START, m_rbcdose_start);
	DDX_Text(pDX, IDC_RBCDOSE_STOP, m_rbcdose_stop);
	DDX_Text(pDX, IDC_RBCDOSE_TESTS, m_rbcdose_tests);
	DDX_Text(pDX, IDC_TBV_DELTA, m_tbv_delta);
	DDX_Text(pDX, IDC_TBV_START, m_tbv_start);
	DDX_Text(pDX, IDC_TBV_STOP, m_tbv_stop);
	DDX_Text(pDX, IDC_TBV_TESTS, m_tbv_tests);
	DDX_Text(pDX, IDC_YIELD_DELTA, m_yield_delta);
	DDX_Text(pDX, IDC_YIELD_START, m_yield_start);
	DDX_Text(pDX, IDC_YIELD_STOP, m_yield_stop);
	DDX_Text(pDX, IDC_YIELD_TESTS, m_yield_tests);
	DDX_Text(pDX, IDC_PERCENT_COMPLETE, m_percent_complete);
	DDX_Text(pDX, IDC_ELAPSED_TIME, m_elapsed_time);
	DDX_Text(pDX, IDC_REMAINING_TIME, m_remaining_time);
	DDX_Text(pDX, IDC_TOTAL_TRIED, m_total_tried);
	DDX_Text(pDX, IDC_TOTAL_PRODUCTS, m_total_products);
	DDX_Text(pDX, IDC_CONC_DELTA, m_conc_delta);
	DDX_Text(pDX, IDC_CONC_START, m_conc_start);
	DDX_Text(pDX, IDC_CONC_STOP, m_conc_stop);
	DDX_Text(pDX, IDC_CONC_TESTS, m_conc_tests);
	DDX_Check(pDX, IDC_AMAP, m_amap);
	DDX_Check(pDX, IDC_MAX_CONCURRENT, m_max_concurrent);
	DDX_Text(pDX, IDC_DRAW_FLOW_DELTA, m_draw_flow_delta);
	DDX_Text(pDX, IDC_DRAW_FLOW_START, m_draw_flow_start);
	DDX_Text(pDX, IDC_DRAW_FLOW_STOP, m_draw_flow_stop);
	DDX_Text(pDX, IDC_DRAW_MANAGEMENT_DELTA, m_draw_management_delta);
	DDX_Text(pDX, IDC_DRAW_MANAGEMENT_START, m_draw_management_start);
	DDX_Text(pDX, IDC_INFUSION_DELTA, m_infusion_delta);
	DDX_Text(pDX, IDC_INFUSION_START, m_infusion_start);
	DDX_Text(pDX, IDC_INFUSION_STOP, m_infusion_stop);
	DDX_Text(pDX, IDC_INLET_RAMP_DELTA, m_inlet_ramp_delta);
	DDX_Text(pDX, IDC_INLET_RAMP_START, m_inlet_ramp_start);
	DDX_Text(pDX, IDC_INLET_RAMP_STOP, m_inlet_ramp_stop);
	DDX_Text(pDX, IDC_RATIO_DELTA, m_ratio_delta);
	DDX_Text(pDX, IDC_RATIO_START, m_ratio_start);
	DDX_Text(pDX, IDC_RATIO_STOP, m_ratio_stop);
	DDX_Text(pDX, IDC_RETURN_MANAGEMENT_DELTA, m_return_management_delta);
	DDX_Text(pDX, IDC_RETURN_MANAGEMENT_START, m_return_management_start);
	DDX_Text(pDX, IDC_RETURN_MANAGEMENT_STOP, m_return_management_stop);
	DDX_Text(pDX, IDC_DRAW_MANAGEMENT_STOP, m_draw_management_stop);
	DDX_Text(pDX, IDC_GOOD, m_good);
	DDX_Text(pDX, IDC_GOOD_LT_2000, m_good_lt_2000);
	DDX_Text(pDX, IDC_GOOD_LT_4000, m_good_lt_4000);
	DDX_Text(pDX, IDC_PERCENT_LT_2000, m_percent_lt_2000);
	DDX_Text(pDX, IDC_PERCENT_LT_4000, m_percent_lt_4000);
	DDX_Text(pDX, IDC_PERCENT_GOOD, m_percent_good);
	DDX_Text(pDX, IDC_UNCONVERGED, m_unconverged);
	DDX_Text(pDX, IDC_UNCONVERGED_LT_2000, m_unconverged_lt_2000);
	DDX_Text(pDX, IDC_UNCONVERGED_LT_4000, m_unconverged_lt_4000);
	DDX_Text(pDX, IDC_GOOD_GT_4000, m_good_gt_4000);
	DDX_Text(pDX, IDC_PERCENT_GT_4000, m_percent_gt_4000);
	DDX_Text(pDX, IDC_UNCONVERGED_GT_4000, m_unconverged_gt_4000);
	DDX_Text(pDX, IDC_INCONC, m_inconc);
	DDX_Text(pDX, IDC_INCONC_GT_4000, m_inconc_gt_4000);
	DDX_Text(pDX, IDC_INCONC_LT_2000, m_inconc_lt_2000);
	DDX_Text(pDX, IDC_INCONC_LT_4000, m_inconc_lt_4000);
	DDX_Text(pDX, IDC_INCONC_PERCENT_GOOD, m_inconc_percent_good);
	DDX_Text(pDX, IDC_INCONC_PERCENT_GT_4000, m_inconc_percent_gt_4000);
	DDX_Text(pDX, IDC_INCONC_PERCENT_LT_4000, m_inconc_percent_lt_4000);
	DDX_Text(pDX, IDC_INCONC_PERCENT_LT_2000, m_inconc_percent_lt_2000);
	DDX_Check(pDX, IDC_DISPLAY_CONCENTRATION, m_display_concentration);
	DDX_Check(pDX, IDC_DISPLAY_UNCONVERGED, m_display_unconverged);
	DDX_Check(pDX, IDC_DISPLAY_VC_ERRS, m_display_vc_errors);
	DDX_Check(pDX, IDC_DISPLAY_VP_ERRS, m_display_vp_errors);
	DDX_Check(pDX, IDC_DISPLAY_CHAMBER_FLOW, m_display_chamber_flow);
	DDX_Text(pDX, IDC_LOCKIN_ERRORS, m_lockin_errors);
	DDX_Check(pDX, IDC_DISPLAY_LOCKIN, m_display_lockin);
	DDX_Text(pDX, IDC_DEBUG, m_debug);
	DDX_Check(pDX, IDC_REPREDICT, m_repredict);
	DDX_Text(pDX, IDC_LISTBOX_SAVE_PATH, m_listbox_save_path);
	DDX_Check(pDX, IDC_SEQUENTIAL, m_sequential);
	DDX_Check(pDX, IDC_USE_NORMAL, m_use_normal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(robustness, CFormView)
	//{{AFX_MSG_MAP(robustness)
	ON_BN_CLICKED(IDC_START_SIMULATION, OnStart)
	ON_BN_CLICKED(IDC_STOP_SIMULATION, OnStop)
	ON_EN_CHANGE(IDC_HCT_START, OnHct)
	ON_EN_CHANGE(IDC_TBV_START, OnTBV)
	ON_EN_CHANGE(IDC_PRECOUNT_DELTA, OnPrecount)
	ON_EN_CHANGE(IDC_CONC_DELTA, OnConc)
	ON_EN_CHANGE(IDC_PLASMA_DELTA, OnPlasma)
	ON_EN_CHANGE(IDC_RBCCRIT_DELTA, OnRBCCrit)
	ON_EN_CHANGE(IDC_RBCDOSE_DELTA, OnRBCDose)
	ON_EN_CHANGE(IDC_YIELD_DELTA, OnYield)
	ON_BN_CLICKED(IDC_EDIT_CONFIG, OnEditConfig)
	ON_BN_CLICKED(IDC_INITIAL_CONDITIONS, OnInitialConditions)
	ON_LBN_DBLCLK(IDC_RESULTS, OnDblclkResults)
	ON_EN_CHANGE(IDC_DRAW_FLOW_DELTA, OnDrawFlow)
	ON_EN_CHANGE(IDC_DRAW_MANAGEMENT_DELTA, OnDrawManagement)
	ON_EN_CHANGE(IDC_INFUSION_DELTA, OnInfusion)
	ON_EN_CHANGE(IDC_INLET_RAMP_DELTA, OnInletRamp)
	ON_EN_CHANGE(IDC_RATIO_DELTA, OnRatio)
	ON_EN_CHANGE(IDC_RETURN_MANAGEMENT_DELTA, OnReturnManagement)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_SAVE_OUTPUT, OnSaveOutput)
	ON_BN_CLICKED(IDC_RESTORE_OUTPUT, OnRestoreOutput)
	ON_BN_CLICKED(IDC_USE_NORMAL, OnUseNormal)
	ON_EN_KILLFOCUS(IDC_HCT_START, OnHct)
	ON_EN_CHANGE(IDC_HCT_STOP, OnHct)
	ON_EN_KILLFOCUS(IDC_HCT_STOP, OnHct)
	ON_EN_KILLFOCUS(IDC_TBV_START, OnTBV)
	ON_EN_CHANGE(IDC_TBV_STOP, OnTBV)
	ON_EN_KILLFOCUS(IDC_TBV_STOP, OnTBV)
	ON_EN_KILLFOCUS(IDC_PRECOUNT_DELTA, OnPrecount)
	ON_EN_CHANGE(IDC_PRECOUNT_START, OnPrecount)
	ON_EN_KILLFOCUS(IDC_PRECOUNT_START, OnPrecount)
	ON_EN_CHANGE(IDC_PRECOUNT_STOP, OnPrecount)
	ON_EN_KILLFOCUS(IDC_PRECOUNT_STOP, OnPrecount)
	ON_EN_CHANGE(IDC_HCT_DELTA, OnHct)
	ON_EN_KILLFOCUS(IDC_HCT_DELTA, OnHct)
	ON_EN_CHANGE(IDC_TBV_DELTA, OnTBV)
	ON_EN_KILLFOCUS(IDC_TBV_DELTA, OnTBV)
	ON_EN_KILLFOCUS(IDC_CONC_DELTA, OnConc)
	ON_EN_CHANGE(IDC_CONC_START, OnConc)
	ON_EN_KILLFOCUS(IDC_CONC_START, OnConc)
	ON_EN_CHANGE(IDC_CONC_STOP, OnConc)
	ON_EN_KILLFOCUS(IDC_CONC_STOP, OnConc)
	ON_EN_KILLFOCUS(IDC_PLASMA_DELTA, OnPlasma)
	ON_EN_CHANGE(IDC_PLASMA_START, OnPlasma)
	ON_EN_KILLFOCUS(IDC_PLASMA_START, OnPlasma)
	ON_EN_CHANGE(IDC_PLASMA_STOP, OnPlasma)
	ON_EN_KILLFOCUS(IDC_PLASMA_STOP, OnPlasma)
	ON_EN_KILLFOCUS(IDC_RBCCRIT_DELTA, OnRBCCrit)
	ON_EN_CHANGE(IDC_RBCCRIT_START, OnRBCCrit)
	ON_EN_KILLFOCUS(IDC_RBCCRIT_START, OnRBCCrit)
	ON_EN_CHANGE(IDC_RBCCRIT_STOP, OnRBCCrit)
	ON_EN_KILLFOCUS(IDC_RBCCRIT_STOP, OnRBCCrit)
	ON_EN_KILLFOCUS(IDC_RBCDOSE_DELTA, OnRBCDose)
	ON_EN_CHANGE(IDC_RBCDOSE_START, OnRBCDose)
	ON_EN_KILLFOCUS(IDC_RBCDOSE_START, OnRBCDose)
	ON_EN_CHANGE(IDC_RBCDOSE_STOP, OnRBCDose)
	ON_EN_KILLFOCUS(IDC_RBCDOSE_STOP, OnRBCDose)
	ON_EN_KILLFOCUS(IDC_YIELD_DELTA, OnYield)
	ON_EN_CHANGE(IDC_YIELD_START, OnYield)
	ON_EN_KILLFOCUS(IDC_YIELD_START, OnYield)
	ON_EN_CHANGE(IDC_YIELD_STOP, OnYield)
	ON_EN_KILLFOCUS(IDC_YIELD_STOP, OnYield)
	ON_EN_KILLFOCUS(IDC_DRAW_FLOW_DELTA, OnDrawFlow)
	ON_EN_CHANGE(IDC_DRAW_FLOW_START, OnDrawFlow)
	ON_EN_KILLFOCUS(IDC_DRAW_FLOW_START, OnDrawFlow)
	ON_EN_CHANGE(IDC_DRAW_FLOW_STOP, OnDrawFlow)
	ON_EN_KILLFOCUS(IDC_DRAW_FLOW_STOP, OnDrawFlow)
	ON_EN_KILLFOCUS(IDC_DRAW_MANAGEMENT_DELTA, OnDrawManagement)
	ON_EN_CHANGE(IDC_DRAW_MANAGEMENT_START, OnDrawManagement)
	ON_EN_KILLFOCUS(IDC_DRAW_MANAGEMENT_START, OnDrawManagement)
	ON_EN_CHANGE(IDC_DRAW_MANAGEMENT_STOP, OnDrawManagement)
	ON_EN_KILLFOCUS(IDC_DRAW_MANAGEMENT_STOP, OnDrawManagement)
	ON_EN_KILLFOCUS(IDC_INFUSION_DELTA, OnInfusion)
	ON_EN_CHANGE(IDC_INFUSION_START, OnInfusion)
	ON_EN_KILLFOCUS(IDC_INFUSION_START, OnInfusion)
	ON_EN_CHANGE(IDC_INFUSION_STOP, OnInfusion)
	ON_EN_KILLFOCUS(IDC_INFUSION_STOP, OnInfusion)
	ON_EN_KILLFOCUS(IDC_INLET_RAMP_DELTA, OnInletRamp)
	ON_EN_CHANGE(IDC_INLET_RAMP_START, OnInletRamp)
	ON_EN_KILLFOCUS(IDC_INLET_RAMP_START, OnInletRamp)
	ON_EN_CHANGE(IDC_INLET_RAMP_STOP, OnInletRamp)
	ON_EN_KILLFOCUS(IDC_INLET_RAMP_STOP, OnInletRamp)
	ON_EN_KILLFOCUS(IDC_RATIO_DELTA, OnRatio)
	ON_EN_CHANGE(IDC_RATIO_START, OnRatio)
	ON_EN_KILLFOCUS(IDC_RATIO_START, OnRatio)
	ON_EN_CHANGE(IDC_RATIO_STOP, OnRatio)
	ON_EN_KILLFOCUS(IDC_RATIO_STOP, OnRatio)
	ON_EN_KILLFOCUS(IDC_RETURN_MANAGEMENT_DELTA, OnReturnManagement)
	ON_EN_CHANGE(IDC_RETURN_MANAGEMENT_START, OnReturnManagement)
	ON_EN_KILLFOCUS(IDC_RETURN_MANAGEMENT_START, OnReturnManagement)
	ON_EN_CHANGE(IDC_RETURN_MANAGEMENT_STOP, OnReturnManagement)
	ON_EN_KILLFOCUS(IDC_RETURN_MANAGEMENT_STOP, OnReturnManagement)
	ON_BN_CLICKED(IDC_SEQUENTIAL, OnSequential)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(robustness, CFormView)
	//{{AFX_DISPATCH_MAP(robustness)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_Irobustness to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {44EE7C11-9EE9-47CC-9B68-38CB6D617B18}
static const IID IID_Irobustness =
{ 0x44ee7c11, 0x9ee9, 0x47cc, { 0x9b, 0x68, 0x38, 0xcb, 0x6d, 0x61, 0x7b, 0x18 } };

BEGIN_INTERFACE_MAP(robustness, CFormView)
	INTERFACE_PART(robustness, IID_Irobustness, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// robustness diagnostics

#ifdef _DEBUG
void robustness::AssertValid() const
{
	optimizer_form::AssertValid();
}

void robustness::Dump(CDumpContext& dc) const
{
	optimizer_form::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// robustness message handlers

void robustness::OnStart() 
{
   //
   //
   //   Seed the random number generator
   //
   srand((unsigned)time(NULL));
   UpdateData(1);

   if (m_sequential) {
	   m_total_products = m_hct_tests*m_precount_tests*m_tbv_tests;
	   m_total_products *= m_yield_tests*m_conc_tests*m_plasma_tests*m_rbccrit_tests*m_rbcdose_tests;
	   m_total_products *= m_ratio_tests*m_infusion_tests*m_draw_flow_tests;
	   m_total_products *= m_draw_management_tests*m_return_management_tests;
	   m_total_products *= m_inlet_ramp_tests;

	   m_current_hct = m_hct_start;
       m_current_precount = m_precount_start;
       m_current_tbv = m_tbv_start;
       m_current_yield = m_yield_start;
       m_current_conc = m_conc_start;
       m_current_plasma = m_plasma_start;
       m_current_rbccrit = m_rbccrit_start;
       m_current_rbcdose = m_rbcdose_start;
       m_current_ratio = m_ratio_start;
       m_current_infusion = m_infusion_start;
       m_current_draw_flow = m_draw_flow_start;
       m_current_draw_management = m_draw_management_start;
       m_current_return_management = m_return_management_start;
       m_current_inlet_ramp = m_inlet_ramp_start;
       m_current_replacement = 1;
       m_current_replacement_percent = 100;

	   UpdateData(0);
   }
   if (m_total_products <= 0) return;

   enable_controls(0);
   optimizer_form::OnStart();

   m_percent_complete = 0;
   m_total_tried = 0;
   m_last_number_updated = 0;
	m_test_suite_progress.SetPos( (int)0 );
   
   m_results_control.ResetContent();

   m_good = 0;
   m_unconverged = 0;
   m_percent_good = 0.0f;
   m_inconc = 0;
   m_inconc_percent_good = 0.0f;

   m_good_lt_2000 = 0;
   m_unconverged_lt_2000 = 0;
   m_percent_lt_2000 = 0.0f;
   m_inconc_lt_2000 = 0;
   m_inconc_percent_lt_2000 = 0.0f;

   m_good_lt_4000 = 0;
   m_unconverged_lt_4000 = 0;
   m_percent_lt_4000 = 0.0f;
   m_inconc_lt_4000 = 0;
   m_inconc_percent_lt_4000 = 0.0f;

   m_good_gt_4000 = 0;
   m_unconverged_gt_4000 = 0;
   m_percent_gt_4000 = 0.0f;
   m_inconc_gt_4000 = 0;
   m_inconc_percent_gt_4000 = 0.0f;

   m_lockin_errors = 0;
   m_debug = 0.0f;

   UpdateData(0);

   // Set memory leak checking
   // int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
   // tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
   // _CrtSetDbgFlag( tmpFlag );

   optimizer().m_predict.Init();
   optimizer().m_predict.FillInConfig();
   optimizer().setInitialConditions(m_initial_conditions);
   m_simulation_active = 1;
}

//
//
//-----------------------------------------------------------------------------
void robustness::enable_controls(const int enable) 
{
   m_start_control.EnableWindow(enable);
   m_edit_config_control.EnableWindow(enable);
   m_initial_conditions_control.EnableWindow(enable);
   m_amap_control.EnableWindow(enable);
   m_max_concurrent_control.EnableWindow(enable);
   m_total_products_control.EnableWindow(enable);
   m_repredict_control.EnableWindow(enable);

   //
   //
   //   These members are "reversed"
   //
   m_stop_control.EnableWindow(!enable);

}

//
//
//-----------------------------------------------------------------------------
void robustness::OnStop() 
{
   float percent_complete = (float)m_total_tried / (float)m_total_products;
   m_percent_complete = (int)(percent_complete * 100.0f);
   m_test_suite_progress.SetPos( m_percent_complete );
   UpdateData(0);
   optimizer_form::OnStop();
   enable_controls(1);
}


//
//
//-----------------------------------------------------------------------------
void robustness::OnInitialUpdate() 
{
	optimizer_form::OnInitialUpdate();

   GetParentFrame()->RecalcLayout();
   CSize size = GetTotalSize();
   SetScaleToFitSize(size);
   ResizeParentToFit( /*FALSE*/ );

   m_popup = new CExtendedResults(GetParent());
   m_popup->Create(IDD_EXTENDED_RESULTS,GetParent());
   m_popup->ShowWindow(SW_HIDE);
   m_popup_repredict = new CExtendedResults(GetParent());
   m_popup_repredict->Create(IDD_EXTENDED_RESULTS,GetParent());
   m_popup_repredict->ShowWindow(SW_HIDE);

   set_processing_function(this,(IdleFunction::idle_loop_function)&robustness::IdleCallback);
   enable_controls(1);

   m_hct_start = 38;
   m_hct_stop  = 55;
   m_hct_delta = 1;
   
   m_tbv_start = 3000;
   m_tbv_stop  = 9000;
   m_tbv_delta = 100;

   m_precount_start = 100;
   m_precount_stop  = 575;
   m_precount_delta = 25;

   m_yield_start = 0;
   m_yield_stop  = 12;
   m_yield_delta = 1;

   m_conc_start = 1000;
   m_conc_stop  = 6000;
   m_conc_delta = 100;

   m_plasma_start = 0;
   m_plasma_stop  = 1000;
   m_plasma_delta = 100;
   m_amap = 1;
   m_max_concurrent = 1;

   m_rbcdose_start = 0;
   m_rbcdose_stop  = 400;
   m_rbcdose_delta = 200;

   m_rbccrit_start = 55;
   m_rbccrit_stop  = 80;
   m_rbccrit_delta = 25;

   m_ratio_start = 10.0f;
   m_ratio_stop  = 13.0f;
   m_ratio_delta = 1.0f;

   m_infusion_start = 1;
   m_infusion_stop  = 6;
   m_infusion_delta = 1;

   m_draw_flow_start = 0;
   m_draw_flow_stop  = 2;
   m_draw_flow_delta = 1;

   m_draw_management_start = 1;
   m_draw_management_stop  = 6;
   m_draw_management_delta = 1;

   m_return_management_start = 1;
   m_return_management_stop  = 6;
   m_return_management_delta = 1;

   m_inlet_ramp_start = 0;
   m_inlet_ramp_stop  = 1;
   m_inlet_ramp_delta = 1;

	m_display_concentration = TRUE;
	m_display_unconverged = TRUE;
	m_display_vc_errors = TRUE;
	m_display_vp_errors = TRUE;
	m_display_chamber_flow = TRUE;
	m_display_lockin = FALSE;
   m_repredict = FALSE;

   static const int RBC_ONLY = 0;
   if (RBC_ONLY) {
      m_plasma_stop = 0;
      m_amap = 0;
      m_max_concurrent = 0;
      m_yield_stop = 0;
      m_rbcdose_start = 100;
      m_rbcdose_stop  = 600;
      m_rbcdose_delta = 100;
      m_rbccrit_start = 55;
      m_rbccrit_stop  = 80;
      m_rbccrit_delta = 5;
   }

   static const int PLASMA_ONLY = 0;
   if (PLASMA_ONLY) {
      m_yield_stop = 0;
      m_rbcdose_start = m_rbcdose_stop = 0;
   }

   m_total_products = 100000;

   UpdateData(0);

   OnHct();
   OnPrecount();
   OnTBV();
   OnYield();
   OnConc();
   OnPlasma();
   OnRBCCrit();
   OnRBCDose();

   OnRatio();
   OnInfusion();
   OnDrawFlow();
   OnDrawManagement();
   OnReturnManagement();
   OnInletRamp();

   OnRestoreOutput();
}

//
//
//-----------------------------------------------------------------------------
void robustness::IdleCallback()
{
   static const int NUMBER_PER_ACTIVE_UPDATE = 100;

   Test();


   float percent_complete = (float)m_total_tried / (float)m_total_products;
   m_percent_complete = (int)(percent_complete * 100.0f);
   if ((m_total_tried - m_last_number_updated) >= NUMBER_PER_ACTIVE_UPDATE) {
      m_last_number_updated = m_total_tried;
      m_test_suite_progress.SetPos( m_percent_complete );
      m_elapsed_time.Format("%d hours %d minutes elapsed",time_elapsed_hours(),time_elapsed_minutes());
      float remaining = ((float)time_elapsed().GetTotalSeconds()/1000.0f) / (float)(m_total_tried);
      remaining *= (float)(m_total_products - m_total_tried);
      int seconds = (int)remaining;
      int hours = seconds / 3600;
      seconds -= (hours*3600);
      int minutes = seconds/60;
      m_remaining_time.Format("%d hours %d minutes remaining",hours,minutes);

      //
      //
      //   Refresh dialog
      //
      UpdateData(0);
      
   }
}

//
//
//-----------------------------------------------------------------------------
void robustness::Test()
{
	//cfg_main_input::userconfig->ReadConfig("config.dat");

   if (!m_simulation_active) return;
   if (m_total_tried >= m_total_products) OnStop();

   if (m_sequential) {
      m_current_hct += m_hct_delta;
      if (m_current_hct > m_hct_stop) {
        m_current_hct = m_hct_start;
        m_current_precount += m_precount_delta;
        if (m_current_precount > m_precount_stop) {
          m_current_precount = m_precount_start;
          m_current_tbv += m_tbv_delta;
          if (m_current_tbv > m_tbv_stop) {
            m_current_tbv = m_tbv_start;
            m_current_yield += m_yield_delta;
            if (m_current_yield > m_yield_stop) {
              m_current_yield = m_yield_start;
              m_current_conc += m_conc_delta;
              if (m_current_conc > m_conc_stop) {
                m_current_conc = m_conc_start;
                m_current_plasma += m_plasma_delta;
                if (m_current_plasma > m_plasma_stop) {
                  m_current_plasma = m_plasma_start;
                  m_current_rbccrit += m_rbccrit_delta;
                  if (m_current_rbccrit > m_rbccrit_stop) {
                    m_current_rbccrit = m_rbccrit_start;
                    m_current_rbcdose += m_rbcdose_delta;
                    if (m_current_rbcdose > m_rbcdose_stop) {
                      m_current_rbcdose = m_rbcdose_start;
                      m_current_ratio += m_ratio_delta;
                      if (m_current_ratio > m_ratio_stop) {
                        m_current_ratio = m_ratio_start;
                        m_current_infusion += m_infusion_delta;
                        if (m_current_infusion > m_infusion_stop) {
                          m_current_infusion = m_infusion_start;
                          m_current_draw_flow += m_draw_flow_delta;
                          if (m_current_draw_flow > m_draw_flow_stop) {
                            m_current_draw_flow = m_draw_flow_start;
                            m_current_draw_management += m_draw_management_delta;
                            if (m_current_draw_management > m_draw_management_stop) {
                              m_current_draw_management = m_draw_management_start;
                              m_current_return_management += m_return_management_delta;
                              if (m_current_return_management > m_return_management_stop) {
                                m_current_return_management = m_return_management_start;
                                m_current_inlet_ramp += m_inlet_ramp_delta;
                                if (m_current_inlet_ramp > m_inlet_ramp_stop) {
                                   OnStop();
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }

   } else {
   //
   //
   //   Donor parameters
   //
      if (m_use_normal) {
         //
         //   Normal distribution: hct 35 to 55 by 1
         //       precount 150 to 400 by 10
         //       TBV 3000 to 7000 by 100
         //
         //
         if (m_total_tried == 0) {
            UpdateData(1);
            //
            //
            //   Initialize starting probability grid
            //
            m_hct_start=35; m_hct_stop=55; m_hct_delta=1; m_hct_tests=0;
            m_precount_start=150; m_precount_stop=400; m_precount_delta=10; m_precount_tests=0;
            m_tbv_start=3000; m_tbv_stop=7000; m_tbv_delta=100; m_tbv_tests=0;

            //
            //
            //   For each parameter go through and figure out the number of
            //   weighted samples at each node.
            //
            int value;
            for (value=m_hct_start; value <= m_hct_stop; value += m_hct_delta) {
               m_hct_tests += pseudodistribution((double)value/45.0,70.0);
            }
            for (value=m_precount_start; value <= m_precount_stop; value += m_precount_delta) {
               m_precount_tests += pseudodistribution((double)value/250.0,9.0);
            }
            for (value=m_tbv_start; value <= m_tbv_stop; value += m_tbv_delta) {
               m_tbv_tests += pseudodistribution((double)value/5000.0,16.0);
            }
            UpdateData(0); 
         }

         //
         //
         //  Generate random number across normalized range
         //
         int randomndx = CreateRand(m_hct_tests,1,0);
         int ndx = 0;
         for (m_current_hct=m_hct_start; m_current_hct <= m_hct_stop; m_current_hct += m_hct_delta) {
            ndx += pseudodistribution((double)m_current_hct/45.0,70.0);
            if (ndx > randomndx) break;
         }
         randomndx = CreateRand(m_precount_tests,1,0);
         ndx = 0;
         for (m_current_precount=m_precount_start; m_current_precount <= m_precount_stop; m_current_precount += m_precount_delta) {
            ndx += pseudodistribution((double)m_current_precount/250.0,9.0);
            if (ndx > randomndx) break;
         }
         randomndx = CreateRand(m_tbv_tests,1,0);
         ndx = 0;
         for (m_current_tbv=m_tbv_start; m_current_tbv <= m_tbv_stop; m_current_tbv += m_tbv_delta) {
            ndx += pseudodistribution((double)m_current_tbv/5000.0,16.0);
            if (ndx > randomndx) break;
         }
#ifdef DEBUG_NORMALIZED_DATA
         static fstream outputstream("d:\\temp\\donordebug.csv",ios::in|ios::out|ios::trunc);
         outputstream << m_current_hct << "," << m_current_precount << "," << m_current_tbv << endl;
#endif

      } else {
         m_current_hct = CreateRand(m_hct_tests,m_hct_delta,m_hct_start);
         m_current_precount = CreateRand(m_precount_tests,m_precount_delta,m_precount_start);
         m_current_tbv = CreateRand(m_tbv_tests,m_tbv_delta,m_tbv_start);
      }

   //
   //
   //   Product parameters
   //
   m_current_yield = CreateRand(m_yield_tests,m_yield_delta,m_yield_start);
   m_current_conc = CreateRand(m_conc_tests,m_conc_delta,m_conc_start);
   m_current_plasma = CreateRand(m_plasma_tests,m_plasma_delta,m_plasma_start);
   m_current_rbccrit = CreateRand(m_rbccrit_tests,m_rbccrit_delta,m_rbccrit_start);
   m_current_rbcdose = CreateRand(m_rbcdose_tests,m_rbcdose_delta,m_rbcdose_start);

   //
   //
   //   Configuration parameters
   //
   m_current_ratio = CreateRand(m_ratio_tests,m_ratio_delta,m_ratio_start);
   m_current_infusion = (int)CreateRand(m_infusion_tests,m_infusion_delta,m_infusion_start);
   m_current_draw_flow = (int)CreateRand(m_draw_flow_tests,m_draw_flow_delta,m_draw_flow_start);
   m_current_draw_management = (int)CreateRand(m_draw_management_tests,m_draw_management_delta,m_draw_management_start);
   m_current_return_management = (int)CreateRand(m_return_management_tests,m_return_management_delta,m_return_management_start);
   m_current_inlet_ramp = (int)CreateRand(m_inlet_ramp_tests,m_inlet_ramp_delta,m_inlet_ramp_start);
   m_current_replacement = (int)CreateRand(2,1,0);
   m_current_replacement_percent = (int)CreateRand(3,20,80);

   //
   //
   //   Spread plasma parameters around if AMAP or max concurrent is selected
   //
   if (m_amap && m_max_concurrent) {
      int r = rand();
      if (r > (RAND_MAX/3)) m_current_plasma = -1.0f;
      if (r > ((RAND_MAX*2)/3)) m_current_plasma = -2.0f;
   } else if (m_amap) {
      if (rand() > (RAND_MAX/2)) m_current_plasma = -1.0f;
   } else if (m_max_concurrent) {
      if (rand() > (RAND_MAX/2)) m_current_plasma = -2.0f;
   }
   }

   //
   //
   //   Randomize adjustments.  Ratio adjustments ignored since ratio itself is already random
   //
   if (m_initial_conditions.m_adjust_random) {
      m_initial_conditions.m_ratio_adjustment = m_current_ratio;
      m_initial_conditions.m_qin_adjustment = CreateRand((142-10),1.0f,10.0f)*2.0f;
      if (m_initial_conditions.m_qin_adjustment > 142.0f) m_initial_conditions.m_qin_adjustment = 142.0f;
      m_initial_conditions.m_qrp_adjustment = CreateRand((302-25),1.0f,25.0f)*2.0f;
      if (m_initial_conditions.m_qrp_adjustment > 302.5f) m_initial_conditions.m_qrp_adjustment = 302.5f;
      m_initial_conditions.m_ir_adjustment = CreateRand((120-30),1.0f,30.0f)*2.0f/100.0f;
      if (m_initial_conditions.m_ir_adjustment > 1.2f) m_initial_conditions.m_ir_adjustment = 1.2f;

      //m_initial_conditions.m_ptf = (int)CreateRand(2,1,0);
      float qrbcrand = CreateRand(70,1.0f,0.0f);
      //m_initial_conditions.m_ptf_qrbc = qrbcrand + 20.0f;
      optimizer().setInitialConditions(m_initial_conditions);
   }

   NextOptimization();
   ++m_total_tried;
}

const float robustness::CreateRand(const int tests,const float delta,const float start)
{
   int randomint = int((((double(rand())*double(tests-1))/double(RAND_MAX))+0.5));
   return (float)randomint * delta + start;
}

//
//
//-----------------------------------------------------------------------------
void robustness::NextOptimization() 
{

   static const float EPCAFULL = 250.0f - 10.0f;
   static char specificstr[200];

   ProcRun_CDS *procruncds = new ProcRun_CDS(ROLE_RO);
   ProcTrimaSet_CDS *setcds = new ProcTrimaSet_CDS(ROLE_RO);


   //
   //
   //  Special disqualification cases here
   //
   float pltvol = m_current_yield*100000.0f/m_current_conc;

   if ((pltvol < 50.0f) && (m_current_yield > 0.0f)) 
       return;

   optimizer().donor(__donorheight,
                     __donorweight,
                     m_current_tbv,
                     m_current_hct/100.0f,
                     m_current_hct/100.0f,
                     m_current_precount,
                     BT_UNKNOWN); 
	

   optimizer().m_predict.create_procedure(m_current_yield,
                                          pltvol,
                                          m_current_plasma,
                                          m_current_rbcdose,
                                          m_current_rbccrit,
                                          1.0f, // RSF
                                          cfg_main_input::userconfig->Procedure.Get().key_proc_time,
                                          BT_ALL,
                                          1, // designator
                                          false, 0, 0,   // Metered Platelets
                                          false, 0,    // Metered Redcells
                                          1); // procedure number

   ProcedureConfig proccfg = cfg_main_input::userconfig->Procedure.Get();
   PredictionConfig predictcfg = cfg_main_input::userconfig->Predict.Get();
   proccfg.key_custom_ratio = m_current_ratio;
   proccfg.key_ac_rate = m_current_infusion;
   proccfg.key_max_draw_flow = m_current_draw_flow;
   predictcfg.key_inlet_management = m_current_draw_management;
   predictcfg.key_return_management = m_current_return_management;
   predictcfg.key_inlet_flow_ramp = m_current_inlet_ramp;  
   predictcfg.key_rbc_replace_fluid = m_current_replacement;
   predictcfg.key_rbc_fluid_percent = m_current_replacement_percent;
   cfg_main_input::userconfig->Procedure.Set(proccfg);
   cfg_main_input::userconfig->Predict.Set(predictcfg);

   optimizer().m_predict.FillInConfig();
   m_maxQin = optimizer().m_predict.getMaxInstQin();
   m_maxQrp = optimizer().m_predict.getMaxNeedleRetFlow();
   m_maxInfusion = optimizer().m_predict.getConfigMaxACRate();

   setcds->Cassette.Set(cassette::UNKNOWN);
   setcds->RBCDetectorCalibrated.Set(1);
   procruncds->Substate.Set(SS_CHANNEL_SETUP);

   optimizer().m_predict._selectedProc = 0;
   optimizer().m_predict.SetUnoptimized();
   procedureDetail* pd = optimizer().m_predict.getProcedure(1);


   if (!pd->getProcPlasmaVolFixed()) 
       pd->setPlasmaTargetCannotChange(0);
   pd->ResetProcDet();
   optimizer().m_predict.predictProcedure(*pd);

   if (optimizer().m_predict._OptimizingSelectedProc) 
   {
      m_initial_conditions.m_procedure_run_time = 0.0f;
   }

   //
   //
   //   Capture statistics about current optimization
   //
   float maxInfusion = 0.0f;
   float maxQin = 0.0f;
   float maxQrp = 0.0f;
   
   if (pd->getStatus() == GOOD_PRODUCT)
   {
      for (int subndx=1; subndx < pd->getNumOfSubstates(); subndx++)
      {
         float infusion = pd->instantaneousInfusion(&(optimizer().m_predict),subndx);
      
         if ((infusion <= 0.0f) && (pd->substate[subndx].T > 0.0f)) 
         {
            sprintf(specificstr,"%d %.3f <= 0",subndx,infusion);                
            LogError("INFUSION",specificstr);
         }
          if (infusion > 1.21f) 
         {
            sprintf(specificstr,"%d %.3f <= 0",subndx,infusion);                
            LogError("INFUSION OVER LIMIT (AC over infusion) ",specificstr);
         }

         if (pd->substate[subndx].T > 0.0f && infusion > maxInfusion) 
             maxInfusion = infusion;

         if (pd->substate[subndx].Qininstd > maxQin) 
             maxQin = pd->substate[subndx].Qininstd;

         float qn = pd->substate[subndx].Qrinst - pd->substate[subndx].Qininstr;

         if (qn > maxQrp) 
             maxQrp = qn;
      }
   }

   float hdmax = 15.0f;
   
   if (pd->getProcCollectPlasma() && pd->getPlasmaTargetCannotChange())
       hdmax = 40.0f;

   float _collectmaxflow, chlock1, firstpirflowlt, chlock2; // trima uses 0.45 tolerance
   static const float secpirflowlt = 2.0f; // same for both std and high
       if (m_current_conc > 2100.0f )
    {
		_collectmaxflow		  = CobeConfig::data().CollectMaximumFlowHigh  ;			    // 4.0f
        chlock1				  = CobeConfig::data().FirstMaxPIRChamberFlowHigh + 0.44f;		// 2.0f
        firstpirflowlt        = CobeConfig::data().FirstPIRFlowLimitHigh;					// 1.0f
        chlock2				  = CobeConfig::data().SecondMaxPIRChamberFlowHigh + 0.44f;		// 3.0f
    }  else {
        _collectmaxflow       = CobeConfig::data().CollectMaximumFlowStandard;				// 5.0f
        chlock1				  = CobeConfig::data().FirstMaxPIRChamberFlowStandard + 0.44f;	// 2.5f
        firstpirflowlt		  = CobeConfig::data().FirstPIRFlowLimitStandard;				// 1.5f
        chlock2				  = CobeConfig::data().SecondMaxPIRChamberFlowStandard + 0.44f;	// 3.8f
    }

   float plasmaRinsebackVolume = 0.0f;
   
   if ((pd->getRecommendedCassette() == cassette::PLTPLSRBC) && (cfg_main_input::userconfig->Procedure.Get().key_plasma_rinseback)) 
       plasmaRinsebackVolume = CobeConfig::data().RinsebackVplrb;

   if (pd->getStatus() == GOOD_PRODUCT)
   {
      m_good++;

#ifdef DEBUG_PFH_EFFECT_OF_CONFIG_ON_TIME_AND_INFUSION
      float facb = (1.0f/((ratio-1.0f)*(1.0f-hct) + 1.0f));
      float facbr = (1.0f/((rbcratio-1.0f)*(1.0f-hct) + 1.0f));
      float acinproducts = pd->_PredVc*facb + pd->_PredVp*facb + pd->getPredRBCVolume()*facbr*(1.0f-pd->getPredRBCCrit()/100.0f);
      float avginfusion = (pd->substate[pd->RBCState()].AccumVac - acinproducts)/pd->_PredTp/(m_current_tbv/1000.0f);
	  sprintf(specificstr,"%.1f %.1f %d %d %d %d %.2f %.2f %.1f %.1f",m_current_tbv,m_current_hct,m_current_infusion,m_current_draw_flow,m_current_draw_management,m_current_return_management,maxInfusion,avginfusion,maxQin,pd->_PredTp);
      LogError("DEBUG_PFH",specificstr);
#endif

#ifdef DEBUG_AVG_PROCEDURE_TIME
      static float totaltime = 0.0f;
      totaltime += pd->_PredTp;
      m_debug = totaltime/(float)m_good;
#endif
      
      float pltErr = 0.0f;
      static const double ALLOWABLE_PLT_ERR = 0.05;
      
      if (pd->getProcFixedCollectVol() > 0.0f)
      {
         pltErr = (pd->_PredVc - pd->getProcFixedCollectVol())/pd->getProcFixedCollectVol();
      }
      
      if (fabs(pltErr) <= ALLOWABLE_PLT_ERR) 
         m_inconc++;
      
      if (m_current_conc <= 2001.0f)
      {
         m_good_lt_2000++;
         
         if (fabs(pltErr) <= ALLOWABLE_PLT_ERR) 
            m_inconc_lt_2000++;
      }
      else if (m_current_conc <= 4001.0f)
      {
         m_good_lt_4000++;
      
         if (fabs(pltErr) <= ALLOWABLE_PLT_ERR) 
             m_inconc_lt_4000++;
      }
      else
      {
         m_good_gt_4000++;
      
         if (fabs(pltErr) <= ALLOWABLE_PLT_ERR) 
             m_inconc_gt_4000++;
      }

      int epcafull = 0;
      
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) 
      {
         epcafull = ((pd->substate[Predict::__EXTENDED_PCA].EndVin - pd->substate[Predict::__EXTENDED_PCA-1].EndVin) > EPCAFULL);
      }

      //
      //
      //   - - - - - debug - - - -
      //
#ifdef debug_vin
      static float debug_minvin = 9999999.0f;
      static float debug_maxvin = 0.0f;
      float vin = pd->VinTarget();
      if (vin < debug_minvin) {
         sprintf(specificstr,"%1.f",vin);
         LogError("DEBUG_MINVIN",specificstr);
         debug_minvin = vin;
      }
      
      if (vin > debug_maxvin)
      {
         sprintf(specificstr,"%1.f",vin);
         LogError("DEBUG_MAXVIN",specificstr);
         debug_maxvin = vin;
      }
#endif
#ifdef debug_pir
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {
         if ((pd->substate[Predict::__PIR_PLASMA].T > 5.0f) &&
            (pd->substate[Predict::__PIR_NOPLASMA].T > 3.0f)) {
            sprintf(specificstr,"%.1f %.1f",pd->substate[Predict::__PIR_PLASMA].T,pd->substate[Predict::__PIR_NOPLASMA].T);
            LogError("DEBUG_PIR",specificstr);
         }
      }
#endif
#ifdef debug_infusion
      float delta = maxInfusion - m_maxInfusion;
      sprintf(specificstr, "%.3f %.3f %.3f", m_maxInfusion,maxInfusion,(maxInfusion - m_maxInfusion));
      LogError("DEBUG_INFUSION",specificstr);
#endif


      //
      //
      //   General error conditions
      //
      if (maxInfusion > (m_maxInfusion+0.0005f))
      {
         sprintf(specificstr,"%.3f > %.3f (%.3f)",maxInfusion,m_maxInfusion,(maxInfusion-m_maxInfusion));                
         LogError("INFUSION",specificstr);
      }

//  Find procedures close to infusion limit
//      if (maxInfusion > (m_maxInfusion - 0.05f)) {
//         sprintf(specificstr,"%.3f > %.3f (%.3f)",maxInfusion,m_maxInfusion,(maxInfusion-m_maxInfusion));                
//         LogError("DEBUG_INFUSION",specificstr);
//      }


      if ((m_current_yield > 0.0f) && (pd->substate[3].EndVin < 399.0f)) 
      {
         sprintf(specificstr,"%1.f",pd->substate[3].EndVin);
         LogError("PLTVALVE",specificstr);
      }

      // substate[3].EndVin can increase if attempting to meet min vol in substate
//      if ((m_current_yield > 0.0f) && (fabs(pd->substate[3].EndVin - pd->getVincv()) > 100.0)) {
//         sprintf(specificstr,"%1.f %.1f",pd->substate[3].EndVin,pd->getVincv());
//         LogError("PLTVALVE_VINCV",specificstr);
//      }

      // substate[3].EndVin can increase if attempting to meet min vol in substate
//      if ((m_current_yield > 0.0f) && ((pd->substate[3].EndVin+20.0f) < pd->getVincv())) {
//         sprintf(specificstr,"%1.f %.1f",pd->substate[3].EndVin,pd->getVincv());
//         LogError("PLTVALVE_LT_VINCV",specificstr);
//      }

      if ((pd->getRecommendedCassette() == cassette::PLTPLSRBC) ||
          (m_current_replacement == 0))
      {
         if (pd->_PredVrep > 0.0f) 
         {
            sprintf(specificstr,"%.3f",pd->_PredVrep);
            LogError("REPLACEMENT",specificstr);
         }
         
         for (int i=1;i<=Predict::__RBC;i++) 
         {
            if (pd->substate[i].Qrep > 0.01f)
            {
               sprintf(specificstr,"%d %.3f",i,pd->substate[i].Qrep);
               LogError("REPLACEMENT_FLOW",specificstr);
            }
            
            if (pd->substate[i].AccumVrep > 0.0f) 
            {
               sprintf(specificstr,"%d %.3f",i,pd->substate[i].AccumVrep);
               LogError("REPLACEMENT_VOL",specificstr);
            }
         }
      }

      if (!pd->getProcCollectPlasma() && pd->_PredVp > 0.0f) 
      {
         if ((plasmaRinsebackVolume == 0.0f) || (fabs(pd->_PredVp - plasmaRinsebackVolume) > 0.4)) 
         {
            sprintf(specificstr,"%.1f",pd->_PredVp);
            LogError("VP_CREATED",specificstr);
         }
      }


#ifdef IT5594_fixed_via_code
      //
      //   Check for Ydch consistency
      //
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) 
      {
         if ((pd->substate[Predict::__PIR_PLASMA].T < 0.0f) &&
            (pd->substate[Predict::__PIR_NOPLASMA].T < 0.0f)) 
         {
            if (fabs(pd->substate[Predict::__PIR_PLASMA].YdchPIR - pd->substate[Predict::__PIR_PLASMA].YdchNoPIR) > 0.05) 
            {
               sprintf(specificstr,"%.3f %.3f",pd->substate[Predict::__PIR_PLASMA].YdchPIR,pd->substate[Predict::__PIR_PLASMA].YdchNoPIR);
               LogError("YDCH_IT5594",specificstr);
            }
         }
      }
#endif

      //
      //
      //    Flow errors
      //
      if (maxQin > (m_maxQin+0.1f)) 
      {
         sprintf(specificstr,"%.1f > %.1f",maxQin,m_maxQin);
         LogError("INLET",specificstr);
      }
      
      if (maxQrp > (m_maxQrp+0.1f))
      {
         sprintf(specificstr,"%.1f > %.1f",maxQrp,m_maxQrp); 
         LogError("RETURN",specificstr);
      }

      for (int subndx = 1; subndx < pd->getNumOfSubstates(); subndx++) 
      {
         if (pd->substate[subndx].T > 0.0f) 
         {
            //  V5.x duty cycle checked in offline flows.cpp
//            float DC = pd->substate[subndx].Td/pd->substate[subndx].Tcn;
//            if (fabs(DC - pd->substate[subndx].D) > 0.02) {
//               sprintf(specificstr,"%d %.3f %.3f",subndx,DC,pd->substate[subndx].D);
//               if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {
//                  LogError("DC_IT5589",specificstr);
//               } else {
//                  LogError("DC_RBCPL_IT5589",specificstr);
//               }
//            }
         
            // IT5792
            if ((pd->substate[subndx].Qininstr > 0.0f) && ((pd->substate[subndx].Qininstr+20.0f) > pd->substate[subndx].Qrinst))
            {
               sprintf(specificstr,"%d %.1f %.1f",subndx,pd->substate[subndx].Qininstr,pd->substate[subndx].Qrinst);
               LogError("QRP_LT_QIN",specificstr);
            }
            
            if (pd->substate[subndx].Qrinst < 20.0f) 
            {
               sprintf(specificstr,"%d %.1f %.1f",subndx,pd->substate[subndx].Qininstr,pd->substate[subndx].Qrinst);
               LogError("QRP_LT_MIN",specificstr);
            }

            //
            //
            //  Chamber flow should not change between draw/return in a substate
            //
            if (pd->getRecommendedCassette() == cassette::PLTPLSRBC)
            {
               if (subndx == Predict::__RBC)
               {
                  if ((pd->substate[subndx].Qcinstr != 0.0f) ||
                      (pd->substate[subndx].Qpinstr != 0.0f) ||
                      (pd->substate[subndx].Qcinstd <= 0.0f) ||
                      (pd->substate[subndx].Qcinstd <= 0.0f))
                  {
                     sprintf(specificstr,"%d %.1f %.1f",subndx,pd->substate[subndx].Qcinstd,pd->substate[subndx].Qcinstr);
                     LogError("BAD_RBC_QCH",specificstr);
                  }
               }
               else
               {
// RKM
#if 0
                  if (pd->substate[subndx].Qcinstd != pd->substate[subndx].Qcinstr)
                  {
                      if ( (subndx != Predict::__RBC_SETUP_1) && (subndx != Predict::__RBC_SETUP_2) )
                      {
                          sprintf(specificstr,"%d %.1f %.1f",subndx,pd->substate[subndx].Qcinstd,pd->substate[subndx].Qcinstr);
                          LogError("BAD_QC_QCH",specificstr);
                      }
                  }
                  
                  if (pd->substate[subndx].Qpinstd != pd->substate[subndx].Qpinstr)
                  {
                      if ( (subndx != Predict::__RBC_SETUP_1) && (subndx != Predict::__RBC_SETUP_2) )
                      {
                      
                          sprintf(specificstr,"%d %.1f %.1f",subndx,pd->substate[subndx].Qcinstd,pd->substate[subndx].Qcinstr);
                          LogError("BAD_QP_QCH",specificstr);
                      }
                  }
#endif
               }
            }

         }
      }

      //
      //
      //  Check platelet collection states to ensure fcmax isn't violated
      //
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC)
      {
         for (subndx = 2; subndx <= 10; subndx++)
         {
            if (pd->substate[subndx].T > 0.0f && subndx != 3)
            {
               float col = pd->substate[subndx].Qcinstd; // 5,6,8,9
               if ((subndx==4) || (subndx==7))
                   col += pd->substate[subndx].Qpinstd;

               if ((subndx==2) || (subndx==10)) 
                   col = pd->substate[subndx].Qpinstd;

               const float generalfc = (1.0f - ((m_current_hct/100.0f)*(1.0f-1.0f/m_current_ratio))/CobeConfig::data().MaxPlateletPlasmaHrbc);
               float fcmax;

              // if (pd->getProcPlateletProdCon() >= CobeConfig::data().RecirculateConcentration)
              // {
              //    fcmax = (generalfc / (1.0f + CobeConfig::data().RecirculatePlateletCollect * (1.0f/pd->substate[subndx].D - 1.0f)));
              // }
              // else
              // {
                  const float A = (1.0f/(1.0f-pd->substate[subndx].D)+CobeConfig::data().MinRecirculatePlateletCollect/pd->substate[subndx].D)/2.0f;
                  const float dcf = 1.0f-CobeConfig::data().RecirculateSnDrawVol/pd->substate[subndx].Vsvn;
                  fcmax = (sqrt(A*A+dcf*generalfc/(1.0f-pd->substate[subndx].D))-A)/dcf;
              // }

               if (col > (fcmax*pd->substate[subndx].Qininstd*1.1f))
               { // allow 10%; fcmax routinely exceeded in PIR
                  sprintf(specificstr,"%d %.1f + %.1f > %.3f (%.1f %.1f)",subndx,pd->substate[subndx].Qpinstd,pd->substate[subndx].Qcinstd,fcmax,col,fcmax*pd->substate[subndx].Qininstd);
                  LogError("FCMAX",specificstr);
               }
            }
         }
      }

      //
      //
      //  Check platelet collection states to ensure Qc during PCA and Midrun are correct.
      //
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC)
      {
         // Qc should not be greater than Qcmr (by definition)
         if ( (pd->substate[6].T > 0.0f) &&
              (pd->substate[6].Qcinstd > pd->toplevel.Qcmr)
            )
         {
            sprintf(specificstr,"%.1f < %.1f)", pd->substate[6].Qcinstd, pd->toplevel.Qcmr);
            LogError("QC-QCMR",specificstr);
         }

         // Midrun flow should be lower than PCA.
         if ( (pd->substate[6].T > 0.0f) && (pd->substate[5].T > 0.0f) &&
              (pd->substate[6].Qcinstd >= pd->substate[5].Qcinstd)
            )
         {
            sprintf(specificstr,"%.1f < %.1f)", pd->substate[5].Qcinstd, pd->substate[6].Qcinstd);
            LogError("QCPCA-QCMR",specificstr);
         }

      }

      //
      //
      //  Check to see that the 100% recirc isn't violated
      //
      //if (pd->getRecommendedCassette() == cassette::PLTPLSRBC)
      //{
      //   for (int i = 2; i <= 10; i++)
      //   {
      //      if ( (pd->substate[i].T > 0.0f) && (m_current_conc > CobeConfig::data().RecirculateConcentration) &&
      //           (pd->substate[i].Qininstd != (pd->substate[i].Qininstr * CobeConfig::data().RecirculatePlateletCollect))
      //         )
      //      {
      //         sprintf(specificstr,"%i %.1f != %.1f)", i, pd->substate[i].Qininstd, pd->substate[i].Qininstr * CobeConfig::data().RecirculatePlateletCollect);
      //         LogError("RECIRC",specificstr);
      //      }
      //   }
      //}


      //
      //
      //  Check PIR volume accuracy
      //
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC)
      {
         float PIRTime = pd->substate[Predict::__PIR_PLASMA].T + pd->substate[Predict::__PIR_NOPLASMA].T;
      
         if (PIRTime > 0.0f) 
         {
            float startqch = (pd->substate[Predict::__PLATELET_PLASMA].Qch > 0.0f) ? pd->substate[Predict::__PLATELET_PLASMA].Qch : pd->substate[Predict::__MIDRUN].Qch;
            float ramptime = (pd->substate[Predict::__PIR_NOPLASMA].Qch - startqch)/CobeConfig::data().CollectRampRate;
            float vcatmax = pd->substate[Predict::__PIR_NOPLASMA].Qch*(PIRTime - ramptime);
         
            if (fabs(vcatmax - 20.0) > 10.0) 
            {
               sprintf(specificstr,"%.1f (%.1f of %.1f)",vcatmax,ramptime,PIRTime);
               LogError("PIR_VC",specificstr);
            }
            
            if ((pd->substate[Predict::__PIR_PLASMA].T > 0.0f) && (pd->substate[Predict::__PIR_NOPLASMA].T > 0.0f))
            {
               if (fabs(pd->substate[Predict::__PIR_PLASMA].Qininstd - pd->substate[Predict::__PIR_NOPLASMA].Qininstd) > 1.0) 
               {
                  sprintf(specificstr,"%.1f %.1f",pd->substate[Predict::__PIR_PLASMA].Qininstd,pd->substate[Predict::__PIR_NOPLASMA].Qininstd);
                  LogError("PIR_QIN",specificstr);
               }
            }
         }
      }


      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) 
      {

         float  collectFlowMax = CobeConfig::data().CollectMaximumFlowStandard;
         if (pd->getProcPlateletProdCon() > 2100.0f)
         {
             collectFlowMax = CobeConfig::data().CollectMaximumFlowHigh;
         }

         if ((pd->substate[Predict::__PCA].T > 0.0f) &&
            ((pd->substate[Predict::__PCA].Qcinstd - pd->substate[Predict::__PCA].Qpinstd) < (collectFlowMax-0.01f))) 
         {
            sprintf(specificstr,"%.1f - %.1f = %.1f",pd->substate[Predict::__PCA].Qcinstd,pd->substate[Predict::__PCA].Qpinstd,(pd->substate[Predict::__PCA].Qcinstd - pd->substate[Predict::__PCA].Qpinstd)); 
            LogError("PCA_FLOW",specificstr);
         }
      }

      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC)
      {
         if (pd->substate[Predict::__RBC].T > 0.0f)
         {
            const float dcf = 1.0f-CobeConfig::data().RecirculateSnDrawVol/pd->substate[Predict::__RBC].Vsvn;
            float avgqin = pd->substate[Predict::__RBC].Qininstd * pd->substate[Predict::__RBC].D * dcf;
            
            if (avgqin > 90.004f)
            {
               sprintf(specificstr,"%.1f * %.3f * %.3f = %.3f",pd->substate[Predict::__RBC].Qininstd,pd->substate[Predict::__RBC].D,dcf,avgqin); 
               LogError("RBC_FLOW",specificstr);
            }

            //
            //
            //   Check Hrbc for RBC state
            //
            float rbcratio = (cfg_main_input::userconfig->Predict.Get().key_rbc_ratio == 0 ? 8.14f : 11.0f);
            
            float Hinr = (m_current_hct/100.0f) * ( 1.0f - 1.0f/rbcratio ) *
                            ( 1.0f-CobeConfig::data().RecirculateSnDrawVol / 
                              pd->substate[Predict::__RBC].Vsvn );
            
            float Hrbc = ( pd->substate[Predict::__RBC].Qininstd * Hinr ) /
                     ( pd->substate[Predict::__RBC].Qininstd - pd->substate[Predict::__RBC].Qcinstd );

            if (fabs(Hrbc - (m_current_rbccrit/100.0f)) > 0.01) 
            {
               sprintf(specificstr,"%.3f %.1f",Hrbc,m_current_rbccrit); 
               LogError("RBC_HRBC",specificstr);
            }

            //
            //
            //  Ensure Qrbcmax limit is not violated
            //
// RKM
#if 0
            if (setcds->RBCPTFSet.Get()) 
            {
               const float qrbc = pd->substate[Predict::__RBC].Qininstd - pd->substate[Predict::__RBC].Qcinstd;
            
               if ((qrbc-0.01f) > procruncds->PTFQrbc.Get())
               {
                  sprintf(specificstr,"Qin %.1f - Qc %.3f = %.3f > %.3f",pd->substate[Predict::__RBC].Qininstd,pd->substate[Predict::__RBC].Qcinstd,qrbc,procruncds->PTFQrbc.Get()); 
                  LogError("RBC_QRBC_LIMIT",specificstr);
               }


               //
               //
               //   Check Hrbc for setup states
               //
               Hinr = (m_current_hct/100.0f) * ( 1.0f - 1.0f/rbcratio ) *
                   ( 1.0f-CobeConfig::data().RecirculateSnDrawVol / 
                   CobeConfig::data().ReservoirVolume );
               
               Hrbc = ( pd->substate[Predict::__RBC_SETUP_1].Qininstd * Hinr ) /
                   ( pd->substate[Predict::__RBC_SETUP_1].Qininstd - pd->substate[Predict::__RBC_SETUP_1].Qcinstd );
               
               if (fabs(Hrbc - (CobeConfig::data().PltRbcSetup1Hrbc/100.0f)) > 0.01)
               {
                   sprintf(specificstr,"%.3f %.1f",Hrbc,CobeConfig::data().PltRbcSetup1Hrbc); 
                   LogError("RBC_HRBC_1",specificstr);
               }
               
               Hrbc = ( pd->substate[Predict::__RBC_SETUP_2].Qininstd * Hinr ) /
                   ( pd->substate[Predict::__RBC_SETUP_2].Qininstd - pd->substate[Predict::__RBC_SETUP_2].Qcinstd );

               if (fabs(Hrbc - (CobeConfig::data().PltRbcSetup2Hrbc/100.0f)) > 0.01)
               {
                   sprintf(specificstr,"%.3f %.1f",Hrbc,CobeConfig::data().PltRbcSetup2Hrbc); 
                   LogError("RBC_HRBC_2",specificstr);
               }
            }
#endif
         }
      }

      float  collectFlowMax = CobeConfig::data().CollectMaximumFlowStandard;
      if (pd->getProcPlateletProdCon() > 2100.0f)
      {
          collectFlowMax = CobeConfig::data().CollectMaximumFlowHigh;
      }
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {


         if ((pd->substate[Predict::__MIDRUN].T > 0.0f) &&
            ((pd->substate[Predict::__MIDRUN].Qcinstd - pd->substate[Predict::__MIDRUN].Qpinstd) < (collectFlowMax-0.01f)) &&
            (pd->substate[Predict::__MIDRUN].Qcinstd > collectFlowMax)) {
            sprintf(specificstr,"%.1f - %.1f = %.1f",pd->substate[Predict::__MIDRUN].Qcinstd,pd->substate[Predict::__MIDRUN].Qpinstd,(pd->substate[Predict::__MIDRUN].Qcinstd - pd->substate[Predict::__MIDRUN].Qpinstd)); 
            LogError("MIDRUN_FLOW",specificstr);
         }
      }
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {


         if ((pd->substate[Predict::__MIDRUN].T > 0.0f) &&
             (pd->substate[Predict::__MIDRUN].Qcinstd < (collectFlowMax-0.01f)) &&
             (pd->substate[Predict::__MIDRUN].Qpinstd > 0.0f)) {
            sprintf(specificstr,"%.1f - %.1f = %.1f",pd->substate[Predict::__MIDRUN].Qcinstd,pd->substate[Predict::__MIDRUN].Qpinstd,(pd->substate[Predict::__MIDRUN].Qcinstd - pd->substate[Predict::__MIDRUN].Qpinstd)); 
            LogError("MIDRUN_QCH",specificstr);
         }
      }

#if 0  // RKM Get with butzke on how to fix this.
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC && pd->getPredRBCVolume() > 0.0f) {
         float pdqrbc = (pd->substate[pd->RBCState()].Qininstd - pd->substate[pd->RBCState()].Qpinstd)*pd->substate[pd->RBCState()].D;
         if (fabs(pdqrbc - pd->substate[pd->RBCState()].Qrbc) > 0.4) {
            float DC = pd->substate[pd->RBCState()].Qrinst/(pd->substate[pd->RBCState()].Qrinst+pd->substate[pd->RBCState()].Qininstd*(1.0f-(1.0f-1.0f/8.14f)*(m_current_hct/100.0f)/(m_current_rbccrit/100.0f)));
            sprintf(specificstr,"%.1f %.1f DC= %.3f %.3f",pdqrbc,pd->substate[pd->RBCState()].Qrbc,DC,pd->substate[pd->RBCState()].D);
//            LogError("RBC_FLOW",specificstr);
         }
      }
#endif

      //
      //
      //  The below code was useful for verification of average flows but in general
      //  hard to check due to dynamic hct.
      //

#if 0   // RKM Get with butzke on how to fix this
      if (pd->getRecommendedCassette() == cassette::RBCPLS)
      {
         if (pd->getProcCollectPlasma())
         {
            float qpc = (pd->substate[Predict::__RP_PLASMA_ONLY].Qin/pd->substate[Predict::__RP_PLASMA_ONLY].D/(1.0f-CobeConfig::data().RecirculateSnDrawVol/pd->substate[Predict::__RP_PLASMA_ONLY].Vsvn))*pd->substate[Predict::__RP_PLASMA_ONLY].D*(1.0f-(pd->substate[Predict::__RP_PLASMA_ONLY].AvgHt*(1.0f-1.0f/m_current_ratio))/CobeConfig::data().MaxRbcPlasmaHrbc);
            
            if (fabs(qpc - pd->substate[Predict::__RP_PLASMA_ONLY].Qpc) > 1.0) 
            {
               float DC;

               if (m_current_replacement) 
               {
                  float balance = cfg_main_input::userconfig->Predict.Get().key_rbc_fluid_percent/100.0f;
                  DC = 1.0f/(1.0f+((balance-1.0f)*
                     (pd->substate[Predict::__RP_PLASMA_ONLY].Qininstd*(1.0f-pd->substate[Predict::__RP_PLASMA_ONLY].AvgHt/(m_current_rbccrit/100.0f)*(1.0f-1.0f/m_current_ratio)))+
                     (1.0f-balance/m_current_ratio)*pd->substate[Predict::__RP_PLASMA_ONLY].Qininstd)/pd->substate[Predict::__RP_PLASMA_ONLY].Qrinst);
               }
               else
               {
                  DC = 1.0f/(1.0f+(pd->substate[Predict::__RP_PLASMA_ONLY].Qininstd*pd->substate[Predict::__RP_PLASMA_ONLY].AvgHt/(m_current_rbccrit/100.0f)*(1.0f-1.0f/m_current_ratio)/pd->substate[Predict::__RP_PLASMA_ONLY].Qrinst));
               }
               
               sprintf(specificstr,"%.2f %.2f DC= %.3f %.3f",qpc,pd->substate[Predict::__RP_PLASMA_ONLY].Qpc,DC,pd->substate[Predict::__RP_PLASMA_ONLY].D);
//               LogError("RP_PLASMA_FLOW",specificstr);
            }
         }

         if (pd->getPredRBCVolume() > 0.0f) 
         {
            float qpc = (pd->substate[Predict::__RP_RBC].Qin/pd->substate[Predict::__RP_RBC].D/(1.0f-CobeConfig::data().RecirculateSnDrawVol/pd->substate[Predict::__RP_RBC].Vsvn))*pd->substate[Predict::__RP_RBC].D*(1.0f-(pd->substate[Predict::__RP_RBC].AvgHt*(1.0f-1.0f/8.14f)*(1.0f-CobeConfig::data().RecirculateSnDrawVol/pd->substate[Predict::__RP_RBC].Vsvn))/(m_current_rbccrit/100.0f));
            float pdqrbc = (pd->substate[pd->RBCState()].Qin - qpc);
         
            if (fabs(pdqrbc - pd->substate[pd->RBCState()].Qrbc) > 0.4)
            {
               sprintf(specificstr,"%.1f %.1f",pdqrbc,pd->substate[pd->RBCState()].Qrbc);
//               LogError("RP_RBC_FLOW",specificstr);
            }
         }
      }
#endif


      // Checking for PTF stuff for Black Stamp
//RKM
#if 0
      if (pd->getRecommendedCassette() == cassette::RBCPLS)
      {
         const struct IntermediateResults &rbcSubstate = pd->substate[Predict::__RP_RBC];
         const struct IntermediateResults &rbcSetup1Substate = pd->substate[Predict::__RP_RBC_SETUP_1];
         const struct IntermediateResults &rbcSetup2Substate = pd->substate[Predict::__RP_RBC_SETUP_2];

         const float rbcratio = (cfg_main_input::userconfig->Predict.Get().key_rbc_ratio == 0 ? 8.14f : 11.0f);

         float Qrbc;
         float Hinr;
         float Hrbc;

         // RBC collection stuff
         if (rbcSubstate.T > 0.0f)
         {
            //
            //
            //  Ensure Qrbcmax limit is not violated
            //
            if (setcds->RBCPTFSet.Get()) 
            {
               Qrbc = rbcSubstate.Qininstd - rbcSubstate.Qpinstd;

               if ((Qrbc - 0.01f) > procruncds->PTFQrbc.Get())
               {
                  sprintf(specificstr,"Qin %.1f - Qc %.3f = %.3f > %.3f", rbcSubstate.Qininstd, rbcSubstate.Qcinstd, Qrbc, procruncds->PTFQrbc.Get()); 
                  LogError("RBC_QRBC_LIMIT",specificstr);
               }

               //
               //
               //   Check Hrbc for setup states
               //
               const float Hinr = (m_current_hct/100.0f) * ( 1.0f - 1.0f/rbcratio ) *
                   ( 1.0f-CobeConfig::data().RecirculateSnDrawVol / CobeConfig::data().ReservoirVolume );

               const float setup1Hrbc = CobeConfig::data().PltRbcSetup1Hrbc/100.0f;

               // Don't do test if crit is not possible to meet.
               if (setup1Hrbc <= 0.8f)
               {
                  Qrbc = rbcSetup1Substate.Qininstd - rbcSetup1Substate.Qpinstd;

                  float Hrbc = ( rbcSetup1Substate.Qininstd * Hinr ) / Qrbc;

                  if (fabs(Hrbc - setup1Hrbc) > 0.01)
                  {
                      sprintf(specificstr,"%.3f %.1f", Hrbc, CobeConfig::data().PltRbcSetup1Hrbc); 
                      LogError("RBC_HRBC_1",specificstr);
                  }
               }


               const float setup2Hrbc = CobeConfig::data().PltRbcSetup2Hrbc/100.0f;

               Qrbc = rbcSetup2Substate.Qininstd - rbcSetup2Substate.Qpinstd;
               Hrbc = ( rbcSetup2Substate.Qininstd * Hinr ) / Qrbc;

               if (fabs(Hrbc - setup2Hrbc) > 0.05)
               {
                   sprintf(specificstr,"%.3f %.1f",Hrbc,CobeConfig::data().PltRbcSetup2Hrbc); 
                   LogError("RBC_HRBC_2",specificstr);
               }
            }

            //
            //
            //   Check Hrbc for RBC state
            //
            Qrbc = rbcSubstate.Qininstd - rbcSubstate.Qpinstd;
            Hinr = (m_current_hct / 100.0f) * ( 1.0f - 1.0f / rbcratio ) *
                            ( 1.0f - CobeConfig::data().RecirculateSnDrawVol / rbcSubstate.Vsvn );
            Hrbc = ( rbcSubstate.Qininstd * Hinr ) / Qrbc;

            if (fabs(Hrbc - (m_current_rbccrit/100.0f)) > 0.01) 
            {
               sprintf(specificstr, "%.3f %.3f", Hrbc, m_current_rbccrit / 100.0f); 
               LogError("RBC_HRBC",specificstr);
            }

         }
      }
#endif

      //
      //
      //   Chamber flow errors
      //
      if (m_display_chamber_flow) {
         if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {
            float chamberflowlock = _collectmaxflow + 0.5f;
            float maxchamberflow = 0.0f;
            float minchamberflow = _collectmaxflow;
            int lockexceeded = 0;
            int foundone = 0;
            for (int subndx=2; subndx < Predict::__EXTENDED_PCA; subndx++) {
               if (pd->substate[subndx].T > 0.0f) {
                   foundone = 1;

                  float qch = pd->substate[subndx].Qcinstd - pd->substate[subndx].Qpinstd;
                  if (subndx == Predict::__PREPLATELET_PLASMA ||
                      subndx == Predict::__PLATELET_PLASMA ||
                      subndx == Predict::__PIR_PLASMA) {
                     qch = pd->substate[subndx].Qcinstd;
                  }
                  if (qch > maxchamberflow) maxchamberflow = qch;
                  if (qch > chamberflowlock) lockexceeded = 1;
                  if (qch < minchamberflow) {
                     minchamberflow = qch;
                     if (chamberflowlock > chlock2 && qch < secpirflowlt) chamberflowlock = chlock2;
                     if (chamberflowlock > chlock1 && qch < firstpirflowlt) chamberflowlock = chlock1;
                  }
               }
            }

            if (foundone && maxchamberflow > 5.05f) {
               sprintf(specificstr,"%.1f",maxchamberflow);
               LogError("MAX_CHAMBERFLOW",specificstr);
            }
            if (foundone && maxchamberflow < 0.5f) {
               sprintf(specificstr,"%.1f",maxchamberflow);
               LogError("MIN_CHAMBERFLOW",specificstr);
            }
      
            if (foundone && lockexceeded) {
               sprintf(specificstr,"%.1f",minchamberflow);
               LogError("CHAMBERFLOW_LOCK",specificstr);
            }

            if ((pd->substate[Predict::__PREPLATELET_PLASMA].T > 0.0f) &&
               (pd->substate[Predict::__PLATELET_PLASMA].T > 0.0f)) {
               if ((pd->substate[Predict::__PREPLATELET_PLASMA].Qch < 2.0f && pd->substate[Predict::__PLATELET_PLASMA].Qcc > chlock2) ||
                  (pd->substate[Predict::__PREPLATELET_PLASMA].Qch < 1.5f && pd->substate[Predict::__PLATELET_PLASMA].Qcc > chlock1)) {
                  sprintf(specificstr,"%.1f %.1f",pd->substate[Predict::__PREPLATELET_PLASMA].Qch,pd->substate[Predict::__PLATELET_PLASMA].Qcc);
                  LogError("QCH_PLTPL",specificstr);
               }
            }
            
            if ((minchamberflow < 1.2f) && (m_current_conc <= 2100.0f)) {
               sprintf(specificstr,"%.1f %.1f",minchamberflow,m_current_conc);
               LogError("QCH_FLAG",specificstr);
            }
            
            if ((pd->substate[Predict::__PREPLATELET_NOPLASMA].T > 0.0f) &&
               (pd->substate[Predict::__MIDRUN].T > 0.0f)) {
               if ((pd->substate[Predict::__PREPLATELET_NOPLASMA].Qch < 2.0f && pd->substate[Predict::__MIDRUN].Qch > chlock2) ||
                  (pd->substate[Predict::__PREPLATELET_NOPLASMA].Qch < 1.5f && pd->substate[Predict::__MIDRUN].Qch > chlock1)) {
                  sprintf(specificstr,"%.1f %.1f",pd->substate[Predict::__PREPLATELET_NOPLASMA].Qch,pd->substate[Predict::__MIDRUN].Qch);
                  LogError("QCH_MIDRUN2",specificstr);
               }
            }
            
            //  IT5363
#ifdef IT5363_nonissue
            if ((pd->substate[Predict::__PREPLATELET_NOPLASMA].T > 0.0f) &&
               (pd->substate[Predict::__MIDRUN].T > 0.0f)) {
               if (pd->substate[Predict::__PREPLATELET_NOPLASMA].Qch < (pd->substate[Predict::__MIDRUN].Qch-1.0f) ) {
                  sprintf(specificstr,"%.1f %.1f",pd->substate[Predict::__PREPLATELET_NOPLASMA].Qch,pd->substate[Predict::__MIDRUN].Qch);
                  LogError("QCH_DROP_3_6",specificstr);
               }
            }
            if ((pd->substate[Predict::__PLATELET_PLASMA].T > 0.0f) &&
               (pd->substate[Predict::__MIDRUN].T > 0.0f)) {
               if (pd->substate[Predict::__PLATELET_PLASMA].Qch < (pd->substate[Predict::__MIDRUN].Qch-1.0f) ) {
                  sprintf(specificstr,"%.1f %.1f",pd->substate[Predict::__PLATELET_PLASMA].Qch,pd->substate[Predict::__MIDRUN].Qch);
                  LogError("QCH_DROP_4_6",specificstr);
               }
            }
#endif

            //IT5427
            if (pd->substate[Predict::__MIDRUN].T > 0.0f) {
               if ((pd->substate[Predict::__MIDRUN].Qch < 4.5f) && (pd->substate[Predict::__MIDRUN].Qcc > 5.05f)) {
                  sprintf(specificstr,"%.1f %.1f",pd->substate[Predict::__MIDRUN].Qch,pd->substate[Predict::__MIDRUN].Qcc);
                  LogError("QCH_IT5427",specificstr);
               }
            }

            
            if ((pd->substate[Predict::__PLATELET_PLASMA].T > 0.0f) &&
               (pd->substate[Predict::__MIDRUN].T > 0.0f)) {
               if ((pd->substate[Predict::__PLATELET_PLASMA].Qcinstd < 2.0f && pd->substate[Predict::__MIDRUN].Qch > chlock2) ||
                  (pd->substate[Predict::__PLATELET_PLASMA].Qcinstd < 1.5f && pd->substate[Predict::__MIDRUN].Qch > chlock1)) {
                  sprintf(specificstr,"%.1f %.1f",pd->substate[Predict::__PLATELET_PLASMA].Qcc,pd->substate[Predict::__MIDRUN].Qch);
                  LogError("QCH_MIDRUN4",specificstr);
               }
            }
         }
      }

      //
      //
      //   Volume errors
      //
      if (m_display_concentration && (fabs(pltErr) > ALLOWABLE_PLT_ERR) &&
         ((pd->substate[Predict::__PLATELET_PLASMA].T > 0.0f) || (!epcafull)
          || (fabs(pd->substate[Predict::__MIDRUN].Qcinstd - pd->substate[Predict::__PCA].Qcinstd) > 0.1))) {
         sprintf(specificstr,"%.1f%% in %.1f",pltErr*100.0f,pd->getProcFixedCollectVol());
         if (fabs(pltErr) > 0.2f) {
            LogError("CONCENTRATION_20",specificstr);
         } else if (fabs(pltErr) > 0.1f) {
            LogError("CONCENTRATION_10",specificstr);
         } else {
            LogError("CONCENTRATION",specificstr);
         }
      }

#if 0  // RKM.  Get with Butzke on how to fix this.
      if (m_display_concentration && (fabs(pltErr) > 0.03f) &&
         (pd->getProcTargetCollectVol() > 50.0f) && (!epcafull)) {
         sprintf(specificstr,"%.1f%% in %.1f",pltErr*100.0f,pd->getProcFixedCollectVol());
         LogError("CONCENTRATION_EPCA",specificstr);
      }
#endif

      if (m_display_concentration && (fabs(pltErr) > ALLOWABLE_PLT_ERR) &&
         (pd->hypovolemiaDistance() > hdmax) &&
          (fabs(pd->substate[Predict::__MIDRUN].Qcc - pd->substate[Predict::__PLATELET_PLASMA].Qcc) > 0.1) &&
          (fabs(pd->getProcFixedPlasmaVol() - pd->substate[Predict::__PLATELET_PLASMA].AccumVp) > 1.0) &&
          (pd->substate[Predict::__MIDRUN].T > 0.0f) &&
          (pd->substate[Predict::__PLATELET_PLASMA].T > 0.0f)) {
         sprintf(specificstr,"%.1f%% in %.1f",pltErr*100.0f,pd->getProcFixedCollectVol());
         LogError("CONCENTRATION_SUBSTATE",specificstr);
      }

      if (pd->getProcCollectPlasma() && pd->getProcPlasmaVolFixed() &&
         (fabs(pd->_PredVp - pd->getProcFixedPlasmaVol() - plasmaRinsebackVolume) > 0.5)) {
         sprintf(specificstr,"%.1f %.1f",pd->_PredVp,pd->getProcFixedPlasmaVol());
         LogError("PLASMA_VOL",specificstr);
      }
      
      if (  m_display_vc_errors &&
           //(fabs(pd->getProcFixedCollectVol() - pd->_PredVc) > 10.0f)
           (fabs(pd->getProcFixedCollectVol() - pd->_PredVc) > (0.05 * pd->getProcFixedCollectVol()))
         )   // RKM.  Was 10?
      {
         if (pd->_PredVc > pd->getProcFixedCollectVol())
         {
            sprintf(specificstr,"%.1f (%.1f)",pd->_PredVc,pd->getProcFixedCollectVol()-pd->_PredVc);
            LogError("COLLECT_VOL",specificstr);
         }
         else
         {
            if ( pd->substate[Predict::__PLATELET_PLASMA].T > 0.0f ||
                 (!epcafull)                                       ||
                 (fabs(pd->substate[Predict::__MIDRUN].Qcinstd - pd->substate[Predict::__PCA].Qcinstd) > 0.1)
               )
            {
               sprintf(specificstr,"%.1f (%.1f)",pd->_PredVc,pd->getProcFixedCollectVol()-pd->_PredVc);
               LogError("COLLECT_VOL",specificstr);
            }
         }
      }

      if ((pd->getRecommendedCassette() == cassette::RBCPLS) && 
         (m_current_plasma == -1.0f) &&
         (pd->hypovolemiaDistance() > 50.0f) &&
         (pd->_PredVp < (optimizer().m_predict.getAMAPMaxVolume()-1.0f)) &&
         ((pd->_PredTp + 0.4f)< cfg_main_input::userconfig->Procedure.Get().key_proc_time)) {
         sprintf(specificstr,"%.1f",pd->hypovolemiaDistance());
         LogError("UNDERCOLLECT",specificstr);
      }

      if ( (pd->getRecommendedCassette() == cassette::PLTPLSRBC) && 
           (m_current_plasma == -1.0f) &&
           (pd->hypovolemiaDistance() > 50.0f) &&
           (pd->_PredVp < (optimizer().m_predict.getAMAPMaxVolume() - 1.0f))
         )
      {
         float minpltime = 20.0f / pd->substate[Predict::__EXTENDED_PLASMA].Qpinstd;

         if ( (pd->_PredTp + minpltime) < cfg_main_input::userconfig->Procedure.Get().key_proc_time) 
         {
            sprintf(specificstr,"%.1f",pd->hypovolemiaDistance());
            LogError("UNDERCOLLECT",specificstr);
         }
      }

      //
      //
      //    Substate errors
      //
      if ((pd->getRecommendedCassette() == cassette::PLTPLSRBC) &&
         (pd->substate[Predict::__MIDRUN].EndVin > 25000.0f)) {
         sprintf(specificstr,"%.1f",pd->substate[Predict::__MIDRUN].EndVin);
         LogError("MIDRUN_ENDVIN",specificstr);
      }

      if ((pd->getRecommendedCassette() == cassette::PLTPLSRBC) &&
         (pd->substate[Predict::__PCA].T > 0.0f) &&
         ((pd->substate[Predict::__PIR_NOPLASMA].T+pd->substate[Predict::__PIR_PLASMA].T) > 0.0f)) { // IT5165
         sprintf(specificstr,"%.1f : %.1f + %.1f",pd->substate[Predict::__PCA].T,pd->substate[Predict::__PIR_NOPLASMA].T,pd->substate[Predict::__PIR_PLASMA].T);           
         LogError("PCA+PIR",specificstr);
      }

      if ((pd->getRecommendedCassette() == cassette::PLTPLSRBC) &&
          (pd->substate[Predict::__MIDRUN].T > 0.0f) &&
          (pd->substate[Predict::__PIR_PLASMA].T > 0.0f) && (!epcafull) &&
          (pd->substate[Predict::__PLATELET_PLASMA].HypovolemiaDistance > hdmax)) {
         sprintf(specificstr,"%.1f %.1f",pd->substate[Predict::__MIDRUN].T,pd->substate[Predict::__PIR_PLASMA].T);
         LogError("MIDRUN+PIRPL",specificstr);
      }

      if ((pd->getRecommendedCassette() == cassette::PLTPLSRBC) &&
         (pd->substate[Predict::__PREPLATELET_NOPLASMA].T > 0.0f) &&
         (pd->substate[Predict::__PREPLATELET_PLASMA].T > 0.0f) &&
         (pd->substate[Predict::__PREPLATELET_PLASMA].HypovolemiaDistance > hdmax) &&
         (pd->substate[Predict::__PREPLATELET_PLASMA].AccumVp < (m_current_plasma-Predict::__plasmaTolerance))) {
         sprintf(specificstr,"%.1f : %.1f",pd->substate[Predict::__PREPLATELET_NOPLASMA].T,pd->substate[Predict::__PREPLATELET_PLASMA].T);           
         LogError("PrePlt+PrePltPl",specificstr);
      }

      if ((pd->getRecommendedCassette() == cassette::PLTPLSRBC) &&
         (pd->substate[Predict::__MIDRUN].T > 0.0f) &&
         (pd->substate[Predict::__PLATELET_PLASMA].T != 0.0f) && (!epcafull) &&
         (pd->substate[Predict::__EXTENDED_PLASMA].T > 0.0f) &&
         ((pd->substate[Predict::__PLATELET_PLASMA].HypovolemiaDistance  - pd->substate[Predict::__PLATELET_PLASMA].HypovolemiaMargin)> hdmax)) {
         sprintf(specificstr,"%.1f : %.1f (%.1f - %.1f vs %.1f)",pd->substate[Predict::__MIDRUN].T,pd->substate[Predict::__EXTENDED_PLASMA].T,
            pd->substate[Predict::__PLATELET_PLASMA].HypovolemiaDistance,pd->substate[Predict::__PLATELET_PLASMA].HypovolemiaMargin,pd->substate[Predict::__EXTENDED_PLASMA].Vp);
//         LogError("MIDRUN+EP",specificstr);
      }

      if ((pd->getRecommendedCassette() == cassette::PLTPLSRBC) &&
         (pd->substate[Predict::__MIDRUN].T > 0.0f) &&
         (pd->substate[Predict::__MIDRUN].Qcinstd < pd->substate[Predict::__PCA].Qcinstd) &&
         (pd->substate[Predict::__EXTENDED_PCA].T > 1.0f)) {
         sprintf(specificstr,"%.1f : %.1f",pd->substate[Predict::__MIDRUN].T,pd->substate[Predict::__EXTENDED_PCA].T);
//         LogError("MIDRUN+EPCA",specificstr);
      }

      if ((pd->getRecommendedCassette() == cassette::PLTPLSRBC) &&
         (pd->substate[Predict::__EXTENDED_PLASMA].T > 0.0f) &&
         (pd->substate[Predict::__EXTENDED_PLASMA].T < 0.5f) &&
         (pd->substate[Predict::__MIDRUN].T > 0.0f) &&
         (pd->substate[Predict::__PLATELET_PLASMA].HypovolemiaDistance > hdmax) && (!epcafull) &&
         (!(pd->getProcCollectPlasma() && !pd->getProcPlasmaVolFixed()) || pd->_PredVp > 151.0f)) {
         sprintf(specificstr,"%.1f ",pd->substate[Predict::__EXTENDED_PLASMA].T);
         LogError("SMALL_EP",specificstr);
      }

      if ((pd->getRecommendedCassette() == cassette::PLTPLSRBC) &&
          ((pd->substate[Predict::__EXTENDED_PCA].EndVin -  pd->substate[Predict::__EXTENDED_PCA-1].EndVin)> 251.0f)) {
         sprintf(specificstr,"%.1f ",(pd->substate[Predict::__EXTENDED_PCA].EndVin -  pd->substate[Predict::__EXTENDED_PCA-1].EndVin));
         LogError("LONG_EPCA",specificstr);
      }


#ifdef generate_substate_sequences
      // generate a sequence string
      static CString allsequences;
      static char *statestr[13]={"1","2","3","4","5","6","7","8","9","10","11","12","13"};
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {
         CString sequence = "SEQUENCE";
         for (int i=1; i<pd->getNumOfSubstates(); i++)
         {
            if (pd->substate[i].T > 0.0f)
            {
               sequence += "_";
               sequence += statestr[i-1];
            }
         }

         if (allsequences.Find(sequence) < 0)
         {
            specificstr[0] = ' ';
            specificstr[1] = '\0';
            LogError(sequence, specificstr);
            allsequences += sequence;
         }
      }
#endif

   
   } // end if good product

   if (pd->getStatus() == HYPOVOLEMIA_EXCEEDED) {
      if (m_current_plasma < 0.0f) {
         float extra = pd->_PredVp - optimizer().m_predict.getAMAPVolume() - plasmaRinsebackVolume;
         if (pd->hypovolemiaDistance()+extra > hdmax) { // control + AMAP + margin
            sprintf(specificstr,"%.1f",pd->_PredVp);
            LogError("HYPOVOLEMIA",specificstr);
         }
      }
   }

   if (pd->getStatus() == TIME_EXCEEDED)
   {
      int plstate = (pd->getRecommendedCassette() == cassette::RBCPLS?Predict::__RP_PLASMA_ONLY:Predict::__EXTENDED_PLASMA);

      if ((m_current_plasma < 0.0f) && (pd->substate[plstate].T > 0.0f))
      {
         if (pd->_PredVp > (optimizer().m_predict.getAMAPVolume()+plasmaRinsebackVolume+Predict::__plasmaTolerance))
         {
            if ((pd->_PredTp - pd->substate[plstate].T) < cfg_main_input::userconfig->Procedure.Get().key_proc_time)
            {
               sprintf(specificstr,"%.1f %.1f", pd->_PredVp, pd->_PredTp);
               LogError("OVERCOLLECT_TIME",specificstr);
            }
         }
      }
   }

   if (pd->getStatus() == UNOPTIMIZED)
   {
      m_unconverged++;

      if (m_current_conc <= 2001.0f)
      {
         m_unconverged_lt_2000++;
      }
      else if (m_current_conc <= 4001.0f)
      {
         m_unconverged_lt_4000++;
      }
      else
      {
         m_unconverged_gt_4000++;
      }

      if (m_display_unconverged)
      {
         sprintf(specificstr,"%d", pd->getconvergenceCtr());
         LogError("UNCONVERGED",specificstr);
      }
   }

   if (m_good > 0) 
      m_percent_good = 100.0f * (float)m_good / ((float)(m_good+m_unconverged));

   if (m_good_lt_2000 > 0) m_percent_lt_2000 = 100.0f * (float)m_good_lt_2000 / ((float)(m_good_lt_2000+m_unconverged_lt_2000));
   if (m_good_lt_4000 > 0) m_percent_lt_4000 = 100.0f * (float)m_good_lt_4000 / ((float)(m_good_lt_4000+m_unconverged_lt_4000));
   if (m_good_gt_4000 > 0) m_percent_gt_4000 = 100.0f * (float)m_good_gt_4000 / ((float)(m_good_gt_4000+m_unconverged_gt_4000));

   if (m_good > 0) m_inconc_percent_good = 100.0f * (float)m_inconc / (float)m_good;
   if (m_good_lt_2000 > 0) m_inconc_percent_lt_2000 = 100.0f * (float)m_inconc_lt_2000 / (float)m_good_lt_2000;
   if (m_good_lt_4000 > 0) m_inconc_percent_lt_4000 = 100.0f * (float)m_inconc_lt_4000 / (float)m_good_lt_4000;
   if (m_good_gt_4000 > 0) m_inconc_percent_gt_4000 = 100.0f * (float)m_inconc_gt_4000 / (float)m_good_gt_4000;

   //
   //
   //   Check for effect of PIR bracket "lockin"
   //
   if (pd->getStatus() == GOOD_PRODUCT) {
      if (m_display_lockin) CheckLockin(pd,1);
      if (m_repredict) Repredict(pd,1);
   }

}

void robustness::LogError(const char *headerstr,const char *specificstr)
{
   static const float cqrp[6] = {0.50f,0.57f,0.66f,0.76f,0.87f,1.00f};
   static const float cqin[6] = {0.66f,0.72f,0.78f,0.85f,0.92f,1.00f};
   static const float cref[6] = {0.90f,0.95f,0.99f,1.01f,1.05f,1.10f};
   static char resultstr[200];

   if (specificstr[0] != '\0') {
      sprintf(resultstr,"%s donor= %d %d %d  product= %3.1f @ %d = %5.1f %4d %3d @ %d config= %.1f %d (%.2f %.2f) %d %d (%.2f) %d (%.2f) %d %d %d %d %.1f adjust= %.1f %.1f %.1f %.1f # %s",
          headerstr,
          (int)m_current_tbv,(int)m_current_precount,(int)m_current_hct,
          m_current_yield,(int)m_current_conc,(m_current_yield*100000.0f/m_current_conc),(int)m_current_plasma,
          (int)m_current_rbcdose,(int)m_current_rbccrit,
          m_current_ratio, m_current_infusion, cref[m_current_infusion-1], m_maxInfusion,
          m_current_draw_flow, m_current_draw_management, cqin[m_current_draw_management-1],
          m_current_return_management, cqrp[m_current_return_management-1], m_current_inlet_ramp,
          m_current_replacement,m_current_replacement_percent,m_initial_conditions.m_ptf,m_initial_conditions.m_ptf_qrbc,
          m_initial_conditions.m_qin_adjustment,m_initial_conditions.m_qrp_adjustment,
          m_initial_conditions.m_ir_adjustment,m_initial_conditions.m_ratio_adjustment,
          specificstr);
       m_results_control.AddString(resultstr);
   }
}

//
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
void robustness::OnDblclkResults() 
{
   int selected = m_results_control.GetCurSel();
   if (selected != LB_ERR) {
      

      CString proctext;
      m_results_control.GetText(selected,proctext.GetBuffer(m_results_control.GetTextLen(selected)));
      proctext.ReleaseBuffer();
      static char tmpbuffer[20]; // big enough to hold unused fields
      sscanf(proctext,"%s %s %f %f %f %s %f %s %f %s %s %f %f %s %f %s %f %d %s %s %d %d %s %d %s %d %d %d %d %f %s %f %f %f %f",
             tmpbuffer,tmpbuffer,
             &m_current_tbv,&m_current_precount,&m_current_hct,
             tmpbuffer, &m_current_yield,tmpbuffer, &m_current_conc, tmpbuffer, tmpbuffer,
             &m_current_plasma,&m_current_rbcdose,tmpbuffer,&m_current_rbccrit, tmpbuffer,
             &m_current_ratio,&m_current_infusion,tmpbuffer,tmpbuffer,
             &m_current_draw_flow,&m_current_draw_management,tmpbuffer,
             &m_current_return_management,tmpbuffer,&m_current_inlet_ramp,&m_current_replacement,&m_current_replacement_percent,
             &m_initial_conditions.m_ptf,&m_initial_conditions.m_ptf_qrbc,tmpbuffer,&m_initial_conditions.m_qin_adjustment,&m_initial_conditions.m_qrp_adjustment,
             &m_initial_conditions.m_ir_adjustment,&m_initial_conditions.m_ratio_adjustment);
	  //cleanup.
   optimizer().m_predict.donor_rejected();
      optimizer().donor(__donorheight,__donorweight,
         m_current_tbv,
         m_current_hct/100.0f,
         m_current_hct/100.0f,
         m_current_precount,
         BT_UNKNOWN); 
      
      
      float pltvol = m_current_yield*100000.0f/m_current_conc;
      optimizer().m_predict.create_procedure(m_current_yield,
         pltvol,
         m_current_plasma,
         m_current_rbcdose,
         m_current_rbccrit,
         1.0f, // RSF
         cfg_main_input::userconfig->Procedure.Get().key_proc_time, // procedure time
         BT_ALL,
         1, // designator
         false, 0, 0,    // Metered Platelets
         false, 0,     // Metered Redcells
         1); // procedure number

   ProcedureConfig proccfg = cfg_main_input::userconfig->Procedure.Get();
   PredictionConfig predictcfg = cfg_main_input::userconfig->Predict.Get();
   proccfg.key_custom_ratio = m_current_ratio;
   proccfg.key_ac_rate = m_current_infusion;
   proccfg.key_max_draw_flow = m_current_draw_flow;
   predictcfg.key_inlet_management = m_current_draw_management;
   predictcfg.key_return_management = m_current_return_management;
   predictcfg.key_inlet_flow_ramp = m_current_inlet_ramp;  
   predictcfg.key_rbc_replace_fluid = m_current_replacement;
   predictcfg.key_rbc_fluid_percent = m_current_replacement_percent;
   cfg_main_input::userconfig->Procedure.Set(proccfg);
   cfg_main_input::userconfig->Predict.Set(predictcfg);
      
      optimizer().m_predict.FillInConfig();
      optimizer().setInitialConditions(m_initial_conditions);

      optimizer().m_predict._selectedProc = 0;
      optimizer().m_predict.SetUnoptimized();
      procedureDetail *pd = optimizer().m_predict.getProcedure(1);
	  //Reset Lockin since it is not initilaized within.
	  pd->ResetProcDet();
      optimizer().m_predict.predictProcedure(*pd);
      
      CString workstring;
      Offline_optimizer::description(pd->getStatus(),workstring);
            
      m_popup->initialize(1,
         workstring,
         m_current_yield,
         pltvol,
         m_current_plasma,
         m_current_rbcdose,
         m_current_rbccrit,
         cfg_main_input::userconfig->Predict.Get().key_rsf,
         BT_ALL,
         BT_ALL,
         cfg_main_input::userconfig->Procedure.Get().key_proc_time,
         cfg_main_input::userconfig->Predict.Get().key_rbc_fluid_percent,
         *pd,
         &(m_optimizer.m_predict));

      m_popup->ShowWindow(SW_SHOW);
      if (m_repredict) Repredict(pd,0);
      if (m_display_lockin) CheckLockin(pd,0);
   }
	
}

void robustness::OnEditConfig() 
{
  optimizer_form::OnEditConfig();
}

void robustness::OnInitialConditions() 
{
   optimizer_form::OnInitialConditions();	
}

//
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
void robustness::Repredict(procedureDetail *pd, const int showresult)
{
   if (!showresult) m_results_control.ResetContent();

   //
   //
   //    Save off current "initial conditions" data
   //
   float _collect_bag_ac     = m_initial_conditions.m_collect_bag_ac;
   float _collect_bag_volume = m_initial_conditions.m_collect_bag_volume;
   float _plasma_bag_ac      = m_initial_conditions.m_plasma_bag_ac;
   float _plasma_bag_volume  = m_initial_conditions.m_plasma_bag_volume;
   float _procedure_run_time = m_initial_conditions.m_procedure_run_time;
   float _rbc_bag_ac         = m_initial_conditions.m_rbc_bag_ac;
   float _rbc_bag_volume     = m_initial_conditions.m_rbc_bag_volume;
   float _vac                = m_initial_conditions.m_vac;
   float _vin                = m_initial_conditions.m_vin;
   float _vrep               = m_initial_conditions.m_vrep;
   float _yield              = m_initial_conditions.m_yield;
   float _rbc_bag_dose       = m_initial_conditions.m_rbc_bag_dose;
   CString _substate         = m_initial_conditions.m_substate;
   float _tcv                = m_initial_conditions.m_tcv;
   float _vincv              = m_initial_conditions.m_vincv;
   int   _cassette           = m_initial_conditions.m_cassette;
   float _minimum_qch        = m_initial_conditions.m_minimum_qch;
   float _current_qch        = m_initial_conditions.m_current_qch;
   int _purgesdone           = m_initial_conditions.m_purges;
   int _epcastart            = m_initial_conditions.m_epca_start;
   optimizer().m_predict._selectedProc = pd;

   //
   //
   //    Start at end of priming sequence
   //
   m_initial_conditions.m_collect_bag_ac     = 0.0f;
   m_initial_conditions.m_collect_bag_volume = 0.0f;
   m_initial_conditions.m_plasma_bag_ac      = 0.0f;
   m_initial_conditions.m_plasma_bag_volume  = 0.0f;
   m_initial_conditions.m_procedure_run_time = pd->getPrimeDuration();
   m_initial_conditions.m_rbc_bag_ac         = 0.0f;
   m_initial_conditions.m_rbc_bag_volume     = 0.0f;
   m_initial_conditions.m_vac                = pd->getVinPrime()/m_current_ratio+18.0f;
   m_initial_conditions.m_vin                = pd->getVinPrime();
   m_initial_conditions.m_vrep               = 0.0f;
   m_initial_conditions.m_yield              = 0.0f;
   m_initial_conditions.m_rbc_bag_dose       = 0.0f;
   m_initial_conditions.m_substate           = "ChannelSetup";
   int substateIndex = 1;
   int nextSubstate = 1;
   m_initial_conditions.m_tcv                = 0.0f;
   m_initial_conditions.m_vincv              = 0.0f;
   m_initial_conditions.m_cassette           = pd->getRecommendedCassette();
   m_initial_conditions.m_minimum_qch        = 5.0f;
   m_initial_conditions.m_current_qch        = 5.0f;
   m_initial_conditions.m_purges             = 0;
   m_initial_conditions.m_epca_start         = 0.0f;
   float maxqch                              = 5.0f;

   float initvc = pd->_PredVc;
   float initvp = pd->_PredVp;
   float inittime = pd->_PredTp;
   float initinfusion = optimizer().m_predict.getConfigMaxACRate();

   int displayedTime = 0;
   int onlyOnePurgeinPIR = 0;
   Predict_CDS *predictcds = new Predict_CDS(ROLE_RW);
   predictcds->accumulatedYdpur.Set(0.0f);
   predictcds->accumulatedPurges.Set(0);
   predictcds->PCAEntered.Set(0);
   delete predictcds;
   predictcds = new Predict_CDS(ROLE_RO);

   //
   //
   //   Step through procedure, repredicting at interval
   //
   while (m_initial_conditions.m_vin < pd->substate[pd->getNumOfSubstates()-1].EndVin) {
      optimizer().setInitialConditions(m_initial_conditions);
      pd->setOptimized(0);
      pd->setStatus(UNOPTIMIZED);
	  pd->ResetProcDet();
      optimizer().m_predict.predictProcedure(*pd);

      if (!showresult) {
         float PIRTime = pd->substate[Predict::__PIR_PLASMA].T + pd->substate[Predict::__PIR_NOPLASMA].T;
         float vcatmax = 0.0f;
         if (PIRTime > 0.0f) {
            float ramptime = (pd->substate[Predict::__PIR_NOPLASMA].Qch - pd->substate[Predict::__MIDRUN].Qch)/CobeConfig::data().CollectRampRate;
            vcatmax = pd->substate[Predict::__PIR_NOPLASMA].Qch*(PIRTime - ramptime);
         }

         static char resultstr[400];
         //                 runtm  subs vin   vac   vc    vacc   vp   vacp    Tp  PredVc  Vp     2.T  3.T 4.T=4.Acvp 5.T  6.T  7.T  8.T  9.T  10.T   Qcmr  Qinc  Ydch  Ydpur(rem)+AcYdpur(pur)pirvc Vintarg #times
         sprintf(resultstr,"%05.1f %2d %6.0f %6.1f %6.1f-%6.1f %6.1f-%6.1f   %5.1f %6.1f %6.1f   %.0f %.0f %.0f=%.1f %.0f %.0f %.0f %.0f %.0f %.0f   %5.2f %5.1f %5.3f  %5.3f(%.4f)+%5.3f(%d) %.1f  %5.0f %2d",
                 m_initial_conditions.m_procedure_run_time,substateIndex,m_initial_conditions.m_vin,m_initial_conditions.m_vac,
                 m_initial_conditions.m_collect_bag_volume,m_initial_conditions.m_collect_bag_ac,
                 m_initial_conditions.m_plasma_bag_volume,m_initial_conditions.m_plasma_bag_ac,
                 pd->_PredTp,pd->_PredVc,pd->_PredVp,
                 pd->substate[2].EndVin,pd->substate[3].EndVin,pd->substate[4].EndVin,pd->substate[4].AccumVp,pd->substate[5].EndVin,
                 pd->substate[6].EndVin,pd->substate[7].EndVin,pd->substate[8].EndVin,pd->substate[9].EndVin,pd->substate[10].EndVin,
                 pd->toplevel.Qcmr,pd->toplevel.Qinc,pd->toplevel.Ydch,pd->toplevel.Ydpur,pd->totalPurges(),predictcds->accumulatedYdpur.Get(),predictcds->accumulatedPurges.Get(),
                 vcatmax,pd->toplevel.Vintarg,pd->getnumOfTimes()
                );
         m_results_control.AddString(resultstr);
      }

      //
      //
      //   Ensure that important parameters have not changed due to the passage
      //   of times and volumes
      //
      static char specificstr[200];
      if (fabs(initvc - pd->_PredVc) > 10.0f) {
         sprintf(specificstr,"%.1f %.1f (%.1f - %.1f = %.1f)",m_initial_conditions.m_procedure_run_time,m_initial_conditions.m_vin,initvc, pd->_PredVc, (initvc - pd->_PredVc));
         if (showresult) LogError("REPREDICT_VC",specificstr);
         break;
      }

      if (fabs(initvp - pd->_PredVp) > 20.0f) {
         sprintf(specificstr,"%.1f %.1f (%.1f - %.1f = %.1f)",m_initial_conditions.m_procedure_run_time,m_initial_conditions.m_vin, initvp, pd->_PredVp,(initvp - pd->_PredVp));
         if (showresult) LogError("REPREDICT_VP",specificstr);
         break;
      }

      if (fabs(inittime - pd->_PredTp) > 2.0f) {
         sprintf(specificstr,"%.1f %.1f, %.1f - %.1f = %.1f",m_initial_conditions.m_procedure_run_time,m_initial_conditions.m_vin,inittime,pd->_PredTp,(inittime - pd->_PredTp));
         if (!displayedTime) {
            displayedTime = 1;
            if (showresult) {
               LogError("REPREDICT_TIME",specificstr);
            } else {
               break;
            }
         }
      }

      if (pd->getStatus() != GOOD_PRODUCT && pd->getStatus() != HYPOVOLEMIA_EXCEEDED) {
         sprintf(specificstr,"%.1f %.1f %d",m_initial_conditions.m_procedure_run_time,m_initial_conditions.m_vin,pd->getStatus());
         if (showresult) LogError("REPREDICT_DISQ",specificstr);
         break;
      }

      float remaining = optimizer().m_predict._hypovolemia.VolumeRemainingToHypovolemia(1,
                           0.0f, m_initial_conditions.m_collect_bag_volume, m_initial_conditions.m_collect_bag_ac,
                           m_initial_conditions.m_plasma_bag_volume, m_initial_conditions.m_plasma_bag_ac,
                           m_initial_conditions.m_rbc_bag_volume, m_initial_conditions.m_rbc_bag_ac,
                           m_initial_conditions.m_vrep, m_initial_conditions.m_vac);
      if (remaining < CobeConfig::data().HypovolemiaAdjust) {
         sprintf(specificstr,"%.1f %.1f %.1f",m_initial_conditions.m_procedure_run_time,m_initial_conditions.m_vin,remaining);
         if (showresult) LogError("REPREDICT_HYPO",specificstr);
         break;
      }


      //
      //
      //   Bump to next reprediction interval if needed
      //
      if ((m_initial_conditions.m_vin+0.5f) >= pd->substate[substateIndex].EndVin) {
         nextSubstate = substateIndex + 1;
         while (pd->substate[nextSubstate].EndVin <= (m_initial_conditions.m_vin+0.5f) && (nextSubstate < pd->getNumOfSubstates())) ++nextSubstate;
         if (nextSubstate >= pd->getNumOfSubstates()) break;
         substateIndex = nextSubstate;
         m_initial_conditions.m_substate = SubstateMap::ssStr(SubstateMap::ss(substateIndex));
         if (substateIndex == Predict::__EXTENDED_PCA && m_initial_conditions.m_epca_start == 0.0f) {
            m_initial_conditions.m_epca_start = m_initial_conditions.m_vin;
         }
      }
      
      float dcfactor = (1.0f-CobeConfig::data().RecirculateSnDrawVol/pd->substate[substateIndex].Vsvn);
      float qin = pd->substate[substateIndex].Qininstd*dcfactor*pd->substate[substateIndex].D;
      if (substateIndex == 1) qin = pd->substate[substateIndex].Qininstd;

      float dt = pd->substate[substateIndex].Tcn;
      float dvin = qin * dt;

      if ((m_initial_conditions.m_vin+dvin) > pd->substate[substateIndex].EndVin) {
         nextSubstate = substateIndex + 1;
         while (pd->substate[nextSubstate].EndVin <= (m_initial_conditions.m_vin+0.5f) && (nextSubstate < pd->getNumOfSubstates())) ++nextSubstate;
         if (nextSubstate >= pd->getNumOfSubstates()) break;
         dvin = (pd->substate[substateIndex].EndVin - m_initial_conditions.m_vin);
         dt = dvin/qin;
         m_initial_conditions.m_vin = pd->substate[substateIndex].EndVin;
      } else {
         m_initial_conditions.m_vin += dvin;
      }
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {
         m_initial_conditions.m_yield  = m_current_yield*1.0e11f*(m_initial_conditions.m_vin/pd->substate[Predict::__PIR_NOPLASMA].EndVin);
      } else {
         m_initial_conditions.m_yield  = 0.0f;
      }



      float pdqcc = 0.0f;
      if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {
         if ((substateIndex >= Predict::__PLATELET_PLASMA) &&
            (substateIndex <= Predict::__EXTENDED_PCA)) {
            pdqcc = pd->substate[substateIndex].Qcinstd;
         }
      }
      float pdqpc = 0.0f;
      if (pd->substate[substateIndex].Vp > 0.0f) {
         pdqpc = pd->substate[substateIndex].Qpinstd;
         if (pd->substate[substateIndex].Qininstr == 0.0f) { // no recirc
            pdqpc *= pd->substate[substateIndex].D;
         }
      }

      float qrep = 0.0f;
      if (pd->substate[substateIndex].Vrep > 0.0f) {
         qrep = pd->substate[substateIndex].Qcinstd*pd->substate[substateIndex].D;
      }

      float pdqrbc = 0.0f;
      if (pd->substate[substateIndex].Vr > 0.0f) {
         pdqrbc = (pd->substate[substateIndex].Qininstd - pd->substate[substateIndex].Qpinstd)*pd->substate[substateIndex].D;
      }
      //
      //
      //  Mimic "stop substate at plasma target" logic in proc
      //
      float collectedPlasma = m_initial_conditions.m_plasma_bag_volume + pdqpc * dt;
      if (collectedPlasma > pd->_PredVp && pdqpc > 0.0f) {
         float deltadt = dt - (pd->_PredVp - m_initial_conditions.m_plasma_bag_volume)/pdqpc;
         if (deltadt > 0.01f) {
            dt -= deltadt;
            dvin -= deltadt*qin;
            m_initial_conditions.m_vin -= deltadt*qin;
         }
      }



      if ((substateIndex == Predict::__PIR_PLASMA) || (substateIndex == Predict::__PIR_NOPLASMA)) {
         pdqcc = m_initial_conditions.m_current_qch;
      }

      if (m_initial_conditions.m_tcv == 0.0f && pdqcc > 0.0f) {
         m_initial_conditions.m_tcv = m_initial_conditions.m_procedure_run_time - dt;
         m_initial_conditions.m_vincv = m_initial_conditions.m_vin - dvin;
      }
      m_initial_conditions.m_procedure_run_time += dt;

      static const float PURGETIME = 3.0f;
      float firstpurge,deltapurge;
      pd->getPurgeData(&optimizer().m_predict,firstpurge,deltapurge);

      if (firstpurge > 0.0f) {
         float purgepoint = firstpurge + deltapurge*m_initial_conditions.m_purges;
         if (substateIndex <= Predict::__MIDRUN) {
            if (m_initial_conditions.m_vin > purgepoint) {
               m_initial_conditions.m_procedure_run_time += PURGETIME;
               ++m_initial_conditions.m_purges;
               purgepoint = 0.0f;
            }
         }   
         //
         //
         //   If we are just entering PIR vest the last purge
         //
//         if ((purgepoint > 0.0f) && (substateIndex == Predict::__MIDRUN) && 
//            (nextSubstate == Predict::__PIR_NOPLASMA || nextSubstate == Predict::__PIR_PLASMA)) {
         if ((purgepoint > 0.0f) && (onlyOnePurgeinPIR == 0) &&
            (substateIndex == Predict::__PIR_NOPLASMA || substateIndex == Predict::__PIR_PLASMA)) {
            if (purgepoint < pd->substate[Predict::__PIR_NOPLASMA].EndVin) {
               m_initial_conditions.m_procedure_run_time += PURGETIME;
               ++m_initial_conditions.m_purges;
               onlyOnePurgeinPIR = 1;
            }
         }
      }

      float AdjustedHct = m_current_hct/100.0f;
      if (m_initial_conditions.m_cassette != cassette::PLTPLSRBC) 
      {
       // Current TBV with set fluid removed
       float fV = m_current_tbv - CobeConfig::data().RBCPlsExtracorporealVolume;
          
       // adjusted hct = (initial Vrbcs - rbcs removed to bag ) / current TBV
       AdjustedHct = ( ( fV * m_current_hct/100.0f - m_initial_conditions.m_rbc_bag_dose ) 
                       / ( fV - m_initial_conditions.m_plasma_bag_volume
                              - m_initial_conditions.m_rbc_bag_volume
                              - m_initial_conditions.m_collect_bag_volume
                              + m_initial_conditions.m_vrep
                              + 0.0f // saline bolus
                              + m_initial_conditions.m_vac )
                     );
         optimizer().donor(__donorheight,__donorweight, // weight
         m_current_tbv,
         m_current_hct/100.0f,
         AdjustedHct,
         m_current_precount,
         BT_UNKNOWN); 

      }
	  //Accumulate volumes?
	  optimizer().m_predict.AccumulateVolumes(*pd);

      float ratio = (pd->substate[substateIndex].Vr > 0.0f) ? (cfg_main_input::userconfig->Predict.Get().key_rbc_ratio == 0 ? 8.14f : 11.0f) : m_current_ratio;
      float facb = (1.0f/((ratio-1.0f)*(1.0f-AdjustedHct) + 1.0f));
      m_initial_conditions.m_collect_bag_ac     += pdqcc * dt * facb;
      m_initial_conditions.m_collect_bag_volume += pdqcc * dt;
      m_initial_conditions.m_plasma_bag_ac      += pdqpc * dt * facb;
      m_initial_conditions.m_plasma_bag_volume  += pdqpc * dt;
      m_initial_conditions.m_rbc_bag_ac         += pdqrbc * dt * facb * (1.0f-pd->getPredRBCCrit()/100.0f);
      m_initial_conditions.m_rbc_bag_volume     += pdqrbc * dt;
      m_initial_conditions.m_vac                += dvin/ratio;
      m_initial_conditions.m_vrep               += qrep * dt;
      m_initial_conditions.m_rbc_bag_dose       += pdqrbc * dt * (pd->getPredRBCCrit()/100.0f);

      //
      //
      //  Mimic "plasma run to target" of proc
      //
      if (fabs(m_initial_conditions.m_plasma_bag_volume - pd->getProcFixedPlasmaVol()) < 2.0) {
         m_initial_conditions.m_plasma_bag_volume = pd->getProcFixedPlasmaVol();
      }

      if (substateIndex > Predict::__PREPLATELET_NOPLASMA && substateIndex < Predict::__EXTENDED_PCA) {
         if (m_initial_conditions.m_minimum_qch > pd->substate[substateIndex].Qch) m_initial_conditions.m_minimum_qch = pd->substate[substateIndex].Qch;
         if (pd->substate[substateIndex].Qch < maxqch) maxqch = pd->substate[substateIndex].Qch;
         maxqch += 0.20f*dt;
         if (maxqch > pd->substate[substateIndex].Qch) maxqch = pd->substate[substateIndex].Qch;
         m_initial_conditions.m_current_qch = maxqch;
      } else {
         m_initial_conditions.m_current_qch = maxqch = pd->substate[substateIndex].Qch;
      }

   }

   //
   //
   //   Reset initial conditions to default
   //
   m_initial_conditions.m_collect_bag_ac     = _collect_bag_ac;
   m_initial_conditions.m_collect_bag_volume = _collect_bag_volume;
   m_initial_conditions.m_plasma_bag_ac      = _plasma_bag_ac;
   m_initial_conditions.m_plasma_bag_volume  = _plasma_bag_volume;
   m_initial_conditions.m_procedure_run_time = _procedure_run_time;
   m_initial_conditions.m_rbc_bag_ac         = _rbc_bag_ac;
   m_initial_conditions.m_rbc_bag_volume     = _rbc_bag_volume;
   m_initial_conditions.m_vac                = _vac;
   m_initial_conditions.m_vin                = _vin;
   m_initial_conditions.m_vrep               = _vrep;
   m_initial_conditions.m_yield              = _yield;
   m_initial_conditions.m_rbc_bag_dose       = _rbc_bag_dose;
   m_initial_conditions.m_substate           = _substate;
   m_initial_conditions.m_tcv                = _tcv;
   m_initial_conditions.m_vincv              = _vincv;
   m_initial_conditions.m_cassette           = _cassette;
   m_initial_conditions.m_minimum_qch        = _minimum_qch;
   m_initial_conditions.m_current_qch        = _current_qch;
   m_initial_conditions.m_purges             = _purgesdone;
   m_initial_conditions.m_epca_start         = _epcastart;
   optimizer().m_predict._selectedProc = 0;
   optimizer().setInitialConditions(m_initial_conditions);
   delete predictcds;
   predictcds = new Predict_CDS(ROLE_RW);
   predictcds->accumulatedYdpur.Set(0.0f);
   predictcds->accumulatedPurges.Set(0);
   predictcds->PCAEntered.Set(0);
   delete predictcds;

   if (!showresult) {
      CString workstring;
      Offline_optimizer::description(pd->getStatus(),workstring);
      float pltvol = m_current_yield*100000.0f/m_current_conc;

      m_popup_repredict->initialize(1,
         workstring,
         m_current_yield,
         pltvol,
         m_current_plasma,
         m_current_rbcdose,
         m_current_rbccrit,
         cfg_main_input::userconfig->Predict.Get().key_rsf,
         BT_ALL,
         BT_ALL,
         cfg_main_input::userconfig->Procedure.Get().key_proc_time,
         cfg_main_input::userconfig->Predict.Get().key_rbc_fluid_percent,
         *pd,
         &(m_optimizer.m_predict));

      m_popup_repredict->ShowWindow(SW_SHOW);
   }

}

//
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
void robustness::CheckLockin(procedureDetail *pd, const int showresult)
{
   static char specificstr[200];
   specificstr[0] = '\0';
   if (pd->getRecommendedCassette() == cassette::PLTPLSRBC) {
      float lockin = 5.0f;
      for (int subndx=3; subndx < pd->getNumOfSubstates(); subndx++) {
         if ((pd->substate[subndx].T > 0.0f) && 
            (subndx > 1) && (subndx < 7)) {
            if (lockin > 3.8f && pd->substate[subndx].Qch < 2.0f) lockin = 3.8f;
            if (lockin > 2.5f && pd->substate[subndx].Qch < 1.5f) lockin = 2.5f;
         }
      }
      if (lockin < 5.0f) {
         
         float vc = pd->_PredVc;
         float vp = pd->_PredVp;
         float pirtime = pd->substate[Predict::__PIR_NOPLASMA].T + pd->substate[Predict::__PIR_PLASMA].T;
         float proctime = pd->_PredTp;
         float purgetime = pd->toplevel.Vinpur;
         optimizer().m_predict._selectedProc = 0;
         optimizer().m_predict.SetUnoptimized();
		 pd->ResetProcDet();
         //optimizer().m_predict.setPIRQchMax(lockin);
         optimizer().m_predict.predictProcedure(*pd);
         //optimizer().m_predict.setPIRQchMax(5.0f);
         if (pd->getStatus() != GOOD_PRODUCT) {
            sprintf(specificstr,"%d %d",
               (int)pd->getStatus(),
               pd->getconvergenceCtr());
            if (showresult && m_display_lockin) LogError("LOCKIN_DISQ",specificstr);
            ++m_lockin_errors;
         }
         if (fabs(pd->_PredVc - vc) > 10.0) {
            sprintf(specificstr,"%.1f %.1f",vc,pd->_PredVc);
            if (showresult && m_display_lockin) LogError("LOCKIN_VC",specificstr);
            ++m_lockin_errors;
         }
         if (fabs(pd->_PredVp - vp) > 10.0) {
            sprintf(specificstr,"%.1f %.1f %.1f (%.1f)",lockin,vp,pd->_PredVp,(vp-pd->_PredVp));
            if (showresult && m_display_lockin) LogError("LOCKIN_VP",specificstr);
            ++m_lockin_errors;
         }
// time in PIR check removed since it is invisible to customer
//         if (fabs(pd->substate[Predict::__PIR_NOPLASMA].T + pd->substate[Predict::__PIR_PLASMA].T - pirtime) > 1.0) {
//            sprintf(specificstr,"%.1f %.1f",pirtime,(pd->substate[Predict::__PIR_NOPLASMA].T + pd->substate[Predict::__PIR_PLASMA].T));
//            if (showresult && m_display_lockin) LogError("LOCKIN_PIR_T",specificstr);
//            ++m_lockin_errors;
//         }
         if (fabs(pd->_PredTp - proctime) > 1.0) {
            sprintf(specificstr,"%.1f %.1f",proctime,pd->_PredTp);
            if (showresult && m_display_lockin) LogError("LOCKIN_TIME",specificstr);
            ++m_lockin_errors;
         }
         if (fabs(pd->toplevel.Vinpur - purgetime) > 100.0) {
            sprintf(specificstr,"%.1f %.1f",purgetime,pd->toplevel.Vinpur);
            if (showresult && m_display_lockin) LogError("LOCKIN_PURGE",specificstr);
            ++m_lockin_errors;
         }
         if (!showresult) {
            CString workstring;
            Offline_optimizer::description(pd->getStatus(),workstring);
            float pltvol = m_current_yield*100000.0f/m_current_conc;
            
            m_popup_repredict->initialize(1,
               workstring,
               m_current_yield,
               pltvol,
               m_current_plasma,
               m_current_rbcdose,
               m_current_rbccrit,
               cfg_main_input::userconfig->Predict.Get().key_rsf,
               BT_ALL,
               BT_ALL,
               cfg_main_input::userconfig->Procedure.Get().key_proc_time,
               cfg_main_input::userconfig->Predict.Get().key_rbc_fluid_percent,
               *pd,
               &(m_optimizer.m_predict));
            
            m_popup_repredict->ShowWindow(SW_SHOW);
         }
         
         
      }
   }
}
//
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
#define updaterow(var) \
   UpdateData(1); \
   m_##var##_tests = 0; \
   if ((m_##var##_start <= m_##var##_stop) && (m_##var##_delta > 0.0f)) { \
      m_##var##_tests = (m_##var##_stop - m_##var##_start)/m_##var##_delta + 1; \
   } \
   UpdateData(0);

void robustness::OnHct() 
{
   updaterow(hct);
}

void robustness::OnTBV() 
{
   updaterow(tbv);
}

void robustness::OnPrecount() 
{
   updaterow(precount);
}

void robustness::OnConc() 
{
   updaterow(conc);
}

void robustness::OnPlasma() 
{
   updaterow(plasma);
   if (m_amap) ++m_plasma_tests;
   if (m_max_concurrent) ++m_plasma_tests;
}

void robustness::OnRBCCrit() 
{
   updaterow(rbccrit);
}

void robustness::OnRBCDose() 
{
   updaterow(rbcdose);
}

void robustness::OnYield() 
{
   updaterow(yield);
}

void robustness::OnDrawFlow() 
{
   updaterow(draw_flow);
}

void robustness::OnDrawManagement() 
{
   updaterow(draw_management);
}

void robustness::OnInfusion() 
{
   updaterow(infusion);
}

void robustness::OnInletRamp() 
{
   updaterow(inlet_ramp);
}

void robustness::OnRatio() 
{
   updaterow(ratio);
}

void robustness::OnReturnManagement() 
{
   updaterow(return_management);
}

void robustness::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	optimizer_form::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
}

void robustness::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	optimizer_form::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

//
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
void robustness::OnSaveOutput() 
{
   UpdateData(1);
   fstream outputstream(m_listbox_save_path,ios::in|ios::out|ios::trunc);
   if (outputstream.fail()) {
      MessageBox("Unable to open output file","File Save Error",MB_ICONSTOP);
      return;
   }

   CString proctext;
   for (int i=0;i < m_results_control.GetCount();i++)
   {
      int n = m_results_control.GetTextLen( i );
      m_results_control.GetText( i, proctext.GetBuffer(n) );
      proctext.ReleaseBuffer();
      outputstream << proctext << endl;
   }
   outputstream.close();
}

void robustness::OnRestoreOutput() 
{
   static const int MAXBUFFERLENGTH = 1024;
   static char buffer[MAXBUFFERLENGTH];

   m_results_control.ResetContent();
   UpdateData(1);
   fstream outputstream(m_listbox_save_path,ios::in);
   if (outputstream.fail()) {
      MessageBox("Unable to open input file","File Load Error",MB_ICONSTOP);
      return;
   }

   CString proctext;
   while (!outputstream.eof()) {
      outputstream.getline(buffer,MAXBUFFERLENGTH); // istream call
      m_results_control.AddString(buffer);
   }
   outputstream.close();
}

void robustness::OnUseNormal() 
{
   UpdateData(1);
   if (m_sequential) {
      MessageBox("Normal distribution and sequential are mutually exclusive","Input Error",MB_ICONSTOP);
      m_use_normal = 0;
      UpdateData(0);
      return;
   }
   m_hct_delta_ctl.EnableWindow(!m_use_normal);
   m_hct_start_ctl.EnableWindow(!m_use_normal);
   m_hct_stop_ctl.EnableWindow(!m_use_normal);
   m_precount_delta_ctl.EnableWindow(!m_use_normal);
   m_precount_start_ctl.EnableWindow(!m_use_normal);
   m_precount_stop_ctl.EnableWindow(!m_use_normal);
   m_tbv_delta_ctl.EnableWindow(!m_use_normal);
   m_tbv_start_ctl.EnableWindow(!m_use_normal);
   m_tbv_stop_ctl.EnableWindow(!m_use_normal);

   m_yield_start = m_yield_stop = 3.8f; m_yield_tests = 1;
   m_conc_start = m_conc_stop=1400.0f; m_conc_tests = 1;
   m_plasma_start = m_plasma_stop = 0.0f; m_plasma_tests = 1;
   m_rbcdose_start = m_rbcdose_stop = 0.0f; m_rbcdose_tests = 1;
   m_amap = 0;
   m_max_concurrent = 0;
   UpdateData(0);
}

void robustness::OnSequential() 
{
   UpdateData(1);
   if (m_use_normal) {
      MessageBox("Normal distribution and sequential are mutually exclusive","Input Error",MB_ICONSTOP);
      m_sequential = 0;
      UpdateData(0);
      return;
   }
}

const int robustness::pseudodistribution(const double vm,const double exp)
{
   float fx = pow(2.0*vm - vm*vm,exp);
   return (int)(fx*1000.0);
}
