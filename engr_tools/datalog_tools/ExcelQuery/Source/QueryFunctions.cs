/*****************************************************************************\
**
** QueryFunctions
**
** Provides interface to the query engine.
**
\*****************************************************************************/

namespace ExcelQuery {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using ExcelUtility.Asset;
using ExcelUtility.Query;
	
/* Interface *****************************************************************/

public interface IQueryFunctions {
	void ShowQueryWindow();
	void ShowManagerWindow();
	void ShowSettingsWindow();

	void ImportSettings();
	void ExportSettings();

	void ProcessQuery(String name);
	void ProcessBatch(String name);
}

/* Classes *******************************************************************/

[ComDefaultInterface(typeof(IQueryFunctions))]
public class QueryFunctions : IQueryFunctions {

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

	public void ShowQueryWindow()
	{
		ExcelQuery.GetInstance().ShowQueryWindow();
	}

	public void ShowManagerWindow()
	{
		ExcelQuery.GetInstance().ShowManagerWindow();
	}

	public void ShowSettingsWindow()
	{
		ExcelQuery.GetInstance().ShowSettingsWindow();
	}

	public void ImportSettings()
	{
		ExcelQuery.GetInstance().ImportSettings();
	}

	public void ExportSettings()
	{
		ExcelQuery.GetInstance().ExportSettings();
	}

	public void ProcessQuery(String name)
	{
		AssetManager<QueryInfo> manager = ExcelQuery.GetInstance().Assets.QueryInfoManager;
		ItemNameCompare<QueryInfo> compare = new ItemNameCompare<QueryInfo>(name);

		if (manager.Exists(compare.Equals)) {
			ExcelQuery.GetInstance().ProcessQuery(manager.Find(compare.Equals));
		}
	}

	public void ProcessBatch(String name)
	{
		AssetManager<AssetBatch<QueryInfo>> manager = ExcelQuery.GetInstance().Assets.BatchInfoManager;
		ItemNameCompare<QueryInfo> compare = new ItemNameCompare<QueryInfo>(name);

		if (manager.Exists(compare.Equals)) {
			ExcelQuery.GetInstance().ProcessBatch(manager.Find(compare.Equals));
		}
	}
}
}
