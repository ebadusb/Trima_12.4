/*****************************************************************************\
**
** SearcherSettings
**
** Implementation of search settings.
**
\*****************************************************************************/

namespace ExcelSearch {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;
using ExcelUtility.Asset;
using ExcelUtility.Search;

/* Classes *******************************************************************/

public class SearcherSettings {
	#region Constructor
	public SearcherSettings()
	{
		mDefaultSheet = "Search Results";
		mHistoryCountMax = 10;
		mSwitchSheet = true;
		mRememberSearch = true;
		mShowLogWindow = false;
	}
	#endregion

	#region Properties
	public String DefaultSheetName
	{
		get { return mDefaultSheet; }
		set { mDefaultSheet = value; }
	}

	public int HistoryCountMax
	{
		get { return mHistoryCountMax; }
		set { mHistoryCountMax = value; }
	}
	
	public bool SwitchSheet
	{
		get { return mSwitchSheet; }
		set { mSwitchSheet = value; }
	}

	public bool RememberSearch
	{
		get { return mRememberSearch; }
		set { mRememberSearch = value; }
	}

	public bool ShowLog
	{
		get { return mShowLogWindow; }
		set { mShowLogWindow = value; }
	}
	#endregion
	
	#region Members
	private String mDefaultSheet;
	private int mHistoryCountMax;
	private bool mSwitchSheet;
	private bool mRememberSearch;
	private bool mShowLogWindow;
	#endregion
}
}
