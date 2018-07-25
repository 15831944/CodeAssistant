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
	    Associate   = GetPrivateProfileInt(_T("Setting"), _T("Associate"), 0, _T("./Setting.ini"));
	int Position    = GetPrivateProfileInt(_T("Setting"), _T("Position"), 0, _T("./Setting.ini"));
	
	int password  = GetPrivateProfileInt(_T("Account"), _T("Remember"),  0, _T("./Setting.ini"));
	int login     = GetPrivateProfileInt(_T("Account"), _T("Auto"),  0, _T("./Setting.ini"));


	// 设置复选框状态
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


// 设置关联strPath所需的注册表环境,strPath为带路径的程序名
void CSettingDlg::SetAssociateEnvironment(CString strPath)
{
	// 设置路径HKEY_CLASSES_ROOT\\mumayi下的键值
	HKEY hkey;
	CString strResult("");
	LPBYTE OwnerGet = new BYTE[80];//定义用户姓名
	memset(OwnerGet, 0, 80);
	DWORD dType1 = REG_SZ;//定义数据类型
	DWORD dLength = 80;//定义数据长度
	CString strValue("");

	if ( RegOpenKey( HKEY_CLASSES_ROOT, "CodeAssistant\\Shell\\Open\\Command\\", &hkey ) == ERROR_SUCCESS )
	{
		RegQueryValueEx(hkey, NULL, NULL, &dType1, OwnerGet, &dLength);
		strResult = (LPTSTR)OwnerGet;

		// 此处为了省事儿，直接写成了MuMaYiInstaller.exe，其实此处的应用名应该从strPath中解析出来
		if ( -1 == strResult.Find(strPath + "\" \"%%1\""))
		{
			strValue.Format("\"%s\" \"%%1\"", strPath);
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_CLASSES_ROOT, "CodeAssistant\\Shell\\Open", &hkey);
			strValue.Empty();
			strValue.Format("用编程助理打开");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_CLASSES_ROOT, "CodeAssistant", &hkey);
			strValue.Empty();
			strValue.Format("方法文件");
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
			strValue.Format("用编程助理打开");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_CLASSES_ROOT, "CodeAssistant", &hkey);
			strValue.Empty();
			strValue.Format("方法文件");
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
			// 该关键字下没有我的软件相关名,修改之
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

	// 设置路径"HKEY_LOCAL_MACHINE\\SORTWARE\\Classes\\mumayi\\"下的值

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
			strValue.Format("用编程助理打开");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_LOCAL_MACHINE, "SORTWARE\\Classes\\CodeAssistant\\", &hkey);
			strValue.Empty();
			strValue.Format("方法文件");
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
			strValue.Format("用编程助理打开");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
			RegOpenKey( HKEY_LOCAL_MACHINE, "SOFTWARE\\Classes\\CodeAssistant\\", &hkey);
			strValue.Empty();
			strValue.Format("方法文件");
			RegSetValue(hkey, _T(""), REG_SZ, strValue, strValue.GetLength()+1);
			RegSetValueEx(hkey, "", 0, REG_SZ, (const BYTE*)strValue.GetBuffer(strValue.GetLength()), strValue.GetLength()+1);
			RegCloseKey(hkey);
		}
	}

	// 设置路径HKEY_CLASSES_ROOT\\mumayi下的键值
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
			// 该关键字下没有我的软件相关名,修改之
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

	// 释放变量
	delete[] OwnerGet;

	// 通知系统，文件关联改变了
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST,NULL,NULL);
	SHChangeNotify(SHCNE_UPDATEIMAGE,SHCNF_DWORD,NULL,NULL);
}


//函数名称：AssociateApkFile
//函数功能：关联apk文件和图标
//输入参数：strType 文件类型  .apk .mpk等
//     strPath 程序路径名 与文件关联的可执行程序的带路径的文件名
//返 回 值：NULL
//函数说明：
void CSettingDlg::AssociateFile(CString strType, CString strPath)
{
	//HKEY_LOCAL_MACHINE/Software/Classes/等于HKEY_CLASSES_ROOT
	HKEY hkey;
	CString strKeyName("CodeAssistant");
	CString strResult("");
	LPBYTE OwnerGet = new BYTE[80];//定义用户姓名
	memset(OwnerGet, 0, 80);
	DWORD dType1 = REG_SZ;//定义数据类型
	DWORD dLength = 80;//定义数据长度
	CString strValue("");
	CString strRegPath("");
	strRegPath = strType + "\\"; // .apk

	// 设置路径"HKEY_CLASS_ROOT\\.apk"的键值，关联.apk文件用
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

	// 设置"SOFTWARE\\Classes\\.apk"的键值
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
			// 该关键字下没有我的软件相关名,修改之
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

	// 设置路径"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.apk\\UserChoice"的值
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
			// 该关键字下没有我的软件相关名,修改之
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
			// 该关键字下没有我的软件相关名,修改之
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

	// 释放变量
	delete[] OwnerGet;

	// 通知系统，文件关联改变了
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST,NULL,NULL);
	SHChangeNotify(SHCNE_UPDATEIMAGE,SHCNF_DWORD,NULL,NULL);
}


void CSettingDlg::DeleteAssociate()
{
	CRegKey regkey;

	// CodeAssistant
	regkey.Attach(HKEY_CLASSES_ROOT);
	if(regkey.RecurseDeleteKey(_T("CodeAssistant")) != ERROR_SUCCESS)
        AfxMessageBox(_T("无法删除 HKEY_CLASSES_ROOT 下的 CodeAssistant"));

	// .code
	regkey.Attach(HKEY_CLASSES_ROOT);
	if(regkey.RecurseDeleteKey(_T(".code")) != ERROR_SUCCESS)
        AfxMessageBox(_T("无法删除 HKEY_CLASSES_ROOT 下的 .code"));

	// SOFTWARE\\Classes\\CodeAssistant
	regkey.Attach(HKEY_CLASSES_ROOT);
	if(regkey.RecurseDeleteKey(_T("SOFTWARE\\Classes\\CodeAssistant")) != ERROR_SUCCESS)
        AfxMessageBox(_T("无法删除 HKEY_CLASSES_ROOT 下的 SOFTWARE\\Classes\\CodeAssistant"));

	// Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.code
	regkey.Attach(HKEY_CURRENT_USER);
	if(regkey.RecurseDeleteKey(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.code")) != ERROR_SUCCESS)
	{
		if(RegDeleteTree(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.code") ) != ERROR_SUCCESS)
			AfxMessageBox(_T("无法删除 HKEY_CURRENT_USER 下的 Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.code"));
	}

	// 通知系统，文件关联改变了
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST,NULL,NULL);
	SHChangeNotify(SHCNE_UPDATEIMAGE,SHCNF_DWORD,NULL,NULL);
}


BOOL CSettingDlg::IsAdministrator() {//判断是否管理员模式

	BOOL bIsElevated = FALSE;
	HANDLE hToken = NULL;
	UINT16 uWinVer = LOWORD(GetVersion());
	uWinVer = MAKEWORD(HIBYTE(uWinVer),LOBYTE(uWinVer));

	if (uWinVer < 0x0600)//不是VISTA、Windows7
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
	CString Clear, Top, Save, Open, Password, Login, Update, Synchronize, Associate, Position;

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

		// 清空
		::WritePrivateProfileString(_T("File"), _T("Class"), _T(""), _T("./Setting.ini"));
		::WritePrivateProfileString(_T("File"), _T("Type"),  _T(""), _T("./Setting.ini"));
		::WritePrivateProfileString(_T("File"), _T("Name"),  _T(""), _T("./Setting.ini"));
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

		// 通知主窗口检查更新
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 3, 0);
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

	// 文件关联
	if( ((CButton*)GetDlgItem(IDC_ASSOCIATE_CHECK))->GetCheck() )
	{
		TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
		GetModuleFileName(NULL,exeFullPath,MAX_PATH);//得到程序模块名称，全路径
		CString Path(exeFullPath);

		// 勾选文件关联
		if(this->Associate == 0)
		{
			if(!IsAdministrator())
			{
				// 赋值
				Associate = _T("0");

				if( MessageBox(_T("需要管理员权限才能继续, 是否提权并重启助理?"), _T("提权确认"), MB_ICONQUESTION | MB_YESNO) == IDYES )
				{
					// 提权 & 重新运行
					SHELLEXECUTEINFO ShExecInfo = {0};
					ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
					ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

					ShExecInfo.lpVerb       = _T("runas");
					ShExecInfo.lpFile       = Path;
					ShExecInfo.lpDirectory  = NULL;
					ShExecInfo.lpParameters = NULL;

					ShExecInfo.nShow = SW_SHOW;
					ShellExecuteEx(&ShExecInfo);

					// 通知主窗口关闭
					::SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_COMMAND, 102, 0);
				}
				else
				{
					// 赋值
					Associate = _T("0");
				}
			}
			else
			{
				// 赋值
				Associate = _T("1");

				// 关联文件
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
		GetModuleFileName(NULL,exeFullPath,MAX_PATH);//得到程序模块名称，全路径
		CString Path(exeFullPath);

		// 勾选文件关联
		if(this->Associate == 1)
		{
			if(!IsAdministrator())
			{
				if( MessageBox(_T("需要管理员权限才能继续, 是否提权并重启助理?"), _T("提权确认"), MB_ICONQUESTION | MB_YESNO) == IDYES )
				{
					// 赋值
					Associate = _T("1");

					// 提权 & 重新运行
					SHELLEXECUTEINFO ShExecInfo = {0};
					ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
					ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

					ShExecInfo.lpVerb       = _T("runas");
					ShExecInfo.lpFile       = Path;
					ShExecInfo.lpDirectory  = NULL;
					ShExecInfo.lpParameters = NULL;

					ShExecInfo.nShow = SW_SHOW;
					ShellExecuteEx(&ShExecInfo);

					// 通知主窗口关闭
					::SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_COMMAND, 102, 0);
				}
				else
				{
					// 赋值
					Associate = _T("1");
				}
			}
			else
			{
				// 赋值
				Associate = _T("0");

				// 删除文件关联
				DeleteAssociate();
			}
		}
		else
		{
			Associate.Format(_T("%d"), this->Associate);
		}
	}

	// 保存位置
	if( ((CButton*)GetDlgItem(IDC_POSITION_CHECK))->GetCheck() )
	{
		// 赋值
		Position = _T("1");
	}
	else
	{
		// 赋值
		Position = _T("0");

		// 清空
		::WritePrivateProfileString(_T("Position"), _T("WindowPosition"), _T(""), _T("./Setting.ini"));
	}


	// 保存配置文件
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
