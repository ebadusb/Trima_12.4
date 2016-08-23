/*****************************************************************************\
**
** SearchMainWindow
**
** Implementation of the search main window.
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

class SearchMainWindow : SearchWindow {
			
	#region Constructors
	public SearchMainWindow()
	{
		mSearchTextfield.Visible = false;
		mDescriptionTextfield.Visible = false;
		mSecondButton.Enabled = true;
		mSecondButton.Visible = true;
		mFirstButton.Text = "Search";
		
		this.mFirstButton.Click += new System.EventHandler(this.FirstButtonHandler);
		this.mSecondButton.Click += new System.EventHandler(this.SecondButtonHandler);
	}
	#endregion

	#region Delegates
	public delegate void SearchInfoHandler(object sender, ProcessAssetArgs<SearchInfo> args);
	
	public event SearchInfoHandler Search;
	#endregion

	#region Functions
	private void FirstButtonHandler(object sender, EventArgs e)
	{
		if (this.Search != null) {
			this.Search(this, new ProcessAssetArgs<SearchInfo>(this.SearchInfo));
			this.ReloadSearchList();
		}
	}

	private void SecondButtonHandler(object sender, EventArgs e)
	{
		SearchInfo info = this.SearchInfo;

		if (String.IsNullOrEmpty(info.Name)) {
			mDescriptionTextfield.Focus();
			System.Windows.Forms.MessageBox.Show(
					"You must provide a description.",
					"Error",
					System.Windows.Forms.MessageBoxButtons.OK,
					System.Windows.Forms.MessageBoxIcon.Error);
		}
		else if (mSearcherAssets.SearchInfoManager.Exists(info)) {
			System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
					"A search with the same description already exists.\nDo you wish to replace it?",
					"Replace save search?",
					System.Windows.Forms.MessageBoxButtons.YesNo,
					System.Windows.Forms.MessageBoxIcon.Question);

			if (results == DialogResult.Yes) {
				mSearcherAssets.RecentInfoManager.Delete(info);
				mSearcherAssets.SearchInfoManager.Edit(info, info);
				this.ReloadSearchList();
			}
		}
		else {
			mSearcherAssets.RecentInfoManager.Delete(info);
			mSearcherAssets.SearchInfoManager.New(info);
			this.ReloadSearchList();
		}
	}
	#endregion
}
}
