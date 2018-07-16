// 编程助理.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "编程助理.h"
#include "MainDlg.h"

#pragma comment(lib, "skin/SkinPPWTL.lib")
#pragma comment(lib, "version.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  VMPBEGIN \
    __asm _emit 0xEB \
    __asm _emit 0x10 \
    __asm _emit 0x56 \
    __asm _emit 0x4D \
    __asm _emit 0x50 \
    __asm _emit 0x72 \
    __asm _emit 0x6F \
    __asm _emit 0x74 \
    __asm _emit 0x65 \
    __asm _emit 0x63 \
    __asm _emit 0x74 \
    __asm _emit 0x20 \
    __asm _emit 0x62 \
    __asm _emit 0x65 \
    __asm _emit 0x67 \
    __asm _emit 0x69 \
    __asm _emit 0x6E \
    __asm _emit 0x00

#define  VMPEND \
  __asm _emit 0xEB \
    __asm _emit 0x0E \
    __asm _emit 0x56 \
    __asm _emit 0x4D \
    __asm _emit 0x50 \
    __asm _emit 0x72 \
    __asm _emit 0x6F \
    __asm _emit 0x74 \
    __asm _emit 0x65 \
    __asm _emit 0x63 \
    __asm _emit 0x74 \
    __asm _emit 0x20 \
    __asm _emit 0x65 \
    __asm _emit 0x6E \
    __asm _emit 0x64 \
    __asm _emit 0x00

// CMainApp

BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
	ON_COMMAND(ID_HELP, &CMainApp::OnHelp)
END_MESSAGE_MAP()


// CMainApp 构造

CMainApp::CMainApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	DataBase_Name = _T("Data.zdb");
	DataBase_Path = _T("Database");
}


// 唯一的一个 CMainApp 对象

CMainApp theApp;


// CMainApp 初始化

BOOL CMainApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// RichEdit初始化 call AfxInitRichEdit2() to initialize richedit2 library.
	AfxInitRichEdit2();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	// SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	if(!InitDataBase())
	{
		AfxMessageBox(_T("无法初始化数据库，请检查安全软件设置！"));
		return false;
	}

	CMainDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

// 数据库初始化
BOOL CMainApp::InitDataBase()
{
	// 如果存在目标数据库
	if (m_Sql.CheckDataBase( DataBase_Name, DataBase_Path))
	{
		// 连接数据库
		if(!m_Sql.OpenDataBase( DataBase_Name, DataBase_Path))
			return false;
	}
	else
	{
		// 创建数据库
		if(!m_Sql.CreateDataBase(DataBase_Name, DataBase_Path))
			return false;

		// 连接数据库
		if(!m_Sql.OpenDataBase(  DataBase_Name, DataBase_Path))
			return false;

		// 创建项目管理表
		if(!m_Sql.CreateDataTable(_T("项目管理"), _T("Name varchar(255), Path varchar(255), Type varchar(10), Version varchar(255)")))
			return false;

		// 创建版本控制表
		if(!m_Sql.CreateDataTable(_T("版本控制"), _T("Name varchar(255), Explain varchar(255), ModifyTime varchar(255), Version varchar(255), Type varchar(10)")))
			return false;

		// 创建用户账户表
		if(!m_Sql.CreateDataTable(_T("用户账户"), _T("id varchar(255), Name varchar(255), Password varchar(255)")))
			return false;
	}
	
	return true;
}


// 获取服务器数据
CString CMainApp::OnGetWebInfo(CString ServerName, CString ServerPath, int ServerPort, CString Param, BOOL &IsSuccess)
{
	TCHAR  *pRecvData = NULL;  //接收数据的指针
	DWORD  dwRecvSize = 0;     //接收数据的大小
	DWORD  dwRetSend = 0;     //发送函数返回值
	IsSuccess = false;        //发送及返回结果

	if (ServerName.IsEmpty() || ServerPath.IsEmpty() || ServerPort <= 0 || ServerPort > 65535)
	{
		return _T("服务器参数不能为空或端口不正确！");
	}
	else
	{
		try
		{
			CHttpFileEx Data;
			Data.SetServerParam(ServerName, ServerPath, ServerPort);
			dwRetSend = Data.PostDataMethod(Param, Param.GetLength(), &pRecvData, dwRecvSize, 2, 2);
			switch (dwRetSend)
			{
				//100：正常成功
			case 100:
			{
				CString RecvData(pRecvData);    //将接收数据复制到strRecv中。
				if (RecvData == _T("请确认参数信息完整性"))
				{
					return _T("请确认参数信息完整性!");
				}
				else if (!RecvData.IsEmpty())
				{
					IsSuccess = true;
					return RecvData;
				}
				else
					return _T("返回数据有误，请稍后再次重试！");

			}break;

			//101：服务器无法连接
			case 101:
				return _T("无法连接到服务器，请检查网络连接后重试！");
				break;

				//102：提交页面无法打开
			case 102:
				return _T("无法提交到服务器，请稍后再次重试！");
				break;

				//103：数据发送失败
			case 103:
				return _T("数据发送失败，请稍后再次重试！");
				break;

				//104：服务器处理失败
			case 104:
				return _T("服务器处理失败，请稍后再次重试！");
				break;

				//500：异常错误
			case 500:
				return _T("服务器发生异常错误，请稍后再次重试！");
				break;

			default:
				return _T("发生异常错误，请稍后再次重试！");
				break;
			}
		}
		catch (...)
		{
			return _T("发生异常错误，请稍后再次重试！");
		}
	}

	return _T("发生异常错误，请稍后再次重试！");
}


// 使用说明
void CMainApp::OnHelp()
{
	::SendMessage(m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 2, 0);
}


// 中文转换
CString CMainApp::Convert(string Source)
{
	//解决中文转码问题
	int len = strlen(Source.c_str())+1;  
	char outch[MAX_PATH];  
	WCHAR * wChar = new WCHAR[len];  
	wChar[0] = 0;  
	MultiByteToWideChar(CP_UTF8, 0, Source.c_str(), len, wChar, len);  
	WideCharToMultiByte(CP_ACP, 0, wChar, len, outch , len, 0, 0);  
	delete [] wChar;  
	char* pchar = (char*)outch;  

	len = strlen(pchar)+1;  
	WCHAR outName[MAX_PATH];  
	MultiByteToWideChar(CP_ACP, 0, pchar, len, outName, len);

	CString Result(outName);
	return  Result;
}


// 得到程序版本号
CString CMainApp::GetApplicationVersion()
{
	TCHAR szFullPath[MAX_PATH];
	DWORD dwVerInfoSize = 0;
	DWORD dwVerHnd;
	VS_FIXEDFILEINFO * pFileInfo;

	GetModuleFileName(NULL, szFullPath, sizeof(szFullPath));
	dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
	if (dwVerInfoSize)
	{
		// If we were able to get the information, process it:
		HANDLE  hMem;
		LPVOID  lpvMem;
		unsigned int uInfoSize = 0;

		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = GlobalLock(hMem);
		GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpvMem);

		::VerQueryValue(lpvMem, (LPTSTR)_T("\\"), (void**)&pFileInfo, &uInfoSize);

		int ret = GetLastError();
		WORD m_nProdVersion[4];

		// Product version from the FILEVERSION of the version info resource 
		m_nProdVersion[0] = HIWORD(pFileInfo->dwProductVersionMS);
		m_nProdVersion[1] = LOWORD(pFileInfo->dwProductVersionMS);
		m_nProdVersion[2] = HIWORD(pFileInfo->dwProductVersionLS);
		m_nProdVersion[3] = LOWORD(pFileInfo->dwProductVersionLS);

		CString strVersion;
		strVersion.Format(_T("v%d.%d.%d.%d"), m_nProdVersion[0],
			m_nProdVersion[1], m_nProdVersion[2], m_nProdVersion[3]);

		GlobalUnlock(hMem);
		GlobalFree(hMem);

		return strVersion;
	}

	return _T("");
}


int CMainApp::ExitInstance()
{
	// 卸载皮肤
	skinppExitSkin();

	return CWinApp::ExitInstance();
}
