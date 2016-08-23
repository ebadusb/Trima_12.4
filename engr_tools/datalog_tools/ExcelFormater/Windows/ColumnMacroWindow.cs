/*****************************************************************************\
**
** ColumnMacroWindow
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

/* Classes *******************************************************************/

public partial class ColumnMacroWindow : Form {
	public ColumnMacroWindow()
	{
		this.InitializeComponent();

		mFormaterAssets = new FormaterAssets();
		mColumnMacro = new CustomMacro();
	}

	#region Properties
	public FormaterAssets Assets
	{
		get { return mFormaterAssets; }
		set { mFormaterAssets = value; }
	}

	public CustomMacro ColumnMacro
	{
		get { return mColumnMacro; }
		set { mColumnMacro = value.Clone(); }
	}
	#endregion


	#region Handlers
	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);

		mNameTextfield.Text = mColumnMacro.Name;
		mFunctionTextfield.Text = mColumnMacro.Function;

		mAvailableList.Items.Clear();
		mSelectedList.Items.Clear();

		foreach (StringAsset asset in mFormaterAssets.KnownColumnAssetManager) {
			mAvailableList.Items.Add(asset, mColumnMacro.List.Contains(asset.Text));
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
			else if (!mNameTextfield.Text.Equals(mColumnMacro.Name)
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

		mColumnMacro.Name = mNameTextfield.Text;
		mColumnMacro.Function = mFunctionTextfield.Text;

		mColumnMacro.List.Clear();
		foreach (StringAsset item in mSelectedList.Items) {
			mColumnMacro.List.Add(item.Text);
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
	protected CustomMacro mColumnMacro;
	#endregion
}
}
