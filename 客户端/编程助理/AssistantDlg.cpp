// AssistantDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "AssistantDlg.h"
#include "afxdialogex.h"

// CAssistantDlg 对话框

IMPLEMENT_DYNAMIC(CAssistantDlg, CDialogEx)

CAssistantDlg::CAssistantDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAssistantDlg::IDD, pParent)
{
	IsCreated = FALSE;
}

CAssistantDlg::~CAssistantDlg()
{
}

void CAssistantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CODE_COMBO, m_ComboBox);
	DDX_Control(pDX, IDC_HIDE_CHECK, m_Hide);
	DDX_Control(pDX, IDC_FOLLOW_CHECK, m_Follow);
}


BEGIN_MESSAGE_MAP(CAssistantDlg, CDialogEx)
	ON_WM_HOTKEY()
	ON_CBN_DROPDOWN(IDC_CODE_COMBO, &CAssistantDlg::OnDropdownCodeCombo)
	ON_BN_CLICKED(IDC_HIDE_CHECK, &CAssistantDlg::OnHideCheck)
	ON_BN_CLICKED(IDC_FOLLOW_CHECK, &CAssistantDlg::OnFollowCheck)
END_MESSAGE_MAP()


// CAssistantDlg 消息处理程序


BOOL CAssistantDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 读取自动隐藏
	if( GetPrivateProfileInt(_T("Setting"), _T("AutoHide"), 1, _T("./Setting.ini")) == 1)
	{
		m_Hide.SetCheck(1);
		ShowWindow(SW_HIDE);
	}
	else
	{
		m_Hide.SetCheck(0);
		ShowWindow(SW_SHOW);
	}

	// 读取跟随鼠标
	if( GetPrivateProfileInt(_T("Setting"), _T("Follow"), 1, _T("./Setting.ini")) == 1)
	{
		m_Follow.SetCheck(1);
	}
	else
	{
		m_Follow.SetCheck(0);
	}

	// 注册热键
	::RegisterHotKey(m_hWnd, ID_HOTKEY_A, MOD_ALT, 'A');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_B, MOD_ALT, 'B');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_C, MOD_ALT, 'C');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_D, MOD_ALT, 'D');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_E, MOD_ALT, 'E');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_F, MOD_ALT, 'F');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_G, MOD_ALT, 'G');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_H, MOD_ALT, 'H');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_I, MOD_ALT, 'I');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_J, MOD_ALT, 'J');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_K, MOD_ALT, 'K');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_L, MOD_ALT, 'L');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_M, MOD_ALT, 'M');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_N, MOD_ALT, 'N');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_O, MOD_ALT, 'O');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_P, MOD_ALT, 'P');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_Q, MOD_ALT, 'Q');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_R, MOD_ALT, 'R');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_S, MOD_ALT, 'S');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_T, MOD_ALT, 'T');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_U, MOD_ALT, 'U');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_V, MOD_ALT, 'V');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_W, MOD_ALT, 'W');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_X, MOD_ALT, 'X');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_Y, MOD_ALT, 'Y');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_Z, MOD_ALT, 'Z');

	::RegisterHotKey(m_hWnd, ID_HOTKEY_LEFT,  MOD_ALT, VK_LEFT);
	::RegisterHotKey(m_hWnd, ID_HOTKEY_RIGHT, MOD_ALT, VK_RIGHT);
	::RegisterHotKey(m_hWnd, ID_HOTKEY_UP,    MOD_ALT, VK_UP);
	::RegisterHotKey(m_hWnd, ID_HOTKEY_DOWN,  MOD_ALT, VK_DOWN);

	// 提示框
	m_toolTips.Create(this, TTS_ALWAYSTIP|WS_POPUP);
	m_toolTips.Activate(TRUE);

	m_toolTips.AddTool(GetDlgItem(IDC_CODE_COMBO),   _T("显示当前分类所有方法。\nAlt + 方法首字母快速查找方法。\nAtl + 上下方向键切换当前类别。\nAtl + 左右方向键切换当前类型。"));
	m_toolTips.AddTool(GetDlgItem(IDC_HIDE_CHECK),   _T("选中此项后编码助理将自动隐藏。\n按Atl+方法首字母调出。"));
	m_toolTips.AddTool(GetDlgItem(IDC_FOLLOW_CHECK), _T("选中此项后隐藏的编码助理将跟随鼠标的位置出现。\n若没有自动隐藏, 不建议使用此功能。"));
	m_toolTips.AddTool(GetDlgItem(IDOK),             _T("读取目标方法并在光标位置插入。"));
	m_toolTips.AddTool(GetDlgItem(IDCANCEL),         _T("隐藏快速编码助理。\n隐藏后可用Atl+方法首字母调出。"));

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


BOOL CAssistantDlg::PreTranslateMessage(MSG* pMsg)
{
	// 功能提示
	if(GetPrivateProfileInt(_T("Setting"), _T("Tip"), 0, _T("./Setting.ini")) == 1)
		m_toolTips.RelayEvent(pMsg); // 接受消息响应

	return CDialogEx::PreTranslateMessage(pMsg);
}


UINT CAssistantDlg::Operate(LPVOID pParam)
{
	// 窗口指针
	CAssistantDlg * pWnd = ((CAssistantDlg*)pParam);

	while(::IsWindow(pWnd->TargetWnd))
	{
		Sleep(100);
	}

	pWnd->ShowWindow(SW_HIDE);
	AfxGetApp()->GetMainWnd()->ShowWindow(SW_RESTORE);
	return TRUE;
}


void CAssistantDlg::OnOK()
{
	// 得到目标数据
	CString Target;
	m_ComboBox.GetWindowText(Target);
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 23, (LPARAM)(LPCTSTR)Target);

	// 读取窗口位置
	if( GetPrivateProfileInt(_T("Setting"), _T("AutoHide"), 1, _T("./Setting.ini")) == 1)
	{
		// 隐藏窗口
		ShowWindow(SW_HIDE);
	}

	// 前置窗口
	::SetForegroundWindow(TargetWnd);

	char * buffer = NULL;
	//打开剪贴板
	CString fromClipboard;
	if ( OpenClipboard() )
	{
		HANDLE hData = GetClipboardData(CF_TEXT);
		char * buffer = (char*)GlobalLock(hData);
		fromClipboard = buffer;
		GlobalUnlock(hData);
		CloseClipboard();
	}

	// 粘贴内容
	keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), 0 ,0);
	keybd_event(86, MapVirtualKey(L'V',0),0,0);

	Sleep(50);
	keybd_event(86,  MapVirtualKey(L'V',0), KEYEVENTF_KEYUP,0);
	keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP,0);
}


void CAssistantDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	CRect rc;
	CRect rect;
	GetWindowRect(&rect);//窗体大小
	rc.CopyRect(&rect);//复制矩形区
	CPoint pOint;
	GetCursorPos(&pOint);//获取鼠标位置

	// 读取跟随鼠标
	if( GetPrivateProfileInt(_T("Setting"), _T("Follow"), 1, _T("./Setting.ini")) == 1)
	{
		// 移动窗口
		MoveWindow(pOint.x,pOint.y,rc.Width(),rc.Height());//移动窗体
	}

	// 显示窗口
	ShowWindow(SW_SHOW);

	// 设置焦点
	m_ComboBox.SetFocus();

	switch (nHotKeyId)
	{
	case ID_HOTKEY_A:
		m_ComboBox.SetWindowText(_T("A"));
		break;

	case ID_HOTKEY_B:
		m_ComboBox.SetWindowText(_T("B"));
		break;

	case ID_HOTKEY_C:
		m_ComboBox.SetWindowText(_T("C"));
		break;

	case ID_HOTKEY_D:
		m_ComboBox.SetWindowText(_T("D"));
		break;

	case ID_HOTKEY_E:
		m_ComboBox.SetWindowText(_T("E"));
		break;

	case ID_HOTKEY_F:
		m_ComboBox.SetWindowText(_T("F"));
		break;

	case ID_HOTKEY_G:
		m_ComboBox.SetWindowText(_T("G"));
		break;

	case ID_HOTKEY_H:
		m_ComboBox.SetWindowText(_T("H"));
		break;

	case ID_HOTKEY_I:
		m_ComboBox.SetWindowText(_T("I"));
		break;

	case ID_HOTKEY_J:
		m_ComboBox.SetWindowText(_T("J"));
		break;

	case ID_HOTKEY_K:
		m_ComboBox.SetWindowText(_T("K"));
		break;

	case ID_HOTKEY_L:
		m_ComboBox.SetWindowText(_T("L"));
		break;

	case ID_HOTKEY_M:
		m_ComboBox.SetWindowText(_T("M"));
		break;

	case ID_HOTKEY_N:
		m_ComboBox.SetWindowText(_T("N"));
		break;

	case ID_HOTKEY_O:
		m_ComboBox.SetWindowText(_T("O"));
		break;

	case ID_HOTKEY_P:
		m_ComboBox.SetWindowText(_T("P"));
		break;

	case ID_HOTKEY_Q:
		m_ComboBox.SetWindowText(_T("Q"));
		break;

	case ID_HOTKEY_R:
		m_ComboBox.SetWindowText(_T("R"));
		break;

	case ID_HOTKEY_S:
		m_ComboBox.SetWindowText(_T("S"));
		break;

	case ID_HOTKEY_T:
		m_ComboBox.SetWindowText(_T("T"));
		break;

	case ID_HOTKEY_U:
		m_ComboBox.SetWindowText(_T("U"));
		break;

	case ID_HOTKEY_V:
		m_ComboBox.SetWindowText(_T("V"));
		break;

	case ID_HOTKEY_W:
		m_ComboBox.SetWindowText(_T("W"));
		break;

	case ID_HOTKEY_X:
		m_ComboBox.SetWindowText(_T("X"));
		break;

	case ID_HOTKEY_Y:
		m_ComboBox.SetWindowText(_T("Y"));
		break;

	case ID_HOTKEY_Z:
		m_ComboBox.SetWindowText(_T("Z"));
		break;

	case ID_HOTKEY_LEFT:
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 24, 0);
		break;

	case ID_HOTKEY_RIGHT:
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 25, 0);
		break;

	case ID_HOTKEY_UP:
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 26, 0);
		break;

	case ID_HOTKEY_DOWN:
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 27, 0);
		break;

	default:
		break;
	}

	// 自动匹配
	m_ComboBox.OnMatch();

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CAssistantDlg::OnSetCode()
{
	GetDlgItem(IDC_TIP_STATIC)->SetWindowText(_T("类型: ") + FileClass + _T("      类别: ") + FileType);

	// 清空
	m_ComboBox.ResetContent();

	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
		{
			continue;
		}
		else
		{
			CString Name = Finder.GetFileName();
			m_ComboBox.AddString(Name.Left(Name.GetLength() - 5));
		}
	}
}


void CAssistantDlg::OnOversee()
{
	AfxBeginThread(Operate, this);
}


void CAssistantDlg::OnDropdownCodeCombo()
{
	// 通知主窗口
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 22, 0);
}


void CAssistantDlg::OnHideCheck()
{
	if(m_Hide.GetCheck())
	{
		::WritePrivateProfileString(_T("Setting"), _T("AutoHide"), _T("1"), _T("./Setting.ini"));
	}
	else
	{
		::WritePrivateProfileString(_T("Setting"), _T("AutoHide"), _T("0"), _T("./Setting.ini"));
	}
}


void CAssistantDlg::OnFollowCheck()
{
	if(m_Follow.GetCheck())
	{
		::WritePrivateProfileString(_T("Setting"), _T("Follow"), _T("1"), _T("./Setting.ini"));
	}
	else
	{
		::WritePrivateProfileString(_T("Setting"), _T("Follow"), _T("0"), _T("./Setting.ini"));
	}
}


BOOL CAssistantDlg::DestroyWindow()
{
	//删除热键
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_A);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_B);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_C);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_D);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_E);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_F);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_G);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_H);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_I);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_J);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_K);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_L);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_M);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_N);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_O);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_P);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_Q);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_R);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_S);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_T);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_U);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_V);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_W);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_X);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_Y);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_Z);

	::UnregisterHotKey(m_hWnd, ID_HOTKEY_LEFT);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_RIGHT);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_UP);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_DOWN);

	return CDialogEx::DestroyWindow();
}


void CAssistantDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}
