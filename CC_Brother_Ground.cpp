#include "stdafx.h"
#include "CC_Brother_Ground.h"
#include "CC_Brother_GraphPara.h"


extern void  DeviceP_To_LogicalP(float x, float y, int *X, int *Y); //设备坐标点映射到逻辑坐标点 P代表Point
extern void  LogicalP_To_DeviceP(int x, int y, float *X, float*Y);
extern int   DeviceL_To_LogicalL(float length);     //设备上的长度转换为逻辑长度 L代表length
extern float LogicalL_To_DeviceL(int length);
extern CC_Brother_GraphPara* p_GraphPara;

IMPLEMENT_SERIAL(CC_Brother_Ground, CObject, 1)

CC_Brother_Ground::CC_Brother_Ground()
{
}

CC_Brother_Ground::CC_Brother_Ground(
	long ColorPen,  //笔色
	long ColorBrush, //
	float LineWide,
	short LineType,
	short Layer,
	int   id_only,
	BOOL Delete,
	float X1,
	float Y1
	)
	:baseShape(ColorPen, ColorBrush, LineWide, LineType, Layer, id_only, Delete)
{
	X = X1;
	Y = Y1;
}


int CC_Brother_Ground::getSizeOfGround()
{
	return size_Of_Ground;
}

CC_Brother_Ground::~CC_Brother_Ground()
{
}

BOOL CC_Brother_Ground::IsGround()
{
	return 0;
}

void CC_Brother_Ground::DrawShape(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor)
{
	int x, y, x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, x7, y7, x8, y8;
	if (b_Delete)
	{
		return;
	}
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return;
	}
	short LineType = m_LineType;
	short LineWide = max(1, DeviceL_To_LogicalL(m_LineWide)); //
	long ColorPen = m_ColorPen;
	if (m_DrawMode1 == 1)  //判断图形是否需要特殊显示
	{
		if (m_LineType != 2)
			LineType = 2;
		else
			LineType++;
		ColorPen = RGB(255, 0, 0);
	}
	if (m_DrawMode1 == 2)  //画笔为背景色 画出来的图形不可见 
		ColorPen = BackColor;
	CPen pen((int)LineType, LineWide, /*p_GraphPara->GetColor*/(ColorPen));
	CPen* pOldPen = pDC->SelectObject(&pen);
	if (m_DrawMode == 0)           //设置画图模式为画笔颜色
		pDC->SetROP2(R2_COPYPEN);
	else if (m_DrawMode == 1)         //设置画图颜色为画板颜色取反s
		pDC->SetROP2(R2_NOT);

	X1 = X + size_Of_Ground;
	Y1 = Y;
	X2 = X + size_Of_Ground;
	Y2 = Y + size_Of_Ground;//这里可能需要根据映射模式稍加修改  在六种映射模式中 判断使用的是哪一种模式 从而得出是 '+' 还是 '-' 不过一般的都是 '-'
	X3 = X2;
	Y3 = Y;
	X4 = X3 + size_Of_Ground;
	Y4 = Y;               //
	X5 = (X + X1) / 2;
	Y5 = Y - 3;
	X6 = (X1 + X4) / 2;
	Y6 = Y - 3;
	X7 = (X5 + X1) / 2;
	Y7 = Y - 6;
	X8 = (X6 + X1) / 2;
	Y8 = Y - 6;


	DeviceP_To_LogicalP(X, Y, &x, &y);    //在这之前可以获取当前的映射模式 Get....  设置成员变量m_MapMode
	DeviceP_To_LogicalP(X1, Y1, &x1, &y1);
	DeviceP_To_LogicalP(X2, Y2, &x2, &y2);
	/*DeviceP_To_LogicalP(X3, Y3, &x3, &y3);*/
	DeviceP_To_LogicalP(X4, Y4, &x4, &y4);  //将设备坐标转换为逻辑坐标
	DeviceP_To_LogicalP(X5, Y5, &x5, &y5);
	DeviceP_To_LogicalP(X6, Y6, &x6, &y6);
	DeviceP_To_LogicalP(X7, Y7, &x7, &y7);
	DeviceP_To_LogicalP(X8, Y8, &x8, &y8);

	pDC->MoveTo(x, y);
	pDC->LineTo(x4, y4);
	pDC->MoveTo(x2, y2);
	pDC->LineTo(x1, y1);

	pDC->MoveTo(x5, y5);
	pDC->LineTo(x6, y6);
	pDC->MoveTo(x7, y7);
	pDC->LineTo(x8, y8);

	pDC->SelectObject(pOldPen);
}

bool CC_Brother_Ground::PointIsCount(float x, float y, float validDistance, float blc)
{
	float xx, x1, x2, y1, y2;
	if (b_Delete)
	{
		return false;
	}
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return false;
	}
	GetRect(&x1, &y1, &x2, &y2);
	if (!(x >= x1 && x <= x2 && y >= y1 && y <= y2))
		return false;
	return true;
}

void CC_Brother_Ground::GetRect(float * x1, float * y1, float * x2, float * y2)
{
	if (b_Delete || !p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		*x1 = 1E+20;
		*x2 = 1E-20;
		*y1 = 1E+20;
		*y2 = 1E-20;
		return;
	}
	//if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	//{
	//	return;
	//}
	*x1 = X;
	*x2 = X4;
	*y1 = Y - 6;
	*y2 = Y + size_Of_Ground;
}

PointStruct CC_Brother_Ground::GetPoint(char c)
{
	if (c == 'G')
		return PointStruct(X, Y);
	else
		return PointStruct(X4, Y4);
}

void CC_Brother_Ground::Serialize(CArchive & ar)
{
	baseShape::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << X << Y << X1 << Y1 << X2 << Y2 << X3 << Y3 << X4 << Y4 << X5 << Y5 << X6 << Y6 << X7 << Y7 << X8 << Y8;
		ar << size_Of_Ground;
	}
	else
	{
		ar >> X >> Y >> X1 >> Y1 >> X2 >> Y2 >> X3 >> Y3 >> X4 >> Y4 >> X5 >> Y5 >> X6 >> Y6 >> X7 >> Y7 >> X8 >> Y8;
		ar >> size_Of_Ground;
	}
}
