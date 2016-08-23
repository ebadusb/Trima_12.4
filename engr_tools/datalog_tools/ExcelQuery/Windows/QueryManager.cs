/*****************************************************************************\
**
** QueryManager
**
** Implementation of the query manager window.
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
using System.Xml.Serialization;
using System.IO;
using AddinUtility;
using ExcelUtility.Asset;
using ExcelUtility.Query;

/* Classes *******************************************************************/

public partial class QueryManager : Form {
	#region Constructor
	public QueryManager()
	{
		this.InitializeComponent();

		mQueryAssets = new QueryAssets();
		mQueryCreateWindow = new QueryCreateWindow();
		mQueryModifyWindow = new QueryModifyWindow();
		mQueryBatchWindow = new QueryBatch();

		mTypeList.SelectedIndex = 0;
	}
	#endregion

	#region Delegates
	public delegate void ProcessQueryHandler(object sender, ProcessAssetArgs<AssetBatch<QueryInfo>> args);

	public event ProcessQueryHandler Query;
	#endregion

	#region Properties
	public QueryAssets Assets
	{
		get 
		{ 
			return mQueryAssets; 
		}
		set 
		{ 
			mQueryAssets = value;
			mQueryCreateWindow.Assets = value;
			mQueryModifyWindow.Assets = value;
			mQueryBatchWindow.Assets = value;
		}
	}
	#endregion

	#region Handlers
	private void NewButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleNewQuery();
			break;
		case 1:
			this.HandleNewBatch();
			break;
		}
	}

	private void EditButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleEditQuery();
			break;
		case 1:
			this.HandleEditBatch();
			break;
		}
	}

	private void DeleteButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleDeleteQuery();
			break;
		case 1:
			this.HandleDeleteBatch();
			break;
		}
	}

	private void ImportButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleImportQuery();
			break;
		}
	}

	private void ExportButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleExportQuery();
			break;
		}
	}
	
	private void ExecuteButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleExecuteQuery();
			break;
		case 1:
			this.HandleExecuteBatch();
			break;
		}

		this.Close();
	}

	private void CloseButtonHandler(object sender, EventArgs e)
	{
		this.Close();
	}

	private void ButtonStateHandler(object sender, EventArgs e)
	{
		this.UpdateButtonStates();
	}

	private void TypeListHandler(object sender, EventArgs e)
	{
		this.UpdateItemList();
		this.UpdateButtonStates();
	}

	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);
		this.UpdateItemList();
		this.UpdateButtonStates();
	}
	#endregion

	#region Functions
	private void UpdateButtonStates()
	{
		mEditButton.Enabled = mItemList.SelectedItems.Count == 1;
		mDeleteButton.Enabled = mItemList.SelectedItems.Count >= 1;
		mImportButton.Enabled = mTypeList.SelectedIndex == 0;
		mExportButton.Enabled = mItemList.SelectedItems.Count >= 1
			&& mTypeList.SelectedIndex == 0;
		mExecuteButton.Enabled = mItemList.SelectedItems.Count >= 1;
	}

	private void UpdateItemList()
	{
		mItemList.Items.Clear();

		switch (mTypeList.SelectedIndex) {
		case 0:
			foreach (QueryInfo item in mQueryAssets.QueryInfoManager) {
				mItemList.Items.Add(item);
			}
			break;
		case 1:
			foreach (AssetBatch<QueryInfo> item in mQueryAssets.BatchInfoManager) {
				mItemList.Items.Add(item);
			}
			break;
		}
	}

	private void HandleNewQuery()
	{
		mQueryCreateWindow.QueryInfo = new QueryInfo();

		if (mQueryCreateWindow.ShowDialog(this) == DialogResult.OK) {
			QueryInfo item = mQueryCreateWindow.QueryInfo;

			if (!mQueryAssets.QueryInfoManager.Exists(item)) {
				mQueryAssets.QueryInfoManager.New(item);
				mItemList.Items.Add(item);
			}
		}
	}

	private void HandleEditQuery()
	{
		if (mItemList.SelectedIndices.Count == 1) {
			QueryInfo oldInfo = (QueryInfo)mItemList.SelectedItem;

			mQueryModifyWindow.QueryInfo = oldInfo;

			if (mQueryModifyWindow.ShowDialog(this) == DialogResult.OK) {
				QueryInfo newInfo = mQueryModifyWindow.QueryInfo;

				mQueryAssets.QueryInfoManager.Edit(oldInfo, newInfo);
				mItemList.Items[mItemList.SelectedIndex] = newInfo;
			}
		}
	}

	private void HandleDeleteQuery()
	{
		DialogResult results = System.Windows.Forms.MessageBox.Show(
				"OK to delete the selected item?",
				"Delete",
				System.Windows.Forms.MessageBoxButtons.OKCancel,
				System.Windows.Forms.MessageBoxIcon.Question);

		if (results == DialogResult.OK) {
			while (mItemList.SelectedIndices.Count > 0) {
				QueryInfo item = (QueryInfo)mItemList.SelectedItem;

				mQueryAssets.QueryInfoManager.Delete(item);
				mItemList.Items.Remove(item);
			}
		}
	}

	private void HandleImportQuery()
	{
		if (mOpenFileDialog.ShowDialog() == DialogResult.OK) {
			List<QueryInfo> list = null;

			SerializeUtility.ImportItem<List<QueryInfo>>(mOpenFileDialog.FileName, out list);

			foreach (QueryInfo item in list) {
				if (!mQueryAssets.QueryInfoManager.Exists(item)) {
					mQueryAssets.QueryInfoManager.New(item);
					mItemList.Items.Add(item);
				}
			}
		}
	}

	private void HandleExportQuery()
	{
		if (mSaveFileDialog.ShowDialog() == DialogResult.OK) {
			List<QueryInfo> list = new List<QueryInfo>();

			foreach (QueryInfo item in mItemList.SelectedItems) {
				list.Add(item);
			}

			SerializeUtility.ExportItem(mSaveFileDialog.FileName, list);
		}
	}

	private void HandleExecuteQuery()
	{
		if (this.Query != null && mItemList.SelectedItems.Count > 0) {
			AssetBatch<QueryInfo> batch = new AssetBatch<QueryInfo>();

			foreach (QueryInfo item in mItemList.SelectedItems) {
				batch.Insert(item);
			}

			this.Query(this, new ProcessAssetArgs<AssetBatch<QueryInfo>>(batch));

			this.Close();
		}
	}

	private void HandleNewBatch()
	{
		mQueryBatchWindow.BatchInfo = new AssetBatch<QueryInfo>();

		if (mQueryBatchWindow.ShowDialog(this) == DialogResult.OK) {
			AssetBatch<QueryInfo> item = mQueryBatchWindow.BatchInfo;

			if (!mQueryAssets.BatchInfoManager.Exists(item)) {
				mQueryAssets.BatchInfoManager.New(item);
				mItemList.Items.Add(item);
			}
		}
	}

	private void HandleEditBatch()
	{
		if (mItemList.SelectedIndices.Count == 1) {
			AssetBatch<QueryInfo> oldInfo = (AssetBatch<QueryInfo>)mItemList.SelectedItem;

			mQueryBatchWindow.BatchInfo = oldInfo;

			if (mQueryBatchWindow.ShowDialog(this) == DialogResult.OK) {
				AssetBatch<QueryInfo> newInfo = mQueryBatchWindow.BatchInfo;

				mQueryAssets.BatchInfoManager.Edit(oldInfo, newInfo);
				mItemList.Items[mItemList.SelectedIndex] = newInfo;
			}
		}
	}

	private void HandleDeleteBatch()
	{
		DialogResult results = System.Windows.Forms.MessageBox.Show(
				"OK to delete the selected item?",
				"Delete",
				System.Windows.Forms.MessageBoxButtons.OKCancel,
				System.Windows.Forms.MessageBoxIcon.Question);

		if (results == DialogResult.OK) {
			while (mItemList.SelectedIndices.Count > 0) {
				AssetBatch<QueryInfo> item = (AssetBatch<QueryInfo>)mItemList.SelectedItem;

				mQueryAssets.BatchInfoManager.Delete(item);
				mItemList.Items.Remove(item);
			}
		}
	}

	private void HandleExecuteBatch()
	{
		if (this.Query != null) {
			foreach (AssetBatch<QueryInfo> batch in mItemList.SelectedItems) {
				this.Query(this, new ProcessAssetArgs<AssetBatch<QueryInfo>>(batch));
			}

			this.Close();
		}
	}
	#endregion

	#region Members
	private QueryAssets mQueryAssets;
	private QueryWindow mQueryCreateWindow;
	private QueryWindow mQueryModifyWindow;
	private QueryBatch mQueryBatchWindow;
	#endregion
}
}
