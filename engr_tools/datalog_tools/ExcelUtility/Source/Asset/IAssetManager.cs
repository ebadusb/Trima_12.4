/*****************************************************************************\
**
** IAssetManager
**
** Implementation of the asset manager interface.
**
\*****************************************************************************/

namespace ExcelUtility.Asset {

/* Usings ********************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

/* Classes *******************************************************************/

public interface IAssetManager<TYPE> : IEnumerable<TYPE> where TYPE : IAssetItem<TYPE> {
	#region Properties
	int Count
	{
		get;
	}
	#endregion

	#region Functions
	bool New(TYPE newItem);

	bool Edit(TYPE oldItem, TYPE newItem);

	bool Delete(TYPE delItem);

	bool Exists(TYPE item);

	bool Exists(Predicate<TYPE> match);

	TYPE Find(Predicate<TYPE> match);
	#endregion
}
}
