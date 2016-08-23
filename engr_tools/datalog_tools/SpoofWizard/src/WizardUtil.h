/*****************************************************************************\
**
** WizardUtil.h
**
** Provides the implementation for the wizard utility.
**
\*****************************************************************************/

#pragma once

/* Class *********************************************************************/

namespace WIZARD {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Xml;

public ref class WizardUtil {
#pragma region Construction
public:
	WizardUtil(void)
	{
	}

protected:
	~WizardUtil(void)
	{
	}
#pragma endregion

public:
	/**
	 *
	 * ExecuteScript
	 *
	 * Executes the given command.
	 *
	 */
	static System::Void ExecuteScript(String ^script, String ^file)
	{
		array<Char> ^delim = { ' ' };
		array<Char> ^trim = { '.' };
		String ^string = System::String::Copy(script)->Trim();

		if (!String::IsNullOrEmpty(string)) {
			int index = string->IndexOfAny(delim);

			if (index == -1) {
				index = string->Length;
			}

			String ^command = string->Substring(0, index)->Trim();
			String ^params = string->Substring(index, string->Length - index)->Trim();
			String ^fullname = file;
			String ^pathname = System::IO::Path::GetDirectoryName(file);
			String ^filename = System::IO::Path::GetFileNameWithoutExtension(file);
			String ^extension = System::IO::Path::GetExtension(file)->Trim(trim);

			params = params->Replace("<Fullname>", fullname);
			params = params->Replace("<Pathname>", pathname);
			params = params->Replace("<Filename>", filename);
			params = params->Replace("<Extension>", extension);

			try {
				System::Diagnostics::Process::Start(command, params);
			}
			catch (...) {
				MessageBox::Show(command + " " + params, 
						"Script Error", 
						MessageBoxButtons::OK, 
						MessageBoxIcon::Error);
			}
		}
	}

	/**
	 *
	 * ProcessBuilderError
	 *
	 * Processes the given error.
	 *
	 */
	static System::Void ProcessBuilderError(BUILDER::SpoofBuilder::Result result)
	{
		String ^title = nullptr;
		String ^message = nullptr;

		switch (result) {
		case BUILDER::SpoofBuilder::kResultDLogFileAlreadyOpen:
			title = "Import Error";
			message = "Datalog file is already open.";
			break;
		case BUILDER::SpoofBuilder::kResultDLogFileNotOpen:
			title = "Import Error";
			message = "Datalog file is not open.";
			break;
		case BUILDER::SpoofBuilder::kResultDLogFileOpenFailed:
			title = "Import Error";
			message = "Failed to open datalog file.";
			break;
		case BUILDER::SpoofBuilder::kResultDLogInvalidHeaderType:
			title = "Import Error";
			message = "Invalid datalog header.";
			break;
		case BUILDER::SpoofBuilder::kResultDLogInvalidVersion:
			title = "Import Error";
			message = "Invalid datalog version.";
			break;
		case BUILDER::SpoofBuilder::kResultDLogUnexpectedEOF:
			title = "Import Error";
			message = "Unexpected end of file while reading datalog.";
			break;
		case BUILDER::SpoofBuilder::kResultDLogUnknownRecordType:
			title = "Import Error";
			message = "Unknown record type while reading datalog.";
			break;
		case BUILDER::SpoofBuilder::kResultDLogUnknownResult:
			title = "Import Error";
			message = "Unknown error while reading datalog.";
			break;
		case BUILDER::SpoofBuilder::kResultTemplateReadFailed:
			title = "Export Error";
			message = "Unable to read template file.";
			break;
		case BUILDER::SpoofBuilder::kResultSourceWriteFailed:
			title = "Export Error";
			message = "Unable to write spoof file.";
			break;
		default:
			title = "Unknown error";
			message = "The Spoof Wixard has encountered an unknown error.";
			break;
		}

		if (result != BUILDER::SpoofBuilder::kResultNone) {
			MessageBox::Show(message, title, MessageBoxButtons::OK, MessageBoxIcon::Error);
		}
	}

	/**
	 *
	 * ImportConfigs
	 *
	 * Imports the wizard configurations.
	 *
	 */
	static System::Void ImportConfigs(String ^file, WizardConfig ^config)
	{
		XmlDocument ^doc = gcnew XmlDocument();

		doc->Load(file);

		XmlNodeList ^dataNodeList = doc->GetElementsByTagName("Data");
		XmlNodeList ^fileNodeList = doc->GetElementsByTagName("File");
		XmlNodeList ^scriptNodeList = doc->GetElementsByTagName("Script");
  
		for (int i = 0; i < dataNodeList->Count; ++i) {
			XmlElement ^element = dynamic_cast<XmlElement^>(dataNodeList[i]);
			String ^key = element->GetAttribute("dataname");
			String ^value = element->GetAttribute("active");
 
			config->DataProcessing[key] = Boolean::Parse(value);
		}

		for (int i = 0; i < fileNodeList->Count; ++i) {
			XmlElement ^element = dynamic_cast<XmlElement^>(fileNodeList[i]);
			String ^file = element->GetAttribute("filename");
			String ^purpose = element->GetAttribute("purpose");	

			if (purpose == "SpoofConfig") {
				config->TemplateFilename = file;
			}
		}

		for (int i = 0; i < scriptNodeList->Count; ++i) {
			XmlElement ^element = dynamic_cast<XmlElement^>(scriptNodeList[i]);
			String ^command = element->GetAttribute("command");
			String ^execute = element->GetAttribute("execute");	

			if (execute == "Pre-Import") {
				config->PreImportScript = command;
			}
			else if (execute == "Post-Import") {
				config->PostImportScript = command;
			}
			else if (execute == "Pre-Export") {
				config->PreExportScript = command;
			}
			else if (execute == "Post-Export") {
				config->PostExportScript = command;
			}
		}
	}

	/**
	 *
	 * ExportConfigs
	 *
	 * Exports the wizard configurations.
	 *
	 */
	static System::Void ExportConfigs(String ^file, WizardConfig ^config)
	{
		XmlDocument ^doc = gcnew XmlDocument();
		XmlDeclaration ^dec = doc->CreateXmlDeclaration("1.0", nullptr, nullptr);
		XmlElement ^root = doc->CreateElement("TrimaSpoofWizard");
		XmlElement ^processDataNode = doc->CreateElement("ProcessData");
		XmlElement ^templatesFilesNode = doc->CreateElement("TemplatesFiles");
		XmlElement ^externalScriptsNode = doc->CreateElement("ExternalScripts");

		doc->AppendChild(dec);
        doc->AppendChild(root);

		root->AppendChild(processDataNode);
		root->AppendChild(templatesFilesNode);
		root->AppendChild(externalScriptsNode);

		Dictionary<String^, bool> ^map = config->DataProcessing;
		Dictionary<String^, bool>::Enumerator enumerator = map->GetEnumerator(); 

		for (; enumerator.MoveNext();) {
			KeyValuePair<String^, bool> ^data = enumerator.Current;
			XmlElement ^node = doc->CreateElement("Data");

			node->SetAttribute("dataname", data->Key);
			node->SetAttribute("active", data->Value.ToString());

			processDataNode->AppendChild(node);
		}

		if (!String::IsNullOrEmpty(config->TemplateFilename->Trim())) {
			XmlElement ^node = doc->CreateElement("File");

			node->SetAttribute("filename", config->TemplateFilename->Trim());
			node->SetAttribute("purpose", "SpoofConfig");

			templatesFilesNode->AppendChild(node);
		}

		if (!String::IsNullOrEmpty(config->PreImportScript->Trim())) {
			XmlElement ^node = doc->CreateElement("Script");

			node->SetAttribute("command", config->PreImportScript->Trim());
			node->SetAttribute("execute", "Pre-Import");

			externalScriptsNode->AppendChild(node);
		}

		if (!String::IsNullOrEmpty(config->PostImportScript->Trim())) {
			XmlElement ^node = doc->CreateElement("Script");

			node->SetAttribute("command", config->PostImportScript->Trim());
			node->SetAttribute("execute", "Post-Import");

			externalScriptsNode->AppendChild(node);
		}

		if (!String::IsNullOrEmpty(config->PreExportScript->Trim())) {
			XmlElement ^node = doc->CreateElement("Script");

			node->SetAttribute("command", config->PreExportScript->Trim());
			node->SetAttribute("execute", "Pre-Export");

			externalScriptsNode->AppendChild(node);
		}

		if (!String::IsNullOrEmpty(config->PostExportScript->Trim())) {
			XmlElement ^node = doc->CreateElement("Script");

			node->SetAttribute("command", config->PostExportScript->Trim());
			node->SetAttribute("execute", "Post-Export");

			externalScriptsNode->AppendChild(node);
		}

		doc->Save(file);
	}
};

} // namespace WIZARD
