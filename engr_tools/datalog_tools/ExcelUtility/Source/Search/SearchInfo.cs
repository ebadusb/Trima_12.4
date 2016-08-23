/*****************************************************************************\
**
** SearchInfo
**
** Implementation of a search info.
**
\*****************************************************************************/

namespace ExcelUtility.Search {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Xml.Serialization;
using Asset;

/* Classes *******************************************************************/

public class SearchInfo : IAssetName<SearchInfo>, IAssetItem<SearchInfo> {
	#region Constructor
	public SearchInfo()
	{
		mName = "";
	}
	#endregion	
	
	#region Properties
	public String Name
	{
		get { return mName; }
		set { mName = value; }
	}
	
	public String SearchString
	{
		get { return mSearchString; }
		set { mSearchString = value; }
	}

	public String SearchArea
	{
		get { return mSearchArea; }
		set { mSearchArea = value; }
	}

	public String ResultSheet
	{
		get { return mResultSheet; }
		set { mResultSheet = value; }
	}

	[XmlIgnoreAttribute()]
	public Color RowColor
	{
		get { return mRowColor; }
		set { mRowColor = value; }
	}

	[XmlIgnoreAttribute()]
	public Color CellColor
	{
		get { return mCellColor; }
		set { mCellColor = value; }
	}

	[XmlElement("RowColor")]
	public string RowColorHtml
	{
		get { return ColorTranslator.ToHtml(mRowColor); }
		set { mRowColor = ColorTranslator.FromHtml(value); }
	}

	[XmlElement("CellColor")]
	public string CellColorHtml
	{
		get { return ColorTranslator.ToHtml(mCellColor); }
		set { mCellColor = ColorTranslator.FromHtml(value); }
	}

	public bool IsExpressions
	{
		get { return mIsExpressions; }
		set { mIsExpressions = value; }
	}

	public bool MatchLetterCase
	{
		get { return mMatchLetterCase; }
		set { mMatchLetterCase = value; }
	}

	public bool MatchWholeWord
	{
		get { return mMatchWholeWord; }
		set { mMatchWholeWord = value; }
	}

	public bool LinkRangeRows
	{
		get { return mLinkRangeRows; }
		set { mLinkRangeRows = value; }
	}

	public bool HighlightSrcCell
	{
		get { return mHighlightSrcCell; }
		set { mHighlightSrcCell = value; }
	}

	public bool HighlightDstCell
	{
		get { return mHighlightDstCell; }
		set { mHighlightDstCell = value; }
	}

	public bool HighlightSrcRow
	{
		get { return mHighlightSrcRow; }
		set { mHighlightSrcRow = value; }
	}

	public bool HighlightDstRow
	{
		get { return mHighlightDstRow; }
		set { mHighlightDstRow = value; }
	}

	public bool InsertSrcHyperlink
	{
		get { return mInsertSrcHyperlink; }
		set { mInsertSrcHyperlink = value; }
	}

	public bool InsertDstHyperlink
	{
		get { return mInsertDstHyperlink; }
		set { mInsertDstHyperlink = value; }
	}

	public bool InsertRowNum
	{
		get { return mInsertRowNum; }
		set { mInsertRowNum = value; }
	}

	public bool SortByRowNum
	{
		get { return mSortRowNum; }
		set { mSortRowNum = value; }
	}

	public bool ResizeColumn
	{
		get { return mResizeColumn; }
		set { mResizeColumn = value; }
	}

	public bool ClearPrevious
	{
		get { return mClearPrevious; }
		set { mClearPrevious = value; }
	}
	#endregion

	#region Functions
	public bool Equals(SearchInfo item)
	{
		return this.Name == item.Name
				&& this.SearchString == item.SearchString
				&& this.SearchArea == item.SearchArea
				&& this.ResultSheet == item.ResultSheet
				&& this.RowColor.ToArgb() == item.RowColor.ToArgb()
				&& this.CellColor.ToArgb() == item.CellColor.ToArgb()
				&& this.IsExpressions == item.IsExpressions
				&& this.MatchLetterCase == item.MatchLetterCase
				&& this.MatchWholeWord == item.MatchWholeWord
				&& this.LinkRangeRows == item.LinkRangeRows
				&& this.HighlightSrcCell == item.HighlightSrcCell
				&& this.HighlightDstCell == item.HighlightDstCell
				&& this.HighlightSrcRow == item.HighlightSrcRow
				&& this.HighlightDstRow == item.HighlightDstRow
				&& this.InsertSrcHyperlink == item.InsertSrcHyperlink
				&& this.InsertDstHyperlink == item.InsertDstHyperlink
				&& this.InsertRowNum == item.InsertRowNum
				&& this.SortByRowNum == item.SortByRowNum
				&& this.ResizeColumn == item.ResizeColumn
				&& this.ClearPrevious == item.ClearPrevious;
	}

	public int Compate(SearchInfo item)
	{
		return this.Name.CompareTo(item.Name);
	}

	public void Copy(SearchInfo item)
	{
		this.Name = item.Name;
		this.SearchString = item.SearchString;
		this.ResultSheet = item.ResultSheet;
		this.SearchArea = item.SearchArea;
		this.CellColor = item.CellColor;
		this.RowColor = item.RowColor;
		this.IsExpressions = item.IsExpressions;
		this.MatchLetterCase = item.MatchLetterCase;
		this.MatchWholeWord = item.MatchWholeWord;
		this.LinkRangeRows = item.LinkRangeRows;
		this.HighlightSrcCell = item.HighlightSrcCell;
		this.HighlightDstCell = item.HighlightDstCell;
		this.HighlightSrcRow = item.HighlightSrcRow;
		this.HighlightDstRow = item.HighlightDstRow;
		this.InsertSrcHyperlink = item.InsertSrcHyperlink;
		this.InsertDstHyperlink = item.InsertDstHyperlink;
		this.InsertRowNum = item.InsertRowNum;
		this.SortByRowNum = item.SortByRowNum;
		this.ResizeColumn = item.ResizeColumn;
		this.ClearPrevious = item.ClearPrevious;
	}

	public SearchInfo Clone()
	{
		SearchInfo item = new SearchInfo();

		item.Copy(this);

		return item;
	}

	public override string ToString()
	{
		return mName;
	}
	#endregion

	#region Members
	private String mName;
	private String mSearchString;
	private String mSearchArea;
	private String mResultSheet;

	private Color mRowColor;
	private Color mCellColor;

	private bool mIsExpressions;
	private bool mMatchLetterCase;
	private bool mMatchWholeWord;
	private bool mLinkRangeRows;
	private bool mHighlightSrcCell;
	private bool mHighlightDstCell;
	private bool mHighlightSrcRow;
	private bool mHighlightDstRow;
	private bool mInsertSrcHyperlink;
	private bool mInsertDstHyperlink;
	private bool mInsertRowNum;
	private bool mSortRowNum;
	private bool mResizeColumn;
	private bool mClearPrevious;
	#endregion
}
}
