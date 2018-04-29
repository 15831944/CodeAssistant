/* 
 * Kenny Liu
 * http://www.codeproject.com/Members/yonken
 *
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use or copy this program
 * for any purpose,  provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 */

#include "StdAfx.h"
#include "CustomDrawCommon.h"
#include "CustomDrawControl.h"

#include <afxmt.h>		// CMutex

#include <algorithm>	// find

#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
#include "afxtoolbarbutton.h"
#include "afxdrawmanager.h"
#include "afxvisualmanager.h"
#include "afxribbonbutton.h"
#include "afxribbonbar.h"
#endif // DERIVE_FROM_MFCTOOLTIPCTRL


/*----------------------------------------------------------------------------*/
/* CCustomDrawHeaderCtrl
/*----------------------------------------------------------------------------*/

IMPLEMENT_DYNCREATE(CCustomDrawHeaderCtrl, CCustomDrawHeaderCtrlBase)

CCustomDrawHeaderCtrl::CCustomDrawHeaderCtrl()
	: m_hFont(NULL)
	, m_bIsMousePressed(FALSE)
	, m_nHighlightedItem(-1)
	, m_bTracked(FALSE)
	, m_pTextDrawer(NULL)
{
	
}

CCustomDrawHeaderCtrl::~CCustomDrawHeaderCtrl()
{
	
}

BEGIN_MESSAGE_MAP(CCustomDrawHeaderCtrl, CCustomDrawHeaderCtrlBase)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()

void CCustomDrawHeaderCtrl::PreSubclassWindow()
{
	CCustomDrawHeaderCtrlBase::PreSubclassWindow();

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == NULL)
	{
		OnInitHeader();
	}
}

int CCustomDrawHeaderCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CCustomDrawHeaderCtrlBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	OnInitHeader();
	
	return 0;
}

BOOL CCustomDrawHeaderCtrl::IsDragDropEnabled() const
{
	return (HDS_DRAGDROP & GetStyle());
}

void CCustomDrawHeaderCtrl::EnableDragDrop( BOOL bEnable /*= TRUE*/ )
{
	DWORD dwStyle = (DWORD)::GetWindowLong(m_hWnd, GWL_STYLE);
	if (bEnable)
		dwStyle |= HDS_DRAGDROP;
	else
		dwStyle &= ~HDS_DRAGDROP;
	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
}

BOOL CCustomDrawHeaderCtrl::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;	// flicker free
}

void CCustomDrawHeaderCtrl::OnDrawItem( CDC* pDC, int iItem, CRect rect, BOOL bIsPressed, BOOL bIsHighlighted )
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	OnFillBackground(pDC, iItem, rect, bIsPressed, bIsHighlighted);

	// Draw border:
	OnDrawBorder(pDC, iItem, rect, bIsPressed, bIsHighlighted);

	if (iItem < 0)
	{
		return;
	}

	HD_ITEM hdItem;
	memset(&hdItem, 0, sizeof(hdItem));
	hdItem.mask = HDI_FORMAT | HDI_BITMAP | HDI_TEXT | HDI_IMAGE;

	TCHAR szText [256];
	hdItem.pszText = szText;
	hdItem.cchTextMax = 255;

	if (!GetItem(iItem, &hdItem))
	{
		return;
	}

	// Draw bitmap and image:
	OnDrawImage(pDC, hdItem, rect, bIsPressed, bIsHighlighted);

	OnDrawBitmap(pDC, hdItem, rect, bIsPressed, bIsHighlighted);

	// Draw text:
	OnDrawText(pDC, hdItem, rect, bIsPressed, bIsHighlighted);
}

void CCustomDrawHeaderCtrl::OnDrawImage( CDC* pDC, HD_ITEM& hdItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted )
{
	if ((hdItem.fmt & HDF_IMAGE) && hdItem.iImage >= 0)
	{
		// The column has a image from imagelist:
		CImageList* pImageList = GetImageList();
		if (pImageList != NULL)
		{
			int cx = 0;
			int cy = 0;
			
			VERIFY(::ImageList_GetIconSize(*pImageList, &cx, &cy));
			
			CPoint pt = rect.TopLeft();
			pt.x ++;
			pt.y = (rect.top + rect.bottom - cy) / 2;
			
			VERIFY(pImageList->Draw(pDC, hdItem.iImage, pt, ILD_NORMAL));
			
			rect.left += cx;
		}
	}
}

void CCustomDrawHeaderCtrl::OnDrawBitmap( CDC* pDC, HD_ITEM& hdItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted )
{
	if ((hdItem.fmt &(HDF_BITMAP | HDF_BITMAP_ON_RIGHT)) && hdItem.hbm != NULL)
	{
		CBitmap* pBmp = CBitmap::FromHandle(hdItem.hbm);
		ASSERT_VALID(pBmp);
		
		BITMAP bmp;
		pBmp->GetBitmap(&bmp);
		
		CRect rectBitmap = rect;
		if (hdItem.fmt & HDF_BITMAP_ON_RIGHT)
		{
			rectBitmap.right--;
			rect.right = rectBitmap.left = rectBitmap.right - bmp.bmWidth;
		}
		else
		{
			rectBitmap.left++;
			rect.left = rectBitmap.right = rectBitmap.left + bmp.bmWidth;
		}
		
		rectBitmap.top += max(0, (rectBitmap.Height() - bmp.bmHeight) / 2);
		rectBitmap.bottom = rectBitmap.top + bmp.bmHeight;
		
		pDC->DrawState(rectBitmap.TopLeft(), rectBitmap.Size(), pBmp, DSS_NORMAL);
	}
}

void CCustomDrawHeaderCtrl::OnDrawText( CDC* pDC, HD_ITEM& hdItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted )
{
	if ((hdItem.fmt & HDF_STRING) && hdItem.pszText != NULL)
	{
		CRect rectLabel = rect;
		const int nTextMargin = 5;
		rectLabel.DeflateRect(nTextMargin, 0);
		if (bIsPressed)
		{
			rectLabel.OffsetRect(1, 1);	// push-like
		}
		
		UINT uiTextFlags = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
		if (hdItem.fmt & HDF_CENTER)
		{
			uiTextFlags |= DT_CENTER;
		}
		else if (hdItem.fmt & HDF_RIGHT)
		{
			uiTextFlags |= DT_RIGHT;
		}
		
		CString strLabel = hdItem.pszText;
		if ( GetTextDrawer() )
			GetTextDrawer()->Draw(pDC, strLabel, rectLabel, uiTextFlags);
		else
			pDC->DrawText(strLabel, rectLabel, uiTextFlags);
	}
}

void CCustomDrawHeaderCtrl::OnFillBackground( CDC* pDC )
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	
	CRect rectClient;
	GetClientRect(rectClient);
	
#ifdef CUSTOMDRAW_GRADIENT
	pDC->FillSolidRect(rectClient, ::GetSysColor(COLOR_WINDOW));
#else
	pDC->FillSolidRect(rectClient, ::GetSysColor(COLOR_BTNFACE));
#endif // CUSTOMDRAW_GRADIENT
}

void CCustomDrawHeaderCtrl::OnFillBackground( CDC* pDC, int iItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted )
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

#ifdef CUSTOMDRAW_GRADIENT
	CCustomDrawHelper drawer;
	drawer.m_bWndEnabled		= !!IsWindowEnabled();
	drawer.m_bWndHasFocus		= ::GetFocus() == GetSafeHwnd();
	drawer.m_bFocusItem			= false;
	drawer.m_bIsHotItem			= bIsHighlighted ? true : false;
	drawer.m_bIsPressed			= bIsPressed ? true : false;
	drawer.m_hWnd				= m_hWnd;
	drawer.DrawThemeBackground(pDC, rect, CDTBT_HEADER);
#endif // CUSTOMDRAW_GRADIENT
}

void CCustomDrawHeaderCtrl::OnDrawBorder( CDC* pDC, int iItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted )
{
	ASSERT_VALID(pDC);
#ifdef CUSTOMDRAW_GRADIENT
	COLORREF clrBorder;
	if (bIsPressed || bIsHighlighted)
	{
		if (bIsPressed)
		{
			clrBorder = RGB(112,185,223);
		}
		else
		{
			clrBorder = RGB(147,201,227);
		}
	}
	else
	{
		clrBorder = RGB(213,213,213);
	}
	CBrush brBorder(clrBorder);
	pDC->FrameRect(rect, &brBorder);
#else
	COLORREF clrHilite	= GetSysColor(COLOR_3DHILIGHT);
	COLORREF clrShadow	= GetSysColor(COLOR_3DSHADOW);
	if (bIsPressed)
	{
		pDC->Draw3dRect(rect, clrShadow, clrShadow);
		
		rect.left++;
		rect.top++;
	}
	else
	{
		pDC->Draw3dRect(rect, clrHilite, clrShadow);
	}
#endif // CUSTOMDRAW_GRADIENT
}

void CCustomDrawHeaderCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CSimpleMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();
	
	CRect rectClip;
	dc.GetClipBox(rectClip);
	
	CRect rectClient;
	GetClientRect(rectClient);
	
	OnFillBackground(pDC);
	
	CFont* pOldFont = SelectFont(pDC);
	ASSERT_VALID(pOldFont);
	
	pDC->SetTextColor(RGB(0x4C,0x4C,0x4C));
	pDC->SetBkMode(TRANSPARENT);
	
	CRect rect;
	GetClientRect(rect);
	
	CRect rectItem;
	int nCount = GetItemCount();
	
	int xMax = 0;
	
	for (int i = 0; i < nCount; i++)
	{
		// Is item pressed?
		CPoint ptCursor;
		::GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);
		
		HDHITTESTINFO hdHitTestInfo;
		hdHitTestInfo.pt = ptCursor;
		
		int iHit = (int) SendMessage(HDM_HITTEST, 0, (LPARAM) &hdHitTestInfo);
		
		BOOL bIsHighlighted = iHit == i &&(hdHitTestInfo.flags & HHT_ONHEADER);
		BOOL bIsPressed = m_bIsMousePressed && bIsHighlighted;
		
		GetItemRect(i, rectItem);
		
		CRgn rgnClip;
		rgnClip.CreateRectRgnIndirect(&rectItem);
		pDC->SelectClipRgn(&rgnClip);
		
		// Draw item:
		OnDrawItem(pDC, i, rectItem, bIsPressed, m_nHighlightedItem == i);
		
		pDC->SelectClipRgn(NULL);
		
		xMax = max(xMax, rectItem.right);
	}
	
	// Draw "tail border":
	if (nCount == 0)
	{
		rectItem = rect;
		rectItem.right++;
	}
	else
	{
		rectItem.left = xMax;
		rectItem.right = rect.right + 1;
	}
	
	OnDrawItem(pDC, -1, rectItem, FALSE, FALSE);
	
	pDC->SelectObject(pOldFont);
}

void CCustomDrawHeaderCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_bIsMousePressed = TRUE;
	CCustomDrawHeaderCtrlBase::OnLButtonDown(nFlags, point);
}

void CCustomDrawHeaderCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bIsMousePressed = FALSE;
	CCustomDrawHeaderCtrlBase::OnLButtonUp(nFlags, point);
}

void CCustomDrawHeaderCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	if ((nFlags & MK_LBUTTON) == 0)
	{
		HDHITTESTINFO hdHitTestInfo;
		hdHitTestInfo.pt = point;
		
		int nPrevHighlightedItem = m_nHighlightedItem;
		m_nHighlightedItem = (int) SendMessage(HDM_HITTEST, 0, (LPARAM) &hdHitTestInfo);
		
		if ((hdHitTestInfo.flags & HHT_ONHEADER) == 0)
		{
			m_nHighlightedItem = -1;
		}
		
		if (!m_bTracked)
		{
			m_bTracked = TRUE;
			
			TRACKMOUSEEVENT trackmouseevent;
			trackmouseevent.cbSize		= sizeof(trackmouseevent);
			trackmouseevent.dwFlags		= TME_LEAVE;
			trackmouseevent.hwndTrack	= GetSafeHwnd();
			trackmouseevent.dwHoverTime	= HOVER_DEFAULT;
			//::AFXTrackMouse(&trackmouseevent);
			_TrackMouseEvent(&trackmouseevent);
		}
		
		if (nPrevHighlightedItem != m_nHighlightedItem)
		{
			RedrawWindow();
		}
	}
	
	CCustomDrawHeaderCtrlBase::OnMouseMove(nFlags, point);
}

void CCustomDrawHeaderCtrl::OnCancelMode()
{
	CCustomDrawHeaderCtrlBase::OnCancelMode();
	
	if (m_nHighlightedItem >= 0)
	{
		m_nHighlightedItem = -1;
		Invalidate(FALSE);
	}
}

LRESULT CCustomDrawHeaderCtrl::OnMouseLeave( WPARAM,LPARAM )
{
	m_bTracked = FALSE;
	
	if (m_nHighlightedItem >= 0)
	{
		m_nHighlightedItem = -1;
		Invalidate(FALSE);
	}
	
	return 0;
}

LRESULT CCustomDrawHeaderCtrl::OnSetFont( WPARAM wParam, LPARAM lParam )
{
	BOOL bRedraw = (BOOL) LOWORD(lParam);
	
	m_hFont = (HFONT) wParam;
	
	if (bRedraw)
	{
		RedrawWindow();
		UpdateWindow();
	}
	
	return 0;
}

CFont* CCustomDrawHeaderCtrl::SelectFont( CDC *pDC )
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	
	CFont* pOldFont = NULL;
	
	if (m_hFont != NULL)
	{
		pOldFont = pDC->SelectObject(CFont::FromHandle(m_hFont));
	}
	else
	{
		pOldFont = (CFont*) pDC->SelectStockObject(DEFAULT_GUI_FONT);
	}
	
	return pOldFont;
}


/*----------------------------------------------------------------------------*/
/* CSortHeaderCtrl
/*----------------------------------------------------------------------------*/
//IMPLEMENT_DYNCREATE(CSortHeaderCtrl, CSortHeaderCtrlBase)
//
//CSortHeaderCtrl::CSortHeaderCtrl()
//	: m_nSortColumn( -1 )
//	, m_bSortable(true)
//{
//}
//
//CSortHeaderCtrl::~CSortHeaderCtrl()
//{
//}
//
//
//BEGIN_MESSAGE_MAP(CSortHeaderCtrl, CSortHeaderCtrlBase)
//END_MESSAGE_MAP()
//
//
//void CSortHeaderCtrl::SetSortArrow( const int iSortColumn, const BOOL bSortAscending )
//{
//	m_iSortColumn = iSortColumn;
//	m_bSortAscending = bSortAscending;
//
//	// change the item to owner drawn.
//	HD_ITEM hditem;
//
//	hditem.mask = HDI_FORMAT;
//	VERIFY( GetItem( iSortColumn, &hditem ) );
//	hditem.fmt |= HDF_OWNERDRAW;
//	VERIFY( SetItem( iSortColumn, &hditem ) );
//
//	// invalidate the header control so it gets redrawn
//	//Invalidate();
//}
//
//BOOL CSortHeaderCtrl::Init( HWND hWndToSubClass )
//{
//	const BOOL bStatus = CSortHeaderCtrlBase::SubclassWindow( hWndToSubClass );
//	return bStatus;
//}
//
//void CSortHeaderCtrl::SwitchSortItem( int nSortItem )
//{
//	if ( m_nSortColumn == nSortItem )
//	{
//		m_bAscending = !m_bAscending;
//	}
//	else
//	{
//		m_nSortColumn = nSortItem;
//		m_bAscending = DEFAULT_SORT_ASCENDING;
//	}
//}
//
//void CSortHeaderCtrl::OnFillBackground( CDC* pDC, int iItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted )
//{
//	CSortHeaderCtrlBase::OnFillBackground(pDC, iItem, rect, bIsPressed, bIsHighlighted);
//	if ( m_bSortable && iItem >= 0 && iItem == m_nSortColumn )
//	{
//		if ( !bIsPressed && !bIsHighlighted )
//		{
//#ifdef CUSTOMDRAW_GRADIENT
//			Fill4ColorsGradient(pDC->GetSafeHdc(), rect, 
//					RGB_HEADER_SORT_FILL_COLOR1, RGB_HEADER_SORT_FILL_COLOR2, RGB_HEADER_SORT_FILL_COLOR3, RGB_HEADER_SORT_FILL_COLOR4, 
//					TRUE, HEADER_FIRST_HALF_PERCENTAGE);
//#else
//			pDC->FillSolidRect(rect, RGB(216,236,246));
//#endif // CUSTOMDRAW_GRADIENT
//		}
//
//		CPoint ptCenter		= rect.CenterPoint();
//		CRect rectArrow;
//		rectArrow.left		= ptCenter.x - 3;
//		rectArrow.right		= ptCenter.x + 3;
//		rectArrow.top		= rect.top + 2;
//		rectArrow.bottom	= rectArrow.top + 3;
//		DrawSortArrow(pDC, rectArrow, m_bAscending);
//	}
//}
//
//void CSortHeaderCtrl::OnDrawBorder( CDC* pDC, int iItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted )
//{
//	if ( m_bSortable && iItem >= 0 && iItem == m_nSortColumn )
//	{
//		COLORREF clrBorder = RGB(150,217,249);
//		CBrush brBorder(clrBorder);
//		pDC->FrameRect(rect, &brBorder);
//	}
//	else
//		CSortHeaderCtrlBase::OnDrawBorder(pDC, iItem, rect, bIsPressed, bIsHighlighted);
//}
//
//void CSortHeaderCtrl::SetSortable( bool bSortable /*= true*/, bool bRedraw /*= true*/ )
//{
//	m_bSortable = bSortable;
//	if (bRedraw)
//	{
//		Invalidate(FALSE);
//	}
//}

/*----------------------------------------------------------------------------*/
/* CCustomDrawToolTipCtrl
/*----------------------------------------------------------------------------*/

#ifndef TTS_BALLOON
	#define TTS_BALLOON 0x40
#endif

#ifndef TTM_SETTITLE
	#define TTM_SETTITLE (WM_USER + 32)
#endif

IMPLEMENT_DYNCREATE(CCustomDrawToolTipCtrl, CCDToolTipCtrlBase)

CCustomDrawToolTipCtrl::CCustomDrawToolTipCtrl(CCustomDrawToolTipInfo* pParams /*= NULL*/)
	: m_DisplayIDWParam(-1)
	, m_DisplayIDLParam(-1)
#ifndef MSVC_NEW_VER
	, m_hFont(NULL)
#endif // MSVC_NEW_VER
	, m_pTextDrawer(NULL)
{
	SetParams(pParams);
#ifndef DERIVE_FROM_MFCTOOLTIPCTRL
	m_ptMargin					= CPoint(0, 0);
	m_sizeImage					= CSize(0, 0);
	m_ptMargin					= CPoint(0, 0);
	m_ptLocation				= CPoint(-1, -1);
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
	m_sizeDescrImage			= CSize(0, 0);
	m_sizeSupplementalImage			= CSize(0, 0);
	m_nTextTabSize				= 4;
	m_nLabelHeight				= 0;

	m_hLabelIcon				= NULL;
	m_hDescrIcon				= NULL;
	m_hSupplementalBmp			= NULL;
}

CCustomDrawToolTipCtrl::~CCustomDrawToolTipCtrl()
{
	
}

BEGIN_MESSAGE_MAP(CCustomDrawToolTipCtrl, CCDToolTipCtrlBase)
#ifndef DERIVE_FROM_MFCTOOLTIPCTRL
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TTN_SHOW, OnShow)
	ON_NOTIFY_REFLECT(TTN_POP, OnPop)
#ifndef MSVC_NEW_VER
	ON_MESSAGE(WM_SETFONT, OnSetFont)
#endif // MSVC_NEW_VER
END_MESSAGE_MAP()

#define TIPTEXTBUFFER_SIZE	80

void CCustomDrawToolTipCtrl::SetText( NMHDR* pNMHDR, LPCTSTR lpcszText )
{
	// the szText can not exceed 80 characters.
	// See http://msdn.microsoft.com/en-us/library/bb760252(VS.85).aspx#tooltip_sample_multiline
	
	// handle both ANSI and UNICODE versions of the message
	LPTOOLTIPTEXTA pTTTA = reinterpret_cast<LPTOOLTIPTEXTA>(pNMHDR);
	LPTOOLTIPTEXTW pTTTW = reinterpret_cast<LPTOOLTIPTEXTW>(pNMHDR);

	size_t nStrLen = _tcslen(lpcszText);
	if ( nStrLen >= TIPTEXTBUFFER_SIZE )
	{
		// the 
#ifdef _UNICODE
		// Unicode environment
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			m_astrLongTipText = toastring(lpcszText);
			pTTTA->lpszText = (PSTR)m_astrLongTipText.c_str();
		}
		else if(pNMHDR->code == TTN_NEEDTEXTW)
		{
			m_wstrLongTipText = lpcszText;
			pTTTW->lpszText = (PWSTR)m_wstrLongTipText.c_str();
		}
#else
		// Ansi environment
		if (pNMHDR->code == TTN_NEEDTEXTA)
		{
			m_astrLongTipText = lpcszText;
			pTTTA->lpszText = (PSTR)m_astrLongTipText.c_str();
		}
		else if(pNMHDR->code == TTN_NEEDTEXTW)
		{
			m_wstrLongTipText = towstring(lpcszText);
			pTTTW->lpszText = (PWSTR)m_wstrLongTipText.c_str();
		}
#endif
	}
	else
	{
		// otherwise, just do the conversion & copy
#ifdef _UNICODE
		// Unicode environment
		if (pNMHDR->code == TTN_NEEDTEXTA)
			_wcstombsz(pTTTA->szText, lpcszText, TIPTEXTBUFFER_SIZE);
		else
			lstrcpyn(pTTTW->szText, lpcszText, TIPTEXTBUFFER_SIZE);
#else
		// Ansi environment
		if (pNMHDR->code == TTN_NEEDTEXTA)
			lstrcpyn(pTTTA->szText, lpcszText, TIPTEXTBUFFER_SIZE);
		else if(pNMHDR->code == TTN_NEEDTEXTW)
		{
		#ifdef MSVC_NEW_VER
			mbstowcs_s(NULL, pTTTW->szText, TIPTEXTBUFFER_SIZE, lpcszText, _TRUNCATE);
		#else
			mbstowcs(pTTTW->szText, lpcszText, TIPTEXTBUFFER_SIZE);
		#endif // MSVC_NEW_VER
		}
#endif
	}
}

void CCustomDrawToolTipCtrl::SetParams( CCustomDrawToolTipInfo* pParams )
{
	ASSERT_VALID(this);
	
	if (pParams == NULL)
	{
		m_Params = CCustomDrawToolTipInfo();
	}
	else
	{
		m_Params = *pParams;
	}

#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	CCDToolTipCtrlBase::m_Params = m_Params;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
}

BOOL CCustomDrawToolTipCtrl::CheckDisplayToolTip( WPARAM wParam, LPARAM lParam /*= 0*/ )
{
	if ( m_DisplayIDWParam != wParam || m_DisplayIDLParam != lParam )
	{
		m_DisplayIDWParam = wParam;
		m_DisplayIDLParam = lParam;
		return TRUE;
	}
	return FALSE;
}

void CCustomDrawToolTipCtrl::SetFixedWidth( int nWidthRegular, int nWidthLargeImage )
{
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	CCDToolTipCtrlBase::SetFixedWidth(nWidthRegular, nWidthLargeImage);
#else
	m_nFixedWidthRegular	= nWidthRegular;
	m_nFixedWidthWithImage	= nWidthLargeImage;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
}

int CCustomDrawToolTipCtrl::GetFixedWidth()
{
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	return CCDToolTipCtrlBase::GetFixedWidth();
#else
	ASSERT_VALID(this);
	#ifdef MSVC_NEW_VER
	if (m_sizeImage.cx <= (int)(afxGlobalData.GetRibbonImageScale() * 32))
	{
		return m_nFixedWidthRegular;
	}
	else
	{
		return m_nFixedWidthWithImage;
	}
	#else
	return 0;
	#endif // MSVC_NEW_VER
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
}

int CCustomDrawToolTipCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	return CCDToolTipCtrlBase::OnCreate(lpCreateStruct);
#else
	if (CCDToolTipCtrlBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyle(WS_BORDER, 0);

	if (m_Params.m_bBalloonTooltip)
	{
		ModifyStyle(0, TTS_BALLOON);
	}
	
	return 0;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
}

BOOL CCustomDrawToolTipCtrl::OnEraseBkgnd( CDC* pDC )
{
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	return CCDToolTipCtrlBase::OnEraseBkgnd(pDC);
#else
	CRect rect;
    GetClientRect (rect);
	
    COLORREF clrDummy;
    OnFillBackground(pDC, rect, clrDummy, clrDummy);
	
	return TRUE;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
}

void CCustomDrawToolTipCtrl::OnShow( NMHDR* pNMHDR, LRESULT* pResult )
{
	*pResult = 0;

	CWnd* pOwnerWnd = GetOwner();
	if ( pOwnerWnd )
	{
		pOwnerWnd->SendMessage(WM_CDTOOLTIPCTRL_NOTIFY, CDTOOLTIP_ONBEFORE_SHOW);
	}

	if (m_Params.m_bVislManagerTheme)
	{
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
		CMFCVisualManager::GetInstance()->GetToolTipInfo(m_Params);
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
		m_Params.m_bVislManagerTheme = TRUE;
	}

	if (m_Params.m_bBalloonTooltip)
	{
		return;
	}

	CPoint ptCursor;
	::GetCursorPos(&ptCursor);

#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	GetHotButton();
#endif // DERIVE_FROM_MFCTOOLTIPCTRL

	m_sizeImage			= m_Params.m_bDrawIcon ? GetIconSize() : CSize(0, 0);
	m_ptMargin			= m_Params.m_bRoundedCorners ? CPoint(6, 4) : CPoint(4, 2);
	m_sizeDescrImage	= m_Params.m_bDrawDescrIcon ? GetDescrIconSize() : CSize(0, 0);
	m_sizeSupplementalImage	= m_Params.m_bDrawSupplementalImage ? GetSupplementalImageSize() : CSize(0, 0);

	CRect rectMargin;
	GetMargin(rectMargin);

	CRect rectDraw;
	GetClientRect(rectDraw);

	CRect rectText(rectDraw);

	CClientDC dc(this);
	CSize sizeText = OnDrawLabel(&dc, rectText, TRUE);

	int cx = sizeText.cx;
	int cy = sizeText.cy;

	CSize sizeDescr(0, 0);

	m_nLabelHeight = cy;
	if (m_sizeImage.cy > 0 && m_Params.m_bDrawIcon)
	{
		m_nLabelHeight = max(cy, m_sizeImage.cy);
	}

	cy = m_nLabelHeight;

	if ( m_Params.m_bDrawDescription && !m_strDescription.IsEmpty() )
	{
		sizeDescr = OnDrawDescription(&dc, rectText, TRUE);

		if (m_sizeDescrImage != CSize(0, 0) && m_Params.m_bDrawDescrIcon )
		{
			sizeDescr.cx += m_sizeDescrImage.cx + m_ptMargin.x;
			sizeDescr.cy = max(sizeDescr.cy, m_sizeDescrImage.cy);
		}

		if ( sizeDescr.cy <= 0)
			cy += 2 * m_ptMargin.y;
		else
		{
			cy += sizeDescr.cy + 4 * m_ptMargin.y;
			cx = max(cx, sizeDescr.cx);
		}
	}

	if (m_sizeImage.cx > 0 && m_Params.m_bDrawIcon)
	{
		cx += m_sizeImage.cx + m_ptMargin.x;
	}

	CSize sizeBody = OnDrawBody(&dc, rectDraw, TRUE);
	if (sizeBody != CSize(0, 0))
	{
		cx = max(cx, sizeBody.cx + 2 * m_ptMargin.x);
		cy += sizeBody.cy + m_ptMargin.y;
	}

	cx += 2 * m_ptMargin.x;
	cy += 2 * m_ptMargin.y;

	const int nFixedWidth = GetFixedWidth();
	if (nFixedWidth > 0 && sizeDescr != CSize(0, 0))
	{
		cx = max(cx, nFixedWidth);
	}

	CRect rectWindow;
	GetWindowRect(rectWindow);

	int x = rectWindow.left;
	int y = rectWindow.top;

	if (m_ptLocation != CPoint(-1, -1))
	{
		x = m_ptLocation.x;
		y = m_ptLocation.y;

		*pResult = 1;
	}

	CRect rectScreen;

	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	if (GetMonitorInfo(MonitorFromPoint(rectWindow.TopLeft(), MONITOR_DEFAULTTONEAREST), &mi))
	{
		rectScreen = mi.rcWork;
	}
	else
	{
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0);
	}

	int nBottom = max(ptCursor.y + cy + ::GetSystemMetrics(SM_CYCURSOR), y + cy + 2);
	if (nBottom > rectScreen.bottom)
	{
		y = ptCursor.y - cy - 1;
		
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
		if (m_pRibbonButton != NULL && m_ptLocation != CPoint(-1, -1))
		{
			ASSERT_VALID(m_pRibbonButton);

			CMFCRibbonBar* pRibbon = m_pRibbonButton->GetTopLevelRibbonBar();
			if (pRibbon->GetSafeHwnd() != NULL)
			{
				CRect rectRibbon;
				pRibbon->GetWindowRect(rectRibbon);

				y = rectRibbon.top - cy;
			}
		}
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
		*pResult = 1;
	}

	if (x + cx + 2 > rectScreen.right)
	{
		if ((*pResult) == 1) // Y has been changed
		{
			x = ptCursor.x - cx - 1;
		}
		else
		{
			x = rectScreen.right - cx - 1;
			*pResult = 1;
		}
	}

// 	const CWnd* pWndInsertAfter = NULL;
// 	UINT nFlags = SWP_NOZORDER;
	const CWnd* pWndInsertAfter = &wndTopMost;
	UINT nFlags = 0;
	if ((*pResult) == 1)
	{
		SetWindowPos(pWndInsertAfter, x, y, cx, cy, nFlags | SWP_NOACTIVATE);
	}
	else
	{
		SetWindowPos(pWndInsertAfter, -1, -1, cx, cy, nFlags | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	if (m_Params.m_bRoundedCorners)
	{
		CRgn rgn;
		rgn.CreateRoundRectRgn(0, 0, cx + 1, cy + 1, 4, 4);

		SetWindowRgn(rgn, FALSE);
	}
}

void CCustomDrawToolTipCtrl::OnPop( NMHDR* pNMHDR, LRESULT* pResult )
{
	CWnd* pOwnerWnd = GetOwner();
	if ( pOwnerWnd )
	{
		pOwnerWnd->SendMessage(WM_CDTOOLTIPCTRL_NOTIFY, CDTOOLTIP_ONBEFORE_POP);
	}

	if ( m_hLabelIcon )
	{
		::DestroyIcon(m_hLabelIcon);
		m_hLabelIcon		= NULL;
	}
	if ( m_hDescrIcon )
	{
		::DestroyIcon(m_hDescrIcon);
		m_hDescrIcon		= NULL;
	}
	if ( m_hSupplementalBmp )
	{
		::DeleteObject(m_hSupplementalBmp);
		m_hSupplementalBmp	= NULL;
	}

	m_strLabel.Empty();
	m_strSupplementalDescription.Empty();

#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	CCDToolTipCtrlBase::OnPop(pNMHDR, pResult);
#else
	m_strDescription.Empty();
	m_ptLocation	= CPoint(-1, -1);
	*pResult = 0;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
}

void CCustomDrawToolTipCtrl::OnPaint()
{
	if (m_Params.m_bBalloonTooltip)
	{
		CToolTipCtrl::OnPaint();
		return;
	}
	
	CPaintDC dcPaint(this); // device context for painting
	
	CSimpleMemDC memDC(dcPaint, this);
	CDC* pDC = &memDC.GetDC();
	
	CRect rect;
	GetClientRect(rect);
	
	CRect rectMargin;
	GetMargin(rectMargin);

	CRect rectDraw(rect);
	
	rectDraw.DeflateRect(rectMargin);
	rectDraw.DeflateRect(m_ptMargin.x, m_ptMargin.y);

	CRect rectText(rectDraw);
	
	COLORREF clrLine = m_Params.m_clrBorder == (COLORREF)-1 ? ::GetSysColor(COLOR_INFOTEXT) : m_Params.m_clrBorder;
	COLORREF clrText = m_Params.m_clrText == (COLORREF)-1 ? ::GetSysColor(COLOR_INFOTEXT) : m_Params.m_clrText;
	
	// Fill background:
	OnFillBackground(pDC, rect, clrText, clrLine);
	
	CPen penLine(PS_SOLID, 1, clrLine);
	CPen* pOldPen = pDC->SelectObject(&penLine);
	
	// Draw border:
	OnDrawBorder(pDC, rect, clrLine);

	int nIconHeight = 0;
	
	// Draw icon:
	if (m_sizeImage != CSize(0, 0) && m_Params.m_bDrawIcon)
	{
		CRect rectImage = rectText;
		rectImage.right = rectImage.left + m_sizeImage.cx;
		rectImage.bottom = rectImage.top + m_sizeImage.cy;
		
		OnDrawIcon(pDC, rectImage);
		
		rectText.left += m_sizeImage.cx + m_ptMargin.x;

		nIconHeight = m_sizeImage.cy;
	}
	
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(clrText);
	
	// Draw label:
	int nLabelHeight = OnDrawLabel(pDC, rectText, FALSE).cy;

	// Draw body:
	rectDraw.top += max(nLabelHeight, nIconHeight);
	CSize sizeBody = OnDrawBody(pDC, rectDraw, FALSE);
	
	// Draw separator + description:
	if (!m_strDescription.IsEmpty() && m_Params.m_bDrawDescription)
	{
		CRect rectDescr = rectDraw;
		if (sizeBody != CSize(0, 0))
		{
			rectDescr.top += m_ptMargin.y;
		}
		rectDescr.top += sizeBody.cy + 3 * m_ptMargin.y / 2;
		
		if (m_Params.m_bDrawSeparator)
		{
			OnDrawSeparator(pDC, rectDescr.left, rectDescr.right, rectDescr.top - m_ptMargin.y / 2);
		}
		
		rectDescr.top += m_ptMargin.y;

		if (m_sizeDescrImage != CSize(0, 0) && m_Params.m_bDrawDescrIcon )
		{
			CRect rectImage = rectDescr;
			rectImage.right = rectImage.left + m_sizeDescrImage.cx;
			rectImage.bottom = rectImage.top + m_sizeDescrImage.cy;
			OnDrawDescriptionIcon(pDC, rectImage);
			rectDescr.left += m_sizeDescrImage.cx + m_ptMargin.x;
		}
		OnDrawDescription(pDC, rectDescr, FALSE);
	}
	
	pDC->SelectObject(pOldPen);
}

void CCustomDrawToolTipCtrl::OnFillBackground( CDC* pDC, CRect rect, COLORREF& clrText, COLORREF& clrLine )
{
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	CCDToolTipCtrlBase::OnFillBackground(pDC, rect, clrText, clrLine);
#else
	if (m_Params.m_clrFill == (COLORREF)-1)
	{
		::FillRect(pDC->GetSafeHdc(), rect, ::GetSysColorBrush(COLOR_INFOBK));
	}
	else
	{
		if (m_Params.m_clrFillGradient == (COLORREF)-1)
		{
			CBrush br(m_Params.m_clrFill);
			pDC->FillRect(rect, &br);
		}
		else
		{
#ifdef MSVC_NEW_VER
			CDrawingManager dm(*pDC);	
			dm.FillGradient2(rect, m_Params.m_clrFillGradient, m_Params.m_clrFill, m_Params.m_nGradientAngle == -1 ? 90 : m_Params.m_nGradientAngle);
#else
	#ifdef CUSTOMDRAW_GRADIENT
			//int nGradientAngle = m_Params.m_nGradientAngle == -1 ? 90 : m_Params.m_nGradientAngle;
			FillGradient(pDC->GetSafeHdc(), rect, m_Params.m_clrFill, m_Params.m_clrFillGradient);
	#else
			pDC->FillSolidRect(rect, m_Params.m_clrFillGradient);
	#endif // CUSTOMDRAW_GRADIENT
#endif // MSVC_NEW_VER
		}
	}
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
}

CSize CCustomDrawToolTipCtrl::DrawText( CDC *pDC, 
									   const CString& strText, 
									   CRect rect, 
									   BOOL bCalcOnly, 
									   BOOL bBold /*= FALSE*/,
									   BOOL bVertCenter /*= FALSE*/,
									   BOOL bWordBreak /*= FALSE*/
									   )
{
	ASSERT_VALID(pDC);
	
	CSize sizeText(0, 0);
	
	DRAWTEXTPARAMS dtp = {sizeof(DRAWTEXTPARAMS)};
	dtp.iTabLength = m_nTextTabSize;
	
	CFont* pOldFont = SelectFont(pDC, bBold);

	UINT nFormat = DT_NOCLIP | DT_NOPREFIX | DT_TABSTOP | DT_EXPANDTABS;

	if (bWordBreak)
	{
		nFormat |= DT_WORDBREAK;
	}
	else if (strText.Find(_T('\n')) < 0) // signle line text
	{
		nFormat = DT_SINGLELINE;
		if (bVertCenter)
		{
			nFormat |= DT_VCENTER;
		}
	}
	if (bCalcOnly)
	{
		nFormat |= DT_CALCRECT;
	}

	if ( GetTextDrawer() )
		sizeText.cy = GetTextDrawer()->Draw(pDC, strText, rect, nFormat);
	else
		sizeText.cy = DrawTextEx(pDC->GetSafeHdc(), (LPTSTR)(LPCTSTR)strText, strText.GetLength(), rect, nFormat, &dtp);
	sizeText.cx = rect.Width();
	
	pDC->SelectObject(pOldFont);
	
	return sizeText;
}

CSize CCustomDrawToolTipCtrl::OnDrawLabel( CDC* pDC, CRect rect, BOOL bCalcOnly )
{
	ASSERT_VALID(pDC);
	
	CString strText(m_strLabel);
	if (strText.IsEmpty())
	{
		GetWindowText(strText);
	}
	
	BOOL bDrawDescr				= m_Params.m_bDrawDescription && !m_strDescription.IsEmpty();
	BOOL bDrawSupplementalDescr	= m_Params.m_bDrawSupplementalDescription && !m_strSupplementalDescription.IsEmpty();
	BOOL bDrawSupplementalImage	= m_Params.m_bDrawSupplementalImage && m_sizeSupplementalImage != CSize(0, 0);
	BOOL bBoldText				= m_Params.m_bBoldLabel && (bDrawDescr || bDrawSupplementalDescr || bDrawSupplementalImage);
	if (!bCalcOnly)
	{
		rect.bottom = rect.top + m_nLabelHeight;
	}
	return DrawText(pDC, strText, rect, bCalcOnly, bBoldText, TRUE);
}

void CCustomDrawToolTipCtrl::OnDrawBorder( CDC* pDC, CRect rect, COLORREF clrLine )
{
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	CCDToolTipCtrlBase::OnDrawBorder(pDC, rect, clrLine);
#else
	if (m_Params.m_bRoundedCorners)
	{
		DrawSimpleRoundRectBorder(pDC, rect, clrLine);
	}
	else
	{
		pDC->Draw3dRect(rect, clrLine, clrLine);
	}
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
}

CSize CCustomDrawToolTipCtrl::OnDrawBody( CDC* pDC, CRect rect, BOOL bCalcOnly )
{
	CRect rectDraw(rect);
	rectDraw.DeflateRect(m_ptMargin.x, m_ptMargin.y);

	CSize sizeBody(0, 0);

	if (m_sizeSupplementalImage != CSize(0, 0) && m_Params.m_bDrawSupplementalImage)
	{
		if (!bCalcOnly)
		{
			CRect rectImage = rectDraw;
			rectImage.right = rectImage.left + m_sizeSupplementalImage.cx;
			rectImage.bottom = rectImage.top + m_sizeSupplementalImage.cy;
			
			OnDrawSupplementalImage(pDC, rectImage);
		}
		sizeBody = m_sizeSupplementalImage;
	}

	// Draw separator + supplemental description:
	if (!m_strSupplementalDescription.IsEmpty() && m_Params.m_bDrawSupplementalDescription)
	{
		CRect rectDescr = rectDraw;
		int cx = 0;
		if (sizeBody.cx > 0)
		{
			cx = m_ptMargin.x;
			rectDescr.left += sizeBody.cx + cx;
		}
		
		CSize sizeDescr = OnDrawSupplementalDescription(pDC, rectDescr, bCalcOnly);
		sizeBody.cx += sizeDescr.cx + cx;
		sizeBody.cy = max(sizeBody.cy, sizeDescr.cy);
	}

	return sizeBody;
}

CSize CCustomDrawToolTipCtrl::GetDescrIconSize()
{
	if (m_hDescrIcon)
	{
		CSize sizeIcon = ::GetIconSize(m_hDescrIcon);
		return sizeIcon;
	}
	return CSize(0, 0);
}

CSize CCustomDrawToolTipCtrl::GetSupplementalImageSize()
{
	if (m_hSupplementalBmp)
	{
		CSize sizeIcon = ::GetBmpSize(m_hSupplementalBmp);
		return sizeIcon;
	}
	return CSize(0, 0);
}

BOOL CCustomDrawToolTipCtrl::OnDrawSupplementalImage( CDC* pDC, CRect rect )
{
	if (m_hSupplementalBmp)
	{
		CBitmap* pBmp = CBitmap::FromHandle(m_hSupplementalBmp);
		if ( pBmp )
		{
			CMemBitmap bmpDrawer(pDC, pBmp);
			bmpDrawer.BeginDraw(m_sizeSupplementalImage.cx, m_sizeSupplementalImage.cy, FALSE);
			pDC->BitBlt(rect.left, rect.top, m_sizeSupplementalImage.cx, m_sizeSupplementalImage.cy, &bmpDrawer.GetDC(), 0, 0, SRCCOPY);
			return TRUE;
		}
	}
	return FALSE;
}

CSize CCustomDrawToolTipCtrl::OnDrawSupplementalDescription( CDC* pDC, CRect rect, BOOL bCalcOnly )
{
	ASSERT_VALID(pDC);
	BOOL bSingleLineText = m_strSupplementalDescription.Find(_T('\n')) < 0;
	if ( bSingleLineText )
	{
		rect.right = rect.left + m_Params.m_nMaxDescrWidth;
	}
	return DrawText(pDC, m_strSupplementalDescription, rect, bCalcOnly, FALSE, FALSE, bSingleLineText);
}

void CCustomDrawToolTipCtrl::SetLabel( const CString strLabel )
{
	ASSERT_VALID(this);
	m_strLabel = strLabel;
}

void CCustomDrawToolTipCtrl::SetDescription( const CString strDescription )
{
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	CCDToolTipCtrlBase::SetDescription(strDescription);
#else
	ASSERT_VALID(this);
	m_strDescription = strDescription;
	//m_strDescription.Replace(_T("\t"), _T("    "));
#endif // DERIVE_FROM_MFCTOOLTIPCTRL	
}

void CCustomDrawToolTipCtrl::SetSupplementalDescription( const CString strDescription )
{
	ASSERT_VALID(this);
	m_strSupplementalDescription = strDescription;
}

CSize CCustomDrawToolTipCtrl::GetIconSize()
{
	if (m_hLabelIcon)
	{
		CSize sizeIcon = ::GetIconSize(m_hLabelIcon);
		return sizeIcon;
	}
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	return CCDToolTipCtrlBase::GetIconSize();
#else
	return CSize(0, 0);
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
}

void CCustomDrawToolTipCtrl::SetLocation( CPoint pt )
{
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	CCDToolTipCtrlBase::SetLocation( pt );
#else
	ASSERT_VALID(this);
	m_ptLocation = pt;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL	
}

void CCustomDrawToolTipCtrl::SetTextTabSize( int nTabSize /*= 4*/ )
{
	ASSERT_VALID(this);
	m_nTextTabSize = nTabSize;
}

BOOL CCustomDrawToolTipCtrl::OnDrawIcon( CDC* pDC, CRect rectImage )
{
	if (m_hLabelIcon)
	{
		return ::DrawIconEx(pDC->GetSafeHdc(), rectImage.left, rectImage.top, m_hLabelIcon, m_sizeImage.cx, m_sizeImage.cy, 0, NULL, DI_NORMAL);
	}
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	return CCDToolTipCtrlBase::OnDrawIcon( pDC, rectImage );
#else
	return FALSE;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL	
}

BOOL CCustomDrawToolTipCtrl::OnDrawDescriptionIcon( CDC* pDC, CRect rect )
{
	if (m_hDescrIcon)
	{
		return ::DrawIconEx(pDC->GetSafeHdc(), rect.left, rect.top, m_hDescrIcon, m_sizeDescrImage.cx, m_sizeDescrImage.cy, 0, NULL, DI_NORMAL);
	}
	return FALSE;
}

CSize CCustomDrawToolTipCtrl::OnDrawDescription( CDC* pDC, CRect rect, BOOL bCalcOnly )
{
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	return CCDToolTipCtrlBase::OnDrawDescription( pDC, rect, bCalcOnly );
#else
	ASSERT_VALID(pDC);
	
	if (!m_Params.m_bDrawDescription)
	{
		return CSize(0, 0);
	}

	int nFixedWidth = GetFixedWidth();
	
	if (nFixedWidth > 0 && m_sizeImage.cx <= 32)
	{
		rect.right = rect.left + nFixedWidth;
		
		if (m_sizeImage.cx > 0 && m_Params.m_bDrawIcon)
		{
			rect.right -= m_sizeImage.cx + m_ptMargin.x;
		}
	}
	else if ( m_Params.m_nMaxDescrWidth > 0)
	{
		rect.right = rect.left + m_Params.m_nMaxDescrWidth;
	}

	return DrawText(pDC, m_strDescription, rect, bCalcOnly, m_Params.m_bBoldDescription, TRUE, TRUE);
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
}

void CCustomDrawToolTipCtrl::OnDrawSeparator( CDC* pDC, int x1, int x2, int y )
{
	COLORREF clrSeparator = m_Params.m_clrSeparator == (COLORREF)-1 ? RGB(158,187,221) : m_Params.m_clrBorder;;
	CPen penDark(PS_SOLID, 1, clrSeparator);
	CPen* pOldPen = pDC->SelectObject(&penDark);

	ASSERT_VALID(pDC);
	pDC->MoveTo(x1, y);
	pDC->LineTo(x2, y);

	CPen penLight(PS_SOLID, 1, RGB(255,255,255));
	pDC->SelectObject(&penLight);
	pDC->MoveTo(x1, y+1);
	pDC->LineTo(x2, y+1);

	pDC->SelectObject(pOldPen);
}

#ifndef MSVC_NEW_VER
LRESULT CCustomDrawToolTipCtrl::OnSetFont( WPARAM wParam, LPARAM lParam )
{
	BOOL bRedraw = (BOOL) LOWORD(lParam);
	
	m_hFont = (HFONT) wParam;

	if (m_fontBold.GetSafeHandle())
		m_fontBold.DeleteObject();
	if ( !m_fontBold.GetSafeHandle() )
	{
		LOGFONT lf = {0};
		GetObject(m_hFont, sizeof(LOGFONT), &lf);
		lf.lfWeight = FW_BOLD;
		m_fontBold.CreateFontIndirect(&lf);
	}
	
	if (bRedraw)
	{
		RedrawWindow();
		UpdateWindow();
	}
	
	return 0;
}
#endif // MSVC_NEW_VER

CFont* CCustomDrawToolTipCtrl::SelectFont( CDC *pDC, BOOL bBold /*= FALSE*/ )
{
#ifdef MSVC_NEW_VER
	CFont* pOldFont = (CFont*) pDC->SelectObject( bBold ? &afxGlobalData.fontBold : &afxGlobalData.fontTooltip);
#else
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	
	CFont* pOldFont = NULL;
	
	if (m_hFont != NULL)
	{
		if (bBold)
		{
			ASSERT(m_fontBold.GetSafeHandle());
			pOldFont = pDC->SelectObject(&m_fontBold);
		}
		else
		{
			pOldFont = pDC->SelectObject(CFont::FromHandle(m_hFont));
		}
	}
	else
	{
		if (bBold)
		{
			if ( !m_fontBold.GetSafeHandle() )
			{
				HFONT hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
				LOGFONT lf = {0};
				GetObject(hDefFont, sizeof(LOGFONT), &lf);
				lf.lfWeight = FW_BOLD;
				m_fontBold.CreateFontIndirect(&lf);
			}
			pOldFont = pDC->SelectObject(&m_fontBold);
		}
		else
		{
			pOldFont = (CFont*) pDC->SelectStockObject(DEFAULT_GUI_FONT);
		}
	}
#endif // MSVC_NEW_VER
	
	return pOldFont;
}

void CCustomDrawToolTipCtrl::SetLabelIcon( HICON hIcon )
{
	ASSERT_VALID(this);
	m_hLabelIcon = hIcon;
}

void CCustomDrawToolTipCtrl::SetDescriptionIcon( HICON hIcon )
{
	ASSERT_VALID(this);
	m_hDescrIcon = hIcon;
}

void CCustomDrawToolTipCtrl::SetSupplementalImage( HBITMAP hBmp )
{
	ASSERT_VALID(this);
	m_hSupplementalBmp = hBmp;
}

/*----------------------------------------------------------------------------*/
/* CCustomDrawListCtrl
/*----------------------------------------------------------------------------*/

IMPLEMENT_DYNCREATE(CCustomDrawListCtrl, CCustomDrawListCtrlBase)

CCustomDrawListCtrl::CCustomDrawListCtrl()
	: m_bEnableCustomDraw(TRUE)
	, m_nHotItem(-1)
	, m_bIsOwnerDraw(FALSE)
	, m_bDrawHotItem(TRUE)
	, m_bMouseEventsTracked(FALSE)
	, m_bExplorerVisualStyle(FALSE)
	, m_nLockDrawCount(0)
	, m_pTextDrawer(NULL)
{
	m_bMarkSortedColumn	= TRUE;
	m_clrSortedColumn	= RGB(225,242,249);
}

CCustomDrawListCtrl::~CCustomDrawListCtrl()
{
}

BEGIN_MESSAGE_MAP(CCustomDrawListCtrl, CCustomDrawListCtrlBase)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnHeaderClicked )
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_WM_SIZE()
#ifdef FIX_LISTCTRL_HILIGHT_UNSELECTED_ITEM_ISSUE
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
#endif // FIX_LISTCTRL_HILIGHT_UNSELECTED_ITEM_ISSUE
	ON_MESSAGE(WM_CDTOOLTIPCTRL_NOTIFY, OnCustomToolTipNotify)
END_MESSAGE_MAP()

void CCustomDrawListCtrl::OnInitList()
{
#ifndef DERIVE_FROM_MFCLISTCTRL
	InitHeader();
#endif // DERIVE_FROM_MFCLISTCTRL

	SetDoubleBuffered(TRUE);

	// Disable the CToolTipCtrl of CListCtrl so it won't disturb our own tooltip-ctrl
	CToolTipCtrl* pToolTipCtrl = GetToolTips();
	if (pToolTipCtrl)
	{
		pToolTipCtrl->Activate(FALSE);
	}

	DWORD dwStyle = GetStyle();
	m_bIsOwnerDraw = (dwStyle & LVS_OWNERDRAWFIXED) == LVS_OWNERDRAWFIXED;
	
	// Enable our own tooltip-ctrl and make it show tooltip even if not having focus
	VERIFY( GetCustomDrawToolTips().Create(this, TTS_ALWAYSTIP) );
	GetCustomDrawToolTips().Activate(TRUE);

	EnableExplorerVisualStyles();
}

void CCustomDrawListCtrl::InitHeader()
{
	if (CListCtrl::GetHeaderCtrl()->GetSafeHwnd())
	{
		//GetSortHeaderCtrl().Init( CListCtrl::GetHeaderCtrl()->GetSafeHwnd() );
		//GetSortHeaderCtrl().SetSortable( IsSortable() );
	}
}

int CCustomDrawListCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CCustomDrawListCtrlBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	OnInitList();
	return 0;
}

void CCustomDrawListCtrl::PreSubclassWindow()
{
	CCustomDrawListCtrlBase::PreSubclassWindow();

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == NULL)
	{
		OnInitList();
	}
}

void CCustomDrawListCtrl::EnableCustomDraw( BOOL bEnable /*= TRUE*/, BOOL bRedraw /*= FALSE*/ )
{
	m_bEnableCustomDraw = bEnable;
	if ( bRedraw )
		RedrawWindow();	
}

BOOL CCustomDrawListCtrl::IsDoubleBuffered()
{
	return (GetExtendedStyle() & LVS_EX_DOUBLEBUFFER);
}

void CCustomDrawListCtrl::SetDoubleBuffered( BOOL bSet /*= TRUE*/ )
{
	DWORD dwExtStyle = GetExtendedStyle();
	if ( bSet )
		dwExtStyle |= LVS_EX_DOUBLEBUFFER;
	else
		dwExtStyle &= ~LVS_EX_DOUBLEBUFFER;
	SetExtendedStyle(dwExtStyle);
}

BOOL CCustomDrawListCtrl::IsFullRowSelect()
{
	return (GetExtendedStyle() & LVS_EX_FULLROWSELECT);
}

void CCustomDrawListCtrl::SetFullRowSelect(BOOL bSet /*= TRUE*/)
{
	DWORD dwExtStyle = GetExtendedStyle();
	if ( bSet )
		dwExtStyle |= LVS_EX_FULLROWSELECT;
	else
		dwExtStyle &= ~LVS_EX_FULLROWSELECT;
	SetExtendedStyle(dwExtStyle);
}

BOOL CCustomDrawListCtrl::IsHeaderDragDrop()
{
	return (GetExtendedStyle() & LVS_EX_HEADERDRAGDROP);
}

void CCustomDrawListCtrl::SetHeaderDragDrop( BOOL bSet /*= TRUE*/ )
{
	DWORD dwExtStyle = GetExtendedStyle();
	if ( bSet )
		dwExtStyle |= LVS_EX_HEADERDRAGDROP;
	else
		dwExtStyle &= ~LVS_EX_HEADERDRAGDROP;
	SetExtendedStyle(dwExtStyle);
}

BOOL CCustomDrawListCtrl::IsSingleSel()
{
	return (GetStyle() & LVS_SINGLESEL);
}

void CCustomDrawListCtrl::SetSignleSel( BOOL bSet /*= TRUE*/ )
{
	DWORD dwStyle = GetStyle();
	if ( bSet )
		dwStyle |= LVS_SINGLESEL;
	else
		dwStyle &= ~LVS_SINGLESEL;
	SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
}

LRESULT CCustomDrawListCtrl::EnableExplorerVisualStyles( bool bValue /*= true*/ )
{
	if (!IsThemeEnabled())
	{
		m_bExplorerVisualStyle = false;
		return S_FALSE;
	}
	
	if (!CheckOSVersion(0x0600))
	{
		m_bExplorerVisualStyle = false;
		return S_FALSE;
	}
	
	LRESULT rc = S_FALSE;
	if (bValue)
		rc = EnableWindowTheme(GetSafeHwnd(), L"ListView", L"Explorer", NULL);
	else
		rc = EnableWindowTheme(GetSafeHwnd(), L"", L"", NULL);
	
	if (bValue && rc==S_OK)
	{
		// OBS! Focus retangle is not painted properly without double-buffering
		m_bExplorerVisualStyle = true;
//#if (_WIN32_WINNT >= 0x501)
		if (CheckOSVersion(0x501))
			SetDoubleBuffered(TRUE);
//#endif
	}
	else
	{
		m_bExplorerVisualStyle = false;
	}

	return rc;
}

#ifndef MSVC_NEW_VER
CToolTipCtrl* CCustomDrawListCtrl::GetToolTips() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, LVM_GETTOOLTIPS, 0, 0L));
}

CToolTipCtrl* CCustomDrawListCtrl::SetToolTips( CToolTipCtrl* pWndTip )
{
	ASSERT(::IsWindow(m_hWnd));
	return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, LVM_SETTOOLTIPS, 0, (LPARAM) pWndTip->GetSafeHwnd()));
}
#endif // MSVC_NEW_VER

#ifndef DERIVE_FROM_MFCLISTCTRL
void CCustomDrawListCtrl::EnableMarkSortedColumn( BOOL bMark /*= TRUE*/, BOOL bRedraw /*= TRUE*/ )
{
	m_bMarkSortedColumn = bMark;
	
	if (GetSafeHwnd() != NULL && bRedraw)
	{
		RedrawWindow();
	}
}
#endif // DERIVE_FROM_MFCLISTCTRL

#define DEBUG_CUSTOMDRAW

#ifdef DEBUG_CUSTOMDRAW
	#define DBGCD_TRACE	TRACE
#else
	#define DBGCD_TRACE
#endif // DEBUG_CUSTOMDRAW

#ifdef _DEBUG
CString getDrawState(UINT uDrawItemState)
{
	CString strState;
	if ( uDrawItemState & CDIS_SELECTED )
	{
		strState += " selected";
	}
	if ( uDrawItemState & CDIS_GRAYED )
	{
		strState += " grayed";
	}
	if ( uDrawItemState & CDIS_DISABLED )
	{
		strState += " disabled";
	}
	if ( uDrawItemState & CDIS_CHECKED )
	{
		strState += " checked";
	}
	if ( uDrawItemState & CDIS_FOCUS )
	{
		strState += " focus";
	}
	if ( uDrawItemState & CDIS_DEFAULT )
	{
		strState += " default";
	}
	if ( uDrawItemState & CDIS_HOT )
	{
		strState += " hot";
	}
	if ( uDrawItemState & CDIS_MARKED )
	{
		strState += " marked";
	}
	if ( uDrawItemState & CDIS_INDETERMINATE )
	{
		strState += " indeterminate";
	}
	if ( uDrawItemState == 0 )
	{
		strState += " nothing";
	}
	return strState;
}

CString getLVItemState(UINT uItemState)
{
	CString strState;
	if ( uItemState & LVIS_FOCUSED )
	{
		strState += " focused";
	}
	if ( uItemState & LVIS_SELECTED )
	{
		strState += " selected";
	}
	if ( uItemState & LVIS_CUT )
	{
		strState += " cut";
	}
	if ( uItemState & LVIS_DROPHILITED )
	{
		strState += " drophilited";
	}
	if ( uItemState & LVIS_ACTIVATING )
	{
		strState += " activating";
	}
	if ( uItemState == 0 )
	{
		strState += " nothing";
	}
	return strState;
}
#endif // _DEBUG

void CCustomDrawListCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	// DrawItem() is not thread safe
	// Reference:
	// http://www.codeproject.com/KB/list/selectentirerow.aspx#Readers Comments
	CMutex mutex(FALSE, "CCustomDrawListCtrl::DrawItem()");
    CSingleLock lock(&mutex);
    lock.Lock(); // Wait until signalled. 
	// Unlock takes place in destructor when function exit
	
	int nItem			= static_cast<int>( lpDrawItemStruct->itemID );
	CDC* pDC			= CDC::FromHandle(lpDrawItemStruct->hDC);
	ASSERT(pDC);

	OnDrawItem(pDC, nItem);
}

void CCustomDrawListCtrl::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult )
{
	LPNMLVCUSTOMDRAW pNMLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	if ( !pNMLVCD )
	{
		ASSERT(0);	// can't fail!
		return;
	}

	DWORD& dwDrawStage	= pNMLVCD->nmcd.dwDrawStage;

	switch (dwDrawStage)
	{
	case CDDS_PREPAINT:
		if ( IsCustomDrawEnabled() )
			*pResult = CDRF_NOTIFYITEMDRAW;
		else
			*pResult = CDRF_DODEFAULT;
		break;
	case CDDS_ITEMPREPAINT:
		{
			int nItem			= static_cast<int>( pNMLVCD->nmcd.dwItemSpec );
			CDC* pDC			= CDC::FromHandle(pNMLVCD->nmcd.hdc);
			ASSERT(pDC);
			OnDrawItem(pDC, nItem);
			*pResult = CDRF_SKIPDEFAULT;
		}
		break;
	default:
		TRACE("===== CCustomDrawListCtrl::OnCustomdraw() unhandled draw stage!\n");
		*pResult = CDRF_DODEFAULT;
		break;
	}
}

CRect CCustomDrawListCtrl::GetSubItemDrawRect( int nItem, int nSubItem )
{
	CRect rcItem;
	int nArea = IsFullRowSelect() ? LVIR_BOUNDS : LVIR_LABEL;	// if it is full-row select, we need to invalidate thw whole row
	VERIFY( GetSubItemRect(nItem, nSubItem, nArea, rcItem) );
	
	if ( nItem+1 < GetItemCount() )
	{
		// this is not the last item
		UINT nState = GetItemState(nItem+1, LVIS_SELECTED|LVIS_FOCUSED);
		if ( 0 == nState )
			--rcItem.bottom;	// avoid bottom border gets covered by the grid lines
	}
	return rcItem;
}

#define TRY_DRAW_LISTCTRL_WITH_MEMDC

void CCustomDrawListCtrl::OnDrawItem( CDC* pDC, int nItem )
{
	CRect rcBounds;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
#ifdef TRY_DRAW_LISTCTRL_WITH_MEMDC
	CSimpleMemDC memDC(*pDC, rcBounds);
	CDC* pDrawDC	= &memDC.GetDC();
	
	CFont* pFont	= pDC->GetCurrentFont();
	CFont* pOldFont	= pDrawDC->SelectObject(pFont);
#else
	CDC* pDrawDC	= pDC;
#endif // TRY_DRAW_LISTCTRL_WITH_MEMDC

	// Erase the background first
	OnEraseBackground(pDrawDC, rcBounds, nItem);
	
	//int nColCount = GetSortHeaderCtrl().GetItemCount();
	
	LVITEM lvitem		= {0};
	lvitem.mask			= LVIF_IMAGE|LVIF_STATE;
	lvitem.iItem		= nItem;
	lvitem.stateMask	= (UINT)-1;
	VERIFY( GetItem(&lvitem) );
	
	//DBGCD_TRACE("====> %d, Draw State is [%s] at tick %X\n", nItem, getLVItemState(lvitem.state), GetTickCount());
	
	/*for (int nCol = 0; nCol < nColCount; ++nCol)
	{
		OnDrawCell(pDrawDC, nItem, nCol, &lvitem);
	}*/
	
#ifdef TRY_DRAW_LISTCTRL_WITH_MEMDC
	pDrawDC->SelectObject(pOldFont);
#endif // TRY_DRAW_LISTCTRL_WITH_MEMDC
}

void CCustomDrawListCtrl::OnEraseBackground( CDC* pDC, CRect rect, int nItem )
{
	// you can also copy the background bitmap from the client window if you wish
	COLORREF clrBk = ::GetSysColor( IsWindowEnabled() ? COLOR_WINDOW : COLOR_BTNFACE );
	pDC->FillSolidRect(rect, clrBk);
}

void CCustomDrawListCtrl::OnDrawCell( CDC* pDC, int nItem, int nSubItem, LPLVITEM lpLVItem )
{
	CRect rcItem = GetSubItemDrawRect(nItem, nSubItem);

	// Fill the background first
	if ( IsWindowEnabled() )
		OnFillBackground(pDC, rcItem, nItem, nSubItem, lpLVItem);
	
	// Draw the icon
	CRect rectIcon;
	VERIFY( GetSubItemRect(nItem, nSubItem, LVIR_ICON, rectIcon) );

	BOOL bHasIcon = OnDrawIcon(pDC, rectIcon, nItem, nSubItem, lpLVItem);
	
	// Draw the text
	CRect rectLabel;
	VERIFY( GetSubItemRect(nItem, nSubItem, LVIR_LABEL, rectLabel) );
	rectLabel.left += 2;
	if ( bHasIcon )
	{
		rectLabel.left = rectIcon.right + 5;
		//rectLabel.top += 1;
	}
	OnDrawCellText(pDC, rectLabel, nItem, nSubItem, lpLVItem);
}

CSize CCustomDrawListCtrl::GetIconSize( int nItem, int nSubItem )
{
	CSize sizeIcon(0,0);
	CImageList* pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		int cx, cy;
		ImageList_GetIconSize(pImageList->GetSafeHandle(), &cx, &cy);
		sizeIcon.cx = cx;
		sizeIcon.cy = cy;
	}
	return sizeIcon;
}

BOOL CCustomDrawListCtrl::OnDrawIcon( CDC* pDC, CRect rect, int nItem, int nSubItem, LPLVITEM lpLVItem )
{
	if ( 0 == nSubItem )
	{
		CImageList* pImageList = GetImageList(LVSIL_SMALL);
		if ( pImageList && lpLVItem->iImage >= 0 )
		{
			CSize sizeImage = GetIconSize(nItem, nSubItem);
			int ny = rect.top + (rect.Height() - sizeImage.cy) / 2;

			// Unfortunately CImageList::Draw(..ILD_TRANSPARENT) can't take care of transparent icon in 
			// expected way, I see black outline on the bitmap, so here I use DrawIconEx instead
// 			CPoint ptImage(rect.left, ny);
// 			pImageList->Draw(pDC, lpLVItem->iImage, ptImage, ILD_TRANSPARENT);

 			HICON hIcon = pImageList->ExtractIcon(lpLVItem->iImage);
 			::DrawIconEx(pDC->GetSafeHdc(), rect.left, ny, hIcon, sizeImage.cx, sizeImage.cy, 0, NULL, DI_NORMAL);
 			::DestroyIcon(hIcon);

			return TRUE;
		}
	}
	return FALSE;
}

void CCustomDrawListCtrl::OnFillBackground( CDC* pDC, CRect rect, int nItem, int nSubItem, LPLVITEM lpLVItem )
{
	UINT& nItemState = lpLVItem->state;

	CCustomDrawHelper itemDrawer;
	itemDrawer.m_bWndHasFocus		= ::GetFocus() == GetSafeHwnd();
	itemDrawer.m_bFocusItem			= ((nItemState & LVIS_FOCUSED) ? true : false);
	itemDrawer.m_bSelected			= ((nItemState & LVIS_SELECTED) ? true : false);
	itemDrawer.m_bIsHotItem			= m_bDrawHotItem && m_nHotItem == nItem;

	BOOL bFilledWithSortClr			= FALSE;

	if ( IsSortable() 
		&& m_bMarkSortedColumn 
		//&& GetSortHeaderCtrl().GetSortColumn() == nSubItem
		&& !itemDrawer.m_bSelected
		&& !itemDrawer.m_bIsHotItem
		)
	{
		CRect rectColumn;
		GetSubItemRect(nItem, nSubItem, LVIR_LABEL, rectColumn);
		pDC->FillSolidRect(rectColumn, m_clrSortedColumn);
		bFilledWithSortClr = TRUE;
	}
	if ( 0 == nSubItem || (bFilledWithSortClr && itemDrawer.m_bFocusItem) )
	{
		itemDrawer.m_bDrawBorderWhenFill	= 0 == nSubItem;
		itemDrawer.DrawItemBackground(pDC, rect);
	}
}

void CCustomDrawListCtrl::OnDrawCellText( CDC* pDC, CRect rect, int nItem, int nSubItem, LPLVITEM lpLVItem )
{
	UINT& nItemState = lpLVItem->state;

	CString strLabel = GetItemText(nItem, nSubItem);
	if ( !strLabel.IsEmpty() )
	{
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH;
		GetColumn(nSubItem, &lvc);

		int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
		COLORREF clrTxtColor = GetItemTextColor(nItem, nSubItem, nItemState);
		COLORREF clrOldTxtColor;
		if (clrTxtColor != CLR_INVALID)
			clrOldTxtColor = pDC->SetTextColor(clrTxtColor);

		UINT nJustify = DT_LEFT;
		switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
		{
		case LVCFMT_RIGHT:
			nJustify = DT_RIGHT;
			break;
		case LVCFMT_CENTER:
			nJustify = DT_CENTER;
			break;
		default:
			break;
		}

		UINT nDTFormat = nJustify|DT_END_ELLIPSIS|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;

		if ( GetTextDrawer() )
			GetTextDrawer()->Draw(pDC, strLabel, rect, nDTFormat);
		else
			pDC->DrawText(strLabel, rect, nDTFormat);

		if (clrTxtColor != CLR_INVALID)
			pDC->SetTextColor(clrOldTxtColor);

		pDC->SetBkMode(nOldBkMode);
	}
}

COLORREF CCustomDrawListCtrl::GetItemTextColor( int nItem, int nSubItem, UINT nItemState )
{
#ifndef CUSTOMDRAW_GRADIENT
	if ( nItemState & LVIS_SELECTED && ::GetFocus() == GetSafeHwnd() && (IsFullRowSelect() || nSubItem == 0) )
	{
		return ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
#endif // CUSTOMDRAW_GRADIENT
	return CLR_INVALID;
}

// Taken from http://www.codeproject.com/KB/list/quicklist.aspx
BOOL CCustomDrawListCtrl::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT* pResult )
{
	UINT nID = pNMHDR->idFrom;
	
	// check if this is the automatic tooltip of the control
	if (nID == 0)
		return TRUE;	// do not allow display of automatic tooltip,
						// or our tooltip will disappear
	
	*pResult = 0;
	
	// get the mouse position
	const MSG* pMessage = GetCurrentMessage();
	ASSERT(pMessage);
	CPoint pt;
	pt = pMessage->pt;		// get the point from the message
	ScreenToClient(&pt);	// convert the point's coords to be relative to this control
	
	// see if the point falls onto a list item
	LVHITTESTINFO hitinfo = {0};
	hitinfo.pt		= pt;
	hitinfo.flags	= 0;
	hitinfo.iItem	= hitinfo.iSubItem = -1;
	
	if ( SubItemHitTest(&hitinfo) >= 0 && ShouldShowToolTipForCell(hitinfo.iItem, hitinfo.iSubItem) )
	{
		CString strToolTip = GetToolTipLabelForCell(hitinfo.iItem, hitinfo.iSubItem);
		
		if (strToolTip.IsEmpty())
		{
			strToolTip = GetItemText(hitinfo.iItem, hitinfo.iSubItem);
		}
		if (!strToolTip.IsEmpty())
		{
			GetCustomDrawToolTips().SetText(pNMHDR, strToolTip);
			GetCustomDrawToolTips().SetLabel(strToolTip);	// otherwise the tooltip can't show correct UNICODE text under ANSI environment
			return TRUE;
		}	
	}
	
	return FALSE;	// we didn't handle the message, let the 
					// framework continue propagating the message
}

void CCustomDrawListCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	CCustomDrawListCtrlBase::OnMouseMove(nFlags, point);
	
	// Find the subitem
	LVHITTESTINFO hitinfo = {0};
	hitinfo.flags	= nFlags;
	hitinfo.pt		= point;
	SubItemHitTest(&hitinfo);

	if ( m_bDrawHotItem )
	{
		if ( !m_bMouseEventsTracked )
		{
			m_bMouseEventsTracked = TRUE;
			
			TRACKMOUSEEVENT trackmouseevent;
			trackmouseevent.cbSize		= sizeof(trackmouseevent);
			trackmouseevent.dwFlags		= TME_LEAVE;
			trackmouseevent.hwndTrack	= GetSafeHwnd();
			trackmouseevent.dwHoverTime	= HOVER_DEFAULT;
			//::AFXTrackMouse(&trackmouseevent);
			_TrackMouseEvent(&trackmouseevent);
		}

		if ( m_nHotItem != hitinfo.iItem )
		{
			int nOldHotItem = m_nHotItem;
			m_nHotItem = hitinfo.iItem;
			
			LockSetRedraw(FALSE);
			SetHotItem(m_nHotItem);

			// RedrawItems produces flicker.
// 			RedrawItems(m_nHotItem, m_nHotItem);
// 			RedrawItems(nOldHotItem, nOldHotItem);
			CRect rect;
			GetItemRect(m_nHotItem, rect, LVIR_BOUNDS);
			InvalidateRect(rect, FALSE);
			GetItemRect(nOldHotItem, rect, LVIR_BOUNDS);
			InvalidateRect(rect, FALSE);

			LockSetRedraw(TRUE);
		}
	}

	if ( ShouldShowToolTipForCell(hitinfo.iItem, hitinfo.iSubItem)
		&& GetCustomDrawToolTips().CheckDisplayToolTip( (WPARAM)hitinfo.iItem, (LPARAM)hitinfo.iSubItem ) 
		)
	{		
		// Remove the old tooltip (if available)
		int nToolCount = GetCustomDrawToolTips().GetToolCount();
		//TRACE("CCustomDrawListCtrl tooltip tool count: %d\n", nToolCount);
		if (nToolCount > 0)
		{
			// Not using CToolTipCtrl::DelTool() because it redirects the messages to CListCtrl parent
			// If we call DelTool(), you will see that the return value of GetToolCount() still keep increasing!
			//GetCustomDrawToolTips().DelTool(this);
			TOOLINFO ti = {0};
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_IDISHWND;	// Indicate that uId is handle to a control
			ti.uId = (UINT_PTR)m_hWnd;	// Handle to the control
			ti.hwnd = m_hWnd;			// Handle to window to receive the tooltip-messages
			ti.hinst = AfxGetInstanceHandle();
			GetCustomDrawToolTips().SendMessage(TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			GetCustomDrawToolTips().Activate(FALSE);
		}
		
		// Add the new tooltip (if available)
		if (hitinfo.iItem != -1 && hitinfo.iSubItem != -1)
		{
			// Not using CToolTipCtrl::AddTool() because it redirects the messages to CListCtrl parent
			//GetCustomDrawToolTips().AddTool(this);
			TOOLINFO ti = {0};
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_IDISHWND;	// Indicate that uId is handle to a control
			ti.uId = (UINT_PTR)m_hWnd;	// Handle to the control
			ti.hwnd = m_hWnd;			// Handle to window to receive the tooltip-messages
			ti.hinst = AfxGetInstanceHandle();
			ti.lpszText = LPSTR_TEXTCALLBACK;
			GetCustomDrawToolTips().SendMessage(TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

			PreAddToolTipForCell(hitinfo.iItem, hitinfo.iSubItem);

			GetCustomDrawToolTips().Activate(TRUE);
		}
	}
}

LRESULT CCustomDrawListCtrl::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	LRESULT result = Default();
	m_bMouseEventsTracked = FALSE;
	
	if (m_nHotItem >= 0)
	{
		int nTmpHotItem = m_nHotItem;
		m_nHotItem = -1;
		//RedrawItems(nTmpHotItem, nTmpHotItem);

		CRect rect;
		GetItemRect(nTmpHotItem, rect, LVIR_BOUNDS);
		InvalidateRect(rect, FALSE);
	}
	
	return result;
}

bool CCustomDrawListCtrl::IsSortable() const
{
	return false;
}

BOOL CCustomDrawListCtrl::OnSort( int nCol )
{
	ASSERT(IsSortable());
	return TRUE;
}

void CCustomDrawListCtrl::OnHeaderClicked( NMHDR* pNMHDR, LRESULT* pResult )
{
	// List view notification header
    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	
	if ( IsSortable() )
	{
		//int nOldSortColumn = GetSortHeaderCtrl().GetSortColumn();
		if ( OnSort(pNMListView->iSubItem) )
		{
			//GetSortHeaderCtrl().SwitchSortItem(pNMListView->iSubItem);
		}
		if (m_bMarkSortedColumn)
		{
			LockSetRedraw(FALSE);
			CRect rectClient;
			GetClientRect(rectClient);
			CRect rectColumn;
			/*if (nOldSortColumn >= 0)
			{
				GetSubItemRect(0, nOldSortColumn, LVIR_LABEL, rectColumn);
				rectColumn.top = rectClient.top;
				rectColumn.bottom = rectClient.bottom;
				InvalidateRect(rectColumn, FALSE);
			}*/
			//int nNewSortColumn = GetSortHeaderCtrl().GetSortColumn();
			/*if (nNewSortColumn >= 0 && nNewSortColumn != nOldSortColumn)
			{
				GetSubItemRect(0, nNewSortColumn, LVIR_LABEL, rectColumn);
				rectColumn.top = rectClient.top;
				rectColumn.bottom = rectClient.bottom;
				InvalidateRect(rectColumn, FALSE);
			}*/
			LockSetRedraw(TRUE);
		}
	}
	
    // Return result
    *pResult = 0;
}

BOOL CCustomDrawListCtrl::PreTranslateMessage( MSG* pMsg )
{
	if (GetCustomDrawToolTips().m_hWnd)
		GetCustomDrawToolTips().RelayEvent(pMsg);
	return CCustomDrawListCtrlBase::PreTranslateMessage(pMsg);
}

BOOL CCustomDrawListCtrl::ShouldShowToolTipForCell( int nRow, int nCol )
{
	return TRUE;
}

CString CCustomDrawListCtrl::GetToolTipLabelForCell( int nRow, int nCol )
{
	return _T("");
}

void CCustomDrawListCtrl::PreAddToolTipForCell( int nRow, int nCol )
{
	// nothing to do
}

void CCustomDrawListCtrl::PreShowToolTipForCell( int nRow, int nCol )
{
	// nothing to do
}

void CCustomDrawListCtrl::PrePopToolTip()
{
	// nothing to do
}

void CCustomDrawListCtrl::OnSize( UINT nType, int cx, int cy )
{
	CCustomDrawListCtrlBase::OnSize(nType, cx, cy);
	
	//if (GetSortHeaderCtrl().GetSafeHwnd() != NULL)
	{
		//GetSortHeaderCtrl().RedrawWindow();
	}
}

void CCustomDrawListCtrl::OnDestroy()
{
	OnDestroyList();
	CCustomDrawListCtrlBase::OnDestroy();
}

#ifdef FIX_LISTCTRL_HILIGHT_UNSELECTED_ITEM_ISSUE
void CCustomDrawListCtrl::RepaintSelectedItems()
{
	RedrawWindow();
}

void CCustomDrawListCtrl::OnKillFocus( CWnd* pNewWnd )
{
	CCustomDrawListCtrlBase::OnKillFocus(pNewWnd);
	
	// check if we are losing focus to label edit box
	if(pNewWnd != NULL && pNewWnd->GetParent() == this)
		return;
	
	// repaint items that should change appearance
	if( m_bIsOwnerDraw && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT )
		RepaintSelectedItems();
}

void CCustomDrawListCtrl::OnSetFocus( CWnd* pOldWnd )
{
	CCustomDrawListCtrlBase::OnSetFocus(pOldWnd);
	
	// check if we are getting focus from label edit box
	if(pOldWnd!=NULL && pOldWnd->GetParent()==this)
		return;
	
	// repaint items that should change appearance
	if( m_bIsOwnerDraw && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT )
		RepaintSelectedItems();
}

void CCustomDrawListCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	CCustomDrawListCtrlBase::OnLButtonDown(nFlags, point);

	// repaint items that should change appearance
	if( m_bIsOwnerDraw 
		&& (GetStyle() & LVS_TYPEMASK) == LVS_REPORT 
		&& !IsSingleSel()
		&& !SHIFT_DOWN 
		&& !CNTRL_DOWN 
		)
		RepaintSelectedItems();
}

void CCustomDrawListCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
	CCustomDrawListCtrlBase::OnRButtonDown(nFlags, point);
	
	// repaint items that should change appearance
	if( m_bIsOwnerDraw 
		&& (GetStyle() & LVS_TYPEMASK) == LVS_REPORT 
		&& !IsSingleSel()
		&& !SHIFT_DOWN 
		&& !CNTRL_DOWN 
		)
		RepaintSelectedItems();
}
#endif // FIX_LISTCTRL_HILIGHT_UNSELECTED_ITEM_ISSUE

LRESULT CCustomDrawListCtrl::OnCustomToolTipNotify( WPARAM wParam, LPARAM lParam )
{
	switch (wParam)
	{
		case CDTOOLTIP_ONBEFORE_SHOW:
			{
				int nRow = (int)GetCustomDrawToolTips().GetDisplayWParam();
				int nCol = (int)GetCustomDrawToolTips().GetDisplayLParam();
				PreShowToolTipForCell(nRow, nCol);
			}
			break;
		case CDTOOLTIP_ONBEFORE_POP:
			PrePopToolTip();
			break;
	}
	return 0;
}

/*----------------------------------------------------------------------------*/
/* CCustomDrawListBox
/*----------------------------------------------------------------------------*/

IMPLEMENT_DYNCREATE(CCustomDrawListBox, CCustomDrawListBoxBase)

CCustomDrawListBox::CCustomDrawListBox()
	: m_pImageList(NULL)
	, m_nHotItem(-1)
	, m_bMouseEventsTracked(FALSE)
	, m_bExplorerVisualStyle(FALSE)
	, m_nLockDrawCount(0)
	, m_pTextDrawer(NULL)
{
	
}

CCustomDrawListBox::~CCustomDrawListBox()
{
	
}

BEGIN_MESSAGE_MAP(CCustomDrawListBox, CCustomDrawListBoxBase)
	ON_MESSAGE(WM_DRAWITEM, OnDrawItem)
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_MESSAGE(WM_CDTOOLTIPCTRL_NOTIFY, OnCustomToolTipNotify)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

int CCustomDrawListBox::GetItemCount()
{
	return GetCount();
}

int CCustomDrawListBox::SetItemCount( int nCount )
{
	ASSERT(::IsWindow(m_hWnd));
	return ::SendMessage(m_hWnd, LB_SETCOUNT, nCount, 0);
}

LRESULT CCustomDrawListBox::EnableExplorerVisualStyles( bool bValue /*= true*/ )
{
	if (!IsThemeEnabled())
	{
		m_bExplorerVisualStyle = false;
		return S_FALSE;
	}
	
	if (!CheckOSVersion(0x0600))
	{
		m_bExplorerVisualStyle = false;
		return S_FALSE;
	}
	
	LRESULT rc = S_FALSE;
	if (bValue)
		rc = EnableWindowTheme(GetSafeHwnd(), L"ListBox", L"Explorer", NULL);
	else
		rc = EnableWindowTheme(GetSafeHwnd(), L"", L"", NULL);
	
	m_bExplorerVisualStyle = (bValue && rc == S_OK);
	
	return rc;
}

LRESULT CCustomDrawListBox::OnDrawItem( WPARAM nIDCtl, LPARAM lpDrawItemStruct )
{
	DrawItem(reinterpret_cast<LPDRAWITEMSTRUCT>(lpDrawItemStruct));
	return 0;
}

void CCustomDrawListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	if ( (int)lpDrawItemStruct->itemID < 0 )
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	int nSavedDC = pDC->SaveDC();

	CRect rectClient;
	GetClientRect(rectClient);
	
	// * Somehow lpDrawItemStruct->rcItem gives wrong value for selected item when they are scrolled outside the listbox
	// and SelectClipRgn not works in this case, so I have to use memory dc to do "clipping"
	CRect rectItem(lpDrawItemStruct->rcItem);

	CSimpleMemDC memDC(*pDC, rectItem);
	CDC* pDrawDC = &memDC.GetDC();

	CFont* pFont = pDC->GetCurrentFont();
	CFont* pOldFont = pDrawDC->SelectObject(pFont);
	
	OnFillBackground(pDrawDC, rectItem, lpDrawItemStruct);
	
	if (OnDrawIcon(pDrawDC, rectItem, lpDrawItemStruct))
	{
		int cx, cy;
		ImageList_GetIconSize(GetImageList()->GetSafeHandle(), &cx, &cy);
		rectItem.left += cx + 4;
	}

	rectItem.left += 2;	// some gap to the text
	OnDrawText(pDrawDC, rectItem, lpDrawItemStruct);

	pDrawDC->SelectObject(pOldFont);

	pDC->RestoreDC(nSavedDC);
}

void CCustomDrawListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	
}

int CCustomDrawListBox::CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct )
{
	return 0;
}

CImageList* CCustomDrawListBox::SetImageList( CImageList* pImageList )
{
	CImageList* pOldImageList = m_pImageList;
	m_pImageList = pImageList;
	return pOldImageList;
}

CImageList* CCustomDrawListBox::GetImageList() const
{
	return m_pImageList;
}

BOOL CCustomDrawListBox::IsComboboxList()
{
	DWORD dwStyle = GetStyle();
	BOOL bIsComboListBox = dwStyle & LBS_COMBOBOX;
	if ( !bIsComboListBox )
	{
		// Note by the way, the class name of listbox in a combobox is "ComboLBox"
		CWnd* pParentWnd = GetParent();
		if (pParentWnd)
		{
			TCHAR szClassName[20] = {0};
			GetClassName(pParentWnd->GetSafeHwnd(), szClassName, 20);
			bIsComboListBox = _tcsicmp(szClassName, _T("ComboBox")) == 0;
		}
	}
	return bIsComboListBox;
}

void CCustomDrawListBox::OnFillBackground( CDC* pDC, CRect rect, LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	BOOL bIsWndEnabled = IsWindowEnabled();
	pDC->FillSolidRect(rect, ::GetSysColor(bIsWndEnabled ? COLOR_WINDOW : COLOR_BTNFACE));

	if ( !bIsWndEnabled )
		return;

	BOOL bIsComboListBox = IsComboboxList();
	
	CCustomDrawHelper itemDrawer;
	itemDrawer.m_bWndHasFocus		= bIsComboListBox || ::GetFocus() == GetSafeHwnd();
	itemDrawer.m_bIsHotItem			= ShouldDrawHotItem() && m_nHotItem == lpDrawItemStruct->itemID;
	itemDrawer.m_bFocusItem			= (lpDrawItemStruct->itemState & ODS_FOCUS) ? true : false;
	itemDrawer.m_bSelected			= (lpDrawItemStruct->itemState & ODS_SELECTED) ? true : false;
	itemDrawer.DrawItemBackground(pDC, rect);
}

BOOL CCustomDrawListBox::OnDrawIcon( CDC* pDC, CRect rect, LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	if (GetImageList())	// not thread safe
	{
		int nImageIndex = GetItemIconIndex(lpDrawItemStruct->itemID);
		if (nImageIndex >= 0)
		{
#ifdef _DEBUG
			int nImgCount = GetImageList()->GetImageCount();
			ASSERT(nImageIndex < nImgCount);
#endif // _DEBUG

			CPoint ptIcon = rect.TopLeft();
			//GetImageList()->Draw(pDC, nImageIndex, ptIcon, ILD_TRANSPARENT);

			int cx, cy;
			ImageList_GetIconSize(GetImageList()->GetSafeHandle(), &cx, &cy);

			// vertical center the icon
			int ny = (rect.Height() - cy) / 2;
			ptIcon.Offset(4, ny);

			HICON hIcon = GetImageList()->ExtractIcon(nImageIndex);
			VERIFY( ::DrawIconEx(pDC->GetSafeHdc(), ptIcon.x, ptIcon.y, hIcon, cx, cy, 0, NULL, DI_NORMAL) );
 			::DestroyIcon(hIcon);
			return TRUE;
		}
	}
	return FALSE;
}

void CCustomDrawListBox::OnDrawText( CDC* pDC, CRect rect, LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CString strText = GetItemText(lpDrawItemStruct->itemID);
	int nOldBkMode = pDC->SetBkMode(TRANSPARENT);

	COLORREF clrTxtColor = GetTextColor(lpDrawItemStruct);
	COLORREF clrOldTxtColor;
	if (clrTxtColor != CLR_INVALID)
		clrOldTxtColor = pDC->SetTextColor(clrTxtColor);

	const UINT nDTFormat = DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX;
	if ( GetTextDrawer() )
		GetTextDrawer()->Draw(pDC, strText, rect, nDTFormat);
	else
		pDC->DrawText(strText, strText.GetLength(), rect, nDTFormat);

	if (clrTxtColor != CLR_INVALID)
		pDC->SetTextColor(clrOldTxtColor);

	pDC->SetBkMode(nOldBkMode);
}

COLORREF CCustomDrawListBox::GetTextColor( LPDRAWITEMSTRUCT lpDIS )
{
	// Gray text does not looks good
#ifdef CUSTOMDRAW_GRADIENT
	if ( !IsWindowEnabled() )
		return ::GetSysColor(COLOR_GRAYTEXT);
#else
	if ( (lpDIS->itemState & ODS_SELECTED) && ::GetFocus() == GetSafeHwnd() )
	{
		return ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
#endif // CUSTOMDRAW_GRADIENT
	return CLR_INVALID;
}

CString CCustomDrawListBox::GetItemText( UINT nItem )
{
	CString str;
	GetText(nItem, str);
	return str;
}

int CCustomDrawListBox::GetItemIconIndex( UINT nItem )
{
	return -1;
}

int CCustomDrawListBox::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CCustomDrawListBoxBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	OnInitListBox();
	return 0;
}

void CCustomDrawListBox::PreSubclassWindow()
{
	CCustomDrawListBoxBase::PreSubclassWindow();
	
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == NULL)
	{
		OnInitListBox();
	}
}

BOOL CCustomDrawListBox::PreTranslateMessage( MSG* pMsg )
{
	if (GetCustomDrawToolTips().m_hWnd)
		GetCustomDrawToolTips().RelayEvent(pMsg);
	return CCustomDrawListBoxBase::PreTranslateMessage(pMsg);
}

BOOL CCustomDrawListBox::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT* pResult )
{
	UINT nID = pNMHDR->idFrom;
	
	// check if this is the automatic tooltip of the control
	if (nID == 0)
		return TRUE;	// do not allow display of automatic tooltip,
						// or our tooltip will disappear
	
	*pResult = 0;
	
	// get the mouse position
	const MSG* pMessage = GetCurrentMessage();
	ASSERT(pMessage);
	CPoint pt;
	pt = pMessage->pt;		// get the point from the message
	ScreenToClient(&pt);	// convert the point's coords to be relative to this control
	
	// see if the point falls onto a list item
	BOOL bOutSide;
	UINT nItem = ItemFromPoint(pt, bOutSide);
	
	if ( !bOutSide && ShouldShowToolTipForItem(nItem) )
	{
		CString strToolTip = GetToolTipLabelForItem(nItem);
		
		if (strToolTip.IsEmpty())
		{
			strToolTip = GetItemText(nItem);
		}
		if (!strToolTip.IsEmpty())
		{
			GetCustomDrawToolTips().SetText(pNMHDR, strToolTip);
			return TRUE;
		}	
	}
	
	return FALSE;	// we didn't handle the message, let the 
					// framework continue propagating the message
}

void CCustomDrawListBox::OnMouseMove( UINT nFlags, CPoint point )
{
	CCustomDrawListBoxBase::OnMouseMove(nFlags, point);

	if ( ShouldDrawHotItem() && !m_bMouseEventsTracked)
	{
		m_bMouseEventsTracked = TRUE;
		
		TRACKMOUSEEVENT trackmouseevent;
		trackmouseevent.cbSize		= sizeof(trackmouseevent);
		trackmouseevent.dwFlags		= TME_LEAVE;
		trackmouseevent.hwndTrack	= GetSafeHwnd();
		trackmouseevent.dwHoverTime	= HOVER_DEFAULT;
		//::AFXTrackMouse(&trackmouseevent);
		_TrackMouseEvent(&trackmouseevent);
	}
	
	// Find the subitem
	BOOL bOutSide;
	UINT nItem = ItemFromPoint(point, bOutSide);
	if ( bOutSide )
	{
		nItem = -1;
	}
		
	if ( ShouldDrawHotItem() && m_nHotItem != nItem )
	{
		int nOldHotItem = m_nHotItem;
		m_nHotItem = nItem;
		
		//LockSetRedraw(FALSE);

		CRect rect;
		GetItemRect(m_nHotItem, rect);
		InvalidateRect(rect, FALSE);	// don't erase background, less flicker

		GetItemRect(nOldHotItem, rect);
		InvalidateRect(rect, FALSE);	// don't erase background, less flicker
		//LockSetRedraw(TRUE);
		UpdateWindow();
	}
	
	if ( ShouldShowToolTipForItem(nItem) && GetCustomDrawToolTips().CheckDisplayToolTip((WPARAM)nItem) )
	{		
		// Remove the old tooltip (if available)
		int nToolCount = GetCustomDrawToolTips().GetToolCount();
		//TRACE("CCustomDrawListBox tooltip tool count: %d\n", nToolCount);
		if (nToolCount > 0)
		{
			// Not using CToolTipCtrl::DelTool() because it redirects the messages to CListBox parent
			// If we call DelTool(), you will see that the return value of GetToolCount() still keep increasing!
			//GetCustomDrawToolTips().DelTool(this);
			TOOLINFO ti = {0};
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_IDISHWND;		// Indicate that uId is handle to a control
			ti.uId = (UINT_PTR)m_hWnd;		// Handle to the control
			ti.hwnd = m_hWnd;				// Handle to window to receive the tooltip-messages
			ti.hinst = AfxGetInstanceHandle();
			GetCustomDrawToolTips().SendMessage(TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
			
			GetCustomDrawToolTips().Activate(FALSE);
		}
		
		// Add the new tooltip (if available)
		if ( !bOutSide )
		{
			// Not using CToolTipCtrl::AddTool() because it redirects the messages to CListBox parent
			//GetCustomDrawToolTips().AddTool(this);
			TOOLINFO ti = {0};
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_IDISHWND;		// Indicate that uId is handle to a control
			ti.uId = (UINT_PTR)m_hWnd;		// Handle to the control
			ti.hwnd = m_hWnd;				// Handle to window to receive the tooltip-messages
			ti.hinst = AfxGetInstanceHandle();
			ti.lpszText = LPSTR_TEXTCALLBACK;
			GetCustomDrawToolTips().SendMessage(TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
			
			PreAddToolTipForItem(nItem);
			
			GetCustomDrawToolTips().Activate(TRUE);
		}
	}
}

void CCustomDrawListBox::OnInitListBox()
{
	// Prevents the system from stripping ampersand characters from a string or terminating a string at a tab character
	VERIFY( GetCustomDrawToolTips().Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX) );
	GetCustomDrawToolTips().Activate(TRUE);

	EnableExplorerVisualStyles();
}

BOOL CCustomDrawListBox::ShouldShowToolTipForItem( int nItem )
{
	return TRUE;
}

CString CCustomDrawListBox::GetToolTipLabelForItem( int nItem )
{
	return GetItemText(nItem);
}

void CCustomDrawListBox::PreAddToolTipForItem( int nItem )
{
	// nothing to do
}

void CCustomDrawListBox::PreShowToolTipForItem( int nItem )
{
	// nothing to do
}

void CCustomDrawListBox::PrePopToolTip()
{
	// nothing to do
}

LRESULT CCustomDrawListBox::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	LRESULT result = Default();
	m_bMouseEventsTracked = FALSE;
	
	if (m_nHotItem >= 0)
	{
		CRect rect;
		GetItemRect(m_nHotItem, rect);
		m_nHotItem = -1;
		InvalidateRect(rect, FALSE);
	}
	
	return result;
}

LRESULT CCustomDrawListBox::OnCustomToolTipNotify( WPARAM wParam, LPARAM lParam )
{
	switch (wParam)
	{
	case CDTOOLTIP_ONBEFORE_SHOW:
		{
			int nItem = (int)GetCustomDrawToolTips().GetDisplayWParam();
			PreShowToolTipForItem(nItem);
		}
		break;
	case CDTOOLTIP_ONBEFORE_POP:
		PrePopToolTip();
		break;
	}
	return 0;
}

void CCustomDrawListBox::OnSetFocus( CWnd* pOldWnd )
{
	CCustomDrawListBoxBase::OnSetFocus(pOldWnd);
	
	if ( !IsComboboxList() )
	{
		RedrawWindow();
	}
}

void CCustomDrawListBox::OnKillFocus( CWnd* pNewWnd )
{
	CCustomDrawListBoxBase::OnKillFocus(pNewWnd);

	if ( !IsComboboxList() )
	{
		RedrawWindow();
	}
}

void CCustomDrawListBox::OnDestroy()
{
	OnDestroyListBox();
	CCustomDrawListBoxBase::OnDestroy();
}

/*----------------------------------------------------------------------------*/
/* CCustomDrawTreeCtrl
/*----------------------------------------------------------------------------*/
IMPLEMENT_DYNCREATE(CCustomDrawTreeCtrl, CCustomDrawTreeCtrlBase)

CCustomDrawTreeCtrl::CCustomDrawTreeCtrl()
	: m_hHotTreeItem(NULL)
	, m_bDrawHotItem(TRUE)
	, m_bCustomDraw(TRUE)
	, m_nLockDrawCount(0)
	, m_pTextDrawer(NULL)
	, m_bExplorerVisualStyle(FALSE)
	, m_bMouseEventsTracked(FALSE)
	, m_SelStyle(CDTSS_LABELRIGHT)
{
}

CCustomDrawTreeCtrl::~CCustomDrawTreeCtrl()
{
	
}

BEGIN_MESSAGE_MAP(CCustomDrawTreeCtrl, CCustomDrawTreeCtrlBase)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_CREATE()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_MESSAGE(WM_CDTOOLTIPCTRL_NOTIFY, OnCustomToolTipNotify)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
#ifdef FIX_TREECTRL_SCROLL_REDRAW_ISSUE
	ON_WM_HSCROLL()
#endif // FIX_TREECTRL_SCROLL_REDRAW_ISSUE
	ON_MESSAGE(WM_STYLECHANGED, OnStyleChanged)
END_MESSAGE_MAP()

LRESULT CCustomDrawTreeCtrl::EnableExplorerVisualStyles( bool bValue /*= true*/ )
{
	if (!IsThemeEnabled())
	{
		m_bExplorerVisualStyle = false;
		return S_FALSE;
	}
	
	if (!CheckOSVersion(0x0600))
	{
		m_bExplorerVisualStyle = false;
		return S_FALSE;
	}
	
	LRESULT rc = S_FALSE;
	if (bValue)
		rc = EnableWindowTheme(GetSafeHwnd(), L"TreeView", L"Explorer", NULL);
	else
		rc = EnableWindowTheme(GetSafeHwnd(), L"", L"", NULL);
	
	m_bExplorerVisualStyle = (bValue && rc == S_OK);
	
	return rc;
}

void CCustomDrawTreeCtrl::SetDrawHotItem( BOOL bDraw /*= TRUE*/, BOOL bReDraw /*= TRUE*/ )
{
	m_bDrawHotItem = bDraw;
	if ( bReDraw )
	{
		RedrawWindow();
	}
}

void CCustomDrawTreeCtrl::EnableCustomDraw( BOOL bEnable /*= TRUE*/, BOOL bRedraw /*= FALSE*/ )
{
	m_bCustomDraw = bEnable;
	if ( bRedraw )
		RedrawWindow();
}

void CCustomDrawTreeCtrl::SetSelStyle( SelStyle selStyle )
{
	m_SelStyle = selStyle;
	RedrawSelectedItem();
}

// MSDN states: TVS_FULLROWSELECT style cannot be used in conjunction with the TVS_HASLINES style. 
// http://msdn.microsoft.com/en-us/library/bb760013(VS.85).aspx
static inline BOOL IsFullRowSelectTreeCtrl(DWORD dwStyle)
{
	return (dwStyle & TVS_FULLROWSELECT) && !(dwStyle & TVS_HASLINES);
}

BOOL CCustomDrawTreeCtrl::HasValidFullRowSelectStyle()
{
	return IsFullRowSelectTreeCtrl( GetStyle() );	
}

LRESULT CCustomDrawTreeCtrl::OnStyleChanged( WPARAM wp, LPARAM lp )
{
	int nStyleType = (int) wp;
	LPSTYLESTRUCT lpStyleStruct = (LPSTYLESTRUCT) lp;
	CCustomDrawTreeCtrlBase::OnStyleChanged(nStyleType, lpStyleStruct);
	if ( nStyleType & GWL_STYLE )
	{
		ASSERT( lpStyleStruct );
		BOOL bOldFullRowSel	= IsFullRowSelectTreeCtrl(lpStyleStruct->styleOld);
		BOOL bFullRowSel	= IsFullRowSelectTreeCtrl(lpStyleStruct->styleNew);
		if ( bFullRowSel ^ bOldFullRowSel )
		{
			SetSelStyle( bFullRowSel ? CDTSS_FULLROWSELECT : (CDTSS_FULLROWSELECT == m_SelStyle ? CDTSS_LABELRIGHT : m_SelStyle) );
		}
	}
	return 0;
}

BOOL CCustomDrawTreeCtrl::OnEraseBkgnd( CDC* pDC )
{
	//return CCustomDrawTreeCtrlBase::OnEraseBkgnd(pDC);
	return TRUE;	// flicker free!
}

void CCustomDrawTreeCtrl::OnPaint()
{
	CRect rect;
	if ( GetUpdateRect(&rect, FALSE) && !rect.IsRectEmpty() )
	{
		// double buffered drawing.
		CPaintDC dc(this);
		CSimpleMemDC memDC(dc, dc.m_ps.rcPaint);
		DefWindowProc(WM_PRINT, (WPARAM)memDC.GetDC().m_hDC, (LPARAM)(PRF_CLIENT|PRF_CHECKVISIBLE));
	}
}

void CCustomDrawTreeCtrl::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult )
{
	LPNMTVCUSTOMDRAW pNMTVCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	if ( !pNMTVCD )
		return;
	switch (pNMTVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		if ( IsCustomDrawEnabled() )
			*pResult = CDRF_NOTIFYITEMDRAW;
		else
			*pResult = CDRF_DODEFAULT;
		break;
	case CDDS_ITEMPREPAINT:
		{
			// set the background and foreground color of the item 
			// to the background,
			// so you don't see the default drawing of the text
			pNMTVCD->clrTextBk = pNMTVCD->clrText = ::GetSysColor(COLOR_WINDOW);

			if ( CDTSS_FULLROWSELECT == m_SelStyle )
			{
				HTREEITEM hItem = reinterpret_cast<HTREEITEM>(pNMTVCD->nmcd.dwItemSpec);
				CDC* pDC = CDC::FromHandle(pNMTVCD->nmcd.hdc);
				
				CRect rect = GetFillRect(hItem);
				OnFillBackground(pDC, rect, hItem, pNMTVCD->nmcd.uItemState);
			}
			
			// we want to get the CDDS_ITEMPOSTPAINT notification
			*pResult = CDRF_NOTIFYPOSTPAINT;
		}
		break;
	case CDDS_ITEMPOSTPAINT:
		{
			HTREEITEM hItem = reinterpret_cast<HTREEITEM>(pNMTVCD->nmcd.dwItemSpec);
			CDC* pDC = CDC::FromHandle(pNMTVCD->nmcd.hdc);

			// draw the item
			OnDrawTreeItem(pDC, hItem, pNMTVCD->nmcd.uItemState);
			//*pResult = CDRF_DODEFAULT;
			*pResult = CDRF_SKIPDEFAULT;
		}
		break;
	default:
		*pResult = CDRF_DODEFAULT;
		break;
	}
}

CRect CCustomDrawTreeCtrl::GetFillRect( HTREEITEM hItem )
{
	CRect rcText;
	GetItemRect(hItem, rcText, TRUE);

	if ( CDTSS_LABEL == m_SelStyle )
		return rcText;

	CRect rcBounds;
	GetItemRect(hItem, &rcBounds, FALSE);

	SCROLLINFO si = {0};
	GetScrollInfo(SB_HORZ, &si, SIF_POS|SIF_RANGE);
	
	rcBounds.right = max(rcBounds.right, si.nMax - si.nPos);

	if ( m_bExplorerVisualStyle )
	{
		rcBounds.left = min(rcBounds.left, -si.nPos);
	}
	else
	{
		rcBounds.left = rcText.left;
	}

	if ( CDTSS_FULLROWSELECT == m_SelStyle )
		return rcBounds;

	ASSERT( CDTSS_LABELRIGHT == m_SelStyle );

	rcText.right = rcBounds.right;

	return rcText;
}

CRect CCustomDrawTreeCtrl::GetSelectRect( HTREEITEM hItem )
{
// 	CRect rcSel;
// 	GetItemRect(hItem, &rcSel, m_bDragSelTextOnly);
// 	return rcSel;
	return GetFillRect(hItem);
}

BOOL CCustomDrawTreeCtrl::PtInItemSelectRect( HTREEITEM hItem, CPoint pt )
{
	CRect rcSelect = GetSelectRect(hItem);
	return rcSelect.PtInRect(pt);
}

//#define DRAW_TREE_ITEM_VIA_MEMDC	// no flicker already, no need to double buffer it

void CCustomDrawTreeCtrl::OnDrawTreeItem( CDC* pDC, HTREEITEM hItem, UINT nItemState )
{
	CRect rcItem;
	GetItemRect(hItem, rcItem, TRUE);

	CRect rcFill = GetFillRect(hItem);
	rcItem.right = rcFill.right;

#ifdef DRAW_TREE_ITEM_VIA_MEMDC
	CSimpleMemDC memDC(*pDC, rcItem);
	CDC* pDrawDC = &memDC.GetDC();
#else
	CDC*& pDrawDC = pDC;
#endif // DRAW_TREE_ITEM_VIA_MEMDC

	CFont* pFont = pDC->GetCurrentFont();
	CFont* pOldFont = pDrawDC->SelectObject(pFont);

	OnFillBackground(pDrawDC, rcFill, hItem, nItemState);
	
	//rcItem.left += 5;	// makes some gap to the text.
	OnDrawText(pDrawDC, rcItem, hItem, nItemState, FALSE);

	pDrawDC->SelectObject(pOldFont);
}

void CCustomDrawTreeCtrl::OnFillBackground( CDC* pDC, CRect rect, HTREEITEM hItem, UINT nItemState )
{
	BOOL bIsWndEnabled = IsWindowEnabled();
	// Erase the background
	pDC->FillSolidRect(rect, ::GetSysColor(bIsWndEnabled ? COLOR_WINDOW : COLOR_BTNFACE));

	if ( !bIsWndEnabled )
		return;

	//rect.left += 2;	// make some gap

	CCustomDrawHelper itemDrawer;
	itemDrawer.m_bWndHasFocus		= ::GetFocus() == GetSafeHwnd();
	itemDrawer.m_bFocusItem			= (nItemState & CDIS_FOCUS) ? true : false;
	itemDrawer.m_bSelected			= (nItemState & CDIS_SELECTED) ? true : false;
	itemDrawer.m_bIsHotItem			= m_bDrawHotItem && m_hHotTreeItem == hItem;
	itemDrawer.m_bDrawBorderWhenFill	= true;
	itemDrawer.DrawItemBackground(pDC, rect);
}

CSize CCustomDrawTreeCtrl::OnDrawText( CDC* pDC, CRect rect, HTREEITEM hItem, UINT nItemState, BOOL bCalcOnly /*= FALSE*/ )
{
	CString strItem = GetItemText(hItem);
	
	COLORREF clrTxtColor;
	if (nItemState & CDIS_DISABLED)
		clrTxtColor = ::GetSysColor(COLOR_GRAYTEXT);
#ifndef CUSTOMDRAW_GRADIENT
	else if ((nItemState & CDIS_SELECTED) && ::GetFocus() == GetSafeHwnd() )
		clrTxtColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
#endif // CUSTOMDRAW_GRADIENT
	else
		clrTxtColor = ::GetSysColor(COLOR_WINDOWTEXT);
	
	return DrawItemText(pDC, rect, hItem, strItem, clrTxtColor, bCalcOnly);
}

CSize CCustomDrawTreeCtrl::DrawItemText( CDC* pDC, CRect rect, HTREEITEM hItem, LPCTSTR lpcszText, COLORREF clrTxtColor, BOOL bCalcOnly /*= FALSE*/ )
{
	int nOldBkMode			= pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldTxtColor	= pDC->SetTextColor(clrTxtColor);
	
	DRAWTEXTPARAMS dtp = {sizeof(DRAWTEXTPARAMS)};
	dtp.iTabLength = 4;
	
	UINT nBoldState = GetItemState(hItem, TVIS_BOLD);
	BOOL bBoldText = nBoldState & TVIS_BOLD;
	
	CFont* pOldFont = NULL;
	CFont fnt;
	if ( bBoldText )
	{
		CFont* pCurFont = pDC->GetCurrentFont();
		LOGFONT lf = {0};
		pCurFont->GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;	// make it bold
		
		fnt.CreateFontIndirect(&lf);
		pOldFont = pDC->SelectObject(&fnt);
	}
	
	UINT uDTFormat = DT_VCENTER|DT_SINGLELINE|DT_TABSTOP|DT_EXPANDTABS|DT_END_ELLIPSIS;
	if ( bCalcOnly )
	{
		uDTFormat |= DT_CALCRECT;
	}

	int nTextHeight;
	if ( GetTextDrawer() )
		nTextHeight = GetTextDrawer()->Draw(pDC, lpcszText, rect, uDTFormat);
	else
		nTextHeight = DrawTextEx(pDC->GetSafeHdc(), (LPTSTR)lpcszText, _tcslen(lpcszText), rect, uDTFormat, &dtp);

	CSize sizeText(rect.Width(), nTextHeight);
	
	if ( bBoldText )
	{
		pDC->SelectObject(pOldFont);
		fnt.DeleteObject();
	}
	
	pDC->SetTextColor(clrOldTxtColor);
	pDC->SetBkMode(nOldBkMode);
	return sizeText;
}

void CCustomDrawTreeCtrl::InvalidateItem( HTREEITEM hItem, BOOL bErase /*= TRUE*/ )
{
	CRect rcItem = GetFillRect(hItem);
	InvalidateRect(rcItem, bErase);
}

void CCustomDrawTreeCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	CCustomDrawTreeCtrlBase::OnMouseMove(nFlags, point);

	UINT uHitTestFlags = 0;
	HTREEITEM hItem = HitTest(point, &uHitTestFlags);

	if ( IsDrawHotItem() )
	{
		if (hItem)
		{
			CRect rcSel = GetFillRect(hItem);
			if ( !rcSel.PtInRect(point) )
				hItem = m_hHotTreeItem;
		}
		if ( hItem != m_hHotTreeItem )
		{
			if ( !m_bMouseEventsTracked )
			{
				m_bMouseEventsTracked = TRUE;
				
				TRACKMOUSEEVENT trackmouseevent;
				trackmouseevent.cbSize		= sizeof(trackmouseevent);
				trackmouseevent.dwFlags		= TME_LEAVE;
				trackmouseevent.hwndTrack	= GetSafeHwnd();
				trackmouseevent.dwHoverTime	= HOVER_DEFAULT;
				//::AFXTrackMouse(&trackmouseevent);
				_TrackMouseEvent(&trackmouseevent);
			}
			// It seems that SetRedraw() will actually produce flickering, so better do invalidate instead.
			if ( m_hHotTreeItem != NULL )
			{
				InvalidateItem(m_hHotTreeItem, TRUE);
			}
			if ( hItem != NULL )
			{
				InvalidateItem(hItem, TRUE);
			}
			m_hHotTreeItem = hItem;
			UpdateWindow();
		}
	}

	// Find the item
	if ( ShouldShowToolTipForCell(hItem, uHitTestFlags)
		&& GetCustomDrawToolTips().CheckDisplayToolTip( (WPARAM)hItem, (LPARAM)uHitTestFlags )
		)
	{
		// Remove the old tooltip (if available)
		int nToolCount = GetCustomDrawToolTips().GetToolCount();
		//TRACE("CCustomDrawTreeCtrl tooltip tool count: %d\n", nToolCount);
		if (nToolCount > 0)
		{
			// Not using CToolTipCtrl::DelTool() because it redirects the messages to CTreeCtrl parent
			// If we call DelTool(), you will see that the return value of GetToolCount() still keep increasing!
			//GetCustomDrawToolTips().DelTool(this);
			TOOLINFO ti = {0};
			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_IDISHWND;	// Indicate that uId is handle to a control
			ti.uId = (UINT_PTR)m_hWnd;	// Handle to the control
			ti.hwnd = m_hWnd;			// Handle to window to receive the tooltip-messages
			ti.hinst = AfxGetInstanceHandle();
			GetCustomDrawToolTips().SendMessage(TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
			
			GetCustomDrawToolTips().Activate(FALSE);
		}
		// Not using CToolTipCtrl::AddTool() because it redirects the messages to CTreeCtrl parent
		//GetCustomDrawToolTips().AddTool(this);
		TOOLINFO ti = {0};
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_IDISHWND;	// Indicate that uId is handle to a control
		ti.uId = (UINT_PTR)m_hWnd;	// Handle to the control
		ti.hwnd = m_hWnd;			// Handle to window to receive the tooltip-messages
		ti.hinst = AfxGetInstanceHandle();
		ti.lpszText = LPSTR_TEXTCALLBACK;
		GetCustomDrawToolTips().SendMessage(TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

		PreAddToolTipForItem(hItem, uHitTestFlags);
		GetCustomDrawToolTips().Activate(TRUE);
	}
}

LRESULT CCustomDrawTreeCtrl::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	LRESULT result = Default();
	m_bMouseEventsTracked = FALSE;
	
	if (m_hHotTreeItem)
	{
		InvalidateItem(m_hHotTreeItem, FALSE);
		m_hHotTreeItem = NULL;
	}
	
	return result;
}

void CCustomDrawTreeCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	if ( CDTSS_LABELRIGHT == m_SelStyle && !HasValidFullRowSelectStyle() )
	{
		UINT uHitTestFlags = 0;
		HTREEITEM hItem = HitTest(point, &uHitTestFlags);
		if ( hItem != NULL && (uHitTestFlags & TVHT_ONITEMRIGHT) )
		{
			Select(hItem, TVGN_CARET);
		}
	}
	CCustomDrawTreeCtrlBase::OnLButtonDown(nFlags, point);
}

int CCustomDrawTreeCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CCustomDrawTreeCtrlBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	OnInitTreeCtrl();
	return 0;
}

void CCustomDrawTreeCtrl::PreSubclassWindow()
{
	CCustomDrawTreeCtrlBase::PreSubclassWindow();

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == NULL)
	{
		OnInitTreeCtrl();
	}
}

BOOL CCustomDrawTreeCtrl::PreTranslateMessage( MSG* pMsg )
{
	if (GetCustomDrawToolTips().m_hWnd)
		GetCustomDrawToolTips().RelayEvent(pMsg);
	return CCustomDrawTreeCtrlBase::PreTranslateMessage(pMsg);
}

void CCustomDrawTreeCtrl::OnInitTreeCtrl()
{
	// disables the automatic tooltip feature of tree view controls
	ModifyStyle(0, TVS_NOTOOLTIPS);
	
	// Enable our own tooltip-ctrl and make it show tooltip even if not having focus
	// Prevents the system from stripping ampersand characters from a string or terminating a string at a tab character
	VERIFY( GetCustomDrawToolTips().Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX) );
	GetCustomDrawToolTips().Activate(TRUE);

	EnableExplorerVisualStyles(TRUE);

	if ( HasValidFullRowSelectStyle() )
	{
		m_SelStyle = CDTSS_FULLROWSELECT;
	}
}

BOOL CCustomDrawTreeCtrl::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	UINT nID = pNMHDR->idFrom;
	
	// check if this is the automatic tooltip of the control
	if (nID == 0)
		return TRUE;	// do not allow display of automatic tooltip,
						// or our tooltip will disappear
	
	*pResult = 0;
	
	// get the mouse position
	const MSG* pMessage = GetCurrentMessage();
	ASSERT(pMessage);
	CPoint pt;
	pt = pMessage->pt;		// get the point from the message
	ScreenToClient(&pt);	// convert the point's coords to be relative to this control
	
	// see if the point falls onto a item
	UINT uHitTestFlags = 0;
	HTREEITEM hItem = HitTest(pt, &uHitTestFlags);
	if ( hItem != NULL && (uHitTestFlags & (TVHT_ONITEM|TVHT_ONITEMRIGHT)) )
	{
		CString strToolTip = GetToolTipLabelForItem(hItem, uHitTestFlags);
		
		if ( !strToolTip.IsEmpty() )
		{
			GetCustomDrawToolTips().SetText(pNMHDR, strToolTip);
			return TRUE;
		}
		
	}
	return FALSE;	// we didn't handle the message, let the 
					// framework continue propagating the message
}

BOOL CCustomDrawTreeCtrl::ShouldShowToolTipForCell( HTREEITEM hItem, UINT uHitTestFlags )
{
	UINT uHT = TVHT_ONITEM | (CDTSS_LABEL == m_SelStyle ? 0 : TVHT_ONITEMRIGHT);
	return hItem != NULL && (uHitTestFlags & uHT);
}

CString CCustomDrawTreeCtrl::GetToolTipLabelForItem( HTREEITEM hItem, UINT uHitTestFlags )
{
	return GetItemText(hItem);
}

void CCustomDrawTreeCtrl::PreAddToolTipForItem( HTREEITEM hItem, UINT uHitTestFlags )
{
	// nothing to do
}

void CCustomDrawTreeCtrl::PreShowToolTipForItem( HTREEITEM hItem, UINT uHitTestFlags )
{
	// nothing to do
}

void CCustomDrawTreeCtrl::PrePopToolTip()
{
	// nothing to do
}

LRESULT CCustomDrawTreeCtrl::OnCustomToolTipNotify( WPARAM wParam, LPARAM lParam )
{
	switch (wParam)
	{
	case CDTOOLTIP_ONBEFORE_SHOW:
		{
			HTREEITEM hItem		= (HTREEITEM)GetCustomDrawToolTips().GetDisplayWParam();
			UINT uHitTestFlags	= (UINT)GetCustomDrawToolTips().GetDisplayLParam();
			PreShowToolTipForItem(hItem, uHitTestFlags);
		}
		break;
	case CDTOOLTIP_ONBEFORE_POP:
		PrePopToolTip();
		break;
	}
	return 0;
}

#ifdef FIX_TREECTRL_SCROLL_REDRAW_ISSUE
void CCustomDrawTreeCtrl::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	CCustomDrawTreeCtrlBase::OnHScroll(nSBCode, nPos, pScrollBar);

	if ( IsDrawHotItem() && m_hHotTreeItem )
	{
		InvalidateItem(m_hHotTreeItem, FALSE);
	}

	RedrawSelectedItem();
}
#endif // FIX_TREECTRL_SCROLL_REDRAW_ISSUE

void CCustomDrawTreeCtrl::RedrawSelectedItem(BOOL bErase /*= FALSE*/)
{
	HTREEITEM hSelItem = GetSelectedItem();
	if ( hSelItem )
	{
		InvalidateItem(hSelItem, FALSE);
	}
}

/*----------------------------------------------------------------------------*/
/* class CTriCheckStateTreeCtrl
/*----------------------------------------------------------------------------*/

IMPLEMENT_DYNCREATE(CTriCheckStateTreeCtrl, CTriCheckStateTreeCtrlBase)

#ifndef STATEIMAGEMASKTOINDEX
	#define STATEIMAGEMASKTOINDEX(i) ((i) >> 12)
#endif // STATEIMAGEMASKTOINDEX

#define TVICON_W	13
#define TVICON_H	13

CTriCheckStateTreeCtrl::CTriCheckStateTreeCtrl()
	: m_uClickHitTestFlags(0)
{
	
}

CTriCheckStateTreeCtrl::~CTriCheckStateTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CTriCheckStateTreeCtrl, CTriCheckStateTreeCtrlBase)
	//{{AFX_MSG_MAP(CTriCheckStateTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClickItem)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTriCheckStateTreeCtrl::OnInitTreeCtrl()
{
	CTriCheckStateTreeCtrlBase::OnInitTreeCtrl();
	if ( GetSafeHwnd() )
	{
		VERIFY( InitCheckBoxImage() );
		SetImageList(&m_ChkBoxImgList, TVSIL_STATE);
	}
}

//#define CUSTOMDRAW_CHECKBOX

BOOL CTriCheckStateTreeCtrl::InitCheckBoxImage()
{
	int cx = TVICON_W;	//GetSystemMetrics(SM_CXICON);
	int cy = TVICON_H;	//GetSystemMetrics(SM_CYICON);

	CMemBitmap bitmapDrawer;
	if ( !bitmapDrawer.BeginDraw(cx * 4, cy) )
	{
		return FALSE;
	}
	CDC& dcMemory = bitmapDrawer.GetDC();

	CRect rect(0, 0, cx, cy);

	HTHEME hTheme = OOpenThemeData(m_hWnd, VSCLASS_BUTTON);
	if ( hTheme )
	{
		rect.OffsetRect(cx, 0);

		const int stateIds[] = {CBS_UNCHECKEDNORMAL, CBS_MIXEDNORMAL, CBS_CHECKEDNORMAL};
		for (int ii = TTCS_UNCHECKED; ii <= TTCS_CHECKED; ++ii)
		{
			ODrawThemeBackground(hTheme, dcMemory, BP_CHECKBOX, stateIds[ii-TTCS_UNCHECKED], rect, NULL);
			rect.OffsetRect(cx, 0);
		}
		OCloseThemeData(hTheme);
	}
	else
	{
		CDC& dcMemory = bitmapDrawer.GetDC();
		
		CRect rect(0, 0, cx, cy);
		CBrush brBorder(RGB(28,81,128));
		for (int ii = TTCS_NONE; ii <= TTCS_CHECKED; ++ii)
		{
			// Fill the background first
		#ifdef CUSTOMDRAW_GRADIENT
			FillGradient(dcMemory.GetSafeHdc(), rect, RGB(223, 222, 215), RGB(255, 255, 255), GFT_DIAG_TL_BR);
		#else
			dcMemory.FillSolidRect(rect, RGB(255,255,255));
		#endif // CUSTOMDRAW_GRADIENT
			// Draw the border
			dcMemory.FrameRect(rect, &brBorder);
			rect.OffsetRect(cx, 0);
		}
		
		// Draw the icon for partial checked button
		rect.left = cx * TTCS_PARTIALCHECKED;
		rect.right = rect.left + cx;
		CRect rectPartial(rect);
		rectPartial.DeflateRect(3, 3);
		dcMemory.FillSolidRect(rectPartial, RGB(115, 193, 114));
		
		// Draw the icon for normal checked button
		rect.left = cx * TTCS_CHECKED;
		rect.right = rect.left + cx;
		POINT ptsTick[3] = {
			{rect.left+3, rect.top+6},
			{rect.left+5, rect.top+8},
			{rect.left+9, rect.top+4}
		};
		
		// Draw the tick
		CPen penTick(PS_SOLID, 3, RGB(33, 161, 33));
		CPen* pOldPen = dcMemory.SelectObject(&penTick);
		
		dcMemory.Polyline(ptsTick, 3);
		
		dcMemory.SelectObject(pOldPen);
	}
	
	// Finish the bitmap
	bitmapDrawer.EndDraw();
	
	m_ChkBoxImgList.Create(cx, cy, ILC_COLOR24, 0, 1);
	int nBmpIndex = m_ChkBoxImgList.Add( bitmapDrawer.GetBitmapPtr(), (CBitmap*)NULL );
	ASSERT(nBmpIndex >= 0);

#ifdef _DEBUG
	int nImgCount = m_ChkBoxImgList.GetImageCount();
	ASSERT(nImgCount > 0);
#endif // _DEBUG
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTriCheckStateTreeCtrl message handlers

void CTriCheckStateTreeCtrl::OnClickItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_uClickHitTestFlags & TVHT_ONITEMSTATEICON)
		*pResult = 1;	// Mark message as handled and suppress default handling,
						// otherwise the original tree control would modify the check state.
	else
		*pResult = 0;
}

void CTriCheckStateTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	HTREEITEM hItem = HitTest(point, &m_uClickHitTestFlags);
	if ( hItem != NULL && (m_uClickHitTestFlags & TVHT_ONITEMSTATEICON) )
	{
		OnClickItemStateIcon(hItem);
	}
	
	CTriCheckStateTreeCtrlBase::OnLButtonDown(nFlags, point);
}

void CTriCheckStateTreeCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	HTREEITEM hItem = HitTest(point, &m_uClickHitTestFlags);
	if ( hItem != NULL && (m_uClickHitTestFlags & (TVHT_ONITEMRIGHT|TVHT_ONITEMLABEL)) )
	{
		OnDblClkItem(hItem);
	}
	
	CTriCheckStateTreeCtrlBase::OnLButtonDblClk(nFlags, point);
}

void CTriCheckStateTreeCtrl::OnClickItemStateIcon( HTREEITEM hItem )
{
	TriCheckState tcs = GetCheckState(hItem);
	if (tcs != TTCS_NONE)
		ToggleCheck(hItem);
}

void CTriCheckStateTreeCtrl::ToggleCheckSelectedItem()
{
	HTREEITEM hItem = GetSelectedItem();
	if ( hItem != NULL )
	{
		TriCheckState tcs = GetCheckState(hItem);
		if (tcs != TTCS_NONE)
		{
			ToggleCheck(hItem);
		}
	}
}

void CTriCheckStateTreeCtrl::DeleteSelectedItem()
{
	HTREEITEM hItem = GetSelectedItem();
	if ( hItem != NULL )
	{
		DeleteItem(hItem);
	}
}

void CTriCheckStateTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
 	if (VK_SPACE == nChar)
 	{
		ToggleCheckSelectedItem();
	}
	else if ( VK_DELETE == nChar )
	{
		DeleteSelectedItem();
	}
	else
		CTriCheckStateTreeCtrlBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTriCheckStateTreeCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = reinterpret_cast<TV_KEYDOWN*>(pNMHDR);
	
	*pResult = 0;
}

BOOL CTriCheckStateTreeCtrl::SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask, BOOL bSearchChild /*= TRUE*/)
{
	if ( (nStateMask & TVIS_STATEIMAGEMASK) && !CanCheckItem(hItem) )
		return FALSE;
	BOOL bReturn = CTriCheckStateTreeCtrlBase::SetItemState( hItem, nState, nStateMask );

	if ( nStateMask & TVIS_STATEIMAGEMASK )
	{
		TriCheckState iState = (TriCheckState)STATEIMAGEMASKTOINDEX(nState);
		if (iState != TTCS_NONE)
		{
			if (bSearchChild)
			{
				TravelCheckChildren(hItem, iState);
			}
			TravelCheckSiblingAndParent(hItem, iState);
		}
	}
	return bReturn;
}

BOOL CTriCheckStateTreeCtrl::SetCheckState( HTREEITEM hItem, TriCheckState tcs /*= TTCS_CHECKED*/ )
{
	return SetItemState(hItem, INDEXTOSTATEIMAGEMASK(tcs), TVIS_STATEIMAGEMASK);
}

TriCheckState CTriCheckStateTreeCtrl::GetCheckState( HTREEITEM hItem ) const
{
	return (TriCheckState)STATEIMAGEMASKTOINDEX(GetItemState( hItem, TVIS_STATEIMAGEMASK ));
}

BOOL CTriCheckStateTreeCtrl::SetCheck( HTREEITEM hItem, BOOL bCheck /*= TRUE*/ )
{
	return SetCheckState(hItem, bCheck ? TTCS_CHECKED : TTCS_UNCHECKED);
}

BOOL CTriCheckStateTreeCtrl::GetCheck( HTREEITEM hItem ) const
{
	TriCheckState tcs = GetCheckState(hItem);
	return TTCS_CHECKED == tcs || TTCS_PARTIALCHECKED == tcs;
}

BOOL CTriCheckStateTreeCtrl::DeleteItem( HTREEITEM hItem )
{
	HTREEITEM hParentItem = GetParentItem(hItem);
	BOOL bRet = CTriCheckStateTreeCtrlBase::DeleteItem(hItem);
	if ( GetDeleteParentIfLastChild() )
	{
		while ( hParentItem != NULL )
		{
			HTREEITEM hSiblingItem = GetChildItem(hParentItem);
			if (NULL == hSiblingItem)
			{
				// The parent item has no child left
				HTREEITEM hTmpParentItem = hParentItem;
				hParentItem = GetParentItem(hParentItem);
				CTriCheckStateTreeCtrlBase::DeleteItem(hTmpParentItem);
			}
			else
				break;
		}
	}
	if ( hParentItem != NULL )
	{
		TriCheckState tcs = GetCheckState(hParentItem);
		if ( TTCS_PARTIALCHECKED == tcs )
		{
			HTREEITEM hChildItem = GetChildItem(hParentItem);
			ASSERT(hChildItem != NULL);	// it must has been deleted by the above code!
			tcs = GetCheckState(hChildItem);
			TravelCheckSiblingAndParent(hChildItem, tcs);
		}
	}
	return bRet;
}

void CTriCheckStateTreeCtrl::TravelCheckChildren(HTREEITEM hItem, TriCheckState nState)
{
	HTREEITEM hChildItem = GetChildItem(hItem);
	while (hChildItem != NULL)
	{
		TriCheckState nChildState = GetCheckState(hChildItem);
		if ( nChildState != TTCS_NONE && CanCheckItem(hChildItem) )
		{
			CTriCheckStateTreeCtrlBase::SetItemState( hChildItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK );
		}
		// recursively check its children
		TravelCheckChildren(hChildItem, nState);
		hChildItem = GetNextSiblingItem(hChildItem);
	}
}

void CTriCheckStateTreeCtrl::TravelCheckSiblingAndParent(HTREEITEM hItem, TriCheckState nState)
{
	HTREEITEM hParentItem = GetParentItem(hItem);
	if ( NULL == hParentItem )
		return;
	// Check the status of all the sibling node
	HTREEITEM hSiblingItem = GetChildItem(hParentItem);
	while (hSiblingItem != NULL)
	{
		TriCheckState nSiblingState = GetCheckState(hSiblingItem);
		// If any of them is different with hItem, we have to do partial check for all of its parents.
		if ( nSiblingState != nState && nSiblingState != TTCS_NONE)
		{
			while (hParentItem != NULL)
			{
				TriCheckState nParentState = GetCheckState( hParentItem );
				if ( nParentState != TTCS_NONE && CanCheckItem(hParentItem) )
				{
					CTriCheckStateTreeCtrlBase::SetItemState( hParentItem, INDEXTOSTATEIMAGEMASK(TTCS_PARTIALCHECKED), TVIS_STATEIMAGEMASK );
				}
				hParentItem = GetParentItem(hParentItem);
			}
			return;	// Done.
		}
		hSiblingItem = GetNextSiblingItem(hSiblingItem);
	}
	// If we come to here, that means we still need to check the parent's sibling
	TriCheckState nParentState = GetCheckState( hParentItem );
	if ( nParentState != TTCS_NONE && CanCheckItem(hParentItem) )
	{
		CTriCheckStateTreeCtrlBase::SetItemState( hParentItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK );
	}
	TravelCheckSiblingAndParent(hParentItem, nState);
}

void CTriCheckStateTreeCtrl::OnDblClkItem( HTREEITEM hItem )
{
	TRACE("CTriCheckStateTreeCtrl::OnDblClkItem\n");
}

BOOL CTriCheckStateTreeCtrl::CanCheckItem( HTREEITEM hItem )
{
	return TRUE;
}

/*----------------------------------------------------------------------------*/
/* CMultiSelTriCheckTreeCtrl
/*----------------------------------------------------------------------------*/

#ifndef SPI_GETFLATMENU
	#define SPI_GETFLATMENU	0x1022	// Determines whether native User menus have flat menu appearance. 
									// The pvParam parameter must point to a BOOL variable that returns
									// TRUE if the flat menu appearance is set, or FALSE otherwise. 
#endif

#ifndef COLOR_MENUHILIGHT 
	#define COLOR_MENUHILIGHT 29	// The color used to highlight menu items when the menu appears as
									// a flat menu (see SystemParametersInfo). The highlighted menu item
									// is outlined with COLOR_HIGHLIGHT.
#endif

// Don't bother why this coded like this
static COLORREF GetSelectionFillColor(BYTE byPercent, COLORREF clrHiLi)
{
	BYTE byRVal = GetRValue(clrHiLi) + byPercent * (255 - GetRValue(clrHiLi)) / 100;
	BYTE byGVal = GetGValue(clrHiLi) + byPercent * (255 - GetGValue(clrHiLi)) / 100;
	BYTE byBVal = GetBValue(clrHiLi) + byPercent * (255 - GetBValue(clrHiLi)) / 100;
	return RGB(byRVal, byGVal, byBVal);
}

static COLORREF GetSelectionFillColor()
{
	BOOL bFlatMenu = TRUE;
	SystemParametersInfo(SPI_GETFLATMENU, 0, &bFlatMenu, 0);
	if (bFlatMenu)
	{
		return ::GetSysColor(COLOR_MENUHILIGHT);
	}
	COLORREF clrHiLight = ::GetSysColor(COLOR_HIGHLIGHT);	// Color of item(s) selected in a control.
	return GetSelectionFillColor(60, clrHiLight);
}

#define HasShiftFlag()				(nFlags & MK_SHIFT)
#define HasCtrlFlag()				(nFlags & MK_CONTROL)
#define HasCtrlOrShiftFlag()		(nFlags & (MK_CONTROL|MK_SHIFT))

COLORREF CMultiSelTriCheckTreeCtrl::s_clrSelFill = GetSelectionFillColor();

// InvalidateRect() & UpdateWindow()
//#define DRAW_SELECTION_ON_PAINT

#define MINIMIZE_MULTREE_UPDATE_SEL_AREA	// try to calculate the minimal update area to reduce drawing time.

CMultiSelTriCheckTreeCtrl::CMultiSelTriCheckTreeCtrl()
{
	m_rcSelBox.SetRectEmpty();
	m_bPendingDragSel	= FALSE;
	m_bDuringDragSel	= FALSE;
	m_bMultiSel			= TRUE;
	
	m_clrSelBorder		= RGB_SELECTION_BORDER;
	m_clrSelFill		= s_clrSelFill;
	m_hSelectAnchor		= NULL;
	m_bEmulated			= FALSE;
	m_bAlphaBlendedSelection	= TRUE;
}

CMultiSelTriCheckTreeCtrl::~CMultiSelTriCheckTreeCtrl()
{
	
}

BEGIN_MESSAGE_MAP(CMultiSelTriCheckTreeCtrl, CMultiSelTriCheckTreeCtrlBase)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_CANCELMODE()
END_MESSAGE_MAP()

void CMultiSelTriCheckTreeCtrl::DrawAlphaBlendedSelection(CDC& dc, CRect rcPaint)
{
	CSimpleMemDC memDCBk(dc, rcPaint);
	DefWindowProc(WM_PRINT, (WPARAM)memDCBk.GetDC().m_hDC, (LPARAM)(PRF_CLIENT|PRF_CHECKVISIBLE));
	::DrawAlphaBlendedSelection(memDCBk, rcPaint, m_rcSelBox, m_clrSelFill, m_clrSelBorder);
}

void CMultiSelTriCheckTreeCtrl::OnPaint()
{
	if ( IsMultiSelectable() && m_bDuringDragSel && IsAlphaBlendedSelectionEnabled() )
	{
		CRect rcUpdate;
		if ( GetUpdateRect(&rcUpdate, FALSE) && !rcUpdate.IsRectEmpty() )
		{
			CPaintDC dc(this);
			DrawAlphaBlendedSelection(dc, dc.m_ps.rcPaint);
		}
	}
	else
	{
		CMultiSelTriCheckTreeCtrlBase::OnPaint();
	}
}

void CMultiSelTriCheckTreeCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	if ( IsMultiSelectable() )
	{
		if ( OnButtonDown(TRUE, nFlags, point) )
			return;	// has been properly handled with this message.
	}
	CMultiSelTriCheckTreeCtrlBase::OnLButtonDown(nFlags, point);
}

void CMultiSelTriCheckTreeCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
	if ( IsMultiSelectable() )
	{
		if ( OnButtonDown(FALSE, nFlags, point) )
			return;	// has been properly handled with this message.
	}
	CMultiSelTriCheckTreeCtrlBase::OnRButtonDown(nFlags, point);
}

#define TVHT_CLICKABLE		(TVHT_ONITEMBUTTON|TVHT_ONITEM)

BOOL CMultiSelTriCheckTreeCtrl::OnButtonDown( BOOL bLeft, UINT nFlags, CPoint point )
{
	ASSERT( IsMultiSelectable() );
	
	if (m_bPendingDragSel)
	{
		// The user is trying to hold down both the two mouse buttons at the same time...
		ASSERT( !m_bLeftBtnDragSel ^ !bLeft );
		if ( HasCapture() )
			::ReleaseCapture();
		if ( !m_bDuringDragSel && !HasCtrlOrShiftFlag() )
			SelectAll(FALSE);
		OnEndDragSelection();
		return FALSE;
	}
	
	ASSERT( !HasCapture() );
	
	UINT uHitTestFlags = 0;
	HTREEITEM hItem = HitTest(point, &uHitTestFlags);
	
	if ( NULL == hItem || !(uHitTestFlags & TVHT_CLICKABLE) )
	{
		ASSERT( m_itemsInSelBox.empty() );
		// set the top left point as the anchor point.
		m_rcSelBox.left		= point.x;
		m_rcSelBox.top		= point.y;
		m_rcSelBox.right	= point.x;
		m_rcSelBox.bottom	= point.y;
		m_bPendingDragSel	= TRUE;
		m_bLeftBtnDragSel	= bLeft;

		if (::GetFocus() != m_hWnd)
			::SetFocus(m_hWnd);
		::SetCapture(m_hWnd);
		return TRUE;	// We have done with this message
	}
	else
	{
		ASSERT( hItem );
		if ( !(uHitTestFlags & (TVHT_ONITEMBUTTON|TVHT_ONITEMSTATEICON)) )
		{
			DoPreSelection(hItem, bLeft, nFlags);
			//DoAction(hItem, bLeft, nFlags, point);
			return TRUE;
		}
	}
	return FALSE;
}

void CMultiSelTriCheckTreeCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	if ( !OnButtonUp(TRUE, nFlags, point) )
		CMultiSelTriCheckTreeCtrlBase::OnLButtonUp(nFlags, point);
}

void CMultiSelTriCheckTreeCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
	if ( !OnButtonUp(FALSE, nFlags, point) )
		CMultiSelTriCheckTreeCtrlBase::OnRButtonUp(nFlags, point);
}

BOOL CMultiSelTriCheckTreeCtrl::OnButtonUp( BOOL bLeft, UINT nFlags, CPoint point )
{
	UINT uHitTestFlags = 0;
	HTREEITEM hItem = HitTest(point, &uHitTestFlags);
	if ( m_bPendingDragSel )
	{
		if ( HasCapture() )
			::ReleaseCapture();
		if (hItem)
		{
			NMTREEVIEW nmtv;
			nmtv.hdr.hwndFrom = m_hWnd;
			nmtv.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);
			nmtv.itemNew.mask = TVIF_HANDLE|TVIF_PARAM;
			nmtv.itemNew.hItem = hItem;
			nmtv.itemNew.lParam = GetItemData(hItem);
			nmtv.hdr.code = m_bLeftBtnDragSel ? NM_CLICK : NM_RCLICK;
			_SendNotify(&nmtv.hdr);
		}
	}
	if ( !m_bDuringDragSel && bLeft && hItem )
	{
		BOOL bInItemSelRect = PtInItemSelectRect(hItem, point);
		if ( !HasCtrlOrShiftFlag() )
		{
			SelectAllIgnore(FALSE, hItem);
			//SelectItem(hItem);
			if ( bInItemSelRect )
			{
				SetItemState(hItem, TVIS_FOCUSED|TVIS_SELECTED, TVIS_FOCUSED|TVIS_SELECTED);
				m_hSelectAnchor = hItem;
			}
		}
		else
		{
			if ( bInItemSelRect )
			{
				if ( HasShiftFlag() )
				{
					if ( !(uHitTestFlags & TVHT_CLICKABLE) )
					{
						if (!m_hSelectAnchor)
							m_hSelectAnchor = GetSelectedItem();	//focus
						/*
						#ifdef _DEBUG
						CString strAnchorText = GetItemText(m_hSelectAnchor);
						CString strTargetText = GetItemText(hItem);
						TRACE("\t====== Selection from [%s] to [%s]\n", strAnchorText, strTargetText);
						#endif // _DEBUG
						*/
						SelectRange(m_hSelectAnchor, hItem, !HasCtrlFlag());
						//SelectItem(hItem);
						SetItemState(hItem, TVIS_FOCUSED, TVIS_FOCUSED);	//focus changes to last clicked

						if ( m_hSelectAnchor != hItem )
							SetItemState(m_hSelectAnchor, TVIS_SELECTED, TVIS_SELECTED);
					}
				}
				else
				{
					UINT nState = TVIS_SELECTED;
					if ( HasCtrlFlag() )
						nState ^= (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED);
					else
						SelectAllIgnore(FALSE, hItem);

					HTREEITEM hSelItem = GetSelectedItem();

					SetItemState(hItem, TVIS_FOCUSED|nState, TVIS_FOCUSED|TVIS_SELECTED);
					m_hSelectAnchor = hItem;

// 					if ( hSelItem && hSelItem != hItem )
// 						SetItemState( hSelItem, TVIS_SELECTED, TVIS_SELECTED);
				}
			}
		}
	}

	if ( m_bPendingDragSel )
	{
		OnEndDragSelection();
	}
	if ( ::GetFocus() != m_hWnd )
		::SetFocus(m_hWnd);
	return TRUE;
}

void CMultiSelTriCheckTreeCtrl::UpdateAlphaBlendedSelection( CRect rcOldSelBox, CRect rcNewSelBox, CRect rcUpdateSelBox )
{
	CClientDC dc(this);
	int nOldWidth	= rcOldSelBox.Width();
	int nOldHeight	= rcOldSelBox.Height();
	int nNewWidth	= rcNewSelBox.Width();
	int nNewHeight	= rcNewSelBox.Height();
	
	rcOldSelBox.NormalizeRect();
	rcNewSelBox.NormalizeRect();
	
	// If we want to minimize the dirty area, we need to do some tricky calculation
	// to make sure that only the area that is required to be redraw become dirty.
	
	// The simplest solution is to calculate the dirty area by combining the two rectangle.
	
#ifdef MINIMIZE_MULTREE_UPDATE_SEL_AREA
	bool bTwoUpdateRect = false;
	CRect rcUpdateSelBox2;
	// If any of them is zero, updating the union area is enough.
	if ( nOldWidth != 0 && nOldHeight != 0 && nNewWidth != 0 && nNewHeight != 0 )
	{
		#define IsSameSign(_nVal1, _nVal2)		( !  ((_nVal1 >= 0) ^ (_nVal2 >= 0))  )
		
		// Flags to see if the selection is flipping.
		const bool bIsHorSameSign = IsSameSign(nOldWidth, nNewWidth);
		const bool bIsVerSameSign = IsSameSign(nOldHeight, nNewHeight);
		
		// 1: If the selection is flipping in both directions, then the two rects need to be update.
		// 2: If the selection is flipping in only one direction, then the union area is good enough.
		// 3: If the selection is not flipping, which is the most complicate case, we need to calculate
		// the dirty area.
		if ( !bIsHorSameSign && !bIsVerSameSign )
		{
			rcUpdateSelBox = rcOldSelBox;
			rcUpdateSelBox2 = rcNewSelBox;
			bTwoUpdateRect = true;
		}
		else if ( bIsHorSameSign && bIsVerSameSign )
		{
			rcUpdateSelBox2 = rcUpdateSelBox;
			// First case: the selection extend/shrink in only one direction (horizontally/vertically)
			if ( nOldWidth == nNewWidth || nOldHeight == nNewHeight )
			{
				if ( nOldWidth != nNewWidth )
				{
					if ( nNewWidth > 0 )
						rcUpdateSelBox.left = (nNewWidth > nOldWidth ? rcOldSelBox.right : rcNewSelBox.right) - 1;
					else
						rcUpdateSelBox.right = (nNewWidth < nOldWidth ? rcOldSelBox.left : rcNewSelBox.left) + 1;
				}
				else
				{
					if ( nNewHeight > 0 )
						rcUpdateSelBox.top = (nNewHeight > nOldHeight ? rcOldSelBox.bottom : rcNewSelBox.bottom) - 1;
					else
						rcUpdateSelBox.bottom = (nNewHeight < nOldHeight ? rcOldSelBox.top : rcNewSelBox.top) + 1;
				}
			}
			else
			{
				// Second case: the selection extend/shrink in both directions.
				bTwoUpdateRect = true;
				if ( nNewWidth > 0 )
				{
					if ( nNewWidth > nOldWidth )
						rcUpdateSelBox2.right = rcUpdateSelBox.left = rcOldSelBox.right-1;
					else
						rcUpdateSelBox2.right = rcUpdateSelBox.left = rcNewSelBox.right-1;
				}
				else
				{
					if ( nNewWidth < nOldWidth )
						rcUpdateSelBox2.left = rcUpdateSelBox.right = rcOldSelBox.left+1;
					else
						rcUpdateSelBox2.left = rcUpdateSelBox.right = rcNewSelBox.left+1;
				}
				
				if ( nNewHeight > 0 )
					rcUpdateSelBox2.top = (nNewHeight > nOldHeight ? rcOldSelBox.bottom : rcNewSelBox.bottom) - 1;
				else
					rcUpdateSelBox2.bottom = (nNewHeight < nOldHeight ? rcOldSelBox.top : rcNewSelBox.top) + 1;
			}
		}
	}
#endif // MINIMIZE_MULTREE_UPDATE_SEL_AREA
	
#ifdef DRAW_SELECTION_ON_PAINT
	InvalidateRect(rcUpdateSelBox);

	#ifdef MINIMIZE_MULTREE_UPDATE_SEL_AREA
	if ( bTwoUpdateRect )
	{
		InvalidateRect(rcUpdateSelBox2);
	}
	#endif // MINIMIZE_MULTREE_UPDATE_SEL_AREA

	UpdateWindow();
#else
	UpdateWindow();

	DrawAlphaBlendedSelection(dc, rcUpdateSelBox);
	
	#ifdef MINIMIZE_MULTREE_UPDATE_SEL_AREA
	if ( bTwoUpdateRect )
	{
		DrawAlphaBlendedSelection(dc, rcUpdateSelBox2);
	}
	#endif // MINIMIZE_MULTREE_UPDATE_SEL_AREA
#endif // DRAW_SELECTION_ON_PAINT
}

void CMultiSelTriCheckTreeCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	if ( IsMultiSelectable() && m_bPendingDragSel )
	{
		if ( !HasCapture() )
			::SetCapture(m_hWnd);

		const bool bFirstTimeDrag = !m_bDuringDragSel;

		CPoint ptScroll;
		SCROLLINFO si = {0};
		GetScrollInfo(SB_HORZ, &si, SIF_POS);
		ptScroll.x = si.nPos;
		GetScrollInfo(SB_VERT, &si, SIF_POS);
		ptScroll.y = si.nPos;

		if ( bFirstTimeDrag )
		{
			CSize sizeDrag(::GetSystemMetrics(SM_CXDRAG), ::GetSystemMetrics(SM_CYDRAG));
			if ( abs( point.x - m_rcSelBox.left - ptScroll.x ) <= sizeDrag.cx 
				&& abs( point.y - m_rcSelBox.top + ptScroll.y ) <= sizeDrag.cy )
			{
				CMultiSelTriCheckTreeCtrlBase::OnMouseMove(nFlags, point);
				return;
			}
			if ( !HasCtrlOrShiftFlag() )
			{
				SelectAll(FALSE);
			}
		}

		m_bDuringDragSel = TRUE;

		CRect rcClient;
		GetClientRect(rcClient);

		// Do the scrolling
		if (point.y < rcClient.top)
			SendMessage(WM_VSCROLL, SB_LINEUP);
		else if (point.y >= rcClient.bottom)
			SendMessage(WM_VSCROLL, SB_LINEDOWN);
		if (point.x < rcClient.left)
			SendMessage(WM_HSCROLL, SB_LINELEFT);
		else if (point.x >= rcClient.right)
			SendMessage(WM_HSCROLL, SB_LINERIGHT);

		CPoint ptNewScroll;
		GetScrollInfo(SB_HORZ, &si, SIF_POS);
		ptNewScroll.x = si.nPos;
		GetScrollInfo(SB_VERT, &si, SIF_POS);
		ptNewScroll.y = si.nPos;

		if ( ptScroll != ptNewScroll )
		{
			ptScroll -= ptNewScroll;
			UINT nItemHeight = GetItemHeight();
			int nVOffset = ptScroll.y * nItemHeight;
			m_rcSelBox.OffsetRect(ptScroll.x, nVOffset);
		}

		CRect rcOldSelBox(m_rcSelBox), rcOldSelBoxCpy(m_rcSelBox);

		CClientDC dc(this);
		CSize sizeEdge(1, 1);
		if ( !IsAlphaBlendedSelectionEnabled() )
		{
			rcOldSelBox.NormalizeRect();
			dc.DrawDragRect(rcOldSelBox, sizeEdge, NULL, sizeEdge);	// remove the old one
		}

		m_rcSelBox.right	= max(rcClient.left, point.x);
		m_rcSelBox.bottom	= max(rcClient.top, point.y);

		m_rcSelBox.right	= min(rcClient.right, m_rcSelBox.right);
		m_rcSelBox.bottom	= min(rcClient.bottom, m_rcSelBox.bottom);

		CRect rcNewSelBox(m_rcSelBox), rcNewSelBoxCpy(m_rcSelBox);

		rcOldSelBox.NormalizeRect();
		rcNewSelBox.NormalizeRect();
		
		// Now we are going to redraw the "dirty" area of the selection,
		// so we need to define what a "dirty" area is.
		if ( rcOldSelBox != rcNewSelBox )
		{
			CRect rcUpdateSelBox;
			rcUpdateSelBox.UnionRect(rcOldSelBox, rcNewSelBox);
			UpdateDragSelection(nFlags, rcNewSelBox, rcUpdateSelBox);

			if ( IsAlphaBlendedSelectionEnabled() )
			{
				UpdateAlphaBlendedSelection(rcOldSelBoxCpy, rcNewSelBoxCpy, rcUpdateSelBox);
			}
			else
			{
				rcNewSelBox.NormalizeRect();
				dc.DrawDragRect(rcNewSelBox, sizeEdge, NULL, sizeEdge);
			}
		}
		return;	// we have done with this message.
	}

	CMultiSelTriCheckTreeCtrlBase::OnMouseMove(nFlags, point);
}

void CMultiSelTriCheckTreeCtrl::OnSetFocus( CWnd* pOldWnd )
{
	CMultiSelTriCheckTreeCtrlBase::OnSetFocus(pOldWnd);
	if ( IsMultiSelectable() )
	{
		HTREEITEM hItem = GetFirstSelectedItem();
		while (hItem)
		{
			InvalidateItem(hItem, FALSE);
			hItem = GetNextSelectedItem(hItem);
		}
		UpdateWindow();
	}
}

void CMultiSelTriCheckTreeCtrl::OnKillFocus( CWnd* pNewWnd )
{
	CMultiSelTriCheckTreeCtrlBase::OnKillFocus(pNewWnd);
	
	if ( m_bPendingDragSel )
	{
		if ( HasCapture() )
			::ReleaseCapture();
		OnEndDragSelection();
	}

	if ( IsMultiSelectable() )
	{
		HTREEITEM hItem = GetFirstSelectedItem();
		while (hItem)
		{
			InvalidateItem(hItem, FALSE);
			hItem = GetNextSelectedItem(hItem);
		}
		UpdateWindow();
	}
}

void CMultiSelTriCheckTreeCtrl::OnCancelMode()
{
	CMultiSelTriCheckTreeCtrlBase::OnCancelMode();
	if ( m_bPendingDragSel )
	{
		if ( HasCapture() )
			::ReleaseCapture();
		OnEndDragSelection();
	}
}

void CMultiSelTriCheckTreeCtrl::OnEndDragSelection()
{
	if (m_bPendingDragSel)
	{
		//TRACE("---------CMultiSelTriCheckTreeCtrl::OnEndDragSelection\n");
		m_bPendingDragSel = m_bDuringDragSel = FALSE;

		m_rcSelBox.NormalizeRect();
		InvalidateRect(m_rcSelBox);
		UpdateWindow();

		m_rcSelBox.SetRectEmpty();

		m_itemsInSelBox.clear();
	}
}

void CMultiSelTriCheckTreeCtrl::DoPreSelection( HTREEITEM hItem, BOOL bLeft, UINT nFlags )
{
	if (::GetFocus() != m_hWnd)
		::SetFocus(m_hWnd);
	if (bLeft)
	{
		//if shift key down, select immediately
		if ( HasShiftFlag() )
		{
			if (!m_hSelectAnchor)
				m_hSelectAnchor = GetSelectedItem();	//focus
			/*
			#ifdef _DEBUG
			CString strAnchorText = GetItemText(m_hSelectAnchor);
			CString strTargetText = GetItemText(hItem);
			TRACE("\t====== Selection from [%s] to [%s]\n", strAnchorText, strTargetText);
			#endif // _DEBUG
			*/
			SelectRange(m_hSelectAnchor, hItem, !HasCtrlFlag());
			//SelectItem(hItem);
			SetItemState(hItem, TVIS_FOCUSED, TVIS_FOCUSED);	//focus changes to last clicked

			if ( m_hSelectAnchor != hItem )
				SetItemState(m_hSelectAnchor, TVIS_SELECTED, TVIS_SELECTED);
		}
		else
		{
			// if ctrl was down, then the selection is delayed until
			// mouse up, otherwise select the one item
			if ( !HasCtrlFlag() )
			{
				if ( !IsSelected(hItem) )
					SelectAllIgnore(FALSE, hItem);
				SetItemState(hItem, TVIS_SELECTED|TVIS_FOCUSED, TVIS_SELECTED|TVIS_FOCUSED);
			}
			//m_hSelectAnchor = NULL;	//reset when a non-shift operation occurs
			m_hSelectAnchor = hItem;
		}
	}
	else
	{
		// right mouse
		if (HasCtrlOrShiftFlag())
		{
			if ( !HasShiftFlag() )
				m_hSelectAnchor = hItem;
		}
		else
		{
			if ( !IsSelected(hItem) )
				SelectAllIgnore(FALSE, hItem);
			SetItemState(hItem, TVIS_SELECTED|TVIS_FOCUSED, TVIS_SELECTED|TVIS_FOCUSED);
		}
	}
}
/*
#ifndef GET_X_LPARAM
	#define GET_X_LPARAM(lParam)    ((int)(short)LOWORD(lParam))
#endif

#ifndef GET_Y_LPARAM
	#define GET_Y_LPARAM(lParam)    ((int)(short)HIWORD(lParam))
#endif

void CMultiSelTriCheckTreeCtrl::DoAction( HTREEITEM hItem, BOOL bLeft, UINT nFlags, CPoint point )
{
	::SetCapture(m_hWnd);
	ASSERT(::GetCapture() == m_hWnd);
	
	MSG msg;
	UINT nDone = 0;
	CPoint pt;
	CSize sizeDrag(::GetSystemMetrics(SM_CXDRAG), ::GetSystemMetrics(SM_CYDRAG));
	
	while (!nDone && ::GetMessage(&msg, NULL, 0, 0))
	{
		if (::GetCapture() != m_hWnd)
			break;
		
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			pt.x = GET_X_LPARAM(msg.lParam);
			pt.y = GET_Y_LPARAM(msg.lParam);
			if ((abs(pt.x - point.x) > sizeDrag.cx)
				|| ((abs(pt.y - point.y) > sizeDrag.cy)) )
				nDone = 2;
			//because we exit loop, button up will still be dispatched
			// which means WM_CONTEXTMENU will be sent after TVN_BEGINRDRAG
			// - this is the same behaviour as original tree
			break;
			
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			nDone = 1;
			break;
			
		default:
			::DispatchMessage(&msg);
			break;
		}
	}
	
	::ReleaseCapture();
	ASSERT(::GetCapture() != m_hWnd);
	
	//construct tree notification info
	NMTREEVIEW nmtv;
	nmtv.hdr.hwndFrom = m_hWnd;
	nmtv.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);
	nmtv.itemNew.mask = TVIF_HANDLE|TVIF_PARAM;
	nmtv.itemNew.hItem = hItem;
	nmtv.itemNew.lParam = GetItemData(hItem);
	DWORD dwStyle = GetStyle();
	
	if (nDone == 1)
	{
		//click
		if (!HasShiftFlag() && bLeft)
		{
			UINT nState = TVIS_SELECTED;
			if ( HasCtrlFlag() )
				nState ^= (GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED);
			else
				SelectAllIgnore(FALSE, hItem);
			SetItemState(hItem, TVIS_FOCUSED|nState, TVIS_FOCUSED|TVIS_SELECTED);
		}
		if (::GetFocus() != m_hWnd)
			::SetFocus(m_hWnd);
		nmtv.hdr.code = bLeft ? NM_CLICK : NM_RCLICK;
		_SendNotify(&nmtv.hdr);
	}
	else if (nDone == 2)
	{
		//drag
		SetItemState(hItem, TVIS_FOCUSED|TVIS_SELECTED, TVIS_FOCUSED|TVIS_SELECTED);
		if (!(dwStyle & TVS_DISABLEDRAGDROP))
		{
			nmtv.hdr.code = bLeft ? TVN_BEGINDRAG : TVN_BEGINRDRAG;
			nmtv.ptDrag = point;
			_SendNotify(&nmtv.hdr);
		}
	}
}
*/

BOOL CMultiSelTriCheckTreeCtrl::PreTranslateMessage( MSG* pMsg )
{
	if ( WM_KEYDOWN == pMsg->message )
	{
		if (VK_ESCAPE == pMsg->wParam)
		{
			if (m_bPendingDragSel)
			{
				if ( HasCapture() )
					::ReleaseCapture();
				SelectAll(FALSE);
				OnEndDragSelection();
			}
		}
	}
	return CMultiSelTriCheckTreeCtrlBase::PreTranslateMessage(pMsg);
}

void CMultiSelTriCheckTreeCtrl::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if ( !IsMultiSelectable() )
	{
		CMultiSelTriCheckTreeCtrlBase::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}
	
	const BOOL bCtrl	= CNTRL_DOWN;
	const BOOL bShift	= SHIFT_DOWN;
	
	BOOL bDir = FALSE;
	HTREEITEM hSel = NULL;
	switch (nChar)
	{
	case VK_UP:
		bDir = TRUE;
	case VK_DOWN:
		//common
		hSel = GetSelectedItem();
		if (!m_hSelectAnchor)
			m_hSelectAnchor = hSel;
		
		if (!bCtrl && !bShift) {
			m_hSelectAnchor = NULL;	//reset
			SelectAll(FALSE);
		}
		break;
	}
	
	CMultiSelTriCheckTreeCtrlBase::OnKeyDown(nChar, nRepCnt, nFlags);
	if (!hSel || (!bCtrl && !bShift) )
		return;
	
	HTREEITEM hNext = bDir ? GetPrevVisibleItem(hSel) : GetNextVisibleItem(hSel);
	if (!hNext)
		hNext = hSel;
	if (bShift)
		SelectRange(m_hSelectAnchor, hNext, TRUE);
	else if (bCtrl)
		SetItemState(hNext, TVIS_FOCUSED, TVIS_FOCUSED);
}

/*
    Operations on each item

    =============================================================================================================================
    Key Pressed                    |          Inside Selection Box              |            Outside Selection Box
    =============================================================================================================================
         No                        |              Select item                   |                Deselect item
    =============================================================================================================================
                 | In itemlist     |                 NOP                        |  Toggle selection & remove item from itemlist
    Control key  |---------------------------------------------------------------------------------------------------------------
                 | Not in itemlist | Toggle selection & add item into itemlist  |                    NOP
    =============================================================================================================================
                 | In itemlist     |                 NOP                        |  Unselect item & remove item from itemlist
    Shift key    |---------------------------------------------------------------------------------------------------------------
                 | Not in itemlist | Select item & add item into itemlist       |                    NOP
    =============================================================================================================================
*/

void CMultiSelTriCheckTreeCtrl::UpdateDragSelection(UINT nFlags, const CRect& rcNewSelBox, const CRect& rcUpdateSelBox)
{
	CRect rcItem;

	const bool bHasCtrlOrShiftFlag = !!HasCtrlOrShiftFlag();

	bool bFoundFirstItemInsideSelBoxVertically = false;
	
	//HTREEITEM hItem = GetRootItem();
	HTREEITEM hItem = HitTest(rcUpdateSelBox.TopLeft());
	while (hItem)
	{
		rcItem = GetSelectRect(hItem);

		CRect rcTmpItem(rcItem);
		const bool bInSelBox	= !!rcTmpItem.IntersectRect(rcItem, rcNewSelBox);
		const bool bWasSelected	= !!IsSelected(hItem);

		bool bUpdateSelState	= true;
		bool bNowSelected		= bInSelBox;

		if ( bHasCtrlOrShiftFlag )
		{
			TreeItemList::iterator itItem	= std::find( m_itemsInSelBox.begin(), m_itemsInSelBox.end(), hItem);
			const bool bInSelList			= itItem != m_itemsInSelBox.end();

			if ( bInSelList ^ bInSelBox )
			{
				if ( HasCtrlFlag() )
					bNowSelected = !bWasSelected;
				if ( bInSelBox )
					m_itemsInSelBox.push_back(hItem);
				else
					m_itemsInSelBox.erase(itItem);
			}
			else
			{
				bUpdateSelState = FALSE;
			}
		}

		if ( bUpdateSelState && (bWasSelected ^ bNowSelected) )
		{
			SetItemState(hItem, bNowSelected ? TVIS_SELECTED : 0, TVIS_SELECTED);
		}

		#define IsValBetween(_nVal, _nMin, _nMax)		(_nMin <= (_nVal) && (_nVal) <= _nMax)
		#define IsRectVerticallyIntersect(_rc1, _rc2)	( IsValBetween(_rc1.top, _rc2.top, _rc2.bottom) \
														|| IsValBetween(_rc1.bottom, _rc2.top, _rc2.bottom) \
														|| IsValBetween(_rc2.top, _rc1.top, _rc1.bottom) )

		const bool bIsItemInsideSelBoxVertically = IsRectVerticallyIntersect(rcItem, rcUpdateSelBox);
		bFoundFirstItemInsideSelBoxVertically = bFoundFirstItemInsideSelBoxVertically || bIsItemInsideSelBoxVertically;

		if ( bFoundFirstItemInsideSelBoxVertically && !bIsItemInsideSelBoxVertically )
			break;

		hItem = GetNextVisibleItem(hItem);
	}
}

size_t CMultiSelTriCheckTreeCtrl::GetSelectedCount() const
{
	size_t nCount = 0;
	HTREEITEM hItem = GetFirstSelectedItem();
	while (hItem)
	{
		++nCount;
		hItem = GetNextSelectedItem(hItem);
	}
	return nCount;
}

HTREEITEM CMultiSelTriCheckTreeCtrl::GetFirstSelectedItem() const
{
	HTREEITEM hItem = GetRootItem();
	while (hItem)
	{
		if ( IsSelected(hItem) )
			break;
		hItem = GetNextVisibleItem(hItem);
	}
	ASSERT( !hItem || IsSelected(hItem) );
	return hItem;
}

HTREEITEM CMultiSelTriCheckTreeCtrl::GetNextSelectedItem( HTREEITEM hItem ) const
{
	hItem = GetNextVisibleItem(hItem);
	while (hItem)
	{
		if ( IsSelected(hItem) )
			break;
		hItem = GetNextVisibleItem(hItem);
	}
	return hItem;
}

HTREEITEM CMultiSelTriCheckTreeCtrl::GetPrevSelectedItem( HTREEITEM hItem ) const
{
	hItem = GetPrevVisibleItem(hItem);
	while (hItem)
	{
		if ( IsSelected(hItem) )
			break;
		hItem = GetPrevVisibleItem(hItem);
	}
	return hItem;
}

void CMultiSelTriCheckTreeCtrl::SelectAll( BOOL bSelect /*= TRUE*/ )
{
	UINT nState = bSelect ? TVIS_SELECTED : 0;
	HTREEITEM hItem = GetRootItem();
	while (hItem)
	{
		if ( !IsSelected(hItem) ^ !bSelect )
			SetItemState(hItem, nState, TVIS_SELECTED);
		hItem = GetNextVisibleItem(hItem);
	}
}

void CMultiSelTriCheckTreeCtrl::SelectRange( HTREEITEM hFirst, HTREEITEM hLast, BOOL bOnly /*= TRUE*/ )
{
	//locate (and select) either first or last
	// (so order is arbitrary)
	HTREEITEM hItem = GetRootItem();
	while (hItem)
	{
		if ((hItem == hFirst) || (hItem == hLast))
		{
			if (hFirst != hLast)
			{
				if (!IsSelected(hItem))
					SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
				hItem = GetNextVisibleItem(hItem);
			}
			break;
		}
		
		if (bOnly && IsSelected(hItem))
			SetItemState(hItem, 0, TVIS_SELECTED);
		hItem = GetNextVisibleItem(hItem);
	}
	//select rest of range
	while (hItem)
	{
		if (!IsSelected(hItem))
			SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
		if ((hItem == hFirst) || (hItem == hLast))
		{
			hItem = GetNextVisibleItem(hItem);
			break;
		}
		hItem = GetNextVisibleItem(hItem);
	}
	if (!bOnly)
		return;
	while (hItem)
	{
		if (IsSelected(hItem))
			SetItemState(hItem, 0, TVIS_SELECTED);
		hItem = GetNextVisibleItem(hItem);
	}
}

void CMultiSelTriCheckTreeCtrl::SelectAllIgnore( BOOL bSelect, HTREEITEM hIgnore )
{
	// special case to avoid multiple notifications for
	// the same item
	UINT nState = bSelect ? TVIS_SELECTED : 0;
	HTREEITEM hItem = GetRootItem();
	while (hItem)
	{
		if (hItem != hIgnore)
		{
			if ( !IsSelected(hItem) ^ !bSelect )
				SetItemState(hItem, nState, TVIS_SELECTED);
		}
		hItem = GetNextVisibleItem(hItem);
	}
}

void CMultiSelTriCheckTreeCtrl::SetMultiSelectable( BOOL bMultiSel /*= TRUE*/ )
{
	m_bMultiSel = bMultiSel;
	if ( !m_bMultiSel )
	{
		HTREEITEM hItem = GetSelectedItem();
		if (hItem && !IsSelected(hItem))
			hItem = NULL;
		SelectAllIgnore(FALSE, hItem);
		if (hItem)
			SelectItem(hItem);
	}
}

void CMultiSelTriCheckTreeCtrl::EnableAlphaBlendedSelection(BOOL bEnable /*= TRUE*/)
{
	m_bAlphaBlendedSelection = bEnable;
	RedrawWindow();
}

void CMultiSelTriCheckTreeCtrl::RedrawSelectedItem(BOOL bErase /*= FALSE*/)
{
	if ( IsMultiSelectable() )
	{
		HTREEITEM hItem = GetRootItem();
		while (hItem)
		{
			if ( IsSelected(hItem) )
			{
				InvalidateItem(hItem, FALSE);
			}
			hItem = GetNextVisibleItem(hItem);
		}
	}
	CMultiSelTriCheckTreeCtrlBase::RedrawSelectedItem();
}

void CMultiSelTriCheckTreeCtrl::SetSelectedItemCheckState( TriCheckState tcs )
{
	HTREEITEM hItem = GetFirstSelectedItem();
	while (hItem)
	{
		SetCheckState(hItem, tcs);
		hItem = GetNextSelectedItem(hItem);
	}
}

void CMultiSelTriCheckTreeCtrl::OnClickItemStateIcon( HTREEITEM hItem )
{
	if ( IsMultiSelectable() && IsSelected(hItem) && !CNTRL_DOWN )
	{
		TriCheckState tcs = GetCheckState(hItem);
		if (tcs != TTCS_NONE)
			SetSelectedItemCheckState(TTCS_CHECKED == tcs ? TTCS_UNCHECKED : TTCS_CHECKED);
	}
	else
		CMultiSelTriCheckTreeCtrlBase::OnClickItemStateIcon(hItem);
}

void CMultiSelTriCheckTreeCtrl::ToggleCheckSelectedItem()
{
	if ( IsMultiSelectable() )
	{
		BOOL bHasAnyCheckedItem = FALSE;
		HTREEITEM hItem = GetFirstSelectedItem();
		while (hItem)
		{
			if ( GetCheck(hItem) )
			{
				bHasAnyCheckedItem = TRUE;
				break;
			}
			hItem = GetNextSelectedItem(hItem);
		}

		SetSelectedItemCheckState(bHasAnyCheckedItem ? TTCS_UNCHECKED : TTCS_CHECKED);
	}
	else
		CMultiSelTriCheckTreeCtrlBase::ToggleCheckSelectedItem();
}

void CMultiSelTriCheckTreeCtrl::DeleteSelectedItem()
{
	if ( IsMultiSelectable() )
	{
		// I can not think of a better solution right now, maybe this can be
		// improved later, if I have the time :)
		TreeItemList selItems;
		GetSelectedList(selItems);
		for (TreeItemList::const_iterator iter = selItems.begin(); iter != selItems.end(); ++iter)
		{
			DeleteItem(*iter);
		}
	}
	else
		CMultiSelTriCheckTreeCtrlBase::DeleteSelectedItem();
}

void CMultiSelTriCheckTreeCtrl::GetSelectedList( TreeItemList& selectedList ) const
{
	selectedList.clear();
	HTREEITEM hItem = GetFirstSelectedItem();
	while (hItem)
	{
		selectedList.push_back(hItem);
		hItem = GetNextSelectedItem(hItem);
	}
}

BOOL CMultiSelTriCheckTreeCtrl::SetItemState( HTREEITEM hItem, UINT nState, UINT nStateMask )
{
	ASSERT(hItem);

	if ( !IsMultiSelectable() )
		return CMultiSelTriCheckTreeCtrlBase::SetItemState(hItem, nState, nStateMask);

	HTREEITEM hFocus = GetSelectedItem();		//current focus
	BOOL bWasFocus = (hFocus == hItem);
	BOOL bFocusWasSel = hFocus && IsSelected(hFocus);	//selection state of current focus
	BOOL bWasSel = IsSelected(hItem);		//select state of acting item

	UINT nS = nState & ~TVIS_FOCUSED;
	UINT nSM = nStateMask & ~TVIS_FOCUSED;

	if (nStateMask & TVIS_FOCUSED)
	{
		//wanted to affect focus
		if (nState & TVIS_FOCUSED)
		{
			//wanted to set focus
			if (!bWasFocus && bFocusWasSel)
			{
				//because SelectItem would de-select the current 'real' selection
				// (the one with focus), need to make the tree ctrl think there is
				// no 'real' selection but still keep the the old item selected
				//it has to be done before the SelectItem call because
				// otherwise the TVN_SELCHANGING/ED notification handlers
				// wouldn't be able to get the proper list of selected items
				CMultiSelTriCheckTreeCtrlBase::SelectItem(NULL);	//will cause notify, but can be taken as focus change
				CMultiSelTriCheckTreeCtrlBase::SetItemState(hFocus, TVIS_SELECTED, TVIS_SELECTED);
				UpdateWindow();
			}
			if (!CMultiSelTriCheckTreeCtrlBase::SelectItem(hItem))	//set focus (will consequently select, if not already focused)
				return FALSE;
			if (nStateMask & TVIS_SELECTED)
			{
				//wanted to affect select state
				if (nState & TVIS_SELECTED)
				{
					//wanted to select, already done if wasn't focused
					if (!bWasFocus || bFocusWasSel)
					{
						nS &= ~TVIS_SELECTED;
						nSM &= ~TVIS_SELECTED;
					}
				}
				//else wanted to clear, base call will do that
			}
			else
			{
				//didn't want to affect select state
				if (!bWasSel)
				{
					//it wasn't previously selected, let base clear (correct)
					nS &= ~TVIS_SELECTED;
					nSM |= TVIS_SELECTED;
				}
				//else was already selected, no harm done
			}
		}
		else
		{
			//wanted to clear focus
			if (bWasFocus)
			{
				//it had the focus
				CMultiSelTriCheckTreeCtrlBase::SelectItem(NULL);	//clear focus
				if (!(nStateMask & TVIS_SELECTED))
				{
					//didn't want to affect select state
					if (bWasSel)
					{
						//it was selected, so restore
						ASSERT( !(nS & TVIS_SELECTED) );
						ASSERT( !(nSM & TVIS_SELECTED) );
						//set state here, to avoid double-notify
						CMultiSelTriCheckTreeCtrlBase::SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
						//let base do other states
					}
				}
				else if (nState & TVIS_SELECTED)
				{
					//wanted to select (but clear focus)
					if (bWasSel)
					{
						//if was selected, restore
						CMultiSelTriCheckTreeCtrlBase::SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
					}
					//don't want to notify, default did it
					nS &= ~TVIS_SELECTED;
					nSM &= ~TVIS_SELECTED;
				}
			}
		}
	}

	if (!nSM)
		return TRUE;	//no other states to alter

	if (nSM & TVIS_SELECTED)
	{
		//still need to alter selection state
		NMTREEVIEW nmtv;
		nmtv.hdr.hwndFrom = m_hWnd;
		nmtv.hdr.idFrom = ::GetDlgCtrlID(m_hWnd);
		nmtv.hdr.code = TVN_SELCHANGING;
		nmtv.itemOld.mask = nmtv.itemNew.mask = 0;
		nmtv.itemOld.hItem = nmtv.itemNew.hItem = NULL;
		TVITEM& item = (nS & TVIS_SELECTED) ? nmtv.itemNew : nmtv.itemOld;
		item.mask = TVIF_HANDLE|TVIF_PARAM;
		item.hItem = hItem;
		item.lParam = GetItemData(hItem);

		if (_SendNotify(&nmtv.hdr))
			return FALSE;	//sel-changing stopped
		VERIFY( CMultiSelTriCheckTreeCtrlBase::SetItemState(hItem, nS, nSM) );
		nmtv.hdr.code = TVN_SELCHANGED;
		_SendNotify(&nmtv.hdr);
		nS &= ~TVIS_SELECTED;
		nSM &= ~TVIS_SELECTED;
	}
	if (!nSM)
		return TRUE;
	return CMultiSelTriCheckTreeCtrlBase::SetItemState(hItem, nS, nSM);
}

UINT CMultiSelTriCheckTreeCtrl::GetItemState( HTREEITEM hItem, UINT nStateMask ) const
{
	UINT nState = CMultiSelTriCheckTreeCtrlBase::GetItemState(hItem, nStateMask & ~TVIS_FOCUSED);
	if (nStateMask & TVIS_FOCUSED)
		if (GetSelectedItem() == hItem)
			nState |= TVIS_FOCUSED;
	return nState;
}

/////////////////////////////////////////////////////////////////////////////
// _SendNotify
// - helper to distinguish between default control generated notifications
//   and this classes emulated ones (so can tell if focus or select notify)
BOOL CMultiSelTriCheckTreeCtrl::_SendNotify( LPNMHDR pNMHDR )
{
	ASSERT(::GetParent(m_hWnd));	//never expected this
	
	BOOL b = m_bEmulated;
	m_bEmulated = TRUE;
	BOOL bRes = ::SendMessage(::GetParent(m_hWnd), WM_NOTIFY, (WPARAM)pNMHDR->idFrom, (LPARAM)pNMHDR);
	m_bEmulated = b;
	return bRes;
}

/*----------------------------------------------------------------------------*/
/* CIconWnd
/*----------------------------------------------------------------------------*/
CIconWnd::CIconWnd()
	: m_hIcon(NULL)
{
}

CIconWnd::~CIconWnd()
{
	DestroyIcon();
}

#ifdef HYBRID_ICONEDIT
BEGIN_MESSAGE_MAP(CIconWnd, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CIconWnd::OnPaint()
{
	CPaintDC dc( this );
	CSimpleMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();
	if( m_hIcon )
	{
		CRect rect;
		GetClientRect( &rect );

		// Clearing the background
		pDC->FillSolidRect( rect, GetSysColor(COLOR_WINDOW) );

		// Drawing the icon
		DrawIcon(pDC);
	}

}

BOOL CIconWnd::OnEraseBkgnd( CDC* )
{
	return TRUE;
}
#endif // HYBRID_ICONEDIT

void CIconWnd::SetIcon(HICON hIcon, BOOL redraw /*= TRUE*/, BOOL bAutoDestroy /*= TRUE*/)
{
	DestroyIcon();
	
	m_hIcon = hIcon;
	
	m_bAutoDestroyIcon = bAutoDestroy;
	
	if(redraw)
		RedrawWindow();
}

void CIconWnd::SetIcon(UINT id, BOOL redraw /*= TRUE*/)
{
	DestroyIcon();
	
	m_hIcon = (HICON)::LoadImage(
		AfxGetResourceHandle(),
		MAKEINTRESOURCE(id),
		IMAGE_ICON,
		16,
		16,
		LR_DEFAULTCOLOR|LR_LOADTRANSPARENT);
	
	ASSERT(m_hIcon != NULL);
	
	// icon was loaded internally
	m_bAutoDestroyIcon = true;
	
	if(redraw)
		RedrawWindow();
}

void CIconWnd::DrawIcon( CDC* pDC )
{
	if( m_hIcon )
	{
		// Drawing the icon
		int width = GetSystemMetrics( SM_CXSMICON );
		int height = GetSystemMetrics( SM_CYSMICON );
		
		::DrawIconEx( 
			pDC->GetSafeHdc(), 
			1, 
			1,
			m_hIcon, 
			width, 
			height, 
			0, 
			NULL, 
			DI_NORMAL);
	}
}

void CIconWnd::DestroyIcon()
{
	// if icon was loaded internally, destroy it
	if(m_bAutoDestroyIcon && m_hIcon != NULL)
		::DestroyIcon(m_hIcon);
}

BOOL CIconWnd::CreateIcon( CWnd* pParent )
{
	ASSERT(pParent);
	CRect rect;
	pParent->GetClientRect(rect);
	rect.right = GetSystemMetrics( SM_CXSMICON );
	return Create("", WS_CHILD | WS_VISIBLE, rect, pParent, 1);
}

IMPLEMENT_DYNAMIC(CIconEdit, CIconEditBase)

CIconEdit::CIconEdit()
{
}

CIconEdit::~CIconEdit()
{
	
}


BEGIN_MESSAGE_MAP(CIconEdit, CIconEditBase)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CIconEdit::PreSubclassWindow() 
{
	RecalcLayout();
	CIconEditBase::PreSubclassWindow();
}

void CIconEdit::SetIcon(HICON hIcon, BOOL redraw /*= TRUE*/, BOOL bAutoDestroy /*= TRUE*/)
{
	CreateIcon();
	m_iconWnd.SetIcon(hIcon, redraw, bAutoDestroy);

	RecalcLayout();
}

void CIconEdit::SetIcon(UINT id, BOOL redraw /*= TRUE*/)
{
	CreateIcon();
	m_iconWnd.SetIcon(id, redraw);

	RecalcLayout();
}

void CIconEdit::RecalcLayout()
{
	if (m_iconWnd.m_hIcon)
	{
		int width = GetSystemMetrics( SM_CXSMICON );
		DWORD dwMargins = GetMargins();
		SetMargins(width, HIWORD(dwMargins));

		CRect rectEditArea;
		GetClientRect(rectEditArea);
		rectEditArea.left = width + 4;
		SetRect(rectEditArea);
	}
}

void CIconEdit::OnSize(UINT nType, int cx, int cy)
{
	CIconEditBase::OnSize(nType, cx, cy);

	RecalcLayout();
}

LRESULT CIconEdit::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	DefWindowProc(WM_SETFONT, wParam, lParam);

	RecalcLayout();

	return 0;
}

void CIconEdit::DrawIcon( CDC* pDC )
{
	m_iconWnd.DrawIcon(pDC);
}

BOOL CIconEdit::OnEraseBkgnd( CDC* pDC )
{
	DrawIcon(pDC);
	return TRUE;
}

void CIconEdit::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	//this will draw the background again
	//so that the button will be drawn if the text exists
	
	InvalidateRect(NULL, FALSE);
	
	CIconEditBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CIconEdit::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	CPoint pntCursor;
	GetCursorPos(&pntCursor);
	ScreenToClient(&pntCursor);
	//if mouse is not in the edit area then
	//show arrow cursor
	CRect rectEditArea;
	GetRect(rectEditArea);
	if (!rectEditArea.PtInRect(pntCursor))
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_ARROW)));
		return TRUE;
	}
	
	return CIconEditBase::OnSetCursor(pWnd, nHitTest, message);
}

void CIconEdit::CreateIcon()
{
	if( !m_iconWnd.GetSafeHwnd() )
	{
		m_iconWnd.CreateIcon(this);
	}
}

/*----------------------------------------------------------------------------*/
/* CComboListBox
/*----------------------------------------------------------------------------*/

IMPLEMENT_DYNCREATE(CComboListBox, CComboListBoxBase)

CComboListBox::CComboListBox()
	: m_pComboBox(NULL)
{
	
}

CComboListBox::~CComboListBox()
{
	
}

BEGIN_MESSAGE_MAP(CComboListBox, CComboListBoxBase)
	ON_MESSAGE(WM_CDTOOLTIPCTRL_NOTIFY, OnCustomToolTipNotify)
END_MESSAGE_MAP()

LRESULT CComboListBox::OnCustomToolTipNotify( WPARAM wParam, LPARAM lParam )
{
	if ( IsComboboxList() && GetBuddyComboBox() )
	{
		return GetBuddyComboBox()->SendMessage(WM_CDTOOLTIPCTRL_NOTIFY, wParam, lParam);
	}
	else
		return CComboListBoxBase::OnCustomToolTipNotify(wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CCustomDrawComboBox

IMPLEMENT_DYNCREATE(CCustomDrawComboBox, CCustomDrawComboBoxBase)

CCustomDrawComboBox::CCustomDrawComboBox()
	: m_hComboEdit(NULL)
	, m_hComboList(NULL)
{
}

CCustomDrawComboBox::~CCustomDrawComboBox()
{
}


BEGIN_MESSAGE_MAP(CCustomDrawComboBox, CCustomDrawComboBoxBase)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_MESSAGE(WM_CDTOOLTIPCTRL_NOTIFY, OnCustomToolTipNotify)
#ifdef SUBCLASS_LISTBOX_EDIT_IN_CTLCOLOR
	ON_WM_CTLCOLOR()
#endif // SUBCLASS_LISTBOX_EDIT_IN_CTLCOLOR
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomDrawComboBox message handlers

void CCustomDrawComboBox::PreSubclassWindow()
{
	CCustomDrawComboBoxBase::PreSubclassWindow();

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == NULL)
	{
		OnInitCombo();
	}
}

int CCustomDrawComboBox::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CCustomDrawComboBoxBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	OnInitCombo();
	return 0;
}

BOOL CCustomDrawComboBox::OnInitCombo()
{
	COMBOBOXINFO cbi = {sizeof(COMBOBOXINFO)};
	if ( ::OGetComboBoxInfo(GetSafeHwnd(), &cbi) )
	{
		m_hComboEdit = cbi.hwndItem;
		m_hComboList = cbi.hwndList;
		
#ifndef SUBCLASS_LISTBOX_EDIT_IN_CTLCOLOR
		GetComboListBox().SubclassWindow(m_hComboList);
		GetComboEdit().SubclassWindow(m_hComboEdit);
#endif // SUBCLASS_LISTBOX_EDIT_IN_CTLCOLOR
		
		//EnableWindowTheme(GetSafeHwnd(), L" ", L" ");
	}

	GetComboListBox().SetBuddyComboBox(this);
	SetExtendedUI(TRUE);	// Pressing the DOWN ARROW key displays the list box
	return TRUE;
}

void CCustomDrawComboBox::OnDestroyCombo()
{
	if ( GetComboEdit().GetSafeHwnd() )
	{
		GetComboEdit().UnsubclassWindow();
	}
	if (GetComboListBox().GetSafeHwnd())
	{
		GetComboListBox().UnsubclassWindow();
	}
}

int CCustomDrawComboBox::GetItemCount()
{
#ifdef USE_HOOK_CHANGE_LISTBOX_STYLE
	return GetComboListBox().m_hWnd ? GetComboListBox().GetItemCount() : 0;
#else
	return GetCount();
#endif // USE_HOOK_CHANGE_LISTBOX_STYLE
}

void CCustomDrawComboBox::SetItemCount( int nCount )
{
	if (!GetComboListBox().m_hWnd)
		return;
#ifdef USE_HOOK_CHANGE_LISTBOX_STYLE
	GetComboListBox().SetItemCount(nCount);

	SetDroppedVisibleItemCount(nCount > MAX_VISIBLE_COMBOLISTITEM_COUNT ? MAX_VISIBLE_COMBOLISTITEM_COUNT : nCount);
#else
	int nCurCount	= GetCount();
	int nExtraCount = nCurCount - nCount;
	if ( nExtraCount > 0 )
	{
		for (int ii = 0; ii < nExtraCount; ++ii)
			DeleteString(0);
	}
	else if ( nExtraCount < 0 )
	{
		for (int ii = 0; ii > nExtraCount; --ii)
			AddString(_T(""));
	}
#endif // USE_HOOK_CHANGE_LISTBOX_STYLE
}


#ifdef USE_HOOK_CHANGE_LISTBOX_STYLE
HHOOK	g_cbtHook = NULL;

LRESULT CALLBACK ComboListCbtCreationHook(int code, WPARAM wParam, LPARAM lParam)
{
	if ( HCBT_CREATEWND == code )
	{
		ASSERT(lParam != NULL);
		LPCREATESTRUCT lpcs = ((LPCBT_CREATEWND)lParam)->lpcs;
		ASSERT(lpcs != NULL);

		// get class name of the window that is being created
		// GetClassName is a more reliable API
		// http://support.microsoft.com/kb/106079/
		TCHAR szClassName[20] = {0};

		HWND hWnd		= reinterpret_cast<HWND>(wParam);
		GetClassName(hWnd, szClassName, 20);

		// Kind of like hacking, the class name of the listbox somehow is not "ListBox" at this moment.
		if ( _tcsicmp(szClassName, _T("ComboLBox")) == 0
			|| _tcsicmp(szClassName, _T("ListBox")) == 0
			)
		{
			lpcs->style &= ~(LBS_HASSTRINGS|LBS_SORT);
			lpcs->style |= LBS_OWNERDRAWFIXED|LBS_NODATA;
			SetWindowLong(hWnd, GWL_STYLE, lpcs->style);
		}
	}
	LRESULT lResult = CallNextHookEx(g_cbtHook, code, wParam, lParam);
	return lResult;
}
#endif // USE_HOOK_CHANGE_LISTBOX_STYLE

BOOL CCustomDrawComboBox::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL bVirtual /*= FALSE*/ )
{
	if (bVirtual)
	{
		dwStyle &= ~CBS_SORT;
		dwStyle |= CBS_OWNERDRAWFIXED|WS_VSCROLL;

		// LBS_NODATA can NOT be set dynamically during runtime, it has to be set BEFORE the listbox
		// is created, so here I use the CBT hook to modify the dwStyle at creation.
	#ifdef USE_HOOK_CHANGE_LISTBOX_STYLE
		ASSERT(NULL == g_cbtHook);
		g_cbtHook = ::SetWindowsHookEx(WH_CBT, ComboListCbtCreationHook, NULL, ::GetCurrentThreadId());
	#endif // USE_HOOK_CHANGE_LISTBOX_STYLE
	}
	
	BOOL bRet = CCustomDrawComboBoxBase::Create(dwStyle, rect, pParentWnd, nID);
	
	if ( bVirtual )
	{
#ifdef USE_HOOK_CHANGE_LISTBOX_STYLE
		::UnhookWindowsHookEx(g_cbtHook);
		g_cbtHook	= NULL;
#endif // USE_HOOK_CHANGE_LISTBOX_STYLE
	}

	return bRet;
}

BOOL CCustomDrawComboBox::CreateFromCtrl( CWnd* pParent, int nID, BOOL bVirtual /*= FALSE*/, DWORD dwStyleAdd /*= 0*/ )
{
	if (!pParent || !pParent->GetSafeHwnd())
		return FALSE;
	CWnd *pCtrl = pParent->GetDlgItem(nID);
	if (!pCtrl)
		return FALSE;

	CFont* pFont = pCtrl->GetFont();
	LOGFONT lf = {0};
	if (pFont)
	{
		pFont->GetLogFont(&lf);
	}

	UINT style = ::GetWindowLong(pCtrl->GetSafeHwnd(), GWL_STYLE);
	CRect controlRect;
	pCtrl->GetWindowRect(controlRect);
	pParent->ScreenToClient(controlRect);
	pCtrl->DestroyWindow();

	BOOL bRet = Create(style | dwStyleAdd, controlRect, pParent, nID, bVirtual);

	m_font.CreateFontIndirect(&lf);
	SetFont(&m_font);
	
	return bRet;
}

void CCustomDrawComboBox::OnDestroy()
{
	//TRACE("CCustomDrawComboBox::OnDestroy()\n");
	OnDestroyCombo();
	CCustomDrawComboBoxBase::OnDestroy();
}

CString CCustomDrawComboBox::GetItemText( UINT nItem )
{
	if (GetComboListBox().GetSafeHwnd())
	{
		return GetComboListBox().GetItemText(nItem);
	}
	return _T("");
}

int CCustomDrawComboBox::GetItemIconIndex( UINT nItem )
{
	if (GetComboListBox().GetSafeHwnd())
	{
		return GetComboListBox().GetItemIconIndex(nItem);
	}
	return -1;
}

void CCustomDrawComboBox::DrawItem( LPDRAWITEMSTRUCT lpDIS )
{
	if (GetComboListBox().GetSafeHwnd())
	{
		GetComboListBox().DrawItem(lpDIS);
	}
}

void CCustomDrawComboBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	if (GetComboListBox().GetSafeHwnd())
	{
		GetComboListBox().MeasureItem(lpMeasureItemStruct);
	}
}

int CCustomDrawComboBox::CompareItem( LPCOMPAREITEMSTRUCT lpCompareItemStruct )
{
	if (GetComboListBox().GetSafeHwnd())
	{
		return GetComboListBox().CompareItem(lpCompareItemStruct);
	}
	return 0;
}

void CCustomDrawComboBox::SetDroppedVisibleItemCount( int nCount )
{
	if (GetComboListBox().GetSafeHwnd())
	{
		SetComboboxDroppedVisibleItemCount(GetSafeHwnd(), nCount);
	}
}

void CCustomDrawComboBox::SetTextOfItem( int nIndex )
{
	if ( nIndex >= 0 )
	{
		CString strText = GetItemText(nIndex);
		SetWindowText(strText);
		GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_EDITCHANGE), (LPARAM)GetSafeHwnd());
	}
}

void CCustomDrawComboBox::PreAddToolTipForItem( int nItem )
{
	// nothing to do
}

void CCustomDrawComboBox::PreShowToolTipForItem( int nItem )
{
	// nothing to do
}

void CCustomDrawComboBox::PrePopToolTip()
{
	// nothing to do
}

LRESULT CCustomDrawComboBox::OnCustomToolTipNotify( WPARAM wParam, LPARAM lParam )
{
	switch (wParam)
	{
	case CDTOOLTIP_ONBEFORE_SHOW:
		{
			int nItem = (int)GetCustomDrawToolTips().GetDisplayWParam();
			PreShowToolTipForItem(nItem);
		}
		break;
	case CDTOOLTIP_ONBEFORE_POP:
		PrePopToolTip();
		break;
	}
	return 0;
}

#ifdef SUBCLASS_LISTBOX_EDIT_IN_CTLCOLOR
HBRUSH CCustomDrawComboBox::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	if ( CTLCOLOR_EDIT == nCtlColor )
	{
		if ( GetComboEdit().GetSafeHwnd() == NULL )
		{
			ASSERT( pWnd->GetSafeHwnd() == m_hComboEdit );
			GetComboEdit().SubclassWindow(pWnd->GetSafeHwnd());
		}
	}
	else if ( CTLCOLOR_LISTBOX == nCtlColor )
	{
		if ( GetComboListBox().GetSafeHwnd() == NULL )
		{
			ASSERT( pWnd->GetSafeHwnd() == m_hComboList );
			GetComboListBox().SubclassWindow(pWnd->GetSafeHwnd());
		}
	}
	return CCustomDrawComboBoxBase::OnCtlColor(pDC, pWnd, nCtlColor);
}
#endif // SUBCLASS_LISTBOX_EDIT_IN_CTLCOLOR

/*----------------------------------------------------------------------------*/
/* CXEditPrompt
/*----------------------------------------------------------------------------*/
IMPLEMENT_DYNCREATE(CXEditPrompt, CXEditPromptBase)

BEGIN_MESSAGE_MAP(CXEditPrompt, CXEditPromptBase)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXEditPrompt::CXEditPrompt()
	:	m_bShowCueBanner(TRUE)
	, m_strPromptText(_T("<Enter text here>"))
	, m_crPromptColor(RGB(119,121,118))	// RAL 9023 (Pearl dark gray)
										// see http://www.highplains.net/pixelcolor.html
	, m_dwCueBannerAlign(ES_CENTER)
{
	m_crBkColor = GetSysColor(COLOR_WINDOW);
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXEditPrompt::~CXEditPrompt()
{
	m_brush.DeleteObject();
	m_robrush.DeleteObject();
}

int CXEditPrompt::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXEditPromptBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	OnInitEdit();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// PreSubclassWindow
void CXEditPrompt::PreSubclassWindow()
{
	CXEditPromptBase::PreSubclassWindow();
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == NULL)
	{
		OnInitEdit();
	}
}

BOOL CXEditPrompt::OnInitEdit()
{
	m_brush.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	
	m_robrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	EnableCueBanner();
	
	//SetWindowText(m_strPromptText);
	SetSel(-1, 0);		// get rid of standard highlighting
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Reset
void CXEditPrompt::Reset()
{
	EnableCueBanner();

	SetWindowText(m_strPromptText);
	SetSel(m_strPromptText.GetLength(), m_strPromptText.GetLength());
	RedrawWindow();
}

///////////////////////////////////////////////////////////////////////////////
// SetPromptColor
void CXEditPrompt::SetPromptColor(COLORREF crText)
{
	m_crPromptColor = crText; 
	//if (m_bFirstTime)
	//	RedrawWindow();
}

///////////////////////////////////////////////////////////////////////////////
// SetPromptText
void CXEditPrompt::SetPromptText(LPCTSTR lpszPrompt)
{ 
	m_strPromptText = lpszPrompt;
	if (m_strPromptText.IsEmpty())
	{
		EnableCueBanner(FALSE);
	}
	if (m_bShowCueBanner)
		SetWindowText(m_strPromptText);
}

///////////////////////////////////////////////////////////////////////////////
// OnSetFocus
void CXEditPrompt::OnSetFocus(CWnd* pOldWnd) 
{
	CXEditPromptBase::OnSetFocus(pOldWnd);
	
	if (m_bShowCueBanner)
	{
		// get rid of standard highlighting
		SetSel(m_strPromptText.GetLength(), m_strPromptText.GetLength());
	}
}

///////////////////////////////////////////////////////////////////////////////
// CtlColor
HBRUSH CXEditPrompt::CtlColor(CDC* pDC, UINT /*nCtlColor*/) 
{
	if ( !IsWindowEnabled() )
		return NULL;
	if (m_bShowCueBanner)
	{
		pDC->SetTextColor(m_crPromptColor);
	}
	if ( GetStyle() & ES_READONLY )
	{
		pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
		return m_robrush;
	}
	pDC->SetBkColor(m_crBkColor);
	return m_brush;		// setting text color will have no effect unless 
						// we return a valid brush
}

///////////////////////////////////////////////////////////////////////////////
// OnKeyDown
void CXEditPrompt::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_bShowCueBanner)
	{
		// key down includes keys like shift and ctrl
		EnableCueBanner(FALSE);
		SetWindowText(_T(""));
	}
	
	CXEditPromptBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CXEditPrompt::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bShowCueBanner)
	{
		EnableCueBanner(FALSE);
		SetWindowText(_T(""));
	}
	
	CXEditPromptBase::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnMButtonDown
void CXEditPrompt::OnMButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bShowCueBanner)
	{
		EnableCueBanner(FALSE);
		SetWindowText(_T(""));
	}
	
	CXEditPromptBase::OnMButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnRButtonDown
void CXEditPrompt::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_bShowCueBanner)
	{
		EnableCueBanner(FALSE);
		SetWindowText(_T(""));
	}
	
	CXEditPromptBase::OnRButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// GetWindowText
int CXEditPrompt::GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const
{
	if (m_bShowCueBanner)
	{
		if (lpszStringBuf && (nMaxCount > 0))
			lpszStringBuf[0] = _T('\0');
		return 0;
	}

	return CXEditPromptBase::GetWindowText(lpszStringBuf, nMaxCount);
}

///////////////////////////////////////////////////////////////////////////////
// GetWindowText
void CXEditPrompt::GetWindowText(CString& rString) const
{
	if (m_bShowCueBanner)
		rString = _T("");
	else
		CXEditPromptBase::GetWindowText(rString);
}

///////////////////////////////////////////////////////////////////////////////
// SetWindowText
void CXEditPrompt::SetWindowText(LPCTSTR lpszString)
{
	// the control is being initialized, just ignore
	if (m_bShowCueBanner && (lpszString[0] == _T('\0')))
		return;

	// if this is not prompt string, then no need to prompt
	if (lpszString && 
		(m_strPromptText.Compare(lpszString) != 0))
		EnableCueBanner(FALSE);

	CXEditPromptBase::SetWindowText(lpszString);
}

///////////////////////////////////////////////////////////////////////////////
// DefWindowProc
LRESULT CXEditPrompt::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_SETTEXT)
	{
		TCHAR *cp = (TCHAR *) lParam;

		if (!cp)
			return TRUE;

		// the control is being initialized, just ignore
		if (m_bShowCueBanner && (cp[0] == _T('\0')))
			return TRUE;

		// if this is not prompt string, then no need to prompt
		if (m_strPromptText.Compare(cp) != 0)
			EnableCueBanner(FALSE);
	}
	else if (message == WM_GETTEXT)
	{
		if (m_bShowCueBanner)
		{
			TCHAR *cp = (TCHAR *) lParam;

			if (cp && (wParam != 0))
				*cp = _T('\0');

			return 0;
		}
	}
	else if ( WM_GETTEXTLENGTH == message )
	{
		if ( m_bShowCueBanner )
			return 0;
	}

	return CXEditPromptBase::DefWindowProc(message, wParam, lParam);
}

void CXEditPrompt::SetBKColor( COLORREF crBK )
{
	m_crBkColor = crBK;
	RedrawWindow();
}

void CXEditPrompt::OnKillFocus(CWnd* pOldWnd)
{
	CXEditPromptBase::OnKillFocus(pOldWnd);
	if (!m_bShowCueBanner)
	{
		CString strWndText;
		GetWindowText(strWndText);
		if (strWndText.IsEmpty())
		{
			EnableCueBanner();
			SetWindowText(m_strPromptText);
		}
	}
}

int CXEditPrompt::GetWindowTextLength() const
{
	if (m_bShowCueBanner)
		return 0;
	return CXEditPromptBase::GetWindowTextLength();
}

void CXEditPrompt::SetCueBannerAlign( DWORD val )
{
	m_dwCueBannerAlign = val;
	if ( m_bShowCueBanner )
		UpdateStyleForCueBanner();
}

/*----------------------------------------------------------------------------*/
/* CFilterEdit
/*----------------------------------------------------------------------------*/
IMPLEMENT_DYNCREATE(CFilterEdit, CXEditPrompt)

CFilterEdit::CFilterEdit(CWnd* pBuddyWnd /*= NULL*/)
:m_pBuddyWnd(pBuddyWnd)
{
	
}

CFilterEdit::~CFilterEdit()
{
	
}

BOOL CFilterEdit::PreTranslateMessage( MSG* pMsg )
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		switch ( pMsg->wParam )
		{
		case VK_DOWN:
		case VK_UP:
		case VK_PRIOR:
		case VK_NEXT:
			{
				CWnd* pWnd = GetFocus();
				if ( pWnd && pWnd == this && m_pBuddyWnd )
				{
// 					CDialog* pDlg = (CDialog*)GetParent();
// 					ASSERT(m_pBuddyWnd);
// 					SetDialogFocus(pDlg->GetSafeHwnd(), m_pBuddyWnd->GetSafeHwnd());
					m_pBuddyWnd->SetFocus();
					m_pBuddyWnd->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
					return TRUE;
				}
			}
			break;
		default:
			break;
		}
	}
	return CXEditPrompt::PreTranslateMessage(pMsg);
}

void CFilterEdit::SetBuddyWindow( CWnd* pBuddyWnd )
{
	m_pBuddyWnd = pBuddyWnd;
}
