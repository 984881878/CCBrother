// CC_Brother_TextDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "CC_Brother_Draw.h"
#include "CC_Brother_TextDlg.h"
#include "afxdialogex.h"
#include "CC_Brother_DrawView.h"

// CC_Brother_TextDlg 对话框

IMPLEMENT_DYNAMIC(CC_Brother_TextDlg, CDialog)

CC_Brother_TextDlg::CC_Brother_TextDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CC_Brother_TextDlg::IDD, pParent)
	, m_Text(_T("aaa"))
{
	//m_pView = (CCCBrotherDrawView*)pParent;
	//m_Text(_T(""))
}

CC_Brother_TextDlg::CC_Brother_TextDlg(CWnd* pParent, CString Text)
	: CDialog(CC_Brother_TextDlg::IDD, pParent)
{
	m_Text = Text;
	m_pView = (CCCBrotherDrawView*)pParent;
}

//CC_Brother_TextDlg::~CC_Brother_TextDlg()
//{
//}     这个析构函数必须注释掉

//文本框与用户交互的参数设置
void CC_Brother_TextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_TEXT_EDIT, m_Text);
	DDX_Text(pDX, IDC_TEXT_EDIT, m_Text);
	DDV_MaxChars(pDX,m_Text,200);
}

//消息响应函数 有数据交换就会响应
void CC_Brother_TextDlg::OnChangeText()
{
	UpdateData();
	m_pView->SendMessage(WM_COMMAND, ID_TEXT_MESSAGE);
	//CDialog::OnChangeText();
}

//消息响应函数 按下确认按键 响应
void CC_Brother_TextDlg::OnOk()
{
	//UpdateData();
	m_pView->SendMessage(WM_COMMAND, ID_TEXT_ONOK);
	CDialog::OnOK();
}

//消息响应函数 按下取消按键 响应
void CC_Brother_TextDlg::OnCancel()
{
	m_pView->SendMessage(WM_COMMAND, ID_TEXT_ONCANCEL);
	CDialog::OnCancel();
}

BOOL CC_Brother_TextDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_TEXT_EDIT);
	pEdit->SetFocus();
	return TRUE;
}

BOOL CC_Brother_TextDlg::IsOpen()
{
	return (m_hWnd ? TRUE : FALSE);
}

BOOL CC_Brother_TextDlg::IsVisible()
{
	if (!IsOpen())
		return FALSE;
	WINDOWPLACEMENT screen;
	GetWindowPlacement(&screen);
	if (screen.showCmd == SW_SHOW)
		return TRUE;
	else
		return FALSE;
}

void CC_Brother_TextDlg::Init(CString Text)
{
	m_Text = Text;
}

BEGIN_MESSAGE_MAP(CC_Brother_TextDlg, CDialog)
	ON_EN_CHANGE(IDC_TEXT_EDIT, OnChangeText)
	ON_COMMAND(IDOK,OnOk)
	ON_COMMAND(IDCANCEL,OnCancel)
END_MESSAGE_MAP()


// CC_Brother_TextDlg 消息处理程序
