// ImageButton.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageButton.h"


// CImageButton



CImageButton::CImageButton()
{

	m_pImagelist = NULL;
	m_bActive = FALSE;
	m_textColor = RGB(0, 0, 0);
	m_normalBkColor = RGB(255, 255, 255);
	m_activeBkColor = RGB(255, 255, 255);
	m_pressedBkColor = RGB(255, 255, 255);
}

CImageButton::~CImageButton()
{
}


BEGIN_MESSAGE_MAP(CImageButton, CButton)
	//{{AFX_MSG_MAP(ImageButton)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ImageButton message handlers

void CImageButton::SetImageIndex(UINT index)
{
	m_ImageIndex = index;
}

void CImageButton::SetImageList(CImageList *pImage)
{
	m_pImagelist = pImage;
}

void CImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CString text;
	CDC dc ;
	dc.Attach(lpDrawItemStruct->hDC);
	CRect clinetRect;
	GetClientRect(clinetRect);	
	dc.SetTextColor(RGB(0, 0, 255));
	CBrush brush;
	if (!m_bActive)
	{
		brush.CreateSolidBrush(m_normalBkColor);
		dc.FillRect(&clinetRect, &brush);	
	}
	else
	{
		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			brush.CreateSolidBrush(m_pressedBkColor);	
		}
		else
		{
			brush.CreateSolidBrush(m_activeBkColor);
		}
		CBrush brush1;
		brush1.CreateSolidBrush(RGB(0, 0, 255));
		dc.FillRect(&clinetRect, &brush);
		dc.FrameRect(&clinetRect, &brush1);
	
	}
	if (m_pImagelist)
	{	
			//获取图像列中图像大小
		IMAGEINFO imageinfo;
		m_pImagelist->GetImageInfo(m_ImageIndex,&imageinfo);
		CSize imagesize;
		imagesize.cx = imageinfo.rcImage.right - imageinfo.rcImage.left;
		imagesize.cy = imageinfo.rcImage.bottom - imageinfo.rcImage.top;
		//在按钮垂直方向居中显示位图
		CPoint point;
		point.x = 2;
		point.y = (clinetRect.Height() - imagesize.cy)/2;
		m_pImagelist->Draw(&dc, m_ImageIndex, point, ILD_NORMAL | ILD_TRANSPARENT);
		//绘制按钮文本
		GetWindowText(text);
		clinetRect.DeflateRect(point.x+imagesize.cx + 2, 0, 0, 0);
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(text,clinetRect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);	
	}
	else
	{

		GetWindowText(text);
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(text,clinetRect,DT_CENTER | DT_SINGLELINE | DT_VCENTER);	
	}
}
void CImageButton::SetTextColor(COLORREF color)
{
	m_textColor = color;
}
void CImageButton::SetNormalBkColor(COLORREF color)
{
	m_normalBkColor = color;
}
void CImageButton::SetActiveBkColor(COLORREF color)
{
	m_activeBkColor = color;
}
void CImageButton::SetPressedBkColor(COLORREF color)
{
	m_pressedBkColor = color;
}
void CImageButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CButton::OnMouseMove(nFlags, point);
	ClientToScreen(&point);
	HWND hwnd = ::WindowFromPoint(point);
	if (hwnd == this->m_hWnd)
	{	
		SetCapture();
		if (!m_bActive)
			Invalidate();
		m_bActive = TRUE;
	}
	else
	{
		ReleaseCapture();
		m_bActive = FALSE;
		Invalidate();
	}
}