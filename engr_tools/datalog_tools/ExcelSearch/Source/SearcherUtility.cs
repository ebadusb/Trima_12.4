/*****************************************************************************\
**
** SearcherSettings
**
** Implementation of formater settings.
**
\*****************************************************************************/

namespace ExcelSearch {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using ExcelUtility.Asset;
using ExcelUtility.Search;

/* Classes *******************************************************************/

public class ItemNameCompare<TYPE> where TYPE : IAssetName<TYPE> {
	#region Constructor
	public ItemNameCompare(String name)
	{
		mName = name;
	}
	#endregion

	#region Functions
	public bool Equals(TYPE item)
	{
		return mName.Equals(item.Name);
	}
	#endregion

	#region Members
	private String mName;
	#endregion
}

public class SearchInfoDisplaySearch {
	public SearchInfoDisplaySearch(SearchInfo info)
	{
		mInfo = info;
	}

	public SearchInfo SearchInfo
	{
		get { return mInfo; }
		set { mInfo = value; }
	}

	public override string ToString()
	{
		return mInfo.SearchString;
	}

	private SearchInfo mInfo;
}
}
