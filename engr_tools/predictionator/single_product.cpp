// single_product.cpp : implementation file
//

#include "stdafx.h"
#include "ap2 predictionater.h"
#include "single_product.h"
#include "cfg_main_input.h"
#include "initial_conditions.h"
#include "ExtendedResults.h"
#include <sstream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CAP2predictionaterApp theApp;

static CListBox *__results_list = 0;
static int __save_intermediate_results=0;

/////////////////////////////////////////////////////////////////////////////
// single_product

IMPLEMENT_DYNCREATE(single_product, CFormView)

single_product::single_product() :
	optimizer_form(single_product::IDD),
   m_donorBloodType(BT_UNKNOWN)
{
	EnableAutomation();
	//{{AFX_DATA_INIT(single_product)
	m_results_string = _T("");
	m_save_path = _T("");
	m_intermediate_results = FALSE;
	m_calculated_tbv = 0.0f;
	m_donor_height = 0;
	m_donor_hematocrit = 0;
	m_donor_precount = 0;
	m_donor_weight = 0;
	m_height_units = _T("");
	m_weight_units = _T("");
	m_female = FALSE;
	m_male = TRUE;
	//}}AFX_DATA_INIT
}

single_product::~single_product()
{
}

void single_product::OnFinalRelease()
{
	optimizer_form::OnFinalRelease();
}

void single_product::DoDataExchange(CDataExchange* pDX)
{
	optimizer_form::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(single_product)
	DDX_Control(pDX, IDC_MALE, m_male_control);
	DDX_Control(pDX, IDC_FEMALE, m_female_control);
	DDX_Control(pDX, IDC_DONOR_BLOOD_TYPE, m_donor_blood_type_control);
	DDX_Control(pDX, IDC_RESULTS_LIST, m_results_list_control);
	DDX_Control(pDX, IDC_OPTIMIZE, m_optimize_button);
	DDX_Text(pDX, IDC_RESULTS, m_results_string);
	DDX_Text(pDX, IDC_SAVE_PATH, m_save_path);
	DDX_Check(pDX, IDC_INTERMEDIATE_RESULTS, m_intermediate_results);
	DDX_Text(pDX, IDC_CALCULATED_TBV, m_calculated_tbv);
	DDX_Text(pDX, IDC_DONOR_HEIGHT, m_donor_height);
	DDX_Text(pDX, IDC_DONOR_HEMATOCRIT, m_donor_hematocrit);
	DDX_Text(pDX, IDC_DONOR_PRECOUNT, m_donor_precount);
	DDX_Text(pDX, IDC_DONOR_WEIGHT, m_donor_weight);
	DDX_Text(pDX, IDC_HEIGHT_UNITS, m_height_units);
	DDX_Text(pDX, IDC_WEIGHT_UNITS, m_weight_units);
	DDX_Check(pDX, IDC_FEMALE, m_female);
	DDX_Check(pDX, IDC_MALE, m_male);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(single_product, CFormView)
	//{{AFX_MSG_MAP(single_product)
	ON_BN_CLICKED(IDC_OPTIMIZE, OnOptimize)
	ON_BN_CLICKED(IDC_CONFIG, OnConfig)
	ON_BN_CLICKED(IDC_SAVE_DATA_TO, OnSaveDataTo)
	ON_BN_CLICKED(IDC_INITIAL_CONDITIONS, OnInitialConditions)
	ON_BN_CLICKED(IDC_MALE, OnMale)
	ON_BN_CLICKED(IDC_FEMALE, OnFemale)
	ON_EN_CHANGE(IDC_DONOR_HEIGHT, OnDonorHeight)
	ON_EN_CHANGE(IDC_DONOR_WEIGHT, OnDonorWeight)
	ON_EN_KILLFOCUS(IDC_DONOR_HEIGHT, OnDonorHeight)
	ON_EN_UPDATE(IDC_DONOR_HEIGHT, OnDonorHeight)
	ON_EN_KILLFOCUS(IDC_DONOR_WEIGHT, OnDonorWeight)
	ON_EN_UPDATE(IDC_DONOR_WEIGHT, OnDonorWeight)
	ON_BN_CLICKED(IDC_EXTENDED_RESULTS, OnExtendedResults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(single_product, CFormView)
	//{{AFX_DISPATCH_MAP(single_product)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_Isingle_product to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {BA8C3793-3607-11D2-80EF-006097402E62}
static const IID IID_Isingle_product =
{ 0xba8c3793, 0x3607, 0x11d2, { 0x80, 0xef, 0x0, 0x60, 0x97, 0x40, 0x2e, 0x62 } };

BEGIN_INTERFACE_MAP(single_product, CFormView)
	INTERFACE_PART(single_product, IID_Isingle_product, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// single_product diagnostics

#ifdef _DEBUG
void single_product::AssertValid() const
{
	optimizer_form::AssertValid();
}

void single_product::Dump(CDumpContext& dc) const
{
	optimizer_form::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// single_product message handlers

void single_product::OnOptimize() 
{
   static const int MAX_STR_MSG_LENGTH = 512;
   static char strstreamWork[MAX_STR_MSG_LENGTH];
   static strstream string(strstreamWork,MAX_STR_MSG_LENGTH,std::ios::in|std::ios::out);

   optimizer_form::OnStart();
   m_optimize_button.EnableWindow(0);
   UpdateData(TRUE);

   //
   //
   //   Open and lock the file.
   //
   std::fstream outfile(m_save_path,std::ios::out);
   if (outfile.fail()) {
      AfxMessageBox(IDS_UNABLE_TO_OPEN_FILE);
      m_optimize_button.EnableWindow(1);
      return;
   }

   //
   //
   //  Set up debug processing with the optimizer
   //
   m_results_list_control.ResetContent();
   __save_intermediate_results = m_intermediate_results;
   __results_list = &m_results_list_control; // for visibility to flows_return
//   m_optimizer.flows_debug(flows_return);

   CString blood_type_string;
   int blood_type_index = m_donor_blood_type_control.GetCurSel();
   m_donor_blood_type_control.GetLBText(blood_type_index,blood_type_string);
   //m_donorBloodType = m_optimizer.convert_blood_type(blood_type_string);
	m_optimizer.donor(float(m_initial_conditions.m_donor_height),
                     float(m_initial_conditions.m_donor_weight),
                     m_initial_conditions.m_calculated_tbv,
                     (float(m_initial_conditions.m_donor_hct)/100.0f), 
                     (float(m_initial_conditions.m_donor_hct)/100.0f),
                     float(m_initial_conditions.m_donor_precount),
                     m_initial_conditions.m_donorBloodType);

   /*m_optimizer.donor(float(m_donor_height),
                     float(m_donor_weight),
                     m_calculated_tbv,
                     (float(m_donor_hematocrit)/100.0f),
                     (float(m_donor_hematocrit)/100.0f),
                     float(m_donor_precount),
                     m_donorBloodType);
	*/

   INTERNAL_PREDICT_RESULTS status;
   CString workstring;
   string.flags(std::ios::showpoint | std::ios::fixed);
   string.precision(2);

   m_optimizer.initialize();
   
   //
   //
   //    Output the results into the edit box.  If the result
   //    is good then give overview details; bad results only
   //    get the failure description.
   //
   m_optimizer.optimize();
   procedureDetail *proc = m_optimizer.getProduct(1);
   status = proc->getStatus();
   string.seekp(0);
   Offline_optimizer::description(status,workstring);
   string << workstring;
   if (status == GOOD_PRODUCT) {
      string << " (plt=" << proc->toplevel.Ytarg/1.0E11f
             << " pl=" << proc->toplevel.Vptarg
             << " rbc=" << proc->getPredRBCVolume()
             << " time=" << proc->toplevel.Tp << ")";
   }
   string << '\0';  // terminate string
   m_results_string = string.str();
   outfile << string.str() << "\n";
   outfile << "time,substate,Qin,Qrp,Qac,Qcol,Qp,DC,Facb,Hrbc,RPM\n";

   int maxindex = m_results_list_control.GetCount();
   for (int strindex=0; strindex<maxindex; strindex++) {
      m_results_list_control.GetText(strindex,workstring);
      outfile << workstring << "\n";
   }
   UpdateData(0);
   m_optimize_button.EnableWindow(1);	

   outfile.close();
   __results_list = 0;
}

/*
void single_product::flows_return(const float &time,
                                  const int &state,
                                  const flow_struct &flows,
                                  void *future_expansion)
{
   static const int MAX_STR_MSG_LENGTH = 512;
   static char strstreamWork[MAX_STR_MSG_LENGTH];
   static strstream string(strstreamWork,MAX_STR_MSG_LENGTH,std::ios::in|std::ios::out);
   static float lasttime = 0.0f;
   if (__results_list) {
      string.seekp(0);
      string.flags(std::ios::showpoint | std::ios:: fixed);
      string.precision(4);
      string << time << ","
             << state << ","
             << flows.Qin << ","
             << flows.Qrp << ","
             << flows.Qac << ","
             << flows.Qcol << ","
             << flows.Qp << ","
             << flows.DC << ","
             << flows.Facb << ","
             << flows.Hrbc << ","
             << flows.RPM;
      string << '\0';  // terminate string
      CString output = string.str();
      if ((!__save_intermediate_results) && (lasttime > time)) {
         __results_list->ResetContent();
      }
      lasttime = time;
      __results_list->AddString(output);
   }
}

*/
void single_product::OnInitialUpdate() 
{
	optimizer_form::OnInitialUpdate();	
   m_donor_blood_type_control.SetCurSel(0);
   UpdateUnits();
   UpdateTBV();
}
void single_product::OnConfig() 
{
	optimizer_form::OnEditConfig();	
    UpdateUnits();
   UpdateTBV();
}

void single_product::OnInitialConditions()
{
   optimizer_form::OnInitialConditions();
}

void single_product::OnSaveDataTo() 
{
   static const CString lpstrFilter = "All files|*.*||";

   UpdateData(1);
   CFileDialog dlg(FALSE,0,0,
      OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
      lpstrFilter);
   int save = dlg.DoModal();
   if (save == IDOK) {
      m_save_path = dlg.GetPathName();
   }
   UpdateData(0);
}

void single_product::UpdateTBV()
{
   UpdateData(TRUE);
   //m_calculated_tbv = m_optimizer.CalculateTBV((float)m_donor_height,cfg_main_input::userconfig->LangUnit.Get().key_height,
   //                                            (float)m_donor_weight,cfg_main_input::userconfig->LangUnit.Get().key_weight,
   //                                            (m_male_control.GetCheck() ? DONOR_SEX_IS_MALE:DONOR_SEX_IS_FEMALE));
   UpdateData(FALSE);
}

void single_product::OnDonorHeight() 
{
   UpdateTBV();
}

void single_product::OnDonorWeight() 
{
   UpdateTBV();
}

void single_product::OnMale() 
{
   m_female_control.SetCheck(!m_male_control.GetCheck());
   UpdateTBV();
}

void single_product::OnFemale() 
{
   m_male_control.SetCheck(!m_female_control.GetCheck());
   UpdateTBV();
}

void single_product::UpdateUnits()
{
   UpdateData(TRUE);
   if (cfg_main_input::userconfig->LangUnit.Get().key_height == FT_HEIGHT) {
      m_height_units = "in";
   } else {
      m_height_units = "cm";
   }

   if (cfg_main_input::userconfig->LangUnit.Get().key_weight == LB_WEIGHT) {
      m_weight_units = "lb";
   } else {
      m_weight_units = "kg";
   }
   UpdateData(FALSE);
}

void single_product::OnExtendedResults() 
{
/*   
   //
   //
   //   Create the popup and send it the results.  It is the responsibility
   //   of the popup to properly interpret the results.
   //
   CExtendedResults *popup = new CExtendedResults(GetParent());
   popup->Create(IDD_EXTENDED_RESULTS,GetParent());
   popup->initialize(0,
      m_results_string,
      PriorityYield(0),
      PriorityPlateletVolume(0),
      PriorityPlasmaVolume(0),
      PriorityRBCDose(0),
      PriorityRBCCrit(0),
      PriorityBloodType(0),
      BLOOD_TYPE_NONE_ENTERED, /// (int)(CONFIG_DATA_LISTBOX_STRINGS)m_donor_input_control.bloodType(),
      PriorityMaxTime(0),
      ReplacementFluidBalance(),
      m_optimizer.results());
	  */
}
