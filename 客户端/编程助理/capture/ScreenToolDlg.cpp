// ScreenToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../编程助理.h"
#include "ScreenToolDlg.h"
#include "PickColorDlg.h"
#include "MagnifierDlg.h"
#include "SnapWindowDlg.h"
#include "common.h"
#include <atlimage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CScreenToolDlg 对话框
CScreenToolDlg::CScreenToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	IsExit  = TRUE;
}

CScreenToolDlg::~CScreenToolDlg()
{
	//删除热键
	UnregisterHotKey(m_hWnd, IDHOTKEYCFS);
	UnregisterHotKey(m_hWnd, IDHOTKEYCWS);
	UnregisterHotKey(m_hWnd, IDHOTKEYCRS);
	UnregisterHotKey(m_hWnd, IDHOTKEYCFWS);
	UnregisterHotKey(m_hWnd, IDHOTKEYSC);
	UnregisterHotKey(m_hWnd, IDHOTKEYSM);
}

void CScreenToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NEWCUT_BTN, m_imageCutBtn);
	DDX_Control(pDX, IDC_PICKCOLOR_BTN, m_imageColorBtn);
	DDX_Control(pDX, IDC_MAGNIFIER_BTN, m_imageMagnifierBtn);
	DDX_Control(pDX, IDC_DROPCUT_BTN, m_imageDropCutBtn);
}

BEGIN_MESSAGE_MAP(CScreenToolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_DROPCUT_BTN, &CScreenToolDlg::OnBnClickedDropcutBtn)

	ON_COMMAND(IDM_RECTCUT,    &CScreenToolDlg::OnRectcut)
	ON_COMMAND(IDM_WINDOWCUT,  &CScreenToolDlg::OnWindowcut)
	ON_COMMAND(IDM_SCREENCUT,  &CScreenToolDlg::OnScreencut)
	ON_COMMAND(IDM_CURRENTCUT, &CScreenToolDlg::OnCurrentcut)

	ON_BN_CLICKED(IDC_NEWCUT_BTN, &CScreenToolDlg::OnBnClickedNewcutBtn)
	ON_BN_CLICKED(IDC_PICKCOLOR_BTN, &CScreenToolDlg::OnBnClickedPickcolorBtn)
	ON_BN_CLICKED(IDC_MAGNIFIER_BTN, &CScreenToolDlg::OnBnClickedMagnifierBtn)
	ON_WM_HOTKEY()
	
	ON_MESSAGE(WM_TRAY_MSG, &CScreenToolDlg::OnTrayCallBackMsg)
	ON_WM_SYSCOMMAND()

	ON_COMMAND(100, &CScreenToolDlg::OnExit)
END_MESSAGE_MAP()


// CScreenToolDlg 消息处理程序

BOOL CScreenToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//加载菜单资源
	m_cutPopupMenu.LoadMenu(IDR_CAPTURE_MENU);
	CMenu *pMenu = m_cutPopupMenu.GetSubMenu(0);
	pMenu->CheckMenuRadioItem(0, 2, 0, MF_BYPOSITION);

	//m_trayPopupMenu.LoadMenu(IDR_MENU2);
	m_clipType = RECTCLIP;

	// TODO: 在此添加额外的初始化代码
	m_images.Create(32, 32, ILC_COLOR32 | ILC_MASK, 1, 0);
	m_images.Add(AfxGetApp()->LoadIcon(IDI_CLIP_ICON)); //加载图标
	m_images.Add(AfxGetApp()->LoadIcon(IDI_SNAP_ICON));
	m_images.Add(AfxGetApp()->LoadIcon(IDI_MAGNIFIER_ICON));
	
	m_imageCutBtn.SetImageList(&m_images);				//设置图像列表
	m_imageMagnifierBtn.SetImageList(&m_images);
	m_imageColorBtn.SetImageList(&m_images);
	//m_imageHelpBtn.SetImageList(&m_images);

	//设置按钮显示的图像索引
	m_imageCutBtn.SetImageIndex(0);
	m_imageColorBtn.SetImageIndex(1);
	m_imageMagnifierBtn.SetImageIndex(2);
	//m_imageHelpBtn.SetImageIndex(3);
	
	//设置按钮背景色
	m_imageCutBtn.SetNormalBkColor(RGB(0xd2, 0xd2, 0xe3));
	m_imageCutBtn.SetActiveBkColor(RGB(0xd8, 0xf6, 0xfc));
	m_imageCutBtn.SetPressedBkColor(RGB(0xbb, 0xec, 0xf9));

	m_imageColorBtn.SetNormalBkColor(RGB(0xd2, 0xd2, 0xe3));
	m_imageColorBtn.SetActiveBkColor(RGB(0xd8, 0xf6, 0xfc));
	m_imageColorBtn.SetPressedBkColor(RGB(0xbb, 0xec, 0xf9));

	m_imageMagnifierBtn.SetNormalBkColor(RGB(0xd2, 0xd2, 0xe3));
	m_imageMagnifierBtn.SetActiveBkColor(RGB(0xd8, 0xf6, 0xfc));
	m_imageMagnifierBtn.SetPressedBkColor(RGB(0xbb, 0xec, 0xf9));

	m_imageDropCutBtn.SetNormalBkColor(RGB(0xd2, 0xd2, 0xe3));
	m_imageDropCutBtn.SetActiveBkColor(RGB(0xd8, 0xf6, 0xfc));
	m_imageDropCutBtn.SetPressedBkColor(RGB(0xbb, 0xec, 0xf9));

	/*m_imageHelpBtn.SetNormalBkColor(RGB(0xd2, 0xd2, 0xe3));
	m_imageHelpBtn.SetActiveBkColor(RGB(0xd8, 0xf6, 0xfc));
	m_imageHelpBtn.SetPressedBkColor(RGB(0xbb, 0xec, 0xf9));*/

	//注册窗口类
	m_pszClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, 0, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	
	//注册热键
	RegisterHotKey(m_hWnd, IDHOTKEYCFS, MOD_CONTROL, 'C');
	RegisterHotKey(m_hWnd, IDHOTKEYCWS, MOD_CONTROL, 'X');
	RegisterHotKey(m_hWnd, IDHOTKEYCRS, MOD_CONTROL, 'Z');
	RegisterHotKey(m_hWnd, IDHOTKEYCFWS, MOD_CONTROL, 'A');
	RegisterHotKey(m_hWnd, IDHOTKEYSC, MOD_CONTROL, 'S');
	RegisterHotKey(m_hWnd, IDHOTKEYSM, MOD_CONTROL, 'D');
	m_bTray = FALSE;
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CScreenToolDlg::OnCancel()
{
	CDialog::OnCancel();
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CScreenToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CScreenToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CScreenToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	if (nCtlColor == CTLCOLOR_DLG)
		hbr = CreateSolidBrush(RGB(0xd2, 0xd2, 0xe3));
	return hbr;
}

BOOL CScreenToolDlg::SetTray(BOOL bAdd)
{
	BOOL bRet = FALSE;
	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = m_hWnd;
	tnd.uID = IDR_MAINFRAME;
	if ( bAdd == TRUE ) {
		tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		tnd.uCallbackMessage = WM_TRAY_MSG;
		tnd.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
		_tcscpy_s(tnd.szTip, sizeof(tnd.szTip), _T("ScreenTool v1.0"));
		bRet = Shell_NotifyIcon(NIM_ADD, &tnd);
	}else{
		bRet = Shell_NotifyIcon(NIM_DELETE, &tnd);
	}
	return bRet;
}

LRESULT CScreenToolDlg::OnTrayCallBackMsg(WPARAM wparam, LPARAM lparam)
{
	switch (lparam)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		ShowWindow(SW_RESTORE);
		break;
	case WM_RBUTTONDOWN:
		CMenu *pMenu = NULL;
		CPoint pt;
		pMenu = m_trayPopupMenu.GetSubMenu(0);
		GetCursorPos(&pt);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
		break;
	}
	return 0;
}

void CScreenToolDlg::OnBnClickedDropcutBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CMenu *menu;
	menu = m_cutPopupMenu.GetSubMenu(0);
	CRect rect;
	GetDlgItem(IDC_DROPCUT_BTN)->GetWindowRect(&rect);
	menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON , rect.left, rect.bottom + 1, this);
}

void CScreenToolDlg::OnRectcut()
{
	// TODO: 在此添加命令处理程序代码
	CMenu *pMenu = m_cutPopupMenu.GetSubMenu(0);
	pMenu->CheckMenuRadioItem(0, 6, 0, MF_BYPOSITION);
	m_clipType = RECTCLIP;
}

void CScreenToolDlg::OnWindowcut()
{
	// TODO: 在此添加命令处理程序代码
	CMenu *pMenu = m_cutPopupMenu.GetSubMenu(0);
	pMenu->CheckMenuRadioItem(0, 6, 2, MF_BYPOSITION);
	m_clipType = WINDOWCLIP;
}

void CScreenToolDlg::OnScreencut()
{
	// TODO: 在此添加命令处理程序代码
	CMenu *pMenu = m_cutPopupMenu.GetSubMenu(0);
	pMenu->CheckMenuRadioItem(0, 6, 4, MF_BYPOSITION);
	m_clipType = SCREENCLIP;
}

void CScreenToolDlg::OnCurrentcut()
{
	CMenu *pMenu = m_cutPopupMenu.GetSubMenu(0);
	pMenu->CheckMenuRadioItem(0, 6, 6, MF_BYPOSITION);
	m_clipType = CURRENTCLIP;
}

void CScreenToolDlg::OnBnClickedNewcutBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	
	//
	
	switch (m_clipType)
	{
	case RECTCLIP:
		ClipRectOfScreen();
		break;
	case WINDOWCLIP:
		ClipWindowOfScreen();
		break;
	case SCREENCLIP:
		ClipFullScreenToFile();
		break;
	case CURRENTCLIP:
		ClipForegroundWindow();
		break;
	default:
		break;
	}
}

void CScreenToolDlg::ClipWindowOfScreen()
{
	CSnapWindowDlg dlg;
	ShowWindow(SW_HIDE);
	dlg.IsExit = IsExit;
	dlg.DoModal();
	ShowWindow(SW_SHOW);
}

//矩形区域截图
void CScreenToolDlg::ClipRectOfScreen()
{
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = GetSystemMetrics(SM_CXSCREEN);
	rect.bottom = GetSystemMetrics(SM_CYSCREEN);

	//隐藏应用程序窗口
	ShowWindow(SW_HIDE);
	Sleep(300);

	//创建全屏截图窗口
	if( !m_fullScreenWnd.CreateEx(0, m_pszClassName, _T("FULLSCREENWND"), /*WS_CHILD |*/ WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, this->GetSafeHwnd(), NULL) )
		AfxMessageBox(_T("窗口创建失败!"));
	else
	{
		m_fullScreenWnd.IsExit = IsExit;
		m_fullScreenWnd.ShowWindow(SW_SHOW);
	}
}

//截取当前前景窗口
void CScreenToolDlg::ClipForegroundWindow()
{
	//定义文件路径
	CString strPathName;

	//定义屏幕矩形区域
	RECT rect;

	//定义位图句柄
	HBITMAP hBitmap;

	//隐藏应用程序窗口
	ShowWindow(SW_HIDE);
	Sleep(300);

	//获取当前前景窗口
	HWND hWnd = ::GetForegroundWindow();

	//获取屏幕矩形区域
	::GetWindowRect(hWnd, &rect);

	//获取屏幕矩形区域位图句柄
	hBitmap = CopyScreenToBitmap(&rect);

	//显示应用程序窗口
	ShowWindow(SW_SHOW);

	if( MessageBox(_T("是否保存截屏为文件?"), _T("另存为选择"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		//定义文件对话框对象
		CFileDialog fd(FALSE, _T("png"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, NULL);

		//设置文件过滤器
		fd.m_ofn.lpstrFilter = _T("PNG 文件\0*.png\0JPEG 文件\0*.jpg\0位图文件\0*.bmp\0All Files\0*.*\0\0");

		//打开文件对话框
		fd.DoModal();

		//获取选择的文件路经
		strPathName = fd.GetPathName();

		//判断文件路径是否为空
		if(strPathName.IsEmpty())
		{
			// 销毁hBitmap对象
			DeleteObject(hBitmap);
			return;
		}

		//定义CImage对象,用来将位图保存到一个文件上
		CImage image;

		//将位图句柄关联到image对象
		image.Attach(hBitmap);

		//将位图保存到一个文件上
		image.Save(strPathName);

		//销毁image对象
		image.Destroy();

		// 销毁hBitmap对象
		DeleteObject(hBitmap);
	}
	else
	{
		// 保存到剪辑版
		if(::OpenClipboard(m_hWnd))
		{
			::EmptyClipboard();
			::SetClipboardData(CF_BITMAP, hBitmap);
			::CloseClipboard();
		}

		// 销毁hBitmap对象
		DeleteObject(hBitmap);

		// 退出截屏
		if(IsExit)
			OnExit();
	}
}

//全屏截图
void CScreenToolDlg::ClipFullScreenToFile()
{
	//定义文件路径
	CString strPathName;

	//定义屏幕矩形区域
	RECT rect;

	//定义位图句柄
	HBITMAP hBitmap;

	//获取屏幕矩形区域
	rect.left = 0;
	rect.top = 0;
	rect.right = GetSystemMetrics(SM_CXSCREEN);
	rect.bottom = GetSystemMetrics(SM_CYSCREEN);

	//隐藏程序主窗口
	ShowWindow(SW_HIDE);
	Sleep(300);

	//获取屏幕矩形区域位图句柄
	hBitmap = CopyScreenToBitmap(&rect);
	ShowWindow(SW_SHOW);

	if( MessageBox(_T("是否保存截屏为文件?"), _T("另存为选择"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		//定义文件对话框对象
		CFileDialog fd(FALSE, _T("png"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, NULL);

		//设置文件过滤器
		fd.m_ofn.lpstrFilter = _T("PNG 文件\0*.png\0JPEG 文件\0*.jpg\0位图文件\0*.bmp\0All Files\0*.*\0\0");

		//打开文件对话框
		fd.DoModal();

		//获取选择的文件路经
		strPathName = fd.GetPathName();

		//判断文件路径是否为空
		if(strPathName.IsEmpty())
		{
			// 销毁hBitmap对象
			DeleteObject(hBitmap);
			return;
		}

		//定义CImage对象,用来将位图保存到一个文件上
		CImage image;

		//将位图句柄关联到image对象
		image.Attach(hBitmap);

		//将位图保存到一个文件上
		image.Save(strPathName);

		//销毁image对象
		image.Destroy();

		// 销毁hBitmap对象
		DeleteObject(hBitmap);
	}
	else
	{
		// 保存到剪辑版
		if(::OpenClipboard(m_hWnd))
		{
			::EmptyClipboard();
			::SetClipboardData(CF_BITMAP, hBitmap);
			::CloseClipboard();
		}

		// 销毁hBitmap对象
		DeleteObject(hBitmap);

		// 退出截屏
		if(IsExit)
			OnExit();
	}
}

void CScreenToolDlg::OnBnClickedPickcolorBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	//定义颜色拾色器对话框
	CPickColorDlg dlg;
	dlg.DoModal();
}

void CScreenToolDlg::OnBnClickedMagnifierBtn()
{
	// TODO: 在此添加控件通知处理程序代码
	//定义放大镜对话框
	CMagnifierDlg dlg;
	dlg.DoModal();
}

void CScreenToolDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	CDialog *pDlg;
	switch (nHotKeyId)
	{
	case IDHOTKEYCFS:
		ClipFullScreenToFile();
		break;
	case IDHOTKEYCWS:
		ClipWindowOfScreen();
		break;
	case IDHOTKEYCRS:
		ClipRectOfScreen();
		break;
	case IDHOTKEYCFWS:
		ClipForegroundWindow();
		break;
	case IDHOTKEYSC:
		pDlg = new CPickColorDlg;
		pDlg->DoModal();
		delete pDlg;
		break;
	case IDHOTKEYSM:
		pDlg = new CMagnifierDlg;
		pDlg->DoModal();
		delete pDlg;
		break;
	default:
		break;
	}
}

void CScreenToolDlg::OnBnClickedHelpBtn()
{
}

void CScreenToolDlg::OnShowMenu()
{
	// TODO: 在此添加命令处理程序代码
	ShowWindow(SW_RESTORE);
}

void CScreenToolDlg::OnHelpMenu()
{
}

void CScreenToolDlg::OnExitMenu()
{
	// TODO: 在此添加命令处理程序代码
	SendMessage(WM_CLOSE, 0, 0);
}

void CScreenToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnSysCommand(nID, lParam);
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
		if (!m_bTray)
		{
			SetTray(TRUE);
			m_bTray = TRUE;
		}
	}
}

void CScreenToolDlg::OnExit()
{
	CDialog::OnOK();
}
