/*****************************************************************************\
**
** ColumnCategory
**
** Implementation of a column category.
**
\*****************************************************************************/

namespace ExcelFormater {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;

/* Classes *******************************************************************/

public abstract class ColumnCategory {
	#region Constructor
	public ColumnCategory()
	{
		mName = "";
	}
	#endregion
	
	#region Properties
	public String Name
	{
		get { return mName; }
		set { mName = value; }
	}
	#endregion

	#region Functions
	public abstract IEnumerable<String> GetColumns();

	public override string ToString()
	{
		return mName;
	}
	#endregion

	#region Members
	private String mName;
	#endregion
}
}
