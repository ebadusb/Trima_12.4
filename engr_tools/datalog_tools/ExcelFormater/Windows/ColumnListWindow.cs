/*****************************************************************************\
**
** ColumnListWindow
**
** Implementation of the column list window.
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

/* Classes *******************************************************************/

public partial class ColumnListWindow : Form {
	#region Constructor
	public ColumnListWindow()
	{
		this.InitializeComponent();

		mFormaterAssets = new FormaterAssets();
		mColumnList = new ColumnList(); 
	}
	#endregion

	#region Properties
	public FormaterAssets Assets
	{
		get { return mFormaterAssets; }
		set { mFormaterAssets = value; }
	}

	public ColumnList ColumnList
	{
		get { return mColumnList; }
		set { mColumnList = value.Clone(); }
	}
	#endregion

	#region Handlers
	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);

		mNameTextfield.Text = mColumnList.Name;

		mAvailableList.Items.Clear();
		mSelectedList.Items.Clear();

		foreach (StringAsset asset in mFormaterAssets.KnownColumnAssetManager) {
			mAvailableList.Items.Add(asset, mColumnList.List.Contains(asset.Text));
		}
	}

	protected override void OnFormClosing(FormClosingEventArgs e)
	{
		if (this.DialogResult == DialogResult.OK) {
			ItemNameCompare<ColumnList> compare = new ItemNameCompare<ColumnList>(mNameTextfield.Text);

			if (String.IsNullOrEmpty(mNameTextfield.Text)) {
				mNameTextfield.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a name.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
			else if (!mNameTextfield.Text.Equals(mColumnList.Name)
					&& (mFormaterAssets.ColumnListsManager.Exists(compare.Equals)
					|| mFormaterAssets.ColumnFiltersManager.Exists(compare.Equals))) {
				System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
						"A category with the same name already exists.\nYou must provide a new name.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
		}

		base.OnFormClosing(e);
	}

	protected override void OnClosed(EventArgs e)
	{
		base.OnClosed(e);

		mColumnList.Name = mNameTextfield.Text;
		
		mColumnList.List.Clear();
		foreach (StringAsset item in mSelectedList.Items) {
			mColumnList.List.Add(item.Text);
		}
	}

	private void ItemCheckedHandler(object sender, ItemCheckEventArgs e)
	{
		if (e.NewValue == CheckState.Checked) {
			mSelectedList.Items.Add(mAvailableList.Items[e.Index]);
		}
		else {
			mSelectedList.Items.Remove(mAvailableList.Items[e.Index]);
		}
	}
	#endregion

	#region Members
	private FormaterAssets mFormaterAssets;
	protected ColumnList mColumnList;
	#endregion
}
}
