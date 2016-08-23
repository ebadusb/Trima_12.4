//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_SINGLE_DONOR_15_PRODUCTS_H__623F9A15_B955_11D1_B0F1_006097402E62__INCLUDED_)
#define AFX_SINGLE_DONOR_15_PRODUCTS_H__623F9A15_B955_11D1_B0F1_006097402E62__INCLUDED_
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// single_donor_15_products.h : header file
//
#include "Offline_optimizer.h"

class cfg_main_input;
#include "Sectionfile.hpp"
#include "optimizer_form.h"

/////////////////////////////////////////////////////////////////////////////
// single_donor_15_products form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

struct Donor
{
	int  id;
	float tbv;
	float hct;
	float pct;
	float height;
	float weight;
	int sex;
};
class single_donor_15_products : public optimizer_form
{
protected:
	single_donor_15_products();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(single_donor_15_products)

// Form Data
public:
	//{{AFX_DATA(single_donor_15_products)
	enum { IDD = IDD_SINGLE_DONOR_15_PRODUCTS };
	CButton	m_LoadCSVFile;
	CButton	m_optimize_button;
	CListBox	m_results_list;
	BOOL	m_enable_debug;
	int		m_predict_request;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void CSVOutputData(procedureDetail *proc,std::ofstream &op, Donor& d);
	void loadCSVFile();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(single_donor_15_products)
	public:
	virtual void OnFinalRelease();
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~single_donor_15_products();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(single_donor_15_products)
	afx_msg void OnOptimize();
	afx_msg void OnDblclkResultsList();
	afx_msg void OnConfig();
	afx_msg void OnInitialConditions();
	afx_msg void OnSendPredictRequest();
	afx_msg void OnIcLoadCsvFile();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(single_donor_15_products)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

protected:
   virtual void UpdateUnits();
   virtual void OnPredictionComplete(procedureDetail *proc);

protected:
	CString m_CSVFilename;
   int m_product;
   BLOODTYPE m_donorBloodType;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLE_DONOR_15_PRODUCTS_H__623F9A15_B955_11D1_B0F1_006097402E62__INCLUDED_)
