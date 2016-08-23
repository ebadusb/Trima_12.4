/*****************************************************************************\
**
** FormatSettingsWindow
**
** Implementation of the format settings window.
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
	
public partial class FormatSettingsWindow : Form {
	#region Constructor
	public FormatSettingsWindow()
	{
		this.InitializeComponent();

		mFormaterAssets = new FormaterAssets();
		mFormaterSettings = new FormaterSettings();
	}
	#endregion

	#region Properties
	public FormaterAssets Assets
	{
		get { return mFormaterAssets; }
		set { mFormaterAssets = value; }
	}

	public FormaterSettings Settings
	{
		get { return mFormaterSettings; }
		set { mFormaterSettings = value; }
	}
	#endregion

	#region Handlers
	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);

		mFormatCheckbox.Checked = mFormaterSettings.FormatOnOpen;
		mHideDefaultSheets.Checked = mFormaterSettings.HideDefaultSheets;

		mFormatList.Items.Clear();
		foreach (FormatInfo item in mFormaterAssets.FormatInfosManager) {
			mFormatList.Items.Add(item.Name);
		}

		if (!String.IsNullOrEmpty(mFormaterSettings.DefaultFormatInfo)) {
			mFormatList.SelectedItem = mFormaterSettings.DefaultFormatInfo;
		}
	}

	protected override void OnClosed(EventArgs e)
	{
		base.OnClosed(e);

		if (this.DialogResult == DialogResult.OK) {
			mFormaterSettings.FormatOnOpen = mFormatCheckbox.Checked;
			mFormaterSettings.HideDefaultSheets = mHideDefaultSheets.Checked;
			mFormaterSettings.DefaultFormatInfo = mFormatList.Text;
		}
	}

	private void FormatOnOpenHandler(object sender, EventArgs e)
	{
		mFormatList.Enabled = mFormatCheckbox.Checked;
	}
	#endregion

	#region Members
	private FormaterSettings mFormaterSettings;
	private FormaterAssets mFormaterAssets;
	#endregion
}
}
