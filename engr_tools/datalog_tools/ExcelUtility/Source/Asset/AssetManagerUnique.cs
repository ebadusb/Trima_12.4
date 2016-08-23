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

public class AssetManagerUnique<TYPE> : AssetManager<TYPE> where TYPE : IAssetItem<TYPE> {
	#region Functions
	public override bool New(TYPE newItem)
	{
		TYPE item = mItems.Find(newItem.Equals);

		if (item == null) {
			mItems.Add(newItem.Clone());
			this.SendNewItemEvent(newItem);
			return true;
		}

		return false;
	}

	public override bool Edit(TYPE oldItem, TYPE newItem)
	{
		TYPE item = mItems.Find(oldItem.Equals);

		if (item != null) {
			item.Copy(newItem);
			this.SendEditItemEvent(oldItem, newItem);
			return true;
		}

		return false;
	}

	public override bool Delete(TYPE delItem)
	{
		TYPE item = mItems.Find(delItem.Equals);

		if (item != null) {
			mItems.Remove(item);
			this.SendDeleteItemEvent(delItem);
			return true;
		}

		return false;
	}

	public override bool Exists(TYPE item)
	{
		return mItems.Exists(item.Equals);
	}

	public override TYPE Find(Predicate<TYPE> match)
	{
		return mItems.Find(match).Clone();
	}

	public override IEnumerator<TYPE> GetEnumerator()
	{
		return new AssetEnumerator(mItems.GetEnumerator());
	}
	#endregion
}
}
