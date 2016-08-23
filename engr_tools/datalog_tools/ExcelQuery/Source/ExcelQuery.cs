/*****************************************************************************\
**
** ExcelQuery
**
** Implementation of the query engine.
**
\*****************************************************************************/

namespace ExcelQuery {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using Microsoft.Office.Core;
using Excel = Microsoft.Office.Interop.Excel;
using System.Xml;
using System.Xml.Serialization;
using ExcelUtility.Asset;
using ExcelUtility.Query;
using AddinUtility;

/* Classes *******************************************************************/
	
public class ExcelQuery {
	#region Globals
	private static readonly ExcelQuery gExcelQuery = new ExcelQuery();
	private static readonly String gConfigFile = "ExcelQuery.config";
	private static readonly String gEventFile = "ExcelQuery.event";
	private static readonly String gVersionNum = "1.1";

	public static ExcelQuery GetInstance()
	{
		return gExcelQuery;
	}
	#endregion

	#region Constructor
	private ExcelQuery()
	{
		System.Windows.Forms.Application.EnableVisualStyles();

		mQuerySettings = new QuerySettings();
		mQueryAssets = new QueryAssets();
		mMainWindow = new NativeWindow();

		mQueryWindow = new Windows.QueryMainWindow();
		mManagerWindow = new Windows.QueryManager();
		mSettingsWindow = new Windows.QuerySettingsWindow();

		mQueryWindow.Assets = mQueryAssets;
		mManagerWindow.Assets = mQueryAssets;
		mSettingsWindow.Assets = mQueryAssets;
		mSettingsWindow.Settings = mQuerySettings;

		mQueryWindow.Query += new Windows.QueryMainWindow.ProcessQueryHandler(this.ProcessQueryHandler);
		mManagerWindow.Query += new Windows.QueryManager.ProcessQueryHandler(this.ProcessQueryHandler);
	}
	#endregion

	#region Properties
	public String Version
	{
		get { return gVersionNum; }
	}

	public QueryAssets Assets
	{
		get { return mQueryAssets; }
	}

	public QuerySettings Settings
	{
		get { return mQuerySettings; }
	}
	#endregion

	#region Handlers
	private void ProcessQueryHandler(object sender, ProcessAssetArgs<QueryInfo> args)
	{
		this.ProcessQuery(args.Item);
	}

	private void ProcessQueryHandler(object sender, ProcessAssetArgs<AssetBatch<QueryInfo>> args)
	{
		this.ProcessBatch(args.Item);
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

	public void ShowQueryWindow()
	{
		try {
			mQueryWindow.ShowDialog(mMainWindow);
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ShowManagerWindow()
	{
		try {
			mManagerWindow.ShowDialog(mMainWindow);
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ShowSettingsWindow()
	{
		try {
			mSettingsWindow.ShowDialog(mMainWindow);
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
			QuerySerialize data = null;

			try {
				serializer = new XmlSerializer(typeof(QuerySerialize));
				reader = new XmlTextReader(file);
				data = (QuerySerialize)serializer.Deserialize(reader);

				mQuerySettings = data.QuerySettings;

				foreach (QueryInfo item in data.QueryAssetList) {
					mQueryAssets.QueryInfoManager.New(item);
				}

				foreach (AssetBatch<QueryInfo> item in data.BatchAssetList) {
					mQueryAssets.BatchInfoManager.New(item);
				}

				mQueryWindow.QueryInfo = data.CurrentQueryInfo;

				mQueryWindow.Assets = mQueryAssets;
				mManagerWindow.Assets = mQueryAssets;
				mSettingsWindow.Assets = mQueryAssets;
				mSettingsWindow.Settings = mQuerySettings;
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
		QuerySerialize data = new QuerySerialize();

		try {
			data.QuerySettings = mQuerySettings;

			foreach (QueryInfo item in mQueryAssets.QueryInfoManager) {
				data.QueryAssetList.Add(item);
			}

			foreach (AssetBatch<QueryInfo> item in mQueryAssets.BatchInfoManager) {
				data.BatchAssetList.Add(item);
			}

			data.CurrentQueryInfo = mQueryWindow.QueryInfo;

			serializer = new XmlSerializer(typeof(QuerySerialize));
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

	public void ProcessQuery(QueryInfo item)
	{
		try {
			if (!this.IsEditMode()) {
				QueryInfo query = item.Clone();

				if (String.IsNullOrEmpty(query.TableName)) {
					query.TableName = mQuerySettings.DefaultSheetName;
				}

				QueryExcel.Query(query, mApplication, mEventLog);

				if (mQuerySettings.SwitchSheet) {
					Excel.Workbook book = mApplication.ActiveWorkbook;
					Excel.Worksheet sheet = (Excel.Worksheet)book.Worksheets[query.TableName];

					sheet.Select(Type.Missing);
				}
			}
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ProcessBatch(AssetBatch<QueryInfo> item)
	{
		try {
			if (!this.IsEditMode()) {
				foreach (String name in item.Items) {
					ItemNameCompare<QueryInfo> compare = new ItemNameCompare<QueryInfo>(name);

					if (mQueryAssets.QueryInfoManager.Exists(compare.Equals)) {
						QueryInfo query = mQueryAssets.QueryInfoManager.Find(compare.Equals);

						if (String.IsNullOrEmpty(query.TableName)) {
							query.TableName = mQuerySettings.DefaultSheetName;
						}

						QueryExcel.Query(query, mApplication, mEventLog);
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
	#endregion

	#region Members
	private Excel.Application mApplication;

	private QuerySettings mQuerySettings;
	private QueryAssets mQueryAssets;

	private NativeWindow mMainWindow;
	private Windows.QueryMainWindow mQueryWindow;
	private Windows.QueryManager mManagerWindow;
	private Windows.QuerySettingsWindow mSettingsWindow;

	private AddinUtility.EventLog mEventLog;
	#endregion
}
}
