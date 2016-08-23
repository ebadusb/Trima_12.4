/*****************************************************************************\
**
** SearchInformer
**
** Implementation of the search informer window.
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

public partial class SearchInformer : Form {
	#region Constructor
	public SearchInformer()
	{
		this.InitializeComponent();
	}
	#endregion

	#region Properties
	public bool ShowLog
	{
		get { return !mShowDialogCheckBox.Checked; }
		set { mShowDialogCheckBox.Checked = !value; }
	}

	public String SearchLog
	{
		get { return mSearchLogTextbox.Text; }
		set { mSearchLogTextbox.Text = value; }
	}
	#endregion
}
}
