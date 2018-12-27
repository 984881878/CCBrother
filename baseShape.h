#pragma once
#include <afx.h>

struct PointStruct;

class baseShape :
	public CObject
{
public:
	baseShape();
	~baseShape();
	baseShape(long ColorPen,  //笔色
		long ColorBrush, //画刷颜色
	float LineWide,   //线宽
	short LineType,   //线型
	short Layer,      //所处图层
	int   id_only,    //图形的唯一标识号
	BOOL Delete);     //是否标记为删除状态

	float DistanceFromPointToLine(float xx, float yy, float x1, float y1, float x2, float y2);  //点到直线的距离
	float CalDistPToP(float x1, float y1, float x2, float y2);                                  //计算两点间的距离
	int GetID();                                                                                //利用多态性获取子类图形的唯一标识符  并且它的所有子类都共用同一个函数
	bool PointInRgn(float x, float y, int num, POINT* PointList, float blc);                    //判断坐标x,y是否在参数所确定的多边形区域内
	void Delete(bool Is);	                                                                    //删除或恢复删除图形元素的函数
	bool IsDelete();
	void setColorPen(long color);
	//BOOL getDlelete();
	void setDelete(BOOL b);

//虚函数
public:
	virtual void DrawShape(CDC* pDc,int m_DrawMode,int m_DrawMode1,long BackColor) = 0;      //画图函数
	virtual bool PointIsCount(float x, float y, float validDistance, float blc) = 0;          //判断一点是否在计算区域内
	virtual void GetRect(float* x1, float* y1, float* x2, float* y2) = 0;                     //获取某一个图形的矩形区域  这个矩形区域是包括该图形的最小矩形
	virtual PointStruct GetPoint(char c) = 0;                                                            //获取图形的基点
	virtual void Serialize(CArchive& ar);
	virtual void Save(CFile* file, CStdioFile* file1, BOOL Yn);

protected:
	long m_ColorPen;  //笔色
	long m_ColorBrush; //
	float m_LineWide;
	short m_LineType;
	short m_Layer;
	int   m_id_only;
	BOOL b_Delete;

};

