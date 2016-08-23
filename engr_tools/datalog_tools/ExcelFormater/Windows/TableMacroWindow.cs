/*****************************************************************************\
**
** TableMacroWindow
**
** Implementation of the column macro window.
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
using ExcelUtility.Format;

/* Classes *******************************************************************/

public partial class TableMacroWindow : Form {
	public TableMacroWindow()
	{
		this.InitializeComponent();

		mFormaterAssets = new FormaterAssets();
		mTableMacro = new CustomMacro();
	}

	#region Properties
	public FormaterAssets Assets
	{
		get { return mFormaterAssets; }
		set { mFormaterAssets = value; }
	}

	public CustomMacro TableMacro
	{
		get { return mTableMacro; }
		set { mTableMacro = value.Clone(); }
	}
	#endregion


	#region Handlers
	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);

		mNameTextfield.Text = mTableMacro.Name;
		mFunctionTextfield.Text = mTableMacro.Function;

		mAvailableList.Items.Clear();
		mSelectedList.Items.Clear();

		foreach (FormatInfo asset in mFormaterAssets.FormatInfosManager) {
			mAvailableList.Items.Add(asset, mTableMacro.List.Contains(asset.Name));
		}
	}

	protected override void OnFormClosing(FormClosingEventArgs e)
	{
		if (this.DialogResult == DialogResult.OK) {
			ItemNameCompare<CustomMacro> compare = new ItemNameCompare<CustomMacro>(mNameTextfield.Text);

			if (String.IsNullOrEmpty(mNameTextfield.Text)) {
				mNameTextfield.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a name.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
			else if (String.IsNullOrEmpty(mFunctionTextfield.Text)) {
				mFunctionTextfield.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a function name.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
			else if (!mNameTextfield.Text.Equals(mTableMacro.Name)
					&& (mFormaterAssets.ColumnMacroManager.Exists(compare.Equals)
					|| mFormaterAssets.ColumnMacroManager.Exists(compare.Equals))) {
				System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
						"A column macro with the same name already exists.\nYou must provide a new name.",
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

		mTableMacro.Name = mNameTextfield.Text;
		mTableMacro.Function = mFunctionTextfield.Text;

		mTableMacro.List.Clear();
		foreach (FormatInfo item in mSelectedList.Items) {
			mTableMacro.List.Add(item.Name);
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
	protected CustomMacro mTableMacro;
	#endregion
}
}
