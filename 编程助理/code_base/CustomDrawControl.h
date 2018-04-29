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


#ifndef __CUSTOMDRAWCONTROL_H__
#define __CUSTOMDRAWCONTROL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#define DEFAULT_SORT_ASCENDING	true

#ifndef LVS_EX_BORDERSELECT
	#define LVS_EX_BORDERSELECT     0x00008000 // border selection style instead of highlight
#endif

#ifndef LVS_EX_LABELTIP
	#define LVS_EX_LABELTIP         0x00004000 // listview unfolds partly hidden labels if it does not have infotip text
#endif // LVS_EX_LABELTIP

#ifndef LVS_EX_INFOTIP
	#define LVS_EX_INFOTIP          0x00000400 // listview does InfoTips for you
#endif // LVS_EX_INFOTIP

#ifndef LVS_EX_DOUBLEBUFFER
	#define LVS_EX_DOUBLEBUFFER     0x00010000	// Paints via double-buffering, which reduces flicker. 
												// This extended style also enables alpha-blended marquee selection on systems where it is supported.
#endif // LVS_EX_DOUBLEBUFFER

#ifndef SHIFT_DOWN
	#define	SHIFT_DOWN	(GetKeyState(VK_SHIFT) < 0)
#endif // SHIFT_DOWN

#ifndef CNTRL_DOWN
	#define CNTRL_DOWN	(GetKeyState(VK_CONTROL) < 0)
#endif // CNTRL_DOWN

/*----------------------------------------------------------------------------*/
/* Global control
/*----------------------------------------------------------------------------*/

// Require VS2010
#ifdef MSVC_NEW_VER
	#define DERIVE_FROM_MFCTOOLTIPCTRL
 	#define DERIVE_FROM_MFCHEADERCTRL
	#define DERIVE_FROM_MFCLISTCTRL
#endif // MSVC_NEW_VER

#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	#include <afxtooltipctrl.h>
#endif

#ifdef DERIVE_FROM_MFCHEADERCTRL
	#include <afxheaderctrl.h>
#endif

#ifdef DERIVE_FROM_MFCLISTCTRL
	#include <afxlistctrl.h>
#endif

#include "CustomDrawUtils.h"

/*----------------------------------------------------------------------------*/
/* CCustomDrawHeaderCtrl
/*----------------------------------------------------------------------------*/

#ifndef CCustomDrawHeaderCtrlBase
	#define CCustomDrawHeaderCtrlBase	CHeaderCtrl
#endif

class CCustomDrawHeaderCtrl : public CCustomDrawHeaderCtrlBase
{
	DECLARE_DYNCREATE(CCustomDrawHeaderCtrl)
public:
	CCustomDrawHeaderCtrl();
	virtual ~CCustomDrawHeaderCtrl();
public:
	inline void			SetTextDrawer(ITextDrawer* pTextDrawer)		{ m_pTextDrawer = pTextDrawer; }
	inline ITextDrawer*	GetTextDrawer()								{ return m_pTextDrawer; }

	BOOL	IsDragDropEnabled() const;
	void	EnableDragDrop(BOOL bEnable = TRUE);
protected:
	virtual void OnFillBackground(CDC* pDC);
	virtual void OnDrawItem(CDC* pDC, int iItem, CRect rect, BOOL bIsPressed, BOOL bIsHighlighted);
	virtual void OnFillBackground(CDC* pDC, int iItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
	virtual void OnDrawBorder(CDC* pDC, int iItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);

	virtual void OnDrawImage(CDC* pDC, HD_ITEM& hdItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
	virtual void OnDrawBitmap(CDC* pDC, HD_ITEM& hdItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
	virtual void OnDrawText(CDC* pDC, HD_ITEM& hdItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
protected:
	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	void	PreSubclassWindow();

	BOOL	OnEraseBkgnd( CDC* pDC );
	void	OnPaint();
	void	OnLButtonDown(UINT nFlags, CPoint point);
	void	OnLButtonUp(UINT nFlags, CPoint point);
	void	OnMouseMove(UINT nFlags, CPoint point);
	void	OnCancelMode();
	LRESULT	OnMouseLeave(WPARAM wParam, LPARAM lParam);
	LRESULT	OnSetFont(WPARAM wParam, LPARAM lParam);
protected:
	CFont*	SelectFont(CDC *pDC);
	virtual void	OnInitHeader()	{ /* Nothing to do */ }
protected:
	HFONT			m_hFont;
	BOOL			m_bTracked;
	BOOL			m_bIsMousePressed;
	int				m_nHighlightedItem;

	ITextDrawer*	m_pTextDrawer;
protected:
	DECLARE_MESSAGE_MAP()
};

/*----------------------------------------------------------------------------*/
/* CSortHeaderCtrl
/*----------------------------------------------------------------------------*/

//#ifndef CSortHeaderCtrlBase
//	#define CSortHeaderCtrlBase CCustomDrawHeaderCtrl
//#endif
//
//class CSortHeaderCtrl : public CSortHeaderCtrlBase
//{
//	DECLARE_DYNCREATE(CSortHeaderCtrl)
//// Construction
//public:
//	CSortHeaderCtrl();
//	virtual ~CSortHeaderCtrl();
//
//	int m_iSortColumn;
//	BOOL m_bSortAscending;
//
//// Operations
//public:
//	void SwitchSortItem(int nSortItem);
//
//	void SetSortable(bool bSortable = true, bool bRedraw = true);
//
//	void SetSortArrow( const int iColumn, const BOOL bAscending );
//
//	inline int GetSortColumn() const		{ return m_nSortColumn; }
//	inline bool IsSortAscending() const		{ return m_bAscending; }
//
//	//#undef SubclassWindow
//    BOOL Init( HWND hWndToSubClass );
//protected:
//	virtual void OnFillBackground(CDC* pDC, int iItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
//	virtual void OnDrawBorder(CDC* pDC, int iItem, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
//protected:
//    bool		m_bAscending;
//    int			m_nSortColumn;
//	bool		m_bSortable;
//
//	DECLARE_MESSAGE_MAP()
//};

/*----------------------------------------------------------------------------*/
/* CCustomDrawToolTipCtrl
/*----------------------------------------------------------------------------*/

class CCustomDrawToolTipInfo
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	: public CMFCToolTipInfo
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
{
public:
#ifndef DERIVE_FROM_MFCTOOLTIPCTRL
	BOOL m_bBalloonTooltip;
	BOOL m_bDrawIcon;
	BOOL m_bDrawDescription;
	BOOL m_bRoundedCorners;
	BOOL m_bBoldLabel;
	BOOL m_bDrawSeparator;
	BOOL m_bVislManagerTheme;
	int  m_nMaxDescrWidth;
	int  m_nGradientAngle;	// 0 - 360, -1 -default (vertical), not yet implemented in VS6.0
	COLORREF m_clrFill;
	COLORREF m_clrFillGradient;
	COLORREF m_clrText;
	COLORREF m_clrBorder;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL

	COLORREF	m_clrSeparator;
	BOOL		m_bDrawSupplementalDescription;
	BOOL		m_bDrawDescrIcon;
	BOOL		m_bDrawSupplementalImage;
	BOOL		m_bBoldDescription;

	CCustomDrawToolTipInfo()
	{
#ifndef DERIVE_FROM_MFCTOOLTIPCTRL
		m_bBalloonTooltip   = FALSE;
		m_bDrawIcon         = TRUE;
		m_bDrawDescription  = TRUE;
		m_bBoldLabel        = TRUE;
		m_bDrawSeparator    = TRUE;
		m_bVislManagerTheme = FALSE;
		m_nMaxDescrWidth    = 200;
		m_nGradientAngle    = -1;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL

		m_bRoundedCorners   = TRUE;
		m_clrFill			= RGB_TOOLTIP_FILL_COLOR1;
		m_clrFillGradient	= RGB_TOOLTIP_FILL_COLOR2;
		m_clrText           = RGB_TOOLTIP_TEXT_COLOR;
		m_clrBorder         = RGB_TOOLTIP_BORDER_COLOR;

		m_clrSeparator		= (COLORREF)-1;
		m_bDrawSupplementalDescription	= FALSE;
		m_bDrawDescrIcon	= FALSE;
		m_bDrawSupplementalImage	= FALSE;
		m_bBoldDescription	= FALSE;
	}
	
	CCustomDrawToolTipInfo& operator= (CCustomDrawToolTipInfo& src)
	{
		m_clrSeparator		= src.m_clrSeparator;
		m_bDrawDescrIcon	= src.m_bDrawDescrIcon;
		m_bDrawSupplementalImage	= src.m_bDrawSupplementalImage;
		m_bDrawSupplementalDescription	 = src.m_bDrawSupplementalDescription;
		m_bBoldDescription	= src.m_bBoldDescription;
#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
		CMFCToolTipInfo::operator=(src);
		return *this;
#else
		m_bBalloonTooltip   = src.m_bBalloonTooltip;
		m_bDrawIcon         = src.m_bDrawIcon;
		m_bDrawDescription  = src.m_bDrawDescription;
		m_bRoundedCorners   = src.m_bRoundedCorners;
		m_bBoldLabel        = src.m_bBoldLabel;
		m_bDrawSeparator    = src.m_bDrawSeparator;
		m_bVislManagerTheme = src.m_bVislManagerTheme;
		m_nMaxDescrWidth    = src.m_nMaxDescrWidth;
		m_nGradientAngle    = src.m_nGradientAngle;
		m_clrFill           = src.m_clrFill;
		m_clrFillGradient   = src.m_clrFillGradient;
		m_clrText           = src.m_clrText;
		m_clrBorder         = src.m_clrBorder;
		
		return *this;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL
	}
};

#ifdef DERIVE_FROM_MFCTOOLTIPCTRL
	#define CCDToolTipCtrlBase	CMFCToolTipCtrl	// requires VS2010
#else
	#define CCDToolTipCtrlBase	CToolTipCtrl
#endif // DERIVE_FROM_MFCTOOLTIPCTRL

/*
	Basically the codes are taken from CMFCToolTipCtrl

	Here's the appearance of a CCustomDrawToolTipCtrl

  Note:
	1) The items that enclosed with [] are required
	2) The items that enclosed with <> are optional
	3) The separator presents only when description is valid

  Example:
	http://msdn.microsoft.com/en-us/library/cc872782.aspx#enhancedTooltips

    +---------------------------------------+
    | <icon>  [title label]                 |
    +---------------------------------------+
    |					                    |
    | <preview>  <supplemental description> |
    |                                       |
    +--------<separator>--------------------+
    | <desc icon>    <description>          |
    +---------------------------------------+

*/

// wParam
enum
{
	CDTOOLTIP_ONBEFORE_SHOW,
	CDTOOLTIP_ONBEFORE_POP,
};


#ifndef MSVC_NEW_VER
	#define TTTOOLINFOA_V1_SIZE		CCSIZEOF_STRUCT(TTTOOLINFOA, lpszText)
	#define TTTOOLINFOW_V1_SIZE		CCSIZEOF_STRUCT(TTTOOLINFOW, lpszText)
	#define TTTOOLINFOA_V2_SIZE		CCSIZEOF_STRUCT(TTTOOLINFOA, lParam)
	#define TTTOOLINFOW_V2_SIZE		CCSIZEOF_STRUCT(TTTOOLINFOW, lParam)
	#define TTTOOLINFOA_V3_SIZE		CCSIZEOF_STRUCT(TTTOOLINFOA, lpReserved)
	#define TTTOOLINFOW_V3_SIZE		CCSIZEOF_STRUCT(TTTOOLINFOW, lpReserved)
#endif // MSVC_NEW_VER

#ifdef _UNICODE
	#define TTTOOLINFO_V1_SIZE		TTTOOLINFOW_V1_SIZE
	#define TTTOOLINFO_V2_SIZE		TTTOOLINFOW_V2_SIZE
	#define TTTOOLINFO_V3_SIZE		TTTOOLINFOW_V3_SIZE
#else
	#define TTTOOLINFO_V1_SIZE		TTTOOLINFOA_V1_SIZE
	#define TTTOOLINFO_V2_SIZE		TTTOOLINFOA_V2_SIZE
	#define TTTOOLINFO_V3_SIZE		TTTOOLINFOA_V3_SIZE
#endif // _UNICODE

#define WM_CDTOOLTIPCTRL_NOTIFY			(WM_USER+0x600)

class CCustomDrawToolTipCtrl : public CCDToolTipCtrlBase
{
	DECLARE_DYNCREATE(CCustomDrawToolTipCtrl)
public:
	CCustomDrawToolTipCtrl(CCustomDrawToolTipInfo* pParams = NULL);
	virtual ~CCustomDrawToolTipCtrl();
public:
	void	SetText(NMHDR* pNMHDR, LPCTSTR lpcszText);

	void	SetParams(CCustomDrawToolTipInfo* pParams); // NULL - default params
	const CCustomDrawToolTipInfo& GetParams() const			{ return m_Params; }

	BOOL	CheckDisplayToolTip(WPARAM wParam, LPARAM lParam = 0);
	inline WPARAM GetDisplayWParam() const					{ return m_DisplayIDWParam; }
	inline WPARAM GetDisplayLParam() const					{ return m_DisplayIDLParam; }

	void	SetFixedWidth(int nWidthRegular, int nWidthLargeImage);
	int		GetFixedWidth();

	virtual void SetLabel(const CString strLabel);
	virtual void SetDescription(const CString strDescription);
	virtual void SetSupplementalDescription(const CString strDescription);
	
	virtual CSize GetIconSize();

	virtual CSize GetDescrIconSize();
	virtual CSize GetSupplementalImageSize();

	void	SetTextTabSize(int nTabSize = 4);

	void	SetLocation(CPoint pt);

	virtual void SetLabelIcon(HICON hIcon);
	virtual void SetDescriptionIcon(HICON hIcon);
	virtual void SetSupplementalImage(HBITMAP hBmp);

	inline void			SetTextDrawer(ITextDrawer* pTextDrawer)		{ m_pTextDrawer = pTextDrawer; }
	inline ITextDrawer*	GetTextDrawer()								{ return m_pTextDrawer; }
public:
	virtual void OnFillBackground(CDC* pDC, CRect rect, COLORREF& clrText, COLORREF& clrLine);
	virtual void OnDrawBorder(CDC* pDC, CRect rect, COLORREF clrLine);

	virtual BOOL OnDrawIcon(CDC* pDC, CRect rectImage);
	virtual CSize OnDrawLabel(CDC* pDC, CRect rect, BOOL bCalcOnly);

	virtual CSize OnDrawBody(CDC* pDC, CRect rect, BOOL bCalcOnly);
	virtual BOOL OnDrawSupplementalImage(CDC* pDC, CRect rect);
	virtual CSize OnDrawSupplementalDescription(CDC* pDC, CRect rect, BOOL bCalcOnly);

	virtual BOOL OnDrawDescriptionIcon(CDC* pDC, CRect rect);

	virtual CSize OnDrawDescription(CDC* pDC, CRect rect, BOOL bCalcOnly);
	virtual void OnDrawSeparator(CDC* pDC, int x1, int x2, int y);
protected:
	CFont*		SelectFont(CDC *pDC, BOOL bBold = FALSE);

	CSize		DrawText(CDC *pDC, 
						const CString& strText, 
						CRect rect, 
						BOOL bCalcOnly, 
						BOOL bBold = FALSE,
						BOOL bVertCenter = FALSE,
						BOOL bWordBreak = FALSE
						);
protected:
	void		OnPaint();
	int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL		OnEraseBkgnd(CDC* pDC);
	void		OnShow(NMHDR* pNMHDR, LRESULT* pResult);
	void		OnPop(NMHDR* pNMHDR, LRESULT* pResult);

#ifndef MSVC_NEW_VER
	LRESULT		OnSetFont(WPARAM wParam, LPARAM lParam);
#endif // MSVC_NEW_VER

public:
#ifndef DERIVE_FROM_MFCTOOLTIPCTRL
	CPoint		m_ptMargin;
	CString		m_strDescription;
	CSize		m_sizeImage;
	CPoint		m_ptLocation;

	int			m_nFixedWidthRegular;
	int			m_nFixedWidthWithImage;
#endif // DERIVE_FROM_MFCTOOLTIPCTRL

	CSize		m_sizeDescrImage;
	CSize		m_sizeSupplementalImage;
	CString		m_strSupplementalDescription;
	CString		m_strLabel;

	HICON		m_hLabelIcon;
	HICON		m_hDescrIcon;
	HBITMAP		m_hSupplementalBmp;

	// See SetText()
	std::string		m_astrLongTipText;
	std::wstring	m_wstrLongTipText;

	CCustomDrawToolTipInfo  m_Params;

#ifndef MSVC_NEW_VER
	HFONT			m_hFont;
	CFont			m_fontBold;
#endif // MSVC_NEW_VER
	int				m_nTextTabSize;

	WPARAM			m_DisplayIDWParam;
	LPARAM			m_DisplayIDLParam;

	// for text alignment
	int				m_nLabelHeight;

	ITextDrawer*	m_pTextDrawer;
protected:
	DECLARE_MESSAGE_MAP()
};

/*----------------------------------------------------------------------------*/
/* CCustomDrawListCtrl
/*----------------------------------------------------------------------------*/

//#define FIX_LISTCTRL_HILIGHT_UNSELECTED_ITEM_ISSUE

#ifdef DERIVE_FROM_MFCLISTCTRL
	#define CCustomDrawListCtrlBase CMFCListCtrl
#endif

#ifndef CCustomDrawListCtrlBase
	#define CCustomDrawListCtrlBase CListCtrl
#endif

class CCustomDrawListCtrl : public CCustomDrawListCtrlBase
{
	DECLARE_DYNCREATE(CCustomDrawListCtrl)
public:
	CCustomDrawListCtrl();
	virtual ~CCustomDrawListCtrl();
public:
	inline BOOL IsCustomDrawEnabled() const								{ return m_bEnableCustomDraw; }
	void		EnableCustomDraw(BOOL bEnable = TRUE, BOOL bRedraw = FALSE);

	// From MSDN:
	// Version 6.00. Paints via double-buffering, which reduces flicker. 
	// This extended style also enables alpha-blended marquee selection on systems where it is supported.
	BOOL		IsDoubleBuffered();
	void		SetDoubleBuffered(BOOL bSet = TRUE);

	BOOL		IsFullRowSelect();
	void		SetFullRowSelect(BOOL bSet = TRUE);

	BOOL		IsHeaderDragDrop();
	void		SetHeaderDragDrop(BOOL bSet = TRUE);

	BOOL		IsSingleSel();
	void		SetSignleSel(BOOL bSet = TRUE);

	//virtual CSortHeaderCtrl& GetSortHeaderCtrl()					{ return m_SortHeaderCtrl; }

	virtual CCustomDrawToolTipCtrl& GetCustomDrawToolTips()			{ return m_OwnToolTipCtrl; }

	inline void			SetTextDrawer(ITextDrawer* pTextDrawer)		{ m_pTextDrawer = pTextDrawer; }
	inline ITextDrawer*	GetTextDrawer()								{ return m_pTextDrawer; }

	// Mark sorted column by background color
#ifndef DERIVE_FROM_MFCLISTCTRL
	void EnableMarkSortedColumn(BOOL bMark = TRUE, BOOL bRedraw = TRUE);
#endif // DERIVE_FROM_MFCLISTCTRL

	LRESULT		EnableExplorerVisualStyles(bool bValue = true);

#ifndef MSVC_NEW_VER
	// Retrieves the tool tip control associated with this control.
	CToolTipCtrl* GetToolTips() const;
	
	// Sets the tool tip control to be used by this control.
	CToolTipCtrl* SetToolTips(CToolTipCtrl* pWndTip);
#endif // MSVC_NEW_VER

	inline void	LockSetRedraw(BOOL bDraw = TRUE)
	{
		if ( (bDraw ? (0 == --m_nLockDrawCount) : (0 == m_nLockDrawCount++)) )
			SetRedraw( bDraw );
	}
protected:
	// custom draw
	void		OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

	// owner draw
	// The LVS_OWNERDRAWFIXED style is only compatible with the LVS_REPORT style
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	virtual void OnDrawItem(CDC* pDC, int nItem);
	virtual void OnEraseBackground(CDC* pDC, CRect rect, int nItem);

	virtual void OnDrawCell(CDC* pDC, int nItem, int nSubItem, LPLVITEM lpLVItem);
	virtual void OnFillBackground(CDC* pDC, CRect rect, int nItem, int nSubItem, LPLVITEM lpLVItem);
	virtual BOOL OnDrawIcon(CDC* pDC, CRect rect, int nItem, int nSubItem, LPLVITEM lpLVItem);
	virtual void OnDrawCellText(CDC* pDC, CRect rect, int nItem, int nSubItem, LPLVITEM lpLVItem);

	virtual CSize GetIconSize(int nItem, int nSubItem);
	virtual CRect GetSubItemDrawRect(int nItem, int nSubItem);

	virtual COLORREF GetItemTextColor(int nItem, int nSubItem, UINT nItemState);

	virtual bool IsSortable() const;
	virtual BOOL OnSort(int nCol);

	virtual BOOL ShouldShowToolTipForCell(int nRow, int nCol);
	virtual CString GetToolTipLabelForCell(int nRow, int nCol);

	virtual void	PreAddToolTipForCell(int nRow, int nCol);
	virtual void	PreShowToolTipForCell(int nRow, int nCol);
	virtual void	PrePopToolTip();
protected:
	int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	void		PreSubclassWindow();
	BOOL		PreTranslateMessage(MSG* pMsg);

#ifdef FIX_LISTCTRL_HILIGHT_UNSELECTED_ITEM_ISSUE
	// Although this class works fine with VS6.0, somehow when compiled with VS2010, 
	// items remain highlighted even though they are not selected, I guess this can
	// be another bug of VS, anyway, we have to erase them by ourself.
	// http://www.codeguru.com/Cpp/controls/listview/selection/article.php/c949/
	void		RepaintSelectedItems();

	void		OnKillFocus(CWnd* pNewWnd);
	void		OnSetFocus(CWnd* pOldWnd);
	void		OnLButtonDown(UINT nFlags, CPoint point);
	void		OnRButtonDown(UINT nFlags, CPoint point);
#endif // FIX_LISTCTRL_HILIGHT_UNSELECTED_ITEM_ISSUE

	BOOL		OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT* pResult);
	void		OnMouseMove(UINT nFlags, CPoint point);

	LRESULT		OnMouseLeave(WPARAM wParam, LPARAM lParam);
	void		OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult);
	void		OnSize(UINT nType, int cx, int cy);
	void		OnDestroy();

	LRESULT		OnCustomToolTipNotify(WPARAM wParam, LPARAM lParam);

	virtual void OnInitList();
	virtual void OnDestroyList()		{ /* nothing to do */}

	virtual void InitHeader();
protected:
	BOOL					m_bEnableCustomDraw;
	BOOL					m_bIsOwnerDraw;
#ifndef DERIVE_FROM_MFCLISTCTRL
	BOOL					m_bMarkSortedColumn;
	COLORREF				m_clrSortedColumn;
#endif // DERIVE_FROM_MFCLISTCTRL

	int						m_nHotItem;
	BOOL					m_bMouseEventsTracked;
	BOOL					m_bDrawHotItem;
private:
	//CSortHeaderCtrl			m_SortHeaderCtrl;
	CCustomDrawToolTipCtrl	m_OwnToolTipCtrl;	// No direct access allowed from derived class.

	BOOL					m_bExplorerVisualStyle;

	int						m_nLockDrawCount;
	ITextDrawer*			m_pTextDrawer;
protected:
	DECLARE_MESSAGE_MAP()
};

/*----------------------------------------------------------------------------*/
/* CCustomDrawListBox
/*----------------------------------------------------------------------------*/
#ifndef CCustomDrawListBoxBase
	#define CCustomDrawListBoxBase	CListBox
#endif

#ifndef LBS_COMBOBOX
	#define LBS_COMBOBOX          0x8000L
#endif

class CCustomDrawListBox : public CCustomDrawListBoxBase
{
	DECLARE_DYNCREATE(CCustomDrawListBox)
public:
	CCustomDrawListBox();
	virtual ~CCustomDrawListBox();
public:
	int			GetItemCount();
	int			SetItemCount(int nCount);

	CImageList* SetImageList( CImageList* pImageList );
	CImageList* GetImageList( ) const;

	virtual CCustomDrawToolTipCtrl& GetCustomDrawToolTips()				{ return m_OwnToolTipCtrl; }

	virtual BOOL ShouldDrawHotItem() const								{ return TRUE; }

	virtual CString GetItemText(UINT nItem);
	virtual int	GetItemIconIndex(UINT nItem);

	inline void			SetTextDrawer(ITextDrawer* pTextDrawer)			{ m_pTextDrawer = pTextDrawer; }
	inline ITextDrawer*	GetTextDrawer()									{ return m_pTextDrawer; }

	LRESULT		EnableExplorerVisualStyles(bool bValue = true);

	inline void	LockSetRedraw(BOOL bDraw = TRUE)
	{
		if ( (bDraw ? (0 == --m_nLockDrawCount) : (0 == m_nLockDrawCount++)) )
			SetRedraw( bDraw );
	}
public:
	LRESULT OnDrawItem(WPARAM nIDCtl, LPARAM lpDrawItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
protected:
	virtual void OnFillBackground(CDC* pDC, CRect rect, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnDrawIcon(CDC* pDC, CRect rect, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void OnDrawText(CDC* pDC, CRect rect, LPDRAWITEMSTRUCT lpDrawItemStruct);

	virtual COLORREF GetTextColor(LPDRAWITEMSTRUCT lpDrawItemStruct);

	virtual BOOL ShouldShowToolTipForItem(int nItem);
	virtual CString GetToolTipLabelForItem(int nItem);

	virtual void	PreAddToolTipForItem(int nItem);
	virtual void	PreShowToolTipForItem(int nItem);
	virtual void	PrePopToolTip();

	virtual BOOL IsComboboxList();
protected:
	int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	void		PreSubclassWindow();
	BOOL		PreTranslateMessage(MSG* pMsg);
	
	BOOL		OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT* pResult);
	void		OnMouseMove(UINT nFlags, CPoint point);
	LRESULT		OnMouseLeave(WPARAM wParam, LPARAM lParam);

	void		OnSetFocus(CWnd* pOldWnd);
	void		OnKillFocus(CWnd* pNewWnd);
	void		OnDestroy();

	LRESULT		OnCustomToolTipNotify(WPARAM wParam, LPARAM lParam);

	virtual void OnInitListBox();
	virtual void OnDestroyListBox()		{ /* nothing to do*/ }
protected:
	CImageList*				m_pImageList;

	int						m_nHotItem;
	BOOL					m_bMouseEventsTracked;
private:
	CCustomDrawToolTipCtrl	m_OwnToolTipCtrl;	// No direct access allowed from derived class.
	friend class	CCustomDrawComboBox;

	BOOL					m_bExplorerVisualStyle;
	int						m_nLockDrawCount;
	ITextDrawer*			m_pTextDrawer;
protected:
	DECLARE_MESSAGE_MAP()
};

/*----------------------------------------------------------------------------*/
/* CCustomDrawTreeCtrl
/*----------------------------------------------------------------------------*/
#ifndef CCustomDrawTreeCtrlBase
	#define CCustomDrawTreeCtrlBase	CTreeCtrl
#endif // CCustomDrawTreeCtrlBase

// Undefine this underneath macro to see what happen when scrolling the tree control horizontally
#define FIX_TREECTRL_SCROLL_REDRAW_ISSUE

class CCustomDrawTreeCtrl : public CCustomDrawTreeCtrlBase
{
	DECLARE_DYNCREATE(CCustomDrawTreeCtrl)
public:
	CCustomDrawTreeCtrl();
	virtual ~CCustomDrawTreeCtrl();
public:
	inline BOOL IsCustomDrawEnabled() const								{ return m_bCustomDraw; }
	void		EnableCustomDraw(BOOL bEnable = TRUE, BOOL bRedraw = FALSE);

	BOOL		IsDrawHotItem() const									{ return m_bDrawHotItem; }
	void		SetDrawHotItem(BOOL bDraw = TRUE, BOOL bReDraw = TRUE);

	virtual CCustomDrawToolTipCtrl& GetCustomDrawToolTips()				{ return m_OwnToolTipCtrl; }

	inline void			SetTextDrawer(ITextDrawer* pTextDrawer)			{ m_pTextDrawer = pTextDrawer; }
	inline ITextDrawer*	GetTextDrawer()									{ return m_pTextDrawer; }

	inline void	LockSetRedraw(BOOL bDraw = TRUE)
	{
		if ( (bDraw ? (0 == --m_nLockDrawCount) : (0 == m_nLockDrawCount++)) )
			SetRedraw( bDraw );
	}

	LRESULT		EnableExplorerVisualStyles(bool bValue = true);

	BOOL		HasValidFullRowSelectStyle();

	enum SelStyle
	{
		CDTSS_LABEL,
		CDTSS_LABELRIGHT,
		CDTSS_FULLROWSELECT,
	};

	inline SelStyle	GetSelStyle() const									{ return m_SelStyle; }
	void			SetSelStyle(SelStyle selStyle);
protected:	// For drawing
	virtual void	OnDrawTreeItem(CDC* pDC, HTREEITEM hItem, UINT nItemState);
	
	virtual void	OnFillBackground(CDC* pDC, CRect rect, HTREEITEM hItem, UINT nItemState);
	
	virtual CSize	OnDrawText(CDC* pDC, CRect rect, HTREEITEM hItem, UINT nItemState, BOOL bCalcOnly = FALSE);
	
	CSize			DrawItemText(CDC* pDC, CRect rect, HTREEITEM hItem, LPCTSTR lpcszText, COLORREF clrTxtColor, BOOL bCalcOnly = FALSE);

	virtual CRect	GetFillRect(HTREEITEM hItem);
	virtual CRect	GetSelectRect(HTREEITEM hItem);	// returns the selectable area
	BOOL			PtInItemSelectRect(HTREEITEM hItem, CPoint pt);
protected:
	void			OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnMouseMove(UINT nFlags, CPoint point);
	LRESULT			OnMouseLeave(WPARAM wParam, LPARAM lParam);

	void			OnLButtonDown(UINT nFlags, CPoint point);

	int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	void			PreSubclassWindow();
	BOOL			PreTranslateMessage(MSG* pMsg);

	BOOL			OnEraseBkgnd(CDC* pDC);
	void			OnPaint();

	LRESULT			OnStyleChanged(WPARAM wp, LPARAM lp);

	virtual void	RedrawSelectedItem(BOOL bErase = FALSE);

#ifdef FIX_TREECTRL_SCROLL_REDRAW_ISSUE
	void			OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
#endif // FIX_TREECTRL_SCROLL_REDRAW_ISSUE
protected:
	virtual void	OnInitTreeCtrl();

	BOOL			OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);

	virtual BOOL	ShouldShowToolTipForCell(HTREEITEM hItem, UINT uHitTestFlags);
	virtual CString GetToolTipLabelForItem(HTREEITEM hItem, UINT uHitTestFlags);

	virtual void	PreAddToolTipForItem(HTREEITEM hItem, UINT uHitTestFlags);
	virtual void	PreShowToolTipForItem(HTREEITEM hItem, UINT uHitTestFlags);
	virtual void	PrePopToolTip();

	LRESULT			OnCustomToolTipNotify(WPARAM wParam, LPARAM lParam);

	void			InvalidateItem(HTREEITEM hItem, BOOL bErase = TRUE);
protected:
	DECLARE_MESSAGE_MAP()
protected:
	BOOL					m_bCustomDraw;
	HTREEITEM				m_hHotTreeItem;
	BOOL					m_bDrawHotItem;
	BOOL					m_bMouseEventsTracked;
	int						m_nLockDrawCount;
	ITextDrawer*			m_pTextDrawer;
	BOOL					m_bExplorerVisualStyle;
	SelStyle				m_SelStyle;
private:
	CCustomDrawToolTipCtrl	m_OwnToolTipCtrl;	// No direct access allowed from derived class.
};

/*----------------------------------------------------------------------------*/
/* class CTriCheckStateTreeCtrl
/*----------------------------------------------------------------------------*/
enum TriCheckState
{
	TTCS_NONE			= 0,
	TTCS_UNCHECKED,
	TTCS_PARTIALCHECKED,
	TTCS_CHECKED,
};

#ifndef CTriCheckStateTreeCtrlBase
	#define CTriCheckStateTreeCtrlBase	CCustomDrawTreeCtrl
#endif // CCustomDrawTreeCtrlBase

class CTriCheckStateTreeCtrl : public CTriCheckStateTreeCtrlBase
{
	DECLARE_DYNCREATE(CTriCheckStateTreeCtrl)
public:
	CTriCheckStateTreeCtrl();
	virtual ~CTriCheckStateTreeCtrl();
	
	// Implementation
public:
	BOOL			SetItemState( HTREEITEM hItem, UINT nState, UINT nStateMask, BOOL bSearchChild = TRUE );

	BOOL			SetCheck(HTREEITEM hItem, BOOL bCheck = TRUE);
	BOOL			GetCheck(HTREEITEM hItem) const;

	inline void		ToggleCheck(HTREEITEM hItem)				{ SetCheck(hItem, !GetCheck(hItem)); }

	BOOL			SetCheckState(HTREEITEM hItem, TriCheckState tcs = TTCS_CHECKED);
	TriCheckState	GetCheckState(HTREEITEM hItem) const;
	
	virtual void	ToggleCheckSelectedItem();
	virtual void	DeleteSelectedItem();

	inline BOOL		GetDeleteParentIfLastChild() const			{ return m_bDeleteParentIfLast; }
	inline void		SetDeleteParentIfLastChild(BOOL val)		{ m_bDeleteParentIfLast = val; }

	BOOL			DeleteItem(HTREEITEM hItem);
protected:
	BOOL			InitCheckBoxImage();
	void			TravelCheckSiblingAndParent(HTREEITEM hItem, TriCheckState nState);
	void			TravelCheckChildren(HTREEITEM hItem, TriCheckState nState);
	
	virtual void	OnDblClkItem(HTREEITEM hItem);
	virtual BOOL	CanCheckItem(HTREEITEM hItem);

	virtual void	OnClickItemStateIcon(HTREEITEM hItem);
protected:
	void			OnInitTreeCtrl();

	void			OnLButtonDown(UINT nFlags, CPoint point);
	void			OnLButtonDblClk(UINT nFlags, CPoint point);
	void			OnClickItem(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	void 			OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:	
	DECLARE_MESSAGE_MAP()
protected:
	UINT		m_uClickHitTestFlags;
	CImageList	m_ChkBoxImgList;
	BOOL		m_bDeleteParentIfLast;
};

/*----------------------------------------------------------------------------*/
/* CMultiSelTriCheckTreeCtrl
/*----------------------------------------------------------------------------*/

#ifndef CMultiSelTriCheckTreeCtrlBase
	#define CMultiSelTriCheckTreeCtrlBase	CTriCheckStateTreeCtrl
#endif // CMultiSelTriCheckTreeCtrlBase

// Inspired by: CMultiTree by Richard Hazlewood
// http://www.codeguru.com/Cpp/controls/treeview/misc-advanced/article.php/c629/

#include <list>

typedef std::list<HTREEITEM> TreeItemList;

//take the liberty of using 1 for focused item - it used
// to be defined anyway, before MS removed it.
#ifndef TVIS_FOCUSED
	#define TVIS_FOCUSED	1
#else
	#if TVIS_FOCUSED != 1
		#error TVIS_FOCUSED was assumed to be 1
	#endif
#endif

class CMultiSelTriCheckTreeCtrl : public CMultiSelTriCheckTreeCtrlBase
{
public:
	CMultiSelTriCheckTreeCtrl();
	virtual ~CMultiSelTriCheckTreeCtrl();
public:
	inline BOOL	IsMultiSelectable() const					{ return m_bMultiSel; }
	void		SetMultiSelectable(BOOL bMultiSel = TRUE);

	inline BOOL IsAlphaBlendedSelectionEnabled() const		{ return m_bAlphaBlendedSelection; }
	void		EnableAlphaBlendedSelection(BOOL bEnable = TRUE);

	inline BOOL	IsSelected(HTREEITEM hItem) const			{ return !!(TVIS_SELECTED & CMultiSelTriCheckTreeCtrlBase::GetItemState(hItem, TVIS_SELECTED)); }

	size_t		GetSelectedCount() const;
	HTREEITEM	GetFirstSelectedItem() const;
	HTREEITEM	GetNextSelectedItem(HTREEITEM hItem) const;
	HTREEITEM	GetPrevSelectedItem(HTREEITEM hItem) const;

	void		SelectAll(BOOL bSelect = TRUE);
	void		SelectRange(HTREEITEM hFirst, HTREEITEM hLast, BOOL bOnly = TRUE);

	//Note: not virtual, so must call this class (or derivative)
	BOOL		SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask);
	UINT		GetItemState(HTREEITEM hItem, UINT nStateMask) const;

	inline BOOL	HasCapture() const							{ return ::GetCapture() == m_hWnd; }

	virtual void ToggleCheckSelectedItem();
	virtual void DeleteSelectedItem();

	BOOL		IsEmulatedNotify() const					{return m_bEmulated;};
	void		GetSelectedList(TreeItemList& selectedList) const;
protected:
	void		OnPaint();
	
	void		DrawAlphaBlendedSelection(CDC& dc, CRect rcPaint);
	void		UpdateAlphaBlendedSelection( CRect rcOldSelBox, CRect rcNewSelBox, CRect rcUpdateSelBox );
	
	void		OnLButtonDown(UINT nFlags, CPoint point);
	void		OnRButtonDown(UINT nFlags, CPoint point);
	void		OnLButtonUp(UINT nFlags, CPoint point);
	void		OnRButtonUp(UINT nFlags, CPoint point);
	void		OnMouseMove(UINT nFlags, CPoint point);
	
	void		OnSetFocus(CWnd* pOldWnd);
	void		OnKillFocus(CWnd* pNewWnd);
	void		OnCancelMode();
	void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:
	BOOL		OnButtonDown(BOOL bLeft, UINT nFlags, CPoint point);
	BOOL		OnButtonUp(BOOL bLeft, UINT nFlags, CPoint point);
	void		OnEndDragSelection();

	//void		DoAction(HTREEITEM hItem, BOOL bLeft, UINT nFlags, CPoint point);
	void		DoPreSelection(HTREEITEM hItem, BOOL bLeft, UINT nFlags);

	void		SelectAllIgnore(BOOL bSelect, HTREEITEM hIgnore);
	void		UpdateDragSelection(UINT nFlags, const CRect& rcNewSelBox, const CRect& rcUpdateSelBox);

	virtual void	RedrawSelectedItem(BOOL bErase = FALSE);

	void			SetSelectedItemCheckState(TriCheckState tcs);
	virtual void	OnClickItemStateIcon(HTREEITEM hItem);

	BOOL			_SendNotify(LPNMHDR pNMHDR);

	BOOL			PreTranslateMessage(MSG* pMsg);
protected:
	BOOL			m_bMultiSel;
	COLORREF		m_clrSelFill;
	COLORREF		m_clrSelBorder;
	BOOL			m_bAlphaBlendedSelection;	// enables alpha-blended marquee selection

	// States
	BOOL			m_bPendingDragSel;		// mouse button is being holded but may not move yet
	BOOL			m_bDuringDragSel;		// m_bPendingDragSel must also be true when this boolean is true
	BOOL			m_bLeftBtnDragSel;
	CRect			m_rcSelBox;

	HTREEITEM		m_hSelectAnchor;
	TreeItemList	m_itemsInSelBox;		// temporary used during dragging

	static COLORREF	s_clrSelFill;

	BOOL			m_bEmulated;
protected:
	DECLARE_MESSAGE_MAP()
};

/*----------------------------------------------------------------------------*/
/* CIconEdit
/*----------------------------------------------------------------------------*/

#define HYBRID_ICONEDIT

class CIconWnd
#ifdef HYBRID_ICONEDIT
	: public CStatic
#endif // HYBRID_ICONEDIT
{
	// Construction/destruction
public:
	CIconWnd();
	virtual ~CIconWnd();
	
	// Operations
public:
	void		SetIcon(HICON hIcon, BOOL redraw = TRUE, BOOL bAutoDestroy = TRUE);
	void		SetIcon(UINT id, BOOL redraw = TRUE);

	void		DrawIcon(CDC* pDC);
	BOOL		CreateIcon(CWnd* pParent);
	
protected:
	void		DestroyIcon();

#ifdef HYBRID_ICONEDIT
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
#endif // HYBRID_ICONEDIT
		
protected:
	HICON		m_hIcon;
	BOOL		m_bAutoDestroyIcon;
	friend class CIconEdit;
};

#ifndef CIconEditBase
	#define CIconEditBase	CEdit
#endif // CSymbolEditBase

class CIconEdit : public CIconEditBase
{
	DECLARE_DYNAMIC(CIconEdit)
public:
	CIconEdit();
	virtual ~CIconEdit();
	
	void		SetIcon(HICON hIcon, BOOL redraw = TRUE, BOOL bAutoDestroy = TRUE);
	void		SetIcon(UINT id, BOOL redraw = TRUE);
protected:
	void		RecalcLayout();
	virtual void PreSubclassWindow();

	void		DrawIcon(CDC* pDC);
	void		CreateIcon();
	
	void		OnSize(UINT nType, int cx, int cy);
	LRESULT		OnSetFont(WPARAM wParam, LPARAM lParam);
	void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL		OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	BOOL		OnEraseBkgnd(CDC* pDC);
	
	DECLARE_MESSAGE_MAP()
protected:
	CIconWnd	m_iconWnd;
};

/*----------------------------------------------------------------------------*/
/* CComboListBox
/*----------------------------------------------------------------------------*/

#ifndef CComboListBoxBase
#define CComboListBoxBase	CCustomDrawListBox
#endif // CObjInfoListBoxBase

class CComboListBox : public CComboListBoxBase
{
	DECLARE_DYNCREATE(CComboListBox)
public:
	CComboListBox();
	virtual ~CComboListBox();
public:
	void		SetBuddyComboBox(CWnd* pComboBox)						{ m_pComboBox = pComboBox; }
	CWnd*		GetBuddyComboBox() const								{ return m_pComboBox; }
protected:
	LRESULT		OnCustomToolTipNotify(WPARAM wParam, LPARAM lParam);
protected:
	CWnd*		m_pComboBox;
protected:
	DECLARE_MESSAGE_MAP()
};

/*----------------------------------------------------------------------------*/
/* CCustomDrawComboBox
/*----------------------------------------------------------------------------*/

#ifndef CCustomDrawComboBoxBase
	#define CCustomDrawComboBoxBase		CComboBox
#endif

#define MAX_VISIBLE_COMBOLISTITEM_COUNT		30

/*
	If we need to make the listbox control (within the combobox control) a "virtual list box", we need to make it have
	LBS_NODATA style, however, this can not be done after its creation. To make the listbox become virtual, we can hook
	the creation of the combobox and modify the listbox's style.

	However, this solution seems fail if Common Control version 6 is enabled (when a manifest file is used), if we decide
	to use ComCtl v6, then we have to turn off this macro.

	If we don't make the listbox virtual (with the style LBS_NODATA, then we will have to insert/delete items during runtime, 
	that's the trade-off.
*/
#define USE_HOOK_CHANGE_LISTBOX_STYLE

// Here's the suggested way to subclass the internal edit and list box control:
// How to subclass CListBox and CEdit inside of CComboBox
// http://support.microsoft.com/kb/174667
#define SUBCLASS_LISTBOX_EDIT_IN_CTLCOLOR

class CCustomDrawComboBox : public CCustomDrawComboBoxBase
{
	DECLARE_DYNCREATE(CCustomDrawComboBox)
// Construction
public:
	CCustomDrawComboBox();
	virtual ~CCustomDrawComboBox();
	
	// Attributes
public:
	HWND		m_hComboList;
	HWND		m_hComboEdit;
public:
	virtual BOOL	Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL bVirtual = FALSE);
	virtual BOOL	CreateFromCtrl(CWnd* pParent, int nID, BOOL bVirtual = FALSE, DWORD dwStyleAdd = 0);

	int				GetItemCount();
	void			SetItemCount(int nCount);
	void			SetDroppedVisibleItemCount(int nCount);

	virtual CComboListBox& GetComboListBox()							{ return m_ComboListBox; }
	virtual CEdit&	GetComboEdit()										{ return m_ComboEdit; }

	virtual CString GetItemText(UINT nItem);
	virtual int		GetItemIconIndex(UINT nItem);

	virtual CCustomDrawToolTipCtrl& GetCustomDrawToolTips()				{ return GetComboListBox().GetCustomDrawToolTips(); }

	inline void			SetTextDrawer(ITextDrawer* pTextDrawer)			{ GetComboListBox().SetTextDrawer(pTextDrawer); }
	inline ITextDrawer*	GetTextDrawer()									{ return GetComboListBox().GetTextDrawer(); }
protected:
	virtual void	DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void	MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int		CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);

	virtual void	PreAddToolTipForItem(int nItem);
	virtual void	PreShowToolTipForItem(int nItem);
	virtual void	PrePopToolTip();

	void			SetTextOfItem(int nIndex);

	virtual BOOL	OnInitCombo();
	virtual void	OnDestroyCombo();

	void			PreSubclassWindow();
	int				OnCreate(LPCREATESTRUCT lpCreateStruct);
	void			OnDestroy();
#ifdef SUBCLASS_LISTBOX_EDIT_IN_CTLCOLOR
	HBRUSH			OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
#endif // SUBCLASS_LISTBOX_EDIT_IN_CTLCOLOR

	LRESULT			OnCustomToolTipNotify(WPARAM wParam, LPARAM lParam);
protected:
	CFont			m_font;
private:
	CComboListBox	m_ComboListBox;	// No direct access allowed from derived class.
	CEdit			m_ComboEdit;
protected:
	DECLARE_MESSAGE_MAP()
};

/*----------------------------------------------------------------------------*/
/* CXEditPrompt
/*----------------------------------------------------------------------------*/

/*
	Initial version by Hans Dietrich, see http://www.codeproject.com/KB/edit/XEditPrompt.aspx
*/

#ifndef CXEditPromptBase
	#define CXEditPromptBase	CEdit
#endif

class CXEditPrompt : public CXEditPromptBase
{
	DECLARE_DYNCREATE(CXEditPrompt)
	// Construction
public:
	CXEditPrompt();
	virtual ~CXEditPrompt();
	
	// Operations
public:
	void Reset();
	
	// Attributes
public:
	COLORREF	GetPromptColor()	{ return m_crPromptColor; }
	CString		GetPromptText()		{ return m_strPromptText; }
	int			GetWindowText(LPTSTR lpszStringBuf, int nMaxCount) const;
	void		GetWindowText(CString& rString) const;
	void		SetPromptColor(COLORREF crText);
	void		SetPromptText(LPCTSTR lpszPrompt);
	void		SetWindowText(LPCTSTR lpszString);
	
	COLORREF	GetBKColor() const	{ return m_crBkColor; }
	void		SetBKColor(COLORREF crBK);
	int			GetWindowTextLength() const;
	DWORD		GetCueBannerAlign() { return m_dwCueBannerAlign; }
	void		SetCueBannerAlign(DWORD val);
	
	BOOL		IsShowingCueBanner() const { return m_bShowCueBanner; }
protected:
	int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PreSubclassWindow();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
	virtual BOOL OnInitEdit();
protected:
	inline void		UpdateStyleForCueBanner()
	{
		m_dwOldAlignStyle = GetStyle() & (ES_LEFT|ES_CENTER|ES_RIGHT);
		ModifyStyle(0, m_dwCueBannerAlign);
	}
	inline void		UpdateStyleForNormalText()
	{
		ModifyStyle(m_dwCueBannerAlign, m_dwOldAlignStyle);
	}
	inline void		EnableCueBanner(BOOL bEnable = TRUE)
	{
		m_bShowCueBanner = bEnable;
		bEnable ? UpdateStyleForCueBanner() : UpdateStyleForNormalText();
	}
private:
	BOOL		m_bShowCueBanner;	// TRUE = user has not yet selected this control
	CString		m_strPromptText;	// prompt text to display initially
	COLORREF	m_crPromptColor;	// color to use for prompt text
	CBrush		m_brush;			// background brush for WM_CTLCOLOR
	
	COLORREF	m_crBkColor;		// color of background
	CBrush		m_robrush;			// Read-only background brush for WM_CTLCOLOR
	DWORD		m_dwCueBannerAlign;	// ES_LEFT
	DWORD		m_dwOldAlignStyle;
	
	// Generated message map functions
protected:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pOldWnd);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	
	DECLARE_MESSAGE_MAP()
};

/*----------------------------------------------------------------------------*/
/* class CFilterEdit
/*----------------------------------------------------------------------------*/

class CFilterEdit : public CXEditPrompt
{
	DECLARE_DYNCREATE(CFilterEdit)
public:
	CFilterEdit(CWnd* pBuddyWnd = NULL);
	virtual ~CFilterEdit();
public:
	void SetBuddyWindow(CWnd* pBuddyWnd);
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	CWnd*		m_pBuddyWnd;
};

#else
#error ### repeated include of CustomDrawControl.h!!!
#endif // __CUSTOMDRAWCONTROL_H__