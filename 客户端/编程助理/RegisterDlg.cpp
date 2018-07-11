// RegisterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"


// CRegisterDlg �Ի���

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialogEx)

CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRegisterDlg::IDD, pParent)
{
	m_hOperate = NULL;
}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CRegisterDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CRegisterDlg::OnCancel)

	ON_COMMAND(100, &CRegisterDlg::OnSuccess)
	ON_COMMAND(101, &CRegisterDlg::OnError)
END_MESSAGE_MAP()


// CRegisterDlg ��Ϣ�������


BOOL CRegisterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CRegisterDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


// �����߳�
UINT CRegisterDlg::Register(LPVOID pParam)
{
	CRegisterDlg * pWnd = ((CRegisterDlg*)pParam);

	CString RecvData;
	BOOL IsSuccess;

	pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);

	try
	{
		RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/register"), 80, pWnd->Parameter, IsSuccess);
		if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		{
			pWnd->Error = _T("�޷����ӵ�������, �������硣");
			pWnd->PostMessage(WM_COMMAND, 101);
		}
		else
		{
			if (IsSuccess)
			{
				if(RecvData == _T("success"))
				{
					pWnd->PostMessage(WM_COMMAND, 100);
				}
				else if(RecvData == _T("used"))
				{
					pWnd->Error = _T("ע��ʧ�ܣ����û����ѱ�ʹ�á�");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
				else
				{
					pWnd->Error = _T("ע��ʧ�ܣ�����ע����Ϣ��");
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
		pWnd->Error = _T("�������쳣��λ��Register��OnGetWebInfo������");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	pWnd->GetDlgItem(IDOK)->EnableWindow();
	pWnd->m_hOperate = NULL;
	return false;
}


void CRegisterDlg::OnSuccess()
{
	GetDlgItem(IDC_ACCOUNT_EDIT)->GetWindowText(UserName);
	GetDlgItem(IDC_PASSWORD_EDIT)->GetWindowText(Password);

	MessageBox(_T("ע��ɹ��������ڿ������û����������¼�ˣ�"));
	CDialogEx::OnOK();
}


void CRegisterDlg::OnError()
{
	AfxMessageBox(Error);
}


void CRegisterDlg::OnOK()
{
	CString Name, Password, Confirm, Email;
	GetDlgItem(IDC_ACCOUNT_EDIT)->GetWindowText(Name);
	GetDlgItem(IDC_PASSWORD_EDIT)->GetWindowText(Password);
	GetDlgItem(IDC_CONFIRM_EDIT)->GetWindowText(Confirm);
	GetDlgItem(IDC_EMAIL_EDIT)->GetWindowText(Email);

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

	if(Confirm.IsEmpty())
	{
		AfxMessageBox(_T("ȷ�����벻��Ϊ�գ�"));
		return;
	}

	if(Email.IsEmpty())
	{
		AfxMessageBox(_T("ע�����䲻��Ϊ�գ�"));
		return;
	}

	if(Password != Confirm)
	{
		AfxMessageBox(_T("�û������ȷ�����벻һ�£�"));
		return;
	}

	// �����������߳�
	if (m_hOperate == NULL)
	{
		Parameter.Format(_T("username=%s&password=%s&confirm=%s&email=%s"), Name, Password, Confirm, Email);
		m_hOperate = AfxBeginThread(Register, this);
	}
}


void CRegisterDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
