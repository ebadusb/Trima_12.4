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
using System.Text;

/* Classes *******************************************************************/

public interface IAssetItem<TYPE> {
	bool Equals(TYPE item);
	
	int Compate(TYPE item);

	void Copy(TYPE item);

	TYPE Clone();
}
}
