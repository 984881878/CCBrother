#include "stdafx.h"
#include "CCBrotherCntrItem.h"
#include "CC_Brother_DrawView.h"
#include "CC_Brother_DrawDoc.h"

extern void DeviceP_To_LogicalP(float x, float y, int *X, int *Y);
extern void LogicalP_To_DeviceP(int x, int y, float *X, float *Y);
extern BOOL IsRectCross(float minx, float miny, float maxx, float maxy);

IMPLEMENT_SERIAL(CCBrotherCntrItem, COleClientItem, 0)

CCBrotherCntrItem::CCBrotherCntrItem(CCCBrotherDrawDoc* pContainer)
	:COleClientItem(pContainer)
{
	LogicalP_To_DeviceP(10, 210, &m_X1, &m_Y1);
	LogicalP_To_DeviceP(210, 10, &m_X2, &m_Y2);
	b_Select = FALSE;
}


CCBrotherCntrItem::~CCBrotherCntrItem()
{

}

CCCBrotherDrawDoc* CCBrotherCntrItem::GetDocument()
{
	return (CCCBrotherDrawDoc*)COleClientItem::GetDocument();
}

CCCBrotherDrawView* CCBrotherCntrItem::GetActiveView()
{
	return (CCCBrotherDrawView*)COleClientItem::GetActiveView();
}

void CCBrotherCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	// When an item is being edited (either in-place or fully open)
	//  it sends OnChange notifications for changes in the state of the
	//  item or visual appearance of its content.

	// TODO: invalidate the item by calling UpdateAllViews
	//  (with hints appropriate to your application)

	GetDocument()->UpdateAllViews(NULL);
	// for now just update ALL views/no hints
}

BOOL CCBrotherCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	// During in-place activation CDrawCntrItem::OnChangeItemPosition
	//  is called by the server to change the position of the in-place
	//  window.  Usually, this is a result of the data in the server
	//  document changing such that the extent has changed or as a result
	//  of in-place resizing.
	//
	// The default here is to call the base class, which will call
	//  COleClientItem::SetItemRects to move the item
	//  to the new position.

	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;

	// TODO: update any cache you may have of the item's rectangle/extent

	return TRUE;
}

void CCBrotherCntrItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	// During in-place activation, CDrawCntrItem::OnGetItemPosition
	//  will be called to determine the location of this item.  The default
	//  implementation created from AppWizard simply returns a hard-coded
	//  rectangle.  Usually, this rectangle would reflect the current
	//  position of the item relative to the view used for activation.
	//  You can obtain the view by calling CDrawCntrItem::GetActiveView.

	// TODO: return correct rectangle (in pixels) in rPosition

	rPosition.SetRect(10, 10, 210, 210);
}

void CCBrotherCntrItem::OnActivate()
{
	// Allow only one inplace activate item per frame
	CCCBrotherDrawView* pView = GetActiveView();
	ASSERT_VALID(pView);
	COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
	if (pItem != NULL && pItem != this)
		pItem->Close();

	COleClientItem::OnActivate();
}

void CCBrotherCntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

	// Hide the object if it is not an outside-in object
	DWORD dwMisc = 0;
	m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
	if (dwMisc & OLEMISC_INSIDEOUT)
		DoVerb(OLEIVERB_HIDE, NULL);
}

void CCBrotherCntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);
	COleClientItem::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_X1 << m_Y1 << m_X2 << m_Y2;
	}
	else
	{
		ar >> m_X1 >> m_Y1 >> m_X2 >> m_Y2;
		b_Select = FALSE;
	}
}

void CCBrotherCntrItem::DrawShape(CDC * pDC)
{
	int x1, y1, x2, y2;
	CRect m_rect;
	//加入判断是否在
	if (!IsRectCross(m_X1, m_Y1, m_X2, m_Y2))
		return;
	DeviceP_To_LogicalP(m_X1, m_Y1, &x1, &y1);
	DeviceP_To_LogicalP(m_X2, m_Y2, &x2, &y2);
	m_rect.SetRect(x1, y2, x2, y1);
	COleClientItem::Draw(pDC, m_rect);
	if (b_Select)
		pDC->InvertRect(&m_rect);
}

BOOL CCBrotherCntrItem::IsPoint(CPoint point)
{
	float x1, y1;
	LogicalP_To_DeviceP(point.x, point.y, &x1, &y1);
	if (x1 >= m_X1 && x1 <= m_X2 && y1 >= m_Y1 && y1 <= m_Y2)
		return TRUE;
	return FALSE;
}

#ifdef _DEBUG
void CCBrotherCntrItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CCBrotherCntrItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif