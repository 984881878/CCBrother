#pragma once
#include "baseShape.h"

struct PointStruct;

class CC_Brother_Switch :
	public baseShape
{
protected:
	DECLARE_SERIAL(CC_Brother_Switch)
public:
	CC_Brother_Switch();
	CC_Brother_Switch(long ColorPen,  //��ɫ
		long ColorBrush, //
		float LineWide,
		short LineType,
		short Layer,
		int   id_only,
		BOOL Delete,
		float X1,
		float Y1);
	~CC_Brother_Switch();
	int getSizeOfSwitch();
	void setPointInc(float x, float y);        //�ṩͼ���ƶ� �����֧��

	//�̳���baseShape  ��д������������麯��
	virtual bool PointIsCount(float x, float y, float validDistance, float blc);
	virtual void GetRect(float* x1, float* y1, float* x2, float* y2);
	virtual void DrawShape(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor);
	virtual PointStruct GetPoint(char c);
	virtual void Serialize(CArchive& ar);
	virtual void Save(CFile* file, BOOL Yn);

//�����ڸ�������в���
protected:
	float X = 0, Y = 0, X1 = 0, Y1 = 0, X2 = 0, Y2 = 0, X3 = 0, Y3 = 0, X4 = 0, Y4 = 0;
	//bool left = 0, right = 0, up = 0, down = 0;
	int size_Of_Switch = 10;
};

