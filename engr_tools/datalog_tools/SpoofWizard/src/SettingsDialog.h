/*****************************************************************************\
**
** SettingsDialog.h
**
** Provides the implementation for the settings dialog.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "WizardConfig.h"

/* Class *********************************************************************/

namespace WIZARD {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class SettingsDialog : public System::Windows::Forms::Form {
#pragma region Construction
public:
	SettingsDialog(void)
	{
		this->InitializeComponent();

		mWizardConfig = gcnew WizardConfig();
		mSelectFileDialog = gcnew OpenFileDialog();
		mSelectFileDialog->Title = "Select Template File";
	}

protected:
	~SettingsDialog(void)
	{
		if (components) {
			delete components;
		}
	}
#pragma endregion

#pragma region Types
	typedef System::Collections::Generic::Dictionary<String^, bool> ProcessMap;
#pragma endregion

#pragma region Handlers
private:
	System::Void LoadHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		mTemplateFileTextfield->Text = mWizardConfig->TemplateFilename;
		mPreImportTextfield->Text = mWizardConfig->PreImportScript;
		mPostImportTextfield->Text = mWizardConfig->PostImportScript;
		mPreExportTextfield->Text = mWizardConfig->PreExportScript;
		mPostExportTextfield->Text = mWizardConfig->PostExportScript;

		ProcessMap::Enumerator enumerator = mWizardConfig->DataProcessing->GetEnumerator();

		mProcessCheckedListBox->Items->Clear();

		for (; enumerator.MoveNext();) {
			KeyValuePair<String^, bool> ^data = enumerator.Current;

			mProcessCheckedListBox->Items->Add(data->Key, data->Value);
		}
	}
	System::Void BrowseHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		if (mSelectFileDialog->ShowDialog() == Windows::Forms::DialogResult::OK) {
			mTemplateFileTextfield->Text = mSelectFileDialog->FileName;
		}
	}
	System::Void OKHandler(System::Object^  sender, System::EventArgs^  e)
	{
		mWizardConfig->TemplateFilename = mTemplateFileTextfield->Text;
		mWizardConfig->PreImportScript = mPreImportTextfield->Text;
		mWizardConfig->PostImportScript = mPostImportTextfield->Text;
		mWizardConfig->PreExportScript = mPreExportTextfield->Text;
		mWizardConfig->PostExportScript = mPostExportTextfield->Text;

		ProcessMap ^map = gcnew ProcessMap();
		ProcessMap::Enumerator enumerator = mWizardConfig->DataProcessing->GetEnumerator();

		for (; enumerator.MoveNext();) {
			KeyValuePair<String^, bool> ^data = enumerator.Current;
			int index = mProcessCheckedListBox->Items->IndexOf(data->Key);

			map[data->Key] = mProcessCheckedListBox->GetItemChecked(index);
		}

		mWizardConfig->DataProcessing = map;
	}
#pragma endregion

#pragma region Properties
public:
	property WizardConfig ^Config
	{
		WizardConfig ^get(void) { return mWizardConfig; }
		void set(WizardConfig ^config) { mWizardConfig = config; }
	}
#pragma endregion

#pragma region Members
private: WizardConfig ^mWizardConfig;
private: System::Windows::Forms::OpenFileDialog ^mSelectFileDialog;
private: System::Windows::Forms::Label^  mTemplateFileLabel;
private: System::Windows::Forms::Button^  mBrowseButton;
private: System::Windows::Forms::TextBox^  mTemplateFileTextfield;
private: System::Windows::Forms::Label^  mProcessLabel;
private: System::Windows::Forms::CheckedListBox^  mProcessCheckedListBox;
private: System::Windows::Forms::Button^  mCancelButton;
private: System::Windows::Forms::Button^  mOKButton;
private: System::Windows::Forms::Label^  mPreImportLabel;
private: System::Windows::Forms::GroupBox^  mExternalScriptsGroupbox;
private: System::Windows::Forms::TextBox^  mPostExportTextfield;
private: System::Windows::Forms::Label^  mPostExportLabel;
private: System::Windows::Forms::TextBox^  mPreExportTextfield;
private: System::Windows::Forms::Label^  mPreExportLabel;
private: System::Windows::Forms::TextBox^  mPostImportTextfield;
private: System::Windows::Forms::Label^  mPostImportLabel;
private: System::Windows::Forms::TextBox^  mPreImportTextfield;
private: System::Windows::Forms::GroupBox^  mDataGenerationGroupbox;
private: System::Windows::Forms::GroupBox^  mExternalFilesGroupbox;
#pragma endregion

#pragma region Generated
private:
	System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
	void InitializeComponent(void)
	{
		this->mTemplateFileLabel = (gcnew System::Windows::Forms::Label());
		this->mBrowseButton = (gcnew System::Windows::Forms::Button());
		this->mTemplateFileTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mProcessLabel = (gcnew System::Windows::Forms::Label());
		this->mProcessCheckedListBox = (gcnew System::Windows::Forms::CheckedListBox());
		this->mCancelButton = (gcnew System::Windows::Forms::Button());
		this->mOKButton = (gcnew System::Windows::Forms::Button());
		this->mPreImportLabel = (gcnew System::Windows::Forms::Label());
		this->mExternalScriptsGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mPostExportTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mPostExportLabel = (gcnew System::Windows::Forms::Label());
		this->mPreExportTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mPreExportLabel = (gcnew System::Windows::Forms::Label());
		this->mPostImportTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mPostImportLabel = (gcnew System::Windows::Forms::Label());
		this->mPreImportTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mDataGenerationGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mExternalFilesGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mExternalScriptsGroupbox->SuspendLayout();
		this->mDataGenerationGroupbox->SuspendLayout();
		this->mExternalFilesGroupbox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mTemplateFileLabel
		// 
		this->mTemplateFileLabel->AutoSize = true;
		this->mTemplateFileLabel->Location = System::Drawing::Point(6, 22);
		this->mTemplateFileLabel->Name = L"mTemplateFileLabel";
		this->mTemplateFileLabel->Size = System::Drawing::Size(73, 13);
		this->mTemplateFileLabel->TabIndex = 0;
		this->mTemplateFileLabel->Text = L"Template File:";
		// 
		// mBrowseButton
		// 
		this->mBrowseButton->Location = System::Drawing::Point(289, 17);
		this->mBrowseButton->Name = L"mBrowseButton";
		this->mBrowseButton->Size = System::Drawing::Size(75, 23);
		this->mBrowseButton->TabIndex = 1;
		this->mBrowseButton->Text = L"Browse...";
		this->mBrowseButton->UseVisualStyleBackColor = true;
		this->mBrowseButton->Click += gcnew System::EventHandler(this, &SettingsDialog::BrowseHandler);
		// 
		// mTemplateFileTextfield
		// 
		this->mTemplateFileTextfield->Location = System::Drawing::Point(85, 19);
		this->mTemplateFileTextfield->Name = L"mTemplateFileTextfield";
		this->mTemplateFileTextfield->Size = System::Drawing::Size(198, 20);
		this->mTemplateFileTextfield->TabIndex = 2;
		// 
		// mProcessLabel
		// 
		this->mProcessLabel->AutoSize = true;
		this->mProcessLabel->Location = System::Drawing::Point(6, 19);
		this->mProcessLabel->Name = L"mProcessLabel";
		this->mProcessLabel->Size = System::Drawing::Size(33, 13);
		this->mProcessLabel->TabIndex = 3;
		this->mProcessLabel->Text = L"Data:";
		// 
		// mProcessCheckedListBox
		// 
		this->mProcessCheckedListBox->FormattingEnabled = true;
		this->mProcessCheckedListBox->Location = System::Drawing::Point(85, 19);
		this->mProcessCheckedListBox->Name = L"mProcessCheckedListBox";
		this->mProcessCheckedListBox->Size = System::Drawing::Size(279, 109);
		this->mProcessCheckedListBox->TabIndex = 4;
		// 
		// mCancelButton
		// 
		this->mCancelButton->DialogResult = System::Windows::Forms::DialogResult::Cancel;
		this->mCancelButton->Location = System::Drawing::Point(301, 368);
		this->mCancelButton->Name = L"mCancelButton";
		this->mCancelButton->Size = System::Drawing::Size(75, 23);
		this->mCancelButton->TabIndex = 5;
		this->mCancelButton->Text = L"Cancel";
		this->mCancelButton->UseVisualStyleBackColor = true;
		// 
		// mOKButton
		// 
		this->mOKButton->DialogResult = System::Windows::Forms::DialogResult::OK;
		this->mOKButton->Location = System::Drawing::Point(220, 368);
		this->mOKButton->Name = L"mOKButton";
		this->mOKButton->Size = System::Drawing::Size(75, 23);
		this->mOKButton->TabIndex = 6;
		this->mOKButton->Text = L"OK";
		this->mOKButton->UseVisualStyleBackColor = true;
		this->mOKButton->Click += gcnew System::EventHandler(this, &SettingsDialog::OKHandler);
		// 
		// mPreImportLabel
		// 
		this->mPreImportLabel->AutoSize = true;
		this->mPreImportLabel->Location = System::Drawing::Point(6, 28);
		this->mPreImportLabel->Name = L"mPreImportLabel";
		this->mPreImportLabel->Size = System::Drawing::Size(58, 13);
		this->mPreImportLabel->TabIndex = 7;
		this->mPreImportLabel->Text = L"Pre-Import:";
		// 
		// mExternalScriptsGroupbox
		// 
		this->mExternalScriptsGroupbox->Controls->Add(this->mPostExportTextfield);
		this->mExternalScriptsGroupbox->Controls->Add(this->mPostExportLabel);
		this->mExternalScriptsGroupbox->Controls->Add(this->mPreExportTextfield);
		this->mExternalScriptsGroupbox->Controls->Add(this->mPreExportLabel);
		this->mExternalScriptsGroupbox->Controls->Add(this->mPostImportTextfield);
		this->mExternalScriptsGroupbox->Controls->Add(this->mPostImportLabel);
		this->mExternalScriptsGroupbox->Controls->Add(this->mPreImportTextfield);
		this->mExternalScriptsGroupbox->Controls->Add(this->mPreImportLabel);
		this->mExternalScriptsGroupbox->Location = System::Drawing::Point(12, 228);
		this->mExternalScriptsGroupbox->Name = L"mExternalScriptsGroupbox";
		this->mExternalScriptsGroupbox->Size = System::Drawing::Size(370, 134);
		this->mExternalScriptsGroupbox->TabIndex = 8;
		this->mExternalScriptsGroupbox->TabStop = false;
		this->mExternalScriptsGroupbox->Text = L"External Scrips";
		// 
		// mPostExportTextfield
		// 
		this->mPostExportTextfield->Location = System::Drawing::Point(85, 101);
		this->mPostExportTextfield->Name = L"mPostExportTextfield";
		this->mPostExportTextfield->Size = System::Drawing::Size(279, 20);
		this->mPostExportTextfield->TabIndex = 14;
		// 
		// mPostExportLabel
		// 
		this->mPostExportLabel->AutoSize = true;
		this->mPostExportLabel->Location = System::Drawing::Point(6, 104);
		this->mPostExportLabel->Name = L"mPostExportLabel";
		this->mPostExportLabel->Size = System::Drawing::Size(64, 13);
		this->mPostExportLabel->TabIndex = 13;
		this->mPostExportLabel->Text = L"Post-Export:";
		// 
		// mPreExportTextfield
		// 
		this->mPreExportTextfield->Location = System::Drawing::Point(85, 75);
		this->mPreExportTextfield->Name = L"mPreExportTextfield";
		this->mPreExportTextfield->Size = System::Drawing::Size(279, 20);
		this->mPreExportTextfield->TabIndex = 12;
		// 
		// mPreExportLabel
		// 
		this->mPreExportLabel->AutoSize = true;
		this->mPreExportLabel->Location = System::Drawing::Point(6, 78);
		this->mPreExportLabel->Name = L"mPreExportLabel";
		this->mPreExportLabel->Size = System::Drawing::Size(59, 13);
		this->mPreExportLabel->TabIndex = 11;
		this->mPreExportLabel->Text = L"Pre-Export:";
		// 
		// mPostImportTextfield
		// 
		this->mPostImportTextfield->Location = System::Drawing::Point(85, 51);
		this->mPostImportTextfield->Name = L"mPostImportTextfield";
		this->mPostImportTextfield->Size = System::Drawing::Size(279, 20);
		this->mPostImportTextfield->TabIndex = 10;
		// 
		// mPostImportLabel
		// 
		this->mPostImportLabel->AutoSize = true;
		this->mPostImportLabel->Location = System::Drawing::Point(6, 54);
		this->mPostImportLabel->Name = L"mPostImportLabel";
		this->mPostImportLabel->Size = System::Drawing::Size(63, 13);
		this->mPostImportLabel->TabIndex = 9;
		this->mPostImportLabel->Text = L"Post-Import:";
		// 
		// mPreImportTextfield
		// 
		this->mPreImportTextfield->Location = System::Drawing::Point(85, 25);
		this->mPreImportTextfield->Name = L"mPreImportTextfield";
		this->mPreImportTextfield->Size = System::Drawing::Size(279, 20);
		this->mPreImportTextfield->TabIndex = 8;
		// 
		// mDataGenerationGroupbox
		// 
		this->mDataGenerationGroupbox->Controls->Add(this->mProcessCheckedListBox);
		this->mDataGenerationGroupbox->Controls->Add(this->mProcessLabel);
		this->mDataGenerationGroupbox->Location = System::Drawing::Point(12, 12);
		this->mDataGenerationGroupbox->Name = L"mDataGenerationGroupbox";
		this->mDataGenerationGroupbox->Size = System::Drawing::Size(370, 145);
		this->mDataGenerationGroupbox->TabIndex = 9;
		this->mDataGenerationGroupbox->TabStop = false;
		this->mDataGenerationGroupbox->Text = L"Data Generation";
		// 
		// mExternalFilesGroupbox
		// 
		this->mExternalFilesGroupbox->Controls->Add(this->mTemplateFileTextfield);
		this->mExternalFilesGroupbox->Controls->Add(this->mTemplateFileLabel);
		this->mExternalFilesGroupbox->Controls->Add(this->mBrowseButton);
		this->mExternalFilesGroupbox->Location = System::Drawing::Point(12, 164);
		this->mExternalFilesGroupbox->Name = L"mExternalFilesGroupbox";
		this->mExternalFilesGroupbox->Size = System::Drawing::Size(370, 58);
		this->mExternalFilesGroupbox->TabIndex = 10;
		this->mExternalFilesGroupbox->TabStop = false;
		this->mExternalFilesGroupbox->Text = L"External Files";
		// 
		// SettingsDialog
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(394, 404);
		this->Controls->Add(this->mExternalFilesGroupbox);
		this->Controls->Add(this->mDataGenerationGroupbox);
		this->Controls->Add(this->mExternalScriptsGroupbox);
		this->Controls->Add(this->mOKButton);
		this->Controls->Add(this->mCancelButton);
		this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
		this->Name = L"SettingsDialog";
		this->Text = L"Spoof Wizard Settings";
		this->Load += gcnew System::EventHandler(this, &SettingsDialog::LoadHandler);
		this->mExternalScriptsGroupbox->ResumeLayout(false);
		this->mExternalScriptsGroupbox->PerformLayout();
		this->mDataGenerationGroupbox->ResumeLayout(false);
		this->mDataGenerationGroupbox->PerformLayout();
		this->mExternalFilesGroupbox->ResumeLayout(false);
		this->mExternalFilesGroupbox->PerformLayout();
		this->ResumeLayout(false);

	}
#pragma endregion
#pragma endregion
};

} // namespace WIZARD
