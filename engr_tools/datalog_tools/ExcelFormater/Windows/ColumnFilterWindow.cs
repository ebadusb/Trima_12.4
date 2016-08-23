/*****************************************************************************\
**
** ColumnFilterWindow
**
** Implementation of the column filter window.
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

public partial class ColumnFilterWindow : Form {
	#region Constructor
	public ColumnFilterWindow()
	{
		this.InitializeComponent();

		mFormaterAssets = new FormaterAssets();
		mColumnFilter = new ColumnFilter();
	}
	#endregion

	#region Properties
	public FormaterAssets Assets
	{
		get { return mFormaterAssets; }
		set { mFormaterAssets = value; }
	}

	public ColumnFilter ColumnFilter
	{
		get { return mColumnFilter; }
		set { mColumnFilter = value.Clone(); }
	}
	#endregion

	#region Handlers
	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);

		mNameTextfield.Text = mColumnFilter.Name;
		mFilterTextfield.Text = mColumnFilter.Filter;
		mNegateCheckbox.Checked = mColumnFilter.Negate;

		this.UpdateColumnList();
	}

	protected override void OnFormClosing(FormClosingEventArgs e)
	{
		if (this.DialogResult == DialogResult.OK) {
			ItemNameCompare<ColumnFilter> compare = new ItemNameCompare<ColumnFilter>(mNameTextfield.Text);

			if (String.IsNullOrEmpty(mNameTextfield.Text)) {
				mNameTextfield.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a name.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
			else if (!mNameTextfield.Text.Equals(mColumnFilter.Name)
					&& (mFormaterAssets.ColumnFiltersManager.Exists(compare.Equals)
					|| mFormaterAssets.ColumnListsManager.Exists(compare.Equals))) {
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

		mColumnFilter.Name = mNameTextfield.Text;
		mColumnFilter.Filter = mFilterTextfield.Text;
		mColumnFilter.Negate = mNegateCheckbox.Checked;
	}

	private void ReloadListHandler(object sender, EventArgs e)
	{
		this.UpdateColumnList();
	}
	#endregion

	#region Functions
	void UpdateColumnList()
	{
		ColumnFilter temp = mColumnFilter.Clone();

		temp.Filter = mFilterTextfield.Text;
		temp.IgnoreCase = mIgnoreCaseCheckbox.Checked;
		temp.Negate = mNegateCheckbox.Checked;

		IEnumerable<String> columns = temp.GetColumns();

		mSelectedList.Items.Clear();
		foreach (String name in columns) {
			mSelectedList.Items.Add(name);
		}
	}
	#endregion

	#region Members
	private FormaterAssets mFormaterAssets;
	protected ColumnFilter mColumnFilter;
	#endregion
}
}
