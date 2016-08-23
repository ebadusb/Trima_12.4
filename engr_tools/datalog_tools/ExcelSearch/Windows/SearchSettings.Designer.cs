namespace ExcelSearch.Windows {
	partial class SearchSettings {
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null)) {
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.mSwitchWorksheetCheckBox = new System.Windows.Forms.CheckBox();
			this.mRememberSearchCheckbox = new System.Windows.Forms.CheckBox();
			this.mDefaultWorksheetTextfield = new System.Windows.Forms.TextBox();
			this.mDefaultWorksheetLabel = new System.Windows.Forms.Label();
			this.mWorksheetGroupBox = new System.Windows.Forms.GroupBox();
			this.mSearchesGroupBox = new System.Windows.Forms.GroupBox();
			this.mClearHistoryButton = new System.Windows.Forms.Button();
			this.mHistoryCountMaxTextfield = new System.Windows.Forms.TextBox();
			this.mHistoryMaxLabel = new System.Windows.Forms.Label();
			this.mOKButton = new System.Windows.Forms.Button();
			this.mCancelButton = new System.Windows.Forms.Button();
			this.mVersionLabel = new System.Windows.Forms.Label();
			this.mVersionText = new System.Windows.Forms.Label();
			this.mWorksheetGroupBox.SuspendLayout();
			this.mSearchesGroupBox.SuspendLayout();
			this.SuspendLayout();
			// 
			// mSwitchWorksheetCheckBox
			// 
			this.mSwitchWorksheetCheckBox.AutoSize = true;
			this.mSwitchWorksheetCheckBox.Location = new System.Drawing.Point(6, 42);
			this.mSwitchWorksheetCheckBox.Name = "mSwitchWorksheetCheckBox";
			this.mSwitchWorksheetCheckBox.Size = new System.Drawing.Size(150, 17);
			this.mSwitchWorksheetCheckBox.TabIndex = 2;
			this.mSwitchWorksheetCheckBox.Text = "Switch to result worksheet";
			this.mSwitchWorksheetCheckBox.UseVisualStyleBackColor = true;
			// 
			// mRememberSearchCheckbox
			// 
			this.mRememberSearchCheckbox.AutoSize = true;
			this.mRememberSearchCheckbox.Location = new System.Drawing.Point(6, 43);
			this.mRememberSearchCheckbox.Name = "mRememberSearchCheckbox";
			this.mRememberSearchCheckbox.Size = new System.Drawing.Size(170, 17);
			this.mRememberSearchCheckbox.TabIndex = 2;
			this.mRememberSearchCheckbox.Text = "Remember last search settings";
			this.mRememberSearchCheckbox.UseVisualStyleBackColor = true;
			// 
			// mDefaultWorksheetTextfield
			// 
			this.mDefaultWorksheetTextfield.Location = new System.Drawing.Point(173, 13);
			this.mDefaultWorksheetTextfield.Name = "mDefaultWorksheetTextfield";
			this.mDefaultWorksheetTextfield.Size = new System.Drawing.Size(86, 20);
			this.mDefaultWorksheetTextfield.TabIndex = 1;
			this.mDefaultWorksheetTextfield.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPressHandler);
			// 
			// mDefaultWorksheetLabel
			// 
			this.mDefaultWorksheetLabel.AutoSize = true;
			this.mDefaultWorksheetLabel.Location = new System.Drawing.Point(3, 16);
			this.mDefaultWorksheetLabel.Name = "mDefaultWorksheetLabel";
			this.mDefaultWorksheetLabel.Size = new System.Drawing.Size(153, 13);
			this.mDefaultWorksheetLabel.TabIndex = 0;
			this.mDefaultWorksheetLabel.Text = "Default result worksheet name:";
			// 
			// mWorksheetGroupBox
			// 
			this.mWorksheetGroupBox.Controls.Add(this.mSwitchWorksheetCheckBox);
			this.mWorksheetGroupBox.Controls.Add(this.mDefaultWorksheetLabel);
			this.mWorksheetGroupBox.Controls.Add(this.mDefaultWorksheetTextfield);
			this.mWorksheetGroupBox.Location = new System.Drawing.Point(12, 12);
			this.mWorksheetGroupBox.Name = "mWorksheetGroupBox";
			this.mWorksheetGroupBox.Size = new System.Drawing.Size(270, 71);
			this.mWorksheetGroupBox.TabIndex = 2;
			this.mWorksheetGroupBox.TabStop = false;
			this.mWorksheetGroupBox.Text = "Worksheets";
			// 
			// mSearchesGroupBox
			// 
			this.mSearchesGroupBox.Controls.Add(this.mClearHistoryButton);
			this.mSearchesGroupBox.Controls.Add(this.mHistoryCountMaxTextfield);
			this.mSearchesGroupBox.Controls.Add(this.mHistoryMaxLabel);
			this.mSearchesGroupBox.Controls.Add(this.mRememberSearchCheckbox);
			this.mSearchesGroupBox.Location = new System.Drawing.Point(12, 89);
			this.mSearchesGroupBox.Name = "mSearchesGroupBox";
			this.mSearchesGroupBox.Size = new System.Drawing.Size(270, 76);
			this.mSearchesGroupBox.TabIndex = 3;
			this.mSearchesGroupBox.TabStop = false;
			this.mSearchesGroupBox.Text = "Searches";
			// 
			// mClearHistoryButton
			// 
			this.mClearHistoryButton.Location = new System.Drawing.Point(173, 15);
			this.mClearHistoryButton.Name = "mClearHistoryButton";
			this.mClearHistoryButton.Size = new System.Drawing.Size(86, 23);
			this.mClearHistoryButton.TabIndex = 3;
			this.mClearHistoryButton.Text = "Clear List";
			this.mClearHistoryButton.UseVisualStyleBackColor = true;
			this.mClearHistoryButton.Click += new System.EventHandler(this.mClearHistoryButton_Click);
			// 
			// mHistoryCountMaxTextfield
			// 
			this.mHistoryCountMaxTextfield.Location = new System.Drawing.Point(74, 17);
			this.mHistoryCountMaxTextfield.Name = "mHistoryCountMaxTextfield";
			this.mHistoryCountMaxTextfield.Size = new System.Drawing.Size(91, 20);
			this.mHistoryCountMaxTextfield.TabIndex = 1;
			this.mHistoryCountMaxTextfield.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPressHandler);
			// 
			// mHistoryMaxLabel
			// 
			this.mHistoryMaxLabel.AutoSize = true;
			this.mHistoryMaxLabel.Location = new System.Drawing.Point(3, 20);
			this.mHistoryMaxLabel.Name = "mHistoryMaxLabel";
			this.mHistoryMaxLabel.Size = new System.Drawing.Size(65, 13);
			this.mHistoryMaxLabel.TabIndex = 0;
			this.mHistoryMaxLabel.Text = "History Size:";
			// 
			// mOKButton
			// 
			this.mOKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mOKButton.Location = new System.Drawing.Point(126, 171);
			this.mOKButton.Name = "mOKButton";
			this.mOKButton.Size = new System.Drawing.Size(75, 23);
			this.mOKButton.TabIndex = 0;
			this.mOKButton.Text = "OK";
			this.mOKButton.UseVisualStyleBackColor = true;
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(207, 171);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(75, 23);
			this.mCancelButton.TabIndex = 1;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			// 
			// mVersionLabel
			// 
			this.mVersionLabel.AutoSize = true;
			this.mVersionLabel.Location = new System.Drawing.Point(12, 176);
			this.mVersionLabel.Name = "mVersionLabel";
			this.mVersionLabel.Size = new System.Drawing.Size(45, 13);
			this.mVersionLabel.TabIndex = 4;
			this.mVersionLabel.Text = "Version:";
			// 
			// mVersionText
			// 
			this.mVersionText.AutoSize = true;
			this.mVersionText.Location = new System.Drawing.Point(63, 176);
			this.mVersionText.Name = "mVersionText";
			this.mVersionText.Size = new System.Drawing.Size(22, 13);
			this.mVersionText.TabIndex = 5;
			this.mVersionText.Text = "0.0";
			// 
			// SearchSettings
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(294, 203);
			this.Controls.Add(this.mVersionText);
			this.Controls.Add(this.mVersionLabel);
			this.Controls.Add(this.mCancelButton);
			this.Controls.Add(this.mOKButton);
			this.Controls.Add(this.mSearchesGroupBox);
			this.Controls.Add(this.mWorksheetGroupBox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "SearchSettings";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Search Settings";
			this.mWorksheetGroupBox.ResumeLayout(false);
			this.mWorksheetGroupBox.PerformLayout();
			this.mSearchesGroupBox.ResumeLayout(false);
			this.mSearchesGroupBox.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.CheckBox mSwitchWorksheetCheckBox;
		private System.Windows.Forms.CheckBox mRememberSearchCheckbox;
		private System.Windows.Forms.TextBox mDefaultWorksheetTextfield;
		private System.Windows.Forms.Label mDefaultWorksheetLabel;
		private System.Windows.Forms.GroupBox mWorksheetGroupBox;
		private System.Windows.Forms.GroupBox mSearchesGroupBox;
		private System.Windows.Forms.Button mOKButton;
		private System.Windows.Forms.Button mCancelButton;
		private System.Windows.Forms.TextBox mHistoryCountMaxTextfield;
		private System.Windows.Forms.Label mHistoryMaxLabel;
		private System.Windows.Forms.Button mClearHistoryButton;
		private System.Windows.Forms.Label mVersionLabel;
		private System.Windows.Forms.Label mVersionText;
	}
}