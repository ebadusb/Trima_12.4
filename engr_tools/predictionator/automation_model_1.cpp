// automation_model_1.cpp : implementation file
//

#include "stdafx.h"
#include "ap2 predictionater.h"
#include "automation_model_1.h"
#include "cfg_main_input.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CAP2predictionaterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// Cautomation_model_1

IMPLEMENT_DYNCREATE(Cautomation_model_1, CFormView)

Cautomation_model_1::Cautomation_model_1() :
	optimizer_form(Cautomation_model_1::IDD),

   m_iDonorCount(0),
   m_iPredictionsRequired(0),
   m_iPredictionsCompleted(0),
   m_fPercentPredictionsComplete(0.0f),
   m_iAutoPltNoLoss_unscaled(0),
   m_iAutoFFPNoLoss_unscaled(0),
   m_iAutoRBCNoLoss_unscaled(0),
   m_iProductsNotCounted(0),
   m_single_yield(0.0f),
   m_plasma_small(0.0f),
   m_rbc_collect_volume(0.0f),
   m_DonorDB()
{
	//{{AFX_DATA_INIT(Cautomation_model_1)
	m_iCollSimCount = 0;
	m_iAutoSelection_1 = 0;
	m_iAutoSelection_10 = 0;
	m_iAutoSelection_11 = 0;
	m_iAutoSelection_12 = 0;
	m_iAutoSelection_13 = 0;
	m_iAutoSelection_14 = 0;
	m_iAutoSelection_15 = 0;
	m_iAutoSelection_2 = 0;
	m_iAutoSelection_3 = 0;
	m_iAutoSelection_4 = 0;
	m_iAutoSelection_5 = 0;
	m_iAutoSelection_6 = 0;
	m_iAutoSelection_7 = 0;
	m_iAutoSelection_8 = 0;
	m_iAutoSelection_9 = 0;
	m_iAutoSelection_1_filtered = 0;
	m_iAutoSelection_10_filtered = 0;
	m_iAutoSelection_11_filtered = 0;
	m_iAutoSelection_12_filtered = 0;
	m_iAutoSelection_13_filtered = 0;
	m_iAutoSelection_14_filtered = 0;
	m_iAutoSelection_15_filtered = 0;
	m_iAutoSelection_2_filtered = 0;
	m_iAutoSelection_3_filtered = 0;
	m_iAutoSelection_4_filtered = 0;
	m_iAutoSelection_5_filtered = 0;
	m_iAutoSelection_6_filtered = 0;
	m_iAutoSelection_7_filtered = 0;
	m_iAutoSelection_8_filtered = 0;
	m_iAutoSelection_9_filtered = 0;
	m_iAutoFFPNoLoss = 0;
	m_iAutoPltNoLoss = 0;
	m_iAutoRBCNoLoss = 0;
	m_donor_db = _T("donors.txt");
	//}}AFX_DATA_INIT
}

Cautomation_model_1::~Cautomation_model_1()
{  
}

void Cautomation_model_1::DoDataExchange(CDataExchange* pDX)
{
	optimizer_form::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cautomation_model_1)
	DDX_Control(pDX, IDC_EDIT_CONFIG, m_edit_config_control);
	DDX_Control(pDX, IDC_STOP_SIM_PB, m_stop_sim_pb_control);
	DDX_Control(pDX, IDC_MANU_SIM_PB, m_manu_sim_pb_control);
	DDX_Control(pDX, IDC_OPT_PROGRESS, m_ctrlOptimizationProgress);
	DDX_Text(pDX, IDC_COLLCOUNT_EDIT, m_iCollSimCount);
	DDX_Text(pDX, IDC_AUTO_CHOICE_1_EDIT, m_iAutoSelection_1);
	DDX_Text(pDX, IDC_AUTO_CHOICE_10_EDIT, m_iAutoSelection_10);
	DDX_Text(pDX, IDC_AUTO_CHOICE_11_EDIT, m_iAutoSelection_11);
	DDX_Text(pDX, IDC_AUTO_CHOICE_12_EDIT, m_iAutoSelection_12);
	DDX_Text(pDX, IDC_AUTO_CHOICE_13_EDIT, m_iAutoSelection_13);
	DDX_Text(pDX, IDC_AUTO_CHOICE_14_EDIT, m_iAutoSelection_14);
	DDX_Text(pDX, IDC_AUTO_CHOICE_15_EDIT, m_iAutoSelection_15);
	DDX_Text(pDX, IDC_AUTO_CHOICE_2_EDIT, m_iAutoSelection_2);
	DDX_Text(pDX, IDC_AUTO_CHOICE_3_EDIT, m_iAutoSelection_3);
	DDX_Text(pDX, IDC_AUTO_CHOICE_4_EDIT, m_iAutoSelection_4);
	DDX_Text(pDX, IDC_AUTO_CHOICE_5_EDIT, m_iAutoSelection_5);
	DDX_Text(pDX, IDC_AUTO_CHOICE_6_EDIT, m_iAutoSelection_6);
	DDX_Text(pDX, IDC_AUTO_CHOICE_7_EDIT, m_iAutoSelection_7);
	DDX_Text(pDX, IDC_AUTO_CHOICE_8_EDIT, m_iAutoSelection_8);
	DDX_Text(pDX, IDC_AUTO_CHOICE_9_EDIT, m_iAutoSelection_9);
	DDX_Text(pDX, IDC_AUTO_CHOICE_1_EDIT2, m_iAutoSelection_1_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_10_EDIT2, m_iAutoSelection_10_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_11_EDIT2, m_iAutoSelection_11_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_12_EDIT2, m_iAutoSelection_12_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_13_EDIT2, m_iAutoSelection_13_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_14_EDIT2, m_iAutoSelection_14_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_15_EDIT2, m_iAutoSelection_15_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_2_EDIT2, m_iAutoSelection_2_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_3_EDIT2, m_iAutoSelection_3_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_4_EDIT2, m_iAutoSelection_4_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_5_EDIT2, m_iAutoSelection_5_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_6_EDIT2, m_iAutoSelection_6_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_7_EDIT2, m_iAutoSelection_7_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_8_EDIT2, m_iAutoSelection_8_filtered);
	DDX_Text(pDX, IDC_AUTO_CHOICE_9_EDIT2, m_iAutoSelection_9_filtered);
	DDX_Text(pDX, IDC_AUTO_FFP_NOLOSS_EDIT, m_iAutoFFPNoLoss);
	DDX_Text(pDX, IDC_AUTO_PLT_NOLOSS_EDIT, m_iAutoPltNoLoss);
	DDX_Text(pDX, IDC_AUTO_RBC_NOLOSS_EDIT, m_iAutoRBCNoLoss);
	DDX_Text(pDX, IDC_DONOR_DB, m_donor_db);
	//}}AFX_DATA_MAP
   //

}


BEGIN_MESSAGE_MAP(Cautomation_model_1, CFormView)
	//{{AFX_MSG_MAP(Cautomation_model_1)
	ON_BN_CLICKED(IDC_MANU_SIM_PB, OnManuSimPb)
	ON_BN_CLICKED(IDC_STOP_SIM_PB, OnStopSimPb)
	ON_BN_CLICKED(IDC_EDIT_CONFIG, OnEditConfig)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cautomation_model_1 diagnostics

#ifdef _DEBUG
void Cautomation_model_1::AssertValid() const
{
	CFormView::AssertValid();
}

void Cautomation_model_1::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Cautomation_model_1 message handlers

void Cautomation_model_1::OnManuSimPb() 
{
   // Run a Manufacturing Simulation
	//

	// get data from form into members
	UpdateData( TRUE );

   //
   //
   //  Disable controls
   enable_controls(0);

   optimizer_form::OnStart();
	

   // dummied for today, only have one donor db file.
   // call here to reset fp to top of stream
   m_iDonorCount = m_DonorDB.Initialize( m_donor_db );
   m_iCollSimCount = m_iDonorCount;

   m_iPredictionsRequired        = m_iDonorCount * cfg_main_input::userconfig->ProcedureList.numberOfProcedures.Get();
   m_iPredictionsCompleted       = 0;
   m_fPercentPredictionsComplete = 0.f;
	m_ctrlOptimizationProgress.SetPos( (int)0 );

   Reset();


   m_single_yield = config_dat().m_plt1_yield;
   m_plasma_small = config_dat().m_plasma1;
   m_rbc_collect_volume = config_dat().m_rbc_volume_1;

   m_simulation_active = 1;

}

void Cautomation_model_1::TestPredictionator()
{

   if (!m_simulation_active) return;

   float fWeight    = 0.f;
   float fHeight    = 100.0f; // used only in BMI calculations
   float fTBV       = 0.f;
   float fHct       = 0.f;
   float fPrecount  = 0.f;
   BLOODTYPE iBloodType = BT_UNKNOWN;   

   if (m_DonorDB.GetNextDonor( fWeight
                              , fTBV
                              , fHct
                              , fPrecount
                              , iBloodType )  )
   {
    
      m_optimizer.donor(fHeight,
                        fWeight,
                        fTBV,
                        fHct/100.0f,
                        fHct/100.0f,
                        fPrecount,
                        iBloodType ); 

      m_iProductsNotCounted = TRUE;

      INTERNAL_PREDICT_RESULTS status;
 
      m_optimizer.initialize();
      m_optimizer.optimize();
      for (int i=1; i<cfg_main_input::userconfig->ProcedureList.numberOfProcedures.Get(); i++) 
      {
        status = m_optimizer.getProduct(i)->getStatus();

        if (status == GOOD_PRODUCT) 
        {
		    // bump achieved counter for this product
		    // 
		    UpdateAchievedCount( i );
		    if( m_iProductsNotCounted )
		    {
			   UpdateProductCount( i );
            UpdateFilteredCount( i );
			   m_iProductsNotCounted = FALSE;
		    }
 	    }  

	    // think about updating the progress bar
	    // use thed defaults where range is 0 : 100
	    // step 10.
	    // each prediction completed represents 
	    // x% completetion where 100% complete = donor count * total products per donor
	    //
	    // 
	    ++m_iPredictionsCompleted;
	    m_fPercentPredictionsComplete = (float)m_iPredictionsCompleted / (float)m_iPredictionsRequired * 100.f;
	    
	    // bad use of CPU ....
	    // update bar each time ( should only do when m_fPercentComplete exceeds current bar 
	    // position ( in pcnt complete ) by 10 or more ow bar does not move .
	    m_ctrlOptimizationProgress.SetPos( (int)m_fPercentPredictionsComplete );
      }
    } else { // we're done
      OnStopSimPb();
    }
}

void Cautomation_model_1::OnInitialUpdate() 
{
	optimizer_form::OnInitialUpdate();
   //
   //
   //   Set up processing with main application.
   //
   set_processing_function(this,(IdleFunction::idle_loop_function)&Cautomation_model_1::TestPredictionator);


   //
   //
   //   Enable controls (actually here the intent is to disable the 'stop' button)
   //
   enable_controls(1);
}

void Cautomation_model_1::Reset( )
{
   m_iAutoSelection_1  = 0; 
   m_iAutoSelection_3  = 0; 
   m_iAutoSelection_2  = 0; 
   m_iAutoSelection_4  = 0; 
   m_iAutoSelection_6  = 0; 
   m_iAutoSelection_7  = 0; 
   m_iAutoSelection_8  = 0; 
   m_iAutoSelection_9  = 0; 
   m_iAutoSelection_10 = 0; 
   m_iAutoSelection_11 = 0; 
   m_iAutoSelection_5  = 0; 
   m_iAutoSelection_12 = 0; 
   m_iAutoSelection_13 = 0; 
   m_iAutoSelection_14 = 0; 
   m_iAutoSelection_15 = 0; 

   m_iAutoSelection_1_filtered  = 0; 
   m_iAutoSelection_2_filtered  = 0; 
   m_iAutoSelection_3_filtered  = 0; 
   m_iAutoSelection_4_filtered  = 0; 
   m_iAutoSelection_5_filtered  = 0; 
   m_iAutoSelection_6_filtered  = 0; 
   m_iAutoSelection_7_filtered  = 0; 
   m_iAutoSelection_8_filtered  = 0; 
   m_iAutoSelection_9_filtered  = 0; 
   m_iAutoSelection_10_filtered = 0; 
   m_iAutoSelection_11_filtered = 0; 
   m_iAutoSelection_12_filtered = 0; 
   m_iAutoSelection_13_filtered = 0; 
   m_iAutoSelection_14_filtered = 0; 
   m_iAutoSelection_15_filtered = 0; 

   m_iAutoPltNoLoss = 0;
   m_iAutoFFPNoLoss = 0;
   m_iAutoRBCNoLoss = 0;
   m_iAutoPltNoLoss_unscaled = 0;
   m_iAutoFFPNoLoss_unscaled = 0;
   m_iAutoRBCNoLoss_unscaled = 0;

}

void Cautomation_model_1::UpdateAchievedCount( int i )
{
	// chitty implementation for this action due to 
	// lack of knowledge about how to have the member vbls be an
	// array for the edit controls ....
	switch( i )
	{
	case 0:
		++m_iAutoSelection_1;
		break;
	case 1:
		++m_iAutoSelection_2;
		break;
	case 2:
		++m_iAutoSelection_3;
		break;
	case 3:
		++m_iAutoSelection_4;
		break;
	case 4:
		++m_iAutoSelection_5;
		break;
	case 5:
		++m_iAutoSelection_6;
		break;
	case 6:
		++m_iAutoSelection_7;
		break;
	case 7:
		++m_iAutoSelection_8;
		break;
	case 8:
		++m_iAutoSelection_9;
		break;
	case 9:
		++m_iAutoSelection_10;
		break;
	case 10:
		++m_iAutoSelection_11;
		break;
	case 11:
		++m_iAutoSelection_12;
		break;
	case 12:
		++m_iAutoSelection_13;
		break;
	case 13:
		++m_iAutoSelection_14;
		break;
	case 14:
		++m_iAutoSelection_15;
		break;
	}
}
void Cautomation_model_1::UpdateFilteredCount( int i )
{
	// chitty implementation for this action due to 
	// lack of knowledge about how to have the member vbls be an
	// array for the edit controls ....
	switch( i )
	{
	case 0:
		++m_iAutoSelection_1_filtered;
		break;
	case 1:
		++m_iAutoSelection_2_filtered;
		break;
	case 2:
		++m_iAutoSelection_3_filtered;
		break;
	case 3:
		++m_iAutoSelection_4_filtered;
		break;
	case 4:
		++m_iAutoSelection_5_filtered;
		break;
	case 5:
		++m_iAutoSelection_6_filtered;
		break;
	case 6:
		++m_iAutoSelection_7_filtered;
		break;
	case 7:
		++m_iAutoSelection_8_filtered;
		break;
	case 8:
		++m_iAutoSelection_9_filtered;
		break;
	case 9:
		++m_iAutoSelection_10_filtered;
		break;
	case 10:
		++m_iAutoSelection_11_filtered;
		break;
	case 11:
		++m_iAutoSelection_12_filtered;
		break;
	case 12:
		++m_iAutoSelection_13_filtered;
		break;
	case 13:
		++m_iAutoSelection_14_filtered;
		break;
	case 14:
		++m_iAutoSelection_15_filtered;
		break;
	}
}

void Cautomation_model_1::UpdateProductCount(int index)
{
   procedureDetail *proc = m_optimizer.getProduct(index);
   //
   //
   //   Note that the yield needs to be fudged slightly to account for
   //   rounding with large (E11) scale numbers
   //
   float yld = (proc->toplevel.Ytarg/1E11f) + 0.1f;
   m_iAutoPltNoLoss_unscaled += (int)(yld / m_single_yield);
   m_iAutoFFPNoLoss_unscaled += (int)(proc->toplevel.Vptarg / m_plasma_small);
   m_iAutoRBCNoLoss_unscaled += (int)(proc->getProcRBCVolume() / m_rbc_collect_volume);

   //
   //
   //   Update simulated totals
   //
   m_iAutoPltNoLoss = m_iAutoPltNoLoss_unscaled;
   m_iAutoFFPNoLoss = m_iAutoFFPNoLoss_unscaled;
   m_iAutoRBCNoLoss = m_iAutoRBCNoLoss_unscaled;

   UpdateData( FALSE );
}


void Cautomation_model_1::OnFinalRelease() 
{
	optimizer_form::OnFinalRelease();
}


//
//
// ----------------------------------------------------------------------------
//                    Enable / disable controls when simulation running
void Cautomation_model_1::enable_controls(const int enable) 
{
   m_manu_sim_pb_control.EnableWindow(enable);
   m_edit_config_control.EnableWindow(enable);

   //
   //
   //   These members are "reversed"
   //
   m_stop_sim_pb_control.EnableWindow(!enable);
}

void Cautomation_model_1::OnStopSimPb() 
{
   optimizer_form::OnStop();
   enable_controls(1);
   UpdateData( FALSE );
}

void Cautomation_model_1::OnEditConfig() 
{
	optimizer_form::OnEditConfig();	
}
