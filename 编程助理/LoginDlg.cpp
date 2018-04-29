// LoginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "LoginDlg.h"
#include "afxdialogex.h"

#include "RegisterDlg.h"
#include "SynchronizeDlg.h"


// CLoginDlg �Ի���

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


// CLoginDlg ��Ϣ�������


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

		if (!theApp.m_Sql.SelectData(_T("�û��˻�"), UserName, 1, NULL, NULL, NULL, _T("Name Is Not Null")))
		{
			AfxMessageBox(_T("�޷���ȡ�û���!"));
		}

		if (!theApp.m_Sql.SelectData(_T("�û��˻�"), Password, 2, NULL, NULL, NULL, _T("Password Is Not Null")))
		{
			AfxMessageBox(_T("�޷���ȡ�û�����!"));
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
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


// �����߳�
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
			pWnd->Error = _T("�޷����ӵ�������, �������硣");
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
					pWnd->Error = _T("��¼ʧ�ܣ��û������������");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			}
			else
			{
				pWnd->Error = _T("�޷����ӵ�������, �������硣");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
		}
	}
	catch (...)
	{
		pWnd->Error = _T("�������쳣��λ��Login��OnGetWebInfo������");
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
		
		

		// �ж����ݱ����Ƿ��������
		if (!theApp.m_Sql.CheckData(_T("�û��˻�"), 1, false, _T(""), _T(""), _T("Name Is Not Null")))
		{
			// ��д�����ݿ�ʧ��
			if (!theApp.m_Sql.InsertData(_T("�û��˻�"), _T("'1','") + UserName + _T("','") + Password + _T("'")))
			{
				AfxMessageBox(_T("�޷�д���û�����!"));
				return;
			}
		}
		else
		{
			CString ColumnParams = _T("id = '1', Name = '") + UserName + _T("', Password = '") + Password + _T("'"),
				Params = _T("Name Is Not Null");

			// ���޸�����ʧ��
			if (!theApp.m_Sql.UpdataData(_T("�û��˻�"), ColumnParams, Params))
			{
				AfxMessageBox(_T("�޷��޸��û�����!"));
			}
		}
	}
	else
	{
		/*::WritePrivateProfileString(_T("Account"), _T("UserName"), _T(""), _T("./Setting.ini"));
		::WritePrivateProfileString(_T("Account"), _T("Password"), _T(""), _T("./Setting.ini"));*/
		if (theApp.m_Sql.CheckData(_T("�û��˻�"), 1, false, _T(""), _T(""), _T("Name Is Not Null")))
		{
			CString ColumnParams = _T("id = '', Name = '', Password = ''"),
				Params = _T("Name Is Not Null");

			// ���޸�����ʧ��
			if (!theApp.m_Sql.UpdataData(_T("�û��˻�"), ColumnParams, Params))
			{
				AfxMessageBox(_T("�޷��޸��û�����!"));
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
		AfxMessageBox(_T("�û����Ʋ���Ϊ�գ�"));
		return;
	}

	if(Password.IsEmpty())
	{
		AfxMessageBox(_T("�û����벻��Ϊ�գ�"));
		return;
	}

	// �����������߳�
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
