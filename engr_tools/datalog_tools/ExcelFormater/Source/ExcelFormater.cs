/*****************************************************************************\
**
** ExcelFormater
**
** Implementation of the formater engine.
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
using ExcelUtility.Utility;

/* Classes *******************************************************************/

public class ExcelFormater {
	#region Globals
	private static readonly ExcelFormater gExcelFormater = new ExcelFormater();
	private static readonly String gConfigFile = "DatalogFormater.config";
	private static readonly String gEventFile = "DatalogFormater.event";
	private static readonly String gVersionNum = "1.1";

	private static readonly String[] gLogTables = {
			"LogInfo",
			"LogHeader",
			"TaskCreated",
			"TaskDeleted",
			"TimeStamps",
			"EndOfFile",
			"PeriodicSet",
			"LogLevel",
			"StartNetCon",
			"PeriodicSetItem",
			"PeriodicLogInt",
			"PeriodicLogFloat",
			"PeriodicLogString",
			"TextOutput",
			"BinaryData",
			"ConnectionInfo"
	};

	public static ExcelFormater GetInstance()
	{
		return gExcelFormater;
	}
	#endregion

	#region Constructor
	private ExcelFormater()
	{
		System.Windows.Forms.Application.EnableVisualStyles();

		mFormaterSettings = new FormaterSettings();
		mFormaterAssets = new FormaterAssets();
		mMainWindow = new NativeWindow();

		mFormatWindow = new Windows.FormatMainWindow();
		mFormatManager = new Windows.FormatManager();
		mFormatSettings = new Windows.FormatSettingsWindow();

		mFormatWindow.Assets = mFormaterAssets;
		mFormatManager.Assets = mFormaterAssets;
		mFormatSettings.Assets = mFormaterAssets;
		mFormatSettings.Settings = mFormaterSettings;

		mFormatWindow.Format += new Windows.FormatMainWindow.ProcessFormatHandler(this.ProcessFormatHandler);
	}
	#endregion
		
	#region Properties
	public String Version
	{
		get { return gVersionNum; }
	}

	public FormaterAssets Assets
	{
		get { return mFormaterAssets; }
	}

	public FormaterSettings Settings
	{
		get { return mFormaterSettings; }
	}
	#endregion

	#region Handlers
	public void WorkbookOpenHandler(Excel.Workbook workbook)
	{
		if (this.IsDatalogDocument()) {
			FormaterPrepare.UpdateAssetManagers(mApplication.ActiveWorkbook, mFormaterAssets);

			if (mFormaterSettings.FormatOnOpen && !String.IsNullOrEmpty(mFormaterSettings.DefaultFormatInfo)) {
				ItemNameCompare<FormatInfo> compare = new ItemNameCompare<FormatInfo>(mFormaterSettings.DefaultFormatInfo);

				if (mFormaterAssets.FormatInfosManager.Exists(compare.Equals)) {
					this.ProcessFormat(mFormaterAssets.FormatInfosManager.Find(compare.Equals));
				}
			}

			if (mFormaterSettings.HideDefaultSheets) {
				int hidden = 0;

				foreach (Excel.Worksheet worksheet in mApplication.ActiveWorkbook.Worksheets) {
					if (Array.Exists(gLogTables, worksheet.Name.Equals)) {
						if (hidden == mApplication.ActiveWorkbook.Worksheets.Count - 1
								&& worksheet.Visible == Microsoft.Office.Interop.Excel.XlSheetVisibility.xlSheetVisible) {
							mApplication.ActiveWorkbook.Worksheets.Add(Type.Missing, Type.Missing, 1, Excel.XlSheetType.xlWorksheet);
						}

						worksheet.Visible = Excel.XlSheetVisibility.xlSheetHidden;
					}

					if (worksheet.Visible != Microsoft.Office.Interop.Excel.XlSheetVisibility.xlSheetVisible) {
						++hidden;
					}
				}
			}
		}
	}

	private void ProcessFormatHandler(object sender, ProcessAssetArgs<FormatInfo> args)
	{
		this.ProcessFormat(args.Item);
	}
	#endregion

	#region Functions
	public void Connect(Excel.Application app)
	{
		String folder = AddinUtility.ExternalFileDirectory.GetEventFolder();

		mEventLog = AddinUtility.EventLogFactory.CreateEventLog(folder, gEventFile, 10);
		mEventLog.Open();
		mApplication = app;
		mMainWindow.AssignHandle(new IntPtr(mApplication.Hwnd));
		mApplication.WorkbookOpen += new Excel.AppEvents_WorkbookOpenEventHandler(this.WorkbookOpenHandler);
		this.ImportSettings();
	}

	public void Disconnect()
	{
		mMainWindow.ReleaseHandle();
		this.ExportSettings();
		mEventLog.Close();
	}

	public void ShowFormatWindow()
	{
		try {
			mFormatWindow.ShowDialog(mMainWindow);
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ShowManagerWindow()
	{
		try {
			mFormatManager.ShowDialog(mMainWindow);
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ShowSettingsWindow()
	{
		try {
			mFormatSettings.ShowDialog(mMainWindow);
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ImportSettings()
	{
		String file = AddinUtility.ExternalFileDirectory.GetConfigFolder() + "\\" + gConfigFile;

		if (!System.IO.File.Exists(file)) {
			System.IO.File.WriteAllText(file, Properties.Resources.Default);
		}

		if (System.IO.File.Exists(file)) {
			XmlTextReader reader = null;
			XmlSerializer serializer = null;
			FormaterSerialize data = null;

			try {
				serializer = new XmlSerializer(typeof(FormaterSerialize));
				reader = new XmlTextReader(file);
				data = (FormaterSerialize)serializer.Deserialize(reader);

				mFormaterSettings = data.FormaterSettings;

				foreach (String item in data.DatalogColumns) {
					mFormaterAssets.KnownColumnAssetManager.New(new StringAsset(item));
				}

				foreach (String item in data.DatalogLevels) {
					mFormaterAssets.KnownLevelAssetManager.New(new StringAsset(item));
				}

				foreach (ColumnFilter item in data.ColumnFilters) {
					mFormaterAssets.ColumnFiltersManager.New(item);
				}

				foreach (ColumnList item in data.ColumnLists) {
					mFormaterAssets.ColumnListsManager.New(item);
				}

				foreach (CustomMacro item in data.ColumnMacros) {
					mFormaterAssets.ColumnMacroManager.New(item);
				}

				foreach (CustomMacro item in data.TableMacros) {
					mFormaterAssets.TableMacroManager.New(item);
				}

				foreach (FormatInfo item in data.FormatInfos) {
					mFormaterAssets.FormatInfosManager.New(item);
				}

				mFormatWindow.FormatInfo = data.CurrentFormatInfo;

				mFormatWindow.Assets = mFormaterAssets;
				mFormatManager.Assets = mFormaterAssets;
				mFormatSettings.Assets = mFormaterAssets;
				mFormatSettings.Settings = mFormaterSettings;
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
		String file = AddinUtility.ExternalFileDirectory.GetConfigFolder() + "\\" + gConfigFile;
		XmlTextWriter writer = null;
		XmlSerializer serializer = null;
		FormaterSerialize data = new FormaterSerialize();

		try {
			data.FormaterSettings = mFormaterSettings;

			foreach (StringAsset item in mFormaterAssets.KnownColumnAssetManager) {
				data.DatalogColumns.Add(item.Text);
			}

			foreach (StringAsset item in mFormaterAssets.KnownLevelAssetManager) {
				data.DatalogLevels.Add(item.Text);
			}

			foreach (ColumnFilter item in mFormaterAssets.ColumnFiltersManager) {
				data.ColumnFilters.Add(item);
			}

			foreach (ColumnList item in mFormaterAssets.ColumnListsManager) {
				data.ColumnLists.Add(item);
			}

			foreach (CustomMacro item in mFormaterAssets.ColumnMacroManager) {
				data.ColumnMacros.Add(item);
			}

			foreach (CustomMacro item in mFormaterAssets.TableMacroManager) {
				data.TableMacros.Add(item);
			}

			foreach (FormatInfo item in mFormaterAssets.FormatInfosManager) {
				data.FormatInfos.Add(item);
			}

			data.CurrentFormatInfo = mFormatWindow.FormatInfo;

			serializer = new XmlSerializer(typeof(FormaterSerialize));
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

	public void ProcessFormat(FormatInfo info)
	{
		try {
			if (!this.IsEditMode()) {
				FormatExcel.Format(info, mApplication, mEventLog);

				String connection = ConnectionUtility.GetDatabaseConnectionStr(mApplication.ActiveWorkbook);

				foreach (Excel.Name item in mApplication.ActiveWorkbook.Names) {
					if (item.Name.IndexOf(info.Destination + '_') == 0) {
						String column = item.Name.Substring((info.Destination + '_').Length);

						foreach (CustomMacro macro in mFormaterAssets.ColumnMacroManager) {
							if (macro.Contains(column)) {
								try {
									mApplication.Run(macro.Function,
										item.Name, connection, Type.Missing, Type.Missing, Type.Missing,
										Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
										Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
										Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
										Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
										Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing);
								}
								catch {
									// Ignore if unable to execute. 
								}
							}
						}
					}
				}

				foreach (CustomMacro macro in mFormaterAssets.TableMacroManager) {
					if (macro.Contains(info.Name)) {
						try {
							mApplication.Run(macro.Function,
								info.Destination, connection, Type.Missing, Type.Missing, Type.Missing,
								Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
								Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
								Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
								Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
								Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing);
						}
						catch {
							// Ignore if unable to execute. 
						}
					}
				}
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

	private bool IsDatalogDocument()
	{
		foreach (Excel.Worksheet worksheet in mApplication.ActiveWorkbook.Worksheets) {
			if (Array.Exists(gLogTables, worksheet.Name.Equals)) {
				return true;
			}
		}

		return false;
	}
	#endregion

	#region Members
	private Excel.Application mApplication;

	private FormaterSettings mFormaterSettings;
	private FormaterAssets mFormaterAssets;

	private NativeWindow mMainWindow;
	private Windows.FormatMainWindow mFormatWindow;
	private Windows.FormatManager mFormatManager;
	private Windows.FormatSettingsWindow mFormatSettings;

	private AddinUtility.EventLog mEventLog;
	#endregion
}
}
