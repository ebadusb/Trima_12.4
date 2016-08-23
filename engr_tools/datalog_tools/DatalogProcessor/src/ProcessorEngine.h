/*****************************************************************************\
**
** ProcessorEngine.h
**
** Provides the implementation of the processor engine.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "ProcessorSettings.h"

namespace DatalogProcessor {

/* Using *********************************************************************/

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Xml;
using namespace System::Xml::Serialization;
using namespace Microsoft::Win32;

/* Classes *******************************************************************/

public ref class ProcessorEngine {
public:
#pragma region Constructor
	ProcessorEngine(void)
	{
		mSettings = gcnew ProcessorSettings();
	}
#pragma endregion

#pragma region Settings
	void LoadSettings(void)
	{
		String ^file = this->GetConfigFolder() + "\\DatalogProcessor.config";

		if (System::IO::File::Exists(file)) {
			XmlTextReader ^reader = nullptr;
			XmlSerializer ^serializer = nullptr;

			try {
				serializer = gcnew XmlSerializer(mSettings->GetType());
				reader = gcnew XmlTextReader(file);
				mSettings = (ProcessorSettings^)serializer->Deserialize(reader);
			}
			catch (Exception ^e) {
				System::Windows::Forms::MessageBox::Show(
						"Could not create \"" + file + "\"",
						"Error",
						System::Windows::Forms::MessageBoxButtons::OK,
						System::Windows::Forms::MessageBoxIcon::Error);
			}

			if (reader != nullptr) {
				reader->Close();
			}
		}
	}

	void StoreSettings(void)
	{
		String ^file = this->GetConfigFolder() + "\\DatalogProcessor.config";
		XmlTextWriter ^writer = nullptr;
		XmlSerializer ^serializer = nullptr;

		try {
			serializer = gcnew XmlSerializer(mSettings->GetType());
			writer = gcnew XmlTextWriter(file, System::Text::Encoding::Default);
			serializer->Serialize(writer, mSettings);
		}
		catch (Exception ^e) {
			System::Windows::Forms::MessageBox::Show(
					"Could not create \"" + file + "\"",
					"Error",
					System::Windows::Forms::MessageBoxButtons::OK,
					System::Windows::Forms::MessageBoxIcon::Error);
		}

		if (writer != nullptr) {
			writer->Close();
		}
	}
#pragma endregion

#pragma region Functions
	void ProcessAccessFile(String ^datalog)
	{
		this->ProcessDatalogFile(datalog, Format::Access);
	}

	void ProcessExcelFile(String ^datalog)
	{
		this->ProcessDatalogFile(datalog, Format::Excel);
	}

	void ProcessHybridFile(String ^datalog)
	{
		this->ProcessDatalogFile(datalog, Format::Hybrid);
	}

	void ProcessDefaultFile(String ^datalog)
	{
		switch (mSettings->DefaultApplication) {
		case Format::Access:
			this->ProcessDatalogFile(datalog, Format::Access);
			break;
		case Format::Excel:
			this->ProcessDatalogFile(datalog, Format::Excel);
			break;
		case Format::Hybrid:
			this->ProcessDatalogFile(datalog, Format::Hybrid);
			break;
		}
	}

	void ProcessTargetFile(String ^datalog, String ^target)
	{
		
	}
#pragma endregion

#pragma region Properties
public:
	property ProcessorSettings ^Settings
	{ 
		ProcessorSettings^ get(void) { return mSettings; }
		void set(ProcessorSettings ^settings) { mSettings = settings; }
	}
#pragma endregion

private:
#pragma region Utility
	void ProcessDatalogFile(String ^log, Format app)
	{
		try {
			this->PrepareDestinationDir(log, app);
			this->ConvertDatalogFile(log, app);
			this->FinalizeDestinationFile(log, app);
		}
		catch (Exception ^e) {
			System::Windows::Forms::MessageBox::Show(
					"Unable to convert datalog.",
					"Error",
					System::Windows::Forms::MessageBoxButtons::OK,
					System::Windows::Forms::MessageBoxIcon::Error);
		}
	}

	void PrepareDestinationDir(String ^log, Format app)
	{
		switch (app) {
		case Format::Access:
			if (!System::IO::Directory::Exists(mSettings->AccessFolder)) {
				System::IO::Directory::CreateDirectory(mSettings->AccessFolder);
			}
			break;
		case Format::Excel:
			if (!System::IO::Directory::Exists(mSettings->ExcelFolder)) {
				System::IO::Directory::CreateDirectory(mSettings->ExcelFolder);
			}
			break;
		case Format::Hybrid:
			if (!System::IO::Directory::Exists(mSettings->AccessFolder)) {
				System::IO::Directory::CreateDirectory(mSettings->AccessFolder);
			}
			if (!System::IO::Directory::Exists(mSettings->ExcelFolder)) {
				System::IO::Directory::CreateDirectory(mSettings->ExcelFolder);
			}
			break;
		}
	}

	void ConvertDatalogFile(String ^log, Format app)
	{
		String ^database = this->GetDatabaseFilename(log, app);

		CONVERT::DatalogConverter::ExtractLogData(log, database);
	}

	void FinalizeDestinationFile(String ^log, Format app)
	{
		String ^database = this->GetDatabaseFilename(log, app);
		String ^filename = this->GetDestinationFilename(log, app);

		ADODB::Connection ^connection = gcnew ADODB::Connection();
		String ^command = AddinUtility::ConnectionUtility::GetConnectionString(filename);
		String ^connect = AddinUtility::ConnectionUtility::GetConnectionString(database);
		String ^drop = "DROP TABLE ConnectionInfo;";
		String ^create = "CREATE TABLE ConnectionInfo (ConnectionString text);";
		String ^insert = "INSERT INTO ConnectionInfo (ConnectionString) VALUES ('" + connect + "');";
		Object ^records = nullptr;			

		connection->Open(command, "", "", 0);

		try { 
			connection->Execute(drop, records, 0);
		}
		catch (Exception ^e) {
			// The table does not exist.
		}

		if (app == Format::Hybrid) {
			try {
				connection->Execute(create, records, 0);
				connection->Execute(insert, records, 0);
			}
			catch (Exception ^e) {
				// Unable to establish link.
			}
		}

		connection->Close();

		if (mSettings->LaunchApplication) {
			System::Diagnostics::Process::Start(filename);
		}
	}

	String ^GetDatabaseFilename(String ^log, Format app)
	{
		String ^filename = "";

		switch (app) {
		case Format::Access:
		case Format::Hybrid:
			filename += mSettings->AccessFolder;
			filename += "\\";
			filename += mSettings->AccessOverride
					? mSettings->AccessFile
					: System::IO::Path::GetFileNameWithoutExtension(log);
			filename += mSettings->AccessExt;
			break;
		case Format::Excel:
			filename += mSettings->ExcelFolder;
			filename += "\\";
			filename += mSettings->ExcelOverride
					? mSettings->ExcelFile
					: System::IO::Path::GetFileNameWithoutExtension(log);
			filename += mSettings->ExcelExt;
			break;
		}

		return filename;
	}	

	String ^GetDestinationFilename(String ^log, Format app)
	{
		String ^filename = "";

		switch (app) {
		case Format::Access:
			filename += mSettings->AccessFolder;
			filename += "\\";
			filename += mSettings->AccessOverride
					? mSettings->AccessFile
					: System::IO::Path::GetFileNameWithoutExtension(log);
			filename += mSettings->AccessExt;
			break;
		case Format::Excel:
		case Format::Hybrid:
			filename += mSettings->ExcelFolder;
			filename += "\\";
			filename += mSettings->ExcelOverride
					? mSettings->ExcelFile
					: System::IO::Path::GetFileNameWithoutExtension(log);
			filename += mSettings->ExcelExt;
			break;
		}

		return filename;
	}

	String ^GetConfigFolder()
	{
		String ^value = Application::LocalUserAppDataPath;

		try {
			RegistryKey ^myKey = Registry::CurrentUser->OpenSubKey("SOFTWARE\\CaridianBCT\\AnalysisToolSet", false);

			if (myKey != nullptr) {
				value = myKey->GetValue("Config Path")->ToString();
			}
		}
		catch (Exception ^e) {
		}

		return value;
	}
#pragma endregion

#pragma region Members
	ProcessorSettings ^mSettings;
#pragma endregion
};

}