
// CC_Brother_Draw.h: CC_Brother_Draw 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CCCBrotherDrawApp:


class CCCBrotherDrawApp : public CWinApp
{
public:
	CCCBrotherDrawApp() noexcept;


// 重写
public:
	virtual BOOL InitInstance();            //初始化实例
	virtual int ExitInstance();             //退出实例

// 实现
	afx_msg void OnAppAbout();              //消息函数 关于本APP的一些消息
	DECLARE_MESSAGE_MAP()
};

extern CCCBrotherDrawApp theApp;             //外部实例
