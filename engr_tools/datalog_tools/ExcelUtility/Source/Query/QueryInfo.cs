/*****************************************************************************\
**
** QueryInfo
**
** Implementation of a query info.
**
\*****************************************************************************/

namespace ExcelUtility.Query {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;
using Asset;

/* Classes *******************************************************************/

public class QueryInfo : IAssetName<QueryInfo>, IAssetItem<QueryInfo> {
	#region Constructor
	public QueryInfo()
	{
		mName = "";
		mQuery = "";
		mTableName = "";
		mTableGeneration = true;
		mRangeGeneration = true;
	}
	#endregion

	#region Properties
	public String Name
	{
		get { return mName; }
		set { mName = value; }
	}

	public String Query
	{
		get { return mQuery; }
		set { mQuery = value; }
	}

	public String TableName
	{
		get { return mTableName; }
		set { mTableName = value; }
	}

	public bool TableGeneration
	{
		get { return mTableGeneration; }
		set { mTableGeneration = value; }
	}

	public bool RangeGeneration
	{
		get { return mRangeGeneration; }
		set { mRangeGeneration = value; }
	}
	#endregion

	#region Functions
	public bool Equals(QueryInfo item)
	{
		return this.Name.Equals(item.Name)
			&& this.Query.Equals(item.Query)
			&& this.TableName.Equals(item.TableName)
			&& this.TableGeneration == item.TableGeneration
			&& this.RangeGeneration == item.RangeGeneration;
	}

	public int Compate(QueryInfo item)
	{
		return this.Name.CompareTo(item.Name);
	}

	public void Copy(QueryInfo item)
	{
		this.Name = item.Name;
		this.Query = item.Query;
		this.TableName = item.TableName;
		this.mTableGeneration = item.mTableGeneration;
		this.mRangeGeneration = item.mRangeGeneration;
	}

	public QueryInfo Clone()
	{
		QueryInfo item = new QueryInfo();

		item.Copy(this);

		return item;
	}

	public override string ToString()
	{
		return mName;
	}
	#endregion

	#region Members
	private String mName;
	private String mQuery;
	private String mTableName;
	private bool mTableGeneration;
	private bool mRangeGeneration;
	#endregion
}
}
