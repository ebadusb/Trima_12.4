/*****************************************************************************\
**
** FormaterSettings
**
** Implementation of formater settings.
**
\*****************************************************************************/

namespace ExcelFormater {

/* Usings ********************************************************************/

using System;
using System.Text;
using System.Xml.Serialization;
using System.Collections.Generic;
using ExcelUtility.Format;

/* Classes *******************************************************************/

public class FormaterSettings {
	#region Constructor
	public FormaterSettings()
	{
		mDefaultFormatInfo = "";
		mFormatOnOpen = false;
		mHideDefaultSheets = false;
	}
	#endregion

	#region Properties
	public String DefaultFormatInfo
	{
		get { return mDefaultFormatInfo; }
		set { mDefaultFormatInfo = value; }
	}
	
	public bool FormatOnOpen
	{
		get { return mFormatOnOpen; }
		set { mFormatOnOpen = value; }
	}

	public bool HideDefaultSheets
	{
		get { return mHideDefaultSheets; }
		set { mHideDefaultSheets = value; }
	}
	#endregion

	#region Members
	private String mDefaultFormatInfo;
	private bool mFormatOnOpen;
	private bool mHideDefaultSheets;
	#endregion
}
}
