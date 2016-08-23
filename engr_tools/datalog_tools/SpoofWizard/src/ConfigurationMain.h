/*****************************************************************************\
**
** SpoofBuilder.h
**
** Spoof builder implementation.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "SpoofBuilder.h"
#include "ConfigPage.h"

/* Class *********************************************************************/

namespace WIZARD {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;

public ref class ConfigurationMain : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigurationMain(BUILDER::SpoofBuilder *spoofBuilderP) 
			: mSpoofBuilderP(spoofBuilderP)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigurationMain()
	{
		if (components) {
			delete components;
		}
	}
#pragma endregion

#pragma region Functions
public:

	/**
	 *
	 * GetControl
	 *
	 * Gets the control associated with this configuration provider.
	 *
	 * @return The control.
	 *
	 */
	virtual System::Windows::Forms::UserControl^ GetControl(void) 
	{ 
		return this; 
	}

	/**
	 *
	 * ValidateConfigurations
	 *
	 * Validates the configurations.
	 *
	 * @return True if the contigurations are valid.
	 *
	 */
	virtual ValidationInfo ^ValidateConfigurations(void) 
	{ 
		ValidationInfo ^info = gcnew ValidationInfo();

		info->result = true;
		info->message = nullptr;
		info->control = nullptr;

		if (System::String::IsNullOrEmpty(mNameTextfield->Text->Trim())) {
			info->result = false;
			info->message = "Invalid spoof name.";
			info->control = mNameTextfield;
		}
		else if (System::String::IsNullOrEmpty(mTimeoutTextfield->Text->Trim())) {
			info->result = false;
			info->message = "Invalid timeout value.";
			info->control = mTimeoutTextfield;
		}

		return info; 
	}

	/**
	 *
	 * UpdateConfigurations
	 *
	 * Updates the configurations.
	 *
	 * @return True if the contigurations were updated.
	 *
	 */
	virtual ValidationInfo ^UpdateConfigurations(void)
	{ 
		ValidationInfo ^info = this->ValidateConfigurations();

		if (info->result) {
			BUILDER::SpoofBuilder::SpoofInfo info;
			System::IntPtr nameTextPtr = Marshal::StringToHGlobalAnsi(mNameTextfield->Text->Trim());
			System::IntPtr commentTextPtr = Marshal::StringToHGlobalAnsi(mCommentTextfield->Text->Trim());
			System::IntPtr helpTextPtr = Marshal::StringToHGlobalAnsi(mHelpTextField->Text->Trim());

			info.name = static_cast<const char*>(nameTextPtr.ToPointer());
			info.comment = static_cast<const char*>(commentTextPtr.ToPointer());
			info.help = static_cast<const char*>(helpTextPtr.ToPointer());
			info.timeout = Int32::Parse(mTimeoutTextfield->Text);

			Marshal::FreeHGlobal(nameTextPtr);
			Marshal::FreeHGlobal(commentTextPtr);
			Marshal::FreeHGlobal(helpTextPtr);

			mSpoofBuilderP->SetSpoofInfo(info);
		}
		
		return info; 
	}

	/**
	 *
	 * RefreshConfigurations
	 *
	 * Refreshes the configurations.
	 *
	 * @return True if the contigurations were updated.
	 *
	 */
	virtual ValidationInfo ^RefreshConfigurations(void)
	{
		ValidationInfo ^info = gcnew ValidationInfo();

		info->result = true;
		info->message = nullptr;
		info->control = nullptr;

		return info; 
	}
#pragma endregion

#pragma region Handlers
	void KeyPressHandler(Object ^o, System::Windows::Forms::KeyPressEventArgs ^e)
	{
		if (!Char::IsNumber(e->KeyChar) 
				&& (e->KeyChar != 127) 
				&& (e->KeyChar != 010)) {
			e->Handled = true;
		}
	}
#pragma endregion

#pragma region Members
	private: BUILDER::SpoofBuilder *mSpoofBuilderP;
	private: System::Windows::Forms::TextBox^  mTimeoutTextfield;
	private: System::Windows::Forms::TextBox^  mHelpTextField;
	private: System::Windows::Forms::TextBox^  mCommentTextfield;
	private: System::Windows::Forms::TextBox^  mNameTextfield;
	private: System::Windows::Forms::Label^  mTimeoutLabel;
	private: System::Windows::Forms::Label^  mHelpLabel;
	private: System::Windows::Forms::Label^  mCommentLabel;
	private: System::Windows::Forms::Label^  mNameLabel;
#pragma endregion

#pragma region Generated
private:
	System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
	void InitializeComponent(void)
	{
		this->mTimeoutTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mHelpTextField = (gcnew System::Windows::Forms::TextBox());
		this->mCommentTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mNameTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mTimeoutLabel = (gcnew System::Windows::Forms::Label());
		this->mHelpLabel = (gcnew System::Windows::Forms::Label());
		this->mCommentLabel = (gcnew System::Windows::Forms::Label());
		this->mNameLabel = (gcnew System::Windows::Forms::Label());
		this->SuspendLayout();
		// 
		// mTimeoutTextfield
		// 
		this->mTimeoutTextfield->Location = System::Drawing::Point(86, 255);
		this->mTimeoutTextfield->Name = L"mTimeoutTextfield";
		this->mTimeoutTextfield->Size = System::Drawing::Size(100, 20);
		this->mTimeoutTextfield->TabIndex = 15;
		// 
		// mHelpTextField
		// 
		this->mHelpTextField->AcceptsReturn = true;
		this->mHelpTextField->Location = System::Drawing::Point(86, 149);
		this->mHelpTextField->Multiline = true;
		this->mHelpTextField->Name = L"mHelpTextField";
		this->mHelpTextField->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
		this->mHelpTextField->Size = System::Drawing::Size(480, 100);
		this->mHelpTextField->TabIndex = 14;
		// 
		// mCommentTextfield
		// 
		this->mCommentTextfield->AcceptsReturn = true;
		this->mCommentTextfield->Location = System::Drawing::Point(86, 43);
		this->mCommentTextfield->Multiline = true;
		this->mCommentTextfield->Name = L"mCommentTextfield";
		this->mCommentTextfield->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
		this->mCommentTextfield->Size = System::Drawing::Size(480, 100);
		this->mCommentTextfield->TabIndex = 13;
		// 
		// mNameTextfield
		// 
		this->mNameTextfield->Location = System::Drawing::Point(86, 16);
		this->mNameTextfield->Name = L"mNameTextfield";
		this->mNameTextfield->Size = System::Drawing::Size(300, 20);
		this->mNameTextfield->TabIndex = 12;
		// 
		// mTimeoutLabel
		// 
		this->mTimeoutLabel->AutoSize = true;
		this->mTimeoutLabel->Location = System::Drawing::Point(15, 258);
		this->mTimeoutLabel->Name = L"mTimeoutLabel";
		this->mTimeoutLabel->Size = System::Drawing::Size(48, 13);
		this->mTimeoutLabel->TabIndex = 11;
		this->mTimeoutLabel->Text = L"Timeout:";
		this->mTimeoutTextfield->KeyPress 
				+= gcnew KeyPressEventHandler(this, &ConfigurationMain::KeyPressHandler);
		// 
		// mHelpLabel
		// 
		this->mHelpLabel->AutoSize = true;
		this->mHelpLabel->Location = System::Drawing::Point(15, 152);
		this->mHelpLabel->Name = L"mHelpLabel";
		this->mHelpLabel->Size = System::Drawing::Size(32, 13);
		this->mHelpLabel->TabIndex = 10;
		this->mHelpLabel->Text = L"Help:";
		// 
		// mCommentLabel
		// 
		this->mCommentLabel->AutoSize = true;
		this->mCommentLabel->Location = System::Drawing::Point(15, 46);
		this->mCommentLabel->Name = L"mCommentLabel";
		this->mCommentLabel->Size = System::Drawing::Size(54, 13);
		this->mCommentLabel->TabIndex = 9;
		this->mCommentLabel->Text = L"Comment:";
		// 
		// mNameLabel
		// 
		this->mNameLabel->AutoSize = true;
		this->mNameLabel->Location = System::Drawing::Point(15, 19);
		this->mNameLabel->Name = L"mNameLabel";
		this->mNameLabel->Size = System::Drawing::Size(38, 13);
		this->mNameLabel->TabIndex = 8;
		this->mNameLabel->Text = L"Name:";
		// 
		// ConfigurationMain
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->Controls->Add(this->mTimeoutTextfield);
		this->Controls->Add(this->mHelpTextField);
		this->Controls->Add(this->mCommentTextfield);
		this->Controls->Add(this->mNameTextfield);
		this->Controls->Add(this->mTimeoutLabel);
		this->Controls->Add(this->mHelpLabel);
		this->Controls->Add(this->mCommentLabel);
		this->Controls->Add(this->mNameLabel);
		this->Name = L"ConfigurationMain";
		this->Size = System::Drawing::Size(590, 340);
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};

} // namespace WIZARD
