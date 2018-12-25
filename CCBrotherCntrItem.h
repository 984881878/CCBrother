#pragma once
#include <afxole.h>

class CCCBrotherDrawDoc;
class CCCBrotherDrawView;

class CCBrotherCntrItem :
	public COleClientItem
{
	DECLARE_SERIAL(CCBrotherCntrItem)

public:
	CCBrotherCntrItem(CCCBrotherDrawDoc* pContainer = nullptr);
	// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
	//  IMPLEMENT_SERIALIZE requires the class have a constructor with
	//  zero arguments.  Normally, OLE items are constructed with a
	//  non-NULL document pointer.
	~CCBrotherCntrItem();
	CCCBrotherDrawDoc* GetDocument();
	CCCBrotherDrawView* GetActiveView();
	BOOL IsPoint(CPoint point);
	void DrawShape(CDC* pDC);


public:
	float m_X1, m_Y1, m_X2, m_Y2;
	BOOL b_Select;	//是否选中
	// 属性

public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	virtual void Serialize(CArchive& ar);

protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

