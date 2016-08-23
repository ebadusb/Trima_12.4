namespace ExcelFormater.Windows {
	partial class FormatWindow {
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
			this.mSecondButton = new System.Windows.Forms.Button();
			this.mColumCategoryLabel = new System.Windows.Forms.Label();
			this.mCancelButton = new System.Windows.Forms.Button();
			this.mNameLabel = new System.Windows.Forms.Label();
			this.mNameTextfield = new System.Windows.Forms.TextBox();
			this.mNameList = new System.Windows.Forms.ComboBox();
			this.mFirstButton = new System.Windows.Forms.Button();
			this.mDextinationLabel = new System.Windows.Forms.Label();
			this.mDestinationTextfield = new System.Windows.Forms.TextBox();
			this.mAvailableColumnList = new System.Windows.Forms.ListBox();
			this.mAvailableLevelList = new System.Windows.Forms.ListBox();
			this.mSelectedColumnList = new System.Windows.Forms.ListBox();
			this.mSelectedLevelList = new System.Windows.Forms.ListBox();
			this.mColumCategoryTextfield = new System.Windows.Forms.ComboBox();
			this.mSettingsGroupbox = new System.Windows.Forms.GroupBox();
			this.mColumnsGroupbox = new System.Windows.Forms.GroupBox();
			this.mColumnIgnoreCaseCheckbox = new System.Windows.Forms.CheckBox();
			this.mHideColumnCheckbox = new System.Windows.Forms.CheckBox();
			this.mDownColumnButton = new System.Windows.Forms.Button();
			this.mUpColumnButton = new System.Windows.Forms.Button();
			this.mExcludeColumnButton = new System.Windows.Forms.Button();
			this.mIncludeColumnButton = new System.Windows.Forms.Button();
			this.mColumnFilterTextfield = new System.Windows.Forms.TextBox();
			this.mColumFilterLabel = new System.Windows.Forms.Label();
			this.mLevelsGroupbox = new System.Windows.Forms.GroupBox();
			this.mLevelIgnoreCaseCheckbox = new System.Windows.Forms.CheckBox();
			this.mHideLevelCheckbox = new System.Windows.Forms.CheckBox();
			this.mAllLogLevels = new System.Windows.Forms.CheckBox();
			this.mExcludeLevelButton = new System.Windows.Forms.Button();
			this.mIncludeLevelButton = new System.Windows.Forms.Button();
			this.mLevelFilterTextfield = new System.Windows.Forms.TextBox();
			this.mLevelFilterLabel = new System.Windows.Forms.Label();
			this.mSortingGroupbox = new System.Windows.Forms.GroupBox();
			this.mSortIndexRadio = new System.Windows.Forms.RadioButton();
			this.mSortTimeRadio = new System.Windows.Forms.RadioButton();
			this.mSettingsGroupbox.SuspendLayout();
			this.mColumnsGroupbox.SuspendLayout();
			this.mLevelsGroupbox.SuspendLayout();
			this.mSortingGroupbox.SuspendLayout();
			this.SuspendLayout();
			// 
			// mSecondButton
			// 
			this.mSecondButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.mSecondButton.Location = new System.Drawing.Point(436, 362);
			this.mSecondButton.Name = "mSecondButton";
			this.mSecondButton.Size = new System.Drawing.Size(75, 23);
			this.mSecondButton.TabIndex = 3;
			this.mSecondButton.Text = "OK";
			this.mSecondButton.UseVisualStyleBackColor = true;
			// 
			// mColumCategoryLabel
			// 
			this.mColumCategoryLabel.AutoSize = true;
			this.mColumCategoryLabel.Location = new System.Drawing.Point(17, 22);
			this.mColumCategoryLabel.Name = "mColumCategoryLabel";
			this.mColumCategoryLabel.Size = new System.Drawing.Size(52, 13);
			this.mColumCategoryLabel.TabIndex = 4;
			this.mColumCategoryLabel.Text = "Category:";
			// 
			// mCancelButton
			// 
			this.mCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.mCancelButton.Location = new System.Drawing.Point(517, 362);
			this.mCancelButton.Name = "mCancelButton";
			this.mCancelButton.Size = new System.Drawing.Size(75, 23);
			this.mCancelButton.TabIndex = 9;
			this.mCancelButton.Text = "Cancel";
			this.mCancelButton.UseVisualStyleBackColor = true;
			// 
			// mNameLabel
			// 
			this.mNameLabel.AutoSize = true;
			this.mNameLabel.Location = new System.Drawing.Point(31, 22);
			this.mNameLabel.Name = "mNameLabel";
			this.mNameLabel.Size = new System.Drawing.Size(38, 13);
			this.mNameLabel.TabIndex = 13;
			this.mNameLabel.Text = "Name:";
			// 
			// mNameTextfield
			// 
			this.mNameTextfield.Location = new System.Drawing.Point(75, 19);
			this.mNameTextfield.Name = "mNameTextfield";
			this.mNameTextfield.Size = new System.Drawing.Size(277, 20);
			this.mNameTextfield.TabIndex = 14;
			// 
			// mNameList
			// 
			this.mNameList.FormattingEnabled = true;
			this.mNameList.Location = new System.Drawing.Point(75, 19);
			this.mNameList.Name = "mNameList";
			this.mNameList.Size = new System.Drawing.Size(277, 21);
			this.mNameList.Sorted = true;
			this.mNameList.TabIndex = 15;
			this.mNameList.SelectedIndexChanged += new System.EventHandler(this.NameListSelectionHandler);
			// 
			// mFirstButton
			// 
			this.mFirstButton.Location = new System.Drawing.Point(355, 362);
			this.mFirstButton.Name = "mFirstButton";
			this.mFirstButton.Size = new System.Drawing.Size(75, 23);
			this.mFirstButton.TabIndex = 16;
			this.mFirstButton.Text = "Save";
			this.mFirstButton.UseVisualStyleBackColor = true;
			// 
			// mDextinationLabel
			// 
			this.mDextinationLabel.AutoSize = true;
			this.mDextinationLabel.Location = new System.Drawing.Point(6, 49);
			this.mDextinationLabel.Name = "mDextinationLabel";
			this.mDextinationLabel.Size = new System.Drawing.Size(63, 13);
			this.mDextinationLabel.TabIndex = 17;
			this.mDextinationLabel.Text = "Destination:";
			// 
			// mDestinationTextfield
			// 
			this.mDestinationTextfield.Location = new System.Drawing.Point(75, 46);
			this.mDestinationTextfield.Name = "mDestinationTextfield";
			this.mDestinationTextfield.Size = new System.Drawing.Size(277, 20);
			this.mDestinationTextfield.TabIndex = 18;
			this.mDestinationTextfield.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.KeyPressHandler);
			// 
			// mAvailableColumnList
			// 
			this.mAvailableColumnList.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
			this.mAvailableColumnList.FormattingEnabled = true;
			this.mAvailableColumnList.Location = new System.Drawing.Point(202, 19);
			this.mAvailableColumnList.Name = "mAvailableColumnList";
			this.mAvailableColumnList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.mAvailableColumnList.Size = new System.Drawing.Size(150, 95);
			this.mAvailableColumnList.Sorted = true;
			this.mAvailableColumnList.TabIndex = 19;
			this.mAvailableColumnList.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.AvailableColumnClickHandler);
			this.mAvailableColumnList.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.DrawColumnListHandler);
			this.mAvailableColumnList.Enter += new System.EventHandler(this.AvailableColumnFocusEnter);
			// 
			// mAvailableLevelList
			// 
			this.mAvailableLevelList.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
			this.mAvailableLevelList.FormattingEnabled = true;
			this.mAvailableLevelList.Location = new System.Drawing.Point(202, 19);
			this.mAvailableLevelList.Name = "mAvailableLevelList";
			this.mAvailableLevelList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.mAvailableLevelList.Size = new System.Drawing.Size(150, 95);
			this.mAvailableLevelList.Sorted = true;
			this.mAvailableLevelList.TabIndex = 20;
			this.mAvailableLevelList.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.AvailableSelectedClickHandler);
			this.mAvailableLevelList.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.DrawLevelListHandler);
			this.mAvailableLevelList.Enter += new System.EventHandler(this.AvailableLevelFocusEnter);
			// 
			// mSelectedColumnList
			// 
			this.mSelectedColumnList.FormattingEnabled = true;
			this.mSelectedColumnList.Location = new System.Drawing.Point(390, 19);
			this.mSelectedColumnList.Name = "mSelectedColumnList";
			this.mSelectedColumnList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.mSelectedColumnList.Size = new System.Drawing.Size(150, 95);
			this.mSelectedColumnList.TabIndex = 21;
			this.mSelectedColumnList.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.SelectedColumnClickHandler);
			this.mSelectedColumnList.Enter += new System.EventHandler(this.SelectedColumnFocusEnter);
			// 
			// mSelectedLevelList
			// 
			this.mSelectedLevelList.FormattingEnabled = true;
			this.mSelectedLevelList.Location = new System.Drawing.Point(390, 19);
			this.mSelectedLevelList.Name = "mSelectedLevelList";
			this.mSelectedLevelList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.mSelectedLevelList.Size = new System.Drawing.Size(150, 95);
			this.mSelectedLevelList.Sorted = true;
			this.mSelectedLevelList.TabIndex = 22;
			this.mSelectedLevelList.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.SelectedLevelClickHandler);
			this.mSelectedLevelList.Enter += new System.EventHandler(this.SelectedLevelFocusEnter);
			// 
			// mColumCategoryTextfield
			// 
			this.mColumCategoryTextfield.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.mColumCategoryTextfield.FormattingEnabled = true;
			this.mColumCategoryTextfield.Location = new System.Drawing.Point(75, 19);
			this.mColumCategoryTextfield.Name = "mColumCategoryTextfield";
			this.mColumCategoryTextfield.Size = new System.Drawing.Size(121, 21);
			this.mColumCategoryTextfield.Sorted = true;
			this.mColumCategoryTextfield.TabIndex = 23;
			this.mColumCategoryTextfield.SelectedIndexChanged += new System.EventHandler(this.ColumnCategoryEvent);
			// 
			// mSettingsGroupbox
			// 
			this.mSettingsGroupbox.Controls.Add(this.mNameList);
			this.mSettingsGroupbox.Controls.Add(this.mNameLabel);
			this.mSettingsGroupbox.Controls.Add(this.mDestinationTextfield);
			this.mSettingsGroupbox.Controls.Add(this.mDextinationLabel);
			this.mSettingsGroupbox.Controls.Add(this.mNameTextfield);
			this.mSettingsGroupbox.Location = new System.Drawing.Point(12, 12);
			this.mSettingsGroupbox.Name = "mSettingsGroupbox";
			this.mSettingsGroupbox.Size = new System.Drawing.Size(366, 79);
			this.mSettingsGroupbox.TabIndex = 24;
			this.mSettingsGroupbox.TabStop = false;
			this.mSettingsGroupbox.Text = "Settings";
			// 
			// mColumnsGroupbox
			// 
			this.mColumnsGroupbox.Controls.Add(this.mColumnIgnoreCaseCheckbox);
			this.mColumnsGroupbox.Controls.Add(this.mHideColumnCheckbox);
			this.mColumnsGroupbox.Controls.Add(this.mDownColumnButton);
			this.mColumnsGroupbox.Controls.Add(this.mUpColumnButton);
			this.mColumnsGroupbox.Controls.Add(this.mExcludeColumnButton);
			this.mColumnsGroupbox.Controls.Add(this.mIncludeColumnButton);
			this.mColumnsGroupbox.Controls.Add(this.mColumnFilterTextfield);
			this.mColumnsGroupbox.Controls.Add(this.mColumFilterLabel);
			this.mColumnsGroupbox.Controls.Add(this.mColumCategoryTextfield);
			this.mColumnsGroupbox.Controls.Add(this.mColumCategoryLabel);
			this.mColumnsGroupbox.Controls.Add(this.mAvailableColumnList);
			this.mColumnsGroupbox.Controls.Add(this.mSelectedColumnList);
			this.mColumnsGroupbox.Location = new System.Drawing.Point(12, 98);
			this.mColumnsGroupbox.Name = "mColumnsGroupbox";
			this.mColumnsGroupbox.Size = new System.Drawing.Size(580, 126);
			this.mColumnsGroupbox.TabIndex = 25;
			this.mColumnsGroupbox.TabStop = false;
			this.mColumnsGroupbox.Text = "Columns";
			// 
			// mColumnIgnoreCaseCheckbox
			// 
			this.mColumnIgnoreCaseCheckbox.AutoSize = true;
			this.mColumnIgnoreCaseCheckbox.Checked = true;
			this.mColumnIgnoreCaseCheckbox.CheckState = System.Windows.Forms.CheckState.Checked;
			this.mColumnIgnoreCaseCheckbox.Location = new System.Drawing.Point(75, 74);
			this.mColumnIgnoreCaseCheckbox.Name = "mColumnIgnoreCaseCheckbox";
			this.mColumnIgnoreCaseCheckbox.Size = new System.Drawing.Size(83, 17);
			this.mColumnIgnoreCaseCheckbox.TabIndex = 31;
			this.mColumnIgnoreCaseCheckbox.Text = "Ignore Case";
			this.mColumnIgnoreCaseCheckbox.UseVisualStyleBackColor = true;
			this.mColumnIgnoreCaseCheckbox.CheckedChanged += new System.EventHandler(this.ReloadColumnHandler);
			// 
			// mHideColumnCheckbox
			// 
			this.mHideColumnCheckbox.AutoSize = true;
			this.mHideColumnCheckbox.Checked = true;
			this.mHideColumnCheckbox.CheckState = System.Windows.Forms.CheckState.Checked;
			this.mHideColumnCheckbox.Location = new System.Drawing.Point(75, 97);
			this.mHideColumnCheckbox.Name = "mHideColumnCheckbox";
			this.mHideColumnCheckbox.Size = new System.Drawing.Size(107, 17);
			this.mHideColumnCheckbox.TabIndex = 30;
			this.mHideColumnCheckbox.Text = "Hide Unavailable";
			this.mHideColumnCheckbox.UseVisualStyleBackColor = true;
			this.mHideColumnCheckbox.CheckedChanged += new System.EventHandler(this.ReloadColumnHandler);
			// 
			// mDownColumnButton
			// 
			this.mDownColumnButton.BackgroundImage = global::ExcelFormater.Properties.Resources.down_32x32;
			this.mDownColumnButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.mDownColumnButton.FlatAppearance.BorderSize = 0;
			this.mDownColumnButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.mDownColumnButton.Location = new System.Drawing.Point(546, 73);
			this.mDownColumnButton.Name = "mDownColumnButton";
			this.mDownColumnButton.Size = new System.Drawing.Size(25, 25);
			this.mDownColumnButton.TabIndex = 29;
			this.mDownColumnButton.UseVisualStyleBackColor = true;
			this.mDownColumnButton.Click += new System.EventHandler(this.DownColumnEvent);
			// 
			// mUpColumnButton
			// 
			this.mUpColumnButton.BackgroundImage = global::ExcelFormater.Properties.Resources.up_32x32;
			this.mUpColumnButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.mUpColumnButton.FlatAppearance.BorderSize = 0;
			this.mUpColumnButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.mUpColumnButton.Location = new System.Drawing.Point(546, 42);
			this.mUpColumnButton.Name = "mUpColumnButton";
			this.mUpColumnButton.Size = new System.Drawing.Size(25, 25);
			this.mUpColumnButton.TabIndex = 28;
			this.mUpColumnButton.UseVisualStyleBackColor = true;
			this.mUpColumnButton.Click += new System.EventHandler(this.UpColumnEvent);
			// 
			// mExcludeColumnButton
			// 
			this.mExcludeColumnButton.BackgroundImage = global::ExcelFormater.Properties.Resources.back_32x32;
			this.mExcludeColumnButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.mExcludeColumnButton.FlatAppearance.BorderSize = 0;
			this.mExcludeColumnButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.mExcludeColumnButton.Location = new System.Drawing.Point(359, 70);
			this.mExcludeColumnButton.Name = "mExcludeColumnButton";
			this.mExcludeColumnButton.Size = new System.Drawing.Size(25, 25);
			this.mExcludeColumnButton.TabIndex = 27;
			this.mExcludeColumnButton.UseVisualStyleBackColor = true;
			this.mExcludeColumnButton.Click += new System.EventHandler(this.ExcludeColumnEvent);
			// 
			// mIncludeColumnButton
			// 
			this.mIncludeColumnButton.BackgroundImage = global::ExcelFormater.Properties.Resources.next_32x32;
			this.mIncludeColumnButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.mIncludeColumnButton.FlatAppearance.BorderSize = 0;
			this.mIncludeColumnButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.mIncludeColumnButton.Location = new System.Drawing.Point(359, 39);
			this.mIncludeColumnButton.Name = "mIncludeColumnButton";
			this.mIncludeColumnButton.Size = new System.Drawing.Size(25, 25);
			this.mIncludeColumnButton.TabIndex = 26;
			this.mIncludeColumnButton.UseVisualStyleBackColor = true;
			this.mIncludeColumnButton.Click += new System.EventHandler(this.IncludeColumnEvent);
			// 
			// mColumnFilterTextfield
			// 
			this.mColumnFilterTextfield.Location = new System.Drawing.Point(75, 47);
			this.mColumnFilterTextfield.Name = "mColumnFilterTextfield";
			this.mColumnFilterTextfield.Size = new System.Drawing.Size(121, 20);
			this.mColumnFilterTextfield.TabIndex = 25;
			this.mColumnFilterTextfield.TextChanged += new System.EventHandler(this.ColumnFilterEvent);
			// 
			// mColumFilterLabel
			// 
			this.mColumFilterLabel.AutoSize = true;
			this.mColumFilterLabel.Location = new System.Drawing.Point(37, 50);
			this.mColumFilterLabel.Name = "mColumFilterLabel";
			this.mColumFilterLabel.Size = new System.Drawing.Size(32, 13);
			this.mColumFilterLabel.TabIndex = 24;
			this.mColumFilterLabel.Text = "Filter:";
			// 
			// mLevelsGroupbox
			// 
			this.mLevelsGroupbox.Controls.Add(this.mLevelIgnoreCaseCheckbox);
			this.mLevelsGroupbox.Controls.Add(this.mHideLevelCheckbox);
			this.mLevelsGroupbox.Controls.Add(this.mAllLogLevels);
			this.mLevelsGroupbox.Controls.Add(this.mExcludeLevelButton);
			this.mLevelsGroupbox.Controls.Add(this.mIncludeLevelButton);
			this.mLevelsGroupbox.Controls.Add(this.mLevelFilterTextfield);
			this.mLevelsGroupbox.Controls.Add(this.mLevelFilterLabel);
			this.mLevelsGroupbox.Controls.Add(this.mSelectedLevelList);
			this.mLevelsGroupbox.Controls.Add(this.mAvailableLevelList);
			this.mLevelsGroupbox.Location = new System.Drawing.Point(12, 230);
			this.mLevelsGroupbox.Name = "mLevelsGroupbox";
			this.mLevelsGroupbox.Size = new System.Drawing.Size(580, 126);
			this.mLevelsGroupbox.TabIndex = 26;
			this.mLevelsGroupbox.TabStop = false;
			this.mLevelsGroupbox.Text = "Levels";
			// 
			// mLevelIgnoreCaseCheckbox
			// 
			this.mLevelIgnoreCaseCheckbox.AutoSize = true;
			this.mLevelIgnoreCaseCheckbox.Checked = true;
			this.mLevelIgnoreCaseCheckbox.CheckState = System.Windows.Forms.CheckState.Checked;
			this.mLevelIgnoreCaseCheckbox.Location = new System.Drawing.Point(75, 68);
			this.mLevelIgnoreCaseCheckbox.Name = "mLevelIgnoreCaseCheckbox";
			this.mLevelIgnoreCaseCheckbox.Size = new System.Drawing.Size(83, 17);
			this.mLevelIgnoreCaseCheckbox.TabIndex = 32;
			this.mLevelIgnoreCaseCheckbox.Text = "Ignore Case";
			this.mLevelIgnoreCaseCheckbox.UseVisualStyleBackColor = true;
			this.mLevelIgnoreCaseCheckbox.CheckedChanged += new System.EventHandler(this.ReloadLevelHandler);
			// 
			// mHideLevelCheckbox
			// 
			this.mHideLevelCheckbox.AutoSize = true;
			this.mHideLevelCheckbox.Checked = true;
			this.mHideLevelCheckbox.CheckState = System.Windows.Forms.CheckState.Checked;
			this.mHideLevelCheckbox.Location = new System.Drawing.Point(75, 91);
			this.mHideLevelCheckbox.Name = "mHideLevelCheckbox";
			this.mHideLevelCheckbox.Size = new System.Drawing.Size(107, 17);
			this.mHideLevelCheckbox.TabIndex = 31;
			this.mHideLevelCheckbox.Text = "Hide Unavailable";
			this.mHideLevelCheckbox.UseVisualStyleBackColor = true;
			this.mHideLevelCheckbox.CheckedChanged += new System.EventHandler(this.ReloadLevelHandler);
			// 
			// mAllLogLevels
			// 
			this.mAllLogLevels.AutoSize = true;
			this.mAllLogLevels.Checked = true;
			this.mAllLogLevels.CheckState = System.Windows.Forms.CheckState.Checked;
			this.mAllLogLevels.Location = new System.Drawing.Point(75, 19);
			this.mAllLogLevels.Name = "mAllLogLevels";
			this.mAllLogLevels.Size = new System.Drawing.Size(92, 17);
			this.mAllLogLevels.TabIndex = 30;
			this.mAllLogLevels.Text = "All Log Levels";
			this.mAllLogLevels.UseVisualStyleBackColor = true;
			this.mAllLogLevels.CheckedChanged += new System.EventHandler(this.AllLogLevelsHandler);
			// 
			// mExcludeLevelButton
			// 
			this.mExcludeLevelButton.BackgroundImage = global::ExcelFormater.Properties.Resources.back_32x32;
			this.mExcludeLevelButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.mExcludeLevelButton.FlatAppearance.BorderSize = 0;
			this.mExcludeLevelButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.mExcludeLevelButton.Location = new System.Drawing.Point(359, 70);
			this.mExcludeLevelButton.Name = "mExcludeLevelButton";
			this.mExcludeLevelButton.Size = new System.Drawing.Size(25, 25);
			this.mExcludeLevelButton.TabIndex = 29;
			this.mExcludeLevelButton.UseVisualStyleBackColor = true;
			this.mExcludeLevelButton.Click += new System.EventHandler(this.ExcludeLevelEvent);
			// 
			// mIncludeLevelButton
			// 
			this.mIncludeLevelButton.BackgroundImage = global::ExcelFormater.Properties.Resources.next_32x32;
			this.mIncludeLevelButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.mIncludeLevelButton.FlatAppearance.BorderSize = 0;
			this.mIncludeLevelButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.mIncludeLevelButton.Location = new System.Drawing.Point(359, 39);
			this.mIncludeLevelButton.Name = "mIncludeLevelButton";
			this.mIncludeLevelButton.Size = new System.Drawing.Size(25, 25);
			this.mIncludeLevelButton.TabIndex = 28;
			this.mIncludeLevelButton.UseVisualStyleBackColor = true;
			this.mIncludeLevelButton.Click += new System.EventHandler(this.IncludeLevelEvent);
			// 
			// mLevelFilterTextfield
			// 
			this.mLevelFilterTextfield.Location = new System.Drawing.Point(75, 42);
			this.mLevelFilterTextfield.Name = "mLevelFilterTextfield";
			this.mLevelFilterTextfield.Size = new System.Drawing.Size(121, 20);
			this.mLevelFilterTextfield.TabIndex = 27;
			this.mLevelFilterTextfield.TextChanged += new System.EventHandler(this.LevelFilterEvent);
			// 
			// mLevelFilterLabel
			// 
			this.mLevelFilterLabel.AutoSize = true;
			this.mLevelFilterLabel.Location = new System.Drawing.Point(37, 45);
			this.mLevelFilterLabel.Name = "mLevelFilterLabel";
			this.mLevelFilterLabel.Size = new System.Drawing.Size(32, 13);
			this.mLevelFilterLabel.TabIndex = 26;
			this.mLevelFilterLabel.Text = "Filter:";
			// 
			// mSortingGroupbox
			// 
			this.mSortingGroupbox.Controls.Add(this.mSortIndexRadio);
			this.mSortingGroupbox.Controls.Add(this.mSortTimeRadio);
			this.mSortingGroupbox.Location = new System.Drawing.Point(385, 13);
			this.mSortingGroupbox.Name = "mSortingGroupbox";
			this.mSortingGroupbox.Size = new System.Drawing.Size(207, 78);
			this.mSortingGroupbox.TabIndex = 27;
			this.mSortingGroupbox.TabStop = false;
			this.mSortingGroupbox.Text = "Sorting";
			// 
			// mSortIndexRadio
			// 
			this.mSortIndexRadio.AutoSize = true;
			this.mSortIndexRadio.Location = new System.Drawing.Point(17, 42);
			this.mSortIndexRadio.Name = "mSortIndexRadio";
			this.mSortIndexRadio.Size = new System.Drawing.Size(125, 17);
			this.mSortIndexRadio.TabIndex = 1;
			this.mSortIndexRadio.TabStop = true;
			this.mSortIndexRadio.Text = "Sort by Record Index";
			this.mSortIndexRadio.UseVisualStyleBackColor = true;
			// 
			// mSortTimeRadio
			// 
			this.mSortTimeRadio.AutoSize = true;
			this.mSortTimeRadio.Location = new System.Drawing.Point(17, 19);
			this.mSortTimeRadio.Name = "mSortTimeRadio";
			this.mSortTimeRadio.Size = new System.Drawing.Size(117, 17);
			this.mSortTimeRadio.TabIndex = 0;
			this.mSortTimeRadio.TabStop = true;
			this.mSortTimeRadio.Text = "Sort by Time Stamp";
			this.mSortTimeRadio.UseVisualStyleBackColor = true;
			// 
			// FormatWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(608, 394);
			this.Controls.Add(this.mSortingGroupbox);
			this.Controls.Add(this.mLevelsGroupbox);
			this.Controls.Add(this.mColumnsGroupbox);
			this.Controls.Add(this.mSettingsGroupbox);
			this.Controls.Add(this.mFirstButton);
			this.Controls.Add(this.mCancelButton);
			this.Controls.Add(this.mSecondButton);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "FormatWindow";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Format Datalog";
			this.mSettingsGroupbox.ResumeLayout(false);
			this.mSettingsGroupbox.PerformLayout();
			this.mColumnsGroupbox.ResumeLayout(false);
			this.mColumnsGroupbox.PerformLayout();
			this.mLevelsGroupbox.ResumeLayout(false);
			this.mLevelsGroupbox.PerformLayout();
			this.mSortingGroupbox.ResumeLayout(false);
			this.mSortingGroupbox.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label mColumCategoryLabel;
		private System.Windows.Forms.Label mNameLabel;
		private System.Windows.Forms.Label mDextinationLabel;
		protected System.Windows.Forms.Button mFirstButton;
		protected System.Windows.Forms.TextBox mDestinationTextfield;
		protected System.Windows.Forms.Button mSecondButton;
		protected System.Windows.Forms.Button mCancelButton;
		protected System.Windows.Forms.TextBox mNameTextfield;
		protected System.Windows.Forms.ComboBox mNameList;
		private System.Windows.Forms.ListBox mAvailableColumnList;
		private System.Windows.Forms.ListBox mAvailableLevelList;
		private System.Windows.Forms.ListBox mSelectedColumnList;
		private System.Windows.Forms.ListBox mSelectedLevelList;
		private System.Windows.Forms.ComboBox mColumCategoryTextfield;
		private System.Windows.Forms.GroupBox mSettingsGroupbox;
		private System.Windows.Forms.GroupBox mColumnsGroupbox;
		private System.Windows.Forms.TextBox mColumnFilterTextfield;
		private System.Windows.Forms.Label mColumFilterLabel;
		private System.Windows.Forms.Button mExcludeColumnButton;
		private System.Windows.Forms.Button mIncludeColumnButton;
		private System.Windows.Forms.GroupBox mLevelsGroupbox;
		private System.Windows.Forms.Button mExcludeLevelButton;
		private System.Windows.Forms.Button mIncludeLevelButton;
		private System.Windows.Forms.TextBox mLevelFilterTextfield;
		private System.Windows.Forms.Label mLevelFilterLabel;
		private System.Windows.Forms.Button mDownColumnButton;
		private System.Windows.Forms.Button mUpColumnButton;
		private System.Windows.Forms.GroupBox mSortingGroupbox;
		private System.Windows.Forms.RadioButton mSortIndexRadio;
		private System.Windows.Forms.RadioButton mSortTimeRadio;
		private System.Windows.Forms.CheckBox mAllLogLevels;
		private System.Windows.Forms.CheckBox mHideColumnCheckbox;
		private System.Windows.Forms.CheckBox mHideLevelCheckbox;
		private System.Windows.Forms.CheckBox mColumnIgnoreCaseCheckbox;
		private System.Windows.Forms.CheckBox mLevelIgnoreCaseCheckbox;
	}
}