/*****************************************************************************\
**
** ExcelAddin.c
**
** Provides the addin registration.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include <windows.h>
#include <xlcall.h>
#include <framewrk.h>

#include "ExcelAddin.h"

/* Globals *******************************************************************/

HANDLE gInstHandle = NULL;
 
/* Functions *****************************************************************/

int AreStringsEqual(LPWSTR s, LPWSTR t)
{
	if (wcslen(s) != *t) {
		return FALSE;
	}

	for (int i = 1; i <= s[0]; ++i) {
		if (towlower(s[i-1]) != towlower(t[i])) {
			return FALSE;
		}
	}							

	return TRUE;
}

extern "C" BOOL APIENTRY DllMain(HANDLE hDLL, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		gInstHandle = hDLL;
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		break;
	}

	return TRUE;
}

extern "C" __declspec(dllexport) int WINAPI xlAutoOpen(void)
{
	XLOPER12 xDLL = { 0 };

	Excel12f(xlGetName, &xDLL, 0);

	for (int i = 0; i < kFunctionCount; ++i) {
		Excel12f(xlfRegister, 0, 11, &xDLL,
			TempStr12(kFunctionList[i][0]),
			TempStr12(kFunctionList[i][1]),
			TempStr12(kFunctionList[i][2]),
			TempStr12(kFunctionList[i][3]),
			TempStr12(kFunctionList[i][4]),
			TempStr12(kFunctionList[i][5]),
			TempStr12(kFunctionList[i][6]),
			TempStr12(kFunctionList[i][7]),
			TempStr12(kFunctionList[i][8]),
			TempStr12(kFunctionList[i][9]));
	}

	return 1;
}

extern "C" __declspec(dllexport) int WINAPI xlAutoClose(void)
{
	for (int i = 0; i < kFunctionCount; i++) {
		Excel12f(xlfSetName, 0, 1, TempStr12(kFunctionList[i][2]));
	}

	return 1;
}

extern "C" __declspec(dllexport) LPXLOPER12 WINAPI xlAutoRegister12(LPXLOPER12 pxName)
{
	XLOPER12 xDLL = { 0 };
	XLOPER12 *xRegId = new XLOPER12;

	xRegId->xltype = xltypeErr;
	xRegId->xltype |= xlbitXLFree;
	xRegId->val.err = xlerrValue;

	for (int i = 0; i < kFunctionCount; ++i) {
		if (AreStringsEqual(kFunctionList[i][0], pxName->val.str)) {
			Excel12f(xlfRegister, 0, 11, &xDLL,
				TempStr12(kFunctionList[i][0]),
				TempStr12(kFunctionList[i][1]),
				TempStr12(kFunctionList[i][2]),
				TempStr12(kFunctionList[i][3]),
				TempStr12(kFunctionList[i][4]),
				TempStr12(kFunctionList[i][5]),
				TempStr12(kFunctionList[i][6]),
				TempStr12(kFunctionList[i][7]),
				TempStr12(kFunctionList[i][8]),
				TempStr12(kFunctionList[i][9]));

			/// Free oper returned by xl
			Excel12f(xlFree, 0, 1, &xDLL);

			return xRegId;
		}
	}

	return xRegId;
}

extern "C" __declspec(dllexport) int WINAPI xlAutoAdd(void)
{
	return 1;
}

extern "C" __declspec(dllexport) int WINAPI xlAutoRemove(void)
{
	return 1;
}

extern "C" __declspec(dllexport) LPXLOPER12 WINAPI xlAddInManagerInfo12(LPXLOPER12 xAction)
{
	XLOPER12 xIntAction = { 0 };
	XLOPER12 *xInfo = new XLOPER12;

	Excel12f(xlCoerce, &xIntAction, 2, xAction, TempInt12(xltypeInt));

	if (xIntAction.val.w == 1) {
		xInfo->xltype = xltypeStr;
		xInfo->xltype |= xlbitXLFree;
		xInfo->val.str = L"\022Trima Predict 6.3 DLL";
	}
	else {
		xInfo->xltype = xltypeErr;
		xInfo->xltype |= xlbitXLFree;
		xInfo->val.err = xlerrValue;
	}

	return xInfo;
}

extern "C" __declspec(dllexport) void WINAPI xlAutoFree12(LPXLOPER12 xAction)
{
}