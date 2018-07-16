// SettingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "SettingDlg.h"
#include "afxdialogex.h"


// CSettingDlg �Ի���

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


// CSettingDlg ��Ϣ�������


BOOL CSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��ȡ�����ļ�
	OnReadSetting();

	// ��ȡƤ��
	::GetPrivateProfileString(_T("Skin"), _T("Name"), _T("AquaOS.ssk"), SkinName.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));
	SkinName.ReleaseBuffer();

	// ����Ƥ��
	int nIndex = m_Skin.FindStringExact(0, SkinName);
	if(nIndex != CB_ERR)
	{
		m_Skin.SetCurSel(nIndex);
		OnSelchangeSkinCombo();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CSettingDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSettingDlg::OnReadSetting()
{
	// ��ȡ�����ļ�
	int Clear = GetPrivateProfileInt(_T("Setting"), _T("Clear"), 0, _T("./Setting.ini"));
	int Top   = GetPrivateProfileInt(_T("Setting"), _T("Top"),   0, _T("./Setting.ini"));
	int Save  = GetPrivateProfileInt(_T("Setting"), _T("Save"),  1, _T("./Setting.ini"));
	int Open  = GetPrivateProfileInt(_T("Setting"), _T("Open"),  1, _T("./Setting.ini"));
	int Update= GetPrivateProfileInt(_T("Setting"), _T("Update"),1, _T("./Setting.ini"));
	int Synchronize = GetPrivateProfileInt(_T("Setting"), _T("Synchronize"), 0, _T("./Setting.ini"));

	int password  = GetPrivateProfileInt(_T("Account"), _T("Remember"),  0, _T("./Setting.ini"));
	int login     = GetPrivateProfileInt(_T("Account"), _T("Auto"),  0, _T("./Setting.ini"));


	// ���ø�ѡ��״̬
	((CButton*)GetDlgItem(IDC_CLEAR_CHECK))->SetCheck(Clear);
	((CButton*)GetDlgItem(IDC_TOP_CHECK))  ->SetCheck(Top);
	((CButton*)GetDlgItem(IDC_SAVE_CHECK)) ->SetCheck(Save);
	((CButton*)GetDlgItem(IDC_OPEN_CHECK)) ->SetCheck(Open);
	((CButton*)GetDlgItem(IDC_UPDATE_CHECK))->SetCheck(Update);
	((CButton*)GetDlgItem(IDC_SYNCHRONIZE_CHECK))->SetCheck(Synchronize);

	((CButton*)GetDlgItem(IDC_PASSWORD_CHECK)) ->SetCheck(password);
	((CButton*)GetDlgItem(IDC_LOGIN_CHECK))    ->SetCheck(login);
	

	// ���ô����ö�״̬
	::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	if(!Top)
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);


	// ��ȡƤ���ļ�
	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(_T("./Skin/*.ssk"));

	// ���ò����Ĭ����
	m_Skin.ResetContent();

	// ����ҵ���Ƥ��
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

	// ����Ƥ������
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

	// ����Ƥ��
	skinppLoadSkin((TCHAR *)(LPCSTR)Name);

	// ˢ�´���
	Invalidate();
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 1, 0);
}


void CSettingDlg::OnDropdownSkinCombo()
{
}


void CSettingDlg::OnOK()
{
	// ����
	CString Clear, Top, Save, Open, Password, Login, Update, Synchronize;

	// �����ʽ
	if( ((CButton*)GetDlgItem(IDC_CLEAR_CHECK))->GetCheck() )
	{
		// ��ֵ
		Clear = _T("1");
	}
	else
	{
		// ��ֵ
		Clear = _T("0");
	}

	// �����ö�
	if( ((CButton*)GetDlgItem(IDC_TOP_CHECK))->GetCheck() )
	{
		// �������ö�
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		// ��ֵ
		Top = _T("1");
	}
	else
	{
		// �����ڻ�ԭ
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		// ��ֵ
		Top = _T("0");
	}

	// �Զ�����
	if( ((CButton*)GetDlgItem(IDC_SAVE_CHECK))->GetCheck() )
	{
		// ��ֵ
		Save = _T("1");
	}
	else
	{
		// ��ֵ
		Save = _T("0");
	}

	// �Զ���
	if( ((CButton*)GetDlgItem(IDC_OPEN_CHECK))->GetCheck() )
	{
		// ��ֵ
		Open = _T("1");
	}
	else
	{
		// ��ֵ
		Open = _T("0");
	}


	// ��������
	if( ((CButton*)GetDlgItem(IDC_PASSWORD_CHECK))->GetCheck() )
	{
		// ��ֵ
		Password = _T("1");
	}
	else
	{
		// ��ֵ
		Password = _T("0");
	}

	// �Զ���¼
	if( ((CButton*)GetDlgItem(IDC_LOGIN_CHECK))->GetCheck() )
	{
		// ��ֵ
		Login = _T("1");
	}
	else
	{
		// ��ֵ
		Login = _T("0");
	}

	// ������
	if( ((CButton*)GetDlgItem(IDC_UPDATE_CHECK))->GetCheck() )
	{
		// ��ֵ
		Update = _T("1");
	}
	else
	{
		// ��ֵ
		Update = _T("0");
	}

	// �Զ�ͬ��
	if( ((CButton*)GetDlgItem(IDC_SYNCHRONIZE_CHECK))->GetCheck() )
	{
		CString UserName, Password;
		if (!theApp.m_Sql.SelectData(_T("�û��˻�"), UserName, 1, _T("Name Is Not Null")))
		{
			AfxMessageBox(_T("�޷���ȡ�û���!�޷��Զ�ͬ��!"));
			return;
		}

		if (!theApp.m_Sql.SelectData(_T("�û��˻�"), Password, 2, _T("Password Is Not Null")))
		{
			AfxMessageBox(_T("�޷���ȡ�û�����!�޷��Զ�ͬ��!"));
			return;
		}

		if(UserName.IsEmpty() || Password.IsEmpty())
		{
			AfxMessageBox(_T("û�н��й��Զ���½!�޷��Զ�ͬ��!"));
			return;
		}

		// ��ֵ
		Synchronize = _T("1");
	}
	else
	{
		// ��ֵ
		Synchronize = _T("0");
	}


	// ���������ļ�
	::WritePrivateProfileString(_T("Setting"), _T("Clear"), Clear, _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Top"),   Top,   _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Save"),  Save,  _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Open"),  Open,  _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Update"),Update,_T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Synchronize"), Synchronize, _T("./Setting.ini"));

	::WritePrivateProfileString(_T("Account"), _T("Remember"),  Password,  _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Account"), _T("Auto"),      Login,     _T("./Setting.ini"));


	// ����Ƥ���ļ�
	m_Skin.GetWindowText(SkinName);
	::WritePrivateProfileString(_T("Skin"), _T("Name"), SkinName, _T("./Setting.ini"));

	// ���ش���
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

		// ����OnLogin
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
	// ��ȡ�����ļ�
	int Clear = GetPrivateProfileInt(_T("Setting"), _T("Clear"), 0, _T("./Setting.ini"));
	int Top   = GetPrivateProfileInt(_T("Setting"), _T("Top"),   0, _T("./Setting.ini"));
	int Save  = GetPrivateProfileInt(_T("Setting"), _T("Save"),  1, _T("./Setting.ini"));
	int Open  = GetPrivateProfileInt(_T("Setting"), _T("Open"),  1, _T("./Setting.ini"));
	int Update= GetPrivateProfileInt(_T("Setting"), _T("Update"),1, _T("./Setting.ini"));
	int Synchronize = GetPrivateProfileInt(_T("Setting"), _T("Synchronize"), 0, _T("./Setting.ini"));

	int password  = GetPrivateProfileInt(_T("Account"), _T("Remember"),  0, _T("./Setting.ini"));
	int login     = GetPrivateProfileInt(_T("Account"), _T("Auto"),  0, _T("./Setting.ini"));


	// ���ø�ѡ��״̬
	((CButton*)GetDlgItem(IDC_CLEAR_CHECK))->SetCheck(Clear);
	((CButton*)GetDlgItem(IDC_TOP_CHECK))  ->SetCheck(Top);
	((CButton*)GetDlgItem(IDC_SAVE_CHECK)) ->SetCheck(Save);
	((CButton*)GetDlgItem(IDC_OPEN_CHECK)) ->SetCheck(Open);
	((CButton*)GetDlgItem(IDC_UPDATE_CHECK))->SetCheck(Update);
	((CButton*)GetDlgItem(IDC_SYNCHRONIZE_CHECK))->SetCheck(Synchronize);

	((CButton*)GetDlgItem(IDC_PASSWORD_CHECK)) ->SetCheck(password);
	((CButton*)GetDlgItem(IDC_LOGIN_CHECK))    ->SetCheck(login);
	

	// ���ô����ö�״̬
	::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	if(!Top)
		::SetWindowPos(AfxGetMainWnd()->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	// �ָ�Ƥ��
	CString Skin;
	::GetPrivateProfileString(_T("Skin"), _T("Name"), _T("AquaOS.ssk"), Skin.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));
	Skin.ReleaseBuffer();

	// ���ȡ���޸�Ƥ��
	if(Skin != SkinName)
	{
		// ����Ƥ��
		int nIndex = m_Skin.FindStringExact(0, Skin);
		if(nIndex != CB_ERR)
		{
			m_Skin.SetCurSel(nIndex);
			OnSelchangeSkinCombo();
		}
	}

	// ���ش���
	ShowWindow(SW_HIDE);
}
