/*****************************************************************************\
**
** FormatManager
**
** Implementation of the format manager window.
**
\*****************************************************************************/

namespace ExcelFormater.Windows {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using AddinUtility;
using ExcelUtility.Asset;
using ExcelUtility.Format;

/* Classes *******************************************************************/

public partial class FormatManager : Form {
	#region Constructor
	public FormatManager()
	{
		this.InitializeComponent();

		mFormaterAssets = new FormaterAssets();
		mFormatCreateWindow = new FormatCreateWindow();
		mFormatModifyWindow = new FormatModifyWindow();
		mColumnFilterWindow = new ColumnFilterWindow();
		mColumnListWindow = new ColumnListWindow();
		mColumnMacroWindow = new ColumnMacroWindow();
		mTableMacroWindow = new TableMacroWindow();

		mTypeList.SelectedIndex = 0;
	}
	#endregion

	#region Properties
	public FormaterAssets Assets
	{
		get
		{
			return mFormaterAssets;
		}
		set
		{
			mFormaterAssets = value;
			mFormatCreateWindow.Assets = value;
			mFormatModifyWindow.Assets = value;
			mColumnFilterWindow.Assets = value;
			mColumnListWindow.Assets = value;
			mColumnMacroWindow.Assets = value;
			mTableMacroWindow.Assets = value;
		}
	}
	#endregion

	#region Handlers
	private void NewButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleNewFormat();
			break;
		case 1:
			this.HandleNewFilter();
			break;
		case 2:
			this.HandleNewList();
			break;
		case 3:
			this.HandleNewColumnMacro();
			break;
		case 4:
			this.HandleNewTableMacro();
			break;
		}
	}

	private void EditButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleEditFormat();
			break;
		case 1:
			this.HandleEditFilter();
			break;
		case 2:
			this.HandleEditList();
			break;
		case 3:
			this.HandleEditColumnMacro();
			break;
		case 4:
			this.HandleEditTableMacro();
			break;
		}
	}

	private void DeleteButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleDeleteFormat();
			break;
		case 1:
			this.HandleDeleteFilter();
			break;
		case 2:
			this.HandleDeleteList();
			break;
		case 3:
			this.HandleDeleteColumnMacro();
			break;
		case 4:
			this.HandleDeleteTableMacro();
			break;
		}
	}
	
	private void ImportButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleImportFormat();
			break;
		case 1:
			this.HandleImportFilter();
			break;
		case 2:
			this.HandleImportList();
			break;
		case 3:
			this.HandleImportColumnMacro();
			break;
		case 4:
			this.HandleImportTableMacro();
			break;
		}
	}

	private void ExportButtonHandler(object sender, EventArgs e)
	{
		switch (mTypeList.SelectedIndex) {
		case 0:
			this.HandleExportFormat();
			break;
		case 1:
			this.HandleExportFilter();
			break;
		case 2:
			this.HandleExportList();
			break;
		case 3:
			this.HandleExportColumnMacro();
			break;
		case 4:
			this.HandleExportColumnMacro();
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
		mExportButton.Enabled = mItemList.SelectedItems.Count >= 1;
	}

	private void UpdateItemList()
	{
		mItemList.Items.Clear();

		switch (mTypeList.SelectedIndex) {
		case 0:
			foreach (FormatInfo item in mFormaterAssets.FormatInfosManager) {
				mItemList.Items.Add(item);
			}
			break;
		case 1:
			foreach (ColumnFilter item in mFormaterAssets.ColumnFiltersManager) {
				mItemList.Items.Add(item);
			}
			break;
		case 2:
			foreach (ColumnList item in mFormaterAssets.ColumnListsManager) {
				mItemList.Items.Add(item);
			}
			break;
		case 3:
			foreach (CustomMacro item in mFormaterAssets.ColumnMacroManager) {
				mItemList.Items.Add(item);
			}
			break;
		case 4:
			foreach (CustomMacro item in mFormaterAssets.TableMacroManager) {
				mItemList.Items.Add(item);
			}
			break;
		}
	}

	private void HandleNewFormat()
	{
		mFormatCreateWindow.FormatInfo = new FormatInfo();

		if (mFormatCreateWindow.ShowDialog(this) == DialogResult.OK) {
			FormatInfo item = mFormatCreateWindow.FormatInfo;

			if (!mFormaterAssets.FormatInfosManager.Exists(item)) {
				mFormaterAssets.FormatInfosManager.New(item);
				mItemList.Items.Add(item);
			}
		}
	}

	private void HandleEditFormat()
	{
		if (mItemList.SelectedIndices.Count == 1) {
			FormatInfo oldInfo = (FormatInfo)mItemList.SelectedItem;

			mFormatModifyWindow.FormatInfo = oldInfo;

			if (mFormatModifyWindow.ShowDialog(this) == DialogResult.OK) {
				FormatInfo newInfo = mFormatModifyWindow.FormatInfo;

				mFormaterAssets.FormatInfosManager.Edit(oldInfo, newInfo);
				mItemList.Items[mItemList.SelectedIndex] = newInfo;
			}
		}
	}

	private void HandleDeleteFormat()
	{
		DialogResult results = System.Windows.Forms.MessageBox.Show(
				"OK to delete the selected item?",
				"Delete",
				System.Windows.Forms.MessageBoxButtons.OKCancel,
				System.Windows.Forms.MessageBoxIcon.Question);

		if (results == DialogResult.OK) {
			while (mItemList.SelectedIndices.Count > 0) {
				FormatInfo item = (FormatInfo)mItemList.SelectedItem;

				mFormaterAssets.FormatInfosManager.Delete(item);
				mItemList.Items.Remove(item);
			}
		}
	}

	private void HandleImportFormat()
	{
		if (mOpenFileDialog.ShowDialog() == DialogResult.OK) {
			List<FormatInfo> list = null;

			SerializeUtility.ImportItem(mOpenFileDialog.FileName, out list);

			foreach (FormatInfo item in list) {
				if (!mFormaterAssets.FormatInfosManager.Exists(item)) {
					mFormaterAssets.FormatInfosManager.New(item);
					mItemList.Items.Add(item);
				}
			}
		}
	}

	private void HandleExportFormat()
	{
		if (mSaveFileDialog.ShowDialog() == DialogResult.OK) {
			List<FormatInfo> list = new List<FormatInfo>();

			foreach (FormatInfo item in mItemList.SelectedItems) {
				list.Add(item);
			}

			SerializeUtility.ExportItem(mSaveFileDialog.FileName, list);
		}
	}

	private void HandleNewFilter()
	{
		mColumnFilterWindow.ColumnFilter = new ColumnFilter();

		if (mColumnFilterWindow.ShowDialog(this) == DialogResult.OK) {
			ColumnFilter item = mColumnFilterWindow.ColumnFilter;

			if (!mFormaterAssets.ColumnFiltersManager.Exists(item)) {
				mFormaterAssets.ColumnFiltersManager.New(item);
				mItemList.Items.Add(item);
			}
		}
	}

	private void HandleEditFilter()
	{
		if (mItemList.SelectedIndices.Count == 1) {
			ColumnFilter oldInfo = (ColumnFilter)mItemList.SelectedItem;
			
			mColumnFilterWindow.ColumnFilter = oldInfo;

			if (mColumnFilterWindow.ShowDialog(this) == DialogResult.OK) {
				ColumnFilter newInfo = mColumnFilterWindow.ColumnFilter;

				mFormaterAssets.ColumnFiltersManager.Edit(oldInfo, newInfo);
				mItemList.Items[mItemList.SelectedIndex] = newInfo;
			}
		}
	}

	private void HandleDeleteFilter()
	{
		DialogResult results = System.Windows.Forms.MessageBox.Show(
				"OK to delete the selected item?",
				"Delete",
				System.Windows.Forms.MessageBoxButtons.OKCancel,
				System.Windows.Forms.MessageBoxIcon.Question);

		if (results == DialogResult.OK) {
			while (mItemList.SelectedIndices.Count > 0) {
				ColumnFilter item = (ColumnFilter)mItemList.SelectedItem;

				mFormaterAssets.ColumnFiltersManager.Delete(item);
				mItemList.Items.Remove(item);
			}
		}
	}

	private void HandleImportFilter()
	{
		if (mOpenFileDialog.ShowDialog() == DialogResult.OK) {
			List<ColumnFilter> list = null;

			SerializeUtility.ImportItem<List<ColumnFilter>>(mOpenFileDialog.FileName, out list);

			foreach (ColumnFilter item in list) {
				if (!mFormaterAssets.ColumnFiltersManager.Exists(item)) {
					mFormaterAssets.ColumnFiltersManager.New(item);
					mItemList.Items.Add(item);
				}
			}
		}
	}

	private void HandleExportFilter()
	{
		if (mSaveFileDialog.ShowDialog() == DialogResult.OK) {
			List<ColumnFilter> list = new List<ColumnFilter>();

			foreach (ColumnFilter item in mItemList.SelectedItems) {
				list.Add(item);
			}

			SerializeUtility.ExportItem(mSaveFileDialog.FileName, list);
		}
	}

	private void HandleNewList()
	{
		mColumnListWindow.ColumnList = new ColumnList();

		if (mColumnListWindow.ShowDialog(this) == DialogResult.OK) {
			ColumnList item = mColumnListWindow.ColumnList;

			if (!mFormaterAssets.ColumnListsManager.Exists(item)) {
				mFormaterAssets.ColumnListsManager.New(item);
				mItemList.Items.Add(item);
			}
		}
	}

	private void HandleEditList()
	{
		if (mItemList.SelectedIndices.Count == 1) {
			ColumnList oldInfo = (ColumnList)mItemList.SelectedItem;

			mColumnListWindow.ColumnList = oldInfo;

			if (mColumnListWindow.ShowDialog(this) == DialogResult.OK) {
				ColumnList newInfo = mColumnListWindow.ColumnList;

				mFormaterAssets.ColumnListsManager.Edit(oldInfo, newInfo);
				mItemList.Items[mItemList.SelectedIndex] = newInfo;
			}
		}
	}

	private void HandleDeleteList()
	{
		DialogResult results = System.Windows.Forms.MessageBox.Show(
				"OK to delete the selected item?",
				"Delete",
				System.Windows.Forms.MessageBoxButtons.OKCancel,
				System.Windows.Forms.MessageBoxIcon.Question);

		if (results == DialogResult.OK) {
			while (mItemList.SelectedIndices.Count > 0) {
				ColumnList item = (ColumnList)mItemList.SelectedItem;

				mFormaterAssets.ColumnListsManager.Delete(item);
				mItemList.Items.Remove(item);
			}
		}
	}
	
	private void HandleImportList()
	{
		if (mOpenFileDialog.ShowDialog() == DialogResult.OK) {
			List<ColumnList> list = null;

			SerializeUtility.ImportItem<List<ColumnList>>(mOpenFileDialog.FileName, out list);

			foreach (ColumnList item in list) {
				if (!mFormaterAssets.ColumnListsManager.Exists(item)) {
					mFormaterAssets.ColumnListsManager.New(item);
					mItemList.Items.Add(item);
				}
			}
		}
	}
	
	private void HandleExportList()
	{
		if (mSaveFileDialog.ShowDialog() == DialogResult.OK) {
			List<ColumnList> list = new List<ColumnList>();

			foreach (ColumnList item in mItemList.SelectedItems) {
				list.Add(item);
			}

			SerializeUtility.ExportItem(mSaveFileDialog.FileName, list);
		}
	}
	
	private void HandleNewColumnMacro()
	{
		mColumnMacroWindow.ColumnMacro = new CustomMacro();

		if (mColumnMacroWindow.ShowDialog(this) == DialogResult.OK) {
			CustomMacro item = mColumnMacroWindow.ColumnMacro;

			if (!mFormaterAssets.ColumnMacroManager.Exists(item)) {
				mFormaterAssets.ColumnMacroManager.New(item);
				mItemList.Items.Add(item);
			}
		}
	}

	private void HandleEditColumnMacro()
	{
		if (mItemList.SelectedIndices.Count == 1) {
			CustomMacro oldInfo = (CustomMacro)mItemList.SelectedItem;

			mColumnMacroWindow.ColumnMacro = oldInfo;

			if (mColumnMacroWindow.ShowDialog(this) == DialogResult.OK) {
				CustomMacro newInfo = mColumnMacroWindow.ColumnMacro;

				mFormaterAssets.ColumnMacroManager.Edit(oldInfo, newInfo);
				mItemList.Items[mItemList.SelectedIndex] = newInfo;
			}
		}
	}

	private void HandleDeleteColumnMacro()
	{
		DialogResult results = System.Windows.Forms.MessageBox.Show(
				"OK to delete the selected item?",
				"Delete",
				System.Windows.Forms.MessageBoxButtons.OKCancel,
				System.Windows.Forms.MessageBoxIcon.Question);

		if (results == DialogResult.OK) {
			while (mItemList.SelectedIndices.Count > 0) {
				CustomMacro item = (CustomMacro)mItemList.SelectedItem;

				mFormaterAssets.ColumnMacroManager.Delete(item);
				mItemList.Items.Remove(item);
			}
		}
	}

	private void HandleImportColumnMacro()
	{
		if (mOpenFileDialog.ShowDialog() == DialogResult.OK) {
			List<CustomMacro> list = null;

			SerializeUtility.ImportItem<List<CustomMacro>>(mOpenFileDialog.FileName, out list);

			foreach (CustomMacro item in list) {
				if (!mFormaterAssets.ColumnMacroManager.Exists(item)) {
					mFormaterAssets.ColumnMacroManager.New(item);
					mItemList.Items.Add(item);
				}
			}
		}
	}

	private void HandleExportColumnMacro()
	{
		if (mSaveFileDialog.ShowDialog() == DialogResult.OK) {
			List<CustomMacro> list = new List<CustomMacro>();

			foreach (CustomMacro item in mItemList.SelectedItems) {
				list.Add(item);
			}

			SerializeUtility.ExportItem(mSaveFileDialog.FileName, list);
		}
	}

	private void HandleNewTableMacro()
	{
		mTableMacroWindow.TableMacro = new CustomMacro();

		if (mTableMacroWindow.ShowDialog(this) == DialogResult.OK) {
			CustomMacro item = mTableMacroWindow.TableMacro;

			if (!mFormaterAssets.TableMacroManager.Exists(item)) {
				mFormaterAssets.TableMacroManager.New(item);
				mItemList.Items.Add(item);
			}
		}
	}

	private void HandleEditTableMacro()
	{
		if (mItemList.SelectedIndices.Count == 1) {
			CustomMacro oldInfo = (CustomMacro)mItemList.SelectedItem;

			mTableMacroWindow.TableMacro = oldInfo;

			if (mTableMacroWindow.ShowDialog(this) == DialogResult.OK) {
				CustomMacro newInfo = mTableMacroWindow.TableMacro;

				mFormaterAssets.TableMacroManager.Edit(oldInfo, newInfo);
				mItemList.Items[mItemList.SelectedIndex] = newInfo;
			}
		}
	}

	private void HandleDeleteTableMacro()
	{
		DialogResult results = System.Windows.Forms.MessageBox.Show(
				"OK to delete the selected item?",
				"Delete",
				System.Windows.Forms.MessageBoxButtons.OKCancel,
				System.Windows.Forms.MessageBoxIcon.Question);

		if (results == DialogResult.OK) {
			while (mItemList.SelectedIndices.Count > 0) {
				CustomMacro item = (CustomMacro)mItemList.SelectedItem;

				mFormaterAssets.TableMacroManager.Delete(item);
				mItemList.Items.Remove(item);
			}
		}
	}

	private void HandleImportTableMacro()
	{
		if (mOpenFileDialog.ShowDialog() == DialogResult.OK) {
			List<CustomMacro> list = null;

			SerializeUtility.ImportItem<List<CustomMacro>>(mOpenFileDialog.FileName, out list);

			foreach (CustomMacro item in list) {
				if (!mFormaterAssets.TableMacroManager.Exists(item)) {
					mFormaterAssets.TableMacroManager.New(item);
					mItemList.Items.Add(item);
				}
			}
		}
	}

	private void HandleExportTableMacro()
	{
		if (mSaveFileDialog.ShowDialog() == DialogResult.OK) {
			List<CustomMacro> list = new List<CustomMacro>();

			foreach (CustomMacro item in mItemList.SelectedItems) {
				list.Add(item);
			}

			SerializeUtility.ExportItem(mSaveFileDialog.FileName, list);
		}
	}
	#endregion

	#region Members
	private FormaterAssets mFormaterAssets;
	private FormatCreateWindow mFormatCreateWindow;
	private FormatModifyWindow mFormatModifyWindow;
	private ColumnFilterWindow mColumnFilterWindow;
	private ColumnListWindow mColumnListWindow;
	private ColumnMacroWindow mColumnMacroWindow;
	private TableMacroWindow mTableMacroWindow;
	#endregion
}
}
