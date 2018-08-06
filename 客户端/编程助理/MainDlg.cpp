// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "MainDlg.h"
#include "afxdialogex.h"
#include <shlobj.h>

#include "atlimage.h"

#include "SearchDlg.h"
#include "EditDlg.h"
#include "LoginDlg.h"
#include "ProjectDlg.h"
#include "TransmissionDlg.h"
#include "SynchronizeDlg.h"
#include "ParagraphDlg.h"


#include "lzma/LzmaLib.h"

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

// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Setting = new CSettingDlg();

	// 工作线程初始化
	Type = TabCount = 0;
	m_hOperate = m_hUpDate = NULL;

	IsNew  = TRUE;
	IsEdit = FALSE;

	CurClass = CurType = _T("");

	// 编码助理
	m_hCursor = AfxGetApp()->LoadCursor(IDC_TARGET_CURSOR);
	m_pAssisDlg = new CAssistantDlg;
	m_preWnd = NULL;
	m_curWnd = NULL;
	m_bSnap = FALSE;

	IsExit = TRUE;
}


CMainDlg::~CMainDlg()
{
	m_Setting->DestroyWindow();
	delete m_Setting;

	if(m_pAssisDlg != NULL)
	{
		m_pAssisDlg->DestroyWindow();
		delete m_pAssisDlg;
	}

	for(int i=0; i<TabCount; i++)
	{
		m_pDocument[i]->DestroyWindow();
		delete m_pDocument[i];
	}
}


void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CODE_LIST, m_List);
	DDX_Control(pDX, IDC_CLASS_COMBO, m_Class);
	DDX_Control(pDX, IDC_TYPE_COMBO, m_Type);
	DDX_Control(pDX, IDC_DOCUMENT_TAB, m_Tab);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &CMainDlg::OnSave)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMainDlg::OnDelete)
	ON_BN_CLICKED(IDOK, &CMainDlg::OnOK)
	ON_BN_CLICKED(IDC_FONT_BUTTON, &CMainDlg::OnFont)
	ON_BN_CLICKED(IDC_NEW_BUTTON, &CMainDlg::OnNew)
	ON_BN_CLICKED(IDC_DIRECTORY_BUTTON, &CMainDlg::OnDirectory)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, &CMainDlg::OnRemove)

	ON_NOTIFY(NM_CLICK, IDC_CODE_LIST, &CMainDlg::OnClickCodeList)
	ON_NOTIFY(NM_RCLICK, IDC_CODE_LIST, &CMainDlg::OnRClickCodeList)

	ON_CBN_KILLFOCUS(IDC_CLASS_COMBO, &CMainDlg::OnKillfocusClassCombo)
	ON_CBN_KILLFOCUS(IDC_TYPE_COMBO, &CMainDlg::OnKillfocusTypeCombo)
	ON_CBN_DROPDOWN(IDC_CLASS_COMBO, &CMainDlg::OnDropdownClassCombo)
	ON_CBN_DROPDOWN(IDC_TYPE_COMBO, &CMainDlg::OnDropdownTypeCombo)

	ON_NOTIFY(NM_DBLCLK, IDC_CODE_LIST, &CMainDlg::OnDblclkCodeList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CODE_LIST, &CMainDlg::OnItemchangedCodeList)

	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CMainDlg::OnEdit)
	ON_BN_CLICKED(IDC_SYNCHRONIZATION_BUTTON, &CMainDlg::OnSynchronization)
	ON_BN_CLICKED(IDC_SETTING_BUTTON, &CMainDlg::OnSetting)

	ON_MESSAGE(WM_CHILDMESSAGE, OnMessageChild)
	ON_BN_CLICKED(IDC_MANAGER_BUTTON, &CMainDlg::OnManager)

	ON_COMMAND(100, &CMainDlg::Complete)
	ON_COMMAND(101, &CMainDlg::OnError)
	ON_COMMAND(102, &CMainDlg::OnCancel)
	ON_COMMAND(103, &CMainDlg::OnSaveCode)
	ON_COMMAND(104, &CMainDlg::OnNewCode)
	ON_COMMAND(105, &CMainDlg::SetPic)
	ON_COMMAND(106, &CMainDlg::OnNewLabel)

	ON_COMMAND(IDM_OPEN,   &CMainDlg::OnOpen)
	ON_COMMAND(IDM_SAVE,   &CMainDlg::OnOK)
	ON_COMMAND(IDM_NEW ,   &CMainDlg::OnNew)
	ON_COMMAND(IDM_EDIT,   &CMainDlg::OnEditFunction)
	ON_COMMAND(IDM_DELETE, &CMainDlg::OnRemove)
	ON_COMMAND(IDM_DIRECTORY, &CMainDlg::OnDirectory)

	ON_COMMAND(IDM_COPY,   &CMainDlg::OnCopy)
	ON_COMMAND(IDM_PASTE,  &CMainDlg::OnPaste)
	ON_COMMAND(IDM_CUT,    &CMainDlg::OnCut)
	ON_COMMAND(IDM_UNDO,   &CMainDlg::OnUndo)
	ON_COMMAND(IDM_REDO,   &CMainDlg::OnRedo)
	ON_COMMAND(IDM_CLEAR,  &CMainDlg::OnClearFormat)

	ON_COMMAND(IDM_IMAGE,   &CMainDlg::OnImage)
	ON_COMMAND(IDM_CAPTURE, &CMainDlg::OnScreenCapture)
	
	ON_COMMAND(IDM_FONT,   &CMainDlg::OnFont)

	ON_COMMAND(IDM_PARAGRAPH,   &CMainDlg::OnParagraph)
	ON_COMMAND(IDM_CENTER, &CMainDlg::OnCenter)
	ON_COMMAND(IDM_LEFT,   &CMainDlg::OnLeft)
	ON_COMMAND(IDM_RIGHT,  &CMainDlg::OnRight)

	ON_COMMAND(IDM_NONE,        &CMainDlg::OnNone)
	ON_COMMAND(IDM_SYMBOL,      &CMainDlg::OnSymbol)
	ON_COMMAND(IDM_NUMBER,      &CMainDlg::OnNumber)
	ON_COMMAND(IDM_LOWER_CASE,  &CMainDlg::OnLowerCase)
	ON_COMMAND(IDM_UPPER_CASE,  &CMainDlg::OnUpperCase)
	ON_COMMAND(IDM_LOWER_ROME,  &CMainDlg::OnLowerRome)
	ON_COMMAND(IDM_UPPER_ROME,  &CMainDlg::OnUpperRome)
	
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CMainDlg 消息处理程序


BOOL CMainDlg::OnInitDialog()
{
	//vmprotect 标记开始处.
	VMPBEGIN

	CDialogEx::OnInitDialog();

	// 设置图标
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// List
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT  //允许整行选中
		| LVS_EX_HEADERDRAGDROP				      //允许整列拖动
		| LVS_EX_SUBITEMIMAGES			          //单击选中项
		| LVS_EX_GRIDLINES);				      //画出网格线

	m_List.SetHeadings(_T("方法名称,165;"));
	m_List.LoadColumnInfo();

	// 字体
	LOGFONT   logfont;//最好弄成类成员,全局变量,静态成员  
    CFont   *pfont1   =   m_List.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //这里可以修改字体的高比例
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //这里可以修改字体的宽比例
    static   CFont   font1;
    font1.CreateFontIndirect(&logfont);
    m_List.SetFont(&font1);  
    font1.Detach();

	// 目录检测
	BOOL IsNew = false;
	CString FilePath = _T("File");
	DWORD Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		IsNew = true;
		CreateDirectory(FilePath, NULL);
	}

	FilePath = _T("Project");
	Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		IsNew = true;
		CreateDirectory(FilePath, NULL);
	}

	FilePath = _T("Code");
	Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		IsNew = true;
		CreateDirectory(FilePath, NULL);
	}

	// 读取窗口位置
	if( GetPrivateProfileInt(_T("Setting"), _T("Position"), 0, _T("./Setting.ini")) == 1)
	{
		// 读取最后关闭文件
		CString Position;

		::GetPrivateProfileString(_T("Position"), _T("WindowPosition"), _T(""), Position.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));
		Position.ReleaseBuffer();

		// 若位置不为空
		if(!Position.IsEmpty())
		{
			int Pos = Position.Find(","), m_Lift, m_Top, m_Width, m_Hight;
			CString Lift, Top, Width, Hight;

			if(Pos > 0)
			{
				Lift = Position.Left(Pos);
				Position = Position.Right(Position.GetLength() - Pos - 1);
				Pos  = Position.Find(",");

				Top  = Position.Left(Pos);
				Position = Position.Right(Position.GetLength() - Pos - 1);
				Pos  = Position.Find(",");

				Width = Position.Left(Pos);
				Position  = Position.Right(Position.GetLength() - Pos - 1);
				Pos   = Position.Find(",");

				Hight = Position.Left(Pos);
			}

			// 赋值
			m_Lift = atoi(Lift);
			m_Top  = atoi(Top);
			m_Width= atoi(Width);
			m_Hight= atoi(Hight);

			// 设置窗口位置
			SetWindowPos(NULL, m_Lift, m_Top, m_Width, m_Hight, SWP_NOSIZE);
		}
	}

	// 贴边自动隐藏
	SetTimer(1,100,NULL);

	// 判断是否第一次运行
	if(IsNew)
	{
		// 显示用户使用说明
		if (m_hOperate == NULL)
		{
			Type = 2;
			m_hOperate = AfxBeginThread(Operate, this);
			CloseHandle(m_hOperate->m_hThread);
		}
	}

	// 设置对话框
	m_Setting->Create(IDD_SETTING_DIALOG, this);
	m_Setting->ShowWindow(SW_HIDE);

	// 添加文件到目标
	if (m_hOperate == NULL)
	{
		Type = 0;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	// 检测更新
	SetWindowText(_T("紫影龙编程助理 ") + theApp.GetApplicationVersion());
	OnCheck();

	//vmprotect 标记结束处.
	VMPEND

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	VMPBEGIN

	// 键盘消息处理
	UINT  nKeyCode = pMsg->wParam; // virtual key code of the key pressed
	if (pMsg->message == WM_KEYDOWN)
	{
		// Ctrl + S (保存方法)
		if ( (nKeyCode == _T('S') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnOK();
			return true;
		}

		// Ctrl + N (添加方法)
		if ( (nKeyCode == _T('N') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnNew();
			return true;
		}

		// Ctrl + D (依赖文件)
		if ( (nKeyCode == _T('D') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnDirectory();
			return true;
		}

		// Ctrl + F (编辑字体)
		if ( (nKeyCode == _T('F') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnFont();
			return true;
		}

		// Ctrl + R (移除方法)
		if ( (nKeyCode == _T('R') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnRemove();
			return true;
		}

		// Ctrl + Q (清除格式)
		if( (nKeyCode == _T('Q') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnClearFormat();
			return true;
		}

		// Ctrl + E (编辑方法)
		if( (nKeyCode == _T('E') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnEditFunction();
			return true;
		}

		// Ctrl + I (插入图片)
		if ( (nKeyCode == _T('I') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnImage();
			return true;
		}

		// Ctrl + P (屏幕截屏)
		if ( (nKeyCode == _T('T') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnScreenCapture();
			return true;
		}

		// Ctrl + O (打开方法)
		if ( (nKeyCode == _T('O') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnOpen();
			return true;
		}

		// Ctrl + P (段落设置)
		if ( (nKeyCode == _T('P') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnParagraph();
			return true;
		}

		// Ctrl + <- (字体居左)
		if ( (nKeyCode == VK_LEFT && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnLeft();
			return true;
		}

		// Ctrl + -> (字体居右)
		if ( (nKeyCode == VK_RIGHT && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnRight();
			return true;
		}

		// Ctrl + M (字体居中)
		if ( (nKeyCode == _T('M') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnCenter();
			return true;
		}

		// 空格 取消选择
		if(nKeyCode == VK_SPACE)
		{
			if (GetDlgItem(IDC_CODE_RICHEDIT) == GetFocus())
			{
				// 不拦截回车换行
			}
			else if (GetDlgItem(IDC_CODE_LIST) == GetFocus())
			{
				// 取消选择
				m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);
				GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("添加方法"));
				return true;
			}
		}

		if(nKeyCode == VK_RETURN)
		{
			//焦点是否在控件上
			if (GetDlgItem(IDC_CODE_LIST) == GetFocus())
			{
				CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
				if (!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
				{
					// 使用写字板打开
					if (m_hOperate == NULL)
					{
						Type = 8;
						m_hOperate = AfxBeginThread(Operate, this);
						CloseHandle(m_hOperate->m_hThread);
					}

					// 屏蔽回车消息
					return true;
				}
			}
			else if (GetDlgItem(IDC_CODE_RICHEDIT) == GetFocus())
			{
				// 不拦截回车换行
			}
			else
			{
				// 屏蔽回车消息
				return true;
			}
		}
	}

	VMPEND
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	CRect rc;
	CRect rect;
	GetWindowRect(&rect);//窗体大小
	rc.CopyRect(&rect);//复制矩形区
	CPoint pOint;
	GetCursorPos(&pOint);//获取鼠标位置

	//if(!Hide)
	//{
	//	rect.top = 0;
	//	MoveWindow(rect.left,rect.top,rc.Width(),rc.Height());//移动窗体
	//	return;
	//}

	if(rect.top < 0 && PtInRect(rect,pOint))//显示窗体
	{
		rect.top = 0;
		MoveWindow(rect.left,rect.top,rc.Width(),rc.Height());//移动窗体
	}

	else if(rect.top > -3 && rect.top < 3 && !PtInRect(rect,pOint))//隐藏窗体
	{
		rect.top = 3-rect.Height();
		MoveWindow(rect.left,rect.top,rc.Width(),rc.Height());
	}

	CDialogEx::OnTimer(nIDEvent);
}


// 编码助理
void CMainDlg::ClearScreen()
{
	HDC hScreenDC;

	//创建屏幕设备表述表
	hScreenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	//假如先前窗口不为NULL 则清除在窗口上所绘制的矩形框
	HDC hMemDC;

	//创建与屏幕兼容的内存设备描述表句柄	
	hMemDC = CreateCompatibleDC(hScreenDC);	

	//将先前保存的窗口区域位图选进设备表述表当中
	SelectObject(hMemDC, m_rectOfWndBmp);

	//将先前保存的窗口区域位图拷贝到屏幕
	BitBlt(hScreenDC, m_rectOfWnd.left, m_rectOfWnd.top, m_rectOfWnd.right - m_rectOfWnd.left, 
			m_rectOfWnd.bottom - m_rectOfWnd.top, hMemDC, 0, 0, SRCCOPY);		

	DeleteObject(m_rectOfWndBmp);
	m_rectOfWndBmp = NULL;
	DeleteDC(hMemDC);
	DeleteDC(hScreenDC);
}


void CMainDlg::SnapWindow(POINT point)
{
	ClientToScreen(&point);
	HDC hScreenDC;

	//得到鼠标位置的窗口句柄
	m_curWnd = ::WindowFromPoint(point);

	if (m_curWnd == m_preWnd || m_curWnd == m_hWnd || ::GetParent(m_curWnd) == m_curWnd)
		return;

	if (m_rectOfWndBmp != NULL)
		ClearScreen();

	//得到当前窗口矩形区域
	::GetWindowRect(m_curWnd, &m_rectOfWnd);
	if (m_rectOfWnd.left < 0)
		m_rectOfWnd.left = 0;
	if (m_rectOfWnd.top <0)
		m_rectOfWnd.top = 0;

	//保存当前窗口矩形区域的内容
	m_rectOfWndBmp = CopyScreenToBitmap(&m_rectOfWnd);
	m_preWnd = m_curWnd;
	CBrush brush;

	//创建红色画刷
	brush.CreateSolidBrush(RGB(255, 0, 0));

	//创建屏幕设备表述表
	hScreenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	//在屏幕窗口位置绘制矩形
	FrameRect(hScreenDC, &m_rectOfWnd, (HBRUSH)brush.m_hObject);
	brush.DeleteObject();
	DeleteDC(hScreenDC);
}


void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCursor(m_hCursor);
	SetCapture();
	m_bSnap = TRUE;

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMainDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	ClearScreen();
	OnCodeAssistant();

	m_preWnd = NULL;
	m_curWnd = NULL;
	m_bSnap = FALSE;

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bSnap)
		SnapWindow(point);

	//CDialogEx::OnMouseMove(nFlags, point);
}


void CMainDlg::OnCodeAssistant()
{
	// 找到最上层的父窗口
	HWND m_hWnd = ::GetParent(m_curWnd);

	if(IsWindow(m_hWnd))
	{
		if(m_hWnd == AfxGetApp()->GetMainWnd()->GetSafeHwnd() || m_hWnd == m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->GetSafeHwnd())
		{
			AfxMessageBox(_T("不能选择程序自身作为目标!"));
			return;
		}

		if(m_pAssisDlg->TargetWnd == m_hWnd)
		{
			AfxMessageBox(_T("目标已选择, 无需再次选择目标!"));
			return;
		}

		// 最小化主窗口
		ShowWindow(SW_MINIMIZE);

		// 释放资源
		if(m_pAssisDlg != NULL)
		{
			m_pAssisDlg->DestroyWindow();
			delete m_pAssisDlg;
		}

		// 新建对象
		m_pAssisDlg = new CAssistantDlg;

		// 将目标窗口作为父对象创建助理对话框
		m_pAssisDlg->Create(IDD_ASSISTANT_DIALOG, FromHandle(m_hWnd));
		m_pAssisDlg->CenterWindow();
		m_pAssisDlg->TargetWnd = m_hWnd;
		m_pAssisDlg->FilePath = _T("Code\\") + CurClass + _T("\\") + CurType;
		m_pAssisDlg->OnSetCode();
	}
	else
	{
		// 不是窗口就返回
		if(IsWindow(m_curWnd))
			return;

		if(m_curWnd == AfxGetApp()->GetMainWnd()->GetSafeHwnd() || m_hWnd == m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->GetSafeHwnd())
		{
			AfxMessageBox(_T("不能选择程序自身作为目标!"));
			return;
		}

		if(m_pAssisDlg->TargetWnd == m_curWnd)
		{
			AfxMessageBox(_T("目标已选择, 无需再次选择目标!"));
			return;
		}

		// 最小化主窗口
		ShowWindow(SW_MINIMIZE);

		// 释放资源
		if(m_pAssisDlg != NULL)
		{
			m_pAssisDlg->DestroyWindow();
			delete m_pAssisDlg;
		}

		// 新建对象
		m_pAssisDlg = new CAssistantDlg;

		// 将目标窗口作为父对象创建助理对话框
		m_pAssisDlg->Create(IDD_ASSISTANT_DIALOG, FromHandle(m_curWnd));
		m_pAssisDlg->CenterWindow();
		m_pAssisDlg->TargetWnd = m_curWnd;
		m_pAssisDlg->FilePath = _T("Code\\") + CurClass + _T("\\") + CurType;
		m_pAssisDlg->OnSetCode();
	}
}


// 子窗口消息
LRESULT CMainDlg::OnMessageChild(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case 1:
		Refresh();
		break;

	case 2:
		OnHelp();
		break;

	case 3:
		OnCheck();
		break;

	case 4:
		// 自动保存
		if (m_hOperate == NULL)
		{
			Type = 7;
			m_hOperate = AfxBeginThread(Operate, this);
			CloseHandle(m_hOperate->m_hThread);
		}
		break;

	case 5:
		if(!IsEdit)
		{
			GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("添加方法"));
			IsNew = TRUE;
			IsEdit=FALSE;
		}
		else
			IsEdit=FALSE;
		break;

	case 6:
		OnOK();
		break;

	case 7:
		OnNew();
		break;

	case 8:
		OnDirectory();
		break;

	case 9:
		OnFont();
		break;

	case 10:
		OnRemove();
		break;

	case 11:
		OnClearFormat();
		break;

	case 12:
		OnEditFunction();
		break;

	case 13:
		OnImage();
		break;

	case 14:
		OnScreenCapture();
		break;

	case 15:
		OnOpen();
		break;

	case 16:
		OnParagraph();
		break;

	case 17:
		OnLeft();
		break;

	case 18:
		OnRight();
		break;

	case 19:
		OnCenter();
		break;

	case 20:
		m_Tab.OnRButtonDown(NULL, NULL);
		break;

	case 21:
		OnSetLabel();
		break;

	case 22:
		m_pAssisDlg->FilePath = _T("Code\\") + CurClass + _T("\\") + CurType;
		m_pAssisDlg->OnSetCode();
		break;

	case 23:
		CString Target = (LPCTSTR)lParam;
		CString Code = theApp.CodePath = _T("Code\\") + CurClass + _T("\\") + CurType + _T("\\") + Target + _T(".code");

		// 新建标签
		FileClass = _T("");
		FileType  = _T("");
		FileName  = Target;
		FilePath  = Code;
		SendMessage(WM_COMMAND, 106);

		// 标记外部方法
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsOutFunction = TRUE;

		// 解压缩编码
		Uncompress(Code, Code.Left(Code.GetLength() -5) + _T(".rtf"));

		// 读取rtf文件
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.StreamInFromResource(Code.Left(Code.GetLength() -5) + _T(".rtf"), _T("SF_RTF"));

		// 删除原文件
		DeleteFile(Code.Left(Code.GetLength() -5) + _T(".rtf"));

		// 复制内容
		CString Source;
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetWindowText(Source);

		//put   your   text   in   source
		if(OpenClipboard())
		{
			HGLOBAL   clipbuffer;
			char   *   buffer;
			EmptyClipboard();
			clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   Source.GetLength() +1);
			buffer   =   (char*)GlobalLock(clipbuffer);
			strcpy_s(buffer, 65535 ,LPCSTR((CStringA)Source));
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT,clipbuffer);
			CloseClipboard();
		}

		// 关闭标签
		m_Tab.OnRButtonDown(NULL, NULL);
		break;
	}

	return TRUE;
}


// 工作线程
UINT CMainDlg::Operate(LPVOID pParam)
{
	// 窗口指针
	CMainDlg * pWnd = ((CMainDlg*)pParam);

	// 异常捕获
	try
	{
		switch(pWnd->Type)
		{
		case 0:
			{
				// 初始化
				CString FilePath = _T("Code");

				// Class
				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						pWnd->m_Class.AddString(Finder.GetFileName());
				}

				pWnd->m_Class.SetCurSel(0);

				// Type
				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				FilePath = _T("Code\\") + Class;

				IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						pWnd->m_Type.AddString(Finder.GetFileName());
				}

				pWnd->m_Type.SetCurSel(0);


				// Code
				pWnd->m_Class.GetWindowText(Class);
				pWnd->m_Type.GetWindowText(Type);
				FilePath = _T("Code\\") + Class + _T("\\") + Type;

				// 设置当前属性
				pWnd->CurClass = Class;
				pWnd->CurType  = Type;

				IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						continue;
					else
					{
						CString Name = Finder.GetFileName();
						pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));
					}
				}

				// 读取参数并打开code文件
				if(!theApp.CodePath.IsEmpty())
				{
					// 读取参数
					if(theApp.CodePath.Right(5) != _T(".code"))
						theApp.CodePath += _T(".code");

					// 参数赋值
					CString Code = theApp.CodePath;

					// 标签参数
					CString Path = Code.Left(Code.GetLength() -5);
					Path.Replace(_T("\\"), _T("/"));
					CString Name = Path.Right(Path.GetLength() - Path.ReverseFind('/') -1);

					// 新建标签
					pWnd->FileName = Name;
					pWnd->FilePath = Code;
					pWnd->SendMessage(WM_COMMAND, 106);

					// 外部方法标记
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->IsOutFunction = TRUE;

					// 解压缩编码
					pWnd->Uncompress(Code, Code.Left(Code.GetLength() -5) + _T(".rtf"));

					// 读取rtf文件
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamInFromResource(Code.Left(Code.GetLength() -5) + _T(".rtf"), _T("SF_RTF"));

					// 删除原文件
					DeleteFile(Code.Left(Code.GetLength() -5) + _T(".rtf"));

					// 若读取一次
					if(theApp.IsReadOnce)
						DeleteFile(Code);
				}

				// 自动打开
				else if(GetPrivateProfileInt(_T("Setting"), _T("Open"), 0, _T("./Setting.ini")) == 1)
				{
					// 读取最后关闭文件
					CString Class, Type, Name;

					::GetPrivateProfileString(_T("File"), _T("Class"), _T(""), Class.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));
					::GetPrivateProfileString(_T("File"), _T("Type"),  _T(""), Type.GetBuffer(MAX_PATH),  MAX_PATH, _T("./Setting.ini"));
					::GetPrivateProfileString(_T("File"), _T("Name"),  _T(""), Name.GetBuffer(MAX_PATH),  MAX_PATH, _T("./Setting.ini"));

					Class.ReleaseBuffer();
					Type.ReleaseBuffer();
					Name.ReleaseBuffer();

					// 自动打开文件
					int nIndex = pWnd->m_Class.FindStringExact(0, Class);
					if(nIndex != CB_ERR)
					{
						pWnd->m_Class.SetCurSel(nIndex);

						// 清空ComboBox
						pWnd->m_Type.ResetContent();

						CString Class;
						pWnd->m_Class.GetWindowText(Class);
						CString FilePath = _T("Code\\") + Class;

						CFileFind Finder;
						BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
						while (IsFind)
						{
							IsFind = Finder.FindNextFile();

							if (Finder.IsDots())
								continue;
							if (Finder.IsDirectory())
								pWnd->m_Type.AddString(Finder.GetFileName());
						}

						// 设置ComboBox
						pWnd->m_Type.SetCurSel(0);
					}
					else
					{
						// 错误消息提示
						pWnd->Error = _T("无法读取配置文件中 File 下的 Class 数据, 自动打开失败。");
						pWnd->PostMessage(WM_COMMAND, 101);

						// 对象置为空
						pWnd->m_hOperate = NULL;
						return FALSE;
					}

					nIndex = pWnd->m_Type.FindStringExact(0, Type);
					if(nIndex != CB_ERR)
					{
						pWnd->m_Type.SetCurSel(nIndex);

						// 清空列表
						pWnd->m_List.DeleteAllItems();

						CString Class, Type;
						pWnd->m_Class.GetWindowText(Class);
						pWnd->m_Type.GetWindowText(Type);
						CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

						CFileFind Finder;
						BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
						while (IsFind)
						{
							IsFind = Finder.FindNextFile();

							if (Finder.IsDots())
								continue;
							if (Finder.IsDirectory())
								continue;
							else
							{
								CString Name = Finder.GetFileName();
								pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));
							}
						}
					}
					else
					{
						// 错误消息提示
						pWnd->Error = _T("无法读取配置文件中 File 下的 Type 数据, 自动打开失败。");
						pWnd->PostMessage(WM_COMMAND, 101);

						// 对象置为空
						pWnd->m_hOperate = NULL;
						return FALSE;
					}

					if(Name.IsEmpty())
					{
						// 错误消息提示
						pWnd->Error = _T("无法读取配置文件中 File 下的 Name 数据, 自动打开失败。");
						pWnd->PostMessage(WM_COMMAND, 101);

						// 对象置为空
						pWnd->m_hOperate = NULL;
						return FALSE;
					}

					int Count = pWnd->m_List.GetItemCount();
					for(int i=0; i<Count; i++)
					{
						CString Function = pWnd->m_List.GetItemText(i, 0);

						if(Function == Name)
						{
							//设置高亮显示  
							pWnd->m_List.SetFocus();//设置焦点  
							pWnd->m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
							pWnd->m_List.EnsureVisible(i, FALSE);//设置当前视图可见
							goto read;
							break;
						}
					}
				}
				else
				{   // 新建标签
					pWnd->FileClass = _T("");
					pWnd->FileType  = _T("");;
					pWnd->FileName  = _T("新方法");
					pWnd->FilePath  = _T("");
					pWnd->SendMessage(WM_COMMAND, 106);
                }
			}break;

		case 1:
			{
read:
				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				pWnd->m_Type.GetWindowText(Type);

				CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");
				int i = pWnd->m_List.GetNextItem(-1, LVIS_SELECTED);
				CString Name = pWnd->m_List.GetItemText(i, 0);

				// 更改按钮文本
				if(pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileName != _T("新方法"))
				{
					pWnd->IsEdit = TRUE;
					pWnd->GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("编辑方法"));

					// 编辑标记
					pWnd->IsNew = FALSE;
				}

				// 目标
				CString Target = FilePath + Name;

				// 判断是否已打开
				for(int i=0; i<(int)pWnd->m_Tab.m_DocumentTab.size(); i++)
				{
					// 标签 Path
					CString Path = pWnd->m_Tab.m_DocumentTab.at(i)->FilePath;
					CString Name = pWnd->m_Tab.m_DocumentTab.at(i)->FileName;

					// 跳到已打开的标签
					if(Target == Path + Name)
					{
						pWnd->m_Tab.SetCurSel(i);
						pWnd->m_Tab.OnLButtonDown(NULL, NULL);

						// 对象置为空
						pWnd->m_hOperate = NULL;
						return TRUE;
					}
				}

				if (!Name.IsEmpty() && i != -1)
				{
					// 显示消息提示
					//AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

					// 新建标签
					pWnd->FileClass = Class;
					pWnd->FileType  = Type;
					pWnd->FileName  = Name;
					pWnd->FilePath  = FilePath;
					pWnd->SendMessage(WM_COMMAND, 106);

					// 解压缩编码
					pWnd->Uncompress(Target + _T(".code"), Target + _T(".rtf"));

					// 读取rtf文件
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamInFromResource(Target + _T(".rtf"), _T("SF_RTF"));

					// 删除原文件
					DeleteFile(FilePath + Name + _T(".rtf"));

					// 操作完成标志
					//theApp.IsFinished = true;
				}
			}break;

		case 2:
			{
				// 新建标签
				pWnd->FileClass = _T("");
				pWnd->FileType  = _T("");;
				pWnd->FileName  = _T("使用说明");
				pWnd->FilePath  = _T("使用说明.code");
				pWnd->SendMessage(WM_COMMAND, 106);

				// 解压缩编码
				pWnd->Uncompress(_T("使用说明.code"), _T("使用说明.rtf"));

				// 显示用户使用说明
				pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamInFromResource(_T("使用说明.rtf"), _T("SF_RTF"));

				// 删除原文件
				DeleteFile(_T("使用说明.rtf"));
			}break;

		case 3:
			{
				// 清空ComboBox
				pWnd->m_Type.ResetContent();

				CString Class;
				pWnd->m_Class.GetWindowText(Class);
				CString FilePath = _T("Code\\") + Class;

				// 设置当前数据
				pWnd->CurClass = Class;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						pWnd->m_Type.AddString(Finder.GetFileName());
				}

				// 设置ComboBox
				pWnd->m_Type.SetCurSel(0);
			}break;

		case 4:
			{
				// 清空编辑框
				//pWnd->m_Edit.SetWindowText(_T(""));

				// 清空列表
				pWnd->m_List.DeleteAllItems();

				// 重置ComboBox
				pWnd->m_Class.ResetContent();
				pWnd->m_Type.ResetContent();

				CString FilePath = _T("Code");
				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						pWnd->m_Class.AddString(Finder.GetFileName());
				}

				pWnd->m_Class.SetCurSel(0);
			}break;

		case 5:
			{
				// 清空编辑框
				//pWnd->m_Edit.SetWindowText(_T(""));

				// 清空列表
				pWnd->m_List.DeleteAllItems();

				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				pWnd->m_Type.GetWindowText(Type);
				CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

				// 设置当前数据
				pWnd->CurClass = Class;
				pWnd->CurType = Type;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						continue;
					else
					{
						CString Name = Finder.GetFileName();
						pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));
					}
				}
			}break;

		case 6:
			{
				// 清空编辑框
				//pWnd->m_Edit.SetWindowText(_T(""));

				// 清空列表
				pWnd->m_List.DeleteAllItems();

				// 重置ComboBox
				pWnd->m_Type.ResetContent();

				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				CString FilePath = _T("Code\\") + Class;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						pWnd->m_Type.AddString(Finder.GetFileName());
				}

				pWnd->m_Type.SetCurSel(0);
			}break;

		case 7:
			{
				// 若设置了自动保存
				if ( GetPrivateProfileInt(_T("Setting"), _T("Save"), 0, _T("./Setting.ini")) == 1)
				{
					CString FilePath = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath, TargetFile = FilePath.Left(FilePath.GetLength() -5);

					// 外部文件
					if( pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->IsOutFunction && !FilePath.IsEmpty())
					{
						pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(TargetFile + _T(".rtf"), _T("SF_RTF"));

						// 压缩编码
						pWnd->Compress(TargetFile + _T(".rtf"), TargetFile + _T(".code"));

						// 删除原文件
						DeleteFile(TargetFile + _T(".rtf"));
					}
					else if(!FilePath.IsEmpty())
					{
						CString Class = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileClass, Type = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileType, Text;
						pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.GetWindowText(Text);

						// 如果分类信息不完整
						if(Class.IsEmpty() || Type.IsEmpty())
						{
							// 对象置为空
							pWnd->m_hOperate = NULL;
							return false;
						}
						else
						{
							// 已有源码
							if(!Text.IsEmpty())
							{
								// 目录检测
								DWORD DirPath = GetFileAttributes(_T("Code\\") + Class);
								if (DirPath == 0xFFFFFFFF)     //文件夹不存在
								{
									CreateDirectory(_T("Code\\") + Class, NULL);
								}

								DirPath = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
								if (DirPath == 0xFFFFFFFF)     //文件夹不存在
								{
									CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
								}

								// 自动保存
								CString Target = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath + pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileName;
								pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(Target + _T(".rtf"), _T("SF_RTF"));

								// 压缩编码
								pWnd->Compress(Target + _T(".rtf"), Target + _T(".code"));

								// 删除原文件
								DeleteFile(Target + _T(".rtf"));
							}
						}
					}
				}
			}break;

		case 8:
			{
				CString Name = pWnd->m_List.GetItemText(pWnd->m_List.GetNextItem(-1, LVIS_SELECTED), 0);
				if (!Name.IsEmpty() && pWnd->m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
				{
					CString Class, Type;
					pWnd->m_Class.GetWindowText(Class);
					pWnd->m_Type.GetWindowText(Type);
					CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

					// 解压缩编码
					pWnd->Uncompress(FilePath + Name + _T(".code"), FilePath + Name + _T(".rtf"));

					// 调用命令行 执行
					//ShellExecute(NULL, NULL, _T("write.exe"), FilePath + Name + _T(".rtf"), NULL, SW_HIDE);

					SHELLEXECUTEINFO ShExecInfo = {0};
					ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
					ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

					ShExecInfo.lpFile       = _T("write.exe");
					ShExecInfo.lpDirectory  = NULL;
					ShExecInfo.lpParameters = FilePath + Name + _T(".rtf");

					ShExecInfo.nShow = SW_SHOW;
					ShellExecuteEx(&ShExecInfo);

					// 最小化主窗口
					pWnd->ShowWindow(SW_MINIMIZE);

					// 显示消息提示
					//AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

					AfxGetApp()->BeginWaitCursor();
					WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
					AfxGetApp()->EndWaitCursor();
					
					// 还原主窗口
					pWnd->ShowWindow(SW_RESTORE);

					// 删除原文件
					DeleteFile(FilePath + Name + _T(".rtf"));

					// 操作完成标志
					//theApp.IsFinished = true;
				}
			}break;

		case 9:
			{
				CString FilePath = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath, TargetFile = FilePath.Left(FilePath.GetLength() -5);

				// 外部文件
				if( pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->IsOutFunction && !FilePath.IsEmpty())
				{
					// 显示消息提示
					AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

					CString TargetFile = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath.Left(pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath.GetLength() -5);
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(TargetFile + _T(".rtf"), _T("SF_RTF"));

					// 压缩编码
					pWnd->Compress(TargetFile + _T(".rtf"), TargetFile + _T(".code"));

					// 删除原文件
					DeleteFile(TargetFile + _T(".rtf"));

					// 操作完成标志
					theApp.IsFinished = true;

					// 消息提示
					pWnd->PostMessage(WM_COMMAND, 103);
				}
				else if(!FilePath.IsEmpty())
				{
					CString Class = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileClass, Type = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileType;

					if(Class.IsEmpty())
					{
						// 错误消息提示
						pWnd->Error = _T("请输入类型!");
						pWnd->PostMessage(WM_COMMAND, 101);

						// 对象置为空
						pWnd->m_hOperate = NULL;
						return false;
					}
					else if(Type.IsEmpty())
					{
						// 错误消息提示
						pWnd->Error = _T("请输入类别!");
						pWnd->PostMessage(WM_COMMAND, 101);

						// 对象置为空
						pWnd->m_hOperate = NULL;
						return false;
					}

					// 目录检测
					DWORD DirPath = GetFileAttributes(_T("Code\\") + Class);
					if (DirPath == 0xFFFFFFFF)     //文件夹不存在
					{
						CreateDirectory(_T("Code\\") + Class, NULL);
					}

					DirPath = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
					if (DirPath == 0xFFFFFFFF)     //文件夹不存在
					{
						CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
					}

					// 显示消息提示
					AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

					CString TargetFile = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath + pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileName;
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(TargetFile + _T(".rtf"), _T("SF_RTF"));

					// 压缩编码
					pWnd->Compress(TargetFile + _T(".rtf"), TargetFile + _T(".code"));

					// 删除原文件
					DeleteFile(TargetFile + _T(".rtf"));

					// 操作完成标志
					theApp.IsFinished = true;

					// 消息提示
					pWnd->PostMessage(WM_COMMAND, 103);

					// 判断是否是新方法
					//if(!Path.IsEmpty() && pWnd->m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
					//{
					//	// 显示消息提示
					//	AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

					//	CString TargetFile = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath + pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileName;
					//	pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(TargetFile + _T(".rtf"), _T("SF_RTF"));

					//	// 压缩编码
					//	pWnd->Compress(TargetFile + _T(".rtf"), TargetFile + _T(".code"));

					//	// 删除原文件
					//	DeleteFile(TargetFile + _T(".rtf"));

					//	// 操作完成标志
					//	theApp.IsFinished = true;

					//	// 消息提示
					//	pWnd->PostMessage(WM_COMMAND, 103);
					//}
					//else
					//{
					//	// 询问是否新建
					//	pWnd->PostMessage(WM_COMMAND, 104);
					//}
				}
				else
					goto New;
			}break;

		case 10:
			{
				New:
				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				pWnd->m_Type.GetWindowText(Type);

				if(Class.IsEmpty())
				{
					// 错误消息提示
					pWnd->Error = _T("请输入类型!");
					pWnd->PostMessage(WM_COMMAND, 101);

					// 对象置为空
					pWnd->m_hOperate = NULL;
					return false;
				}
				else if(Type.IsEmpty())
				{
					// 错误消息提示
					pWnd->Error = _T("请输入类别!");
					pWnd->PostMessage(WM_COMMAND, 101);

					// 对象置为空
					pWnd->m_hOperate = NULL;
					return false;
				}

				// 目录检测
				DWORD DirPath = GetFileAttributes(_T("Code\\") + Class);
				if (DirPath == 0xFFFFFFFF)     //文件夹不存在
				{
					CreateDirectory(_T("Code\\") + Class, NULL);
				}

				DirPath = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
				if (DirPath == 0xFFFFFFFF)     //文件夹不存在
				{
					CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
				}

				CString File = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

				TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
				GetModuleFileName(NULL,exeFullPath,MAX_PATH);//得到程序模块名称，全路径
				CString Path(exeFullPath), Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1), Dir = Path.Left(Path.GetLength() - Name.GetLength()) + File;

				CFileDialog FileDlg(TRUE, _T("code"), _T("Function"), OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, _T("Code File(*.code)|*.code;||"), pWnd);
				FileDlg.m_ofn.lpstrInitialDir = Dir;
				if(FileDlg.DoModal() != IDOK)
				{
					// 对象置为空
					pWnd->m_hOperate = NULL;
					return false;
				}
				else
				{
					// 显示消息提示
					AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

					CString strFile = FileDlg.GetPathName(); //文件不存在就自动创建
					CString Target  = strFile.Left(strFile.GetLength() -5);
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(Target + _T(".rtf"), _T("SF_RTF"));

					// 压缩编码
					pWnd->Compress(Target + _T(".rtf"), Target + _T(".code"));

					// 删除原文件
					DeleteFile(Target + _T(".rtf"));

					// 操作完成标志
					theApp.IsFinished = true;

					// 添加 & 标记
					BOOL  isFind = FALSE;
					CString Name = FileDlg.GetFileName();

					// 更改标签属性
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileClass = Class;
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileType  = Type;
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath  = File;
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileName  = Name.Left(Name.GetLength() - 5);

					pWnd->m_Tab.SetPageTitle(pWnd->m_Tab.m_selTabID, (TCHAR*)(LPCTSTR)Name.Left(Name.GetLength() - 5));
					pWnd->m_Tab.Invalidate();

					// 取消选择
					pWnd->m_List.SetItemState(pWnd->m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

					int Count = pWnd->m_List.GetItemCount();
					for(int i=0; i<Count; i++)
					{
						CString Function = pWnd->m_List.GetItemText(i, 0);

						if(Function == Name.Left(Name.GetLength() - 5))
						{
							//标志
							isFind = TRUE;

							//设置高亮显示  
							pWnd->m_List.SetFocus();//设置焦点  
							pWnd->m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
							pWnd->m_List.EnsureVisible(i, FALSE);//设置当前视图可见 
							break;
						}
					}

					if(!isFind)
					{
						// 加入列表
						int i = pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));

						//设置高亮显示  
						pWnd->m_List.SetFocus();//设置焦点  
						pWnd->m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
						pWnd->m_List.EnsureVisible(i, FALSE);//设置当前视图可见 
					}

					// 消息提示
					pWnd->PostMessage(WM_COMMAND, 103);
				}
			}break;

			case 11:
			{
				int nItem = -1;
				POSITION pos;

				// 清空编辑框
				pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.SetWindowText(_T(""));

				// 显示消息提示
				AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

				// 移除方法
				while (pos = pWnd->m_List.GetFirstSelectedItemPosition())
				{
					nItem = -1;
					nItem = pWnd->m_List.GetNextSelectedItem(pos);
					if (nItem >= 0 && pWnd->m_List.GetSelectedCount() > 0)
					{
						CString Name = pWnd->m_List.GetItemText(nItem, 0);

						CString Class, Type;
						pWnd->m_Class.GetWindowText(Class);
						pWnd->m_Type.GetWindowText(Type);

						// 删除文件
						DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Name + _T(".code"));

						// 移除列表
						pWnd->m_List.DeleteItem(nItem);

						// 删除依赖
						pWnd->DeleteDirectory(_T("File\\") + Class + _T("\\") + Type + _T("\\") + Name);

						// 判断是否已打开
						for(int i=0; i<(int)pWnd->m_Tab.m_DocumentTab.size(); i++)
						{
							// 标签 Path
							CString Code = _T("Code\\") + Class + _T("\\") + Type + _T("\\") + Name, Target = pWnd->m_Tab.m_DocumentTab.at(i)->FilePath + pWnd->m_Tab.m_DocumentTab.at(i)->FileName;

							// 关闭已打开的标签
							if( Code == Target )
							{
								pWnd->m_Tab.SetCurSel(i);
								pWnd->m_Tab.OnRButtonDown(NULL, NULL);
							}
						}

						// 判断是否为空
						if( pWnd->CountFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\")) <= 0 )
						{
							pWnd->DeleteDirectory( _T("Code\\") + Class + _T("\\") + Type + _T("\\") );

							// 处理类别

							// 删除ComboBox 里的值
							pWnd->m_Type.DeleteString(pWnd->m_Type.GetCurSel());

							// 设置选项
							pWnd->m_Type.SetCurSel(0);

							// 内容变更消息

							// 清空列表
							pWnd->m_List.DeleteAllItems();

							pWnd->m_Class.GetWindowText(Class);
							pWnd->m_Type.GetWindowText(Type);
							CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

							CFileFind Finder;
							BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
							while (IsFind)
							{
								IsFind = Finder.FindNextFile();

								if (Finder.IsDots())
									continue;
								if (Finder.IsDirectory())
									continue;
								else
								{
									CString Name = Finder.GetFileName();
									pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));
								}
							}
						}

						if( pWnd->CountFile(_T("Code\\") + Class + _T("\\")) <= 0 )
						{
							pWnd->DeleteDirectory( _T("Code\\") + Class + _T("\\") );

							// 处理类型与类别

							// 删除ComboBox 里的值
							pWnd->m_Class.DeleteString(pWnd->m_Class.GetCurSel());
							pWnd->m_Type.DeleteString(pWnd->m_Type.GetCurSel());

							// 设置选项
							pWnd->m_Class.SetCurSel(0);

							// 内容变更消息

							// 清空编辑框
							pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.SetWindowText(_T(""));

							// 清空ComboBox
							pWnd->m_Type.ResetContent();

							CString Class;
							pWnd->m_Class.GetWindowText(Class);
							CString FilePath = _T("Code\\") + Class;

							CFileFind Finder;
							BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
							while (IsFind)
							{
								IsFind = Finder.FindNextFile();

								if (Finder.IsDots())
									continue;
								if (Finder.IsDirectory())
									pWnd->m_Type.AddString(Finder.GetFileName());
							}

							// 设置ComboBox
							pWnd->m_Type.SetCurSel(0);


							// 清空列表
							pWnd->m_List.DeleteAllItems();

							pWnd->m_Class.GetWindowText(Class);
							pWnd->m_Type.GetWindowText(Type);
							FilePath = _T("Code\\") + Class + _T("\\") + Type;

							IsFind = Finder.FindFile(FilePath + _T("./*.*"));
							while (IsFind)
							{
								IsFind = Finder.FindNextFile();

								if (Finder.IsDots())
									continue;
								if (Finder.IsDirectory())
									continue;
								else
								{
									CString Name = Finder.GetFileName();
									pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));
								}
							}
						}
					}
				}

				// 操作完成标志
				theApp.IsFinished = true;

			}break;

			case 12:
			{
				CString sFilter = _T("Bmp Files(*.bmp)|*.bmp||");
				CFileDialog dlg(TRUE,0,0, OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING| OFN_FILEMUSTEXIST ,(LPCTSTR)sFilter, pWnd);
				if(dlg.DoModal()==IDOK)
				{
					pWnd->FilePath = dlg.GetPathName();

					// 插入图片
					pWnd->PostMessage(WM_COMMAND, 105);
				}

				pWnd->m_pPicObj = NULL;
			}break;

			case 13:
			{
				CString sFilter = _T("Code Files(*.code)|*.code||");
				CFileDialog dlg(TRUE, 0, 0, OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING| OFN_FILEMUSTEXIST ,(LPCTSTR)sFilter, pWnd);
				if(dlg.DoModal() == IDOK)
				{
					// 取消选择
					pWnd->m_List.SetItemState(pWnd->m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

					// 参数赋值
					CString Code = theApp.CodePath = dlg.GetPathName();

					// 新建标签
					CString Name    = Code.Right(Code.GetLength() - Code.ReverseFind('\\') -1);
					        Name    = Name.Left(Name.GetLength() -5);

					pWnd->FileClass = _T("");
					pWnd->FileType  = _T("");
					pWnd->FileName  = Name;
					pWnd->FilePath  = Code;
					pWnd->SendMessage(WM_COMMAND, 106);

					// 标记外部方法
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->IsOutFunction = TRUE;

					// 显示消息提示
					//AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

					// 解压缩编码
					pWnd->Uncompress(Code, Code.Left(Code.GetLength() -5) + _T(".rtf"));

					// 读取rtf文件
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamInFromResource(Code.Left(Code.GetLength() -5) + _T(".rtf"), _T("SF_RTF"));

					// 删除原文件
					DeleteFile(Code.Left(Code.GetLength() -5) + _T(".rtf"));

					// 操作完成标志
					//theApp.IsFinished = true;
				}
			}break;

			case 14:
			{
				try
				{
					// 隐藏主窗口
					pWnd->ShowWindow(SW_HIDE);

					// 显示工具
					CScreenToolDlg dlg;
					INT_PTR nResponse = dlg.DoModal();

					// 显示主窗口
					pWnd->ShowWindow(SW_SHOW);

					// 窗口置顶
					pWnd->SetForegroundWindow();

					// 粘贴截屏
					if(nResponse == IDOK)
						pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.Paste();
				}
				catch(...)
				{
					AfxMessageBox(_T("发生了未知异常，位于第14号线程!"));
				}
			}break;
		}
	}
	catch(...)
	{
		AfxMessageBox(_T("发生了异常, 位于CMainDlg的Operate方法."));
	}

	// 对象置为空
	pWnd->m_hOperate = NULL;
	return TRUE;
}


// 检查更新
UINT CMainDlg::UpDate(LPVOID pParam)
{
	// 窗口指针
	CMainDlg * pWnd = ((CMainDlg*)pParam);
	BOOL IsSuccess  = false;

	// 获取服务器数据
	try
	{
		CString RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetUpDataInfo"), 80, NULL, IsSuccess);
		if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		{
			/*pWnd->Error = _T("无法连接到服务器, 请检查网络。");
			pWnd->PostMessage(WM_COMMAND, 101);*/
		}
		else
		{
			if (IsSuccess)
			{
				if( RecvData.Replace(_T(";"), _T(";")) )
				{
					pWnd->UpDateInfo = RecvData;
					pWnd->PostMessage(WM_COMMAND, 100);
				}
				else
				{
					/*pWnd->Error = _T("数据读取失败，请稍后再试。");
					pWnd->PostMessage(WM_COMMAND, 101);*/
				}
			}
			else
			{
				/*pWnd->Error = _T("无法连接到服务器, 请检查网络。");
				pWnd->PostMessage(WM_COMMAND, 101);*/
			}
		}
	}
	catch (...)
	{
		pWnd->Error = _T("发生了异常，位于UpData的OnGetWebInfo方法。");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	pWnd->m_hUpDate = NULL;
	return true;
}


void CMainDlg::DeleteDirectory(CString Directory)   //删除一个文件夹下的所有内容  
{
	VMPBEGIN

	if(Directory.IsEmpty())   
	{ 
		RemoveDirectory(Directory); 
		return; 
	} 

	//首先删除文件及子文件夹 
	CFileFind   ff; 

	BOOL bFound = ff.FindFile(Directory + _T("\\*"),0); 
	while(bFound) 
	{ 
		bFound = ff.FindNextFile(); 
		if(ff.GetFileName()== _T(".")||ff.GetFileName()== _T("..")) 
			continue; 

		//去掉文件(夹)只读等属性 
		SetFileAttributes(ff.GetFilePath(),FILE_ATTRIBUTE_NORMAL); 

		if(ff.IsDirectory())  
		{   
			//递归删除子文件夹 
			DeleteDirectory(ff.GetFilePath()); 
			RemoveDirectory(ff.GetFilePath());
		} 
		else   
		{ 
			DeleteFile(ff.GetFilePath());   //删除文件 
		} 
	} 

	ff.Close(); 

	//然后删除该文件夹 
	RemoveDirectory(Directory);

	VMPEND
} 


void CMainDlg::Refresh()
{
	// 刷新窗口
	Invalidate();

	//设置高亮显示  
	m_List.SetFocus();//设置焦点  

	if(m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		m_List.SetItemState( m_List.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态
		m_List.EnsureVisible(m_List.GetItemCount() -1, FALSE);//设置当前视图可见
		m_List.EnsureVisible(m_List.GetNextItem(-1, LVIS_SELECTED), FALSE);//设置当前视图可见
	}
	else
	{
		m_List.EnsureVisible(m_List.GetItemCount() -1, FALSE);//设置当前视图可见
		m_List.EnsureVisible(0, FALSE);//设置当前视图可见
	}
}


void CMainDlg::OnHelp()
{
	// 显示帮助文档
	if (m_hOperate == NULL)
	{
		Type = 2;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}
}


void CMainDlg::Split(CString source, CString divKey, CStringArray &dest)
{
	dest.RemoveAll();
	int pos = 0;
	int pre_pos = 0;
	while ( -1 != pos )
	{
		pre_pos = pos;
		pos     = source.Find(divKey, (pos +1));

		CString temp(source.Mid(pre_pos , (pos -pre_pos )));
		temp.Replace(divKey, _T(""));
		dest.Add(temp);
	}
}


bool CMainDlg::Compress  (const char* scrfilename, const char* desfilename)
{
	FILE *fin, *fout;
	fopen_s(&fin, scrfilename, "rb");
	if(fin == NULL)
	{
		printf("Open ScrFile ERR:%s\n",scrfilename);
		return false;
	}
	fopen_s(&fout, desfilename, "wb");
	if(fout == NULL)
	{
		printf("Open DesFile ERR:%s\n",desfilename);
		fclose(fin);
		return false;
	}

	fseek(fin,0,SEEK_END);
	size_t saveinsize=ftell(fin);
	fseek(fin,0,SEEK_SET);
	size_t saveoutsize = saveinsize * (size_t)1.1 + 1026 * 16;
	unsigned char* inbuff=(unsigned char*)malloc(saveinsize);
	unsigned char* outbuff=(unsigned char*)malloc(saveoutsize);
	unsigned char props[5]={0};
	size_t propsSize=5;
	size_t readlength=fread(inbuff,1,saveinsize,fin);
	if(readlength!=saveinsize){
		printf("read err\n");
		fclose(fin);
		fclose(fout);
		return false;
	}
	int	res = LzmaCompress(outbuff,&saveoutsize,inbuff,saveinsize,
			props,&propsSize,
			5,
			1 << 24,
			3,
			0,
			2,
			32,
			1);
	if(res!=0){
		printf("LzmaCompressErr:%d\n",res);
		fclose(fin);
		fclose(fout);
		return true;
	}
	int zero=0;
	fwrite(props,1,propsSize,fout);
	fwrite(&saveinsize,1,4,fout);
	fwrite(&zero,1,4,fout);
	fwrite(outbuff,1,saveoutsize,fout);
	
	fclose(fin);
	fclose(fout);
	return true;
}


bool CMainDlg::Uncompress(const char* scrfilename, const char* desfilename)
{
	FILE *fin, *fout;
	fopen_s(&fin, scrfilename, "rb");
	if(fin == NULL)
	{
		printf("Open ScrFile ERR:%s\n",scrfilename);
		return false;
	}
	fopen_s(&fout, desfilename, "wb");
	if(fout == NULL)
	{
		printf("Open DesFile ERR:%s\n",desfilename);
		fclose(fin);
		return false;
	}

	fseek(fin,0,SEEK_END);
	size_t saveinsize = ftell(fin);
	fseek(fin,0,SEEK_SET);
	size_t saveoutsize = saveinsize * (size_t)1.1 + 1026 * 16;
	unsigned char* inbuff=(unsigned char*)malloc(saveinsize);
	unsigned char props[5]={0};
	size_t propsSize=5;

	fread(props,1,5,fin);
	fread(&saveoutsize,1,4,fin);
	fseek(fin,4,SEEK_CUR);
	unsigned char* outbuff=(unsigned char*)malloc(saveoutsize);
	size_t readlength=fread(inbuff,1,saveinsize-13,fin);
	if(readlength!=(saveinsize-13)){
		printf("read err\n");
		fclose(fin);
		fclose(fout);
		return false;
	}
	int	res=LzmaUncompress(outbuff,&saveoutsize,inbuff,&readlength,
			props,propsSize);
	if(res!=0){
		printf("LzmaUncompress:%d\n",res);
		fclose(fin);
		fclose(fout);
		return true;
	}
	fwrite(outbuff,1,saveoutsize,fout);
	fclose(fin);
	fclose(fout);
	return true;
}


void CMainDlg::Complete()
{
	CStringArray TextArray;
	Split(UpDateInfo, _T(";"), TextArray);

	CString Version  = TextArray.GetAt(0);
	CString FileInfo = TextArray.GetAt(1);
	CString TipText  = Version;

	Version.Replace(_T("v"), _T(""));
	Version.Replace(_T("."), _T(""));

	CString OldVersion = theApp.GetApplicationVersion();
	OldVersion.Replace(_T("v"), _T(""));
	OldVersion.Replace(_T("."), _T(""));

	// 比对
	int old_version = atoi(OldVersion);
	int new_version = atoi(Version);

	if(new_version  > old_version)
	{
		TipText.Format(_T("新版本 %s 已发布, 是否需要更新?"), TipText);
		if( MessageBox(TipText, _T("检测到更新"), MB_ICONQUESTION | MB_YESNO) == IDYES )
		{
			// 开始更新
			CStringArray TargetList;
			TargetList.Add(FileInfo);

			CTransmissionDlg pDlg;
			pDlg.TargetList = &TargetList;
			pDlg.IsDownload = true;
			pDlg.IsCode     = false;
			pDlg.IsProject  = false;
			pDlg.IsUpDate   = true;
			pDlg.DoModal();
		}
	}
}


void CMainDlg::OnError()
{
	// 提示错误
	AfxMessageBox(Error);
}


void CMainDlg::OnCheck()
{
	// 检测更新
	if(GetPrivateProfileInt(_T("Setting"), _T("Update"),1, _T("./Setting.ini")) == 1)
	{
		if(m_hUpDate == NULL)
			m_hUpDate = AfxBeginThread(UpDate, this);
	}
}


void CMainDlg::OnSaveCode()
{
	// 消息提示
	MessageBox(_T("方法已保存成功!"), _T("编程助理"), MB_ICONINFORMATION);

	//设置高亮显示  
	m_List.SetFocus();//设置焦点  
	m_List.SetItemState( m_List.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
	m_List.EnsureVisible(m_List.GetNextItem(-1, LVIS_SELECTED), FALSE);//设置当前视图可见 
}


void CMainDlg::OnNewCode()
{
	// 询问是否新建
	if( MessageBox(_T("没有选中的方法, 是否添加为新方法?"), _T("是否添加方法"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		// 新建方法
		if (m_hOperate == NULL)
		{
			this->Type = 10;
			m_hOperate = AfxBeginThread(Operate, this);
		}
	}
}


void CMainDlg::OnCopy()
{
	CString   source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();     
	//put   your   text   in   source   
	if(OpenClipboard())   
	{   
		HGLOBAL   clipbuffer;   
		char   *   buffer;   
		EmptyClipboard();   
		clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
		buffer   =   (char*)GlobalLock(clipbuffer);
		strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
		GlobalUnlock(clipbuffer);   
		SetClipboardData(CF_TEXT,clipbuffer);   
		CloseClipboard();   
	}
}


void CMainDlg::OnPaste()
{
	if( GetPrivateProfileInt(_T("Setting"), _T("Clear"), 0, _T("./Setting.ini")) == 1)
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.PasteSpecial(CF_TEXT);
	else
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.Paste();
}


void CMainDlg::OnCut()
{
	CString   source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();     
	//put   your   text   in   source   
	if(OpenClipboard())   
	{   
		HGLOBAL   clipbuffer;   
		char   *   buffer;   
		EmptyClipboard();   
		clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
		buffer   =   (char*)GlobalLock(clipbuffer);
		strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
		GlobalUnlock(clipbuffer);   
		SetClipboardData(CF_TEXT,clipbuffer);   
		CloseClipboard();   
	}

	// 剪切
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.Cut();
}


void CMainDlg::OnUndo()
{
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.Undo();
}


void CMainDlg::OnRedo()
{
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.Redo();
}


void CMainDlg::OnLeft()
{
	CString source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		PARAFORMAT pf;
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_LEFT; //PFA_CENTER or PFA_RIGHT
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf);
	}
	else
	{
		AfxMessageBox(_T("没有需要居左的内容!"));
	}
}


void CMainDlg::OnRight()
{
	CString source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		PARAFORMAT pf;
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_RIGHT; //PFA_CENTER or PFA_RIGHT
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf);
	}
	else
	{
		AfxMessageBox(_T("没有需要居右的内容!"));
	}
}


void CMainDlg::OnCenter()
{
	CString source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		PARAFORMAT pf;
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_CENTER; //PFA_CENTER or PFA_RIGHT
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf);
	}
	else
	{
		AfxMessageBox(_T("没有需要居中的内容!"));
	}
}


BOOL CMainDlg::GetPic(REOBJECT FAR* pObject)
{
	//CRichEditCtrl* pTextIn1 = (CRichEditCtrl*)GetDlgItem(IDC_CODE_RICHEDIT);
	//CRichEditCtrl* pTextIn1 = (CRichEditCtrl*)m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->GetDlgItem(IDC_CODE_RICHEDIT);
	CRichEditCtrl* pTextIn1 = (CRichEditCtrl*)m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->GetDlgItem(IDC_CODE_RICHEDIT);
	IRichEditOle* pITextIn1 = pTextIn1->GetIRichEditOle();
	LONG lCount=pITextIn1->GetObjectCount();

	LRESULT lRet;
	for(int i=0;i<lCount;i++)
	{
		lRet=pITextIn1->GetObject(0,pObject,REO_GETOBJ_POLEOBJ);
		if(lRet==S_OK)
		{
			pObject->poleobj->Release();
			if(pObject->poleobj == m_pPicObj)
			{
				return TRUE;
			}
		}

	}
	return FALSE;
}


void CMainDlg::SetPic()
{
	HBITMAP hBitmap = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), FilePath, IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_DEFAULTCOLOR);
	/*CImage image;
	image.Load(FilePath);
	HBITMAP hBitmap = image.Detach();*/

	STGMEDIUM stgm;
	stgm.tymed   = TYMED_GDI;
	stgm.hBitmap = hBitmap;
	stgm.pUnkForRelease = NULL;

	FORMATETC fm;
	fm.cfFormat = CF_BITMAP;
	fm.ptd      = NULL;
	fm.dwAspect = DVASPECT_CONTENT;
	fm.lindex   = -1;
	fm.tymed    = TYMED_GDI;

	IStorage *pStorage;
	LPLOCKBYTES lpLockBytes = NULL;
	SCODE sc=::CreateILockBytesOnHGlobal(NULL,TRUE,&lpLockBytes);
	if(sc!=S_OK) AfxThrowOleException(sc);
	ASSERT(lpLockBytes!=NULL);
	sc=::StgCreateDocfileOnILockBytes(lpLockBytes,STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE,0,&pStorage);
	if(sc!=S_OK)
	{
		VERIFY(lpLockBytes->Release()==0);
		lpLockBytes=NULL;
		AfxThrowOleException(sc);

	}
	ASSERT(pStorage != NULL);

	COleDataSource *pDataSource = new COleDataSource;
	pDataSource->CacheData(CF_BITMAP, &stgm);
	LPDATAOBJECT lpDataObject=(LPDATAOBJECT)pDataSource->GetInterface(&IID_IDataObject);

	LPOLECLIENTSITE  lpClientSite;

	//CRichEditCtrl* pTextIn1 = (CRichEditCtrl*)GetDlgItem(IDC_CODE_RICHEDIT);
	CRichEditCtrl* pTextIn1 = (CRichEditCtrl*)m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->GetDlgItem(IDC_CODE_RICHEDIT);

	pTextIn1->GetIRichEditOle()->GetClientSite(&lpClientSite);

	//创建OLE对象
	IOleObject *pOleObject;
	sc=OleCreateStaticFromData(lpDataObject,IID_IOleObject,OLERENDER_FORMAT,
		&fm,lpClientSite,pStorage,(void **)&pOleObject);
	if(sc!=S_OK) AfxThrowOleException(sc);

	//插入OLE对象
	REOBJECT reobject;
	ZeroMemory(&reobject,sizeof(REOBJECT));
	reobject.cbStruct=sizeof(REOBJECT);


	CLSID clsid; 
	sc = pOleObject->GetUserClassID(&clsid); 
	if (sc != S_OK) 
		AfxThrowOleException(sc);

	reobject.clsid = clsid; 
	reobject.cp = REO_CP_SELECTION; 
	reobject.dvaspect = DVASPECT_CONTENT; 
	reobject.poleobj = pOleObject; 
	reobject.polesite = lpClientSite; 
	reobject.pstg = pStorage; 
	reobject.dwUser = 0;

	REOBJECT obj;
	obj.cbStruct=sizeof(obj);
	if( GetPic(&obj) )
	{
		long lStart,lEnd;
		pTextIn1->GetSel(lStart,lEnd);
		pTextIn1->SetSel(obj.cp,obj.cp+1);
		pTextIn1->Clear();

		pTextIn1->GetIRichEditOle()->InsertObject(&reobject);
		pTextIn1->SetSel(lStart,lEnd);
	}
	else
	{
		pTextIn1->GetIRichEditOle()->InsertObject(&reobject);
	}

	// 释放对象
	if (hBitmap)
	{
		DeleteObject(hBitmap);
	}
	//image.Destroy();

	delete pDataSource;
	m_pPicObj = pOleObject;

	::SendMessage(pTextIn1->m_hWnd,EM_SCROLLCARET,(WPARAM)0,(LPARAM)0);
	pTextIn1->SetFocus();
}


void CMainDlg::OnImage()
{
	// 插入图片
	if (m_hOperate == NULL)
	{
		Type = 12;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}
}


void  CMainDlg::OnScreenCapture()
{
	// 插入图片
	if (m_hOperate == NULL)
	{
		Type = 14;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	// 延迟
	//Sleep(300);

	//CDC* pScreenDc = CDC::FromHandle(::GetDC(NULL));//屏幕DC
	//CDC memDc;// 内存DC
	//CBitmap memBmp;
	//CBitmap* pOldBmp;
	//int cx = GetSystemMetrics(SM_CXSCREEN);
	//int cy = GetSystemMetrics(SM_CYSCREEN);
	//memDc.CreateCompatibleDC(pScreenDc);
	//memBmp.CreateCompatibleBitmap(pScreenDc, cx, cy);
	//pOldBmp = memDc.SelectObject(&memBmp);
	//memDc.BitBlt(0, 0, cx, cy, pScreenDc, 0, 0, SRCCOPY);

	////复制到剪切板
	//OpenClipboard();
	//EmptyClipboard();
	//SetClipboardData(CF_BITMAP, memBmp.Detach());
	//CloseClipboard();

	//// 显示到当前界面
	///*CClientDC clientDc(this);
	//CRect rcClient;
	//GetClientRect(rcClient);
	//clientDc.StretchBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDc, 0, 0, cx, cy, SRCCOPY);*/

	//memDc.SelectObject(pOldBmp);
	//memDc.DeleteDC();
	//memBmp.DeleteObject();

	//// 粘贴
	//m_Edit.Paste();

	//m_Screen->CenterWindow();
	//m_Screen->ShowWindow(SW_SHOW);
}


int CMainDlg::CountFile(CString DirPath)
{
	int count = 0;
	CFileFind finder;
	BOOL working = finder.FindFile(DirPath + "./*.*");

	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
			count += CountFile(finder.GetFilePath());
		else
		{
			count++;
		}
	}
	return count;
}


void CMainDlg::OnOpen()
{
	// 打开方法
	if (m_hOperate == NULL)
	{
		Type = 13;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}
}


void CMainDlg::OnParagraph()
{
	CParagraphDlg dlg;

	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);

	//dlg.m_Left.Format(_T("%d"),    pf.dxOffset);
	//dlg.m_Tab.Format(_T("%d"),     pf.cTabCount);

	dlg.m_Index.Format(_T("%d"),   pf.dxStartIndent);
	dlg.m_Right.Format(_T("%d"),   pf.dxRightIndent);
	dlg.m_Spacing.Format(_T("%d"), pf.dyLineSpacing);
	dlg.m_Part.Format(_T("%d"),    pf.dySpaceAfter);
	dlg.m_Tab.Format(_T("%d"),     pf.dxOffset);

	// 对齐方式
	if(pf.wAlignment == PFA_LEFT)
		dlg.Alignment = 0;
	else if(pf.wAlignment == PFA_RIGHT)
		dlg.Alignment = 1;
	else
		dlg.Alignment = 2;

	// 列表样式
	if(pf.wNumbering == PFN_BULLET)
		dlg.Tab = 1;
	else if(pf.wNumbering == PFN_ARABIC)
		dlg.Tab = 2;
	else if(pf.wNumbering == PFN_LCLETTER)
		dlg.Tab = 3;
	else if(pf.wNumbering == PFN_UCLETTER)
		dlg.Tab = 4;
	else if(pf.wNumbering == PFN_LCROMAN)
		dlg.Tab = 5;
	else if(pf.wNumbering == PFN_UCROMAN)
		dlg.Tab = 6;
	else
		dlg.Tab = 0;

	if(dlg.DoModal() == IDOK)
	{
		pf.cbSize = sizeof(PARAFORMAT2); // 缩进 首行缩进           段间距           行间距            // 右间距        // 对齐方式     // 列表样式
		pf.dwMask = PFM_NUMBERING | PFM_OFFSET | PFM_STARTINDENT | PFM_SPACEAFTER | PFM_LINESPACING | PFM_RIGHTINDENT | PFM_ALIGNMENT | PFM_TABSTOPS;
		pf.dxStartIndent = atol(dlg.m_Index);
		pf.dxRightIndent = atol(dlg.m_Right);
		pf.dyLineSpacing = atol(dlg.m_Spacing);
		pf.dySpaceAfter  = atol(dlg.m_Part);
		//pf.cTabCount     = atoi(dlg.m_Tab);
		pf.dxOffset      = atoi(dlg.m_Tab);
		
		// 对齐方式
		if(dlg.Alignment == 0)
			pf.wAlignment = PFA_LEFT;
		else if(dlg.Alignment == 1)
			pf.wAlignment = PFA_RIGHT;
		else
			pf.wAlignment = PFA_CENTER;

		// 列表样式
		if(dlg.Tab == 0)
			pf.wNumbering = NULL;
		else if(dlg.Tab == 1)
			pf.wNumbering = PFN_BULLET;
		else if(dlg.Tab == 2)
			pf.wNumbering = PFN_ARABIC;
		else if(dlg.Tab == 3)
			pf.wNumbering = PFN_LCLETTER;
		else if(dlg.Tab == 4)
			pf.wNumbering = PFN_UCLETTER;
		else if(dlg.Tab == 5)
			pf.wNumbering = PFN_LCROMAN;
		else if(dlg.Tab == 6)
			pf.wNumbering = PFN_UCROMAN;

		VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
	}
}


void CMainDlg::OnNone()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // 列表样式
	pf.wNumbering = NULL;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnSymbol()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // 列表样式
	pf.wNumbering = PFN_BULLET;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnNumber()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // 列表样式
	pf.wNumbering = PFN_ARABIC;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnLowerCase()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // 列表样式
	pf.wNumbering = PFN_LCLETTER;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnUpperCase()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // 列表样式
	pf.wNumbering = PFN_UCLETTER;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnLowerRome()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // 列表样式
	pf.wNumbering = PFN_LCROMAN;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnUpperRome()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // 列表样式
	pf.wNumbering = PFN_UCROMAN;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnNewLabel()
{
	// 新建标签
	m_pDocument[TabCount] = new CDocumentDlg();
	m_Tab.CreateTabPage(m_pDocument[TabCount], IDD_DOCUMENT_DIALOG, FileName);
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileClass = FileClass;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileType  = FileType;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName  = FileName;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FilePath  = FilePath;
	TabCount++;
}


void CMainDlg::OnSetLabel()
{
	if(!m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsOutFunction)
	{
		CString CurClass, CurType, TargetClass, TargetType;
		m_Class.GetWindowText(CurClass);
		m_Type.GetWindowText (CurType);

		TargetClass = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileClass;
		TargetType  = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileType;

		if(!TargetClass.IsEmpty() && TargetClass != CurClass)
		{
			int nIndex = m_Class.FindStringExact(0, TargetClass);
			if(nIndex != CB_ERR)
			{
				m_Class.SetCurSel(nIndex);

				// 清空ComboBox
				m_Type.ResetContent();

				CString FilePath = _T("Code\\") + TargetClass;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						m_Type.AddString(Finder.GetFileName());
				}

				// 设置ComboBox
				m_Type.SetCurSel(0);
			}
		}

		if(!TargetType.IsEmpty() && TargetType != CurType)
		{
			int nIndex = m_Type.FindStringExact(0, TargetType);
			if(nIndex != CB_ERR)
			{
				m_Type.SetCurSel(nIndex);

				// 清空列表
				m_List.DeleteAllItems();

				CString FilePath = _T("Code\\") + TargetClass + _T("\\") + TargetType;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						continue;
					else
					{
						CString Name = Finder.GetFileName();
						m_List.AddItem(Name.Left(Name.GetLength() - 5));
					}
				}
			}
		}

		// 取消选择
		m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

		// 找到对应的方法
		int Count = m_List.GetItemCount();
		for(int i=0; i<Count; i++)
		{
			CString Function = m_List.GetItemText(i, 0);

			if(Function == m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName)
			{
				//设置高亮显示  
				m_List.SetFocus();//设置焦点  
				m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
				m_List.EnsureVisible(i, FALSE);//设置当前视图可见
				break;
			}
		}

		// 焦点转移
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetFocus();

		// 更改按钮文本
		CString Text;
		GetDlgItem(IDC_NEW_BUTTON)->GetWindowText(Text);

		CString Title = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName;
		if(Title != _T("新方法"))
		{
			if(Text != _T("编辑方法"))
			{
				// 更改按钮文本
				GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("编辑方法"));

				// 编辑标记
				IsNew = FALSE;
			}
		}
		else if(!Title.IsEmpty())
		{
			// 更改按钮文本
			GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("添加方法"));

			// 添加标记
			IsNew  = TRUE;
		}

		// 设置焦点
		m_List.SetFocus();
	}
	else
	{
		// 取消选择
		m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

		// 更改按钮文本
		CString Text;
		GetDlgItem(IDC_NEW_BUTTON)->GetWindowText(Text);

		if(Text != _T("编辑方法"))
		{
			// 更改按钮文本
			GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("编辑方法"));

			// 编辑标记
			IsNew = FALSE;
		}
    }
}


void CMainDlg::OnClickCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	VMPEND
}


void CMainDlg::OnRClickCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	// 取消选择
	m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);
	CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

	// 搜索目标
	CSearchDlg dlg;
	dlg.FilePath = FilePath;
	if(dlg.DoModal() == IDOK)
	{
		CString Function = dlg.Function;

		int Count = m_List.GetItemCount();
		for(int i=0; i<Count; i++)
		{
			CString Name = m_List.GetItemText(i, 0);

			if(Name == Function)
			{
				//设置高亮显示  
				m_List.SetFocus();//设置焦点  
				m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
				m_List.EnsureVisible(i, FALSE);//设置当前视图可见 
				break;
			}
		}
	}

	*pResult = 0;

	VMPEND
}


void CMainDlg::OnDblclkCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// 使用写字板打开
	if (m_hOperate == NULL)
	{
		Type = 8;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND

	*pResult = 0;
}


void CMainDlg::OnItemchangedCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN
		// 读取目标文件
		if (m_hOperate == NULL)
		{
			Type = 1;
			m_hOperate = AfxBeginThread(Operate, this);
			CloseHandle(m_hOperate->m_hThread);
		}
	VMPEND
}


void CMainDlg::OnSave()
{
	VMPBEGIN

	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	if(Class.IsEmpty())
	{
		AfxMessageBox(_T("请输入类型!"));
		return;
	}
	else if(Type.IsEmpty())
	{
		AfxMessageBox(_T("请输入类别!"));
		return;
	}

	DWORD Path = GetFileAttributes(_T("Code\\") + Class);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		CreateDirectory(_T("Code\\") + Class, NULL);
	}

	Path = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
	}

	MessageBox(_T("成功创建分类"), _T("分类创建成功"), MB_ICONINFORMATION | MB_OK);

	VMPEND
}


void CMainDlg::OnDelete()
{
	VMPBEGIN

	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	if(Class.IsEmpty())
	{
		AfxMessageBox(_T("请输入类型!"));
		return;
	}
	else if(!Class.IsEmpty() && Type.IsEmpty())
	{
		// 删除目录
		DeleteDirectory(_T("Code\\") + Class);
		DeleteDirectory(_T("File\\") + Class);

		// 删除ComboBox 里的值
		m_Class.DeleteString(m_Class.GetCurSel());
		m_Class.SetCurSel(0);

		// 内容变更消息

		// 清空编辑框
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetWindowText(_T(""));

		// 清空ComboBox
		m_Type.ResetContent();

		CString Class;
		m_Class.GetWindowText(Class);
		CString FilePath = _T("Code\\") + Class;

		CFileFind Finder;
		BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
		while (IsFind)
		{
			IsFind = Finder.FindNextFile();

			if (Finder.IsDots())
				continue;
			if (Finder.IsDirectory())
				m_Type.AddString(Finder.GetFileName());
		}

		// 设置ComboBox
		m_Type.SetCurSel(0);


		// 清空列表
		m_List.DeleteAllItems();

		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);
		FilePath = _T("Code\\") + Class + _T("\\") + Type;

		IsFind = Finder.FindFile(FilePath + _T("./*.*"));
		while (IsFind)
		{
			IsFind = Finder.FindNextFile();

			if (Finder.IsDots())
				continue;
			if (Finder.IsDirectory())
				continue;
			else
			{
				CString Name = Finder.GetFileName();
				m_List.AddItem(Name.Left(Name.GetLength() - 5));
			}
		}

		// 消息提示
		MessageBox(_T("删除完毕!"), _T("分类删除成功"), MB_ICONINFORMATION | MB_OK);
	}
	else if(!Class.IsEmpty() && !Type.IsEmpty())
	{
		int code = MessageBox(_T("是否删除类型? 选否只删除该类别。"), _T("选择操作类型"), MB_YESNOCANCEL);

		if(code != IDCANCEL)
		{
			if(code == IDYES)
			{
				// 删除目录
				DeleteDirectory(_T("Code\\") + Class);
				DeleteDirectory(_T("File\\") + Class);

				// 删除ComboBox 里的值
				m_Class.DeleteString(m_Class.GetCurSel());
				m_Type.DeleteString(m_Type.GetCurSel());

				// 设置选项
				m_Class.SetCurSel(0);

				// 清空编辑框
				m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetWindowText(_T(""));

				// 清空ComboBox
				m_Type.ResetContent();

				CString Class;
				m_Class.GetWindowText(Class);
				CString FilePath = _T("Code\\") + Class;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						m_Type.AddString(Finder.GetFileName());
				}

				// 设置ComboBox
				m_Type.SetCurSel(0);


				// 清空列表
				m_List.DeleteAllItems();

				m_Class.GetWindowText(Class);
				m_Type.GetWindowText(Type);
				FilePath = _T("Code\\") + Class + _T("\\") + Type;

				IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						continue;
					else
					{
						CString Name = Finder.GetFileName();
						m_List.AddItem(Name.Left(Name.GetLength() - 5));
					}
				}

				// 消息提示
				MessageBox(_T("删除完毕!"), _T("分类删除成功"), MB_ICONINFORMATION | MB_OK);
			}
			else
			{
				// 删除目录
				DeleteDirectory(_T("Code\\") + Class + _T("\\") + Type);
				DeleteDirectory(_T("File\\") + Class + _T("\\") + Type);

				// 删除ComboBox 里的值
				m_Type.DeleteString(m_Type.GetCurSel());

				// 设置选项
				m_Type.SetCurSel(0);

				// 内容变更消息

				// 清空列表
				m_List.DeleteAllItems();

				m_Class.GetWindowText(Class);
				m_Type.GetWindowText(Type);
				CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						continue;
					else
					{
						CString Name = Finder.GetFileName();
						m_List.AddItem(Name.Left(Name.GetLength() - 5));
					}
				}

				// 清空编辑框
				m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetWindowText(_T(""));

				// 消息提示
				MessageBox(_T("删除完毕!"), _T("分类删除成功"), MB_ICONINFORMATION | MB_OK);
			}
		}
	}

	VMPEND
}


void CMainDlg::OnEdit()
{
	CString Class,Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	CEditDlg dlg;
	dlg.Class = Class;
	dlg.Type  = Type;
	dlg.IsFunction = false;

	if(dlg.DoModal() == IDOK)
	{
		// 如果只修改了类别
		if(dlg.Class == Class && dlg.Type != Type)
		{
			CString old_Path = _T("Code\\") + Class + _T("\\") + Type;
			CString new_Path = _T("Code\\") + Class + _T("\\") + dlg.Type;

			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				m_Type.SetWindowText(dlg.Type);
				MessageBox(_T("分类修改成功!"), _T("修改成功"), MB_ICONINFORMATION | MB_OK);
			}
			else
			{
				AfxMessageBox(_T("分类修改失败!"));
			}
		}

		// 如果只修改了类型
		else if(dlg.Class != Class && dlg.Type == Type)
		{
			CString old_Path = _T("Code\\") + Class;
			CString new_Path = _T("Code\\") + dlg.Class;

			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				m_Class.SetWindowText(dlg.Class);
				MessageBox(_T("分类修改成功!"), _T("修改成功"), MB_ICONINFORMATION | MB_OK);
			}
			else
			{
				AfxMessageBox(_T("分类修改失败!"));
			}
		}

		// 两种都改了
		else
		{
			CString old_Class = _T("Code\\") + Class;
			CString new_Class = _T("Code\\") + dlg.Class;

			CString old_Type = _T("Code\\") + dlg.Class + _T("\\") + Type;
			CString new_Type = _T("Code\\") + dlg.Class + _T("\\") + dlg.Type;

			if(MoveFileEx(old_Class, new_Class, MOVEFILE_REPLACE_EXISTING) && MoveFileEx(old_Type, new_Type, MOVEFILE_REPLACE_EXISTING))
			{
				m_Class.SetWindowText(dlg.Class);
				m_Type.SetWindowText(dlg.Type);
				MessageBox(_T("分类修改成功!"), _T("修改成功"), MB_ICONINFORMATION | MB_OK);
			}
			else
			{
				AfxMessageBox(_T("分类修改失败!"));
			}
		}
	}
}


void CMainDlg::OnManager()
{
	DWORD Path = GetFileAttributes(_T("Project"));
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		CreateDirectory(_T("Project"), NULL);
	}

	CProjectDlg dlg;
	dlg.DoModal();
}


void CMainDlg::OnKillfocusClassCombo()
{
	VMPBEGIN

	// 添加目标
	if (m_hOperate == NULL)
	{
		Type = 3;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND
}


void CMainDlg::OnDropdownClassCombo()
{
	VMPBEGIN

	// 添加目标
	if (m_hOperate == NULL)
	{
		Type = 4;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND
}


void CMainDlg::OnKillfocusTypeCombo()
{
	VMPBEGIN

	// 添加目标
	if (m_hOperate == NULL)
	{
		Type = 5;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND
}


void CMainDlg::OnDropdownTypeCombo()
{
	VMPBEGIN

	// 添加目标
	if (m_hOperate == NULL)
	{
		Type = 6;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND
}


void CMainDlg::OnEditFunction()
{
	// 判断外部方法
	if(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsOutFunction && !m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FilePath.IsEmpty())
	{
		CEditDlg dlg;
		dlg.Type  = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName;
		dlg.IsFunction = true;

		if(dlg.DoModal() == IDOK)
		{
			CString old_Path = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FilePath;
			CString new_Path = old_Path.Left(old_Path.GetLength() - m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName.GetLength() -5) + dlg.Type + _T(".code");

			// 重命名
			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				// 修改标签属性
				m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName   = dlg.Type;
				m_Tab.SetPageTitle(m_Tab.m_selTabID, (TCHAR*)(LPCTSTR)dlg.Type);

				// 刷新
				m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->Invalidate();

				// 提示消息
				MessageBox(_T("方法修改成功!"), _T("修改成功"), MB_ICONINFORMATION | MB_OK);
			}
			else
			{
				AfxMessageBox(_T("方法修改失败!"));
			}
		}
	}
	else
	{
		CString Class = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileClass, 
			Type  = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileType, 
			Function = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName;

		// 判断选中方法
		if (!Function.IsEmpty() && !Class.IsEmpty() && !Type.IsEmpty())
		{
			CEditDlg dlg;
			dlg.Type  = Function;
			dlg.IsFunction = true;

			if(dlg.DoModal() == IDOK)
			{
				CString old_Path = _T("Code\\") + Class + _T("\\") + Type + _T("\\") + Function + _T(".code");
				CString new_Path = _T("Code\\") + Class + _T("\\") + Type + _T("\\") + dlg.Type + _T(".code");

				if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
				{
					// 修改列表
					int Count = m_List.GetItemCount();
					for(int i=0; i<Count; i++)
					{
						CString Name = m_List.GetItemText(i, 0);
						if(Function == Name)
						{
							m_List.SetItemText(i, 0, dlg.Type);

							// 修改标签属性
							m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName   = dlg.Type;
							m_Tab.SetPageTitle(m_Tab.m_selTabID, (TCHAR*)(LPCTSTR)dlg.Type);

							// 提示消息
							MessageBox(_T("方法修改成功!"), _T("修改成功"), MB_ICONINFORMATION | MB_OK);

							// 取消选择
							m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

							//设置高亮显示
							m_List.SetFocus();//设置焦点
							m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态
							m_List.EnsureVisible(i, FALSE);//设置当前视图可见
							break;
						}
					}
				}
				else
				{
					AfxMessageBox(_T("方法修改失败!"));
				}
			}
		}
		else
		{
			AfxMessageBox(_T("新方法无需编辑, 请保存方法!"));
		}
	}
}


void CMainDlg::OnOK()
{
	VMPBEGIN

	// 保存方法
	if (m_hOperate == NULL)
	{
		Type = 9;
		m_hOperate = AfxBeginThread(Operate, this);
	}

	VMPEND
}


void CMainDlg::OnFont()
{
	VMPBEGIN

	CHARFORMAT cf;
	LOGFONT lf;
	memset(&cf, 0, sizeof(CHARFORMAT));
	memset(&lf, 0, sizeof(LOGFONT)); 

	//判断是否选择了内容
	BOOL bSelect = (m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelectionType() != SEL_EMPTY) ? TRUE : FALSE; 
	if (bSelect)
	{
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelectionCharFormat(cf);
	}
	else
	{
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetDefaultCharFormat(cf);
	}

	//得到相关字体属性
	BOOL bIsBold = cf.dwEffects & CFE_BOLD;
	BOOL bIsItalic = cf.dwEffects & CFE_ITALIC;
	BOOL bIsUnderline = cf.dwEffects & CFE_UNDERLINE;
	BOOL bIsStrickout = cf.dwEffects & CFE_STRIKEOUT;

	//设置属性
	lf.lfCharSet = cf.bCharSet;
	lf.lfHeight = cf.yHeight/15;
	lf.lfPitchAndFamily = cf.bPitchAndFamily;
	lf.lfItalic = bIsItalic;
	lf.lfWeight = (bIsBold ? FW_BOLD : FW_NORMAL);
	lf.lfUnderline = bIsUnderline;
	lf.lfStrikeOut = bIsStrickout;

	//sprintf_s(lf.lfFaceName, 65535, cf.szFaceName);
	/*asci：strcpy(m_NotifyData.szTip, "认证系统客户端");
	unicode：wcscpy(m_NotifyData.szTip, L"认证系统客户端");*/

	//wcscpy_s(lf.lfFaceName, cf.szFaceName);
	strcpy_s(lf.lfFaceName, cf.szFaceName);

	CFontDialog dlg(&lf);
	dlg.m_cf.rgbColors = cf.crTextColor;

	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCharFormat(cf);//获得所选字体的属性
		if (bSelect)
			m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetSelectionCharFormat(cf);    //为选定的内容设定所选字体
		else
			m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetWordCharFormat(cf);         //为将要输入的内容设定字体
	}

	VMPEND
}


void CMainDlg::OnNew()
{
	// 创建标签
	if(IsNew)
	{
		// 取消选择
		//m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

		// 新建标签
		m_pDocument[TabCount] = new CDocumentDlg();
		m_Tab.CreateTabPage(m_pDocument[TabCount], IDD_DOCUMENT_DIALOG, _T("新方法"));
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName = _T("新方法");
		TabCount++;
	}
	else
	{
		// 编辑方法
		OnEditFunction();
	}
}


void CMainDlg::OnClearFormat()
{
	CString source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		//CHARFORMAT cf;  
		//m_Edit.GetSelectionCharFormat(cf);  
		//cf.dwMask|=CFM_BOLD;  
		//cf.dwEffects|=CFE_BOLD;//设置粗体，取消用cf.dwEffects&=~CFE_BOLD;  
		//cf.dwMask|=CFM_ITALIC;  
		//cf.dwEffects|=CFE_ITALIC;//设置斜体，取消用cf.dwEffects&=~CFE_ITALIC;  
		//cf.dwMask|=CFM_UNDERLINE;  
		//cf.dwEffects|=CFE_UNDERLINE;//设置斜体，取消用cf.dwEffects&=~CFE_UNDERLINE;  
		//cf.dwMask|=CFM_COLOR;  
		//cf.crTextColor = RGB(0,0,0);//设置颜色  
		//cf.dwMask|=CFM_SIZE;  
		//cf.yHeight =200;//设置高度  
		//cf.dwMask|=CFM_FACE;  
		//strcpy_s(cf.szFaceName ,_T("宋体"));//设置字体  
		//m_Edit.SetSelectionCharFormat(cf);  

		CHARFORMAT cf;
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelectionCharFormat(cf);
		cf.dwMask|=CFM_BOLD;
		cf.dwEffects&=~CFE_BOLD;      //设置粗体，取消用cf.dwEffects&=~CFE_BOLD;
		cf.dwMask|=CFM_ITALIC;
		cf.dwEffects&=~CFE_ITALIC;    //设置斜体，取消用cf.dwEffects&=~CFE_ITALIC;
		cf.dwMask|=CFM_UNDERLINE;
		cf.dwEffects&=~CFE_UNDERLINE; //设置斜体，取消用cf.dwEffects&=~CFE_UNDERLINE;
		cf.dwMask|=CFM_COLOR;
		cf.crTextColor = RGB(0,0,0);  //设置颜色
		cf.dwMask|=CFM_SIZE;
		cf.yHeight = 14 * 14;         //设置高度
		cf.dwMask|=CFM_FACE;
		strcpy_s(cf.szFaceName ,_T("宋体"));//设置字体
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetSelectionCharFormat(cf);
	}
	else
	{
		AfxMessageBox(_T("没有需要清除格式的内容!"));
	}
}


void CMainDlg::OnDirectory()
{
	VMPBEGIN

	if(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsOutFunction)
	{
		AfxMessageBox(_T("外部方法不存在依赖文件!"));
		return;
	}

	CString Class = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileClass, Type = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileType, Dir;

	if(Class.IsEmpty() || Type.IsEmpty())
	{
		AfxMessageBox(_T("新方法不存在依赖文件, 请先保存方法!"));
		return;
	}

	CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
	if (!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		DWORD Path = GetFileAttributes(_T("File\\") + Class);
		if (Path == 0xFFFFFFFF)     //文件夹不存在
		{
			CreateDirectory(_T("File\\") + Class, NULL);
		}

		Path = GetFileAttributes(_T("File\\") + Class + _T("\\") + Type);
		if (Path == 0xFFFFFFFF)     //文件夹不存在
		{
			CreateDirectory(_T("File\\") + Class + _T("\\") + Type, NULL);
		}

		Dir  = _T("File\\") + Class + _T("\\") + Type + _T("\\") + Name;
		Path = GetFileAttributes(Dir);

		if (Path == 0xFFFFFFFF)     //文件夹不存在
		{
			CreateDirectory(_T("File\\") + Class + _T("\\") + Type + _T("\\") + Name, NULL);
		}

		ShellExecute(NULL, _T("open"), Dir, NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		AfxMessageBox(_T("请选择方法后再执行本操作!"));
	}

	VMPEND
}


void CMainDlg::OnRemove()
{
	VMPBEGIN

	// 判断外部方法
	if(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsOutFunction)
	{
		if(MessageBox(_T("是否要移除当前方法? "),  _T("方法移除确认"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			// 移除方法
			DeleteFile(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FilePath);

			// 关闭标签
			m_Tab.OnRButtonDown(NULL, NULL);
		}
	}
	else
	{
		int Count = m_List.GetSelectedCount();
		CString Path = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);

		if (!Path.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
		{
			if(Count == 1)
			{
				if (!Path.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
				{
					if(MessageBox(_T("是否要移除当前方法? "),  _T("方法移除确认"), MB_ICONQUESTION | MB_YESNO) == IDYES)
					{
						// 移除方法
						if (m_hOperate == NULL)
						{
							Type = 11;
							m_hOperate = AfxBeginThread(Operate, this);
						}
						//CString Class, Type;
						//m_Class.GetWindowText(Class);
						//m_Type.GetWindowText(Type);

						//// 删除文件
						//DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".code"));

						//// 移除列表
						//m_List.DeleteItem(m_List.GetNextItem(-1, LVIS_SELECTED));

						//// 清空编辑框
						//m_Edit.SetWindowText(_T(""));

						//// 删除依赖
						//DeleteDirectory(_T("File\\") + Class + _T("\\") + Type + _T("\\") + Path);
					}
				}
				else
				{
					AfxMessageBox(_T("请选择要移除的方法!"));
				}
			}
			else if(Count > 1)
			{
				if(MessageBox(_T("是否要移除选中的方法? "), _T("方法移除确认"), MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					// 移除方法
					if (m_hOperate == NULL)
					{
						Type = 11;
						m_hOperate = AfxBeginThread(Operate, this);
					}
					//int nItem = -1;
					//POSITION pos;

					//while (pos = m_List.GetFirstSelectedItemPosition())
					//{
					//	nItem = -1;
					//	nItem = m_List.GetNextSelectedItem(pos);
					//	if (nItem >= 0 && m_List.GetSelectedCount() > 0)
					//	{
					//		CString Path = m_List.GetItemText(nItem, 0);

					//		CString Class, Type;
					//		m_Class.GetWindowText(Class);
					//		m_Type.GetWindowText(Type);

					//		// 删除文件
					//		DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".code"));

					//		// 移除列表
					//		m_List.DeleteItem(nItem);

					//		// 删除依赖
					//		DeleteDirectory(_T("File\\") + Class + _T("\\") + Type + _T("\\") + Path);
					//	}
					//}

					//for (int i = 0; i < Count; i++)
					//{
					//	int Row = m_List.GetNextItem(i - 1, LVIS_SELECTED);
					//	CString Path = m_List.GetItemText(Row, 0);

					//	CString Class, Type;
					//	m_Class.GetWindowText(Class);
					//	m_Type.GetWindowText(Type);

					//	DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".rtf"));
					//	//m_List.DeleteItem(m_List.GetNextItem(-1, LVIS_SELECTED));
					//}
				}
			}
			else
			{
				AfxMessageBox(_T("请选择要移除的方法!"));
			}
		}
		else
		{
			AfxMessageBox(_T("请选择要移除的方法!"));
		}
	}

	VMPEND
}


void CMainDlg::OnSynchronization()
{
	CLoginDlg dlg;
	dlg.DoModal();
}


void CMainDlg::OnSetting()
{
	if(!m_Setting->IsWindowVisible())
	{
		m_Setting->CenterWindow();
		m_Setting->SetForegroundWindow();
		m_Setting->OnReadSetting();
		m_Setting->ShowWindow(SW_SHOW);
	}
	else
	{
		m_Setting->ShowWindow(SW_HIDE);
	}
}


void CMainDlg::OnCancel()
{
	// 结束计数器
	KillTimer(1);

	// 工作线程处理
	DWORD Code = 0;
	if(GetExitCodeThread(m_hOperate, &Code))
	{
		if(Code == STILL_ACTIVE)
		{
			TerminateThread(m_hOperate, 0);
			CloseHandle(m_hOperate);
		}
	}
	if(GetExitCodeThread(m_hUpDate, &Code))
	{
		if(Code == STILL_ACTIVE)
		{
			TerminateThread(m_hUpDate, 0);
			CloseHandle(m_hUpDate);
		}
	}

	// 是否自动打开
	if( GetPrivateProfileInt(_T("Setting"), _T("Open"), 0, _T("./Setting.ini")) == 1)
	{
		CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0), Class, Type;
		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);

		// 判断是否选择了源码
		if(!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
		{
			// 保存配置文件
			::WritePrivateProfileString(_T("File"), _T("Class"), Class, _T("./Setting.ini"));
			::WritePrivateProfileString(_T("File"), _T("Type"),  Type,  _T("./Setting.ini"));
			::WritePrivateProfileString(_T("File"), _T("Name"),  Name,  _T("./Setting.ini"));
		}
	}

	// 是否自动同步
	if( GetPrivateProfileInt(_T("Setting"), _T("Synchronize"), 0, _T("./Setting.ini")) == 1)
	{
		CString UserName, Password;
		if (!theApp.m_Sql.SelectData(_T("用户账户"), UserName, 1, _T("Name Is Not Null")))
		{
			AfxMessageBox(_T("无法读取用户名!自动同步失败!"));
		}

		if (!theApp.m_Sql.SelectData(_T("用户账户"), Password, 2, _T("Password Is Not Null")))
		{
			AfxMessageBox(_T("无法读取用户密码!自动同步失败!"));
		}

		if(UserName.IsEmpty() || Password.IsEmpty())
		{
			AfxMessageBox(_T("没有进行过自动登陆!自动同步失败!"));
			CDialogEx::OnCancel();
			return;
		}

		try
		{
			CString Parameter;
			Parameter.Format(_T("username=%s&password=%s"), UserName, Password);
			BOOL IsSuccess = FALSE;

			CString RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/login"), 80, Parameter, IsSuccess);
			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				AfxMessageBox(_T("无法连接到服务器, 请检查网络。自动同步失败!"));
				return;
			}
			else
			{
				if (IsSuccess)
				{
					if( RecvData == RecvData.SpanIncluding( _T("0123456789") ) )
					{
						CSynchronizeDlg * dlg = new CSynchronizeDlg;
						dlg->UserId   = RecvData;
						dlg->UserName = UserName;
						dlg->IsAutoSynchronize=true;

						dlg->Create(IDD_SYNCHRONIZE_DIALOG, this);
						dlg->OnAutoSynchronize();

						dlg->DestroyWindow();
						delete dlg;
					}
					else
					{
						AfxMessageBox(_T("登录失败，用户名或密码错误。自动同步失败!"));
					}
				}
				else
				{
					AfxMessageBox(_T("无法连接到服务器, 请检查网络。自动同步失败!"));
				}
			}
		}
		catch (...)
		{
			AfxMessageBox(_T("发生了异常，位于CMainDlg的OnCancel方法。"));
		}
	}

	// 是否保存窗口位置
	if( GetPrivateProfileInt(_T("Setting"), _T("Position"), 0, _T("./Setting.ini")) == 1)
	{
		// 得到窗口位置
		CRect rc;
		CRect rect;
		GetWindowRect(&rect);  //窗体大小

		CString Position;
		Position.Format("%d,%d,%d,%d,",rect.left,rect.top,rc.Width(),rc.Height());

		// 保存配置文件
		::WritePrivateProfileString(_T("Position"), _T("WindowPosition"), Position, _T("./Setting.ini"));
	}

	CDialogEx::OnCancel();
}
