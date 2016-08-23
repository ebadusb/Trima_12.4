/*****************************************************************************\
**
** AssetManagerUnique
**
** Implementation of the unique asset manager.
**
\*****************************************************************************/

namespace ExcelUtility.Asset {

/* Usings ********************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

/* Classes *******************************************************************/

public class AssetManagerStacked<TYPE> : AssetManagerUnique<TYPE> where TYPE : IAssetItem<TYPE> {
	#region Functions
	public override bool New(TYPE newItem)
	{
		TYPE item = mItems.Find(newItem.Equals);
		
		if (item == null) {
			mItems.Insert(0, newItem.Clone());
			this.SendNewItemEvent(newItem.Clone());
			this.TrimOverflow();
			return true;
		}

		return false;
	}

	public override bool Edit(TYPE oldItem, TYPE newItem)
	{
		TYPE item = mItems.Find(oldItem.Equals);

		if (item != null) {
			mItems.Remove(item);
			mItems.Insert(0, newItem.Clone());
			this.SendEditItemEvent(oldItem, newItem);
			return true;
		}

		return false;
	}

	public void Clear()
	{
		while (mItems.Count > 0) {
			TYPE delItem = mItems[0];

			mItems.Remove(delItem);
			this.SendDeleteItemEvent(delItem);
		}
	}
	#endregion

	#region Properties
	public int Max
	{
		get { return mMax; }
		set { mMax = value; this.TrimOverflow(); }
	}
	#endregion

	#region Functions
	private void TrimOverflow()
	{
		if ((mMax > 0) && (mItems.Count > mMax)) {
			if (mItems.Count > mMax) {
				mItems.RemoveRange(mMax, mItems.Count - mMax);
			}
		}
	}
	#endregion

	#region Members
	protected int mMax;
	#endregion
}
}
