#pragma once
#include "baseShape.h"

struct PointStruct;

class CC_Brother_Line :
	public baseShape
{
protected:
	DECLARE_SERIAL(CC_Brother_Line);		//�������λ�

public:
	CC_Brother_Line();
	~CC_Brother_Line();
	CC_Brother_Line(long ColorPen,  //��ɫ
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

//�̳���baseShape  ��д������������麯��
public:
	virtual void DrawShape(CDC* pDc, int m_DrawMode, int m_DrawMode1, long BackColor);
	virtual bool PointIsCount(float x, float y, float validDistance, float blc);
	virtual void GetRect(float* x1, float* y1, float* x2, float* y2);
	virtual PointStruct GetPoint(char c);
	virtual void Serialize(CArchive& ar);	//���λ�����
	virtual void Save(CFile* file, BOOL Yn);

protected:
	float m_X1, m_Y1, m_X2, m_Y2;     //�����ڸ�������в���  ȷ��ֱ�ߵ����������

};

