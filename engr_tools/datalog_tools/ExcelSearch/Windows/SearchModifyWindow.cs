/*****************************************************************************\
**
** SearchModifyWindow
**
** Implementation of the search modify window.
**
\*****************************************************************************/

namespace ExcelSearch.Windows {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using ExcelUtility.Asset;
using ExcelUtility.Search;

/* Classes *******************************************************************/

class SearchModifyWindow : SearchWindow {

	#region Constructors
	public SearchModifyWindow()
	{
		mSearchList.Visible = false;
		mDescriptionList.Visible = false;
		mSecondButton.Enabled = false;
		mSecondButton.Visible = false;
		mFirstButton.Text = "Save";
	}
	#endregion
	
	#region Handlers
	protected override void OnFormClosing(FormClosingEventArgs e)
	{
		if (this.DialogResult == DialogResult.OK) {
			SearchInfo info = this.SearchInfo;

			if (String.IsNullOrEmpty(info.Name)) {
				mDescriptionTextfield.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a description.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
			else if (!info.Name.Equals(mSearchInfo.Name)
					&& mSearcherAssets.SearchInfoManager.Exists(info)) {
				System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
						"A search with the same description already exists.\nYou must provide a new description.",
						"Provide a new description!",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Information);
				e.Cancel = true;
			}
		}
		
		base.OnFormClosing(e);
	}
	#endregion
}
}
