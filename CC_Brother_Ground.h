#pragma once
#include "baseShape.h"

struct PointStruct;

class CC_Brother_Ground : public baseShape
{
protected:
	DECLARE_SERIAL(CC_Brother_Ground)
public:
	CC_Brother_Ground();
	CC_Brother_Ground(long ColorPen,  //笔色
		long ColorBrush, //
		float LineWide,
		short LineType,
		short Layer,
		int   id_only,
		BOOL Delete,
		float X1,
		float Y1
		);
	int getSizeOfGround();
	~CC_Brother_Ground();

	BOOL IsGround();

	//区别于父类的特有参数
protected:
	//float m_X1, m_Y1, m_X2, m_Y2;
	float X = 0, Y = 0, X1 = 0, Y1 = 0, X2 = 0, Y2 = 0, X3 = 0, Y3 = 0, X4 = 0, Y4 = 0;
	float X5 = 0, Y5 = 0, X6 = 0, Y6 = 0, X7 = 0, Y7 = 0 , X8 =0 , Y8 = 0;
	int size_Of_Ground = 10;

public:
	virtual void DrawShape(CDC* pDc, int m_DrawMode, int m_DrawMode1, long BackColor);
	virtual bool PointIsCount(float x, float y, float validDistance, float blc);
	virtual void GetRect(float* x1, float* y1, float* x2, float* y2);
	virtual PointStruct GetPoint(char c);
	virtual void Serialize(CArchive& ar);

};

