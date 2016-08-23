/*****************************************************************************\
**
** AssetBatch
**
** Implementation of a batch asset.
**
\*****************************************************************************/

namespace ExcelUtility.Asset {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;

/* Classes *******************************************************************/

public class AssetBatch<TYPE> : IAssetName<AssetBatch<TYPE>>, IAssetItem<AssetBatch<TYPE>>  
		where TYPE: IAssetName<TYPE>, IAssetItem<TYPE> {
	#region Constructor
	public AssetBatch()
	{
		mName = "";
		mComment = "";
		mList = new List<String>();
	}
	#endregion

	#region Properties
	public String Name
	{
		get { return mName; }
		set { mName = value; }
	}

	public String Comment
	{
		get { return mComment; }
		set { mComment = value; }
	}

	public IEnumerable<String> Items
	{
		get { return mList; }
	}

	public List<String> List
	{
		get { return mList; }
		set { mList = value; }
	}
	#endregion

	#region Functions
	public void Insert(TYPE item)
	{
		mList.Add(item.Name);
	}

	public void Remove(TYPE item)
	{
		mList.Remove(item.Name);
	}

	public bool Contains(TYPE item)
	{
		return mList.Exists(item.Name.Equals);
	}

	public bool Equals(AssetBatch<TYPE> batch)
	{
		if (!this.Name.Equals(batch.Name)
			|| !this.Comment.Equals(batch.Comment)
			|| this.mList.Count != batch.mList.Count) {
			return false;
		}

		foreach (String name in this.mList) {
			if (!batch.mList.Exists(name.Equals)) {
				return false;
			}
		}

		return true;
	}

	public int Compate(AssetBatch<TYPE> batch)
	{
		return this.Name.CompareTo(batch.Name);
	}

	public void Copy(AssetBatch<TYPE> batch)
	{
		this.Name = batch.Name;
		this.Comment = batch.Comment;
		this.mList.Clear();
		this.mList.AddRange(batch.mList);
	}

	public AssetBatch<TYPE> Clone()
	{
		AssetBatch<TYPE> item = new AssetBatch<TYPE>();

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
	private String mComment;
	private List<String> mList;
	#endregion
}
}
