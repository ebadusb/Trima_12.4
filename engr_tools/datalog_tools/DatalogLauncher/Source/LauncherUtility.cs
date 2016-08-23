﻿/*****************************************************************************\
**
** LauncherUtility
**
** Implementation of the launcher utility.
**
\*****************************************************************************/

namespace DatalogLauncher {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Windows.Forms;

/* Classes *******************************************************************/

public class LauncherUtility {
	#region Functions
	public static String GetTextFromClipboard()
	{
		IDataObject data = Clipboard.GetDataObject();

		if (data.GetDataPresent(DataFormats.Text)) {
			return data.GetData(DataFormats.Text).ToString();
		}

		return null;
	}

	public static LauncherWindow GetWindowForSettings(LauncherEngine engine)
	{
		engine.LoadSettings();

		if (engine.Settings.Applications.Count > 0) {
			return new LauncherWindowExtended(engine);
		}
		else {
			return new LauncherWindowSimple(engine);
		}
	}
	#endregion
}
}
