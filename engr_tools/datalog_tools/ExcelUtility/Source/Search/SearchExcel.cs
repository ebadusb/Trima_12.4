/*****************************************************************************\
**
** ExcelSearch
**
** Implementation of the excel search.
**
\*****************************************************************************/

namespace ExcelUtility.Search {

/* Usings ********************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.Office.Interop.Excel;
using Asset;
using AddinUtility;

/* Classes *******************************************************************/

public class SearchExcel {
	#region Functions
	public static void Search(SearchInfo info, Application app, EventLog log)
	{
		log.Info("Beging search worksheet.");

		bool update = app.ScreenUpdating;

		app.ScreenUpdating = false;

		try {
			SearchUtil util = new SearchUtil(app, info, log);

			util.Search();
		}
		catch (Exception e) {
			log.Error("Could not search worksheet: " + e.Message);
		}

		app.ScreenUpdating = update;

		log.Info("End search worksheet.");
	}
	#endregion

	#region Utility Class
	private class SearchUtil {
		#region Constructor
		public SearchUtil(Application app, SearchInfo info, EventLog log)
		{
			mApp = app;
			mInfo = info;
			mEventLog = log;
			mWorkbook = mApp.ActiveWorkbook;
			mWorksheet = (Worksheet)mApp.ActiveSheet;
			mMatchedCells = new Dictionary<Range, Range>();

			mEventLog.Info("Setting up search.");
			
			if (mWorksheet != null) {
				this.PrepareSearchRange();
				this.PrepareResultWorksheet();
				mWorksheet.Activate();
			}
			else {
				mEventLog.Info("No active worksheet.");
			}
		}
		#endregion

		#region Prepare Search Functions
		private void PrepareSearchRange()
		{
			mEventLog.Info("Preparing search ranges.");

			mSearchRange = mWorksheet.UsedRange;
			mInsertRowNumber = mInfo.InsertRowNum;
			mSortByRowNumber = mInfo.InsertRowNum && mInfo.SortByRowNum;
			mLinkRangeRows = mInfo.LinkRangeRows;
			
			try {
				Range selection = (Range)mApp.Selection;

				mEventLog.Info("Processing user selection:");

				if ((selection != null) && (selection.Count > 1)) {
					mEventLog.Info("Intersecting selection with used range.");
					mSearchRange = mApp.Intersect(mSearchRange, selection, Type.Missing, Type.Missing, Type.Missing,
							Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
							Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
							Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
							Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
							Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing);
				}
				else {
					mEventLog.Info("Selection is empty.");
				}
			}
			catch (Exception e) {
				mEventLog.Error("Unable to process selected range: " + e.Message);
			}

			if (!String.IsNullOrEmpty(mInfo.SearchArea)) {
				try {
					Range selection  = mWorksheet.get_Range(mInfo.SearchArea, Type.Missing);

					mEventLog.Info("Processing search selection:");

					if (selection != null) {
						mEventLog.Info("Intersecting selection with used range.");
						mSearchRange = mApp.Intersect(mSearchRange, selection, Type.Missing, Type.Missing, Type.Missing,
								Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
								Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
								Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
								Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,
								Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing);
					}
					else {
						mEventLog.Info("Selection is empty.");
					}
				}
				catch (Exception e) {
					mEventLog.Error("Unable to process selected range: " + e.Message);
				}
			}

			if (mLinkRangeRows && mSearchRange != null) {
				int rowIndex = 0;

				mEventLog.Info("Validating search ranges.");

				if (mSearchRange.Areas.Count > 0) {
					mSearchRowCount = mSearchRange.Areas[1].Rows.Count;
					rowIndex = mSearchRange.Areas[1].Row;
				}

				for (int i = 2; i <= mSearchRange.Areas.Count; ++i) {
					if (mSearchRange.Areas[i].Rows.Count != mSearchRowCount) {
						mEventLog.Warning("The specified ranges must have equal number of rows.");
						mLinkRangeRows = false;
					}

					if (mLinkRangeRows && mSearchRange.Areas[i].Row != rowIndex) {
						mEventLog.Warning("The specified ranges must have same row index.");
						mInsertRowNumber = false;
						mSortByRowNumber = false;
					}
				}
			}
		}

		private void PrepareResultWorksheet()
		{
			String destination = String.IsNullOrEmpty(mInfo.ResultSheet) 
					? "Search Results" : mInfo.ResultSheet;

			mInsertRowIndex = 1;

			mEventLog.Info("Preparing result worksheet.");

			// Find the result worksheet
			foreach (object obj in mWorkbook.Worksheets) {
				Worksheet tempsheet = (Worksheet)obj;

				if (tempsheet.Name == destination) {
					mEventLog.Info("Found result worksheet.");
					mResultsheet = tempsheet;
					mInsertRowIndex = mResultsheet.UsedRange.Rows.Count + 1;
					break;
				}
			}

			// Create the result worksheet 
			if (mResultsheet == null) {
				mEventLog.Info("Creating result worksheet.");
				mResultsheet = (Worksheet)mWorkbook.Worksheets.Add(
						Type.Missing, mWorksheet, 1, XlSheetType.xlWorksheet);
				mResultsheet.Name = destination;
			}

			// Clear the result worksheet
			if (mInfo.ClearPrevious) {
				mEventLog.Info("Clearing result worksheet.");
				mResultsheet.UsedRange.Clear();
				mInsertRowIndex = 1;
			}
		}
		#endregion

		#region Perform Search Functions
		public void Search()
		{
			if (mSearchRange != null) {
				this.SearchRanges();
				this.HighLightCells();
				this.HyperlinkCells();
				this.SortResultRows();
				this.ResizeColumns();
			}
		}

		private void SearchRanges()
		{
			if (mInfo.IsExpressions && mLinkRangeRows) {
				this.SearchExpressionLinkedRanges();
			}
			else if (mInfo.IsExpressions && !mLinkRangeRows) {
				this.SearchExpressionUnlinkedRanges();
			}
			else if (!mInfo.IsExpressions && mLinkRangeRows) {
				this.SearchWordLinkedRanges();
			}
			else if (!mInfo.IsExpressions && !mLinkRangeRows) {
				this.SearchWordUnlinkedRanges();
			}
		}

		private void SearchExpressionLinkedRanges()
		{
			mEventLog.Info("Searching expression in linked ranges.");

			// Process each row 
			for (int i = 1; i <= mSearchRowCount; ++i) {
				List<Range> cells = this.GetCellsFromRange(mSearchRange.Areas, i);
				List<Range> match = new List<Range>();

				if (this.CellListContainsMatch(cells, match)) {
					this.ProcessMatchedCellsByReference(cells, match);
					++mInsertRowIndex;
				}
			}
		}

		private void SearchExpressionUnlinkedRanges()
		{
			mEventLog.Info("Searching expression in unlinked ranges.");

			// Process all the ranges
			for (int i = 1; i <= mSearchRange.Areas.Count; ++i) {
				Range area = mSearchRange.Areas[i];

				// Process each row in range
				foreach (object rowObj in area.Rows) {
					List<Range> cells = this.GetCellsFromRow((Range)rowObj);
					List<Range> match = new List<Range>();

					if (this.CellListContainsMatch(cells, match)) {
						this.ProcessMatchedCellsByReference(cells, match);
						++mInsertRowIndex;
					}
				}
			}
		}

		private void SearchWordLinkedRanges()
		{
			mEventLog.Info("Searching word in linked ranges.");

			Dictionary<int, List<Range>> match = new Dictionary<int, List<Range>>();
			Range found = mSearchRange.Find(
					mInfo.SearchString,
					Type.Missing,
					XlFindLookIn.xlValues,
					mInfo.MatchWholeWord ? XlLookAt.xlWhole : XlLookAt.xlPart,
					XlSearchOrder.xlByRows,
					XlSearchDirection.xlNext,
					mInfo.MatchLetterCase,
					Type.Missing,
					Type.Missing);

			if (found != null) {
				String first = found.get_Address(
						true, true, 
						XlReferenceStyle.xlA1, 
						Type.Missing, 
						Type.Missing);
				String address = first;

				do {
					int index = found.Row - mSearchRange.Row + 1;
					List<Range> list = null;

					if (!match.TryGetValue(index, out list)) {
						list = new List<Range>();
						match.Add(index, list);
					}

					list.Add(found);
					found = mSearchRange.FindNext(found);
					address = found.get_Address(
							true, true, 
							XlReferenceStyle.xlA1, 
							Type.Missing, Type.Missing);
				} while (!address.Equals(first));
			}

			foreach (KeyValuePair<int, List<Range>> pair in match) {
				List<Range> cells = this.GetCellsFromRange(mSearchRange.Areas, pair.Key);

				this.ProcessMatchedCellsByAddress(cells, pair.Value);
				++mInsertRowIndex;
			}
		}

		private void SearchWordUnlinkedRanges()
		{
			mEventLog.Info("Searching word in unlinked ranges.");

			// Process all the ranges
			for (int i = 1; i <= mSearchRange.Areas.Count; ++i) {
				Range area = mSearchRange.Areas[i];
				Range found = area.Find(
						mInfo.SearchString, 
						Type.Missing, 
						XlFindLookIn.xlValues, 
						mInfo.MatchWholeWord ? XlLookAt.xlWhole : XlLookAt.xlPart,
						XlSearchOrder.xlByRows,
						XlSearchDirection.xlNext,
						mInfo.MatchLetterCase,
						Type.Missing,
						Type.Missing);

				if (found != null) {
					String first = found.get_Address(
							true, true, 
							XlReferenceStyle.xlA1, 
							Type.Missing, Type.Missing);
					String address = first;

					do {
						int index = found.Row - area.Row + 1;
						Range row = (Range)area.Rows[index, Type.Missing];
						List<Range> cells = this.GetCellsFromRow(row);
						List<Range> match = new List<Range>();
						
						do {
							match.Add(found);
							found = area.FindNext(found);
							address = found.get_Address(
									true, true, 
									XlReferenceStyle.xlA1, 
									Type.Missing, Type.Missing);
						} while (!address.Equals(first) && (found.Row == index));

						this.ProcessMatchedCellsByAddress(cells, match);
						++mInsertRowIndex;
					} while (!address.Equals(first));
				}
			}
		}
		#endregion

		#region Compare Value Functions
		private bool IsValueMatch(String text)
		{
			RegexOptions options = mInfo.MatchLetterCase 
					? RegexOptions.None : RegexOptions.IgnoreCase; 
			bool matchFound = false;

			if (!String.IsNullOrEmpty(text)) {
				if (mInfo.IsExpressions) {
					if (Regex.IsMatch(text, mInfo.SearchString, options)) {
						matchFound = true;
					}
				}
				else if (mInfo.MatchLetterCase && mInfo.MatchWholeWord) {
					if (text.Equals(mInfo.SearchString, StringComparison.CurrentCulture)) {
						matchFound = true;
					}
				}
				else if (mInfo.MatchWholeWord) {
					if (text.Equals(mInfo.SearchString, StringComparison.CurrentCultureIgnoreCase)) {
						matchFound = true;
					}
				}
				else if (mInfo.MatchLetterCase) {
					if (text.IndexOf(mInfo.SearchString, StringComparison.CurrentCulture) != -1) {
						matchFound = true;
					}
				}
				else {
					if (text.IndexOf(mInfo.SearchString, StringComparison.CurrentCultureIgnoreCase) != -1) {
						matchFound = true;
					}
				}
			}

			return matchFound;
		}
		#endregion

		#region Process Results Functions
		private void HighLightCells()
		{
			int rowColor = System.Drawing.ColorTranslator.ToWin32(mInfo.RowColor);
			int cellColor = System.Drawing.ColorTranslator.ToWin32(mInfo.CellColor);

			// Paint the rows
			foreach (KeyValuePair<Range, Range> pair in mMatchedCells) {
				if (mInfo.HighlightSrcRow) {
					pair.Key.EntireRow.Interior.Color = rowColor;
				}

				if (mInfo.HighlightDstRow) {
					pair.Value.EntireRow.Interior.Color = rowColor;
				}
			}

			// Paint the cells
			foreach (KeyValuePair<Range, Range> pair in mMatchedCells) {
				if (mInfo.HighlightSrcCell) {
					pair.Key.Interior.Color = cellColor;
				}

				if (mInfo.HighlightDstCell) {
					pair.Value.Interior.Color = cellColor;
				}
			}
		}

		private void HyperlinkCells()
		{
			foreach (KeyValuePair<Range, Range> pair in mMatchedCells) {
				if (mInfo.InsertSrcHyperlink && !mSortByRowNumber) {
					String text = pair.Value.Value2.ToString();
					String address = pair.Value.get_Address(true, true,
								XlReferenceStyle.xlA1,
								Type.Missing, Type.Missing);

					mWorksheet.Hyperlinks.Add(pair.Key, "", "'" + mResultsheet.Name + "'!" + address, text, text);
				}
		
				if (mInfo.InsertDstHyperlink) {
					String text = pair.Key.Value2.ToString();
					String address = pair.Key.get_Address(true, true, 
								XlReferenceStyle.xlA1, 
								Type.Missing, Type.Missing);

					mResultsheet.Hyperlinks.Add(pair.Value, "", "'" + mWorksheet.Name + "'!" + address, text, text);
				}
			}
		}

		private void SortResultRows()
		{
			if (mSortByRowNumber && mResultsheet.UsedRange.Count > 1) {
				mResultsheet.UsedRange.Sort(
						mResultsheet.Columns[1, Type.Missing],
						XlSortOrder.xlAscending,
						Type.Missing,
						Type.Missing,
						XlSortOrder.xlAscending,
						Type.Missing,
						XlSortOrder.xlAscending,
						XlYesNoGuess.xlNo,
						Type.Missing,
						true,
						XlSortOrientation.xlSortColumns,
						XlSortMethod.xlPinYin,
						XlSortDataOption.xlSortNormal,
						XlSortDataOption.xlSortNormal,
						XlSortDataOption.xlSortNormal); 
			}
		}

		private void ResizeColumns()
		{
			if (mInfo.ResizeColumn) {
				foreach (object colObj in mResultsheet.UsedRange.Columns) {
					Range col = (Range)colObj;

					try {
						col.EntireColumn.AutoFit();
					}
					catch (Exception e) {
						mEventLog.Error("Unable to auto fit column: " + e.Message);
					}
				}
			}
		}
		#endregion

		#region Extract Cell Functions
		private List<Range> GetCellsFromRange(Areas areas, int row)
		{
			List<Range> list = new List<Range>();

			for (int j = 1; j <= areas.Count; ++j) {
				Range range = (Range)mSearchRange.Areas[j].Rows[row, Type.Missing];

				foreach (object colObj in range.Cells) {
					list.Add((Range)colObj);
				}
			}

			return list;
		}

		private List<Range> GetCellsFromRow(Range row)
		{
			List<Range> list = new List<Range>();

			foreach (object colObj in row.Cells) {
				list.Add((Range)colObj);
			}

			return list;
		}

		private Range GetCellFromWorksheet(Worksheet sheet, int row, int col)
		{
			return (Range)sheet.Cells[row, col];
		}
		#endregion

		#region Check Cell List
		private bool CellListContainsMatch(List<Range> list, List<Range> match)
		{
			bool containsMatch = false;

			foreach (Range cell in list) {
				if ((cell.Value2 != null) && (this.IsValueMatch(cell.Value2.ToString()))) {
					match.Add(cell);
					containsMatch = true;
				}
			}

			return containsMatch;
		}
		#endregion

		#region Process Cell Functions
		private void ProcessMatchedCellsByReference(List<Range> list, List<Range> match)
		{
			int index = 1;

			if (mInsertRowNumber) {
				this.GetCellFromWorksheet(mResultsheet, mInsertRowIndex, index).Value2 = list[0].Row;
				++index;
			}

			foreach (Range src in list) {
				if (src.Value2 != null) {
					Range dst = this.GetCellFromWorksheet(mResultsheet, mInsertRowIndex, index);

					dst.Value2 = src.Value2;
					dst.NumberFormat = src.NumberFormat;

					if (match.Contains(src)) {
						mMatchedCells.Add(src, dst);
					}
				}

				++index;
			}
		}
		private void ProcessMatchedCellsByAddress(List<Range> list, List<Range> match)
		{
			int index = 1;

			if (mInsertRowNumber) {
				this.GetCellFromWorksheet(mResultsheet, mInsertRowIndex, index).Value2 = list[0].Row;
				++index;
			}

			foreach (Range src in list) {
				if (src.Value2 != null) {
					Range dst = this.GetCellFromWorksheet(mResultsheet, mInsertRowIndex, index);
					Range item = match.Find(new CompareCellByAddress(src).IsEqual);

					dst.Value2 = src.Value2;
					dst.NumberFormat = src.NumberFormat;
					
					if (item != null) {
						mMatchedCells.Add(item, dst);
					}
				}

				++index;
			}
		}
		#endregion

		#region Utility Class
		public class CompareCellByAddress {
			public CompareCellByAddress(Range cell)
			{
				mCell = cell;
				mAddress = mCell.get_Address(
						true, true,
						XlReferenceStyle.xlA1,
						Type.Missing,
						Type.Missing);
			}

			public bool IsEqual(Range cell)
			{
				String address = cell.get_Address(
						true, true,
						XlReferenceStyle.xlA1,
						Type.Missing,
						Type.Missing);

				return mAddress.Equals(address);
			}

			private Range mCell;
			private String mAddress;
		}
		#endregion
				
		private Application mApp;
		private SearchInfo mInfo;
		private Workbook mWorkbook;
		private Worksheet mWorksheet;
		private Worksheet mResultsheet;
		private Range mSearchRange;
		private EventLog mEventLog;

		private int mInsertRowIndex;
		private int mSearchRowCount;
		private bool mInsertRowNumber;
		private bool mSortByRowNumber;
		private bool mLinkRangeRows;

		private Dictionary<Range, Range> mMatchedCells;
	}
	#endregion
}
}
