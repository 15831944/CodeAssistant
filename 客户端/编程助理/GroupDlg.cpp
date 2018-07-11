// GroupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "GroupDlg.h"
#include "afxdialogex.h"

#include "SearchDlg.h"

// CGroupDlg 对话框

IMPLEMENT_DYNAMIC(CGroupDlg, CDialogEx)

CGroupDlg::CGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGroupDlg::IDD, pParent)
	, m_UserName(_T(""))
	, m_Role(_T(""))
	, m_Version(_T(""))
{

}

CGroupDlg::~CGroupDlg()
{
}

void CGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USER_EDIT, m_UserName);
	DDX_CBString(pDX, IDC_ROLE_COMBO, m_Role);
	DDX_CBString(pDX, IDC_VERSION_COMBO, m_Version);
}


BEGIN_MESSAGE_MAP(CGroupDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGroupDlg::OnOK)
	ON_BN_CLICKED(IDC_SEARCH_BUTTON, &CGroupDlg::OnSearch)
	ON_CBN_SELCHANGE(IDC_ROLE_COMBO, &CGroupDlg::OnSelchangeRoleCombo)
	ON_BN_CLICKED(IDC_UNDO_BUTTON, &CGroupDlg::OnUndo)
	ON_BN_CLICKED(IDCANCEL, &CGroupDlg::OnCancel)
END_MESSAGE_MAP()


// CGroupDlg 消息处理程序


BOOL CGroupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ComboBox 初始化
	((CComboBox*)GetDlgItem(IDC_ROLE_COMBO))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_VERSION_COMBO))->SetCurSel(0);

	if(IsNew)
	{
		SetWindowText(_T("添加共享成员"));
		GetDlgItem(IDC_UNDO_BUTTON)->EnableWindow(false);
	}
	else
	{
		// 显示数据
		UpdateData(FALSE);
	}

	// 设置 ComboBox 状态
	OnSelchangeRoleCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CGroupDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CGroupDlg::OnOK()
{
	// 读取数据
	UpdateData();

	if(m_UserName.IsEmpty())
	{
		AfxMessageBox(_T("用户名称不能为空!"));
	}
	else
	{
		CDialogEx::OnOK();
	}
}


void CGroupDlg::OnSearch()
{
	CSearchDlg dlg;
	dlg.Title    = _T("查找用户");
	dlg.FilePath = _T("index/account/GetUserInfo");
	dlg.NeedFind = false;
	dlg.Web_Data = true;
	dlg.Resolve  = _T("UserName");

	if(dlg.DoModal() == IDOK)
	{
		// 赋值
		m_UserName = dlg.Function;

		// 显示数据
		UpdateData(FALSE);
	}
}


void CGroupDlg::OnSelchangeRoleCombo()
{
	if( ((CComboBox*)GetDlgItem(IDC_ROLE_COMBO))->GetCurSel() == 0 )
	{
		((CComboBox*)GetDlgItem(IDC_VERSION_COMBO))->SetCurSel(0);
		((CComboBox*)GetDlgItem(IDC_VERSION_COMBO))->EnableWindow(false);
	}
	else
	{
		((CComboBox*)GetDlgItem(IDC_VERSION_COMBO))->EnableWindow();
	}
}


void CGroupDlg::OnUndo()
{
	// 显示数据
	UpdateData(FALSE);
}


void CGroupDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
