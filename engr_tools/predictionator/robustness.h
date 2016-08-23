#if !defined(AFX_ROBUSTNESS_H__092B8619_63BC_4844_A56D_2A5ECC502E6C__INCLUDED_)
#define AFX_ROBUSTNESS_H__092B8619_63BC_4844_A56D_2A5ECC502E6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// robustness.h : header file
//

#include "optimizer_form.h"
class CExtendedResults;

/////////////////////////////////////////////////////////////////////////////
// robustness form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class robustness : public optimizer_form
{
protected:
	robustness();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(robustness)

// Form Data
public:
	//{{AFX_DATA(robustness)
	enum { IDD = IDD_ROBUSTNESS };
	CEdit	m_tbv_stop_ctl;
	CEdit	m_tbv_start_ctl;
	CEdit	m_tbv_delta_ctl;
	CEdit	m_precount_stop_ctl;
	CEdit	m_precount_start_ctl;
	CEdit	m_precount_delta_ctl;
	CEdit	m_hct_stop_ctl;
	CEdit	m_hct_start_ctl;
	CEdit	m_hct_delta_ctl;
	CButton	m_repredict_control;
	CEdit	m_total_products_control;
	CButton	m_amap_control;
	CButton	m_max_concurrent_control;
	CListBox	m_results_control;
	CButton	m_stop_control;
	CButton	m_start_control;
	CButton	m_initial_conditions_control;
	CButton	m_edit_config_control;
	CProgressCtrl	m_test_suite_progress;
	float	m_hct_delta;
	float	m_hct_start;
	float	m_hct_stop;
	int		m_hct_tests;
	float	m_plasma_delta;
	float	m_plasma_start;
	float	m_plasma_stop;
	int		m_plasma_tests;
	float	m_precount_delta;
	float	m_precount_start;
	float	m_precount_stop;
	int		m_precount_tests;
	float	m_rbccrit_delta;
	float	m_rbccrit_start;
	float	m_rbccrit_stop;
	int		m_rbccrit_tests;
	float	m_rbcdose_delta;
	float	m_rbcdose_start;
	float	m_rbcdose_stop;
	int		m_rbcdose_tests;
	float	m_tbv_delta;
	float	m_tbv_start;
	float	m_tbv_stop;
	int		m_tbv_tests;
	float	m_yield_delta;
	float	m_yield_start;
	float	m_yield_stop;
	int		m_yield_tests;
	int		m_percent_complete;
	CString	m_elapsed_time;
	CString	m_remaining_time;
	int		m_total_tried;
	int		m_total_products;
	float	m_conc_delta;
	float	m_conc_start;
	float	m_conc_stop;
	int		m_conc_tests;
	BOOL	m_amap;
	BOOL	m_max_concurrent;
	int		m_draw_flow_delta;
	int		m_draw_flow_start;
	int		m_draw_flow_stop;
	int		m_draw_management_delta;
	int		m_draw_management_start;
	int		m_infusion_delta;
	int		m_infusion_start;
	int		m_infusion_stop;
	int		m_inlet_ramp_delta;
	int		m_inlet_ramp_start;
	int		m_inlet_ramp_stop;
	float	m_ratio_delta;
	float	m_ratio_start;
	float	m_ratio_stop;
	int		m_return_management_delta;
	int		m_return_management_start;
	int		m_return_management_stop;
	int		m_draw_management_stop;
	int		m_good;
	int		m_good_lt_2000;
	int		m_good_lt_4000;
	float	m_percent_lt_2000;
	float	m_percent_lt_4000;
	float	m_percent_good;
	int		m_unconverged;
	int		m_unconverged_lt_2000;
	int		m_unconverged_lt_4000;
	int		m_good_gt_4000;
	float	m_percent_gt_4000;
	int		m_unconverged_gt_4000;
	int		m_inconc;
	int		m_inconc_gt_4000;
	int		m_inconc_lt_2000;
	int		m_inconc_lt_4000;
	float	m_inconc_percent_good;
	float	m_inconc_percent_gt_4000;
	float	m_inconc_percent_lt_4000;
	float	m_inconc_percent_lt_2000;
	BOOL	m_display_concentration;
	BOOL	m_display_unconverged;
	BOOL	m_display_vc_errors;
	BOOL	m_display_vp_errors;
	BOOL	m_display_chamber_flow;
	int		m_lockin_errors;
	BOOL	m_display_lockin;
	float	m_debug;
	BOOL	m_repredict;
	CString	m_listbox_save_path;
	BOOL	m_sequential;
	BOOL	m_use_normal;
	int		m_ptf_delta;
	int		m_ptf_end;
	int		m_ptf_start;
	int		m_ptf_stop;
	int		m_ppc_mod_lt_2000;
	int		m_ppc_mod_gt_4000;
	int		m_ppc_mod_lt_4000;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(robustness)
	public:
	virtual void OnFinalRelease();
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~robustness();
   virtual void enable_controls(const int enable);
   virtual void Test();
   virtual void NextOptimization();
   virtual void LogError(const char *headerstr,const char *specificstr);
   virtual const float CreateRand(const int tests,const float delta,const float start);
   virtual void Repredict(procedureDetail *pd, const int showresult);
   virtual void CheckLockin(procedureDetail *pd, const int showresult);
   virtual const int pseudodistribution(const double vm, const double exp);


   //  Function for idle loop processing
   void IdleCallback();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(robustness)
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnHct();
	afx_msg void OnTBV();
	afx_msg void OnPrecount();
	afx_msg void OnConc();
	afx_msg void OnPlasma();
	afx_msg void OnRBCCrit();
	afx_msg void OnRBCDose();
	afx_msg void OnYield();
	afx_msg void OnEditConfig();
	afx_msg void OnInitialConditions();
	afx_msg void OnDblclkResults();
	afx_msg void OnDrawFlow();
	afx_msg void OnDrawManagement();
	afx_msg void OnInfusion();
	afx_msg void OnInletRamp();
	afx_msg void OnRatio();
	afx_msg void OnReturnManagement();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSaveOutput();
	afx_msg void OnRestoreOutput();
	afx_msg void OnUseNormal();
	afx_msg void OnSequential();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(robustness)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

protected:

   CExtendedResults *m_popup;
   CExtendedResults *m_popup_repredict;

   int m_last_number_updated;
   float m_maxInfusion;
   float m_maxQin;
   float m_maxQrp;

   float m_current_conc;
   float m_current_hct;
   float m_current_plasma;
   float m_current_precount;
   float m_current_rbccrit;
   float m_current_rbcdose;
   float m_current_tbv;
   float m_current_yield;

   float m_current_ratio;
   int m_current_infusion;
   int m_current_draw_flow;
   int m_current_draw_management;
   int m_current_return_management;
   int m_current_inlet_ramp;
   int m_current_replacement;
   int m_current_replacement_percent;

   float m_ratio_tests;
   int m_infusion_tests;
   int m_draw_flow_tests;
   int m_draw_management_tests;
   int m_return_management_tests;
   int m_inlet_ramp_tests;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROBUSTNESS_H__092B8619_63BC_4844_A56D_2A5ECC502E6C__INCLUDED_)
