#pragma once
#include "baseShape.h"

struct PointStruct;

class CC_Brother_Text :
	public baseShape
{
protected:
	DECLARE_SERIAL(CC_Brother_Text);
public:
	CC_Brother_Text();
	~CC_Brother_Text();
	CC_Brother_Text(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, BOOL Delete,
		float m_xStart, float m_yStart, float m_angle1, float m_angle2, float m_TextHeight, float m_TextWide, float m_cellWide,
		unsigned char m_TextFont, CString m_Text);
	void setPoint(float x,float y);
	void setText(CString TextContent);
	void setID(int id);
	void updateColor(long color);
	void prepare(int id, long color/*, short curLayer*/);
	void setCurLayre(short layre);
	/*void setAllmember(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, BOOL Delete,
		float m_xStart, float m_yStart, float m_angle1, float m_angle2, float m_TextHeight, float m_TextWide, float m_cellWide,
		unsigned char m_TextFont, CString m_Text);*/

	//�̳���baseShape  ��д������������麯��
	virtual void DrawShape(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor);
	virtual void GetRect(float* x1, float* y1, float* x2, float* y2);
	virtual bool PointIsCount(float x, float y, float validDistance, float blc);
	virtual PointStruct GetPoint(char c);
	virtual void Serialize(CArchive& ar);
	virtual void Save(CFile* file, CStdioFile* file1, BOOL Yn);

//�����ڸ�������в���
protected:
	float m_xStart,m_yStart;
	float m_angle1,m_angle2;  //��ע�Ƕ� ��ת�Ƕ�
	float m_TextHeight,m_TextWide;  //�������
	float m_cellWide;   //������
	unsigned char m_TextFont;  //�����ʽ�����֣�
	CString m_Text;    //�ı��ַ�������
	int m_TextLength;    //�ı�����
};

