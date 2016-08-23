/*****************************************************************************\
**
** ExcelFormater
**
** Implementation of excel layout formater.
**
\*****************************************************************************/

namespace ExcelUtility.Format {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using Excel = Microsoft.Office.Interop.Excel;
using Query;
using Utility;

/* Classes *******************************************************************/
	
public class FormatExcel {
	#region Functions
	public static void Format(FormatInfo info, Excel.Application app, AddinUtility.EventLog log)
	{
		log.Info("Beging formating datalog.");

		bool update = app.ScreenUpdating;

		app.ScreenUpdating = false;

		try {
			FormatUtil util = new FormatUtil(info, app, log);

			util.Format();
		}
		catch (Exception e) {
			log.Error("Could not fill worksheet: " + e.Message);
		}

		app.ScreenUpdating = update;

		log.Info("End fill worksheet.");
	}
	#endregion
	
	#region Utility Class
	private class FormatUtil {
		#region Constructors
		public FormatUtil(FormatInfo format, Excel.Application application, AddinUtility.EventLog log)
		{
			mFormat = format;
			mApplication = application;
			mWorkbook = application.ActiveWorkbook;
			mEventLog = log;
		}
		#endregion

		#region Functions
		public void Format()
		{
			mApplication.ScreenUpdating = false;
			mConnection = ConnectionUtility.OpenDatabaseConnection(mWorkbook);
			mIsLocalConnection = ConnectionUtility.IsLocalConnection(mWorkbook, mConnection);

			mEventLog.Info("Beging formating datalog.");
			mEventLog.Info(mConnection.ConnectionString);

			try {
				if (mConnection != null) {
					if (mIsLocalConnection) {
						mEventLog.Info("Datalog data is local.");
						this.PrepareInternalTempTables(mFormat);
					}
					else {
						mEventLog.Info("Datalog data is external.");
						this.PrepareExternalTempTables(mFormat);
					}
					
					QueryInfo info = new QueryInfo();

					mEventLog.Info("Creating main table.");
					info.Query = this.GetLogTableQuery(mFormat);
					info.TableName = mFormat.Destination;
					info.TableGeneration = true;

					QueryExcel.Query(info, mConnection, mWorkbook.Application, mEventLog);

					mConnection.Close();
				}
			}
			catch (Exception e) {
				mEventLog.Error("Could not format datalog: " + e.Message);
				mConnection.Close();
			}

			mEventLog.Info("End formating datalog.");

			mApplication.ScreenUpdating = true;
		}

		private void PrepareInternalTempTables(FormatInfo format)
		{
			QueryInfo info = new QueryInfo();

			mEventLog.Info("Creating temp table: qFloat");
			info.Query = this.GetFloatTableQuery(format);
			info.TableName = "qFloat";
			QueryExcel.Query(info, mConnection, mWorkbook.Application, mEventLog);

			if (this.WorksheetExist("qFloat")) {
				((Excel.Worksheet)mWorkbook.Worksheets["qFloat"]).Visible = Excel.XlSheetVisibility.xlSheetHidden;
			}

			mEventLog.Info("Creating temp table: qInt");
			info.Query = this.GetIntTableQuery(format);
			info.TableName = "qInt";
			QueryExcel.Query(info, mConnection, mWorkbook.Application, mEventLog);

			if (this.WorksheetExist("qInt")) {
				((Excel.Worksheet)mWorkbook.Worksheets["qInt"]).Visible = Excel.XlSheetVisibility.xlSheetHidden;
			}

			mEventLog.Info("Creating temp table: qString");
			info.Query = this.GetStringTableQuery(format);
			info.TableName = "qString";
			QueryExcel.Query(info, mConnection, mWorkbook.Application, mEventLog);

			if (this.WorksheetExist("qString")) {
				((Excel.Worksheet)mWorkbook.Worksheets["qString"]).Visible = Excel.XlSheetVisibility.xlSheetHidden;
			}

			mEventLog.Info("Creating temp table: qPeriodicData");
			info.Query = this.GetPeriodicTableQuery(format);
			info.TableName = "qPeriodicData";
			QueryExcel.Query(info, mConnection, mWorkbook.Application, mEventLog);

			if (this.WorksheetExist("qPeriodicData")) {
				((Excel.Worksheet)mWorkbook.Worksheets["qPeriodicData"]).Visible = Excel.XlSheetVisibility.xlSheetHidden;
			}

			mEventLog.Info("Creating temp table: qEpisodicData");
			info.Query = this.GetEpisodicTableQuery(format);
			info.TableName = "qEpisodicData";
			QueryExcel.Query(info, mConnection, mWorkbook.Application, mEventLog);
					
			if (this.WorksheetExist("qEpisodicData")) {
				((Excel.Worksheet)mWorkbook.Worksheets["qEpisodicData"]).Visible = Excel.XlSheetVisibility.xlSheetHidden;
			}

			// Is the following needed?
			mConnection.Close();
			mApplication.ScreenUpdating = true;
			mApplication.ScreenUpdating = false;
			mConnection = ConnectionUtility.OpenWorkbookConnection(mWorkbook);

			mEventLog.Info("Creating temp table: quTimes");
			info.Query = this.GetTimeTableQuery(format);
			info.TableName = "quTimes";
			QueryExcel.Query(info, mConnection, mWorkbook.Application, mEventLog);

			if (this.WorksheetExist("quTimes")) {
				((Excel.Worksheet)mWorkbook.Worksheets["quTimes"]).Visible = Excel.XlSheetVisibility.xlSheetHidden;
			}

			// Is the following needed?
			mConnection.Close();
			mApplication.ScreenUpdating = true;
			mApplication.ScreenUpdating = false;
			mConnection = ConnectionUtility.OpenWorkbookConnection(mWorkbook);
		}
					
		private void PrepareExternalTempTables(FormatInfo format)
		{
			const int kConnectionOptions = (int)ADODB.CommandTypeEnum.adCmdText
					| (int)ADODB.ExecuteOptionEnum.adExecuteNoRecords;

			String query = null; 
			Object records = null;
			
			try {
				mEventLog.Info("Creating temp table: qFloat");
				query = this.GetFloatTableQuery(format);
				records = null;
				
				if (this.TableExist("qFloat")) {
					mEventLog.Warning("Temp table exist: qFloat");
					mConnection.Execute("DROP PROCEDURE qFloat;", out records, kConnectionOptions);
				}

				if (String.IsNullOrEmpty(query)) {
					mEventLog.Warning("Empty table query: qFloat");
				}

				if (!String.IsNullOrEmpty(query)) {
					mEventLog.Info("Running table query: qFloat");
					mConnection.Execute("CREATE PROCEDURE qFloat AS " + query, out records, kConnectionOptions);
				}
			}
			catch (Exception e) {
				mEventLog.Error("Could not create table: " + e.Message);
			}

			try {
				mEventLog.Info("Creating temp table: qInt");
				query = this.GetIntTableQuery(format);
				records = null;

				if (this.TableExist("qInt")) {
					mEventLog.Warning("Temp table exist: qInt");
					mConnection.Execute("DROP PROCEDURE qInt;", out records, kConnectionOptions);
				}

				if (String.IsNullOrEmpty(query)) {
					mEventLog.Warning("Empty table query: qInt");
				}

				if (!String.IsNullOrEmpty(query)) {
					mEventLog.Info("Running table query: qInt");
					mConnection.Execute("CREATE PROCEDURE qInt AS " + query, out records, kConnectionOptions);
				}
			}
			catch (Exception e) {
				mEventLog.Error("Could not create table: " + e.Message);
			}

			try {
				mEventLog.Info("Creating temp table: qString");
				query = this.GetStringTableQuery(format);
				records = null;

				if (this.TableExist("qString")) {
					mEventLog.Warning("Temp table exist: qString");
					mConnection.Execute("DROP PROCEDURE qString;", out records, kConnectionOptions);
				}

				if (String.IsNullOrEmpty(query)) {
					mEventLog.Warning("Empty table query: qString");
				}

				if (!String.IsNullOrEmpty(query)) {
					mEventLog.Info("Running table query: qString");
					mConnection.Execute("CREATE PROCEDURE qString AS " + query, out records, kConnectionOptions);
				}
			}
			catch (Exception e) {
				mEventLog.Error("Could not create table: " + e.Message);
			}

			try {
				mEventLog.Info("Creating temp table: qPeriodicData");
				query = this.GetPeriodicTableQuery(format);
				records = null;

				if (this.TableExist("qPeriodicData")) {
					mEventLog.Warning("Temp table exist: qPeriodicData");
					mConnection.Execute("DROP PROCEDURE qPeriodicData;", out records, kConnectionOptions);
				}

				if (String.IsNullOrEmpty(query)) {
					mEventLog.Warning("Empty table query: qPeriodicData");
				}

				if (!String.IsNullOrEmpty(query)) {
					mEventLog.Info("Running table query: qPeriodicData");
					mConnection.Execute("CREATE PROCEDURE qPeriodicData AS " + query, out records, kConnectionOptions);
				}
			}
			catch (Exception e) {
				mEventLog.Error("Could not create table: " + e.Message);
			}

			try {
				mEventLog.Info("Creating temp table: qEpisodicData");
				query = this.GetEpisodicTableQuery(format);
				records = null;

				if (this.TableExist("qEpisodicData")) {
					mEventLog.Warning("Temp table exist: qEpisodicData");
					mConnection.Execute("DROP PROCEDURE qEpisodicData;", out records, kConnectionOptions);
				}

				if (String.IsNullOrEmpty(query)) {
					mEventLog.Warning("Empty table query: qEpisodicData");
				}

				if (!String.IsNullOrEmpty(query)) {
					mEventLog.Info("Running table query: qEpisodicData");
					mConnection.Execute("CREATE PROCEDURE qEpisodicData AS " + query, out records, kConnectionOptions);
				}
			}
			catch (Exception e) {
				mEventLog.Error("Could not create table: " + e.Message);
			}

			try {
				mEventLog.Info("Creating temp table: quTimes");
				query = this.GetTimeTableQuery(format);
				records = null;

				if (this.TableExist("quTimes")) {
					mEventLog.Warning("Temp table exist: quTimes");
					mConnection.Execute("DROP PROCEDURE quTimes;", out records, kConnectionOptions);
				}

				if (String.IsNullOrEmpty(query)) {
					mEventLog.Warning("Empty table query: quTimes");
				}

				if (!String.IsNullOrEmpty(query)) {
					mEventLog.Info("Running table query: quTimes");
					mConnection.Execute("CREATE PROCEDURE quTimes AS " + query, out records, kConnectionOptions);
				}
			}
			catch (Exception e) {
				mEventLog.Error("Could not create table: " + e.Message);
			}
		}

		private String GetTimeTableQuery(FormatInfo format)
		{
			String query = "";

			if (this.TableExist("qPeriodicData")) {
				query += "SELECT PDRecordIndex AS RecordIndex, PDRecordTime AS RecordTime FROM qPeriodicData ";
			}
				
			if (this.TableExist("qEpisodicData")) {
				if (!String.IsNullOrEmpty(query)) {
					query += "UNION ";
				}

				query += "SELECT EDRecordIndex AS RecordIndex, EDRecordTime AS RecordTime FROM qEpisodicData";
			}

			query += ";";

			mEventLog.Info(query);

			return query;
		}

		private String GetIntTableQuery(FormatInfo format)
		{
			String query = "";

			if (format.Columns.Count > 0) {
				query += "TRANSFORM Avg(PeriodicLogInt.Data) AS Data ";
				query += "SELECT First(PeriodicLogInt.RecordIndex) AS RecordIndex, CDbl(PeriodicLogInt.TimeSeconds + PeriodicLogInt.TimeNanoSec / 1000000000) AS RecordTime ";
				query += "FROM PeriodicSetItem INNER JOIN PeriodicLogInt ON (PeriodicSetItem.NodeID = PeriodicLogInt.NodeID) AND (PeriodicSetItem.KeyCode = PeriodicLogInt.KeyCode) ";

				query += "WHERE (";

				foreach (String item in format.Columns) {
					query += "(PeriodicSetItem.Name = '" + item + "') OR ";
				}

				query = query.Remove(query.Length - 4, 4);
				query += ") ";

				query += "GROUP BY CDbl(PeriodicLogInt.TimeSeconds + PeriodicLogInt.TimeNanoSec / 1000000000) ";
				query += "ORDER BY CDbl(PeriodicLogInt.TimeSeconds + PeriodicLogInt.TimeNanoSec / 1000000000) ";
				query += "PIVOT PeriodicSetItem.Name;";
			}

			mEventLog.Info(query);

			return query;
		}

		private String GetFloatTableQuery(FormatInfo format)
		{
			String query = "";

			if (format.Columns.Count > 0) {
				query += "TRANSFORM Avg(PeriodicLogFloat.Data) AS Data ";
				query += "SELECT First(PeriodicLogFloat.RecordIndex) AS RecordIndex, CDbl(PeriodicLogFloat.TimeSeconds + PeriodicLogFloat.TimeNanoSec / 1000000000) AS RecordTime ";
				query += "FROM PeriodicSetItem INNER JOIN PeriodicLogFloat ON (PeriodicSetItem.NodeID = PeriodicLogFloat.NodeID) AND (PeriodicSetItem.KeyCode = PeriodicLogFloat.KeyCode) ";

				query += "WHERE (";

				foreach (String item in format.Columns) {
					query += "(PeriodicSetItem.Name = '" + item + "') OR ";
				}

				query = query.Remove(query.Length - 4, 4);
				query += ") ";

				query += "GROUP BY CDbl(PeriodicLogFloat.TimeSeconds + PeriodicLogFloat.TimeNanoSec / 1000000000) ";
				query += "ORDER BY CDbl(PeriodicLogFloat.TimeSeconds + PeriodicLogFloat.TimeNanoSec / 1000000000) ";
				query += "PIVOT PeriodicSetItem.Name;";
			}

			mEventLog.Info(query);

			return query;
		}

		private String GetStringTableQuery(FormatInfo format)
		{
			String query = "";

			if (format.Columns.Count > 0) {
				query += "TRANSFORM Last(PeriodicLogString.Data) AS Data ";
				query += "SELECT First(PeriodicLogString.RecordIndex) AS RecordIndex, CDbl(PeriodicLogString.TimeSeconds + PeriodicLogString.TimeNanoSec / 1000000000) AS RecordTime ";
				query += "FROM PeriodicSetItem INNER JOIN PeriodicLogString ON (PeriodicSetItem.NodeID = PeriodicLogString.NodeID) AND (PeriodicSetItem.KeyCode = PeriodicLogString.KeyCode) ";

				query += "WHERE (";

				foreach (String item in format.Columns) {
					query += "(PeriodicSetItem.Name = '" + item + "') OR ";
				}

				query = query.Remove(query.Length - 4, 4);
				query += ") ";

				query += "GROUP BY CDbl(PeriodicLogString.TimeSeconds + PeriodicLogString.TimeNanoSec / 1000000000) ";
				query += "ORDER BY CDbl(PeriodicLogString.TimeSeconds + PeriodicLogString.TimeNanoSec / 1000000000) ";
				query += "PIVOT PeriodicSetItem.Name;";
			}

			mEventLog.Info(query);

			return query;
		}

		private String GetPeriodicTableQuery(FormatInfo format)
		{
			String query = "";

			if (this.TableExist("qFloat") || this.TableExist("qInt") || this.TableExist("qString")) {
				query += "SELECT quPeriodicTimes.RecordIndex AS PDRecordIndex, quPeriodicTimes.RecordTime AS PDRecordTime, ";

				if (this.TableExist("qFloat")) {
					query += "qFloat.*, ";
				}

				if (this.TableExist("qInt")) {
					query += "qInt.*, ";
				}
				
				if (this.TableExist("qString")) {
					query += "qString.*, ";
				}

				query = query.Remove(query.Length - 2, 1);

				query += "FROM ((((";
				query += "SELECT First(qTemp.RecordIndex) AS RecordIndex, qTemp.RecordTime AS RecordTime ";
				query += "FROM (";

				if (this.TableExist("qFloat")) {
					query += "SELECT Distinct qFloat.RecordIndex, qFloat.RecordTime FROM qFloat ";
				}

				if (this.TableExist("qInt")) {
					if (query[query.Length - 1] != '(') {
						query += "UNION ";
					}

					query += "SELECT Distinct qInt.RecordIndex, qInt.RecordTime FROM qInt ";
				}

				if (this.TableExist("qString")) {
					if (query[query.Length - 1] != '(') {
						query += "UNION ";
					}

					query += "SELECT Distinct qString.RecordIndex, qString.RecordTime FROM qString ";
				}

				query += ") AS qTemp ";
				query += "GROUP BY qTemp.RecordTime ";
				query += ") AS quPeriodicTimes ";

				if (this.TableExist("qFloat")) {
					query += "LEFT JOIN qFloat ON quPeriodicTimes.RecordTime = qFloat.RecordTime";
				}
				
				query += ") ";

				if (this.TableExist("qInt")) {
					query += "LEFT JOIN qInt ON quPeriodicTimes.RecordTime = qInt.RecordTime";
				}
				
				query += ") ";

				if (this.TableExist("qString")) {
					query += "LEFT JOIN qString ON quPeriodicTimes.RecordTime = qString.RecordTime";
				}
				
				query += ");";
			}

			mEventLog.Info(query);

			return query;
		}

		private String GetEpisodicTableQuery(FormatInfo format)
		{
			String query = "";

			if ((format.Levels.Count > 0 || format.AllLevels) && this.ContainsEpisodic(format)) {
				query += "SELECT TextOutput.RecordIndex AS EDRecordIndex, TextOutput.TimeSeconds + TextOutput.TimeNanosec / 1000000000 AS EDRecordTime, TextOutput.LevelID, TextOutput.TaskID, TextOutput.NodeID, LogLevel.LevelName, TaskCreated.TaskName, TextOutput.File, TextOutput.Line, TextOutput.Message ";
				query += "FROM TaskCreated INNER JOIN (LogLevel INNER JOIN TextOutput ON (LogLevel.LevelID = TextOutput.LevelID) AND (LogLevel.NodeID = TextOutput.NodeID)) ON (TaskCreated.TaskID = TextOutput.TaskID) ";

				if (!format.AllLevels) {
					query += "WHERE (";

					foreach (String item in format.Levels) {
						query += "(LogLevel.LevelName = '" + item + "') OR ";
					}

					query = query.Remove(query.Length - 4, 4);
					query += ") ";
				}
				
				query += "ORDER BY TextOutput.RecordIndex, TextOutput.TimeSeconds + TextOutput.TimeNanoSec / 1000000000; ";
			}

			mEventLog.Info(query);

			return query;
		}

		private String GetLogTableQuery(FormatInfo format)
		{
			String query = "";
			HashSet<String> columns = new HashSet<string>();

			columns.UnionWith(this.GetTableColumns("qPeriodicData"));
			columns.UnionWith(this.GetTableColumns("qEpisodicData"));

			query += "SELECT ";

			foreach (String item in format.Columns) {
				if (item == "RecordIndex") {
					query += "quTimes.RecordIndex AS RecordIndex, ";
				}
				else if (item == "RecordTime") {
					query += "quTimes.RecordTime / 86400 AS RecordTime, ";
				}
				else if (item == "RecordClock") {
					query += "quTimes.RecordTime AS RecordClock, ";
				}
				else if (item == "RecordDate") {
					query += "((quTimes.RecordTime / 86400) + " + this.GetNumberDays() + ") AS RecordDate, ";
				}
				else if (columns.Contains(item)) {
					query += item + ", ";
				}
			}
			
			query = query.Remove(query.Length - 2, 1);
			
			query += "FROM ((quTimes ";

			if (this.TableExist("qPeriodicData")) {
				query += "LEFT JOIN qPeriodicData ON (quTimes.RecordTime=qPeriodicData.PDRecordTime) AND (quTimes.RecordIndex=qPeriodicData.PDRecordIndex)";
			}
				
			query += ") ";

			if (this.TableExist("qEpisodicData")) {
				query += "LEFT JOIN qEpisodicData ON (quTimes.RecordTime=qEpisodicData.EDRecordTime) AND (quTimes.RecordIndex=qEpisodicData.EDRecordIndex)";
			}

			query += ") ";

			switch (format.Order) {
			case FormatInfo.Ordering.Time:
				query += "ORDER BY quTimes.RecordTime, quTimes.RecordIndex;";
				break;
			case FormatInfo.Ordering.Index:
				query += "ORDER BY quTimes.RecordIndex;";
				break;
			default:
				query += ";";
				break;
			}

			mEventLog.Info(query);

			return query;
		}

		private HashSet<String> GetTableColumns(String table)
		{
			HashSet<String> list = new HashSet<string>();

			if (this.TableExist(table)) {
				try {
					ADODB.Recordset recordset;

					recordset = new ADODB.Recordset();
					recordset.Open(
							"SELECT * FROM " + table + ";",
							mConnection,
							ADODB.CursorTypeEnum.adOpenKeyset,
							ADODB.LockTypeEnum.adLockOptimistic,
							-1);

					if ((recordset.State != 0) && !recordset.EOF) {
						foreach (ADODB.Field field in recordset.Fields) {
							list.Add(field.Name);
						}

						recordset.Close();
					}
				}
				catch (Exception e) {
					mEventLog.Error("Could not get column names: " + e.Message);
				}
			}

			return list;
		}

		private double GetNumberDays()
		{
			double totalNumbeDays = 0;

			int year = 0;
			int month = 0;
			int day = 0;
			int hour = 0;
			int min = 0;
			int sec = 0;
	
			try {
				ADODB.Recordset recordset;

				recordset = new ADODB.Recordset();
				recordset.Open(
						"SELECT * FROM LogHeader;",
						mConnection,
						ADODB.CursorTypeEnum.adOpenKeyset,
						ADODB.LockTypeEnum.adLockOptimistic,
						-1);

				if ((recordset.State != 0) && !recordset.EOF) {
					year = Int32.Parse(recordset.Fields["TimeYear"].Value.ToString());
					month = Int32.Parse(recordset.Fields["TimeMonth"].Value.ToString());
					day = Int32.Parse(recordset.Fields["TimeDay"].Value.ToString());
					hour = Int32.Parse(recordset.Fields["TimeHour"].Value.ToString());
					min = Int32.Parse(recordset.Fields["TimeMin"].Value.ToString());
					sec = Int32.Parse(recordset.Fields["TimeSec"].Value.ToString());
					mEventLog.Info("Log Date: " + year + ":" + month + ":" + day);
					mEventLog.Info("Log Time: " + hour + ":" + min + ":" + sec);
					recordset.Close();
				}
			}
			catch (Exception e) {
				mEventLog.Error("Could not get log date: " + e.Message);
			}

			try {
				DateTime baseTime = new DateTime(1900, 1, 1);
				mEventLog.Info("Base Time: " + baseTime.ToLongDateString());
				DateTime currentTime = new DateTime(year, month, day, hour, min, sec);
				mEventLog.Info("Current Time: " + currentTime.ToLongDateString());
				TimeSpan diff = currentTime - baseTime;
				diff = diff.Add(new TimeSpan(1, 0, 0, 0)); // For Excel 1900-Jan-0
				diff = diff.Add(new TimeSpan(1, 0, 0, 0)); // For Excel 1900-Feb-29
				mEventLog.Info("Difference Seconds: " + diff.TotalSeconds);
				totalNumbeDays = diff.TotalSeconds / 86400.0;
				mEventLog.Info("Difference Days: " + totalNumbeDays);
			}
			catch (Exception e) {
				mEventLog.Error("Could not get log date: " + e.Message);
			}

			return totalNumbeDays;
		}

		private bool TableExist(String name)
		{
			if (mIsLocalConnection) {
				try {
					foreach (Excel.Name item in mWorkbook.Names) {
						if (item.Name == name) {
							return true;
						}
					}
				}
				catch (Exception e) {
					mEventLog.Error("Could not get table names: " + e.Message);
				}
			}
			else {
				try {
					ADODB.Recordset recordset = mConnection.OpenSchema(
							ADODB.SchemaEnum.adSchemaProcedures,
							Type.Missing,
							Type.Missing);

					while (!recordset.EOF) {
						String table = recordset.Fields["PROCEDURE_NAME"].Value.ToString();

						if (table.Equals(name, StringComparison.InvariantCultureIgnoreCase)) {
							recordset.Close();
							return true;
						}

						recordset.MoveNext();
					}
					recordset.Close();
				}
				catch (Exception e) {
					mEventLog.Error("Could not get procedure names: " + e.Message);
				}

				try {
					ADODB.Recordset recordset = mConnection.OpenSchema(
							ADODB.SchemaEnum.adSchemaTables,
							Type.Missing,
							Type.Missing);

					while (!recordset.EOF) {
						String table = recordset.Fields["TABLE_NAME"].Value.ToString();

						if (table.Equals(name, StringComparison.InvariantCultureIgnoreCase)) {
							recordset.Close();
							return true;
						}

						recordset.MoveNext();
					}
					recordset.Close();
				}
				catch (Exception e) {
					mEventLog.Error("Could not get table names: " + e.Message);
				}
			}

			return false;
		}

		private bool WorksheetExist(String name)
		{
			try {
				foreach (Excel.Worksheet sheet in mWorkbook.Worksheets) {
					if (sheet.Name == name) {
						return true;
					}
				}
			}
			catch (Exception e) {
				mEventLog.Error("Could not get worksheet names: " + e.Message);
			}

			return false;
		}

		private bool ContainsEpisodic(FormatInfo format)
		{
			return format.Columns.Contains("NodeID")
				|| format.Columns.Contains("TaskID")
				|| format.Columns.Contains("TaskName")
				|| format.Columns.Contains("LevelName")
				|| format.Columns.Contains("File")
				|| format.Columns.Contains("Line")
				|| format.Columns.Contains("Message");
		}
		#endregion

		#region Members
		private FormatInfo mFormat;
		private Excel.Application mApplication;
		private Excel.Workbook mWorkbook;
		private ADODB.Connection mConnection;
		private bool mIsLocalConnection;
		private AddinUtility.EventLog mEventLog;
		#endregion
	}
	#endregion
}
}
