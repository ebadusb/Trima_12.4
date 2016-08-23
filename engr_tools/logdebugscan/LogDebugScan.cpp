// LogDebugScan.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LogDebugScan.h"
#include "DebugLogScanner.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 2;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else if ( argc > 1 )
	{
		DebugLogScanner	converter;
	
		HANDLE	handle = CreateFile(argv[1], GENERIC_READ,FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if ( handle != INVALID_HANDLE_VALUE )
		{
			if (argc == 3 
				 && strcmp( argv[2], "s" ) == 0 )
			{
        		converter.Convert(handle, 0x0100000a);
			}
			else
			{
				converter.Convert(handle, 0x0200000a);
			}
			CloseHandle(handle);
			nRetCode = 0;
		}
		else
		{
         cerr << _T("Could not open file ") << _T(argv[1]) << _T(".") << endl;

			nRetCode = 3;
		}
	}

	return nRetCode;
}


