/*****************************************************************************\
**
** FormaterSerialize
**
** Implementation of serializable data.
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

public class FormaterSerialize {
	#region Constructor
	public FormaterSerialize()
	{
		mFormaterSettings = new FormaterSettings();
		mDatalogColumns = new List<string>();
		mDatalogLevels = new List<string>();
		mColumnLists = new List<ColumnList>();
		mColumnFilters = new List<ColumnFilter>();
		mColumnMacros = new List<CustomMacro>();
		mTableMacros = new List<CustomMacro>();
		mFormatInfos = new List<FormatInfo>();
		mCurrentFormatInfo = new FormatInfo();
	}
	#endregion

	#region Properties
	public FormaterSettings FormaterSettings
	{
		get { return mFormaterSettings; }
		set { mFormaterSettings = value; }
	}

	public List<String> DatalogColumns
	{
		get { return mDatalogColumns; }
		set { mDatalogColumns = value; }
	}

	public List<String> DatalogLevels
	{
		get { return mDatalogLevels; }
		set { mDatalogLevels = value; }
	}

	public List<ColumnList> ColumnLists
	{
		get { return mColumnLists; }
		set { mColumnLists = value; }
	}

	public List<ColumnFilter> ColumnFilters
	{
		get { return mColumnFilters; }
		set { mColumnFilters = value; }
	}

	public List<CustomMacro> ColumnMacros
	{
		get { return mColumnMacros; }
		set { mColumnMacros = value; }
	}

	public List<CustomMacro> TableMacros
	{
		get { return mTableMacros; }
		set { mTableMacros = value; }
	}

	public List<FormatInfo> FormatInfos
	{
		get { return mFormatInfos; }
		set { mFormatInfos = value; }
	}

	public FormatInfo CurrentFormatInfo
	{
		get { return mCurrentFormatInfo; }
		set { mCurrentFormatInfo = value; }
	}
	#endregion

	#region Members
	private FormaterSettings mFormaterSettings;
	private List<String> mDatalogColumns;
	private List<String> mDatalogLevels;
	private List<ColumnList> mColumnLists;
	private List<ColumnFilter> mColumnFilters;
	private List<CustomMacro> mColumnMacros;
	private List<CustomMacro> mTableMacros;
	private List<FormatInfo> mFormatInfos;
	private FormatInfo mCurrentFormatInfo;
	#endregion
}
}
