/*****************************************************************************\
**
** ConfigPlatelets.h
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

public ref class ConfigPlatelets : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigPlatelets(void)
	{
		this->InitializeComponent();

		mPltYield = gcnew array<float>(NUM_YIELD_VOLUME);
		mPltVolume = gcnew array<float>(NUM_YIELD_VOLUME);
		mPltMss = gcnew array<int>(NUM_YIELD_VOLUME);   
		mPltCarryover = gcnew array<int>(NUM_YIELD_VOLUME);

		mPlateletProductIndex = 1;
	}

protected:
	~ConfigPlatelets(void)
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
		CONFIG_DATA_KEYS key = KEY_PLT_YIELD_1;
		int offset = KEY_PLT_PCT_CARRYOVER_1 - KEY_PLT_YIELD_1 + 1;

		this->StorePlateletsProducts();

		for (int i = 0; i < NUM_YIELD_VOLUME ; ++i) {
			float floatMin = 0;
			float floatMax = 0;
			int intMin = 0;
			int intMax = 0;

			key = static_cast<CONFIG_DATA_KEYS>(KEY_PLT_YIELD_1 + (offset * i));
			if (!CONFIG::TrimaConfigCDSUtil::IsValueValid(key, mPltYield[i], &floatMin, &floatMax)) {
				info->result = false;
				info->message = "Value out of range. \n" 
						+ "The value must be between " 
						+ String::Format("{0:N}", floatMin)
						+ " and "
						+ String::Format("{0:N}", floatMax);

				switch (i) {
				case NUM_YIELD_VOLUME - 1:
					info->control = mYield3Textfield;
					mPlateletProductIndex = i - 1;
					break;
				case NUM_YIELD_VOLUME - 2:
					info->control = mYield2Textfield;
					mPlateletProductIndex = i;
					break;
				default:
					info->control = mYield1Textfield;
					mPlateletProductIndex = i + 1;
					break;
				}

				this->LoadPlateletsProducts();
				return info;
			}

			key = static_cast<CONFIG_DATA_KEYS>(KEY_PLT_PCT_CARRYOVER_1 + (offset * i));
			if (!CONFIG::TrimaConfigCDSUtil::IsValueValid(key, mPltCarryover[i], &intMin, &intMax)) {
				info->result = false;
				info->message = "Value out of range. \n" 
						+ "The value must be between " 
						+ String::Format("{0:G}", intMin)
						+ " and "
						+ String::Format("{0:G}", intMax);

				switch (i) {
				case NUM_YIELD_VOLUME - 1:
					info->control = mCarryover3Textfield;
					mPlateletProductIndex = i - 1;
					break;
				case NUM_YIELD_VOLUME - 2:
					info->control = mCarryover2Textfield;
					mPlateletProductIndex = i;
					break;
				default:
					info->control = mCarryover1Textfield;
					mPlateletProductIndex = i + 1;
					break;
				}

				this->LoadPlateletsProducts();
				return info;
			}

			key = static_cast<CONFIG_DATA_KEYS>(KEY_PLT_VOLUME_1 + (offset * i));
			if (!CONFIG::TrimaConfigCDSUtil::IsValueValid(key, mPltVolume[i], &floatMin, &floatMax)) {
				info->result = false;
				info->message = "Value out of range. \n" 
						+ "The value must be between " 
						+ String::Format("{0:N}", floatMin)
						+ " and "
						+ String::Format("{0:N}", floatMax);

				switch (i) {
				case NUM_YIELD_VOLUME - 1:
					info->control = mVolume3Textfield;
					mPlateletProductIndex = i - 1;
					break;
				case NUM_YIELD_VOLUME - 2:
					info->control = mVolume2Textfield;
					mPlateletProductIndex = i;
					break;
				default:
					info->control = mVolume1Textfield;
					mPlateletProductIndex = i + 1;
					break;
				}

				this->LoadPlateletsProducts();
				return info;
			}
		}

		ConfigUtil::ValidateTextfieldFloat(mScalingFactorTextfield, KEY_YSF, info);

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
		mPlateletProductIndex = 1;

		for (int i = 0; i < NUM_YIELD_VOLUME; ++i) {
			mPltYield[i] = config.prodTemplates.key_plt_yield[i];
			mPltVolume[i] = config.prodTemplates.key_pls_volume[i];
			mPltMss[i] = config.prodTemplates.key_plt_mss[i];
			mPltCarryover[i] = config.prodTemplates.key_plt_pct_carryover[i];
		}

		ConfigUtil::SetTextfieldFloat(mScalingFactorTextfield, config.predictionCfg.key_ysf);
		ConfigUtil::SetListSelection(mDonorsAllowedList, config.predictionCfg.key_male_only_plt);

		this->LoadPlateletsProducts();
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
		this->StorePlateletsProducts();

		for (int i = 0; i < NUM_YIELD_VOLUME; ++i) {
			config.prodTemplates.key_plt_yield[i] = mPltYield[i];
			config.prodTemplates.key_pls_volume[i] = mPltVolume[i];
			config.prodTemplates.key_plt_mss[i] = mPltMss[i];
			config.prodTemplates.key_plt_pct_carryover[i] = mPltCarryover[i];
		}

		config.predictionCfg.key_ysf = ConfigUtil::GetTextfieldFloat(mScalingFactorTextfield);
		config.predictionCfg.key_male_only_plt = ConfigUtil::GetListSelection(mDonorsAllowedList);
	}

private:
	void LoadPlateletsProducts(void)
	{
		ConfigUtil::SetTextfieldInt(mIndex1Textfield, mPlateletProductIndex);
		ConfigUtil::SetCheckBoxState(mAdditive1Checkbox, mPltMss[mPlateletProductIndex - 1]);
		ConfigUtil::SetTextfieldFloat(mYield1Textfield, mPltYield[mPlateletProductIndex - 1]);
		ConfigUtil::SetTextfieldInt(mCarryover1Textfield, mPltCarryover[mPlateletProductIndex - 1]);
		ConfigUtil::SetTextfieldFloat(mVolume1Textfield, mPltVolume[mPlateletProductIndex - 1]);
	
		ConfigUtil::SetTextfieldInt(mIndex2Textfield, mPlateletProductIndex + 1);
		ConfigUtil::SetCheckBoxState(mAdditive2Checkbox, mPltMss[mPlateletProductIndex]);
		ConfigUtil::SetTextfieldFloat(mYield2Textfield, mPltYield[mPlateletProductIndex]);
		ConfigUtil::SetTextfieldInt(mCarryover2Textfield, mPltCarryover[mPlateletProductIndex]);
		ConfigUtil::SetTextfieldFloat(mVolume2Textfield, mPltVolume[mPlateletProductIndex]);
	
		ConfigUtil::SetTextfieldInt(mIndex3Textfield, mPlateletProductIndex + 2);
		ConfigUtil::SetCheckBoxState(mAdditive3Checkbox, mPltMss[mPlateletProductIndex + 1]);
		ConfigUtil::SetTextfieldFloat(mYield3Textfield, mPltYield[mPlateletProductIndex + 1]);
		ConfigUtil::SetTextfieldInt(mCarryover3Textfield, mPltCarryover[mPlateletProductIndex + 1]);
		ConfigUtil::SetTextfieldFloat(mVolume3Textfield, mPltVolume[mPlateletProductIndex + 1]);
	}

	void StorePlateletsProducts(void)
	{
		mPltMss[mPlateletProductIndex - 1] = ConfigUtil::GetCheckBoxState(mAdditive1Checkbox);
		mPltYield[mPlateletProductIndex - 1] = ConfigUtil::GetTextfieldFloat(mYield1Textfield);
		mPltCarryover[mPlateletProductIndex - 1] = ConfigUtil::GetTextfieldInt(mCarryover1Textfield);
		mPltVolume[mPlateletProductIndex - 1] = ConfigUtil::GetTextfieldFloat(mVolume1Textfield);

		mPltMss[mPlateletProductIndex] = ConfigUtil::GetCheckBoxState(mAdditive2Checkbox);
		mPltYield[mPlateletProductIndex] = ConfigUtil::GetTextfieldFloat(mYield2Textfield);
		mPltCarryover[mPlateletProductIndex] = ConfigUtil::GetTextfieldInt(mCarryover2Textfield);
		mPltVolume[mPlateletProductIndex] = ConfigUtil::GetTextfieldFloat(mVolume2Textfield);

		mPltMss[mPlateletProductIndex + 1] = ConfigUtil::GetCheckBoxState(mAdditive3Checkbox);
		mPltYield[mPlateletProductIndex + 1] = ConfigUtil::GetTextfieldFloat(mYield3Textfield);
		mPltCarryover[mPlateletProductIndex + 1] = ConfigUtil::GetTextfieldInt(mCarryover3Textfield);
		mPltVolume[mPlateletProductIndex + 1] = ConfigUtil::GetTextfieldFloat(mVolume3Textfield);
	}
#pragma endregion

#pragma region Handlers
private: 
	System::Void mUpButton_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		if (mPlateletProductIndex > 1) {
			this->StorePlateletsProducts();
			--mPlateletProductIndex;
			this->LoadPlateletsProducts();
		}
	}

	System::Void mDownButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (mPlateletProductIndex < NUM_YIELD_VOLUME - 2) {
			this->StorePlateletsProducts();
			++mPlateletProductIndex;
			this->LoadPlateletsProducts();
		}
	}
#pragma endregion

#pragma region Members
private: int mPlateletProductIndex;
private: array<float> ^mPltYield;
private: array<float> ^mPltVolume;
private: array<int> ^mPltMss;   
private: array<int> ^mPltCarryover;

private: System::Windows::Forms::TextBox^  mYield1Textfield;
private: System::Windows::Forms::TextBox^  mCarryover1Textfield;
private: System::Windows::Forms::TextBox^  mVolume1Textfield;
private: System::Windows::Forms::TextBox^  mIndex1Textfield;
private: System::Windows::Forms::CheckBox^  mAdditive1Checkbox;
private: System::Windows::Forms::GroupBox^  mPlateletsGroupbox;
private: System::Windows::Forms::Label^  mVolumeLabel;
private: System::Windows::Forms::Label^  mCarryoverLabel;
private: System::Windows::Forms::Label^  mYieldLabel;
private: System::Windows::Forms::CheckBox^  mAdditive3Checkbox;
private: System::Windows::Forms::TextBox^  mYield3Textfield;
private: System::Windows::Forms::TextBox^  mIndex3Textfield;
private: System::Windows::Forms::TextBox^  mCarryover3Textfield;
private: System::Windows::Forms::TextBox^  mVolume3Textfield;
private: System::Windows::Forms::CheckBox^  mAdditive2Checkbox;
private: System::Windows::Forms::TextBox^  mYield2Textfield;
private: System::Windows::Forms::TextBox^  mIndex2Textfield;
private: System::Windows::Forms::TextBox^  mCarryover2Textfield;
private: System::Windows::Forms::TextBox^  mVolume2Textfield;
private: System::Windows::Forms::Label^  mScalingFactorLabel;
private: System::Windows::Forms::TextBox^  mScalingFactorTextfield;
private: System::Windows::Forms::Label^  mDonorsAllowedLabel;
private: System::Windows::Forms::ComboBox^  mDonorsAllowedList;
private: System::Windows::Forms::Button^  mDownButton;
private: System::Windows::Forms::Button^  mUpButton;
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
		this->mYield1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mCarryover1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolume1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mIndex1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mAdditive1Checkbox = (gcnew System::Windows::Forms::CheckBox());
		this->mPlateletsGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mVolumeLabel = (gcnew System::Windows::Forms::Label());
		this->mCarryoverLabel = (gcnew System::Windows::Forms::Label());
		this->mYieldLabel = (gcnew System::Windows::Forms::Label());
		this->mDownButton = (gcnew System::Windows::Forms::Button());
		this->mAdditive3Checkbox = (gcnew System::Windows::Forms::CheckBox());
		this->mYield3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mIndex3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mCarryover3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolume3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mAdditive2Checkbox = (gcnew System::Windows::Forms::CheckBox());
		this->mYield2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mIndex2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mCarryover2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mVolume2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mUpButton = (gcnew System::Windows::Forms::Button());
		this->mScalingFactorLabel = (gcnew System::Windows::Forms::Label());
		this->mScalingFactorTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mDonorsAllowedLabel = (gcnew System::Windows::Forms::Label());
		this->mDonorsAllowedList = (gcnew System::Windows::Forms::ComboBox());
		this->mPlateletsGroupbox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mYield1Textfield
		// 
		this->mYield1Textfield->Location = System::Drawing::Point(170, 48);
		this->mYield1Textfield->Name = L"mYield1Textfield";
		this->mYield1Textfield->Size = System::Drawing::Size(100, 20);
		this->mYield1Textfield->TabIndex = 0;
		this->mYield1Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mCarryover1Textfield
		// 
		this->mCarryover1Textfield->Location = System::Drawing::Point(276, 48);
		this->mCarryover1Textfield->Name = L"mCarryover1Textfield";
		this->mCarryover1Textfield->Size = System::Drawing::Size(100, 20);
		this->mCarryover1Textfield->TabIndex = 1;
		this->mCarryover1Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mVolume1Textfield
		// 
		this->mVolume1Textfield->Location = System::Drawing::Point(382, 48);
		this->mVolume1Textfield->Name = L"mVolume1Textfield";
		this->mVolume1Textfield->Size = System::Drawing::Size(100, 20);
		this->mVolume1Textfield->TabIndex = 2;
		this->mVolume1Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
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
		// mPlateletsGroupbox
		// 
		this->mPlateletsGroupbox->Controls->Add(this->mVolumeLabel);
		this->mPlateletsGroupbox->Controls->Add(this->mCarryoverLabel);
		this->mPlateletsGroupbox->Controls->Add(this->mYieldLabel);
		this->mPlateletsGroupbox->Controls->Add(this->mDownButton);
		this->mPlateletsGroupbox->Controls->Add(this->mAdditive3Checkbox);
		this->mPlateletsGroupbox->Controls->Add(this->mYield3Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mIndex3Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mCarryover3Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mVolume3Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mAdditive2Checkbox);
		this->mPlateletsGroupbox->Controls->Add(this->mYield2Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mIndex2Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mCarryover2Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mVolume2Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mUpButton);
		this->mPlateletsGroupbox->Controls->Add(this->mAdditive1Checkbox);
		this->mPlateletsGroupbox->Controls->Add(this->mYield1Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mIndex1Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mCarryover1Textfield);
		this->mPlateletsGroupbox->Controls->Add(this->mVolume1Textfield);
		this->mPlateletsGroupbox->Location = System::Drawing::Point(4, 15);
		this->mPlateletsGroupbox->Name = L"mPlateletsGroupbox";
		this->mPlateletsGroupbox->Size = System::Drawing::Size(572, 167);
		this->mPlateletsGroupbox->TabIndex = 5;
		this->mPlateletsGroupbox->TabStop = false;
		this->mPlateletsGroupbox->Text = L"Platelets";
		// 
		// mVolumeLabel
		// 
		this->mVolumeLabel->AutoSize = true;
		this->mVolumeLabel->Location = System::Drawing::Point(382, 27);
		this->mVolumeLabel->Name = L"mVolumeLabel";
		this->mVolumeLabel->Size = System::Drawing::Size(96, 13);
		this->mVolumeLabel->TabIndex = 18;
		this->mVolumeLabel->Text = L"Collect Volume (ml)";
		// 
		// mCarryoverLabel
		// 
		this->mCarryoverLabel->AutoSize = true;
		this->mCarryoverLabel->Location = System::Drawing::Point(276, 28);
		this->mCarryoverLabel->Name = L"mCarryoverLabel";
		this->mCarryoverLabel->Size = System::Drawing::Size(58, 13);
		this->mCarryoverLabel->TabIndex = 17;
		this->mCarryoverLabel->Text = L"Plasma (%)";
		// 
		// mYieldLabel
		// 
		this->mYieldLabel->AutoSize = true;
		this->mYieldLabel->Location = System::Drawing::Point(170, 28);
		this->mYieldLabel->Name = L"mYieldLabel";
		this->mYieldLabel->Size = System::Drawing::Size(30, 13);
		this->mYieldLabel->TabIndex = 16;
		this->mYieldLabel->Text = L"Yield";
		// 
		// mDownButton
		// 
		this->mDownButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 6.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->mDownButton->Location = System::Drawing::Point(12, 126);
		this->mDownButton->Name = L"mDownButton";
		this->mDownButton->Size = System::Drawing::Size(40, 23);
		this->mDownButton->TabIndex = 15;
		this->mDownButton->Text = L"Down";
		this->mDownButton->UseVisualStyleBackColor = true;
		this->mDownButton->Click += gcnew System::EventHandler(this, &ConfigPlatelets::mDownButton_Click);
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
		// mYield3Textfield
		// 
		this->mYield3Textfield->Location = System::Drawing::Point(170, 100);
		this->mYield3Textfield->Name = L"mYield3Textfield";
		this->mYield3Textfield->Size = System::Drawing::Size(100, 20);
		this->mYield3Textfield->TabIndex = 10;
		this->mYield3Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
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
		// mCarryover3Textfield
		// 
		this->mCarryover3Textfield->Location = System::Drawing::Point(276, 100);
		this->mCarryover3Textfield->Name = L"mCarryover3Textfield";
		this->mCarryover3Textfield->Size = System::Drawing::Size(100, 20);
		this->mCarryover3Textfield->TabIndex = 11;
		this->mCarryover3Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mVolume3Textfield
		// 
		this->mVolume3Textfield->Location = System::Drawing::Point(382, 100);
		this->mVolume3Textfield->Name = L"mVolume3Textfield";
		this->mVolume3Textfield->Size = System::Drawing::Size(100, 20);
		this->mVolume3Textfield->TabIndex = 12;
		this->mVolume3Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
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
		// mYield2Textfield
		// 
		this->mYield2Textfield->Location = System::Drawing::Point(170, 74);
		this->mYield2Textfield->Name = L"mYield2Textfield";
		this->mYield2Textfield->Size = System::Drawing::Size(100, 20);
		this->mYield2Textfield->TabIndex = 5;
		this->mYield2Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
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
		// mCarryover2Textfield
		// 
		this->mCarryover2Textfield->Location = System::Drawing::Point(276, 74);
		this->mCarryover2Textfield->Name = L"mCarryover2Textfield";
		this->mCarryover2Textfield->Size = System::Drawing::Size(100, 20);
		this->mCarryover2Textfield->TabIndex = 6;
		this->mCarryover2Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressIntOnly);
		// 
		// mVolume2Textfield
		// 
		this->mVolume2Textfield->Location = System::Drawing::Point(382, 74);
		this->mVolume2Textfield->Name = L"mVolume2Textfield";
		this->mVolume2Textfield->Size = System::Drawing::Size(100, 20);
		this->mVolume2Textfield->TabIndex = 7;
		this->mVolume2Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mUpButton
		// 
		this->mUpButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 6.5F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->mUpButton->Location = System::Drawing::Point(12, 19);
		this->mUpButton->Name = L"mUpButton";
		this->mUpButton->Size = System::Drawing::Size(40, 23);
		this->mUpButton->TabIndex = 0;
		this->mUpButton->Text = L"Up";
		this->mUpButton->UseVisualStyleBackColor = true;
		this->mUpButton->Click += gcnew System::EventHandler(this, &ConfigPlatelets::mUpButton_Click);
		// 
		// mScalingFactorLabel
		// 
		this->mScalingFactorLabel->AutoSize = true;
		this->mScalingFactorLabel->Location = System::Drawing::Point(60, 202);
		this->mScalingFactorLabel->Name = L"mScalingFactorLabel";
		this->mScalingFactorLabel->Size = System::Drawing::Size(104, 13);
		this->mScalingFactorLabel->TabIndex = 6;
		this->mScalingFactorLabel->Text = L"Yield Scaling Factor:";
		// 
		// mScalingFactorTextfield
		// 
		this->mScalingFactorTextfield->Location = System::Drawing::Point(174, 199);
		this->mScalingFactorTextfield->Name = L"mScalingFactorTextfield";
		this->mScalingFactorTextfield->Size = System::Drawing::Size(100, 20);
		this->mScalingFactorTextfield->TabIndex = 7;
		this->mScalingFactorTextfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
		// 
		// mDonorsAllowedLabel
		// 
		this->mDonorsAllowedLabel->AutoSize = true;
		this->mDonorsAllowedLabel->Location = System::Drawing::Point(60, 229);
		this->mDonorsAllowedLabel->Name = L"mDonorsAllowedLabel";
		this->mDonorsAllowedLabel->Size = System::Drawing::Size(84, 13);
		this->mDonorsAllowedLabel->TabIndex = 8;
		this->mDonorsAllowedLabel->Text = L"Donors Allowed:";
		// 
		// mDonorsAllowedList
		// 
		this->mDonorsAllowedList->FormattingEnabled = true;
		this->mDonorsAllowedList->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Males Only", L"Males and AB Females", 
			L"All"});
		this->mDonorsAllowedList->Location = System::Drawing::Point(174, 226);
		this->mDonorsAllowedList->Name = L"mDonorsAllowedList";
		this->mDonorsAllowedList->Size = System::Drawing::Size(100, 21);
		this->mDonorsAllowedList->TabIndex = 9;
		// 
		// ConfigPlatelets
		// 
		this->Controls->Add(this->mDonorsAllowedList);
		this->Controls->Add(this->mDonorsAllowedLabel);
		this->Controls->Add(this->mScalingFactorTextfield);
		this->Controls->Add(this->mScalingFactorLabel);
		this->Controls->Add(this->mPlateletsGroupbox);
		this->Name = L"ConfigPlatelets";
		this->Size = System::Drawing::Size(599, 300);
		this->mPlateletsGroupbox->ResumeLayout(false);
		this->mPlateletsGroupbox->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};
}
