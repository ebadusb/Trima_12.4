/*****************************************************************************\
**
** QuerySerialize
**
** Implementation of query serializable data.
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

public class QuerySerialize {
	#region Constructor
	public QuerySerialize()
	{
		mQuerySettings = new QuerySettings();
		mCurrentQuery = new QueryInfo();
		mQueryAssetList = new List<QueryInfo>();
		mBatchAssetList = new List<AssetBatch<QueryInfo>>();
	}
	#endregion

	#region Properties
	public QuerySettings QuerySettings
	{
		get { return mQuerySettings; }
		set { mQuerySettings = value; }
	}

	public QueryInfo CurrentQueryInfo
	{
		get { return mCurrentQuery; }
		set { mCurrentQuery = value; }
	}

	public List<QueryInfo> QueryAssetList
	{
		get { return mQueryAssetList; }
		set { mQueryAssetList = value; }
	}

	public List<AssetBatch<QueryInfo>> BatchAssetList
	{
		get { return mBatchAssetList; }
		set { mBatchAssetList = value; }
	}
	#endregion

	#region Members
	private QuerySettings mQuerySettings;
	private QueryInfo mCurrentQuery;
	private List<QueryInfo> mQueryAssetList;
	private List<AssetBatch<QueryInfo>> mBatchAssetList;
	#endregion
}
}
