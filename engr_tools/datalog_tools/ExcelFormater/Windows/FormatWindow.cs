/*****************************************************************************\
**
** FormatWindow
**
** Implementation of the base format window.
**
\*****************************************************************************/

namespace ExcelFormater.Windows {

/* Usings ********************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using AddinUtility;
using ExcelUtility.Asset;
using ExcelUtility.Format;
using System.Text.RegularExpressions;

/* Classes *******************************************************************/

public partial class FormatWindow : Form {
	#region Constructor
	public FormatWindow()
	{
		this.InitializeComponent();

		mFormaterAssets = new FormaterAssets();
		mFormatInfo = new FormatInfo();
	}
	#endregion
	
	#region Properties
	public FormaterAssets Assets
	{
		get { return mFormaterAssets; }
		set { mFormaterAssets = value; }
	}

	public FormatInfo FormatInfo
	{
		get 
		{
			FormatInfo info = new FormatInfo();

			info.Name = mNameList.Visible
					? mNameList.Text
					: mNameTextfield.Text;
			info.Destination = mDestinationTextfield.Text;
			info.Order = mSortTimeRadio.Checked
					? FormatInfo.Ordering.Time
					: FormatInfo.Ordering.Index;
			info.AllLevels = mAllLogLevels.Checked;

			foreach (object item in mSelectedColumnList.Items) {
				info.Columns.Add(item.ToString());
			}

			foreach (object item in mSelectedLevelList.Items) {
				info.Levels.Add(item.ToString());
			}

			return info;
		}
		set 
		{
			mFormatInfo.Copy(value);
			mNameList.Text = value.Name;
			mNameTextfield.Text = value.Name;
			mDestinationTextfield.Text = value.Destination;
			mSortTimeRadio.Checked = value.Order == FormatInfo.Ordering.Time;
			mSortIndexRadio.Checked = value.Order == FormatInfo.Ordering.Index;
			mAllLogLevels.Checked = value.AllLevels;

			mSelectedColumnList.Items.Clear();
			foreach (String item in value.Columns) {
				mSelectedColumnList.Items.Add(item);
			}
			
			mSelectedLevelList.Items.Clear();
			foreach (String item in value.Levels) {
				mSelectedLevelList.Items.Add(item);
			}
		}
	}
	#endregion

	#region Handlers
	private void NameListSelectionHandler(object sender, EventArgs e)
	{
		this.FormatInfo = (FormatInfo)mNameList.SelectedItem;
	}

	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);
		this.ReloadCategoryList();
		this.ReloadColumnList();
		this.ReloadLevelList();
		this.ReloadInfoList();
	}

	private void ColumnCategoryEvent(object sender, EventArgs e)
	{
		this.ReloadColumnList();
	}

	private void ColumnFilterEvent(object sender, EventArgs e)
	{
		this.ReloadColumnList();
	}

	private void LevelFilterEvent(object sender, EventArgs e)
	{
		this.ReloadLevelList();
	}

	private void IncludeColumnEvent(object sender, EventArgs e)
	{
		foreach (String item in mAvailableColumnList.SelectedItems) {
			if (!mSelectedColumnList.Items.Contains(item)) {
				mSelectedColumnList.Items.Add(item);
			}
		}
	}

	private void ExcludeColumnEvent(object sender, EventArgs e)
	{
		while (mSelectedColumnList.SelectedItems.Count > 0) {
			mSelectedColumnList.Items.Remove(mSelectedColumnList.SelectedItem);
		}
		mAvailableColumnList.Refresh();
	}

	private void IncludeLevelEvent(object sender, EventArgs e)
	{
		foreach (String item in mAvailableLevelList.SelectedItems) {
			if (!mSelectedLevelList.Items.Contains(item)) {
				mSelectedLevelList.Items.Add(item);
			}
		}
	}

	private void ExcludeLevelEvent(object sender, EventArgs e)
	{
		while (mSelectedLevelList.SelectedItems.Count > 0) {
			mSelectedLevelList.Items.Remove(mSelectedLevelList.SelectedItem);
		}
		mAvailableLevelList.Refresh();
	}

	private void UpColumnEvent(object sender, EventArgs e)
	{
		ArrayList selections = new ArrayList(mSelectedColumnList.SelectedIndices);

		selections.Sort();
		
		if (selections.Count > 0 && (int)selections[0] > 0) {
			mSelectedColumnList.SelectedItems.Clear();

			foreach (int i in selections) {
				object temp = mSelectedColumnList.Items[i - 1];

				mSelectedColumnList.Items[i - 1] = mSelectedColumnList.Items[i];
				mSelectedColumnList.Items[i] = temp;
				mSelectedColumnList.SelectedIndices.Add(i - 1);
			}
		}
	}

	private void DownColumnEvent(object sender, EventArgs e)
	{
		ArrayList selections = new ArrayList(mSelectedColumnList.SelectedIndices);

		selections.Sort();
		selections.Reverse();

		if (selections.Count > 0 && (int)selections[0] < mSelectedColumnList.Items.Count - 1) {
			mSelectedColumnList.SelectedItems.Clear();
	
			foreach (int i in selections) {
				object temp = mSelectedColumnList.Items[i + 1];

				mSelectedColumnList.Items[i + 1] = mSelectedColumnList.Items[i];
				mSelectedColumnList.Items[i] = temp;
				mSelectedColumnList.SelectedIndices.Add(i + 1);
			}
		}
	}

	private void AvailableColumnClickHandler(object sender, MouseEventArgs e)
	{
		if (e.Clicks == 2) {
			mSelectedColumnList.Items.Add(mAvailableColumnList.SelectedItem);
		}
	}

	private void AvailableSelectedClickHandler(object sender, MouseEventArgs e)
	{
		if (e.Clicks == 2) {
			mSelectedLevelList.Items.Add(mAvailableLevelList.SelectedItem);
		}
	}
	
	private void SelectedColumnClickHandler(object sender, MouseEventArgs e)
	{
		if (e.Clicks == 2) {
			mSelectedColumnList.Items.RemoveAt(mSelectedColumnList.SelectedIndex);
		}
	}

	private void SelectedLevelClickHandler(object sender, MouseEventArgs e)
	{
		if (e.Clicks == 2) {
			mSelectedLevelList.Items.RemoveAt(mSelectedLevelList.SelectedIndex);
		}
	}
	
	private void AvailableColumnFocusEnter(object sender, EventArgs e)
	{
		mAvailableLevelList.SelectedItems.Clear();
		mSelectedColumnList.SelectedItems.Clear();
		mSelectedLevelList.SelectedItems.Clear();
	}

	private void AvailableLevelFocusEnter(object sender, EventArgs e)
	{
		mAvailableColumnList.SelectedItems.Clear();
		mSelectedColumnList.SelectedItems.Clear();
		mSelectedLevelList.SelectedItems.Clear();
	}

	private void SelectedLevelFocusEnter(object sender, EventArgs e)
	{
		mAvailableColumnList.SelectedItems.Clear();
		mAvailableLevelList.SelectedItems.Clear();
		mSelectedColumnList.SelectedItems.Clear();
	}

	private void SelectedColumnFocusEnter(object sender, EventArgs e)
	{
		mAvailableColumnList.SelectedItems.Clear();
		mAvailableLevelList.SelectedItems.Clear();
		mSelectedLevelList.SelectedItems.Clear();
	}

	private void KeyPressHandler(object sender, KeyPressEventArgs e)
	{
		KeyPressUtility.LettersOnlyHandler(sender, e);
	}
		
	private void AllLogLevelsHandler(object sender, EventArgs e)
	{
		mAvailableLevelList.Enabled = !mAllLogLevels.Checked;
		mIncludeLevelButton.Enabled = !mAllLogLevels.Checked;
		mExcludeLevelButton.Enabled = !mAllLogLevels.Checked;
		mSelectedLevelList.Enabled = !mAllLogLevels.Checked;
		mLevelFilterLabel.Enabled = !mAllLogLevels.Checked;
		mLevelFilterTextfield.Enabled = !mAllLogLevels.Checked;
		mLevelIgnoreCaseCheckbox.Enabled = !mAllLogLevels.Checked;
		mHideLevelCheckbox.Enabled = !mAllLogLevels.Checked;
	}

	private void ReloadColumnHandler(object sender, EventArgs e)
	{
		this.ReloadColumnList();
	}

	private void ReloadLevelHandler(object sender, EventArgs e)
	{
		this.ReloadLevelList();
	}

	private void DrawColumnListHandler(object sender, DrawItemEventArgs e)
	{
		String text = "";
		Brush brush = Brushes.Black;

		if (mAvailableColumnList.Items.Count > 0) {
			text = (String)mAvailableColumnList.Items[e.Index];

			if (((e.State & DrawItemState.Selected) != DrawItemState.Selected)
					&& (mSelectedColumnList.Items.Contains(text))) {
				brush = Brushes.Gray;
			}
		}

		e.DrawBackground();
		e.Graphics.DrawString(text, e.Font, brush, e.Bounds, StringFormat.GenericDefault);
		e.DrawFocusRectangle();
	}

	private void DrawLevelListHandler(object sender, DrawItemEventArgs e)
	{
		String text = "";
		Brush brush = Brushes.Black;

		if (mAvailableLevelList.Items.Count > 0) {
			text = (String)mAvailableLevelList.Items[e.Index];

			if (((e.State & DrawItemState.Selected) != DrawItemState.Selected)
					&& (mSelectedLevelList.Items.Contains(text))
					|| mAllLogLevels.Checked) {
				brush = Brushes.Gray;
			}
		}

		e.DrawBackground();
		e.Graphics.DrawString(text, e.Font, brush, e.Bounds, StringFormat.GenericDefault);
		e.DrawFocusRectangle();
	}
	#endregion

	#region Functions
	protected void ReloadInfoList()
	{
		mNameList.Items.Clear();

		foreach (FormatInfo info in mFormaterAssets.FormatInfosManager) {
			mNameList.Items.Add(info);
		}
	}

	protected void ReloadCategoryList()
	{
		mColumCategoryTextfield.Items.Clear();
		foreach (ColumnFilter item in mFormaterAssets.ColumnFiltersManager) {
			mColumCategoryTextfield.Items.Add(item);
		}
		foreach (ColumnList item in mFormaterAssets.ColumnListsManager) {
			mColumCategoryTextfield.Items.Add(item);
		}
		if (mColumCategoryTextfield.Items.Count > 0) {
			mColumCategoryTextfield.SelectedIndex = 0;
		}
	}

	protected void ReloadColumnList()
	{
		mAvailableColumnList.Items.Clear();
		if (mColumCategoryTextfield.SelectedItem != null) {
			ColumnCategory category = (ColumnCategory)mColumCategoryTextfield.SelectedItem;
			RegexOptions options = mColumnIgnoreCaseCheckbox.Checked
					? RegexOptions.IgnoreCase : RegexOptions.None; 
			
			try {
				foreach (String item in category.GetColumns()) {
					bool include = true;

					if (mHideColumnCheckbox.Checked) {
						ItemStringCompare<StringAsset> compare = new ItemStringCompare<StringAsset>(item);
						include &= mFormaterAssets.AvailableColumnAssetManager.Exists(compare.Equals);
					}

					if (!String.IsNullOrEmpty(mColumnFilterTextfield.Text)) {
						include &= Regex.IsMatch(item, mColumnFilterTextfield.Text, options);
					}

					if (include) {
						mAvailableColumnList.Items.Add(item);
					}
				}
			}
			catch {
				// It's an invalid expression... do nothing. 
			}
		}
	}

	protected void ReloadLevelList()
	{
		RegexOptions options = mLevelIgnoreCaseCheckbox.Checked
				? RegexOptions.IgnoreCase : RegexOptions.None; 

		mAvailableLevelList.Items.Clear();
		
		try {
			foreach (StringAsset item in mFormaterAssets.KnownLevelAssetManager) {
				bool include = true;

				if (mHideLevelCheckbox.Checked) {
					include &= mFormaterAssets.AvailableLevelAssetManager.Exists(item);
				}

				if (!String.IsNullOrEmpty(mLevelFilterTextfield.Text)) {
					include &= Regex.IsMatch(item.Text, mLevelFilterTextfield.Text, options);
				}

				if (include) {
					mAvailableLevelList.Items.Add(item.Text);
				}
			}
		}
		catch {
			// It's an invalid expression... do nothing. 
		}
	}
	#endregion

	#region Members
	protected FormaterAssets mFormaterAssets;
	protected FormatInfo mFormatInfo;
	#endregion
}
}
