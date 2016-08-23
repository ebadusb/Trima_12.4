/*****************************************************************************\
**
** QuerySettings
**
** Implementation of search settings.
**
\*****************************************************************************/

namespace ExcelQuery {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;
using ExcelUtility.Asset;
using ExcelUtility.Query;

/* Classes *******************************************************************/

public class QuerySettings {
	#region Constructor
	public QuerySettings()
	{
		mDefaultSheet = "Results";
		mSwitchSheet = true;
	}
	#endregion

	#region Properties
	public String DefaultSheetName
	{
		get { return mDefaultSheet; }
		set { mDefaultSheet = value; }
	}

	public bool SwitchSheet
	{
		get { return mSwitchSheet; }
		set { mSwitchSheet = value; }
	}
	#endregion

	#region Members
	private String mDefaultSheet;
	private bool mSwitchSheet;
	#endregion
}
}
