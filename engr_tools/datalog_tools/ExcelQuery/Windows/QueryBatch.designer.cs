namespace ExcelQuery.Windows {
	partial class QueryBatch {
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
			this.mQueryList = new System.Windows.Forms.CheckedListBox();
			this.mCommentTextbox = new System.Windows.Forms.TextBox();
			this.mOKButton = new System.Windows.Forms.Button();
			this.mCancelButton = new System.Windows.Forms.Button();
			this.mNameLabel = new System.Windows.Forms.Label();
			this.mNameTextfield = new System.Windows.Forms.TextBox();
			this.mCommentLabel = new System.Windows.Forms.Label();
			this.mQueryLabel = new System.Windows.Forms.Label();
			this.mOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.SuspendLayout();
			// 
			// mQueryList
			// 
			this.mQueryList.CheckOnClick = true;
			this.mQueryList.FormattingEnabled = true;
			this.mQueryList.Location = new System.Drawing.Point(72, 39);
			this.mQueryList.Name = "mQueryList";
			this.mQueryList.Size = new System.Drawing.Size(248, 109);
			this.mQueryList.TabIndex = 6;
			// 
			// mCommentTextbox
			// 
			this.mCommentTextbox.Location = new System.Drawing.Point(72, 154);
			this.mCommentTextbox.Multiline = true;
			this.mCommentTextbox.Name = "mCommentTextbox";
			this.mCommentTextbox.Size = new System.Drawing.Size(248, 50);
			this.mCommentTextbox.TabIndex = 8;
			// 
			// mOKButton
			// 
			this.mOKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mOKButton.Location = new System.Drawing.Point(164, 210);
			this.mOKButton.Name = "mOKButton";
			this.mOKButton.Size = new System.Drawing.Size(75, 23);
			this.mOKButton.TabIndex = 0;
			this.mOKButton.Text = "OK";
			this.mOKButton.UseVisualStyleBackColor = true;
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(245, 210);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(75, 23);
			this.mCancelButton.TabIndex = 1;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			// 
			// mNameLabel
			// 
			this.mNameLabel.AutoSize = true;
			this.mNameLabel.Location = new System.Drawing.Point(12, 16);
			this.mNameLabel.Name = "mNameLabel";
			this.mNameLabel.Size = new System.Drawing.Size(38, 13);
			this.mNameLabel.TabIndex = 2;
			this.mNameLabel.Text = "Name:";
			// 
			// mNameTextfield
			// 
			this.mNameTextfield.Location = new System.Drawing.Point(72, 13);
			this.mNameTextfield.Name = "mNameTextfield";
			this.mNameTextfield.Size = new System.Drawing.Size(248, 20);
			this.mNameTextfield.TabIndex = 3;
			// 
			// mCommentLabel
			// 
			this.mCommentLabel.AutoSize = true;
			this.mCommentLabel.Location = new System.Drawing.Point(12, 155);
			this.mCommentLabel.Name = "mCommentLabel";
			this.mCommentLabel.Size = new System.Drawing.Size(54, 13);
			this.mCommentLabel.TabIndex = 7;
			this.mCommentLabel.Text = "Comment:";
			// 
			// mQueryLabel
			// 
			this.mQueryLabel.AutoSize = true;
			this.mQueryLabel.Location = new System.Drawing.Point(12, 41);
			this.mQueryLabel.Name = "mQueryLabel";
			this.mQueryLabel.Size = new System.Drawing.Size(45, 13);
			this.mQueryLabel.TabIndex = 4;
			this.mQueryLabel.Text = "Include:";
			// 
			// mOpenFileDialog
			// 
			this.mOpenFileDialog.Title = "Impot Icon";
			// 
			// QueryBatch
			// 
			this.AcceptButton = this.mOKButton;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.mCancelButton;
			this.ClientSize = new System.Drawing.Size(332, 246);
			this.Controls.Add(this.mQueryLabel);
			this.Controls.Add(this.mCommentLabel);
			this.Controls.Add(this.mNameTextfield);
			this.Controls.Add(this.mNameLabel);
			this.Controls.Add(this.mCancelButton);
			this.Controls.Add(this.mOKButton);
			this.Controls.Add(this.mCommentTextbox);
			this.Controls.Add(this.mQueryList);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "QueryBatch";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Query Batch";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.CheckedListBox mQueryList;
		private System.Windows.Forms.TextBox mCommentTextbox;
		private System.Windows.Forms.Button mOKButton;
		private System.Windows.Forms.Button mCancelButton;
		private System.Windows.Forms.Label mNameLabel;
		private System.Windows.Forms.TextBox mNameTextfield;
		private System.Windows.Forms.Label mCommentLabel;
		private System.Windows.Forms.Label mQueryLabel;
		private System.Windows.Forms.OpenFileDialog mOpenFileDialog;
	}
}