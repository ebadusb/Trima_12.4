/*****************************************************************************\
**
** SearchSettings
**
** Implementation of the search settings window.
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

/* Classes *******************************************************************/

public partial class SearchSettings : Form {
	#region Constructor
	public SearchSettings()
	{
		this.InitializeComponent();

		mSearcherAssets = new SearcherAssets();
		mSearcherSettings = new SearcherSettings();
		mVersionText.Text = Properties.Resources.VersionNum;
	}
	#endregion

	#region Properties
	public SearcherAssets Assets
	{
		get { return mSearcherAssets; }
		set { mSearcherAssets = value; }
	}

	public SearcherSettings Settings
	{
		get { return mSearcherSettings; }
		set { mSearcherSettings = value; }
	}
	#endregion

	#region Handlers
	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);

		mSwitchWorksheetCheckBox.Checked = mSearcherSettings.SwitchSheet;
		mDefaultWorksheetTextfield.Text = mSearcherSettings.DefaultSheetName; 
		mRememberSearchCheckbox.Checked = mSearcherSettings.RememberSearch; 
		mHistoryCountMaxTextfield.Text = mSearcherSettings.HistoryCountMax.ToString(); 
	}

	protected override void OnFormClosing(FormClosingEventArgs e)
	{
		if (this.DialogResult == DialogResult.OK) {
			if (String.IsNullOrEmpty(mDefaultWorksheetTextfield.Text)) {
				mDefaultWorksheetTextfield.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide default a name.",
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
			mSearcherSettings.SwitchSheet = mSwitchWorksheetCheckBox.Checked;
			mSearcherSettings.DefaultSheetName = mDefaultWorksheetTextfield.Text;
			mSearcherSettings.RememberSearch = mRememberSearchCheckbox.Checked;
			mSearcherSettings.HistoryCountMax = Int32.Parse(mHistoryCountMaxTextfield.Text);
		}
	}

	private void mClearHistoryButton_Click(object sender, EventArgs e)
	{
		mSearcherAssets.RecentInfoManager.Clear();
	}
	
	private void KeyPressHandler(object sender, KeyPressEventArgs e)
	{
		if (sender == mDefaultWorksheetTextfield) {
			if (!Char.IsLetter(e.KeyChar)
					&& (e.KeyChar != 127)		// DEL
					&& (e.KeyChar != 008)) {	// BS
				e.Handled = true;
			}
		}
		else if (sender == mHistoryCountMaxTextfield) {
			if (!Char.IsNumber(e.KeyChar) 
					&& (e.KeyChar != 127)		// DEL
					&& (e.KeyChar != 008)) {	// BS
				e.Handled = true;
			}
		}
	}
	#endregion

	#region Members
	SearcherSettings mSearcherSettings;
	SearcherAssets mSearcherAssets;
	#endregion
}
}
