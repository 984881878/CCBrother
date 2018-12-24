#pragma once
#include "baseShape.h"

struct PointStruct;

class CC_Brother_Line :
	public baseShape
{
protected:
	DECLARE_SERIAL(CC_Brother_Line);		//声明串形化

public:
	CC_Brother_Line();
	~CC_Brother_Line();
	CC_Brother_Line(long ColorPen,  //笔色
		long ColorBrush, //
		float LineWide,
		short LineType,
		short Layer,
		int   id_only,
		BOOL Delete,
		float X1,
		float Y1,
		float X2,
		float Y2);

//继承于baseShape  重写父类的三个纯虚函数
public:
	virtual void DrawShape(CDC* pDc, int m_DrawMode, int m_DrawMode1, long BackColor);
	virtual bool PointIsCount(float x, float y, float validDistance, float blc);
	virtual void GetRect(float* x1, float* y1, float* x2, float* y2);
	virtual PointStruct GetPoint(char c);
	virtual void Serialize(CArchive& ar);	//串形化函数
	virtual void Save(CFile* file, BOOL Yn);

protected:
	float m_X1, m_Y1, m_X2, m_Y2;     //区别于父类的特有参数  确定直线的两个坐标点

};

