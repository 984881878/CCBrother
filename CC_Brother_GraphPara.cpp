#include "stdafx.h"
#include "CC_Brother_GraphPara.h"

CC_Brother_GraphPara::CC_Brother_GraphPara()
{
	m_AllColorNum = 500;
	m_AllLayerNum = 100;
	m_CurrentColorNum = 4;
	m_CurrentLayerNum = 1;
	m_ColorList = new long[m_AllColorNum];
	m_LayerList = new LayerStruct[m_AllLayerNum];

	for (int i = 0; i < m_AllLayerNum; i++)
	{
		m_LayerList[i].m_id_only = i;
		m_LayerList[i].b_Display = 0;
	}
	m_LayerList[0].b_Display = 1;
	m_LayerList[0].m_Name = _T("图层0");

	m_ColorList[0] = RGB(0, 0, 0);   //黑色
	m_ColorList[1] = RGB(255, 0, 0); //红色
	m_ColorList[2] = RGB(0, 255, 0); //绿色
	m_ColorList[3] = RGB(0, 0, 255); //蓝色
	m_ColorList[4] = RGB(255, 255, 255);
	//m_LayerList[0].b_Display = 1;       //图层0  可视
	//strcpy_s(m_LayerList[0].m_Name,"Layer 0"); //初始化 图层0
}


CC_Brother_GraphPara::~CC_Brother_GraphPara()
{
	delete[] m_LayerList;
	delete[] m_ColorList;
}

COLORREF CC_Brother_GraphPara::GetColor(int n)
{
	return m_ColorList[n];
}

BOOL CC_Brother_GraphPara::GetDisplayState(int n)
{
	return m_LayerList[n].b_Display;
}
