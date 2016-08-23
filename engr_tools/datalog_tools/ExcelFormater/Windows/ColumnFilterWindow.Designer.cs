namespace ExcelFormater.Windows {
	partial class ColumnFilterWindow {
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
			this.mCancelButton = new System.Windows.Forms.Button();
			this.mOKButton = new System.Windows.Forms.Button();
			this.mSelectedList = new System.Windows.Forms.ListBox();
			this.mNameTextfield = new System.Windows.Forms.TextBox();
			this.mNameLabel = new System.Windows.Forms.Label();
			this.mFilterTextfield = new System.Windows.Forms.TextBox();
			this.mFilterLabel = new System.Windows.Forms.Label();
			this.mNegateCheckbox = new System.Windows.Forms.CheckBox();
			this.mIgnoreCaseCheckbox = new System.Windows.Forms.CheckBox();
			this.SuspendLayout();
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(205, 185);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(75, 23);
			this.mCancelButton.TabIndex = 11;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			// 
			// mOKButton
			// 
			this.mOKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mOKButton.Location = new System.Drawing.Point(124, 185);
			this.mOKButton.Name = "mOKButton";
			this.mOKButton.Size = new System.Drawing.Size(75, 23);
			this.mOKButton.TabIndex = 10;
			this.mOKButton.Text = "OK";
			this.mOKButton.UseVisualStyleBackColor = true;
			// 
			// mSelectedList
			// 
			this.mSelectedList.FormattingEnabled = true;
			this.mSelectedList.Location = new System.Drawing.Point(12, 84);
			this.mSelectedList.Name = "mSelectedList";
			this.mSelectedList.SelectionMode = System.Windows.Forms.SelectionMode.None;
			this.mSelectedList.Size = new System.Drawing.Size(268, 95);
			this.mSelectedList.Sorted = true;
			this.mSelectedList.TabIndex = 9;
			// 
			// mNameTextfield
			// 
			this.mNameTextfield.Location = new System.Drawing.Point(57, 8);
			this.mNameTextfield.Name = "mNameTextfield";
			this.mNameTextfield.Size = new System.Drawing.Size(223, 20);
			this.mNameTextfield.TabIndex = 7;
			// 
			// mNameLabel
			// 
			this.mNameLabel.AutoSize = true;
			this.mNameLabel.Location = new System.Drawing.Point(13, 11);
			this.mNameLabel.Name = "mNameLabel";
			this.mNameLabel.Size = new System.Drawing.Size(38, 13);
			this.mNameLabel.TabIndex = 6;
			this.mNameLabel.Text = "Name:";
			// 
			// mFilterTextfield
			// 
			this.mFilterTextfield.Location = new System.Drawing.Point(57, 34);
			this.mFilterTextfield.Name = "mFilterTextfield";
			this.mFilterTextfield.Size = new System.Drawing.Size(223, 20);
			this.mFilterTextfield.TabIndex = 12;
			this.mFilterTextfield.TextChanged += new System.EventHandler(this.ReloadListHandler);
			// 
			// mFilterLabel
			// 
			this.mFilterLabel.AutoSize = true;
			this.mFilterLabel.Location = new System.Drawing.Point(12, 37);
			this.mFilterLabel.Name = "mFilterLabel";
			this.mFilterLabel.Size = new System.Drawing.Size(32, 13);
			this.mFilterLabel.TabIndex = 13;
			this.mFilterLabel.Text = "Filter:";
			// 
			// mNegateCheckbox
			// 
			this.mNegateCheckbox.AutoSize = true;
			this.mNegateCheckbox.Location = new System.Drawing.Point(146, 60);
			this.mNegateCheckbox.Name = "mNegateCheckbox";
			this.mNegateCheckbox.Size = new System.Drawing.Size(61, 17);
			this.mNegateCheckbox.TabIndex = 14;
			this.mNegateCheckbox.Text = "Negate";
			this.mNegateCheckbox.UseVisualStyleBackColor = true;
			this.mNegateCheckbox.CheckedChanged += new System.EventHandler(this.ReloadListHandler);
			// 
			// mIgnoreCaseCheckbox
			// 
			this.mIgnoreCaseCheckbox.AutoSize = true;
			this.mIgnoreCaseCheckbox.Location = new System.Drawing.Point(57, 61);
			this.mIgnoreCaseCheckbox.Name = "mIgnoreCaseCheckbox";
			this.mIgnoreCaseCheckbox.Size = new System.Drawing.Size(83, 17);
			this.mIgnoreCaseCheckbox.TabIndex = 15;
			this.mIgnoreCaseCheckbox.Text = "Ignore Case";
			this.mIgnoreCaseCheckbox.UseVisualStyleBackColor = true;
			this.mIgnoreCaseCheckbox.CheckedChanged += new System.EventHandler(this.ReloadListHandler);
			// 
			// ColumnFilterWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(292, 221);
			this.Controls.Add(this.mIgnoreCaseCheckbox);
			this.Controls.Add(this.mNegateCheckbox);
			this.Controls.Add(this.mFilterLabel);
			this.Controls.Add(this.mFilterTextfield);
			this.Controls.Add(this.mCancelButton);
			this.Controls.Add(this.mOKButton);
			this.Controls.Add(this.mSelectedList);
			this.Controls.Add(this.mNameTextfield);
			this.Controls.Add(this.mNameLabel);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "ColumnFilterWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Column Filter";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button mCancelButton;
		private System.Windows.Forms.Button mOKButton;
		private System.Windows.Forms.ListBox mSelectedList;
		private System.Windows.Forms.TextBox mNameTextfield;
		private System.Windows.Forms.Label mNameLabel;
		private System.Windows.Forms.TextBox mFilterTextfield;
		private System.Windows.Forms.Label mFilterLabel;
		private System.Windows.Forms.CheckBox mNegateCheckbox;
		private System.Windows.Forms.CheckBox mIgnoreCaseCheckbox;
	}
}