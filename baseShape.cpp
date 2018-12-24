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
	m_ColorPen = ColorPen;  //��ɫ
	m_ColorBrush = ColorBrush; //
	m_LineWide = LineWide;
	m_LineType = LineType;
	//m_xScale = xScale;/*���ͺ������*/
	m_Layer = Layer;
	m_id_only = id_only;
	b_Delete = Delete;
}

int baseShape::GetID()
{
	return m_id_only;
}


//�������������ڼ���㵽ֱ�ߵľ��루�ü������������������ ϸ�ڲ�̫�ñ�� �ײ�ɹ�
float baseShape::DistanceFromPointToLine(float xx, float yy, float x1, float y1, float x2, float y2)
{
	float a, b, c, ang1, ang2, ang;
	//���������ߵľ���
	a = CalDistPToP(x1, y1, xx, yy); 
	if (a == 0.0)return 0.0;
	b = CalDistPToP(x2, y2, xx, yy); 
	if (b == 0.0)return 0.0;
	c = CalDistPToP(x1, y1, x2, y2);
	//�����x1,y1)��(x2,y2)��һ����ֱ�ӷ��ؾ���
	if (c == 0.0) return a;
	if (a < b) //���(xx,yy)�ĵ�(x1,y1�������߽϶�
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
			if (y1 > y2)ang1 = (float)Pi * 2 - ang1;  //ֱ��(x1,y1)-(x2,y2)�Ļ���
		}
		ang2 = (float)acos((xx - x1) / a);
		if (y1 > yy)ang2 = (float)Pi * 2 - ang2;  //ֱ��(x1,y1)-(xx,yy)�Ļ���
		ang = ang2 - ang1;
		if (ang < 0)ang = -ang;
		if (ang > Pi) ang = (float)Pi * 2 - ang;  //���ǵĴ�С
		if (ang > Pi / 2) return a;    //���Ϊ�۽ǣ�ֱ�ӷ��ؾ���
		else
			return (a*(float)sin(ang)); //���򷵻ؼ���õ��ľ��� 
	}
	else //���(xx,yy)�ĵ�(x2,y2�������߽϶�
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
			ang1 = (float)acos((x1 - x2) / c);     //ֱ�ߣ�x2,y2)-(x1,y1)��б�ʵĻ���
			if (y2 > y1)ang1 = (float)Pi * 2 - ang1;
		}
		ang2 = (float)acos((xx - x2) / b);     //ֱ�ߣ�x2,x1��-(xx,yy)��б�ʵĻ���
		if (y2 > yy)ang2 = (float)Pi * 2 - ang2;
		ang = ang2 - ang1;
		if (ang < 0) ang = -ang;
		if (ang > Pi) ang = (float)Pi * 2 - ang;  //���ǵĴ�С ?

		if (ang > Pi / 2)  return b;   //���Ϊ�۽ǣ�ֱ�ӷ��ؾ���
		else
			return(b*(float)sin(ang));  //���򷵻ؼ���õ��ľ���
	}
	//���������ε������Ǻ�������
	//float a, b, c, ang1, ang2, ang;
	//a = CalDistPToP(x1, y1, xx, yy);
	//if (a == 0.0) return 0.0;
	//b = CalDistPToP(x2, y2, xx, yy);
	//if (b == 0.0) return 0.0;
	//c = CalDistPToP(x1, y1, x2, y2);
	//if (c == 0.0) return a;   //��ʱ�ض���a=b  ��˷���a����b������ν

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
	//			ang1 = (float)Pi * 2 - ang1;   //ע�������ang1��ֱ����ˮƽ����ļн�  �����������ε�ĳһ���ڽǣ�����
	//	}
	//	ang2 = (float)acos((xx - x1) / a);  //a��ˮƽ����ļн�
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
	//			ang1 = (float)Pi * 2 - ang1;   //ע�������ang1��ֱ����ˮƽ����ļн�  �����������ε�ĳһ���ڽǣ�����
	//	}
	//	ang2 = (float)acos((xx - x2) / b);  //a��ˮƽ����ļн�
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
		pPoint[i].y = (int)((PointList[i].y) / blc);     //��ʼ������������ÿ����������
	}
	rgn.CreatePolygonRgn(pPoint, num, ALTERNATE);     //ALTERNATE ������������� 
	i = (rgn.PtInRegion(x1, y1) != 0);                //�жϵ� �Ƿ��ڶ����������
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
	if (Yn)	//������ڽ��б���
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