// SettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "SettingDlg.h"
#include "afxdialogex.h"


// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingDlg::IDD, pParent)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SKIN_COMBO, m_Skin);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_SKIN_COMBO, &CSettingDlg::OnSelchangeSkinCombo)
	ON_CBN_DROPDOWN(IDC_SKIN_COMBO, &CSettingDlg::OnDropdownSkinCombo)
	ON_BN_CLICKED(IDOK, &CSettingDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CSettingDlg::OnCancel)
	ON_BN_CLICKED(IDC_LOGIN_CHECK, &CSettingDlg::OnLogin)
	ON_BN_CLICKED(IDC_SYNCHRONIZE_CHECK, &CSettingDlg::OnSynchronize)
	ON_BN_CLICKED(IDC_PASSWORD_CHECK, &CSettingDlg::OnPassword)
END_MESSAGE_MAP()


// CSettingDlg 消息处理程序


BOOL CSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 读取配置文件
	OnReadSetting();

	// 读取皮肤
	::GetPrivateProfileString(_T("Skin"), _T("Name"), _T("AquaOS.ssk"), SkinName.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));
	SkinName.ReleaseBuffer();

	// 设置皮肤
	int nIndex = m_Skin.FindStringExact(0, SkinName);
	if(nIndex != CB_ERR)
	{
		m_Skin.SetCurSel(nIndex);
		OnSelchangeSkinCombo();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSettingDlg::OnReadSetting()
{
	// 读取配置文件
	int Clear = GetPrivateProfileInt(_T("Setting"), _T("Clear"), 0, _T("./Setting.ini"));
	int Top   = GetPrivateProfileInt(_T("Setting"), _T("Top"),   0, _T("./Setting.ini"));
	int Save  = GetPrivateProfileInt(_T("Setting"), _T("Save"),  1, _T("./Setting.ini"));
	int Open  = GetPrivateProfileInt(_T("Setting"), _T("Open"),  1, _T("./Setting.ini"));
	int Update= GetPrivateProfileInt(_T("Setting"), _T("Update"),1, _T("./Setting.ini"));
	int Synchronize = GetPrivateProfileInt(_T("Setting"), _T("Synchronize"), 0, _T("./Setting.ini"));

	int password  = GetPrivateProfileInt(_T("Account"), _T("Remember"),  0, _T("./Setting.ini"));
	int login     = GetPrivateProfileInt(_T("Account"), _T("Auto"),  0, _T("./Setting.ini"));


	// 设置复选框状态
	((CButton*)GetDlgItem(IDC_CLEAR_CHECK))->SetCheck(Clear);
	((CButton*)GetDlgItem(IDC_TOP_CHECK))  ->SetCheck(Top);
	((CButton*)GetDlgItem(IDC_SAVE_CHECK)) ->SetCheck(Save);
	((CButton*)GetDlgItem(IDC_OPEN_CHECK)) ->SetCheck(Open);
	((CButton*)GetDlgItem(IDC_UPDATE_CHECK))->SetCheck(Update);
	((CButton*)GetDlgItem(IDC_SYNCHRONIZE_CHECK))->SetCheck(Synchronize);

	((CButton*)GetDlgItem(IDC_PASSWORD_CHECK)) ->SetCheck(password);
	((CButton*)GetDlgItem(IDC_LOGIN_CHECK))    ->SetCheck(login);
	

	// 设置窗口置顶状态
	::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	if(!Top)
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);


	// 读取皮肤文件
	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(_T("./Skin/*.ssk"));

	// 重置并添加默认项
	m_Skin.ResetContent();

	// 添加找到的皮肤
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			continue;
		else
			m_Skin.AddString(Finder.GetFileName());
	}

	// 设置皮肤名称
	int nIndex = m_Skin.FindStringExact(0, SkinName);
	if(nIndex != CB_ERR)
	{
		m_Skin.SetCurSel(nIndex);
	}
}


void CSettingDlg::OnSelchangeSkinCombo()
{
	m_Skin.GetWindowText(SkinName);
	CString Name = _T("Skin\\") + SkinName;

	// 更换皮肤
	skinppLoadSkin((TCHAR *)(LPCSTR)Name);

	// 刷新窗口
	Invalidate();
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 1, 0);
}


void CSettingDlg::OnDropdownSkinCombo()
{
}


void CSettingDlg::OnOK()
{
	// 变量
	CString Clear, Top, Save, Open, Password, Login, Update, Synchronize;

	// 清除格式
	if( ((CButton*)GetDlgItem(IDC_CLEAR_CHECK))->GetCheck() )
	{
		// 赋值
		Clear = _T("1");
	}
	else
	{
		// 赋值
		Clear = _T("0");
	}

	// 窗口置顶
	if( ((CButton*)GetDlgItem(IDC_TOP_CHECK))->GetCheck() )
	{
		// 主窗口置顶
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		// 赋值
		Top = _T("1");
	}
	else
	{
		// 主窗口还原
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		// 赋值
		Top = _T("0");
	}

	// 自动保存
	if( ((CButton*)GetDlgItem(IDC_SAVE_CHECK))->GetCheck() )
	{
		// 赋值
		Save = _T("1");
	}
	else
	{
		// 赋值
		Save = _T("0");
	}

	// 自动打开
	if( ((CButton*)GetDlgItem(IDC_OPEN_CHECK))->GetCheck() )
	{
		// 赋值
		Open = _T("1");
	}
	else
	{
		// 赋值
		Open = _T("0");
	}


	// 保存密码
	if( ((CButton*)GetDlgItem(IDC_PASSWORD_CHECK))->GetCheck() )
	{
		// 赋值
		Password = _T("1");
	}
	else
	{
		// 赋值
		Password = _T("0");
	}

	// 自动登录
	if( ((CButton*)GetDlgItem(IDC_LOGIN_CHECK))->GetCheck() )
	{
		// 赋值
		Login = _T("1");
	}
	else
	{
		// 赋值
		Login = _T("0");
	}

	// 检测更新
	if( ((CButton*)GetDlgItem(IDC_UPDATE_CHECK))->GetCheck() )
	{
		// 赋值
		Update = _T("1");
	}
	else
	{
		// 赋值
		Update = _T("0");
	}

	// 自动同步
	if( ((CButton*)GetDlgItem(IDC_SYNCHRONIZE_CHECK))->GetCheck() )
	{
		CString UserName, Password;
		if (!theApp.m_Sql.SelectData(_T("用户账户"), UserName, 1, _T("Name Is Not Null")))
		{
			AfxMessageBox(_T("无法读取用户名!无法自动同步!"));
			return;
		}

		if (!theApp.m_Sql.SelectData(_T("用户账户"), Password, 2, _T("Password Is Not Null")))
		{
			AfxMessageBox(_T("无法读取用户密码!无法自动同步!"));
			return;
		}

		if(UserName.IsEmpty() || Password.IsEmpty())
		{
			AfxMessageBox(_T("没有进行过自动登陆!无法自动同步!"));
			return;
		}

		// 赋值
		Synchronize = _T("1");
	}
	else
	{
		// 赋值
		Synchronize = _T("0");
	}


	// 保存配置文件
	::WritePrivateProfileString(_T("Setting"), _T("Clear"), Clear, _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Top"),   Top,   _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Save"),  Save,  _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Open"),  Open,  _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Update"),Update,_T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Synchronize"), Synchronize, _T("./Setting.ini"));

	::WritePrivateProfileString(_T("Account"), _T("Remember"),  Password,  _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Account"), _T("Auto"),      Login,     _T("./Setting.ini"));


	// 保存皮肤文件
	m_Skin.GetWindowText(SkinName);
	::WritePrivateProfileString(_T("Skin"), _T("Name"), SkinName, _T("./Setting.ini"));

	// 隐藏窗口
	ShowWindow(SW_HIDE);
}


void CSettingDlg::OnLogin()
{
	if( ((CButton*)GetDlgItem(IDC_LOGIN_CHECK))->GetCheck() )
	{
		((CButton*)GetDlgItem(IDC_PASSWORD_CHECK))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_PASSWORD_CHECK))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_SYNCHRONIZE_CHECK))->SetCheck(0);
	}
}


void CSettingDlg::OnSynchronize()
{
	if( ((CButton*)GetDlgItem(IDC_SYNCHRONIZE_CHECK))->GetCheck() )
	{
		((CButton*)GetDlgItem(IDC_LOGIN_CHECK))->SetCheck(1);

		// 调用OnLogin
		OnLogin();
	}
	else
	{
		//((CButton*)GetDlgItem(IDC_LOGIN_CHECK))->SetCheck(0);
	}
}


void CSettingDlg::OnPassword()
{
	if( !((CButton*)GetDlgItem(IDC_PASSWORD_CHECK))->GetCheck() )
	{
		((CButton*)GetDlgItem(IDC_LOGIN_CHECK))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_SYNCHRONIZE_CHECK))->SetCheck(0);
	}
}


void CSettingDlg::OnCancel()
{
	// 读取配置文件
	int Clear = GetPrivateProfileInt(_T("Setting"), _T("Clear"), 0, _T("./Setting.ini"));
	int Top   = GetPrivateProfileInt(_T("Setting"), _T("Top"),   0, _T("./Setting.ini"));
	int Save  = GetPrivateProfileInt(_T("Setting"), _T("Save"),  1, _T("./Setting.ini"));
	int Open  = GetPrivateProfileInt(_T("Setting"), _T("Open"),  1, _T("./Setting.ini"));
	int Update= GetPrivateProfileInt(_T("Setting"), _T("Update"),1, _T("./Setting.ini"));
	int Synchronize = GetPrivateProfileInt(_T("Setting"), _T("Synchronize"), 0, _T("./Setting.ini"));

	int password  = GetPrivateProfileInt(_T("Account"), _T("Remember"),  0, _T("./Setting.ini"));
	int login     = GetPrivateProfileInt(_T("Account"), _T("Auto"),  0, _T("./Setting.ini"));


	// 设置复选框状态
	((CButton*)GetDlgItem(IDC_CLEAR_CHECK))->SetCheck(Clear);
	((CButton*)GetDlgItem(IDC_TOP_CHECK))  ->SetCheck(Top);
	((CButton*)GetDlgItem(IDC_SAVE_CHECK)) ->SetCheck(Save);
	((CButton*)GetDlgItem(IDC_OPEN_CHECK)) ->SetCheck(Open);
	((CButton*)GetDlgItem(IDC_UPDATE_CHECK))->SetCheck(Update);
	((CButton*)GetDlgItem(IDC_SYNCHRONIZE_CHECK))->SetCheck(Synchronize);

	((CButton*)GetDlgItem(IDC_PASSWORD_CHECK)) ->SetCheck(password);
	((CButton*)GetDlgItem(IDC_LOGIN_CHECK))    ->SetCheck(login);
	

	// 设置窗口置顶状态
	::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	if(!Top)
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	// 恢复皮肤
	CString Skin;
	::GetPrivateProfileString(_T("Skin"), _T("Name"), _T("AquaOS.ssk"), Skin.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));
	Skin.ReleaseBuffer();

	// 如果取消修改皮肤
	if(Skin != SkinName)
	{
		// 设置皮肤
		int nIndex = m_Skin.FindStringExact(0, Skin);
		if(nIndex != CB_ERR)
		{
			m_Skin.SetCurSel(nIndex);
			OnSelchangeSkinCombo();
		}
	}

	// 隐藏窗口
	ShowWindow(SW_HIDE);
}
