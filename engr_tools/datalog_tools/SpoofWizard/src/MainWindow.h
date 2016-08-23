/*****************************************************************************\
**
** ConfigurationRedGreen.h
**
** Provides the implementation for red/green configurations.
**
\*****************************************************************************/

#pragma once

/* Includes ******************************************************************/

#include "SpoofBuilder.h"
#include "ConfigPage.h"
#include "WizardConfig.h"
#include "SettingsDialog.h"
#include "AutoPtr.h"
#include "SharedPtr.h"

/* Class *********************************************************************/

namespace WIZARD {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

public ref class MainWindow : public System::Windows::Forms::Form {
#pragma region Construction
public: 
	MainWindow(void);

protected:
	~MainWindow(void);
#pragma endregion

#pragma region Types
typedef AutoPtr<BUILDER::SpoofBuilder> SpoofBuilderPtr;
typedef SharedPtr<BUILDER::ImportGenerator> ImportGeneratorPtr;
typedef SharedPtr<BUILDER::ExportGenerator> ExportGeneratorPtr;
#pragma endregion

#pragma region Functions
private: System::Void ReloadSettings(void);
#pragma endregion

#pragma region Handlers
private:
	System::Void ExitHandler(System::Object^  sender, System::EventArgs^  e);
	System::Void ImportHandler(System::Object^  sender, System::EventArgs^  e);
	System::Void ExportHandler(System::Object^  sender, System::EventArgs^  e);
	System::Void LoadHandler(System::Object^  sender, System::EventArgs^  e);
	System::Void StoreHandler(System::Object^  sender, System::EventArgs^  e);
	System::Void SettingsHandler(System::Object^  sender, System::EventArgs^  e);
	System::Void SelectionHandler(System::Object^  sender, System::EventArgs^  e);
#pragma endregion

#pragma region Members
private: SpoofBuilderPtr mSpoofBuilderP;
private: WizardConfig ^mWizardConfig;
private: SettingsDialog ^mSettingsDialog;
private: System::Windows::Forms::OpenFileDialog ^mOpenFileDialog;
private: System::Windows::Forms::OpenFileDialog ^mLoadFileDialog;
private: System::Windows::Forms::SaveFileDialog ^mSaveFileDialog;
private: System::Windows::Forms::SaveFileDialog ^mStoreFileDialog;
private: System::Collections::Generic::Dictionary<String^, ConfigPage^> mConfigurationMap;
private: System::Collections::Generic::Dictionary<String^, ImportGeneratorPtr^> mImportGeneratorMap;
private: System::Collections::Generic::Dictionary<String^, ExportGeneratorPtr^> mExportGeneratorMap;
private: System::Collections::ArrayList mPointerList;
private: System::Windows::Forms::MenuStrip^  mMenuStrip;
private: System::Windows::Forms::ToolStripMenuItem^  mFileMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  mImportMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  mExportMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  mSeparatorMenuItem1;
private: System::Windows::Forms::ToolStripMenuItem^  mExitMenuItem;
private: System::Windows::Forms::TabControl^  mTabControl;
private: System::Windows::Forms::StatusStrip^  mStatusStrip;
private: System::Windows::Forms::ToolStripSeparator^  mSeparatorMenuItem2;
private: System::Windows::Forms::ToolStripMenuItem^  mSettingsMenuItem;
private: System::Windows::Forms::ToolStrip^  mToolStrip;
private: System::Windows::Forms::ToolStripLabel^  mConfigurationLabel;
private: System::Windows::Forms::ToolStripComboBox^  mConfigurationDroplist;
private: System::Windows::Forms::ToolStripButton^  mImportButton;
private: System::Windows::Forms::ToolStripButton^  mExportButton;
private: System::Windows::Forms::ToolStripSeparator^  mToolStripSeparator1;
private: System::Windows::Forms::ToolStripButton^  mSettingsButton;
private: System::Windows::Forms::ToolStripMenuItem^  mLoadMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  mStoreMenuItem;
#pragma endregion

#pragma region Generated
private:
	System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
	void InitializeComponent(void)
	{
		System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainWindow::typeid));
		this->mMenuStrip = (gcnew System::Windows::Forms::MenuStrip());
		this->mFileMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mImportMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mExportMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mSeparatorMenuItem2 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->mLoadMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mSettingsMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mSeparatorMenuItem1 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->mExitMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mTabControl = (gcnew System::Windows::Forms::TabControl());
		this->mStatusStrip = (gcnew System::Windows::Forms::StatusStrip());
		this->mToolStrip = (gcnew System::Windows::Forms::ToolStrip());
		this->mImportButton = (gcnew System::Windows::Forms::ToolStripButton());
		this->mExportButton = (gcnew System::Windows::Forms::ToolStripButton());
		this->mToolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->mSettingsButton = (gcnew System::Windows::Forms::ToolStripButton());
		this->mConfigurationLabel = (gcnew System::Windows::Forms::ToolStripLabel());
		this->mConfigurationDroplist = (gcnew System::Windows::Forms::ToolStripComboBox());
		this->mStoreMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->mMenuStrip->SuspendLayout();
		this->mToolStrip->SuspendLayout();
		this->SuspendLayout();
		// 
		// mMenuStrip
		// 
		this->mMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->mFileMenuItem});
		this->mMenuStrip->Location = System::Drawing::Point(0, 0);
		this->mMenuStrip->Name = L"mMenuStrip";
		this->mMenuStrip->Size = System::Drawing::Size(592, 24);
		this->mMenuStrip->TabIndex = 0;
		this->mMenuStrip->Text = L"menuStrip1";
		// 
		// mFileMenuItem
		// 
		this->mFileMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {this->mImportMenuItem, 
			this->mExportMenuItem, this->mSeparatorMenuItem2, this->mLoadMenuItem, this->mStoreMenuItem, this->mSettingsMenuItem, this->mSeparatorMenuItem1, 
			this->mExitMenuItem});
		this->mFileMenuItem->Name = L"mFileMenuItem";
		this->mFileMenuItem->Size = System::Drawing::Size(35, 20);
		this->mFileMenuItem->Text = L"File";
		// 
		// mImportMenuItem
		// 
		this->mImportMenuItem->Name = L"mImportMenuItem";
		this->mImportMenuItem->Size = System::Drawing::Size(174, 22);
		this->mImportMenuItem->Text = L"Import DLog...";
		this->mImportMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::ImportHandler);
		// 
		// mExportMenuItem
		// 
		this->mExportMenuItem->Name = L"mExportMenuItem";
		this->mExportMenuItem->Size = System::Drawing::Size(174, 22);
		this->mExportMenuItem->Text = L"Export Spoof...";
		this->mExportMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::ExportHandler);
		// 
		// mSeparatorMenuItem2
		// 
		this->mSeparatorMenuItem2->Name = L"mSeparatorMenuItem2";
		this->mSeparatorMenuItem2->Size = System::Drawing::Size(171, 6);
		// 
		// mLoadMenuItem
		// 
		this->mLoadMenuItem->Name = L"mLoadMenuItem";
		this->mLoadMenuItem->Size = System::Drawing::Size(174, 22);
		this->mLoadMenuItem->Text = L"Load Settings...";
		this->mLoadMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::LoadHandler);
		// 
		// mSettingsMenuItem
		// 
		this->mSettingsMenuItem->Name = L"mSettingsMenuItem";
		this->mSettingsMenuItem->Size = System::Drawing::Size(174, 22);
		this->mSettingsMenuItem->Text = L"Update Settings...";
		this->mSettingsMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::SettingsHandler);
		// 
		// mSeparatorMenuItem1
		// 
		this->mSeparatorMenuItem1->Name = L"mSeparatorMenuItem1";
		this->mSeparatorMenuItem1->Size = System::Drawing::Size(171, 6);
		// 
		// mExitMenuItem
		// 
		this->mExitMenuItem->Name = L"mExitMenuItem";
		this->mExitMenuItem->Size = System::Drawing::Size(174, 22);
		this->mExitMenuItem->Text = L"Exit";
		this->mExitMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::ExitHandler);
		// 
		// mTabControl
		// 
		this->mTabControl->Dock = System::Windows::Forms::DockStyle::Fill;
		this->mTabControl->Location = System::Drawing::Point(0, 24);
		this->mTabControl->Name = L"mTabControl";
		this->mTabControl->SelectedIndex = 0;
		this->mTabControl->Size = System::Drawing::Size(592, 342);
		this->mTabControl->TabIndex = 1;
		this->mTabControl->TabStop = false;
		// 
		// mStatusStrip
		// 
		this->mStatusStrip->Location = System::Drawing::Point(0, 344);
		this->mStatusStrip->Name = L"mStatusStrip";
		this->mStatusStrip->Size = System::Drawing::Size(592, 22);
		this->mStatusStrip->TabIndex = 2;
		this->mStatusStrip->Text = L"statusStrip1";
		// 
		// mToolStrip
		// 
		this->mToolStrip->BackColor = System::Drawing::SystemColors::ActiveCaption;
		this->mToolStrip->GripStyle = System::Windows::Forms::ToolStripGripStyle::Hidden;
		this->mToolStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->mImportButton, this->mExportButton, 
			this->mToolStripSeparator1, this->mSettingsButton, this->mConfigurationLabel, this->mConfigurationDroplist});
		this->mToolStrip->Location = System::Drawing::Point(0, 24);
		this->mToolStrip->Name = L"mToolStrip";
		this->mToolStrip->RenderMode = System::Windows::Forms::ToolStripRenderMode::System;
		this->mToolStrip->Size = System::Drawing::Size(592, 25);
		this->mToolStrip->TabIndex = 3;
		this->mToolStrip->Text = L"toolStrip1";
		// 
		// mImportButton
		// 
		this->mImportButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->mImportButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"mImportButton.Image")));
		this->mImportButton->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->mImportButton->Name = L"mImportButton";
		this->mImportButton->Size = System::Drawing::Size(23, 22);
		this->mImportButton->Text = L"Import...";
		this->mImportButton->Click += gcnew System::EventHandler(this, &MainWindow::ImportHandler);
		// 
		// mExportButton
		// 
		this->mExportButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->mExportButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"mExportButton.Image")));
		this->mExportButton->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->mExportButton->Name = L"mExportButton";
		this->mExportButton->Size = System::Drawing::Size(23, 22);
		this->mExportButton->Text = L"Export...";
		this->mExportButton->Click += gcnew System::EventHandler(this, &MainWindow::ExportHandler);
		// 
		// mToolStripSeparator1
		// 
		this->mToolStripSeparator1->ForeColor = System::Drawing::SystemColors::ControlText;
		this->mToolStripSeparator1->Name = L"mToolStripSeparator1";
		this->mToolStripSeparator1->Size = System::Drawing::Size(6, 25);
		// 
		// mSettingsButton
		// 
		this->mSettingsButton->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->mSettingsButton->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"mSettingsButton.Image")));
		this->mSettingsButton->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->mSettingsButton->Name = L"mSettingsButton";
		this->mSettingsButton->Size = System::Drawing::Size(23, 22);
		this->mSettingsButton->Text = L"Settings...";
		this->mSettingsButton->Click += gcnew System::EventHandler(this, &MainWindow::SettingsHandler);
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
		this->mConfigurationDroplist->Size = System::Drawing::Size(121, 25);
		this->mConfigurationDroplist->SelectedIndexChanged += gcnew System::EventHandler(this, &MainWindow::SelectionHandler);
		// 
		// mStoreMenuItem
		// 
		this->mStoreMenuItem->Name = L"mStoreMenuItem";
		this->mStoreMenuItem->Size = System::Drawing::Size(174, 22);
		this->mStoreMenuItem->Text = L"Store Settings...";
		this->mStoreMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::StoreHandler);
		// 
		// MainWindow
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->ClientSize = System::Drawing::Size(592, 366);
		this->Controls->Add(this->mToolStrip);
		this->Controls->Add(this->mStatusStrip);
		this->Controls->Add(this->mTabControl);
		this->Controls->Add(this->mMenuStrip);
		this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
		this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
		this->MainMenuStrip = this->mMenuStrip;
		this->MaximizeBox = false;
		this->Name = L"MainWindow";
		this->Text = L"Trima Spoof Wizard";
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

} // namespace WIZARD

