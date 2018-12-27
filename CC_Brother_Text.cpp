#include "stdafx.h"
#include "CC_Brother_Text.h"
#include "CC_Brother_GraphPara.h"
#include "CC_Brother_DrawDoc.h"

extern int textCount;

extern void  DeviceP_To_LogicalP(float x, float y, int *X, int *Y); //设备坐标点映射到逻辑坐标点 P代表Point
extern void  LogicalP_To_DeviceP(int x, int y, float *X, float*Y);
extern int   DeviceL_To_LogicalL(float length);     //设备上的长度转换为逻辑长度 L代表length
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
	//这个函数实现了不仅是英文的注释 还可以是中文的注释 这是一个很特殊的功能 不要以为你就是输出文字那么简单  因为还要实现能够对其进行缩放的功能
	//而一个中文占2字节 一个英文占1字节  所以需要区别对待 放缩的时候 需要准确定位到每个字符的其实坐标位置
	if (b_Delete)
		return;
	if (!p_GraphPara->m_LayerList[m_Layer].b_Display)
	{
		return;
	}
	long ColorPen = m_ColorPen;
	if (m_DrawMode1 == 2)
		ColorPen = BackColor;
	CFont font;  //定义字符的显示格式
	int high, wide, cc1, cc2, cd, angg;
	unsigned char c1, c2;
	float x1, y1, ang1;
	char bz[4];

	pDC->SetROP2(R2_COPYPEN);

	if (m_DrawMode == 0 && m_DrawMode1 == 1) //设置画图模式为画笔颜色
	{
		//pDC->SetROP2(R2_COPYPEN);
		ColorPen = RGB(255, 0, 0);
	}
	
	//pDC->SetBkMode(TRANSPARENT);
	/*输出的字符串时，发现输出字符串的背景色不同于控件的颜色，这样的输出是破坏背景的。那需要使用什么方法来保持背景不变，而又能输出红色的字符串呢？
	比如按钮的文字颜色是黑色的，而背景是灰色的。这就需要使用SetBkMode函数来设置DrawText函数的输出方式，显示设备共有两种输出方式：OPAQUE和TRANSPARENT。
	OPAQUE的方式是用当前背景的画刷的颜色输出显示文字的背景，而TRANSPARENT是使用透明的输出，也就是文字的背景是不改变的。*/
	high = DeviceL_To_LogicalL(m_TextHeight);	//获得字体的逻辑高度
	ang1 = (float)(m_angle1*Pi / 180);			//获得文本行与x轴正向的弧度
	if (high < 3 || high>200)
		return;				//字体太小或者太大都将不显示
	x1 = m_xStart - m_TextHeight * (float)sin(ang1);
	y1 = m_yStart + m_TextHeight * (float)cos(ang1);
	angg = (int)(m_angle2 * 10);
	wide = DeviceL_To_LogicalL(m_TextWide);	//字体的逻辑坐标宽度
	DeviceP_To_LogicalP(x1, y1, &cc1, &cc2);
	font.CreateFontW(high, wide, angg, 0, 50, 0, 0, 0, 255, OUT_TT_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FIXED_PITCH,NULL);//创建字体格式对象
	CFont* dcf = pDC->SelectObject(&font);
	pDC->SetBkMode(TRANSPARENT);        //背景模式为非覆盖
	pDC->SetTextColor(/*p_GraphPara->GetColor*/(ColorPen));       


	//宽字符转窄字符  动态申请内存 将宽字符一个字节一个字节的取出 放到申请的内存中 然后对内存中的数据以窄字符的格式进行解析
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


	//一个字符一个字符的输出到画板
	cd = strlen(lsl);
	while (cd > 0)
	{
		c1 = *lsl;
		c2 = *(lsl + 1);
		if (c1 > 127 && c2 > 127)
		{
			strncpy_s(bz, lsl, 2);  //拷贝一个汉字到字符串bz
			bz[2] = 0;    //字符串的结束位置标志 \0 转换成int类型就是 0
			lsl += 2;     //
			CString BZ(bz);
			pDC->TextOutW(cc1, cc2, BZ);     //输出
			cd -= 2;
			x1 += (m_TextWide*2 + m_cellWide)*(float)cos(ang1);
			y1 += (m_TextWide*2 + m_cellWide)*(float)sin(ang1);  //确定下一个字符的出现坐标
			DeviceP_To_LogicalP(x1, y1, &cc1, &cc2);
		}
		else
		{
			strncpy_s(bz, lsl, 1);
			bz[1] = 0;
			lsl++;
			CString BZ(bz);
			pDC->TextOutW(cc1, cc2, BZ);            //输出
			cd--;
			x1 += (m_TextWide + m_cellWide/2)*(float)cos(ang1);
			y1 += (m_TextWide + m_cellWide/2)*(float)sin(ang1); //确定下一个字符的出现坐标
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

//使用代码的方式表达数学上的方法：确定一个最小矩形，要求其能够囊括文本框
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
	x[1] = m_xStart + (float)((m_TextWide + m_cellWide)*m_TextLength*cos(ang));             //这里的m_Textlength一直为0！！！！！！！！！！！
	y[1] = m_yStart + (float)((m_TextWide + m_cellWide)*m_TextLength*sin(ang));
	x[2] = x[1] - (float)(sin(ang)*m_TextHeight);
	y[2] = y[1] + (float)(cos(ang)*m_TextHeight);
	x[3] = m_xStart - (float)(sin(ang)*m_TextHeight);
	y[3] = m_yStart + (float)(cos(ang)*m_TextHeight);  //获取标注文本框的四个顶点坐标

	*x1 = *x2 = m_xStart;
	*y1 = *y2 = m_yStart;

	for (int i = 0; i < 4; i++)
	{
		if (*x1 > x[i]) *x1 = x[i];
		if (*y1 > y[i]) *y1 = y[i];
		if (*x2 < x[i]) *x2 = x[i];
		if (*y2 < y[i]) *y2 = y[i];
	}  //取出四个顶点坐标中最左下角和最右上角的坐标
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
	if (!(x >= x1 && x <= x2 && y >= y1 && y <= y2))   //首先判断按中点是否在一个更大的范围内   不在就可以直接返回 提高程序性能
		return false;

	float ang = (float)Pi * m_angle1 / 180;
	pList[0].x = m_xStart;
	pList[0].y = m_yStart;
	pList[1].x = m_xStart + (float)((m_TextWide + m_cellWide)*m_TextLength*cos(ang));
	pList[1].y = m_yStart + (float)((m_TextWide + m_cellWide)*m_TextLength*sin(ang));
	pList[2].x = pList[1].x - (float)(sin(ang)*m_TextHeight);
	pList[2].y = pList[1].y + (float)(cos(ang)*m_TextHeight);
	pList[3].x = m_xStart - (float)(sin(ang)*m_TextHeight);
	pList[3].y = m_yStart + (float)(cos(ang)*m_TextHeight);//初始化多边形区域各个顶点坐标
	if (PointInRgn(x, y, 4, pList, blc))           //判断坐标是否在改区域中
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
		wmemset(buf, 0, m_TextLength/* + 1*/); // 注意是WMEMSET，而不是MEMSET
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
//	m_ColorPen = ColorPen;  //笔色
//	m_ColorBrush = ColorBrush; //
//	m_LineWide = LineWide;
//	m_LineType = LineType;
//	//m_xScale = xScale;/*线型横向参数*/
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