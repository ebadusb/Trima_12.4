/*****************************************************************************\
**
** ConfigRedBloodCells.h
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

public ref class ConfigRedBloodCells : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigRedBloodCells(void)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigRedBloodCells(void)
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

		ConfigUtil::ValidateTextfieldFloat(mDose1Textfield, KEY_RBC_DOSE_1, info);
		ConfigUtil::ValidateTextfieldFloat(mHematocrit1Textfield, KEY_RBC_CRIT_1, info);
		ConfigUtil::ValidateTextfieldInt(mVolume1Textfield, KEY_RBC_MSS_VOLUME_1, info);
		ConfigUtil::ValidateTextfieldFloat(mDose2Textfield, KEY_RBC_DOSE_2, info);
		ConfigUtil::ValidateTextfieldFloat(mHematocrit2Textfield, KEY_RBC_CRIT_2, info);
		ConfigUtil::ValidateTextfieldInt(mVolume2Textfield, KEY_RBC_MSS_VOLUME_2, info);
		ConfigUtil::ValidateTextfieldFloat(mDose3Textfield, KEY_RBC_DOSE_3, info);
		ConfigUtil::ValidateTextfieldFloat(mHematocrit3Textfield, KEY_RBC_CRIT_3, info);
		ConfigUtil::ValidateTextfieldInt(mVolume3Textfield, KEY_RBC_MSS_VOLUME_3, info);
		ConfigUtil::ValidateTextfieldFloat(mScalingFactorTextfield, KEY_RSF, info);
		ConfigUtil::ValidateTextfieldInt(mThresholdTextfield, KEY_DRBC_THRESHOLD, info);

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
		mIndex1Textfield->Text = "1";
		ConfigUtil::SetCheckBoxState(mAdditive1Checkbox, config.prodTemplates.key_rbc_mss[0]);
		ConfigUtil::SetTextfieldFloat(mDose1Textfield, config.prodTemplates.key_rbc_dose[0]);
		ConfigUtil::SetTextfieldFloat(mHematocrit1Textfield, config.prodTemplates.key_rbc_crit[0]);
		ConfigUtil::SetTextfieldInt(mVolume1Textfield, config.prodTemplates.key_rbc_mss_volume[0]);

		mIndex2Textfield->Text = "2";
		ConfigUtil::SetCheckBoxState(mAdditive2Checkbox, config.prodTemplates.key_rbc_mss[1]);
		ConfigUtil::SetTextfieldFloat(mDose2Textfield, config.prodTemplates.key_rbc_dose[1]);
		ConfigUtil::SetTextfieldFloat(mHematocrit2Textfield, config.prodTemplates.key_rbc_crit[1]);
		ConfigUtil::SetTextfieldInt(mVolume2Textfield, config.prodTemplates.key_rbc_mss_volume[1]);

		mIndex3Textfield->Text = "3";
		ConfigUtil::SetCheckBoxState(mAdditive3Checkbox, config.prodTemplates.key_rbc_mss[2]);
		ConfigUtil::SetTextfieldFloat(mDose3Textfield, config.prodTemplates.key_rbc_dose[2]);
		ConfigUtil::SetTextfieldFloat(mHematocrit3Textfield, config.prodTemplates.key_rbc_crit[2]);
		ConfigUtil::SetTextfieldInt(mVolume3Textfield, config.prodTemplates.key_rbc_mss_volume[2]);

		ConfigUtil::SetTextfieldFloat(mScalingFactorTextfield, config.predictionCfg.key_rsf);
		ConfigUtil::SetCheckBoxState(mThresholdCheckbox, config.procedureCfg.key_drbc_alert);
		ConfigUtil::SetTextfieldInt(mThresholdTextfield, config.procedureCfg.key_drbc_threshold);
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
		config.prodTemplates.key_rbc_mss[0] = ConfigUtil::GetCheckBoxState(mAdditive1Checkbox);
		config.prodTemplates.key_rbc_dose[0] = ConfigUtil::GetTextfieldFloat(mDose1Textfield);
		config.prodTemplates.key_rbc_crit[0] = ConfigUtil::GetTextfieldFloat(mHematocrit1Textfield);
		config.prodTemplates.key_rbc_mss_volume[0] = ConfigUtil::GetTextfieldInt(mVolume1Textfield);

		config.prodTemplates.key_rbc_mss[1] = ConfigUtil::GetCheckBoxState(mAdditive2Checkbox);
		config.prodTemplates.key_rbc_dose[1] = ConfigUtil::GetTextfieldFloat(mDose2Textfield);
		config.prodTemplates.key_rbc_crit[1] = ConfigUtil::GetTextfieldFloat(mHematocrit2Textfield);
		config.prodTemplates.key_rbc_mss_volume[1] = ConfigUtil::GetTextfieldInt(mVolume2Textfield);

		config.prodTemplates.key_rbc_mss[2] = ConfigUtil::GetCheckBoxState(mAdditive3Checkbox);
		config.prodTemplates.key_rbc_dose[2] = ConfigUtil::GetTextfieldFloat(mDose3Textfield);
		config.prodTemplates.key_rbc_crit[2] = ConfigUtil::GetTextfieldFloat(mHematocrit3Textfield);
		config.prodTemplates.key_rbc_mss_volume[2] = ConfigUtil::GetTextfieldInt(mVolume3Textfield);

		config.predictionCfg.key_rsf = ConfigUtil::GetTextfieldFloat(mScalingFactorTextfield);
		config.procedureCfg.key_drbc_alert = ConfigUtil::GetCheckBoxState(mThresholdCheckbox);
		config.procedureCfg.key_drbc_threshold = ConfigUtil::GetTextfieldInt(mThresholdTextfield);
	}
#pragma endregion

#pragma region Members
private: System::Windows::Forms::TextBox^  mDose1Textfield;
private: System::Windows::Forms::TextBox^  mHematocrit1Textfield;
private: System::Windows::Forms::TextBox^  mVolume1Textfield;
private: System::Windows::Forms::TextBox^  mIndex1Textfield;
private: System::Windows::Forms::CheckBox^  mAdditive1Checkbox;
private: System::Windows::Forms::GroupBox^  mRBCGroupbox;
private: System::Windows::Forms::Label^  mVolumeLabel;
private: System::Windows::Forms::Label^  mHematocritLabel;
private: System::Windows::Forms::Label^  mDoseLabel;
private: System::Windows::Forms::CheckBox^  mAdditive3Checkbox;
private: System::Windows::Forms::TextBox^  mDose3Textfield;
private: System::Windows::Forms::TextBox^  mIndex3Textfield;
private: System::Windows::Forms::TextBox^  mHematocrit3Textfield;
private: System::Windows::Forms::TextBox^  mVolume3Textfield;
private: System::Windows::Forms::CheckBox^  mAdditive2Checkbox;
private: System::Windows::Forms::TextBox^  mDose2Textfield;
private: System::Windows::Forms::TextBox^  mIndex2Textfield;
private: System::Windows::Forms::TextBox^  mHematocrit2Textfield;
private: System::Windows::Forms::TextBox^  mVolume2Textfield;
private: System::Windows::Forms::Label^  mScalingFactorLabel;
private: System::Windows::Forms::TextBox^  mThresholdTextfield;
private: System::Windows::Forms::CheckBox^  mThresholdCheckbox;
private: System::Windows::Forms::TextBox^  mScalingFactorTextfield;
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
		this->mDose1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mHematocrit1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolume1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mIndex1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mAdditive1Checkbox = (gcnew System::Windows::Forms::CheckBox());
		this->mRBCGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mVolumeLabel = (gcnew System::Windows::Forms::Label());
		this->mHematocritLabel = (gcnew System::Windows::Forms::Label());
		this->mDoseLabel = (gcnew System::Windows::Forms::Label());
		this->mAdditive3Checkbox = (gcnew System::Windows::Forms::CheckBox());
		this->mDose3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mIndex3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mHematocrit3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolume3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mAdditive2Checkbox = (gcnew System::Windows::Forms::CheckBox());
		this->mDose2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mIndex2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mHematocrit2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolume2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mScalingFactorLabel = (gcnew System::Windows::Forms::Label());
		this->mScalingFactorTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mThresholdTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mThresholdCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mRBCGroupbox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mDose1Textfield
		// 
		this->mDose1Textfield->Location = System::Drawing::Point(170, 48);
		this->mDose1Textfield->Name = L"mDose1Textfield";
		this->mDose1Textfield->Size = System::Drawing::Size(100, 20);
		this->mDose1Textfield->TabIndex = 0;
		this->mDose1Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mHematocrit1Textfield
		// 
		this->mHematocrit1Textfield->Location = System::Drawing::Point(276, 48);
		this->mHematocrit1Textfield->Name = L"mHematocrit1Textfield";
		this->mHematocrit1Textfield->Size = System::Drawing::Size(100, 20);
		this->mHematocrit1Textfield->TabIndex = 1;
		this->mHematocrit1Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mVolume1Textfield
		// 
		this->mVolume1Textfield->Location = System::Drawing::Point(382, 48);
		this->mVolume1Textfield->Name = L"mVolume1Textfield";
		this->mVolume1Textfield->Size = System::Drawing::Size(100, 20);
		this->mVolume1Textfield->TabIndex = 2;
		this->mVolume1Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mIndex1Textfield
		// 
		this->mIndex1Textfield->Location = System::Drawing::Point(12, 48);
		this->mIndex1Textfield->Name = L"mIndex1Textfield";
		this->mIndex1Textfield->ReadOnly = true;
		this->mIndex1Textfield->Size = System::Drawing::Size(40, 20);
		this->mIndex1Textfield->TabIndex = 3;
		this->mIndex1Textfield->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
		// 
		// mAdditive1Checkbox
		// 
		this->mAdditive1Checkbox->AutoSize = true;
		this->mAdditive1Checkbox->Location = System::Drawing::Point(59, 50);
		this->mAdditive1Checkbox->Name = L"mAdditive1Checkbox";
		this->mAdditive1Checkbox->Size = System::Drawing::Size(105, 17);
		this->mAdditive1Checkbox->TabIndex = 4;
		this->mAdditive1Checkbox->Text = L"Additive Solution";
		this->mAdditive1Checkbox->UseVisualStyleBackColor = true;
		// 
		// mRBCGroupbox
		// 
		this->mRBCGroupbox->Controls->Add(this->mVolumeLabel);
		this->mRBCGroupbox->Controls->Add(this->mHematocritLabel);
		this->mRBCGroupbox->Controls->Add(this->mDoseLabel);
		this->mRBCGroupbox->Controls->Add(this->mAdditive3Checkbox);
		this->mRBCGroupbox->Controls->Add(this->mDose3Textfield);
		this->mRBCGroupbox->Controls->Add(this->mIndex3Textfield);
		this->mRBCGroupbox->Controls->Add(this->mHematocrit3Textfield);
		this->mRBCGroupbox->Controls->Add(this->mVolume3Textfield);
		this->mRBCGroupbox->Controls->Add(this->mAdditive2Checkbox);
		this->mRBCGroupbox->Controls->Add(this->mDose2Textfield);
		this->mRBCGroupbox->Controls->Add(this->mIndex2Textfield);
		this->mRBCGroupbox->Controls->Add(this->mHematocrit2Textfield);
		this->mRBCGroupbox->Controls->Add(this->mVolume2Textfield);
		this->mRBCGroupbox->Controls->Add(this->mAdditive1Checkbox);
		this->mRBCGroupbox->Controls->Add(this->mDose1Textfield);
		this->mRBCGroupbox->Controls->Add(this->mIndex1Textfield);
		this->mRBCGroupbox->Controls->Add(this->mHematocrit1Textfield);
		this->mRBCGroupbox->Controls->Add(this->mVolume1Textfield);
		this->mRBCGroupbox->Location = System::Drawing::Point(4, 15);
		this->mRBCGroupbox->Name = L"mRBCGroupbox";
		this->mRBCGroupbox->Size = System::Drawing::Size(572, 144);
		this->mRBCGroupbox->TabIndex = 5;
		this->mRBCGroupbox->TabStop = false;
		this->mRBCGroupbox->Text = L"Red Blood Cells";
		// 
		// mVolumeLabel
		// 
		this->mVolumeLabel->AutoSize = true;
		this->mVolumeLabel->Location = System::Drawing::Point(382, 27);
		this->mVolumeLabel->Name = L"mVolumeLabel";
		this->mVolumeLabel->Size = System::Drawing::Size(102, 13);
		this->mVolumeLabel->TabIndex = 18;
		this->mVolumeLabel->Text = L"Solution Volume (ml)";
		// 
		// mHematocritLabel
		// 
		this->mHematocritLabel->AutoSize = true;
		this->mHematocritLabel->Location = System::Drawing::Point(276, 28);
		this->mHematocritLabel->Name = L"mHematocritLabel";
		this->mHematocritLabel->Size = System::Drawing::Size(75, 13);
		this->mHematocritLabel->TabIndex = 17;
		this->mHematocritLabel->Text = L"Hematocrit (%)";
		// 
		// mDoseLabel
		// 
		this->mDoseLabel->AutoSize = true;
		this->mDoseLabel->Location = System::Drawing::Point(170, 28);
		this->mDoseLabel->Name = L"mDoseLabel";
		this->mDoseLabel->Size = System::Drawing::Size(57, 13);
		this->mDoseLabel->TabIndex = 16;
		this->mDoseLabel->Text = L"RBC Dose";
		// 
		// mAdditive3Checkbox
		// 
		this->mAdditive3Checkbox->AutoSize = true;
		this->mAdditive3Checkbox->Location = System::Drawing::Point(59, 102);
		this->mAdditive3Checkbox->Name = L"mAdditive3Checkbox";
		this->mAdditive3Checkbox->Size = System::Drawing::Size(105, 17);
		this->mAdditive3Checkbox->TabIndex = 14;
		this->mAdditive3Checkbox->Text = L"Additive Solution";
		this->mAdditive3Checkbox->UseVisualStyleBackColor = true;
		// 
		// mDose3Textfield
		// 
		this->mDose3Textfield->Location = System::Drawing::Point(170, 100);
		this->mDose3Textfield->Name = L"mDose3Textfield";
		this->mDose3Textfield->Size = System::Drawing::Size(100, 20);
		this->mDose3Textfield->TabIndex = 10;
		this->mDose3Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mIndex3Textfield
		// 
		this->mIndex3Textfield->Location = System::Drawing::Point(12, 100);
		this->mIndex3Textfield->Name = L"mIndex3Textfield";
		this->mIndex3Textfield->ReadOnly = true;
		this->mIndex3Textfield->Size = System::Drawing::Size(40, 20);
		this->mIndex3Textfield->TabIndex = 13;
		this->mIndex3Textfield->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
		// 
		// mHematocrit3Textfield
		// 
		this->mHematocrit3Textfield->Location = System::Drawing::Point(276, 100);
		this->mHematocrit3Textfield->Name = L"mHematocrit3Textfield";
		this->mHematocrit3Textfield->Size = System::Drawing::Size(100, 20);
		this->mHematocrit3Textfield->TabIndex = 11;
		this->mHematocrit3Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mVolume3Textfield
		// 
		this->mVolume3Textfield->Location = System::Drawing::Point(382, 100);
		this->mVolume3Textfield->Name = L"mVolume3Textfield";
		this->mVolume3Textfield->Size = System::Drawing::Size(100, 20);
		this->mVolume3Textfield->TabIndex = 12;
		this->mVolume3Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mAdditive2Checkbox
		// 
		this->mAdditive2Checkbox->AutoSize = true;
		this->mAdditive2Checkbox->Location = System::Drawing::Point(59, 76);
		this->mAdditive2Checkbox->Name = L"mAdditive2Checkbox";
		this->mAdditive2Checkbox->Size = System::Drawing::Size(105, 17);
		this->mAdditive2Checkbox->TabIndex = 9;
		this->mAdditive2Checkbox->Text = L"Additive Solution";
		this->mAdditive2Checkbox->UseVisualStyleBackColor = true;
		// 
		// mDose2Textfield
		// 
		this->mDose2Textfield->Location = System::Drawing::Point(170, 74);
		this->mDose2Textfield->Name = L"mDose2Textfield";
		this->mDose2Textfield->Size = System::Drawing::Size(100, 20);
		this->mDose2Textfield->TabIndex = 5;
		this->mDose2Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mIndex2Textfield
		// 
		this->mIndex2Textfield->Location = System::Drawing::Point(12, 74);
		this->mIndex2Textfield->Name = L"mIndex2Textfield";
		this->mIndex2Textfield->ReadOnly = true;
		this->mIndex2Textfield->Size = System::Drawing::Size(40, 20);
		this->mIndex2Textfield->TabIndex = 8;
		this->mIndex2Textfield->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
		// 
		// mHematocrit2Textfield
		// 
		this->mHematocrit2Textfield->Location = System::Drawing::Point(276, 74);
		this->mHematocrit2Textfield->Name = L"mHematocrit2Textfield";
		this->mHematocrit2Textfield->Size = System::Drawing::Size(100, 20);
		this->mHematocrit2Textfield->TabIndex = 6;
		this->mHematocrit2Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mVolume2Textfield
		// 
		this->mVolume2Textfield->Location = System::Drawing::Point(382, 74);
		this->mVolume2Textfield->Name = L"mVolume2Textfield";
		this->mVolume2Textfield->Size = System::Drawing::Size(100, 20);
		this->mVolume2Textfield->TabIndex = 7;
		this->mVolume2Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mScalingFactorLabel
		// 
		this->mScalingFactorLabel->AutoSize = true;
		this->mScalingFactorLabel->Location = System::Drawing::Point(60, 179);
		this->mScalingFactorLabel->Name = L"mScalingFactorLabel";
		this->mScalingFactorLabel->Size = System::Drawing::Size(103, 13);
		this->mScalingFactorLabel->TabIndex = 6;
		this->mScalingFactorLabel->Text = L"RBC Scaling Factor:";
		// 
		// mScalingFactorTextfield
		// 
		this->mScalingFactorTextfield->Location = System::Drawing::Point(197, 176);
		this->mScalingFactorTextfield->Name = L"mScalingFactorTextfield";
		this->mScalingFactorTextfield->Size = System::Drawing::Size(100, 20);
		this->mScalingFactorTextfield->TabIndex = 7;
		this->mScalingFactorTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mThresholdTextfield
		// 
		this->mThresholdTextfield->Location = System::Drawing::Point(197, 203);
		this->mThresholdTextfield->Name = L"mThresholdTextfield";
		this->mThresholdTextfield->Size = System::Drawing::Size(100, 20);
		this->mThresholdTextfield->TabIndex = 9;
		this->mThresholdTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mThresholdCheckbox
		// 
		this->mThresholdCheckbox->AutoSize = true;
		this->mThresholdCheckbox->Location = System::Drawing::Point(63, 205);
		this->mThresholdCheckbox->Name = L"mThresholdCheckbox";
		this->mThresholdCheckbox->Size = System::Drawing::Size(128, 17);
		this->mThresholdCheckbox->TabIndex = 10;
		this->mThresholdCheckbox->Text = L"DRBC Threshold (ml):";
		this->mThresholdCheckbox->UseVisualStyleBackColor = true;
		// 
		// ConfigRedBloodCells
		// 
		this->Controls->Add(this->mThresholdCheckbox);
		this->Controls->Add(this->mThresholdTextfield);
		this->Controls->Add(this->mScalingFactorTextfield);
		this->Controls->Add(this->mScalingFactorLabel);
		this->Controls->Add(this->mRBCGroupbox);
		this->Name = L"ConfigRedBloodCells";
		this->Size = System::Drawing::Size(599, 300);
		this->mRBCGroupbox->ResumeLayout(false);
		this->mRBCGroupbox->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};
}
