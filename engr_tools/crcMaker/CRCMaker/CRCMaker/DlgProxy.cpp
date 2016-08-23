// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "CRCMaker.h"
#include "DlgProxy.h"
#include "CRCMakerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCRCMakerDlgAutoProxy

IMPLEMENT_DYNCREATE(CCRCMakerDlgAutoProxy, CCmdTarget)

CCRCMakerDlgAutoProxy::CCRCMakerDlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF(CCRCMakerDlg, AfxGetApp()->m_pMainWnd);
	m_pDialog = (CCRCMakerDlg*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

CCRCMakerDlgAutoProxy::~CCRCMakerDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CCRCMakerDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CCRCMakerDlgAutoProxy, CCmdTarget)
	//{{AFX_MSG_MAP(CCRCMakerDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CCRCMakerDlgAutoProxy, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CCRCMakerDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ICRCMaker to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {F30042B5-F236-4229-84ED-5A4645650743}
static const IID IID_ICRCMaker =
{ 0xf30042b5, 0xf236, 0x4229, { 0x84, 0xed, 0x5a, 0x46, 0x45, 0x65, 0x7, 0x43 } };

BEGIN_INTERFACE_MAP(CCRCMakerDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CCRCMakerDlgAutoProxy, IID_ICRCMaker, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {3054ED1B-C493-4581-973D-28D1338F089A}
IMPLEMENT_OLECREATE2(CCRCMakerDlgAutoProxy, "CRCMaker.Application", 0x3054ed1b, 0xc493, 0x4581, 0x97, 0x3d, 0x28, 0xd1, 0x33, 0x8f, 0x8, 0x9a)

/////////////////////////////////////////////////////////////////////////////
// CCRCMakerDlgAutoProxy message handlers
