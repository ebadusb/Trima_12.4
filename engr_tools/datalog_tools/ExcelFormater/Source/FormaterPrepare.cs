/*****************************************************************************\
**
** FormatPrepare
**
** Utility class to prepare workbook for formating.
**
\*****************************************************************************/

namespace ExcelFormater {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using Excel = Microsoft.Office.Interop.Excel;
using ExcelUtility.Asset;
using ExcelUtility.Query;
using ExcelUtility.Utility;

/* Classes *******************************************************************/

public class FormaterPrepare {
	#region Constructor
	public FormaterPrepare(Excel.Workbook workbook)
	{
		mWorkbook = workbook;
	}
	#endregion
	
	#region Functions
	public static void UpdateAssetManagers(Excel.Workbook workbook, FormaterAssets assets)
	{
		FormaterPrepare preparer = new FormaterPrepare(workbook);

		preparer.UpdateAssetManagers(assets);
	}

	public void UpdateAssetManagers(FormaterAssets assets)
	{
		try {
			this.ConnectToDatabase();
			this.GetDatalogColumns(assets.KnownColumnAssetManager);
			this.GetDatalogColumns(assets.AvailableColumnAssetManager);
			this.GetDatalogLevels(assets.KnownLevelAssetManager);
			this.GetDatalogLevels(assets.AvailableLevelAssetManager);
			this.AddCriticalOutputLevel();
			this.DisconnectFromDatabase();
		}
		catch {
		}
	}

	private void ConnectToDatabase()
	{
		mConnection = ConnectionUtility.OpenDatabaseConnection(mWorkbook);
	}
	
	private void DisconnectFromDatabase()
	{
		mConnection.Close();
	}

	private void GetDatalogColumns(AssetManager<StringAsset> manager)
	{
		ADODB.Recordset recordset;

		recordset = new ADODB.Recordset();
		recordset.Open(
				"SELECT PeriodicSetItem.Name FROM PeriodicSetItem;",
				mConnection,
				ADODB.CursorTypeEnum.adOpenKeyset,
				ADODB.LockTypeEnum.adLockOptimistic,
				-1);

		while (!recordset.EOF) {
			manager.New(new StringAsset(recordset.Fields[0].Value.ToString()));
			recordset.MoveNext();
		}

		recordset.Close();

		manager.New(new StringAsset("RecordIndex"));
		manager.New(new StringAsset("RecordTime"));
		manager.New(new StringAsset("RecordClock"));
		manager.New(new StringAsset("RecordDate"));
		manager.New(new StringAsset("NodeID"));
		manager.New(new StringAsset("TaskID"));
		manager.New(new StringAsset("TaskName"));
		manager.New(new StringAsset("LevelName"));
		manager.New(new StringAsset("File"));
		manager.New(new StringAsset("Line"));
		manager.New(new StringAsset("Message"));
	}

	private void GetDatalogLevels(AssetManager<StringAsset> manager)
	{
		ADODB.Recordset recordset;

		recordset = new ADODB.Recordset();
		recordset.Open(
				"SELECT LogLevel.LevelName FROM LogLevel;",
				mConnection,
				ADODB.CursorTypeEnum.adOpenKeyset,
				ADODB.LockTypeEnum.adLockOptimistic,
				-1);

		while (!recordset.EOF) {
			manager.New(new StringAsset(recordset.Fields[0].Value.ToString()));
			recordset.MoveNext();
		}

		recordset.Close();

		manager.New(new StringAsset("critical_output"));
	}

	private void AddCriticalOutputLevel()
	{
		ADODB.Recordset levelRecordset = new ADODB.Recordset();
		String query = null;

		query = "SELECT * FROM LogLevel WHERE LevelName='critical_output';";
		levelRecordset.Open(query, 
				mConnection, 
				ADODB.CursorTypeEnum.adOpenKeyset,
				ADODB.LockTypeEnum.adLockOptimistic,
				-1);
		
		if (levelRecordset.EOF) {
			ADODB.Recordset nodeRecordset = new ADODB.Recordset();

			query = "SELECT NodeID FROM TaskCreated GROUP BY NodeID;";
			nodeRecordset.Open(query, 
					mConnection,
					ADODB.CursorTypeEnum.adOpenKeyset,
					ADODB.LockTypeEnum.adLockOptimistic,
					-1);

			while (!nodeRecordset.EOF) {
				Object recordsEffected = null;

				query = "INSERT INTO LogLevel(RecordIndex, TimeSeconds, TimeNanosec, LevelID, NodeID, LevelName) ";
				query += "VALUES (0, 0, 0, 0, " + nodeRecordset.Fields[0].Value.ToString() + ", 'critical_output');";

				mConnection.Execute(query, out recordsEffected, 0);
				nodeRecordset.MoveNext();
			}

			nodeRecordset.Close();
		}

		levelRecordset.Close();
	}
	#endregion

	#region Members
	private Excel.Workbook mWorkbook;
	private ADODB.Connection mConnection;
	#endregion
}
}
