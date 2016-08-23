/*****************************************************************************\
**
** ConfigDonorVolumeLimits.h
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

/* Constants *****************************************************************/

enum VOLUME_REMOVAL_TYPE {
   VOLUME_REMOVAL_TBV = 1,
   VOLUME_REMOVAL_WEIGHT = 2,
   VOLUME_REMOVAL_ML_PER_KG = 4,
   VOLUME_REMOVAL_DRBC_BODY_VOL = 8
};

/* Classes *******************************************************************/

public ref class ConfigDonorVolumeLimits : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigDonorVolumeLimits(void)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigDonorVolumeLimits(void)
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

		ConfigUtil::ValidateTextfieldFloat(mVolWeightTextfield, KEY_WEIGHT_3_SETTING, info);
		ConfigUtil::ValidateTextfieldFloat(mVolMinTextfield, KEY_WEIGHT_3_LESS_THAN_VOL, info);
		ConfigUtil::ValidateTextfieldFloat(mVolMaxTextfield, KEY_WEIGHT_3_GREATER_THAN_VOL, info);
		ConfigUtil::ValidateTextfieldFloat(mBodyWeightTextfield, KEY_ML_PER_KG, info);
		ConfigUtil::ValidateTextfieldFloat(mTBVPercentTextfield, KEY_TBV_PERCENT, info);
		ConfigUtil::ValidateTextfieldFloat(mDRBCVolumeTextfield, KEY_DRBC_BODY_VOL, info);

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
		ConfigUtil::SetCheckBoxState(mVolumeCheckBox, config.procedureCfg.key_tbv_vol_setting & (int)VOLUME_REMOVAL_WEIGHT);
		ConfigUtil::SetTextfieldFloat(mVolWeightTextfield, config.procedureCfg.key_weight_3_setting);
		ConfigUtil::SetTextfieldFloat(mVolMinTextfield, config.procedureCfg.key_weight_3_less_than_vol);
		ConfigUtil::SetTextfieldFloat(mVolMaxTextfield, config.procedureCfg.key_weight_3_greater_than_vol);
		ConfigUtil::SetCheckBoxState(mWeightCheckBox, config.procedureCfg.key_tbv_vol_setting & (int)VOLUME_REMOVAL_ML_PER_KG);
		ConfigUtil::SetTextfieldFloat(mBodyWeightTextfield, config.procedureCfg.key_ml_per_kg);
		ConfigUtil::SetCheckBoxState(mTBVCheckBox, config.procedureCfg.key_tbv_vol_setting & (int)VOLUME_REMOVAL_TBV);
		ConfigUtil::SetTextfieldFloat(mTBVPercentTextfield, config.procedureCfg.key_tbv_percent);
		ConfigUtil::SetCheckBoxState(mDRBCCheckBox, config.procedureCfg.key_tbv_vol_setting & (int)VOLUME_REMOVAL_DRBC_BODY_VOL);
		ConfigUtil::SetTextfieldFloat(mDRBCVolumeTextfield, config.procedureCfg.key_drbc_body_vol);
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
		config.procedureCfg.key_tbv_vol_setting = 0;

		if (ConfigUtil::GetCheckBoxState(mVolumeCheckBox)) {
			config.procedureCfg.key_tbv_vol_setting |= (int)VOLUME_REMOVAL_WEIGHT;
		}
		
		config.procedureCfg.key_weight_3_setting = ConfigUtil::GetTextfieldFloat(mVolWeightTextfield);
		config.procedureCfg.key_weight_3_less_than_vol = ConfigUtil::GetTextfieldFloat(mVolMinTextfield);
		config.procedureCfg.key_weight_3_greater_than_vol = ConfigUtil::GetTextfieldFloat(mVolMaxTextfield);
		
		if (ConfigUtil::GetCheckBoxState(mWeightCheckBox)) {
			config.procedureCfg.key_tbv_vol_setting |= (int)VOLUME_REMOVAL_ML_PER_KG;
		}

		config.procedureCfg.key_ml_per_kg = ConfigUtil::GetTextfieldFloat(mBodyWeightTextfield);

		if (ConfigUtil::GetCheckBoxState(mTBVCheckBox)) {
			config.procedureCfg.key_tbv_vol_setting |= (int)VOLUME_REMOVAL_TBV;
		}

		config.procedureCfg.key_tbv_percent = ConfigUtil::GetTextfieldFloat(mTBVPercentTextfield);
	
		if (ConfigUtil::GetCheckBoxState(mDRBCCheckBox)) {
			config.procedureCfg.key_tbv_vol_setting |= (int)VOLUME_REMOVAL_DRBC_BODY_VOL;
		}

		config.procedureCfg.key_drbc_body_vol = ConfigUtil::GetTextfieldFloat(mDRBCVolumeTextfield);
	}
#pragma endregion

#pragma region Members
private: System::Windows::Forms::GroupBox^  mVolumeGroupBox;
private: System::Windows::Forms::TextBox^  mVolMinTextfield;
private: System::Windows::Forms::Label^  mVolMinLabel;
private: System::Windows::Forms::TextBox^  mVolWeightTextfield;
private: System::Windows::Forms::Label^  mVolWeightLabel;
private: System::Windows::Forms::CheckBox^  mVolumeCheckBox;
private: System::Windows::Forms::Label^  mVolMaxLabel;
private: System::Windows::Forms::TextBox^  mVolMaxTextfield;
private: System::Windows::Forms::GroupBox^  mTBVGroupBox;
private: System::Windows::Forms::Label^  mTBVPercentLabel;
private: System::Windows::Forms::TextBox^  mTBVPercentTextfield;
private: System::Windows::Forms::CheckBox^  mTBVCheckBox;
private: System::Windows::Forms::GroupBox^  mDRBCGroupBox;
private: System::Windows::Forms::Label^  mDRBCVolumeLabel;
private: System::Windows::Forms::CheckBox^  mDRBCCheckBox;
private: System::Windows::Forms::TextBox^  mDRBCVolumeTextfield;
private: System::Windows::Forms::GroupBox^  mWeightGroupBox;
private: System::Windows::Forms::Label^  mBodyWeightLabel;
private: System::Windows::Forms::CheckBox^  mWeightCheckBox;
private: System::Windows::Forms::TextBox^  mBodyWeightTextfield;
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
		this->mVolumeGroupBox = (gcnew System::Windows::Forms::GroupBox());
		this->mVolMaxLabel = (gcnew System::Windows::Forms::Label());
		this->mVolMaxTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolMinTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolMinLabel = (gcnew System::Windows::Forms::Label());
		this->mVolWeightTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolWeightLabel = (gcnew System::Windows::Forms::Label());
		this->mVolumeCheckBox = (gcnew System::Windows::Forms::CheckBox());
		this->mTBVGroupBox = (gcnew System::Windows::Forms::GroupBox());
		this->mTBVPercentLabel = (gcnew System::Windows::Forms::Label());
		this->mTBVPercentTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mTBVCheckBox = (gcnew System::Windows::Forms::CheckBox());
		this->mDRBCGroupBox = (gcnew System::Windows::Forms::GroupBox());
		this->mDRBCVolumeLabel = (gcnew System::Windows::Forms::Label());
		this->mDRBCCheckBox = (gcnew System::Windows::Forms::CheckBox());
		this->mDRBCVolumeTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mWeightGroupBox = (gcnew System::Windows::Forms::GroupBox());
		this->mBodyWeightLabel = (gcnew System::Windows::Forms::Label());
		this->mWeightCheckBox = (gcnew System::Windows::Forms::CheckBox());
		this->mBodyWeightTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolumeGroupBox->SuspendLayout();
		this->mTBVGroupBox->SuspendLayout();
		this->mDRBCGroupBox->SuspendLayout();
		this->mWeightGroupBox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mVolumeGroupBox
		// 
		this->mVolumeGroupBox->Controls->Add(this->mVolMaxLabel);
		this->mVolumeGroupBox->Controls->Add(this->mVolMaxTextfield);
		this->mVolumeGroupBox->Controls->Add(this->mVolMinTextfield);
		this->mVolumeGroupBox->Controls->Add(this->mVolMinLabel);
		this->mVolumeGroupBox->Controls->Add(this->mVolWeightTextfield);
		this->mVolumeGroupBox->Controls->Add(this->mVolWeightLabel);
		this->mVolumeGroupBox->Controls->Add(this->mVolumeCheckBox);
		this->mVolumeGroupBox->Location = System::Drawing::Point(3, 12);
		this->mVolumeGroupBox->Name = L"mVolumeGroupBox";
		this->mVolumeGroupBox->Size = System::Drawing::Size(278, 139);
		this->mVolumeGroupBox->TabIndex = 0;
		this->mVolumeGroupBox->TabStop = false;
		this->mVolumeGroupBox->Text = L"Volume";
		// 
		// mVolMaxLabel
		// 
		this->mVolMaxLabel->AutoSize = true;
		this->mVolMaxLabel->Location = System::Drawing::Point(6, 99);
		this->mVolMaxLabel->Name = L"mVolMaxLabel";
		this->mVolMaxLabel->Size = System::Drawing::Size(158, 13);
		this->mVolMaxLabel->TabIndex = 6;
		this->mVolMaxLabel->Text = L"Volume (ml) greater than weight:";
		// 
		// mVolMaxTextfield
		// 
		this->mVolMaxTextfield->Location = System::Drawing::Point(170, 96);
		this->mVolMaxTextfield->Name = L"mVolMaxTextfield";
		this->mVolMaxTextfield->Size = System::Drawing::Size(100, 20);
		this->mVolMaxTextfield->TabIndex = 5;
		this->mVolMaxTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mVolMinTextfield
		// 
		this->mVolMinTextfield->Location = System::Drawing::Point(170, 70);
		this->mVolMinTextfield->Name = L"mVolMinTextfield";
		this->mVolMinTextfield->Size = System::Drawing::Size(100, 20);
		this->mVolMinTextfield->TabIndex = 4;
		this->mVolMinTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mVolMinLabel
		// 
		this->mVolMinLabel->AutoSize = true;
		this->mVolMinLabel->Location = System::Drawing::Point(6, 73);
		this->mVolMinLabel->Name = L"mVolMinLabel";
		this->mVolMinLabel->Size = System::Drawing::Size(143, 13);
		this->mVolMinLabel->TabIndex = 3;
		this->mVolMinLabel->Text = L"Volume (ml) less than weight:";
		// 
		// mVolWeightTextfield
		// 
		this->mVolWeightTextfield->Location = System::Drawing::Point(170, 44);
		this->mVolWeightTextfield->Name = L"mVolWeightTextfield";
		this->mVolWeightTextfield->Size = System::Drawing::Size(100, 20);
		this->mVolWeightTextfield->TabIndex = 2;
		this->mVolWeightTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mVolWeightLabel
		// 
		this->mVolWeightLabel->AutoSize = true;
		this->mVolWeightLabel->Location = System::Drawing::Point(6, 46);
		this->mVolWeightLabel->Name = L"mVolWeightLabel";
		this->mVolWeightLabel->Size = System::Drawing::Size(61, 13);
		this->mVolWeightLabel->TabIndex = 1;
		this->mVolWeightLabel->Text = L"Weight (lb):";
		// 
		// mVolumeCheckBox
		// 
		this->mVolumeCheckBox->AutoSize = true;
		this->mVolumeCheckBox->Location = System::Drawing::Point(7, 20);
		this->mVolumeCheckBox->Name = L"mVolumeCheckBox";
		this->mVolumeCheckBox->Size = System::Drawing::Size(99, 17);
		this->mVolumeCheckBox->TabIndex = 0;
		this->mVolumeCheckBox->Text = L"Limit by Volume";
		this->mVolumeCheckBox->UseVisualStyleBackColor = true;
		// 
		// mTBVGroupBox
		// 
		this->mTBVGroupBox->Controls->Add(this->mTBVPercentLabel);
		this->mTBVGroupBox->Controls->Add(this->mTBVPercentTextfield);
		this->mTBVGroupBox->Controls->Add(this->mTBVCheckBox);
		this->mTBVGroupBox->Location = System::Drawing::Point(287, 12);
		this->mTBVGroupBox->Name = L"mTBVGroupBox";
		this->mTBVGroupBox->Size = System::Drawing::Size(288, 90);
		this->mTBVGroupBox->TabIndex = 1;
		this->mTBVGroupBox->TabStop = false;
		this->mTBVGroupBox->Text = L"TBV";
		// 
		// mTBVPercentLabel
		// 
		this->mTBVPercentLabel->AutoSize = true;
		this->mTBVPercentLabel->Location = System::Drawing::Point(6, 46);
		this->mTBVPercentLabel->Name = L"mTBVPercentLabel";
		this->mTBVPercentLabel->Size = System::Drawing::Size(48, 13);
		this->mTBVPercentLabel->TabIndex = 2;
		this->mTBVPercentLabel->Text = L"TBV (%):";
		// 
		// mTBVPercentTextfield
		// 
		this->mTBVPercentTextfield->Location = System::Drawing::Point(182, 43);
		this->mTBVPercentTextfield->Name = L"mTBVPercentTextfield";
		this->mTBVPercentTextfield->Size = System::Drawing::Size(100, 20);
		this->mTBVPercentTextfield->TabIndex = 1;
		this->mTBVPercentTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mTBVCheckBox
		// 
		this->mTBVCheckBox->AutoSize = true;
		this->mTBVCheckBox->Location = System::Drawing::Point(7, 20);
		this->mTBVCheckBox->Name = L"mTBVCheckBox";
		this->mTBVCheckBox->Size = System::Drawing::Size(85, 17);
		this->mTBVCheckBox->TabIndex = 0;
		this->mTBVCheckBox->Text = L"Limit by TBV";
		this->mTBVCheckBox->UseVisualStyleBackColor = true;
		// 
		// mDRBCGroupBox
		// 
		this->mDRBCGroupBox->Controls->Add(this->mDRBCVolumeLabel);
		this->mDRBCGroupBox->Controls->Add(this->mDRBCCheckBox);
		this->mDRBCGroupBox->Controls->Add(this->mDRBCVolumeTextfield);
		this->mDRBCGroupBox->Location = System::Drawing::Point(287, 111);
		this->mDRBCGroupBox->Name = L"mDRBCGroupBox";
		this->mDRBCGroupBox->Size = System::Drawing::Size(288, 83);
		this->mDRBCGroupBox->TabIndex = 2;
		this->mDRBCGroupBox->TabStop = false;
		this->mDRBCGroupBox->Text = L"DRBC Min TBV";
		// 
		// mDRBCVolumeLabel
		// 
		this->mDRBCVolumeLabel->AutoSize = true;
		this->mDRBCVolumeLabel->Location = System::Drawing::Point(4, 46);
		this->mDRBCVolumeLabel->Name = L"mDRBCVolumeLabel";
		this->mDRBCVolumeLabel->Size = System::Drawing::Size(60, 13);
		this->mDRBCVolumeLabel->TabIndex = 4;
		this->mDRBCVolumeLabel->Text = L"Volume (L):";
		// 
		// mDRBCCheckBox
		// 
		this->mDRBCCheckBox->AutoSize = true;
		this->mDRBCCheckBox->Location = System::Drawing::Point(7, 20);
		this->mDRBCCheckBox->Name = L"mDRBCCheckBox";
		this->mDRBCCheckBox->Size = System::Drawing::Size(138, 17);
		this->mDRBCCheckBox->TabIndex = 0;
		this->mDRBCCheckBox->Text = L"Limit by DRBC Min TBV";
		this->mDRBCCheckBox->UseVisualStyleBackColor = true;
		// 
		// mDRBCVolumeTextfield
		// 
		this->mDRBCVolumeTextfield->Location = System::Drawing::Point(182, 43);
		this->mDRBCVolumeTextfield->Name = L"mDRBCVolumeTextfield";
		this->mDRBCVolumeTextfield->Size = System::Drawing::Size(100, 20);
		this->mDRBCVolumeTextfield->TabIndex = 3;
		this->mDRBCVolumeTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mWeightGroupBox
		// 
		this->mWeightGroupBox->Controls->Add(this->mBodyWeightLabel);
		this->mWeightGroupBox->Controls->Add(this->mWeightCheckBox);
		this->mWeightGroupBox->Controls->Add(this->mBodyWeightTextfield);
		this->mWeightGroupBox->Location = System::Drawing::Point(3, 157);
		this->mWeightGroupBox->Name = L"mWeightGroupBox";
		this->mWeightGroupBox->Size = System::Drawing::Size(278, 83);
		this->mWeightGroupBox->TabIndex = 3;
		this->mWeightGroupBox->TabStop = false;
		this->mWeightGroupBox->Text = L"Body Weight";
		// 
		// mBodyWeightLabel
		// 
		this->mBodyWeightLabel->AutoSize = true;
		this->mBodyWeightLabel->Location = System::Drawing::Point(6, 42);
		this->mBodyWeightLabel->Name = L"mBodyWeightLabel";
		this->mBodyWeightLabel->Size = System::Drawing::Size(80, 13);
		this->mBodyWeightLabel->TabIndex = 6;
		this->mBodyWeightLabel->Text = L"Weight (ml/kg):";
		// 
		// mWeightCheckBox
		// 
		this->mWeightCheckBox->AutoSize = true;
		this->mWeightCheckBox->Location = System::Drawing::Point(9, 20);
		this->mWeightCheckBox->Name = L"mWeightCheckBox";
		this->mWeightCheckBox->Size = System::Drawing::Size(125, 17);
		this->mWeightCheckBox->TabIndex = 0;
		this->mWeightCheckBox->Text = L"Limit by Body Weight";
		this->mWeightCheckBox->UseVisualStyleBackColor = true;
		// 
		// mBodyWeightTextfield
		// 
		this->mBodyWeightTextfield->Location = System::Drawing::Point(170, 39);
		this->mBodyWeightTextfield->Name = L"mBodyWeightTextfield";
		this->mBodyWeightTextfield->Size = System::Drawing::Size(100, 20);
		this->mBodyWeightTextfield->TabIndex = 5;
		this->mBodyWeightTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// ConfigDonorVolumeLimits
		// 
		this->Controls->Add(this->mWeightGroupBox);
		this->Controls->Add(this->mDRBCGroupBox);
		this->Controls->Add(this->mTBVGroupBox);
		this->Controls->Add(this->mVolumeGroupBox);
		this->Name = L"ConfigDonorVolumeLimits";
		this->Size = System::Drawing::Size(599, 300);
		this->mVolumeGroupBox->ResumeLayout(false);
		this->mVolumeGroupBox->PerformLayout();
		this->mTBVGroupBox->ResumeLayout(false);
		this->mTBVGroupBox->PerformLayout();
		this->mDRBCGroupBox->ResumeLayout(false);
		this->mDRBCGroupBox->PerformLayout();
		this->mWeightGroupBox->ResumeLayout(false);
		this->mWeightGroupBox->PerformLayout();
		this->ResumeLayout(false);

	}
#pragma endregion
#pragma endregion
};
}
