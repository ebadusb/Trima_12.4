#if !defined(AFX_EXTENDEDRESULTS_H__1D3363B3_BA32_11D1_B0F5_006097402E62__INCLUDED_)
#define AFX_EXTENDEDRESULTS_H__1D3363B3_BA32_11D1_B0F5_006097402E62__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ExtendedResults.h : header file
//
#include "procdet.h"
#include "gridctrl.h"
/////////////////////////////////////////////////////////////////////////////
// CExtendedResults dialog

class CExtendedResults : public CDialog
{
// Construction
public:
	CExtendedResults(CWnd* pParent = NULL);   // standard constructor

   virtual void initialize(const int product,
                                  const CString &status,
                                  const float configured_yield,
                                  const float configured_platelet_volume,
                                  const float configured_plasma_volume,
                                  const float configured_rbc_dose,
                                  const float configured_rbc_crit,
                                  const float rbc_scaling_factor,
                                  const int product_blood_type,
                                  const int donor_blood_type,
                                  const float configured_max_time,
                                  const float replacement_percent,
                                  procedureDetail &results,
                                  Predict *parent);

// Dialog Data
	//{{AFX_DATA(CExtendedResults)
	enum { IDD = IDD_EXTENDED_RESULTS };
	float	m_actual_concentration;
	float	m_actual_plasma;
	float	m_actual_platelets;
	float	m_actual_rbcs;
	float	m_actual_yield;
	CString	m_cassette;
	float	m_configured_concentration;
	float	m_configured_platelets;
	float	m_configured_yield;
	float	m_remaining_volume;
	float	m_actual_max_time;
	float	m_configured_max_time;
	CString	m_results;
	float	m_actual_replacement_volume;
	CString	m_configured_replacement_volume;
	CString	m_configured_rbcs;
	float	m_volume_from_donor;
	float	m_qac_1;
	float	m_qac_10;
	float	m_qac_11;
	float	m_qac_2;
	float	m_qac_3;
	float	m_qac_4;
	float	m_qac_5;
	float	m_qac_6;
	float	m_qac_7;
	float	m_qac_8;
	float	m_qac_9;
	float	m_qrp_1;
	float	m_qrp_10;
	float	m_qrp_11;
	float	m_qrp_2;
	float	m_qrp_3;
	float	m_qrp_4;
	float	m_qrp_5;
	float	m_qrp_6;
	float	m_qrp_7;
	float	m_qrp_8;
	float	m_qrp_9;
	float	m_time_1;
	float	m_time_10;
	float	m_time_11;
	float	m_time_2;
	float	m_time_3;
	float	m_time_4;
	float	m_time_5;
	float	m_time_6;
	float	m_time_7;
	float	m_time_8;
	float	m_time_9;
	float	m_vin_1;
	float	m_vin_10;
	float	m_vin_11;
	float	m_vin_2;
	float	m_vin_3;
	float	m_vin_4;
	float	m_vin_5;
	float	m_vin_6;
	float	m_vin_7;
	float	m_vin_8;
	float	m_vin_9;
	float	m_qcdraw_1;
	float	m_qcdraw_10;
	float	m_qcdraw_11;
	float	m_qcdraw_2;
	float	m_qcdraw_3;
	float	m_qcdraw_4;
	float	m_qcdraw_5;
	float	m_qcdraw_6;
	float	m_qcdraw_7;
	float	m_qcdraw_8;
	float	m_qcdraw_9;
	float	m_qindraw_1;
	float	m_qindraw_10;
	float	m_qindraw_11;
	float	m_qindraw_2;
	float	m_qindraw_3;
	float	m_qindraw_4;
	float	m_qindraw_5;
	float	m_qindraw_6;
	float	m_qindraw_7;
	float	m_qindraw_8;
	float	m_qindraw_9;
	float	m_qinreturn_1;
	float	m_qinreturn_10;
	float	m_qinreturn_11;
	float	m_qinreturn_2;
	float	m_qinreturn_3;
	float	m_qinreturn_4;
	float	m_qinreturn_5;
	float	m_qinreturn_6;
	float	m_qinreturn_7;
	float	m_qinreturn_8;
	float	m_qinreturn_9;
	float	m_qpdraw_1;
	float	m_qpdraw_10;
	float	m_qpdraw_11;
	float	m_qpdraw_2;
	float	m_qpdraw_3;
	float	m_qpdraw_4;
	float	m_qpdraw_5;
	float	m_qpdraw_6;
	float	m_qpdraw_7;
	float	m_qpdraw_8;
	float	m_qpdraw_9;
	int		m_total_iterations;
	float	m_qch_1;
	float	m_qch_10;
	float	m_qch_11;
	float	m_qch_2;
	float	m_qch_3;
	float	m_qch_4;
	float	m_qch_5;
	float	m_qch_6;
	float	m_qch_7;
	float	m_qch_8;
	float	m_qch_9;
	float	m_vp_1;
	float	m_vp_10;
	float	m_vp_11;
	float	m_vp_2;
	float	m_vp_3;
	float	m_vp_4;
	float	m_vp_5;
	float	m_vp_6;
	float	m_vp_7;
	float	m_vp_8;
	float	m_vp_9;
	float	m_vc_1;
	float	m_vc_10;
	float	m_vc_11;
	float	m_vc_2;
	float	m_vc_3;
	float	m_vc_4;
	float	m_vc_5;
	float	m_vc_6;
	float	m_vc_7;
	float	m_vc_8;
	float	m_vc_9;
	float	m_vac_1;
	float	m_vac_10;
	float	m_vac_11;
	float	m_vac_2;
	float	m_vac_3;
	float	m_vac_4;
	float	m_vac_5;
	float	m_vac_6;
	float	m_vac_7;
	float	m_vac_8;
	float	m_vac_9;
	float	m_ir_1;
	float	m_ir_10;
	float	m_ir_11;
	float	m_ir_2;
	float	m_ir_3;
	float	m_ir_4;
	float	m_ir_5;
	float	m_ir_6;
	float	m_ir_7;
	float	m_ir_8;
	float	m_ir_9;
	CString	m_convergence;
	float	m_hd_1;
	float	m_hd_10;
	float	m_hd_11;
	float	m_hd_2;
	float	m_hd_3;
	float	m_hd_4;
	float	m_hd_5;
	float	m_hd_6;
	float	m_hd_7;
	float	m_hd_8;
	float	m_hd_9;
	float	m_vsvn_1;
	float	m_vsvn_10;
	float	m_vsvn_11;
	float	m_vsvn_2;
	float	m_vsvn_3;
	float	m_vsvn_4;
	float	m_vsvn_5;
	float	m_vsvn_6;
	float	m_vsvn_7;
	float	m_vsvn_8;
	float	m_vsvn_9;
	float	m_vsvn_12;
	float	m_vp_12;
	float	m_vin_12;
	float	m_vc_12;
	float	m_vac_12;
	float	m_time_12;
	float	m_qrp_12;
	float	m_qpdraw_12;
	float	m_qinreturn_12;
	float	m_qindraw_12;
	float	m_qch_12;
	float	m_qcdraw_12;
	float	m_qac_12;
	float	m_ir_12;
	float	m_hd_12;
	float	m_hd_13;
	float	m_ir_13;
	float	m_qac_13;
	float	m_qcdraw_13;
	float	m_qch_13;
	float	m_qindraw_13;
	float	m_qinreturn_13;
	float	m_qpdraw_13;
	float	m_qrp_13;
	float	m_time_13;
	float	m_vac_13;
	float	m_vc_13;
	float	m_vin_13;
	float	m_vp_13;
	float	m_vsvn_13;
	CString	m_substate_2;
	CString	m_substate_3;
	CString	m_substate_4;
	CString	m_substate_5;
	CString	m_configured_plasma;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtendedResults)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CGridCtrl* m_Grid;
	// Generated message map functions
	//{{AFX_MSG(CExtendedResults)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    bool testIfNeedQrbc(int index,procedureDetail &results);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTENDEDRESULTS_H__1D3363B3_BA32_11D1_B0F5_006097402E62__INCLUDED_)
