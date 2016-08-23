namespace ExcelQuery.Windows {
	partial class QuerySettingsWindow {
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
			this.mDefaultWorksheetTextfield = new System.Windows.Forms.TextBox();
			this.mDefaultDestinationLabel = new System.Windows.Forms.Label();
			this.mWorksheetGroupBox = new System.Windows.Forms.GroupBox();
			this.mOKButton = new System.Windows.Forms.Button();
			this.mCancelButton = new System.Windows.Forms.Button();
			this.mWorksheetGroupBox.SuspendLayout();
			this.SuspendLayout();
			// 
			// mSwitchWorksheetCheckBox
			// 
			this.mSwitchWorksheetCheckBox.AutoSize = true;
			this.mSwitchWorksheetCheckBox.Location = new System.Drawing.Point(135, 39);
			this.mSwitchWorksheetCheckBox.Name = "mSwitchWorksheetCheckBox";
			this.mSwitchWorksheetCheckBox.Size = new System.Drawing.Size(150, 17);
			this.mSwitchWorksheetCheckBox.TabIndex = 2;
			this.mSwitchWorksheetCheckBox.Text = "Switch to result worksheet";
			this.mSwitchWorksheetCheckBox.UseVisualStyleBackColor = true;
			// 
			// mDefaultWorksheetTextfield
			// 
			this.mDefaultWorksheetTextfield.Location = new System.Drawing.Point(135, 13);
			this.mDefaultWorksheetTextfield.Name = "mDefaultWorksheetTextfield";
			this.mDefaultWorksheetTextfield.Size = new System.Drawing.Size(229, 20);
			this.mDefaultWorksheetTextfield.TabIndex = 1;
			this.mDefaultWorksheetTextfield.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPressHandler);
			// 
			// mDefaultDestinationLabel
			// 
			this.mDefaultDestinationLabel.AutoSize = true;
			this.mDefaultDestinationLabel.Location = new System.Drawing.Point(3, 16);
			this.mDefaultDestinationLabel.Name = "mDefaultDestinationLabel";
			this.mDefaultDestinationLabel.Size = new System.Drawing.Size(126, 13);
			this.mDefaultDestinationLabel.TabIndex = 0;
			this.mDefaultDestinationLabel.Text = "Default result destination:";
			// 
			// mWorksheetGroupBox
			// 
			this.mWorksheetGroupBox.Controls.Add(this.mSwitchWorksheetCheckBox);
			this.mWorksheetGroupBox.Controls.Add(this.mDefaultDestinationLabel);
			this.mWorksheetGroupBox.Controls.Add(this.mDefaultWorksheetTextfield);
			this.mWorksheetGroupBox.Location = new System.Drawing.Point(12, 12);
			this.mWorksheetGroupBox.Name = "mWorksheetGroupBox";
			this.mWorksheetGroupBox.Size = new System.Drawing.Size(370, 70);
			this.mWorksheetGroupBox.TabIndex = 2;
			this.mWorksheetGroupBox.TabStop = false;
			this.mWorksheetGroupBox.Text = "Worksheets";
			// 
			// mOKButton
			// 
			this.mOKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mOKButton.Location = new System.Drawing.Point(226, 88);
			this.mOKButton.Name = "mOKButton";
			this.mOKButton.Size = new System.Drawing.Size(75, 23);
			this.mOKButton.TabIndex = 0;
			this.mOKButton.Text = "OK";
			this.mOKButton.UseVisualStyleBackColor = true;
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(307, 88);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(75, 23);
			this.mCancelButton.TabIndex = 1;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			// 
			// QuerySettingsWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(394, 123);
			this.Controls.Add(this.mCancelButton);
			this.Controls.Add(this.mOKButton);
			this.Controls.Add(this.mWorksheetGroupBox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "QuerySettingsWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Query Settings";
			this.mWorksheetGroupBox.ResumeLayout(false);
			this.mWorksheetGroupBox.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.CheckBox mSwitchWorksheetCheckBox;
		private System.Windows.Forms.TextBox mDefaultWorksheetTextfield;
		private System.Windows.Forms.Label mDefaultDestinationLabel;
		private System.Windows.Forms.GroupBox mWorksheetGroupBox;
		private System.Windows.Forms.Button mOKButton;
		private System.Windows.Forms.Button mCancelButton;
	}
}