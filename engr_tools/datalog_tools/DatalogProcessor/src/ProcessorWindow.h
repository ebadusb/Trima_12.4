/*****************************************************************************\
**
** ProcessorWindow.h
**
** Provides implementation of the processor window.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "ProcessorEngine.h"
#include "ProcessorSettings.h"

namespace DatalogProcessor {

/* Using *********************************************************************/

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

/* Classes *******************************************************************/

public ref class ProcessorWindow : public System::Windows::Forms::Form {
public:
#pragma region Constructors
	ProcessorWindow(void)
	{
		this->InitializeComponent();
		mEngine = gcnew ProcessorEngine();
		mEngine->LoadSettings();
		this->Settings = mEngine->Settings;
	}

protected:
	~ProcessorWindow(void)
	{
		if (components) {
			delete components;
		}
	}
#pragma endregion

public:
#pragma region Properties
	property ProcessorSettings ^Settings
	{ 
		ProcessorSettings^ get(void)
		{ 
			ProcessorSettings ^settings = gcnew ProcessorSettings();

			settings->DefaultApplication = this->GetFormatSetting();
			settings->LaunchApplication = mLaunchCheckbox->Checked;
			settings->AccessFolder = mAccessFolderTextfield->Text;
			settings->AccessFile = mAccessOverrideTextfield->Text;
			settings->AccessExt = mAccessExtensionList->Text;
			settings->AccessOverride = mAccessOverrideRadio->Checked;
			settings->ExcelFolder = mExcelFolderTextfield->Text;
			settings->ExcelFile = mExcelOverrideTextfield->Text;
			settings->ExcelExt = mExcelExtensionList->Text;
			settings->ExcelOverride = mExcelOverrideRadio->Checked;

			return settings; 
		}
		void set(ProcessorSettings ^settings) 
		{ 
			mAccessRadio->Checked = settings->DefaultApplication == Format::Access;
			mExcelRadio->Checked = settings->DefaultApplication == Format::Excel;
			mHybridRadio->Checked = settings->DefaultApplication == Format::Hybrid;
			mLaunchCheckbox->Checked = settings->LaunchApplication;
			mAccessFolderTextfield->Text = settings->AccessFolder;
			mAccessOverrideTextfield->Text = settings->AccessFile;
			mAccessExtensionList->Text = settings->AccessExt;
			mAccessOverrideRadio->Checked = settings->AccessOverride;
			mAccessCreateRadio->Checked = !settings->AccessOverride;
			mExcelFolderTextfield->Text = settings->ExcelFolder;
			mExcelOverrideTextfield->Text = settings->ExcelFile;
			mExcelOverrideRadio->Checked = settings->ExcelOverride;
			mExcelExtensionList->Text = settings->ExcelExt;
			mExcelCreateRadio->Checked = !settings->ExcelOverride;
		}
	}
#pragma endregion

private: 
#pragma region Functions
	Format GetFormatSetting(void)
	{
		if (mAccessRadio->Checked) {
			return Format::Access;
		}
		else if (mExcelRadio->Checked) {
			return Format::Excel;
		}
		else if (mHybridRadio->Checked) {
			return Format::Hybrid;
		}
	}
#pragma endregion

private: 
#pragma region Handlers
	System::Void AccessFolderBrowseHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		if (mFolderBrowserDialog->ShowDialog() == Windows::Forms::DialogResult::OK) {
			mAccessFolderTextfield->Text = mFolderBrowserDialog->SelectedPath;
		}
	}
	System::Void ExcelFolderBrowseHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		if (mFolderBrowserDialog->ShowDialog() == Windows::Forms::DialogResult::OK) {
			mExcelFolderTextfield->Text = mFolderBrowserDialog->SelectedPath;
		}
	}
	System::Void DatalogFileBrowseHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		if (mOpenFileDialog->ShowDialog() == Windows::Forms::DialogResult::OK) {
			mDatalogTextfield->Text = mOpenFileDialog->FileName;
		}
	}
	System::Void ConvertButtonHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		if (System::IO::File::Exists(mDatalogTextfield->Text)) {
			mEngine->Settings = this->Settings;
			mEngine->ProcessDefaultFile(mDatalogTextfield->Text);
		}
		else if (!System::String::IsNullOrEmpty(mDatalogTextfield->Text)) {
			String ^file = AddinUtility::DatalogPathname::GetCompletePathname(mDatalogTextfield->Text, "DatalogPaths.config");

			if (System::IO::File::Exists(file)) {
				mEngine->Settings = this->Settings;
				mEngine->ProcessDefaultFile(file);
			}
			else {
				MessageBox::Show("Could not locate the given file.", 
						"Error", 
						MessageBoxButtons::OK, 
						MessageBoxIcon::Error);
			}
		}
		else {
			MessageBox::Show("You must provide a datalog.\nThis may be a log name or full path.", 
					"Error", 
					MessageBoxButtons::OK, 
					MessageBoxIcon::Error);
		}
	}
	System::Void SaveButtonHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		mEngine->Settings = this->Settings;
		mEngine->StoreSettings();
	}
	System::Void ExitButtonHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		Application::Exit();
	}
#pragma endregion

private: 
#pragma region Members
	ProcessorEngine^ mEngine;
	System::Windows::Forms::TextBox^  mAccessFolderTextfield;
	System::Windows::Forms::Label^  mAccessFolderLabel;
	System::Windows::Forms::Button^  mAccessFolderButton;
	System::Windows::Forms::RadioButton^  mExcelRadio;
	System::Windows::Forms::RadioButton^  mAccessRadio;
	System::Windows::Forms::Label^  mFormatLabel;
	System::Windows::Forms::CheckBox^  mLaunchCheckbox;
	System::Windows::Forms::TextBox^  mAccessOverrideTextfield;
	System::Windows::Forms::GroupBox^  mDefaultGroupBox;
	System::Windows::Forms::GroupBox^  mAccessGroupBox;
	System::Windows::Forms::Button^  mExitButton;
	System::Windows::Forms::Button^  mSaveButton;
	System::Windows::Forms::RadioButton^  mAccessCreateRadio;
	System::Windows::Forms::RadioButton^  mAccessOverrideRadio;
	System::Windows::Forms::GroupBox^  mExcelGroupBox;
	System::Windows::Forms::RadioButton^  mExcelCreateRadio;
	System::Windows::Forms::RadioButton^  mExcelOverrideRadio;
	System::Windows::Forms::TextBox^  mExcelFolderTextfield;
	System::Windows::Forms::Label^  mExcelFolderLabel;
	System::Windows::Forms::TextBox^  mExcelOverrideTextfield;
	System::Windows::Forms::Button^  mExcelFolderButton;
	System::Windows::Forms::Label^  mDatalogLabel;
	System::Windows::Forms::TextBox^  mDatalogTextfield;
	System::Windows::Forms::GroupBox^  mDatalogGroupBox;
	System::Windows::Forms::Button^  mDatalogButton;
	System::Windows::Forms::Button^  mConvertButton;
	System::Windows::Forms::FolderBrowserDialog^  mFolderBrowserDialog;
	System::Windows::Forms::OpenFileDialog^  mOpenFileDialog;
private: System::Windows::Forms::ComboBox^  mAccessExtensionList;
private: System::Windows::Forms::ComboBox^  mExcelExtensionList;
private: System::Windows::Forms::Label^  mAccessExtensionLabel;
private: System::Windows::Forms::Label^  mExcelExtensionLabel;
private: System::Windows::Forms::RadioButton^  mHybridRadio;
#pragma endregion

#pragma region Generated
	private:
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(ProcessorWindow::typeid));
			this->mAccessFolderTextfield = (gcnew System::Windows::Forms::TextBox());
			this->mAccessFolderLabel = (gcnew System::Windows::Forms::Label());
			this->mAccessFolderButton = (gcnew System::Windows::Forms::Button());
			this->mExcelRadio = (gcnew System::Windows::Forms::RadioButton());
			this->mAccessRadio = (gcnew System::Windows::Forms::RadioButton());
			this->mFormatLabel = (gcnew System::Windows::Forms::Label());
			this->mLaunchCheckbox = (gcnew System::Windows::Forms::CheckBox());
			this->mAccessOverrideTextfield = (gcnew System::Windows::Forms::TextBox());
			this->mDefaultGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->mHybridRadio = (gcnew System::Windows::Forms::RadioButton());
			this->mAccessGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->mAccessExtensionLabel = (gcnew System::Windows::Forms::Label());
			this->mAccessExtensionList = (gcnew System::Windows::Forms::ComboBox());
			this->mAccessCreateRadio = (gcnew System::Windows::Forms::RadioButton());
			this->mAccessOverrideRadio = (gcnew System::Windows::Forms::RadioButton());
			this->mExitButton = (gcnew System::Windows::Forms::Button());
			this->mSaveButton = (gcnew System::Windows::Forms::Button());
			this->mExcelGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->mExcelExtensionLabel = (gcnew System::Windows::Forms::Label());
			this->mExcelExtensionList = (gcnew System::Windows::Forms::ComboBox());
			this->mExcelCreateRadio = (gcnew System::Windows::Forms::RadioButton());
			this->mExcelOverrideRadio = (gcnew System::Windows::Forms::RadioButton());
			this->mExcelFolderTextfield = (gcnew System::Windows::Forms::TextBox());
			this->mExcelFolderLabel = (gcnew System::Windows::Forms::Label());
			this->mExcelOverrideTextfield = (gcnew System::Windows::Forms::TextBox());
			this->mExcelFolderButton = (gcnew System::Windows::Forms::Button());
			this->mDatalogLabel = (gcnew System::Windows::Forms::Label());
			this->mDatalogTextfield = (gcnew System::Windows::Forms::TextBox());
			this->mDatalogGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->mDatalogButton = (gcnew System::Windows::Forms::Button());
			this->mConvertButton = (gcnew System::Windows::Forms::Button());
			this->mFolderBrowserDialog = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->mOpenFileDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->mDefaultGroupBox->SuspendLayout();
			this->mAccessGroupBox->SuspendLayout();
			this->mExcelGroupBox->SuspendLayout();
			this->mDatalogGroupBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// mAccessFolderTextfield
			// 
			this->mAccessFolderTextfield->Location = System::Drawing::Point(64, 23);
			this->mAccessFolderTextfield->Name = L"mAccessFolderTextfield";
			this->mAccessFolderTextfield->Size = System::Drawing::Size(300, 20);
			this->mAccessFolderTextfield->TabIndex = 0;
			// 
			// mAccessFolderLabel
			// 
			this->mAccessFolderLabel->AutoSize = true;
			this->mAccessFolderLabel->Location = System::Drawing::Point(19, 26);
			this->mAccessFolderLabel->Name = L"mAccessFolderLabel";
			this->mAccessFolderLabel->Size = System::Drawing::Size(39, 13);
			this->mAccessFolderLabel->TabIndex = 1;
			this->mAccessFolderLabel->Text = L"Folder:";
			// 
			// mAccessFolderButton
			// 
			this->mAccessFolderButton->Location = System::Drawing::Point(370, 21);
			this->mAccessFolderButton->Name = L"mAccessFolderButton";
			this->mAccessFolderButton->Size = System::Drawing::Size(75, 23);
			this->mAccessFolderButton->TabIndex = 5;
			this->mAccessFolderButton->Text = L"Browse...";
			this->mAccessFolderButton->UseVisualStyleBackColor = true;
			this->mAccessFolderButton->Click += gcnew System::EventHandler(this, &ProcessorWindow::AccessFolderBrowseHandler);
			// 
			// mExcelRadio
			// 
			this->mExcelRadio->AutoSize = true;
			this->mExcelRadio->Location = System::Drawing::Point(75, 46);
			this->mExcelRadio->Name = L"mExcelRadio";
			this->mExcelRadio->Size = System::Drawing::Size(97, 17);
			this->mExcelRadio->TabIndex = 6;
			this->mExcelRadio->TabStop = true;
			this->mExcelRadio->Text = L"Microsoft Excel";
			this->mExcelRadio->UseVisualStyleBackColor = true;
			// 
			// mAccessRadio
			// 
			this->mAccessRadio->AutoSize = true;
			this->mAccessRadio->Location = System::Drawing::Point(75, 23);
			this->mAccessRadio->Name = L"mAccessRadio";
			this->mAccessRadio->Size = System::Drawing::Size(106, 17);
			this->mAccessRadio->TabIndex = 7;
			this->mAccessRadio->TabStop = true;
			this->mAccessRadio->Text = L"Microsoft Access";
			this->mAccessRadio->UseVisualStyleBackColor = true;
			// 
			// mFormatLabel
			// 
			this->mFormatLabel->AutoSize = true;
			this->mFormatLabel->Location = System::Drawing::Point(8, 23);
			this->mFormatLabel->Name = L"mFormatLabel";
			this->mFormatLabel->Size = System::Drawing::Size(61, 13);
			this->mFormatLabel->TabIndex = 8;
			this->mFormatLabel->Text = L"File Format:";
			// 
			// mLaunchCheckbox
			// 
			this->mLaunchCheckbox->AutoSize = true;
			this->mLaunchCheckbox->Location = System::Drawing::Point(230, 22);
			this->mLaunchCheckbox->Name = L"mLaunchCheckbox";
			this->mLaunchCheckbox->Size = System::Drawing::Size(117, 17);
			this->mLaunchCheckbox->TabIndex = 9;
			this->mLaunchCheckbox->Text = L"Launch Application";
			this->mLaunchCheckbox->UseVisualStyleBackColor = true;
			// 
			// mAccessOverrideTextfield
			// 
			this->mAccessOverrideTextfield->Location = System::Drawing::Point(115, 49);
			this->mAccessOverrideTextfield->Name = L"mAccessOverrideTextfield";
			this->mAccessOverrideTextfield->Size = System::Drawing::Size(249, 20);
			this->mAccessOverrideTextfield->TabIndex = 11;
			// 
			// mDefaultGroupBox
			// 
			this->mDefaultGroupBox->Controls->Add(this->mHybridRadio);
			this->mDefaultGroupBox->Controls->Add(this->mExcelRadio);
			this->mDefaultGroupBox->Controls->Add(this->mAccessRadio);
			this->mDefaultGroupBox->Controls->Add(this->mFormatLabel);
			this->mDefaultGroupBox->Controls->Add(this->mLaunchCheckbox);
			this->mDefaultGroupBox->Location = System::Drawing::Point(20, 12);
			this->mDefaultGroupBox->Name = L"mDefaultGroupBox";
			this->mDefaultGroupBox->Size = System::Drawing::Size(457, 100);
			this->mDefaultGroupBox->TabIndex = 14;
			this->mDefaultGroupBox->TabStop = false;
			this->mDefaultGroupBox->Text = L"Default";
			// 
			// mHybridRadio
			// 
			this->mHybridRadio->AutoSize = true;
			this->mHybridRadio->Location = System::Drawing::Point(75, 69);
			this->mHybridRadio->Name = L"mHybridRadio";
			this->mHybridRadio->Size = System::Drawing::Size(85, 17);
			this->mHybridRadio->TabIndex = 10;
			this->mHybridRadio->TabStop = true;
			this->mHybridRadio->Text = L"Hybrid Mode";
			this->mHybridRadio->UseVisualStyleBackColor = true;
			// 
			// mAccessGroupBox
			// 
			this->mAccessGroupBox->Controls->Add(this->mAccessExtensionLabel);
			this->mAccessGroupBox->Controls->Add(this->mAccessExtensionList);
			this->mAccessGroupBox->Controls->Add(this->mAccessCreateRadio);
			this->mAccessGroupBox->Controls->Add(this->mAccessOverrideRadio);
			this->mAccessGroupBox->Controls->Add(this->mAccessFolderTextfield);
			this->mAccessGroupBox->Controls->Add(this->mAccessFolderLabel);
			this->mAccessGroupBox->Controls->Add(this->mAccessOverrideTextfield);
			this->mAccessGroupBox->Controls->Add(this->mAccessFolderButton);
			this->mAccessGroupBox->Location = System::Drawing::Point(20, 118);
			this->mAccessGroupBox->Name = L"mAccessGroupBox";
			this->mAccessGroupBox->Size = System::Drawing::Size(457, 108);
			this->mAccessGroupBox->TabIndex = 15;
			this->mAccessGroupBox->TabStop = false;
			this->mAccessGroupBox->Text = L"Access";
			// 
			// mAccessExtensionLabel
			// 
			this->mAccessExtensionLabel->AutoSize = true;
			this->mAccessExtensionLabel->Location = System::Drawing::Point(227, 76);
			this->mAccessExtensionLabel->Name = L"mAccessExtensionLabel";
			this->mAccessExtensionLabel->Size = System::Drawing::Size(56, 13);
			this->mAccessExtensionLabel->TabIndex = 17;
			this->mAccessExtensionLabel->Text = L"Extension:";
			// 
			// mAccessExtensionList
			// 
			this->mAccessExtensionList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->mAccessExtensionList->FormattingEnabled = true;
			this->mAccessExtensionList->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L".accdb", L".mdb"});
			this->mAccessExtensionList->Location = System::Drawing::Point(289, 73);
			this->mAccessExtensionList->Name = L"mAccessExtensionList";
			this->mAccessExtensionList->Size = System::Drawing::Size(75, 21);
			this->mAccessExtensionList->TabIndex = 16;
			// 
			// mAccessCreateRadio
			// 
			this->mAccessCreateRadio->AutoSize = true;
			this->mAccessCreateRadio->Location = System::Drawing::Point(22, 74);
			this->mAccessCreateRadio->Name = L"mAccessCreateRadio";
			this->mAccessCreateRadio->Size = System::Drawing::Size(190, 17);
			this->mAccessCreateRadio->TabIndex = 15;
			this->mAccessCreateRadio->TabStop = true;
			this->mAccessCreateRadio->Text = L"Create File (same name as datalog)";
			this->mAccessCreateRadio->UseVisualStyleBackColor = true;
			// 
			// mAccessOverrideRadio
			// 
			this->mAccessOverrideRadio->AutoSize = true;
			this->mAccessOverrideRadio->Location = System::Drawing::Point(22, 50);
			this->mAccessOverrideRadio->Name = L"mAccessOverrideRadio";
			this->mAccessOverrideRadio->Size = System::Drawing::Size(87, 17);
			this->mAccessOverrideRadio->TabIndex = 14;
			this->mAccessOverrideRadio->TabStop = true;
			this->mAccessOverrideRadio->Text = L"Override File:";
			this->mAccessOverrideRadio->UseVisualStyleBackColor = true;
			// 
			// mExitButton
			// 
			this->mExitButton->Location = System::Drawing::Point(390, 406);
			this->mExitButton->Name = L"mExitButton";
			this->mExitButton->Size = System::Drawing::Size(75, 23);
			this->mExitButton->TabIndex = 16;
			this->mExitButton->Text = L"Exit";
			this->mExitButton->UseVisualStyleBackColor = true;
			this->mExitButton->Click += gcnew System::EventHandler(this, &ProcessorWindow::ExitButtonHandler);
			// 
			// mSaveButton
			// 
			this->mSaveButton->Location = System::Drawing::Point(309, 406);
			this->mSaveButton->Name = L"mSaveButton";
			this->mSaveButton->Size = System::Drawing::Size(75, 23);
			this->mSaveButton->TabIndex = 17;
			this->mSaveButton->Text = L"Save";
			this->mSaveButton->UseVisualStyleBackColor = true;
			this->mSaveButton->Click += gcnew System::EventHandler(this, &ProcessorWindow::SaveButtonHandler);
			// 
			// mExcelGroupBox
			// 
			this->mExcelGroupBox->Controls->Add(this->mExcelExtensionLabel);
			this->mExcelGroupBox->Controls->Add(this->mExcelExtensionList);
			this->mExcelGroupBox->Controls->Add(this->mExcelCreateRadio);
			this->mExcelGroupBox->Controls->Add(this->mExcelOverrideRadio);
			this->mExcelGroupBox->Controls->Add(this->mExcelFolderTextfield);
			this->mExcelGroupBox->Controls->Add(this->mExcelFolderLabel);
			this->mExcelGroupBox->Controls->Add(this->mExcelOverrideTextfield);
			this->mExcelGroupBox->Controls->Add(this->mExcelFolderButton);
			this->mExcelGroupBox->Location = System::Drawing::Point(20, 232);
			this->mExcelGroupBox->Name = L"mExcelGroupBox";
			this->mExcelGroupBox->Size = System::Drawing::Size(457, 108);
			this->mExcelGroupBox->TabIndex = 18;
			this->mExcelGroupBox->TabStop = false;
			this->mExcelGroupBox->Text = L"Excel";
			// 
			// mExcelExtensionLabel
			// 
			this->mExcelExtensionLabel->AutoSize = true;
			this->mExcelExtensionLabel->Location = System::Drawing::Point(227, 76);
			this->mExcelExtensionLabel->Name = L"mExcelExtensionLabel";
			this->mExcelExtensionLabel->Size = System::Drawing::Size(56, 13);
			this->mExcelExtensionLabel->TabIndex = 18;
			this->mExcelExtensionLabel->Text = L"Extension:";
			// 
			// mExcelExtensionList
			// 
			this->mExcelExtensionList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->mExcelExtensionList->FormattingEnabled = true;
			this->mExcelExtensionList->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L".xlsx", L".xlsb", L".xlsm"});
			this->mExcelExtensionList->Location = System::Drawing::Point(289, 73);
			this->mExcelExtensionList->Name = L"mExcelExtensionList";
			this->mExcelExtensionList->Size = System::Drawing::Size(75, 21);
			this->mExcelExtensionList->TabIndex = 17;
			// 
			// mExcelCreateRadio
			// 
			this->mExcelCreateRadio->AutoSize = true;
			this->mExcelCreateRadio->Location = System::Drawing::Point(22, 74);
			this->mExcelCreateRadio->Name = L"mExcelCreateRadio";
			this->mExcelCreateRadio->Size = System::Drawing::Size(190, 17);
			this->mExcelCreateRadio->TabIndex = 15;
			this->mExcelCreateRadio->TabStop = true;
			this->mExcelCreateRadio->Text = L"Create File (same name as datalog)";
			this->mExcelCreateRadio->UseVisualStyleBackColor = true;
			// 
			// mExcelOverrideRadio
			// 
			this->mExcelOverrideRadio->AutoSize = true;
			this->mExcelOverrideRadio->Location = System::Drawing::Point(22, 50);
			this->mExcelOverrideRadio->Name = L"mExcelOverrideRadio";
			this->mExcelOverrideRadio->Size = System::Drawing::Size(87, 17);
			this->mExcelOverrideRadio->TabIndex = 14;
			this->mExcelOverrideRadio->TabStop = true;
			this->mExcelOverrideRadio->Text = L"Override File:";
			this->mExcelOverrideRadio->UseVisualStyleBackColor = true;
			// 
			// mExcelFolderTextfield
			// 
			this->mExcelFolderTextfield->Location = System::Drawing::Point(64, 23);
			this->mExcelFolderTextfield->Name = L"mExcelFolderTextfield";
			this->mExcelFolderTextfield->Size = System::Drawing::Size(300, 20);
			this->mExcelFolderTextfield->TabIndex = 0;
			// 
			// mExcelFolderLabel
			// 
			this->mExcelFolderLabel->AutoSize = true;
			this->mExcelFolderLabel->Location = System::Drawing::Point(19, 26);
			this->mExcelFolderLabel->Name = L"mExcelFolderLabel";
			this->mExcelFolderLabel->Size = System::Drawing::Size(39, 13);
			this->mExcelFolderLabel->TabIndex = 1;
			this->mExcelFolderLabel->Text = L"Folder:";
			// 
			// mExcelOverrideTextfield
			// 
			this->mExcelOverrideTextfield->Location = System::Drawing::Point(115, 49);
			this->mExcelOverrideTextfield->Name = L"mExcelOverrideTextfield";
			this->mExcelOverrideTextfield->Size = System::Drawing::Size(249, 20);
			this->mExcelOverrideTextfield->TabIndex = 11;
			// 
			// mExcelFolderButton
			// 
			this->mExcelFolderButton->Location = System::Drawing::Point(370, 21);
			this->mExcelFolderButton->Name = L"mExcelFolderButton";
			this->mExcelFolderButton->Size = System::Drawing::Size(75, 23);
			this->mExcelFolderButton->TabIndex = 5;
			this->mExcelFolderButton->Text = L"Browse...";
			this->mExcelFolderButton->UseVisualStyleBackColor = true;
			this->mExcelFolderButton->Click += gcnew System::EventHandler(this, &ProcessorWindow::ExcelFolderBrowseHandler);
			// 
			// mDatalogLabel
			// 
			this->mDatalogLabel->AutoSize = true;
			this->mDatalogLabel->Location = System::Drawing::Point(8, 25);
			this->mDatalogLabel->Name = L"mDatalogLabel";
			this->mDatalogLabel->Size = System::Drawing::Size(47, 13);
			this->mDatalogLabel->TabIndex = 19;
			this->mDatalogLabel->Text = L"Datalog:";
			// 
			// mDatalogTextfield
			// 
			this->mDatalogTextfield->Location = System::Drawing::Point(61, 22);
			this->mDatalogTextfield->Name = L"mDatalogTextfield";
			this->mDatalogTextfield->Size = System::Drawing::Size(303, 20);
			this->mDatalogTextfield->TabIndex = 20;
			// 
			// mDatalogGroupBox
			// 
			this->mDatalogGroupBox->Controls->Add(this->mDatalogButton);
			this->mDatalogGroupBox->Controls->Add(this->mDatalogLabel);
			this->mDatalogGroupBox->Controls->Add(this->mDatalogTextfield);
			this->mDatalogGroupBox->Location = System::Drawing::Point(20, 346);
			this->mDatalogGroupBox->Name = L"mDatalogGroupBox";
			this->mDatalogGroupBox->Size = System::Drawing::Size(457, 54);
			this->mDatalogGroupBox->TabIndex = 21;
			this->mDatalogGroupBox->TabStop = false;
			this->mDatalogGroupBox->Text = L"Datalog";
			// 
			// mDatalogButton
			// 
			this->mDatalogButton->Location = System::Drawing::Point(370, 20);
			this->mDatalogButton->Name = L"mDatalogButton";
			this->mDatalogButton->Size = System::Drawing::Size(75, 23);
			this->mDatalogButton->TabIndex = 21;
			this->mDatalogButton->Text = L"Browse...";
			this->mDatalogButton->UseVisualStyleBackColor = true;
			this->mDatalogButton->Click += gcnew System::EventHandler(this, &ProcessorWindow::DatalogFileBrowseHandler);
			// 
			// mConvertButton
			// 
			this->mConvertButton->Location = System::Drawing::Point(228, 406);
			this->mConvertButton->Name = L"mConvertButton";
			this->mConvertButton->Size = System::Drawing::Size(75, 23);
			this->mConvertButton->TabIndex = 22;
			this->mConvertButton->Text = L"Convert";
			this->mConvertButton->UseVisualStyleBackColor = true;
			this->mConvertButton->Click += gcnew System::EventHandler(this, &ProcessorWindow::ConvertButtonHandler);
			// 
			// mOpenFileDialog
			// 
			this->mOpenFileDialog->FileName = L"openFileDialog1";
			// 
			// ProcessorWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(490, 442);
			this->Controls->Add(this->mConvertButton);
			this->Controls->Add(this->mDatalogGroupBox);
			this->Controls->Add(this->mExcelGroupBox);
			this->Controls->Add(this->mSaveButton);
			this->Controls->Add(this->mExitButton);
			this->Controls->Add(this->mAccessGroupBox);
			this->Controls->Add(this->mDefaultGroupBox);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"ProcessorWindow";
			this->Text = L"Datalog Processor";
			this->mDefaultGroupBox->ResumeLayout(false);
			this->mDefaultGroupBox->PerformLayout();
			this->mAccessGroupBox->ResumeLayout(false);
			this->mAccessGroupBox->PerformLayout();
			this->mExcelGroupBox->ResumeLayout(false);
			this->mExcelGroupBox->PerformLayout();
			this->mDatalogGroupBox->ResumeLayout(false);
			this->mDatalogGroupBox->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
#pragma endregion
};
}

