/*****************************************************************************\
**
** SearcherFunctions
**
** Provides interface to the search engine.
**
\*****************************************************************************/

namespace ExcelSearch {

/* Usings ********************************************************************/

using System;
using Extensibility;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using ExcelUtility.Asset;
using ExcelUtility.Search;

/* Interface *****************************************************************/

public interface ISearcherFunctions {
	void ShowSearchWindow();
	void ShowManagerWindow();
	void ShowSettingsWindow();
	
	void ImportSettings();
	void ExportSettings();

	void ProcessSearch(String name);
	void ProcessBatch(String name);
}

/* Classes *******************************************************************/

[ComDefaultInterface(typeof(ISearcherFunctions))]
public class SearcherFunctions : ISearcherFunctions {

	[ComRegisterFunctionAttribute]
	public static void RegisterFunction(Type type)
	{
		Microsoft.Win32.Registry.ClassesRoot.CreateSubKey(GetSubKeyName(type));
	}

	[ComUnregisterFunctionAttribute]
	public static void UnregisterFunction(Type type)
	{
		Microsoft.Win32.Registry.ClassesRoot.DeleteSubKey(GetSubKeyName(type), false);
	}

	private static string GetSubKeyName(Type type)
	{
		return @"CLSID\{" + type.GUID.ToString().ToUpper() + @"}\Programmable";
	}

	public void ShowSearchWindow()
	{
		ExtendedSearch.GetInstance().ShowSearchWindow();
	}

	public void ShowManagerWindow()
	{
		ExtendedSearch.GetInstance().ShowManagerWindow();
	}

	public void ShowSettingsWindow()
	{
		ExtendedSearch.GetInstance().ShowSettingsWindow();
	}

	public void ImportSettings()
	{
		ExtendedSearch.GetInstance().ImportSettings();
	}

	public void ExportSettings()
	{
		ExtendedSearch.GetInstance().ExportSettings();
	}

	public void ProcessSearch(String name)
	{
		AssetManager<SearchInfo> manager = ExtendedSearch.GetInstance().Assets.SearchInfoManager;
		ItemNameCompare<SearchInfo> compare = new ItemNameCompare<SearchInfo>(name);

		if (manager.Exists(compare.Equals)) {
			ExtendedSearch.GetInstance().ProcessSearch(manager.Find(compare.Equals));
		}
	}

	public void ProcessBatch(String name)
	{
		AssetManager<AssetBatch<SearchInfo>> manager = ExtendedSearch.GetInstance().Assets.BatchInfoManager;
		ItemNameCompare<AssetBatch<SearchInfo>> compare = new ItemNameCompare<AssetBatch<SearchInfo>>(name);

		if (manager.Exists(compare.Equals)) {
			ExtendedSearch.GetInstance().ProcessBatch(manager.Find(compare.Equals));
		}
	}
}
}



