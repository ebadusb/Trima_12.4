/*****************************************************************************\
**
** FillExcel
**
** Implementation of excel fill.
**
\*****************************************************************************/

namespace ExcelUtility.Fill {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using Microsoft.Office.Interop.Excel;
using ExcelUtility.Asset;
using AddinUtility;

/* Classes *******************************************************************/

public class FillExcel {
	#region Functions
	public static void Fill(FillInfo info, Application app, EventLog log)
	{
		log.Info("Beging fill worksheet.");

		bool update = app.ScreenUpdating;

		app.ScreenUpdating = false;

		try {
			FillUtil util = new FillUtil(app, info, log);

			util.Fill();
		}
		catch (Exception e) {
			log.Error("Could not fill worksheet: " + e.Message);
		}

		app.ScreenUpdating = update;

		log.Info("End fill worksheet.");
	}
	#endregion

	#region Utility Class
	private class FillUtil {
		#region Constructor
		public FillUtil(Application app, FillInfo info, AddinUtility.EventLog log)
		{
			mApp = app;
			mInfo = info;
			mEventLog = log;
			mWorkbook = mApp.ActiveWorkbook;
			mWorksheet = (Worksheet)mApp.ActiveSheet;
			mSelection = mApp.Intersect((Range)mApp.Selection, mWorksheet.UsedRange, Type.Missing, Type.Missing, Type.Missing,
							Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
							Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
							Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
							Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
							Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing);
		}
		#endregion

		#region Functions
		public void Fill()
		{
			if (mSelection != null) {
				foreach (Range column in mSelection.Columns) {
					Range area = mWorksheet.get_Range(
							column.EntireColumn.Rows[1, Type.Missing], 
							column.Rows[1, Type.Missing]);
					Range found = area.Find(
							"*",
							Type.Missing,
							XlFindLookIn.xlValues,
							XlLookAt.xlPart,
							XlSearchOrder.xlByRows,
							XlSearchDirection.xlPrevious,
							false,
							Type.Missing,
							Type.Missing);
					String address = found.get_Address(
							true, true,
							XlReferenceStyle.xlA1,
							Type.Missing,
							Type.Missing);
					String text = found.Value2.ToString();

					if (found != null) {
						foreach (Range cell in column.Cells) {
							if (cell.Value2 == null) {
								cell.Value2 = found.Value2;

								if (mInfo.Highlight) {
									cell.Interior.Color = ColorTranslator.ToWin32(mInfo.Color);
								}

								if (mInfo.Hyperlink) {
									mWorksheet.Hyperlinks.Add(cell, "", address, text, text);
								}
							}
							else {
								found = cell;
								address = cell.get_Address(
										true, true,
										XlReferenceStyle.xlA1,
										Type.Missing,
										Type.Missing);
								text = found.Value2.ToString();
							}
						}
					}
				}
			}
		}
		#endregion

		private Application mApp;
		private FillInfo mInfo;
		private Workbook mWorkbook;
		private Worksheet mWorksheet;
		private Range mSelection;
		private EventLog mEventLog;
	}
	#endregion
}
}
