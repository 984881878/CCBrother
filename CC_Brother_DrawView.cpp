
// CC_Brother_DrawView.cpp: CCCBrotherDrawView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CC_Brother_Draw.h"
#endif

#include "CC_Brother_DrawDoc.h"
#include "CC_Brother_DrawView.h"
#include "TempShape.h"
#include "CCBrotherCntrItem.h"
//
#include "CC_Brother_TextDlg.h"
//
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//全局的实例对象 指针  有待激活，指向了一个实例化的对象，从而能够调用他的成员函数实现下面的函数的功能
class CCCBrotherDrawView *ptr_View;

extern CC_Brother_GraphPara* p_GraphPara;   //全局指针  方便获取图形颜色等值

extern void Draw_Tmp_Switch(CClientDC* dc,CPen* pen,CPoint Point,int size_Of_Switch);

extern int firstPrinting;

//四个全局的坐标转换函数，以便其他类能够直接调用这些函数，而不是通过类的实例对象来访问   当然这四行显得有点冗余......
void  DeviceP_To_LogicalP(float x, float y, int *X, int *Y); //设备坐标点映射到逻辑坐标点 P代表Point
void  LogicalP_To_DeviceP(int x, int y, float *X, float*Y);
int   DeviceL_To_LogicalL(float length);     //设备上的长度转换为逻辑长度 L代表length
float LogicalL_To_DeviceL(int length);

//这四个函数的实现
void  DeviceP_To_LogicalP(float x, float y, int *X, int *Y)
{
	ptr_View->DeviceP_To_LogicalP(x,y,X,Y);
}

void  LogicalP_To_DeviceP(int x, int y, float *X, float*Y)
{
	ptr_View->LogicalP_To_DeviceP(x, y, X, Y);
}

int   DeviceL_To_LogicalL(float length)
{
	return ptr_View->DeviceL_To_LogicalL(length);
}

float LogicalL_To_DeviceL(int length)
{
	return ptr_View->LogicalL_To_DeviceL(length);
}


// CCCBrotherDrawView

IMPLEMENT_DYNCREATE(CCCBrotherDrawView, CView)

BEGIN_MESSAGE_MAP(CCCBrotherDrawView, CView)
	// 标准打印命令
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINTING,OnFilePrinting)
	ON_COMMAND(ID_BUTTON32771,OnDrawLine)
	ON_COMMAND(ID_BUTTON32790, OnDrawPLine)
	ON_COMMAND(ID_BUTTON32791, OnDrawPPLine)
	ON_COMMAND(ID_BUTTON32775, OnDrawText)
	ON_COMMAND(ID_BUTTON32776, OnDrawSwitch)
	ON_COMMAND(ID_BUTTON_GROUND, OnDrawGround)
	ON_COMMAND(ID_BUTTON32779, OnEnLarge)
	ON_COMMAND(ID_GRAPH_ZOOM, OnEnLarge)
	ON_COMMAND(ID_BUTTON_Shrink,OnShrink)
	ON_COMMAND(ID_GRAPH_SHIFT, OnShift)
	ON_COMMAND(ID_BUTTON32787, OnRecovery)
	ON_COMMAND(ID_GRAPH_ALL, OnGraphAll)		//全屏显示
	ON_COMMAND(ID_TEXT_MESSAGE,EditText)
	ON_COMMAND(ID_TEXT_ONOK,DlgOK)
	ON_COMMAND(ID_TEXT_ONCANCEL, DlgCancel)
	ON_COMMAND(ID_SELECT_MOUSE, OnSelectMouse)
	ON_COMMAND(ID_SELECT_CLEAR, OnSelectClear)
	ON_COMMAND(ID_SELECT_DELETE, OnSelectDelete)
	ON_COMMAND(ID_SELECT_CURSOR, OnSelectCursor)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_COLOR_PEN,OnPenColor)
	ON_COMMAND(ID_COLOR_BACK,OnBackColor)
	ON_COMMAND(ID_GRAPH_COLOR, OnGraphColor)
	ON_COMMAND(ID_LAYRE,OnLayre)
	ON_COMMAND(ID_LINETYPE,OnLineType)
	ON_COMMAND(ID_LINEWIDE,OnLineWide)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CCCBrotherDrawView 构造/析构

CCCBrotherDrawView::CCCBrotherDrawView() noexcept
{
	// TODO: 在此处添加构造代码
	m_xStart = 0.0;
	m_yStart = 0.0;
	blc = 1.0;//建立映射关系以及坐标系

	m_penColor = 0;
	m_backgroundColor = RGB(255, 255, 255);
	m_brushColor = 0;
	m_lineWide = 1;
	m_lineType = 0;
	m_curLayer = 0;
	m_MapMode = 1;
	//m_xScale = 1.0;
	sizeOfSwitch = 10;//对baseShape的初始化
	
	nScrollMin = 50;
	nXLine = 1;
	nYLine = 1;//对滚动条的参数初始化

	startDraw = true;
	finishDraw = false;  //对画图状态的初始化

	//对标注窗口的初始化
	pTextDlg = nullptr;   //窗口指针现在为空
	m_FontHeight = 20;
	m_FontWide = 10;
	m_TextAngle = 0;
	m_FontAngle = 0;
	m_FontBetween = 1;
	m_TextString.Empty();
	m_Text1 = new CC_Brother_Text(m_penColor, m_brushColor, m_lineWide, m_lineType, m_curLayer, 0, 0, 100, 100, 0, 0,
		m_FontHeight, m_FontWide, m_FontBetween, 0, m_TextString);

	PointXyz = new PointStruct[1000];
	GraphUndo = new GraphSelectStruct[10];
	p_Screen = new ScreenStructForBoost[1000];
}

CCCBrotherDrawView::~CCCBrotherDrawView()
{
	delete[] PointXyz;
	delete m_Text1;
	delete[] GraphUndo;
//	delete[] p_Screen;             //内存不能被重复释放
}

BOOL CCCBrotherDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	cs.style = cs.style | WS_HSCROLL | WS_VSCROLL;
	return CView::PreCreateWindow(cs);
}

void CCCBrotherDrawView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	if (pInfo == NULL)  //如果是屏幕显示
		return;
	//当前打印页所在的行
	pDC->SetMapMode(m_MapMode);
	//int nRow = pInfo->m_nCurPage / nPageX + (pInfo->m_nCurPage%nPageX);
	int nRow = (pInfo->m_nCurPage - 1) / nPageX + 1;
	//当前打印页所在的列
	int nCol = (pInfo->m_nCurPage - 1) % nPageX + 1;
	m_xStart = xLeft + (nCol - 1)*xPage;  //当前页输出的图形左下角的实际横坐标
	m_yStart = yTop - nRow * yPage;   //当前页输出的图形左下角的实际纵坐标
	pDC->SetWindowOrg(-100/*0*/, /*0*/(m_hScreen) / 3);/*pInfo->m_rectDraw.top - pInfo->m_rectDraw.bottom*/  /*pInfo->m_rectDraw.right - pInfo->m_rectDraw.left*/
	//pDC->SetWindowOrg(0, 0);
	CView::OnPrepareDC(pDC, pInfo);
}

// CCCBrotherDrawView 绘图

void CCCBrotherDrawView::OnDraw(CDC* pDC)
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	DrawBakcground(pDC);
	//m_MapMode = pDC->GetMapMode();
	pDoc->ReDraw(pDC, 0, 0, m_backgroundColor);
	// TODO: 在此处为本机数据添加绘制代码
}

void CCCBrotherDrawView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CCCBrotherDrawDoc* pDoc = GetDocument();
	m_pSelection = NULL;    // initialize selection
	//p_View = this;
	//m_pLinkSet = &pDoc->m_linkdata1;

	//CSize sizeTotal;
	//// TODO: 计算此视图的合计大小
	//sizeTotal.cx = sizeTotal.cy = 1000;
	//SetScrollSizes(MM_TEXT, sizeTotal);
}

// CCCBrotherDrawView 打印

BOOL CCCBrotherDrawView::OnPreparePrinting(CPrintInfo* pInfo)
{
	//pInfo->SetMinPage(1);
	//pInfo->SetMaxPage(10);
	if (!firstPrinting)
	{
		MessageBox(L"The first Printing, automatically calling default functions to redraw graph to fill in all the board!\nIf you wish higher effects, please enlarge the graph manually.");
		firstPrinting++;
		OnGraphAll();
	}
	return DoPreparePrinting(pInfo);
}

void CCCBrotherDrawView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CPoint point;
	int mPageHeight, mPageWidth, nPage;
	m_MapMode = 2;
	pDC->SetMapMode(m_MapMode);
	//整个图形左下角的坐标
	xLeft = p_Screen[0].startX;
	yBottom = p_Screen[0].startY;
	//整个图形右上角的坐标为
	xRight = xLeft + p_Screen[0].blc*m_wScreen;
	yTop = yBottom + p_Screen[0].blc*m_hScreen;
	//图形的横向和纵向的幅度为
	xWide = xRight - xLeft;
	yHigh = yTop - yBottom;
	point.y=pDC->GetDeviceCaps(VERTRES);  //得到设备竖直方向的像素数
	point.x=pDC->GetDeviceCaps(HORZRES);   //得到设备水平方向的像素数
	pDC->DPtoLP(&point);
	pDC->DPtoLP(&(pInfo->m_rectDraw));
	mPageHeight = abs(/*pInfo->m_rectDraw.top - pInfo->m_rectDraw.bottom*/point.x);
	mPageWidth = abs(/*pInfo->m_rectDraw.right - pInfo->m_rectDraw.left*/point.y);
	//每页的横向和纵向大小
	xPage = blc * mPageWidth;                     //每页的实际宽度
	yPage = blc * mPageHeight;                    //每页的实际高度
	nPageX = (int)(xWide / xPage) + (xWide > xPage*(int)(xWide / xPage));//横向页数
	nPageY = (int)(yHigh / yPage) + (yHigh > yPage*(int)(yHigh / yPage));//纵向页数
	nPage = nPageX * nPageY;       //总页数
	pInfo->SetMinPage(1);      //设置最小页号
	pInfo->SetMaxPage(nPage);  //设置最大页号
	m_hScreen1 = m_hScreen;
	m_wScreen1 = m_wScreen;
	m_hScreen = mPageHeight;    //显示窗口的高度
	m_wScreen = mPageWidth;     //显示窗口的宽度
}

void CCCBrotherDrawView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	CCCBrotherDrawDoc *pDoc = (CCCBrotherDrawDoc *)GetDocument();
	//恢复屏幕显示的参数
	m_xStart = p_Screen[pDoc->m_CurrentScreen].startX;
	m_yStart = p_Screen[pDoc->m_CurrentScreen].startY;
	blc = p_Screen[pDoc->m_CurrentScreen].blc;
	// TODO: add cleanup after printing
	m_hScreen = m_hScreen1;
	m_wScreen = m_wScreen1;
	m_MapMode = 1;
}

void CCCBrotherDrawView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	/*char p1[4];
	CString strHeader = GetDocument()->GetTitle();
	sprintf_s(p1, "%d", pInfo->m_nCurPage);
	strHeader += " 第";
	strHeader += p1;
	strHeader += "页";
	POINT point;*/
	m_MapMode = pDC->GetMapMode();
	//point.x = -100; point.y = m_hScreen / 2;    //原点(-100，m_hScreen / 2)
	//pDC->DPtoLP(&point);
	//pDC->SetTextAlign(TA_LEFT/*TA_CENTER*/);
	//pDC->TextOut(0, point.y, strHeader);
	//TEXTMETRIC textMetric;
	//pDC->GetTextMetrics(&textMetric);
	//int y;
	//if (m_MapMode == 1)
	//	y = point.y + textMetric.tmHeight;
	//else
	//	y = point.y - textMetric.tmHeight;
	//pDC->MoveTo(0, y);
	//pDC->LineTo(pInfo->m_rectDraw.right, y);

	CBrush brush(/*p_GraphPara->GetColor*/(m_backgroundColor));
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->PatBlt(0,/*0*/-(m_hScreen) / 3, m_wScreen - 1100, m_hScreen - 800, PATCOPY);
	pDC->SelectObject(pOldBrush);

	CCCBrotherDrawDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	pDoc->ReDraw(pDC, 0, 0, m_backgroundColor);
	/*OnDraw(pDC);*/
}

//添加的自定义函数
void  CCCBrotherDrawView::DeviceP_To_LogicalP(float x, float y, int *X, int *Y)
{
	*X = (int)((x - m_xStart) / blc);
	if (m_MapMode == 1)
		*Y = m_hScreen - (int)((y - m_yStart) / blc);
	else
		*Y = (int)((y - m_yStart) / blc) - m_hScreen;
}

void  CCCBrotherDrawView::LogicalP_To_DeviceP(int x, int y, float *X, float*Y)
{
	*X = m_xStart + x * blc;
	if (m_MapMode == 1)
		*Y = m_yStart + blc * (m_hScreen - y);
	else
		*Y = m_yStart + blc * (m_hScreen + y);
}

int   CCCBrotherDrawView::DeviceL_To_LogicalL(float length)
{
	return int(length / blc);
}

float CCCBrotherDrawView::LogicalL_To_DeviceL(int length)
{
	return blc * length;
}

void CCCBrotherDrawView::DrawBakcground(CDC* pDC)
{
	CBrush brush(/*p_GraphPara->GetColor*/(m_backgroundColor));
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->PatBlt(0,0,m_wScreen, m_hScreen,PATCOPY);
	pDC->SelectObject(pOldBrush);
}

void CCCBrotherDrawView::InitVScroll()      //初始化Vertical滚动条
{
	nVScrollMax = (int)(((float)m_hScreen*p_Screen[0].blc) / (blc * nScrollMin));//得到滚动条的最大范围
	nVScrollPos = nVScrollMax - (int)((m_yStart - p_Screen[0].startY) / (blc*nScrollMin));
	SetScrollRange(SB_VERT, 0, nVScrollMax, TRUE);
	SetScrollPos(SB_VERT, nVScrollPos);
	UpdateWindow();
}

void CCCBrotherDrawView::InitHScroll()      //初始化Horizonal滚动条
{
	nHScrollMax = (int)(((float)m_wScreen*p_Screen[0].blc) / (blc * nScrollMin));
	nHScrollPos = /*nHScrollMax - */(int)((m_xStart - p_Screen[0].startX) / (blc*nScrollMin));
	SetScrollRange(SB_HORZ, 0, nHScrollMax, TRUE);
	SetScrollPos(SB_HORZ, nHScrollPos);
	UpdateWindow();
}

void CCCBrotherDrawView::Delete(CDC* pDC, int Lb, int Index)
{
	float x1, y1, x2, y2;
	CCCBrotherDrawDoc* pDoc = GetDocument();
	if (Lb == 1)	//如果是直线，得到直线指针并以屏幕底色重画达到删除的效果
	{
		CC_Brother_Line* p_Line = pDoc->GetLine(Index);
		p_Line->DrawShape(pDC, 0, 2, m_backgroundColor);
		p_Line->Delete(1);	//做删除标识
	}
	else if (Lb == 4)	//如果是连续直线或多边形
	{
		CC_Brother_PLine* p_PLine = pDoc->GetPLine(Index);	//得到连续直线（多边形）的指针
		if (p_PLine->IsPLine()) //如果是连续直线，以屏幕底色重画达到删除的效果
			p_PLine->DrawShape(pDC, 0, 2, m_backgroundColor);
		else		//如果是多边形区域
		{
			p_PLine->GetRect(&x1, &y1, &x2, &y2);	//得到多边形区域的边界矩形
			ReDrawRect(x1, y1, x2, y2);			//重画多边形区域所在顶区域
		}
		p_PLine->Delete(1);	//做删除标识
	}
	else if (Lb == 3)	//如果是开关
	{
		CC_Brother_Switch* p_Switch = pDoc->GetSwitch(Index);//得到开关的指针
		p_Switch->GetRect(&x1, &y1, &x2, &y2);
		ReDrawRect(x1, y1, x2, y2);    //局部重画有边缘印记的BUG
		//ReDrawRect(x1, y1 - 1, x2, y2);     //BUG修复
		p_Switch->Delete(1);	//做删除标识
	}
	else if (Lb == 2)	//如果是标注文字，则得到其边界矩形并重画这个区域
	{
		CC_Brother_Text* p_Text = pDoc->GetText(Index);	//得到标注文字的指针
		p_Text->GetRect(&x1, &y1, &x2, &y2);
		p_Text->Delete(1);		//做删除标识
		ReDrawRect(x1, y1, x2, y2);//重画屏幕区域
	}
	else if (Lb == 6)
	{
		CC_Brother_Ground* pGround = pDoc->GetGround(Index);
		pGround->GetRect(&x1, &y1, &x2, &y2);
		pGround->Delete(1);
		ReDrawRect(x1, y1, x2, y2);//重画屏幕区域
	}
	//删除图形操作
}

//此函数用来重画由点（X1,Y1)和点(X2,Y2)确定的区域
void CCCBrotherDrawView::ReDrawRect(float X1, float Y1, float X2, float Y2)
{
	float xx1, xx2, yy1, yy2;
	CRect r1;
	xx1 = m_xStart; xx2 = m_xStart + blc * m_wScreen;
	yy1 = m_yStart; yy2 = m_yStart + blc * m_hScreen;
	//得到区域与视图区域相交的矩形
	BOOL IsCross = RectCross(&xx1, &yy1, &xx2, &yy2, X1, Y1, X2, Y2);
	if (IsCross)	//如果相交，则重画这一区域
	{
		//以下得到这个区域顶像素坐标
		r1.left = (int)((xx1 - m_xStart) / blc) - 1;
		r1.right = (int)((xx2 - m_xStart) / blc) + 1;
		r1.top = m_hScreen - (int)((yy2 - m_yStart) / blc) - 1;
		r1.bottom = m_hScreen - (int)((yy1 - m_yStart) / blc) + 1;
		InvalidateRect(r1);
	}
}

//此函数用来计算由(*x1,*y1),(*x2,*y2)和(xx1,yy1),(xx2,yy2)决定的两个区域的相交区域
//返回：TURE，两区域相交，相交区域由(*x1,*y1),(*x2,*y2)决定
BOOL CCCBrotherDrawView::RectCross(float* x1, float* y1, float* x2, float* y2, float xx1,
	float yy1, float xx2, float yy2)
{
	float m_X1, m_Y1, m_X2, m_Y2;
	m_X1 = *x1; m_Y1 = *y1; m_X2 = *x2; m_Y2 = *y2;
	if (m_X1 > xx2 || m_X2 < xx1 || m_Y1 > yy2 || m_Y2 < yy1) //两个矩形区域不相交
		return FALSE;	//如不相交函数返回0
	else	//两个矩形相交，得到相交矩形的坐标
	{
		*x1 = max(m_X1, xx1);
		*y1 = max(m_Y1, yy1);
		*x2 = min(m_X2, xx2);
		*y2 = min(m_Y2, yy2);
		return TRUE; //如果相交就返回1
	}
}

//重载父类虚函数
void CCCBrotherDrawView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactivateView)
{
	//CCCBrotherDrawDoc *pDoc = GetDocument();
	//p_Screen = pDoc->getScreen();
	ptr_View = this;
	CView::OnActivateView(bActivate, pActivateView, pDeactivateView);
}


//自定义的一些消息处理函数
void CCCBrotherDrawView::OnSize(UINT nType,int cx, int cy)
{
	CCCBrotherDrawDoc *pDoc = GetDocument();
	p_Screen = pDoc->m_Screen;

	m_wScreen = cx;
	m_hScreen = cy;//获取屏幕的长和宽

	nXPage = m_hScreen / nScrollMin;
	nYPage = m_wScreen / nScrollMin;//横向和纵向滚动的总次数

	InitHScroll();
	InitVScroll();

	CView::OnSize(nType, cx, cy);
}

void CCCBrotherDrawView::OnMouseMove(UINT nFlags, CPoint point)//关于为什么线一直都是黑色的  可以设置一个全局的dc对象，每次画完以后擦除这个橡皮筋就可以实现擦除的目的了
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	CClientDC m_dc(this);
	CPen pen(0, (int)1/blc, RGB(0,0,0));
	/*CPen* pOldPen = */m_dc.SelectObject(&pen);
	m_dc.SetROP2(R2_NOT);
	int Lb;
	float x, y, x1, y1;
	if (m_currentShape == 1 && LButtonPushNum == 1)
	{
		if (m_oldPoint != point)
		{
			m_dc.MoveTo(m_originalPoint);
			m_dc.LineTo(m_oldPoint);   //这里是在擦除上一条直线，因为设置绘制模式为R2_NOT 意味着画笔是与背景色取反，画两次就刚好取反两次，还原成为了背景色
			m_dc.MoveTo(m_originalPoint);
			m_dc.LineTo(point);
			m_oldPoint = point;
		}
		//m_dc.MoveTo(m_oldPoint);
		//m_dc.LineTo(point); //如果只有这两句的话会有神奇的效果出现
	}
	else if (m_currentShape == 3)
	{
		if (startDraw)
		{
			SetCapture();
			if (!LButtonPushNum)
			{
				Draw_Tmp_Switch(&m_dc, &pen, point, sizeOfSwitch);
			}
			else if (m_oldPoint != point)
			{
				Draw_Tmp_Switch(&m_dc, &pen, m_oldPoint, sizeOfSwitch);
				Draw_Tmp_Switch(&m_dc, &pen, point, sizeOfSwitch);
			}
			m_oldPoint = point;
			LButtonPushNum = 1;   //这里可以写成LButtonPushNum++  只是考虑到鼠标移动的消息可能有很多 到时候这个数字递增可能会溢出  所以这样子写
		}
	}
	else if ((m_currentShape == 4 || m_currentShape == 5) && LButtonPushNum >= 1)       //连续直线
	{
		if (m_oldPoint != point)
		{
			m_dc.MoveTo(m_originalPoint);
			m_dc.LineTo(m_oldPoint);
			m_dc.MoveTo(m_originalPoint);
			m_dc.LineTo(point);
			m_oldPoint = point;
		}
	}
	else if (m_currentShape == 28)                  //进行图形移动
	{
		if (SelectedGraph&&LButtonPushNum == 2)
		{
			if (m_oldPoint != point)
			{
				Lb = pDoc->GraphSelect[0].Lb;
				switch (Lb)
				{
				case 1:                //直线

					break;
				case 2:                //文本
					break;
				case 3:                //开关
				{
					LogicalP_To_DeviceP(point.x, point.y, &x, &y);
					LogicalP_To_DeviceP(m_oldPoint.x, m_oldPoint.y, &x1, &y1);
					((CC_Brother_Switch*)pDoc->GetGraph(3, pDoc->GraphSelect[0].Index))->setPointInc(x - x1, y - y1);
					break;
				}
				case 4:                //连续直线
					break;
				default:
					break;
				}
				m_oldPoint = point;
				Invalidate();
			}
		}
	}
	else if (m_currentShape == 29 && LButtonPushNum == 1)     //对两个元器件之间进行连线
	{
		if (m_oldPoint != point)
		{
			m_dc.MoveTo(m_originalPoint);
			m_dc.LineTo(m_oldPoint);   //这里是在擦除上一条直线，因为设置绘制模式为R2_NOT 意味着画笔是与背景色取反，画两次就刚好取反两次，还原成为了背景色
			m_dc.MoveTo(m_originalPoint);
			m_dc.LineTo(point);
			m_oldPoint = point;
		}
	}
	CView::OnMouseMove(nFlags, point);
}

//鼠标左键按下  该函数响应
void CCCBrotherDrawView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CCCBrotherDrawDoc* pDoc = GetDocument();  //获取文档对象
	CClientDC dc(this);
	//float r; //画圆用到
	float xx1, yy1, xx2, yy2;
	int /*x1, y1, x2, y2,*/ Lb, Index, id_only;
   	//bool m_fill;  //是否填充                 //这里注释的一些变量是可以为实现拓展功能使用的
	static bool selected1 = 0, selected2 = 0;             //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!static!!!!!!!!!!!!!!!!!!!!!!!!!
	float validDistance = blc * 4;              //有效距离是4个逻辑单元   当然也可以改成其他的合理距离
	if (m_currentShape == 0)
	{
		//MessageBox(_T("请选择一个需要画的图形!"));
		pDoc->ClearGraphSelect();
		Invalidate();
		UpdateWindow();
	}
	else if (m_currentShape == 1)          //当前图形为直线
	{
			LButtonPushNum++;
			m_oldPoint = point;
			m_originalPoint = point;
			SetCapture();         //捕捉鼠标  将鼠标的活动空间设置为当前文本框 并且时刻捕捉其发送的消息
	}
	else if (m_currentShape == 2)         //当前图形为文本
	{
		LogicalP_To_DeviceP(point.x, point.y, &xx1, &yy1);
		m_Text1->setPoint(xx1, yy1);
		if (!pTextDlg)  //如果对象不存在  也就是还是空指针
		{
			pTextDlg = new CC_Brother_TextDlg(this,m_TextString);
			ASSERT(pTextDlg);
		}
		if (!(pTextDlg->IsOpen()))  //打开。。。。。。 判断句柄资源是否存在
		{
			pTextDlg->Create(IDD_WRITE_TEXT,this);
		}
		if (!(pTextDlg->IsVisible())) //判断是否可视  不可视就让窗口可视化
		{
			pTextDlg->ShowWindow(SW_SHOW);
		}
		pTextDlg->SetWindowTextW(_T("标注文本输入对话框"));
	}
	else if (m_currentShape == 3&&LButtonPushNum==1)   //当前画开关   左键已经按下一次
	{
		LogicalP_To_DeviceP(point.x, point.y, &xx1, &yy1);
		id_only = pDoc->GetGraphID(3);
		pDoc->AddSwitch(m_penColor, m_brushColor, m_lineWide, m_lineType, m_curLayer, id_only, xx1, yy1)->DrawShape(&dc, 0, 0, m_backgroundColor);
		ReleaseCapture();
		startDraw = false;
		finishDraw = true;
		m_currentShape = 0;
		Invalidate();    //全部重画
		UpdateWindow();  //刷新窗口
		GraphUndo[0].Index = pDoc->GetGraphUpperBound(3);
		GraphUndo[0].Lb = 3;
		pDoc->AddUndo(2, 1, GraphUndo);
	}
	else if (m_currentShape == 4 || m_currentShape ==5)         //连续直线或者多边形
	{
		LogicalP_To_DeviceP(point.x, point.y, &xx1, &yy1);
		PointXyz[LButtonPushNum].x = xx1;
		PointXyz[LButtonPushNum].y = yy1;
		if (LButtonPushNum == 0)
		{
			SetCapture();
			m_originalPoint = point;
			m_oldPoint = point;
		}
		else
		{
			m_originalPoint = m_oldPoint;
			m_oldPoint = point;
		}
		LButtonPushNum++;
	}
	else if (m_currentShape == 6)
	{
		LogicalP_To_DeviceP(point.x, point.y, &xx1, &yy1);
		id_only = pDoc->GetGraphID(6);
		pDoc->AddGround(m_penColor, m_brushColor, m_lineWide, m_lineType, m_curLayer, id_only, xx1, yy1)->DrawShape(&dc, 0, 0, m_backgroundColor);
		Invalidate();    //全部重画
		UpdateWindow();  //刷新窗口
		m_currentShape = 0;
		/*还可以添加一下撤销操作*/
		GraphUndo[0].Index = pDoc->GetGraphUpperBound(6);
		GraphUndo[0].Lb = 6;
		pDoc->AddUndo(2, 1, GraphUndo);
	}
	else if (m_currentShape == 28)   //鼠标选中图形 进行移动
	{
		LogicalP_To_DeviceP(point.x, point.y, &xx1, &yy1);
		if (pDoc->PointSelect(xx1, yy1, validDistance, blc, &Lb, &Index, &id_only))
		{
			pDoc->DrawSelectedGraph(&dc, Lb, Index, 0, 1, m_backgroundColor);
			pDoc->AddSelectList(Lb, Index, id_only);
			if (LButtonPushNum == 1 && pDoc->n_GraphSelect == 1)
			{
				LButtonPushNum++;
				SetCapture();
				m_oldPoint = point;
				//SetCursorPos(point.x, point.y);
			}
			else if (LButtonPushNum == 0)
			{
				SelectedGraph = true;
				LButtonPushNum++;
			}
			else
			{
				LButtonPushNum = 1;
				pDoc->ClearGraphSelect();
				pDoc->AddSelectList(Lb, Index, id_only);
				Invalidate();
			}
		}
		else
		{
			if (SelectedGraph)
			{
				pDoc->ClearGraphSelect();
				LButtonPushNum = 0;
				SelectedGraph = false;
				Invalidate();
			}
		}
	}
	else if(m_currentShape==29)       //对两个元器件之间进行连线
	{
		if (LButtonPushNum == 0)
		{
			LButtonPushNum++;
			m_oldPoint = point;
			m_originalPoint = point;
			LogicalP_To_DeviceP(point.x, point.y, &xx1, &yy1);
			selected1 = pDoc->PointSelect(xx1, yy1, validDistance, blc, &Lb, &Index, &id_only);
			if (selected1)
			{
				pDoc->DrawSelectedGraph(&dc, Lb, Index, 0, 1, m_backgroundColor);
				bool pb1 = pDoc->AddSelectList(Lb, Index, id_only);
			/*	if (pb1)
					pDoc->DrawSelectedGraph(&dc, Lb, Index, 0, 1, m_backgroundColor);*/
			}
			SetCapture();
		}
		else if (LButtonPushNum == 1)
		{
			LogicalP_To_DeviceP(point.x, point.y, &xx1, &yy1);
			selected2 = pDoc->PointSelect(xx1, yy1, validDistance, blc, &Lb, &Index, &id_only);
			if (selected2)
			{
				pDoc->DrawSelectedGraph(&dc, Lb, Index, 0, 1, m_backgroundColor);
				bool pb1 = pDoc->AddSelectList(Lb, Index, id_only);
				//if (pb1)
				//	pDoc->DrawSelectedGraph(&dc, Lb, Index, 0, 1, m_backgroundColor);
			}	
			if (selected1&&selected2)
			{
				DrawRegularLineBetweenTwoComponents(pDoc,0,&dc, 0, 0, m_backgroundColor);  //画完之后需要清空保存在GraphSelect中的图形
			}
			else
			{
				pDoc->ClearGraphSelect();
			}          //这里还可以再改改 每次可多选
			Invalidate();
			UpdateWindow();
			LButtonPushNum = 0;
			ReleaseCapture();
		}
	}
	else if (m_currentShape == 30)        //对图形进行框选
	{
		if (pDoc->n_GraphSelect == 1000)
		{
			MessageBox(_T("最多只能选择1000个图形元素"));  
			return;
		}
		LogicalP_To_DeviceP(point.x, point.y, &xx1, &yy1);
		bool pb = pDoc->PointSelect(xx1, yy1, validDistance, blc, &Lb, &Index, &id_only);
		if (pb)
		{
			bool pb1 = pDoc->AddSelectList(Lb, Index, id_only);
			if (pb1)
				pDoc->DrawSelectedGraph(&dc, Lb, Index, 0, 1, m_backgroundColor);
			Invalidate();
			UpdateWindow();
		}
	}
	else
	{
	MessageBox(_T("敬请期待！！！"));
	}
	CView::OnLButtonDown(nFlags, point);
}

void CCCBrotherDrawView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	CClientDC dc(this);
	float xx1, yy1, xx2, yy2;
	int id_only;
	if (m_currentShape == 1 && LButtonPushNum == 1)
	{
			LogicalP_To_DeviceP(m_originalPoint.x, m_originalPoint.y, &xx1, &yy1);
			LogicalP_To_DeviceP(point.x, point.y, &xx2, &yy2);
			id_only = pDoc->GetGraphID(1);

			pDoc->AddLine(m_penColor, m_brushColor, m_lineWide, m_lineType, m_curLayer, id_only, xx1, yy1, xx2, yy2)->DrawShape(&dc, 0, 0, m_backgroundColor);
			LButtonPushNum = 0;
			ReleaseCapture();    //释放对鼠标的捕捉
			Invalidate();    //全部重画
			UpdateWindow();  //刷新窗口
			GraphUndo[0].Index = pDoc->GetGraphUpperBound(1);//得到此直线的在m_lineArray中的下标。因为是最新添加的 所以会是数组的最大有效下标
			GraphUndo[0].Lb = 1;	//直线
			pDoc->AddUndo(2, 1, GraphUndo);
	}
	else if (m_currentShape == 28)      //结束图形的移动
	{
		if (SelectedGraph&&LButtonPushNum == 2)
		{
			pDoc->ClearGraphSelect();
			Invalidate();    //全部重画
			UpdateWindow();  //刷新窗口
			LButtonPushNum = 0;
			SelectedGraph = false;
			ReleaseCapture();
		}
	}
	CView::OnLButtonUp(nFlags, point);
}

//鼠标右键按下  该函数响应
void CCCBrotherDrawView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	CClientDC dc(this);
	CPen pen(0, 0, RGB(0, 0, 0));
	CPen* pOldPen = dc.SelectObject(&pen);//保存旧的画笔
	int id_only;
	dc.SetROP2(R2_NOT);                   //设置画板背景模式
	if (m_currentShape == 1 && LButtonPushNum == 1)
	{
		dc.MoveTo(m_originalPoint);
		dc.LineTo(point);     //画直线  作用是擦除原来的直线
		LButtonPushNum = 0;
		ReleaseCapture();
	}
	else if (m_currentShape==3 && startDraw == true && finishDraw == false)
	{
		Draw_Tmp_Switch(&dc, &pen, point, sizeOfSwitch);        //零时图形（开关）的绘制  因为可能需要撤销

		LButtonPushNum = 0;
		ReleaseCapture();

		startDraw = false;
		finishDraw = true;
	}
	else if ((m_currentShape == 4 || m_currentShape == 5) && LButtonPushNum >= 1)
	{
		dc.MoveTo(m_originalPoint);
		dc.LineTo(point);     //画直线 作用是擦除原来的直线
		if (m_currentShape == 4 && LButtonPushNum > 2)
		{
			id_only = pDoc->GetGraphID(4);
			pDoc->AddPLine(m_penColor, m_brushColor, m_lineWide, m_lineType, m_curLayer, id_only, LButtonPushNum, PointXyz, 0)->DrawShape(&dc,0,0,m_backgroundColor);
			ReleaseCapture();
			GraphUndo[0].Index = pDoc->GetGraphUpperBound(4);
			GraphUndo[0].Lb = 4;
			pDoc->AddUndo(2, 1, GraphUndo);
		}
		else if (m_currentShape == 5 && LButtonPushNum > 1)
		{
			//敬请期待！！！
		}
		LButtonPushNum = 0;
		ReleaseCapture();
		Invalidate();    //全部重画
		UpdateWindow();  //刷新窗口
	}
	dc.SelectObject(pOldPen);
	CView::OnRButtonDown(nFlags, point);
}


//Line按钮按下  该函数响应
void CCCBrotherDrawView::OnDrawLine()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	pDoc->ClearGraphSelect();    //选中的图形初始化为0个  即使之前有选中的图形了 因为下面要进行连线操作。
	LButtonPushNum = 0;
	m_currentShape = 1;
}

//PLine按钮按下  该函数响应
void CCCBrotherDrawView::OnDrawPLine()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	pDoc->ClearGraphSelect();    //选中的图形初始化为0个  即使之前有选中的图形了 因为下面要进行连线操作。

	LButtonPushNum = 0;
	m_currentShape = 29;

	startDraw = true;
	finishDraw = false;
}

void CCCBrotherDrawView::OnDrawPPLine()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	pDoc->ClearGraphSelect();    //选中的图形初始化为0个  即使之前有选中的图形了 因为下面要进行连线操作。
	LButtonPushNum = 0;
	m_currentShape = 4;
}

//文本按钮按下  该函数响应
void CCCBrotherDrawView::OnDrawText()
{
	//LButtonPushNum = 0;
	CCCBrotherDrawDoc* pDoc = GetDocument();
	pDoc->ClearGraphSelect();    //选中的图形初始化为0个  即使之前有选中的图形了 因为下面要进行连线操作。
	m_currentShape = 2;
}

//开关按钮按下  该函数响应
void CCCBrotherDrawView::OnDrawSwitch()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	pDoc->ClearGraphSelect();    //选中的图形初始化为0个  即使之前有选中的图形了 因为下面要进行连线操作。

	LButtonPushNum = 0;
	m_currentShape = 3;

	startDraw = true;
	finishDraw = false;
}

void CCCBrotherDrawView::OnDrawGround()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	pDoc->ClearGraphSelect();    //选中的图形初始化为0个  即使之前有选中的图形了 因为下面要进行连线操作。

	LButtonPushNum = 0;
	m_currentShape = 6;

	startDraw = true;
	finishDraw = false;
}

//文本框内容发生变化的消息响应函数 
void CCCBrotherDrawView::EditText()
{
	Invalidate();
	UpdateWindow();  //更新视图 全部重画
	CClientDC dc(this);
	if (m_currentShape == 100)
	{
	}
	else if (m_currentShape == 99)
	{
	}
	else if (m_currentShape == 98)
	{
	}
	else
	{
		m_Text1->setText(pTextDlg->m_Text);     //设置文本框内容
		m_Text1->DrawShape(&dc, 0, 0, m_backgroundColor);    //画图
	}
}

//对话框的ok按钮按下  该函数响应  添加一个文本对象并保存起来  并且将其显示在画板上
void CCCBrotherDrawView::DlgOK()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();   //获取文档对象
	CClientDC dc(this);                        //画板资源
	int TextLength; 
	m_TextString = pTextDlg->m_Text;           //获取文本内容
	TextLength = m_TextString.GetLength();     //获取文本长度
	if (TextLength > 0) 
	{
		if (m_currentShape == 100)
		{
			int layrenum = _ttoi(m_TextString);
			if (layrenum < 0 || layrenum >= 1000)
			{
				MessageBox(_T("非法图层"));
			}
			else
			{
				(pDoc->m_GraphPara).m_LayerList[m_curLayer].b_Display = 0;
				(pDoc->m_GraphPara).m_LayerList[layrenum].b_Display = 1;
				m_curLayer = layrenum;
				m_Text1->setCurLayre(m_curLayer);   //这一行就是解 图层功能紊乱BUG的
			}
			m_currentShape = 0;
			Invalidate();
		}
		else if (m_currentShape == 99)
		{
			int linetype = _ttoi(m_TextString);
			if (linetype < 0 || linetype > 4)
			{
				MessageBox(_T("非法线型"));
			}
			else
			{
				m_lineType = linetype;
			}
			m_currentShape = 0;
		}
		else if (m_currentShape == 98)
		{
			int linewide = _ttoi(m_TextString);
			if (linewide < 0 || linewide > 14)
			{
				MessageBox(_T("非法线型"));
			}
			else
			{
				m_lineWide = linewide;
			}
			m_currentShape = 0;
		}
		else
		{
			int id_only = pDoc->GetGraphID(2);     //申请获取图形标识符
			m_Text1->prepare(id_only, m_penColor/*, m_curLayer*/);
			//m_Text1->setID(id_only);               //设置图形标识符
			//m_Text1->updateColor(m_penColor);
			pDoc->AddText1(m_Text1)->DrawShape(&dc, 0, 0, m_backgroundColor);
			//pTextDlg->m_Text.Empty();
			m_Text1 = new CC_Brother_Text(m_penColor, m_brushColor, m_lineWide, m_lineType, m_curLayer, 0, 0, 100, 100, 0, 0,
				m_FontHeight, m_FontWide, m_FontBetween, 0, _T(""));
			//pTextDlg->SendMessage(WM_INITDIALOG);    //发消息调用对话框的OnInitialDialog函数
			GraphUndo[0].Index = pDoc->GetGraphUpperBound(2);//此标注文字的位置	
			GraphUndo[0].Lb = 2;	//标注文字
			pDoc->AddUndo(2, 1, GraphUndo);
		}
	}
	pTextDlg->m_Text.Empty();
	pTextDlg->SendMessage(WM_INITDIALOG);    //发消息调用对话框的OnInitialDialog函数
}

//对话框的取消按钮按下 该函数响应 清空文本框内容  视图刷新 全部重画  发消息给资源框
void CCCBrotherDrawView::DlgCancel()
{
	pTextDlg->m_Text.Empty();  //文本框内容初始化为空
	Invalidate();
	UpdateWindow();  //刷新视图 全部重画
	pTextDlg->SendMessage(WM_INITDIALOG);     //取消对话款的文本编辑 向对话框发送消息 有相应的消息响应函数响应消息
	m_currentShape = 0;
}

//放大按钮按下 该函数响应
void CCCBrotherDrawView::OnEnLarge()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();

	InitHScroll();
	InitVScroll();//初始化滚动条
	
	float minx, miny, maxx, maxy;
	if (blc > 0.3)
	{
		blc -= 0.05;
		bool pb = pDoc->GetRect(&minx, &miny, &maxx, &maxy);
		//m_xStart += 8/*7.3*/;   //找一个函数关系 维持视图时钟居中
		//m_yStart += 8/*7.3*/;
		m_xStart = minx - 10;
		m_yStart = miny - 10;
		Invalidate();
		UpdateWindow();    //改变比例尺以及 原点坐标  刷新视图 全部重画
	}

	p_Screen[0].startX = m_xStart;
	p_Screen[0].startY = m_yStart;
	p_Screen[0].blc = blc;
}

void CCCBrotherDrawView::OnShrink()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();

	InitHScroll();
	InitVScroll();//初始化滚动条

	float minx, miny, maxx, maxy;
	if (blc < 1)
	{
		blc += 0.05;
		Invalidate();
		UpdateWindow();    //改变比例尺以及 原点坐标  刷新视图 全部重画
	}
	else
	{
		OnRecovery();
		/*m_xStart = 0;
		m_yStart = 0;
		blc = 1;*/
	}
}

//一键还原按钮按下 该函数响应  清空所选择的图形 设置原点以及比例尺  更新视图
void CCCBrotherDrawView::OnRecovery()
{
	p_Screen[0].startX = m_xStart;
	p_Screen[0].startY = m_yStart;
	p_Screen[0].blc = 1;
	InitHScroll();
	InitVScroll();//初始化滚动条

	CCCBrotherDrawDoc* pDoc = GetDocument();
	pDoc->ClearGraphSelect();         

	m_xStart = 0.0, m_yStart = 0.0;	//可能对p_Screen也要进行处理
	blc = 1.0;
	InitHScroll();
	InitVScroll();
	Invalidate();
	UpdateWindow();
}

void CCCBrotherDrawView::OnGraphAll()      //显示全屏
{
	float minx, miny, maxx, maxy, bl1;
	CCCBrotherDrawDoc* pDoc = GetDocument();
	//捕捉光标，并使光标成沙漏等待光标
	//把最小值给非常大，把最大值给非常小，为了对变量进行初试化
	minx = (float)1E+20; miny = (float)1E+20; maxx = -(float)1E+20; maxy = -(float)1E+20;
	SetCapture();
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	bool pb = pDoc->GetRect(&minx, &miny, &maxx, &maxy);
	//将光标变为箭头形，放弃捕捉的光标
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	ReleaseCapture();
	if (!pb)		//没有图形元素,程序返回
		return;
	bl1 = (maxx - minx) / (m_wScreen - 20);//横向的比例
	blc = (maxy - miny) / (m_hScreen - 20);//实际纵向比例
	if (bl1 > blc)blc = bl1;  //取较大的一个作为比例尺
	/*以下确定屏幕左下角的实际坐标，即在左侧和下侧都留了10点阵的区域
	（minx,miny)在屏幕的左下角向右10点阵同时向上10点阵处。*/
	m_xStart = minx - 10 * blc;
	m_yStart = miny - 10 * blc;
	//将全图的显示比例放在首屏，并把屏幕设为首屏
	p_Screen[0].startX = m_xStart;
	p_Screen[0].startY = m_yStart;
	p_Screen[0].blc = blc;
	pDoc->m_CurrentScreen = 0;						//对p_Screen的改变就是对m_Screen的改变，因为两者指向同一物理地址
	InitVScroll(); 
	InitHScroll();
	Invalidate();
}

void CCCBrotherDrawView::OnShift()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	m_currentShape = 28;
	LButtonPushNum = 0;
	pDoc->ClearGraphSelect();
	SelectedGraph = false;
	//SetCapture();
	//MessageBox(_T("图形移动功能 敬请期待！"));
}

//鼠标垫选按钮按下 该函数响应 
void CCCBrotherDrawView::OnSelectMouse()
{
	m_currentShape = 30;
	LButtonPushNum = 0;
	//m_currentShape = 1;    //测试消息函数是否响应
}


void CCCBrotherDrawView::OnSelectClear()
{
	
	//m_currentShape = 3;     //测试消息函数是否响应
}

//“删除图形”菜单项的对应函数，用来删除鼠标选中的图形
void CCCBrotherDrawView::OnSelectDelete()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	CClientDC ht(this);
	for (int i = 0; i < pDoc->n_GraphSelect; i++)	//删除选中的图形
		Delete(&ht, pDoc->GraphSelect[i].Lb, pDoc->GraphSelect[i].Index);
	//以下是记录删除图形这一操作，供逆操作时用
	pDoc->AddUndo(3, pDoc->n_GraphSelect, pDoc->GraphSelect);
	ReleaseCapture();
	//pDoc->n_GraphSelect = 0;	//选中的图形元素为0
	pDoc->ClearGraphSelect();
	Invalidate();
	//if (pDoc->b_IsOleSelect)
	//{
	//	POSITION pos = pDoc->GetStartPosition();
	//	while (pos != NULL)
	//	{
	//		CCBrotherCntrItem *pItem = (CCBrotherCntrItem *)pDoc->GetNextClientItem(pos);
	//		if (pItem != NULL)
	//		{
	//			if (pItem->b_Select)
	//			{
	//				pDoc->RemoveItem(pItem);
	//				delete pItem;
	//				m_pSelection = NULL;   // set selection to last inserted item
	//			}
	//		} //////////////////////////////////////////////////////////////////////////////////////////////////////
	//	}
	//	pDoc->UpdateAllViews(this);
	//}
}

void CCCBrotherDrawView::OnSelectCursor()
{
	m_currentShape = 0;
	SetCapture();
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	int n = 1000000000;
	while (n--);
	//将光标变为箭头形，放弃捕捉的光标
	//SetCursor(LoadCursor(NULL, IDC_ARROW));
	ReleaseCapture();
}

void CCCBrotherDrawView::OnEditUndo()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	pDoc->ClearGraphSelect();                       //加上这句话就不会出现运行时错误了  不加的话 就会出现运行时错误 Assertin Failed
	//CClientDC ht(this);                 //！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	pDoc->OnEditUndo();			//进行一步逆操作
	//pDoc->UpdateAllViews(this);	//重画所有视图
}

//系统消息响应函数  被系统调用 
void CCCBrotherDrawView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)   
{
	//short nScrollInc = 0;        //滚动增量
	//int nNewPos = 0;             //滚动以后的滚动条位置
	//switch (nSBCode)
	//{
	//case SB_TOP:                 //滚动条在顶部  不能再向上滚动了 只能向下滚动
	//	nScrollInc = -nHScrollPos;
	//	break;
	//case SB_BOTTOM:              //滚动条在底部  不能再向下滚动了 只能向上滚动
	//	nScrollInc = nHScrollMax - nHScrollPos;
	//	break;
	//case SB_LINEUP:               //滚动条向上滚动   滚动条设置为横向最小负增量
	//	nScrollInc = -nXLine;
	//	break;
	//case SB_LINEDOWN:             //滚动条向下滚动   滚动条设置为横向最小增量
	//	nScrollInc = nXLine;
	//	break;
	//case SB_PAGEUP:               //滚动条向上翻页   滚动条设置为横向页负增量
	//	nScrollInc = -nXPage;
	//	break;
	//case SB_PAGEDOWN:             //滚动条向下翻页   滚动条设置为横向页增量
	//	nScrollInc = nXPage;
	//	break;
	//case SB_THUMBTRACK:           //滚动条的任何一个动作都会反复发消息  不断捕捉 响应
	//	nScrollInc = nPos - nHScrollPos;    //nPos - nHScrollPos
	//	break;
	//deafult:
	//	nScrollInc = 0;
	//}
	//nNewPos = max(0, min(nHScrollPos + nScrollInc, nHScrollMax));//确保移动范围不会越界！！！
	//nScrollInc = nNewPos - nHScrollPos;
	//if (nScrollInc)
	//{
	//	nHScrollPos = nNewPos;
	//	SetScrollPos(SB_HORZ, nHScrollPos);
	//	UpdateWindow();
	//	m_xStart = m_xStart + blc * nScrollInc*nScrollMin;     //切忌把加号写成减号  太惨了 呜呜呜
	//	Invalidate();
	//}

	short nScrollInc;
	int nNewPos;
	CRect r1;
	switch (nSBCode)
	{
	case SB_TOP:
		nScrollInc = -nHScrollPos;
		break;
	case SB_BOTTOM:
		nScrollInc = nHScrollMax - nHScrollPos;
		break;
	case SB_LINEUP:
		nScrollInc = -nXLine;
		break;
	case SB_LINEDOWN:
		nScrollInc = nXLine;
		break;
	case SB_PAGEUP:
		nScrollInc = -nXPage;
		break;
	case SB_PAGEDOWN:
		nScrollInc = nXPage;
		break;
	case SB_THUMBTRACK:           //滚动条的任何一个动作都会反复发消息  不断捕捉 响应
		nScrollInc = nPos - nHScrollPos;    //nPos - nHScrollPos
		break;
	/*case SB_THUMBPOSITION:
		nScrollInc = nPos - nHScrollPos;
		break;*/
	default:
		nScrollInc = 0;
	}
	nNewPos = max(0, min(nHScrollPos + nScrollInc, nHScrollMax));
	nScrollInc = nNewPos - nHScrollPos;
	if (nScrollInc) //如果产生了滚动
	{
		nHScrollPos = nNewPos;  //设定新的滚动位置
		SetScrollPos(SB_HORZ, nHScrollPos);
		UpdateWindow();//使滚动条的位置改动在屏幕上实现
		m_xStart = m_xStart + blc * nScrollInc*nScrollMin;//调整纵坐标使图形产生滚动
		GetClientRect(&r1);  //得到客户区的矩形边界
		if (abs(nScrollInc)*nScrollMin < r1.right) //如果滚动后的屏幕与滚动前有重叠
		{
			if (nScrollInc > 0)    //如果是图形向上滚动
				r1.left = nScrollInc * nScrollMin; //得到滚动屏幕上重叠区域的矩形
			else                //如果图形向下滚动
				r1.right = r1.right - nScrollInc * nScrollMin; //得到重叠区域的矩形
			ScrollWindow(-nScrollInc * nScrollMin, 0, r1);  //滚动重叠的区域
			if (nScrollInc > 0)                            //如果是向上滚动
				r1.left = r1.right - nScrollInc * nScrollMin;   //得到需要重画的区域
			else                                        //如果是向下滚动
				r1.right = -nScrollInc * nScrollMin;         //得到需要重画的区域
			InvalidateRect(r1, 0);                       //对图形进行局部重画
		}
		else //如果滚动后的区域与滚动前的区域没有重叠，则全屏重画
			Invalidate();
		//恢复矩形rr原来的坐标
		//        r.left=0;
		//      r.right=ScreenWide;
	}
	/*
	if(nScrollInc)
	{
		nHScrollPos=nNewPos;
		SetScrollPos(SB_HORZ,nHScrollPos);
		UpdateWindow();
		startx=startx+blc*nScrollInc*nScrollMin;
		Screenxy[numb][0]=startx;
		Invalidate();
		r.top=0;
		r.bottom=ScreenHigh;
	}*/
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

//系统消息响应函数  被系统调用 
void CCCBrotherDrawView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//short nScrollInc = 0;
	//int nNewPos = 0;
	//switch (nSBCode)
	//{
	//case SB_TOP:                 //滚动条在顶部  不能再向上滚动了 只能向下滚动
	//	nScrollInc = -nVScrollPos;
	//	break;
	//case SB_BOTTOM:              //滚动条在底部  不能再向下滚动了 只能向上滚动
	//	nScrollInc = nVScrollMax - nVScrollPos;
	//	break;
	//case SB_LINEUP:               //滚动条向上滚动   滚动条设置为横向最小负增量
	//	nScrollInc = -nYLine;
	//	break;
	//case SB_LINEDOWN:
	//	nScrollInc = nYLine;
	//	break;
	//case SB_PAGEUP:
	//	nScrollInc = -nYPage;
	//	break;
	//case SB_PAGEDOWN:
	//	nScrollInc = nYPage;
	//	break;
	//case SB_THUMBTRACK:
	//	nScrollInc = nPos - nVScrollPos;    //nPos - nVScrollPos
	//	break;
	//deafult:
	//	nScrollInc = 0;
	//}
	//nNewPos = max(0, min(nVScrollPos + nScrollInc, nVScrollMax));//确保移动范围不会越界！！！
	//nScrollInc = nNewPos - nVScrollPos;
	//if (nScrollInc)
	//{
	//	nVScrollPos = nNewPos;
	//	SetScrollPos(SB_VERT, nVScrollPos);
	//	UpdateWindow();
	//	m_yStart = m_yStart - blc * nScrollInc*nScrollMin;     //切忌把减号写成加号  太惨了 呜呜呜
	//	Invalidate();
	//}

	short nScrollInc;
	int nNewPos;
	CRect r1;
	switch(nSBCode) //判断按键位置
	{
		case SB_TOP:                 //如果将滚动条滚动到顶部
			nScrollInc=-nVScrollPos;   //得到相对滚动范围
			break;
		case SB_BOTTOM:              //如果将滚动条滚动到底部
			nScrollInc=nVScrollMax-nVScrollPos; //得到相对的滚动范围
			break;
		case SB_LINEUP:             //如果按了滚动条中的向上的按键
			nScrollInc=-nYLine;      //得到相对滚动位置
			break;
		case SB_LINEDOWN:           //如果按中了向下的按键
			nScrollInc=nYLine;        //得到相对的滚动位置
			break;
		case SB_PAGEUP:             //如果按中了中间活动按钮的上部区域
			nScrollInc=-nYPage;       //得到相对的滚动位置
			break;                    
		case SB_PAGEDOWN:           //如果按中了中间活动钮的下部位置
			nScrollInc=nYPage;        //得到相对的滚动位置
			break;
		case SB_THUMBTRACK:
			nScrollInc = nPos - nVScrollPos;    //nPos - nVScrollPos
			break;
		//case SB_THUMBPOSITION:      //如果用鼠标拖动中间活动钮到一个位置
		//	nScrollInc=nPos-nVScrollPos; //通过信息处理函数得到的按钮位置得到相对移动位置
		//	break;
		default:
			nScrollInc=0;
	}
	//进行滚动边界检查，得到实际的滚动位置（不能超出滚动条的滚动范围）
	nNewPos=max(0,min(nVScrollPos+nScrollInc,nVScrollMax));
	//得到实际的相对滚动范围
	nScrollInc=nNewPos-nVScrollPos;
	if(nScrollInc) //如果产生了滚动
	{
		nVScrollPos=nNewPos;  //设定新的滚动位置
		SetScrollPos(SB_VERT,nVScrollPos);
		UpdateWindow();//使滚动条的位置改动在屏幕上实现
		m_yStart=m_yStart-blc*nScrollInc*nScrollMin;//调整纵坐标使图形产生滚动
		GetClientRect(&r1);  //得到客户区的矩形边界
		if(abs(nScrollInc)*nScrollMin<r1.bottom) //如果滚动后的屏幕与滚动前有重叠
		{
			if(nScrollInc>0)    //如果是图形向上滚动
				r1.top=nScrollInc*nScrollMin; //得到滚动屏幕上重叠区域的矩形
			else                //如果图形向下滚动
				r1.bottom=r1.bottom+nScrollInc*nScrollMin; //得到重叠区域的矩形
			ScrollWindow(0,-nScrollInc*nScrollMin,r1);  //滚动重叠的区域
			if(nScrollInc>0)                            //如果是向上滚动
				r1.top=r1.bottom-nScrollInc*nScrollMin;   //得到需要重画的区域
			else                                        //如果是向下滚动
				r1.bottom=-nScrollInc*nScrollMin;         //得到需要重画的区域
			InvalidateRect(r1,0);                       //对图形进行局部重画
			//pbdy=1;
		}
		else //如果滚动后的区域与滚动前的区域没有重叠，则全屏重画
			Invalidate();
	}
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CCCBrotherDrawView::DrawRegularLineBetweenTwoComponents(CCCBrotherDrawDoc* pDoc, bool mode, CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor)
{
	short int Lb0 = pDoc->GraphSelect[0].Lb, Lb1 ,Index0 = pDoc->GraphSelect[0].Index, Index1;
	baseShape *shape0 = pDoc->GetGraph(Lb0, Index0), *shape1;
	if (pDoc->n_GraphSelect<=1)
	{
		pDoc->ClearGraphSelect();
		MessageBox(_T("现在还不支持自己连自己哦 敬请期待！"));
		return;
	}
	else
	{
		Lb1 = pDoc->GraphSelect[1].Lb;
		Index1 = pDoc->GraphSelect[1].Index;
		shape1 = pDoc->GetGraph(Lb1, Index1);
	}
	PointStruct p0 = shape0->GetPoint('L');
	PointStruct p1 = shape1->GetPoint('L');
	float x1, y1, x2, y2/*, number = 4*/;
	//PointStruct* list = new PointStruct[10];
	bool flag = false;
	int id_only;

	x1 = min(p0.x, p1.x);
	y1 = min(p0.y, p1.y);
	x2 = max(p0.x, p1.x);
	y2 = max(p0.y, p1.y);

	id_only = pDoc->GetGraphID(4);
	CC_Brother_PLine* regularPLine = new CC_Brother_PLine(m_penColor, m_brushColor, m_lineWide, m_lineType, m_curLayer, id_only, 0, 0, nullptr, 0);
	if (x1 != x2 && y1 != y2)
	{
		if (Lb1 == 3 && Lb0 == 3)			//两个器件都是开关
		{
			regularPLine->LineFromSwitchToSwitch(pDoc, Lb0, Index0, Lb1, Index1);
			//pDoc->ClearGraphSelect();   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!症结所在
			//pDoc->AddPLine(regularPLine);
			//if (p0.x > p1.x)
			//{
			//	p0 = shape0->GetPoint('R');
			//	p1 = shape1->GetPoint('R');
			//	list[0] = p0;
			//	list[1].x = p0.x + 10, list[1].y = p0.y;
			//	list[2].x = p0.x + 10, list[2].y = p1.y;
			//	list[3] = p1;
			//	//list[3].x = p1.x - 1, list[3].y = p1.y;	    //这个bug修复的有点无厘头
			//}
			//else
			//{
			//	list[0] = p0;
			//	list[1].x = p0.x - 10, list[1].y = p0.y;
			//	list[2].x = p0.x - 10, list[2].y = p1.y;
			//	list[3] = p1;
			//}
			flag = true;
		}
		else if ((Lb1 == 1 && Lb0 == 3) || (Lb0 == 1 && Lb1 == 3))
		{
			MessageBox(_T("开关 + 直线？"));
		}
		else if (Lb1 == 1 && Lb0 == 1)
		{
			MessageBox(_T("直线 + 直线？"));
		}
		else if ((Lb1 == 1 && Lb0 == 2) || (Lb0 == 1 && Lb1 == 2))
		{
			MessageBox(_T("文本 + 直线？"));
		}
		else if (Lb1 == 2 && Lb0 == 2)
		{
			MessageBox(_T("文本 + 文本？"));
		}
		else if ((Lb1 == 2 && Lb0 == 3) || (Lb0 == 2 && Lb1 == 3))
		{
			MessageBox(_T("开关 + 文本？"));
		}
		else
		{
			MessageBox(_T("哎  别试了 连不了的。。。 敬请期待哟！"));
		}
		if (flag)
		{
			pDoc->AddPLine(regularPLine);
			GraphUndo[0].Index = pDoc->GetGraphUpperBound(4);//得到此连续直线的在m_PLineArray中的下标。因为是最新添加的 所以会是数组的最大有效下标
			GraphUndo[0].Lb = 4;	//连续直线
			pDoc->AddUndo(2, 1, GraphUndo);
		}
		else
		{
			delete regularPLine;
		}
	}
	else
	{
		MessageBox(_T("!!!!!"));
	}
	pDoc->ClearGraphSelect();
	//delete list;
}

void CCCBrotherDrawView::OnPenColor()
{
	CColorDialog ColorDialogBox(0, 0, this);
	if (ColorDialogBox.DoModal() == IDOK)
	{
		m_penColor = ColorDialogBox.GetColor();
	}
}

void CCCBrotherDrawView::OnBackColor()
{
	CColorDialog ColorDialogBox(0, 0, this);
	if (ColorDialogBox.DoModal() == IDOK)
	{
		m_backgroundColor = ColorDialogBox.GetColor();
	}
	Invalidate();
	UpdateWindow();
}

void CCCBrotherDrawView::OnGraphColor()
{
	CCCBrotherDrawDoc* pDoc = GetDocument();
	long color;

	CColorDialog ColorDialogBox(0, 0, this);
	if (ColorDialogBox.DoModal() == IDOK)
	{
		color = ColorDialogBox.GetColor();
	}

	for (int i = 0; i < pDoc->n_GraphSelect; i++)	//修改选中图形颜色
	{
		(pDoc->GetGraph(pDoc->GraphSelect[i].Lb, pDoc->GraphSelect[i].Index))->setColorPen(color);
	}
	//ReleaseCapture();
	//pDoc->n_GraphSelect = 0;	//选中的图形元素为0  同下面的函数。。。
	pDoc->ClearGraphSelect();
	Invalidate();
}

void CCCBrotherDrawView::OnLayre()
{
	if (!pTextDlg)  //如果对象不存在  也就是还是空指针
	{
		pTextDlg = new CC_Brother_TextDlg(this, m_TextString);
		ASSERT(pTextDlg);
	}
	if (!(pTextDlg->IsOpen()))  //打开。。。。。。 判断句柄资源是否存在
	{
		pTextDlg->Create(IDD_WRITE_TEXT, this);
	}
	if (!(pTextDlg->IsVisible())) //判断是否可视  不可视就让窗口可视化
	{
		pTextDlg->ShowWindow(SW_SHOW);
	}
	pTextDlg->SetWindowTextW(_T("图层选择"));
	m_currentShape = 100;
}

void CCCBrotherDrawView::OnLineType()
{
	if (!pTextDlg)  //如果对象不存在  也就是还是空指针
	{
		pTextDlg = new CC_Brother_TextDlg(this, m_TextString);
		ASSERT(pTextDlg);
	}
	if (!(pTextDlg->IsOpen()))  //打开。。。。。。 判断句柄资源是否存在
	{
		pTextDlg->Create(IDD_WRITE_TEXT, this);
	}
	if (!(pTextDlg->IsVisible())) //判断是否可视  不可视就让窗口可视化
	{
		pTextDlg->ShowWindow(SW_SHOW);
	}
	pTextDlg->SetWindowTextW(_T("线型选择"));
	m_currentShape = 99;
}

void CCCBrotherDrawView::OnLineWide()
{
	if (!pTextDlg)  //如果对象不存在  也就是还是空指针
	{
		pTextDlg = new CC_Brother_TextDlg(this, m_TextString);
		ASSERT(pTextDlg);
	}
	if (!(pTextDlg->IsOpen()))  //打开。。。。。。 判断句柄资源是否存在
	{
		pTextDlg->Create(IDD_WRITE_TEXT, this);
	}
	if (!(pTextDlg->IsVisible())) //判断是否可视  不可视就让窗口可视化
	{
		pTextDlg->ShowWindow(SW_SHOW);
	}
	pTextDlg->SetWindowTextW(_T("线宽选择"));
	m_currentShape = 98;
}

void CCCBrotherDrawView::OnFilePrinting()
{
	CPrintDialog print(false);
	if (print.DoModal() == IDOK)
	{
		CDC printcd;
		printcd.Attach(print.GetPrinterDC());

		DOCINFO pdoc;
		pdoc.cbSize = sizeof(pdoc);
		pdoc.lpszDocName = _T("pdoc");
		pdoc.lpszDatatype = NULL;
		pdoc.fwType = NULL;
		pdoc.lpszOutput = NULL;
		if (printcd.StartDoc(&pdoc) >= 0)
		{
			/*LOGFONT logfont;
			memset(&logfont, 0, sizeof(LOGFONT));
			logfont.lfHeight = 75;

			CFont font;
			CFont *oldfont = NULL;
			if (font.CreateFontIndirect(&logfont))
				oldfont = (CFont*)printcd.SelectObject(&font);
			CString stt;
			stt.Format(_T("第%d页"), 1);*/

			//printcd.SetWindowOrg(0, blc*m_hScreen);
			printcd.StartPage();
			OnDraw(&printcd);                //OnDraw画图区域是当前视图中所有图形的所在区域，而不会把整块屏幕，包括看不见的那部分空白区域给画出来
			printcd.EndPage();

			printcd.EndDoc();
		}
		printcd.DeleteDC();
	}
}

// CCCBrotherDrawView 诊断

#ifdef _DEBUG
void CCCBrotherDrawView::AssertValid() const
{
	CView::AssertValid();
}

void CCCBrotherDrawView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCCBrotherDrawDoc* CCCBrotherDrawView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCCBrotherDrawDoc)));
	return (CCCBrotherDrawDoc*)m_pDocument;
}
#endif //_DEBUG

