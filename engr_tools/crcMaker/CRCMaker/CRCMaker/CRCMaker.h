// CRCMaker.h : main header file for the CRCMAKER application
//

#if !defined(AFX_CRCMAKER_H__237C59B4_35D2_47AF_A694_640AD0C16FC6__INCLUDED_)
#define AFX_CRCMAKER_H__237C59B4_35D2_47AF_A694_640AD0C16FC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCRCMakerApp:
// See CRCMaker.cpp for the implementation of this class
//

class CCRCMakerApp : public CWinApp
{
public:
	CCRCMakerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCRCMakerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCRCMakerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRCMAKER_H__237C59B4_35D2_47AF_A694_640AD0C16FC6__INCLUDED_)
