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

#ifndef __CONTROLANCHOR_H__
#define __CONTROLANCHOR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxwin.h>
#include <afxtempl.h>

#define CtrlAnchorNone		0x00
#define CtrlAnchorLeft		0x01			// Anchor to the left side
#define CtrlAnchorRight		0x02			// Anchor to the right side
#define CtrlAnchorTop		0x04			// Anchor to the top side
#define CtrlAnchorBottom	0x08			// Anchor to the bottom side
#define CtrlAnchorAll		0x0F			// Anchor to the all 4 sides

#define CtrlAnchorTopLeft		(CtrlAnchorTop|CtrlAnchorLeft)
#define CtrlAnchorTopRight		(CtrlAnchorTop|CtrlAnchorRight)
#define CtrlAnchorBottomLeft	(CtrlAnchorBottom|CtrlAnchorLeft)
#define CtrlAnchorBottomRight	(CtrlAnchorBottom|CtrlAnchorRight)

#define CtrlAnchorLeftRight		(CtrlAnchorLeft|CtrlAnchorRight)
#define CtrlAnchorTopBottom		(CtrlAnchorTop|CtrlAnchorBottom)

#define CtrlAnchorAllButLeft	(CtrlAnchorTop|CtrlAnchorRight|CtrlAnchorBottom)
#define CtrlAnchorAllButRight	(CtrlAnchorLeft|CtrlAnchorTop|CtrlAnchorBottom)
#define CtrlAnchorAllButTop		(CtrlAnchorLeft|CtrlAnchorRight|CtrlAnchorBottom)
#define CtrlAnchorAllButBottom	(CtrlAnchorLeft|CtrlAnchorTop|CtrlAnchorRight)

/*----------------------------------------------------------------------------*/
/* CControlAnchor
/*----------------------------------------------------------------------------*/

class CGripperCtrl;

class CControlAnchor
{
public:
	CControlAnchor();
	~CControlAnchor();
public:
	enum
	{
		CTRL_CREATE_GRIPPERCTRL	= 0x00000001,
	};
	void	Init(CWnd *pParent, DWORD dwCtrl = CTRL_CREATE_GRIPPERCTRL);

	BOOL	Add(CWnd* pCtrl, BYTE anchorSides);

	inline BOOL Add(CWnd& ctrl, BYTE anchorSides)
	{
		return Add(&ctrl, anchorSides);
	}

	inline BOOL	Add(int nID, BYTE anchorSides)
	{
		return m_pParentWnd ? Add(m_pParentWnd->GetDlgItem(nID), anchorSides) : FALSE;
	}

	inline BOOL	Delete(int nID)
	{
		return m_pParentWnd ? Delete(m_pParentWnd->GetDlgItem(nID)) : FALSE;
	}

	BOOL	Delete(CWnd* pCtrl);

	void	OnSize(UINT nType, int cx, int cy);
protected:
	struct CtrlAnchorInfo
	{
		CtrlAnchorInfo(CWnd* _pWnd = NULL, BYTE _anchorSides = CtrlAnchorNone)
			: pWnd(_pWnd)
			, anchorSides(_anchorSides)
		{
		}
		CWnd*	pWnd;			// Pointer to the control
		BYTE	anchorSides;
		LONG	nWidth;
		LONG	nHeight;
		POINT	centerOffset;	// Top left point offset to the parent's center point
		RECT	Distance;		// Distances to each sides
	};
	typedef CArray<CtrlAnchorInfo, CtrlAnchorInfo&>	CCtrlAnchorInfoArray;
	CCtrlAnchorInfoArray	m_arrCtrlAnchorInfo;
	CWnd*					m_pParentWnd;
	SIZE					m_szParentWnd;
	CGripperCtrl*			m_pGripperCtrl;
};

/*----------------------------------------------------------------------------*/
/* CGripperCtrl
/*----------------------------------------------------------------------------*/

#ifndef CGripperCtrlBase
#define CGripperCtrlBase	CScrollBar
#endif

// Reference
// http://www.codeguru.com/cpp/w-d/dislog/resizabledialogs/comments.php/c1917/?thread=24836

class CGripperCtrl : public CGripperCtrlBase
{
public:
	CGripperCtrl();
	virtual ~CGripperCtrl();
public:
	BOOL Create(CWnd* pParent);
	void OnSize(UINT nType);
protected:
	afx_msg LRESULT OnNcHitTest(CPoint point);
protected:
	DECLARE_MESSAGE_MAP()
};

#else
#error ### repeated include of C:\c\MFC\oide\ControlAnchor.h!!!
#endif // __CONTROLANCHOR_H__