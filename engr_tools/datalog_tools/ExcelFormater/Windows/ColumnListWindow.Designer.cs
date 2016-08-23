namespace ExcelFormater.Windows {
	partial class ColumnListWindow {
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
			this.mNameLabel = new System.Windows.Forms.Label();
			this.mNameTextfield = new System.Windows.Forms.TextBox();
			this.mAvailableList = new System.Windows.Forms.CheckedListBox();
			this.mSelectedList = new System.Windows.Forms.ListBox();
			this.mOKButton = new System.Windows.Forms.Button();
			this.mCancelButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// mNameLabel
			// 
			this.mNameLabel.AutoSize = true;
			this.mNameLabel.Location = new System.Drawing.Point(13, 13);
			this.mNameLabel.Name = "mNameLabel";
			this.mNameLabel.Size = new System.Drawing.Size(38, 13);
			this.mNameLabel.TabIndex = 0;
			this.mNameLabel.Text = "Name:";
			// 
			// mNameTextfield
			// 
			this.mNameTextfield.Location = new System.Drawing.Point(57, 10);
			this.mNameTextfield.Name = "mNameTextfield";
			this.mNameTextfield.Size = new System.Drawing.Size(223, 20);
			this.mNameTextfield.TabIndex = 1;
			// 
			// mAvailableList
			// 
			this.mAvailableList.FormattingEnabled = true;
			this.mAvailableList.Location = new System.Drawing.Point(13, 36);
			this.mAvailableList.Name = "mAvailableList";
			this.mAvailableList.Size = new System.Drawing.Size(267, 94);
			this.mAvailableList.Sorted = true;
			this.mAvailableList.TabIndex = 2;
			this.mAvailableList.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.ItemCheckedHandler);
			// 
			// mSelectedList
			// 
			this.mSelectedList.FormattingEnabled = true;
			this.mSelectedList.Location = new System.Drawing.Point(12, 136);
			this.mSelectedList.Name = "mSelectedList";
			this.mSelectedList.SelectionMode = System.Windows.Forms.SelectionMode.None;
			this.mSelectedList.Size = new System.Drawing.Size(268, 95);
			this.mSelectedList.Sorted = true;
			this.mSelectedList.TabIndex = 3;
			// 
			// mOKButton
			// 
			this.mOKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mOKButton.Location = new System.Drawing.Point(124, 237);
			this.mOKButton.Name = "mOKButton";
			this.mOKButton.Size = new System.Drawing.Size(75, 23);
			this.mOKButton.TabIndex = 4;
			this.mOKButton.Text = "OK";
			this.mOKButton.UseVisualStyleBackColor = true;
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(205, 237);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(75, 23);
			this.mCancelButton.TabIndex = 5;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			// 
			// ColumnListWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(292, 271);
			this.Controls.Add(this.mCancelButton);
			this.Controls.Add(this.mOKButton);
			this.Controls.Add(this.mSelectedList);
			this.Controls.Add(this.mAvailableList);
			this.Controls.Add(this.mNameTextfield);
			this.Controls.Add(this.mNameLabel);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "ColumnListWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Column List";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label mNameLabel;
		private System.Windows.Forms.TextBox mNameTextfield;
		private System.Windows.Forms.CheckedListBox mAvailableList;
		private System.Windows.Forms.ListBox mSelectedList;
		private System.Windows.Forms.Button mOKButton;
		private System.Windows.Forms.Button mCancelButton;
	}
}