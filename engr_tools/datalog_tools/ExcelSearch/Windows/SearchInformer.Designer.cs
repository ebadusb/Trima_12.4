namespace ExcelSearch.Windows {
	partial class SearchInformer {
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
			this.mSearchLogTextbox = new System.Windows.Forms.TextBox();
			this.mCloseButton = new System.Windows.Forms.Button();
			this.mShowDialogCheckBox = new System.Windows.Forms.CheckBox();
			this.SuspendLayout();
			// 
			// mSearchLogTextbox
			// 
			this.mSearchLogTextbox.AcceptsReturn = true;
			this.mSearchLogTextbox.AcceptsTab = true;
			this.mSearchLogTextbox.Location = new System.Drawing.Point(12, 12);
			this.mSearchLogTextbox.Multiline = true;
			this.mSearchLogTextbox.Name = "mSearchLogTextbox";
			this.mSearchLogTextbox.ReadOnly = true;
			this.mSearchLogTextbox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.mSearchLogTextbox.Size = new System.Drawing.Size(368, 132);
			this.mSearchLogTextbox.TabIndex = 1;
			// 
			// mCloseButton
			// 
			this.mCloseButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mCloseButton.Location = new System.Drawing.Point(305, 152);
			this.mCloseButton.Name = "mCloseButton";
			this.mCloseButton.Size = new System.Drawing.Size(75, 23);
			this.mCloseButton.TabIndex = 0;
			this.mCloseButton.Text = "Close";
			this.mCloseButton.UseVisualStyleBackColor = true;
			// 
			// mShowDialogCheckBox
			// 
			this.mShowDialogCheckBox.AutoSize = true;
			this.mShowDialogCheckBox.Location = new System.Drawing.Point(12, 156);
			this.mShowDialogCheckBox.Name = "mShowDialogCheckBox";
			this.mShowDialogCheckBox.Size = new System.Drawing.Size(134, 17);
			this.mShowDialogCheckBox.TabIndex = 2;
			this.mShowDialogCheckBox.Text = "Do not show log dialog";
			this.mShowDialogCheckBox.UseVisualStyleBackColor = true;
			// 
			// SearchInformer
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(394, 186);
			this.Controls.Add(this.mShowDialogCheckBox);
			this.Controls.Add(this.mCloseButton);
			this.Controls.Add(this.mSearchLogTextbox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "SearchInformer";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Search Log";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TextBox mSearchLogTextbox;
		private System.Windows.Forms.Button mCloseButton;
		private System.Windows.Forms.CheckBox mShowDialogCheckBox;
	}
}