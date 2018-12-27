#include "stdafx.h"
#include "CC_Brother_Text.h"
#include "CC_Brother_GraphPara.h"
#include "CC_Brother_DrawDoc.h"

extern int textCount;

extern void  DeviceP_To_LogicalP(float x, float y, int *X, int *Y); //�豸�����ӳ�䵽�߼������ P����Point
extern void  LogicalP_To_DeviceP(int x, int y, float *X, float*Y);
extern int   DeviceL_To_LogicalL(float length);     //�豸�ϵĳ���ת��Ϊ�߼����� L����length
extern float LogicalL_To_DeviceL(int length);
extern CC_Brother_GraphPara* p_GraphPara;

IMPLEMENT_SERIAL(CC_Brother_Text, CObject, 1)

CC_Brother_Text::CC_Brother_Text()
{
}


CC_Brother_Text::~CC_Brother_Text()
{
}

CC_Brother_Text::CC_Brother_Text(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, BOOL Delete,
	float xStart, float yStart, float angle1, float angle2, float TextHeight, float TextWide, float cellWide,unsigned char TextFont, CString Text)
	:baseShape(ColorPen, ColorBrush, LineWide, LineType, Layer, id_only, Delete)
{
	m_xStart = xStart;
	m_yStart = yStart;
	m_angle1 = angle1;
	m_angle2 = angle2;
	m_TextHeight = TextHeight;
	m_TextWide = TextWide;
	m_cellWide = cellWide;
	m_TextFont = TextFont;
	m_Text = Text;
	m_TextLength = Text.GetLength();
}

void CC_Brother_Text::DrawShape(CDC* pDC, int m_DrawMode, int m_DrawMode1, long BackColor)
{
	//�������ʵ���˲�����Ӣ�ĵ�ע�� �����������ĵ�ע�� ����һ��������Ĺ��� ��Ҫ��Ϊ��������������ô��  ��Ϊ��Ҫʵ���ܹ�����������ŵĹ���
	//��һ������ռ2�ֽ� һ��Ӣ��ռ1�ֽ�  ������Ҫ����Դ� ������ʱ�� ��Ҫ׼ȷ��λ��ÿ���ַ�����ʵ����λ��
	if (b_Delete)
		return;
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return;
	}
	long ColorPen = m_ColorPen;
	if (m_DrawMode1 == 2)
		ColorPen = BackColor;
	CFont font;  //�����ַ�����ʾ��ʽ
	int high, wide, cc1, cc2, cd, angg;
	unsigned char c1, c2;
	float x1, y1, ang1;
	char bz[4];

	pDC->SetROP2(R2_COPYPEN);

	if (m_DrawMode == 0 && m_DrawMode1 == 1) //���û�ͼģʽΪ������ɫ
	{
		//pDC->SetROP2(R2_COPYPEN);
		ColorPen = RGB(255, 0, 0);
	}
	
	//pDC->SetBkMode(TRANSPARENT);
	/*������ַ���ʱ����������ַ����ı���ɫ��ͬ�ڿؼ�����ɫ��������������ƻ������ġ�����Ҫʹ��ʲô���������ֱ������䣬�����������ɫ���ַ����أ�
	���簴ť��������ɫ�Ǻ�ɫ�ģ��������ǻ�ɫ�ġ������Ҫʹ��SetBkMode����������DrawText�����������ʽ����ʾ�豸�������������ʽ��OPAQUE��TRANSPARENT��
	OPAQUE�ķ�ʽ���õ�ǰ�����Ļ�ˢ����ɫ�����ʾ���ֵı�������TRANSPARENT��ʹ��͸���������Ҳ�������ֵı����ǲ��ı�ġ�*/
	high = DeviceL_To_LogicalL(m_TextHeight);	//���������߼��߶�
	ang1 = (float)(m_angle1*Pi / 180);			//����ı�����x������Ļ���
	if (high < 3 || high>200)
		return;				//����̫С����̫�󶼽�����ʾ
	x1 = m_xStart - m_TextHeight * (float)sin(ang1);
	y1 = m_yStart + m_TextHeight * (float)cos(ang1);
	angg = (int)(m_angle2 * 10);
	wide = DeviceL_To_LogicalL(m_TextWide);	//������߼�������
	DeviceP_To_LogicalP(x1, y1, &cc1, &cc2);
	font.CreateFontW(high, wide, angg, 0, 50, 0, 0, 0, 255, OUT_TT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FIXED_PITCH,NULL);//���������ʽ����
	CFont* dcf = pDC->SelectObject(&font);
	pDC->SetBkMode(TRANSPARENT);        //����ģʽΪ�Ǹ���
	pDC->SetTextColor(/*p_GraphPara->GetColor*/(ColorPen));       


	//���ַ�תխ�ַ�  ��̬�����ڴ� �����ַ�һ���ֽ�һ���ֽڵ�ȡ�� �ŵ�������ڴ��� Ȼ����ڴ��е�������խ�ַ��ĸ�ʽ���н���
	char *lsl = NULL;
	const wchar_t* pawstr = m_Text.GetBuffer(m_Text.GetLength()+1);    
	DWORD num = WideCharToMultiByte(CP_ACP, 0, pawstr, -1, NULL, 0, NULL, 0);
	m_TextLength = num;
	lsl = (char*)malloc(num * sizeof(char));
	if (lsl == NULL)
		free(lsl);
	memset(lsl,0, num * sizeof(char));
	WideCharToMultiByte(CP_ACP,0, pawstr,-1,lsl,num,NULL,0);
	//wcstombs_s(&length,lsl,100,pawstr,length);
	//lsl = (char*)pawstr;


	//һ���ַ�һ���ַ������������
	cd = strlen(lsl);
	while (cd > 0)
	{
		c1 = *lsl;
		c2 = *(lsl + 1);
		if (c1 > 127 && c2 > 127)
		{
			strncpy_s(bz, lsl, 2);  //����һ�����ֵ��ַ���bz
			bz[2] = 0;    //�ַ����Ľ���λ�ñ�־ \0 ת����int���;��� 0
			lsl += 2;     //
			CString BZ(bz);
			pDC->TextOutW(cc1, cc2, BZ);     //���
			cd -= 2;
			x1 += (m_TextWide*2 + m_cellWide)*(float)cos(ang1);
			y1 += (m_TextWide*2 + m_cellWide)*(float)sin(ang1);  //ȷ����һ���ַ��ĳ�������
			DeviceP_To_LogicalP(x1, y1, &cc1, &cc2);
		}
		else
		{
			strncpy_s(bz, lsl, 1);
			bz[1] = 0;
			lsl++;
			CString BZ(bz);
			pDC->TextOutW(cc1, cc2, BZ);            //���
			cd--;
			x1 += (m_TextWide + m_cellWide/2)*(float)cos(ang1);
			y1 += (m_TextWide + m_cellWide/2)*(float)sin(ang1); //ȷ����һ���ַ��ĳ�������
			DeviceP_To_LogicalP(x1, y1, &cc1, &cc2);
		}
	}
	pDC->SelectObject(dcf);
	pDC->SetBkMode(OPAQUE);
}

void CC_Brother_Text::setPoint(float x, float y)
{
	m_xStart = x;
	m_yStart = y;
}

void CC_Brother_Text::setText(CString TextContent)
{
	m_Text = TextContent;
}

void CC_Brother_Text::setID(int id)
{
	m_id_only = id;
}

void CC_Brother_Text::updateColor(long color)
{
	m_ColorPen = color;
}

void CC_Brother_Text::prepare(int id, long color/*, short curLayer*/)
{
	setID(id);
	updateColor(color);
	//m_Layer = curLayer;
}

void CC_Brother_Text::setCurLayre(short layre)
{
	m_Layer = layre;
}

//ʹ�ô���ķ�ʽ�����ѧ�ϵķ�����ȷ��һ����С���Σ�Ҫ�����ܹ������ı���
void CC_Brother_Text::GetRect(float* x1, float* y1, float* x2, float* y2)
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
	float x[4], y[4];
	double ang = Pi * m_angle1 / 180;
	x[0] = m_xStart;
	y[0] = m_yStart;
	x[1] = m_xStart + (float)((m_TextWide + m_cellWide)*m_TextLength*cos(ang));             //�����m_TextlengthһֱΪ0����������������������
	y[1] = m_yStart + (float)((m_TextWide + m_cellWide)*m_TextLength*sin(ang));
	x[2] = x[1] - (float)(sin(ang)*m_TextHeight);
	y[2] = y[1] + (float)(cos(ang)*m_TextHeight);
	x[3] = m_xStart - (float)(sin(ang)*m_TextHeight);
	y[3] = m_yStart + (float)(cos(ang)*m_TextHeight);  //��ȡ��ע�ı�����ĸ���������

	*x1 = *x2 = m_xStart;
	*y1 = *y2 = m_yStart;

	for (int i = 0; i < 4; i++)
	{
		if (*x1 > x[i]) *x1 = x[i];
		if (*y1 > y[i]) *y1 = y[i];
		if (*x2 < x[i]) *x2 = x[i];
		if (*y2 < y[i]) *y2 = y[i];
	}  //ȡ���ĸ����������������½Ǻ������Ͻǵ�����
}

bool CC_Brother_Text::PointIsCount(float x, float y, float validDistance, float blc)
{
	POINT pList[4];
	float x1, y1, x2, y2;
	if (b_Delete)
		return false;
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return false;
	}
	GetRect(&x1, &y1, &x2, &y2);
	if (!(x >= x1 && x <= x2 && y >= y1 && y <= y2))   //�����жϰ��е��Ƿ���һ������ķ�Χ��   ���ھͿ���ֱ�ӷ��� ��߳�������
		return false;

	float ang = (float)Pi * m_angle1 / 180;
	pList[0].x = m_xStart;
	pList[0].y = m_yStart;
	pList[1].x = m_xStart + (float)((m_TextWide + m_cellWide)*m_TextLength*cos(ang));
	pList[1].y = m_yStart + (float)((m_TextWide + m_cellWide)*m_TextLength*sin(ang));
	pList[2].x = pList[1].x - (float)(sin(ang)*m_TextHeight);
	pList[2].y = pList[1].y + (float)(cos(ang)*m_TextHeight);
	pList[3].x = m_xStart - (float)(sin(ang)*m_TextHeight);
	pList[3].y = m_yStart + (float)(cos(ang)*m_TextHeight);//��ʼ����������������������
	if (PointInRgn(x, y, 4, pList, blc))           //�ж������Ƿ��ڸ�������
		return true;
	return false;
}

PointStruct CC_Brother_Text::GetPoint(char c)
{
	return PointStruct(m_xStart, m_yStart);
}

void CC_Brother_Text::Serialize(CArchive& ar)
{
	baseShape::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_xStart << m_yStart << m_angle1 << m_angle2 << m_TextHeight;
		ar << m_TextWide << m_cellWide << m_TextFont << m_TextLength;
		ar << m_Text;
	}
	else
	{
		ar >> m_xStart >> m_yStart >> m_angle1 >> m_angle2 >> m_TextHeight;
		ar >> m_TextWide >> m_cellWide >> m_TextFont >> m_TextLength;
		ar >> m_Text;
	}
}

void CC_Brother_Text::Save(CFile* file, CStdioFile* file1, BOOL Yn)
{
	baseShape::Save(file, file1, Yn);
	if (Yn)
	{
		CString tmp;
		tmp.Empty();
		tmp.Format(_T("-->>children#text%d:\n"), textCount++);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    startPointx:%f\n"), m_xStart);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    startPointy:%f\n"), m_yStart);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    wordsAngle:%f\n"), m_angle1);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    textAngle:%f\n"), m_angle2);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    TextHeight:%f\n"), m_TextHeight);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    TextWide:%f\n"), m_TextWide);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    cellWide(the distance from a words to another):%f\n"), m_cellWide);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    TextFont:%d\n"), m_TextFont);
		file1->WriteString(tmp);
		tmp.Empty();
		tmp.Format(_T("    TextLength:%d\n"), m_Text.GetLength() * sizeof(TCHAR));
		file1->WriteString(tmp);
		tmp.Empty();
		tmp = _T("    Content:");
		tmp += m_Text;
		tmp += "\n\n";
		file1->WriteString(tmp);

		file->Write((unsigned char *)&m_xStart, sizeof(m_xStart));
		file->Write((unsigned char *)&m_yStart, sizeof(m_yStart));
		file->Write((unsigned char *)&m_angle1, sizeof(m_angle1));
		file->Write((unsigned char *)&m_angle2, sizeof(m_angle2));
		file->Write((unsigned char *)&m_TextHeight, sizeof(m_TextHeight));
		file->Write((unsigned char *)&m_TextWide, sizeof(m_TextWide));
		file->Write((unsigned char *)&m_cellWide, sizeof(m_cellWide));
		file->Write((unsigned char *)&m_TextFont, sizeof(m_TextFont));
		m_TextLength = m_Text.GetLength() * sizeof(TCHAR);
		file->Write((unsigned char *)&m_TextLength, sizeof(m_TextLength));
		file->Write((LPCTSTR)m_Text.GetBuffer(m_Text.GetLength()), m_TextLength);
	}
	else
	{
		file->Read((unsigned char *)&m_xStart, sizeof(m_xStart));
		file->Read((unsigned char *)&m_yStart, sizeof(m_yStart));
		file->Read((unsigned char *)&m_angle1, sizeof(m_angle1));
		file->Read((unsigned char *)&m_angle2, sizeof(m_angle2));
		file->Read((unsigned char *)&m_TextHeight, sizeof(m_TextHeight));
		file->Read((unsigned char *)&m_TextWide, sizeof(m_TextWide));
		file->Read((unsigned char *)&m_cellWide, sizeof(m_cellWide));
		file->Read((unsigned char *)&m_TextFont, sizeof(m_TextFont));
		file->Read((unsigned char *)&m_TextLength, sizeof(m_TextLength));
		//file->Read(m_Text.GetBuffer(m_TextLength), m_TextLength);


		wchar_t * buf = new wchar_t[m_TextLength/* + 1*/];
		wmemset(buf, 0, m_TextLength/* + 1*/); // ע����WMEMSET��������MEMSET
		file->Read(buf, m_TextLength/* + 1*/);

		m_Text = (CString)buf;
		delete[]buf;

		/*m_Text.Empty();
		char *p =new char[m_TextLength +1];
		file->Read((unsigned char *)&p, m_TextLength);
		CString Text(p, m_TextLength);
		m_Text = Text;*/
		//delete[] p;
	}
}

//void CC_Brother_Text::setAllmember(long ColorPen, long ColorBrush, float LineWide, short LineType, short Layer, int id_only, BOOL Delete,
//	float m_x_Start, float m_y_Start, float _m_angle1, float _m_angle2, float _m_TextHeight, float _m_TextWide, float _m_cellWide,
//	unsigned char _m_TextFont, CString _m_Text)
//{
//	m_ColorPen = ColorPen;  //��ɫ
//	m_ColorBrush = ColorBrush; //
//	m_LineWide = LineWide;
//	m_LineType = LineType;
//	//m_xScale = xScale;/*���ͺ������*/
//	m_Layer = Layer;
//	m_id_only = id_only;
//	b_Delete = Delete;
//	m_xStart = m_x_Start;
//	m_yStart = m_y_Start;
//	m_angle1 = _m_angle1;
//	m_angle2 = _m_angle2;
//	m_TextHeight = _m_TextHeight;
//	m_TextWide = _m_TextWide;
//	m_cellWide = _m_cellWide;
//	m_TextFont = _m_TextFont;
//	m_Text = _m_Text;
//	m_TextLength = 0;
//}