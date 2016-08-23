/*****************************************************************************\
**
** SearchManager
**
** Implementation of the search manager window.
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
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using AddinUtility;
using ExcelUtility.Asset;
using ExcelUtility.Search;

/* Classes *******************************************************************/

public partial class SearchManager : Form {
	#region Constructor
	public SearchManager()
	{
		this.InitializeComponent();

		mSearcherAssets = new SearcherAssets();
		mSearchCreateWindow = new SearchCreateWindow();
		mSearchModifyWindow = new SearchModifyWindow();
		mSearchBatch = new SearchBatch();

		mTypeList.SelectedIndex = 0;
	}
	#endregion

	#region Delegates
	public delegate void ProcessSearchHandler(object sender, ProcessAssetArgs<AssetBatch<SearchInfo>> args);

	public event ProcessSearchHandler Search;
	#endregion

	#region Properties
	public SearcherAssets Assets
	{
		get
		{
			return mSearcherAssets;
		}
		set
		{
			mSearcherAssets = value;
			mSearchCreateWindow.Assets = value;
			mSearchModifyWindow.Assets = value;
			mSearchBatch.Assets = value;
		}
	}
	#endregion
	
	#region Handlers
	private void NewButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleNewSearch();
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
			this.HandleEditSearch();
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
			this.HandleDeleteSearch();
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
			this.HandleImportSearch();
			break;
		}
	}

	private void ExportButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleExportSearch();
			break;
		}
	}

	private void ExecuteButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleExecuteSearch();
			break;
		case 1:
			this.HandleExecuteBatch();
			break;
		}
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
			foreach (SearchInfo item in mSearcherAssets.SearchInfoManager) {
				mItemList.Items.Add(item);
			}
			break;
		case 1:
			foreach (AssetBatch<SearchInfo> item in mSearcherAssets.BatchInfoManager) {
				mItemList.Items.Add(item);
			}
			break;
		}
	}

	private void HandleNewSearch()
	{
		mSearchCreateWindow.SearchInfo = new SearchInfo();

		if (mSearchCreateWindow.ShowDialog(this) == DialogResult.OK) {
			SearchInfo item = mSearchCreateWindow.SearchInfo;

			if (!mSearcherAssets.SearchInfoManager.Exists(item)) {
				mSearcherAssets.SearchInfoManager.New(item);
				mItemList.Items.Add(item);
			}
		}
	}

	private void HandleEditSearch()
	{
		if (mItemList.SelectedIndices.Count == 1) {
			SearchInfo oldInfo = (SearchInfo)mItemList.SelectedItem;

			mSearchModifyWindow.SearchInfo = oldInfo;

			if (mSearchModifyWindow.ShowDialog(this) == DialogResult.OK) {
				SearchInfo newInfo = mSearchModifyWindow.SearchInfo;

				mSearcherAssets.SearchInfoManager.Edit(oldInfo, newInfo);
				mItemList.Items[mItemList.SelectedIndex] = newInfo;
			}
		}
	}

	private void HandleDeleteSearch()
	{
		DialogResult results = System.Windows.Forms.MessageBox.Show(
				"OK to delete the selected search presets?",
				"Delete",
				System.Windows.Forms.MessageBoxButtons.OKCancel,
				System.Windows.Forms.MessageBoxIcon.Question);

		if (results == DialogResult.OK) {
			while (mItemList.SelectedIndices.Count > 0) {
				SearchInfo item = (SearchInfo)mItemList.SelectedItem;

				mSearcherAssets.SearchInfoManager.Delete(item);
				mItemList.Items.Remove(item);
			}
		}
	}

	private void HandleImportSearch()
	{
		if (mOpenFileDialog.ShowDialog() == DialogResult.OK) {
			List<SearchInfo> list = null;

			SerializeUtility.ImportItem(mOpenFileDialog.FileName, out list);

			foreach (SearchInfo item in list) {
				if (!mSearcherAssets.SearchInfoManager.Exists(item)) {
					mSearcherAssets.SearchInfoManager.New(item);
					mItemList.Items.Add(item);
				}
			}
		}
	}

	private void HandleExportSearch()
	{
		if (mSaveFileDialog.ShowDialog() == DialogResult.OK) {
			List<SearchInfo> list = new List<SearchInfo>();

			foreach (SearchInfo item in mItemList.SelectedItems) {
				list.Add(item);
			}

			SerializeUtility.ExportItem(mSaveFileDialog.FileName, list);
		}
	}

	private void HandleNewBatch()
	{
		mSearchBatch.BatchInfo = new AssetBatch<SearchInfo>();

		if (mSearchBatch.ShowDialog(this) == DialogResult.OK) {
			AssetBatch<SearchInfo> info = mSearchBatch.BatchInfo;

			if (!mSearcherAssets.BatchInfoManager.Exists(info)) {
				mSearcherAssets.BatchInfoManager.New(info);
				mItemList.Items.Add(info);
			}
		}
	}

	private void HandleEditBatch()
	{
		if (mItemList.SelectedIndices.Count == 1) {
			AssetBatch<SearchInfo> oldInfo = (AssetBatch<SearchInfo>)mItemList.SelectedItem;

			mSearchBatch.BatchInfo = oldInfo;

			if (mSearchBatch.ShowDialog(this) == DialogResult.OK) {
				AssetBatch<SearchInfo> newInfo = mSearchBatch.BatchInfo;

				mSearcherAssets.BatchInfoManager.Edit(oldInfo, newInfo);
				mItemList.Items[mItemList.SelectedIndex] = newInfo;
			}
		}
	}

	private void HandleDeleteBatch()
	{
		DialogResult results = System.Windows.Forms.MessageBox.Show(
				"OK to delete the selected batch presets?",
				"Delete",
				System.Windows.Forms.MessageBoxButtons.OKCancel,
				System.Windows.Forms.MessageBoxIcon.Question);

		if (results == DialogResult.OK) {
			while (mItemList.SelectedIndices.Count > 0) {
				AssetBatch<SearchInfo> item = (AssetBatch<SearchInfo>)mItemList.SelectedItem;

				mSearcherAssets.BatchInfoManager.Delete(item);
				mItemList.Items.Remove(item);
			}
		}
	}

	private void HandleExecuteSearch()
	{
		if (this.Search != null && mItemList.SelectedItems.Count > 0) {
			AssetBatch<SearchInfo> batch = new AssetBatch<SearchInfo>();

			foreach (SearchInfo item in mItemList.SelectedItems) {
				batch.Insert(item);
			}

			this.Search(this, new ProcessAssetArgs<AssetBatch<SearchInfo>>(batch));

			this.Close();
		}
	}

	private void HandleExecuteBatch()
	{
		if (this.Search != null) {
			foreach (AssetBatch<SearchInfo> batch in mItemList.SelectedItems) {
				this.Search(this, new ProcessAssetArgs<AssetBatch<SearchInfo>>(batch));
			}

			this.Close();
		}
	}
	#endregion

	#region Members
	protected SearcherAssets mSearcherAssets;
	private SearchWindow mSearchCreateWindow;
	private SearchWindow mSearchModifyWindow;
	private SearchBatch mSearchBatch;
	#endregion
}
}
