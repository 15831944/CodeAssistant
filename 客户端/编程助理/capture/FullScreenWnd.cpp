// SreenWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "FullScreenWnd.h"
#include <atlimage.h>
#include "common.h"
// CSreenWnd



CFullScreenWnd::CFullScreenWnd()
{
	m_brush.CreateSolidBrush(RGB(0, 0, 255));
	//获取4种标准光标资源句柄
	hCursor[0] = LoadCursor(NULL, IDC_CROSS);
	hCursor[1] = LoadCursor(NULL, IDC_SIZENS);
	hCursor[2] = LoadCursor(NULL, IDC_SIZEWE);
	hCursor[3] = LoadCursor(NULL, IDC_SIZEALL);
	hCursor[4] = LoadCursor(NULL, IDC_ARROW);
	m_catchMouse = TRUE;

	IsExit = TRUE;
}

CFullScreenWnd::~CFullScreenWnd()
{
}

BEGIN_MESSAGE_MAP(CFullScreenWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
// CSreenWnd 消息处理程序
BOOL CFullScreenWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//定义屏幕宽度,高度
	int nWidth, nHeight;
	//获取屏幕宽度和高度
	nWidth = GetSystemMetrics(SM_CXSCREEN);
	nHeight = GetSystemMetrics(SM_CYSCREEN);
	
	pDC->BitBlt(0, 0, nWidth, nHeight, &m_compatibleDC, 0, 0, SRCCOPY);
	return TRUE;
}


void CFullScreenWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bClip)
		SetCursor(hCursor[0]);
	else
	{
		if (m_catchMouse)
		{
		m_mousePos = CalPointPosition(&m_drawRect, point);
		if (m_mousePos == UPPER || m_mousePos == DOWN)
			SetCursor(hCursor[1]);
		else if(m_mousePos == INNER)
			SetCursor(hCursor[3]);
		else if(m_mousePos == LEFT || m_mousePos == RIGHT)
			SetCursor(hCursor[2]);
		else if(m_mousePos == OUTSIDE)
			SetCursor(hCursor[4]);
		}
		
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CFullScreenWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWnd::OnRButtonDown(nFlags, point);
}

void CFullScreenWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//this->ShowWindow(SW_HIDE);
	this->GetParent()->ShowWindow(SW_SHOW);
	m_bClip = TRUE;
	this->DestroyWindow();

	CWnd::OnRButtonUp(nFlags, point);
}

void CFullScreenWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//设置定时器

	SetTimer(888, 30, NULL);
	m_oriPoint.x = point.x;
	m_oriPoint.y = point.y;
	m_catchMouse = FALSE;
	CWnd::OnLButtonDown(nFlags, point);
}

void CFullScreenWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	KillTimer(888);
	if (m_bClip)
		m_bClip = FALSE;
	m_catchMouse = TRUE;
	ShowTip();
	CWnd::OnLButtonUp(nFlags, point);
}

//功能:显示提示信息
void CFullScreenWnd::ShowTip()
{
	CDC *pDC;
	pDC = GetDC();
	RECT rt;
	CSize size;
	int x, y;
	//定义屏幕宽度,高度
	int nWidth, nHeight;
	//获取屏幕宽度和高度
	nWidth = GetSystemMetrics(SM_CXSCREEN);
	nHeight = GetSystemMetrics(SM_CYSCREEN);
	//设置背景为透明
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 0, 0));
	CString str(_T("温馨提示: 双击左键为保存图片,单击右键为取消"));
	size = pDC->GetTextExtent(str);
	if (m_drawRect.bottom + size.cy > nHeight)
		y = m_drawRect.top - size.cy - 4;
	else
		y = m_drawRect.bottom + 4;
	x = m_drawRect.right - size.cx;
	if (m_drawRect.right - size.cx < 0)
		x = 0;
	if (m_drawRect.right > nWidth)
		x = nWidth - size.cx;
	rt.left = x;
	rt.top = y;
	rt.right = x + size.cx;
	rt.bottom = y + size.cy;
	pDC->FrameRect(&rt, &m_brush);
	pDC->TextOut(x, y, str, str.GetLength());
	ReleaseDC(pDC);
}

int CFullScreenWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		AfxMessageBox(_T("窗口创建失败!"));
		return -1;
	}

	// TODO:  在此添加您专用的创建代码
	m_bClip = TRUE;	
	CRect rect;
	if (m_compatibleDC.m_hDC == NULL)
	{
		CDC *theDC;
		theDC = GetDC();
		m_compatibleDC.CreateCompatibleDC(theDC);
		ReleaseDC(theDC);
	}
	rect.left = 0;
	rect.top = 0;
	rect.right = GetSystemMetrics(SM_CXSCREEN);
	rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	m_hScreenBmp = CopyScreenToBitmap(&rect);
	CBitmap bitmap, *hOldBitmap;
	bitmap.Attach(m_hScreenBmp);
	hOldBitmap = m_compatibleDC.SelectObject(&bitmap);
	hOldBitmap->DeleteObject();
	return 0;
}

void CFullScreenWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

/*
	功能:
		计算鼠标坐标在绘制的矩形区域中的位置
	参数:
		lpRect 指向绘制的矩形区域
		point 鼠标位置坐标
	返回值:
		鼠标在绘制的矩形区域中的位置
*/
MOUSEPOSITION CFullScreenWnd::CalPointPosition(LPRECT lpRect, POINT point)
{
	//定义返回结果
	MOUSEPOSITION retPos;
	//定义矩形区域宽度,高度
	int nWidth, nHeight;
	//定义矩形区域的坐标位置
	int x1, x2, y1, y2;
	//获取矩形区域的两点位置
	x1 = lpRect->left;
	x2 = lpRect->right;
	y1 = lpRect->top;
	y2 = lpRect->bottom;	
	//计算矩形区域宽度, 高度
	nWidth = x2 - x1;
	nHeight = y2 - y1;
	//计算鼠标是否在矩形区域的内部
	if ((point.x > x1 && point.x < x2) &&(point.y > y1 && point.y < y2))
		retPos = INNER;
	//计算鼠标是否在矩形区域的上条边以中心位置开始的四周2个像素范围的位置
	else if ((point.x >= x1 + nWidth/2 - 2 && point.x <= x1 + nWidth/2 + 2)
			&&(point.y >= y1 - 2 && point.y <= y1 + 2))
		retPos = UPPER;
	//计算鼠标是否在矩形区域的下条边以中心位置开始的四周2个像素范围的位置
	else if ((point.x >= x1 + nWidth/2 - 2 && point.x <= x1 + nWidth/2 + 2)
		&&(point.y >= y2 - 2 && point.y <= y2 + 2))
		retPos = DOWN;
	//计算鼠标是否在矩形区域的左条边以中心位置开始的四周2个像素范围的位置
	else if ((point.x >= x1 - 2 && point.x <= x1 + 2)
		&&(point.y >= y1 + nHeight/2 - 2 && point.y <= y1 + nHeight/2 + 2))
		retPos = LEFT;
	//计算鼠标是否在矩形区域的右条边以中心位置开始的四周2个像素范围的位置
	else if ((point.x >= x2 - 2 && point.x <= x2 + 2)
		&&(point.y >= y1 + nHeight/2 - 2 && point.y <= y1 + nHeight/2 + 2))
		retPos = RIGHT;
	else
		retPos = OUTSIDE;
	return retPos;
}

void CFullScreenWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TrackDraw();
}

//动态绘制
void CFullScreenWnd::TrackDraw()
{
	CClientDC clientDC(this);
	int nWidth, nHeight;
	//获取屏幕宽度和高度
	nWidth = GetSystemMetrics(SM_CXSCREEN);
	nHeight = GetSystemMetrics(SM_CYSCREEN);
	POINT point;
	//得到当前鼠标的位置
	GetCursorPos(&point);
	//判断是否是截图标志
	if (m_bClip)
	{
		//获取矩形区域
		m_drawRect.left = m_oriPoint.x<point.x?m_oriPoint.x:point.x;
		m_drawRect.top  = m_oriPoint.y<point.y?m_oriPoint.y:point.y;
		m_drawRect.right = m_oriPoint.x<point.x?point.x:m_oriPoint.x;
		m_drawRect.bottom  = m_oriPoint.y<point.y?point.y:m_oriPoint.y;
		//清屏
		clientDC.BitBlt(0, 0, nWidth, nHeight, &m_compatibleDC, 0, 0, SRCCOPY);
		//绘制矩形
		DrawRect(&clientDC, &m_drawRect, &m_brush);
	}
	else 
	{
		int moveWidth, width, moveHeight, height;
		int temp;
		if (m_mousePos == OUTSIDE)
			return;
		//计算鼠标移动的距离
		moveWidth = point.x - m_oriPoint.x;
		moveHeight = point.y - m_oriPoint.y;
		//判断鼠标的位置
		switch (m_mousePos)
		{
			//在矩形区域的内部
		case INNER:
			//获取矩形的宽度和高度
			width = m_drawRect.right - m_drawRect.left;
			height = m_drawRect.bottom - m_drawRect.top;
			//重新计算矩形区域的位置
			m_drawRect.left += moveWidth;
			m_drawRect.top += moveHeight;
			m_drawRect.right = m_drawRect.left + width;
			m_drawRect.bottom = m_drawRect.top + height;	
			break;
			//在矩形区域的上部
		case UPPER:
			if (m_drawRect.top > m_drawRect.bottom)
			{
				
				temp = m_drawRect.top;
				m_drawRect.top = m_drawRect.bottom;
				m_drawRect.bottom = temp;
				m_mousePos = DOWN;
			}
			else
				m_drawRect.top += moveHeight;
			break;
			//在矩形区域的下部
		case DOWN:
			if (m_drawRect.bottom < m_drawRect.top)
			{

				temp = m_drawRect.top;
				m_drawRect.top = m_drawRect.bottom;
				m_drawRect.bottom = temp;
				m_mousePos = UPPER;
			}
			else
				m_drawRect.bottom += moveHeight;
			break;
			//在矩形区域的左部
		case LEFT:
			if (m_drawRect.left > m_drawRect.right)
			{

				temp = m_drawRect.left;
				m_drawRect.left = m_drawRect.right;
				m_drawRect.right = temp;
				m_mousePos = RIGHT;
			}
			else
				m_drawRect.left += moveWidth;
			break;
			//在矩形区域的右部
		case RIGHT:
			if (m_drawRect.right < m_drawRect.left)
			{

				temp = m_drawRect.left;
				m_drawRect.left = m_drawRect.right;
				m_drawRect.right = temp;
				m_mousePos = LEFT;
			}
			m_drawRect.right += moveWidth;
			break;
		default:
			break;
		}	
		//清屏
		clientDC.BitBlt(0, 0, nWidth, nHeight, &m_compatibleDC, 0, 0, SRCCOPY);
		//绘制矩形
		DrawRect(&clientDC, &m_drawRect, &m_brush);
		//重新设置原点
		m_oriPoint = point;	
	}
	ReleaseDC(&clientDC);
}

/*
	功能:
		在指定的设备描述表当中画矩形,以及在矩形四条边上的中心画小矩形框
	参数:
		pDC 指向设备描述表的句柄
		pRect 指向绘制师的矩形的区域
		pBrush 指向绘制时使用的画刷
	返回值:
		void 
*/
void CFullScreenWnd::DrawRect(CDC *pDC, RECT *pRect, CBrush *pBrush)
{
	//定义矩形的宽度,高度
	int width, height;
	//定义画矩形框需要的临时变量
	RECT rt;
	CBrush *oldBrush;
	CPen pen, *oldPen;
	//计算矩形的宽度和高度
	width = pRect->right - pRect->left;
	height = pRect->bottom - pRect->top;
	//画矩形框
	pDC->FrameRect(pRect, pBrush);
	
	//创建画笔
	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(pBrush);

	//在矩形的四条边的中心位置绘制4像素的矩形框
	//在矩形框的上条边的中心位置绘制矩形
	rt.left = pRect->left + width/2 - 2;
	rt.right = pRect->left + width/2 + 2;
	rt.top = pRect->top - 2;
	rt.bottom = pRect->top + 2;
	pDC->Rectangle(&rt);

	//在矩形框的下条边的中心位置绘制矩形
	rt.left = pRect->left + width/2 - 2;
	rt.right = pRect->left + width/2 + 2;
	rt.top = pRect->bottom - 2;
	rt.bottom = pRect->bottom + 2;
	pDC->Rectangle(&rt);

	//在矩形框的左条边的中心位置绘制矩形
	rt.left = pRect->left - 2;
	rt.right = pRect->left + 2;
	rt.top = pRect->top + height/2 - 2;
	rt.bottom = pRect->top + height/2 + 2;
	pDC->Rectangle(&rt);

	//在矩形框的右条边的中心位置绘制矩形
	rt.left = pRect->right - 2;
	rt.right = pRect->right + 2;
	rt.top = pRect->top + height/2 - 2;
	rt.bottom = pRect->top + height/2 + 2;
	pDC->Rectangle(&rt);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pen.DeleteObject();	
}

//保存屏幕矩形区域到一个文件
void CFullScreenWnd::Save()
{
	//得到矩形区域位图句柄
	HBITMAP hBitmap = CopyDCToBitmap(&m_drawRect, m_compatibleDC.m_hDC);

	if( MessageBox(_T("是否保存截屏为文件?"), _T("另存为选择"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		//打开文件对话框
		CFileDialog fd(FALSE, _T("png"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, NULL);
		fd.m_ofn.lpstrFilter = _T("PNG 文件\0*.png\0JPEG 文件\0*.jpg\0位图文件\0*.bmp\0All Files\0*.*\0\0");
		fd.DoModal();

		//获取文件路径
		CString strPathName = fd.GetPathName();
		if(strPathName.IsEmpty())
		{
			// 销毁hBitmap对象
			DeleteObject(hBitmap);

			//向窗口发送鼠标右键按下消息
			SendMessage(WM_RBUTTONUP, 0, 0);
			return;
		}

		//定义CImage对象,用来将位图保存到一个文件上
		CImage image;

		//将位图句柄与CImgae对象关联
		image.Attach(hBitmap);

		//保存到文件
		image.Save(strPathName);

		//销毁image对象
		image.Destroy();

		// 销毁hBitmap对象
		DeleteObject(hBitmap);

		//向窗口发送鼠标右键按下消息
		SendMessage(WM_RBUTTONUP, 0, 0);
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

		// 结束标识
		m_bClip = TRUE;
		if(IsExit)
		{
			// 发送消息给父窗口
			::SendMessage(this->GetParent()->GetSafeHwnd(), WM_COMMAND, 100, 0);
			this->DestroyWindow();
		}
		else
			//向窗口发送鼠标右键按下消息
			SendMessage(WM_RBUTTONUP, 0, 0);
	}
}

void CFullScreenWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//保存位图
	Save();
}
