/*****************************************************************************\
**
** FillFunctions
**
** Provides interface to the fill engine.
**
\*****************************************************************************/

namespace ExcelFill {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using ExcelUtility.Asset;
using ExcelUtility.Fill;

/* Interface *****************************************************************/

public interface IFillFunctions {
	void ShowSettingsWindow();

	void ImportSettings();
	void ExportSettings();

	void ProcessDefault();
}

/* Classes *******************************************************************/

[ComDefaultInterface(typeof(IFillFunctions))]
class FillFunctions : IFillFunctions {
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

	public void ShowSettingsWindow()
	{
		QuickFill.GetInstance().ShowFillWindow();
	}

	public void ImportSettings()
	{
		QuickFill.GetInstance().ImportSettings();
	}

	public void ExportSettings()
	{
		QuickFill.GetInstance().ExportSettings();
	}

	public void ProcessDefault()
	{
		QuickFill.GetInstance().ProcessDefaultFill();
	}
}
}
