#include "stdafx.h"
#include "CC_Brother_PLine.h"
#include "CC_Brother_GraphPara.h"
#include "CC_Brother_Switch.h"
#include "CC_Brother_DrawDoc.h"

extern void  DeviceP_To_LogicalP(float x, float y, int *X, int *Y); //设备坐标点映射到逻辑坐标点 P代表Point
extern void  LogicalP_To_DeviceP(int x, int y, float *X, float*Y);
extern int   DeviceL_To_LogicalL(float length);     //设备上的长度转换为逻辑长度 L代表length
extern float LogicalL_To_DeviceL(int length);
extern CC_Brother_GraphPara* p_GraphPara;
extern CCCBrotherDrawDoc* ptr_DrawDoc;

IMPLEMENT_SERIAL(CC_Brother_PLine, CObject, 1)

CC_Brother_PLine::CC_Brother_PLine()
{
	m_Number = 0;
}


CC_Brother_PLine::~CC_Brother_PLine()
{
	if (m_Number > 0)       //只有当m_Number>0时才需要释放内存
		delete[] m_PointList;
}

BOOL CC_Brother_PLine::IsPLine()
{
	return !b_fill;
}

void CC_Brother_PLine::LineFromSwitchToSwitch(CCCBrotherDrawDoc* pDoc, short int lb0, short int index0, short int lb1, short int index1)
{
	//初始化
	Lb0 = lb0, Lb1 = lb1, Index0 = index0, Index1 = index1;
	if (m_Number < 4)
	{
		delete[] m_PointList;
		m_Number = 4;
		m_PointList = new PointStruct[m_Number];
	}
	if (Lb0&&Lb1)
	{
		shape0 = pDoc->GetGraph(Lb0, Index0);
		shape1 = pDoc->GetGraph(Lb1, Index1);
	}
	CC_Brother_Switch* g0 = (CC_Brother_Switch*)shape0;
	CC_Brother_Switch* g1 = (CC_Brother_Switch*)shape1;

	PointStruct p0 = shape0->GetPoint('L');
	PointStruct p1 = shape1->GetPoint('L');


	if (p0.x > p1.x)
	{
		p0 = shape0->GetPoint('R');
		p1 = shape1->GetPoint('R');
		m_PointList[0].x = p0.x , m_PointList[0].y = p0.y;
		m_PointList[1].x = p0.x + 10, m_PointList[1].y = p0.y;
		m_PointList[2].x = p0.x + 10, m_PointList[2].y = p1.y;
		m_PointList[3].x = p1.x - 1, m_PointList[3].y = p1.y;
		/*m_PointList[3] = p1;*/
		//list[3].x = p1.x - 1, list[3].y = p1.y;	    //这个bug修复的有点无厘头
	}
	else
	{
		m_PointList[0].x = p0.x, m_PointList[0].y = p0.y;
		m_PointList[1].x = p0.x - 10, m_PointList[1].y = p0.y;
		m_PointList[2].x = p0.x - 10, m_PointList[2].y = p1.y;
		m_PointList[3].x = p1.x, m_PointList[3].y = p1.y;
	}
}

CC_Brother_PLine::CC_Brother_PLine(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, BOOL Delete, int Number, PointStruct* List, bool fill)
	:baseShape(ColorPen, ColorBrush, LineWide, LineType, Layer, id_only, Delete)
{
	m_Number = Number;
	b_fill = fill;
	m_PointList = new PointStruct[Number + 1];  //初始化连续直线或者多边形区域每一个顶点坐标
	if (Number > 0)
	{
		for (int i = 0; i < Number; i++)
		{
			m_PointList[i] = List[i];
		}
	}
	Lb0 = 0, Lb1 = 0, Index0 = 0, Index1 = 0;
}

void CC_Brother_PLine::DrawShape(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor)
{
	if (Lb0&&Lb1)
	{
		LineFromSwitchToSwitch(ptr_DrawDoc, Lb0, Index0, Lb1, Index1);
	}
	
	int x1, y1;
	if (b_Delete)
		return;
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return;
	}
	short LineType = m_LineType;
	short LineWide = max(1, DeviceL_To_LogicalL(m_LineWide));
	long ColorPen = m_ColorPen;
	long ColorBrush = m_ColorBrush;
	//准备画图 参数配置

	if (m_DrawMode1 == 1)//判断图形是否需要特殊显示
	{
		ColorPen = RGB(255, 0, 0);
		if (m_LineType != 2)
			LineType = 2;
		else
			LineType++;
	}
	else if (m_DrawMode1 == 2) //画笔、画刷为背景色 画出来的图形不可见 
	{
		ColorPen = BackColor;
		ColorBrush = BackColor;
	}

	CPen pen((int)LineType, LineWide, /*p_GraphPara->GetColor*/(ColorPen));
	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush brush(/*p_GraphPara->GetColor*/(m_ColorBrush));
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	CPoint *cpPoint = new CPoint[m_Number + 1];
	//PointStruct* pPoint = new PointStruct[m_Number + 1];
	CRgn rgn;
	if (m_DrawMode == 0)//设置画图模式为画笔颜色
		pDC->SetROP2(R2_COPYPEN);
	else if (m_DrawMode == 1)//设置画图颜色为画板颜色取反s
		pDC->SetROP2(R2_NOT);
	for (int i = 0; i < m_Number; i++)
	{
		DeviceP_To_LogicalP(m_PointList[i].x, m_PointList[i].y, &x1, &y1);
		//pPoint[i].x = x1;
		//pPoint[i].y = y1;          //初始化连续直线或者多边形区域每一个顶点坐标
		cpPoint[i].x = x1;
		cpPoint[i].y = y1;
	}
	if (!b_fill)              //判断是连续直线
	{
		cpPoint[m_Number] = cpPoint[0];
		pDC->MoveTo(cpPoint[0].x, cpPoint[0].y);
		for (int i = 1; i < m_Number; i++)
			pDC->LineTo(cpPoint[i].x, cpPoint[i].y);
	}
	else                    //判断是多边形
	{
		if (m_DrawMode1 == 0 || m_DrawMode1 == 2)
			pDC->Polygon(cpPoint, m_Number);       //创建多边形
		else if (m_DrawMode1 == 1)
		{
			rgn.CreatePolygonRgn(cpPoint, m_Number, ALTERNATE);  //创建虚线边框的多边形
			pDC->InvertRgn(&rgn);
		}
	}
	//delete pPoint;
	delete cpPoint;
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	//首先初始化画图资源的参数配置 保存原来的画图资源对象  初始化新的画图资源  开始画图  还原旧的画图资源  结束一次画图
}


bool CC_Brother_PLine::PointIsCount(float x, float y, float validDistance, float blc)
{
	int i;
	float xx, x1, y1, x2, y2;
	if (b_Delete)
		return FALSE;
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return false;
	}
	GetRect(&x1, &y1, &x2, &y2);  //得到每条连续直线的边界矩形
	if (!(x >= x1 - validDistance && x <= x2 + validDistance && y >= y1 - validDistance && y <= y2 + validDistance))
		return FALSE;	//如果点不在边界矩形内,则不能选中

	if (!b_fill)			//如果是连续直线
	{
		for (i = 0; i < m_Number - 1; i++)  //依次对连续直线的每条直线进行判断
		{
			xx = DistanceFromPointToLine(x, y, m_PointList[i].x, m_PointList[i].y, m_PointList[i + 1].x, m_PointList[i + 1].y);
			if (xx < validDistance)                     //如果选中
				return TRUE;
		}
	}
	else				//如果是多边形区域
	{
		CPoint *cpPoint = new CPoint[m_Number + 1];
		for (i = 0; i < m_Number; i++)
		{
			cpPoint[i].x = m_PointList[i].x;
			cpPoint[i].y = m_PointList[i].y;
		}
		delete cpPoint;
		if (PointInRgn(x, y, m_Number, cpPoint, blc))    //如果点在封闭区域内
			return TRUE;
	}
	return FALSE; //没有选中连续直线或封闭区域，返回0
}

void CC_Brother_PLine::GetRect(float *minX, float *minY, float *maxX, float *maxY)
{
	if (b_Delete || !p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		*minX = 1E+20;
		*maxX = 1E-20;
		*minY = 1E+20;
		*maxY = 1E-20;
		return;
	}
	/*if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return;
	}*/
	float x1, y1, x2, y2;
	//用第一个顶点的坐标初试化变量
	x1 = m_PointList[0].x; x2 = m_PointList[0].x;
	y1 = m_PointList[0].y; y2 = m_PointList[0].y;                //原来一直的代码中也可能有逻辑错误导致功能紊乱（并不是我的项目不兼容）
	for (int i = 1; i < m_Number; i++)          //从第二个顶点开始循环
	{
		x1 = min(x1, m_PointList[i].x);    //得到最小x坐标
		x2 = max(x2, m_PointList[i].x);    //得到最大x坐标
		y1 = min(y1, m_PointList[i].y);    //得到最小y坐标
		y2 = max(y2, m_PointList[i].y);    //得到最大y坐标
	}
	*minX = x1; *maxX = x2; *minY = y1; *maxY = y2; //将值赋给地址返回
}

PointStruct CC_Brother_PLine::GetPoint(char c)
{
	return PointStruct(m_PointList[0].x, m_PointList[0].y);
}

void CC_Brother_PLine::Serialize(CArchive& ar)
{
	int i;
	baseShape::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << b_fill << m_Number;
		ar << Lb0 << Index0 << Lb1 << Index1;
		for (i = 0; i < m_Number; i++)
		{
			ar << m_PointList[i].x << m_PointList[i].y;
		}
		//if (shape0&&shape1)
		//{
		//	ar.WriteCount(1);
		//	shape0->Serialize(ar);
		//	shape1->Serialize(ar);
		//}
		//else
		//{
		//	ar.WriteCount(0);
		//}
	}
	else
	{
		ar >> b_fill >> m_Number;
		ar >> Lb0 >> Index0 >> Lb1 >> Index1;
		m_PointList = new PointStruct[m_Number];
		for (i = 0; i < m_Number; i++)
		{
			ar >> m_PointList[i].x >> m_PointList[i].y;
		}
		//baseShape *shape0, *shape1;
		/*shape0 = (baseShape*)ar.ReadObject(RUNTIME_CLASS(baseShape));
		shape1 = (baseShape*)ar.ReadObject(RUNTIME_CLASS(baseShape));*/
		//ar >> shape0 >> shape1;
		//if (ar.ReadCount())
		//{
		//	shape0->Serialize(ar);
		//	shape1->Serialize(ar);
		//}
		//else
		//{
		//	//空操作
		//}
	}
}

void CC_Brother_PLine::Save(CFile* file, BOOL Yn)
{
	int i;
	baseShape::Save(file, Yn);
	if (Yn)
	{
		file->Write((unsigned char *)&b_fill, sizeof(b_fill));
		file->Write((unsigned char *)&m_Number, sizeof(m_Number));
		for (i = 0; i < m_Number; i++)
		{
			file->Write((unsigned char *)&m_PointList[i], sizeof(m_PointList[i]));
		}
	}
	else
	{
		file->Read((unsigned char *)&b_fill, sizeof(b_fill));
		file->Read((unsigned char *)&m_Number, sizeof(m_Number));
		m_PointList = new PointStruct[m_Number];
		for (i = 0; i < m_Number; i++)
		{
			file->Read((unsigned char *)&m_PointList[i], sizeof(m_PointList[i]));
		}
	}
}