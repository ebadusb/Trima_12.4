/*****************************************************************************\
**
** FormatInfo
**
** Implementation of a format info.
**
\*****************************************************************************/

namespace ExcelUtility.Format {

/* Usings ********************************************************************/
	
using System;
using System.Collections.Generic;
using System.Text;
using Asset;

/* Classes *******************************************************************/

public class FormatInfo : IAssetName<FormatInfo>, IAssetItem<FormatInfo> {
	public enum Ordering {
		Time,
		Index
	}

	#region Constructor
	public FormatInfo()
	{
		mName = "";
		mDestination = "";
		mColumns = new List<String>();
		mLevels = new List<String>();
		mOrdering = Ordering.Index;
		mAllLevels = false;
	}
	#endregion

	#region Properties
	public String Name
	{
		get { return mName; }
		set { mName = value; }
	}

	public String Destination
	{
		get { return mDestination; }
		set { mDestination = value; }
	}

	public List<String> Columns
	{
		get { return mColumns; }
		set { mColumns = value; }
	}

	public List<String> Levels
	{
		get { return mLevels; }
		set { mLevels = value; }
	}

	public Ordering Order
	{
		get { return mOrdering; }
		set { mOrdering = value; }
	}

	public bool AllLevels
	{
		get { return mAllLevels; }
		set { mAllLevels = value; }
	}
	#endregion

	#region Functions
	public bool Equals(FormatInfo item)
	{
		if (!this.Name.Equals(item.Name)
				|| !this.Destination.Equals(item.Destination) 
				|| this.Columns.Count != item.Columns.Count
				|| this.Levels.Count != item.Levels.Count
				|| this.Order != item.Order
				|| this.AllLevels != item.AllLevels) {
			return false;
		}

		foreach (String name in this.Columns) {
			if (!item.Columns.Exists(name.Equals)) {
				return false;
			}
		}

		foreach (String name in this.Levels) {
			if (!item.Levels.Exists(name.Equals)) {
				return false;
			}
		}

		return true;
	}

	public int Compate(FormatInfo item)
	{
		return this.Name.CompareTo(item.Name);
	}

	public void Copy(FormatInfo item)
	{
		this.Name = item.Name;
		this.Destination = item.Destination;
		this.Order = item.Order;
		this.AllLevels = item.AllLevels;

		this.Columns.Clear();
		this.Levels.Clear();

		this.Columns.AddRange(item.mColumns);
		this.Levels.AddRange(item.mLevels);
	}

	public FormatInfo Clone()
	{
		FormatInfo item = new FormatInfo();

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
	private String mDestination;
	private List<String> mColumns;
	private List<String> mLevels;
	private Ordering mOrdering;
	private bool mAllLevels;
	#endregion
}
}
