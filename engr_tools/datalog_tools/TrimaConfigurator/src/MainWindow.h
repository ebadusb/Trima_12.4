/*****************************************************************************\
**
** MainWindow.h
**
** Configurator main window.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "TrimaConfigCDS.h"
#include "ConfigLanguageUnit.h"
#include "ConfigProcedure.h"
#include "ConfigAutomatedProcesses.h"
#include "ConfigPlatelets.h"
#include "ConfigPlasma.h"
#include "ConfigRedBloodCells.h"
#include "ConfigDonor.h"
#include "ConfigDonorVolumeLimits.h"
#include "ConfigReplacementFluid.h"
#include "ConfigProcedurePriority.h"

namespace CONFIGURATOR {

/* Classes *******************************************************************/

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;

public ref class MainWindow : public System::Windows::Forms::Form {
#pragma region Types
public:
	typedef Dictionary<String^, ConfigPage^> ConfigPageDict;
#pragma endregion

#pragma region Construction
public:
	MainWindow(void)
	{
		typedef Dictionary<String^, ConfigPage^> ConfigPageDict;

		this->InitializeComponent();

		mOpenFileDialog = gcnew OpenFileDialog();
		mSaveFileDialog	= gcnew SaveFileDialog();
		mImportFileDialog = gcnew OpenFileDialog();

		// Set up dialogs.
		mOpenFileDialog->Title = "Open a Config File";
		mOpenFileDialog->Filter = "Config Files|*.dat";
		mSaveFileDialog->Title = "Save a Config File";
		mSaveFileDialog->Filter = "Config Files|*.dat";
		mImportFileDialog->Title = "Import a Datalog File";
		mImportFileDialog->Filter = "Datalog Files|*.dlog";
		
		mConfigurationMap.Add("Units & Formats", gcnew ConfigLanguageUnit());
		mConfigurationMap.Add("Procedure", gcnew ConfigProcedure());
		mConfigurationMap.Add("Automated Processes", gcnew ConfigAutomatedProcesses());

		mConfigurationMap.Add("Platelets", gcnew ConfigPlatelets());
		mConfigurationMap.Add("Plasma", gcnew ConfigPlasma());
		mConfigurationMap.Add("Red Blood Cells", gcnew ConfigRedBloodCells());

		mConfigurationMap.Add("Donor", gcnew ConfigDonor());
		mConfigurationMap.Add("Donor Volume Limits", gcnew ConfigDonorVolumeLimits());
		mConfigurationMap.Add("Replacement Fluid", gcnew ConfigReplacementFluid());
		mConfigurationMap.Add("Procedure Priority", gcnew ConfigProcedurePriority());
		
		// Load the pages
		for (ConfigPageDict::Enumerator e = mConfigurationMap.GetEnumerator(); e.MoveNext();) {
			TabPage ^page = gcnew TabPage(e.Current.Key);

			page->Name = e.Current.Key;
			page->Controls->Add(e.Current.Value->GetControl());

			mConfigurationDroplist->Items->Add(e.Current.Key);
			mTabControl->TabPages->Add(page);
		}

		mTabControl->SelectedIndex = 0;
		mConfigurationDroplist->SelectedIndex = 0;
	}

protected:
	~MainWindow(void)
	{
		if (components) {
			delete components;
		}
	}
#pragma endregion

#pragma region Handlers
private: 
	System::Void ExitHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		Application::Exit();
	}

	System::Void OpenHandler(System::Object^  sender, System::EventArgs^  e)
	{
		if (mOpenFileDialog->ShowDialog() == Windows::Forms::DialogResult::OK) {
			CONFIG::TrimaConfigCDS cds;

			System::IntPtr intPtr = Marshal::StringToHGlobalAnsi(mOpenFileDialog->FileName);
			const char *filename = static_cast<const char*>(intPtr.ToPointer());

			if (cds.ReadConfig(filename)) {
				ConfigStruct config = cds.GetConfig();
				ConfigPageDict::Enumerator e = mConfigurationMap.GetEnumerator();

				while (e.MoveNext()) {
					e.Current.Value->UpdateConfigurations(config);
				}
			}

			Marshal::FreeHGlobal(intPtr);
		}
	}

	System::Void SaveHandler(System::Object^  sender, System::EventArgs^  e) 
	{
		ConfigPageDict::Enumerator enumerator = mConfigurationMap.GetEnumerator();

		while (enumerator.MoveNext()) {
			KeyValuePair<String^, ConfigPage^> ^data = enumerator.Current;
			ValidationInfo ^results = data->Value->ValidateConfigurations();
			int index = mConfigurationDroplist->Items->IndexOf(data->Key);

			if (!results->result) {
				mConfigurationDroplist->SelectedIndex = index;
				results->control->Focus();
				MessageBox::Show(results->message, 
						"Configuration Error", 
						MessageBoxButtons::OK, 
						MessageBoxIcon::Error);
				return;
			}
		}

		if (mSaveFileDialog->ShowDialog() == Windows::Forms::DialogResult::OK) {
			System::IntPtr intPtr = Marshal::StringToHGlobalAnsi(mSaveFileDialog->FileName);
			const char *filename = static_cast<const char*>(intPtr.ToPointer());
			CONFIG::TrimaConfigCDS cds;
			ConfigStruct config = cds.GetConfig();
			ConfigPageDict::Enumerator enumerator = mConfigurationMap.GetEnumerator();

			while (enumerator.MoveNext()) {
				KeyValuePair<String^, ConfigPage^> ^data = enumerator.Current;
	
				data->Value->RefreshConfigurations(config);
			}

			cds.SetConfig(config);
			cds.WriteConfig(filename);
			Marshal::FreeHGlobal(intPtr);
		}
	}

	System::Void ImportHandler(System::Object^  sender, System::EventArgs^  e)
	{
		if (mImportFileDialog->ShowDialog() == Windows::Forms::DialogResult::OK) {
			CONFIG::TrimaConfigCDS cds;

			System::IntPtr intPtr = Marshal::StringToHGlobalAnsi(mImportFileDialog->FileName);
			const char *filename = static_cast<const char*>(intPtr.ToPointer());

			if (CONFIG::TrimaConfigCDSUtil::LoadCDSFromDatalog(filename, cds)) {
				ConfigStruct config = cds.GetConfig();
				ConfigPageDict::Enumerator e = mConfigurationMap.GetEnumerator();

				while (e.MoveNext()) {
					e.Current.Value->UpdateConfigurations(config);
				}
			}

			Marshal::FreeHGlobal(intPtr);
		}
	}

	System::Void ValidateHandler(System::Object^  sender, System::EventArgs^  e)
	{
		String ^text = mTabControl->SelectedTab->Text;
		ConfigPage ^page = nullptr;

		if (mConfigurationMap.TryGetValue(text, page)) {
			ValidationInfo ^results = page->ValidateConfigurations();

			if (!results->result) {
				mConfigurationDroplist->SelectedIndex = mTabControl->SelectedIndex;
				results->control->Focus();
				MessageBox::Show(results->message, 
						"Configuration Error", 
						MessageBoxButtons::OK, 
						MessageBoxIcon::Error);
			}
		}
	}

	System::Void SelectionHandler(System::Object^  sender, System::EventArgs^  e)
	{
		mTabControl->SelectTab(mConfigurationDroplist->SelectedItem->ToString());
	}
#pragma endregion

#pragma region Members
private: System::Collections::Generic::Dictionary<String^, ConfigPage^> mConfigurationMap;
private: System::Windows::Forms::MenuStrip^  mMenuStrip;
private: System::Windows::Forms::ToolStripMenuItem^  mFileMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  mOpenMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  mSaveFMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  mSeparatorMenuItem1;
private: System::Windows::Forms::ToolStripMenuItem^  mExitMenuItem;
private: System::Windows::Forms::StatusStrip^  mStatusStrip;
private: System::Windows::Forms::TabControl^  mTabControl;
private: System::Windows::Forms::OpenFileDialog ^mOpenFileDialog;
private: System::Windows::Forms::SaveFileDialog ^mSaveFileDialog;
private: System::Windows::Forms::OpenFileDialog ^mImportFileDialog;
private: System::Windows::Forms::ToolStrip^  mToolStrip;
private: System::Windows::Forms::ToolStripButton^  mOpenButton;
private: System::Windows::Forms::ToolStripButton^  mSaveButton;
private: System::Windows::Forms::ToolStripSeparator^  mToolStripSeparator;
private: System::Windows::Forms::ToolStripLabel^  mConfigurationLabel;
private: System::Windows::Forms::ToolStripComboBox^  mConfigurationDroplist;
private: System::Windows::Forms::ToolStripMenuItem^  mImportMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  mSeparatorMenuItem2;
private: System::Windows::Forms::ToolStripButton^  toolStripButton1;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
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
		System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainWindow::typeid));
		this->mMenuStrip = (gcnew System::Windows::Forms::MenuStrip());
		this->mFileMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mOpenMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mSaveFMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mSeparatorMenuItem1 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->mImportMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mSeparatorMenuItem2 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->mExitMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mStatusStrip = (gcnew System::Windows::Forms::StatusStrip());
		this->mTabControl = (gcnew System::Windows::Forms::TabControl());
		this->mToolStrip = (gcnew System::Windows::Forms::ToolStrip());
		this->mOpenButton = (gcnew System::Windows::Forms::ToolStripButton());
		this->mSaveButton = (gcnew System::Windows::Forms::ToolStripButton());
		this->mToolStripSeparator = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->toolStripButton1 = (gcnew System::Windows::Forms::ToolStripButton());
		this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->mConfigurationLabel = (gcnew System::Windows::Forms::ToolStripLabel());
		this->mConfigurationDroplist = (gcnew System::Windows::Forms::ToolStripComboBox());
		this->mMenuStrip->SuspendLayout();
		this->mToolStrip->SuspendLayout();
		this->SuspendLayout();
		// 
		// mMenuStrip
		// 
		this->mMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->mFileMenuItem});
		this->mMenuStrip->Location = System::Drawing::Point(0, 0);
		this->mMenuStrip->Name = L"mMenuStrip";
		this->mMenuStrip->Size = System::Drawing::Size(590, 24);
		this->mMenuStrip->TabIndex = 0;
		this->mMenuStrip->Text = L"menuStrip1";
		// 
		// mFileMenuItem
		// 
		this->mFileMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->mOpenMenuItem, 
			this->mSaveFMenuItem, this->mSeparatorMenuItem1, this->mImportMenuItem, this->mSeparatorMenuItem2, this->mExitMenuItem});
		this->mFileMenuItem->Name = L"mFileMenuItem";
		this->mFileMenuItem->Size = System::Drawing::Size(35, 20);
		this->mFileMenuItem->Text = L"File";
		// 
		// mOpenMenuItem
		// 
		this->mOpenMenuItem->Name = L"mOpenMenuItem";
		this->mOpenMenuItem->Size = System::Drawing::Size(129, 22);
		this->mOpenMenuItem->Text = L"Open...";
		this->mOpenMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::OpenHandler);
		// 
		// mSaveFMenuItem
		// 
		this->mSaveFMenuItem->Name = L"mSaveFMenuItem";
		this->mSaveFMenuItem->Size = System::Drawing::Size(129, 22);
		this->mSaveFMenuItem->Text = L"Save...";
		this->mSaveFMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::SaveHandler);
		// 
		// mSeparatorMenuItem1
		// 
		this->mSeparatorMenuItem1->Name = L"mSeparatorMenuItem1";
		this->mSeparatorMenuItem1->Size = System::Drawing::Size(126, 6);
		// 
		// mImportMenuItem
		// 
		this->mImportMenuItem->Name = L"mImportMenuItem";
		this->mImportMenuItem->Size = System::Drawing::Size(129, 22);
		this->mImportMenuItem->Text = L"Import...";
		this->mImportMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::ImportHandler);
		// 
		// mSeparatorMenuItem2
		// 
		this->mSeparatorMenuItem2->Name = L"mSeparatorMenuItem2";
		this->mSeparatorMenuItem2->Size = System::Drawing::Size(126, 6);
		// 
		// mExitMenuItem
		// 
		this->mExitMenuItem->Name = L"mExitMenuItem";
		this->mExitMenuItem->Size = System::Drawing::Size(129, 22);
		this->mExitMenuItem->Text = L"Exit";
		this->mExitMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::ExitHandler);
		// 
		// mStatusStrip
		// 
		this->mStatusStrip->Location = System::Drawing::Point(0, 342);
		this->mStatusStrip->Name = L"mStatusStrip";
		this->mStatusStrip->Size = System::Drawing::Size(590, 22);
		this->mStatusStrip->TabIndex = 1;
		this->mStatusStrip->Text = L"statusStrip1";
		// 
		// mTabControl
		// 
		this->mTabControl->Dock = System::Windows::Forms::DockStyle::Fill;
		this->mTabControl->Location = System::Drawing::Point(0, 24);
		this->mTabControl->Name = L"mTabControl";
		this->mTabControl->SelectedIndex = 0;
		this->mTabControl->Size = System::Drawing::Size(590, 318);
		this->mTabControl->TabIndex = 3;
		this->mTabControl->TabStop = false;
		// 
		// mToolStrip
		// 
		this->mToolStrip->BackColor = System::Drawing::Color::Silver;
		this->mToolStrip->GripStyle = System::Windows::Forms::ToolStripGripStyle::Hidden;
		this->mToolStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {this->mOpenButton, this->mSaveButton, 
			this->mToolStripSeparator, this->toolStripButton1, this->toolStripSeparator1, this->mConfigurationLabel, this->mConfigurationDroplist});
		this->mToolStrip->Location = System::Drawing::Point(0, 24);
		this->mToolStrip->Name = L"mToolStrip";
		this->mToolStrip->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
		this->mToolStrip->Size = System::Drawing::Size(590, 25);
		this->mToolStrip->TabIndex = 4;
		this->mToolStrip->Text = L"toolStrip1";
		// 
		// mOpenButton
		// 
		this->mOpenButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->mOpenButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"mOpenButton.Image")));
		this->mOpenButton->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->mOpenButton->Name = L"mOpenButton";
		this->mOpenButton->Size = System::Drawing::Size(23, 22);
		this->mOpenButton->Text = L"Open...";
		this->mOpenButton->Click += gcnew System::EventHandler(this, &MainWindow::OpenHandler);
		// 
		// mSaveButton
		// 
		this->mSaveButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->mSaveButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"mSaveButton.Image")));
		this->mSaveButton->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->mSaveButton->Name = L"mSaveButton";
		this->mSaveButton->Size = System::Drawing::Size(23, 22);
		this->mSaveButton->Text = L"Save...";
		this->mSaveButton->Click += gcnew System::EventHandler(this, &MainWindow::SaveHandler);
		// 
		// mToolStripSeparator
		// 
		this->mToolStripSeparator->BackColor = System::Drawing::Color::Black;
		this->mToolStripSeparator->ForeColor = System::Drawing::Color::Black;
		this->mToolStripSeparator->Name = L"mToolStripSeparator";
		this->mToolStripSeparator->Size = System::Drawing::Size(6, 25);
		// 
		// toolStripButton1
		// 
		this->toolStripButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->toolStripButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton1.Image")));
		this->toolStripButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->toolStripButton1->Name = L"toolStripButton1";
		this->toolStripButton1->Size = System::Drawing::Size(23, 22);
		this->toolStripButton1->Text = L"Validate";
		this->toolStripButton1->Click += gcnew System::EventHandler(this, &MainWindow::ValidateHandler);
		// 
		// toolStripSeparator1
		// 
		this->toolStripSeparator1->BackColor = System::Drawing::Color::Black;
		this->toolStripSeparator1->ForeColor = System::Drawing::Color::Black;
		this->toolStripSeparator1->Name = L"toolStripSeparator1";
		this->toolStripSeparator1->Size = System::Drawing::Size(6, 25);
		// 
		// mConfigurationLabel
		// 
		this->mConfigurationLabel->Name = L"mConfigurationLabel";
		this->mConfigurationLabel->Size = System::Drawing::Size(77, 22);
		this->mConfigurationLabel->Text = L"Configurations";
		// 
		// mConfigurationDroplist
		// 
		this->mConfigurationDroplist->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
		this->mConfigurationDroplist->Name = L"mConfigurationDroplist";
		this->mConfigurationDroplist->Size = System::Drawing::Size(150, 25);
		this->mConfigurationDroplist->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::SelectionHandler);
		// 
		// MainWindow
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(590, 364);
		this->Controls->Add(this->mToolStrip);
		this->Controls->Add(this->mTabControl);
		this->Controls->Add(this->mStatusStrip);
		this->Controls->Add(this->mMenuStrip);
		this->MainMenuStrip = this->mMenuStrip;
		this->Name = L"MainWindow";
		this->Text = L"Trima Configurator";
		this->mMenuStrip->ResumeLayout(false);
		this->mMenuStrip->PerformLayout();
		this->mToolStrip->ResumeLayout(false);
		this->mToolStrip->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}
#pragma endregion
#pragma endregion
};
}

