//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_TEST_SUITE_H__AAC75BB4_EA69_11D1_8054_006097402E62__INCLUDED_)
#define AFX_TEST_SUITE_H__AAC75BB4_EA69_11D1_8054_006097402E62__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// test_suite.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// test_suite form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "optimizer_form.h"

class test_suite : public optimizer_form
{
protected:
	test_suite();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(test_suite)

// Form Data
public:
	//{{AFX_DATA(test_suite)
	enum { IDD = IDD_TEST_SUITE };
	CButton	m_initial_conditions_control;
	CButton	m_stop_button;
	CButton	m_start_button;
	CButton	m_edit_config_control;
	CEdit	m_first_to_try_control;
	CButton	m_product_rbcs_control;
	CButton	m_product_platelet_rbcs_control;
	CButton	m_product_platelet_plasma_rbcs_control;
	CButton	m_product_platelet_plasma_control;
	CButton	m_product_platelet_control;
	CButton	m_product_plasma_rbcs_control;
	CButton	m_product_plasma_control;
	CComboBox	m_test_substate_control;
	CComboBox	m_qrp_adjustment_control;
	CComboBox	m_qin_limit_control;
	CButton	m_stop_test;
	CButton	m_start_test;
	CProgressCtrl	m_test_suite_progress;
	int		m_percent_complete;
	int		m_total_products;
	int		m_total_good;
	int		m_qrp_adjustment_index;
	int		m_qin_limit_index;
	int		m_test_substate_index;
	BOOL	m_product_plasma;
	BOOL	m_product_plasma_rbcs;
	BOOL	m_product_platelet;
	BOOL	m_product_platelet_plasma;
	BOOL	m_product_platelet_plasma_rbcs;
	BOOL	m_product_platelet_rbcs;
	BOOL	m_product_rbcs;
	int		m_total_tried;
	CString	m_remaining_time;
	int		m_total_plasma;
	int		m_total_plasma_rbcs;
	int		m_total_platelets;
	int		m_total_platelets_plasma;
	int		m_total_platelets_rbcs;
	int		m_good_plasma;
	int		m_good_plasma_rbcs;
	int		m_good_platelets;
	int		m_good_platelets_plasma;
	int		m_good_platelets_plasma_rbcs;
	int		m_good_platelets_rbcs;
	int		m_good_rbcs;
	int		m_total_platelets_plasma_rbcs;
	int		m_total_rbcs;
	int		m_first_to_try;
	int		m_iterations_plasma;
	int		m_iterations_plasma_rbcs;
	int		m_iterations_platelets;
	int		m_iterations_platelets_plasma;
	int		m_iterations_platelets_plasma_rbcs;
	int		m_iterations_platelets_rbcs;
	int		m_iterations_rbcs;
	int		m_iterations_total;
	int		m_conc_plasma;
	int		m_conc_plasma_rbcs;
	int		m_conc_platelets;
	int		m_conc_platelets_plasma;
	int		m_conc_platelets_plasma_rbcs;
	int		m_conc_platelets_rbcs;
	int		m_conc_rbcs;
	int		m_conc_total;
	int		m_max_iterations_amap_pl;
	int		m_max_iterations_amap_pl_rbc;
	int		m_max_iterations_amap_plt;
	int		m_max_iterations_amap_plt_pl;
	int		m_max_iterations_amap_plt_pl_rbc;
	int		m_max_iterations_amap_plt_rbc;
	int		m_max_iterations_amap_rbc;
	int		m_max_iterations_pl;
	int		m_max_iterations_pl_rbc;
	int		m_max_iterations_plt;
	int		m_max_iterations_plt_pl;
	int		m_max_iterations_plt_pl_rbc;
	int		m_max_iterations_rbc;
	int		m_max_iterations_plt_rbc;
	CString	m_elapsed_time;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(test_suite)
	public:
	virtual void OnFinalRelease();
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~test_suite();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(test_suite)
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnCloseupQinLimit();
	afx_msg void OnCloseupQrpAdjustment();
	afx_msg void OnCloseupTestSubstate();
	afx_msg void OnProductSelection();
	afx_msg void OnInitialConditions();
	afx_msg void OnPredictionComplete();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(test_suite)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

protected:
   void enable_controls(const int enable);
   virtual void Test();
   void update_products();
   int product_active(const float pltvol,const float plasmavol,const float rbcvol);


   //  Function for idle loop processing
   void IdleCallback();

protected:

   //
   // index variables for simulation
   int m_ratio_index;
   int m_hypo_index;
   int m_proctime_index;
   int m_icrit_index;
   int m_ysf_index;
   int m_tbv_index;
   int m_hct_index;
   int m_pltpre_index;
   int m_plasma_product_index;
   int m_rbc_product_index;
   int m_platelet_product_index;
   int m_draw_flow_index;
   int m_balance_index;

   int m_last_number_updated;

   float m_qrp_adjustment_value;
   float m_qin_limit_value;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEST_SUITE_H__AAC75BB4_EA69_11D1_8054_006097402E62__INCLUDED_)
