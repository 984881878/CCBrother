#pragma once

//#include "CC_Brother_DrawView.h"
// CC_Brother_TextDlg 对话框
class CCCBrotherDrawView;

class CC_Brother_TextDlg : public CDialog
{
	DECLARE_DYNAMIC(CC_Brother_TextDlg)

public:
	CC_Brother_TextDlg(CWnd* pParent/* = nullptr*/);   // 标准构造函数
	CC_Brother_TextDlg(CWnd* pParent, CString Text);  //重写构造
	/*virtual ~CC_Brother_TextDlg();*/
	BOOL IsOpen();
	BOOL IsVisible();
	void Init(CString Text);

	CCCBrotherDrawView* m_pView = nullptr;       //对象指针
// 对话框数据
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WRITE_TEXT };
	CString m_Text;
//#endif
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持  数据交换
	virtual void OnOk();
	virtual void OnCancel();
	virtual BOOL OnInitDialog(); //将重载的消息函数改成自定义的消息函数  显示(与隐式)的覆盖父类的函数  参考项目HDraw

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	//virtual void OnOk();
	//virtual void OnCancel();
	//virtual BOOL OnInitDialog(); 将重载的消息函数改成自定义的消息函数  显示(与隐式)的覆盖父类的函数  参考项目HDraw
	afx_msg void OnChangeText();
	DECLARE_MESSAGE_MAP()
//public:
//	CString m_Text;
};
