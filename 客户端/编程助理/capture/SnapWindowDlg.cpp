// SnapWindowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../编程助理.h"
#include "SnapWindowDlg.h"
#include "common.h"
#include <atlimage.h>
// CSnapWindowDlg 对话框
CSnapWindowDlg::CSnapWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSnapWindowDlg::IDD, pParent)
{
	m_hCursor = AfxGetApp()->LoadCursor(IDC_TARGET_CURSOR);
	m_rectOfWndBmp = NULL;
	m_preWnd = NULL;
	m_curWnd = NULL;
	m_bSnap = FALSE;

	IsExit = TRUE;
}

CSnapWindowDlg::~CSnapWindowDlg()
{
}

BOOL CSnapWindowDlg::OnInitDialog()
{
	return TRUE;
}

void CSnapWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CSnapWindowDlg::ClearScreen()
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

void CSnapWindowDlg::SnapWindow(POINT point)
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

void CSnapWindowDlg::SaveBmpToFile()
{
	//隐藏应用程序窗口
	ShowWindow(SW_HIDE);
	Sleep(300);

	HBITMAP hBitmap = CopyScreenToBitmap(&m_rectOfWnd);

	//定义文件路径
	CString strPathName;

	if( MessageBox(_T("是否保存截屏为文件?"), _T("另存为选择"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		//定义文件对话框对象
		CFileDialog fd(FALSE, _T("jpg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, NULL);

		//设置文件过滤器
		fd.m_ofn.lpstrFilter = _T("PNG 文件\0*.png\0JPEG 文件\0*.jpg\0位图文件\0*.bmp\0All Files\0*.*\0\0");

		//打开文件对话框
		fd.DoModal();

		//获取选择的文件路经
		strPathName = fd.GetPathName();

		//判断文件路径是否为空
		if(strPathName.IsEmpty())
		{
			//销毁hBitmap对象
			DeleteObject(hBitmap);

			ShowWindow(SW_SHOW);
		}

		//定义CImage对象,用来将位图保存到一个文件上
		CImage image;

		//将位图句柄关联到image对象
		image.Attach(hBitmap);

		//将位图保存到一个文件上
		image.Save(strPathName);

		//销毁image对象
		image.Destroy();

		//销毁hBitmap对象
		DeleteObject(hBitmap);

		// 显示窗口
		ShowWindow(SW_SHOW);
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

		//销毁hBitmap对象
		DeleteObject(hBitmap);

		if(IsExit)
		{
			CDialog::OnOK();

			// 发送消息给父窗口
			::SendMessage(this->GetParent()->GetSafeHwnd(), WM_COMMAND, 100, 0);
		}
		else
			ShowWindow(SW_SHOW);
	}
}

BEGIN_MESSAGE_MAP(CSnapWindowDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CSnapWindowDlg 消息处理程序

void CSnapWindowDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetCursor(m_hCursor);
	SetCapture();
	m_bSnap = TRUE;
	CDialog::OnLButtonDown(nFlags, point);
}

void CSnapWindowDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ReleaseCapture();
	ClearScreen();
	SaveBmpToFile();
	m_preWnd = NULL;
	m_curWnd = NULL;
	m_bSnap = FALSE;
	
}

void CSnapWindowDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bSnap)
		SnapWindow(point);
}
