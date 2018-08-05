// PickColorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../编程助理.h"
#include "PickColorDlg.h"
// CPickColorDlg 对话框
CPickColorDlg::CPickColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPickColorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_SNAP_ICON);
	m_bPause = FALSE;
}

CPickColorDlg::~CPickColorDlg()
{
	
}
BOOL CPickColorDlg::OnInitDialog()
{
	SetIcon(m_hIcon, FALSE);
	RegisterHotKey(m_hWnd, ID_HOTKEYPAUSE, MOD_CONTROL, 'Q');
	RegisterHotKey(m_hWnd, ID_HOTKEYSTART, MOD_CONTROL, 'W');
	SetTimer(222, 100, NULL);
	return TRUE;
}
void CPickColorDlg::OnCancel()
{
	KillTimer(222);
	DestroyIcon(m_hIcon);
	UnregisterHotKey(m_hWnd, ID_HOTKEYSTART);
	UnregisterHotKey(m_hWnd, ID_HOTKEYPAUSE);
	CDialog::OnCancel();
}
void CPickColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPickColorDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_HOTKEY()
END_MESSAGE_MAP()


// CPickColorDlg 消息处理程序
void CPickColorDlg::ShowColorValue()
{
	//定义设备描述表句柄
	CDC *pDC;
	RECT rt;
	RECT clientRect;
	//得到客户区的矩形区域
	GetClientRect(&clientRect);
	rt.left = clientRect.left + 20;
	rt.top = clientRect.top + 10;
	rt.right = clientRect.right - 20;
	rt.bottom = 50;
	//定义缓冲区
	TCHAR buff[50];
	CBrush brush;
	//创建单色画刷
	brush.CreateSolidBrush(m_color);
	//获取客户区设备描述表句柄
	pDC = GetDC();
	//用画刷填充矩形区域
	pDC->FillRect(&rt, &brush);
	brush.DeleteObject();
	
	wsprintf(buff, _T("%d,%d,%d"), GetRValue(m_color), GetGValue(m_color), GetBValue(m_color));
	GetDlgItem(IDC_RGBCOLOR_EDIT)->SetWindowText(buff);
	wsprintf(buff, _T("#%08X"), m_color);
	GetDlgItem(IDC_PAGECOLOR_EDIT)->SetWindowText(buff);
	wsprintf(buff, _T("0x%08X"), m_color);
	GetDlgItem(IDC_HEXCOLOR_EDIT)->SetWindowText(buff);
	ReleaseDC(pDC);
}
void CPickColorDlg::GetColorOfScreen()
{
	//定义屏幕设备句柄
	HDC hScreenDC;
	POINT pt;
	//获取鼠标位置
	GetCursorPos(&pt);
	//得到屏幕设备句柄
	hScreenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	//得到屏幕指定点的颜色值
	m_color = GetPixel(hScreenDC, pt.x, pt.y);
	DeleteDC(hScreenDC);
}
void CPickColorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!m_bPause)
	{
		//得到鼠标位置的屏幕颜色值
		GetColorOfScreen();
		//显示获取到的颜色值
		ShowColorValue();
		
	}	
}

void CPickColorDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nHotKeyId == ID_HOTKEYPAUSE)
		m_bPause = TRUE;
	else if (nHotKeyId == ID_HOTKEYSTART)
		m_bPause = FALSE;
	CDialog::OnHotKey(nHotKeyId, nKey1, nKey2);
}
