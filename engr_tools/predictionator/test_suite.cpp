// test_suite.cpp : implementation file
//

#include "stdafx.h"
#include "ap2 predictionater.h"
#include "test_suite.h"

extern CAP2predictionaterApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int numRunData = 10 ;

static const int ratiotries = 4;
static const float ratiostarts[ratiotries] = { 6.0f,13.7f, 7.0f,  6.0f};
static const float ratioends[ratiotries]   = { 6.0f,13.7f,11.0f, 13.7f};
static const float ratiotimes[ratiotries]  = {10.0f,10.0f,15.0f,100.0f};

static const int hcttries = 6;
static float hct[hcttries] = {0.34f, 0.39f, 0.41f, 0.44f, 0.50f, 0.55f};

static const int pltproducts = 16;
static const float yield[pltproducts] = {0.0f,   2.0f,   2.0f,   2.0f,   2.0f,   4.0f,   4.0f,   4.0f,   4.0f,   8.0f,   8.0f,   8.0f,   8.0f,  15.0f,   15.0f, 15.0f};
static const float conc[pltproducts]  = {0.0f,1100.0f,1400.0f,2100.0f,3000.0f,1100.0f,1400.0f,2100.0f,3000.0f,1100.0f,1400.0f,2100.0f,3000.0f, 800.0f, 1400.0f, 3000.0f};

static const int rbcproducts = 8;
static const float rbcdose[rbcproducts] = {0.0f,  35.0f, 35.0f, 250.0f, 250.0f, 250.0f, 550.0f, 550.0f};
static const float rbccrit[rbcproducts] = {0.0f,  0.80f, 0.55f, 0.80f,  0.70f,  0.55f,   0.80f,  0.55f};

static const int plasmaproducts = 6;
static const float plasmavolume[plasmaproducts] = {0.0f, 200.0f, 400.0f, 600.0f, -1.0f, -2.0f};

static const int fluidbalancetries = 4;
static const float fluidbalance[fluidbalancetries] = {0.0f,   0.80f,  1.0f,   1.2f};

static const int hypotries = 7;
static const int hypomethod[hypotries]        = {TBV_SETTING, TBV_SETTING, TBV_SETTING, VOLUME_SETTING, VOLUME_SETTING, VOLUME_SETTING, VOLUME_SETTING};
static const float hypopercent[hypotries]     = {  0.01f,        0.12f,        0.15f,       0.0f,          0.01f,           0.0f,            0.0f};
static const float hypolessthan[hypotries]    = {   0.0f,        0.0f,         0.0f,      400.0f,         500.0f,           0.0f,            0.0f};
static const float hypogreaterthan[hypotries] = {   0.0f,        0.0f,         0.0f,        0.0f,           0.0f,         500.0f,          600.0f};
static const float hypoweight[hypotries]      = {   0.0f,        0.0f,         0.0f,      175.0f,         175.0f,         225.0f,          225.0f};

static const int proctimetries = 3;
static const float proctimes[proctimetries] = { 10.0f, 80.0f, 150.0f };

/////////////////////////////////////////////////////////////////////////////
// test_suite

IMPLEMENT_DYNCREATE(test_suite, CFormView)

test_suite::test_suite() :
	optimizer_form(test_suite::IDD),
   m_ratio_index(0),
   m_hypo_index(0),
   m_proctime_index(0),
   m_icrit_index(0),
   m_ysf_index(0),
   m_tbv_index(0),
   m_hct_index(0),
   m_pltpre_index(0),
   m_rbc_product_index(0),
   m_plasma_product_index(0),
   m_platelet_product_index(0),
   m_draw_flow_index(0),
   m_balance_index(0),
   m_last_number_updated(0),
   m_qrp_adjustment_value(0.0f),
   m_qin_limit_value(0.0f)
{
	EnableAutomation();
	//{{AFX_DATA_INIT(test_suite)
	m_percent_complete = 0;
	m_total_products = 0;
	m_total_good = 0;
	m_qrp_adjustment_index = -1;
	m_qin_limit_index = -1;
	m_test_substate_index = -1;
	m_product_plasma = FALSE;
	m_product_plasma_rbcs = FALSE;
	m_product_platelet = FALSE;
	m_product_platelet_plasma = FALSE;
	m_product_platelet_plasma_rbcs = FALSE;
	m_product_platelet_rbcs = FALSE;
	m_product_rbcs = FALSE;
	m_total_tried = 0;
	m_remaining_time = _T("");
	m_total_plasma = 0;
	m_total_plasma_rbcs = 0;
	m_total_platelets = 0;
	m_total_platelets_plasma = 0;
	m_total_platelets_rbcs = 0;
	m_good_plasma = 0;
	m_good_plasma_rbcs = 0;
	m_good_platelets = 0;
	m_good_platelets_plasma = 0;
	m_good_platelets_plasma_rbcs = 0;
	m_good_platelets_rbcs = 0;
	m_good_rbcs = 0;
	m_total_platelets_plasma_rbcs = 0;
	m_total_rbcs = 0;
	m_first_to_try = 0;
	m_iterations_plasma = 0;
	m_iterations_plasma_rbcs = 0;
	m_iterations_platelets = 0;
	m_iterations_platelets_plasma = 0;
	m_iterations_platelets_plasma_rbcs = 0;
	m_iterations_platelets_rbcs = 0;
	m_iterations_rbcs = 0;
	m_iterations_total = 0;
	m_conc_plasma = 0;
	m_conc_plasma_rbcs = 0;
	m_conc_platelets = 0;
	m_conc_platelets_plasma = 0;
	m_conc_platelets_plasma_rbcs = 0;
	m_conc_platelets_rbcs = 0;
	m_conc_rbcs = 0;
	m_conc_total = 0;
	m_max_iterations_amap_pl = 0;
	m_max_iterations_amap_pl_rbc = 0;
	m_max_iterations_amap_plt = 0;
	m_max_iterations_amap_plt_pl = 0;
	m_max_iterations_amap_plt_pl_rbc = 0;
	m_max_iterations_amap_plt_rbc = 0;
	m_max_iterations_amap_rbc = 0;
	m_max_iterations_pl = 0;
	m_max_iterations_pl_rbc = 0;
	m_max_iterations_plt = 0;
	m_max_iterations_plt_pl = 0;
	m_max_iterations_plt_pl_rbc = 0;
	m_max_iterations_rbc = 0;
	m_max_iterations_plt_rbc = 0;
	m_elapsed_time = _T("");
	//}}AFX_DATA_INIT
}

test_suite::~test_suite()
{
}

void test_suite::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	optimizer_form::OnFinalRelease();
}

void test_suite::DoDataExchange(CDataExchange* pDX)
{
	optimizer_form::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(test_suite)
	DDX_Control(pDX, IDC_INITIAL_CONDITIONS, m_initial_conditions_control);
	DDX_Control(pDX, IDC_STOP_SIMULATION, m_stop_button);
	DDX_Control(pDX, IDC_START_SIMULATION, m_start_button);
	DDX_Control(pDX, IDC_EDIT_CONFIG, m_edit_config_control);
	DDX_Control(pDX, IDC_FIRST_TO_TRY, m_first_to_try_control);
	DDX_Control(pDX, IDC_PRODUCT_RBCS, m_product_rbcs_control);
	DDX_Control(pDX, IDC_PRODUCT_PLATELET_RBCS, m_product_platelet_rbcs_control);
	DDX_Control(pDX, IDC_PRODUCT_PLATELET_PLASMA_RBCS, m_product_platelet_plasma_rbcs_control);
	DDX_Control(pDX, IDC_PRODUCT_PLATELET_PLASMA, m_product_platelet_plasma_control);
	DDX_Control(pDX, IDC_PRODUCT_PLATELET, m_product_platelet_control);
	DDX_Control(pDX, IDC_PRODUCT_PLASMA_RBCS, m_product_plasma_rbcs_control);
	DDX_Control(pDX, IDC_PRODUCT_PLASMA, m_product_plasma_control);
	DDX_Control(pDX, IDC_TEST_SUBSTATE, m_test_substate_control);
	DDX_Control(pDX, IDC_QRP_ADJUSTMENT, m_qrp_adjustment_control);
	DDX_Control(pDX, IDC_QIN_LIMIT, m_qin_limit_control);
	DDX_Control(pDX, IDC_TEST_SUITE_PROGRESS, m_test_suite_progress);
	DDX_Text(pDX, IDC_PERCENT_COMPLETE, m_percent_complete);
	DDX_Text(pDX, IDC_TOTAL_PRODUCTS, m_total_products);
	DDX_Text(pDX, IDC_TOTAL_GOOD, m_total_good);
	DDX_CBIndex(pDX, IDC_QRP_ADJUSTMENT, m_qrp_adjustment_index);
	DDX_CBIndex(pDX, IDC_QIN_LIMIT, m_qin_limit_index);
	DDX_CBIndex(pDX, IDC_TEST_SUBSTATE, m_test_substate_index);
	DDX_Check(pDX, IDC_PRODUCT_PLASMA, m_product_plasma);
	DDX_Check(pDX, IDC_PRODUCT_PLASMA_RBCS, m_product_plasma_rbcs);
	DDX_Check(pDX, IDC_PRODUCT_PLATELET, m_product_platelet);
	DDX_Check(pDX, IDC_PRODUCT_PLATELET_PLASMA, m_product_platelet_plasma);
	DDX_Check(pDX, IDC_PRODUCT_PLATELET_PLASMA_RBCS, m_product_platelet_plasma_rbcs);
	DDX_Check(pDX, IDC_PRODUCT_PLATELET_RBCS, m_product_platelet_rbcs);
	DDX_Check(pDX, IDC_PRODUCT_RBCS, m_product_rbcs);
	DDX_Text(pDX, IDC_TOTAL_TRIED, m_total_tried);
	DDX_Text(pDX, IDC_REMAINING_TIME, m_remaining_time);
	DDX_Text(pDX, IDC_TOTAL_PLASMA, m_total_plasma);
	DDX_Text(pDX, IDC_TOTAL_PLASMA_RBCS, m_total_plasma_rbcs);
	DDX_Text(pDX, IDC_TOTAL_PLATELETS, m_total_platelets);
	DDX_Text(pDX, IDC_TOTAL_PLATELETS_PLASMA, m_total_platelets_plasma);
	DDX_Text(pDX, IDC_TOTAL_PLATELETS_RBCS, m_total_platelets_rbcs);
	DDX_Text(pDX, IDC_GOOD_PLASMA, m_good_plasma);
	DDX_Text(pDX, IDC_GOOD_PLASMA_RBCS, m_good_plasma_rbcs);
	DDX_Text(pDX, IDC_GOOD_PLATELETS, m_good_platelets);
	DDX_Text(pDX, IDC_GOOD_PLATELETS_PLASMA, m_good_platelets_plasma);
	DDX_Text(pDX, IDC_GOOD_PLATELETS_PLASMA_RBCS, m_good_platelets_plasma_rbcs);
	DDX_Text(pDX, IDC_GOOD_PLATELETS_RBCS, m_good_platelets_rbcs);
	DDX_Text(pDX, IDC_GOOD_RBCS, m_good_rbcs);
	DDX_Text(pDX, IDC_TOTAL_PLATELETS_PLASMA_RBCS, m_total_platelets_plasma_rbcs);
	DDX_Text(pDX, IDC_TOTAL_RBCS, m_total_rbcs);
	DDX_Text(pDX, IDC_FIRST_TO_TRY, m_first_to_try);
	DDX_Text(pDX, IDC_ITERATIONS_PLASMA, m_iterations_plasma);
	DDX_Text(pDX, IDC_ITERATIONS_PLASMA_RBCS, m_iterations_plasma_rbcs);
	DDX_Text(pDX, IDC_ITERATIONS_PLATELETS, m_iterations_platelets);
	DDX_Text(pDX, IDC_ITERATIONS_PLATELETS_PLASMA, m_iterations_platelets_plasma);
	DDX_Text(pDX, IDC_ITERATIONS_PLATELETS_PLASMA_RBCS, m_iterations_platelets_plasma_rbcs);
	DDX_Text(pDX, IDC_ITERATIONS_PLATELETS_RBCS, m_iterations_platelets_rbcs);
	DDX_Text(pDX, IDC_ITERATIONS_RBCS, m_iterations_rbcs);
	DDX_Text(pDX, IDC_ITERATIONS_TOTAL, m_iterations_total);
	DDX_Text(pDX, IDC_CONC_PLASMA, m_conc_plasma);
	DDX_Text(pDX, IDC_CONC_PLASMA_RBCS, m_conc_plasma_rbcs);
	DDX_Text(pDX, IDC_CONC_PLATELETS, m_conc_platelets);
	DDX_Text(pDX, IDC_CONC_PLATELETS_PLASMA, m_conc_platelets_plasma);
	DDX_Text(pDX, IDC_CONC_PLATELETS_PLASMA_RBCS, m_conc_platelets_plasma_rbcs);
	DDX_Text(pDX, IDC_CONC_PLATELETS_RBCS, m_conc_platelets_rbcs);
	DDX_Text(pDX, IDC_CONC_RBCS, m_conc_rbcs);
	DDX_Text(pDX, IDC_CONC_TOTAL, m_conc_total);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_AMAP_PL, m_max_iterations_amap_pl);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_AMAP_PL_RBC, m_max_iterations_amap_pl_rbc);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_AMAP_PLT, m_max_iterations_amap_plt);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_AMAP_PLT_PL, m_max_iterations_amap_plt_pl);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_AMAP_PLT_PL_RBC, m_max_iterations_amap_plt_pl_rbc);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_AMAP_PLT_RBC, m_max_iterations_amap_plt_rbc);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_AMAP_RBC, m_max_iterations_amap_rbc);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_PL, m_max_iterations_pl);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_PL_RBC, m_max_iterations_pl_rbc);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_PLT, m_max_iterations_plt);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_PLT_PL, m_max_iterations_plt_pl);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_PLT_PL_RBC, m_max_iterations_plt_pl_rbc);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_RBC, m_max_iterations_rbc);
	DDX_Text(pDX, IDC_MAX_ITERATIONS_PLT_RBC, m_max_iterations_plt_rbc);
	DDX_Text(pDX, IDC_ELAPSED_TIME, m_elapsed_time);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(test_suite, optimizer_form)
	//{{AFX_MSG_MAP(test_suite)
	ON_CBN_CLOSEUP(IDC_QIN_LIMIT, OnCloseupQinLimit)
	ON_CBN_CLOSEUP(IDC_QRP_ADJUSTMENT, OnCloseupQrpAdjustment)
	ON_CBN_CLOSEUP(IDC_TEST_SUBSTATE, OnCloseupTestSubstate)
	ON_BN_CLICKED(IDC_PRODUCT_PLASMA, OnProductSelection)
	ON_BN_CLICKED(IDC_INITIAL_CONDITIONS, OnInitialConditions)
	ON_BN_CLICKED(IDC_EDIT_CONFIG, OnEditConfig)
	ON_BN_CLICKED(IDC_PRODUCT_PLASMA_RBCS, OnProductSelection)
	ON_BN_CLICKED(IDC_PRODUCT_PLATELET, OnProductSelection)
	ON_BN_CLICKED(IDC_PRODUCT_PLATELET_PLASMA, OnProductSelection)
	ON_BN_CLICKED(IDC_PRODUCT_PLATELET_PLASMA_RBCS, OnProductSelection)
	ON_BN_CLICKED(IDC_PRODUCT_PLATELET_RBCS, OnProductSelection)
	ON_BN_CLICKED(IDC_PRODUCT_RBCS, OnProductSelection)
	ON_BN_CLICKED(IDC_START_SIMULATION, OnStart)
	ON_BN_CLICKED(IDC_STOP_SIMULATION, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(test_suite, optimizer_form)
	//{{AFX_DISPATCH_MAP(test_suite)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_Itest_suite to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {AAC75BB3-EA69-11D1-8054-006097402E62}
static const IID IID_Itest_suite =
{ 0xaac75bb3, 0xea69, 0x11d1, { 0x80, 0x54, 0x0, 0x60, 0x97, 0x40, 0x2e, 0x62 } };

BEGIN_INTERFACE_MAP(test_suite, optimizer_form)
	INTERFACE_PART(test_suite, IID_Itest_suite, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// test_suite diagnostics

#ifdef _DEBUG
void test_suite::AssertValid() const
{
	optimizer_form::AssertValid();
}

void test_suite::Dump(CDumpContext& dc) const
{
	optimizer_form::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// test_suite message handlers

void test_suite::OnStart() 
{
   //
   //
   //   The following data is from an actual run file (Al020397.009)
   //   Modified 3/17/97 to account for new states.h and to include
   //   pir processed volume and flows (numbers for these are made up.)
   //   12/5/97: made up data for start states (up to and including start4)
   //
   static const double       procTime[numRunData]  = {0.00,                    12.44,                    17.00,        19.00,        21.69,     26.67,             37.45,                     41.30,     41.30,       46.23};
   static const double       procRunT[numRunData]  = {0.00,                     0.44,                    4.39,         6.88,         9.86,      14.84,             25.62,                     29.48,     29.48,       34.40};
   static const double       vin[numRunData]       = {0.00,                    14.25,                    135.26,       228.0,        442.00,    929.76,            1961.69,                   2278.85,   2278.85,     2574.65};
   static const double       vac[numRunData]       = {0.00,                    22.37,                    35.44,        45.20,        78.03,     126.34,            220.31,                    251.55,    251.55,      286.48};
   static const double       vplasma[numRunData]   = {0.00,                     0.00,                    18.44,        54.44,        127.52,    216.83,            495.76,                    701.09,    701.09,      781.21};
   static const double       vcollect[numRunData]  = {0.00,                     0.00,                    13.11,        18.44,        42.24,     95.28,             261.96,                    365.82,    365.82,      434.63};
   static const double       vreturn[numRunData]   = {0.00,                   -19.33,                    -37.43,       49.44,        306.22,    839.25,            2524.88,                   3407.91,   3407.91,     3615.46};
   static const double       vcolbag[numRunData]   = {0.00,                     0.00,                    0.0,          2.44,         19.14,     72.19,             182.32,                    215.79,    215.79,      221.06};
   static const double       vplbag[numRunData]    = {0.00,                     0.00,                    0.0,          0.0,          0.00,      44.30,             100.63,                    100.63,    100.63,      100.63};
   static const double       vrbcbag[numRunData]   = {0.00,                     0.00,                    0.0,          0.0,          0.00,      0.00,              0.00,                      0.00,      0.00,        72.49};
   static const State_names  state[numRunData]     = {FIRST_STATE,             BLOOD_PRIME,              BLOOD_RUN,    BLOOD_RUN,    BLOOD_RUN, BLOOD_RUN,         BLOOD_RUN,                 BLOOD_RUN, BLOOD_RUN,   BLOOD_RUN};
   static const State_names  laststate[numRunData] = {PRESSURIZE_RETURN_LINE,  BLOOD_PRIME_RETURN_LINE,  STARTUP_RUN2, STARTUP_RUN4, RUN,       PLASMA_COLLECTION, FINAL_PLATELET_COLLECTION, PIR,       PLASMA_ONLY, RBC_COLLECTION};
   static const State_names  substate[numRunData]  = {PRESSURIZE_RETURN_LINE,  BLOOD_PRIME_RETURN_LINE,  STARTUP_RUN2, STARTUP_RUN4, RUN,       PLASMA_COLLECTION, SPILLOVER_RECOVERY,        PIR,       PLASMA_ONLY, RBC_COLLECTION};
   static const bool         recovery[numRunData]  = {lFALSE,                  lFALSE,                   lFALSE,       lFALSE,       lFALSE,    lFALSE,            lTRUE,                     lFALSE,    lFALSE,      lFALSE};
   static const double       vaccol[numRunData]    = {0.00,                    0.00,                     0.00,         0.44,         3.51,      12.13,             28.62,                     33.95,     33.95,       34.79};
   static const double       vacpl[numRunData]     = {0.00,                    0.00,                     0.0,          0.0,          0.00,      7.08,              15.59,                     15.59,     15.59,       15.59};
   static const double       vacrbc[numRunData]    = {0.00,                    0.00,                     0.0,          0.0,          0.00,      0.00,              0.00,                      0.00,      0.00,        5.11};
   static const double       pltyld[numRunData]    = {0.00,                    0.00,                     0.0,          0.0,          0.00,      1.06,              3.36,                      4.01,      4.01,        4.12};
   static const double       PIRvol[numRunData]    = {0.00,                    0.00,                     0.0,          0.0,          0.00,      0.00,              0.00,                      25.00,     25.00,       40.00};
   static const double       Qin[numRunData]       = {0.00,                    0.00,                     20.0,         72.2,         89.0,      142.0,             89.0,                      91.0,      91.0,        64.0};
   static const double       Qcol[numRunData]      = {0.00,                    0.00,                     3.50,         3.39,         4.2,       5.5,               20.0,                      9.0,       9.0,         18.0};
   static const double       RPM[numRunData]       = {0.00,                   200.0,                     2000.0,       3000.0,       3000.0,    3000.0,            3000.0,                    3000.0,    3000.0,      3000.0};

   enable_controls(0);
   optimizer_form::OnStart();

   // get data from form into members
	UpdateData( TRUE );
   update_products();

   SetProcedureTime((float)procTime[m_test_substate_index]);
   SetProcedureRunTime((float)procRunT[m_test_substate_index]);
   Set_Vin((float)vin[m_test_substate_index]);
   Set_Vac((float)vac[m_test_substate_index]);
   Set_Vplasma((float)vplasma[m_test_substate_index]);
   Set_Vcollect((float)vcollect[m_test_substate_index]);
   Set_Vreturn((float)vreturn[m_test_substate_index]);
   Set_VcollectBag((float)vcolbag[m_test_substate_index]);
   Set_VplasmaBag((float)vplbag[m_test_substate_index]);
   Set_VrbcBag((float)vrbcbag[m_test_substate_index]);
   SetSystemState(state[m_test_substate_index]);
   SetLastSubstate(laststate[m_test_substate_index]);
   SetSubstate(substate[m_test_substate_index]);
   SetRecovery(recovery[m_test_substate_index]);
   Vac_collectBag((float)vaccol[m_test_substate_index]);
   Set_VacplasmaBag((float)vacpl[m_test_substate_index]);
   Set_VacrbcBag((float)vacrbc[m_test_substate_index]);
   Set_PltYield((float)pltyld[m_test_substate_index]*1.0E11f);
   SetPIRProcessedVolume((float)PIRvol[m_test_substate_index]);
   Set_Qin((float)Qin[m_test_substate_index]);
   Set_Qcollect((float)Qcol[m_test_substate_index]);
   SetCentrifugeSpeed((float)RPM[m_test_substate_index]);

   SetMinPostcount(50.0f);
   SetMinPostHct(0.32f);
   AsMuchAsPossiblePlasma(50.0f);
   MinVolumeRequiringReplacement(10.0f);
   //  Qin and Qrp adjustment values set via dialog
   SetRatioAdjustmentValue(13.7f);
   SetIRAdjustmentValue(1.2f);
   SetMaxProcedureTime(140.0f);
     

   m_percent_complete = 0;
   m_total_good = 0;
   m_total_tried = 0;
   m_last_number_updated = 0;
	m_test_suite_progress.SetPos( (int)0 );

   
   //
   //
   //   Reinitialize index variables
   //
   m_ratio_index = 0; 
   m_hypo_index = 0;
   m_proctime_index = 0;
   m_icrit_index = -1;// special first time through case
   m_ysf_index = 0;
   m_tbv_index = 0;
   m_hct_index = 0;
   m_pltpre_index = 0;
   m_rbc_product_index = 0;
   m_plasma_product_index = 0;
   m_platelet_product_index = 0;
   m_draw_flow_index = 0;
   m_balance_index = 0;

   m_total_plasma = 0;
	m_total_plasma_rbcs = 0;
	m_total_platelets = 0;
	m_total_platelets_plasma = 0;
	m_total_platelets_rbcs = 0;
	m_good_plasma = 0;
	m_good_plasma_rbcs = 0;
	m_good_platelets = 0;
	m_good_platelets_plasma = 0;
	m_good_platelets_plasma_rbcs = 0;
	m_good_platelets_rbcs = 0;
	m_good_rbcs = 0;
	m_total_platelets_plasma_rbcs = 0;
	m_total_rbcs = 0;

	m_iterations_plasma = 0;
	m_iterations_plasma_rbcs = 0;
	m_iterations_platelets = 0;
	m_iterations_platelets_plasma = 0;
	m_iterations_platelets_plasma_rbcs = 0;
	m_iterations_platelets_rbcs = 0;
	m_iterations_rbcs = 0;
	m_iterations_total = 0;
	m_iterations_plasma = 0;
	m_iterations_plasma_rbcs = 0;
	m_iterations_platelets = 0;
	m_iterations_platelets_plasma = 0;
	m_iterations_platelets_plasma_rbcs = 0;
	m_iterations_platelets_rbcs = 0;
	m_iterations_rbcs = 0;
	m_iterations_total = 0;
	m_conc_plasma = 0;
	m_conc_plasma_rbcs = 0;
	m_conc_platelets = 0;
	m_conc_platelets_plasma = 0;
	m_conc_platelets_plasma_rbcs = 0;
	m_conc_platelets_rbcs = 0;
	m_conc_rbcs = 0;
	m_conc_total = 0;
	m_max_iterations_amap_pl = 0;
	m_max_iterations_amap_pl_rbc = 0;
	m_max_iterations_amap_plt = 0;
	m_max_iterations_amap_plt_pl = 0;
	m_max_iterations_amap_plt_pl_rbc = 0;
	m_max_iterations_amap_plt_rbc = 0;
	m_max_iterations_amap_rbc = 0;
	m_max_iterations_pl = 0;
	m_max_iterations_pl_rbc = 0;
	m_max_iterations_plt = 0;
	m_max_iterations_plt_pl = 0;
	m_max_iterations_plt_pl_rbc = 0;
	m_max_iterations_rbc = 0;
	m_max_iterations_plt_rbc = 0;

   UpdateData(0);
   OnCloseupQinLimit(); // note calls UpdateData so above data must
                        // be updated before call
   OnCloseupQrpAdjustment();

   // Set memory leak checking
   // int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
   // tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
   // _CrtSetDbgFlag( tmpFlag );

   m_simulation_active = 1;
}

void test_suite::OnStop() 
{
   optimizer_form::OnStop();
   enable_controls(1);
}


void test_suite::OnCloseupQinLimit() 
{
   UpdateData(1);
   CString data;
   m_qin_limit_control.GetLBText(m_qin_limit_index, data);
   m_qin_limit_value = (float)atof(data);
   SetQinAdjustmentValue(m_qin_limit_value);
   update_products();
}

void test_suite::OnCloseupQrpAdjustment() 
{
   UpdateData(1);
   CString data;
   m_qrp_adjustment_control.GetLBText(m_qrp_adjustment_index, data);
   m_qrp_adjustment_value = (float)atof(data);
   SetQrpAdjustmentValue(m_qrp_adjustment_value);
   update_products();
}

void test_suite::OnCloseupTestSubstate() 
{
   update_products();
}

void test_suite::OnInitialUpdate() 
{
	optimizer_form::OnInitialUpdate();
   set_processing_function(this,(IdleFunction::idle_loop_function)&test_suite::IdleCallback);
   enable_controls(1);
   m_qrp_adjustment_control.SetCurSel(m_qrp_adjustment_control.GetCount()-1);
   m_qin_limit_control.SetCurSel(m_qin_limit_control.GetCount()-1);
   m_test_substate_control.SetCurSel(0);
}

//
//
// ----------------------------------------------------------------------------
//                    Enable / disable controls when simulation running
void test_suite::enable_controls(const int enable) 
{
   m_product_plasma_control.EnableWindow(enable);
   m_product_plasma_rbcs_control.EnableWindow(enable);
   m_product_platelet_control.EnableWindow(enable);
   m_product_platelet_plasma_control.EnableWindow(enable);
   m_product_platelet_plasma_rbcs_control.EnableWindow(enable);
   m_product_platelet_rbcs_control.EnableWindow(enable);
   m_product_rbcs_control.EnableWindow(enable);
   m_qin_limit_control.EnableWindow(enable);
   m_qrp_adjustment_control.EnableWindow(enable);
   m_test_substate_control.EnableWindow(enable);
   m_first_to_try_control.EnableWindow(enable);
   m_start_button.EnableWindow(enable);
   m_edit_config_control.EnableWindow(enable);
   m_initial_conditions_control.EnableWindow(enable);

   //
   //
   //   These members are "reversed"
   //
   m_stop_button.EnableWindow(!enable);

}
//
// ----------------------------------------------------------------------------
//   Determine if a given product is active.  Note that this method assumes
//   that the values for the check boxes have been properly updated.
//
int test_suite::product_active(const float pltvol,const float plasmavol,
                                const float rbcvol)
{
   int result = 0;
   //
   //
   //   See what kind of product it is and if the appropriate box
   //   is checked.  There's got to be an easier way but perhaps not
   //   a lazier way.
   //
   if (pltvol == 0.0f) {
      if (rbcvol == 0.0f) {
         if (plasmavol == 0.0f) {
            // no product
         } else {
            if (m_product_plasma) result = fluidbalancetries;
         }
      } else {
         if (plasmavol == 0.0f) {
            if (m_product_rbcs) result = fluidbalancetries*2;
         } else {
            if (m_product_plasma_rbcs) result = fluidbalancetries*2;
         }
      }
   } else if (plasmavol == 0.0f) {
      if (rbcvol == 0.0f) {
         if (m_product_platelet) result = ratiotries;
      } else {
         if (m_product_platelet_rbcs) result = ratiotries*2;
      }
   } else {
      if (rbcvol == 0.0f) {
         if (m_product_platelet_plasma) result = ratiotries;
      } else {
         if (m_product_platelet_plasma_rbcs) result = ratiotries*2;
      }
   }
   return result;
}
//
// ----------------------------------------------------------------------------
void test_suite::update_products()
{
   UpdateData(TRUE);
   int selected_products = 0;
   for (int pltprod=0; pltprod<pltproducts; pltprod++) {
      for (int plasmaprod=0; plasmaprod<plasmaproducts; plasmaprod++) {
         for (int rbcprod=0; rbcprod<rbcproducts; rbcprod++) {
            //
            //
            //   Get the appropriate volumes for this product
            //
            float pltvol = 0.0f;
            if (conc[pltprod] > 0.0f) {
               pltvol = yield[pltprod]*100000.0f/conc[pltprod];
            }        
            selected_products += product_active(pltvol,(float)plasmavolume[plasmaprod],(float)rbcdose[rbcprod]);
         }
      }
   }
   m_total_products = selected_products
                      * 3 // inlet draw flow limits
                      * hypotries
                      * proctimetries
                      * 6 // ac infusion
                      * 3 // YSF
                      * 4 // TBV
                      * hcttries
                      * 4; // precount

   UpdateData(FALSE);
}

//
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void test_suite::IdleCallback()
{
   static const int NUMBER_PER_ACTIVE_UPDATE = 100;
   static const int NUMBER_PER_INACTIVE_UPDATE = 1000;

   Test();


   int number_per_update = ((m_total_tried > m_first_to_try)? NUMBER_PER_ACTIVE_UPDATE: NUMBER_PER_INACTIVE_UPDATE);
   float percent_complete = (float)m_total_tried / (float)m_total_products;
   m_percent_complete = (int)(percent_complete * 100.0f);
   if ((m_total_tried - m_last_number_updated) >= number_per_update) {
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
      UpdateData(FALSE);
      
   }
}

void test_suite::Test()
{
   if (!m_simulation_active) return;

   if (m_balance_index == 0) {
      if (++m_icrit_index >= 6) {
         m_icrit_index = 0;
         if (++m_hypo_index >= hypotries) {
            m_hypo_index = 0;
            if (++m_proctime_index >= proctimetries) {
               m_proctime_index = 0;
               if (++m_ysf_index >= 3) {
                  m_ysf_index = 0;
                  if (++m_tbv_index >= 4) {
                     m_tbv_index = 0;
                     if (++m_hct_index >= hcttries) {
                        m_hct_index = 0;
                        if (++m_pltpre_index >= 4) {
                           m_pltpre_index = 0;
                           if (++m_draw_flow_index >= 3) {
                              m_draw_flow_index = 0;
                              if (++m_platelet_product_index >= pltproducts) {
                                 m_platelet_product_index = 0;
                                 if (++m_plasma_product_index >= plasmaproducts) {
                                    m_plasma_product_index = 0;
                                    if (++m_rbc_product_index >= rbcproducts) {
                                       m_rbc_product_index = 0;
                                       UpdateData(FALSE);
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

   float plasmavol = plasmavolume[m_plasma_product_index];
   float pltvol = 0.0f;
   if (conc[m_platelet_product_index] > 0.0f) {
      pltvol = yield[m_platelet_product_index]*100000.0f/conc[m_platelet_product_index];
   }


   //
   //
   //   Perform the next optimization.
   //
   
   int iterations = product_active(pltvol,plasmavol,rbcdose[m_rbc_product_index]);
   if (iterations > 0) {
      ACInfusionSetting(m_icrit_index+1);
      SetYieldScalingFactor(0.8f + m_ysf_index*0.2f);
      if (hypomethod[m_hypo_index] == TBV_SETTING) {
         SetMaxPercentRemoved(hypopercent[m_hypo_index]); 
         SetRemoveVolumeGreaterthan(0.0f);
         SetRemoveVolumeWeight(0.0f);
         SetRemoveVolumeLessthan(0.0f);
      } else {
         SetMaxPercentRemoved(0.0f);
         SetRemoveVolumeGreaterthan(hypogreaterthan[m_hypo_index]);
         SetRemoveVolumeWeight(hypoweight[m_hypo_index]);       
         SetRemoveVolumeLessthan(hypolessthan[m_hypo_index]);   
      }

      switch (m_draw_flow_index) {
         case DRAW_FLOW_SLOW: MaxDrawFlowConfigLimit(100.0f); break;
         case DRAW_FLOW_MODERATE: MaxDrawFlowConfigLimit(120.0f); break;
         case DRAW_FLOW_FAST: MaxDrawFlowConfigLimit(MAX_QIN_INSTANTANEOUS); break;
      }
      
      //
      //
      //  These values are used for DRBC products and ignored for others
      //
      if (m_balance_index < fluidbalancetries) {
         if (fluidbalance[m_balance_index] > 0.0f) {
            ReplacementFluidBalance(fluidbalance[m_balance_index]);
            ConfigReplacementFluidRequired(RBC_REPLACE_FLUID_YES);
         } else {
            ConfigReplacementFluidRequired(RBC_REPLACE_FLUID_NO);
         }
      }
      //
      //
      //   These values are used for non-DRBC products and ignored for DRBC
      //
      if (m_balance_index < ratiotries) {
         SetCustomRatioStart(ratiostarts[m_balance_index]);
         SetCustomRatioEnd(ratioends[m_balance_index]);
         SetCustomTimeEnd(ratiotimes[m_balance_index]);
      }
      
      //
      //
      //   For RBC products we try both at 8.14 and 11.0 ratios
      //
      if (rbcdose[m_rbc_product_index] > 0.0f) {
         if (++m_ratio_index > 1) {
            m_ratio_index = 0;
            ++m_balance_index;
         }
      } else {
         ++m_balance_index;
      }
      if (m_ratio_index == 0) {
         SetRBCRatio(8.14f);
      } else {
         SetRBCRatio(11.0f);
      }
      if (m_first_to_try <= m_total_tried) {
         OnPredictionComplete();
      } else {
         ++m_total_tried;
      }
      if (m_balance_index >= iterations) m_balance_index = 0;
   }
}

void test_suite::OnProductSelection() 
{
   update_products();
}

void test_suite::OnInitialConditions() 
{
   optimizer_form::OnInitialConditions();	
}

BEGIN_EVENTSINK_MAP(test_suite, optimizer_form)
    //{{AFX_EVENTSINK_MAP(test_suite)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void test_suite::OnPredictionComplete() 
{

   optimizer().donor(200.0f, // weight
      ((float)m_tbv_index*2000.0f + 2500.0f),
      hct[m_hct_index],
      (200.0f + ((float)m_pltpre_index * 75.0f)),
      BLOOD_TYPE_NONE_ENTERED); // blood type 
      
   float pltvol = 0.0f;
      if (conc[m_platelet_product_index] > 0.0f) {
         pltvol = yield[m_platelet_product_index]*100000.0f/conc[m_platelet_product_index];
      }

   optimizer().initialize(yield[m_platelet_product_index],
      pltvol,
      plasmavolume[m_plasma_product_index],
      rbcdose[m_rbc_product_index],
      rbccrit[m_rbc_product_index],
      proctimes[m_proctime_index],
      BLOOD_TYPE_ALL);
   
   INTERNAL_PREDICT_RESULTS status = optimizer().optimize();
   ++m_total_tried;
   int iterations = optimizer().iterations();
   m_iterations_total += iterations;
   bool in_concentration = lFALSE;

   float plasmavol = plasmavolume[m_plasma_product_index];
   
   if (status == GOOD_PRODUCT) {
      ++m_total_good;
      if (optimizer().concentration_in_tolerance()) {
         ++m_conc_total;
         in_concentration = lTRUE;
      }
   }
   

   //
   //  Update the appropriate counters...
   //
   if (pltvol == 0.0f) {
      if (rbcdose[m_rbc_product_index] == 0.0f) {
         if (plasmavol == 0.0f) {
            // no product
         } else {
            ++m_total_plasma;
            if (status == GOOD_PRODUCT) ++m_good_plasma;
            if (plasmavol < 0.0f) {
               if (iterations > m_max_iterations_amap_pl) m_max_iterations_amap_pl = iterations;
            }  else {
               if (iterations > m_max_iterations_pl) m_max_iterations_pl = iterations;
            }
            m_iterations_plasma += iterations;
            if (in_concentration) ++m_conc_plasma;
         }
      } else {
         if (plasmavol == 0.0f) {
            ++m_total_rbcs;
            if (status == GOOD_PRODUCT) ++m_good_rbcs;
            if (iterations > m_max_iterations_rbc) m_max_iterations_rbc = iterations;
            m_iterations_rbcs += iterations;
            if (in_concentration) ++m_conc_rbcs;
         } else {
            ++m_total_plasma_rbcs;
            if (status == GOOD_PRODUCT) ++m_good_plasma_rbcs;
            if (plasmavol < 0.0f) {
               if (iterations > m_max_iterations_amap_pl_rbc) m_max_iterations_amap_pl_rbc = iterations;
            }  else {
               if (iterations > m_max_iterations_pl_rbc) m_max_iterations_pl_rbc = iterations;
            }
            if (in_concentration) ++m_conc_plasma_rbcs;
            m_iterations_plasma_rbcs += iterations;
         }
      }
   } else if (plasmavol == 0.0f) {
      if (rbcdose[m_rbc_product_index] == 0.0f) {
         ++m_total_platelets;
         if (status == GOOD_PRODUCT) ++m_good_platelets;
         if (iterations > m_max_iterations_plt) m_max_iterations_plt = iterations;
         m_iterations_platelets += iterations;
         if (in_concentration) ++m_conc_platelets;
      } else {
         ++m_total_platelets_rbcs;
         if (status == GOOD_PRODUCT) ++m_good_platelets_rbcs;
         if (iterations > m_max_iterations_plt_rbc) m_max_iterations_plt_rbc = iterations;
         m_iterations_platelets_rbcs += iterations;
         if (in_concentration) ++m_conc_platelets_rbcs;
      }
   } else {
      if (rbcdose[m_rbc_product_index] == 0.0f) {
         ++m_total_platelets_plasma;
         if (status == GOOD_PRODUCT) ++m_good_platelets_plasma;
         if (plasmavol < 0.0f) {
            if (iterations > m_max_iterations_amap_plt_pl) m_max_iterations_amap_plt_pl = iterations;
         }  else {
            if (iterations > m_max_iterations_plt_pl) m_max_iterations_plt_pl = iterations;
         }
         m_iterations_platelets_plasma += iterations;
         if (in_concentration) ++m_conc_platelets_plasma;
      } else {
         ++m_total_platelets_plasma_rbcs;
         if (status == GOOD_PRODUCT) ++m_good_platelets_plasma_rbcs;
         if (plasmavol < 0.0f) {
            if (iterations > m_max_iterations_amap_plt_pl_rbc) m_max_iterations_amap_plt_pl_rbc = iterations;
         }  else {
            if (iterations > m_max_iterations_plt_pl_rbc) m_max_iterations_plt_pl_rbc = iterations;
         }
         m_iterations_platelets_plasma_rbcs += iterations;
         if (in_concentration) ++m_conc_platelets_plasma_rbcs;
      }
   }
}

