/*****************************************************************************\
**
** ConnectionUtility
**
** Implementation of connection utility.
**
\*****************************************************************************/

namespace ExcelUtility.Utility {

/* Usings ********************************************************************/
	
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Excel = Microsoft.Office.Interop.Excel;

/* Classes *******************************************************************/

public class ConnectionUtility {

	public static ADODB.Connection OpenWorkbookConnection(Excel.Workbook workbook)
	{
		ADODB.Connection connection = new ADODB.Connection();
		ADODB.Recordset recordset = new ADODB.Recordset();
		String conectionString = AddinUtility.ConnectionUtility.GetConnectionString(workbook.FullName);

		try {
			connection.Open(conectionString, "", "", 0);
		}
		catch {
			connection = null;
		}
		
		return connection;
	}


	public static ADODB.Connection OpenDatabaseConnection(Excel.Workbook workbook)
	{
		ADODB.Connection connection = new ADODB.Connection();
		ADODB.Recordset recordset = new ADODB.Recordset();
		String conectionString = AddinUtility.ConnectionUtility.GetConnectionString(workbook.FullName);

		try {
			connection.Open(conectionString, "", "", 0);

			try {
				conectionString = null;
				recordset.Open("SELECT ConnectionString FROM ConnectionInfo;",
						connection,
						ADODB.CursorTypeEnum.adOpenKeyset,
						ADODB.LockTypeEnum.adLockOptimistic,
						-1);

				if (!recordset.EOF) {
					conectionString = recordset.Fields[0].Value.ToString();
				}

				recordset.Close();

				if (conectionString != null) {
					connection.Close();
					connection.Open(conectionString, "", "", 0);
				}
			}
			catch {
				recordset = null;
			}
		}
		catch {
			connection = null;
		}

		return connection;
	}

	public static String GetDatabaseConnectionStr(Excel.Workbook workbook)
	{
		ADODB.Connection connection = new ADODB.Connection();
		ADODB.Recordset recordset = new ADODB.Recordset();
		String conectionString = AddinUtility.ConnectionUtility.GetConnectionString(workbook.FullName);

		try {
			connection.Open(conectionString, "", "", 0);

			try {
				recordset.Open("SELECT ConnectionString FROM ConnectionInfo;",
						connection,
						ADODB.CursorTypeEnum.adOpenKeyset,
						ADODB.LockTypeEnum.adLockOptimistic,
						-1);

				if (!recordset.EOF) {
					conectionString = recordset.Fields[0].Value.ToString();
				}

				recordset.Close();
			}
			catch {
				recordset = null;
			}

			connection.Close();
		}
		catch {
			connection = null;
		}

		return conectionString;
	}

	public static bool IsLocalConnection(Excel.Workbook workbook, ADODB.Connection connection)
	{
		String localConnection = AddinUtility.ConnectionUtility.GetConnectionString(workbook.FullName);
		String currentConnection = connection.ConnectionString;

		int localDataSourceIndex = localConnection.IndexOf("Data Source=");
		int currentDataSourceIndex = currentConnection.IndexOf("Data Source=");
		int localDataSourceLength = localConnection.IndexOf(";", localDataSourceIndex) - localDataSourceIndex;
		int currentDataSourceLength = currentConnection.IndexOf(";", currentDataSourceIndex) - currentDataSourceIndex;

		localConnection = localConnection.Substring(localDataSourceIndex, localDataSourceLength);
		currentConnection = currentConnection.Substring(currentDataSourceIndex, currentDataSourceLength);
		
		return localConnection == currentConnection;
	}
}
}
