/*****************************************************************************\
**
** SearcherSerialize
**
** Implementation of serializable data.
**
\*****************************************************************************/

namespace ExcelSearch {

/* Usings ********************************************************************/

using System;
using System.Text;
using System.Xml.Serialization;
using System.Collections.Generic;
using ExcelUtility.Asset;
using ExcelUtility.Search;

/* Classes *******************************************************************/

public class SearcherSerialize {
	#region Constructor
	public SearcherSerialize()
	{
		mSearcherSettings = new SearcherSettings();
		mCurrentSearchInfo = new SearchInfo();
		mRecentInfoList = new List<SearchInfo>();
		mSearchInfoList = new List<SearchInfo>();
		mBatchInfoList = new List<AssetBatch<SearchInfo>>();
	}
	#endregion

	#region Properties
	public SearcherSettings SearcherSettings
	{
		get { return mSearcherSettings; }
		set { mSearcherSettings = value; }
	}

	public SearchInfo CurrentSearchInfo
	{
		get { return mCurrentSearchInfo; }
		set { mCurrentSearchInfo = value; }
	}

	public List<SearchInfo> RecentInfoList
	{
		get { return mRecentInfoList; }
		set { mRecentInfoList = value; }
	}

	public List<SearchInfo> SearchInfoList
	{
		get { return mSearchInfoList; }
		set { mSearchInfoList = value; }
	}

	public List<AssetBatch<SearchInfo>> BatchInfoList
	{
		get { return mBatchInfoList; }
		set { mBatchInfoList = value; }
	}
	#endregion

	#region Members
	private SearcherSettings mSearcherSettings;
	private SearchInfo mCurrentSearchInfo;
	private List<SearchInfo> mRecentInfoList;
	private List<SearchInfo> mSearchInfoList;
	private List<AssetBatch<SearchInfo>> mBatchInfoList;
	#endregion
}
}

