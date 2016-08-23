/*****************************************************************************\
**
** QueryExcel
**
** Implementation of the excel query.
**
\*****************************************************************************/

namespace ExcelUtility.Query {

/* Usings ********************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using Asset;
using Utility;
using Excel = Microsoft.Office.Interop.Excel;
	
/* Classes *******************************************************************/

public class QueryExcel {
	#region Functions
	public static void Query(QueryInfo info, Excel.Application app, AddinUtility.EventLog log)
	{
		QueryExcel.Query(info, null, app, log);
	}

	public static void Query(QueryInfo info, ADODB.Connection con, Excel.Application app, AddinUtility.EventLog log)
	{
		log.Info("Beging query datsabase.");

		bool update = app.ScreenUpdating;

		app.ScreenUpdating = false;

		try {
			QueryUtil util = new QueryUtil(app, con, info, log);

			util.Query();
		}
		catch (Exception e) {
			log.Error("Could not query datsabase: " + e.Message);
		}

		app.ScreenUpdating = update;

		log.Info("End query datsabase.");
	}
	#endregion

	#region Utility Class
	private class QueryUtil {
		#region Constructor
		public QueryUtil(Excel.Application app, ADODB.Connection con, QueryInfo info, AddinUtility.EventLog log)
		{
			mApplication = app;
			mConnection = con;
			mInfo = info;
			mWorkbook = app.ActiveWorkbook;
			mWorksheet = (Excel.Worksheet)mWorkbook.ActiveSheet;
			mFilename = mWorkbook.FullName;
			mOwnsConnection = mConnection == null;
			mEventLog = log;
		}
		#endregion

		#region Perform Query Function
		public void Query()
		{
			if (System.IO.File.Exists(mFilename)) {
				try {
					this.OpenConnection();
					this.OpenRecordset();
					this.PreProcessResults();
					this.ProcessRecordset();
					this.PostProcessResults();
					this.CloseRecordset();
					this.CloseConnection();
				}
				catch (Exception ex) {
					mEventLog.Error(ex.Message);
				}
			}
		}

		public void OpenConnection()
		{
			try {
				if (mOwnsConnection && !String.IsNullOrEmpty(mInfo.Query)) {
					mConnection = ConnectionUtility.OpenDatabaseConnection(mWorkbook);
				}
			}
			catch (Exception ex) {
				mEventLog.Error("Could not open connection: " + ex.Message);
			}
		}

		public void CloseConnection()
		{
			try {
				if (mOwnsConnection && mConnection != null) {
					mConnection.Close();
					mConnection = null;
				}
			}
			catch (Exception ex) {
				mEventLog.Error("Could not close connection: " + ex.Message);
			}
		}

		public void OpenRecordset()
		{
			try {
				if (mConnection != null) {
					mRecordset = new ADODB.Recordset();
					mRecordset.Open(mInfo.Query, mConnection,
							ADODB.CursorTypeEnum.adOpenKeyset,
							ADODB.LockTypeEnum.adLockOptimistic, -1);
				}
			}
			catch (Exception ex) {
				mEventLog.Error("Could not open record set: " + ex.Message);
			}
		}

		public void CloseRecordset()
		{
			try {
				if (mRecordset != null) {
					mRecordset.Close();
					mRecordset = null;
				}
			}
			catch (Exception ex) {
				mEventLog.Error("Could not close record set: " + ex.Message);
			}
		}

		public void PreProcessResults()
		{
			try {
				// Find the result worksheet
				foreach (object obj in mWorkbook.Worksheets) {
					Excel.Worksheet tempsheet = (Excel.Worksheet)obj;

					if (tempsheet.Name == mInfo.TableName) {
						mEventLog.Info("Found result worksheet: " + mInfo.TableName);
						mApplication.DisplayAlerts = false;
						mResultsheet = (Excel.Worksheet)mWorkbook.Worksheets.Add(
								Type.Missing, tempsheet, 1, Excel.XlSheetType.xlWorksheet);
						tempsheet.Delete();
						mResultsheet.Name = mInfo.TableName;
						mApplication.DisplayAlerts = true;
						break;
					}
				}

				// Create the result worksheet 
				if (mResultsheet == null) {
					mEventLog.Info("Creating result worksheet: " + mInfo.TableName);
					mResultsheet = (Excel.Worksheet)mWorkbook.Worksheets.Add(
							Type.Missing, Type.Missing, 1, Excel.XlSheetType.xlWorksheet);
					mResultsheet.Name = mInfo.TableName;
				}
			}
			catch (Exception ex) {
				mEventLog.Error("Could not prepare results worksheet: " + ex.Message);
			}
		}

		public void ProcessRecordset()
		{
			try {
				if ((mRecordset != null)
						&& (mRecordset.State != (int)ADODB.ObjectStateEnum.adStateClosed)
						&& (mRecordset.RecordCount > 0)
						&& (mResultsheet != null)) {
					Excel.Range cell = null;
					Excel.Range range = null;

					// Copy column headers
					for (int i = 0; i < mRecordset.Fields.Count; ++i) {
						cell = (Excel.Range)mResultsheet.Cells[1, i + 1];
						cell.Value2 = mRecordset.Fields[i].Name;
					}

					// Copy content of search
					cell = (Excel.Range)mResultsheet.Cells[2, 1];
					cell.CopyFromRecordset(mRecordset, Type.Missing, Type.Missing);

					// Name the range
					range = mResultsheet.UsedRange;
					range.Name = mInfo.TableName;

					if (mInfo.RangeGeneration) {
						int maxRows = mRecordset.RecordCount + 1;
						char columnIndex = 'A';

						for (int i = 0; i < mRecordset.Fields.Count; ++i, ++columnIndex) {
							range = mResultsheet.get_Range(
								columnIndex.ToString() + "2",
								columnIndex.ToString() + maxRows.ToString());
							range.Name = mInfo.TableName + "_" + mRecordset.Fields[i].Name;
						}
					}
				}
			}
			catch (Exception ex) {
				mEventLog.Error("Could not process results: " + ex.Message);
			}
		}

		public void PostProcessResults()
		{
			try {
				if ((mRecordset != null)
						&& (mRecordset.State != (int)ADODB.ObjectStateEnum.adStateClosed)
						&& (mRecordset.RecordCount > 0)
						&& (mResultsheet != null)) {
					// Create the table
					if (mInfo.TableGeneration) {
						Excel.ListObjects lists = mResultsheet.ListObjects;
						Excel.ListObject table = lists.Add(
								Excel.XlListObjectSourceType.xlSrcRange,
								mResultsheet.UsedRange, 
								Type.Missing, 
								Excel.XlYesNoGuess.xlYes, 
								Type.Missing);
						table.TableStyle = null;
						table.Name = "Table" + mInfo.TableName;
					}
				}
			}
			catch (Exception ex) {
				mEventLog.Error("Could not finalyse results worksheet: " + ex.Message);
			}
		}
		#endregion

		private ADODB.Connection mConnection;
		private ADODB.Recordset mRecordset;

		private Excel.Application mApplication;
		private Excel.Workbook mWorkbook;
		private Excel.Worksheet mWorksheet;
		private Excel.Worksheet mResultsheet;

		private QueryInfo mInfo;
		private String mFilename;
		private bool mOwnsConnection;
		private AddinUtility.EventLog mEventLog;
	}
	#endregion
}
}
