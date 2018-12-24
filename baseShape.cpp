#include "stdafx.h"
#include "baseShape.h"


baseShape::baseShape()
{
}


baseShape::~baseShape()
{
}

baseShape::baseShape(long ColorPen, long ColorBrush,float LineWide,short LineType,short Layer,int id_only,BOOL Delete)
{
	m_ColorPen = ColorPen;  //笔色
	m_ColorBrush = ColorBrush; //
	m_LineWide = LineWide;
	m_LineType = LineType;
	//m_xScale = xScale;/*线型横向参数*/
	m_Layer = Layer;
	m_id_only = id_only;
	b_Delete = Delete;
}

int baseShape::GetID()
{
	return m_id_only;
}


//整个函数就是在计算点到直线的距离（用计算机方方法表达出来） 细节不太好表达 亲测成功
float baseShape::DistanceFromPointToLine(float xx, float yy, float x1, float y1, float x2, float y2)
{
	float a, b, c, ang1, ang2, ang;
	//计算三条边的距离
	a = CalDistPToP(x1, y1, xx, yy); 
	if (a == 0.0)return 0.0;
	b = CalDistPToP(x2, y2, xx, yy); 
	if (b == 0.0)return 0.0;
	c = CalDistPToP(x1, y1, x2, y2);
	//如果（x1,y1)和(x2,y2)是一个点直接返回距离
	if (c == 0.0) return a;
	if (a < b) //如果(xx,yy)的点(x1,y1）这条边较短
	{
		if (y1 == y2)
		{
			if (x1 < x2)
				ang1 = 0;
			else
				ang1 = (float)Pi;
		}
		else
		{
			ang1 = (float)acos((x2 - x1) / c);
			if (y1 > y2)ang1 = (float)Pi * 2 - ang1;  //直线(x1,y1)-(x2,y2)的弧度
		}
		ang2 = (float)acos((xx - x1) / a);
		if (y1 > yy)ang2 = (float)Pi * 2 - ang2;  //直线(x1,y1)-(xx,yy)的弧度
		ang = ang2 - ang1;
		if (ang < 0)ang = -ang;
		if (ang > Pi) ang = (float)Pi * 2 - ang;  //交角的大小
		if (ang > Pi / 2) return a;    //如果为钝角，直接返回距离
		else
			return (a*(float)sin(ang)); //否则返回计算得到的距离 
	}
	else //如果(xx,yy)的点(x2,y2）这条边较短
	{
		if (y1 == y2)
		{
			if (x1 < x2)
				ang1 = (float)Pi;
			else
				ang1 = 0;
		}
		else
		{
			ang1 = (float)acos((x1 - x2) / c);     //直线（x2,y2)-(x1,y1)的斜率的弧度
			if (y2 > y1)ang1 = (float)Pi * 2 - ang1;
		}
		ang2 = (float)acos((xx - x2) / b);     //直线（x2,x1）-(xx,yy)的斜率的弧度
		if (y2 > yy)ang2 = (float)Pi * 2 - ang2;
		ang = ang2 - ang1;
		if (ang < 0) ang = -ang;
		if (ang > Pi) ang = (float)Pi * 2 - ang;  //交角的大小 ?

		if (ang > Pi / 2)  return b;   //如果为钝角，直接返回距离
		else
			return(b*(float)sin(ang));  //否则返回计算得到的距离
	}
	//计算三角形的三个角和三条边
	//float a, b, c, ang1, ang2, ang;
	//a = CalDistPToP(x1, y1, xx, yy);
	//if (a == 0.0) return 0.0;
	//b = CalDistPToP(x2, y2, xx, yy);
	//if (b == 0.0) return 0.0;
	//c = CalDistPToP(x1, y1, x2, y2);
	//if (c == 0.0) return a;   //此时必定有a=b  因此返回a或者b都无所谓

	//if (a < b)    
	//{
	//	if (y1 == y2)
	//	{
	//		if(x1<x2)
	//			ang1 = 0;
	//		else
	//			ang1 = (float)Pi;
	//	}
	//	else
	//	{
	//		ang1 = (float)acos((x2 - x1) / c);
	//		if (y1 > y2)
	//			ang1 = (float)Pi * 2 - ang1;   //注意这里的ang1是直线与水平方向的夹角  而不是三角形的某一个内角！！！
	//	}
	//	ang2 = (float)acos((xx - x1) / a);  //a与水平方向的夹角
	//	if (y1 > yy) ang2 = (float)Pi * 2 - ang2;
	//	ang = ang2 - ang1;
	//	if (ang < 0) ang = -ang;
	//	if (ang > Pi) ang = (float)Pi * 2 - ang;
	//	if (ang > Pi / 2) return a;
	//	else
	//		return (a*(float)sin(ang));
	//}
	//else
	//{
	//	if (y1 == y2)
	//	{
	//		if (x1 < x2)
	//			ang1 = 0;
	//		else
	//			ang1 = (float)Pi;
	//	}
	//	else
	//	{
	//		ang1 = (float)acos((x1 - x2) / c);
	//		if (y2 > y1)
	//			ang1 = (float)Pi * 2 - ang1;   //注意这里的ang1是直线与水平方向的夹角  而不是三角形的某一个内角！！！
	//	}
	//	ang2 = (float)acos((xx - x2) / b);  //a与水平方向的夹角
	//	if (y2 > yy) ang2 = (float)Pi * 2 - ang2;
	//	ang = ang2 - ang1;
	//	if (ang < 0) ang = -ang;
	//	if (ang > Pi) ang = (float)Pi * 2 - ang;
	//	if (ang > Pi / 2) return b;
	//	else
	//		return (b*(float)sin(ang));
	//}
}



float baseShape::CalDistPToP(float x1, float y1, float x2, float y2)
{
	return (float)sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

bool baseShape::PointInRgn(float x, float y, int num, POINT* PointList, float blc)
{
	CRgn rgn;
	int x1, y1,i;
	//bool b;
	POINT* pPoint;
	if (num < 3)
		return 0;
	pPoint = new POINT[num];
	x1 = (int)(x / blc);
	y1 = (int)(y / blc);
	for (i = 0; i < num; i++)
	{
		pPoint[i].x = (int)((PointList[i].x) / blc);
		pPoint[i].y = (int)((PointList[i].y) / blc);     //初始化多边形区域的每个顶点坐标
	}
	rgn.CreatePolygonRgn(pPoint, num, ALTERNATE);     //ALTERNATE 创建多边形区域 
	i = (rgn.PtInRegion(x1, y1) != 0);                //判断点 是否在多边形区域内
	rgn.DeleteObject();
	return i;
}

void baseShape::Delete(bool Is)
{
	if (Is)
		b_Delete = TRUE;
	else
		b_Delete = FALSE;
}

bool baseShape::IsDelete()
{
	return b_Delete;
}

void baseShape::setColorPen(long color)
{
	m_ColorPen = color;
}

void baseShape::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_ColorPen << m_ColorBrush << m_LineWide << m_LineType << m_Layer << m_id_only << b_Delete;
	}
	else
	{
		ar >> m_ColorPen >> m_ColorBrush >> m_LineWide >> m_LineType >> m_Layer >> m_id_only >> b_Delete;
	}
}

void baseShape::Save(CFile* file, BOOL Yn)
{
	if (Yn)	//如果是在进行保存
	{
		file->Write((unsigned char *)&m_ColorPen, sizeof(m_ColorPen));
		file->Write((unsigned char *)&m_ColorBrush, sizeof(m_ColorBrush));
		file->Write((unsigned char *)&m_LineWide, sizeof(m_LineWide));
		file->Write((unsigned char *)&m_LineType, sizeof(m_LineType));
		file->Write((unsigned char *)&m_Layer, sizeof(m_Layer));
		file->Write((unsigned char *)&b_Delete, sizeof(b_Delete));
	}
	else
	{
		file->Read((unsigned char *)&m_ColorPen, sizeof(m_ColorPen));
		file->Read((unsigned char *)&m_ColorBrush, sizeof(m_ColorBrush));
		file->Read((unsigned char *)&m_LineWide, sizeof(m_LineWide));
		file->Read((unsigned char *)&m_LineType, sizeof(m_LineType));
		file->Read((unsigned char *)&m_Layer, sizeof(m_Layer));
		file->Read((unsigned char *)&b_Delete, sizeof(b_Delete));
	}
}