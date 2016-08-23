/*****************************************************************************\
**
** QueryAssets
**
** Implementation of the query asset repository.
**
\*****************************************************************************/

namespace ExcelQuery {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using ExcelUtility.Asset;
using ExcelUtility.Query;

/* Classes *******************************************************************/

public class QueryAssets {
	#region Constructor
	public QueryAssets()
	{
		mQueryInfoManager = new AssetManagerNamed<QueryInfo>();
		mBatchInfoManager = new AssetManager<AssetBatch<QueryInfo>>();
	}
	#endregion

	#region Properties
	public AssetManager<QueryInfo> QueryInfoManager
	{
		get { return mQueryInfoManager; }
	}

	public AssetManager<AssetBatch<QueryInfo>> BatchInfoManager
	{
		get { return mBatchInfoManager; }
	}
	#endregion
	
	#region Members
	private AssetManager<QueryInfo> mQueryInfoManager;
	private AssetManager<AssetBatch<QueryInfo>> mBatchInfoManager;
	#endregion
}
}
