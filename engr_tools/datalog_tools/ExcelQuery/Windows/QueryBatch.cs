/*****************************************************************************\
**
** QueryBatch
**
** Implementation of the query batch window.
**
\*****************************************************************************/

namespace ExcelQuery.Windows {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using ExcelUtility.Asset;
using ExcelUtility.Query;

/* Classes *******************************************************************/

public partial class QueryBatch : Form {
	#region Constructors
	public QueryBatch()
	{
		this.InitializeComponent();

		mQueryAssets = new QueryAssets();
		mBatchInfo = new AssetBatch<QueryInfo>();
	}
	#endregion

	#region Properties
	public QueryAssets Assets
	{
		get { return mQueryAssets; }
		set { mQueryAssets = value; }
	}

	public AssetBatch<QueryInfo> BatchInfo
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

		mQueryList.Items.Clear();
		foreach (QueryInfo item in mQueryAssets.QueryInfoManager) {
			mQueryList.Items.Add(item, mBatchInfo.Contains(item));
		}
	}

	protected override void OnFormClosing(FormClosingEventArgs e)
	{
		if (this.DialogResult == DialogResult.OK) {
			AssetBatch<QueryInfo> info = this.BatchInfo;

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
					&& mQueryAssets.BatchInfoManager.Exists(info)) {
				System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
						"A batch with the same name already exists.\nYou must provide a new name.",
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

		foreach (QueryInfo item in mQueryList.CheckedItems) {
			mBatchInfo.Insert(item);
		}
	}
	#endregion

	#region Members
	private QueryAssets mQueryAssets;
	private AssetBatch<QueryInfo> mBatchInfo;
	#endregion
}
}
