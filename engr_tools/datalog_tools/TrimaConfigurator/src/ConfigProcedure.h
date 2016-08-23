/*****************************************************************************\
**
** ConfigProcedure.h
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

public ref class ConfigProcedure : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigProcedure(void)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigProcedure(void)
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

		ConfigUtil::ValidateTextfieldInt(mMaximumProcedureTimeTextfield, KEY_PROC_TIME, info);
		ConfigUtil::ValidateTextfieldFloat(mReturnPressureHighLimitTextfield, KEY_RETURN_PRESS, info);
		ConfigUtil::ValidateTextfieldFloat(mDrawPressureLowLimitTextfield, KEY_DRAW_PRESS, info);
		ConfigUtil::ValidateTextfieldFloat(mInletACRatioTextfield, KEY_CUSTOM_RATIO, info);

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
		ConfigUtil::SetTextfieldInt(mMaximumProcedureTimeTextfield, config.procedureCfg.key_proc_time);
		ConfigUtil::SetTextfieldFloat(mReturnPressureHighLimitTextfield, config.procedureCfg.key_return_press);
		ConfigUtil::SetTextfieldFloat(mDrawPressureLowLimitTextfield, config.procedureCfg.key_draw_press);
		ConfigUtil::SetCheckBoxState(mBloodDiversionCheckbox, config.procedureCfg.key_blood_diversion);
		ConfigUtil::SetCheckBoxState(mPlasmaRinseback, config.procedureCfg.key_plasma_rinseback);
		ConfigUtil::SetTextfieldFloat(mInletACRatioTextfield, config.procedureCfg.key_custom_ratio);
		ConfigUtil::SetListSelection(mInletACRatioRBCList, config.predictionCfg.key_rbc_ratio);
		ConfigUtil::SetCheckBoxState(mOperatorActionValidationCheckbox, config.procedureCfg.key_audit_tracking);
		ConfigUtil::SetCheckBoxState(mProductBagAirRemovalCheckbox, config.procedureCfg.key_air_removal);
		ConfigUtil::SetCheckBoxState(mSalineRinsebackCheckbox, config.procedureCfg.key_saline_rinseback);
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
		config.procedureCfg.key_proc_time = ConfigUtil::GetTextfieldInt(mMaximumProcedureTimeTextfield);
		config.procedureCfg.key_return_press = ConfigUtil::GetTextfieldFloat(mReturnPressureHighLimitTextfield);
		config.procedureCfg.key_draw_press = ConfigUtil::GetTextfieldFloat(mDrawPressureLowLimitTextfield);
		config.procedureCfg.key_blood_diversion = ConfigUtil::GetCheckBoxState(mBloodDiversionCheckbox);
		config.procedureCfg.key_plasma_rinseback = ConfigUtil::GetCheckBoxState(mPlasmaRinseback);
		config.procedureCfg.key_custom_ratio = ConfigUtil::GetTextfieldFloat(mInletACRatioTextfield);
		config.predictionCfg.key_rbc_ratio = ConfigUtil::GetListSelection(mInletACRatioRBCList);
		config.procedureCfg.key_audit_tracking = ConfigUtil::GetCheckBoxState(mOperatorActionValidationCheckbox);
		config.procedureCfg.key_air_removal = ConfigUtil::GetCheckBoxState(mProductBagAirRemovalCheckbox);
		config.procedureCfg.key_saline_rinseback = ConfigUtil::GetCheckBoxState(mSalineRinsebackCheckbox);
	}
#pragma endregion

#pragma region Members
private: System::Windows::Forms::Label^  mMaximumProcedureTimeLabel;
private: System::Windows::Forms::TextBox^  mMaximumProcedureTimeTextfield;
private: System::Windows::Forms::Label^  mReturnPressureHighLimitLabel;
private: System::Windows::Forms::TextBox^  mReturnPressureHighLimitTextfield;
private: System::Windows::Forms::Label^  mDrawPressureLowLimitLabel;
private: System::Windows::Forms::TextBox^  mDrawPressureLowLimitTextfield;
private: System::Windows::Forms::TextBox^  mInletACRatioTextfield;
private: System::Windows::Forms::CheckBox^  mBloodDiversionCheckbox;
private: System::Windows::Forms::CheckBox^  mPlasmaRinseback;
private: System::Windows::Forms::Label^  mInletACRatioLabel;
private: System::Windows::Forms::Label^  mInletACRatioRBCLabel;
private: System::Windows::Forms::CheckBox^  mOperatorActionValidationCheckbox;
private: System::Windows::Forms::CheckBox^  mProductBagAirRemovalCheckbox;
private: System::Windows::Forms::CheckBox^  mSalineRinsebackCheckbox;
private: System::Windows::Forms::GroupBox^  mInletRationGroupbox;
private: System::Windows::Forms::GroupBox^  mPressureLimitsGroupbox;
private: System::Windows::Forms::GroupBox^  mProcedureOptionsGroupbox;
private: System::Windows::Forms::ComboBox^  mInletACRatioRBCList;
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
		this->mMaximumProcedureTimeLabel = (gcnew System::Windows::Forms::Label());
		this->mReturnPressureHighLimitLabel = (gcnew System::Windows::Forms::Label());
		this->mMaximumProcedureTimeTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mReturnPressureHighLimitTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mDrawPressureLowLimitLabel = (gcnew System::Windows::Forms::Label());
		this->mDrawPressureLowLimitTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mInletACRatioTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mBloodDiversionCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mPlasmaRinseback = (gcnew System::Windows::Forms::CheckBox());
		this->mInletACRatioLabel = (gcnew System::Windows::Forms::Label());
		this->mInletACRatioRBCLabel = (gcnew System::Windows::Forms::Label());
		this->mOperatorActionValidationCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mProductBagAirRemovalCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mSalineRinsebackCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mInletRationGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mInletACRatioRBCList = (gcnew System::Windows::Forms::ComboBox());
		this->mPressureLimitsGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mProcedureOptionsGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mInletRationGroupbox->SuspendLayout();
		this->mPressureLimitsGroupbox->SuspendLayout();
		this->mProcedureOptionsGroupbox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mMaximumProcedureTimeLabel
		// 
		this->mMaximumProcedureTimeLabel->AutoSize = true;
		this->mMaximumProcedureTimeLabel->Location = System::Drawing::Point(12, 23);
		this->mMaximumProcedureTimeLabel->Name = L"mMaximumProcedureTimeLabel";
		this->mMaximumProcedureTimeLabel->Size = System::Drawing::Size(172, 13);
		this->mMaximumProcedureTimeLabel->TabIndex = 0;
		this->mMaximumProcedureTimeLabel->Text = L"Maximum Procedure Time (minute):";
		// 
		// mReturnPressureHighLimitLabel
		// 
		this->mReturnPressureHighLimitLabel->AutoSize = true;
		this->mReturnPressureHighLimitLabel->Location = System::Drawing::Point(6, 16);
		this->mReturnPressureHighLimitLabel->Name = L"mReturnPressureHighLimitLabel";
		this->mReturnPressureHighLimitLabel->Size = System::Drawing::Size(174, 13);
		this->mReturnPressureHighLimitLabel->TabIndex = 1;
		this->mReturnPressureHighLimitLabel->Text = L"Return Pressure High Limit (mmHg):";
		// 
		// mMaximumProcedureTimeTextfield
		// 
		this->mMaximumProcedureTimeTextfield->Location = System::Drawing::Point(207, 20);
		this->mMaximumProcedureTimeTextfield->Name = L"mMaximumProcedureTimeTextfield";
		this->mMaximumProcedureTimeTextfield->Size = System::Drawing::Size(100, 20);
		this->mMaximumProcedureTimeTextfield->TabIndex = 2;
		this->mMaximumProcedureTimeTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mReturnPressureHighLimitTextfield
		// 
		this->mReturnPressureHighLimitTextfield->Location = System::Drawing::Point(201, 13);
		this->mReturnPressureHighLimitTextfield->Name = L"mReturnPressureHighLimitTextfield";
		this->mReturnPressureHighLimitTextfield->Size = System::Drawing::Size(100, 20);
		this->mReturnPressureHighLimitTextfield->TabIndex = 3;
		this->mReturnPressureHighLimitTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mDrawPressureLowLimitLabel
		// 
		this->mDrawPressureLowLimitLabel->AutoSize = true;
		this->mDrawPressureLowLimitLabel->Location = System::Drawing::Point(6, 43);
		this->mDrawPressureLowLimitLabel->Name = L"mDrawPressureLowLimitLabel";
		this->mDrawPressureLowLimitLabel->Size = System::Drawing::Size(165, 13);
		this->mDrawPressureLowLimitLabel->TabIndex = 4;
		this->mDrawPressureLowLimitLabel->Text = L"Draw Pressure Low Limit (mmHg):";
		// 
		// mDrawPressureLowLimitTextfield
		// 
		this->mDrawPressureLowLimitTextfield->Location = System::Drawing::Point(201, 40);
		this->mDrawPressureLowLimitTextfield->Name = L"mDrawPressureLowLimitTextfield";
		this->mDrawPressureLowLimitTextfield->Size = System::Drawing::Size(100, 20);
		this->mDrawPressureLowLimitTextfield->TabIndex = 5;
		this->mDrawPressureLowLimitTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mInletACRatioTextfield
		// 
		this->mInletACRatioTextfield->Location = System::Drawing::Point(201, 13);
		this->mInletACRatioTextfield->Name = L"mInletACRatioTextfield";
		this->mInletACRatioTextfield->Size = System::Drawing::Size(100, 20);
		this->mInletACRatioTextfield->TabIndex = 6;
		this->mInletACRatioTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mBloodDiversionCheckbox
		// 
		this->mBloodDiversionCheckbox->AutoSize = true;
		this->mBloodDiversionCheckbox->Location = System::Drawing::Point(16, 19);
		this->mBloodDiversionCheckbox->Name = L"mBloodDiversionCheckbox";
		this->mBloodDiversionCheckbox->Size = System::Drawing::Size(100, 17);
		this->mBloodDiversionCheckbox->TabIndex = 7;
		this->mBloodDiversionCheckbox->Text = L"Blood Diversion";
		this->mBloodDiversionCheckbox->UseVisualStyleBackColor = true;
		// 
		// mPlasmaRinseback
		// 
		this->mPlasmaRinseback->AutoSize = true;
		this->mPlasmaRinseback->Location = System::Drawing::Point(16, 43);
		this->mPlasmaRinseback->Name = L"mPlasmaRinseback";
		this->mPlasmaRinseback->Size = System::Drawing::Size(114, 17);
		this->mPlasmaRinseback->TabIndex = 8;
		this->mPlasmaRinseback->Text = L"Plasma Rinseback";
		this->mPlasmaRinseback->UseVisualStyleBackColor = true;
		// 
		// mInletACRatioLabel
		// 
		this->mInletACRatioLabel->AutoSize = true;
		this->mInletACRatioLabel->Location = System::Drawing::Point(6, 16);
		this->mInletACRatioLabel->Name = L"mInletACRatioLabel";
		this->mInletACRatioLabel->Size = System::Drawing::Size(77, 13);
		this->mInletACRatioLabel->TabIndex = 9;
		this->mInletACRatioLabel->Text = L"Inlet/AC Ratio:";
		// 
		// mInletACRatioRBCLabel
		// 
		this->mInletACRatioRBCLabel->AutoSize = true;
		this->mInletACRatioRBCLabel->Location = System::Drawing::Point(6, 42);
		this->mInletACRatioRBCLabel->Name = L"mInletACRatioRBCLabel";
		this->mInletACRatioRBCLabel->Size = System::Drawing::Size(117, 13);
		this->mInletACRatioRBCLabel->TabIndex = 11;
		this->mInletACRatioRBCLabel->Text = L"Inlet/AC Ratio for RBC:";
		// 
		// mOperatorActionValidationCheckbox
		// 
		this->mOperatorActionValidationCheckbox->AutoSize = true;
		this->mOperatorActionValidationCheckbox->Location = System::Drawing::Point(16, 114);
		this->mOperatorActionValidationCheckbox->Name = L"mOperatorActionValidationCheckbox";
		this->mOperatorActionValidationCheckbox->Size = System::Drawing::Size(149, 17);
		this->mOperatorActionValidationCheckbox->TabIndex = 12;
		this->mOperatorActionValidationCheckbox->Text = L"Operator Action Validation";
		this->mOperatorActionValidationCheckbox->UseVisualStyleBackColor = true;
		// 
		// mProductBagAirRemovalCheckbox
		// 
		this->mProductBagAirRemovalCheckbox->AutoSize = true;
		this->mProductBagAirRemovalCheckbox->Location = System::Drawing::Point(16, 67);
		this->mProductBagAirRemovalCheckbox->Name = L"mProductBagAirRemovalCheckbox";
		this->mProductBagAirRemovalCheckbox->Size = System::Drawing::Size(145, 17);
		this->mProductBagAirRemovalCheckbox->TabIndex = 13;
		this->mProductBagAirRemovalCheckbox->Text = L"Product Bag Air Removal";
		this->mProductBagAirRemovalCheckbox->UseVisualStyleBackColor = true;
		// 
		// mSalineRinsebackCheckbox
		// 
		this->mSalineRinsebackCheckbox->AutoSize = true;
		this->mSalineRinsebackCheckbox->Location = System::Drawing::Point(16, 91);
		this->mSalineRinsebackCheckbox->Name = L"mSalineRinsebackCheckbox";
		this->mSalineRinsebackCheckbox->Size = System::Drawing::Size(109, 17);
		this->mSalineRinsebackCheckbox->TabIndex = 14;
		this->mSalineRinsebackCheckbox->Text = L"Saline Rinseback";
		this->mSalineRinsebackCheckbox->UseVisualStyleBackColor = true;
		// 
		// mInletRationGroupbox
		// 
		this->mInletRationGroupbox->Controls->Add(this->mInletACRatioRBCList);
		this->mInletRationGroupbox->Controls->Add(this->mInletACRatioLabel);
		this->mInletRationGroupbox->Controls->Add(this->mInletACRatioTextfield);
		this->mInletRationGroupbox->Controls->Add(this->mInletACRatioRBCLabel);
		this->mInletRationGroupbox->Location = System::Drawing::Point(6, 126);
		this->mInletRationGroupbox->Name = L"mInletRationGroupbox";
		this->mInletRationGroupbox->Size = System::Drawing::Size(324, 78);
		this->mInletRationGroupbox->TabIndex = 15;
		this->mInletRationGroupbox->TabStop = false;
		this->mInletRationGroupbox->Text = L"Inlet Ratio";
		// 
		// mInletACRatioRBCList
		// 
		this->mInletACRatioRBCList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mInletACRatioRBCList->FormattingEnabled = true;
		this->mInletACRatioRBCList->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"8.14", L"11.00"});
		this->mInletACRatioRBCList->Location = System::Drawing::Point(201, 39);
		this->mInletACRatioRBCList->Name = L"mInletACRatioRBCList";
		this->mInletACRatioRBCList->Size = System::Drawing::Size(99, 21);
		this->mInletACRatioRBCList->TabIndex = 12;
		// 
		// mPressureLimitsGroupbox
		// 
		this->mPressureLimitsGroupbox->Controls->Add(this->mReturnPressureHighLimitLabel);
		this->mPressureLimitsGroupbox->Controls->Add(this->mReturnPressureHighLimitTextfield);
		this->mPressureLimitsGroupbox->Controls->Add(this->mDrawPressureLowLimitLabel);
		this->mPressureLimitsGroupbox->Controls->Add(this->mDrawPressureLowLimitTextfield);
		this->mPressureLimitsGroupbox->Location = System::Drawing::Point(6, 46);
		this->mPressureLimitsGroupbox->Name = L"mPressureLimitsGroupbox";
		this->mPressureLimitsGroupbox->Size = System::Drawing::Size(324, 74);
		this->mPressureLimitsGroupbox->TabIndex = 16;
		this->mPressureLimitsGroupbox->TabStop = false;
		this->mPressureLimitsGroupbox->Text = L"Pressure Limits";
		// 
		// mProcedureOptionsGroupbox
		// 
		this->mProcedureOptionsGroupbox->Controls->Add(this->mBloodDiversionCheckbox);
		this->mProcedureOptionsGroupbox->Controls->Add(this->mPlasmaRinseback);
		this->mProcedureOptionsGroupbox->Controls->Add(this->mOperatorActionValidationCheckbox);
		this->mProcedureOptionsGroupbox->Controls->Add(this->mSalineRinsebackCheckbox);
		this->mProcedureOptionsGroupbox->Controls->Add(this->mProductBagAirRemovalCheckbox);
		this->mProcedureOptionsGroupbox->Location = System::Drawing::Point(337, 46);
		this->mProcedureOptionsGroupbox->Name = L"mProcedureOptionsGroupbox";
		this->mProcedureOptionsGroupbox->Size = System::Drawing::Size(228, 158);
		this->mProcedureOptionsGroupbox->TabIndex = 17;
		this->mProcedureOptionsGroupbox->TabStop = false;
		this->mProcedureOptionsGroupbox->Text = L"Options";
		// 
		// ConfigProcedure
		// 
		this->Controls->Add(this->mProcedureOptionsGroupbox);
		this->Controls->Add(this->mPressureLimitsGroupbox);
		this->Controls->Add(this->mInletRationGroupbox);
		this->Controls->Add(this->mMaximumProcedureTimeTextfield);
		this->Controls->Add(this->mMaximumProcedureTimeLabel);
		this->Name = L"ConfigProcedure";
		this->Size = System::Drawing::Size(599, 300);
		this->mInletRationGroupbox->ResumeLayout(false);
		this->mInletRationGroupbox->PerformLayout();
		this->mPressureLimitsGroupbox->ResumeLayout(false);
		this->mPressureLimitsGroupbox->PerformLayout();
		this->mProcedureOptionsGroupbox->ResumeLayout(false);
		this->mProcedureOptionsGroupbox->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};
}
