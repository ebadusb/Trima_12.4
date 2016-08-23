/*****************************************************************************\
**
** SearchBatch
**
** Implementation of the search batch window.
**
\*****************************************************************************/

namespace ExcelSearch.Windows {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using ExcelUtility.Asset;
using ExcelUtility.Search;

/* Classes *******************************************************************/

public partial class SearchBatch : Form {
	#region Constructors
	public SearchBatch()
	{
		this.InitializeComponent();

		mSearcherAssets = new SearcherAssets();
		mBatchInfo = new AssetBatch<SearchInfo>();
	}
	#endregion

	#region Properties
	public SearcherAssets Assets
	{
		get { return mSearcherAssets; }
		set { mSearcherAssets = value; }
	}

	public AssetBatch<SearchInfo> BatchInfo
	{
		get { return mBatchInfo; }
		set { mBatchInfo = value.Clone(); }
	}
	#endregion

	#region Handlers
	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);

		mNameTextfield.Text = mBatchInfo.Name;
		mCommentTextbox.Text = mBatchInfo.Comment;

		mSearchList.Items.Clear();
		foreach (SearchInfo item in mSearcherAssets.SearchInfoManager) {
			mSearchList.Items.Add(item, mBatchInfo.Contains(item));
		}
	}

	protected override void OnFormClosing(FormClosingEventArgs e)
	{
		if (this.DialogResult == DialogResult.OK) {
			AssetBatch<SearchInfo> info = this.BatchInfo;

			if (String.IsNullOrEmpty(mNameTextfield.Text)) {
				mNameTextfield.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a name.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
			else if (!info.Name.Equals(mBatchInfo.Name)
					&& mSearcherAssets.BatchInfoManager.Exists(info)) {
				System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
						"A batch search with the same name already exists.\nYou must provide a new name.",
						"Provide a new name!",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Information);
				e.Cancel = true;
			}
		}

		base.OnFormClosing(e);
	}

	protected override void OnClosed(EventArgs e)
	{
		base.OnClosed(e);

		mBatchInfo.Name = mNameTextfield.Text;
		mBatchInfo.Comment = mCommentTextbox.Text;

		foreach (SearchInfo item in mSearchList.CheckedItems) {
			mBatchInfo.Insert(item);
		}
	}
	#endregion

	#region Members
	private SearcherAssets mSearcherAssets;
	private AssetBatch<SearchInfo> mBatchInfo;
	#endregion
}
}
