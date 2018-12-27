#pragma once
#include "baseShape.h"

struct PointStruct;
class CCCBrotherDrawDoc;

class CC_Brother_PLine :
	public baseShape
{
protected:
	DECLARE_SERIAL(CC_Brother_PLine);		//声明串形化
public:
	CC_Brother_PLine();
	CC_Brother_PLine(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, BOOL Delete,int Number, PointStruct* List,bool fill);
	~CC_Brother_PLine();

	BOOL IsPLine();
	void LineFromSwitchToSwitch(CCCBrotherDrawDoc* pDoc,short int Lb0, short int Index0, short int Lb1, short int Index1);
	
//区别于父类的特有参数
protected:
	int m_Number = 0;   //连续直线或者多边形的顶点数目
	bool b_fill = 0;    //是否填充 1：多边形区域  0：连续直线
	PointStruct* m_PointList =nullptr;

//如果这条连续直线是连接两个元器件的话 这两个指针分别指向两个元器件对象
public:
	short int Lb0, Lb1, Index0, Index1;
	baseShape* shape0 = nullptr, *shape1 = nullptr;
	
//继承于baseShape  重写父类的三个纯虚函数
public:
	virtual void DrawShape(CDC* pDc, int m_DrawMode, int m_DrawMode1, long BackColor);
	virtual bool PointIsCount(float x, float y, float validDistance, float blc);
	virtual void GetRect(float* x1, float* y1, float* x2, float* y2);
	virtual PointStruct GetPoint(char c);
	virtual void Serialize(CArchive& ar);
	virtual void Save(CFile* file, CStdioFile* file1, BOOL Yn);
};

