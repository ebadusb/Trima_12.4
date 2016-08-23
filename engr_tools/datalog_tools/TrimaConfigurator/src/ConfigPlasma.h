/*****************************************************************************\
**
** ConfigPlasma.h
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

public ref class ConfigPlasma : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigPlasma(void)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigPlasma(void)
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

		ConfigUtil::ValidateTextfieldFloat(mPlasmaVolume1Textfield, KEY_PLS_VOLUME_1, info);
		ConfigUtil::ValidateTextfieldFloat(mPlasmaVolume2Textfield, KEY_PLS_VOLUME_2, info);
		ConfigUtil::ValidateTextfieldFloat(mPlasmaVolume3Textfield, KEY_PLS_VOLUME_3, info);
		ConfigUtil::ValidateTextfieldFloat(mPlasmaVolume4Textfield, KEY_PLS_VOLUME_4, info);
		ConfigUtil::ValidateTextfieldFloat(mPlasmaVolume5Textfield, KEY_PLS_VOLUME_5, info);
		ConfigUtil::ValidateTextfieldFloat(mPlasmaVolume6Textfield, KEY_PLS_VOLUME_6, info);
		ConfigUtil::ValidateTextfieldInt(mMaxPossibleMinTextfield, KEY_PLASMA_AMAP_MINIMUM, info);
		ConfigUtil::ValidateTextfieldInt(mMaxPossibleMaxTextfield, KEY_PLASMA_AMAP_MAXIMUM, info);

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
		ConfigUtil::SetTextfieldFloat(mPlasmaVolume1Textfield, config.prodTemplates.key_pls_volume[0]);
		ConfigUtil::SetTextfieldFloat(mPlasmaVolume2Textfield, config.prodTemplates.key_pls_volume[1]);
		ConfigUtil::SetTextfieldFloat(mPlasmaVolume3Textfield, config.prodTemplates.key_pls_volume[2]);
		ConfigUtil::SetTextfieldFloat(mPlasmaVolume4Textfield, config.prodTemplates.key_pls_volume[3]);
		ConfigUtil::SetTextfieldFloat(mPlasmaVolume5Textfield, config.prodTemplates.key_pls_volume[4]);
		ConfigUtil::SetTextfieldFloat(mPlasmaVolume6Textfield, config.prodTemplates.key_pls_volume[5]);
		ConfigUtil::SetTextfieldInt(mMaxPossibleMinTextfield, config.predictionCfg.key_pls_amap_min);
		ConfigUtil::SetTextfieldInt(mMaxPossibleMaxTextfield, config.predictionCfg.key_pls_amap_max);
		ConfigUtil::SetListSelection(mDonorsAllowedList, config.predictionCfg.key_male_only_plasma);
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
		config.prodTemplates.key_pls_volume[0] = ConfigUtil::GetTextfieldFloat(mPlasmaVolume1Textfield);
		config.prodTemplates.key_pls_volume[1] = ConfigUtil::GetTextfieldFloat(mPlasmaVolume2Textfield);
		config.prodTemplates.key_pls_volume[2] = ConfigUtil::GetTextfieldFloat(mPlasmaVolume3Textfield);
		config.prodTemplates.key_pls_volume[3] = ConfigUtil::GetTextfieldFloat(mPlasmaVolume4Textfield);
		config.prodTemplates.key_pls_volume[4] = ConfigUtil::GetTextfieldFloat(mPlasmaVolume5Textfield);
		config.prodTemplates.key_pls_volume[5] = ConfigUtil::GetTextfieldFloat(mPlasmaVolume6Textfield);
		config.predictionCfg.key_pls_amap_min = ConfigUtil::GetTextfieldInt(mMaxPossibleMinTextfield);
		config.predictionCfg.key_pls_amap_max = ConfigUtil::GetTextfieldInt(mMaxPossibleMaxTextfield);
		config.predictionCfg.key_male_only_plasma = ConfigUtil::GetListSelection(mDonorsAllowedList);
	}
#pragma endregion

#pragma region Members 
private: System::Windows::Forms::GroupBox^  mPlasmaVolumesGroupbox;
private: System::Windows::Forms::TextBox^  mPlasmaVolume6Textfield;
private: System::Windows::Forms::TextBox^  mPlasmaVolume5Textfield;
private: System::Windows::Forms::TextBox^  mPlasmaVolume4Textfield;
private: System::Windows::Forms::TextBox^  mPlasmaVolume3Textfield;
private: System::Windows::Forms::TextBox^  mPlasmaVolume2Textfield;
private: System::Windows::Forms::TextBox^  mPlasmaVolume1Textfield;
private: System::Windows::Forms::Label^  mPlasmaVolume6Label;
private: System::Windows::Forms::Label^  mPlasmaVolume5Label;
private: System::Windows::Forms::Label^  mPlasmaVolume4Label;
private: System::Windows::Forms::Label^  mPlasmaVolume3Label;
private: System::Windows::Forms::Label^  mPlasmaVolume2Label;
private: System::Windows::Forms::Label^  mPlasmaVolume1Label;
private: System::Windows::Forms::GroupBox^  mPlasmaLimitsGroupbox;
private: System::Windows::Forms::TextBox^  mMaxPossibleMaxTextfield;
private: System::Windows::Forms::Label^  mMaxPossibleMaxLabel;
private: System::Windows::Forms::TextBox^  mMaxPossibleMinTextfield;
private: System::Windows::Forms::Label^  mMaxPossibleMinLabel;
private: System::Windows::Forms::Label^  mDonorsAllowedLabel;
private: System::Windows::Forms::ComboBox^  mDonorsAllowedList;
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
		this->mPlasmaVolumesGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mPlasmaVolume6Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mPlasmaVolume5Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mPlasmaVolume4Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mPlasmaVolume3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mPlasmaVolume2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mPlasmaVolume1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mPlasmaVolume6Label = (gcnew System::Windows::Forms::Label());
		this->mPlasmaVolume5Label = (gcnew System::Windows::Forms::Label());
		this->mPlasmaVolume4Label = (gcnew System::Windows::Forms::Label());
		this->mPlasmaVolume3Label = (gcnew System::Windows::Forms::Label());
		this->mPlasmaVolume2Label = (gcnew System::Windows::Forms::Label());
		this->mPlasmaVolume1Label = (gcnew System::Windows::Forms::Label());
		this->mPlasmaLimitsGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mMaxPossibleMaxTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mMaxPossibleMaxLabel = (gcnew System::Windows::Forms::Label());
		this->mMaxPossibleMinTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mMaxPossibleMinLabel = (gcnew System::Windows::Forms::Label());
		this->mDonorsAllowedLabel = (gcnew System::Windows::Forms::Label());
		this->mDonorsAllowedList = (gcnew System::Windows::Forms::ComboBox());
		this->mPlasmaVolumesGroupbox->SuspendLayout();
		this->mPlasmaLimitsGroupbox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mPlasmaVolumesGroupbox
		// 
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume6Textfield);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume5Textfield);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume4Textfield);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume3Textfield);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume2Textfield);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume1Textfield);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume6Label);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume5Label);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume4Label);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume3Label);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume2Label);
		this->mPlasmaVolumesGroupbox->Controls->Add(this->mPlasmaVolume1Label);
		this->mPlasmaVolumesGroupbox->Location = System::Drawing::Point(4, 13);
		this->mPlasmaVolumesGroupbox->Name = L"mPlasmaVolumesGroupbox";
		this->mPlasmaVolumesGroupbox->Size = System::Drawing::Size(247, 194);
		this->mPlasmaVolumesGroupbox->TabIndex = 0;
		this->mPlasmaVolumesGroupbox->TabStop = false;
		this->mPlasmaVolumesGroupbox->Text = L"Plasma Volumes";
		// 
		// mPlasmaVolume6Textfield
		// 
		this->mPlasmaVolume6Textfield->Location = System::Drawing::Point(122, 149);
		this->mPlasmaVolume6Textfield->Name = L"mPlasmaVolume6Textfield";
		this->mPlasmaVolume6Textfield->Size = System::Drawing::Size(100, 20);
		this->mPlasmaVolume6Textfield->TabIndex = 11;
		this->mPlasmaVolume6Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mPlasmaVolume5Textfield
		// 
		this->mPlasmaVolume5Textfield->Location = System::Drawing::Point(122, 123);
		this->mPlasmaVolume5Textfield->Name = L"mPlasmaVolume5Textfield";
		this->mPlasmaVolume5Textfield->Size = System::Drawing::Size(100, 20);
		this->mPlasmaVolume5Textfield->TabIndex = 10;
		this->mPlasmaVolume5Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mPlasmaVolume4Textfield
		// 
		this->mPlasmaVolume4Textfield->Location = System::Drawing::Point(122, 97);
		this->mPlasmaVolume4Textfield->Name = L"mPlasmaVolume4Textfield";
		this->mPlasmaVolume4Textfield->Size = System::Drawing::Size(100, 20);
		this->mPlasmaVolume4Textfield->TabIndex = 9;
		this->mPlasmaVolume4Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mPlasmaVolume3Textfield
		// 
		this->mPlasmaVolume3Textfield->Location = System::Drawing::Point(122, 71);
		this->mPlasmaVolume3Textfield->Name = L"mPlasmaVolume3Textfield";
		this->mPlasmaVolume3Textfield->Size = System::Drawing::Size(100, 20);
		this->mPlasmaVolume3Textfield->TabIndex = 8;
		this->mPlasmaVolume3Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mPlasmaVolume2Textfield
		// 
		this->mPlasmaVolume2Textfield->Location = System::Drawing::Point(122, 45);
		this->mPlasmaVolume2Textfield->Name = L"mPlasmaVolume2Textfield";
		this->mPlasmaVolume2Textfield->Size = System::Drawing::Size(100, 20);
		this->mPlasmaVolume2Textfield->TabIndex = 7;
		this->mPlasmaVolume2Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mPlasmaVolume1Textfield
		// 
		this->mPlasmaVolume1Textfield->Location = System::Drawing::Point(122, 19);
		this->mPlasmaVolume1Textfield->Name = L"mPlasmaVolume1Textfield";
		this->mPlasmaVolume1Textfield->Size = System::Drawing::Size(100, 20);
		this->mPlasmaVolume1Textfield->TabIndex = 6;
		this->mPlasmaVolume1Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mPlasmaVolume6Label
		// 
		this->mPlasmaVolume6Label->AutoSize = true;
		this->mPlasmaVolume6Label->Location = System::Drawing::Point(7, 152);
		this->mPlasmaVolume6Label->Name = L"mPlasmaVolume6Label";
		this->mPlasmaVolume6Label->Size = System::Drawing::Size(110, 13);
		this->mPlasmaVolume6Label->TabIndex = 5;
		this->mPlasmaVolume6Label->Text = L"Plasma 6 Volume (ml):";
		// 
		// mPlasmaVolume5Label
		// 
		this->mPlasmaVolume5Label->AutoSize = true;
		this->mPlasmaVolume5Label->Location = System::Drawing::Point(7, 126);
		this->mPlasmaVolume5Label->Name = L"mPlasmaVolume5Label";
		this->mPlasmaVolume5Label->Size = System::Drawing::Size(110, 13);
		this->mPlasmaVolume5Label->TabIndex = 4;
		this->mPlasmaVolume5Label->Text = L"Plasma 5 Volume (ml):";
		// 
		// mPlasmaVolume4Label
		// 
		this->mPlasmaVolume4Label->AutoSize = true;
		this->mPlasmaVolume4Label->Location = System::Drawing::Point(7, 100);
		this->mPlasmaVolume4Label->Name = L"mPlasmaVolume4Label";
		this->mPlasmaVolume4Label->Size = System::Drawing::Size(110, 13);
		this->mPlasmaVolume4Label->TabIndex = 3;
		this->mPlasmaVolume4Label->Text = L"Plasma 4 Volume (ml):";
		// 
		// mPlasmaVolume3Label
		// 
		this->mPlasmaVolume3Label->AutoSize = true;
		this->mPlasmaVolume3Label->Location = System::Drawing::Point(7, 74);
		this->mPlasmaVolume3Label->Name = L"mPlasmaVolume3Label";
		this->mPlasmaVolume3Label->Size = System::Drawing::Size(110, 13);
		this->mPlasmaVolume3Label->TabIndex = 2;
		this->mPlasmaVolume3Label->Text = L"Plasma 3 Volume (ml):";
		// 
		// mPlasmaVolume2Label
		// 
		this->mPlasmaVolume2Label->AutoSize = true;
		this->mPlasmaVolume2Label->Location = System::Drawing::Point(7, 48);
		this->mPlasmaVolume2Label->Name = L"mPlasmaVolume2Label";
		this->mPlasmaVolume2Label->Size = System::Drawing::Size(110, 13);
		this->mPlasmaVolume2Label->TabIndex = 1;
		this->mPlasmaVolume2Label->Text = L"Plasma 2 Volume (ml):";
		// 
		// mPlasmaVolume1Label
		// 
		this->mPlasmaVolume1Label->AutoSize = true;
		this->mPlasmaVolume1Label->Location = System::Drawing::Point(6, 22);
		this->mPlasmaVolume1Label->Name = L"mPlasmaVolume1Label";
		this->mPlasmaVolume1Label->Size = System::Drawing::Size(110, 13);
		this->mPlasmaVolume1Label->TabIndex = 0;
		this->mPlasmaVolume1Label->Text = L"Plasma 1 Volume (ml):";
		// 
		// mPlasmaLimitsGroupbox
		// 
		this->mPlasmaLimitsGroupbox->Controls->Add(this->mMaxPossibleMaxTextfield);
		this->mPlasmaLimitsGroupbox->Controls->Add(this->mMaxPossibleMaxLabel);
		this->mPlasmaLimitsGroupbox->Controls->Add(this->mMaxPossibleMinTextfield);
		this->mPlasmaLimitsGroupbox->Controls->Add(this->mMaxPossibleMinLabel);
		this->mPlasmaLimitsGroupbox->Location = System::Drawing::Point(258, 13);
		this->mPlasmaLimitsGroupbox->Name = L"mPlasmaLimitsGroupbox";
		this->mPlasmaLimitsGroupbox->Size = System::Drawing::Size(320, 87);
		this->mPlasmaLimitsGroupbox->TabIndex = 1;
		this->mPlasmaLimitsGroupbox->TabStop = false;
		this->mPlasmaLimitsGroupbox->Text = L"Limits";
		// 
		// mMaxPossibleMaxTextfield
		// 
		this->mMaxPossibleMaxTextfield->Location = System::Drawing::Point(184, 45);
		this->mMaxPossibleMaxTextfield->Name = L"mMaxPossibleMaxTextfield";
		this->mMaxPossibleMaxTextfield->Size = System::Drawing::Size(100, 20);
		this->mMaxPossibleMaxTextfield->TabIndex = 9;
		this->mMaxPossibleMaxTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mMaxPossibleMaxLabel
		// 
		this->mMaxPossibleMaxLabel->AutoSize = true;
		this->mMaxPossibleMaxLabel->Location = System::Drawing::Point(6, 48);
		this->mMaxPossibleMaxLabel->Name = L"mMaxPossibleMaxLabel";
		this->mMaxPossibleMaxLabel->Size = System::Drawing::Size(175, 13);
		this->mMaxPossibleMaxLabel->TabIndex = 8;
		this->mMaxPossibleMaxLabel->Text = L"As Much As Possible Maximum (ml):";
		// 
		// mMaxPossibleMinTextfield
		// 
		this->mMaxPossibleMinTextfield->Location = System::Drawing::Point(184, 19);
		this->mMaxPossibleMinTextfield->Name = L"mMaxPossibleMinTextfield";
		this->mMaxPossibleMinTextfield->Size = System::Drawing::Size(100, 20);
		this->mMaxPossibleMinTextfield->TabIndex = 7;
		this->mMaxPossibleMinTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mMaxPossibleMinLabel
		// 
		this->mMaxPossibleMinLabel->AutoSize = true;
		this->mMaxPossibleMinLabel->Location = System::Drawing::Point(6, 22);
		this->mMaxPossibleMinLabel->Name = L"mMaxPossibleMinLabel";
		this->mMaxPossibleMinLabel->Size = System::Drawing::Size(172, 13);
		this->mMaxPossibleMinLabel->TabIndex = 1;
		this->mMaxPossibleMinLabel->Text = L"As Much As Possible Minimum (ml):";
		// 
		// mDonorsAllowedLabel
		// 
		this->mDonorsAllowedLabel->AutoSize = true;
		this->mDonorsAllowedLabel->Location = System::Drawing::Point(331, 112);
		this->mDonorsAllowedLabel->Name = L"mDonorsAllowedLabel";
		this->mDonorsAllowedLabel->Size = System::Drawing::Size(84, 13);
		this->mDonorsAllowedLabel->TabIndex = 6;
		this->mDonorsAllowedLabel->Text = L"Donors Allowed:";
		// 
		// mDonorsAllowedList
		// 
		this->mDonorsAllowedList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mDonorsAllowedList->FormattingEnabled = true;
		this->mDonorsAllowedList->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Males Only", L"Males and AB Females", 
			L"All"});
		this->mDonorsAllowedList->Location = System::Drawing::Point(421, 109);
		this->mDonorsAllowedList->Name = L"mDonorsAllowedList";
		this->mDonorsAllowedList->Size = System::Drawing::Size(121, 21);
		this->mDonorsAllowedList->TabIndex = 7;
		// 
		// ConfigPlasma
		// 
		this->Controls->Add(this->mDonorsAllowedList);
		this->Controls->Add(this->mDonorsAllowedLabel);
		this->Controls->Add(this->mPlasmaLimitsGroupbox);
		this->Controls->Add(this->mPlasmaVolumesGroupbox);
		this->Name = L"ConfigPlasma";
		this->Size = System::Drawing::Size(599, 300);
		this->mPlasmaVolumesGroupbox->ResumeLayout(false);
		this->mPlasmaVolumesGroupbox->PerformLayout();
		this->mPlasmaLimitsGroupbox->ResumeLayout(false);
		this->mPlasmaLimitsGroupbox->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};
}
