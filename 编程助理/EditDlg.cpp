// EditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "EditDlg.h"
#include "afxdialogex.h"


// CEditDlg 对话框

IMPLEMENT_DYNAMIC(CEditDlg, CDialogEx)

CEditDlg::CEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditDlg::IDD, pParent)
{

}

CEditDlg::~CEditDlg()
{
}

void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLASS_EDIT, m_Class);
	DDX_Control(pDX, IDC_TYPE_EDIT, m_Type);
}


BEGIN_MESSAGE_MAP(CEditDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEditDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CEditDlg::OnCancel)
END_MESSAGE_MAP()


// CEditDlg 消息处理程序


BOOL CEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if(IsFunction)
	{
		SetWindowText(_T("编辑方法"));

		m_Class.EnableWindow(false);
		GetDlgItem(IDC_TYPE_STATIC)->SetWindowText(_T("方法: "));
		m_Type.SetWindowText(Type);
	}
	else
	{
		m_Class.SetWindowText(Class);
		m_Type.SetWindowText(Type);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CEditDlg::OnOK()
{
	CString new_Class, new_Type;
	m_Class.GetWindowText(new_Class);
	m_Type.GetWindowText(new_Type);

	// 修改分类
	if(!IsFunction)
	{
		// 没有修改
		if(new_Class == Class && new_Type == Type)
			CDialogEx::OnCancel();
		else
		{
			Class = new_Class;
			Type  = new_Type;

			CDialogEx::OnOK();
		}
	}
	else
	{
		// 没有修改
		if(new_Type == Type)
			CDialogEx::OnCancel();
		else
		{
			Type  = new_Type;
			CDialogEx::OnOK();
		}
	}
}


void CEditDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
