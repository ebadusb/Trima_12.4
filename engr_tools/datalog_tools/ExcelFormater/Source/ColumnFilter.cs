/*****************************************************************************\
**
** ColumnFilter
**
** Implementation of a column filter category.
**
\*****************************************************************************/

namespace ExcelFormater {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using ExcelUtility.Asset;
using System.Text.RegularExpressions;

/* Classes *******************************************************************/

public class ColumnFilter : ColumnCategory, IAssetName<ColumnFilter>, IAssetItem<ColumnFilter> {
	#region Constructor
	public ColumnFilter()
	{
		mFilter = "";
		mIgnoreCase = false;
		mNegate = false;
	}
	#endregion

	#region Properties
	public String Filter
	{
		get { return mFilter; }
		set { mFilter = value; }
	}

	public bool IgnoreCase
	{
		get { return mIgnoreCase; }
		set { mIgnoreCase = value; }
	}

	public bool Negate
	{
		get { return mNegate; }
		set { mNegate = value; }
	}
	#endregion

	#region Functions
	public override IEnumerable<String> GetColumns()
	{
		List<String> list = new List<string>();
		RegexOptions options = mIgnoreCase ? RegexOptions.IgnoreCase : RegexOptions.None; 

		try {
			foreach (StringAsset item in ExcelFormater.GetInstance().Assets.KnownColumnAssetManager) {
				bool match = Regex.IsMatch(item.Text, mFilter, options);

				if (mNegate ? !match : match) {
					list.Add(item.Text);
				}
			}
		}
		catch {
			// It's an invalid expression... do nothing. 
		}
		
		return list;
	}

	public bool Equals(ColumnFilter item)
	{
		return this.Name.Equals(item.Name)
			|| this.Filter.Equals(item.Filter);
	}

	public int Compate(ColumnFilter item)
	{
		return this.Name.CompareTo(item.Name);
	}

	public void Copy(ColumnFilter item)
	{
		this.Name = item.Name;
		this.Filter = item.Filter;
		this.Negate = item.Negate;
	}

	public ColumnFilter Clone()
	{
		ColumnFilter item = new ColumnFilter();

		item.Copy(this);

		return item;
	}
	#endregion

	#region Members
	private String mFilter;
	private bool mIgnoreCase;
	private bool mNegate;
	#endregion
}
}
