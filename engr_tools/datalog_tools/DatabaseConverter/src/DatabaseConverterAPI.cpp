/*****************************************************************************\
**
** DatabaseConverter.h
**
** Extracts the datalog data into an database using ADO.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include "DatabaseConverter.h"

#if MANAGED
#include "DatabaseSecurity.h"

using namespace System::Runtime::InteropServices;
#endif

namespace CONVERT {

/* Macros ********************************************************************/

#define DLLEXPORT extern "C" __declspec(dllexport)

/* Functions *****************************************************************/

#if MANAGED
public interface class IDatalogConverter {
	int Process(System::String ^logFile, System::String ^datfile);
};

[GuidAttribute("8BEF95EB-8670-4e66-BA4A-D2B8D16F6076")] 
[System::Runtime::InteropServices::ComDefaultInterface(IDatalogConverter::typeid)]
public ref class DatalogConverter : public IDatalogConverter {
public:
	[System::Runtime::InteropServices::ComRegisterFunctionAttribute]
	static void RegisterFunction(System::Type ^type)
	{
		Microsoft::Win32::Registry::ClassesRoot->CreateSubKey(GetSubKeyName(type));
	}

	[System::Runtime::InteropServices::ComUnregisterFunctionAttribute]
	static void UnregisterFunction(System::Type ^type)
	{
		Microsoft::Win32::Registry::ClassesRoot->DeleteSubKey(GetSubKeyName(type), false);
	}

	static System::String ^GetSubKeyName(System::Type ^type)
	{
		return "CLSID\{" + type->GUID.ToString()->ToUpper() + "}\Programmable";
	}

	virtual int Process(System::String ^logFile, System::String ^datfile)
	{
		using namespace System::Runtime::InteropServices;

		DECODER::Result results = CONVERT::DatabaseConverter::kResultProductUnregistered;

		if (DatabaseSecurity::IsProductActivated()) {
			CoInitialize(NULL);

			System::IntPtr logFileP = Marshal::StringToHGlobalAnsi(logFile);
			System::IntPtr datfileP = Marshal::StringToHGlobalAnsi(datfile);

			results = CONVERT::DatabaseConverter::ExtractLogData((char*)logFileP.ToPointer(), (char*)datfileP.ToPointer());

			Marshal::FreeHGlobal(logFileP);
			Marshal::FreeHGlobal(datfileP);

			CoUninitialize();
		}

		return results;
	}

	static int ExtractLogData(System::String ^logFile, System::String ^datfile)
	{
		DatalogConverter ^converter = gcnew DatalogConverter();

		return converter->Process(logFile, datfile);
	}
};
#else
DLLEXPORT int WINAPI ExtractLogData(LPCSTR logFile, LPCSTR datfile)
{
	CoInitialize(NULL);

	DECODER::Result results = CONVERT::DatabaseConverter::ExtractLogData(logFile, datfile);

	CoUninitialize();

	return results;
}
#endif

} // namespace CONVERT


