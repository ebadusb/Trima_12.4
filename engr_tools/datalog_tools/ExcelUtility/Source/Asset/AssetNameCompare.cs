/*****************************************************************************\
**
** AssetNameCompare
**
** Compares the name of assets.
**
\*****************************************************************************/

namespace ExcelUtility.Asset {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;

/* Classes *******************************************************************/

public class AssetNameCompare<TYPE> where TYPE : IAssetName<TYPE> {
	#region Constructor
	public AssetNameCompare(TYPE item)
	{
		mItem = item;
	}
	#endregion

	#region Functions
	public bool Equals(TYPE item)
	{
		return mItem.Name.Equals(item.Name);
	}

	public int Compate(TYPE item)
	{
		return mItem.Name.CompareTo(item.Name);
	}
	#endregion

	#region Members
	private TYPE mItem;
	#endregion
}
}
