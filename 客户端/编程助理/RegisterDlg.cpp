// RegisterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"


// CRegisterDlg 对话框

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


// CRegisterDlg 消息处理程序


BOOL CRegisterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 提示框
	m_toolTips.Create(this, TTS_ALWAYSTIP|WS_POPUP);
	m_toolTips.Activate(TRUE);

	// 提示文字
	m_toolTips.AddTool(GetDlgItem(IDC_ACCOUNT_EDIT),  _T("输入您的账户名称。\n不做限制, 除了标点符号均可。\n作为登陆账户。"));
	m_toolTips.AddTool(GetDlgItem(IDC_PASSWORD_EDIT), _T("输入您的账户密码。\n不做限制, 除了标点符号均可。\n作为登陆密码。"));
	m_toolTips.AddTool(GetDlgItem(IDC_CONFIRM_EDIT),  _T("再次输入账户密码。"));
	m_toolTips.AddTool(GetDlgItem(IDC_EMAIL_EDIT),    _T("输入您常用的邮箱。\n同样可用于云端账户登陆。"));
	m_toolTips.AddTool(GetDlgItem(IDOK),              _T("使用输入的账户信息注册助理云端账户。"));
	m_toolTips.AddTool(GetDlgItem(IDCANCEL),          _T("关闭注册窗口。"));

	//文字颜色
	m_toolTips.SetTipTextColor(RGB(0,0,255));

	//鼠标指向多久后显示提示，毫秒
	m_toolTips.SetDelayTime(TTDT_INITIAL, 10); 

	//鼠标保持指向，提示显示多久，毫秒
	m_toolTips.SetDelayTime(TTDT_AUTOPOP, 9000000);

	//设定显示宽度，超长内容自动换行
	m_toolTips.SetMaxTipWidth(300);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CRegisterDlg::PreTranslateMessage(MSG* pMsg)
{
	// 功能提示
	if(GetPrivateProfileInt(_T("Setting"), _T("Tip"), 0, _T("./Setting.ini")) == 1)
		m_toolTips.RelayEvent(pMsg); // 接受消息响应

	return CDialogEx::PreTranslateMessage(pMsg);
}


// 工作线程
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
			pWnd->Error = _T("无法连接到服务器, 请检查网络。");
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
					pWnd->Error = _T("注册失败，该用户名已被使用。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
				else
				{
					pWnd->Error = _T("注册失败，请检查注册信息：") + theApp.Convert(RecvData.GetBuffer(0));
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
		pWnd->Error = _T("发生了异常，位于Register的OnGetWebInfo方法。");
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

	MessageBox(_T("注册成功，你现在可以用用户名或邮箱登录了！"));
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
		AfxMessageBox(_T("用户名称不能为空！"));
		return;
	}

	if(Password.IsEmpty())
	{
		AfxMessageBox(_T("用户密码不能为空！"));
		return;
	}

	if(Confirm.IsEmpty())
	{
		AfxMessageBox(_T("确认密码不能为空！"));
		return;
	}

	if(Email.IsEmpty())
	{
		AfxMessageBox(_T("注册邮箱不能为空！"));
		return;
	}

	if(Password != Confirm)
	{
		AfxMessageBox(_T("用户密码和确认密码不一致！"));
		return;
	}

	// 启动工作者线程
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
