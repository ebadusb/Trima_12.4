namespace ExcelFormater.Windows {
	partial class ColumnMacroWindow {
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
			this.mNameTextfield = new System.Windows.Forms.TextBox();
			this.mNameLabel = new System.Windows.Forms.Label();
			this.mFunctionTextfield = new System.Windows.Forms.TextBox();
			this.mFunctionLabel = new System.Windows.Forms.Label();
			this.mCancelButton = new System.Windows.Forms.Button();
			this.mOKButton = new System.Windows.Forms.Button();
			this.mSelectedList = new System.Windows.Forms.ListBox();
			this.mAvailableList = new System.Windows.Forms.CheckedListBox();
			this.SuspendLayout();
			// 
			// mNameTextfield
			// 
			this.mNameTextfield.Location = new System.Drawing.Point(68, 6);
			this.mNameTextfield.Name = "mNameTextfield";
			this.mNameTextfield.Size = new System.Drawing.Size(211, 20);
			this.mNameTextfield.TabIndex = 17;
			// 
			// mNameLabel
			// 
			this.mNameLabel.AutoSize = true;
			this.mNameLabel.Location = new System.Drawing.Point(24, 9);
			this.mNameLabel.Name = "mNameLabel";
			this.mNameLabel.Size = new System.Drawing.Size(38, 13);
			this.mNameLabel.TabIndex = 16;
			this.mNameLabel.Text = "Name:";
			// 
			// mFunctionTextfield
			// 
			this.mFunctionTextfield.Location = new System.Drawing.Point(68, 32);
			this.mFunctionTextfield.Name = "mFunctionTextfield";
			this.mFunctionTextfield.Size = new System.Drawing.Size(211, 20);
			this.mFunctionTextfield.TabIndex = 23;
			// 
			// mFunctionLabel
			// 
			this.mFunctionLabel.AutoSize = true;
			this.mFunctionLabel.Location = new System.Drawing.Point(11, 35);
			this.mFunctionLabel.Name = "mFunctionLabel";
			this.mFunctionLabel.Size = new System.Drawing.Size(51, 13);
			this.mFunctionLabel.TabIndex = 22;
			this.mFunctionLabel.Text = "Function:";
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(205, 259);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(75, 23);
			this.mCancelButton.TabIndex = 26;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			// 
			// mOKButton
			// 
			this.mOKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mOKButton.Location = new System.Drawing.Point(124, 259);
			this.mOKButton.Name = "mOKButton";
			this.mOKButton.Size = new System.Drawing.Size(75, 23);
			this.mOKButton.TabIndex = 25;
			this.mOKButton.Text = "OK";
			this.mOKButton.UseVisualStyleBackColor = true;
			// 
			// mSelectedList
			// 
			this.mSelectedList.FormattingEnabled = true;
			this.mSelectedList.Location = new System.Drawing.Point(11, 158);
			this.mSelectedList.Name = "mSelectedList";
			this.mSelectedList.SelectionMode = System.Windows.Forms.SelectionMode.None;
			this.mSelectedList.Size = new System.Drawing.Size(268, 95);
			this.mSelectedList.Sorted = true;
			this.mSelectedList.TabIndex = 28;
			// 
			// mAvailableList
			// 
			this.mAvailableList.FormattingEnabled = true;
			this.mAvailableList.Location = new System.Drawing.Point(12, 58);
			this.mAvailableList.Name = "mAvailableList";
			this.mAvailableList.Size = new System.Drawing.Size(267, 94);
			this.mAvailableList.Sorted = true;
			this.mAvailableList.TabIndex = 27;
			this.mAvailableList.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.ItemCheckedHandler);
			// 
			// ColumnMacroWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(292, 291);
			this.Controls.Add(this.mSelectedList);
			this.Controls.Add(this.mAvailableList);
			this.Controls.Add(this.mCancelButton);
			this.Controls.Add(this.mOKButton);
			this.Controls.Add(this.mFunctionTextfield);
			this.Controls.Add(this.mFunctionLabel);
			this.Controls.Add(this.mNameTextfield);
			this.Controls.Add(this.mNameLabel);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "ColumnMacroWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Column Macro";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TextBox mNameTextfield;
		private System.Windows.Forms.Label mNameLabel;
		private System.Windows.Forms.TextBox mFunctionTextfield;
		private System.Windows.Forms.Label mFunctionLabel;
		private System.Windows.Forms.Button mCancelButton;
		private System.Windows.Forms.Button mOKButton;
		private System.Windows.Forms.ListBox mSelectedList;
		private System.Windows.Forms.CheckedListBox mAvailableList;
	}
}