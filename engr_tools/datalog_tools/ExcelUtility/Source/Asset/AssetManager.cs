/*****************************************************************************\
**
** AssetManager
**
** Implementation of the asset manager.
**
\*****************************************************************************/

namespace ExcelUtility.Asset {

/* Usings ********************************************************************/

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

/* Classes *******************************************************************/

public class AssetManager<TYPE> : IAssetManager<TYPE> where TYPE : IAssetItem<TYPE> {
	#region Constructor
	public AssetManager()
	{
		mItems = new List<TYPE>();
		mEventsEnable = true;
	}
	#endregion

	#region Delegates
	public delegate void NewAssetHandler(object sender, NewAssetArgs<TYPE> args);
	public delegate void EditAssetHandler(object sender, EditAssetArgs<TYPE> args);
	public delegate void DeleteAssetHandler(object sender, DeleteAssetArgs<TYPE> args);

	public event NewAssetHandler NewAsset;
	public event EditAssetHandler EditAsset;
	public event DeleteAssetHandler DeleteAsset;
	#endregion

	#region Properties
	public bool EventsEnable
	{
		get { return mEventsEnable; }
		set { mEventsEnable = value; }
	}

	public int Count
	{
		get { return mItems.Count; }
	}
	#endregion

	#region Functions
	public virtual bool New(TYPE newItem)
	{
		if (!mItems.Contains(newItem)) {
			mItems.Add(newItem);
			this.SendNewItemEvent(newItem);
			return true;
		}

		return false;
	}

	public virtual bool Edit(TYPE oldItem, TYPE newItem)
	{
		if (mItems.Contains(oldItem)) {
			oldItem.Copy(newItem);
			this.SendEditItemEvent(oldItem, newItem);
			return true;
		}

		return false;
	}

	public virtual bool Delete(TYPE delItem)
	{
		if (mItems.Contains(delItem)) {
			mItems.Remove(delItem);
			this.SendDeleteItemEvent(delItem);
			return true;
		}

		return false;
	}

	public virtual bool Exists(TYPE item)
	{
		return mItems.Contains(item);
	}

	public virtual bool Exists(Predicate<TYPE> match)
	{
		return mItems.Exists(match);
	}

	public virtual TYPE Find(Predicate<TYPE> match)
	{
		return mItems.Find(match);
	}

	public virtual IEnumerator<TYPE> GetEnumerator()
	{
		return mItems.GetEnumerator();
	}

	IEnumerator IEnumerable.GetEnumerator()
	{
		return mItems.GetEnumerator();
	}

	protected void SendNewItemEvent(TYPE newItem)
	{
		if (mEventsEnable && this.NewAsset != null) {
			this.NewAsset(this, new NewAssetArgs<TYPE>(newItem));
		}
	}

	protected void SendEditItemEvent(TYPE oldItem, TYPE newItem)
	{
		if (mEventsEnable && this.EditAsset != null) {
			this.EditAsset(this, new EditAssetArgs<TYPE>(oldItem, newItem));
		}
	}

	protected void SendDeleteItemEvent(TYPE delItem)
	{
		if (mEventsEnable && this.DeleteAsset != null) {
			this.DeleteAsset(this, new DeleteAssetArgs<TYPE>(delItem));
		}
	}
	#endregion

	#region Classes
	protected class AssetEnumerator : IEnumerator<TYPE> {
		#region Constructor
		public AssetEnumerator(IEnumerator<TYPE> enumerator)
		{
			mEnumerator = enumerator;
		}
		#endregion

		#region Functions
		public void Reset()
		{
			mEnumerator.Reset();
		}

		public bool MoveNext()
		{
			return mEnumerator.MoveNext();
		}

		public void Dispose()
		{
		}
		#endregion

		#region Properties
		public TYPE Current
		{
			get { return mEnumerator.Current.Clone(); }
		}

		object IEnumerator.Current
		{
			get { return mEnumerator.Current.Clone(); }
		}
		#endregion

		#region Members
		private IEnumerator<TYPE> mEnumerator;
		#endregion
	}
	#endregion

	#region Members
	protected List<TYPE> mItems;
	protected bool mEventsEnable;
	#endregion
}
}
