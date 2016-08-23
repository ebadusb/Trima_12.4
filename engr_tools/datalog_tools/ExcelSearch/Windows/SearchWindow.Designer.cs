namespace ExcelSearch.Windows {
	partial class SearchWindow {
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
			this.mFirstButton = new System.Windows.Forms.Button();
			this.mSearchGroupbox = new System.Windows.Forms.GroupBox();
			this.mLinkRowsCheckbox = new System.Windows.Forms.CheckBox();
			this.mSearchAreaLabel = new System.Windows.Forms.Label();
			this.mSearchAreaTextfield = new System.Windows.Forms.TextBox();
			this.mExpressionRadio = new System.Windows.Forms.RadioButton();
			this.mWordRadio = new System.Windows.Forms.RadioButton();
			this.mMatchWordCheckbox = new System.Windows.Forms.CheckBox();
			this.mMatchCaseCheckbox = new System.Windows.Forms.CheckBox();
			this.mResultsGroupbox = new System.Windows.Forms.GroupBox();
			this.mSortCheckbox = new System.Windows.Forms.CheckBox();
			this.mColumnsLabel = new System.Windows.Forms.Label();
			this.mRowsLabel = new System.Windows.Forms.Label();
			this.mHyperlinkSrcCheckbox = new System.Windows.Forms.CheckBox();
			this.mHyperlinkLabel = new System.Windows.Forms.Label();
			this.mHighlightDstRowCheckbox = new System.Windows.Forms.CheckBox();
			this.mHighlightRowLabel = new System.Windows.Forms.Label();
			this.mHighlightCellLabel = new System.Windows.Forms.Label();
			this.mHighlightDstCellCheckbox = new System.Windows.Forms.CheckBox();
			this.mResizeCheckbox = new System.Windows.Forms.CheckBox();
			this.mColorRowButton = new System.Windows.Forms.Button();
			this.mColorCellButton = new System.Windows.Forms.Button();
			this.mHighlightSrcRowCheckbox = new System.Windows.Forms.CheckBox();
			this.mDestinationLabel = new System.Windows.Forms.Label();
			this.mDestinationTextfield = new System.Windows.Forms.TextBox();
			this.mHyperlinkDstCheckbox = new System.Windows.Forms.CheckBox();
			this.mClearCheckbox = new System.Windows.Forms.CheckBox();
			this.mRowNumCheckbox = new System.Windows.Forms.CheckBox();
			this.mHighlightSrcCellCheckbox = new System.Windows.Forms.CheckBox();
			this.mSecondButton = new System.Windows.Forms.Button();
			this.mColorDialog = new System.Windows.Forms.ColorDialog();
			this.mSearchList = new System.Windows.Forms.ComboBox();
			this.mDescriptionTextfield = new System.Windows.Forms.TextBox();
			this.mDescriptionList = new System.Windows.Forms.ComboBox();
			this.mSearchTextfield = new System.Windows.Forms.TextBox();
			this.mSearchGroupbox.SuspendLayout();
			this.mResultsGroupbox.SuspendLayout();
			this.SuspendLayout();
			// 
			// mFirstButton
			// 
			this.mFirstButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mFirstButton.Location = new System.Drawing.Point(229, 10);
			this.mFirstButton.Name = "mFirstButton";
			this.mFirstButton.Size = new System.Drawing.Size(75, 23);
			this.mFirstButton.TabIndex = 2;
			this.mFirstButton.Text = "Search";
			this.mFirstButton.UseVisualStyleBackColor = true;
			// 
			// mSearchGroupbox
			// 
			this.mSearchGroupbox.Controls.Add(this.mLinkRowsCheckbox);
			this.mSearchGroupbox.Controls.Add(this.mSearchAreaLabel);
			this.mSearchGroupbox.Controls.Add(this.mSearchAreaTextfield);
			this.mSearchGroupbox.Controls.Add(this.mExpressionRadio);
			this.mSearchGroupbox.Controls.Add(this.mWordRadio);
			this.mSearchGroupbox.Controls.Add(this.mMatchWordCheckbox);
			this.mSearchGroupbox.Controls.Add(this.mMatchCaseCheckbox);
			this.mSearchGroupbox.Location = new System.Drawing.Point(11, 71);
			this.mSearchGroupbox.Name = "mSearchGroupbox";
			this.mSearchGroupbox.Size = new System.Drawing.Size(291, 119);
			this.mSearchGroupbox.TabIndex = 6;
			this.mSearchGroupbox.TabStop = false;
			this.mSearchGroupbox.Text = "Search Options";
			// 
			// mLinkRowsCheckbox
			// 
			this.mLinkRowsCheckbox.AutoSize = true;
			this.mLinkRowsCheckbox.Location = new System.Drawing.Point(42, 92);
			this.mLinkRowsCheckbox.Name = "mLinkRowsCheckbox";
			this.mLinkRowsCheckbox.Size = new System.Drawing.Size(139, 17);
			this.mLinkRowsCheckbox.TabIndex = 6;
			this.mLinkRowsCheckbox.Text = "Link Rows from Ranges";
			this.mLinkRowsCheckbox.UseVisualStyleBackColor = true;
			// 
			// mSearchAreaLabel
			// 
			this.mSearchAreaLabel.AutoSize = true;
			this.mSearchAreaLabel.Location = new System.Drawing.Point(4, 69);
			this.mSearchAreaLabel.Name = "mSearchAreaLabel";
			this.mSearchAreaLabel.Size = new System.Drawing.Size(32, 13);
			this.mSearchAreaLabel.TabIndex = 4;
			this.mSearchAreaLabel.Text = "Area:";
			// 
			// mSearchAreaTextfield
			// 
			this.mSearchAreaTextfield.Location = new System.Drawing.Point(42, 66);
			this.mSearchAreaTextfield.Name = "mSearchAreaTextfield";
			this.mSearchAreaTextfield.Size = new System.Drawing.Size(233, 20);
			this.mSearchAreaTextfield.TabIndex = 5;
			// 
			// mExpressionRadio
			// 
			this.mExpressionRadio.AutoSize = true;
			this.mExpressionRadio.Location = new System.Drawing.Point(7, 43);
			this.mExpressionRadio.Name = "mExpressionRadio";
			this.mExpressionRadio.Size = new System.Drawing.Size(116, 17);
			this.mExpressionRadio.TabIndex = 3;
			this.mExpressionRadio.Text = "Regular Expresions";
			this.mExpressionRadio.UseVisualStyleBackColor = true;
			this.mExpressionRadio.CheckedChanged += new System.EventHandler(this.mExpressionRadio_CheckedChanged);
			// 
			// mWordRadio
			// 
			this.mWordRadio.AutoSize = true;
			this.mWordRadio.Checked = true;
			this.mWordRadio.Location = new System.Drawing.Point(7, 19);
			this.mWordRadio.Name = "mWordRadio";
			this.mWordRadio.Size = new System.Drawing.Size(88, 17);
			this.mWordRadio.TabIndex = 0;
			this.mWordRadio.TabStop = true;
			this.mWordRadio.Text = "Word Search";
			this.mWordRadio.UseVisualStyleBackColor = true;
			this.mWordRadio.CheckedChanged += new System.EventHandler(this.mWordRadio_CheckedChanged);
			// 
			// mMatchWordCheckbox
			// 
			this.mMatchWordCheckbox.AutoSize = true;
			this.mMatchWordCheckbox.Location = new System.Drawing.Point(158, 44);
			this.mMatchWordCheckbox.Name = "mMatchWordCheckbox";
			this.mMatchWordCheckbox.Size = new System.Drawing.Size(119, 17);
			this.mMatchWordCheckbox.TabIndex = 2;
			this.mMatchWordCheckbox.Text = "Match Whole Word";
			this.mMatchWordCheckbox.UseVisualStyleBackColor = true;
			// 
			// mMatchCaseCheckbox
			// 
			this.mMatchCaseCheckbox.AutoSize = true;
			this.mMatchCaseCheckbox.Location = new System.Drawing.Point(158, 20);
			this.mMatchCaseCheckbox.Name = "mMatchCaseCheckbox";
			this.mMatchCaseCheckbox.Size = new System.Drawing.Size(83, 17);
			this.mMatchCaseCheckbox.TabIndex = 1;
			this.mMatchCaseCheckbox.Text = "Match Case";
			this.mMatchCaseCheckbox.UseVisualStyleBackColor = true;
			// 
			// mResultsGroupbox
			// 
			this.mResultsGroupbox.Controls.Add(this.mSortCheckbox);
			this.mResultsGroupbox.Controls.Add(this.mColumnsLabel);
			this.mResultsGroupbox.Controls.Add(this.mRowsLabel);
			this.mResultsGroupbox.Controls.Add(this.mHyperlinkSrcCheckbox);
			this.mResultsGroupbox.Controls.Add(this.mHyperlinkLabel);
			this.mResultsGroupbox.Controls.Add(this.mHighlightDstRowCheckbox);
			this.mResultsGroupbox.Controls.Add(this.mHighlightRowLabel);
			this.mResultsGroupbox.Controls.Add(this.mHighlightCellLabel);
			this.mResultsGroupbox.Controls.Add(this.mHighlightDstCellCheckbox);
			this.mResultsGroupbox.Controls.Add(this.mResizeCheckbox);
			this.mResultsGroupbox.Controls.Add(this.mColorRowButton);
			this.mResultsGroupbox.Controls.Add(this.mColorCellButton);
			this.mResultsGroupbox.Controls.Add(this.mHighlightSrcRowCheckbox);
			this.mResultsGroupbox.Controls.Add(this.mDestinationLabel);
			this.mResultsGroupbox.Controls.Add(this.mDestinationTextfield);
			this.mResultsGroupbox.Controls.Add(this.mHyperlinkDstCheckbox);
			this.mResultsGroupbox.Controls.Add(this.mClearCheckbox);
			this.mResultsGroupbox.Controls.Add(this.mRowNumCheckbox);
			this.mResultsGroupbox.Controls.Add(this.mHighlightSrcCellCheckbox);
			this.mResultsGroupbox.Location = new System.Drawing.Point(11, 196);
			this.mResultsGroupbox.Name = "mResultsGroupbox";
			this.mResultsGroupbox.Size = new System.Drawing.Size(291, 187);
			this.mResultsGroupbox.TabIndex = 7;
			this.mResultsGroupbox.TabStop = false;
			this.mResultsGroupbox.Text = "Result Options";
			// 
			// mSortCheckbox
			// 
			this.mSortCheckbox.AutoSize = true;
			this.mSortCheckbox.Enabled = false;
			this.mSortCheckbox.Location = new System.Drawing.Point(158, 112);
			this.mSortCheckbox.Name = "mSortCheckbox";
			this.mSortCheckbox.Size = new System.Drawing.Size(99, 17);
			this.mSortCheckbox.TabIndex = 15;
			this.mSortCheckbox.Text = "Sort by Indexes";
			this.mSortCheckbox.UseVisualStyleBackColor = true;
			this.mSortCheckbox.CheckedChanged += new System.EventHandler(this.mSortCheckbox_CheckedChanged);
			// 
			// mColumnsLabel
			// 
			this.mColumnsLabel.AutoSize = true;
			this.mColumnsLabel.Location = new System.Drawing.Point(6, 113);
			this.mColumnsLabel.Name = "mColumnsLabel";
			this.mColumnsLabel.Size = new System.Drawing.Size(50, 13);
			this.mColumnsLabel.TabIndex = 13;
			this.mColumnsLabel.Text = "Columns:";
			// 
			// mRowsLabel
			// 
			this.mRowsLabel.AutoSize = true;
			this.mRowsLabel.Location = new System.Drawing.Point(6, 90);
			this.mRowsLabel.Name = "mRowsLabel";
			this.mRowsLabel.Size = new System.Drawing.Size(37, 13);
			this.mRowsLabel.TabIndex = 11;
			this.mRowsLabel.Text = "Rows:";
			// 
			// mHyperlinkSrcCheckbox
			// 
			this.mHyperlinkSrcCheckbox.AutoSize = true;
			this.mHyperlinkSrcCheckbox.Location = new System.Drawing.Point(88, 66);
			this.mHyperlinkSrcCheckbox.Name = "mHyperlinkSrcCheckbox";
			this.mHyperlinkSrcCheckbox.Size = new System.Drawing.Size(60, 17);
			this.mHyperlinkSrcCheckbox.TabIndex = 9;
			this.mHyperlinkSrcCheckbox.Text = "Source";
			this.mHyperlinkSrcCheckbox.UseVisualStyleBackColor = true;
			// 
			// mHyperlinkLabel
			// 
			this.mHyperlinkLabel.AutoSize = true;
			this.mHyperlinkLabel.Location = new System.Drawing.Point(6, 68);
			this.mHyperlinkLabel.Name = "mHyperlinkLabel";
			this.mHyperlinkLabel.Size = new System.Drawing.Size(54, 13);
			this.mHyperlinkLabel.TabIndex = 8;
			this.mHyperlinkLabel.Text = "Hyperlink:";
			// 
			// mHighlightDstRowCheckbox
			// 
			this.mHighlightDstRowCheckbox.AutoSize = true;
			this.mHighlightDstRowCheckbox.Location = new System.Drawing.Point(158, 44);
			this.mHighlightDstRowCheckbox.Name = "mHighlightDstRowCheckbox";
			this.mHighlightDstRowCheckbox.Size = new System.Drawing.Size(79, 17);
			this.mHighlightDstRowCheckbox.TabIndex = 6;
			this.mHighlightDstRowCheckbox.Text = "Destination";
			this.mHighlightDstRowCheckbox.UseVisualStyleBackColor = true;
			// 
			// mHighlightRowLabel
			// 
			this.mHighlightRowLabel.AutoSize = true;
			this.mHighlightRowLabel.Location = new System.Drawing.Point(6, 44);
			this.mHighlightRowLabel.Name = "mHighlightRowLabel";
			this.mHighlightRowLabel.Size = new System.Drawing.Size(76, 13);
			this.mHighlightRowLabel.TabIndex = 4;
			this.mHighlightRowLabel.Text = "Highlight Row:";
			// 
			// mHighlightCellLabel
			// 
			this.mHighlightCellLabel.AutoSize = true;
			this.mHighlightCellLabel.Location = new System.Drawing.Point(6, 20);
			this.mHighlightCellLabel.Name = "mHighlightCellLabel";
			this.mHighlightCellLabel.Size = new System.Drawing.Size(71, 13);
			this.mHighlightCellLabel.TabIndex = 0;
			this.mHighlightCellLabel.Text = "Highlight Cell:";
			// 
			// mHighlightDstCellCheckbox
			// 
			this.mHighlightDstCellCheckbox.AutoSize = true;
			this.mHighlightDstCellCheckbox.Location = new System.Drawing.Point(158, 19);
			this.mHighlightDstCellCheckbox.Name = "mHighlightDstCellCheckbox";
			this.mHighlightDstCellCheckbox.Size = new System.Drawing.Size(79, 17);
			this.mHighlightDstCellCheckbox.TabIndex = 2;
			this.mHighlightDstCellCheckbox.Text = "Destination";
			this.mHighlightDstCellCheckbox.UseVisualStyleBackColor = true;
			// 
			// mResizeCheckbox
			// 
			this.mResizeCheckbox.AutoSize = true;
			this.mResizeCheckbox.Location = new System.Drawing.Point(88, 112);
			this.mResizeCheckbox.Name = "mResizeCheckbox";
			this.mResizeCheckbox.Size = new System.Drawing.Size(58, 17);
			this.mResizeCheckbox.TabIndex = 14;
			this.mResizeCheckbox.Text = "Resize";
			this.mResizeCheckbox.UseVisualStyleBackColor = true;
			// 
			// mColorRowButton
			// 
			this.mColorRowButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
			this.mColorRowButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.mColorRowButton.Location = new System.Drawing.Point(255, 39);
			this.mColorRowButton.Name = "mColorRowButton";
			this.mColorRowButton.Size = new System.Drawing.Size(20, 20);
			this.mColorRowButton.TabIndex = 7;
			this.mColorRowButton.UseVisualStyleBackColor = false;
			this.mColorRowButton.Click += new System.EventHandler(this.mColorRowButton_Click);
			// 
			// mColorCellButton
			// 
			this.mColorCellButton.BackColor = System.Drawing.Color.Yellow;
			this.mColorCellButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.mColorCellButton.Location = new System.Drawing.Point(255, 16);
			this.mColorCellButton.Name = "mColorCellButton";
			this.mColorCellButton.Size = new System.Drawing.Size(20, 20);
			this.mColorCellButton.TabIndex = 3;
			this.mColorCellButton.UseVisualStyleBackColor = false;
			this.mColorCellButton.Click += new System.EventHandler(this.mColorCellButton_Click);
			// 
			// mHighlightSrcRowCheckbox
			// 
			this.mHighlightSrcRowCheckbox.AutoSize = true;
			this.mHighlightSrcRowCheckbox.Location = new System.Drawing.Point(88, 44);
			this.mHighlightSrcRowCheckbox.Name = "mHighlightSrcRowCheckbox";
			this.mHighlightSrcRowCheckbox.Size = new System.Drawing.Size(60, 17);
			this.mHighlightSrcRowCheckbox.TabIndex = 5;
			this.mHighlightSrcRowCheckbox.Text = "Source";
			this.mHighlightSrcRowCheckbox.UseVisualStyleBackColor = true;
			// 
			// mDestinationLabel
			// 
			this.mDestinationLabel.AutoSize = true;
			this.mDestinationLabel.Location = new System.Drawing.Point(6, 138);
			this.mDestinationLabel.Name = "mDestinationLabel";
			this.mDestinationLabel.Size = new System.Drawing.Size(63, 13);
			this.mDestinationLabel.TabIndex = 16;
			this.mDestinationLabel.Text = "Destination:";
			// 
			// mDestinationTextfield
			// 
			this.mDestinationTextfield.Location = new System.Drawing.Point(75, 135);
			this.mDestinationTextfield.Name = "mDestinationTextfield";
			this.mDestinationTextfield.Size = new System.Drawing.Size(200, 20);
			this.mDestinationTextfield.TabIndex = 17;
			// 
			// mHyperlinkDstCheckbox
			// 
			this.mHyperlinkDstCheckbox.AutoSize = true;
			this.mHyperlinkDstCheckbox.Location = new System.Drawing.Point(158, 67);
			this.mHyperlinkDstCheckbox.Name = "mHyperlinkDstCheckbox";
			this.mHyperlinkDstCheckbox.Size = new System.Drawing.Size(79, 17);
			this.mHyperlinkDstCheckbox.TabIndex = 10;
			this.mHyperlinkDstCheckbox.Text = "Destination";
			this.mHyperlinkDstCheckbox.UseVisualStyleBackColor = true;
			// 
			// mClearCheckbox
			// 
			this.mClearCheckbox.AutoSize = true;
			this.mClearCheckbox.Location = new System.Drawing.Point(75, 161);
			this.mClearCheckbox.Name = "mClearCheckbox";
			this.mClearCheckbox.Size = new System.Drawing.Size(132, 17);
			this.mClearCheckbox.TabIndex = 18;
			this.mClearCheckbox.Text = "Clear Previous Results";
			this.mClearCheckbox.UseVisualStyleBackColor = true;
			// 
			// mRowNumCheckbox
			// 
			this.mRowNumCheckbox.AutoSize = true;
			this.mRowNumCheckbox.Location = new System.Drawing.Point(88, 89);
			this.mRowNumCheckbox.Name = "mRowNumCheckbox";
			this.mRowNumCheckbox.Size = new System.Drawing.Size(64, 17);
			this.mRowNumCheckbox.TabIndex = 12;
			this.mRowNumCheckbox.Text = "Indexed";
			this.mRowNumCheckbox.UseVisualStyleBackColor = true;
			this.mRowNumCheckbox.CheckedChanged += new System.EventHandler(this.mRowNumCheckbox_CheckedChanged);
			// 
			// mHighlightSrcCellCheckbox
			// 
			this.mHighlightSrcCellCheckbox.AutoSize = true;
			this.mHighlightSrcCellCheckbox.Location = new System.Drawing.Point(88, 19);
			this.mHighlightSrcCellCheckbox.Name = "mHighlightSrcCellCheckbox";
			this.mHighlightSrcCellCheckbox.Size = new System.Drawing.Size(60, 17);
			this.mHighlightSrcCellCheckbox.TabIndex = 1;
			this.mHighlightSrcCellCheckbox.Text = "Source";
			this.mHighlightSrcCellCheckbox.UseVisualStyleBackColor = true;
			// 
			// mSecondButton
			// 
			this.mSecondButton.Location = new System.Drawing.Point(229, 36);
			this.mSecondButton.Name = "mSecondButton";
			this.mSecondButton.Size = new System.Drawing.Size(75, 23);
			this.mSecondButton.TabIndex = 5;
			this.mSecondButton.Text = "Save";
			this.mSecondButton.UseVisualStyleBackColor = true;
			// 
			// mSearchList
			// 
			this.mSearchList.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
			this.mSearchList.FormattingEnabled = true;
			this.mSearchList.Location = new System.Drawing.Point(12, 11);
			this.mSearchList.Name = "mSearchList";
			this.mSearchList.Size = new System.Drawing.Size(210, 21);
			this.mSearchList.TabIndex = 0;
			this.mSearchList.SelectedIndexChanged += new System.EventHandler(this.mSearchTextfield_SelectedIndexChanged);
			// 
			// mDescriptionTextfield
			// 
			this.mDescriptionTextfield.Location = new System.Drawing.Point(12, 38);
			this.mDescriptionTextfield.Name = "mDescriptionTextfield";
			this.mDescriptionTextfield.Size = new System.Drawing.Size(211, 20);
			this.mDescriptionTextfield.TabIndex = 4;
			// 
			// mDescriptionList
			// 
			this.mDescriptionList.FormattingEnabled = true;
			this.mDescriptionList.Location = new System.Drawing.Point(12, 38);
			this.mDescriptionList.Name = "mDescriptionList";
			this.mDescriptionList.Size = new System.Drawing.Size(210, 21);
			this.mDescriptionList.TabIndex = 3;
			this.mDescriptionList.SelectedIndexChanged += new System.EventHandler(this.mDescriptionList_SelectedIndexChanged);
			// 
			// mSearchTextfield
			// 
			this.mSearchTextfield.Location = new System.Drawing.Point(12, 11);
			this.mSearchTextfield.Name = "mSearchTextfield";
			this.mSearchTextfield.Size = new System.Drawing.Size(211, 20);
			this.mSearchTextfield.TabIndex = 1;
			// 
			// SearchWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(316, 398);
			this.Controls.Add(this.mSearchTextfield);
			this.Controls.Add(this.mDescriptionTextfield);
			this.Controls.Add(this.mDescriptionList);
			this.Controls.Add(this.mSearchList);
			this.Controls.Add(this.mSecondButton);
			this.Controls.Add(this.mResultsGroupbox);
			this.Controls.Add(this.mSearchGroupbox);
			this.Controls.Add(this.mFirstButton);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "SearchWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Extended Search";
			this.mSearchGroupbox.ResumeLayout(false);
			this.mSearchGroupbox.PerformLayout();
			this.mResultsGroupbox.ResumeLayout(false);
			this.mResultsGroupbox.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ColorDialog mColorDialog;
		protected System.Windows.Forms.Button mFirstButton;
		protected System.Windows.Forms.GroupBox mSearchGroupbox;
		protected System.Windows.Forms.GroupBox mResultsGroupbox;
		protected System.Windows.Forms.Button mSecondButton;
		protected System.Windows.Forms.ComboBox mSearchList;
		protected System.Windows.Forms.TextBox mDescriptionTextfield;
		protected System.Windows.Forms.CheckBox mMatchWordCheckbox;
		protected System.Windows.Forms.CheckBox mMatchCaseCheckbox;
		protected System.Windows.Forms.RadioButton mExpressionRadio;
		protected System.Windows.Forms.RadioButton mWordRadio;
		protected System.Windows.Forms.CheckBox mRowNumCheckbox;
		protected System.Windows.Forms.CheckBox mHighlightSrcCellCheckbox;
		protected System.Windows.Forms.Label mSearchAreaLabel;
		protected System.Windows.Forms.TextBox mSearchAreaTextfield;
		protected System.Windows.Forms.CheckBox mHyperlinkDstCheckbox;
		protected System.Windows.Forms.CheckBox mClearCheckbox;
		protected System.Windows.Forms.Label mDestinationLabel;
		protected System.Windows.Forms.TextBox mDestinationTextfield;
		protected System.Windows.Forms.CheckBox mLinkRowsCheckbox;
		protected System.Windows.Forms.CheckBox mHighlightSrcRowCheckbox;
		protected System.Windows.Forms.CheckBox mResizeCheckbox;
		protected System.Windows.Forms.Button mColorRowButton;
		protected System.Windows.Forms.Button mColorCellButton;
		protected System.Windows.Forms.Label mHighlightRowLabel;
		protected System.Windows.Forms.Label mHighlightCellLabel;
		protected System.Windows.Forms.CheckBox mHighlightDstCellCheckbox;
		protected System.Windows.Forms.CheckBox mHighlightDstRowCheckbox;
		protected System.Windows.Forms.Label mHyperlinkLabel;
		protected System.Windows.Forms.CheckBox mHyperlinkSrcCheckbox;
		protected System.Windows.Forms.Label mRowsLabel;
		protected System.Windows.Forms.Label mColumnsLabel;
		protected System.Windows.Forms.CheckBox mSortCheckbox;
		protected System.Windows.Forms.ComboBox mDescriptionList;
		protected System.Windows.Forms.TextBox mSearchTextfield;
	}
}