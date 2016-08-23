namespace ExcelFill.Windows {
	partial class FillWindow {
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
			this.mHighlightCheckbox = new System.Windows.Forms.CheckBox();
			this.mSettingsGroupbox = new System.Windows.Forms.GroupBox();
			this.mColorButton = new System.Windows.Forms.Button();
			this.mHyperlinkCheckbox = new System.Windows.Forms.CheckBox();
			this.mOKButton = new System.Windows.Forms.Button();
			this.mColorDialog = new System.Windows.Forms.ColorDialog();
			this.mSettingsGroupbox.SuspendLayout();
			this.SuspendLayout();
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(207, 92);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(75, 23);
			this.mCancelButton.TabIndex = 0;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			// 
			// mHighlightCheckbox
			// 
			this.mHighlightCheckbox.AutoSize = true;
			this.mHighlightCheckbox.Location = new System.Drawing.Point(6, 19);
			this.mHighlightCheckbox.Name = "mHighlightCheckbox";
			this.mHighlightCheckbox.Size = new System.Drawing.Size(136, 17);
			this.mHighlightCheckbox.TabIndex = 1;
			this.mHighlightCheckbox.Text = "Highlight Updated Cells";
			this.mHighlightCheckbox.UseVisualStyleBackColor = true;
			// 
			// mSettingsGroupbox
			// 
			this.mSettingsGroupbox.Controls.Add(this.mColorButton);
			this.mSettingsGroupbox.Controls.Add(this.mHyperlinkCheckbox);
			this.mSettingsGroupbox.Controls.Add(this.mHighlightCheckbox);
			this.mSettingsGroupbox.Location = new System.Drawing.Point(12, 12);
			this.mSettingsGroupbox.Name = "mSettingsGroupbox";
			this.mSettingsGroupbox.Size = new System.Drawing.Size(270, 74);
			this.mSettingsGroupbox.TabIndex = 2;
			this.mSettingsGroupbox.TabStop = false;
			this.mSettingsGroupbox.Text = "Settings";
			// 
			// mColorButton
			// 
			this.mColorButton.BackColor = System.Drawing.Color.Yellow;
			this.mColorButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.mColorButton.Location = new System.Drawing.Point(148, 16);
			this.mColorButton.Name = "mColorButton";
			this.mColorButton.Size = new System.Drawing.Size(20, 20);
			this.mColorButton.TabIndex = 3;
			this.mColorButton.UseVisualStyleBackColor = false;
			this.mColorButton.Click += new System.EventHandler(this.ColorButtonHandler);
			// 
			// mHyperlinkCheckbox
			// 
			this.mHyperlinkCheckbox.AutoSize = true;
			this.mHyperlinkCheckbox.Location = new System.Drawing.Point(7, 43);
			this.mHyperlinkCheckbox.Name = "mHyperlinkCheckbox";
			this.mHyperlinkCheckbox.Size = new System.Drawing.Size(139, 17);
			this.mHyperlinkCheckbox.TabIndex = 2;
			this.mHyperlinkCheckbox.Text = "Hyperlink Updated Cells";
			this.mHyperlinkCheckbox.UseVisualStyleBackColor = true;
			// 
			// mOKButton
			// 
			this.mOKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mOKButton.Location = new System.Drawing.Point(126, 92);
			this.mOKButton.Name = "mOKButton";
			this.mOKButton.Size = new System.Drawing.Size(75, 23);
			this.mOKButton.TabIndex = 3;
			this.mOKButton.Text = "OK";
			this.mOKButton.UseVisualStyleBackColor = true;
			// 
			// FillWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(294, 128);
			this.Controls.Add(this.mOKButton);
			this.Controls.Add(this.mSettingsGroupbox);
			this.Controls.Add(this.mCancelButton);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "FillWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Quick Fill Settings";
			this.mSettingsGroupbox.ResumeLayout(false);
			this.mSettingsGroupbox.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Button mCancelButton;
		private System.Windows.Forms.CheckBox mHighlightCheckbox;
		private System.Windows.Forms.GroupBox mSettingsGroupbox;
		private System.Windows.Forms.CheckBox mHyperlinkCheckbox;
		private System.Windows.Forms.Button mOKButton;
		private System.Windows.Forms.Button mColorButton;
		private System.Windows.Forms.ColorDialog mColorDialog;
	}
}