// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "LoginDlg.h"
#include "afxdialogex.h"

#include "RegisterDlg.h"
#include "SynchronizeDlg.h"


// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
{
	m_hOperate = NULL;
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REMEMBER_CHECK, m_Remember);
	DDX_Control(pDX, IDC_AUTO_CHECK, m_Auto);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnOK)
	ON_BN_CLICKED(IDC_REGISTER_BUTTON, &CLoginDlg::OnRegister)
	ON_BN_CLICKED(IDCANCEL, &CLoginDlg::OnCancel)

	ON_COMMAND(100, &CLoginDlg::OnSuccess)
	ON_COMMAND(101, &CLoginDlg::OnError)
	ON_BN_CLICKED(IDC_REMEMBER_CHECK, &CLoginDlg::OnRemember)
	ON_BN_CLICKED(IDC_AUTO_CHECK, &CLoginDlg::OnAuto)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序


BOOL CLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int remember   = GetPrivateProfileInt(_T("Account"), _T("Remember"), 0, _T("./Setting.ini"));
	int auto_login = GetPrivateProfileInt(_T("Account"), _T("Auto"), 0, _T("./Setting.ini"));

	m_Remember.SetCheck(remember);
	m_Auto.SetCheck(auto_login);

	if(remember)
	{
		CString UserName, Password;
		/*::GetPrivateProfileString(_T("Account"), _T("UserName"), _T(""), UserName.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));
		::GetPrivateProfileString(_T("Account"), _T("Password"), _T(""), Password.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));

		UserName.ReleaseBuffer();
		Password.ReleaseBuffer();*/

		if (!theApp.m_Sql.SelectData(_T("用户账户"), UserName, 1, NULL, NULL, NULL, _T("Name Is Not Null")))
		{
			AfxMessageBox(_T("无法读取用户名!"));
		}

		if (!theApp.m_Sql.SelectData(_T("用户账户"), Password, 2, NULL, NULL, NULL, _T("Password Is Not Null")))
		{
			AfxMessageBox(_T("无法读取用户密码!"));
		}
		

		if(!UserName.IsEmpty())
		{
			GetDlgItem(IDC_ACCOUNT_EDIT)->SetWindowText(UserName);
		}

		if(!Password.IsEmpty())
		{
			GetDlgItem(IDC_PASSWORD_EDIT)->SetWindowText(Password);
		}


		if(auto_login && !UserName.IsEmpty() && !Password.IsEmpty())
		{
			OnOK();
		}
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


// 工作线程
UINT CLoginDlg::Login(LPVOID pParam)
{
	CLoginDlg * pWnd = ((CLoginDlg*)pParam);

	CString RecvData;
	BOOL IsSuccess;

	pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);

	try
	{
		RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/login"), 80, pWnd->Parameter, IsSuccess);
		if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		{
			pWnd->Error = _T("无法连接到服务器, 请检查网络。");
			pWnd->PostMessage(WM_COMMAND, 101);
		}
		else
		{
			if (IsSuccess)
			{
				if( RecvData == RecvData.SpanIncluding( _T("0123456789") ) )
				{
					pWnd->UserId = RecvData;
					pWnd->PostMessage(WM_COMMAND, 100);
				}
				else
				{
					pWnd->Error = _T("登录失败，用户名或密码错误。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			}
			else
			{
				pWnd->Error = _T("无法连接到服务器, 请检查网络。");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
		}
	}
	catch (...)
	{
		pWnd->Error = _T("发生了异常，位于Login的OnGetWebInfo方法。");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	pWnd->GetDlgItem(IDOK)->EnableWindow();
	pWnd->m_hOperate = NULL;
	return false;
}


void CLoginDlg::OnSuccess()
{
	CString UserName, Password;
	GetDlgItem(IDC_ACCOUNT_EDIT)->GetWindowText(UserName);
	GetDlgItem(IDC_PASSWORD_EDIT)->GetWindowText(Password);

	if(m_Remember.GetCheck())
	{
		/*::WritePrivateProfileString(_T("Account"), _T("UserName"), UserName, _T("./Setting.ini"));
		::WritePrivateProfileString(_T("Account"), _T("Password"), Password, _T("./Setting.ini"));*/
		
		

		// 判断数据表中是否存在数据
		if (!theApp.m_Sql.CheckData(_T("用户账户"), 1, false, _T(""), _T(""), _T("Name Is Not Null")))
		{
			// 若写入数据库失败
			if (!theApp.m_Sql.InsertData(_T("用户账户"), _T("'1','") + UserName + _T("','") + Password + _T("'")))
			{
				AfxMessageBox(_T("无法写入用户数据!"));
				return;
			}
		}
		else
		{
			CString ColumnParams = _T("id = '1', Name = '") + UserName + _T("', Password = '") + Password + _T("'"),
				Params = _T("Name Is Not Null");

			// 若修改数据失败
			if (!theApp.m_Sql.UpdataData(_T("用户账户"), ColumnParams, Params))
			{
				AfxMessageBox(_T("无法修改用户数据!"));
			}
		}
	}
	else
	{
		/*::WritePrivateProfileString(_T("Account"), _T("UserName"), _T(""), _T("./Setting.ini"));
		::WritePrivateProfileString(_T("Account"), _T("Password"), _T(""), _T("./Setting.ini"));*/
		if (theApp.m_Sql.CheckData(_T("用户账户"), 1, false, _T(""), _T(""), _T("Name Is Not Null")))
		{
			CString ColumnParams = _T("id = '', Name = '', Password = ''"),
				Params = _T("Name Is Not Null");

			// 若修改数据失败
			if (!theApp.m_Sql.UpdataData(_T("用户账户"), ColumnParams, Params))
			{
				AfxMessageBox(_T("无法修改用户数据!"));
			}
		}
	}

	CDialogEx::OnOK();

	CSynchronizeDlg dlg;
	dlg.UserId   = UserId;
	dlg.UserName = UserName;
	dlg.DoModal();
}


void CLoginDlg::OnError()
{
	AfxMessageBox(Error);
}


void CLoginDlg::OnOK()
{
	CString Name, Password;
	GetDlgItem(IDC_ACCOUNT_EDIT)->GetWindowText(Name);
	GetDlgItem(IDC_PASSWORD_EDIT)->GetWindowText(Password);

	if(Name.IsEmpty())
	{
		AfxMessageBox(_T("用户名称不能为空！"));
		return;
	}

	if(Password.IsEmpty())
	{
		AfxMessageBox(_T("用户密码不能为空！"));
		return;
	}

	// 启动工作者线程
	if (m_hOperate == NULL)
	{
		Parameter.Format(_T("username=%s&password=%s"), Name, Password);
		m_hOperate = AfxBeginThread(Login, this);
	}
}


void CLoginDlg::OnRegister()
{
	CRegisterDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		GetDlgItem(IDC_ACCOUNT_EDIT)->SetWindowText(dlg.UserName);
		GetDlgItem(IDC_PASSWORD_EDIT)->SetWindowText(dlg.Password);
	}
}


void CLoginDlg::OnRemember()
{
	if(m_Remember.GetCheck())
	{
		::WritePrivateProfileString(_T("Account"), _T("Remember"), _T("1"), _T("./Setting.ini"));
	}
	else
	{
		::WritePrivateProfileString(_T("Account"), _T("Remember"), _T("0"), _T("./Setting.ini"));
	}
}


void CLoginDlg::OnAuto()
{
	if(m_Auto.GetCheck())
	{
		if(!m_Remember.GetCheck())
		{
			m_Remember.SetCheck(1);
			OnRemember();
		}

		::WritePrivateProfileString(_T("Account"), _T("Auto"), _T("1"), _T("./Setting.ini"));
	}
	else
	{
		if(m_Remember.GetCheck())
		{
			m_Remember.SetCheck(0);
			OnRemember();
		}

		::WritePrivateProfileString(_T("Account"), _T("Auto"), _T("0"), _T("./Setting.ini"));
	}
}


void CLoginDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
