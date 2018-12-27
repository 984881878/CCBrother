#include "stdafx.h"
#include "CC_Brother_Line.h"
#include "CC_Brother_GraphPara.h"
#include "CC_Brother_DrawDoc.h"

extern int lineCount;

extern void  DeviceP_To_LogicalP(float x, float y, int *X, int *Y); //�豸�����ӳ�䵽�߼������ P����Point
extern void  LogicalP_To_DeviceP(int x, int y, float *X, float*Y);
extern int   DeviceL_To_LogicalL(float length);     //�豸�ϵĳ���ת��Ϊ�߼����� L����length
extern float LogicalL_To_DeviceL(int length);
extern CC_Brother_GraphPara* p_GraphPara;

IMPLEMENT_SERIAL(CC_Brother_Line,CObject,1)

CC_Brother_Line::CC_Brother_Line()
{
}


CC_Brother_Line::~CC_Brother_Line()
{
}

CC_Brother_Line::CC_Brother_Line(
	long ColorPen,  //��ɫ
	long ColorBrush, //
	float LineWide,
	short LineType,
	short Layer,
	int   id_only,
	BOOL Delete,
	float X1,
	float Y1,
	float X2,
	float Y2)
	:baseShape(ColorPen, ColorBrush, LineWide, LineType, Layer, id_only, Delete)
{
	m_X1 = X1;
	m_Y1 = Y1;
	m_X2 = X2;
	m_Y2 = Y2;
}


//�麯����ʵ��
void CC_Brother_Line::DrawShape(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor)
{
	int x1, y1, x2, y2;
	if (b_Delete)
		return;
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return;
	}
	//if (m_DrawMode1 == 1)
	//	return;
	short LineType = m_LineType;
	short LineWide = max(1, DeviceL_To_LogicalL(m_LineWide)); //
	long ColorPen = m_ColorPen;
	if (m_DrawMode1 == 1)  //�ж�ͼ���Ƿ���Ҫ������ʾ
	{
		if (m_LineType != 2)
			LineType = 2;
		else
			LineType++;
		ColorPen = RGB(255, 0, 0);
	}
	if (m_DrawMode1 == 2)  //����Ϊ����ɫ ��������ͼ�β��ɼ� 
		ColorPen = BackColor;
	CPen pen((int)LineType,LineWide,/*p_GraphPara->GetColor*/(ColorPen)); 
	CPen* pOldPen= pDC->SelectObject(&pen);
	if (m_DrawMode == 0)           //���û�ͼģʽΪ������ɫ
		pDC->SetROP2(R2_COPYPEN);
	else if(m_DrawMode==1)         //���û�ͼ��ɫΪ������ɫȡ��s
		pDC->SetROP2(R2_NOT);
	DeviceP_To_LogicalP(m_X1,m_Y1,&x1,&y1);    //����֮ǰ���Ի�ȡ��ǰ��ӳ��ģʽ Get....  ���ó�Ա����m_MapMode
	DeviceP_To_LogicalP(m_X2,m_Y2,&x2,&y2);
	pDC->MoveTo(x1,y1);
	pDC->LineTo(x2,y2);
	pDC->SelectObject(pOldPen);
}

bool CC_Brother_Line::PointIsCount(float x, float y, float validDistance, float blc)
{
	float xx, x1, x2, y1, y2;
	if (b_Delete)         //���ͼ�α��Ϊɾ��  �����ж� ֱ�ӷ���
		return false;
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return false;
	}
	GetRect(&x1,&y1,&x2,&y2);        
	if (!(x >= x1 && x <= x2 && y >= y1 && y <= y2))           //�жϵ��Ƿ��ھ���������
		return false;
	xx = DistanceFromPointToLine(x, y, m_X1, m_Y1, m_X2, m_Y2);
	if (xx < validDistance)            //�жϵ㵽ֱ�ߵľ����Ƿ�Ϊ��Ч����
		return true;
	return false;
}

void CC_Brother_Line::GetRect(float* x1, float* y1, float* x2, float* y2)    //��ȡ���ε� ���½Ǻ����Ͻ�����
{
	if (b_Delete || !p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		*x1 = 1E+20;
		*x2 = 1E-20;
		*y1 = 1E+20;
		*y2 = 1E-20;
		return;
	}
	/*if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return;
	}*/
	*x1 = min(m_X1, m_X2);
	*x2 = max(m_X1, m_X2);
	*y1 = min(m_Y1, m_Y2);
	*y2 = max(m_Y1, m_Y2);
}

PointStruct CC_Brother_Line::GetPoint(char c)
{
	return PointStruct(m_X1, m_Y1);
}

void CC_Brother_Line::Serialize(CArchive& ar)
{
	baseShape::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_X1 << m_Y1 << m_X2 << m_Y2;
	}
	else
	{
		ar >> m_X1 >> m_Y1 >> m_X2 >> m_Y2;
	}
}

void CC_Brother_Line::Save(CFile* file, CStdioFile* file1, BOOL Yn)
{
	baseShape::Save(file, file1, Yn);
	if (Yn)
	{
		//static int lineCount = 0;
		file->Write((unsigned char *)&m_X1, sizeof(m_X1));
		file->Write((unsigned char *)&m_Y1, sizeof(m_Y1));
		file->Write((unsigned char *)&m_X2, sizeof(m_X2));
		file->Write((unsigned char *)&m_Y2, sizeof(m_Y2));

		CString tmp;
		tmp.Empty();
		tmp.Format(_T("-->>children#line%d:\n"), lineCount++);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    startPointx:%f\n"), m_X1);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    startPointy:%f\n"), m_Y1);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    endPointx:%f\n"), m_X2);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    endPointy:%f\n\n"), m_Y2);
		file1->WriteString(tmp);
	}
	else
	{
		file->Read((unsigned char *)&m_X1, sizeof(m_X1));
		file->Read((unsigned char *)&m_Y1, sizeof(m_Y1));
		file->Read((unsigned char *)&m_X2, sizeof(m_X2));
		file->Read((unsigned char *)&m_Y2, sizeof(m_Y2));
		/*file1->Read((unsigned char *)&m_X1, sizeof(m_X1));
		file1->Read((unsigned char *)&m_Y1, sizeof(m_Y1));
		file1->Read((unsigned char *)&m_X2, sizeof(m_X2));
		file1->Read((unsigned char *)&m_Y2, sizeof(m_Y2));*/
	}
}