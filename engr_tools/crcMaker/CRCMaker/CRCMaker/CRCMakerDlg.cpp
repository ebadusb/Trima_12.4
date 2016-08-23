// CRCMakerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CRCMaker.h"
#include "CRCMakerDlg.h"
#include "DlgProxy.h"
#include "crcgen.h"
#include <string>
 using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCRCMakerDlg dialog

IMPLEMENT_DYNAMIC(CCRCMakerDlg, CDialog);

CCRCMakerDlg::CCRCMakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCRCMakerDlg::IDD, pParent), _doFile(false)
{
	//{{AFX_DATA_INIT(CCRCMakerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CCRCMakerDlg::~CCRCMakerDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}

void CCRCMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCRCMakerDlg)
	DDX_Control(pDX, IDC_string, m_stringInput);
	DDX_Control(pDX, IDC_filename, m_filenameInput);
	DDX_Control(pDX, IDC_doCRC32, m_doCRC32);
	DDX_Control(pDX, IDC_crcHex, m_crcHexOutput);
	DDX_Control(pDX, IDC_Browse, m_browseButton);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCRCMakerDlg, CDialog)
	//{{AFX_MSG_MAP(CCRCMakerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_doFileRadio, OndoFileRadio)
	ON_BN_CLICKED(IDC_doStringRadio, OndoStringRadio)
	ON_BN_CLICKED(IDC_Browse, OnBrowse)
	ON_BN_CLICKED(IDC_doCRC32, OndoCRC32)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCRCMakerDlg message handlers

BOOL CCRCMakerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
 

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
		m_stringInput.EnableWindow(false);
    m_filenameInput.EnableWindow(false);
    m_browseButton.EnableWindow(false);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCRCMakerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCRCMakerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CCRCMakerDlg::OnClose() 
{
	if (CanExit())
		CDialog::OnClose();
}

void CCRCMakerDlg::OnOK() 
{
	if (CanExit())
		CDialog::OnOK();
}

void CCRCMakerDlg::OnCancel() 
{
	if (CanExit())
		CDialog::OnCancel();
}

BOOL CCRCMakerDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

void CCRCMakerDlg::OndoFileRadio() 
{
_doFile = true;
	m_stringInput.EnableWindow(false);
    m_filenameInput.EnableWindow(true);
    m_browseButton.EnableWindow(true);

}

void CCRCMakerDlg::OndoStringRadio() 
{
	_doFile = false;
	m_stringInput.EnableWindow(true);
    m_filenameInput.EnableWindow(false);
    m_browseButton.EnableWindow(false);
	
}

void CCRCMakerDlg::OnBrowse() 
{
	char szFilter[] = "All Files(*.*)|*.*||";
	CFileDialog fd(1,NULL,NULL,NULL,szFilter);
	fd.DoModal();
	_file = fd.GetFileName();
	_path = fd.GetPathName();
	m_filenameInput.SetWindowText( _path );
}

void CCRCMakerDlg::OndoCRC32() 
{
	CString crcString = "";
   	CString crcHexString = "";
    CString s1 = "";
	unsigned long  crc = 0;

	 if (_doFile)
     {   

		  crc = 0xFFFFFFFFL; // seed
		 m_filenameInput.GetWindowText(s1);
         const char* data_char = (LPCTSTR) s1;
        if (file_crcgen32 ( data_char, &crc) == -1)		 
             m_filenameInput.SetWindowText( "ERROR");


     }
	 else
	 {

	      m_stringInput.GetWindowText(s1);

          const char* data_char = (LPCTSTR) s1;
         if (crcgen32(&crc, (unsigned char *)data_char, strlen(data_char)) == -1)
			  m_stringInput.SetWindowText( "ERROR");
     }


	crcString.Format("%d", crc);

	crcHexString.Format("%x", crc);
    m_crcHexOutput.SetWindowText(crcHexString);

}
