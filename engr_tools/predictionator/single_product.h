//{{AFX_INCLUDES()
#include "donorinput.h"
//}}AFX_INCLUDES
#if !defined(AFX_SINGLE_PRODUCT_H__BA8C3794_3607_11D2_80EF_006097402E62__INCLUDED_)
#define AFX_SINGLE_PRODUCT_H__BA8C3794_3607_11D2_80EF_006097402E62__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// single_product.h : header file
//
#include "Offline_optimizer.h"
class cfg_main_input;
class initial_conditions;
#include "Sectionfile.hpp"
#include "optimizer_form.h"

/////////////////////////////////////////////////////////////////////////////
// single_product form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class single_product : public optimizer_form
{
protected:
	single_product();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(single_product)

// Form Data
public:
	//{{AFX_DATA(single_product)
	enum { IDD = IDD_SINGLE_PRODUCT };
	CButton	m_male_control;
	CButton	m_female_control;
	CComboBox	m_donor_blood_type_control;
	CListBox	m_results_list_control;
	CButton	m_optimize_button;
	CString	m_results_string;
	CString	m_save_path;
	BOOL	m_intermediate_results;
	float	m_calculated_tbv;
	int		m_donor_height;
	int		m_donor_hematocrit;
	int		m_donor_precount;
	int		m_donor_weight;
	CString	m_height_units;
	CString	m_weight_units;
	BOOL	m_female;
	BOOL	m_male;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(single_product)
	public:
	virtual void OnFinalRelease();
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~single_product();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(single_product)
	afx_msg void OnOptimize();
	afx_msg void OnConfig();
	afx_msg void OnSaveDataTo();
	afx_msg void OnInitialConditions();
	afx_msg void OnMale();
	afx_msg void OnFemale();
	afx_msg void OnDonorHeight();
	afx_msg void OnDonorWeight();
	afx_msg void OnExtendedResults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(single_product)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

protected:
   virtual void UpdateTBV();
   virtual void UpdateUnits();

protected:
   BLOODTYPE m_donorBloodType;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLE_PRODUCT_H__BA8C3794_3607_11D2_80EF_006097402E62__INCLUDED_)
