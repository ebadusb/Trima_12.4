/*****************************************************************************\
**
** FillSerialize
**
** Implementation of serializable data.
**
\*****************************************************************************/

namespace ExcelFill {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using ExcelUtility.Fill;

/* Classes *******************************************************************/

public class FillSerialize {
	#region Constructor
	public FillSerialize()
	{
		mCurrentFillInfo = new FillInfo();
	}
	#endregion

	#region Properties
	public FillInfo CurrentFillInfo
	{
		get { return mCurrentFillInfo; }
		set { mCurrentFillInfo = value; }
	}
	#endregion

	#region Members
	private FillInfo mCurrentFillInfo;
	#endregion
}
}
