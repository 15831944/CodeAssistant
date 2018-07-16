// �������.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "�������.h"
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


// CMainApp ����

CMainApp::CMainApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	DataBase_Name = _T("Data.zdb");
	DataBase_Path = _T("Database");
}


// Ψһ��һ�� CMainApp ����

CMainApp theApp;


// CMainApp ��ʼ��

BOOL CMainApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// RichEdit��ʼ�� call AfxInitRichEdit2() to initialize richedit2 library.
	AfxInitRichEdit2();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	// SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	if(!InitDataBase())
	{
		AfxMessageBox(_T("�޷���ʼ�����ݿ⣬���鰲ȫ������ã�"));
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

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

// ���ݿ��ʼ��
BOOL CMainApp::InitDataBase()
{
	// �������Ŀ�����ݿ�
	if (m_Sql.CheckDataBase( DataBase_Name, DataBase_Path))
	{
		// �������ݿ�
		if(!m_Sql.OpenDataBase( DataBase_Name, DataBase_Path))
			return false;
	}
	else
	{
		// �������ݿ�
		if(!m_Sql.CreateDataBase(DataBase_Name, DataBase_Path))
			return false;

		// �������ݿ�
		if(!m_Sql.OpenDataBase(  DataBase_Name, DataBase_Path))
			return false;

		// ������Ŀ�����
		if(!m_Sql.CreateDataTable(_T("��Ŀ����"), _T("Name varchar(255), Path varchar(255), Type varchar(10), Version varchar(255)")))
			return false;

		// �����汾���Ʊ�
		if(!m_Sql.CreateDataTable(_T("�汾����"), _T("Name varchar(255), Explain varchar(255), ModifyTime varchar(255), Version varchar(255), Type varchar(10)")))
			return false;

		// �����û��˻���
		if(!m_Sql.CreateDataTable(_T("�û��˻�"), _T("id varchar(255), Name varchar(255), Password varchar(255)")))
			return false;
	}
	
	return true;
}


// ��ȡ����������
CString CMainApp::OnGetWebInfo(CString ServerName, CString ServerPath, int ServerPort, CString Param, BOOL &IsSuccess)
{
	TCHAR  *pRecvData = NULL;  //�������ݵ�ָ��
	DWORD  dwRecvSize = 0;     //�������ݵĴ�С
	DWORD  dwRetSend = 0;     //���ͺ�������ֵ
	IsSuccess = false;        //���ͼ����ؽ��

	if (ServerName.IsEmpty() || ServerPath.IsEmpty() || ServerPort <= 0 || ServerPort > 65535)
	{
		return _T("��������������Ϊ�ջ�˿ڲ���ȷ��");
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
				//100�������ɹ�
			case 100:
			{
				CString RecvData(pRecvData);    //���������ݸ��Ƶ�strRecv�С�
				if (RecvData == _T("��ȷ�ϲ�����Ϣ������"))
				{
					return _T("��ȷ�ϲ�����Ϣ������!");
				}
				else if (!RecvData.IsEmpty())
				{
					IsSuccess = true;
					return RecvData;
				}
				else
					return _T("���������������Ժ��ٴ����ԣ�");

			}break;

			//101���������޷�����
			case 101:
				return _T("�޷����ӵ��������������������Ӻ����ԣ�");
				break;

				//102���ύҳ���޷���
			case 102:
				return _T("�޷��ύ�������������Ժ��ٴ����ԣ�");
				break;

				//103�����ݷ���ʧ��
			case 103:
				return _T("���ݷ���ʧ�ܣ����Ժ��ٴ����ԣ�");
				break;

				//104������������ʧ��
			case 104:
				return _T("����������ʧ�ܣ����Ժ��ٴ����ԣ�");
				break;

				//500���쳣����
			case 500:
				return _T("�����������쳣�������Ժ��ٴ����ԣ�");
				break;

			default:
				return _T("�����쳣�������Ժ��ٴ����ԣ�");
				break;
			}
		}
		catch (...)
		{
			return _T("�����쳣�������Ժ��ٴ����ԣ�");
		}
	}

	return _T("�����쳣�������Ժ��ٴ����ԣ�");
}


// ʹ��˵��
void CMainApp::OnHelp()
{
	::SendMessage(m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 2, 0);
}


// ����ת��
CString CMainApp::Convert(string Source)
{
	//�������ת������
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


// �õ�����汾��
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
	// ж��Ƥ��
	skinppExitSkin();

	return CWinApp::ExitInstance();
}
