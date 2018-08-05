// MagnifierDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../编程助理.h"
#include "MagnifierDlg.h"
#include "common.h"
// CMagnifierDlg 对话框



CMagnifierDlg::CMagnifierDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMagnifierDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAGNIFIER_ICON);
}

CMagnifierDlg::~CMagnifierDlg()
{
}
void CMagnifierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BOOL CMagnifierDlg::OnInitDialog()
{
	SetTimer(222, 100, NULL);
	SetIcon(m_hIcon, TRUE);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CMagnifierDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMagnifierDlg 消息处理程序
void CMagnifierDlg::MagnifyScreen()
{
	POINT pt;
	//获取鼠标位置
	GetCursorPos(&pt);
	int x, y;
	int width, height;
	//设置捕获屏幕大小
	width = 100;
	height = 80;
	RECT rt;
	//设置需要放大的屏幕矩形区域
	rt.left = pt.x - width/2;
	rt.right = pt.x + width/2;
	rt.top = pt.y - height/2;
	rt.bottom = pt.y + height/2;
	HBITMAP hBitmap;
	hBitmap = CopyScreenToBitmap(&rt);
	CDC	pMemDC;
	CDC *pDC;
	pDC = GetDC();
	pMemDC.CreateCompatibleDC(pDC);
	pMemDC.SelectObject(hBitmap);
	x = 10;
	y = 10;
	pDC->StretchBlt(x, y, width * 2, height * 2, &pMemDC, 0, 0, width, height, SRCCOPY);
	DeleteObject(hBitmap);
	pMemDC.DeleteDC();
	ReleaseDC(pDC);
}
void CMagnifierDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	MagnifyScreen();
}
void CMagnifierDlg::OnCancel()
{
	DestroyIcon(m_hIcon);
	KillTimer(222);
	CDialog::OnCancel();
}