// SearchDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "SearchDlg.h"
#include "afxdialogex.h"

// json
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4005)
#include "json/include/json.h"
#pragma warning (pop)
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

// CSearchDlg 对话框

IMPLEMENT_DYNAMIC(CSearchDlg, CDialogEx)

CSearchDlg::CSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchDlg::IDD, pParent)
{
	Title    = _T(_T("查找方法"));
	NeedFind = FindFile = true;

	Web_Data = false;
	m_hOperate = NULL;
}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEARCH_COMBO, m_ComboBox);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSearchDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CSearchDlg::OnCancel)
END_MESSAGE_MAP()


// CSearchDlg 消息处理程序


BOOL CSearchDlg::OnInitDialog()
{
	VMPBEGIN
	CDialogEx::OnInitDialog();

	SetWindowText(Title);

	if(NeedFind)
	{
		CFileFind Finder;
		BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
		while (IsFind)
		{
			IsFind = Finder.FindNextFile();

			if (Finder.IsDots())
				continue;
			if (Finder.IsDirectory())
			{
				if(FindFile)
					continue;
				else
				{
					CString Name = Finder.GetFileName();
					m_ComboBox.AddString(Name.Left(Name.GetLength()));
				}
			}
			else
			{
				CString Name = Finder.GetFileName();
				m_ComboBox.AddString(Name.Left(Name.GetLength() - 5));
			}
		}
	}
	else
	{
		if(Web_Data)
		{
			// 启动工作者线程
			if (m_hOperate == NULL)
			{
				m_hOperate = AfxBeginThread(Operate, this);
			}
		}
	}
	
	VMPEND

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CSearchDlg::PreTranslateMessage(MSG* pMsg)
{
	VMPBEGIN
	VMPEND

	return CDialogEx::PreTranslateMessage(pMsg);
}


// 工作线程
UINT CSearchDlg::Operate(LPVOID pParam)
{
	// 窗口指针
	CSearchDlg * pWnd = ((CSearchDlg*)pParam);

	// 局部变量
	CString RecvData;
	BOOL IsSuccess;

	// 禁用按钮
	pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);

	// 获取服务器数据
	try
	{
		// 从服务器获取数据 www.shadowviolet.cn
		RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), pWnd->FilePath, 80, pWnd->Parameter, IsSuccess);
		if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		{
			pWnd->Error = _T("无法连接到服务器, 请检查网络。");
			pWnd->PostMessage(WM_COMMAND, 101);
		}
		else
		{
			if (IsSuccess)
			{
				Json::Reader reader;
				Json::Value root;

				// 定义接收数据
				CStringA m_RecvData(RecvData);

				// reader将Json字符串解析到root，root将包含Json里所有子元素
				if (reader.parse(m_RecvData.GetBuffer(), root))
				{
					if(!root["data"].isNull())
					{
						Json::Value Items = root["data"];
						Json::Value Total = root["total"];

						int total = Total.asInt();
						for(int i=0; i < total; i++)
						{
							std::string Name  = Items[i][pWnd->Resolve].asString();
							CString m_Name    = theApp.Convert(Name);

							pWnd->m_ComboBox.AddString(m_Name);
						}
					}
				}
				else
				{
					pWnd->Error = _T("无法解析数据。");
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
		pWnd->Error = _T("发生了异常，位于Operate的OnGetWebInfo方法。");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	// 激活按钮
	pWnd->GetDlgItem(IDOK)->EnableWindow();

	// 对象置为空
	pWnd->m_hOperate = NULL;
	return false;
}


void CSearchDlg::OnError()
{
	AfxMessageBox(Error);
}


void CSearchDlg::OnOK()
{
	VMPBEGIN
	m_ComboBox.GetWindowText(Function);

	if(!Function.IsEmpty())
		CDialogEx::OnOK();
	else
		CDialogEx::OnCancel();

	VMPEND
}


void CSearchDlg::OnCancel()
{
	VMPBEGIN
	CDialogEx::OnCancel();
	VMPEND
}
