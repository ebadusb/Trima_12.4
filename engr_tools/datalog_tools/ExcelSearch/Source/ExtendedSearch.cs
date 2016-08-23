/*****************************************************************************\
**
** ExtendedSearch
**
** Implementation of the search engine.
**
\*****************************************************************************/

namespace ExcelSearch {

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
using ExcelUtility.Search;
using AddinUtility;

/* Classes *******************************************************************/

public class ExtendedSearch {
	#region Globals
	private static readonly ExtendedSearch gExtendedSearch = new ExtendedSearch();
	private static readonly String gConfigFile = "ExtendedSearch.config";
	private static readonly String gEventFile = "ExtendedSearch.event";

	public static ExtendedSearch GetInstance()
	{
		return gExtendedSearch;
	}
	#endregion

	#region Constructor
	private ExtendedSearch()
	{
		System.Windows.Forms.Application.EnableVisualStyles();

		mSearcherSettings = new SearcherSettings();
		mSearcherAssets = new SearcherAssets();
		mMainWindow = new NativeWindow();

		mSearchWindow = new Windows.SearchMainWindow();
		mSearchManager = new Windows.SearchManager();
		mSearchSettings = new Windows.SearchSettings();

		mSearchWindow.Assets = mSearcherAssets;
		mSearchManager.Assets = mSearcherAssets;
		mSearchSettings.Assets = mSearcherAssets;
		mSearchSettings.Settings = mSearcherSettings;

		mSearchWindow.Search += new Windows.SearchMainWindow.SearchInfoHandler(this.ProcessSearchHandler);
		mSearchManager.Search += new Windows.SearchManager.ProcessSearchHandler(this.ProcessBatchHandler);
	}
	#endregion
		
	#region Properties
	public String Version
	{
		get { return Properties.Resources.VersionNum; }
	}

	public SearcherAssets Assets
	{
		get { return mSearcherAssets; }
	}

	public SearcherSettings Settings
	{
		get { return mSearcherSettings; }
	}
	#endregion

	#region Handlers
	private void ProcessSearchHandler(object sender, ProcessAssetArgs<SearchInfo> args)
	{
		this.ProcessSearch(args.Item);
	}

	private void ProcessBatchHandler(object sender, ProcessAssetArgs<AssetBatch<SearchInfo>> args)
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

	public void ShowSearchWindow()
	{
		try {
			mSearchWindow.ShowDialog(mMainWindow);
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ShowManagerWindow()
	{
		try {
			mSearchManager.ShowDialog(mMainWindow);
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ShowSettingsWindow()
	{
		try {
			mSearchSettings.ShowDialog(mMainWindow);
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
			SearcherSerialize data = null;

			try {
				serializer = new XmlSerializer(typeof(SearcherSerialize));
				reader = new XmlTextReader(file);
				data = (SearcherSerialize)serializer.Deserialize(reader);

				mSearcherSettings = data.SearcherSettings;

				foreach (SearchInfo item in data.SearchInfoList) {
					mSearcherAssets.SearchInfoManager.New(item);
				}

				foreach (SearchInfo item in data.RecentInfoList) {
					mSearcherAssets.RecentInfoManager.New(item);
				}

				foreach (AssetBatch<SearchInfo> item in data.BatchInfoList) {
					mSearcherAssets.BatchInfoManager.New(item);
				}

				mSearchWindow.SearchInfo = data.CurrentSearchInfo;
				mSearcherAssets.RecentInfoManager.Max = mSearcherSettings.HistoryCountMax;

				mSearchWindow.Assets = mSearcherAssets;
				mSearchManager.Assets = mSearcherAssets;
				mSearchSettings.Assets = mSearcherAssets;
				mSearchSettings.Settings = mSearcherSettings;
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
		SearcherSerialize data = new SearcherSerialize();

		try {
			data.SearcherSettings = mSearcherSettings;

			foreach (SearchInfo item in mSearcherAssets.SearchInfoManager) {
				data.SearchInfoList.Add(item);
			}

			foreach (SearchInfo item in mSearcherAssets.RecentInfoManager) {
				data.RecentInfoList.Add(item);
			}

			foreach (AssetBatch<SearchInfo> item in mSearcherAssets.BatchInfoManager) {
				data.BatchInfoList.Add(item);
			}

			data.CurrentSearchInfo = mSearchWindow.SearchInfo;

			serializer = new XmlSerializer(typeof(SearcherSerialize));
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

	public void ProcessSearch(SearchInfo item)
	{
		try {
			if (!this.IsEditMode()) {
				SearchInfo info = item.Clone();

				if (!mSearcherAssets.SearchInfoManager.Exists(info.Equals)) {
					if (!mSearcherAssets.RecentInfoManager.Exists(info)) {
						mSearcherAssets.RecentInfoManager.New(info);
					}
					else {
						mSearcherAssets.RecentInfoManager.Edit(info, info);
					}
				}

				if (String.IsNullOrEmpty(info.ResultSheet)) {
					info.ResultSheet = mSearcherSettings.DefaultSheetName;
				}

				SearchExcel.Search(info, mApplication, mEventLog);

				if (mSearcherSettings.SwitchSheet) {
					Excel.Workbook book = mApplication.ActiveWorkbook;
					Excel.Worksheet sheet = (Excel.Worksheet)book.Worksheets[info.ResultSheet];

					sheet.Select(Type.Missing);
				}
			}
		}
		catch (Exception e) {
			mEventLog.Error(e.Message);
		}
	}

	public void ProcessBatch(AssetBatch<SearchInfo> item)
	{
		try {
			if (!this.IsEditMode()) {
				foreach (String name in item.Items) {
					ItemNameCompare<SearchInfo> compare = new ItemNameCompare<SearchInfo>(name);

					if (mSearcherAssets.SearchInfoManager.Exists(compare.Equals)) {
						SearchInfo search = mSearcherAssets.SearchInfoManager.Find(compare.Equals);

						if (String.IsNullOrEmpty(search.ResultSheet)) {
							search.ResultSheet = mSearcherSettings.DefaultSheetName;
						}

						SearchExcel.Search(search, mApplication, mEventLog);
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

	private SearcherSettings mSearcherSettings;
	private SearcherAssets mSearcherAssets;

	private NativeWindow mMainWindow;
	private Windows.SearchMainWindow mSearchWindow;
	private Windows.SearchManager mSearchManager;
	private Windows.SearchSettings mSearchSettings;

	private EventLog mEventLog;
	#endregion
}
}


