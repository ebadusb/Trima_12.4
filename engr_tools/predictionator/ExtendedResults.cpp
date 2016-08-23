// ExtendedResults.cpp : implementation file
//
#include "stdafx.h"
#include "ap2 predictionater.h"
#include "ExtendedResults.h"
#include "Cassette.h"
#include "offline_optimizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define __GRID_X__ 10
#define __GRID_Y__ 175
#define __GRID_WIDTH__ 950
#define __GRID_NUM_ROWS_PLTPLSRBC__ 16
#define __GRID_NUM_ROWS_PLSRBC__ 12
#define __GRID_NUM_COLS__ 22
#define __GRID_TITLE_COL_WIDTH__ 125
#define __GRID_COL_WIDTH__ (__GRID_WIDTH__- __GRID_TITLE_COL_WIDTH__-1)/(__GRID_NUM_COLS__-1)
#define __GRID_ROW_HEIGHT__ 20
#define __GRID_HEIGHT__ __GRID_NUM_ROWS__*__GRID_ROW_HEIGHT__+5


/////////////////////////////////////////////////////////////////////////////
// CExtendedResults dialog


CExtendedResults::CExtendedResults(CWnd* pParent /*=NULL*/)
	: CDialog(CExtendedResults::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtendedResults)
	m_actual_concentration = 0.0f;
	m_actual_plasma = 0.0f;
	m_actual_platelets = 0.0f;
	m_actual_rbcs = 0.0f;
	m_actual_yield = 0.0f;
	m_cassette = _T("");
	m_configured_concentration = 0.0f;
	m_configured_platelets = 0.0f;
	m_configured_yield = 0.0f;
	m_remaining_volume = 0.0f;
	m_actual_max_time = 0.0f;
	m_configured_max_time = 0.0f;
	m_results = _T("");
	m_actual_replacement_volume = 0.0f;
	m_configured_replacement_volume = _T("");
	m_configured_rbcs = _T("");
	m_volume_from_donor = 0.0f;
	m_qac_1 = 0.0f;
	m_qac_10 = 0.0f;
	m_qac_11 = 0.0f;
	m_qac_2 = 0.0f;
	m_qac_3 = 0.0f;
	m_qac_4 = 0.0f;
	m_qac_5 = 0.0f;
	m_qac_6 = 0.0f;
	m_qac_7 = 0.0f;
	m_qac_8 = 0.0f;
	m_qac_9 = 0.0f;
	m_qrp_1 = 0.0f;
	m_qrp_10 = 0.0f;
	m_qrp_11 = 0.0f;
	m_qrp_2 = 0.0f;
	m_qrp_3 = 0.0f;
	m_qrp_4 = 0.0f;
	m_qrp_5 = 0.0f;
	m_qrp_6 = 0.0f;
	m_qrp_7 = 0.0f;
	m_qrp_8 = 0.0f;
	m_qrp_9 = 0.0f;
	m_time_1 = 0.0f;
	m_time_10 = 0.0f;
	m_time_11 = 0.0f;
	m_time_2 = 0.0f;
	m_time_3 = 0.0f;
	m_time_4 = 0.0f;
	m_time_5 = 0.0f;
	m_time_6 = 0.0f;
	m_time_7 = 0.0f;
	m_time_8 = 0.0f;
	m_time_9 = 0.0f;
	m_vin_1 = 0.0f;
	m_vin_10 = 0.0f;
	m_vin_11 = 0.0f;
	m_vin_2 = 0.0f;
	m_vin_3 = 0.0f;
	m_vin_4 = 0.0f;
	m_vin_5 = 0.0f;
	m_vin_6 = 0.0f;
	m_vin_7 = 0.0f;
	m_vin_8 = 0.0f;
	m_vin_9 = 0.0f;
	m_qcdraw_1 = 0.0f;
	m_qcdraw_10 = 0.0f;
	m_qcdraw_11 = 0.0f;
	m_qcdraw_2 = 0.0f;
	m_qcdraw_3 = 0.0f;
	m_qcdraw_4 = 0.0f;
	m_qcdraw_5 = 0.0f;
	m_qcdraw_6 = 0.0f;
	m_qcdraw_7 = 0.0f;
	m_qcdraw_8 = 0.0f;
	m_qcdraw_9 = 0.0f;
	m_qindraw_1 = 0.0f;
	m_qindraw_10 = 0.0f;
	m_qindraw_11 = 0.0f;
	m_qindraw_2 = 0.0f;
	m_qindraw_3 = 0.0f;
	m_qindraw_4 = 0.0f;
	m_qindraw_5 = 0.0f;
	m_qindraw_6 = 0.0f;
	m_qindraw_7 = 0.0f;
	m_qindraw_8 = 0.0f;
	m_qindraw_9 = 0.0f;
	m_qinreturn_1 = 0.0f;
	m_qinreturn_10 = 0.0f;
	m_qinreturn_11 = 0.0f;
	m_qinreturn_2 = 0.0f;
	m_qinreturn_3 = 0.0f;
	m_qinreturn_4 = 0.0f;
	m_qinreturn_5 = 0.0f;
	m_qinreturn_6 = 0.0f;
	m_qinreturn_7 = 0.0f;
	m_qinreturn_8 = 0.0f;
	m_qinreturn_9 = 0.0f;
	m_qpdraw_1 = 0.0f;
	m_qpdraw_10 = 0.0f;
	m_qpdraw_11 = 0.0f;
	m_qpdraw_2 = 0.0f;
	m_qpdraw_3 = 0.0f;
	m_qpdraw_4 = 0.0f;
	m_qpdraw_5 = 0.0f;
	m_qpdraw_6 = 0.0f;
	m_qpdraw_7 = 0.0f;
	m_qpdraw_8 = 0.0f;
	m_qpdraw_9 = 0.0f;
	m_total_iterations = 0;
	m_qch_1 = 0.0f;
	m_qch_10 = 0.0f;
	m_qch_11 = 0.0f;
	m_qch_2 = 0.0f;
	m_qch_3 = 0.0f;
	m_qch_4 = 0.0f;
	m_qch_5 = 0.0f;
	m_qch_6 = 0.0f;
	m_qch_7 = 0.0f;
	m_qch_8 = 0.0f;
	m_qch_9 = 0.0f;
	m_vp_1 = 0.0f;
	m_vp_10 = 0.0f;
	m_vp_11 = 0.0f;
	m_vp_2 = 0.0f;
	m_vp_3 = 0.0f;
	m_vp_4 = 0.0f;
	m_vp_5 = 0.0f;
	m_vp_6 = 0.0f;
	m_vp_7 = 0.0f;
	m_vp_8 = 0.0f;
	m_vp_9 = 0.0f;
	m_vc_1 = 0.0f;
	m_vc_10 = 0.0f;
	m_vc_11 = 0.0f;
	m_vc_2 = 0.0f;
	m_vc_3 = 0.0f;
	m_vc_4 = 0.0f;
	m_vc_5 = 0.0f;
	m_vc_6 = 0.0f;
	m_vc_7 = 0.0f;
	m_vc_8 = 0.0f;
	m_vc_9 = 0.0f;
	m_vac_1 = 0.0f;
	m_vac_10 = 0.0f;
	m_vac_11 = 0.0f;
	m_vac_2 = 0.0f;
	m_vac_3 = 0.0f;
	m_vac_4 = 0.0f;
	m_vac_5 = 0.0f;
	m_vac_6 = 0.0f;
	m_vac_7 = 0.0f;
	m_vac_8 = 0.0f;
	m_vac_9 = 0.0f;
	m_ir_1 = 0.0f;
	m_ir_10 = 0.0f;
	m_ir_11 = 0.0f;
	m_ir_2 = 0.0f;
	m_ir_3 = 0.0f;
	m_ir_4 = 0.0f;
	m_ir_5 = 0.0f;
	m_ir_6 = 0.0f;
	m_ir_7 = 0.0f;
	m_ir_8 = 0.0f;
	m_ir_9 = 0.0f;
	m_convergence = _T("");
	m_hd_1 = 0.0f;
	m_hd_10 = 0.0f;
	m_hd_11 = 0.0f;
	m_hd_2 = 0.0f;
	m_hd_3 = 0.0f;
	m_hd_4 = 0.0f;
	m_hd_5 = 0.0f;
	m_hd_6 = 0.0f;
	m_hd_7 = 0.0f;
	m_hd_8 = 0.0f;
	m_hd_9 = 0.0f;
	m_vsvn_1 = 0.0f;
	m_vsvn_10 = 0.0f;
	m_vsvn_11 = 0.0f;
	m_vsvn_2 = 0.0f;
	m_vsvn_3 = 0.0f;
	m_vsvn_4 = 0.0f;
	m_vsvn_5 = 0.0f;
	m_vsvn_6 = 0.0f;
	m_vsvn_7 = 0.0f;
	m_vsvn_8 = 0.0f;
	m_vsvn_9 = 0.0f;
	m_vsvn_12 = 0.0f;
	m_vp_12 = 0.0f;
	m_vin_12 = 0.0f;
	m_vc_12 = 0.0f;
	m_vac_12 = 0.0f;
	m_time_12 = 0.0f;
	m_qrp_12 = 0.0f;
	m_qpdraw_12 = 0.0f;
	m_qinreturn_12 = 0.0f;
	m_qindraw_12 = 0.0f;
	m_qch_12 = 0.0f;
	m_qcdraw_12 = 0.0f;
	m_qac_12 = 0.0f;
	m_ir_12 = 0.0f;
	m_hd_12 = 0.0f;
	m_hd_13 = 0.0f;
	m_ir_13 = 0.0f;
	m_qac_13 = 0.0f;
	m_qcdraw_13 = 0.0f;
	m_qch_13 = 0.0f;
	m_qindraw_13 = 0.0f;
	m_qinreturn_13 = 0.0f;
	m_qpdraw_13 = 0.0f;
	m_qrp_13 = 0.0f;
	m_time_13 = 0.0f;
	m_vac_13 = 0.0f;
	m_vc_13 = 0.0f;
	m_vin_13 = 0.0f;
	m_vp_13 = 0.0f;
	m_vsvn_13 = 0.0f;
	m_substate_2 = _T("");
	m_substate_3 = _T("");
	m_substate_4 = _T("");
	m_substate_5 = _T("");
	m_configured_plasma = _T("");
	//}}AFX_DATA_INIT
}


void CExtendedResults::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtendedResults)
	DDX_Text(pDX, IDC_ACTUAL_CONCENTRATION, m_actual_concentration);
	DDX_Text(pDX, IDC_ACTUAL_PLASMA, m_actual_plasma);
	DDX_Text(pDX, IDC_ACTUAL_PLATELETS, m_actual_platelets);
	DDX_Text(pDX, IDC_ACTUAL_RBCS, m_actual_rbcs);
	DDX_Text(pDX, IDC_ACTUAL_YIELD, m_actual_yield);
	DDX_Text(pDX, IDC_CASSETTE, m_cassette);
	DDX_Text(pDX, IDC_CONFIGURED_CONCENTRATION, m_configured_concentration);
	DDX_Text(pDX, IDC_CONFIGURED_PLATELETS, m_configured_platelets);
	DDX_Text(pDX, IDC_CONFIGURED_YIELD, m_configured_yield);
	DDX_Text(pDX, IDC_REMAINING_VOLUME, m_remaining_volume);
	DDX_Text(pDX, IDC_ACTUAL_MAX_TIME, m_actual_max_time);
	DDX_Text(pDX, IDC_CONFIGURED_MAX_TIME, m_configured_max_time);
	DDX_Text(pDX, IDC_RESULTS, m_results);
	DDX_Text(pDX, IDC_ACTUAL_REPLACEMENT_VOLUME, m_actual_replacement_volume);
	DDX_Text(pDX, IDC_CONFIGURED_REPLACEMENT_VOLUME, m_configured_replacement_volume);
	DDX_Text(pDX, IDC_CONFIGURED_RBCS, m_configured_rbcs);
	DDX_Text(pDX, IDC_VOLUME_FROM_DONOR, m_volume_from_donor);
   DDX_Text(pDX, IDC_TOTAL_ITERATIONS, m_total_iterations);
	DDX_Text(pDX, IDC_CONVERGENCE, m_convergence);
	DDX_Text(pDX, IDC_CONFIGURED_PLASMA, m_configured_plasma);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExtendedResults, CDialog)
	//{{AFX_MSG_MAP(CExtendedResults)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtendedResults::initialize(const int product,
                                  const CString &status,
                                  const float configured_yield,
                                  const float configured_platelet_volume,
                                  const float configured_plasma_volume,
                                  const float configured_rbc_dose,
                                  const float configured_rbc_crit,
                                  const float rbc_scaling_factor,
                                  const int product_blood_type,
                                  const int donor_blood_type,
                                  const float configured_max_time,
                                  const float replacement_percent,
                                  procedureDetail &results,
                                  Predict *parent)
{
	CString title;
	title.LoadString(IDS_AP2_PREDICTIONATER);
	CString work;
	work.LoadString(IDS_EXTENDED_RESULTS);
	title += work;
	work.LoadString(IDS_PRODUCT);
	title += work;
	char product_string[3];
	itoa(product,product_string,10);
	title += product_string;
	SetWindowText(title);

   m_results = status;
   m_configured_concentration = 0.0f;

   if (configured_platelet_volume > 0.0f) 
      m_configured_concentration = configured_yield*100000.0f/configured_platelet_volume;

   m_configured_plasma = results.toplevel.Vptarg;
   m_configured_plasma.Format("(%d) %d", int(configured_plasma_volume), int(results.toplevel.Vptarg));

   m_configured_platelets = configured_platelet_volume;
   m_configured_yield = configured_yield;
   
   m_configured_rbcs.Empty();
   m_configured_rbcs.Format("%d@%d@%.2f=%.2f",
      int(configured_rbc_dose),
      int(configured_rbc_crit),
      rbc_scaling_factor,
      ((configured_rbc_crit > 0.0f) ? (100.0f * configured_rbc_dose / configured_rbc_crit / rbc_scaling_factor) : 0.0f));
   
   m_configured_max_time = configured_max_time;

   m_actual_max_time = results._PredTp;
   m_total_iterations = results.getnumOfTimes();
   m_convergence.Empty();
   m_remaining_volume = results.hypovolemiaDistance();

   m_cassette.Empty();

   m_actual_plasma = results._PredVp;
   m_actual_platelets = results._PredVc;
   m_actual_rbcs = results.getPredRBCVolume();

   m_actual_yield = results.toplevel.Ytarg;
   m_actual_concentration = (results._PredVc > 0.0f) ? (100000.0f*m_actual_yield/results._PredVc) : 0.0f;

   m_volume_from_donor = 
         m_actual_platelets + m_actual_rbcs + m_actual_plasma
         - results._PredVac - results._PredVrep;
   m_actual_replacement_volume = results._PredVrep;
   m_configured_replacement_volume.Empty();
   
   if (m_actual_replacement_volume > 0.0f) 
   {
      m_configured_replacement_volume.Format("%.0f%%",replacement_percent);
   }
   else
   {
      m_configured_replacement_volume = "NO";
   }

   int numOfGridRows;
   int gridHeight;

   if (results.getRecommendedCassette() == (int)cassette::PLTPLSRBC)
      numOfGridRows = __GRID_NUM_ROWS_PLTPLSRBC__;
   else
      numOfGridRows = __GRID_NUM_ROWS_PLSRBC__;

   gridHeight = numOfGridRows * __GRID_ROW_HEIGHT__ + 5;


	// Create a Grid
	m_Grid = new CGridCtrl();
	m_Grid->Create(CRect (__GRID_X__,__GRID_Y__,__GRID_X__+__GRID_WIDTH__,__GRID_Y__ + gridHeight), this, IDC_GRID_1);
	
	// Allocate Grid Resources
	try {
		m_Grid->SetRowCount(numOfGridRows);
		m_Grid->SetFixedRowCount(1);
		m_Grid->SetColumnCount(__GRID_NUM_COLS__);
		m_Grid->SetFixedColumnCount(1);
    }
	catch (CMemoryException* e)
	{
		TRACE0 ("Error in grid!");
        e->ReportError();
        e->Delete();
        exit(1);
    }

	// Set Grid Features and stuff
	m_Grid->SetEditable(false);
	m_Grid->EnableSelection(false);
	m_Grid->EnableDragAndDrop(false);
	m_Grid->SetRowResize(true);
	m_Grid->SetColumnResize(true);
	m_Grid->SetHandleTabKey(false);
	m_Grid->SetTrackFocusCell(false);
	m_Grid->SetFrameFocusCell(false);

	// Set up an array of grid data
	GV_ITEM item[__GRID_NUM_ROWS_PLTPLSRBC__][__GRID_NUM_COLS__];
	for (int row = 0; row < m_Grid->GetRowCount(); row++)
	{
		for (int col = 0; col <m_Grid->GetColumnCount(); col++)
		{
			item[row][col].row = row;
			item[row][col].col = col;
			item[row][col].mask = GVIF_TEXT;
		}
	}

	// Format Grid Rows
	{
	for (int row = 0; row < m_Grid->GetRowCount(); row++)
		m_Grid->SetRowHeight (row,__GRID_ROW_HEIGHT__);
	}
	

	// Format Grid Columns
	m_Grid->SetColumnWidth (0,__GRID_TITLE_COL_WIDTH__);
	for (int col = 1; col <m_Grid->GetColumnCount(); col++)
		m_Grid->SetColumnWidth (col,__GRID_COL_WIDTH__);
    
	// Set up static row labels
	item[1][0].strText.Format(_T("Channel Setup (1)"));

	// Set up static column labels
	item[0][1].strText.Format(_T("Time"));
	item[0][2].strText.Format(_T("Vin"));
	item[0][3].strText.Format(_T("Vac"));
	item[0][4].strText.Format(_T("QinD"));
	item[0][5].strText.Format(_T("QinR"));
	item[0][6].strText.Format(_T("Qp"));
	item[0][7].strText.Format(_T("Qc"));
	item[0][8].strText.Format(_T("Qac"));
	item[0][9].strText.Format(_T("Qrp"));
	item[0][10].strText.Format(_T("Qch"));
	item[0][11].strText.Format(_T("Vp"));
	item[0][12].strText.Format(_T("Vc"));
	item[0][13].strText.Format(_T("IR"));
	item[0][14].strText.Format(_T("HD"));
	item[0][15].strText.Format(_T("Vsvn"));
	item[0][16].strText.Format(_T("D"));

	item[0][17].strText.Format(_T("Vr"));
	item[0][18].strText.Format(_T("Hin"));
	item[0][19].strText.Format(_T("Hend"));
	item[0][20].strText.Format(_T("Qrbc"));

	// Set up dynamic row labels
   if (results.getRecommendedCassette() == (int)cassette::PLTPLSRBC)
   {
     m_cassette.LoadString(IDS_WHITE);
  	  item[2][0].strText.Format("Preplt Plasma (2)");
	  item[3][0].strText.Format("Preplt (3)");
	  item[4][0].strText.Format("Platelet Plasma (4)");
	  item[5][0].strText.Format("PCA (5)");
     item[6][0].strText.Format(_T("Midrun (6)"));
     item[7][0].strText.Format(_T("PIR+Plasma (7)"));
     item[8][0].strText.Format(_T("PIR (8)"));
     item[9][0].strText.Format(_T("Extended PCA (9)"));
     item[10][0].strText.Format(_T("Extended Plasma (10)"));
     item[11][0].strText.Format(_T("RBC Setup 1 (11)"));
     item[12][0].strText.Format(_T("RBC Setup 2 (12)"));
     item[13][0].strText.Format(_T("RBC Prime  (13)"));
     item[14][0].strText.Format(_T("RBC Collect (14)"));
   }
   else if (results.getRecommendedCassette() == (int)cassette::RBCPLS)
   {
     m_cassette.LoadString(IDS_BLACK);
	  item[2][0].strText.Format(IDS_RP_PLASMA_SUBSTATE);
	  item[3][0].strText.Format(IDS_RP_RBC_SETUP_1_SUBSTATE);
	  item[4][0].strText.Format(IDS_RP_RBC_SETUP_2_SUBSTATE);
	  item[5][0].strText.Format(IDS_RP_RBC_SUBSTATE);
	  item[6][0].strText.Format(IDS_RP_RBC_SETUP_1_SUBSTATE);
	  item[7][0].strText.Format(IDS_RP_RBC_SETUP_2_SUBSTATE);
	  item[8][0].strText.Format(IDS_RP_RBC_SUBSTATE);

   }
   else 
   {
      m_cassette.LoadString(IDS_UNKNOWN);
   }

// Macro for inserting a row of data
#define substatedata(index) item[index][1].strText.Format(_T("%.4f"), results.substate[index].T);\
	item[index][2].strText.Format("%.0f", results.substate[index].EndVin);\
	item[index][3].strText.Format(_T("%.1f"), results.substate[index].AccumVac);\
	item[index][4].strText.Format(_T("%.1f"), results.substate[index].Qininstd);\
	item[index][5].strText.Format(_T("%.1f"), results.substate[index].Qininstr);\
	item[index][6].strText.Format(_T("%.1f"), results.substate[index].Qpinstd);\
	item[index][7].strText.Format(_T("%.1f"), results.substate[index].Qcinstd);\
	item[index][8].strText.Format(_T("%.1f"), results.substate[index].Qacinst);\
	item[index][9].strText.Format(_T("%.1f"), results.substate[index].Qrinst);\
	item[index][10].strText.Format(_T("%.1f"), results.substate[index].Qch);\
	item[index][11].strText.Format(_T("%.1f"), results.substate[index].AccumVp);\
	item[index][12].strText.Format(_T("%.1f"), results.substate[index].AccumVc);\
	item[index][13].strText.Format(_T("%.3f"), results.instantaneousInfusion(parent,index));\
	item[index][14].strText.Format(_T("%.1f"), results.substate[index].HypovolemiaDistance);\
	item[index][15].strText.Format(_T("%.1f"), results.substate[index].Vsvn);\
	item[index][16].strText.Format(_T("%.2f"), results.substate[index].D);   \
    item[index][17].strText.Format(_T("%.2f"), results.substate[index].Vr); \
    item[index][18].strText.Format(_T("%.2f"), results.substate[index].Hin); \
    item[index][19].strText.Format(_T("%.2f"), results.substate[index].Hend); \
    item[index][20].strText.Format(_T("%.2f"), (  ( testIfNeedQrbc(index,results) )?(results.substate[index].Qininstd - results.substate[index].Qpinstd):0.0f ));
   
   
   // End of macro

// Calls to the above macro
   substatedata(1);
   substatedata(2);
   substatedata(3);
   substatedata(4);
   substatedata(5);
   substatedata(6);
   substatedata(7);
   substatedata(8);


   if (results.getRecommendedCassette() == (int)cassette::PLTPLSRBC)
   {
      substatedata(4);
      substatedata(5);
      substatedata(6);
      substatedata(7);
      substatedata(8);
      substatedata(9);
      substatedata(10);
      substatedata(11);
      substatedata(12);
      substatedata(13);
      substatedata(14);

    }

   // Put the data into the grid.
   {	
   for (int row = 0; row < m_Grid->GetRowCount(); row++)
	{
		for (int col = 0; col <m_Grid->GetColumnCount(); col++)
		{
			m_Grid->SetItem(&item[row][col]);
		}
	}
   }

   //m_Grid->
   
   UpdateData(0);
}

bool 
CExtendedResults::testIfNeedQrbc(int index,procedureDetail &results )
{
    if (   (results.getRecommendedCassette() == (int)cassette::RBCPLS)
        && (index == 5 || index == 8) )
        return true;
    else
        return  false;


}
/////////////////////////////////////////////////////////////////////////////
// CExtendedResults message handlers
