#include "stdafx.h"
#include "CC_Brother_Switch.h"
#include "CC_Brother_GraphPara.h"
#include "CC_Brother_DrawDoc.h"

extern int switchCount;

extern void  DeviceP_To_LogicalP(float x, float y, int *X, int *Y); //�豸�����ӳ�䵽�߼������ P����Point
extern void  LogicalP_To_DeviceP(int x, int y, float *X, float*Y);
extern int   DeviceL_To_LogicalL(float length);     //�豸�ϵĳ���ת��Ϊ�߼����� L����length
extern float LogicalL_To_DeviceL(int length);
extern CC_Brother_GraphPara* p_GraphPara;

IMPLEMENT_SERIAL(CC_Brother_Switch, CObject, 1)

CC_Brother_Switch::CC_Brother_Switch()
{
}


CC_Brother_Switch::~CC_Brother_Switch()
{
}

CC_Brother_Switch::CC_Brother_Switch(long ColorPen,  //��ɫ
	long ColorBrush, //
	float LineWide,
	short LineType,
	short Layer,
	int   id_only,
	BOOL Delete,
	float X1,
	float Y1)
	:baseShape(ColorPen, ColorBrush, LineWide, LineType, Layer, id_only, Delete)
{
	X = X1;
	Y = Y1;//�������½�����
}

int CC_Brother_Switch::getSizeOfSwitch()
{
	return size_Of_Switch;
}

void CC_Brother_Switch::setPointInc(float x, float y)           //�ṩͼ���ƶ� �����֧��
{
	X += x;
	Y += y;
}

PointStruct CC_Brother_Switch::GetPoint(char c)
{
	if (c == 'L')
		return PointStruct(X, Y);
	else
		return PointStruct(X4, Y4);
}

void CC_Brother_Switch::Serialize(CArchive& ar)
{
	baseShape::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << X << Y << X1 << Y1 << X2 << Y2 << X3 << Y3 << X4 << Y4;
		ar << size_Of_Switch;
	}
	else
	{
		ar >> X >> Y >> X1 >> Y1 >> X2 >> Y2 >> X3 >> Y3 >> X4 >> Y4;
		ar >> size_Of_Switch;
	}
}

void CC_Brother_Switch::Save(CFile* file, CStdioFile* file1, BOOL Yn)
{
	baseShape::Save(file, file1, Yn);
	if (Yn)
	{
		file->Write((unsigned char *)&X, sizeof(X));
		file->Write((unsigned char *)&Y, sizeof(Y));
		file->Write((unsigned char *)&X1, sizeof(X1));
		file->Write((unsigned char *)&Y1, sizeof(Y1));
		file->Write((unsigned char *)&X2, sizeof(X2));
		file->Write((unsigned char *)&Y2, sizeof(Y2));
		file->Write((unsigned char *)&X3, sizeof(X3));
		file->Write((unsigned char *)&Y3, sizeof(Y3));
		file->Write((unsigned char *)&X4, sizeof(X4));
		file->Write((unsigned char *)&Y4, sizeof(Y4));
		file->Write((unsigned char *)&size_Of_Switch, sizeof(size_Of_Switch));

		CString tmp;
		tmp.Empty();
		tmp.Format(_T("-->>children#switch%d:\n"), switchCount++);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    startPointx:%f    "), X);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    startPointy:%f\n"), Y);
		file1->WriteString(tmp);
		tmp.Format(_T("    NextOnex:%f    "), X1);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("     NextOnexy:%f\n"), Y1);
		file1->WriteString(tmp);
		tmp.Format(_T("    NNextOnex:%f    "), X2);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("  NNextOney:%f\n"), Y2);
		file1->WriteString(tmp);
		tmp.Format(_T("    NNNextOnex:%f    "), X3);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("NNNextOney:%f\n"), Y3);
		file1->WriteString(tmp);
		tmp.Format(_T("    NNNNextOnex:%f  "), X4);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("NNNNextOney:%f\n"), Y4);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    size_Of_Switch:%d\n\n"), size_Of_Switch);
		file1->WriteString(tmp);
		/*	file1->Write((unsigned char *)&X, sizeof(X));
			file1->Write((unsigned char *)&Y, sizeof(Y));
			file1->Write((unsigned char *)&X1, sizeof(X1));
			file1->Write((unsigned char *)&Y1, sizeof(Y1));
			file1->Write((unsigned char *)&X2, sizeof(X2));
			file1->Write((unsigned char *)&Y2, sizeof(Y2));
			file1->Write((unsigned char *)&X3, sizeof(X3));
			file1->Write((unsigned char *)&Y3, sizeof(Y3));
			file1->Write((unsigned char *)&X4, sizeof(X4));
			file1->Write((unsigned char *)&Y4, sizeof(Y4));
			file1->Write((unsigned char *)&size_Of_Switch, sizeof(size_Of_Switch));*/
	}
	else
	{
		file->Read((unsigned char *)&X, sizeof(X));
		file->Read((unsigned char *)&Y, sizeof(Y));
		file->Read((unsigned char *)&X1, sizeof(X1));
		file->Read((unsigned char *)&Y1, sizeof(Y1));
		file->Read((unsigned char *)&X2, sizeof(X2));
		file->Read((unsigned char *)&Y2, sizeof(Y2));
		file->Read((unsigned char *)&X3, sizeof(X3));
		file->Read((unsigned char *)&Y3, sizeof(Y3));
		file->Read((unsigned char *)&X4, sizeof(X4));
		file->Read((unsigned char *)&Y4, sizeof(Y4));
		file->Read((unsigned char *)&size_Of_Switch, sizeof(size_Of_Switch));
	}
}

void CC_Brother_Switch::DrawShape(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor)
{
	int x, y, x1, y1, x2, y2, x3, y3, x4, y4;
	if (b_Delete)
		return;
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return;
	}
	short LineType = m_LineType;
	short LineWide = max(1, DeviceL_To_LogicalL(m_LineWide)); //
	long ColorPen = m_ColorPen;
	if (m_DrawMode1 == 1)//�ж�ͼ���Ƿ���Ҫ������ʾ
	{
		if (m_LineType != 2)
			LineType = 2;
		else
			LineType++;
		ColorPen = RGB(255, 0, 0);
	}
	if (m_DrawMode1 == 2) //����Ϊ����ɫ ��������ͼ�β��ɼ� 
		ColorPen = BackColor;
	CPen pen((int)LineType, LineWide, /*p_GraphPara->GetColor*/(ColorPen));
	CPen* pOldPen = pDC->SelectObject(&pen);
	if (m_DrawMode == 0)//���û�ͼģʽΪ������ɫ
		pDC->SetROP2(R2_COPYPEN);
	else if (m_DrawMode == 1)//���û�ͼ��ɫΪ������ɫȡ��s
		pDC->SetROP2(R2_NOT);

	X1 = X + size_Of_Switch;
	Y1 = Y;
	X2 = X + size_Of_Switch * 2;
	Y2 = Y + size_Of_Switch;//���������Ҫ����ӳ��ģʽ�Լ��޸�  ������ӳ��ģʽ�� �ж�ʹ�õ�����һ��ģʽ �Ӷ��ó��� '+' ���� '-' ����һ��Ķ��� '-'
	X3 = X2;
	Y3 = Y;
	X4 = X3 + size_Of_Switch;
	Y4 = Y;               //��ʼ�����ص���ǵ������

	DeviceP_To_LogicalP(X, Y, &x, &y);    //����֮ǰ���Ի�ȡ��ǰ��ӳ��ģʽ Get....  ���ó�Ա����m_MapMode
	DeviceP_To_LogicalP(X1, Y1, &x1, &y1);
	DeviceP_To_LogicalP(X2, Y2, &x2, &y2);
	DeviceP_To_LogicalP(X3, Y3, &x3, &y3);
	DeviceP_To_LogicalP(X4, Y4, &x4, &y4);  //���豸����ת��Ϊ�߼�����

	pDC->MoveTo(x, y);
	pDC->LineTo(x1, y1);
	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y2);
	pDC->MoveTo(x3, y3);
	pDC->LineTo(x4, y4);             //������

	pDC->SelectObject(pOldPen);
}

bool CC_Brother_Switch::PointIsCount(float x, float y, float validDistance, float blc)
{
	float xx, x1, x2, y1, y2;
	if (b_Delete)
		return false;
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return false;
	}
	GetRect(&x1, &y1, &x2, &y2);
	if (!(x >= x1 && x <= x2 && y >= y1 && y <= y2))
		return false;
	return true;
}

void CC_Brother_Switch::GetRect(float* x1, float* y1, float* x2, float* y2)
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
	*x1 = X;
	*x2 = X + size_Of_Switch * 3;
	*y1 = Y;
	*y2 = Y + size_Of_Switch;        //����ǰ����ȷ��������ˮƽ���õ�  ���ܹ�����ֱ���� ����Ҫ������ж�
}