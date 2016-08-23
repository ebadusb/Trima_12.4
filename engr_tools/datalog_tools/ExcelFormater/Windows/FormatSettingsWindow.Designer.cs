namespace ExcelFormater.Windows {
	partial class FormatSettingsWindow {
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
			this.mFormatCheckbox = new System.Windows.Forms.CheckBox();
			this.mFormatList = new System.Windows.Forms.ComboBox();
			this.mWorkbooksGroup = new System.Windows.Forms.GroupBox();
			this.mHideDefaultSheets = new System.Windows.Forms.CheckBox();
			this.mOKButton = new System.Windows.Forms.Button();
			this.mCancelButton = new System.Windows.Forms.Button();
			this.mWorkbooksGroup.SuspendLayout();
			this.SuspendLayout();
			// 
			// mFormatCheckbox
			// 
			this.mFormatCheckbox.AutoSize = true;
			this.mFormatCheckbox.Location = new System.Drawing.Point(6, 19);
			this.mFormatCheckbox.Name = "mFormatCheckbox";
			this.mFormatCheckbox.Size = new System.Drawing.Size(100, 17);
			this.mFormatCheckbox.TabIndex = 0;
			this.mFormatCheckbox.Text = "Format on open";
			this.mFormatCheckbox.UseVisualStyleBackColor = true;
			this.mFormatCheckbox.CheckedChanged += new System.EventHandler(this.FormatOnOpenHandler);
			// 
			// mFormatList
			// 
			this.mFormatList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.mFormatList.Enabled = false;
			this.mFormatList.FormattingEnabled = true;
			this.mFormatList.Location = new System.Drawing.Point(112, 17);
			this.mFormatList.Name = "mFormatList";
			this.mFormatList.Size = new System.Drawing.Size(150, 21);
			this.mFormatList.TabIndex = 1;
			// 
			// mWorkbooksGroup
			// 
			this.mWorkbooksGroup.Controls.Add(this.mHideDefaultSheets);
			this.mWorkbooksGroup.Controls.Add(this.mFormatCheckbox);
			this.mWorkbooksGroup.Controls.Add(this.mFormatList);
			this.mWorkbooksGroup.Location = new System.Drawing.Point(13, 13);
			this.mWorkbooksGroup.Name = "mWorkbooksGroup";
			this.mWorkbooksGroup.Size = new System.Drawing.Size(278, 76);
			this.mWorkbooksGroup.TabIndex = 2;
			this.mWorkbooksGroup.TabStop = false;
			this.mWorkbooksGroup.Text = "Workbooks";
			// 
			// mHideDefaultSheets
			// 
			this.mHideDefaultSheets.AutoSize = true;
			this.mHideDefaultSheets.Location = new System.Drawing.Point(7, 43);
			this.mHideDefaultSheets.Name = "mHideDefaultSheets";
			this.mHideDefaultSheets.Size = new System.Drawing.Size(117, 17);
			this.mHideDefaultSheets.TabIndex = 2;
			this.mHideDefaultSheets.Text = "Hide default sheets";
			this.mHideDefaultSheets.UseVisualStyleBackColor = true;
			// 
			// mOKButton
			// 
			this.mOKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mOKButton.Location = new System.Drawing.Point(135, 95);
			this.mOKButton.Name = "mOKButton";
			this.mOKButton.Size = new System.Drawing.Size(75, 23);
			this.mOKButton.TabIndex = 3;
			this.mOKButton.Text = "OK";
			this.mOKButton.UseVisualStyleBackColor = true;
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(216, 95);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(75, 23);
			this.mCancelButton.TabIndex = 4;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			// 
			// FormatSettingsWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(307, 131);
			this.Controls.Add(this.mCancelButton);
			this.Controls.Add(this.mOKButton);
			this.Controls.Add(this.mWorkbooksGroup);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "FormatSettingsWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Format Settings";
			this.mWorkbooksGroup.ResumeLayout(false);
			this.mWorkbooksGroup.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.CheckBox mFormatCheckbox;
		private System.Windows.Forms.ComboBox mFormatList;
		private System.Windows.Forms.GroupBox mWorkbooksGroup;
		private System.Windows.Forms.Button mOKButton;
		private System.Windows.Forms.Button mCancelButton;
		private System.Windows.Forms.CheckBox mHideDefaultSheets;
	}
}