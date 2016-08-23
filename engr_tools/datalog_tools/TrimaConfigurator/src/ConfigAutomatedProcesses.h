/*****************************************************************************\
**
** ConfigAutomatedProcesses.h
**
** Automated processes config page.
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

public ref class ConfigAutomatedProcesses : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigAutomatedProcesses(void)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigAutomatedProcesses(void)
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

		ConfigUtil::ValidateTextfieldInt(mVolPlateletBagTextfield, KEY_PLT_DEF_BAG_VOL, info);
		ConfigUtil::ValidateTextfieldInt(mVolRBCBagTextfield, KEY_RBC_DEF_BAG_VOL, info);

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
		ConfigUtil::SetCheckBoxState(mSolutionAdditionCheckbox, config.procedureCfg.key_mss_plt_on);
		ConfigUtil::SetTextfieldInt(mVolPlateletBagTextfield, config.procedureCfg.key_plt_def_bag_vol);
		ConfigUtil::SetCheckBoxState(mAutoPASBagVolOverrideCheckbox, config.procedureCfg.key_override_pas_bag_vol);
		ConfigUtil::SetCheckBoxState(mPasSplitToggleCheckbox, config.procedureCfg.key_plt_mss_split_notif);
		ConfigUtil::SetCheckBoxState(mLeukoreductionSolutionAddCheckbox, config.procedureCfg.key_mss_rbc_on);
		ConfigUtil::SetTextfieldInt(mVolRBCBagTextfield, config.procedureCfg.key_rbc_def_bag_vol);
		ConfigUtil::SetCheckBoxState(mShowLeukoreductionMessagesCheckbox, config.procedureCfg.key_show_pre_aas_flags);
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
		config.procedureCfg.key_mss_plt_on = ConfigUtil::GetCheckBoxState(mSolutionAdditionCheckbox);
		config.procedureCfg.key_plt_def_bag_vol = ConfigUtil::GetTextfieldInt(mVolPlateletBagTextfield);
		config.procedureCfg.key_override_pas_bag_vol = ConfigUtil::GetCheckBoxState(mAutoPASBagVolOverrideCheckbox);
		config.procedureCfg.key_plt_mss_split_notif = ConfigUtil::GetCheckBoxState(mPasSplitToggleCheckbox);
		config.procedureCfg.key_mss_rbc_on = ConfigUtil::GetCheckBoxState(mLeukoreductionSolutionAddCheckbox);
		config.procedureCfg.key_rbc_def_bag_vol = ConfigUtil::GetTextfieldInt(mVolRBCBagTextfield);
		config.procedureCfg.key_show_pre_aas_flags = ConfigUtil::GetCheckBoxState(mShowLeukoreductionMessagesCheckbox);
	}
#pragma endregion

#pragma region Members
private: System::Windows::Forms::GroupBox^  mPlateletProcessesGroupBox;
private: System::Windows::Forms::CheckBox^  mSolutionAdditionCheckbox;
private: System::Windows::Forms::TextBox^  mVolPlateletBagTextfield;
private: System::Windows::Forms::Label^  mVolPlateletBagLabel;
private: System::Windows::Forms::CheckBox^  mAutoPASBagVolOverrideCheckbox;
private: System::Windows::Forms::CheckBox^  mPasSplitToggleCheckbox;
private: System::Windows::Forms::GroupBox^  mRBCProcessesGroupBox;
private: System::Windows::Forms::TextBox^  mVolRBCBagTextfield;
private: System::Windows::Forms::Label^  mVolRBCBagLabel;
private: System::Windows::Forms::CheckBox^  mLeukoreductionSolutionAddCheckbox;
private: System::Windows::Forms::CheckBox^  mShowLeukoreductionMessagesCheckbox;
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
		this->mPlateletProcessesGroupBox = (gcnew System::Windows::Forms::GroupBox());
		this->mPasSplitToggleCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mAutoPASBagVolOverrideCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mVolPlateletBagTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolPlateletBagLabel = (gcnew System::Windows::Forms::Label());
		this->mSolutionAdditionCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mRBCProcessesGroupBox = (gcnew System::Windows::Forms::GroupBox());
		this->mVolRBCBagTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolRBCBagLabel = (gcnew System::Windows::Forms::Label());
		this->mLeukoreductionSolutionAddCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mShowLeukoreductionMessagesCheckbox = (gcnew System::Windows::Forms::CheckBox());
		this->mPlateletProcessesGroupBox->SuspendLayout();
		this->mRBCProcessesGroupBox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mPlateletProcessesGroupBox
		// 
		this->mPlateletProcessesGroupBox->Controls->Add(this->mPasSplitToggleCheckbox);
		this->mPlateletProcessesGroupBox->Controls->Add(this->mAutoPASBagVolOverrideCheckbox);
		this->mPlateletProcessesGroupBox->Controls->Add(this->mVolPlateletBagTextfield);
		this->mPlateletProcessesGroupBox->Controls->Add(this->mVolPlateletBagLabel);
		this->mPlateletProcessesGroupBox->Controls->Add(this->mSolutionAdditionCheckbox);
		this->mPlateletProcessesGroupBox->Location = System::Drawing::Point(3, 12);
		this->mPlateletProcessesGroupBox->Name = L"mPlateletProcessesGroupBox";
		this->mPlateletProcessesGroupBox->Size = System::Drawing::Size(575, 124);
		this->mPlateletProcessesGroupBox->TabIndex = 0;
		this->mPlateletProcessesGroupBox->TabStop = false;
		this->mPlateletProcessesGroupBox->Text = L"Platelet Processes";
		// 
		// mPasSplitToggleCheckbox
		// 
		this->mPasSplitToggleCheckbox->AutoSize = true;
		this->mPasSplitToggleCheckbox->Location = System::Drawing::Point(7, 96);
		this->mPasSplitToggleCheckbox->Name = L"mPasSplitToggleCheckbox";
		this->mPasSplitToggleCheckbox->Size = System::Drawing::Size(244, 17);
		this->mPasSplitToggleCheckbox->TabIndex = 4;
		this->mPasSplitToggleCheckbox->Text = L"Platelete / Additive Solution Separate Addition";
		this->mPasSplitToggleCheckbox->UseVisualStyleBackColor = true;
		// 
		// mAutoPASBagVolOverrideCheckbox
		// 
		this->mAutoPASBagVolOverrideCheckbox->AutoSize = true;
		this->mAutoPASBagVolOverrideCheckbox->Location = System::Drawing::Point(7, 72);
		this->mAutoPASBagVolOverrideCheckbox->Name = L"mAutoPASBagVolOverrideCheckbox";
		this->mAutoPASBagVolOverrideCheckbox->Size = System::Drawing::Size(184, 17);
		this->mAutoPASBagVolOverrideCheckbox->TabIndex = 3;
		this->mAutoPASBagVolOverrideCheckbox->Text = L"Auto P.A.S. Bag Volume Override";
		this->mAutoPASBagVolOverrideCheckbox->UseVisualStyleBackColor = true;
		// 
		// mVolPlateletBagTextfield
		// 
		this->mVolPlateletBagTextfield->Location = System::Drawing::Point(231, 41);
		this->mVolPlateletBagTextfield->Name = L"mVolPlateletBagTextfield";
		this->mVolPlateletBagTextfield->Size = System::Drawing::Size(100, 20);
		this->mVolPlateletBagTextfield->TabIndex = 2;
		this->mVolPlateletBagTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mVolPlateletBagLabel
		// 
		this->mVolPlateletBagLabel->AutoSize = true;
		this->mVolPlateletBagLabel->Location = System::Drawing::Point(7, 44);
		this->mVolPlateletBagLabel->Name = L"mVolPlateletBagLabel";
		this->mVolPlateletBagLabel->Size = System::Drawing::Size(218, 13);
		this->mVolPlateletBagLabel->TabIndex = 1;
		this->mVolPlateletBagLabel->Text = L"Volume (ml) of Platelet Additive Solution Bag:";
		// 
		// mSolutionAdditionCheckbox
		// 
		this->mSolutionAdditionCheckbox->AutoSize = true;
		this->mSolutionAdditionCheckbox->Location = System::Drawing::Point(7, 20);
		this->mSolutionAdditionCheckbox->Name = L"mSolutionAdditionCheckbox";
		this->mSolutionAdditionCheckbox->Size = System::Drawing::Size(105, 17);
		this->mSolutionAdditionCheckbox->TabIndex = 0;
		this->mSolutionAdditionCheckbox->Text = L"Solution Addition";
		this->mSolutionAdditionCheckbox->UseVisualStyleBackColor = true;
		// 
		// mRBCProcessesGroupBox
		// 
		this->mRBCProcessesGroupBox->Controls->Add(this->mVolRBCBagTextfield);
		this->mRBCProcessesGroupBox->Controls->Add(this->mVolRBCBagLabel);
		this->mRBCProcessesGroupBox->Controls->Add(this->mLeukoreductionSolutionAddCheckbox);
		this->mRBCProcessesGroupBox->Location = System::Drawing::Point(3, 143);
		this->mRBCProcessesGroupBox->Name = L"mRBCProcessesGroupBox";
		this->mRBCProcessesGroupBox->Size = System::Drawing::Size(575, 77);
		this->mRBCProcessesGroupBox->TabIndex = 1;
		this->mRBCProcessesGroupBox->TabStop = false;
		this->mRBCProcessesGroupBox->Text = L"RBC Processes";
		// 
		// mVolRBCBagTextfield
		// 
		this->mVolRBCBagTextfield->Location = System::Drawing::Point(230, 40);
		this->mVolRBCBagTextfield->Name = L"mVolRBCBagTextfield";
		this->mVolRBCBagTextfield->Size = System::Drawing::Size(100, 20);
		this->mVolRBCBagTextfield->TabIndex = 5;
		this->mVolRBCBagTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mVolRBCBagLabel
		// 
		this->mVolRBCBagLabel->AutoSize = true;
		this->mVolRBCBagLabel->Location = System::Drawing::Point(6, 43);
		this->mVolRBCBagLabel->Name = L"mVolRBCBagLabel";
		this->mVolRBCBagLabel->Size = System::Drawing::Size(205, 13);
		this->mVolRBCBagLabel->TabIndex = 4;
		this->mVolRBCBagLabel->Text = L"Volume (ml) of RBC Additive Solution Bag:";
		// 
		// mLeukoreductionSolutionAddCheckbox
		// 
		this->mLeukoreductionSolutionAddCheckbox->AutoSize = true;
		this->mLeukoreductionSolutionAddCheckbox->Location = System::Drawing::Point(6, 19);
		this->mLeukoreductionSolutionAddCheckbox->Name = L"mLeukoreductionSolutionAddCheckbox";
		this->mLeukoreductionSolutionAddCheckbox->Size = System::Drawing::Size(203, 17);
		this->mLeukoreductionSolutionAddCheckbox->TabIndex = 3;
		this->mLeukoreductionSolutionAddCheckbox->Text = L"Leukoreduction and Solution Addition";
		this->mLeukoreductionSolutionAddCheckbox->UseVisualStyleBackColor = true;
		// 
		// mShowLeukoreductionMessagesCheckbox
		// 
		this->mShowLeukoreductionMessagesCheckbox->AutoSize = true;
		this->mShowLeukoreductionMessagesCheckbox->Location = System::Drawing::Point(10, 227);
		this->mShowLeukoreductionMessagesCheckbox->Name = L"mShowLeukoreductionMessagesCheckbox";
		this->mShowLeukoreductionMessagesCheckbox->Size = System::Drawing::Size(230, 17);
		this->mShowLeukoreductionMessagesCheckbox->TabIndex = 2;
		this->mShowLeukoreductionMessagesCheckbox->Text = L"Show Additional Leukoreduction Messages";
		this->mShowLeukoreductionMessagesCheckbox->UseVisualStyleBackColor = true;
		// 
		// ConfigAutomatedProcesses
		// 
		this->Controls->Add(this->mShowLeukoreductionMessagesCheckbox);
		this->Controls->Add(this->mRBCProcessesGroupBox);
		this->Controls->Add(this->mPlateletProcessesGroupBox);
		this->Name = L"ConfigAutomatedProcesses";
		this->Size = System::Drawing::Size(599, 300);
		this->mPlateletProcessesGroupBox->ResumeLayout(false);
		this->mPlateletProcessesGroupBox->PerformLayout();
		this->mRBCProcessesGroupBox->ResumeLayout(false);
		this->mRBCProcessesGroupBox->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};
}
