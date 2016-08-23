// DecodeDlog.cpp: implementation of the CDecodeDlog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <fstream>
#include "ap2 predictionater.h"
#include "DecodeDlog.h"
#include "SplitPath.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDecodeDlog::CDecodeDlog()
{
	TCHAR szCurrentDirectory[256];
	::GetCurrentDirectory(sizeof(szCurrentDirectory) - 1, szCurrentDirectory);
	m_CWD = CString(szCurrentDirectory);
}

CDecodeDlog::~CDecodeDlog()
{

}

CString CDecodeDlog::Decode(CString source)
{
	CSplitPath objSplitPath((LPCSTR)source);
	//CString decodedFile = _T("C:\\temp\\predict\\") + objSplitPath.GetFileName() + objSplitPath.GetExtension() + ".csv";
	CString decodedFile = _T("C:\\temp\\") + objSplitPath.GetFileName() + objSplitPath.GetExtension() + ".csv";
	std::ifstream rundata(decodedFile,std::ios::in | std::ios::binary);
	if (rundata.fail()) { 
		//Shell out and call dld gui here. User must have a 'registered' version of 
		//DLDGui.
		SHELLEXECUTEINFO sei = {0};
		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
		sei.fMask = SEE_MASK_FLAG_NO_UI;
		sei.hwnd = 0;
		sei.lpVerb = "open"; //"open" ==> runs the dldgui.exe program
		//This is the default location for dld gui.
		sei.lpFile = "C:\\Program Files\\Gambro BCT\\DLDGui\\DLDGui.exe";
		CString clParams =  m_CWD  + _T("\\predict.dld ") + source;//+ _T(_T(" > ") + path + _T(".csv"));
		sei.lpParameters = clParams;
		sei.nShow = SW_SHOWMINNOACTIVE;
		sei.cbSize = sizeof(sei);
		sei.hInstApp = NULL;
		bool result = ShellExecuteEx(&sei);
		DWORD error;
		if(!result){
			error = GetLastError();
			//TODO: Show a dialog box after extracting
			//proper error message.
			return "";
		}
		//Why don't you wait WaitForSingleObject????
		WaitForSingleObject(sei.hProcess, 10000);
		//Heck ..I'll force you to wait.
		//TODO: figure out why it doesn't wait
		Sleep(10000);
	}
	return decodedFile;
}
