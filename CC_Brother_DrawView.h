
// CC_Brother_DrawView.h: CCCBrotherDrawView 类的接口
//

#pragma once

class CCCBrotherDrawDoc;
class CC_Brother_TextDlg;
class CC_Brother_Text;
class ScreenStructForBoost;
class CCBrotherCntrItem;
struct PointStruct;
struct GraphSelectStruct;

class CCCBrotherDrawView : public CView
{
protected: // 仅从序列化创建
	CCCBrotherDrawView() noexcept;
	DECLARE_DYNCREATE(CCCBrotherDrawView)

// 特性
public:
	CCCBrotherDrawDoc* GetDocument() const;
	
	CCBrotherCntrItem* m_pSelection = nullptr;  
	/*继承于COleClientItem类定义了OLE项的包容器界面。OLE项代表由
	服务器应用建立并维护的数据，它可以“无缝地”和文档合成一体，
	成为“复合文档”，而在用户看来就如同一个单一的文档一样。“复合
	文档”由OLE项和一包含文档组成。*/

	// 操作
//添加的自定义函数
public:
	void DrawRegularLineBetweenTwoComponents(CCCBrotherDrawDoc* pDoc, bool mode, CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor);//画两个元器件之间的规则导线
	void  DeviceP_To_LogicalP(float x,float y,int *X,int *Y); //设备坐标点映射到逻辑坐标点 P代表Point
	void  LogicalP_To_DeviceP(int x, int y, float *X, float*Y);
	int   DeviceL_To_LogicalL(float length);     //设备上的长度转换为逻辑长度 L代表length
	float LogicalL_To_DeviceL(int length);
	void  DrawBakcground(CDC* pDC);//画版背景颜色的设置
	void InitVScroll();            //初始化纵向滚动条 Vertical
	void InitHScroll();			   //初始化横向滚动条
	void Delete(CDC* pDC, int Lb, int Index);	//删除图形操作
	void ReDrawRect(float X1, float Y1, float X2, float Y2);
	BOOL RectCross(float* x1, float* y1, float* x2, float* y2, float xx1, float yy1, float xx2, float yy2);
	
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);      //创建窗口前的准备工作
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用 ///////aaaaaaaaaaaaaaaaaa啊啊啊啊 抓狂！！！！！！！！！！
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);            //响应消息  打印工作的准备
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);    //响应消息  开始打印
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);      //响应消息  结束打印
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactivateView);   //激活某一个窗口

// 实现
public:
	virtual ~CCCBrotherDrawView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//对话框类对象指针和一些成员变量
protected:
	CC_Brother_TextDlg* pTextDlg;             //文本框资源对象指针
	float m_FontHeight, m_FontWide, m_FontBetween, m_TextAngle, m_FontAngle;  //文本字体高度、宽度、间隔、文本偏离水平角度 字体偏离水平角度
	CString m_TextString;       //文本字符串内容
	CC_Brother_Text* m_Text1;   //文本框对象指针

// 生成的消息映射函数
protected:
	afx_msg void OnSize(UINT nType,int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDrawLine();
	afx_msg void OnDrawPLine();
	afx_msg void OnDrawPPLine();
	afx_msg void OnDrawText();
	afx_msg void OnDrawSwitch();
	afx_msg void OnDrawGround();
	afx_msg void OnEnLarge();
	afx_msg void OnShrink();
	afx_msg void OnSelectMouse();
	afx_msg void OnSelectClear();
	afx_msg void OnSelectDelete();
	afx_msg void OnSelectCursor();
	afx_msg void OnEditUndo();
	afx_msg void EditText();
	afx_msg void DlgOK();
	afx_msg void DlgCancel();
	afx_msg void OnRecovery();//一键还原
	afx_msg void OnGraphAll();		//重画全屏
	afx_msg void OnShift();     //选中图形 移动
	afx_msg void OnPenColor();
	afx_msg void OnBackColor();
	afx_msg void OnGraphColor();
	afx_msg void OnLayre();   //图层管理
	afx_msg void OnLineType();
	afx_msg void OnLineWide();
	afx_msg void OnFilePrinting();
	DECLARE_MESSAGE_MAP()


protected:
	float xLeft, xRight, yTop, yBottom, xPage, yPage, xWide, yHigh;
	int   nPageX, nPageY;
	int m_hScreen1, m_wScreen1;	//当前视图的高度和宽度  零时的保存原来的屏幕的宽度和高度 等待打印之后 根据这个零时数据还原原来的窗口尺寸
	int nHScrollMax = 0, nHScrollPos = 0, nVScrollMax = 0, nVScrollPos = 0;//横向和纵向的最大滚动范围以及滚动条当前位置horizonal vertial
	int nXPage = 0, nXLine, nYPage = 0, nYLine, nScrollMin;//横向以及纵向页最小滚动距离以及最小滚动距离 滚动一行或一页

//添加的自定义变量 关于基类图形的一些属性
public:
	int m_MapMode;       //还需要添加他的setMapMode和GetMapMode函数，这里的值只是一个初始的数值
	float m_xStart, m_yStart, blc;  //原点坐标 以及这个坐标与实际坐标之间转换的比例尺
	int m_wScreen = 0, m_hScreen = 0; //屏幕的宽度和高度
	int m_currentShape = 0;	   //当前画的图形的类型
	int LButtonPushNum = 0;  //鼠标左键按下的次数
	CPoint m_originalPoint, m_oldPoint;  //提供对画图重影的支持
	bool startDraw, finishDraw;   //根据画图状态的4个状态进行操作
	
    //baseShape的7个参数 省略了一个b_Delete  构造的时候 将其默认为0
	long m_penColor;
	long m_backgroundColor;/* = 4;*/ //可以添加模块获取其他的背景色 通过 getColor和setColor来获取或者设置其值
	long m_brushColor;
	short m_lineWide;
	short m_lineType;
	short m_curLayer;
	//float m_xScale; //线型的横向参数

	int sizeOfSwitch;//开关尺寸

	bool SelectedGraph = false;
	
	ScreenStructForBoost *p_Screen = nullptr;   //用来记录每一个放大的状态一遍还原上一状态和回复到最初状态

	PointStruct* PointXyz;      //存储已经按下的连续直线顶点的实际坐标

	GraphSelectStruct* GraphUndo/*[4]*/;
};

#ifndef _DEBUG  // CC_Brother_DrawView.cpp 中的调试版本
inline CCCBrotherDrawDoc* CCCBrotherDrawView::GetDocument() const
   { return reinterpret_cast<CCCBrotherDrawDoc*>(m_pDocument); }
#endif

