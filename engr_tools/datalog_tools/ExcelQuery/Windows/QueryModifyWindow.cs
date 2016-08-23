/*****************************************************************************\
**
** QueryModifyWindow
**
** Implementation of the query modify window.
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

class QueryModifyWindow : QueryWindow {
			
	#region Constructors
	public QueryModifyWindow()
	{
		mFirstButton.Enabled = false;
		mFirstButton.Visible = false;
		mSecondButton.Text = "Save";
	}
	#endregion

	#region Handlers
	protected override void OnFormClosing(FormClosingEventArgs e)
	{
		if (this.DialogResult == DialogResult.OK) {
			QueryInfo info = this.QueryInfo;
			AssetNameCompare<QueryInfo> compare = new AssetNameCompare<QueryInfo>(info);

			if (String.IsNullOrEmpty(info.Name)) {
				mQueryList.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a name.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
			else if (String.IsNullOrEmpty(info.Query)) {
				mQueryTextbox.Focus();
				System.Windows.Forms.MessageBox.Show(
						"You must provide a query.",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
			else if (!compare.Equals(mQueryInfo)
					&& mQueryAssets.QueryInfoManager.Exists(info)) {
				System.Windows.Forms.DialogResult results = System.Windows.Forms.MessageBox.Show(
						"A query with the same description already exists.\nYou must provide a new name.",
						"Replace saved query?",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
				e.Cancel = true;
			}
		}

		base.OnFormClosing(e);
	}
	#endregion

	#region Functions
	private void InitializeComponent()
	{
		ExcelUtility.Query.QueryInfo queryInfo1 = new ExcelUtility.Query.QueryInfo();
		this.SuspendLayout();
		// 
		// QueryModifyWindow
		// 
		this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
		this.ClientSize = new System.Drawing.Size(494, 233);
		this.Name = "QueryModifyWindow";
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
