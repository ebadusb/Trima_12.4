/*****************************************************************************\
**
** QueryUtility
**
** Implementation of utility classes.
**
\*****************************************************************************/

namespace ExcelQuery {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using ExcelUtility.Asset;

/* Classes *******************************************************************/

public class ItemNameCompare<TYPE> where TYPE : IAssetName<TYPE> {
	#region Constructor
	public ItemNameCompare(String name)
	{
		mName = name;
	}
	#endregion

	#region Functions
	public bool Equals(TYPE item)
	{
		return mName.Equals(item.Name);
	}
	#endregion

	#region Members
	private String mName;
	#endregion
}
}
