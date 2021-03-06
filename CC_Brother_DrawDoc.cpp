
// CC_Brother_DrawDoc.cpp: CCCBrotherDrawDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CC_Brother_Draw.h"
#endif

#include "CC_Brother_DrawDoc.h"
#include "CCBrotherCntrItem.h"
#include "CC_Brother_DrawView.h"
#include <propkey.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CCCBrotherDrawDoc* ptr_DrawDoc;

int baseCount, lineCount, textCount, switchCount, plineCount, groundCount;
int firstPrinting;

// CCCBrotherDrawDoc

IMPLEMENT_DYNCREATE(CCCBrotherDrawDoc, COleDocument)

BEGIN_MESSAGE_MAP(CCCBrotherDrawDoc, COleDocument)
	//{{AFX_MSG_MAP(CDrawDoc)
	ON_UPDATE_COMMAND_UI(ID_SELECT_CLEAR, OnUpdateSelectClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_SELECT_DELETE, OnUpdateSelectClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateSelectClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateSelectClear)
	//}}AFX_MSG_MAP
	// 启用默认的 OLE 容器实现
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &COleDocument::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, &COleDocument::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, &COleDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, &COleDocument::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, &COleDocument::OnUpdateEditLinksMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_POPUP, &CCCBrotherDrawDoc::OnUpdateObjectVerbPopup)
	ON_COMMAND(ID_OLE_EDIT_LINKS, &COleDocument::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, &COleDocument::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

CC_Brother_GraphPara* p_GraphPara; //全局的图形参数类指针

extern CCCBrotherDrawView *ptr_View;

void init();

//int RunTimes = 0;

//float xMinScreen = ptr_View->m_xStart, yMinScreen = ptr_View->m_yStart, xMaxScreen = ptr_View->m_wScreen*ptr_View->blc, yMaxScreen = ptr_View->m_hScreen*ptr_View->blc;

BOOL IsRectCross(float minx, float miny, float maxx, float maxy)
{
	static float xMinScreen = ptr_View->m_xStart, yMinScreen = ptr_View->m_yStart, xMaxScreen = ptr_View->m_wScreen*ptr_View->blc, yMaxScreen = ptr_View->m_hScreen*ptr_View->blc;

	if (minx > xMaxScreen || maxx<xMinScreen || miny>yMaxScreen || maxy < yMinScreen) //两个矩形区域不相交
		return 0; //如不相交函数返回0
	else
		return 1; //如果相交就返回1
}

// CCCBrotherDrawDoc 构造/析构

CCCBrotherDrawDoc::CCCBrotherDrawDoc() noexcept
{
	m_MaxScreen = 2000;             //最多能够记录在100个历史屏幕
	m_Screen = new ScreenStructForBoost[m_MaxScreen];
	m_CurrentScreen = 0;
	m_Screen[0].blc = 1.0;
	m_Screen[0].startX = 0.0;
	m_Screen[0].startY = 0.0;//对放大的路径进行记录 以便恢复原来的图形
	
	p_GraphPara = &m_GraphPara;
	m_OnlyIndex = new int[10000];   //用来支持产生某类图形的某一个具体图形的唯一识别号
	//layre = new LayerStruct[1000];
	/*for (int i = 0; i < 1000; i++)
	{
		layre[i].m_id_only = i;
		layre[i].b_Display = 0;
	}
	layre[0].b_Display = 1;
	layre[0].m_Name = _T("图层0");*/

	GraphSelect = new GraphSelectStruct[1000];
	n_GraphSelect = 0;
	b_IsOleSelect = 0;

	//提供对文档逆向操作的支持
	n_MaxUnIndex = 10000;	//规定可以进行1万步的逆操作
	m_UndoList = new UndoStruct[n_MaxUnIndex];
	n_CurUnIndex = 0;		//当前的逆操作序号为0
	m_UndoList[0].l_Start = 0;
	FileName += _T("draw.tmp");
	setlocale(LC_CTYPE, ("chs"));
	if (!Fundo.Open(FileName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
	{
		OutputDebugString(_T("文件打开失败！！！！！！！"));
		//printf("%s", "文件打开失败！！！");
		exit(11);
	}                //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//RunTimes++;
	EnableCompoundFile();         //复合文件设置
	
	ptr_DrawDoc = this;
}

//释放堆中申请的内存
CCCBrotherDrawDoc::~CCCBrotherDrawDoc()
{
	delete[] GraphSelect; 
	delete[] m_Screen;             //内存不能被重复释放
	delete[] m_OnlyIndex;
	delete[] m_UndoList;

	Fundo.Close();
	USES_CONVERSION;
	remove(T2A(FileName.GetBuffer(FileName.GetLength())));
}												//把断点设置在这里而不是上一行的用意在于确保能够移除文件  而不会因为每次断电以后终止进程而导致零时文件无法移除

BOOL CCCBrotherDrawDoc::OnNewDocument()
{
	if (!COleDocument::OnNewDocument())
		return FALSE;

	//m_xStart = 0.0, m_yStart = 0.0;	//可能对p_Screen也要进行处理
	//blc = 1.0;
	//m_backgroundColor = RGB(255, 255, 255);
	if (ptr_View)
	{
		ptr_View->m_xStart = 0.0, ptr_View->m_yStart = 0.0;
		ptr_View->blc = 1.0;
		ptr_View->m_backgroundColor = RGB(255, 255, 255);
		ptr_View->m_lineType = 0;
		ptr_View->m_lineWide = 1;

		(m_GraphPara).m_LayerList[ptr_View->m_curLayer].b_Display = 0;
		(m_GraphPara).m_LayerList[0].b_Display = 1;
		ptr_View->m_curLayer = 0;
	}
	firstPrinting = 0;
	return TRUE;
}

// CCCBrotherDrawDoc 序列化

/*
void CDrawDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		//line1=new CLine	(0,0,0,0,0,0,0,0,100,100);
//		ar>>line1;
		// TODO: add loading code here
	}
	int nn=m_LineArray.GetSize();
	while(nn--)
	{
		if(m_LineArray.GetAt(nn)->IsDelete())
		{
			delete m_LineArray.GetAt(nn);
			m_LineArray.RemoveAt(nn);
		}
	}
	nn=m_PLineArray.GetSize();
	while(nn--)
	{
		if(m_PLineArray.GetAt(nn)->IsDelete())
		{
			delete m_PLineArray.GetAt(nn);
			m_PLineArray.RemoveAt(nn);
		}
	}
	nn=m_CircleArray.GetSize();
	while(nn--)
	{
		if(m_CircleArray.GetAt(nn)->IsDelete())
		{
			delete m_CircleArray.GetAt(nn);
			m_CircleArray.RemoveAt(nn);
		}
	}
	nn=m_ArcArray.GetSize();
	while(nn--)
	{
		if(m_ArcArray.GetAt(nn)->IsDelete())
		{
			delete m_ArcArray.GetAt(nn);
			m_ArcArray.RemoveAt(nn);
		}
	}
	nn=m_TextArray.GetSize();
	while(nn--)
	{
		if(m_TextArray.GetAt(nn)->IsDelete())
		{
			delete m_TextArray.GetAt(nn);
			m_TextArray.RemoveAt(nn);
		}
	}
	m_LineArray.Serialize(ar);
	m_PLineArray.Serialize(ar);
	m_CircleArray.Serialize(ar);
	m_ArcArray.Serialize(ar);
	m_TextArray.Serialize(ar);
	n_CurUnIndex=0;
	SetModifiedFlag(0);
	// Calling the base class COleDocument enables serialization
	//  of the container document's COleClientItem objects.
	COleDocument::Serialize(ar);
}
*/

void CCCBrotherDrawDoc::Serialize(CArchive& ar)
{
	//if (ar.IsStoring())
	//{
	//	//注释的功能被下面的代码替代
	//	int nn = 0;
	//	for (int i = 1; i <= 10; i++)
	//	{
	//		nn = GetGraphUpperBound(i) + 1;
	//		while (nn--)
	//		{
	//			if (GetGraph(i, nn)->IsDelete())
	//			{
	//				delete GetGraph(i, nn);
	//				DeleteGraph(i,nn);
	//			}
	//		}
	//	}
	//	m_xStart = ptr_View->m_xStart;
	//	m_yStart = ptr_View->m_yStart;
	//	blc = ptr_View->blc;
	//	m_backgroundColor = ptr_View->m_backgroundColor;
	////	m_backgroundColor = ptr_View->m_backgroundColor;
	//	m_lineType = ptr_View->m_lineType;
	//	m_lineWide = ptr_View->m_lineWide;
	//	m_currLayer = ptr_View->m_curLayer;
	//	//(m_GraphPara).m_LayerList[m_currLayer].b_Display = 0;
	//	//(m_GraphPara).m_LayerList[0].b_Display = 1;
	//	ar << m_xStart << m_yStart << blc << m_backgroundColor << m_lineType << m_lineWide << m_currLayer;
	//}

	//else
	//{
	//	ar >> m_xStart >> m_yStart >> blc >> m_backgroundColor >> m_lineType >> m_lineWide >> m_currLayer;
	//	ptr_View->m_xStart = m_xStart;
	//	ptr_View->m_yStart = m_yStart;
	//	ptr_View->blc = blc;
	//	ptr_View->m_backgroundColor = m_backgroundColor;
	//	ptr_View->m_lineType = m_lineType;
	//	ptr_View->m_lineWide = m_lineWide;
	//	(m_GraphPara).m_LayerList[ptr_View->m_curLayer].b_Display = 0;
	//	ptr_View->m_curLayer = m_currLayer;
	//	(m_GraphPara).m_LayerList[m_currLayer].b_Display = 1;
	//}

	//m_LineArray.Serialize(ar);
	//m_PLineArray.Serialize(ar);
	//m_SwitchArray.Serialize(ar);
	//m_TextArray.Serialize(ar);
	//m_GroundArray.Serialize(ar);

	CFile *file1;
	CStdioFile *file2;
	int n_LineNumb, n_PLineNumb, n_SwitchNumb, n_TextNumb, n_GroundNumb;
	file1 = ar.GetFile();
	file2 = &DatabaseFile;
	if (ar.IsStoring())
	{
		DatabaseFileName.Empty();
		DatabaseFileName = this->GetTitle();

		if (/*DatabaseFileName == _T("无标题")*/!DatabaseFileName.CompareNoCase(_T("无标题")))
		{
			MessageBox(NULL, L"This operation will store database files into default document \"无标题.txt\".\n If you want to save as your_document_titlt.txt.\n Please Tyr again!", L"Tips", IDOK/*MB_OK*/);
			//MessageBox(TEXT("你选择了YES"), TEXT("YES"), MB_OK);
			//MessageBox(L"This operation will store database files ioto");
			//MessageBox(_T("最多只能选择1000个图形元素"));
		}
		DatabaseFileName = _T("DatabaseFile/") + DatabaseFileName + _T(".txt");
		if (!DatabaseFile.Open(DatabaseFileName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText))
		{
			OutputDebugString(_T("支持数据库存储的零时文件打开失败！！！！！！！\n"));
			exit(11);
		}
		int nn = 0;
		for (int i = 1; i <= 10; i++)
		{
			nn = GetGraphUpperBound(i) + 1;
			while (nn--)
			{
				if (GetGraph(i, nn)->IsDelete())
				{
					delete GetGraph(i, nn);
					DeleteGraph(i, nn);
				}
			}
		}
		n_LineNumb = m_LineArray.GetSize();
		n_TextNumb = m_TextArray.GetSize();
		n_SwitchNumb = m_SwitchArray.GetSize();
		n_PLineNumb = m_PLineArray.GetSize();
		n_GroundNumb = m_GroundArray.GetSize();

		m_xStart = ptr_View->m_xStart;
		m_yStart = ptr_View->m_yStart;
		blc = ptr_View->blc;
		m_backgroundColor = ptr_View->m_backgroundColor;
		m_lineType = ptr_View->m_lineType;
		m_lineWide = ptr_View->m_lineWide;
		m_currLayer = ptr_View->m_curLayer;

		CString tmp;
		file2->SeekToBegin();
		init();
		file2->WriteString(_T("CCBrother Database Support File For Current Document..................................\n\n"));
		file2->WriteString(_T("Drawing board information for current document:\n"));

		tmp.Empty();
		tmp.Format(_T("    Origin Point X:%.2f    Origion Point Y:%.2f\n"), m_xStart, m_xStart);
		file2->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    blc(Parameter for logical point mapping to device point):%.2f\n"), blc);
		file2->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    BackgroundColor:%06x\n"), m_backgroundColor);
		file2->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    LineType:%d\n"), m_lineType);
		file2->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    LineWide:%d\n"), m_lineWide);
		file2->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    CurrentLayer:%d\n\n"), m_currLayer);
		file2->WriteString(tmp);

		file2->WriteString(_T("Overview about this document:\n"));
		tmp.Empty();
		tmp.Format(_T("    LineNum:%d\n"), n_LineNumb);
		file2->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    TextNum:%d\n"), n_TextNumb);
		file2->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    SwitchNum:%d\n"), n_SwitchNumb);
		file2->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    PlineNum:%d\n"), n_PLineNumb);
		file2->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    GroundNum:%d\n\n"), n_GroundNumb);
		file2->WriteString(tmp);

		file2->WriteString(_T("Detail informations:\n"));

		file1->Write((unsigned char *)&m_xStart, sizeof(float));
		file1->Write((unsigned char *)&m_yStart, sizeof(float));
		file1->Write((unsigned char *)&blc, sizeof(float));
		file1->Write((unsigned char *)&m_backgroundColor, sizeof(long));
		file1->Write((unsigned char *)&m_lineType, sizeof(short));
		file1->Write((unsigned char *)&m_lineWide, sizeof(short));
		file1->Write((unsigned char *)&m_currLayer, sizeof(short));

		file1->Write((unsigned char *)&n_LineNumb, sizeof(int));
		file1->Write((unsigned char *)&n_TextNumb, sizeof(int));
		file1->Write((unsigned char *)&n_SwitchNumb, sizeof(int));
		file1->Write((unsigned char *)&n_PLineNumb, sizeof(int));
		file1->Write((unsigned char *)&n_GroundNumb, sizeof(int));
		if (n_LineNumb > 0)
		{
			nn = m_LineArray.GetUpperBound() + 1;
			while (nn--)
			{
				if (m_LineArray.GetAt(nn))
					GetLine(nn)->Save(file1, file2, 1);
			}
		}
		if (n_TextNumb > 0)
		{
			nn = m_TextArray.GetUpperBound() + 1;
			while (nn--)
			{
				if (m_TextArray.GetAt(nn))
					GetText(nn)->Save(file1, file2, 1);
			}
		}
		if (n_SwitchNumb > 0)
		{
			nn = m_SwitchArray.GetUpperBound() + 1;
			while (nn--)
			{
				if (m_SwitchArray.GetAt(nn))
					GetSwitch(nn)->Save(file1, file2, 1);
			}
		}
		if (n_PLineNumb > 0)
		{
			nn = m_PLineArray.GetUpperBound() + 1;
			while (nn--)
			{
				if (m_PLineArray.GetAt(nn))
					GetPLine(nn)->Save(file1, file2, 1);
			}
		}
		if (n_GroundNumb > 0)
		{
			nn = m_GroundArray.GetUpperBound() + 1;
			while (nn--)
			{
				if (m_GroundArray.GetAt(nn))
					GetGround(nn)->Save(file1, file2, 1);
			}
		}
		DatabaseFile.Close();
	}
	else
	{
		//file1->Read((unsigned char *)&m_xStart, sizeof(float));
		file1->Read((unsigned char *)&m_xStart, sizeof(float));
		file1->Read((unsigned char *)&m_yStart, sizeof(float));
		file1->Read((unsigned char *)&blc, sizeof(float));
		file1->Read((unsigned char *)&m_backgroundColor, sizeof(long));
		file1->Read((unsigned char *)&m_lineType, sizeof(short));
		file1->Read((unsigned char *)&m_lineWide, sizeof(short));
		file1->Read((unsigned char *)&m_currLayer, sizeof(short));

		ptr_View->m_xStart = m_xStart;
		ptr_View->m_yStart = m_yStart;
		ptr_View->blc = blc;
		ptr_View->m_backgroundColor = m_backgroundColor;
		ptr_View->m_lineType = m_lineType;
		ptr_View->m_lineWide = m_lineWide;
		(m_GraphPara).m_LayerList[ptr_View->m_curLayer].b_Display = 0;
		ptr_View->m_curLayer = m_currLayer;
		(m_GraphPara).m_LayerList[m_currLayer].b_Display = 1;
		
		file1->Read((unsigned char *)&n_LineNumb, sizeof(int));
		file1->Read((unsigned char *)&n_TextNumb, sizeof(int));
		file1->Read((unsigned char *)&n_SwitchNumb, sizeof(int));
		file1->Read((unsigned char *)&n_PLineNumb, sizeof(int));
		file1->Read((unsigned char *)&n_GroundNumb, sizeof(int));
		for (int i = 0; i < n_LineNumb; i++)          //添加直线
		{
			CC_Brother_Line* p_Line = new CC_Brother_Line();
			p_Line->Save(file1, file2, 0);
			m_LineArray.Add(p_Line);
		}
		for (int i = 0; i < n_TextNumb; i++)          //添加文本
		{
			CC_Brother_Text* p_Text = new CC_Brother_Text();
			p_Text->Save(file1, file2, 0);
			m_TextArray.Add(p_Text);
		}
		for (int i = 0; i < n_SwitchNumb; i++)          //添加开关
		{
			CC_Brother_Switch* p_Switch = new CC_Brother_Switch();
			p_Switch->Save(file1, file2, 0);
			m_SwitchArray.Add(p_Switch);
		}
		for (int i = 0; i < n_PLineNumb; i++)          //添加连续直线
		{
			CC_Brother_PLine* p_PLine = new CC_Brother_PLine();
			p_PLine->Save(file1, file2, 0);
			m_PLineArray.Add(p_PLine);
		}
		for (int i = 0; i < n_GroundNumb; i++)          //添加文本
		{
			CC_Brother_Ground* p_Ground = new CC_Brother_Ground();
			p_Ground->Save(file1, file2, 0);
			m_GroundArray.Add(p_Ground);
		}
	}
	
	SetModifiedFlag(0);            //文件刚被打开 设置修改标志为0 表示未被修改
	n_CurUnIndex = 0;
	CDocument::Serialize(ar);
}

void CCCBrotherDrawDoc::DeleteContents()
{
	// TODO: Add your specialized code here and/or call the base class

	int nn = 0;
	for (int i = 1; i <= 10; i++)
	{
		nn = GetGraphUpperBound(i) + 1;
		while (nn--)
		{
				delete GetGraph(i, nn);
		}
	}
	m_LineArray.RemoveAll();
	m_PLineArray.RemoveAll();
	m_SwitchArray.RemoveAll();
	m_TextArray.RemoveAll();
	m_GroundArray.RemoveAll();
	//int nn = m_LineArray.GetSize();
	//while (nn--)
	//	delete m_LineArray.GetAt(nn);
	//m_LineArray.RemoveAll();
	//nn = m_PLineArray.GetSize();
	//while (nn--)
	//	delete m_PLineArray.GetAt(nn);
	//m_PLineArray.RemoveAll();
	//nn = m_SwitchArray.GetSize();
	//while (nn--)
	//	delete m_SwitchArray.GetAt(nn);
	//m_SwitchArray.RemoveAll();
	//nn = m_TextArray.GetSize();
	//while (nn--)
	//	delete m_TextArray.GetAt(nn);
	//m_TextArray.RemoveAll();

	CDocument::DeleteContents();
}

void CCCBrotherDrawDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class
	CDocument::OnCloseDocument();
}

BOOL CCCBrotherDrawDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	firstPrinting = 0;
	// TODO: Add your specialized creation code here
	return TRUE;
}

BOOL CCCBrotherDrawDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	return CDocument::OnSaveDocument(lpszPathName);
}

//增添的成员函数的实现
baseShape* CCCBrotherDrawDoc::GetGraph(short Lb, int Index)
{
	switch (Lb)
	{
	case 1:
		if (Index<0 || Index>m_LineArray.GetUpperBound())
			return 0;
		return m_LineArray.GetAt(Index);
		break;
	case 2:
		if (Index<0 || Index>m_TextArray.GetUpperBound())
			return 0;
		return m_TextArray.GetAt(Index);
		break;
	case 3:
		if (Index<0 || Index>m_SwitchArray.GetUpperBound())
			return 0;
		return m_SwitchArray.GetAt(Index);
		break;
	case 4:
		if (Index<0 || Index>m_PLineArray.GetUpperBound())
			return 0;
		return m_PLineArray.GetAt(Index);
		break;
	case 6:
		if (Index<0 || Index>m_GroundArray.GetUpperBound())
			return 0;
		return m_GroundArray.GetAt(Index);
		//break;
	default:
		return 0;
	}
}

void CCCBrotherDrawDoc::DeleteGraph(short Lb, int Index)                 //可以不需要这个函数 因为在操作过程中 删除操作并不会真正的删除内存中的对象
{
	switch (Lb)
	{
	case 1:
		if (Index<0 || Index>m_LineArray.GetUpperBound())
			return;
		return m_LineArray.RemoveAt(Index);  //这里的RemoveAt()返回值类型是void
		//break;
	case 2:
		if (Index<0 || Index>m_TextArray.GetUpperBound())
			return;
		return m_TextArray.RemoveAt(Index);  //这里的RemoveAt()返回值类型是void
		//break;
	case 3:
		if (Index<0 || Index>m_SwitchArray.GetUpperBound())
			return;
		return m_SwitchArray.RemoveAt(Index);  //这里的RemoveAt()返回值类型是void
		//break;
	case 4:
		if (Index<0 || Index>m_PLineArray.GetUpperBound())
			return;
		return m_PLineArray.RemoveAt(Index);
	case 6:
		if (Index<0 || Index>m_GroundArray.GetUpperBound())
			return;
		return m_GroundArray.RemoveAt(Index);
		//break;
	default:
		return;
	}
}

int CCCBrotherDrawDoc::GetGraphNum(short Lb)
{
	switch (Lb)
	{
	case 1:
		return m_LineArray.GetSize();
	case 2:
		return m_TextArray.GetSize();
	case 3:
		return m_SwitchArray.GetSize();
	case 4:
		return m_PLineArray.GetSize();
	case 6:
		return m_GroundArray.GetSize();
	default:
		return 0;          //只能返回0  不能反悔其他任何值  根据我的设计需要
	}
}

int CCCBrotherDrawDoc::GetGraphUpperBound(short Lb)
{
	switch (Lb)
	{
	case 1:
		return m_LineArray.GetUpperBound();
	case 2:
		return m_TextArray.GetUpperBound();
	case 3:
		return m_SwitchArray.GetUpperBound();
	case 4:
		return m_PLineArray.GetUpperBound();
	case 6:
		return  m_GroundArray.GetUpperBound();
	default:
		return -1;
	}
}

void CCCBrotherDrawDoc::ReDraw(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor)
{
	for (int i = 1; i <= 10; i++)
	{
		int nn = GetGraphNum(i);
		while (nn--) GetGraph(i, nn)->DrawShape(pDC, m_DrawMode, m_DrawMode1, BackColor);
	}
	for (int i = 0; i < n_GraphSelect; i++)
	{
		DrawSelectedGraph(pDC, GraphSelect[i].Lb, GraphSelect[i].Index, 0, 1, BackColor);
	}
}

//void CCCBrotherDrawDoc::tmp(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor)
//{
//	for (int i = 0; i < n_GraphSelect; i++)
//	{
//		DrawSelectedGraph(pDC, GraphSelect[i].Lb, GraphSelect[i].Index, 0, 1, BackColor);
//	}CCCBrotherDrawDoc::GetGraph(...) 返回 nullptr。
//}

void CCCBrotherDrawDoc::DrawSelectedGraph(CDC* pDC, int Lb, int Index, int m_DrawMode, int m_DrawMode1, long BackColor)
{
	GetGraph(Lb, Index)->DrawShape(pDC, m_DrawMode, m_DrawMode1, BackColor);
}

int CCCBrotherDrawDoc::GetGraphID(short Lb)
{
	int n = GetGraphUpperBound(Lb);
	for (int i = 0; i < 10000; i++)
		m_OnlyIndex[i] = 0;
	for (int i = 0; i < n; i++)
	{
		if (GetGraph(Lb, i))
			m_OnlyIndex[GetGraph(Lb, i)->GetID()] = 1;
	}
	for (int i = 0; i < 10000; i++)
	{
		if (m_OnlyIndex[i] == 0)
			return i;
	}
	return -1;
}

CC_Brother_Text* CCCBrotherDrawDoc::AddText(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, BOOL Delete,
	float xStart, float yStart, float angle1, float angle2, float TextHeight, float TextWide, float cellWide, unsigned char TextFont, CString Text)
{
	CC_Brother_Text* p_Text = new CC_Brother_Text(ColorPen, ColorBrush, LineWide, LineType, Layer, id_only, Delete,
		xStart, yStart, angle1, angle2, TextHeight, TextWide, cellWide, TextFont, Text);
	m_TextArray.Add(p_Text);
	return p_Text;
}

CC_Brother_Text* CCCBrotherDrawDoc::AddText1(CC_Brother_Text* ent)
{
	m_TextArray.Add(ent);
	return ent;
}

CC_Brother_Text* CCCBrotherDrawDoc::GetText(int Index)
{
	if (Index<0 || Index>m_TextArray.GetUpperBound())
		return 0;
	return m_TextArray.GetAt(Index);
}

CC_Brother_Line* CCCBrotherDrawDoc::AddLine(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, float X1, float Y1, float X2, float Y2)
{
	CC_Brother_Line* p_Line = new CC_Brother_Line(ColorPen, ColorBrush, LineWide, LineType, Layer, id_only, 0, X1, Y1, X2, Y2);
	m_LineArray.Add(p_Line);
	return p_Line;
}

CC_Brother_Line * CCCBrotherDrawDoc::GetLine(int Index)
{
	if (Index<0 || Index>m_LineArray.GetUpperBound())
		return 0;
	return m_LineArray.GetAt(Index);

}

CC_Brother_Switch* CCCBrotherDrawDoc::AddSwitch(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, float X, float Y)
{
	CC_Brother_Switch* pSwitch = new CC_Brother_Switch(ColorPen, ColorBrush, LineWide, LineType, Layer, id_only,0,X,Y);
	m_SwitchArray.Add(pSwitch);
	return pSwitch;
}

CC_Brother_Switch* CCCBrotherDrawDoc::GetSwitch(int Index)
{
	if (Index<0 || Index>m_SwitchArray.GetUpperBound())
		return 0;
	return m_SwitchArray.GetAt(Index);
}

void CCCBrotherDrawDoc::AddScreen(float StartX, float StartY, float blc)
{
	int i;
	if (m_CurrentScreen == m_MaxScreen)
	{
		for (i = 1; i < m_MaxScreen - 1; i++)
		{
			m_Screen[i] = m_Screen[i + 1];
		}
	}
	else
		m_CurrentScreen++;
	m_Screen[m_CurrentScreen].blc = blc;
	m_Screen[m_CurrentScreen].startX = StartX;
	m_Screen[m_CurrentScreen].startY = StartY;
}

bool CCCBrotherDrawDoc::GetRect(float* x1, float* y1, float* x2, float* y2)
{
	float xx1 = -1, yy1 = -1, xx2 = -1, yy2 = -1;
	//bool pb = 0;
	*x1 = *y1 = 1E+20;
	*x2 = *y2 = 1E-20;
	for (int i = 1; i <= 10; i++)     //!!!!!!!!!! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{
		int nn = GetGraphUpperBound(i) + 1;
		while (nn--)
		{
			if (GetGraph(i, nn))
				GetGraph(i, nn)->GetRect(&xx1, &yy1, &xx2, &yy2);
			else 
				continue;
			//pb = 1;                //表示能够计算出矩形的边界
			*x1 = min(*x1, xx1);
			*y1 = min(*y1, yy1);
			*x2 = max(*x2, xx2);
			*y2 = max(*y2, yy2);
		}
	}
	if (xx1 != -1 && yy1 != -1 && xx2 != -1 && yy2 != -1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	//return pb;
}

CC_Brother_PLine* CCCBrotherDrawDoc::AddPLine(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, int Number, PointStruct* List, bool fill)
{
	CC_Brother_PLine* pLine = new CC_Brother_PLine(ColorPen, ColorBrush, LineWide, LineType, Layer, id_only, 0, Number, List, fill);
	m_PLineArray.Add(pLine);
	return pLine;
}

void CCCBrotherDrawDoc::AddPLine(CC_Brother_PLine* ent)
{
	m_PLineArray.Add(ent);
//	return ent;
}

CC_Brother_PLine* CCCBrotherDrawDoc::GetPLine(int Index)
{
	if (Index<0 || Index>m_PLineArray.GetUpperBound())
		return 0;
	return m_PLineArray.GetAt(Index);
}

CC_Brother_Ground* CCCBrotherDrawDoc::AddGround(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, float X, float Y)
{
	CC_Brother_Ground* pGround = new CC_Brother_Ground(ColorPen, ColorBrush, LineWide, LineType, Layer, id_only, 0, X, Y);
	m_GroundArray.Add(pGround);
	return pGround;
}


CC_Brother_Ground* CCCBrotherDrawDoc::GetGround(int Index)
{
	if (Index<0 || Index>m_GroundArray.GetUpperBound())
		return 0;
	return m_GroundArray.GetAt(Index);
}

bool CCCBrotherDrawDoc::AddSelectList(int Lb, int Index, int Id_Only)
{
	for (int i = 0; i < n_GraphSelect; i++)
	{
		if (Lb == GraphSelect[i].Lb&&Index == GraphSelect[i].Index&&Id_Only == GraphSelect[i].id_only)
			return false;
	}
	GraphSelect[n_GraphSelect].Lb = Lb;
	GraphSelect[n_GraphSelect].Index = Index;
	GraphSelect[n_GraphSelect++].id_only = Id_Only;
	return true;
}

bool CCCBrotherDrawDoc::PointSelect(float x, float y, float validDistance, float blc, int* Lb, int* Index, int* id_only)
{
	for (int i = 1; i <= 10; i++)           //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{
		int nn = GetGraphUpperBound(i) + 1;
		while (nn--)
		{
			if (GetGraph(i, nn)->PointIsCount(x, y, validDistance, blc))
			{
				*Index = nn;
				*Lb = i;
				*id_only = GetGraph(i, nn)->GetID();
				return true;
			}
		}
	}
	return false;
}
//---...---//
/*这个函数明显还可以被优化 这是我移植的代码 一次全部遍历只能返回一个图形元素的信息 
其实可以传一个数组参数到这里 一次遍历就可以吧所有被选中的图形元素信息存到其中 这样只需要遍历一次 性能大大提高*/

bool CCCBrotherDrawDoc::AddUndo(int Lb, int Numble, GraphSelectStruct * GraphList)
{
	long l;									//请注意这里为什么要使用long类型！！！
	l = m_UndoList[n_CurUnIndex].l_Start;	//得到在文件中的起点
	Fundo.Seek(l, CFile::begin);
	for (int i = 0; i < Numble; i++)
	{
		Fundo.Write((unsigned char *)&GraphList[i], sizeof(GraphList[i]));
	}
	m_UndoList[n_CurUnIndex].Lb = Lb;	//判别是什么操作   记录操作的类型
	m_UndoList[n_CurUnIndex].Index = Numble;
	n_CurUnIndex++;
	m_UndoList[n_CurUnIndex].l_Start = l + sizeof(GraphList[0])*Numble;
	SetModifiedFlag(1);
	return TRUE;
}

void CCCBrotherDrawDoc::ClearGraphSelect()
{
	//delete[] GraphSelect;
	//GraphSelect = new GraphSelectStruct[1000];
	n_GraphSelect = 0;
}

void CCCBrotherDrawDoc::OnEditUndo()
{
	int i, Numble;
	if (n_CurUnIndex <= 0)
	{
		return;
	}
	if (m_UndoList[n_CurUnIndex - 1].Lb == 2)//如果原来是增加操作
	{
		Numble = m_UndoList[n_CurUnIndex - 1].Index;
		Fundo.Seek(m_UndoList[n_CurUnIndex - 1].l_Start, 0);
		for (i = 0; i < Numble; i++)
			Fundo.Read((unsigned char *)&GraphSelect[i], sizeof(GraphSelect[i]));
		for (i = Numble - 1; i >= 0; i--)
		{
			int Lb = GraphSelect[i].Lb;
			if (Lb == 1)
			{
				delete GetLine(GraphSelect[i].Index);
				m_LineArray.RemoveAt(GraphSelect[i].Index);
			}
			else if (Lb == 2)
			{
				delete GetText(GraphSelect[i].Index);
				m_TextArray.RemoveAt(GraphSelect[i].Index);
			}
			else if (Lb == 3)
			{
				delete GetSwitch(GraphSelect[i].Index);
				m_SwitchArray.RemoveAt(GraphSelect[i].Index);
			}
			else if (Lb == 4)
			{
				delete GetPLine(GraphSelect[i].Index);
				m_PLineArray.RemoveAt(GraphSelect[i].Index);
			}
			else if (Lb == 6)
			{
				delete GetGround(GraphSelect[i].Index);
				m_GroundArray.RemoveAt(GraphSelect[i].Index);
			}
			else
			{
				exit(12);           //文件读取异常
			}
		}
	}
	if (m_UndoList[n_CurUnIndex - 1].Lb == 3)//如果原来是删除操作
	{
		Numble = m_UndoList[n_CurUnIndex - 1].Index;
		Fundo.Seek(m_UndoList[n_CurUnIndex - 1].l_Start, 0);
		for (i = 0; i < Numble; i++)
		{
			Fundo.Read((unsigned char *)&GraphSelect[i], sizeof(GraphSelect[i]));
			int Lb = GraphSelect[i].Lb;
			if (Lb == 1)
			{
				GetLine(GraphSelect[i].Index)->Delete(0);
			}
			else if (Lb == 2)
			{
				GetText(GraphSelect[i].Index)->Delete(0);
			}
			else if (Lb == 3)
			{
				GetSwitch(GraphSelect[i].Index)->Delete(0);
			}
			else if (Lb == 4)
			{
				GetPLine(GraphSelect[i].Index)->Delete(0);
			}
			else if (Lb == 6)
			{
				GetGround(GraphSelect[i].Index)->Delete(0);
			}
			else
			{
				exit(12);           //文件读取异常
			}
		}
	}
	n_CurUnIndex--;
	if (n_CurUnIndex == 0)
	{
		SetModifiedFlag(0);
	}
	UpdateAllViews(NULL);
}

void init()
{
	baseCount = 0, lineCount = 0, textCount = 0, switchCount = 0, plineCount = 0, groundCount = 0;
}


//void CCCBrotherDrawDoc::DrawRegularLineBetweenTwoComponents(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor)
//{
//	baseShape* shape0 = GetGraph(GraphSelect[0].Lb, GraphSelect[0].Index);
//	baseShape* shape1 = GetGraph(GraphSelect[1].Lb, GraphSelect[1].Index);
//	POINT p0 = shape0->GetPoint(char c);
//	POINT p1 = shape1->GetPoint(char c);
//	//AddPLine()
//
//	ClearGraphSelect();
//}

//ScreenStructForBoost* CCCBrotherDrawDoc::getScreen()
//{
//	return m_Screen;
//}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CCCBrotherDrawDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CCCBrotherDrawDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CCCBrotherDrawDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCCBrotherDrawDoc 诊断

#ifdef _DEBUG
void CCCBrotherDrawDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CCCBrotherDrawDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG


// CCCBrotherDrawDoc 命令
void CCCBrotherDrawDoc::OnUpdateSelectClear(CCmdUI* pCmdUI)
{
	if (n_GraphSelect > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CCCBrotherDrawDoc::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	if (n_CurUnIndex > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}