/*****************************************************************************\
**
** ConfigReplacementFluid.h
**
** Platelets config page.
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

public ref class ConfigReplacementFluid : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigReplacementFluid(void)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigReplacementFluid(void)
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

		ConfigUtil::ValidateTextfieldInt(mThresholdTextfield, KEY_MIN_REPLACEMENT_VOLUME, info);
		ConfigUtil::ValidateTextfieldFloat(mBalanceTextfield, KEY_RBC_FLUID_PERCENT, info);

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
		ConfigUtil::SetCheckBoxState(mReplacementCheckBox, config.predictionCfg.key_rbc_replace_fluid);
		ConfigUtil::SetTextfieldInt(mThresholdTextfield, config.procedureCfg.key_min_replacement_volume);
		ConfigUtil::SetTextfieldFloat(mBalanceTextfield, config.predictionCfg.key_rbc_fluid_percent);
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
		config.predictionCfg.key_rbc_replace_fluid = ConfigUtil::GetCheckBoxState(mReplacementCheckBox);
		config.procedureCfg.key_min_replacement_volume = ConfigUtil::GetTextfieldInt(mThresholdTextfield);
		config.predictionCfg.key_rbc_fluid_percent = ConfigUtil::GetTextfieldFloat(mBalanceTextfield);
	}
#pragma endregion

#pragma region Members
private: System::Windows::Forms::GroupBox^  mReplacementGroupBox;
private: System::Windows::Forms::TextBox^  mBalanceTextfield;
private: System::Windows::Forms::Label^  mBalanceLabel;
private: System::Windows::Forms::TextBox^  mThresholdTextfield;
private: System::Windows::Forms::Label^  mThresholdLabel;
private: System::Windows::Forms::CheckBox^  mReplacementCheckBox;
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
		this->mReplacementGroupBox = (gcnew System::Windows::Forms::GroupBox());
		this->mBalanceTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mBalanceLabel = (gcnew System::Windows::Forms::Label());
		this->mThresholdTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mThresholdLabel = (gcnew System::Windows::Forms::Label());
		this->mReplacementCheckBox = (gcnew System::Windows::Forms::CheckBox());
		this->mReplacementGroupBox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mReplacementGroupBox
		// 
		this->mReplacementGroupBox->Controls->Add(this->mBalanceTextfield);
		this->mReplacementGroupBox->Controls->Add(this->mBalanceLabel);
		this->mReplacementGroupBox->Controls->Add(this->mThresholdTextfield);
		this->mReplacementGroupBox->Controls->Add(this->mThresholdLabel);
		this->mReplacementGroupBox->Controls->Add(this->mReplacementCheckBox);
		this->mReplacementGroupBox->Location = System::Drawing::Point(3, 12);
		this->mReplacementGroupBox->Name = L"mReplacementGroupBox";
		this->mReplacementGroupBox->Size = System::Drawing::Size(570, 110);
		this->mReplacementGroupBox->TabIndex = 0;
		this->mReplacementGroupBox->TabStop = false;
		this->mReplacementGroupBox->Text = L"Replacement Fluid";
		// 
		// mBalanceTextfield
		// 
		this->mBalanceTextfield->Location = System::Drawing::Point(154, 70);
		this->mBalanceTextfield->Name = L"mBalanceTextfield";
		this->mBalanceTextfield->Size = System::Drawing::Size(100, 20);
		this->mBalanceTextfield->TabIndex = 4;
		this->mBalanceTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mBalanceLabel
		// 
		this->mBalanceLabel->AutoSize = true;
		this->mBalanceLabel->Location = System::Drawing::Point(6, 73);
		this->mBalanceLabel->Name = L"mBalanceLabel";
		this->mBalanceLabel->Size = System::Drawing::Size(91, 13);
		this->mBalanceLabel->TabIndex = 3;
		this->mBalanceLabel->Text = L"Fluid Balance (%):";
		// 
		// mThresholdTextfield
		// 
		this->mThresholdTextfield->Location = System::Drawing::Point(154, 43);
		this->mThresholdTextfield->Name = L"mThresholdTextfield";
		this->mThresholdTextfield->Size = System::Drawing::Size(100, 20);
		this->mThresholdTextfield->TabIndex = 2;
		this->mThresholdTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mThresholdLabel
		// 
		this->mThresholdLabel->AutoSize = true;
		this->mThresholdLabel->Location = System::Drawing::Point(6, 46);
		this->mThresholdLabel->Name = L"mThresholdLabel";
		this->mThresholdLabel->Size = System::Drawing::Size(142, 13);
		this->mThresholdLabel->TabIndex = 1;
		this->mThresholdLabel->Text = L"Replacement Threshold (ml):";
		// 
		// mReplacementCheckBox
		// 
		this->mReplacementCheckBox->AutoSize = true;
		this->mReplacementCheckBox->Location = System::Drawing::Point(7, 20);
		this->mReplacementCheckBox->Name = L"mReplacementCheckBox";
		this->mReplacementCheckBox->Size = System::Drawing::Size(114, 17);
		this->mReplacementCheckBox->TabIndex = 0;
		this->mReplacementCheckBox->Text = L"Replacement Fluid";
		this->mReplacementCheckBox->UseVisualStyleBackColor = true;
		// 
		// ConfigReplacementFluid
		// 
		this->Controls->Add(this->mReplacementGroupBox);
		this->Name = L"ConfigReplacementFluid";
		this->Size = System::Drawing::Size(599, 300);
		this->mReplacementGroupBox->ResumeLayout(false);
		this->mReplacementGroupBox->PerformLayout();
		this->ResumeLayout(false);

	}
#pragma endregion
#pragma endregion
};
}
