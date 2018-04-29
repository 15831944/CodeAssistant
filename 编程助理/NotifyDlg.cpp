// NotifyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "NotifyDlg.h"
#include "afxdialogex.h"


// CNotifyDlg 对话框

IMPLEMENT_DYNAMIC(CNotifyDlg, CDialogEx)

CNotifyDlg::CNotifyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNotifyDlg::IDD, pParent)
{
}

CNotifyDlg::~CNotifyDlg()
{
}

void CNotifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNotifyDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CNotifyDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CNotifyDlg::OnCancel)
END_MESSAGE_MAP()


// CNotifyDlg 消息处理程序


BOOL CNotifyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AfxBeginThread(NotifyThread, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


// 消息提示线程
UINT CNotifyDlg::NotifyThread(LPVOID lpParameter)
{
	CNotifyDlg * pDlg = (CNotifyDlg*)lpParameter;

	// 等待操作完成
	while (!theApp.IsFinished)
	{
		Sleep(50);
	}
	
	pDlg->EndDialog(TRUE);
	return TRUE;
}


void CNotifyDlg::OnOK()
{
}


void CNotifyDlg::OnCancel()
{
}
