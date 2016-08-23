// DlgProxy.h : header file
//

#if !defined(AFX_DLGPROXY_H__9DE12E9E_0610_490D_8A44_91DF7E998DD4__INCLUDED_)
#define AFX_DLGPROXY_H__9DE12E9E_0610_490D_8A44_91DF7E998DD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCRCMakerDlg;

/////////////////////////////////////////////////////////////////////////////
// CCRCMakerDlgAutoProxy command target

class CCRCMakerDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CCRCMakerDlgAutoProxy)

	CCRCMakerDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CCRCMakerDlg* m_pDialog;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCRCMakerDlgAutoProxy)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCRCMakerDlgAutoProxy();

	// Generated message map functions
	//{{AFX_MSG(CCRCMakerDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CCRCMakerDlgAutoProxy)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CCRCMakerDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROXY_H__9DE12E9E_0610_490D_8A44_91DF7E998DD4__INCLUDED_)
