/*****************************************************************************\
**
** ColumnList
**
** Implementation of a column list category.
**
\*****************************************************************************/

namespace ExcelFormater {

/* Usings ********************************************************************/
	
using System;
using System.Collections.Generic;
using System.Text;
using ExcelUtility.Asset;

/* Classes *******************************************************************/

public class ColumnList : ColumnCategory, IAssetName<ColumnList>, IAssetItem<ColumnList> {
	#region Constructor
	public ColumnList()
	{
		mColumns = new List<string>();
	}
	#endregion

	#region Properties
	public List<String> List
	{
		get { return mColumns; }
		set { mColumns = value; }
	}
	#endregion

	#region Functions
	public override IEnumerable<String> GetColumns()
	{
		return mColumns; 
	}

	public void Insert(String name)
	{
		mColumns.Add(name);
	}

	public void Remove(String name)
	{
		mColumns.Remove(name);
	}

	public bool Contains(String name)
	{
		return mColumns.Exists(name.Equals);
	}

	public bool Equals(ColumnList item)
	{
		if (!this.Name.Equals(item.Name)
				|| this.mColumns.Count != item.mColumns.Count) {
			return false;
		}

		foreach (String name in this.mColumns) {
			if (!item.mColumns.Exists(name.Equals)) {
				return false;
			}
		}

		return true;
	}

	public int Compate(ColumnList item)
	{
		return this.Name.CompareTo(item.Name);
	}

	public void Copy(ColumnList item)
	{
		this.Name = item.Name;
		mColumns.Clear();
		mColumns.AddRange(item.mColumns);
	}

	public ColumnList Clone()
	{
		ColumnList item = new ColumnList();

		item.Copy(this);

		return item;
	}
	#endregion

	#region Members
	private List<String> mColumns;
	#endregion
}
}
