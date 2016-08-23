//////////////////////////////////////////////////////////////////////
// SplitPath.h : interface for the CSplitPath class.                //
// (c) 1999, Kevin Lussier											//
//(http://www.codeproject.com/cpp/csplitpath.asp)                   //
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITPATH_H__2DD9EEC9_54D6_4252_A0D5_DF7B4AD14B1F__INCLUDED_)
#define AFX_SPLITPATH_H__2DD9EEC9_54D6_4252_A0D5_DF7B4AD14B1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSplitPath
{ 
// Construction
public: 
    CSplitPath( LPCTSTR lpszPath = NULL );

// Operations
    BOOL    Split(LPCTSTR lpszPath );
    CString GetPath( void ) { return path_buffer; }
    CString GetDrive( void ) { return drive; }
    CString GetDirectory( void ) { return dir; }
    CString GetFileName( void ) { return fname; }
    CString GetExtension( void ) { return ext; }

// Attributes
protected:
    TCHAR path_buffer[_MAX_PATH];
    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];
}; 


#endif // !defined(AFX_SPLITPATH_H__2DD9EEC9_54D6_4252_A0D5_DF7B4AD14B1F__INCLUDED_)
