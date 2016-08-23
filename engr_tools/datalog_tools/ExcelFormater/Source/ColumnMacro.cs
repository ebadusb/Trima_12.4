/*****************************************************************************\
**
** ColumnMacro
**
** Implementation of a column macro.
**
\*****************************************************************************/

namespace ExcelFormater {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using ExcelUtility.Asset;

/* Classes *******************************************************************/

public class ColumnMacro : IAssetName<ColumnMacro>, IAssetItem<ColumnMacro> {
	#region Constructor
	public ColumnMacro()
	{
		mName = "";
		mFunction = "";
		mColumns = new List<string>();
	}
	#endregion
	
	#region Properties
	public String Name
	{
		get { return mName; }
		set { mName = value; }
	}

	public String Function
	{
		get { return mFunction; }
		set { mFunction = value; }
	}

	public List<String> List
	{
		get { return mColumns; }
		set { mColumns = value; }
	}
	#endregion

	#region Functions
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

	public override string ToString()
	{
		return mName;
	}

	public bool Equals(ColumnMacro item)
	{
		if (!this.Name.Equals(item.Name)
				|| !this.Function.Equals(item.Function)
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

	public int Compate(ColumnMacro item)
	{
		return this.Name.CompareTo(item.Name);
	}

	public void Copy(ColumnMacro item)
	{
		this.Name = item.Name;
		this.Function = item.Function;
		mColumns.Clear();
		mColumns.AddRange(item.mColumns);
	}

	public ColumnMacro Clone()
	{
		ColumnMacro item = new ColumnMacro();

		item.Copy(this);

		return item;
	}
	#endregion

	#region Members
	private String mName;
	private String mFunction;
	private List<String> mColumns;
	#endregion
}

}