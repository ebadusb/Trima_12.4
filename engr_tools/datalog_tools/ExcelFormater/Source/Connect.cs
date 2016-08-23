/*****************************************************************************\
**
** Connect
**
** Implementation of Excel Add-in interface.
**
\*****************************************************************************/

namespace ExcelFormater {

/* Usings ********************************************************************/

using System;
using Extensibility;
using System.Runtime.InteropServices;
using Microsoft.Office.Core;
using System.Windows.Forms;
using Excel = Microsoft.Office.Interop.Excel;
using Threading = System.Threading;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using ExcelUtility.Asset;
using ExcelUtility.Query;
using ExcelUtility.Format;
using AddinUtility;

/* Classes *******************************************************************/
	
[GuidAttribute("9FA20BCA-F263-4B18-A4AC-4F1A345599EE"), ProgId("ExcelFormater.Connect")]
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

		ExcelFormater.GetInstance().Connect((Excel.Application)application);
	}

	public void OnDisconnection(Extensibility.ext_DisconnectMode disconnectMode, ref System.Array custom)
	{
		mInstance = null;
		mApplication = null;

		ExcelFormater.GetInstance().Disconnect();
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
	public void FormatDatalog(IRibbonControl control)
	{
		ExcelFormater.GetInstance().ShowFormatWindow();
	}

	public void FormatManager(IRibbonControl control)
	{
		ExcelFormater.GetInstance().ShowManagerWindow();
	}

	public void FormatSettings(IRibbonControl control)
	{
		ExcelFormater.GetInstance().ShowSettingsWindow();
	}

	public void RibbonLoaded(IRibbonUI ribbon)
	{
		mRibbon = ribbon;
	}

	public stdole.IPictureDisp GetImage(string imageName)
	{
		System.Drawing.Bitmap image = null;

		if (imageName == "format-48x48.png") {
			image = Properties.Resources.format_48x48;
		}
		else if (imageName == "manage-48x48.png") {
			image = Properties.Resources.manage_48x48;
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
