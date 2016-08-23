/*****************************************************************************\
**
** Connect
**
** Implementation of Excel Add-in interface.
**
\*****************************************************************************/

namespace ExcelQuery {

/* Usings ********************************************************************/

using System;
using Extensibility;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using System.Drawing;
using Microsoft.Office.Core;
using Excel = Microsoft.Office.Interop.Excel;
using ExcelUtility.Query;
using AddinUtility;

/* Classes *******************************************************************/

[GuidAttribute("A40ACAEC-CD32-43C8-B03E-0459F23F6DC0"), ProgId("ExcelQuery.Connect")]
public class Connect : Object, IDTExtensibility2, IRibbonExtensibility {
	#region Constructor
	public Connect()
	{
	}
	#endregion

	#region IDTExtensibility2
	public void OnConnection(object application, Extensibility.ext_ConnectMode connectMode, object addInInst, ref System.Array custom)
	{
		mInstance = addInInst;
		mApplication = (Excel.Application)application;

		ExcelQuery.GetInstance().Connect((Excel.Application)application);
	}

	public void OnDisconnection(Extensibility.ext_DisconnectMode disconnectMode, ref System.Array custom)
	{
		mInstance = null;
		mApplication = null;

		ExcelQuery.GetInstance().Disconnect();
	}

	public void OnAddInsUpdate(ref System.Array custom)
	{
	}

	public void OnStartupComplete(ref System.Array custom)
	{
	}

	public void OnBeginShutdown(ref System.Array custom)
	{
	}
	#endregion

	#region IRibbonExtensibility
	public string GetCustomUI(string id)
	{
		return Properties.Resources.Ribbon;
	}
	#endregion

	#region Handlers 
	public void QueryWorksheet(IRibbonControl control)
	{
		ExcelQuery.GetInstance().ShowQueryWindow();
	}

	public void SearchManager(IRibbonControl control)
	{
		ExcelQuery.GetInstance().ShowManagerWindow();
	}

	public void SearchSettings(IRibbonControl control)
	{
		ExcelQuery.GetInstance().ShowSettingsWindow();
	}

	public void RibbonLoaded(IRibbonUI ribbon)
	{
		mRibbon = ribbon;
	}

	public stdole.IPictureDisp GetImage(string imageName)
	{
		System.Drawing.Bitmap image = null;

		if (imageName == "search-48x48.png") {
			image = Properties.Resources.search_48x48;
		}
		else if (imageName == "manager-48x48.png") {
			image = Properties.Resources.manager_48x48;
		}
		else if (imageName == "settings-48x48.png") {
			image = Properties.Resources.settings_48x48;
		}

		return image != null ? PictureConverter.ImageToPictureDisp(image) : null;
	}
	#endregion

	#region Members
	private object mInstance;
	private Excel.Application mApplication;
	private IRibbonUI mRibbon;
	#endregion
}
}