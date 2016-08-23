/*****************************************************************************\
**
** StringAsset
**
** Implementation of string assets.
**
\*****************************************************************************/

namespace ExcelFormater {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;
using ExcelUtility.Asset;

/* Classes *******************************************************************/

public class StringAsset : IAssetItem<StringAsset> {
	#region Constructors
	public StringAsset()
	{
		mText = "";
	}

	public StringAsset(String text)
	{
		mText = text;
	}
	#endregion

	#region Properties
	public String Text
	{
		get { return mText; }
		set { mText = value; }
	}
	#endregion

	#region Functions
	public bool Equals(StringAsset item)
	{
		return this.Text.Equals(item.Text);
	}

	public int Compate(StringAsset item)
	{
		return this.Text.CompareTo(item.Text);
	}

	public void Copy(StringAsset item)
	{
		this.Text = (String)item.Text.Clone();
	}

	public StringAsset Clone()
	{
		return new StringAsset((String)this.Text.Clone());
	}

	public override string ToString()
	{
		return mText;
	}
	#endregion

	#region Members
	private String mText;
	#endregion
}
}
