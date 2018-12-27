#pragma once
#include "baseShape.h"

struct PointStruct;
class CCCBrotherDrawDoc;

class CC_Brother_PLine :
	public baseShape
{
protected:
	DECLARE_SERIAL(CC_Brother_PLine);		//�������λ�
public:
	CC_Brother_PLine();
	CC_Brother_PLine(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, BOOL Delete,int Number, PointStruct* List,bool fill);
	~CC_Brother_PLine();

	BOOL IsPLine();
	void LineFromSwitchToSwitch(CCCBrotherDrawDoc* pDoc,short int Lb0, short int Index0, short int Lb1, short int Index1);
	
//�����ڸ�������в���
protected:
	int m_Number = 0;   //����ֱ�߻��߶���εĶ�����Ŀ
	bool b_fill = 0;    //�Ƿ���� 1�����������  0������ֱ��
	PointStruct* m_PointList =nullptr;

//�����������ֱ������������Ԫ�����Ļ� ������ָ��ֱ�ָ������Ԫ��������
public:
	short int Lb0, Lb1, Index0, Index1;
	baseShape* shape0 = nullptr, *shape1 = nullptr;
	
//�̳���baseShape  ��д������������麯��
public:
	virtual void DrawShape(CDC* pDc, int m_DrawMode, int m_DrawMode1, long BackColor);
	virtual bool PointIsCount(float x, float y, float validDistance, float blc);
	virtual void GetRect(float* x1, float* y1, float* x2, float* y2);
	virtual PointStruct GetPoint(char c);
	virtual void Serialize(CArchive& ar);
	virtual void Save(CFile* file, CStdioFile* file1, BOOL Yn);
};

