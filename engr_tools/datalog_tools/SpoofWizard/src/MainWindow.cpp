/*****************************************************************************\
**
** ConfigurationRedGreen.h
**
** Provides the implementation for red/green configurations.
**
\*****************************************************************************/

/* Includes ******************************************************************/

#include "MainWindow.h"
#include "GeneratorRedGreen.h"
#include "GeneratorSystemStates.h"
#include "GeneratorAlarmStates.h"
#include "GeneratorRecoveryStates.h"
#include "ConfigurationMain.h"
#include "ConfigurationTimeFrame.h"
#include "ConfigurationRedGreen.h"
#include "WizardUtil.h"

namespace WIZARD {

using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;

/* Constants *****************************************************************/

/* Functions *****************************************************************/

/**
 *
 * MainWindow
 *
 * Constructor.
 *
 */
MainWindow::MainWindow(void) : mSpoofBuilderP(new BUILDER::SpoofBuilder)
{
	typedef Dictionary<String^, ImportGeneratorPtr^> ImportGeneratorDict;
	typedef Dictionary<String^, ExportGeneratorPtr^> ExportGeneratorDict;
	typedef Dictionary<String^, ConfigPage^> PageDict;
	typedef AutoPtr<BUILDER::ImportGenerator> ImportGenerator;
	typedef AutoPtr<BUILDER::ExportGenerator> ExportGenerator;

	String ^textMain = "Main"; 
	String ^textTime = "Time Frame"; 
	String ^textReflector = "Red/Green"; 
	String ^textSystemStates = "System States"; 
	String ^textAlarmStates = "Alarm States"; 
	String ^textRecoveryStates = "Recovery States"; 

	this->InitializeComponent();

	mWizardConfig = gcnew WizardConfig();
	mSettingsDialog = gcnew SettingsDialog();
	mOpenFileDialog = gcnew OpenFileDialog();
	mLoadFileDialog = gcnew OpenFileDialog();
	mSaveFileDialog	= gcnew SaveFileDialog();
	mStoreFileDialog = gcnew SaveFileDialog();

	// Set up dialogs.
	mOpenFileDialog->Title = "Import a Datalog File";
	mOpenFileDialog->Filter = "Datalog Files|*.dlog";
	mLoadFileDialog->Title = "Load Wizard Configuration File";
	mLoadFileDialog->Filter = "Config Files|*.xml";
	mSaveFileDialog->Title = "Export a Spoof Config File";
	mSaveFileDialog->Filter = "Spoof Files|*.cpp";
	mStoreFileDialog->Title = "Store Wizard Configuration File";
	mStoreFileDialog->Filter = "Config Files|*.xml";
	mSettingsDialog->Config = mWizardConfig;

	// Create all generators.
	BUILDER::TimeFrameMgr *timeFrameMgr = new BUILDER::TimeFrameMgr();
	BUILDER::GeneratorRedGreen *generatorRedGreen = new BUILDER::GeneratorRedGreen(timeFrameMgr);
	BUILDER::GeneratorSystemStates *generatorSystemStates = new BUILDER::GeneratorSystemStates();
	BUILDER::GeneratorAlarmStates *generatorAlarmStates = new BUILDER::GeneratorAlarmStates();
	BUILDER::GeneratorRecoveryStates *generatorRecoveryStates = new BUILDER::GeneratorRecoveryStates();

	// Add the pointers to the pointer list. These will be deleted automatically.
	mPointerList.Add(gcnew AutoPtr<BUILDER::GeneratorRedGreen>(generatorRedGreen));
	mPointerList.Add(gcnew AutoPtr<BUILDER::GeneratorSystemStates>(generatorSystemStates));
	mPointerList.Add(gcnew AutoPtr<BUILDER::GeneratorAlarmStates>(generatorAlarmStates));
	mPointerList.Add(gcnew AutoPtr<BUILDER::GeneratorRecoveryStates>(generatorRecoveryStates));
	mPointerList.Add(gcnew AutoPtr<BUILDER::TimeFrameMgr>(timeFrameMgr));

	// Add import generators to the generator map
	mImportGeneratorMap.Add(textReflector, gcnew ImportGeneratorPtr(generatorRedGreen));
	mImportGeneratorMap.Add(textSystemStates, gcnew ImportGeneratorPtr(generatorSystemStates));
	mImportGeneratorMap.Add(textAlarmStates, gcnew ImportGeneratorPtr(generatorAlarmStates));
	mImportGeneratorMap.Add(textRecoveryStates, gcnew ImportGeneratorPtr(generatorRecoveryStates));

	// Add export generators to the generator map
	mExportGeneratorMap.Add(textReflector, gcnew ExportGeneratorPtr(generatorRedGreen));

	// Add configuration pages to pages map.
	mConfigurationMap.Add(textMain, gcnew ConfigurationMain(mSpoofBuilderP.Get()));
	mConfigurationMap.Add(textTime, gcnew ConfigurationTimeFrame(timeFrameMgr, 
			generatorSystemStates, generatorAlarmStates, generatorRecoveryStates));
	mConfigurationMap.Add(textReflector, gcnew ConfigurationRedGreen(generatorRedGreen));

	// Register all import generators
	for (ImportGeneratorDict::Enumerator e = mImportGeneratorMap.GetEnumerator(); e.MoveNext();) {
		mSpoofBuilderP->IncludeImportGenerator(*e.Current.Value);
	}

	// Register all export generators
	for (ExportGeneratorDict::Enumerator e = mExportGeneratorMap.GetEnumerator(); e.MoveNext();) {
		mSpoofBuilderP->IncludeExportGenerator(*e.Current.Value);
	}

	// Load the pages
	for (PageDict::Enumerator e = mConfigurationMap.GetEnumerator(); e.MoveNext();) {
		TabPage ^page = gcnew TabPage(e.Current.Key);

		page->Name = e.Current.Key;
		page->Controls->Add(e.Current.Value->GetControl());

		mConfigurationDroplist->Items->Add(e.Current.Key);
		mTabControl->TabPages->Add(page);
	}

	// Add default settings to the config 
	mWizardConfig->DataProcessing[textReflector] = true;

	// Set the default selections
	mTabControl->SelectTab(textMain);
	mConfigurationDroplist->SelectedItem = textMain;
}

/**
 *
 * ~MainWindow
 *
 * Destructor.
 *
 */
MainWindow::~MainWindow(void)
{
	typedef Dictionary<String^, ImportGeneratorPtr^> ImportGeneratorDict;
	typedef Dictionary<String^, ExportGeneratorPtr^> ExportGeneratorDict;

	// Unregister all import generators
	for (ImportGeneratorDict::Enumerator e = mImportGeneratorMap.GetEnumerator(); e.MoveNext();) {
		mSpoofBuilderP->ExcludeImportGenerator(*e.Current.Value);
	}

	// Unregister all export generators
	for (ExportGeneratorDict::Enumerator e = mExportGeneratorMap.GetEnumerator(); e.MoveNext();) {
		mSpoofBuilderP->ExcludeExportGenerator(*e.Current.Value);
	}

	if (components) {
		delete components;
	}
}

/**
 *
 * ExitHandler
 *
 * Handles the menu exit option.
 *
 */
System::Void MainWindow::ExitHandler(System::Object^  sender, System::EventArgs^  e) 
{
	Application::Exit();
}

/**
 *
 * ImportHandler
 *
 * Handles the menu import option.
 *
 */
System::Void MainWindow::ImportHandler(System::Object^  sender, System::EventArgs^  e) 
{
	typedef Dictionary<String^, ConfigPage^> PageDict;

	if (mOpenFileDialog->ShowDialog() == Windows::Forms::DialogResult::OK) {
		WizardUtil::ExecuteScript(mWizardConfig->PreImportScript, mOpenFileDialog->FileName);

		System::IntPtr intPtr = Marshal::StringToHGlobalAnsi(mOpenFileDialog->FileName);
		const char *filename = static_cast<const char*>(intPtr.ToPointer());
		BUILDER::SpoofBuilder::Result result = mSpoofBuilderP->ImportLogData(filename);

		if (result != BUILDER::SpoofBuilder::kResultNone) {
			WizardUtil::ProcessBuilderError(result);
		}

		Marshal::FreeHGlobal(intPtr);

		// Update the configurations
		for (PageDict::Enumerator e = mConfigurationMap.GetEnumerator(); e.MoveNext();) {
			KeyValuePair<String^, ConfigPage^> ^data = e.Current;
			ValidationInfo ^info = data->Value->RefreshConfigurations();

			if (!info->result) {
				MessageBox::Show(info->message, 
						"Configuration Error", 
						MessageBoxButtons::OK, 
						MessageBoxIcon::Error);
			}
		}

		WizardUtil::ExecuteScript(mWizardConfig->PostImportScript, mOpenFileDialog->FileName);
	}
}

/**
 *
 * ExportHandler
 *
 * Handles the menu export option.
 *
 */
System::Void MainWindow::ExportHandler(System::Object^  sender, System::EventArgs^  e) 
{
	typedef Dictionary<String^, ConfigPage^> PageDict;

	System::Boolean result = true;

	// Update the configurations
	for (PageDict::Enumerator e = mConfigurationMap.GetEnumerator(); e.MoveNext();) {
		KeyValuePair<String^, ConfigPage^> ^data = e.Current;
		int index = mConfigurationDroplist->Items->IndexOf(data->Key);

		if (mTabControl->TabPages[index]->Enabled) {
			ValidationInfo ^info = data->Value->UpdateConfigurations();

			if (!info->result) {
				mConfigurationDroplist->SelectedIndex = index;
				info->control->Focus();
				MessageBox::Show(info->message, 
						"Configuration Error", 
						MessageBoxButtons::OK, 
						MessageBoxIcon::Error);
				result = false;
				break;
			}
		}
	}
	
	if (result && mSaveFileDialog->ShowDialog() == Windows::Forms::DialogResult::OK) {
		WizardUtil::ExecuteScript(mWizardConfig->PreExportScript, mSaveFileDialog->FileName);
		
		System::IntPtr intPtr = Marshal::StringToHGlobalAnsi(mSaveFileDialog->FileName);
		const char *filename = static_cast<const char*>(intPtr.ToPointer());
		BUILDER::SpoofBuilder::Result result = mSpoofBuilderP->ExportSpoofData(filename);

		if (result != BUILDER::SpoofBuilder::kResultNone) {
			WizardUtil::ProcessBuilderError(result);
		}

		Marshal::FreeHGlobal(intPtr);

		WizardUtil::ExecuteScript(mWizardConfig->PostExportScript, mSaveFileDialog->FileName);
	}
}

/**
 *
 * LoadHandler
 *
 * Handles the menu load settings option.
 *
 */
System::Void MainWindow::LoadHandler(System::Object^  sender, System::EventArgs^  e)
{
	if (mLoadFileDialog->ShowDialog(this) == Windows::Forms::DialogResult::OK) {
		try {
			WizardConfig ^wizardConfig = gcnew WizardConfig();

			WizardUtil::ImportConfigs(mLoadFileDialog->FileName, wizardConfig);
			mWizardConfig = wizardConfig;
			this->ReloadSettings();
		}
		catch (Exception ^ex) {
			MessageBox::Show(ex->Message, 
					"Configuration Error", 
					MessageBoxButtons::OK, 
					MessageBoxIcon::Error);
		}
	}
}

/**
 *
 * StoreHandler
 *
 * Handles the menu store settings option.
 *
 */
System::Void MainWindow::StoreHandler(System::Object^  sender, System::EventArgs^  e)
{
	if (mStoreFileDialog->ShowDialog(this) == Windows::Forms::DialogResult::OK) {
		try {
			WizardUtil::ExportConfigs(mStoreFileDialog->FileName, mWizardConfig);
		}
		catch (Exception ^ex) {
			MessageBox::Show(ex->Message, 
					"Configuration Error", 
					MessageBoxButtons::OK, 
					MessageBoxIcon::Error);
		}
	}
}

/**
 *
 * SettingsHandler
 *
 * Handles the menu export option.
 *
 */
System::Void MainWindow::SettingsHandler(System::Object^  sender, System::EventArgs^  e)
{
	mSettingsDialog->Config = mWizardConfig;
	if (mSettingsDialog->ShowDialog(this) == Windows::Forms::DialogResult::OK) {
		this->ReloadSettings();
	}
}

/**
 *
 * SelectionHandler
 *
 * Handles the configuration drop list selection.
 *
 */
System::Void MainWindow::SelectionHandler(System::Object^  sender, System::EventArgs^  e)
{
	typedef Dictionary<String^, ConfigPage^> PageDict;

	ConfigPage ^page = nullptr;

	if (mTabControl->TabPages[mTabControl->SelectedIndex]->Enabled) {
		String ^text = mTabControl->SelectedTab->Text;

		if (mConfigurationMap.TryGetValue(text, page)) {
			page->UpdateConfigurations();
		}
	}

	// Update the configurations
	for (PageDict::Enumerator e = mConfigurationMap.GetEnumerator(); e.MoveNext();) {
		KeyValuePair<String^, ConfigPage^> ^data = e.Current;

		if (data->Value != page) {
			data->Value->RefreshConfigurations();
		}
	}

	mTabControl->SelectTab(mConfigurationDroplist->SelectedItem->ToString());
}

/**
 *
 * ReloadSettings
 *
 * Reloads the wizard settings.
 *
 */
System::Void MainWindow::ReloadSettings(void)
{
	System::IntPtr intPtr = Marshal::StringToHGlobalAnsi(mWizardConfig->TemplateFilename);
	BUILDER::SpoofBuilder::SpoofConfig config = { 
		static_cast<const char*>(intPtr.ToPointer()) 
	};

	mSpoofBuilderP->SetSpoofConfig(config);
	Marshal::FreeHGlobal(intPtr);

	Dictionary<String^, bool> ^map = mWizardConfig->DataProcessing;
	Dictionary<String^, bool>::Enumerator enumerator = map->GetEnumerator(); 

	for (; enumerator.MoveNext();) {
		KeyValuePair<String^, bool> ^data = enumerator.Current;
		ExportGeneratorPtr ^ptr = nullptr;
		int index = mTabControl->TabPages->IndexOfKey(data->Key);

		if (mExportGeneratorMap.TryGetValue(data->Key, ptr)) {
			if (data->Value) {
				mSpoofBuilderP->IncludeExportGenerator(*ptr);
			}
			else {
				mSpoofBuilderP->ExcludeExportGenerator(*ptr);
			}
		}

		mTabControl->TabPages[index]->Enabled = data->Value;
	}
}

} // namespace WIZARD