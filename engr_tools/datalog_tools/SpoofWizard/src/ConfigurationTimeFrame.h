/*****************************************************************************\
**
** ConfigurationTimeFrame.h
**
** Provides the implementation for time frame configurations.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "TimeFrameMgr.h"

/* Class *********************************************************************/

namespace WIZARD {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class ConfigurationTimeFrame :
		public System::Windows::Forms::UserControl,
		public ConfigPage {
public:
#pragma region Construction
	ConfigurationTimeFrame(
			BUILDER::TimeFrameMgr *timeFrameMgr, 
			BUILDER::GeneratorSystemStates *generatorSystemStates,
			BUILDER::GeneratorAlarmStates *generatorAlarmStates,
			BUILDER::GeneratorRecoveryStates *generatorRecoveryStates) :
			mTimeFrameMgr(timeFrameMgr),
			mGeneratorSystemStatesP(generatorSystemStates),
			mGeneratorAlarmStatesP(generatorAlarmStates),
			mGeneratorRecoveryStatesP(generatorRecoveryStates)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigurationTimeFrame()
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

		info->result = true;
		info->message = nullptr;
		info->control = nullptr;

		if (System::String::IsNullOrEmpty(mBeginTimeTextfield->Text->Trim())) {
			info->result = false;
			info->message = "Invalid begin time.";
			info->control = mBeginTimeTextfield;
		}
		else if (System::String::IsNullOrEmpty(mEndTimeTextField->Text->Trim())) {
			info->result = false;
			info->message = "Invalid end time.";
			info->control = mEndTimeTextField;
		}

		return info; 
	}

	/**
	 *
	 * UpdateConfigurations
	 *
	 * Updates the configurations.
	 *
	 * @return True if the contigurations were updated.
	 *
	 */
	virtual ValidationInfo ^UpdateConfigurations(void) 
	{ 
		ValidationInfo ^info = this->ValidateConfigurations();

		if (info->result) {
			int beginTime = Int32::Parse(mBeginTimeTextfield->Text);
			int endTime = Int32::Parse(mEndTimeTextField->Text);
			
			mTimeFrameMgr->ExcludeAllTimeFrame();
			mTimeFrameMgr->IncludeTimeFrame(std::make_pair(beginTime, endTime));

			if (mAlarmCheckBox->Checked) {
				BUILDER::SpoofUtil::FilterAlarmStates(*mTimeFrameMgr, 
						mGeneratorAlarmStatesP->GetSystemStateValuesBegin(), 
						mGeneratorAlarmStatesP->GetSystemStateValuesEnd());
			}

			if (mRecoveryCheckBox->Checked) {
				BUILDER::SpoofUtil::FilterRecoveryStates(*mTimeFrameMgr, 
						mGeneratorRecoveryStatesP->GetSystemStateValuesBegin(), 
						mGeneratorRecoveryStatesP->GetSystemStateValuesEnd());
			}
		}
		
		return info; 	
	}

	/**
	 *
	 * RefreshConfigurations
	 *
	 * Refreshes the configurations.
	 *
	 * @return True if the contigurations were updated.
	 *
	 */
	virtual ValidationInfo ^RefreshConfigurations(void)
	{
		typedef BUILDER::GeneratorSystemStates::SystemStateValues SystemStateValues;

		ValidationInfo ^info = gcnew ValidationInfo();
		BUILDER::Uint32 beginTime = mTimeFrameMgr->GetBeginTime();
		BUILDER::Uint32 endTime = mTimeFrameMgr->GetEndTime();
		BUILDER::Uint32 totalTime = mTimeFrameMgr->GetFrameTime();
		BUILDER::String beginState = mGeneratorSystemStatesP->GetStateForTime(beginTime);
		BUILDER::String endState = mGeneratorSystemStatesP->GetStateForTime(endTime);

		info->result = true;
		info->message = nullptr;
		info->control = nullptr;

		SystemStateValues::const_iterator iter = mGeneratorSystemStatesP->GetSystemStateValuesBegin();
		SystemStateValues::const_iterator end = mGeneratorSystemStatesP->GetSystemStateValuesEnd();

		mBeginTimeTextfield->Text = Convert::ToString(beginTime);
		mEndTimeTextField->Text = Convert::ToString(endTime);
		mTimeLabel->Text = Convert::ToString(totalTime);

		mBeginTimeDroplist->Items->Clear();
		mEndTimeDroplist->Items->Clear();

		for (; iter != end; ++iter) {
			System::String ^text = gcnew System::String((*iter).second.c_str());

			mBeginTimeDroplist->Items->Add(text);
			mEndTimeDroplist->Items->Add(text);
		}

		mBeginTimeDroplist->SelectedItem = gcnew String(beginState.c_str(), 0, beginState.length());
		mEndTimeDroplist->SelectedItem = gcnew String(endState.c_str(), 0, endState.length());

		return info; 
	}
#pragma endregion

#pragma region Handlers
	System::Void KeyPressHandler(Object ^o, System::Windows::Forms::KeyPressEventArgs ^e)
	{
		if (!Char::IsNumber(e->KeyChar) 
				&& (e->KeyChar != 127) 
				&& (e->KeyChar != 010)) {
			e->Handled = true;
		}
		else if (o == mBeginTimeTextfield) {
			mBeginTimeDroplist->SelectedIndex = -1;
		}
		else if (o == mEndTimeTextField) {
			mEndTimeDroplist->SelectedIndex = -1;
		}
	}
	System::Void SelectionHandler(System::Object^  sender, System::EventArgs^  e)
	{
		if (sender == mBeginTimeDroplist && mBeginTimeDroplist->SelectedItem != nullptr) {
			System::String ^text = mBeginTimeDroplist->SelectedItem->ToString();
			System::IntPtr pointer = Marshal::StringToHGlobalAnsi(text);
			const char *buffer = static_cast<const char*>(pointer.ToPointer());
			BUILDER::Uint32 time = mGeneratorSystemStatesP->GetTimeForState(buffer);
			Marshal::FreeHGlobal(pointer);

			mBeginTimeTextfield->Text = Convert::ToString(time);
			this->UpdateConfigurations();
			mTimeLabel->Text = Convert::ToString(mTimeFrameMgr->GetFrameTime());
		}
		else if (sender == mEndTimeDroplist && mEndTimeDroplist->SelectedItem != nullptr) {
			System::String ^text = mEndTimeDroplist->SelectedItem->ToString();
			System::IntPtr pointer = Marshal::StringToHGlobalAnsi(text);
			const char *buffer = static_cast<const char*>(pointer.ToPointer());
			BUILDER::Uint32 time = mGeneratorSystemStatesP->GetTimeForState(buffer);
			Marshal::FreeHGlobal(pointer);

			mEndTimeTextField->Text = Convert::ToString(time);
			this->UpdateConfigurations();
			mTimeLabel->Text = Convert::ToString(mTimeFrameMgr->GetFrameTime());
		}
	}
	System::Void UpdateHandler(System::Object^  sender, System::EventArgs^  e)
	{
		this->UpdateConfigurations();
		mTimeLabel->Text = Convert::ToString(mTimeFrameMgr->GetFrameTime());
	}
#pragma endregion

#pragma region Members
	private: BUILDER::TimeFrameMgr *mTimeFrameMgr;
	private: BUILDER::GeneratorSystemStates *mGeneratorSystemStatesP;
	private: BUILDER::GeneratorAlarmStates *mGeneratorAlarmStatesP;
	private: BUILDER::GeneratorRecoveryStates *mGeneratorRecoveryStatesP;
	private: System::Windows::Forms::GroupBox^  mIntervalGroupbox;
	private: System::Windows::Forms::ComboBox^  mEndTimeDroplist;
	private: System::Windows::Forms::ComboBox^  mBeginTimeDroplist;
	private: System::Windows::Forms::TextBox^  mBeginTimeTextfield;
	private: System::Windows::Forms::Label^  mBeginTimeLabel;
	private: System::Windows::Forms::TextBox^  mEndTimeTextField;
	private: System::Windows::Forms::Label^  mEndTimeLabel;
	private: System::Windows::Forms::CheckBox^  mAlarmCheckBox;
	private: System::Windows::Forms::GroupBox^  mFilterGroupBox;
	private: System::Windows::Forms::CheckBox^  mRecoveryCheckBox;
	private: System::Windows::Forms::Label^  mTotalLabel;
	private: System::Windows::Forms::Label^  mTimeLabel;
#pragma endregion

#pragma region Generated
private:
	System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
	void InitializeComponent(void)
	{
		this->mIntervalGroupbox = (gcnew System::Windows::Forms::GroupBox());
		this->mEndTimeDroplist = (gcnew System::Windows::Forms::ComboBox());
		this->mBeginTimeDroplist = (gcnew System::Windows::Forms::ComboBox());
		this->mBeginTimeTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mBeginTimeLabel = (gcnew System::Windows::Forms::Label());
		this->mEndTimeTextField = (gcnew System::Windows::Forms::TextBox());
		this->mEndTimeLabel = (gcnew System::Windows::Forms::Label());
		this->mAlarmCheckBox = (gcnew System::Windows::Forms::CheckBox());
		this->mFilterGroupBox = (gcnew System::Windows::Forms::GroupBox());
		this->mRecoveryCheckBox = (gcnew System::Windows::Forms::CheckBox());
		this->mTotalLabel = (gcnew System::Windows::Forms::Label());
		this->mTimeLabel = (gcnew System::Windows::Forms::Label());
		this->mIntervalGroupbox->SuspendLayout();
		this->mFilterGroupBox->SuspendLayout();
		this->SuspendLayout();
		// 
		// mIntervalGroupbox
		// 
		this->mIntervalGroupbox->Controls->Add(this->mEndTimeDroplist);
		this->mIntervalGroupbox->Controls->Add(this->mBeginTimeDroplist);
		this->mIntervalGroupbox->Controls->Add(this->mBeginTimeTextfield);
		this->mIntervalGroupbox->Controls->Add(this->mBeginTimeLabel);
		this->mIntervalGroupbox->Controls->Add(this->mEndTimeTextField);
		this->mIntervalGroupbox->Controls->Add(this->mEndTimeLabel);
		this->mIntervalGroupbox->Location = System::Drawing::Point(19, 18);
		this->mIntervalGroupbox->Name = L"mIntervalGroupbox";
		this->mIntervalGroupbox->Size = System::Drawing::Size(547, 91);
		this->mIntervalGroupbox->TabIndex = 9;
		this->mIntervalGroupbox->TabStop = false;
		this->mIntervalGroupbox->Text = L"Interval";
		// 
		// mEndTimeDroplist
		// 
		this->mEndTimeDroplist->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mEndTimeDroplist->FormattingEnabled = true;
		this->mEndTimeDroplist->Location = System::Drawing::Point(196, 54);
		this->mEndTimeDroplist->Name = L"mEndTimeDroplist";
		this->mEndTimeDroplist->Size = System::Drawing::Size(150, 21);
		this->mEndTimeDroplist->TabIndex = 8;
		this->mEndTimeDroplist->SelectedIndexChanged += gcnew System::EventHandler(this, &ConfigurationTimeFrame::SelectionHandler);
		// 
		// mBeginTimeDroplist
		// 
		this->mBeginTimeDroplist->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mBeginTimeDroplist->FormattingEnabled = true;
		this->mBeginTimeDroplist->Location = System::Drawing::Point(196, 28);
		this->mBeginTimeDroplist->Name = L"mBeginTimeDroplist";
		this->mBeginTimeDroplist->Size = System::Drawing::Size(150, 21);
		this->mBeginTimeDroplist->TabIndex = 5;
		this->mBeginTimeDroplist->SelectedIndexChanged += gcnew System::EventHandler(this, &ConfigurationTimeFrame::SelectionHandler);
		// 
		// mBeginTimeTextfield
		// 
		this->mBeginTimeTextfield->Location = System::Drawing::Point(90, 28);
		this->mBeginTimeTextfield->Name = L"mBeginTimeTextfield";
		this->mBeginTimeTextfield->Size = System::Drawing::Size(100, 20);
		this->mBeginTimeTextfield->TabIndex = 3;
		this->mBeginTimeTextfield->Text = L"0";
		this->mBeginTimeTextfield->TextChanged += gcnew System::EventHandler(this, &ConfigurationTimeFrame::UpdateHandler);
		// 
		// mBeginTimeLabel
		// 
		this->mBeginTimeLabel->AutoSize = true;
		this->mBeginTimeLabel->Location = System::Drawing::Point(21, 31);
		this->mBeginTimeLabel->Name = L"mBeginTimeLabel";
		this->mBeginTimeLabel->Size = System::Drawing::Size(63, 13);
		this->mBeginTimeLabel->TabIndex = 0;
		this->mBeginTimeLabel->Text = L"Begin Time:";
		// 
		// mEndTimeTextField
		// 
		this->mEndTimeTextField->Location = System::Drawing::Point(90, 54);
		this->mEndTimeTextField->Name = L"mEndTimeTextField";
		this->mEndTimeTextField->Size = System::Drawing::Size(100, 20);
		this->mEndTimeTextField->TabIndex = 4;
		this->mEndTimeTextField->Text = L"0";
		this->mEndTimeTextField->TextChanged += gcnew System::EventHandler(this, &ConfigurationTimeFrame::UpdateHandler);
		// 
		// mEndTimeLabel
		// 
		this->mEndTimeLabel->AutoSize = true;
		this->mEndTimeLabel->Location = System::Drawing::Point(21, 57);
		this->mEndTimeLabel->Name = L"mEndTimeLabel";
		this->mEndTimeLabel->Size = System::Drawing::Size(55, 13);
		this->mEndTimeLabel->TabIndex = 1;
		this->mEndTimeLabel->Text = L"End Time:";
		// 
		// mAlarmCheckBox
		// 
		this->mAlarmCheckBox->AutoSize = true;
		this->mAlarmCheckBox->Location = System::Drawing::Point(24, 28);
		this->mAlarmCheckBox->Name = L"mAlarmCheckBox";
		this->mAlarmCheckBox->Size = System::Drawing::Size(110, 17);
		this->mAlarmCheckBox->TabIndex = 10;
		this->mAlarmCheckBox->Text = L"Filter Alarm States";
		this->mAlarmCheckBox->UseVisualStyleBackColor = true;
		this->mAlarmCheckBox->CheckStateChanged += gcnew System::EventHandler(this, &ConfigurationTimeFrame::UpdateHandler);
		// 
		// mFilterGroupBox
		// 
		this->mFilterGroupBox->Controls->Add(this->mRecoveryCheckBox);
		this->mFilterGroupBox->Controls->Add(this->mAlarmCheckBox);
		this->mFilterGroupBox->Location = System::Drawing::Point(19, 116);
		this->mFilterGroupBox->Name = L"mFilterGroupBox";
		this->mFilterGroupBox->Size = System::Drawing::Size(547, 85);
		this->mFilterGroupBox->TabIndex = 11;
		this->mFilterGroupBox->TabStop = false;
		this->mFilterGroupBox->Text = L"Filter";
		// 
		// mRecoveryCheckBox
		// 
		this->mRecoveryCheckBox->AutoSize = true;
		this->mRecoveryCheckBox->Location = System::Drawing::Point(24, 51);
		this->mRecoveryCheckBox->Name = L"mRecoveryCheckBox";
		this->mRecoveryCheckBox->Size = System::Drawing::Size(130, 17);
		this->mRecoveryCheckBox->TabIndex = 11;
		this->mRecoveryCheckBox->Text = L"Filter Recovery States";
		this->mRecoveryCheckBox->UseVisualStyleBackColor = true;
		this->mRecoveryCheckBox->CheckStateChanged += gcnew System::EventHandler(this, &ConfigurationTimeFrame::UpdateHandler);
		// 
		// mTotalLabel
		// 
		this->mTotalLabel->AutoSize = true;
		this->mTotalLabel->Location = System::Drawing::Point(16, 215);
		this->mTotalLabel->Name = L"mTotalLabel";
		this->mTotalLabel->Size = System::Drawing::Size(109, 13);
		this->mTotalLabel->TabIndex = 12;
		this->mTotalLabel->Text = L"Total Time (seconds):";
		// 
		// mTimeLabel
		// 
		this->mTimeLabel->AutoSize = true;
		this->mTimeLabel->Location = System::Drawing::Point(131, 215);
		this->mTimeLabel->Name = L"mTimeLabel";
		this->mTimeLabel->Size = System::Drawing::Size(13, 13);
		this->mTimeLabel->TabIndex = 13;
		this->mTimeLabel->Text = L"0";
		// 
		// ConfigurationTimeFrame
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->Controls->Add(this->mTimeLabel);
		this->Controls->Add(this->mTotalLabel);
		this->Controls->Add(this->mFilterGroupBox);
		this->Controls->Add(this->mIntervalGroupbox);
		this->Name = L"ConfigurationTimeFrame";
		this->Size = System::Drawing::Size(590, 340);
		this->mIntervalGroupbox->ResumeLayout(false);
		this->mIntervalGroupbox->PerformLayout();
		this->mFilterGroupBox->ResumeLayout(false);
		this->mFilterGroupBox->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};
}
