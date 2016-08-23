#if !defined(AFX_CFG_MAIN_INPUT_H__AD0CDA08_BBA2_11D1_B0F8_006097402E62__INCLUDED_)
#define AFX_CFG_MAIN_INPUT_H__AD0CDA08_BBA2_11D1_B0F8_006097402E62__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// cfg_main_input.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cfg_main_input form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "vxworks.h"
#include "config_cds.h"
#include "resource.h"
#include <iostream>

class cfg_main_input : public CDialog
{
public:
	cfg_main_input(CWnd *pParent = NULL);
	virtual ~cfg_main_input();

// Form Data
	//{{AFX_DATA(cfg_main_input)
	enum { IDD = IDD_CFG_MAIN };
	float	m_percent_tbv;
	float	m_remove_weight;
	float	m_remove_weight_less_than;
	float	m_remove_weight_greater_than;
	int		m_height_units;
	int		m_infusion_rate;
	int		m_language;
	float	m_max_procedure_time;
	float	m_minimum_postcount;
	float	m_minimum_postcrit;
	float	m_plt1_concentration;
	float	m_plt1_volume;
	float	m_plt1_yield;
	float	m_plt2_concentration;
	float	m_plt2_volume;
	float	m_plt2_yield;
	float	m_plt3_concentration;
	float	m_plt3_volume;
	float	m_plt3_yield;
	int		m_weight_units;
	float	m_ysf;
	int		m_rbc_ratio_index;
	int		m_replacement_fluid_index;
	float	m_replacement_fluid_percent;
	float	m_plt4_concentration;
	float	m_plt4_volume;
	float	m_plt4_yield;
	float	m_plt5_concentration;
	float	m_plt5_volume;
	float	m_plt5_yield;
	float	m_plt6_concentration;
	float	m_plt6_volume;
	float	m_plt6_yield;
	float	m_rbc_volume_1;
	float	m_rbc_volume_2;
	float	m_rbc_volume_3;
	float	m_rbc_crit_1;
	float	m_rbc_crit_2;
	float	m_rbc_dose_1;
	float	m_rbc_dose_2;
	float	m_rbc_dose_3;
	float	m_rbc_crit_3;
	int		m_product_plasma_1;
	int		m_product_plasma_10;
	int		m_product_plasma_11;
	int		m_product_plasma_12;
	int		m_product_plasma_13;
	int		m_product_plasma_14;
	int		m_product_plasma_15;
	int		m_product_plasma_2;
	int		m_product_plasma_3;
	int		m_product_plasma_4;
	int		m_product_plasma_5;
	int		m_product_plasma_6;
	int		m_product_plasma_7;
	int		m_product_plasma_8;
	int		m_product_plasma_9;
	int		m_product_platelet_1;
	int		m_product_platelet_10;
	int		m_product_platelet_11;
	int		m_product_platelet_12;
	int		m_product_platelet_13;
	int		m_product_platelet_14;
	int		m_product_platelet_15;
	int		m_product_platelet_2;
	int		m_product_platelet_3;
	int		m_product_platelet_4;
	int		m_product_platelet_5;
	int		m_product_platelet_6;
	int		m_product_platelet_7;
	int		m_product_platelet_8;
	int		m_product_platelet_9;
	int		m_product_rbc_1;
	int		m_product_rbc_10;
	int		m_product_rbc_11;
	int		m_product_rbc_12;
	int		m_product_rbc_13;
	int		m_product_rbc_14;
	int		m_product_rbc_15;
	int		m_product_rbc_2;
	int		m_product_rbc_3;
	int		m_product_rbc_4;
	int		m_product_rbc_5;
	int		m_product_rbc_6;
	int		m_product_rbc_7;
	int		m_product_rbc_8;
	int		m_product_rbc_9;
	float	m_replace_volume_trigger;
	int		m_max_draw_flow;
	int		m_product_blood_type_1;
	int		m_product_blood_type_10;
	int		m_product_blood_type_11;
	int		m_product_blood_type_12;
	int		m_product_blood_type_13;
	int		m_product_blood_type_14;
	int		m_product_blood_type_15;
	int		m_product_blood_type_2;
	int		m_product_blood_type_3;
	int		m_product_blood_type_4;
	int		m_product_blood_type_5;
	int		m_product_blood_type_6;
	int		m_product_blood_type_7;
	int		m_product_blood_type_8;
	int		m_product_blood_type_9;
	float	m_product_time_10;
	float	m_product_time_1;
	float	m_product_time_11;
	float	m_product_time_12;
	float	m_product_time_13;
	float	m_product_time_14;
	float	m_product_time_15;
	float	m_product_time_2;
	float	m_product_time_3;
	float	m_product_time_4;
	float	m_product_time_5;
	float	m_product_time_6;
	float	m_product_time_7;
	float	m_product_time_8;
	float	m_product_time_9;
	float	m_custom_ratio;
	int		m_draw_management;
	BOOL	m_ml_per_kg_active;
	float	m_ml_per_kg;
	float	m_rbc_ysf;
	BOOL	m_tbv_limit_active;
	BOOL	m_volume_limit_active;
	int		m_return_management;
	int		m_inlet_ramp;
	int		m_plasma_amap_max;
	int		m_plasma_amap_min;
	int		m_plasma1;
	int		m_plasma2;
	int		m_plasma3;
	int		m_plasma4;
	int		m_plasma5;
	int		m_plasma6;
	int		m_plasma_rinseback;
	int		m_product_rbc_16;
	int		m_product_rbc_17;
	int		m_product_rbc_18;
	int		m_product_rbc_19;
	int		m_product_rbc_20;
	float	m_product_time_16;
	float	m_product_time_17;
	float	m_product_time_18;
	float	m_product_time_19;
	float	m_product_time_20;
	int		m_product_blood_type_16;
	int		m_product_blood_type_17;
	int		m_product_blood_type_18;
	int		m_product_blood_type_19;
	int		m_product_blood_type_20;
	int		m_product_plasma_16;
	int		m_product_plasma_17;
	int		m_product_plasma_18;
	int		m_product_plasma_19;
	int		m_product_plasma_20;
	int		m_product_platelet_16;
	int		m_product_platelet_17;
	int		m_product_platelet_18;
	int		m_product_platelet_19;
	int		m_product_platelet_20;
	int		m_saline_rinseback;
	BOOL	m_plt_mss_1;
	BOOL	m_plt_mss_2;
	BOOL	m_plt_mss_3;
	BOOL	m_plt_mss_4;
	BOOL	m_plt_mss_5;
	BOOL	m_plt_mss_6;
	BOOL	m_rbc_mss_1;
	BOOL	m_rbc_mss_2;
	BOOL	m_rbc_mss_3;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cfg_main_input)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(cfg_main_input)
	afx_msg void OnLoadFile();
	afx_msg void OnSaveFile();
	afx_msg void OnCloseupHeightUnits();
	afx_msg void OnCloseupWeightUnits();
	afx_msg void OnKillfocusMaxProcedureTime();
	afx_msg void OnKillfocusMinimumPostcount();
	afx_msg void OnKillfocusMinimumPostcrit();
	afx_msg void OnKillfocusRemoveWeight();
	afx_msg void OnCloseupInfusionRate();
	afx_msg void OnKillfocusYsf();
	afx_msg void OnKillfocusPlt1Concentration();
	afx_msg void OnKillfocusPlt1Volume();
	afx_msg void OnKillfocusPlt1Yield();
	afx_msg void OnKillfocusPlt2Concentration();
	afx_msg void OnKillfocusPlt2Volume();
	afx_msg void OnKillfocusPlt2Yield();
	afx_msg void OnKillfocusPlt3Concentration();
	afx_msg void OnKillfocusPlt3Volume();
	afx_msg void OnKillfocusPlt3Yield();
	afx_msg void OnKillfocusVolumeLessThan();
	afx_msg void OnKillfocusPercentTbv();
	afx_msg void OnKillfocusVolumeGreaterThan();
	afx_msg void OnCloseupReplacementFluid();
	afx_msg void OnKillfocusReplacementFluidPercent();
	afx_msg void OnCloseupRbcRatio();
	afx_msg void OnKillfocusPlt4Concentration();
	afx_msg void OnKillfocusPlt4Volume();
	afx_msg void OnKillfocusPlt4Yield();
	afx_msg void OnKillfocusPlt5Concentration();
	afx_msg void OnKillfocusPlt5Volume();
	afx_msg void OnKillfocusPlt5Yield();
	afx_msg void OnKillfocusPlt6Concentration();
	afx_msg void OnKillfocusPlt6Volume();
	afx_msg void OnKillfocusPlt6Yield();
	afx_msg void OnKillfocusRbcCollectVolume1();
	afx_msg void OnKillfocusRbcCollectVolume2();
	afx_msg void OnKillfocusRbcCollectVolume3();
	afx_msg void OnKillfocusRbcCrit2();
	afx_msg void OnKillfocusRbcCrit3();
	afx_msg void OnKillfocusRbcDose1();
	afx_msg void OnKillfocusRbcDose2();
	afx_msg void OnKillfocusRbcDose3();
	afx_msg void OnKillfocusRbcCrit1();
	afx_msg void OnUpdateProduct10();
	afx_msg void OnUpdateProduct1();
	afx_msg void OnUpdateProduct11();
	afx_msg void OnUpdateProduct12();
	afx_msg void OnUpdateProduct13();
	afx_msg void OnUpdateProduct14();
	afx_msg void OnUpdateProduct15();
	afx_msg void OnUpdateProduct2();
	afx_msg void OnUpdateProduct3();
	afx_msg void OnUpdateProduct4();
	afx_msg void OnUpdateProduct5();
	afx_msg void OnUpdateProduct6();
	afx_msg void OnUpdateProduct7();
	afx_msg void OnUpdateProduct8();
	afx_msg void OnUpdateProduct9();
	afx_msg void OnCloseupMaxDrawFlow();
	afx_msg void OnKillfocusReplacementVolumeTrigger();
	afx_msg void OnClose();
	afx_msg void OnCloseupReturnManagement();
	afx_msg void OnKillfocusRbcYsf();
	afx_msg void OnKillfocusMlPerKg();
	afx_msg void OnCloseupInletRamp();
	afx_msg void OnCloseupDrawManagement();
	afx_msg void OnKillfocusCustomRatio();
	afx_msg void OnUpdateRemovalSetting();
	afx_msg void OnChangePlasma1();
	afx_msg void OnChangePlasma2();
	afx_msg void OnChangePlasma3();
	afx_msg void OnChangePlasma4();
	afx_msg void OnChangePlasma5();
	afx_msg void OnChangePlasma6();
	afx_msg void OnChangePlasmaAmapMax();
	afx_msg void OnChangePlasmaAmapMin();
	afx_msg void OnCloseupPlasmaRinseback();
	afx_msg void OnUpdateProduct16();
	afx_msg void OnUpdateProduct17();
	afx_msg void OnUpdateProduct18();
	afx_msg void OnUpdateProduct19();
	afx_msg void OnUpdateProduct20();
	afx_msg void OnPltMssCheck1Click();
	afx_msg void OnPltMssCheck2Click();
	afx_msg void OnPltMssCheck3Click();
	afx_msg void OnPltMssCheck4Click();
	afx_msg void OnPltMssCheck5Click();
	afx_msg void OnPltMssCheck6Click();
	afx_msg void OnRbcMssCheck1Click();
	afx_msg void OnRbcMssCheck2Click();
	afx_msg void OnRbcMssCheck3Click();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(cfg_main_input)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

public:
   //
   //
   //  The one and only global instance of the configuration
   //
   static Config_CDS *userconfig;

   virtual bool update_config(const char *path);
   virtual bool load(); // uses configpath path

   //
   //
   //   You can ask me if I have been modified since I have last saved or
   //   loaded my file.  I return 1 if modifications have been made or 0
   //   if my contents still match the file.
   //
   virtual const int modified() const;
   virtual const CString &path() const { return configpath; }

protected:
   int m_modified;
   CString configpath;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFG_MAIN_INPUT_H__AD0CDA08_BBA2_11D1_B0F8_006097402E62__INCLUDED_)
