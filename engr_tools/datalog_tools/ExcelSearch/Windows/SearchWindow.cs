/*****************************************************************************\
**
** SearchWindow
**
** Implementation of the search base window.
**
\*****************************************************************************/

namespace ExcelSearch.Windows {

/* Usings ********************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using Microsoft.Office.Interop.Excel;
using System.Windows.Forms;
using ExcelUtility.Search;

/* Classes *******************************************************************/

public partial class SearchWindow : Form {

	#region Constructors
	public SearchWindow()
	{
		this.InitializeComponent();

		mSearcherAssets = new SearcherAssets();
		mSearchInfo = new SearchInfo();

		mSearchList.DrawItem += new DrawItemEventHandler(this.DrawRowHandler);
	}
	#endregion

	#region Properties
	public SearcherAssets Assets
	{
		get { return mSearcherAssets; }
		set { mSearcherAssets = value; }
	}

	public SearchInfo SearchInfo
	{
		get
		{
			SearchInfo info = new SearchInfo();

			info.SearchString = mSearchList.Visible 
					? mSearchList.Text
					: mSearchTextfield.Text;
			info.Name = mDescriptionList.Visible
					? mDescriptionList.Text
					: mDescriptionTextfield.Text;
			info.ResultSheet = mDestinationTextfield.Text;
			info.SearchArea = mSearchAreaTextfield.Text;
			info.CellColor = mColorCellButton.BackColor;
			info.RowColor = mColorRowButton.BackColor;
			info.IsExpressions = mExpressionRadio.Checked;
			info.MatchLetterCase = mMatchCaseCheckbox.Checked;
			info.MatchWholeWord = mMatchWordCheckbox.Checked;
			info.LinkRangeRows = mLinkRowsCheckbox.Checked;
			info.HighlightSrcCell = mHighlightSrcCellCheckbox.Checked;
			info.HighlightDstCell = mHighlightDstCellCheckbox.Checked;
			info.HighlightSrcRow = mHighlightSrcRowCheckbox.Checked;
			info.HighlightDstRow = mHighlightDstRowCheckbox.Checked;
			info.InsertSrcHyperlink = mHyperlinkSrcCheckbox.Checked;
			info.InsertDstHyperlink = mHyperlinkDstCheckbox.Checked;
			info.InsertRowNum = mRowNumCheckbox.Checked;
			info.SortByRowNum = mSortCheckbox.Checked;
			info.ResizeColumn = mResizeCheckbox.Checked;
			info.ClearPrevious = mClearCheckbox.Checked;

			return info;
		}
		set
		{
			mSearchInfo.Copy(value);
			mSearchList.Text = value.SearchString;
			mSearchTextfield.Text = value.SearchString;
			mDescriptionList.Text = value.Name;
			mDescriptionTextfield.Text = value.Name;
			mDestinationTextfield.Text = value.ResultSheet;
			mSearchAreaTextfield.Text = value.SearchArea;
			mColorCellButton.BackColor = value.CellColor;
			mColorRowButton.BackColor = value.RowColor;
			mWordRadio.Checked = !value.IsExpressions;
			mExpressionRadio.Checked = value.IsExpressions;
			mMatchCaseCheckbox.Checked = value.MatchLetterCase;
			mMatchWordCheckbox.Checked = value.MatchWholeWord;
			mLinkRowsCheckbox.Checked = value.LinkRangeRows;
			mHighlightSrcCellCheckbox.Checked = value.HighlightSrcCell;
			mHighlightDstCellCheckbox.Checked = value.HighlightDstCell;
			mHighlightSrcRowCheckbox.Checked = value.HighlightSrcRow;
			mHighlightDstRowCheckbox.Checked = value.HighlightDstRow;
			mHyperlinkSrcCheckbox.Checked = value.InsertSrcHyperlink;
			mHyperlinkDstCheckbox.Checked = value.InsertDstHyperlink;
			mRowNumCheckbox.Checked = value.InsertRowNum;
			mSortCheckbox.Checked = value.SortByRowNum;
			mResizeCheckbox.Checked = value.ResizeColumn;
			mClearCheckbox.Checked = value.ClearPrevious;
		}
	}
	#endregion

	#region Handlers
	private void mWordRadio_CheckedChanged(object sender, EventArgs e)
	{
		mMatchWordCheckbox.Enabled = true;
	}

	private void mExpressionRadio_CheckedChanged(object sender, EventArgs e)
	{
		mMatchWordCheckbox.Enabled = false;
	}
	
	private void mColorCellButton_Click(object sender, EventArgs e)
	{
		if (mColorDialog.ShowDialog() == DialogResult.OK) {
			mColorCellButton.BackColor = mColorDialog.Color;
		}
	}

	private void mColorRowButton_Click(object sender, EventArgs e)
	{
		if (mColorDialog.ShowDialog() == DialogResult.OK) {
			mColorRowButton.BackColor = mColorDialog.Color;
		}
	}

	private void mSearchTextfield_SelectedIndexChanged(object sender, EventArgs e)
	{
		if (mDescriptionList.Tag == null) {
			SearchInfo info = ((SearchInfoDisplaySearch)mSearchList.SelectedItem).SearchInfo;

			mSearchList.Tag = info;
			this.SearchInfo = info;
			mSearchList.Tag = null;
		}
	}

	private void mDescriptionList_SelectedIndexChanged(object sender, EventArgs e)
	{
		if (mSearchList.Tag == null) {
			SearchInfo info = (SearchInfo)mDescriptionList.SelectedItem;

			mDescriptionList.Tag = info;
			this.SearchInfo = info;
			mDescriptionList.Tag = null;
		}
	}

	private void mRowNumCheckbox_CheckedChanged(object sender, EventArgs e)
	{
		mSortCheckbox.Enabled = mRowNumCheckbox.Checked;
	}

	private void mSortCheckbox_CheckedChanged(object sender, EventArgs e)
	{
		mHyperlinkSrcCheckbox.Enabled = !mSortCheckbox.Checked;
	}

	private void DrawRowHandler(object sender, DrawItemEventArgs e)
	{
		e.DrawBackground();

		if (((e.State & DrawItemState.Selected) != DrawItemState.Selected)
				&& (e.Index >= mSearcherAssets.RecentInfoManager.Count)
				&& (e.Index < mSearcherAssets.SearchInfoManager.Count + mSearcherAssets.RecentInfoManager.Count)) {
			e.Graphics.FillRectangle(Brushes.LightBlue, e.Bounds);
		}

		e.Graphics.DrawString(
				mSearchList.Items[e.Index].ToString(),
				e.Font, Brushes.Black, e.Bounds, 
				StringFormat.GenericDefault);
		e.DrawFocusRectangle();
	}

	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);
		this.ReloadSearchList();
	}
	#endregion

	#region Functions
	protected void ReloadSearchList()
	{
		mSearchList.Items.Clear();
		mDescriptionList.Items.Clear();

		foreach (SearchInfo item in mSearcherAssets.RecentInfoManager) {
			mSearchList.Items.Add(new SearchInfoDisplaySearch(item));
		}

		foreach (SearchInfo item in mSearcherAssets.SearchInfoManager) {
			mSearchList.Items.Add(new SearchInfoDisplaySearch(item));
		}

		foreach (SearchInfo item in mSearcherAssets.SearchInfoManager) {
			mDescriptionList.Items.Add(item);
		}
	}
	#endregion

	#region Members
	protected SearcherAssets mSearcherAssets;
	protected SearchInfo mSearchInfo;
	#endregion
}
}
