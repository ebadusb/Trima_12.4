// CntrItem.h : interface of the CAP2predictionaterCntrItem class
//

#if !defined(AFX_CNTRITEM_H__D62B9910_B89E_11D1_B0ED_AAE114000000__INCLUDED_)
#define AFX_CNTRITEM_H__D62B9910_B89E_11D1_B0ED_AAE114000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CAP2predictionaterDoc;
class CAP2predictionaterView;

class CAP2predictionaterCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CAP2predictionaterCntrItem)

// Constructors
public:
	CAP2predictionaterCntrItem(CAP2predictionaterDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CAP2predictionaterDoc* GetDocument()
		{ return (CAP2predictionaterDoc*)COleClientItem::GetDocument(); }
	CAP2predictionaterView* GetActiveView()
		{ return (CAP2predictionaterView*)COleClientItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAP2predictionaterCntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual BOOL CanActivate();
	//}}AFX_VIRTUAL

// Implementation
public:
	~CAP2predictionaterCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__D62B9910_B89E_11D1_B0ED_AAE114000000__INCLUDED_)
