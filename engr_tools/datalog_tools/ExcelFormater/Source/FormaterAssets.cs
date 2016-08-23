/*****************************************************************************\
**
** FormaterAssets
**
** Implementation of the formater asset repository.
**
\*****************************************************************************/

namespace ExcelFormater {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using ExcelUtility.Asset;
using ExcelUtility.Format;

/* Classes *******************************************************************/

public class FormaterAssets {
	#region Constructor
	public FormaterAssets()
	{
		mColumnFiltersManager = new AssetManagerNamed<ColumnFilter>();
		mColumnListsManager = new AssetManagerNamed<ColumnList>();
		mColumnMacroManager = new AssetManager<CustomMacro>();
		mTableMacroManager = new AssetManager<CustomMacro>();
		mFormatInfosManager = new AssetManagerNamed<FormatInfo>();
		mKnownColumnAssetManager = new AssetManagerUnique<StringAsset>();
		mKnownLevelAssetManager = new AssetManagerUnique<StringAsset>();
		mAvailableColumnAssetManager = new AssetManagerUnique<StringAsset>();
		mAvailableLevelAssetManager = new AssetManagerUnique<StringAsset>();
	}
	#endregion

	#region Properties
	public AssetManager<ColumnFilter> ColumnFiltersManager
	{
		get { return mColumnFiltersManager; }
	}

	public AssetManager<ColumnList> ColumnListsManager
	{
		get { return mColumnListsManager; }
	}

	public AssetManager<CustomMacro> ColumnMacroManager
	{
		get { return mColumnMacroManager; }
	}

	public AssetManager<CustomMacro> TableMacroManager
	{
		get { return mTableMacroManager; }
	}

	public AssetManager<FormatInfo> FormatInfosManager
	{
		get { return mFormatInfosManager; }
	}

	public AssetManager<StringAsset> KnownColumnAssetManager
	{
		get { return mKnownColumnAssetManager; }
	}

	public AssetManager<StringAsset> KnownLevelAssetManager
	{
		get { return mKnownLevelAssetManager; }
	}

	public AssetManager<StringAsset> AvailableColumnAssetManager
	{
		get { return mAvailableColumnAssetManager; }
	}

	public AssetManager<StringAsset> AvailableLevelAssetManager
	{
		get { return mAvailableLevelAssetManager; }
	}
	#endregion
	
	#region Members
	private AssetManager<ColumnFilter> mColumnFiltersManager;
	private AssetManager<ColumnList> mColumnListsManager;
	private AssetManager<CustomMacro> mColumnMacroManager;
	private AssetManager<CustomMacro> mTableMacroManager;
	private AssetManager<FormatInfo> mFormatInfosManager;
	private AssetManager<StringAsset> mKnownColumnAssetManager;
	private AssetManager<StringAsset> mKnownLevelAssetManager;
	private AssetManager<StringAsset> mAvailableColumnAssetManager;
	private AssetManager<StringAsset> mAvailableLevelAssetManager;
	#endregion
}
}
