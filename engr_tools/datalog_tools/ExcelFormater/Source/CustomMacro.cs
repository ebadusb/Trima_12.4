/*****************************************************************************\
**
** CustomMacro
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

public class CustomMacro : IAssetName<CustomMacro>, IAssetItem<CustomMacro> {
	#region Constructor
	public CustomMacro()
	{
		mName = "";
		mFunction = "";
		mItems = new List<string>();
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
		get { return mItems; }
		set { mItems = value; }
	}
	#endregion

	#region Functions
	public void Insert(String name)
	{
		mItems.Add(name);
	}

	public void Remove(String name)
	{
		mItems.Remove(name);
	}

	public bool Contains(String name)
	{
		return mItems.Exists(name.Equals);
	}

	public override string ToString()
	{
		return mName;
	}

	public bool Equals(CustomMacro item)
	{
		if (!this.Name.Equals(item.Name)
				|| !this.Function.Equals(item.Function)
				|| this.mItems.Count != item.mItems.Count) {
			return false;
		}

		foreach (String name in this.mItems) {
			if (!item.mItems.Exists(name.Equals)) {
				return false;
			}
		}

		return true;
	}

	public int Compate(CustomMacro item)
	{
		return this.Name.CompareTo(item.Name);
	}

	public void Copy(CustomMacro item)
	{
		this.Name = item.Name;
		this.Function = item.Function;
		mItems.Clear();
		mItems.AddRange(item.mItems);
	}

	public CustomMacro Clone()
	{
		CustomMacro item = new CustomMacro();

		item.Copy(this);

		return item;
	}
	#endregion

	#region Members
	private String mName;
	private String mFunction;
	private List<String> mItems;
	#endregion
}

}