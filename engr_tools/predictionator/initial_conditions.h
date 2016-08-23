#if !defined(AFX_INITIAL_CONDITIONS_H__5AEA7865_95C5_11D2_81AB_006097402E62__INCLUDED_)
#define AFX_INITIAL_CONDITIONS_H__5AEA7865_95C5_11D2_81AB_006097402E62__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Initial_conditions.h : header file
//
#include <fstream>
#include "bloodtype.h"
#include "BloodHelper.h"
/////////////////////////////////////////////////////////////////////////////
// Initial_conditions dialog

class Initial_conditions : public CDialog
{
	void processRundataBody(std::ifstream& rundata);
// Construction
// Dialog Data
public:
	Initial_conditions(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(Initial_conditions)
	enum { IDD = IDD_INITIAL_CONDITIONS_DIALOG };
	CButton	m_ctlRadioMale;
	CComboBox	m_donor_blood_type_control;
	float	m_collect_bag_ac;
	float	m_collect_bag_volume;
	float	m_ir_adjustment;
	float	m_plasma_bag_ac;
	float	m_plasma_bag_volume;
	float	m_procedure_run_time;
	float	m_qac;
	float	m_qin;
	float	m_qin_adjustment;
	float	m_qp;
	float	m_qrep;
	float	m_qrp;
	float	m_qrp_adjustment;
	float	m_ratio_adjustment;
	float	m_rbc_bag_ac;
	float	m_rbc_bag_volume;
	float	m_rpm;
	float	m_vac;
	float	m_vcol;
	float	m_vin;
	float	m_vp;
	float	m_vrep;
	float	m_vrp;
	float	m_yield;
	float	m_qcol;
	float	m_abstime;
	CString	m_rundata_file;
	float	m_rbc_bag_dose;
	CString	m_substate;
	float	m_tcv;
	int		m_cassette;
	float	m_minimum_qch;
	float	m_current_qch;
	int		m_purges;
	float	m_epca_start;
	BOOL	m_adjust_random;
	float	m_vincv;
	float	m_vbolus;
	int		m_ptf;
	float	m_ptf_qrbc;
	UINT	m_donor_height;
	UINT	m_donor_weight;
	CString	m_height_units;
	float	m_donor_hct;
	float	m_donor_precount;
	CString	m_weight_units;
	float	m_calculated_tbv;
	CString	m_cobe_config_file;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Initial_conditions)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

   public:
	   CString m_rundata_file_decoded;
	   int m_substate_int;
	   int m_donor_gender;
	   BLOODTYPE m_donorBloodType;
	   void UpdateTBV();
	   CEdit donorHeight;
	   CString m_CWD;
	virtual void initialize();
	virtual void populate_datastore() const;
	virtual void process_file(const CString &path, const float abstime);
	virtual void process_dlog_file(const CString &path);
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Initial_conditions)
	afx_msg void OnClose();
	afx_msg void OnSnarf();
	afx_msg void OnIcBrowse();
	afx_msg void OnIcReset();
	afx_msg void OnChangeDonorHeight();
	afx_msg void OnChangeDonorWeight();
	afx_msg void OnIcCobeConfig();
	afx_msg void OnRADIOSmall();
	afx_msg void OnRADIOMedium();
	afx_msg void OnRADIOLarge();
	afx_msg void OnRADIOMale();
	afx_msg void OnRADIOFemale();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(Initial_conditions)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
	void split_string(CString workstring, char** predit_input, const char* delimiters, unsigned int nTokens);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INITIAL_CONDITIONS_H__5AEA7865_95C5_11D2_81AB_006097402E62__INCLUDED_)
