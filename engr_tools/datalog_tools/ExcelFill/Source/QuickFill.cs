/*****************************************************************************\
**
** QuickFill
**
** Implementation of the quick fill engine.
**
\*****************************************************************************/

namespace ExcelFill {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Serialization;
using Microsoft.Office.Core;
using Excel = Microsoft.Office.Interop.Excel;
using AddinUtility;
using ExcelUtility.Asset;
using ExcelUtility.Fill;

/* Classes *******************************************************************/

public class QuickFill {
	#region Globals
	private static readonly QuickFill gQuickFill = new QuickFill();
	private static readonly String gConfigFile = "QuickFill.config";
	private static readonly String gEventFile = "QuickFill.event";

	public static QuickFill GetInstance()
	{
		return gQuickFill;
	}
	#endregion

	#region Constructor
	private QuickFill()
	{
		System.Windows.Forms.Application.EnableVisualStyles();

		mMainWindow = new NativeWindow();
		mFillWindow = new ExcelFill.Windows.FillWindow();
	}
	#endregion

	#region Properties
	public String Version
	{
		get { return Properties.Resources.VersionNum; }
	}
	#endregion

	#region Handlers
	private void ProcessFillHandler(object sender, ProcessAssetArgs<FillInfo> args)
	{
		this.ProcessFill(args.Item);
	}
	#endregion

	#region Functions
	public void Connect(Excel.Application app)
	{
		mEventLog = EventLogFactory.CreateEventLog(ExternalFileDirectory.GetEventFolder(), gEventFile, 10);
		mEventLog.Open();
		mApplication = app;
		mMainWindow.AssignHandle(new IntPtr(mApplication.Hwnd));
		this.ImportSettings();
	}

	public void Disconnect()
	{
		mMainWindow.ReleaseHandle();
		this.ExportSettings();
		mEventLog.Close();
	}

	public void ShowFillWindow()
	{
		try {
			mFillWindow.ShowDialog(mMainWindow);
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ImportSettings()
	{
		String file = ExternalFileDirectory.GetConfigFolder() + "\\" + gConfigFile;

		if (!System.IO.File.Exists(file)) {
			System.IO.File.WriteAllText(file, Properties.Resources.Default);
		}

		if (System.IO.File.Exists(file)) {
			XmlTextReader reader = null;
			XmlSerializer serializer = null;
			FillSerialize data = null;

			try {
				serializer = new XmlSerializer(typeof(FillSerialize));
				reader = new XmlTextReader(file);
				data = (FillSerialize)serializer.Deserialize(reader);

				mFillWindow.FillInfo = data.CurrentFillInfo;
			}
			catch {
				System.Windows.Forms.MessageBox.Show(
						"Could not create \"" + file + "\"",
						"Error",
						System.Windows.Forms.MessageBoxButtons.OK,
						System.Windows.Forms.MessageBoxIcon.Error);
			}

			if (reader != null) {
				reader.Close();
			}
		}
	}

	public void ExportSettings()
	{
		String file = ExternalFileDirectory.GetConfigFolder() + "\\" + gConfigFile;
		XmlTextWriter writer = null;
		XmlSerializer serializer = null;
		FillSerialize data = new FillSerialize();

		try {
			data.CurrentFillInfo = mFillWindow.FillInfo;

			serializer = new XmlSerializer(typeof(FillSerialize));
			writer = new XmlTextWriter(file, System.Text.Encoding.Default);
			serializer.Serialize(writer, data);
		}
		catch {
			System.Windows.Forms.MessageBox.Show(
					"Could not create \"" + file + "\"",
					"Error",
					System.Windows.Forms.MessageBoxButtons.OK,
					System.Windows.Forms.MessageBoxIcon.Error);
		}

		if (writer != null) {
			writer.Close();
		}
	}

	public void ProcessDefaultFill()
	{
		try {
			this.ProcessFill(mFillWindow.FillInfo);
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ProcessFill(FillInfo item)
	{
		try {
			if (!this.IsEditMode()) {
				FillExcel.Fill(item, mApplication, mEventLog);
			}
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	private bool IsEditMode()
	{
		const int MENU_ITEM_TYPE = 1;
		const int NEW_MENU = 18;

		CommandBar commandBar = mApplication.CommandBars["Worksheet Menu Bar"];
		CommandBarControl menuItem = commandBar.FindControl(MENU_ITEM_TYPE, NEW_MENU, Type.Missing, Type.Missing, true);

		return (menuItem != null) && (!menuItem.Enabled);
	}
	#endregion

	#region Members
	private Excel.Application mApplication;

	private NativeWindow mMainWindow;
	private Windows.FillWindow mFillWindow;

	private EventLog mEventLog;
	#endregion
}
}
