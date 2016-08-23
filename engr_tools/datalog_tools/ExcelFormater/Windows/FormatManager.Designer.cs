namespace ExcelFormater.Windows {
	partial class FormatManager {
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
			this.mEditButton = new System.Windows.Forms.Button();
			this.mItemList = new System.Windows.Forms.ListBox();
			this.mDeleteButton = new System.Windows.Forms.Button();
			this.mNewButton = new System.Windows.Forms.Button();
			this.mCloseButton = new System.Windows.Forms.Button();
			this.mTypeList = new System.Windows.Forms.ComboBox();
			this.mOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.mSaveFileDialog = new System.Windows.Forms.SaveFileDialog();
			this.mImportButton = new System.Windows.Forms.Button();
			this.mExportButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// mEditButton
			// 
			this.mEditButton.Location = new System.Drawing.Point(248, 68);
			this.mEditButton.Name = "mEditButton";
			this.mEditButton.Size = new System.Drawing.Size(75, 23);
			this.mEditButton.TabIndex = 12;
			this.mEditButton.Text = "Edit...";
			this.mEditButton.UseVisualStyleBackColor = true;
			this.mEditButton.Click += new System.EventHandler(this.EditButtonHandler);
			// 
			// mItemList
			// 
			this.mItemList.FormattingEnabled = true;
			this.mItemList.Location = new System.Drawing.Point(12, 39);
			this.mItemList.Name = "mItemList";
			this.mItemList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.mItemList.Size = new System.Drawing.Size(230, 186);
			this.mItemList.Sorted = true;
			this.mItemList.TabIndex = 10;
			this.mItemList.SelectedIndexChanged += new System.EventHandler(this.ButtonStateHandler);
			// 
			// mDeleteButton
			// 
			this.mDeleteButton.Location = new System.Drawing.Point(248, 97);
			this.mDeleteButton.Name = "mDeleteButton";
			this.mDeleteButton.Size = new System.Drawing.Size(75, 23);
			this.mDeleteButton.TabIndex = 13;
			this.mDeleteButton.Text = "Delete";
			this.mDeleteButton.UseVisualStyleBackColor = true;
			this.mDeleteButton.Click += new System.EventHandler(this.DeleteButtonHandler);
			// 
			// mNewButton
			// 
			this.mNewButton.Location = new System.Drawing.Point(248, 39);
			this.mNewButton.Name = "mNewButton";
			this.mNewButton.Size = new System.Drawing.Size(75, 23);
			this.mNewButton.TabIndex = 11;
			this.mNewButton.Text = "New...";
			this.mNewButton.UseVisualStyleBackColor = true;
			this.mNewButton.Click += new System.EventHandler(this.NewButtonHandler);
			// 
			// mCloseButton
			// 
			this.mCloseButton.Location = new System.Drawing.Point(248, 202);
			this.mCloseButton.Name = "mCloseButton";
			this.mCloseButton.Size = new System.Drawing.Size(75, 23);
			this.mCloseButton.TabIndex = 15;
			this.mCloseButton.Text = "Close";
			this.mCloseButton.UseVisualStyleBackColor = true;
			this.mCloseButton.Click += new System.EventHandler(this.CloseButtonHandler);
			// 
			// mTypeList
			// 
			this.mTypeList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.mTypeList.FormattingEnabled = true;
			this.mTypeList.Items.AddRange(new object[] {
            "Saved Formats",
            "Column Filters",
            "Column Lists",
            "Column Macros",
            "Table Macros"});
			this.mTypeList.Location = new System.Drawing.Point(12, 12);
			this.mTypeList.Name = "mTypeList";
			this.mTypeList.Size = new System.Drawing.Size(230, 21);
			this.mTypeList.TabIndex = 16;
			this.mTypeList.SelectedIndexChanged += new System.EventHandler(this.TypeListHandler);
			// 
			// mImportButton
			// 
			this.mImportButton.Location = new System.Drawing.Point(248, 126);
			this.mImportButton.Name = "mImportButton";
			this.mImportButton.Size = new System.Drawing.Size(75, 23);
			this.mImportButton.TabIndex = 17;
			this.mImportButton.Text = "Import";
			this.mImportButton.UseVisualStyleBackColor = true;
			this.mImportButton.Click += new System.EventHandler(this.ImportButtonHandler);
			// 
			// mExportButton
			// 
			this.mExportButton.Location = new System.Drawing.Point(248, 155);
			this.mExportButton.Name = "mExportButton";
			this.mExportButton.Size = new System.Drawing.Size(75, 23);
			this.mExportButton.TabIndex = 18;
			this.mExportButton.Text = "Export";
			this.mExportButton.UseVisualStyleBackColor = true;
			this.mExportButton.Click += new System.EventHandler(this.ExportButtonHandler);
			// 
			// FormatManager
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(332, 239);
			this.Controls.Add(this.mExportButton);
			this.Controls.Add(this.mImportButton);
			this.Controls.Add(this.mTypeList);
			this.Controls.Add(this.mCloseButton);
			this.Controls.Add(this.mEditButton);
			this.Controls.Add(this.mItemList);
			this.Controls.Add(this.mDeleteButton);
			this.Controls.Add(this.mNewButton);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "FormatManager";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Format Manager";
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Button mEditButton;
		private System.Windows.Forms.ListBox mItemList;
		private System.Windows.Forms.Button mDeleteButton;
		private System.Windows.Forms.Button mNewButton;
		private System.Windows.Forms.Button mCloseButton;
		private System.Windows.Forms.ComboBox mTypeList;
		private System.Windows.Forms.OpenFileDialog mOpenFileDialog;
		private System.Windows.Forms.SaveFileDialog mSaveFileDialog;
		private System.Windows.Forms.Button mImportButton;
		private System.Windows.Forms.Button mExportButton;
	}
}