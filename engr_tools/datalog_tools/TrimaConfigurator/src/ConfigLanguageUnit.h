/*****************************************************************************\
**
** ConfigLanguageUnit.h
**
** Language and units config page.
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
using namespace System::Data;
using namespace System::Drawing;

/* Classes *******************************************************************/

public ref class ConfigLanguageUnit : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigLanguageUnit(void)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigLanguageUnit(void) 
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
		return gcnew ValidationInfo();
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
		ConfigUtil::SetListSelection(mLanguageList, config.langCfg.key_lang);
		ConfigUtil::SetListSelection(mHeightList, config.langCfg.key_height);
		ConfigUtil::SetListSelection(mWeightList, config.langCfg.key_weight);
		ConfigUtil::SetListSelection(mRBCUnitList, config.langCfg.key_crit_or_glob);
		ConfigUtil::SetListSelection(mDelimiterList, config.langCfg.key_decimal_delimiter);
		ConfigUtil::SetListSelection(mDateFormatList, config.langCfg.key_date_format);
		ConfigUtil::SetListSelection(mTimeFormatList, config.langCfg.key_time_format);
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
		config.langCfg.key_lang = ConfigUtil::GetListSelection(mLanguageList);
		config.langCfg.key_height = ConfigUtil::GetListSelection(mHeightList);
		config.langCfg.key_weight = ConfigUtil::GetListSelection(mWeightList);
		config.langCfg.key_crit_or_glob = ConfigUtil::GetListSelection(mRBCUnitList);
		config.langCfg.key_decimal_delimiter = ConfigUtil::GetListSelection(mDelimiterList);
		config.langCfg.key_date_format = ConfigUtil::GetListSelection(mDateFormatList);
		config.langCfg.key_time_format = ConfigUtil::GetListSelection(mTimeFormatList);
	}
#pragma endregion

#pragma region Members
private: System::Windows::Forms::Label^  mLanguageLabel;
private: System::Windows::Forms::ComboBox^  mLanguageList;
private: System::Windows::Forms::Label^  mHeightLabel;
private: System::Windows::Forms::ComboBox^  mHeightList;
private: System::Windows::Forms::ComboBox^  mWeightList;
private: System::Windows::Forms::Label^  mWeightLabel;
private: System::Windows::Forms::Label^  mDelimiterLabel;
private: System::Windows::Forms::ComboBox^  mDelimiterList;
private: System::Windows::Forms::Label^  mRBCUnitLabel;
private: System::Windows::Forms::ComboBox^  mRBCUnitList;
private: System::Windows::Forms::Label^  mDateFormatLabel;
private: System::Windows::Forms::ComboBox^  mDateFormatList;
private: System::Windows::Forms::ComboBox^  mTimeFormatList;
private: System::Windows::Forms::Label^  mTimeFormatLabel;
private: System::Windows::Forms::GroupBox^  mUnitsGroupbox;
private: System::Windows::Forms::GroupBox^  mFormatGroupbox;
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
		this->mLanguageLabel = (gcnew System::Windows::Forms::Label());
		this->mLanguageList = (gcnew System::Windows::Forms::ComboBox());
		this->mHeightLabel = (gcnew System::Windows::Forms::Label());
		this->mHeightList = (gcnew System::Windows::Forms::ComboBox());
		this->mWeightList = (gcnew System::Windows::Forms::ComboBox());
		this->mWeightLabel = (gcnew System::Windows::Forms::Label());
		this->mDelimiterLabel = (gcnew System::Windows::Forms::Label());
		this->mDelimiterList = (gcnew System::Windows::Forms::ComboBox());
		this->mRBCUnitLabel = (gcnew System::Windows::Forms::Label());
		this->mRBCUnitList = (gcnew System::Windows::Forms::ComboBox());
		this->mDateFormatLabel = (gcnew System::Windows::Forms::Label());
		this->mDateFormatList = (gcnew System::Windows::Forms::ComboBox());
		this->mTimeFormatList = (gcnew System::Windows::Forms::ComboBox());
		this->mTimeFormatLabel = (gcnew System::Windows::Forms::Label());
		this->mUnitsGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mFormatGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mUnitsGroupbox->SuspendLayout();
		this->mFormatGroupbox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mLanguageLabel
		// 
		this->mLanguageLabel->AutoSize = true;
		this->mLanguageLabel->Location = System::Drawing::Point(12, 18);
		this->mLanguageLabel->Name = L"mLanguageLabel";
		this->mLanguageLabel->Size = System::Drawing::Size(58, 13);
		this->mLanguageLabel->TabIndex = 0;
		this->mLanguageLabel->Text = L"Language:";
		// 
		// mLanguageList
		// 
		this->mLanguageList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mLanguageList->FormattingEnabled = true;
		this->mLanguageList->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"English", L"French"});
		this->mLanguageList->Location = System::Drawing::Point(107, 15);
		this->mLanguageList->Name = L"mLanguageList";
		this->mLanguageList->Size = System::Drawing::Size(121, 21);
		this->mLanguageList->TabIndex = 1;
		// 
		// mHeightLabel
		// 
		this->mHeightLabel->AutoSize = true;
		this->mHeightLabel->Location = System::Drawing::Point(6, 16);
		this->mHeightLabel->Name = L"mHeightLabel";
		this->mHeightLabel->Size = System::Drawing::Size(41, 13);
		this->mHeightLabel->TabIndex = 2;
		this->mHeightLabel->Text = L"Height:";
		// 
		// mHeightList
		// 
		this->mHeightList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mHeightList->FormattingEnabled = true;
		this->mHeightList->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"cm", L"ft/in"});
		this->mHeightList->Location = System::Drawing::Point(101, 13);
		this->mHeightList->Name = L"mHeightList";
		this->mHeightList->Size = System::Drawing::Size(121, 21);
		this->mHeightList->TabIndex = 3;
		// 
		// mWeightList
		// 
		this->mWeightList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mWeightList->FormattingEnabled = true;
		this->mWeightList->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"kg", L"lb"});
		this->mWeightList->Location = System::Drawing::Point(101, 40);
		this->mWeightList->Name = L"mWeightList";
		this->mWeightList->Size = System::Drawing::Size(121, 21);
		this->mWeightList->TabIndex = 5;
		// 
		// mWeightLabel
		// 
		this->mWeightLabel->AutoSize = true;
		this->mWeightLabel->Location = System::Drawing::Point(6, 43);
		this->mWeightLabel->Name = L"mWeightLabel";
		this->mWeightLabel->Size = System::Drawing::Size(44, 13);
		this->mWeightLabel->TabIndex = 4;
		this->mWeightLabel->Text = L"Weight:";
		// 
		// mDelimiterLabel
		// 
		this->mDelimiterLabel->AutoSize = true;
		this->mDelimiterLabel->Location = System::Drawing::Point(6, 22);
		this->mDelimiterLabel->Name = L"mDelimiterLabel";
		this->mDelimiterLabel->Size = System::Drawing::Size(91, 13);
		this->mDelimiterLabel->TabIndex = 6;
		this->mDelimiterLabel->Text = L"Decimal Delimiter:";
		// 
		// mDelimiterList
		// 
		this->mDelimiterList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mDelimiterList->FormattingEnabled = true;
		this->mDelimiterList->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"0,000.00", L"0.000,00"});
		this->mDelimiterList->Location = System::Drawing::Point(101, 19);
		this->mDelimiterList->Name = L"mDelimiterList";
		this->mDelimiterList->Size = System::Drawing::Size(121, 21);
		this->mDelimiterList->TabIndex = 7;
		// 
		// mRBCUnitLabel
		// 
		this->mRBCUnitLabel->AutoSize = true;
		this->mRBCUnitLabel->Location = System::Drawing::Point(6, 70);
		this->mRBCUnitLabel->Name = L"mRBCUnitLabel";
		this->mRBCUnitLabel->Size = System::Drawing::Size(59, 13);
		this->mRBCUnitLabel->TabIndex = 8;
		this->mRBCUnitLabel->Text = L"RBC Units:";
		// 
		// mRBCUnitList
		// 
		this->mRBCUnitList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mRBCUnitList->FormattingEnabled = true;
		this->mRBCUnitList->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"Hematocrit (%)", L"Hemoglobin (g/dl)"});
		this->mRBCUnitList->Location = System::Drawing::Point(101, 67);
		this->mRBCUnitList->Name = L"mRBCUnitList";
		this->mRBCUnitList->Size = System::Drawing::Size(121, 21);
		this->mRBCUnitList->TabIndex = 9;
		// 
		// mDateFormatLabel
		// 
		this->mDateFormatLabel->AutoSize = true;
		this->mDateFormatLabel->Location = System::Drawing::Point(6, 49);
		this->mDateFormatLabel->Name = L"mDateFormatLabel";
		this->mDateFormatLabel->Size = System::Drawing::Size(68, 13);
		this->mDateFormatLabel->TabIndex = 10;
		this->mDateFormatLabel->Text = L"Date Format:";
		// 
		// mDateFormatList
		// 
		this->mDateFormatList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mDateFormatList->FormattingEnabled = true;
		this->mDateFormatList->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"mm/dd/yy", L"dd/mm/yy", L"yy/mm/dd"});
		this->mDateFormatList->Location = System::Drawing::Point(101, 46);
		this->mDateFormatList->Name = L"mDateFormatList";
		this->mDateFormatList->Size = System::Drawing::Size(121, 21);
		this->mDateFormatList->TabIndex = 11;
		// 
		// mTimeFormatList
		// 
		this->mTimeFormatList->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mTimeFormatList->FormattingEnabled = true;
		this->mTimeFormatList->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"hh:mm", L"hh:mm am/pm"});
		this->mTimeFormatList->Location = System::Drawing::Point(101, 73);
		this->mTimeFormatList->Name = L"mTimeFormatList";
		this->mTimeFormatList->Size = System::Drawing::Size(121, 21);
		this->mTimeFormatList->TabIndex = 12;
		// 
		// mTimeFormatLabel
		// 
		this->mTimeFormatLabel->AutoSize = true;
		this->mTimeFormatLabel->Location = System::Drawing::Point(6, 76);
		this->mTimeFormatLabel->Name = L"mTimeFormatLabel";
		this->mTimeFormatLabel->Size = System::Drawing::Size(68, 13);
		this->mTimeFormatLabel->TabIndex = 13;
		this->mTimeFormatLabel->Text = L"Time Format:";
		// 
		// mUnitsGroupbox
		// 
		this->mUnitsGroupbox->Controls->Add(this->mHeightList);
		this->mUnitsGroupbox->Controls->Add(this->mHeightLabel);
		this->mUnitsGroupbox->Controls->Add(this->mWeightLabel);
		this->mUnitsGroupbox->Controls->Add(this->mWeightList);
		this->mUnitsGroupbox->Controls->Add(this->mRBCUnitList);
		this->mUnitsGroupbox->Controls->Add(this->mRBCUnitLabel);
		this->mUnitsGroupbox->Location = System::Drawing::Point(6, 42);
		this->mUnitsGroupbox->Name = L"mUnitsGroupbox";
		this->mUnitsGroupbox->Size = System::Drawing::Size(564, 102);
		this->mUnitsGroupbox->TabIndex = 14;
		this->mUnitsGroupbox->TabStop = false;
		this->mUnitsGroupbox->Text = L"Units";
		// 
		// mFormatGroupbox
		// 
		this->mFormatGroupbox->Controls->Add(this->mDelimiterList);
		this->mFormatGroupbox->Controls->Add(this->mDelimiterLabel);
		this->mFormatGroupbox->Controls->Add(this->mTimeFormatLabel);
		this->mFormatGroupbox->Controls->Add(this->mDateFormatLabel);
		this->mFormatGroupbox->Controls->Add(this->mDateFormatList);
		this->mFormatGroupbox->Controls->Add(this->mTimeFormatList);
		this->mFormatGroupbox->Location = System::Drawing::Point(6, 151);
		this->mFormatGroupbox->Name = L"mFormatGroupbox";
		this->mFormatGroupbox->Size = System::Drawing::Size(564, 112);
		this->mFormatGroupbox->TabIndex = 15;
		this->mFormatGroupbox->TabStop = false;
		this->mFormatGroupbox->Text = L"Formats";
		// 
		// ConfigLanguageUnit
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->Controls->Add(this->mFormatGroupbox);
		this->Controls->Add(this->mUnitsGroupbox);
		this->Controls->Add(this->mLanguageList);
		this->Controls->Add(this->mLanguageLabel);
		this->Name = L"ConfigLanguageUnit";
		this->Size = System::Drawing::Size(599, 300);
		this->mUnitsGroupbox->ResumeLayout(false);
		this->mUnitsGroupbox->PerformLayout();
		this->mFormatGroupbox->ResumeLayout(false);
		this->mFormatGroupbox->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};
}
