#pragma once
#include <afx.h>

struct PointStruct;

class baseShape :
	public CObject
{
public:
	baseShape();
	~baseShape();
	baseShape(long ColorPen,  //��ɫ
		long ColorBrush, //��ˢ��ɫ
	float LineWide,   //�߿�
	short LineType,   //����
	short Layer,      //����ͼ��
	int   id_only,    //ͼ�ε�Ψһ��ʶ��
	BOOL Delete);     //�Ƿ���Ϊɾ��״̬

	float DistanceFromPointToLine(float xx, float yy, float x1, float y1, float x2, float y2);  //�㵽ֱ�ߵľ���
	float CalDistPToP(float x1, float y1, float x2, float y2);                                  //���������ľ���
	int GetID();                                                                                //���ö�̬�Ի�ȡ����ͼ�ε�Ψһ��ʶ��  ���������������඼����ͬһ������
	bool PointInRgn(float x, float y, int num, POINT* PointList, float blc);                    //�ж�����x,y�Ƿ��ڲ�����ȷ���Ķ����������
	void Delete(bool Is);	                                                                    //ɾ����ָ�ɾ��ͼ��Ԫ�صĺ���
	bool IsDelete();
	void setColorPen(long color);
	//BOOL getDlelete();
	void setDelete(BOOL b);

//�麯��
public:
	virtual void DrawShape(CDC* pDc,int m_DrawMode,int m_DrawMode1,long BackColor) = 0;      //��ͼ����
	virtual bool PointIsCount(float x, float y, float validDistance, float blc) = 0;          //�ж�һ���Ƿ��ڼ���������
	virtual void GetRect(float* x1, float* y1, float* x2, float* y2) = 0;                     //��ȡĳһ��ͼ�εľ�������  ������������ǰ�����ͼ�ε���С����
	virtual PointStruct GetPoint(char c) = 0;                                                            //��ȡͼ�εĻ���
	virtual void Serialize(CArchive& ar);
	virtual void Save(CFile* file, CStdioFile* file1, BOOL Yn);

protected:
	long m_ColorPen;  //��ɫ
	long m_ColorBrush; //
	float m_LineWide;
	short m_LineType;
	short m_Layer;
	int   m_id_only;
	BOOL b_Delete;

};

