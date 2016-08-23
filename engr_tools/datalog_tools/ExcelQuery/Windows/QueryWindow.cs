/*****************************************************************************\
**
** QueryWindow
**
** Implementation of the query base window.
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
using ExcelUtility.Query;

/* Classes *******************************************************************/

public partial class QueryWindow : Form {
	#region Constructor
	public QueryWindow()
	{
		this.InitializeComponent();

		mQueryAssets = new QueryAssets();
		mQueryInfo = new QueryInfo();
	}
	#endregion

	#region Properties
	public QueryAssets Assets
	{
		get { return mQueryAssets; }
		set { mQueryAssets = value; }
	}

	public QueryInfo QueryInfo
	{
		get
		{
			QueryInfo info = new QueryInfo();

			info.Name = mQueryList.Text;
			info.Query = mQueryTextbox.Text;
			info.TableName = mDestinationTextfield.Text;

			return info;
		}
		set
		{
			mQueryInfo.Copy(value);
			mQueryList.Text = value.Name;
			mQueryTextbox.Text = value.Query;
			mDestinationTextfield.Text = value.TableName;
		}
	}
	#endregion

	#region Handlers
	private void QueryListSelectionHandler(object sender, EventArgs e)
	{
		this.QueryInfo = (QueryInfo)mQueryList.SelectedItem;
	}

	protected override void OnShown(EventArgs e)
	{
		base.OnShown(e);
		this.ReloadInfoList();
	}

	private void KeyPressHandler(object sender, KeyPressEventArgs e)
	{
		KeyPressUtility.LettersOnlyHandler(sender, e);
	}
	#endregion

	#region Functions
	protected void ReloadInfoList()
	{
		mQueryList.Items.Clear();

		foreach (QueryInfo info in mQueryAssets.QueryInfoManager) {
			mQueryList.Items.Add(info);
		}
	}
	#endregion

	#region Members
	protected QueryAssets mQueryAssets;
	protected QueryInfo mQueryInfo;
	#endregion
}
}
