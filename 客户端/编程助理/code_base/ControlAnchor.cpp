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
#include "ControlAnchor.h"

#ifndef RECT_WIDTH
	#define RECT_WIDTH(rect)        ((rect)->right - (rect)->left)
#endif // RECT_WIDTH

#ifndef RECT_HEIGHT
	#define RECT_HEIGHT(rect)       ((rect)->bottom - (rect)->top)
#endif // RECT_HEIGHT

CControlAnchor::CControlAnchor()
	: m_pParentWnd(NULL)
	, m_pGripperCtrl(NULL)
{
	
}

CControlAnchor::~CControlAnchor()
{
	m_arrCtrlAnchorInfo.RemoveAll();
	delete m_pGripperCtrl;
}

void CControlAnchor::Init(CWnd *pParent, DWORD dwCtrl /*= CTRL_CREATE_GRIPPERCTRL*/)
{
	if ( !pParent || m_pParentWnd )
	{
		ASSERT(FALSE);
		return;
	}
	RECT rectClient;  
	pParent->GetClientRect(&rectClient);

	m_pParentWnd		= pParent;
	m_szParentWnd.cx	= RECT_WIDTH(&rectClient);
	m_szParentWnd.cy	= RECT_HEIGHT(&rectClient);

	if ( dwCtrl & CTRL_CREATE_GRIPPERCTRL )
	{
		ASSERT( !m_pGripperCtrl );
		m_pGripperCtrl = new CGripperCtrl;
		m_pGripperCtrl->Create(pParent);
		// Add the gripper control right now so that the gripper won't obscure the other controls
		// right now hard coded bottom right, may make this configurable later
		Add(m_pGripperCtrl, CtrlAnchorBottomRight);
	}
}

BOOL CControlAnchor::Add(CWnd* pCtrl, BYTE anchorSides)
{
	if ( !m_pParentWnd || !pCtrl )
		return FALSE;
	CtrlAnchorInfo AnchorInfo(pCtrl, anchorSides);
	
	RECT ctrlRect, parentRect;
	AnchorInfo.pWnd->GetWindowRect(&ctrlRect);
	m_pParentWnd->GetClientRect(&parentRect);
	m_pParentWnd->ClientToScreen(&parentRect);
	
	AnchorInfo.nWidth			= RECT_WIDTH(&ctrlRect);
	AnchorInfo.nHeight			= RECT_HEIGHT(&ctrlRect);
	AnchorInfo.Distance.left	= ctrlRect.left-parentRect.left;
	AnchorInfo.Distance.right	= parentRect.right-ctrlRect.right;
	AnchorInfo.Distance.top		= ctrlRect.top-parentRect.top;
	AnchorInfo.Distance.bottom	= parentRect.bottom-ctrlRect.bottom;
	AnchorInfo.centerOffset.x	= (parentRect.right-parentRect.left)/2 - AnchorInfo.Distance.left;
	AnchorInfo.centerOffset.y	= (parentRect.bottom-parentRect.top)/2 - AnchorInfo.Distance.top;
	m_arrCtrlAnchorInfo.Add(AnchorInfo);
	return TRUE;
}

BOOL CControlAnchor::Delete(CWnd* pCtrl)
{
	if (!pCtrl)
		return FALSE;
	for (int ii = 0; ii < m_arrCtrlAnchorInfo.GetSize(); ++ii)
	{
		if ( pCtrl == m_arrCtrlAnchorInfo[ii].pWnd )
		{
			m_arrCtrlAnchorInfo.RemoveAt(ii);
			return TRUE;
		}
	}
	return FALSE;
}

#define USE_DEFER_WIN_POS	// make the resizing procedure more smoother

void CControlAnchor::OnSize(UINT nType, int cx, int cy)
{
	if ( !m_pParentWnd || !m_pParentWnd->IsWindowVisible() )
		return;

	int nCtrlCount = m_arrCtrlAnchorInfo.GetSize();
	if ( nCtrlCount <= 0 )
		return;

	if ( m_pGripperCtrl )
	{
		m_pGripperCtrl->OnSize(nType);
	}

#ifdef USE_DEFER_WIN_POS
	HDWP hdwp = BeginDeferWindowPos(nCtrlCount);
	if ( !hdwp )
	{
		DWORD dwLastErr = GetLastError();
		ASSERT(FALSE);
	}
#endif // USE_DEFER_WIN_POS
	
	for(int ii = 0; ii < nCtrlCount; ++ii)
	{
		LONG nX, nY;
		CtrlAnchorInfo&	anchorInfo	= m_arrCtrlAnchorInfo[ii];
		BYTE&			anchorSides	= anchorInfo.anchorSides;
		if ( anchorSides & CtrlAnchorLeft )
			nX = anchorInfo.Distance.left;
		if ( anchorSides & CtrlAnchorTop )
			nY = anchorInfo.Distance.top;
		switch (anchorSides)
		{
		case CtrlAnchorLeft:
			nY = cy/2 - anchorInfo.centerOffset.y;
			break;
		case CtrlAnchorRight:
			nX = cx - anchorInfo.Distance.right - anchorInfo.nWidth;
			nY = cy/2 - anchorInfo.centerOffset.y;
			break;
		case CtrlAnchorTop:
			nX = cx/2 - anchorInfo.centerOffset.x;
			break;
		case CtrlAnchorBottom:
			nX = cx/2 - anchorInfo.centerOffset.x;
			nY = cy - anchorInfo.Distance.bottom - anchorInfo.nHeight;
			break;
		case CtrlAnchorLeftRight:
			nY = cy/2 - anchorInfo.centerOffset.y;
			anchorInfo.nWidth += cx - m_szParentWnd.cx;
			break;
		case CtrlAnchorTopLeft:
			continue;
			break;
		case CtrlAnchorBottomLeft:
			nY = cy - anchorInfo.Distance.bottom - anchorInfo.nHeight;
			break;
		case CtrlAnchorTopRight:
			nX = cx - anchorInfo.Distance.right - anchorInfo.nWidth;
			break;
		case CtrlAnchorBottomRight:
			nX = cx - anchorInfo.Distance.right - anchorInfo.nWidth;
			nY = cy - anchorInfo.Distance.bottom - anchorInfo.nHeight;
			break;
		case CtrlAnchorTopBottom:
			nX = cx/2 - anchorInfo.centerOffset.x;
			anchorInfo.nHeight += cy - m_szParentWnd.cy;
			break;
		case CtrlAnchorAllButBottom:
			anchorInfo.nWidth += cx - m_szParentWnd.cx;
			break;
		case CtrlAnchorAllButTop:
			nY = cy - anchorInfo.Distance.bottom - anchorInfo.nHeight;
			anchorInfo.nWidth += cx - m_szParentWnd.cx;
			break;
		case CtrlAnchorAllButLeft:
			nX = cx - anchorInfo.Distance.right - anchorInfo.nWidth;
			anchorInfo.nHeight += cy - m_szParentWnd.cy;
			break;
		case CtrlAnchorAllButRight:
			anchorInfo.nHeight += cy - m_szParentWnd.cy;
			break;
		case CtrlAnchorAll:
			anchorInfo.nWidth += cx - m_szParentWnd.cx;
			anchorInfo.nHeight += cy - m_szParentWnd.cy;
			break;
		case CtrlAnchorNone:
			nX =  cx/2 - anchorInfo.centerOffset.x;
			nY =  cy/2 - anchorInfo.centerOffset.y;
			break;
		}
#ifdef USE_DEFER_WIN_POS
		if (hdwp)
		{
			hdwp = DeferWindowPos(hdwp, 
							anchorInfo.pWnd->GetSafeHwnd(), 
							NULL, 
							nX, 
							nY, 
							anchorInfo.nWidth, 
							anchorInfo.nHeight, 
							SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE
							);
		}
#else
		anchorInfo.pWnd->MoveWindow(nX, nY, anchorInfo.nWidth, anchorInfo.nHeight, FALSE);
#endif // USE_DEFER_WIN_POS
	}

#ifdef USE_DEFER_WIN_POS
	if (hdwp)
		EndDeferWindowPos(hdwp);
#else
	m_pParentWnd->Invalidate();
#endif // USE_DEFER_WIN_POS
	m_szParentWnd.cx = cx;
	m_szParentWnd.cy = cy;
}

/*----------------------------------------------------------------------------*/
/* CGripperCtrl
/*----------------------------------------------------------------------------*/
CGripperCtrl::CGripperCtrl()
{
	
}

CGripperCtrl::~CGripperCtrl()
{
	
}

BEGIN_MESSAGE_MAP(CGripperCtrl, CGripperCtrlBase)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


BOOL CGripperCtrl::Create( CWnd* pParent )
{
	ASSERT_VALID(pParent);
	CRect initRect;
	pParent->GetClientRect(initRect);
	initRect.left	= initRect.right - GetSystemMetrics(SM_CXHSCROLL);
	initRect.top	= initRect.bottom - GetSystemMetrics(SM_CYVSCROLL);
	const DWORD dwStyle = WS_CHILD|SBS_SIZEBOXBOTTOMRIGHTALIGN|SBS_SIZEGRIP|WS_VISIBLE;
	BOOL bRet = CGripperCtrlBase::Create(dwStyle, initRect, pParent, AFX_IDW_SIZE_BOX);
	
	// Put it at the correct spot in the z-order  (make sure other controls don't obscure it)
	
	SetWindowPos(&wndTop, initRect.left, initRect.top, initRect.Width(), initRect.Height(), 0);
	
	return bRet;
}

LRESULT CGripperCtrl::OnNcHitTest( CPoint point )
{
	LRESULT ht = CGripperCtrlBase::OnNcHitTest(point);
	if ( HTCLIENT == ht )
	{
		ht = HTBOTTOMRIGHT;
	}
	return ht;
}

void CGripperCtrl::OnSize( UINT nType )
{
	if ( ::IsWindow(GetSafeHwnd()) )
		ShowWindow( SIZE_MAXIMIZED != nType );
}