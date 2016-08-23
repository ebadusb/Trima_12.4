/*****************************************************************************\
**
** ConfigProcedurePriority.h
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

public ref class ConfigProcedurePriority : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigProcedurePriority(void)
	{
		this->InitializeComponent();

		mProducts = gcnew array<ConfigProductDefinition^>(NUM_OFFLINE_PROCEDURES);

		for (int i = 0; i < NUM_OFFLINE_PROCEDURES; ++i) {
			mProducts[i] = gcnew ConfigProductDefinition();
		}

		mProductIndex = 1;
	}

protected:
	~ConfigProcedurePriority(void)
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
		CONFIG_DATA_KEYS key = KEY_PRI_TIME_A;
		int offset = KEY_PRI_TIME_A - KEY_PRI_PLATELET_A + 1;

		this->StoreProducts();

		for (int i = 0; i < NUM_OFFLINE_PROCEDURES ; ++i) {
			float value = mProducts[i]->key_time;
			float min = 0;
			float max = 0;

			key = static_cast<CONFIG_DATA_KEYS>(KEY_PRI_TIME_A + (offset * i));

			if (!CONFIG::TrimaConfigCDSUtil::IsValueValid(key, value, &min, &max)) {
				info->result = false;
				info->message = "Value out of range. \n" 
						+ "The value must be between " 
						+ String::Format("{0:N}", min)
						+ " and "
						+ String::Format("{0:N}", max);

				switch (i) {
				case NUM_OFFLINE_PROCEDURES - 1:
					info->control = mDuration3Textfield;
					mProductIndex = i - 1;
					break;
				case NUM_OFFLINE_PROCEDURES - 2:
					info->control = mDuration2Textfield;
					mProductIndex = i;
					break;
				default:
					info->control = mDuration1Textfield;
					mProductIndex = i + 1;
					break;
				}

				this->LoadProducts();
				return info;
			}
		}

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
		mProductIndex = 1;

		for (int i = 0; i < NUM_OFFLINE_PROCEDURES; ++i) {
			mProducts[i]->key_platelet = config.prodDefArray[i].key_platelet;
			mProducts[i]->key_plasma = config.prodDefArray[i].key_plasma;
			mProducts[i]->key_rbc = config.prodDefArray[i].key_rbc;
			mProducts[i]->key_blood_type = config.prodDefArray[i].key_blood_type;
			mProducts[i]->key_time = config.prodDefArray[i].key_time;
		}

		this->LoadProducts();
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
		this->StoreProducts();

		for (int i = 0; i < NUM_OFFLINE_PROCEDURES; ++i) {
			config.prodDefArray[i].key_platelet = mProducts[i]->key_platelet;
			config.prodDefArray[i].key_plasma = mProducts[i]->key_plasma;
			config.prodDefArray[i].key_rbc = mProducts[i]->key_rbc;
			config.prodDefArray[i].key_blood_type = mProducts[i]->key_blood_type;
			config.prodDefArray[i].key_time = mProducts[i]->key_time;
		}
	}

private:
	void LoadProducts(void)
	{
		ConfigUtil::SetTextfieldInt(mIndex1Textfield, mProductIndex);
		ConfigUtil::SetListSelection(mPlatelets1List, mProducts[mProductIndex - 1]->key_platelet);
		ConfigUtil::SetListSelection(mPlasma1List, mProducts[mProductIndex - 1]->key_plasma);
		ConfigUtil::SetListSelection(mRBC1List, mProducts[mProductIndex - 1]->key_rbc);
		ConfigUtil::SetListSelection(mType1List, mProducts[mProductIndex - 1]->key_blood_type);
		ConfigUtil::SetTextfieldFloat(mDuration1Textfield, mProducts[mProductIndex - 1]->key_time);
	
		ConfigUtil::SetTextfieldInt(mIndex2Textfield, mProductIndex + 1);
		ConfigUtil::SetListSelection(mPlatelets2List, mProducts[mProductIndex]->key_platelet);
		ConfigUtil::SetListSelection(mPlasma2List, mProducts[mProductIndex]->key_plasma);
		ConfigUtil::SetListSelection(mRBC2List, mProducts[mProductIndex]->key_rbc);
		ConfigUtil::SetListSelection(mType2List, mProducts[mProductIndex]->key_blood_type);
		ConfigUtil::SetTextfieldFloat(mDuration2Textfield, mProducts[mProductIndex]->key_time);
		
		ConfigUtil::SetTextfieldInt(mIndex3Textfield, mProductIndex + 2);
		ConfigUtil::SetListSelection(mPlatelets3List, mProducts[mProductIndex + 1]->key_platelet);
		ConfigUtil::SetListSelection(mPlasma3List, mProducts[mProductIndex + 1]->key_plasma);
		ConfigUtil::SetListSelection(mRBC3List, mProducts[mProductIndex + 1]->key_rbc);
		ConfigUtil::SetListSelection(mType3List, mProducts[mProductIndex + 1]->key_blood_type);
		ConfigUtil::SetTextfieldFloat(mDuration3Textfield, mProducts[mProductIndex + 1]->key_time);
	}

	void StoreProducts(void)
	{
		mProducts[mProductIndex - 1]->key_platelet = ConfigUtil::GetListSelection(mPlatelets1List);
		mProducts[mProductIndex - 1]->key_plasma = ConfigUtil::GetListSelection(mPlasma1List);
		mProducts[mProductIndex - 1]->key_rbc = ConfigUtil::GetListSelection(mRBC1List);
		mProducts[mProductIndex - 1]->key_blood_type = ConfigUtil::GetListSelection(mType1List);
		mProducts[mProductIndex - 1]->key_time = ConfigUtil::GetTextfieldFloat(mDuration1Textfield);
	
		mProducts[mProductIndex]->key_platelet = ConfigUtil::GetListSelection(mPlatelets2List);
		mProducts[mProductIndex]->key_plasma = ConfigUtil::GetListSelection(mPlasma2List);
		mProducts[mProductIndex]->key_rbc = ConfigUtil::GetListSelection(mRBC2List);
		mProducts[mProductIndex]->key_blood_type = ConfigUtil::GetListSelection(mType2List);
		mProducts[mProductIndex]->key_time = ConfigUtil::GetTextfieldFloat(mDuration2Textfield);

		mProducts[mProductIndex + 1]->key_platelet = ConfigUtil::GetListSelection(mPlatelets3List);
		mProducts[mProductIndex + 1]->key_plasma = ConfigUtil::GetListSelection(mPlasma3List);
		mProducts[mProductIndex + 1]->key_rbc = ConfigUtil::GetListSelection(mRBC3List);
		mProducts[mProductIndex + 1]->key_blood_type = ConfigUtil::GetListSelection(mType3List);
		mProducts[mProductIndex + 1]->key_time = ConfigUtil::GetTextfieldFloat(mDuration3Textfield);
	}
#pragma endregion

#pragma region Handlers 
private: 
	System::Void UpButtonHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		if (mProductIndex > 1) {
			this->StoreProducts();
			--mProductIndex;
			this->LoadProducts();
		}
	}

	System::Void DownButtonHandler(System::Object^  sender, System::EventArgs^  e)
	{
		if (mProductIndex < NUM_OFFLINE_PROCEDURES - 2) {
			this->StoreProducts();
			++mProductIndex;
			this->LoadProducts();
		}
	}

	System::Void ClearButtonHandler(System::Object^  sender, System::EventArgs^  e)
	{
		for (int i = 0; i < NUM_OFFLINE_PROCEDURES; ++i) {
			mProducts[i]->key_platelet = 0;
			mProducts[i]->key_plasma = 0;
			mProducts[i]->key_rbc = 0;
			mProducts[i]->key_blood_type = 0;
			mProducts[i]->key_time = 120;
		}

		mProductIndex = 1;
		this->LoadProducts();
	}
#pragma endregion

#pragma region Types 
	ref struct ConfigProductDefinition {
		int   key_platelet;
		int   key_plasma;
		int   key_rbc;
		int   key_blood_type;
		float key_time;
	};
#pragma endregion

#pragma region Members
private: int mProductIndex;
private: array<ConfigProductDefinition^> ^mProducts;
private: System::Windows::Forms::TextBox^  mDuration1Textfield;
private: System::Windows::Forms::TextBox^  mIndex1Textfield;
private: System::Windows::Forms::GroupBox^  mProductsGroupbox;
private: System::Windows::Forms::Label^  mDurationLabel;
private: System::Windows::Forms::Label^  mRBCLabel;
private: System::Windows::Forms::Label^  mPlasmaLabel;
private: System::Windows::Forms::TextBox^  mIndex3Textfield;
private: System::Windows::Forms::TextBox^  mDuration3Textfield;
private: System::Windows::Forms::TextBox^  mIndex2Textfield;
private: System::Windows::Forms::TextBox^  mDuration2Textfield;
private: System::Windows::Forms::Button^  mDownButton;
private: System::Windows::Forms::Label^  mPlateletsLabel;
private: System::Windows::Forms::ComboBox^  mPlatelets3List;
private: System::Windows::Forms::ComboBox^  mPlatelets2List;
private: System::Windows::Forms::ComboBox^  mPlatelets1List;
private: System::Windows::Forms::ComboBox^  mPlasma1List;
private: System::Windows::Forms::ComboBox^  mPlasma2List;
private: System::Windows::Forms::ComboBox^  mPlasma3List;
private: System::Windows::Forms::ComboBox^  mRBC2List;
private: System::Windows::Forms::ComboBox^  mRBC3List;
private: System::Windows::Forms::ComboBox^  mRBC1List;
private: System::Windows::Forms::ComboBox^  mType2List;
private: System::Windows::Forms::ComboBox^  mType3List;
private: System::Windows::Forms::ComboBox^  mType1List;
private: System::Windows::Forms::Label^  mTypeLabel;
private: System::Windows::Forms::Button^  mUpButton;
private: System::Windows::Forms::Button^  mClearButton;
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
		this->mDuration1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mIndex1Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mProductsGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mType2List = (gcnew System::Windows::Forms::ComboBox());
		this->mType3List = (gcnew System::Windows::Forms::ComboBox());
		this->mType1List = (gcnew System::Windows::Forms::ComboBox());
		this->mTypeLabel = (gcnew System::Windows::Forms::Label());
		this->mPlateletsLabel = (gcnew System::Windows::Forms::Label());
		this->mPlatelets3List = (gcnew System::Windows::Forms::ComboBox());
		this->mPlatelets2List = (gcnew System::Windows::Forms::ComboBox());
		this->mPlatelets1List = (gcnew System::Windows::Forms::ComboBox());
		this->mPlasma1List = (gcnew System::Windows::Forms::ComboBox());
		this->mPlasma2List = (gcnew System::Windows::Forms::ComboBox());
		this->mPlasma3List = (gcnew System::Windows::Forms::ComboBox());
		this->mRBC2List = (gcnew System::Windows::Forms::ComboBox());
		this->mRBC3List = (gcnew System::Windows::Forms::ComboBox());
		this->mRBC1List = (gcnew System::Windows::Forms::ComboBox());
		this->mDurationLabel = (gcnew System::Windows::Forms::Label());
		this->mRBCLabel = (gcnew System::Windows::Forms::Label());
		this->mPlasmaLabel = (gcnew System::Windows::Forms::Label());
		this->mDownButton = (gcnew System::Windows::Forms::Button());
		this->mIndex3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mDuration3Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mIndex2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mDuration2Textfield = (gcnew System::Windows::Forms::TextBox());
		this->mUpButton = (gcnew System::Windows::Forms::Button());
		this->mClearButton = (gcnew System::Windows::Forms::Button());
		this->mProductsGroupbox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mDuration1Textfield
		// 
		this->mDuration1Textfield->Location = System::Drawing::Point(482, 49);
		this->mDuration1Textfield->Name = L"mDuration1Textfield";
		this->mDuration1Textfield->Size = System::Drawing::Size(70, 20);
		this->mDuration1Textfield->TabIndex = 2;
		this->mDuration1Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
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
		// mProductsGroupbox
		// 
		this->mProductsGroupbox->Controls->Add(this->mType2List);
		this->mProductsGroupbox->Controls->Add(this->mType3List);
		this->mProductsGroupbox->Controls->Add(this->mType1List);
		this->mProductsGroupbox->Controls->Add(this->mTypeLabel);
		this->mProductsGroupbox->Controls->Add(this->mPlateletsLabel);
		this->mProductsGroupbox->Controls->Add(this->mPlatelets3List);
		this->mProductsGroupbox->Controls->Add(this->mPlatelets2List);
		this->mProductsGroupbox->Controls->Add(this->mPlatelets1List);
		this->mProductsGroupbox->Controls->Add(this->mPlasma1List);
		this->mProductsGroupbox->Controls->Add(this->mPlasma2List);
		this->mProductsGroupbox->Controls->Add(this->mPlasma3List);
		this->mProductsGroupbox->Controls->Add(this->mRBC2List);
		this->mProductsGroupbox->Controls->Add(this->mRBC3List);
		this->mProductsGroupbox->Controls->Add(this->mRBC1List);
		this->mProductsGroupbox->Controls->Add(this->mDurationLabel);
		this->mProductsGroupbox->Controls->Add(this->mRBCLabel);
		this->mProductsGroupbox->Controls->Add(this->mPlasmaLabel);
		this->mProductsGroupbox->Controls->Add(this->mDownButton);
		this->mProductsGroupbox->Controls->Add(this->mIndex3Textfield);
		this->mProductsGroupbox->Controls->Add(this->mDuration3Textfield);
		this->mProductsGroupbox->Controls->Add(this->mIndex2Textfield);
		this->mProductsGroupbox->Controls->Add(this->mDuration2Textfield);
		this->mProductsGroupbox->Controls->Add(this->mUpButton);
		this->mProductsGroupbox->Controls->Add(this->mIndex1Textfield);
		this->mProductsGroupbox->Controls->Add(this->mDuration1Textfield);
		this->mProductsGroupbox->Location = System::Drawing::Point(4, 15);
		this->mProductsGroupbox->Name = L"mProductsGroupbox";
		this->mProductsGroupbox->Size = System::Drawing::Size(572, 167);
		this->mProductsGroupbox->TabIndex = 5;
		this->mProductsGroupbox->TabStop = false;
		this->mProductsGroupbox->Text = L"Products";
		// 
		// mType2List
		// 
		this->mType2List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mType2List->FormattingEnabled = true;
		this->mType2List->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"All", L"O+/O-/A+/A-", L"O+/O-", L"O-", L"O-/A-", 
			L"A-/A+", L"B-/B+", L"AB-/AB+"});
		this->mType2List->Location = System::Drawing::Point(376, 74);
		this->mType2List->Name = L"mType2List";
		this->mType2List->Size = System::Drawing::Size(100, 21);
		this->mType2List->TabIndex = 32;
		// 
		// mType3List
		// 
		this->mType3List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mType3List->FormattingEnabled = true;
		this->mType3List->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"All", L"O+/O-/A+/A-", L"O+/O-", L"O-", L"O-/A-", 
			L"A-/A+", L"B-/B+", L"AB-/AB+"});
		this->mType3List->Location = System::Drawing::Point(376, 99);
		this->mType3List->Name = L"mType3List";
		this->mType3List->Size = System::Drawing::Size(100, 21);
		this->mType3List->TabIndex = 31;
		// 
		// mType1List
		// 
		this->mType1List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mType1List->FormattingEnabled = true;
		this->mType1List->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"All", L"O+/O-/A+/A-", L"O+/O-", L"O-", L"O-/A-", 
			L"A-/A+", L"B-/B+", L"AB-/AB+"});
		this->mType1List->Location = System::Drawing::Point(376, 48);
		this->mType1List->Name = L"mType1List";
		this->mType1List->Size = System::Drawing::Size(100, 21);
		this->mType1List->TabIndex = 30;
		// 
		// mTypeLabel
		// 
		this->mTypeLabel->AutoSize = true;
		this->mTypeLabel->Location = System::Drawing::Point(373, 32);
		this->mTypeLabel->Name = L"mTypeLabel";
		this->mTypeLabel->Size = System::Drawing::Size(31, 13);
		this->mTypeLabel->TabIndex = 29;
		this->mTypeLabel->Text = L"Type";
		// 
		// mPlateletsLabel
		// 
		this->mPlateletsLabel->AutoSize = true;
		this->mPlateletsLabel->Location = System::Drawing::Point(58, 32);
		this->mPlateletsLabel->Name = L"mPlateletsLabel";
		this->mPlateletsLabel->Size = System::Drawing::Size(47, 13);
		this->mPlateletsLabel->TabIndex = 28;
		this->mPlateletsLabel->Text = L"Platelets";
		// 
		// mPlatelets3List
		// 
		this->mPlatelets3List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mPlatelets3List->FormattingEnabled = true;
		this->mPlatelets3List->Items->AddRange(gcnew cli::array< System::Object^  >(11) {L"No Platelets", L"PLT 1", L"PLT 2", L"PLT 3", 
			L"PLT 4", L"PLT 5", L"PLT 6", L"PLT 7", L"PLT 8", L"PLT 9", L"PLT 10"});
		this->mPlatelets3List->Location = System::Drawing::Point(58, 99);
		this->mPlatelets3List->Name = L"mPlatelets3List";
		this->mPlatelets3List->Size = System::Drawing::Size(100, 21);
		this->mPlatelets3List->TabIndex = 27;
		// 
		// mPlatelets2List
		// 
		this->mPlatelets2List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mPlatelets2List->FormattingEnabled = true;
		this->mPlatelets2List->Items->AddRange(gcnew cli::array< System::Object^  >(11) {L"No Platelets", L"PLT 1", L"PLT 2", L"PLT 3", 
			L"PLT 4", L"PLT 5", L"PLT 6", L"PLT 7", L"PLT 8", L"PLT 9", L"PLT 10"});
		this->mPlatelets2List->Location = System::Drawing::Point(58, 73);
		this->mPlatelets2List->Name = L"mPlatelets2List";
		this->mPlatelets2List->Size = System::Drawing::Size(100, 21);
		this->mPlatelets2List->TabIndex = 26;
		// 
		// mPlatelets1List
		// 
		this->mPlatelets1List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mPlatelets1List->FormattingEnabled = true;
		this->mPlatelets1List->Items->AddRange(gcnew cli::array< System::Object^  >(11) {L"No Platelets", L"PLT 1", L"PLT 2", L"PLT 3", 
			L"PLT 4", L"PLT 5", L"PLT 6", L"PLT 7", L"PLT 8", L"PLT 9", L"PLT 10"});
		this->mPlatelets1List->Location = System::Drawing::Point(58, 48);
		this->mPlatelets1List->Name = L"mPlatelets1List";
		this->mPlatelets1List->Size = System::Drawing::Size(100, 21);
		this->mPlatelets1List->TabIndex = 25;
		// 
		// mPlasma1List
		// 
		this->mPlasma1List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mPlasma1List->FormattingEnabled = true;
		this->mPlasma1List->Items->AddRange(gcnew cli::array< System::Object^  >(9) {L"No Plasma", L"Plasma 1", L"Plasma 2", L"Plasma 3", 
			L"Plasma 4", L"Plasma 5", L"Plasma 6", L"As Much as Possible", L"Maximum Concurrent"});
		this->mPlasma1List->Location = System::Drawing::Point(164, 48);
		this->mPlasma1List->Name = L"mPlasma1List";
		this->mPlasma1List->Size = System::Drawing::Size(100, 21);
		this->mPlasma1List->TabIndex = 24;
		// 
		// mPlasma2List
		// 
		this->mPlasma2List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mPlasma2List->FormattingEnabled = true;
		this->mPlasma2List->Items->AddRange(gcnew cli::array< System::Object^  >(9) {L"No Plasma", L"Plasma 1", L"Plasma 2", L"Plasma 3", 
			L"Plasma 4", L"Plasma 5", L"Plasma 6", L"As Much as Possible", L"Maximum Concurrent"});
		this->mPlasma2List->Location = System::Drawing::Point(164, 73);
		this->mPlasma2List->Name = L"mPlasma2List";
		this->mPlasma2List->Size = System::Drawing::Size(100, 21);
		this->mPlasma2List->TabIndex = 23;
		// 
		// mPlasma3List
		// 
		this->mPlasma3List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mPlasma3List->FormattingEnabled = true;
		this->mPlasma3List->Items->AddRange(gcnew cli::array< System::Object^  >(9) {L"No Plasma", L"Plasma 1", L"Plasma 2", L"Plasma 3", 
			L"Plasma 4", L"Plasma 5", L"Plasma 6", L"As Much as Possible", L"Maximum Concurrent"});
		this->mPlasma3List->Location = System::Drawing::Point(164, 99);
		this->mPlasma3List->Name = L"mPlasma3List";
		this->mPlasma3List->Size = System::Drawing::Size(100, 21);
		this->mPlasma3List->TabIndex = 22;
		// 
		// mRBC2List
		// 
		this->mRBC2List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mRBC2List->FormattingEnabled = true;
		this->mRBC2List->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"No RBCs", L"RBC 1", L"RBC 2", L"RBC 3"});
		this->mRBC2List->Location = System::Drawing::Point(270, 74);
		this->mRBC2List->Name = L"mRBC2List";
		this->mRBC2List->Size = System::Drawing::Size(100, 21);
		this->mRBC2List->TabIndex = 21;
		// 
		// mRBC3List
		// 
		this->mRBC3List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mRBC3List->FormattingEnabled = true;
		this->mRBC3List->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"No RBCs", L"RBC 1", L"RBC 2", L"RBC 3"});
		this->mRBC3List->Location = System::Drawing::Point(270, 99);
		this->mRBC3List->Name = L"mRBC3List";
		this->mRBC3List->Size = System::Drawing::Size(100, 21);
		this->mRBC3List->TabIndex = 20;
		// 
		// mRBC1List
		// 
		this->mRBC1List->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mRBC1List->FormattingEnabled = true;
		this->mRBC1List->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"No RBCs", L"RBC 1", L"RBC 2", L"RBC 3"});
		this->mRBC1List->Location = System::Drawing::Point(270, 48);
		this->mRBC1List->Name = L"mRBC1List";
		this->mRBC1List->Size = System::Drawing::Size(100, 21);
		this->mRBC1List->TabIndex = 19;
		// 
		// mDurationLabel
		// 
		this->mDurationLabel->AutoSize = true;
		this->mDurationLabel->Location = System::Drawing::Point(479, 32);
		this->mDurationLabel->Name = L"mDurationLabel";
		this->mDurationLabel->Size = System::Drawing::Size(47, 13);
		this->mDurationLabel->TabIndex = 18;
		this->mDurationLabel->Text = L"Duration";
		// 
		// mRBCLabel
		// 
		this->mRBCLabel->AutoSize = true;
		this->mRBCLabel->Location = System::Drawing::Point(267, 32);
		this->mRBCLabel->Name = L"mRBCLabel";
		this->mRBCLabel->Size = System::Drawing::Size(29, 13);
		this->mRBCLabel->TabIndex = 17;
		this->mRBCLabel->Text = L"RBC";
		// 
		// mPlasmaLabel
		// 
		this->mPlasmaLabel->AutoSize = true;
		this->mPlasmaLabel->Location = System::Drawing::Point(164, 32);
		this->mPlasmaLabel->Name = L"mPlasmaLabel";
		this->mPlasmaLabel->Size = System::Drawing::Size(41, 13);
		this->mPlasmaLabel->TabIndex = 16;
		this->mPlasmaLabel->Text = L"Plasma";
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
		this->mDownButton->Click += gcnew System::EventHandler(this, &ConfigProcedurePriority::DownButtonHandler);
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
		// mDuration3Textfield
		// 
		this->mDuration3Textfield->Location = System::Drawing::Point(482, 101);
		this->mDuration3Textfield->Name = L"mDuration3Textfield";
		this->mDuration3Textfield->Size = System::Drawing::Size(70, 20);
		this->mDuration3Textfield->TabIndex = 12;
		this->mDuration3Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
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
		// mDuration2Textfield
		// 
		this->mDuration2Textfield->Location = System::Drawing::Point(482, 75);
		this->mDuration2Textfield->Name = L"mDuration2Textfield";
		this->mDuration2Textfield->Size = System::Drawing::Size(70, 20);
		this->mDuration2Textfield->TabIndex = 7;
		this->mDuration2Textfield->KeyPress += gcnew KeyPressEventHandler(&ConfigUtil::KeyPressFloatOnly);
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
		this->mUpButton->Click += gcnew System::EventHandler(this, &ConfigProcedurePriority::UpButtonHandler);
		// 
		// mClearButton
		// 
		this->mClearButton->Location = System::Drawing::Point(476, 188);
		this->mClearButton->Name = L"mClearButton";
		this->mClearButton->Size = System::Drawing::Size(100, 23);
		this->mClearButton->TabIndex = 6;
		this->mClearButton->Text = L"Clear All Products";
		this->mClearButton->UseVisualStyleBackColor = true;
		this->mClearButton->Click += gcnew System::EventHandler(this, &ConfigProcedurePriority::ClearButtonHandler);
		// 
		// ConfigProcedurePriority
		// 
		this->Controls->Add(this->mClearButton);
		this->Controls->Add(this->mProductsGroupbox);
		this->Name = L"ConfigProcedurePriority";
		this->Size = System::Drawing::Size(599, 300);
		this->mProductsGroupbox->ResumeLayout(false);
		this->mProductsGroupbox->PerformLayout();
		this->ResumeLayout(false);
	}
#pragma endregion
#pragma endregion
};
}
