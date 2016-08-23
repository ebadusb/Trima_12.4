/*****************************************************************************\
**
** QuerySettingsWindow
**
** Implementation of the query settings window.
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
using AddinUtility;


/* Classes *******************************************************************/

public partial class QuerySettingsWindow : Form {
	#region Constructor
	public QuerySettingsWindow()
	{
		this.InitializeComponent();

		mQuerySettings = new QuerySettings();
		mQueryAssets = new QueryAssets();
	}
	#endregion

	#region Properties
	public QueryAssets Assets
	{
		get { return mQueryAssets; }
		set { mQueryAssets = value; }
	}

	public QuerySettings Settings
	{
		get { return mQuerySettings; }
		set { mQuerySettings = value; }
	}
	#endregion
	
	#region Handlers
	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);

		mDefaultWorksheetTextfield.Text = mQuerySettings.DefaultSheetName;
		mSwitchWorksheetCheckBox.Checked = mQuerySettings.SwitchSheet;
	}

	protected override void OnFormClosing(FormClosingEventArgs e)
	{
		if (this.DialogResult == DialogResult.OK) {
			if (String.IsNullOrEmpty(mDefaultWorksheetTextfield.Text)) {
				mDefaultWorksheetTextfield.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a name.",
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

		if (this.DialogResult == DialogResult.OK) {
			mQuerySettings.DefaultSheetName = mDefaultWorksheetTextfield.Text;
			mQuerySettings.SwitchSheet = mSwitchWorksheetCheckBox.Checked;
		}
	}

	private void KeyPressHandler(object sender, KeyPressEventArgs e)
	{
		KeyPressUtility.LettersOnlyHandler(sender, e);
	}
	#endregion

	#region Members
	private QuerySettings mQuerySettings;
	private QueryAssets mQueryAssets;
	#endregion
}
}
