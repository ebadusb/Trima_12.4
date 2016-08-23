// CRCMakerDlg.h : header file
//

#if !defined(AFX_CRCMAKERDLG_H__BF58116A_5A8B_47DB_9D4D_4A6B1CC4120D__INCLUDED_)
#define AFX_CRCMAKERDLG_H__BF58116A_5A8B_47DB_9D4D_4A6B1CC4120D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCRCMakerDlgAutoProxy;

/////////////////////////////////////////////////////////////////////////////
// CCRCMakerDlg dialog

class CCRCMakerDlg : public CDialog
{
	DECLARE_DYNAMIC(CCRCMakerDlg);
	friend class CCRCMakerDlgAutoProxy;

// Construction
public:
	CCRCMakerDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CCRCMakerDlg();

// Dialog Data
	//{{AFX_DATA(CCRCMakerDlg)
	enum { IDD = IDD_CRCMAKER_DIALOG };
	CEdit	m_stringInput;
	CEdit	m_filenameInput;
	CButton	m_doCRC32;
	CEdit	m_crcHexOutput;
	CEdit	m_crcDecOutput;
	CButton	m_browseButton;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCRCMakerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CCRCMakerDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

    BOOL  _doFile;
	CString _path;
	CString _file;

	// Generated message map functions
	//{{AFX_MSG(CCRCMakerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OndoFileRadio();
	afx_msg void OndoStringRadio();
	afx_msg void OnBrowse();
	afx_msg void OndoCRC32();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRCMAKERDLG_H__BF58116A_5A8B_47DB_9D4D_4A6B1CC4120D__INCLUDED_)
