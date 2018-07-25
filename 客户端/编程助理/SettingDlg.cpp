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
	    Associate   = GetPrivateProfileInt(_T("Setting"), _T("Associate"), 0, _T("./Setting.ini"));
	int Position    = GetPrivateProfileInt(_T("Setting"), _T("Position"), 0, _T("./Setting.ini"));
	
	int password  = GetPrivateProfileInt(_T("Account"), _T("Remember"),  0, _T("./Setting.ini"));
	int login     = GetPrivateProfileInt(_T("Account"), _T("Auto"),  0, _T("./Setting.ini"));


	// ���ø�ѡ��״̬
	((CButton*)GetDlgItem(IDC_CLEAR_CHECK))->SetCheck(Clear);
	((CButton*)GetDlgItem(IDC_TOP_CHECK))  ->SetCheck(Top);
	((CButton*)GetDlgItem(IDC_SAVE_CHECK)) ->SetCheck(Save);
	((CButton*)GetDlgItem(IDC_OPEN_CHECK)) ->SetCheck(Open);
	((CButton*)GetDlgItem(IDC_UPDATE_CHECK))->SetCheck(Update);
	((CButton*)GetDlgItem(IDC_SYNCHRONIZE_CHECK))->SetCheck(Synchronize);
	((CButton*)GetDlgItem(IDC_ASSOCIATE_CHECK))->SetCheck(Associate);
	((CButton*)GetDlgItem(IDC_POSITION_CHECK))->SetCheck(Position);

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


// ���ù���strPath�����ע�����,strPathΪ��·���ĳ�����
void CSettingDlg::SetAssociateEnvironment(CString strPath)
{
	// ����·��HKEY_CLASSES_ROOT\\mumayi�µļ�ֵ
	HKEY hkey;
	CString strResult("");
	LPBYTE OwnerGet = new BYTE[80];//�����û�����
	memset(OwnerGet, 0, 80);
	DWORD dType1 = REG_SZ;//������������
	DWORD dLength = 80;//�������ݳ���
	CString strValue("");

	if ( RegOpenKey( HKEY_CLASSES_ROOT, "CodeAssistant\\Shell\\Open\\Command\\", &hkey ) == ERROR_SUCCESS )
	{
		RegQueryValueEx(hkey, NULL, NULL, &dType1, OwnerGet, &dLength);
		strResult = (LPTSTR)OwnerGet;

		// �˴�Ϊ��ʡ�¶���ֱ��д����MuMaYiInstaller.exe����ʵ�˴���Ӧ����Ӧ�ô�strPath�н�������
		if ( -1 == strResult.Find(strPath + "\" \"%%1\""))
		{
			strValue.Format("\"%s\" \"%%1\"", strPath);
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_CLASSES_ROOT, "CodeAssistant\\Shell\\Open", &hkey);
			strValue.Empty();
			strValue.Format("�ñ�������");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_CLASSES_ROOT, "CodeAssistant", &hkey);
			strValue.Empty();
			strValue.Format("�����ļ�");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}
	else
	{
		if ( RegCreateKey( HKEY_CLASSES_ROOT, "CodeAssistant\\Shell\\Open\\Command", &hkey ) != ERROR_SUCCESS )
		{
			return;
		}
		else
		{
			strValue.Format("\"%s\" \"%%1\"", strPath);
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_CLASSES_ROOT, "CodeAssistant\\Shell\\Open", &hkey);
			strValue.Empty();
			strValue.Format("�ñ�������");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_CLASSES_ROOT, "CodeAssistant", &hkey);
			strValue.Empty();
			strValue.Format("�����ļ�");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}

	memset(OwnerGet, 0, 80);
	if (strResult.IsEmpty())
		strResult.Empty();
	strValue.Empty();
	if ( RegOpenKey( HKEY_CLASSES_ROOT, "CodeAssistant\\DefaultIcon\\", &hkey ) == ERROR_SUCCESS )
	{
		RegQueryValueEx(hkey, NULL, NULL, &dType1, OwnerGet, &dLength);
		strResult = (LPTSTR)OwnerGet;
		if ( -1 == strResult.Find(strPath + _T(",0")))
		{
			// �ùؼ�����û���ҵ���������,�޸�֮
			strValue.Format("%s,1", strPath);
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}
	else
	{
		if ( RegCreateKey( HKEY_CLASSES_ROOT, "CodeAssistant\\DefaultIcon", &hkey ) != ERROR_SUCCESS )
		{
			return;
		}
		else
		{
			strValue.Format("%s,1", strPath);
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}

	// ����·��"HKEY_LOCAL_MACHINE\\SORTWARE\\Classes\\mumayi\\"�µ�ֵ

	memset(OwnerGet, 0, 80);
	if (strResult.IsEmpty())
		strResult.Empty();
	strValue.Empty();
	if ( RegOpenKey( HKEY_LOCAL_MACHINE, "SORTWARE\\Classes\\CodeAssistant\\Shell\\Open\\Command\\", &hkey ) == ERROR_SUCCESS )
	{
		RegQueryValueEx(hkey, NULL, NULL, &dType1, OwnerGet, &dLength);
		strResult = (LPTSTR)OwnerGet;
		if ( -1 == strResult.Find(strPath + _T("\" \"%%1\"")))
		{
			strValue.Format("\"%s\" \"%%1\"", strPath);
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_LOCAL_MACHINE, "SORTWARE\\Classes\\CodeAssistant\\Shell\\Open\\", &hkey);
			strValue.Empty();
			strValue.Format("�ñ�������");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_LOCAL_MACHINE, "SORTWARE\\Classes\\CodeAssistant\\", &hkey);
			strValue.Empty();
			strValue.Format("�����ļ�");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}
	else
	{
		if ( RegCreateKey( HKEY_LOCAL_MACHINE, "SOFTWARE\\Classes\\CodeAssistant\\Shell\\Open\\Command", &hkey ) != ERROR_SUCCESS )
		{
			return;
		}
		else
		{
			strValue.Format("\"%s\" \"%%1\"", strPath);
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_LOCAL_MACHINE, "SOFTWARE\\Classes\\CodeAssistant\\Shell\\Open\\", &hkey);
			strValue.Empty();
			strValue.Format("�ñ�������");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_LOCAL_MACHINE, "SOFTWARE\\Classes\\CodeAssistant\\", &hkey);
			strValue.Empty();
			strValue.Format("�����ļ�");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}

	// ����·��HKEY_CLASSES_ROOT\\mumayi�µļ�ֵ
	memset(OwnerGet, 0, 80);
	if (strResult.IsEmpty())
		strResult.Empty();
	strValue.Empty();
	if ( RegOpenKey( HKEY_CLASSES_ROOT, "SOFTWARE\\Classes\\CodeAssistant\\DefaultIcon\\", &hkey ) == ERROR_SUCCESS )
	{
		RegQueryValueEx(hkey, NULL, NULL, &dType1, OwnerGet, &dLength);
		strResult = (LPTSTR)OwnerGet;
		if ( -1 == strResult.Find(strPath + _T(",0")) )
		{
			// �ùؼ�����û���ҵ���������,�޸�֮
			strValue.Format("%s,1", strPath);
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}
	else
	{
		if ( RegCreateKey( HKEY_CLASSES_ROOT, "SOFTWARE\\Classes\\CodeAssistant\\DefaultIcon", &hkey ) != ERROR_SUCCESS )
		{
			return;
		}
		else
		{
			strValue.Format("%s,1", strPath);
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}

	// �ͷű���
	delete[] OwnerGet;

	// ֪ͨϵͳ���ļ������ı���
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST,NULL,NULL);
	SHChangeNotify(SHCNE_UPDATEIMAGE,SHCNF_DWORD,NULL,NULL);
}


//�������ƣ�AssociateApkFile
//�������ܣ�����apk�ļ���ͼ��
//���������strType �ļ�����  .apk .mpk��
//     strPath ����·���� ���ļ������Ŀ�ִ�г���Ĵ�·�����ļ���
//�� �� ֵ��NULL
//����˵����
void CSettingDlg::AssociateFile(CString strType, CString strPath)
{
	//HKEY_LOCAL_MACHINE/Software/Classes/����HKEY_CLASSES_ROOT
	HKEY hkey;
	CString strKeyName("CodeAssistant");
	CString strResult("");
	LPBYTE OwnerGet = new BYTE[80];//�����û�����
	memset(OwnerGet, 0, 80);
	DWORD dType1 = REG_SZ;//������������
	DWORD dLength = 80;//�������ݳ���
	CString strValue("");
	CString strRegPath("");
	strRegPath = strType + "\\"; // .apk

	// ����·��"HKEY_CLASS_ROOT\\.apk"�ļ�ֵ������.apk�ļ���
	if ( RegOpenKey( HKEY_CLASSES_ROOT, strRegPath, &hkey ) == ERROR_SUCCESS )
	{
		RegQueryValueEx(hkey, NULL, NULL, &dType1, OwnerGet, &dLength);
		strResult = (LPTSTR)OwnerGet;
		if ( -1 == strResult.Find(CString("CodeAssistant")))
		{
			strValue.Format("CodeAssistant");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}
	else
	{
		if ( RegCreateKey( HKEY_CLASSES_ROOT, strType, &hkey ) != ERROR_SUCCESS )
		{
			return;
		}
		else
		{
			strValue.Format("CodeAssistant");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}

	// ����"SOFTWARE\\Classes\\.apk"�ļ�ֵ
	memset(OwnerGet, 0, 80);
	if (strResult.IsEmpty())
		strResult.Empty();
	strValue.Empty();
	strRegPath = CString("SOFTWARE\\Classes\\") + strType + "\\"; // SOFTWARE\\Classes\\.apk\\

	if ( RegOpenKey( HKEY_LOCAL_MACHINE, strRegPath, &hkey ) == ERROR_SUCCESS )
	{
		RegQueryValueEx(hkey, NULL, NULL, &dType1, OwnerGet, &dLength);
		strResult = (LPTSTR)OwnerGet;
		if ( -1 == strResult.Find("CodeAssistant"))
		{
			// �ùؼ�����û���ҵ���������,�޸�֮
			strValue.Format("CodeAssistant");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}
	else
	{
		if ( RegCreateKey( HKEY_LOCAL_MACHINE, strRegPath, &hkey ) != ERROR_SUCCESS )
		{
			return;
		}
		else
		{
			strValue.Format("CodeAssistant");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}

	// ����·��"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.apk\\UserChoice"��ֵ
	memset(OwnerGet, 0, 80);
	if (strResult.IsEmpty())
		strResult.Empty();
	strValue.Empty();
	strRegPath = CString("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\") + strType + "\\UserChoice";

	if ( RegOpenKey( HKEY_CURRENT_USER, strRegPath, &hkey ) == ERROR_SUCCESS )
	{
		RegQueryValueEx(hkey, NULL, NULL, &dType1, OwnerGet, &dLength);
		strResult = (LPTSTR)OwnerGet;
		if ( -1 == strResult.Find("CodeAssistant"))
		{
			// �ùؼ�����û���ҵ���������,�޸�֮
			strValue.Format("CodeAssistant");
			RegSetValue(hkey, "Progid", REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "Progid", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}
	else
	{
		if ( RegCreateKey( HKEY_CURRENT_USER, strRegPath, &hkey ) != ERROR_SUCCESS )
		{
			return;
		}
		else
		{
			strValue.Format("CodeAssistant");
			RegSetValue(hkey, "Progid", REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "Progid", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}
	memset(OwnerGet, 0, 80);
	if (strResult.IsEmpty())
		strResult.Empty();
	strValue.Empty();
	strRegPath = CString("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\") + strType;
	if ( RegOpenKey( HKEY_CURRENT_USER, strRegPath, &hkey ) == ERROR_SUCCESS )
	{
		RegQueryValueEx(hkey, NULL, NULL, &dType1, OwnerGet, &dLength);
		strResult = (LPTSTR)OwnerGet;
		if ( -1 == strResult.Find("CodeAssistant"))
		{
			// �ùؼ�����û���ҵ���������,�޸�֮
			strValue.Format("CodeAssistant");
			RegSetValue(hkey, "Progid", REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "Progid", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}
	else
	{
		if ( RegCreateKey( HKEY_CURRENT_USER, strRegPath, &hkey ) != ERROR_SUCCESS )
		{
			return;
		}
		else
		{
			strValue.Format("CodeAssistant");
			RegSetValue(hkey, "Progid", REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "Progid", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}

	// �ͷű���
	delete[] OwnerGet;

	// ֪ͨϵͳ���ļ������ı���
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST,NULL,NULL);
	SHChangeNotify(SHCNE_UPDATEIMAGE,SHCNF_DWORD,NULL,NULL);
}


void CSettingDlg::DeleteAssociate()
{
	CRegKey regkey;

	// CodeAssistant
	regkey.Attach(HKEY_CLASSES_ROOT);
	if(regkey.RecurseDeleteKey(_T("CodeAssistant")) != ERROR_SUCCESS)
        AfxMessageBox(_T("�޷�ɾ�� HKEY_CLASSES_ROOT �µ� CodeAssistant"));

	// .code
	regkey.Attach(HKEY_CLASSES_ROOT);
	if(regkey.RecurseDeleteKey(_T(".code")) != ERROR_SUCCESS)
        AfxMessageBox(_T("�޷�ɾ�� HKEY_CLASSES_ROOT �µ� .code"));

	// SOFTWARE\\Classes\\CodeAssistant
	regkey.Attach(HKEY_CLASSES_ROOT);
	if(regkey.RecurseDeleteKey(_T("SOFTWARE\\Classes\\CodeAssistant")) != ERROR_SUCCESS)
        AfxMessageBox(_T("�޷�ɾ�� HKEY_CLASSES_ROOT �µ� SOFTWARE\\Classes\\CodeAssistant"));

	// Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.code
	regkey.Attach(HKEY_CURRENT_USER);
	if(regkey.RecurseDeleteKey(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.code")) != ERROR_SUCCESS)
	{
		if(RegDeleteTree(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.code") ) != ERROR_SUCCESS)
			AfxMessageBox(_T("�޷�ɾ�� HKEY_CURRENT_USER �µ� Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.code"));
	}

	// ֪ͨϵͳ���ļ������ı���
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST,NULL,NULL);
	SHChangeNotify(SHCNE_UPDATEIMAGE,SHCNF_DWORD,NULL,NULL);
}


BOOL CSettingDlg::IsAdministrator() {//�ж��Ƿ����Աģʽ

	BOOL bIsElevated = FALSE;
	HANDLE hToken = NULL;
	UINT16 uWinVer = LOWORD(GetVersion());
	uWinVer = MAKEWORD(HIBYTE(uWinVer),LOBYTE(uWinVer));

	if (uWinVer < 0x0600)//����VISTA��Windows7
		return(FALSE);

	if (OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hToken)) {

		struct {
			DWORD TokenIsElevated;
		} /*TOKEN_ELEVATION*/te;
		DWORD dwReturnLength = 0;

		if (GetTokenInformation(hToken,/*TokenElevation*/(_TOKEN_INFORMATION_CLASS)20,&te,sizeof(te),&dwReturnLength)) {
			if (dwReturnLength == sizeof(te))
				bIsElevated = te.TokenIsElevated;
		}
		CloseHandle( hToken );
	}
	return bIsElevated;
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
	CString Clear, Top, Save, Open, Password, Login, Update, Synchronize, Associate, Position;

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

		// ���
		::WritePrivateProfileString(_T("File"), _T("Class"), _T(""), _T("./Setting.ini"));
		::WritePrivateProfileString(_T("File"), _T("Type"),  _T(""), _T("./Setting.ini"));
		::WritePrivateProfileString(_T("File"), _T("Name"),  _T(""), _T("./Setting.ini"));
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

		// ֪ͨ�����ڼ�����
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 3, 0);
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

	// �ļ�����
	if( ((CButton*)GetDlgItem(IDC_ASSOCIATE_CHECK))->GetCheck() )
	{
		TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
		GetModuleFileName(NULL,exeFullPath,MAX_PATH);//�õ�����ģ�����ƣ�ȫ·��
		CString Path(exeFullPath);

		// ��ѡ�ļ�����
		if(this->Associate == 0)
		{
			if(!IsAdministrator())
			{
				// ��ֵ
				Associate = _T("0");

				if( MessageBox(_T("��Ҫ����ԱȨ�޲��ܼ���, �Ƿ���Ȩ����������?"), _T("��Ȩȷ��"), MB_ICONQUESTION | MB_YESNO) == IDYES )
				{
					// ��Ȩ & ��������
					SHELLEXECUTEINFO ShExecInfo = {0};
					ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
					ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

					ShExecInfo.lpVerb       = _T("runas");
					ShExecInfo.lpFile       = Path;
					ShExecInfo.lpDirectory  = NULL;
					ShExecInfo.lpParameters = NULL;

					ShExecInfo.nShow = SW_SHOW;
					ShellExecuteEx(&ShExecInfo);

					// ֪ͨ�����ڹر�
					::SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_COMMAND, 102, 0);
				}
				else
				{
					// ��ֵ
					Associate = _T("0");
				}
			}
			else
			{
				// ��ֵ
				Associate = _T("1");

				// �����ļ�
				SetAssociateEnvironment(Path);
				AssociateFile(_T(".code"), Path);
			}
		}
		else
		{
			Associate.Format(_T("%d"), this->Associate);
		}
	}
	else
	{
		TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
		GetModuleFileName(NULL,exeFullPath,MAX_PATH);//�õ�����ģ�����ƣ�ȫ·��
		CString Path(exeFullPath);

		// ��ѡ�ļ�����
		if(this->Associate == 1)
		{
			if(!IsAdministrator())
			{
				if( MessageBox(_T("��Ҫ����ԱȨ�޲��ܼ���, �Ƿ���Ȩ����������?"), _T("��Ȩȷ��"), MB_ICONQUESTION | MB_YESNO) == IDYES )
				{
					// ��ֵ
					Associate = _T("1");

					// ��Ȩ & ��������
					SHELLEXECUTEINFO ShExecInfo = {0};
					ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
					ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

					ShExecInfo.lpVerb       = _T("runas");
					ShExecInfo.lpFile       = Path;
					ShExecInfo.lpDirectory  = NULL;
					ShExecInfo.lpParameters = NULL;

					ShExecInfo.nShow = SW_SHOW;
					ShellExecuteEx(&ShExecInfo);

					// ֪ͨ�����ڹر�
					::SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_COMMAND, 102, 0);
				}
				else
				{
					// ��ֵ
					Associate = _T("1");
				}
			}
			else
			{
				// ��ֵ
				Associate = _T("0");

				// ɾ���ļ�����
				DeleteAssociate();
			}
		}
		else
		{
			Associate.Format(_T("%d"), this->Associate);
		}
	}

	// ����λ��
	if( ((CButton*)GetDlgItem(IDC_POSITION_CHECK))->GetCheck() )
	{
		// ��ֵ
		Position = _T("1");
	}
	else
	{
		// ��ֵ
		Position = _T("0");

		// ���
		::WritePrivateProfileString(_T("Position"), _T("WindowPosition"), _T(""), _T("./Setting.ini"));
	}


	// ���������ļ�
	::WritePrivateProfileString(_T("Setting"), _T("Clear"), Clear, _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Top"),   Top,   _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Save"),  Save,  _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Open"),  Open,  _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Update"),Update,_T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Synchronize"), Synchronize, _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Associate"),   Associate,   _T("./Setting.ini"));
	::WritePrivateProfileString(_T("Setting"), _T("Position"),    Position,    _T("./Setting.ini"));

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
