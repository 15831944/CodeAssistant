// ModifyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "ModifyDlg.h"
#include "afxdialogex.h"


// CModifyDlg 对话框

IMPLEMENT_DYNAMIC(CModifyDlg, CDialogEx)

CModifyDlg::CModifyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CModifyDlg::IDD, pParent)
	, m_Name(_T(""))
	, m_Path(_T(""))
	, m_Version(_T(""))
{

}

CModifyDlg::~CModifyDlg()
{
}

void CModifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION_COMBO, m_ComboBox);
	DDX_Text(pDX, IDC_NAME_EDIT, m_Name);
	DDX_Text(pDX, IDC_PATH_EDIT, m_Path);
	DDX_Text(pDX, IDC_VERSION_EDIT, m_Version);
}


BEGIN_MESSAGE_MAP(CModifyDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CModifyDlg::OnOK)
	ON_BN_CLICKED(IDC_CHOOSE_BUTTON, &CModifyDlg::OnChoose)
	ON_BN_CLICKED(IDC_UNDO_BUTTON, &CModifyDlg::OnUndo)
	ON_BN_CLICKED(IDCANCEL, &CModifyDlg::OnCancel)
	ON_CBN_SELCHANGE(IDC_VERSION_COMBO, &CModifyDlg::OnSelchangeVersion)
END_MESSAGE_MAP()


// CModifyDlg 消息处理程序


BOOL CModifyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 如果是项目
	if(IsProject)
	{
		if(IsNew)
		{
			SetWindowText(_T("添加本地项目"));
			m_ComboBox.EnableWindow(false);
			GetDlgItem(IDOK)->SetWindowText(_T("添加项目"));
			GetDlgItem(IDC_UNDO_BUTTON)->EnableWindow(false);
			GetDlgItem(IDCANCEL)->SetWindowText(_T("取消添加"));
		}
		else
		{
			// 显示数据
			UpdateData(FALSE);
			GetDlgItem(IDC_VERSION_EDIT)->EnableWindow(false);

			// 得到版本数据
			m_ComboBox.ResetContent();

			// 循环添加数据(有多少列 i 就加多少)
			for (int i = 0 ;i < (int)this->pResult.size(); i += 5)
			{
				CString Version  = this->pResult[i + 3];
				m_ComboBox.AddString(Version);
			}

			// 设置选中数据
			int nIndex = m_ComboBox.FindStringExact(0, m_Version);
			if(nIndex != CB_ERR)
			{
				m_ComboBox.SetCurSel(nIndex);
			}
		}
	}
	else
	{
		if(IsNew)
		{
			SetWindowText(_T("添加项目版本"));
			GetDlgItem(IDC_NAME_STATIC)->SetWindowText(_T("修改说明"));
			GetDlgItem(IDC_PATH_STATIC)->SetWindowText(_T("修改时间"));

			GetDlgItem(IDC_PATH_EDIT)->EnableWindow(false);
			GetDlgItem(IDC_CHOOSE_BUTTON)->EnableWindow(false);
			GetDlgItem(IDC_UNDO_BUTTON)->EnableWindow(false);

			m_ComboBox.EnableWindow(false);

			GetDlgItem(IDOK)->SetWindowText(_T("添加版本"));
			GetDlgItem(IDCANCEL)->SetWindowText(_T("取消添加"));
			
		}
		else
		{
			// 显示数据
			UpdateData(FALSE);

			SetWindowText(_T("项目版本编辑"));
			GetDlgItem(IDC_NAME_STATIC)->SetWindowText(_T("修改说明"));
			GetDlgItem(IDC_PATH_STATIC)->SetWindowText(_T("修改时间"));

			GetDlgItem(IDC_PATH_EDIT)->EnableWindow(false);
			GetDlgItem(IDC_CHOOSE_BUTTON)->EnableWindow(false);
			m_ComboBox.EnableWindow(false);

			// 设置选中数据
			m_ComboBox.ResetContent();
			m_ComboBox.AddString(m_Version);
			m_ComboBox.SetCurSel(0);
		}
	}

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CModifyDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CModifyDlg::OnSelchangeVersion()
{
	CString Version;
	m_ComboBox.GetWindowText(Version);

	GetDlgItem(IDC_VERSION_EDIT)->SetWindowText(Version);
	
}


void CModifyDlg::OnOK()
{
	// 读取数据
	UpdateData(TRUE);

	if(m_Name.IsEmpty())
	{
		if(IsProject)
			AfxMessageBox(_T("项目名称不能为空!"));
		else
			AfxMessageBox(_T("修改说明不能为空!"));
	}
	else if(m_Path.IsEmpty() && IsProject)
	{
		AfxMessageBox(_T("项目路径不能为空!"));
	}
	else if(m_Version.IsEmpty())
	{
		AfxMessageBox(_T("项目版本不能为空!"));
	}
	else
	{
		CDialogEx::OnOK();
	}
}


void CModifyDlg::OnChoose()
{
	TCHAR           szFolderPath[MAX_PATH] = {0};  
	CString         strFolderPath = TEXT("");  

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot  = 0;
	sInfo.lpszTitle = _T("请选择一个目录：");
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	sInfo.lpfn      = NULL;

	// 显示文件夹选择对话框
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath)) 
		{
			strFolderPath = szFolderPath;

			// 显示选择路径
			GetDlgItem(IDC_PATH_EDIT)->SetWindowText(strFolderPath);

			// 设置焦点
			GetDlgItem(IDC_VERSION_EDIT)->SetFocus();
		}
	}
	if(lpidlBrowse != NULL)  
	{  
		::CoTaskMemFree(lpidlBrowse);  
	}
}


void CModifyDlg::OnUndo()
{
	if(IsNew)
	{
		AfxMessageBox(_T("新增操作无需撤销!"));
	}
	else
	{
		// 显示数据
		UpdateData(FALSE);

		// 设置选中数据
		int nIndex = m_ComboBox.FindStringExact(0, m_Version);
		if(nIndex != CB_ERR)
		{
			m_ComboBox.SetCurSel(nIndex);
		}
	}
}


void CModifyDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
