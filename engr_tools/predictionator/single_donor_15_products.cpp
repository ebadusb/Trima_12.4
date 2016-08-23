// single_donor_15_products.cpp : implementation file
//

#include "stdafx.h"
#include "AP2 predictionater.h"
#include "single_donor_15_products.h"
#include "ExtendedResults.h"
#include "cfg_main_input.h"
#include <sstream>
#include <fstream>

#include "datalog.h"
#include "CSVParser.h"
#include "BloodHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CAP2predictionaterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// single_donor_15_products

IMPLEMENT_DYNCREATE(single_donor_15_products, CFormView)

single_donor_15_products::single_donor_15_products() :
	optimizer_form(single_donor_15_products::IDD),
   m_product(0),
   m_donorBloodType(BT_UNKNOWN)
{
	EnableAutomation();
	//{{AFX_DATA_INIT(single_donor_15_products)
	m_enable_debug = FALSE;
	m_predict_request = 0;
	//}}AFX_DATA_INIT
}

single_donor_15_products::~single_donor_15_products()
{
}

void single_donor_15_products::OnFinalRelease()
{
	optimizer_form::OnFinalRelease();
}

void single_donor_15_products::DoDataExchange(CDataExchange* pDX)
{
	optimizer_form::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(single_donor_15_products)
	DDX_Control(pDX, IDC_IC_LOAD_CSV_FILE, m_LoadCSVFile);
	DDX_Control(pDX, IDC_OPTIMIZE, m_optimize_button);
	DDX_Control(pDX, IDC_RESULTS_LIST, m_results_list);
	DDX_Check(pDX, IDC_ENABLE_DEBUG, m_enable_debug);
	DDX_Text(pDX, IDC_PREDICT_REQUEST, m_predict_request);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(single_donor_15_products, optimizer_form)
	//{{AFX_MSG_MAP(single_donor_15_products)
	ON_BN_CLICKED(IDC_OPTIMIZE, OnOptimize)
	ON_LBN_DBLCLK(IDC_RESULTS_LIST, OnDblclkResultsList)
	ON_BN_CLICKED(IDC_CONFIG, OnConfig)
	ON_BN_CLICKED(IDC_INITIAL_CONDITIONS, OnInitialConditions)
	ON_BN_CLICKED(IDC_SEND_PREDICT_REQUEST, OnSendPredictRequest)
	ON_BN_CLICKED(IDC_IC_LOAD_CSV_FILE, OnIcLoadCsvFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(single_donor_15_products, optimizer_form)
	//{{AFX_DISPATCH_MAP(single_donor_15_products)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_Isingle_donor_15_products to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {623F9A14-B955-11D1-B0F1-006097402E62}
static const IID IID_Isingle_donor_15_products =
{ 0x623f9a14, 0xb955, 0x11d1, { 0xb0, 0xf1, 0x0, 0x60, 0x97, 0x40, 0x2e, 0x62 } };

BEGIN_INTERFACE_MAP(single_donor_15_products, optimizer_form)
	INTERFACE_PART(single_donor_15_products, IID_Isingle_donor_15_products, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// single_donor_15_products diagnostics

#ifdef _DEBUG
void single_donor_15_products::AssertValid() const
{
	optimizer_form::AssertValid();
}

void single_donor_15_products::Dump(CDumpContext& dc) const
{
	optimizer_form::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// single_donor_15_products message handlers

void single_donor_15_products::OnOptimize() 
{
   UpdateData(TRUE);

   optimizer_form::OnStart();
   m_optimize_button.EnableWindow(0);

   m_results_list.ResetContent();

   /*CString blood_type_string;
   int blood_type_index = m_donor_blood_type_control.GetCurSel();
   m_donor_blood_type_control.GetLBText(blood_type_index,blood_type_string);
   m_donorBloodType = m_optimizer.convert_blood_type(blood_type_string);
	*/
   m_optimizer.donor(float(m_initial_conditions.m_donor_height),
                     float(m_initial_conditions.m_donor_weight),
                     m_initial_conditions.m_calculated_tbv,
                     (float(m_initial_conditions.m_donor_hct)/100.0f), 
                     (float(m_initial_conditions.m_donor_hct)/100.0f),
                     float(m_initial_conditions.m_donor_precount),
                     m_initial_conditions.m_donorBloodType);
   m_optimizer.initialize();
   //setup logfile
   //DataLog_Level::logfile("C:\\temp\\predictionator.csv", std::ios::app);
   //logfile.open("C:\\temp\\predictionator.csv", std::ios::app);
   m_optimizer.setInitialConditions(m_initial_conditions);
   m_optimizer.optimize();
   
   for (m_product=1; m_product<=cfg_main_input::userconfig->ProcedureList.numberOfProcedures.Get(); m_product++) {  
      OnPredictionComplete(m_optimizer.getProduct(m_product));
   }
	//for easier access to offline logfile.
   DataLog_Level dl;
   //dl.closeLogfile();
   
   UpdateData(0);
   m_optimize_button.EnableWindow(1);
}

void single_donor_15_products::OnDblclkResultsList() 
{
   int selected = m_results_list.GetCurSel();
   if (selected != LB_ERR) {
                 
      procedureDetail *detail = m_optimizer.getProduct(selected+1);
      CString workstring;
      Offline_optimizer::description(detail->getStatus(),workstring);
      
      //
      //
      //   Create the popup and send it the results.  It is the responsibility
      //   of the popup to properly interpret the results.
      //

      CExtendedResults *popup = new CExtendedResults(GetParent());
      popup->Create(IDD_EXTENDED_RESULTS,GetParent());

      const ProductListArray &defs = cfg_main_input::userconfig->ProcedureList.Procedure.Get();
      ProcedureDescription proc = defs[selected];
      //m_optimizer.m_predict.outputSpreadsheetData(detail);

      popup->initialize(selected+1,
                        workstring,
                        proc.yield,
                        proc.platelet_volume,
                        proc.plasma_volume,
                        proc.rbc_dose,
                        proc.rbc_crit,
                        cfg_main_input::userconfig->Predict.Get().key_rsf,
                        proc.blood_type,
                        m_donorBloodType,
                        proc.max_procedure_time,
                        cfg_main_input::userconfig->Predict.Get().key_rbc_fluid_percent,
                        *detail,
                        &(m_optimizer.m_predict));
						
   }
}


void single_donor_15_products::OnInitialUpdate() 
{
	optimizer_form::OnInitialUpdate();	
   //m_donor_blood_type_control.SetCurSel(0);
   //UpdateUnits();
   //UpdateTBV();
}


void single_donor_15_products::OnConfig() 
{
    optimizer_form::OnEditConfig();
    //UpdateUnits();
	
}



void single_donor_15_products::OnInitialConditions() 
{
   optimizer_form::OnInitialConditions();
}

BEGIN_EVENTSINK_MAP(single_donor_15_products, optimizer_form)
    //{{AFX_EVENTSINK_MAP(single_donor_15_products)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void single_donor_15_products::OnPredictionComplete(procedureDetail *proc) 
{
   CString output;
   //
   //
   //    Output the results into the std::list box.  If the result
   //    is good then give overview details; bad results only
   //    get the failure description.
   //
   output.Empty();
   if (proc->getStatus() == GOOD_PRODUCT) {
      output.Format("%d: %.1f in %.1f  %.1f  %.1f  %.1f",
         m_product,
         proc->toplevel.Ytarg,
         proc->_PredVc,
         proc->_PredVp,
         proc->getPredRBCVolume(),
         proc->_PredTp);
   } else {
      CString workstring;
      Offline_optimizer::description((INTERNAL_PREDICT_RESULTS)proc->getStatus(),workstring);
      output.Format("%d: %s",m_product,workstring);
   }
   m_results_list.AddString(output);	
}

void single_donor_15_products::UpdateUnits()
{
   UpdateData(TRUE);
/*   if (cfg_main_input::userconfig->LangUnit.Get().key_height == FT_HEIGHT) {
      m_height_units = "in";
   } else {
      m_height_units = "cm";
   }

   if (cfg_main_input::userconfig->LangUnit.Get().key_weight == LB_WEIGHT) {
      m_weight_units = "lb";
   } else {
      m_weight_units = "kg";
   }
   */
   UpdateData(FALSE);
}

//
//
//   Lots of chance for reuse here if cut and paste wasn't so easy
void single_donor_15_products::OnSendPredictRequest() 
{
   UpdateData(1);
   m_results_list.ResetContent();

   /*CString blood_type_string;
   int blood_type_index = m_donor_blood_type_control.GetCurSel();
   m_donor_blood_type_control.GetLBText(blood_type_index,blood_type_string);
   m_donorBloodType = m_optimizer.convert_blood_type(blood_type_string);
	*/
   m_optimizer.donor(float(m_initial_conditions.m_donor_height),
                     float(m_initial_conditions.m_donor_weight),
                     m_initial_conditions.m_calculated_tbv,
                     (float(m_initial_conditions.m_donor_hct)/100.0f), 
                     (float(m_initial_conditions.m_donor_hct)/100.0f),
                     float(m_initial_conditions.m_donor_precount),
                     m_initial_conditions.m_donorBloodType);

   m_optimizer.setInitialConditions(m_initial_conditions);
   m_optimizer.m_predict.ProcDataReceived((DoPrediction_t)m_predict_request);
   for (m_product=1; m_product<=cfg_main_input::userconfig->ProcedureList.numberOfProcedures.Get(); m_product++) {  
      OnPredictionComplete(m_optimizer.getProduct(m_product));
   }
   UpdateData(0);
}

void single_donor_15_products::OnIcLoadCsvFile() 
{
	static const CString fileExtFilter = "Bob's CSV files|*.csv|All files|*.*||";
    CFileDialog dlg(TRUE,0,"",
                    OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR,
                    fileExtFilter);

    int load = dlg.DoModal();
    if (load == IDOK)
    {
       m_CSVFilename = dlg.GetPathName();
       loadCSVFile();
    }
}


void single_donor_15_products::loadCSVFile()
{
	  m_LoadCSVFile.EnableWindow(0); // disable out button while processing
	
    // Mircosoft BS #*$&%*(#)#)@
    char* filename = (char*) (LPCTSTR) m_CSVFilename;
	
	//To calculate TBV
	CBloodHelper bloodHelper;

    // Open  Input CSV file
    std::ifstream ifs(filename);

    // Open  Output Data File
     std::ofstream ofs("OutputOfPredict.csv");
    //std::ofstream ofs(outfilename);

	 //Turn mobilization on (5.r-pas)
	 cfg_main_input::userconfig->SWOptionSet("postcount_platelet_mobilization_algorithm", 1);

// Write Header to Output file
    ofs << "Run No., TBV, Cpre, H(%), Sex , Height(in) , Weight(lbs),"
		"Time(min), Postcount, HypoD(ml), Postcrit, Status,"
		"Time(min), Postcount, HypoD(ml), Postcrit, Status,"
		"Time(min), Postcount, HypoD(ml), Postcrit, Status,"
		"Time(min), Postcount, HypoD(ml), Postcrit, Status,"
		"Time(min), Postcount, HypoD(ml), Postcrit, Status,"
		"Time(min), Postcount, HypoD(ml), Postcrit, Status"	 << std::endl;

    
    
     // ofs << "Run No., TBV, Cpre, H(%), Sex , Height(in) , Weight(lbs) ,  Number of Purges" << std::endl;
     char* linebuffer = new char[512];

    int linecnt = 0;
    float myTBV =0.0;

	Donor d;
    // While <lines to parse>
    while ( (!ifs.eof() ) || (linecnt < 2500) )
    {
		  linecnt++;

          // make a parser  -- slow but sure
          CSVParser parser;

          //    parse file line 
          ifs.getline(linebuffer, 512 );
 
          parser.parseALine(linebuffer);


          const char* a =     parser.getColm(0);
          const char* endflag = "ENDDATA";
          if (!strcmp(a,endflag) )
          {
              break;
          }
     
          // run predict on the line data
          UpdateData(TRUE);

          optimizer_form::OnStart();
          m_optimize_button.EnableWindow(0);

          m_results_list.ResetContent();

          m_donorBloodType = BT_A_POS; //64  who cares
		  
		  d.id = atoi(parser.getColm(0));
		  d.height = atof(parser.getColm(2)); d.weight = atof(parser.getColm(3));
		  d.hct = atof(parser.getColm(6))/100.0f; d.pct = atof(parser.getColm(5)); d.sex = atoi(parser.getColm(1));       
		  
		  d.tbv = bloodHelper.CalculateTBV(d.height,
                                            FT_HEIGHT,
                                            d.weight,
                                            LB_WEIGHT,
                                            d.sex);
		   
          


          // regurgitate data
          ofs << d.id << ","  
              << d.tbv << ","  
              << d.pct << "," 
              << d.hct << "," 
              << d.sex << "," 
              << d.height << "," 
              << d.weight  
              << '\0';

			
          m_optimizer.donor( d.height  ,       // height
                            d.weight   ,       // weight
                            d.tbv        ,                    // TBV
                            d.hct,// hematocrit
                            d.hct,// adjustedcrit
                            d.pct,        // precount
                            m_donorBloodType);                 // blood_type
                                                               

          m_optimizer.initialize();
          m_optimizer.setInitialConditions(m_initial_conditions);
          m_optimizer.optimize();



          //    output the Procedure params to file
          for (m_product=1; 
               m_product <= 6 /*m_product<=cfg_main_input::userconfig->ProcedureList.numberOfProcedures.Get()*/;
               m_product++) 
          {  
             CSVOutputData(m_optimizer.getProduct(m_product), ofs, d);
          }

          ofs << std::endl;

          UpdateData(0);
          m_optimize_button.EnableWindow(1);

          linebuffer = ""; // clean up

    // endWhile
    }

    // Close Output File
    ofs.close();

    // Close Input File
    ifs.close();

    // update listbox
    CString s = "Your file has been Gonculated! see OutputOfPredict.csv";
    m_results_list.AddString(s);


    m_LoadCSVFile.EnableWindow(1); // disable out button while processing
}

void single_donor_15_products::CSVOutputData(procedureDetail *proc,std::ofstream &op,  Donor& d) 
{
      
    
    PostHct hct; 
	CString workstring;
	
	m_optimizer.description(proc->getStatus(), workstring);

	op << "," <<  ((proc->_PredTp > 0) ? (proc->_PredTp): 0)
           << "," << Postcount(d.pct, 
							   d.tbv,
							   proc->getDesiredYield()*1E11,        // precount, tbv, yield, minpostcount 
							   m_optimizer.m_predict.getMinPostCount()).postcount()
							   << "," << ((proc->hypovolemiaDistance() > 0) ? (proc->hypovolemiaDistance()) : 0 )
           << "," << hct.postHct(d.hct, 
								 d.tbv,
								 proc->getProcRBCDose(),
								 proc->getResidualRBCVolume(),
								 /*m_optimizer.m_predict.getSampleVolume()*/ 20.0f)

			<< "," << proc->getStatus();

}
