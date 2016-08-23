/*****************************************************************************\
**
** ExcelFill
**
** Implementation of a fill info.
**
\*****************************************************************************/

namespace ExcelUtility.Fill {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Xml.Serialization;
using Asset;

/* Classes *******************************************************************/

public class FillInfo : IAssetName<FillInfo>, IAssetItem<FillInfo> {
	#region Constructor
	public FillInfo()
	{
		mName = "";
		mColor = System.Drawing.Color.Yellow;
		mHighlight = false;
		mHyperlink = false;
	}
	#endregion

	#region Properties
	public String Name
	{
		get { return mName; }
		set { mName = value; }
	}

	[XmlIgnoreAttribute()]
	public Color Color
	{
		get { return mColor; }
		set { mColor = value; }
	}

	[XmlElement("Color")]
	public string ColorHtml
	{
		get { return ColorTranslator.ToHtml(mColor); }
		set { mColor = ColorTranslator.FromHtml(value); }
	}

	public bool Highlight
	{
		get { return mHighlight; }
		set { mHighlight = value; }
	}

	public bool Hyperlink
	{
		get { return mHyperlink; }
		set { mHyperlink = value; }
	}
	#endregion

	#region Functions
	public bool Equals(FillInfo item)
	{
		return this.Name.Equals(item.Name)
			&& this.Color.ToArgb() == item.Color.ToArgb()
			&& this.Highlight == item.Highlight
			&& this.Hyperlink == item.Hyperlink;
	}

	public int Compate(FillInfo item)
	{
		return this.Name.CompareTo(item.Name);
	}

	public void Copy(FillInfo item)
	{
		this.Name = item.Name;
		this.Color = item.Color;
		this.Highlight = item.Highlight;
		this.Hyperlink = item.Hyperlink;
	}

	public FillInfo Clone()
	{
		FillInfo item = new FillInfo();

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
	private Color mColor;
	private bool mHighlight;
	private bool mHyperlink;
	#endregion
}
}
