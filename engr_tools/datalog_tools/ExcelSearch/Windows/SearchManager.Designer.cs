namespace ExcelSearch.Windows {
	partial class SearchManager {
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
			this.mNewButton = new System.Windows.Forms.Button();
			this.mDeleteButton = new System.Windows.Forms.Button();
			this.mExecuteButton = new System.Windows.Forms.Button();
			this.mItemList = new System.Windows.Forms.ListBox();
			this.mImportButton = new System.Windows.Forms.Button();
			this.mExportButton = new System.Windows.Forms.Button();
			this.mOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.mSaveFileDialog = new System.Windows.Forms.SaveFileDialog();
			this.mEditButton = new System.Windows.Forms.Button();
			this.mCloseButton = new System.Windows.Forms.Button();
			this.mTypeList = new System.Windows.Forms.ComboBox();
			this.SuspendLayout();
			// 
			// mNewButton
			// 
			this.mNewButton.Location = new System.Drawing.Point(249, 39);
			this.mNewButton.Name = "mNewButton";
			this.mNewButton.Size = new System.Drawing.Size(75, 23);
			this.mNewButton.TabIndex = 4;
			this.mNewButton.Text = "New...";
			this.mNewButton.UseVisualStyleBackColor = true;
			this.mNewButton.Click += new System.EventHandler(this.NewButtonHandler);
			// 
			// mDeleteButton
			// 
			this.mDeleteButton.Location = new System.Drawing.Point(249, 97);
			this.mDeleteButton.Name = "mDeleteButton";
			this.mDeleteButton.Size = new System.Drawing.Size(75, 23);
			this.mDeleteButton.TabIndex = 6;
			this.mDeleteButton.Text = "Delete";
			this.mDeleteButton.UseVisualStyleBackColor = true;
			this.mDeleteButton.Click += new System.EventHandler(this.DeleteButtonHandler);
			// 
			// mExecuteButton
			// 
			this.mExecuteButton.Location = new System.Drawing.Point(248, 184);
			this.mExecuteButton.Name = "mExecuteButton";
			this.mExecuteButton.Size = new System.Drawing.Size(75, 23);
			this.mExecuteButton.TabIndex = 0;
			this.mExecuteButton.Text = "Execute";
			this.mExecuteButton.UseVisualStyleBackColor = true;
			this.mExecuteButton.Click += new System.EventHandler(this.ExecuteButtonHandler);
			// 
			// mItemList
			// 
			this.mItemList.FormattingEnabled = true;
			this.mItemList.Location = new System.Drawing.Point(12, 39);
			this.mItemList.Name = "mItemList";
			this.mItemList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.mItemList.Size = new System.Drawing.Size(230, 199);
			this.mItemList.Sorted = true;
			this.mItemList.TabIndex = 3;
			this.mItemList.SelectedIndexChanged += new System.EventHandler(this.ButtonStateHandler);
			// 
			// mImportButton
			// 
			this.mImportButton.Location = new System.Drawing.Point(248, 126);
			this.mImportButton.Name = "mImportButton";
			this.mImportButton.Size = new System.Drawing.Size(75, 23);
			this.mImportButton.TabIndex = 7;
			this.mImportButton.Text = "Import...";
			this.mImportButton.UseVisualStyleBackColor = true;
			this.mImportButton.Click += new System.EventHandler(this.ImportButtonHandler);
			// 
			// mExportButton
			// 
			this.mExportButton.Location = new System.Drawing.Point(248, 155);
			this.mExportButton.Name = "mExportButton";
			this.mExportButton.Size = new System.Drawing.Size(75, 23);
			this.mExportButton.TabIndex = 8;
			this.mExportButton.Text = "Export...";
			this.mExportButton.UseVisualStyleBackColor = true;
			this.mExportButton.Click += new System.EventHandler(this.ExportButtonHandler);
			// 
			// mOpenFileDialog
			// 
			this.mOpenFileDialog.Filter = "XML Files|*.xml";
			this.mOpenFileDialog.Title = "Import File";
			// 
			// mSaveFileDialog
			// 
			this.mSaveFileDialog.Filter = "XML Files|*.xml";
			this.mSaveFileDialog.Title = "Export File";
			// 
			// mEditButton
			// 
			this.mEditButton.Location = new System.Drawing.Point(249, 68);
			this.mEditButton.Name = "mEditButton";
			this.mEditButton.Size = new System.Drawing.Size(75, 23);
			this.mEditButton.TabIndex = 5;
			this.mEditButton.Text = "Edit...";
			this.mEditButton.UseVisualStyleBackColor = true;
			this.mEditButton.Click += new System.EventHandler(this.EditButtonHandler);
			// 
			// mCloseButton
			// 
			this.mCloseButton.Location = new System.Drawing.Point(249, 213);
			this.mCloseButton.Name = "mCloseButton";
			this.mCloseButton.Size = new System.Drawing.Size(75, 23);
			this.mCloseButton.TabIndex = 1;
			this.mCloseButton.Text = "Close";
			this.mCloseButton.UseVisualStyleBackColor = true;
			this.mCloseButton.Click += new System.EventHandler(this.CloseButtonHandler);
			// 
			// mTypeList
			// 
			this.mTypeList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.mTypeList.FormattingEnabled = true;
			this.mTypeList.Items.AddRange(new object[] {
            "Saved Searches",
            "Batch Searches"});
			this.mTypeList.Location = new System.Drawing.Point(12, 12);
			this.mTypeList.Name = "mTypeList";
			this.mTypeList.Size = new System.Drawing.Size(230, 21);
			this.mTypeList.TabIndex = 9;
			this.mTypeList.SelectedIndexChanged += new System.EventHandler(this.TypeListHandler);
			// 
			// SearchManager
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(334, 251);
			this.Controls.Add(this.mTypeList);
			this.Controls.Add(this.mCloseButton);
			this.Controls.Add(this.mEditButton);
			this.Controls.Add(this.mExportButton);
			this.Controls.Add(this.mImportButton);
			this.Controls.Add(this.mItemList);
			this.Controls.Add(this.mExecuteButton);
			this.Controls.Add(this.mDeleteButton);
			this.Controls.Add(this.mNewButton);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "SearchManager";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Search Manager";
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Button mNewButton;
		private System.Windows.Forms.Button mDeleteButton;
		private System.Windows.Forms.Button mExecuteButton;
		private System.Windows.Forms.ListBox mItemList;
		private System.Windows.Forms.Button mImportButton;
		private System.Windows.Forms.Button mExportButton;
		private System.Windows.Forms.OpenFileDialog mOpenFileDialog;
		private System.Windows.Forms.SaveFileDialog mSaveFileDialog;
		private System.Windows.Forms.Button mEditButton;
		private System.Windows.Forms.Button mCloseButton;
		private System.Windows.Forms.ComboBox mTypeList;
	}
}