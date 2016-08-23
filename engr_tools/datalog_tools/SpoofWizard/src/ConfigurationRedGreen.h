/*****************************************************************************\
**
** ConfigurationRedGreen.h
**
** Provides the implementation for red/green configurations.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "GeneratorRedGreen.h"
#include "GeneratorSystemStates.h"
#include "ConfigPage.h"

/* Class *********************************************************************/

namespace WIZARD {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class ConfigurationRedGreen : 
		public System::Windows::Forms::UserControl,
		public ConfigPage {
#pragma region Construction
public:
	ConfigurationRedGreen(BUILDER::GeneratorRedGreen *generatorRedGreen) : 
			mGeneratorRedGreenP(generatorRedGreen)
	{
		this->InitializeComponent();
	}

protected:
	~ConfigurationRedGreen()
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

		if (System::String::IsNullOrEmpty(mNoiseTextfield->Text->Trim())) {
			info->result = false;
			info->message = "Invalid noise factor.";
			info->control = mNoiseTextfield;
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
			mGeneratorRedGreenP->SetNoiseFactor(Int32::Parse(mNoiseTextfield->Text));
			mGeneratorRedGreenP->SetInterpolateState(mInterpolateCheckBox->Checked);
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
		typedef BUILDER::GeneratorRedGreen::RedGreenValues RedGreenValues;
		typedef BUILDER::TimeFrameMgr TimeFrameMgr;
		typedef BUILDER::TimeFrameMgr::TimeFrameList TimeFrameList;

		ValidationInfo ^info = gcnew ValidationInfo();

		info->result = true;
		info->message = nullptr;
		info->control = nullptr;

		mNoiseTextfield->Text = Convert::ToString(mGeneratorRedGreenP->GetNoiseFactor());
		mInterpolateCheckBox->Checked = mGeneratorRedGreenP->GetInterpolateState();

		const TimeFrameMgr *mgr = mGeneratorRedGreenP->GetTimeFrameMgr();
		TimeFrameList::const_iterator timeIter = mgr->GetTimeFrameListBegin();
		TimeFrameList::const_iterator timeEnd = mgr->GetTimeFrameListEnd();

		int time = mgr->GetFrameTime();
		int width = mGraphPictureBox->Size.Width;
		int height = mGraphPictureBox->Size.Height;
		int xScale = time / width > 0 ? time / width : 1;
		int yScale = 10;
		int index = 0;

		System::Drawing::Bitmap ^image = gcnew System::Drawing::Bitmap(width, height);
		System::Drawing::Graphics ^graphics = System::Drawing::Graphics::FromImage(image);
		System::Drawing::Pen ^red = System::Drawing::Pens::Red;
		System::Drawing::Pen ^green = System::Drawing::Pens::Green;

		for (; timeIter != timeEnd; ++timeIter) {
			RedGreenValues::const_iterator valueIter = mGeneratorRedGreenP->FindRedGreenValues((*timeIter).first);
			RedGreenValues::const_iterator valueEnd = mGeneratorRedGreenP->FindRedGreenValues((*timeIter).second);

			for (; valueIter != valueEnd; ++valueIter, ++index) {
				graphics->DrawEllipse(red, 
						index / xScale, 
						height - (*valueIter).second.red / yScale, 
						1, 1);
				graphics->DrawEllipse(green, 
						index / xScale, 
						height - (*valueIter).second.green / yScale,
						1, 1);
			}
		}

		graphics->Flush();
		mGraphPictureBox->Image = image;

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
	}
	System::Void TextHandler(System::Object^  sender, System::EventArgs^  e)
	{
		int value = 0;

		if (!String::IsNullOrEmpty(mNoiseTextfield->Text->Trim())) {
			value = Int32::Parse(mNoiseTextfield->Text);
		}

		mGeneratorRedGreenP->SetNoiseFactor(value);
		this->RefreshConfigurations();
	}
	System::Void CheckedHandler(System::Object^  sender, System::EventArgs^  e)
	{
		mGeneratorRedGreenP->SetInterpolateState(mInterpolateCheckBox->Checked);
		this->RefreshConfigurations();
	}
#pragma endregion

#pragma region Members
private: BUILDER::GeneratorRedGreen *mGeneratorRedGreenP;
private: System::Windows::Forms::Label^  mNoiseLabel;
private: System::Windows::Forms::TextBox^  mNoiseTextfield;
private: System::Windows::Forms::CheckBox^  mInterpolateCheckBox;
private: System::Windows::Forms::PictureBox^  mGraphPictureBox;
#pragma endregion

#pragma region Generated
private:
	System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
	void InitializeComponent(void)
	{
		this->mNoiseLabel = (gcnew System::Windows::Forms::Label());
		this->mNoiseTextfield = (gcnew System::Windows::Forms::TextBox());
		this->mInterpolateCheckBox = (gcnew System::Windows::Forms::CheckBox());
		this->mGraphPictureBox = (gcnew System::Windows::Forms::PictureBox());
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->mGraphPictureBox))->BeginInit();
		this->SuspendLayout();
		// 
		// mNoiseLabel
		// 
		this->mNoiseLabel->AutoSize = true;
		this->mNoiseLabel->Location = System::Drawing::Point(19, 189);
		this->mNoiseLabel->Name = L"mNoiseLabel";
		this->mNoiseLabel->Size = System::Drawing::Size(37, 13);
		this->mNoiseLabel->TabIndex = 2;
		this->mNoiseLabel->Text = L"Noise:";
		// 
		// mNoiseTextfield
		// 
		this->mNoiseTextfield->Location = System::Drawing::Point(62, 186);
		this->mNoiseTextfield->Name = L"mNoiseTextfield";
		this->mNoiseTextfield->Size = System::Drawing::Size(100, 20);
		this->mNoiseTextfield->TabIndex = 7;
		this->mNoiseTextfield->Text = L"0";
		this->mNoiseTextfield->TextChanged += gcnew System::EventHandler(this, &ConfigurationRedGreen::TextHandler);
		// 
		// mInterpolateCheckBox
		// 
		this->mInterpolateCheckBox->AutoSize = true;
		this->mInterpolateCheckBox->Location = System::Drawing::Point(168, 188);
		this->mInterpolateCheckBox->Name = L"mInterpolateCheckBox";
		this->mInterpolateCheckBox->Size = System::Drawing::Size(111, 17);
		this->mInterpolateCheckBox->TabIndex = 8;
		this->mInterpolateCheckBox->Text = L"Interpolate Values";
		this->mInterpolateCheckBox->UseVisualStyleBackColor = true;
		this->mInterpolateCheckBox->CheckStateChanged += gcnew System::EventHandler(this, &ConfigurationRedGreen::CheckedHandler);
		// 
		// mGraphPictureBox
		// 
		this->mGraphPictureBox->BackColor = System::Drawing::SystemColors::ControlLightLight;
		this->mGraphPictureBox->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
		this->mGraphPictureBox->Location = System::Drawing::Point(18, 16);
		this->mGraphPictureBox->Name = L"mGraphPictureBox";
		this->mGraphPictureBox->Size = System::Drawing::Size(550, 160);
		this->mGraphPictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
		this->mGraphPictureBox->TabIndex = 9;
		this->mGraphPictureBox->TabStop = false;
		// 
		// ConfigurationRedGreen
		// 
		this->AccessibleRole = System::Windows::Forms::AccessibleRole::PageTab;
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->Controls->Add(this->mGraphPictureBox);
		this->Controls->Add(this->mInterpolateCheckBox);
		this->Controls->Add(this->mNoiseTextfield);
		this->Controls->Add(this->mNoiseLabel);
		this->Name = L"ConfigurationRedGreen";
		this->Size = System::Drawing::Size(590, 340);
		(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->mGraphPictureBox))->EndInit();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};

} // namespace WIZARD
