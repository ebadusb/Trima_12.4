// DecodeDlog.h: interface for the CDecodeDlog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DECODEDLOG_H__528F4871_601F_4E4F_A68D_FEA3BBBE2F94__INCLUDED_)
#define AFX_DECODEDLOG_H__528F4871_601F_4E4F_A68D_FEA3BBBE2F94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDecodeDlog  
{
public:
	/* Current working directory */
	CString m_CWD;
	CString Decode(CString source);
	CDecodeDlog();
	virtual ~CDecodeDlog();

};

#endif // !defined(AFX_DECODEDLOG_H__528F4871_601F_4E4F_A68D_FEA3BBBE2F94__INCLUDED_)
