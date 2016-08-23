/*****************************************************************************\
**
** AssetEventArgs
**
** Implementation of the asset event arguments.
**
\*****************************************************************************/

namespace ExcelUtility.Asset {

/* Usings ********************************************************************/

using System;
using System.Collections.Generic;
using System.Text;

/* Classes *******************************************************************/

public class ProcessAssetArgs<_TYPE> {
	public ProcessAssetArgs(_TYPE item)
	{
		mItem = item;
	}

	public _TYPE Item
	{
		get { return mItem; }
	}

	private readonly _TYPE mItem;
}

public class SaveAssetArgs<_TYPE> {
	public SaveAssetArgs(_TYPE item)
	{
		mItem = item;
		mReplace = false;
	}

	public SaveAssetArgs(_TYPE item, bool replace)
	{
		mItem = item;
		mReplace = replace;
	}

	public _TYPE Item
	{
		get { return mItem; }
	}

	public bool Replace
	{
		get { return mReplace; }
	}

	private readonly _TYPE mItem;
	private readonly bool mReplace;
}

public class LogAssetArgs<_TYPE> {
	public LogAssetArgs(_TYPE item, String log)
	{
		mItem = item;
		mLog = log;
	}

	public _TYPE Item
	{
		get { return mItem; }
	}
	
	public String Log
	{
		get { return mLog; }
	}

	private readonly _TYPE mItem;
	private readonly String mLog;
}

public class NewAssetArgs<_TYPE> {
	public NewAssetArgs(_TYPE item)
	{
		mItem = item;
	}

	public _TYPE Item
	{
		get { return mItem; }
	}

	private readonly _TYPE mItem;
}

public class EditAssetArgs<_TYPE> {
	public EditAssetArgs(_TYPE oldItem, _TYPE newItem)
	{
		mOldItem = oldItem;
		mNewItem = newItem;
	}

	public _TYPE OldItem
	{
		get { return mOldItem; }
	}

	public _TYPE NewItem
	{
		get { return mNewItem; }
	}

	private readonly _TYPE mOldItem;
	private readonly _TYPE mNewItem;
}

public class DeleteAssetArgs<_TYPE> {
	public DeleteAssetArgs(_TYPE item)
	{
		mItem = item;
	}

	public _TYPE Item
	{
		get { return mItem; }
	}

	private readonly _TYPE mItem;
}
}
