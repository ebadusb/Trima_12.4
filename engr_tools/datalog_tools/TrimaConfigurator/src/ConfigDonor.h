/*****************************************************************************\
**
** ConfigDonor.h
**
** Procedure config page.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "ConfigPage.h"
#include "ConfigUtil.h"

namespace CONFIGURATOR {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

/* Classes *******************************************************************/

public ref class ConfigDonor : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigDonor(void)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigDonor(void)
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

		ConfigUtil::ValidateTextfieldInt(mACInfusionRateManagementTextfield, KEY_AC_RATE, info);
		ConfigUtil::ValidateTextfieldFloat(mMinDonorPostHematocritTextfield, KEY_POST_CRIT, info);
		ConfigUtil::ValidateTextfieldFloat(mMinDonorPostPlateletCountTextfield, KEY_POST_PLAT, info);
		ConfigUtil::ValidateTextfieldInt(mDrawManagemenTextfield, KEY_INLET_MANAGEMENT, info);
		ConfigUtil::ValidateTextfieldInt(mReturnManagementTextfield, KEY_RETURN_MANAGEMENT, info);

		return info;
	}

	/**
	 *
	 * UpdateConfigurations
	 *
	 * Updates the configurations.
	 *
	 * @param config The configuration structure.
	 *
	 */
	virtual void UpdateConfigurations(ConfigStruct &config)
	{
		ConfigUtil::SetTextfieldInt(mACInfusionRateManagementTextfield, config.procedureCfg.key_ac_rate);
		ConfigUtil::SetTextfieldFloat(mMinDonorPostHematocritTextfield, config.procedureCfg.key_post_crit);
		ConfigUtil::SetTextfieldFloat(mMinDonorPostPlateletCountTextfield, config.procedureCfg.key_post_plat);
		ConfigUtil::SetListSelection(mMaximumDrawFlowList, config.procedureCfg.key_max_draw_flow);
		ConfigUtil::SetCheckBoxState(mInitialDrawRampCheckbox, config.predictionCfg.key_inlet_flow_ramp);
		ConfigUtil::SetTextfieldInt(mDrawManagemenTextfield, config.predictionCfg.key_inlet_management);
		ConfigUtil::SetTextfieldInt(mReturnManagementTextfield, config.predictionCfg.key_return_management);
	}

	/**
	 *
	 * RefreshConfigurations
	 *
	 * Refreshes the configurations.
	 *
	 * @param config The configuration structure.
	 *
	 */
	virtual void RefreshConfigurations(ConfigStruct & config)
	{
		config.procedureCfg.key_ac_rate = ConfigUtil::GetTextfieldInt(mACInfusionRateManagementTextfield);
		config.procedureCfg.key_post_crit = ConfigUtil::GetTextfieldFloat(mMinDonorPostHematocritTextfield);
		config.procedureCfg.key_post_plat = ConfigUtil::GetTextfieldFloat(mMinDonorPostPlateletCountTextfield);
		config.procedureCfg.key_max_draw_flow = ConfigUtil::GetListSelection(mMaximumDrawFlowList);
		config.predictionCfg.key_inlet_flow_ramp = ConfigUtil::GetCheckBoxState(mInitialDrawRampCheckbox);
		config.predictionCfg.key_inlet_management = ConfigUtil::GetTextfieldInt(mDrawManagemenTextfield);
		config.predictionCfg.key_return_management = ConfigUtil::GetTextfieldInt(mReturnManagementTextfield);
	}
#pragma endregion

#pragma region Members
private: System::Windows::Forms::Label^  mACInfusionRateManagementLabel;
private: System::Windows::Forms::TextBox^  mACInfusionRateManagementTextfield;
private: System::Windows::Forms::Label^  mMinDonorPostHematocritLabel;
private: System::Windows::Forms::TextBox^  mMinDonorPostHematocritTextfield;
private: System::Windows::Forms::Label^  mMinDonorPostPlateletCountLabel;
private: System::Windows::Forms::TextBox^  mMinDonorPostPlateletCountTextfield;
private: System::Windows::Forms::Label^  mMaximumDrawFlowLabel;
private: System::Windows::Forms::TextBox^  mDrawManagemenTextfield;
private: System::Windows::Forms::Label^  mDrawManagemenLabel;
private: System::Windows::Forms::ComboBox^  mMaximumDrawFlowList;
private: System::Windows::Forms::CheckBox^  mInitialDrawRampCheckbox;
private: System::Windows::Forms::Label^  mReturnManagementLabel;
private: System::Windows::Forms::TextBox^  mReturnManagementTextfield;
private: System::Windows::Forms::GroupBox^  mDrawGroupBox;
private: System::Windows::Forms::GroupBox^  mMinimumsGroupBox;
#pragma endregion

#pragma region Generated
private:
	/// <summary>
	/// Required designer variable.
	/// </summary>
	System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
	/// <summary>
	/// Required method for Designer support - do not modify
	/// the contents of this method with the code editor.
	/// </summary>
	void InitializeComponent(void)
	{
		this->mACInfusionRateManagementLabel = (gcnew System::Windows::Forms::Label());
		this->mMinDonorPostHematocritLabel = (gcnew System::Windows::Forms::Label());
		this->mACInfusionRateManagementTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mMinDonorPostHematocritTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mMinDonorPostPlateletCountLabel = (gcnew System::Windows::Forms::Label());
		this->mMinDonorPostPlateletCountTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mMaximumDrawFlowLabel = (gcnew System::Windows::Forms::Label());
		this->mDrawManagemenTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mDrawManagemenLabel = (gcnew System::Windows::Forms::Label());
		this->mMaximumDrawFlowList = (gcnew System::Windows::Forms::ComboBox());
		this->mInitialDrawRampCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mReturnManagementLabel = (gcnew System::Windows::Forms::Label());
		this->mReturnManagementTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mDrawGroupBox = (gcnew System::Windows::Forms::GroupBox());
		this->mMinimumsGroupBox = (gcnew System::Windows::Forms::GroupBox());
		this->mDrawGroupBox->SuspendLayout();
		this->mMinimumsGroupBox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mACInfusionRateManagementLabel
		// 
		this->mACInfusionRateManagementLabel->AutoSize = true;
		this->mACInfusionRateManagementLabel->Location = System::Drawing::Point(16, 17);
		this->mACInfusionRateManagementLabel->Name = L"mACInfusionRateManagementLabel";
		this->mACInfusionRateManagementLabel->Size = System::Drawing::Size(155, 13);
		this->mACInfusionRateManagementLabel->TabIndex = 0;
		this->mACInfusionRateManagementLabel->Text = L"AC Infusion Rate Management:";
		// 
		// mMinDonorPostHematocritLabel
		// 
		this->mMinDonorPostHematocritLabel->AutoSize = true;
		this->mMinDonorPostHematocritLabel->Location = System::Drawing::Point(10, 16);
		this->mMinDonorPostHematocritLabel->Name = L"mMinDonorPostHematocritLabel";
		this->mMinDonorPostHematocritLabel->Size = System::Drawing::Size(175, 13);
		this->mMinDonorPostHematocritLabel->TabIndex = 1;
		this->mMinDonorPostHematocritLabel->Text = L"Minimum Donor Post Hematocrit (%)";
		// 
		// mACInfusionRateManagementTextfield
		// 
		this->mACInfusionRateManagementTextfield->Location = System::Drawing::Point(211, 14);
		this->mACInfusionRateManagementTextfield->Name = L"mACInfusionRateManagementTextfield";
		this->mACInfusionRateManagementTextfield->Size = System::Drawing::Size(100, 20);
		this->mACInfusionRateManagementTextfield->TabIndex = 2;
		this->mACInfusionRateManagementTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mMinDonorPostHematocritTextfield
		// 
		this->mMinDonorPostHematocritTextfield->Location = System::Drawing::Point(205, 13);
		this->mMinDonorPostHematocritTextfield->Name = L"mMinDonorPostHematocritTextfield";
		this->mMinDonorPostHematocritTextfield->Size = System::Drawing::Size(100, 20);
		this->mMinDonorPostHematocritTextfield->TabIndex = 3;
		this->mMinDonorPostHematocritTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mMinDonorPostPlateletCountLabel
		// 
		this->mMinDonorPostPlateletCountLabel->AutoSize = true;
		this->mMinDonorPostPlateletCountLabel->Location = System::Drawing::Point(10, 43);
		this->mMinDonorPostPlateletCountLabel->Name = L"mMinDonorPostPlateletCountLabel";
		this->mMinDonorPostPlateletCountLabel->Size = System::Drawing::Size(176, 13);
		this->mMinDonorPostPlateletCountLabel->TabIndex = 4;
		this->mMinDonorPostPlateletCountLabel->Text = L"Minimum Donor Post Platelet Count:";
		// 
		// mMinDonorPostPlateletCountTextfield
		// 
		this->mMinDonorPostPlateletCountTextfield->Location = System::Drawing::Point(205, 40);
		this->mMinDonorPostPlateletCountTextfield->Name = L"mMinDonorPostPlateletCountTextfield";
		this->mMinDonorPostPlateletCountTextfield->Size = System::Drawing::Size(100, 20);
		this->mMinDonorPostPlateletCountTextfield->TabIndex = 5;
		this->mMinDonorPostPlateletCountTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mMaximumDrawFlowLabel
		// 
		this->mMaximumDrawFlowLabel->AutoSize = true;
		this->mMaximumDrawFlowLabel->Location = System::Drawing::Point(10, 15);
		this->mMaximumDrawFlowLabel->Name = L"mMaximumDrawFlowLabel";
		this->mMaximumDrawFlowLabel->Size = System::Drawing::Size(104, 13);
		this->mMaximumDrawFlowLabel->TabIndex = 9;
		this->mMaximumDrawFlowLabel->Text = L"Maximum Draw Flow";
		// 
		// mDrawManagemenTextfield
		// 
		this->mDrawManagemenTextfield->Location = System::Drawing::Point(205, 39);
		this->mDrawManagemenTextfield->Name = L"mDrawManagemenTextfield";
		this->mDrawManagemenTextfield->Size = System::Drawing::Size(100, 20);
		this->mDrawManagemenTextfield->TabIndex = 10;
		this->mDrawManagemenTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mDrawManagemenLabel
		// 
		this->mDrawManagemenLabel->AutoSize = true;
		this->mDrawManagemenLabel->Location = System::Drawing::Point(10, 42);
		this->mDrawManagemenLabel->Name = L"mDrawManagemenLabel";
		this->mDrawManagemenLabel->Size = System::Drawing::Size(100, 13);
		this->mDrawManagemenLabel->TabIndex = 11;
		this->mDrawManagemenLabel->Text = L"Draw Management:";
		// 
		// mMaximumDrawFlowList
		// 
		this->mMaximumDrawFlowList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mMaximumDrawFlowList->FormattingEnabled = true;
		this->mMaximumDrawFlowList->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Slow", L"Moderate", L"Fast"});
		this->mMaximumDrawFlowList->Location = System::Drawing::Point(205, 12);
		this->mMaximumDrawFlowList->Name = L"mMaximumDrawFlowList";
		this->mMaximumDrawFlowList->Size = System::Drawing::Size(100, 21);
		this->mMaximumDrawFlowList->TabIndex = 15;
		// 
		// mInitialDrawRampCheckbox
		// 
		this->mInitialDrawRampCheckbox->AutoSize = true;
		this->mInitialDrawRampCheckbox->Location = System::Drawing::Point(13, 68);
		this->mInitialDrawRampCheckbox->Name = L"mInitialDrawRampCheckbox";
		this->mInitialDrawRampCheckbox->Size = System::Drawing::Size(109, 17);
		this->mInitialDrawRampCheckbox->TabIndex = 16;
		this->mInitialDrawRampCheckbox->Text = L"Initial Draw Ramp";
		this->mInitialDrawRampCheckbox->UseVisualStyleBackColor = true;
		// 
		// mReturnManagementLabel
		// 
		this->mReturnManagementLabel->AutoSize = true;
		this->mReturnManagementLabel->Location = System::Drawing::Point(16, 259);
		this->mReturnManagementLabel->Name = L"mReturnManagementLabel";
		this->mReturnManagementLabel->Size = System::Drawing::Size(107, 13);
		this->mReturnManagementLabel->TabIndex = 18;
		this->mReturnManagementLabel->Text = L"Return Management:";
		// 
		// mReturnManagementTextfield
		// 
		this->mReturnManagementTextfield->Location = System::Drawing::Point(211, 256);
		this->mReturnManagementTextfield->Name = L"mReturnManagementTextfield";
		this->mReturnManagementTextfield->Size = System::Drawing::Size(100, 20);
		this->mReturnManagementTextfield->TabIndex = 17;
		this->mReturnManagementTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mDrawGroupBox
		// 
		this->mDrawGroupBox->Controls->Add(this->mMaximumDrawFlowLabel);
		this->mDrawGroupBox->Controls->Add(this->mDrawManagemenTextfield);
		this->mDrawGroupBox->Controls->Add(this->mDrawManagemenLabel);
		this->mDrawGroupBox->Controls->Add(this->mInitialDrawRampCheckbox);
		this->mDrawGroupBox->Controls->Add(this->mMaximumDrawFlowList);
		this->mDrawGroupBox->Location = System::Drawing::Point(6, 137);
		this->mDrawGroupBox->Name = L"mDrawGroupBox";
		this->mDrawGroupBox->Size = System::Drawing::Size(568, 104);
		this->mDrawGroupBox->TabIndex = 19;
		this->mDrawGroupBox->TabStop = false;
		this->mDrawGroupBox->Text = L"Draw";
		// 
		// mMinimumsGroupBox
		// 
		this->mMinimumsGroupBox->Controls->Add(this->mMinDonorPostHematocritLabel);
		this->mMinimumsGroupBox->Controls->Add(this->mMinDonorPostHematocritTextfield);
		this->mMinimumsGroupBox->Controls->Add(this->mMinDonorPostPlateletCountLabel);
		this->mMinimumsGroupBox->Controls->Add(this->mMinDonorPostPlateletCountTextfield);
		this->mMinimumsGroupBox->Location = System::Drawing::Point(6, 48);
		this->mMinimumsGroupBox->Name = L"mMinimumsGroupBox";
		this->mMinimumsGroupBox->Size = System::Drawing::Size(568, 83);
		this->mMinimumsGroupBox->TabIndex = 20;
		this->mMinimumsGroupBox->TabStop = false;
		this->mMinimumsGroupBox->Text = L"Minimums";
		// 
		// ConfigDonor
		// 
		this->Controls->Add(this->mMinimumsGroupBox);
		this->Controls->Add(this->mReturnManagementLabel);
		this->Controls->Add(this->mReturnManagementTextfield);
		this->Controls->Add(this->mDrawGroupBox);
		this->Controls->Add(this->mACInfusionRateManagementTextfield);
		this->Controls->Add(this->mACInfusionRateManagementLabel);
		this->Name = L"ConfigDonor";
		this->Size = System::Drawing::Size(599, 300);
		this->mDrawGroupBox->ResumeLayout(false);
		this->mDrawGroupBox->PerformLayout();
		this->mMinimumsGroupBox->ResumeLayout(false);
		this->mMinimumsGroupBox->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};
}
