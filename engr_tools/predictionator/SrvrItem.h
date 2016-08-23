// SrvrItem.h : interface of the CAP2predictionaterSrvrItem class
//

#if !defined(AFX_SRVRITEM_H__D62B990C_B89E_11D1_B0ED_AAE114000000__INCLUDED_)
#define AFX_SRVRITEM_H__D62B990C_B89E_11D1_B0ED_AAE114000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CAP2predictionaterSrvrItem : public COleServerItem
{
	DECLARE_DYNAMIC(CAP2predictionaterSrvrItem)

// Constructors
public:
	CAP2predictionaterSrvrItem(CAP2predictionaterDoc* pContainerDoc);

// Attributes
	CAP2predictionaterDoc* GetDocument() const
		{ return (CAP2predictionaterDoc*)COleServerItem::GetDocument(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAP2predictionaterSrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CAP2predictionaterSrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SRVRITEM_H__D62B990C_B89E_11D1_B0ED_AAE114000000__INCLUDED_)
