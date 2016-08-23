/*****************************************************************************\
**
** Connect
**
** Implementation of Excel Add-in interface.
**
\*****************************************************************************/

namespace ExcelSearch {

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
using ExcelUtility.Asset;
using ExcelUtility.Search;
using AddinUtility;

/* Classes *******************************************************************/

[GuidAttribute("D5D0B7C8-18E3-4B0B-87A2-77A99994E945"), ProgId("ExcelSearch.Connect")]
public class Connect : Object,  IDTExtensibility2, IRibbonExtensibility {
	#region Constructor
	public Connect()
	{
	}
	#endregion

	#region IDTExtensibility2
	/**
	 *
	 * OnDisconnection
	 *
	 * Implements the OnConnection method of the IDTExtensibility2 interface.
	 * Receives notification that the Add-in is being loaded.
	 *
	 * @param application Root object of the host application.
	 * @param mode Describes how the Add-in is being loaded.
	 * @param addInInst Object representing this Add-in. 
	 * @param custom Array of parameters that are host application specific.   
	 *
	 */
	public void OnConnection(object application, Extensibility.ext_ConnectMode mode, object addInInst, ref System.Array custom)
	{
		mInstance = addInInst;
		mApplication = (Excel.Application)application;

		ExtendedSearch.GetInstance().Connect((Excel.Application)application);
	}

	/**
	 *
	 * OnDisconnection
	 *
	 * Implements the OnDisconnection method of the IDTExtensibility2 interface.
	 * Receives notification that the Add-in is being unloaded.
	 *
	 * @param mode Describes how the Add-in is being unloaded.
	 * @param custom Array of parameters that are host application specific. 
	 *
	 */
	public void OnDisconnection(Extensibility.ext_DisconnectMode mode, ref System.Array custom)
	{
		mInstance = null;
		mApplication = null;

		ExtendedSearch.GetInstance().Disconnect();
	}

	/**
	 *
	 * OnAddInsUpdate
	 *
	 * Implements the OnAddInsUpdate method of the IDTExtensibility2 interface.
	 * Receives notification that the collection of Add-ins has changed.
	 *
	 * @param custom Array of parameters that are host application specific.
	 *
	 */
	public void OnAddInsUpdate(ref System.Array custom)
	{
	}

	/**
	 *
	 * OnStartupComplete
	 *
	 * Implements the OnStartupComplete method of the IDTExtensibility2 interface.
	 * Receives notification that the host application has completed loading.
	 *
	 * @param custom Array of parameters that are host application specific.
	 *
	 */
	public void OnStartupComplete(ref System.Array custom)
	{
	}

	/**
	 *
	 * OnBeginShutdown
	 *
	 * Implements the OnBeginShutdown method of the IDTExtensibility2 interface.
	 * Receives notification that the host application is being unloaded.
	 *
	 * @param custom Array of parameters that are host application specific.
	 *
	 */
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
	/**
	 *
	 * SearchWorksheet
	 *
	 * Handler that launches the Trima Configurator.
	 *
	 * @param control The ribbon control object.
	 *
	 */
	public void SearchWorksheet(IRibbonControl control)
	{
		ExtendedSearch.GetInstance().ShowSearchWindow();
	}

	public void SearchManager(IRibbonControl control)
	{
		ExtendedSearch.GetInstance().ShowManagerWindow();
	}

	public void SearchSettings(IRibbonControl control)
	{
		ExtendedSearch.GetInstance().ShowSettingsWindow();
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
		else if (imageName == "quick-48x48.png") {
			image = Properties.Resources.quick_48x48;
		}
		else if (imageName == "settings-48x48.png") {
			image = Properties.Resources.settings_48x48;
		}

		return image != null ? PictureConverter.ImageToPictureDisp(image) : null;
	}

	public void RibbonLoaded(IRibbonUI ribbon)
	{
		mRibbon = ribbon;
	}
	#endregion
	
	#region Members
	private Excel.Application mApplication;
	private IRibbonUI mRibbon;
	private object mInstance;
	#endregion
}
}