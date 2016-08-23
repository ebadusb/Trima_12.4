/*****************************************************************************\
**
** IAssetItem
**
** Interface for assets.
**
\*****************************************************************************/

namespace ExcelUtility.Asset {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

/* Classes *******************************************************************/

public interface IAssetName<TYPE> {
	String Name
	{
		get;
		set;
	}
}
}
