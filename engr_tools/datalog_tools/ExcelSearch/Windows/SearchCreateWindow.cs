/*****************************************************************************\
**
** SearchCreateWindow
**
** Implementation of the search create window.
**
\*****************************************************************************/

namespace ExcelSearch.Windows {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using ExcelUtility.Search;

/* Classes *******************************************************************/

class SearchCreateWindow : SearchWindow {

	#region Constructors
	public SearchCreateWindow()
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
			else if (mSearcherAssets.SearchInfoManager.Exists(info)) {
				System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
						"A search with the same description already exists.\nDo you wish to replace it?",
						"Replace save search?",
						System.Windows.Forms.MessageBoxButtons.YesNo,
						System.Windows.Forms.MessageBoxIcon.Question);

				if (results != DialogResult.Yes) {
					e.Cancel = true;
				}
			}
		}

		base.OnFormClosing(e);
	}
	#endregion
}
}
