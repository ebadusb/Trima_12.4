/*****************************************************************************\
**
** DatalogImportMain
**
** Implementation of datalog importer.
**
\*****************************************************************************/

namespace AccessImporter {

/* Usings ********************************************************************/

using System;
using Extensibility;
using System.Runtime.InteropServices;
using Microsoft.Office.Core;
using System.Windows.Forms;
using Access = Microsoft.Office.Interop.Access;
using AddinUtility;

/* Classes *******************************************************************/

[GuidAttribute("01B156A3-AC9D-48DE-91F6-4288099CE3A1"), ProgId("AccessImporter.Connect")]
public class Connect : Object, IDTExtensibility2, IRibbonExtensibility {
	#region Constructor
	public Connect()
	{
		mOpenFileDialog = new OpenFileDialog();
	}
	#endregion

	#region Properties
	public static String Version
	{
		get { return "1.1"; }
	}
	#endregion

	#region IDTExtensibility2 Functions
	public void OnConnection(object application, Extensibility.ext_ConnectMode connectMode, object addInInst, ref System.Array custom)
	{
		mApplication = (Access.Application)application;
		mInstance = addInInst;
	}

	public void OnDisconnection(Extensibility.ext_DisconnectMode disconnectMode, ref System.Array custom)
	{
		mApplication = null;
		mInstance = null;
		mOpenFileDialog = null;
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

	#region Ribbon Functions
	public string GetCustomUI(string id)
	{
		return Properties.Resources.Ribbon;
	}
	#endregion

	#region Button Handler Functions
	/**
	 *
	 * ImportDatalog
	 *
	 * Handler that launches the Trima Configurator.
	 *
	 * @param control The ribbon control object.
	 *
	 */
	public void ImportDatalog(IRibbonControl control)
	{
		if (!CONVERT.DatabaseSecurity.IsProductActivated()) {
			System.Windows.Forms.MessageBox.Show(
					"The product is not activated.", "Error",
					System.Windows.Forms.MessageBoxButtons.OK,
					System.Windows.Forms.MessageBoxIcon.Error);
		}
		else if (mOpenFileDialog.ShowDialog() == DialogResult.OK) {
			String datalog = mOpenFileDialog.FileName;
			String database = mApplication.CurrentProject.FullName;

			try {
				String message = "";
				int results = CONVERT.DatalogConverter.ExtractLogData(datalog, database);

				mApplication.RefreshDatabaseWindow();

				switch (results) {
				case 1:
				case 3:
				case 4:
				case 5:
					message = "Failed to read the datalog file.";
					break;
				case 8:
					message = "Failed to connect to the database.";
					break;
				case 9:
					message = "Failed to disconnect from the database.";
					break;
				case 10:
					message = "A database query has failed.";
					break;
				}

				if (!String.IsNullOrEmpty(message)) {
					System.Windows.Forms.MessageBox.Show(
							message, "Error",
							System.Windows.Forms.MessageBoxButtons.OK,
							System.Windows.Forms.MessageBoxIcon.Error);
				}
			}
			catch (System.Exception e) {
				System.Windows.Forms.MessageBox.Show(
					e.Message, "Error",
					System.Windows.Forms.MessageBoxButtons.OK,
					System.Windows.Forms.MessageBoxIcon.Error);
			}
		}
	}
	#endregion

	#region Ribbon Callback Functions
	public void RibbonLoaded(IRibbonUI ribbon)
	{
		mRibbon = ribbon;
	}

	public stdole.IPictureDisp GetImage(string imageName)
	{
		System.Drawing.Bitmap image = null;

		if (imageName == "import-48x48.png") {
			image = Properties.Resources.import_48x48;
		}

		return image != null ? PictureConverter.ImageToPictureDisp(image) : null;
	}
	#endregion
	
	#region Members
	private object mInstance;
	private Access.Application mApplication;
	private IRibbonUI mRibbon;
	private OpenFileDialog mOpenFileDialog;
	#endregion
}
}