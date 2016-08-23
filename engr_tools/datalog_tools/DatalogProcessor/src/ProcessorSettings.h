/*****************************************************************************\
**
** ProcessorSettings.h
**
** Provides implementation of the converter settings.
**
\*****************************************************************************/

#pragma once

namespace DatalogProcessor {

/* Using *********************************************************************/

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

/* Constants *****************************************************************/

public enum class Format {
	Access,
	Excel,
	Hybrid
};

/* Classes *******************************************************************/

public ref class ProcessorSettings {
public:
#pragma region Constructor
	ProcessorSettings(void)
	{
		mDefaultApplication = Format::Hybrid;
		mLaunchApplication = true;
		mAccessFolder = "D:\\Datalogs\\Access";
		mExcelFolder = "D:\\Datalogs\\Excel";
		mAccessFile = "datalog";
		mExcelFile = "datalog";
		mAccessExt = ".accdb";
		mExcelExt = ".xlsx";
		mAccessOverride = false;
		mExcelOverride = false;
	}
#pragma endregion

#pragma region Properties
	property Format DefaultApplication
	{ 
		Format get(void) { return mDefaultApplication; }
		void set(Format app) { mDefaultApplication = app; }
	}

	property bool LaunchApplication
	{
		bool get(void) { return mLaunchApplication; }
		void set(bool launch) { mLaunchApplication = launch; }
	}

	property String^ AccessFolder
	{
		String^ get(void) { return mAccessFolder; }
		void set(String^ folder) { mAccessFolder = folder; }
	}

	property String^ ExcelFolder
	{
		String^ get(void) { return mExcelFolder; }
		void set(String^ folder) { mExcelFolder = folder; }
	}

	property String^ AccessFile
	{
		String^ get(void) { return mAccessFile; }
		void set(String^ file) { mAccessFile = file; }
	}

	property String^ ExcelFile
	{
		String^ get(void) { return mExcelFile; }
		void set(String^ file) { mExcelFile = file; }
	}

	property String^ AccessExt
	{
		String^ get(void) { return mAccessExt; }
		void set(String^ file) { mAccessExt = file; }
	}

	property String^ ExcelExt
	{
		String^ get(void) { return mExcelExt; }
		void set(String^ file) { mExcelExt = file; }
	}

	property bool AccessOverride
	{
		bool get(void) { return mAccessOverride; }
		void set(bool overrideFile) { mAccessOverride = overrideFile; }
	}

	property bool ExcelOverride
	{
		bool get(void) { return mExcelOverride; }
		void set(bool overrideFile) { mExcelOverride = overrideFile; }
	}
#pragma endregion

private:
#pragma region Mempers
	Format mDefaultApplication;
	bool mLaunchApplication;
	String^ mAccessFolder;
	String^ mExcelFolder;
	String^ mAccessFile;
	String^ mExcelFile;
	String^ mAccessExt;
	String^ mExcelExt;
	bool mAccessOverride;
	bool mExcelOverride;
#pragma endregion
};

}