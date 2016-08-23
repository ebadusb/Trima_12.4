// Initial_conditions.cpp : implementation file
//

#include "stdafx.h"

#include <fstream>
#include <string>
#include <shellapi.h>
#include <cmath>
#include "ap2 predictionater.h"
#include "Initial_conditions.h"
#include "states.h"
#include "cassette.h"
#include "DecodeDlog.h"
#include "optimizer_form.h"
#include "cobeconfig.h"
#include "cfg_main_input.h"
#include "vxworks.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Initial_conditions dialog
//
//
//   Filter for rundata filenames
//
static const CString lpstrFilter = "Trima rundata files(*.dat;*.csv;*.dlog)|*.csv,*.dat;*.dlog|All files|*.*||";


Initial_conditions::Initial_conditions(CWnd* pParent /*=NULL*/)
	: CDialog(Initial_conditions::IDD, pParent)
{
	EnableAutomation();

	//{{AFX_DATA_INIT(Initial_conditions)
	m_collect_bag_ac = 0.0f;
	m_collect_bag_volume = 0.0f;
	m_ir_adjustment = 0.0f;
	m_plasma_bag_ac = 0.0f;
	m_plasma_bag_volume = 0.0f;
	m_procedure_run_time = 0.0f;
	m_qac = 0.0f;
	m_qin = 0.0f;
	m_qin_adjustment = 0.0f;
	m_qp = 0.0f;
	m_qrep = 0.0f;
	m_qrp = 0.0f;
	m_qrp_adjustment = 0.0f;
	m_ratio_adjustment = 0.0f;
	m_rbc_bag_ac = 0.0f;
	m_rbc_bag_volume = 0.0f;
	m_rpm = 0.0f;
	m_vac = 0.0f;
	m_vcol = 0.0f;
	m_vin = 0.0f;
	m_vp = 0.0f;
	m_vrep = 0.0f;
	m_vrp = 0.0f;
	m_yield = 0.0f;
	m_qcol = 0.0f;
	m_abstime = 0.0f;
	m_rundata_file = _T("");
	m_rbc_bag_dose = 0.0f;
	m_substate = _T("");
	m_tcv = 0.0f;
	m_cassette = 0;
	m_minimum_qch = 0.0f;
	m_current_qch = 0.0f;
	m_purges = 0;
	m_epca_start = 0.0f;
	m_adjust_random = TRUE;
	m_vincv = 0.0f;
	m_vbolus = 0.0f;
	m_ptf = 0;
	m_ptf_qrbc = 0.0f;
	m_donor_height = 0;
	m_donor_weight = 0;
	m_height_units = _T("");
	m_donor_hct = 0.0f;
	m_donor_precount = 0.0f;
	m_weight_units = _T("");
	m_calculated_tbv = 0.0f;
	m_cobe_config_file = _T("");
	//}}AFX_DATA_INIT
}


void Initial_conditions::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void Initial_conditions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Initial_conditions)
	DDX_Control(pDX, IDC_RADIO_Male, m_ctlRadioMale);
	DDX_Control(pDX, IDC_DONOR_BLOOD_TYPE, m_donor_blood_type_control);
	DDX_Text(pDX, IDC_IC_COLLECT_BAG_AC, m_collect_bag_ac);
	DDX_Text(pDX, IDC_IC_COLLECT_BAG_VOLUME, m_collect_bag_volume);
	DDX_Text(pDX, IDC_IC_IR_ADJUSTMENT, m_ir_adjustment);
	DDX_Text(pDX, IDC_IC_PLASMA_BAG_AC, m_plasma_bag_ac);
	DDX_Text(pDX, IDC_IC_PLASMA_BAG_VOLUME, m_plasma_bag_volume);
	DDX_Text(pDX, IDC_IC_PROCEDURE_RUN_TIME, m_procedure_run_time);
	DDX_Text(pDX, IDC_IC_QAC, m_qac);
	DDX_Text(pDX, IDC_IC_QIN, m_qin);
	DDX_Text(pDX, IDC_IC_QIN_ADJUSTMENT, m_qin_adjustment);
	DDX_Text(pDX, IDC_IC_QP, m_qp);
	DDX_Text(pDX, IDC_IC_QREP, m_qrep);
	DDX_Text(pDX, IDC_IC_QRP, m_qrp);
	DDX_Text(pDX, IDC_IC_QRP_ADJUSTMENT, m_qrp_adjustment);
	DDX_Text(pDX, IDC_IC_RATIO_ADJUSTMENT, m_ratio_adjustment);
	DDX_Text(pDX, IDC_IC_RBC_BAG_AC, m_rbc_bag_ac);
	DDX_Text(pDX, IDC_IC_RBC_BAG_VOLUME, m_rbc_bag_volume);
	DDX_Text(pDX, IDC_IC_RPM, m_rpm);
	DDX_Text(pDX, IDC_IC_VAC, m_vac);
	DDX_Text(pDX, IDC_IC_VCOL, m_vcol);
	DDX_Text(pDX, IDC_IC_VIN, m_vin);
	DDX_Text(pDX, IDC_IC_VP, m_vp);
	DDX_Text(pDX, IDC_IC_VREP, m_vrep);
	DDX_Text(pDX, IDC_IC_VRP, m_vrp);
	DDX_Text(pDX, IDC_IC_YIELD, m_yield);
	DDX_Text(pDX, IDC_IC_QCOL, m_qcol);
	DDX_Text(pDX, IDC_IC_ABSTIME, m_abstime);
	DDX_Text(pDX, IDC_IC_RUNDATA_FILE, m_rundata_file);
	DDX_Text(pDX, IDC_IC_RBC_BAG_DOSE, m_rbc_bag_dose);
	DDX_Text(pDX, IDC_IC_SUBSTATE, m_substate);
	DDX_Text(pDX, IDC_IC_TCV, m_tcv);
	DDX_Text(pDX, IDC_IC_CASSETTE, m_cassette);
	DDX_Text(pDX, IDC_IC_MINIMUM_QCH, m_minimum_qch);
	DDX_Text(pDX, IDC_IC_CURRENT_QCH, m_current_qch);
	DDX_Text(pDX, IDC_IC_PURGES, m_purges);
	DDX_Text(pDX, IDC_IC_EPCA_START, m_epca_start);
	DDX_Check(pDX, IDC_ADJUST_RANDOM, m_adjust_random);
	DDX_Text(pDX, IDC_IC_VINCV, m_vincv);
	DDX_Text(pDX, IDC_IC_VBOLUS, m_vbolus);
	DDX_Text(pDX, IDC_PTF, m_ptf);
	DDX_Text(pDX, IDC_PTF_QRBC, m_ptf_qrbc);
	DDX_Text(pDX, IDC_DONOR_HEIGHT2, m_donor_height);
	DDX_Text(pDX, IDC_DONOR_WEIGHT2, m_donor_weight);
	DDX_Text(pDX, IDC_HEIGHT_UNITS, m_height_units);
	DDX_Text(pDX, IDC_DONOR_HEMATOCRIT2, m_donor_hct);
	DDX_Text(pDX, IDC_DONOR_PRECOUNT2, m_donor_precount);
	DDX_Text(pDX, IDC_WEIGHT_UNITS, m_weight_units);
	DDX_Text(pDX, IDC_CALCULATED_TBV2, m_calculated_tbv);
	DDX_Text(pDX, IDC_IC_COBE_CONFIG_FILE, m_cobe_config_file);
	DDV_MaxChars(pDX, m_cobe_config_file, 512);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Initial_conditions, CDialog)
	//{{AFX_MSG_MAP(Initial_conditions)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_IC_SNARF, OnSnarf)
	ON_BN_CLICKED(IDC_IC_BROWSE, OnIcBrowse)
	ON_BN_CLICKED(IDC_IC_RESET, OnIcReset)
	ON_EN_CHANGE(IDC_DONOR_HEIGHT2, OnChangeDonorHeight)
	ON_EN_CHANGE(IDC_DONOR_WEIGHT2, OnChangeDonorWeight)
	ON_BN_CLICKED(IDC_IC_COBE_CONFIG, OnIcCobeConfig)
	ON_BN_CLICKED(IDC_RADIO_Small, OnRADIOSmall)
	ON_BN_CLICKED(IDC_RADIO_Medium, OnRADIOMedium)
	ON_BN_CLICKED(IDC_RADIO_Large, OnRADIOLarge)
	ON_EN_KILLFOCUS(IDC_DONOR_HEIGHT2, OnChangeDonorHeight)
	ON_EN_UPDATE(IDC_DONOR_HEIGHT2, OnChangeDonorHeight)
	ON_EN_KILLFOCUS(IDC_DONOR_WEIGHT2, OnChangeDonorWeight)
	ON_EN_UPDATE(IDC_DONOR_WEIGHT2, OnChangeDonorWeight)
	ON_BN_CLICKED(IDC_RADIO_Male, OnRADIOMale)
	ON_BN_CLICKED(IDC_RADIO_Female, OnRADIOFemale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(Initial_conditions, CDialog)
	//{{AFX_DISPATCH_MAP(Initial_conditions)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IInitial_conditions to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {5AEA7864-95C5-11D2-81AB-006097402E62}
static const IID IID_IInitial_conditions =
{ 0x5aea7864, 0x95c5, 0x11d2, { 0x81, 0xab, 0x0, 0x60, 0x97, 0x40, 0x2e, 0x62 } };

BEGIN_INTERFACE_MAP(Initial_conditions, CDialog)
	INTERFACE_PART(Initial_conditions, IID_IInitial_conditions, Dispatch)
END_INTERFACE_MAP()

void Initial_conditions::initialize()
{
	m_collect_bag_ac = 0.0f;
	m_collect_bag_volume = 0.0f;
	m_ir_adjustment = 1.2f;
	m_plasma_bag_ac = 0.0f;
	m_plasma_bag_volume = 0.0f;
	m_procedure_run_time = 0.0f;
	m_qac = 0.0f;
	m_qcol = 0;
	m_qin = 0.0f;
	m_qin_adjustment = 142.0f;
	m_qp = 0.0f;
	m_qrep = 0.0f;
	m_qrp = 0.0f;
	m_qrp_adjustment = 302.5f;
	m_ratio_adjustment = 13.7f;
	m_rbc_bag_ac = 0.0f;
	m_rbc_bag_volume = 0.0f;
	m_rpm = 0.0f;
	m_substate.Empty();
	m_vac = 0.0f;
	m_vcol = 0.0f;
	m_vin = 0.0f;
	m_vp = 0.0f;
	m_vrep = 0.0f;
    m_vbolus = 0.0f;
	m_vrp = 0.0f;
	m_yield = 0.0f;
   m_rbc_bag_dose = 0.0f;
   m_tcv = 0.0f;
   m_vincv = 0.0f;
   m_minimum_qch = 5.0f;
   m_cassette = 0;
   m_purges = 0;
   m_epca_start = 0.0f;
   m_ptf = 0;
   m_ptf_qrbc = 90.0f;
   TCHAR szCWD[256];
   //Sriram - Added to keep track of current working directory
   //The 'File Open' dialog changes it.
   ::GetCurrentDirectory(sizeof(szCWD) - 1, szCWD);
   m_CWD = CString(szCWD);
   //Sriram - Choose a medium size donor by default
   m_donor_height = 70;
   m_height_units = "in";
	m_donor_weight = 160;
	m_weight_units = "lb";
	m_donor_hct = 45;
	m_donor_precount = 250;
	m_donor_gender = 1; //male
	//So for simple tests, you don't have to 
	//open this dialog.	
	UpdateTBV(); 
   //Display which cobe_config is loaded by default
   m_cobe_config_file = PATHNAME;
   m_substate_int = 0;
}

void Initial_conditions::populate_datastore() const
{
/*
	Vac_collectBag(m_collect_bag_ac);
	Set_VcollectBag(m_collect_bag_volume);
	SetIRAdjustmentValue(m_ir_adjustment);
	SetPIRProcessedVolume(m_pir_processed_volume);
	Set_VacplasmaBag(m_plasma_bag_ac);
	Set_VplasmaBag(m_plasma_bag_volume);
	Set_PltConcBag(m_pltconcbag);
	SetPrePIRProcessedVolume(m_prepir_processed_volume);
	SetProcedureRunTime(m_procedure_run_time);
	Set_Qac(m_qac);
	Set_Qcollect(m_qcol);
	Set_Qin(m_qin);
	SetQinAdjustmentValue(m_qin_adjustment);
	Set_Qplasma(m_qp);
   //m_qrep is not used
	Set_Qreturn(m_qrp);
	SetQrpAdjustmentValue(m_qrp_adjustment);
	SetRatioAdjustmentValue(m_ratio_adjustment);
	Set_VacrbcBag(m_rbc_bag_ac);
	Set_VrbcBag(m_rbc_bag_volume);
	SetRBCSetupProcessedVolume(m_rbc_setup_processed_volume);
	SetCentrifugeSpeed(m_rpm);
	if (m_source_plasma_entered) SourcePlasmaEntered(lTRUE); else SourcePlasmaEntered(lFALSE);
	SetSubstate((State_names)m_substate);
	SetSystemState((State_names)m_system_state);
	Set_Vac(m_vac);
	SetVbpPlatelet(m_vbp_platelet);
	Set_Vcollect(m_vcol);
	Set_Vin(m_vin);
	Set_Vplasma(m_vp);
	ReplacementVolume(m_vrep);
	Set_Vreturn(m_vrp);
	Set_PltYield(m_yield*1.0E11f);
*/
  }
/////////////////////////////////////////////////////////////////////////////
// Initial_conditions message handlers

void Initial_conditions::OnClose() 
{
   UpdateData(1);
	
	CDialog::OnClose();
}

void Initial_conditions::OnSnarf() 
{	
   UpdateData(1);
   if(m_rundata_file.Find(".dlog") != -1)
	   process_dlog_file(m_rundata_file);
   else if(m_rundata_file.Find(".csv") != -1)
		process_file(m_rundata_file,m_abstime);	
}

void Initial_conditions::OnIcBrowse() 
{
   CFileDialog dlg(TRUE,0,m_rundata_file,
                   OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR,
                   lpstrFilter);
   int load = dlg.DoModal();
   if (load == IDOK) {
      UpdateData(1);
      m_rundata_file = dlg.GetPathName();
      UpdateData(0);
   }
}

void Initial_conditions::OnIcReset() 
{
   initialize();
   UpdateData(0);
}


//Use DLDGui's command line capabilites to decode Trima 5.1 / 5.2 compressed 
//encrypted dlog files.
void Initial_conditions::process_dlog_file(const CString &path)
{
	CDecodeDlog decodeDlog;
	m_rundata_file_decoded = decodeDlog.Decode(path);
	
	bool bUpdateScreen = false;
	bool bFoundAdjCaps = false;
	bool bFoundDonorStats = false;
	bool bFoundProcData = false;
	char* arrPredictStr[40];
	char* arrAdjustmentCapsStr[4];
	char* arrColumnData[9];
	char* arrDonorStats[12];
	float procTime = 0.0f;
	static const int MAXBUFFERLENGTH = 5000;
	static char buffer[MAXBUFFERLENGTH];
	std::ifstream rundata(m_rundata_file_decoded,std::ios::in | std::ios::binary);

	if(!rundata.is_open()) 
		rundata.open(m_rundata_file_decoded,std::ios::in | std::ios::binary);
	if (rundata.fail()) { 
	   return;
	}
   

	//float absoluteTime, relativeTime;
	int status = 0,nStart = 0;
	int loopCounter = 5;
	while(loopCounter--)
		rundata.getline(buffer, MAXBUFFERLENGTH);
	
	rundata.getline(buffer, MAXBUFFERLENGTH);

	loopCounter = 2;
	while(loopCounter--)
		rundata.getline(buffer, MAXBUFFERLENGTH);

	//char* c_workstring = NULL;
	while (!rundata.eof()) 
	{
		CString workstring(buffer);
		if(bFoundProcData &&
			bFoundAdjCaps && bFoundDonorStats)
		{
				bUpdateScreen  = true;
				break;
		}
		
		if(workstring.Find("Predict: ProcData") != -1)
		{
			//Get Column data
			split_string(workstring.Left(workstring.ReverseFind(',')), arrColumnData, ",", 9);
			procTime = (float) atof(arrColumnData[1]);
			if((procTime >= m_abstime) && !bFoundProcData)
			{
					//else parse input string.		
					//Here is a sample input string
					/*12:56:26.049,Predict: ProcData: subst(0) 10011(1) yld(2) 10.000(3) Vc(4) 667.157(5) -(6) 97.344(7)
					Vp(8) 0.000(9) -(10) 0.000(11) Vrep(12) 0.000(13) Vrbc(14) 239.597(15) Dose(16) 191.376(17) Vin(18) 7379.233(19) Vac(20) 690.064(21) 
					Qch(22) 5.000(23) minQch(24) 5.000(25) maxQch(26) 5.000(27) time(28) 115.738(29) PIRSpill(30) false(31) Purges(32) 3(33) 
					Vincv(34) 402.456(35) VincvTm(36) 11.325(37) EPCAstart(38) 6839.711(39)*/
					//workstring contains string starting from 'subst'
					split_string(workstring.Right(workstring.GetLength() - (workstring.Find("ProcData: ") + 10)), arrPredictStr, " ", 40);
					bFoundProcData = true;
				//	break;
			}
		}
		else if(workstring.Find("Adjustment caps:") != -1)//get most recent
		{
			//reusing arr Predict Str;
			//workstring contains string starting from 'Qin'
			//11:03:30.716,Adjustment caps:Qin Qrp IR R: 142.000 302.500 1.200 13.70
			split_string(workstring.Right(workstring.GetLength() - (workstring.Find("IR R: ") + 6)), arrAdjustmentCapsStr, " ", 4);			
			bFoundAdjCaps = true;
		}
		else if(workstring.Find("Donor Stats. ") != -1)//get most recent
		{
			//Donor Stats.  TBV:(0) 4719.686(1) PreCnt:(2) 224(3) Hct:(4) 0.450(5) 
			//Wt:(6) 69.000(7) BT:(8) 4(9) SampleVol:(10) 20.000(11)
			split_string(workstring.Right(workstring.GetLength() - (workstring.Find("Donor Stats. ") + 13)), arrDonorStats, " ",  12);
			bFoundDonorStats = true;
		}
		
		rundata.getline(buffer, MAXBUFFERLENGTH);
	}
	if (bUpdateScreen == false)
		return;
	UpdateData(1);
		//Proc debug entry
		m_substate = arrPredictStr[1];
		m_substate_int = (int)strtoul(m_substate, NULL, 10);
		m_yield = (float)atof(arrPredictStr[3]);//yield;
		m_collect_bag_volume = (float)atof(arrPredictStr[5]);
		m_collect_bag_ac = (float)atof(arrPredictStr[7]);
		m_plasma_bag_volume = (float)atof(arrPredictStr[9]);
		m_vp = (float)atof(arrPredictStr[9]);//Vp;
		
		m_plasma_bag_ac = (float)atof(arrPredictStr[11]); //
		m_vrep = (float)atof(arrPredictStr[13]);//replacementVolume;
		m_vin = (float)atof(arrPredictStr[19]);//Vin;
		m_rbc_bag_ac = (float)atof(arrPredictStr[15]);
		m_rbc_bag_dose = (float)atof(arrPredictStr[17]);
		m_vac = (float)atof(arrPredictStr[21]);
		m_current_qch = (float) atof(arrPredictStr[23]);
		m_minimum_qch = (float) atof(arrPredictStr[25]);
		m_procedure_run_time = (float)atof(arrPredictStr[29]);
		m_vincv = (float)atof(arrPredictStr[35]);//vincv;
		m_tcv = (float)atof(arrPredictStr[37]);//tcv;
		m_epca_start =  (float) atof(arrPredictStr[39]);

		m_qac = 0.0f; //don't care?
		m_qcol = 0.0f; //don't care?
		m_qin = 0.0f; //don't care?
		m_qp = 0.0f; //don't care?
		m_qrep = 0.0f;
		m_qrp = 0.0f;//don't care?
		m_rbc_bag_volume = 0.0;
		m_rpm = 0.0f;//don't care?
		m_vcol = 0.0f; //Vc - don't care
		m_vbolus = 0.0f; // no place to snarf from
		m_vrp = 0.0f;//Vrp;
				
		//Adjustment Caps
		m_qin_adjustment = (float)atof(arrAdjustmentCapsStr[0]);
		m_qrp_adjustment = (float)atof(arrAdjustmentCapsStr[1]);
		m_ir_adjustment = (float)atof(arrAdjustmentCapsStr[2]);
		m_ratio_adjustment = (float)atof(arrAdjustmentCapsStr[3]);
	
		//Donor Stats
		//Donor Stats.  TBV:(0) 4719.686(1) PreCnt:(2) 224(3) Hct:(4) 0.450(5) 
		//Wt:(6) 69.000(7) BT:(8) 4(9) SampleVol:(10) 20.000(11)
		m_donor_precount = (float) atof(arrDonorStats[3]);
		m_donor_hct = (float) atof(arrDonorStats[5]);
		//Hct is divided by 100 in the optimizer. This was 
		//probably done for older dlog files. 
		m_donor_hct = m_donor_hct * 100;
		//Column data
		m_height_units = (*arrColumnData[2] == '1') ? "in" : "cm";
		//m_weight_units = (1) ? "lb" : "kg";
		m_weight_units = (*arrColumnData[3] == '1') ? "lb" : "kg";
		m_donor_height = (UINT) atoi(arrColumnData[4]);
		m_donor_weight = (UINT) atoi(arrColumnData[5]);
		m_cassette = (int) atoi(arrColumnData[8]);
		
		//Blood Type
		CString blood_type_string;
		int blood_type_index = m_donor_blood_type_control.GetCurSel();
		if(blood_type_index != -1)
			m_donor_blood_type_control.GetLBText(blood_type_index,blood_type_string);
		m_donorBloodType = CBloodHelper::convert_blood_type(blood_type_string);
		
		m_donor_gender = (int)atoi(arrColumnData[7]) + 1;
		if(m_donor_gender == DONOR_SEX_IS_MALE) //1 for Male and 2 for female
			m_ctlRadioMale.SetCheck(BST_CHECKED);	
		UpdateTBV();
	UpdateData(0);
	
	
	rundata.close();
}

void Initial_conditions::split_string(CString workstring, char **arrStr, const char* delimiters, unsigned int nTokens)
{
	//char* c_workstring = (char*)malloc(workstring.GetLength() + 1);
	char* c_workstring = new char [workstring.GetLength() + 1]; //memory leak!
	strcpy(c_workstring, workstring);
	arrStr[0] = strtok(c_workstring, delimiters);
	int count = 0;

	while(arrStr[count] != NULL)
	{
		count++;
		if(count == nTokens) break;
		arrStr[count] = strtok(NULL, delimiters);
	}
	//save up allocated c_workstrings so they can be reclaimed later
}


//Sriram
//This method (process_file) parses Trima csv files only. The new dlog files
//are compressed and encrypted. See process_dlog_file for parsing dlog files
void Initial_conditions::process_file(const CString &path,const float abstime)
{
   static const int MAXBUFFERLENGTH = 5000;
   static char buffer[MAXBUFFERLENGTH];

   std::ifstream rundata(path,std::ios::in);
   if (rundata.fail()) {
      return;
   }
   
   int status = 0;
   rundata.getline(buffer,MAXBUFFERLENGTH); // istream call
   CString workstring(buffer);
   CString tempstr;
   
   //
   //
   //  Extract header information from the first line.
   //
   int nStart;
   if ((nStart = workstring.Find("ProjectRevision")) > 0) {
      int nEnd = workstring.Find(" ",nStart+17);
      CString build = workstring.Mid(nStart+17,nEnd - nStart - 17);
      
      if (build < "5.100") {
         rundata.close();
         return;
      }
   } else {
      rundata.close();
      return; // can't find project rev so we don't know which rev
   }
   
   //
   //  lines 1-4 are header lines
   //
   rundata.getline(buffer,MAXBUFFERLENGTH);
   rundata.getline(buffer,MAXBUFFERLENGTH);
   rundata.getline(buffer,MAXBUFFERLENGTH);
   rundata.getline(buffer,MAXBUFFERLENGTH); // first information line
   
  processRundataBody(rundata);
   
}


void Initial_conditions::processRundataBody(std::ifstream& rundata)
{
	CString workstring;
	static const int MAXBUFFERLENGTH = 5000;
	static char buffer[MAXBUFFERLENGTH];
	int nStart = 0;
	 //
   //
   //   Tracking variables for states throughout the run
   //
	
   CString substate;
   CString commandedCollectValve;
   float absoluteTime = 0.0f;
   float systemRunTime = 0.0f;
   float procedureTime = 0.0f;
   float totalACUsed = 0.0f;
   float plateletVolume = 0.0f;
   float ACinPlateletBag = 0.0f;
   float plasmaVolume = 0.0f;
   float ACinPlasmaBag = 0.0f;
   float rbcVolume = 0.0f;
   float rbcDose = 0.0f;
   float ACinRBCBag = 0.0f;
   float replacementVolume = 0.0f;
   float VBPTotal = 0.0f;
   float VBPPlatelet = 0.0f;
   float yield = 0.0f;
   float qinAdjustment = 0.0f;
   float qrpAdjustment = 0.0f;
   float ratioAdjustment = 0.0f;
   float irAdjustment = 0.0f;
   float Vin = 0.0f;
   float Vac = 0.0f;
   float Vp = 0.0f;
   float Vc = 0.0f;
   float Vrp = 0.0f;
   float Qin = 0.0f;
   float Qac = 0.0f;
   float Qp = 0.0f;
   float Qc = 0.0f;
   float Qrp = 0.0f;
   float RPM = 0.0f;
   float tcv = 0.0f;
   float vincv = 0.0f;
   float minQch = 0.0f;
   int cassette = 0;
   
   BOOL firstDrawCycle = TRUE;
   BOOL firstReturnCycle = TRUE;
   BOOL paused = TRUE;
   BOOL inBloodRun = FALSE;
   float tempQrp;
   
#define EXTRACT(var) {\
   nStart = workstring.Find(',',nStart+1); \
   if (workstring[nStart+1] != ',') { \
   var = (float)atof(workstring.Mid(nStart+1,workstring.Find(',',nStart+1)-nStart-1)); \
      }}
#define EXTRACTSTR(var) {\
   nStart = workstring.Find(',',nStart+1); \
   if (workstring[nStart+1] != ',') { \
   var = workstring.Mid(nStart+1,workstring.Find(',',nStart+1)-nStart-1); \
      }}
   
   while (!rundata.eof()) {
      workstring = buffer;
      nStart = 0;
      nStart = workstring.Find(',',nStart);
      if (nStart >= 0){// && (workstring.Find("Predict: ProcData") != -1)) {
      //
      //
      //  We use Find to step through each rundata record.  There are problems
      //  using sscanf and strtok since multiple delimiters can be next to each
      //  other.
      //
	  MessageBox(workstring);
      EXTRACT(absoluteTime);
      if (absoluteTime > m_abstime) break;
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at procedure time
      EXTRACT(systemRunTime);
      EXTRACT(procedureTime);
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at system state
      if (workstring[nStart+1] == '8') {
         inBloodRun = TRUE;
      } else if (workstring[nStart+1] != ',') {
         inBloodRun = FALSE;
      }
      EXTRACTSTR(substate);
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at recovery state
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at paused state
      if (workstring[nStart+1] == '0') {
         paused = FALSE;
      } else if (workstring[nStart+1] == '1') {
         paused = TRUE;
      }
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at reservoir status
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at res volume
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at draw cycle
      if (workstring[nStart+1] == '0') { // return cycle found
         if (!firstDrawCycle) firstReturnCycle = FALSE;
         firstDrawCycle = FALSE;
      }
      EXTRACT(Qin);
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at actual inlet
      EXTRACT(Vin);
      if ((substate == "ExtendedPCA") && (m_epca_start == 0.0f)) m_epca_start = Vin;
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at total inlet volume
      EXTRACT(Qac);
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at actual AC
      EXTRACT(Vac);
      EXTRACT(totalACUsed);
      EXTRACT(Qp)
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at actual plasma
      EXTRACT(Vp);
      EXTRACT(Qc);
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at actual collect
      EXTRACT(Vc);
      EXTRACT(tempQrp); if (tempQrp > 0.0f) Qrp = tempQrp;
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at actual return
      EXTRACT(Vrp);
      EXTRACT(RPM);
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at actual cent
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at rbc red
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at rbc green
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at collect valve pos
      EXTRACTSTR(commandedCollectValve);
      if (inBloodRun && (tcv == 0.0f) && (commandedCollectValve == "C")) {
         tcv = procedureTime;
         vincv = Vin;
      }
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at plasma valve pos
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at plasma valve cmd
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at RBC valve pos
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at RBC valve cmd
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at cassette position
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at cassette command
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at cent pressure
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at APS low limit
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at access pressure
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at APS pressure limit (V3.2); APS high limit (v4)
      EXTRACT(qinAdjustment);
      EXTRACT(qrpAdjustment);
      EXTRACT(ratioAdjustment);
      EXTRACT(irAdjustment);
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at adjusted hct
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at res hct
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at hrbc
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at Coagulated needle blood
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at AC detected
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at leak detected
      nStart = workstring.Find(',',nStart+1); // nStart+1 points at collect integ yield
      EXTRACT(plateletVolume);
      EXTRACT(ACinPlateletBag);
      EXTRACT(plasmaVolume);
      EXTRACT(ACinPlasmaBag);
      EXTRACT(rbcVolume);
      EXTRACT(rbcDose);
      EXTRACT(ACinRBCBag);
      EXTRACT(replacementVolume);
      EXTRACT(VBPTotal);
      EXTRACT(VBPPlatelet);
      EXTRACT(yield);
      
      //
      //
      //  Extract cassette type
      //
      int miscStart = workstring.Find(" stamp detected",nStart+1);
      if (miscStart >= 0) {
         while(workstring[--miscStart] != ',');
         CString cassetteType = workstring.Mid(miscStart+1,workstring.Find(' ',miscStart+1)-miscStart-1);
         cassetteType.MakeLower();
         if ((cassetteType == "white") ||
             (cassetteType == "pltplsrbc")) {
            cassette = (int)cassette::PLTPLSRBC;
         } else if ((cassetteType == "black") ||
                    (cassetteType == "rbcpls")) {
            cassette = (int)cassette::RBCPLS;
         } else {
            cassette = (int)cassette::UNKNOWN;
         }
      }
      }

      //
      //   Get next line
      rundata.getline(buffer,MAXBUFFERLENGTH);
   }
   
    UpdateData(1);
		m_collect_bag_ac = ACinPlateletBag;
		m_collect_bag_volume = plateletVolume;
		m_ir_adjustment = irAdjustment;
		m_plasma_bag_ac = ACinPlasmaBag;
		m_plasma_bag_volume = plasmaVolume;
		m_procedure_run_time = procedureTime;
		m_qac = Qac;
		m_qcol = Qc;
		m_qin = Qin;
		m_qin_adjustment = qinAdjustment;
		m_qp = Qp;
		m_qrep = 0.0f;
		m_qrp = Qrp;
		m_qrp_adjustment = qrpAdjustment;
		m_ratio_adjustment = ratioAdjustment;
		m_rbc_bag_ac = ACinRBCBag;
		m_rbc_bag_volume = rbcVolume;
		m_rbc_bag_dose = rbcDose;
		m_rpm = RPM;
		m_substate = substate;
		//Sriram
		m_substate_int = (int)strtoul(m_substate, NULL, 10);
		m_vac = Vac;
		m_vcol = Vc;
		m_vin = Vin;
		m_vp = Vp;
		m_vrep = replacementVolume;
		m_vbolus = 0.0f; // no place to snarf from
		m_vrp = Vrp;
		m_yield = yield;
		m_tcv = tcv;
		m_vincv = vincv;
		m_cassette = cassette;
	UpdateData(0);

	rundata.close();
}


void Initial_conditions::OnChangeDonorHeight() 
{
	UpdateData(TRUE);
	UpdateTBV();	
	UpdateData(FALSE);
}

void Initial_conditions::OnChangeDonorWeight() 
{
	UpdateData(TRUE);
	UpdateTBV();	
	UpdateData(FALSE);
}

void Initial_conditions::UpdateTBV()
{
   //UpdateData(TRUE);
   /*m_calculated_tbv = CBloodHelper::CalculateTBV((float)m_donor_height,cfg_main_input::userconfig->LangUnit.Get().key_height,
                                               (float)m_donor_weight,cfg_main_input::userconfig->LangUnit.Get().key_weight,
                                               (m_male_control.GetCheck() ? DONOR_SEX_IS_MALE:DONOR_SEX_IS_FEMALE));
   */
   m_calculated_tbv = CBloodHelper::CalculateTBV((float)m_donor_height, (m_height_units == "cm") ? 0 : 1,
	   (float) m_donor_weight, (m_weight_units == "kg") ? 0 : 1, m_donor_gender);
	//UpdateData(FALSE);

}




void Initial_conditions::OnIcCobeConfig() 
{
	char* oldPathname = (char*) malloc (sizeof(PATHNAME) + 1);
	strcpy(oldPathname, PATHNAME);

	CFileDialog dlg(TRUE,0,NULL,
                   OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR,
                   "Trima data files|*.dat|All files|*.*||");
	dlg.m_ofn.lpstrInitialDir = m_CWD;
	
   int load = dlg.DoModal();
   if (load == IDOK && !dlg.GetFileName().IsEmpty())
   {
        strcpy(PATHNAME, dlg.m_ofn.lpstrFile);
	    ALARM_VALUES status = CobeConfig::initialize();
		// Check status for cobe config initialization.
		if (status != NULL_ALARM)
		{
		   ::MessageBox(NULL, "Cobe Config failed to read", "ERROR", MB_OK);
		   //::PostMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
			strcpy(PATHNAME, "");
		}
   }	
      UpdateData(1);
     	m_cobe_config_file = PATHNAME;
	  UpdateData(0);
}


void Initial_conditions::OnRADIOSmall() 
{
	// TODO: Add your control notification handler code here
	UpdateData(1);
	m_donor_height = 60;
	m_height_units = "in";
	m_donor_weight = 115;
	m_weight_units = "lb";
	m_donor_hct = 45;
	m_donor_precount = 250;
	m_donor_gender = 1; //male
	UpdateData(0);
}

void Initial_conditions::OnRADIOMedium() 
{
	// TODO: Add your control notification handler code here
		UpdateData(1);
	m_donor_height = 70;
	m_height_units = "in";
	m_donor_weight = 160;
	m_weight_units = "lb";
	m_donor_hct = 45;
	m_donor_precount = 250;
	m_donor_gender = 1; //male
	UpdateData(0);
}

void Initial_conditions::OnRADIOLarge() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_donor_height = 83;
	m_height_units = "in";
	m_donor_weight = 280;
	m_weight_units = "lb";
	m_donor_hct = 45;
	m_donor_precount = 250;
	m_donor_gender = 1; //male
	UpdateData(FALSE);
}

void Initial_conditions::OnRADIOFemale() 
{
	UpdateData(TRUE);
	//m_female_control.SetCheck(1);
	//m_male_control.SetCheck(0);
	m_donor_gender = 2; //1 is for male. and 2, female
	UpdateTBV();
	UpdateData(FALSE);
	
}

void Initial_conditions::OnRADIOMale() 
{
	UpdateData(TRUE);
	//m_female_control.SetCheck(0);
	//m_male_control.SetCheck(1);
	m_donor_gender = 1; //1 is for male. and 2, female
	//m_female_control.SetCheck(!m_male_control.GetCheck());
	UpdateTBV();
	UpdateData(FALSE);
	
}

