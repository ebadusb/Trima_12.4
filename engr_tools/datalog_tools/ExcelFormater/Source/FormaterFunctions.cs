/*****************************************************************************\
**
** FormaterFunctions
**
** Provides interface to the formater engine.
**
\*****************************************************************************/

namespace ExcelFormater {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using ExcelUtility.Asset;
using ExcelUtility.Format;

/* Interface *****************************************************************/

public interface IFormaterFunctions {
	void ShowFormatWindow();
	void ShowManagerWindow();
	void ShowSettingsWindow();

	void ImportSettings();
	void ExportSettings();

	void ProcessFormat(String name);
}

/* Classes *******************************************************************/

[ComDefaultInterface(typeof(IFormaterFunctions))]
public class FormaterFunctions : IFormaterFunctions {

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

	public void ShowFormatWindow()
	{
		ExcelFormater.GetInstance().ShowFormatWindow();
	}

	public void ShowManagerWindow()
	{
		ExcelFormater.GetInstance().ShowManagerWindow();
	}

	public void ShowSettingsWindow()
	{
		ExcelFormater.GetInstance().ShowSettingsWindow();
	}

	public void ImportSettings()
	{
		ExcelFormater.GetInstance().ImportSettings();
	}

	public void ExportSettings()
	{
		ExcelFormater.GetInstance().ExportSettings();
	}

	public void ProcessFormat(String name)
	{
		AssetManager<FormatInfo> manager = ExcelFormater.GetInstance().Assets.FormatInfosManager;
		ItemNameCompare<FormatInfo> compare = new ItemNameCompare<FormatInfo>(name);

		if (manager.Exists(compare.Equals)) {
			ExcelFormater.GetInstance().ProcessFormat(manager.Find(compare.Equals));
		}
	}
}
}
