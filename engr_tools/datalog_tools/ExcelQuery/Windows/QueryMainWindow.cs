/*****************************************************************************\
**
** QueryMainWindow
**
** Implementation of the query main window.
**
\*****************************************************************************/

namespace ExcelQuery.Windows {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using ExcelUtility.Asset;
using ExcelUtility.Query;

/* Classes *******************************************************************/

class QueryMainWindow : QueryWindow {
			
	#region Constructors
	public QueryMainWindow()
	{
		this.mFirstButton.Click += new System.EventHandler(this.FirstButtonHandler);
		this.mSecondButton.Click += new System.EventHandler(this.SecondButtonHandler);
	}
	#endregion

	#region Delegates
	public delegate void ProcessQueryHandler(object sender, ProcessAssetArgs<QueryInfo> args);

	public event ProcessQueryHandler Query;
	#endregion

	#region Functions
	private void FirstButtonHandler(object sender, EventArgs e)
	{
		QueryInfo info = this.QueryInfo;

		if (String.IsNullOrEmpty(info.Name)) {
			mQueryList.Focus();
			System.Windows.Forms.MessageBox.Show(
					"You must provide a name.",
					"Error",
					System.Windows.Forms.MessageBoxButtons.OK,
					System.Windows.Forms.MessageBoxIcon.Error);
		}
		else if (String.IsNullOrEmpty(info.Query)) {
			mQueryTextbox.Focus();
			System.Windows.Forms.MessageBox.Show(
					"You must provide a query.",
					"Error",
					System.Windows.Forms.MessageBoxButtons.OK,
					System.Windows.Forms.MessageBoxIcon.Error);
		}
		else if (mQueryAssets.QueryInfoManager.Exists(info)) {
			System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
					"A query with the same description already exists.\nDo you wish to replace it?",
					"Replace save search?",
					System.Windows.Forms.MessageBoxButtons.YesNo,
					System.Windows.Forms.MessageBoxIcon.Question);

			if (results == DialogResult.Yes) {
				mQueryAssets.QueryInfoManager.Edit(info, info);
				this.ReloadInfoList();
			}
		}
		else {
			mQueryAssets.QueryInfoManager.New(info);
			this.ReloadInfoList();
		}
	}

	private void SecondButtonHandler(object sender, EventArgs e)
	{
		if (this.Query != null) {
			this.Query(this, new ProcessAssetArgs<QueryInfo>(this.QueryInfo));
		}
	}
	#endregion

	#region Functions
	private void InitializeComponent()
	{
		ExcelUtility.Query.QueryInfo queryInfo1 = new ExcelUtility.Query.QueryInfo();
		this.SuspendLayout();
		// 
		// QueryMainWindow
		// 
		this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
		this.ClientSize = new System.Drawing.Size(494, 233);
		this.Name = "QueryMainWindow";
		queryInfo1.Name = "";
		queryInfo1.Query = "";
		queryInfo1.RangeGeneration = true;
		queryInfo1.TableGeneration = true;
		queryInfo1.TableName = "";
		this.QueryInfo = queryInfo1;
		this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
		this.ResumeLayout(false);
		this.PerformLayout();

	}
	#endregion
}
}
