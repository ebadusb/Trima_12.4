#if !defined(AFX_AUTOMATION_MODEL_1_H__A9732ABA_BE66_11D1_8F86_006008346074__INCLUDED_)
#define AFX_AUTOMATION_MODEL_1_H__A9732ABA_BE66_11D1_8F86_006008346074__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// automation_model_1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Cautomation_model_1 form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

//
#include "optimizer_form.h"
#include "DonorDB.h"

class Cautomation_model_1 : public optimizer_form
{
protected:
	Cautomation_model_1();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(Cautomation_model_1)

// Form Data
public:
	//{{AFX_DATA(Cautomation_model_1)
	enum { IDD = IDD_AUTOMATION_MODEL_1 };
	CButton	m_edit_config_control;
	CButton	m_stop_sim_pb_control;
	CButton	m_manu_sim_pb_control;
	CProgressCtrl	m_ctrlOptimizationProgress;
	int		m_iCollSimCount;
	int		m_iAutoSelection_1;
	int		m_iAutoSelection_10;
	int		m_iAutoSelection_11;
	int		m_iAutoSelection_12;
	int		m_iAutoSelection_13;
	int		m_iAutoSelection_14;
	int		m_iAutoSelection_15;
	int		m_iAutoSelection_2;
	int		m_iAutoSelection_3;
	int		m_iAutoSelection_4;
	int		m_iAutoSelection_5;
	int		m_iAutoSelection_6;
	int		m_iAutoSelection_7;
	int		m_iAutoSelection_8;
	int		m_iAutoSelection_9;
	int		m_iAutoSelection_1_filtered;
	int		m_iAutoSelection_10_filtered;
	int		m_iAutoSelection_11_filtered;
	int		m_iAutoSelection_12_filtered;
	int		m_iAutoSelection_13_filtered;
	int		m_iAutoSelection_14_filtered;
	int		m_iAutoSelection_15_filtered;
	int		m_iAutoSelection_2_filtered;
	int		m_iAutoSelection_3_filtered;
	int		m_iAutoSelection_4_filtered;
	int		m_iAutoSelection_5_filtered;
	int		m_iAutoSelection_6_filtered;
	int		m_iAutoSelection_7_filtered;
	int		m_iAutoSelection_8_filtered;
	int		m_iAutoSelection_9_filtered;
	int		m_iAutoFFPNoLoss;
	int		m_iAutoPltNoLoss;
	int		m_iAutoRBCNoLoss;
	CString	m_donor_db;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cautomation_model_1)
	public:
	virtual void OnInitialUpdate();
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~Cautomation_model_1();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(Cautomation_model_1)
	afx_msg void OnManuSimPb();
	afx_msg void OnStopSimPb();
	afx_msg void OnEditConfig();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	/// testing methods ....
	/// 
	void TestPredictionator();
	virtual void UpdateAchievedCount( int i );
	virtual void UpdateFilteredCount( int i );
	virtual void UpdateProductCount(int i);
   virtual void Reset( );
   virtual void enable_controls(const int enable);

protected:


   // progress determinants
   //
   int   m_iDonorCount;
   int   m_iPredictionsRequired;
   int   m_iPredictionsCompleted;
   float m_fPercentPredictionsComplete;

   //
   //
   //  unscaled placeholders
   //
   int m_iAutoPltNoLoss_unscaled;
   int m_iAutoFFPNoLoss_unscaled;
   int m_iAutoRBCNoLoss_unscaled;


   // flag to gate product counting
   // count products first (highest priority) optimization achieved 
   // 
   int   m_iProductsNotCounted;

   //
   //
   //    Values of "standard" products for product distribution counting
   //
   float m_single_yield;
   float m_plasma_small;
   float m_rbc_collect_volume;

   DonorDB   m_DonorDB;


private:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOMATION_MODEL_1_H__A9732ABA_BE66_11D1_8F86_006008346074__INCLUDED_)
