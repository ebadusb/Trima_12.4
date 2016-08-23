/*****************************************************************************\
**
** WizardConfig.h
**
** Provides the implementation for the wizard configurations.
**
\*****************************************************************************/

#pragma once

/* Class *********************************************************************/

namespace WIZARD {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;

public ref class WizardConfig {
#pragma region Construction
public:
	WizardConfig(void)
	{
		mProcassMap = gcnew ProcessMap();
		mTemplateFilename = gcnew String("");
		mPreImportScript = gcnew String("");
		mPostImportScript = gcnew String("");
		mPreExportScript = gcnew String("");
		mPostExportScript = gcnew String("");
	}

protected:
	~WizardConfig(void)
	{
	}
#pragma endregion

#pragma region Types
	typedef System::Collections::Generic::Dictionary<String^, bool> ProcessMap;
#pragma endregion

#pragma region Properties
public:
	property ProcessMap ^DataProcessing
	{
		ProcessMap ^get(void) { return mProcassMap; }
		void set(ProcessMap ^map) { mProcassMap = map; }
	}
	property String ^TemplateFilename
	{
		String ^get(void) { return mTemplateFilename; }
		void set(String ^filename) { mTemplateFilename = filename; }
	}
	property String ^PreImportScript
	{
		String ^get(void) { return mPreImportScript; }
		void set(String ^script) { mPreImportScript = script; }
	}
	property String ^PostImportScript
	{
		String ^get(void) { return mPostImportScript; }
		void set(String ^script) { mPostImportScript = script; }
	}
	property String ^PreExportScript
	{
		String ^get(void) { return mPreExportScript; }
		void set(String ^script) { mPreExportScript = script; }
	}
	property String ^PostExportScript
	{
		String ^get(void) { return mPostExportScript; }
		void set(String ^script) { mPostExportScript = script; }
	}
#pragma endregion

#pragma region Members
private: ProcessMap ^mProcassMap;
private: String ^mTemplateFilename;
private: String ^mPreImportScript;
private: String ^mPostImportScript;
private: String ^mPreExportScript;
private: String ^mPostExportScript;
#pragma endregion
};

} // namespace WIZARD
