/*****************************************************************************\
**
** Connect
**
** Implementation of Excel Add-in interface.
**
\*****************************************************************************/

namespace ExcelFill {

/* Usings ********************************************************************/

using System;
using Extensibility;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Microsoft.Office.Core;
using Excel = Microsoft.Office.Interop.Excel;
using AddinUtility;

/* Classes *******************************************************************/

[GuidAttribute("B16BEA40-E70A-4913-A3A8-FC65C2DE6863"), ProgId("ExcelFill.Connect")]
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

		QuickFill.GetInstance().Connect((Excel.Application)application);
	}

	public void OnDisconnection(Extensibility.ext_DisconnectMode disconnectMode, ref System.Array custom)
	{
		mInstance = null;
		mApplication = null;

		QuickFill.GetInstance().Disconnect();
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
	public void FillDefault(IRibbonControl control)
	{
		QuickFill.GetInstance().ProcessDefaultFill();
	}

	public void FillSettings(IRibbonControl control)
	{
		QuickFill.GetInstance().ShowFillWindow();
	}

	public void RibbonLoaded(IRibbonUI ribbon)
	{
		mRibbon = ribbon;
	}

	public stdole.IPictureDisp GetImage(string imageName)
	{
		System.Drawing.Bitmap image = null;

		if (imageName == "fill-48x48.png") {
			image = Properties.Resources.fill_48x48;
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