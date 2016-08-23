/*****************************************************************************\
**
** SearcherAssets
**
** Implementation of the search asset repository.
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

public class SearcherAssets {
	#region Constructor
	public SearcherAssets()
	{
		mSearchInfoManager = new AssetManagerNamed<SearchInfo>();
		mRecentInfoManager = new AssetManagerStacked<SearchInfo>();
		mBatchInfoManager = new AssetManagerNamed<AssetBatch<SearchInfo>>();
	}
	#endregion

	#region Properties
	public AssetManager<SearchInfo> SearchInfoManager
	{
		get { return mSearchInfoManager; }
	}

	public AssetManagerStacked<SearchInfo> RecentInfoManager
	{
		get { return mRecentInfoManager; }
	}

	public AssetManager<AssetBatch<SearchInfo>> BatchInfoManager
	{
		get { return mBatchInfoManager; }
	}
	#endregion
	
	#region Members
	private AssetManager<SearchInfo> mSearchInfoManager;
	private AssetManagerStacked<SearchInfo> mRecentInfoManager;
	private AssetManager<AssetBatch<SearchInfo>> mBatchInfoManager;
	#endregion
}
}
