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

#ifndef __CUSTOMDRAWUTILS_H__
#define __CUSTOMDRAWUTILS_H__

/*----------------------------------------------------------------------------*/
/* Global control
/*----------------------------------------------------------------------------*/
#define CUSTOMDRAW_THEME_BACKGROUND

// Color for item
#define RGB_ITEM_SELECTED_FILL_COLOR1			RGB(235, 244, 254)
#define RGB_ITEM_SELECTED_FILL_COLOR2			RGB(207, 228, 254)

#define RGB_ITEM_SELFOCUS_FILL_COLOR1			RGB(220, 235, 252)
#define RGB_ITEM_SELFOCUS_FILL_COLOR2			RGB(193, 219, 252)
#define RGB_ITEM_SELFOCUS_OUTER_BORDER_COLOR	RGB(125, 162, 206)
#define RGB_ITEM_SELFOCUS_INNER_BORDER_COLOR	RGB(235, 244, 253)

#define RGB_ITEM_SELNOFOCUS_FILL_COLOR1			RGB(248, 248, 248)
#define RGB_ITEM_SELNOFOCUS_FILL_COLOR2			RGB(229, 229, 229)
#define RGB_ITEM_SELNOFOCUS_OUTER_BORDER_COLOR	RGB(217, 217, 217)
#define RGB_ITEM_SELNOFOCUS_INNER_BORDER_COLOR	RGB(240, 240, 240)

#define RGB_ITEM_HOT_FILL_COLOR1				RGB(250, 251, 253)
#define RGB_ITEM_HOT_FILL_COLOR2				RGB(235, 243, 253)
#define RGB_ITEM_HOT_OUTER_BORDER_COLOR			RGB(184, 214, 251)
#define RGB_ITEM_HOT_INNER_BORDER_COLOR			RGB(252, 243, 254)

// Color for tooltip
#define RGB_TOOLTIP_FILL_COLOR1					RGB(255, 255, 255)
#define RGB_TOOLTIP_FILL_COLOR2					RGB(201, 217, 239)
#define RGB_TOOLTIP_TEXT_COLOR					RGB(0x4C,0x4C,0x4C)		// ::GetSysColor(COLOR_INFOTEXT)
#define RGB_TOOLTIP_BORDER_COLOR				RGB(107, 107, 107)		// ::GetSysColor(COLOR_INFOTEXT)

// Color for button
#define RGB_BUTTON_NORMAL_FILL_COLOR1			RGB(242,242,242)
#define RGB_BUTTON_NORMAL_FILL_COLOR2			RGB(235,235,235)
#define RGB_BUTTON_NORMAL_FILL_COLOR3			RGB(221,221,221)
#define RGB_BUTTON_NORMAL_FILL_COLOR4			RGB(207,207,207)

#define RGB_BUTTON_HOT_FILL_COLOR1				RGB(234,246,253)
#define RGB_BUTTON_HOT_FILL_COLOR2				RGB(217,240,252)
#define RGB_BUTTON_HOT_FILL_COLOR3				RGB(190,230,253)
#define RGB_BUTTON_HOT_FILL_COLOR4				RGB(167,217,245)

#define RGB_BUTTON_PUSHED_FILL_COLOR1			RGB(229,244,252)
#define RGB_BUTTON_PUSHED_FILL_COLOR2			RGB(196,229,249)
#define RGB_BUTTON_PUSHED_FILL_COLOR3			RGB(152,209,239)
#define RGB_BUTTON_PUSHED_FILL_COLOR4			RGB(109,182,221)

#define BUTTON_FIRST_HALF_PERCENTAGE			47

// Color for header
#define RGB_HEADER_NORMAL_FILL_COLOR1			RGB(255,255,255)
#define RGB_HEADER_NORMAL_FILL_COLOR2			RGB(255,255,255)
#define RGB_HEADER_NORMAL_FILL_COLOR3			RGB(247,248,250)
#define RGB_HEADER_NORMAL_FILL_COLOR4			RGB(241,242,244)

#define RGB_HEADER_HOT_FILL_COLOR1				RGB(227,247,255)
#define RGB_HEADER_HOT_FILL_COLOR2				RGB(227,247,255)
#define RGB_HEADER_HOT_FILL_COLOR3				RGB(189,237,255)
#define RGB_HEADER_HOT_FILL_COLOR4				RGB(183,231,251)

#define RGB_HEADER_PUSHED_FILL_COLOR1			RGB(188,228,249)
#define RGB_HEADER_PUSHED_FILL_COLOR2			RGB(227,247,255)
#define RGB_HEADER_PUSHED_FILL_COLOR3			RGB(141,214,247)
#define RGB_HEADER_PUSHED_FILL_COLOR4			RGB(138,209,245)

#define RGB_HEADER_SORT_FILL_COLOR1				RGB(242,249,252)
#define RGB_HEADER_SORT_FILL_COLOR2				RGB(242,249,252)
#define RGB_HEADER_SORT_FILL_COLOR3				RGB(225,241,249)
#define RGB_HEADER_SORT_FILL_COLOR4				RGB(216,236,246)

#define HEADER_FIRST_HALF_PERCENTAGE			41

#define RGB_COMBO_BORDER_NORMAL					RGB(190, 190, 190)
#define RGB_COMBO_BORDER_HILIGHT				RGB(60, 127, 177)

#define RGB_TRANSPARENT							RGB(255,0,255)

#define RGB_SELECTION_BORDER					::GetSysColor(COLOR_HIGHLIGHT)

#define ALPHA_SELECTION_FILL					70

//#define IMPLEMENT_BITMAP_TRANSITION	// TODO


#ifdef CUSTOMDRAW_GRADIENT
BOOL FillGradient(HDC hDC, PRECT pRect, COLORREF c1, COLORREF c2, int nGradientType = GFT_TOP_BOTTOM, BYTE byAlpha = 255);

typedef struct tagCOLORS4
{
	COLORREF c1;
	COLORREF c2;
	COLORREF c3;
	COLORREF c4;
} COLORS4, *PCOLORS4;

typedef const COLORS4	*PCCOLORS4;

void Fill4ColorsGradient(HDC hDC, CRect rect, 
						 COLORREF colorStart1, COLORREF colorFinish1, 
						 COLORREF colorStart2, COLORREF colorFinish2,
						 BOOL bHorz = TRUE,
						 int nPercentage = 50,
						 BYTE byAlpha = 255);

void Fill4ColorsGradient(HDC hDC, CRect rect, 
						 PCCOLORS4 pColors,
						 BOOL bHorz = TRUE,
						 int nPercentage = 50,
						 BYTE byAlpha = 255);

BOOL SimpleGradientRoundRect(HDC hDC, PRECT pRect, COLORREF c1, COLORREF c2, COLORREF clrBorder = CLR_INVALID, int nGradientType = GFT_TOP_BOTTOM, BYTE byAlpha = 255);

void DrawRoundRectItem(CDC* pDC, CRect rect, COLORREF clrFillColor1, COLORREF clrFillColor2, COLORREF clrInnerBorder, COLORREF clrOuterBorder, BYTE byAlpha = 255);

enum CDTBT
{
	CDTBT_PUSHBUTTON,
	CDTBT_HEADER,
	CDTBT_COMBODOWNBUTTON,
	CDTBT_COMBOBUTTON,
};

enum CDSTATEID
{
	CDSTATEID_DISABLED,
	CDSTATEID_HOT,
	CDSTATEID_NORMAL,
	CDSTATEID_PRESSED,
	CDSTATEID_MAX
};

void DrawDropDownArrow(HDC hDC, const RECT* pRect);
HRESULT CustomDrawThemeBackground(HWND hwnd, HDC hDC, CDTBT type, CDSTATEID state, const RECT* pRect, const RECT* pClipRect = NULL);

#endif // CUSTOMDRAW_GRADIENT

void DrawSortArrow(CDC* pDC, CRect rectArrow, BOOL bIsUp);
void DrawSimpleRoundRectBorder( CDC* pDC, RECT rect, COLORREF clrLine, bool bForClipping = false );


void DrawAlphaBlendedSelection( CDC& dc, CRect rcPaint, CRect rcSelBox, COLORREF clrSelFill, COLORREF clrSelBorder );


#ifndef PW_CLIENTONLY
	#define PW_CLIENTONLY           0x00000001
#endif // PW_CLIENTONLY

BOOL OPrintWindow(HWND hwnd, HDC hdcBlt, UINT nFlags = 0);

#ifndef AW_BLEND
	#define AW_HOR_POSITIVE             0x00000001
	#define AW_HOR_NEGATIVE             0x00000002
	#define AW_VER_POSITIVE             0x00000004
	#define AW_VER_NEGATIVE             0x00000008
	#define AW_CENTER                   0x00000010
	#define AW_HIDE                     0x00010000
	#define AW_ACTIVATE                 0x00020000
	#define AW_SLIDE                    0x00040000
	#define AW_BLEND                    0x00080000
#endif // AW_BLEND

#if _MSC_VER < 1400	// MS VC++ 8.0
	BOOL OAnimateWindow(HWND hwnd, DWORD dwTime, DWORD dwFlags);
#else
	#define OAnimateWindow	AnimateWindow
#endif	//  _MSC_VER < 1400

BOOL ORandomAnimateWindow(HWND hwnd, DWORD dwTIme);

// References:
// http://www.codeproject.com/KB/list/CGridListCtrlEx.aspx
// http://msdn.microsoft.com/en-us/library/dd373487(VS.85).aspx

bool CheckOSVersion(WORD requestOS);
bool IsCommonControlsEnabled();
bool IsThemeEnabled();

#ifndef VSCLASS_BUTTON
	#define VSCLASS_BUTTON			L"BUTTON"
#endif // VSCLASS_BUTTON

#ifndef VSCLASS_COMBOBOX
	#define VSCLASS_COMBOBOX		L"COMBOBOX"
#endif // VSCLASS_COMBOBOX

#ifndef VSCLASS_HEADER
	#define VSCLASS_HEADER			L"HEADER"
#endif // VSCLASS_HEADER


#ifdef MSVC_NEW_VER
	#include <VSStyle.h>
#else
	enum BUTTONPARTS {
		BP_PUSHBUTTON = 1,
		BP_RADIOBUTTON = 2,
		BP_CHECKBOX = 3,
		BP_GROUPBOX = 4,
		BP_USERBUTTON = 5,
		BP_COMMANDLINK = 6,
		BP_COMMANDLINKGLYPH = 7,
	};

	enum PUSHBUTTONSTATES {
		PBS_NORMAL = 1,
		PBS_HOT = 2,
		PBS_PRESSED = 3,
		PBS_DISABLED = 4,
		PBS_DEFAULTED = 5,
		PBS_DEFAULTED_ANIMATING = 6,
	};

	enum CHECKBOXSTATES {
		CBS_UNCHECKEDNORMAL = 1,
		CBS_UNCHECKEDHOT = 2,
		CBS_UNCHECKEDPRESSED = 3,
		CBS_UNCHECKEDDISABLED = 4,
		CBS_CHECKEDNORMAL = 5,
		CBS_CHECKEDHOT = 6,
		CBS_CHECKEDPRESSED = 7,
		CBS_CHECKEDDISABLED = 8,
		CBS_MIXEDNORMAL = 9,
		CBS_MIXEDHOT = 10,
		CBS_MIXEDPRESSED = 11,
		CBS_MIXEDDISABLED = 12,
		CBS_IMPLICITNORMAL = 13,
		CBS_IMPLICITHOT = 14,
		CBS_IMPLICITPRESSED = 15,
		CBS_IMPLICITDISABLED = 16,
		CBS_EXCLUDEDNORMAL = 17,
		CBS_EXCLUDEDHOT = 18,
		CBS_EXCLUDEDPRESSED = 19,
		CBS_EXCLUDEDDISABLED = 20,
	};

	enum COMBOBOXPARTS {
		CP_DROPDOWNBUTTON = 1,
		CP_BACKGROUND = 2,
		CP_TRANSPARENTBACKGROUND = 3,
		CP_BORDER = 4,
		CP_READONLY = 5,
		CP_DROPDOWNBUTTONRIGHT = 6,
		CP_DROPDOWNBUTTONLEFT = 7,
		CP_CUEBANNER = 8,
	};

	enum COMBOBOXSTYLESTATES {
		CBXS_NORMAL = 1,
		CBXS_HOT = 2,
		CBXS_PRESSED = 3,
		CBXS_DISABLED = 4,
	};
	
	enum HEADERPARTS {
		HP_HEADERITEM = 1,
		HP_HEADERITEMLEFT = 2,
		HP_HEADERITEMRIGHT = 3,
		HP_HEADERSORTARROW = 4,
		HP_HEADERDROPDOWN = 5,
		HP_HEADERDROPDOWNFILTER = 6,
		HP_HEADEROVERFLOW = 7,
	};

	enum HEADERITEMSTATES {
		HIS_NORMAL = 1,
		HIS_HOT = 2,
		HIS_PRESSED = 3,
		HIS_SORTEDNORMAL = 4,
		HIS_SORTEDHOT = 5,
		HIS_SORTEDPRESSED = 6,
		HIS_ICONNORMAL = 7,
		HIS_ICONHOT = 8,
		HIS_ICONPRESSED = 9,
		HIS_ICONSORTEDNORMAL = 10,
		HIS_ICONSORTEDHOT = 11,
		HIS_ICONSORTEDPRESSED = 12,
	};

	typedef HANDLE HTHEME;          // handle to a section of theme data for class
#endif // MSVC_NEW_VER

// MSDN suggests that: Do not link to UxTheme.lib.
// see http://msdn.microsoft.com/en-us/library/bb773175(VS.85).aspx#whistler_windows	
	
#ifdef LINK_TO_UXTHEME
	#include <Uxtheme.h>
	#pragma comment(lib, "UxTheme.lib")

	#define OOpenThemeData								OpenThemeData
	#define OCloseThemeData								CloseThemeData
	#define OSetWindowTheme								SetWindowTheme
	#define ODrawThemeBackground						DrawThemeBackground
	#define OIsThemeBackgroundPartiallyTransparent		IsThemeBackgroundPartiallyTransparent
	#define ODrawThemeParentBackground					DrawThemeParentBackground
#else
	HTHEME	OOpenThemeData(HWND hwnd, LPCWSTR pszClassList);
	HRESULT	OCloseThemeData(HTHEME hTheme);
	HRESULT	OSetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
	HRESULT	ODrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect);
	BOOL	OIsThemeBackgroundPartiallyTransparent(HTHEME hTheme, int iPartId, int iStateId);
	HRESULT	ODrawThemeParentBackground(HWND hwnd, HDC hdc, const RECT* prc);
#endif // LINK_TO_UXTHEME

LRESULT EnableWindowTheme(HWND hwnd, LPCWSTR classList, LPCWSTR subApp, LPCWSTR idlist);

SIZE GetBmpSize(HBITMAP hBmp);
SIZE GetIconSize(HICON hIcon);

/*----------------------------------------------------------------------------*/
/* CCustomDrawHelper
/*----------------------------------------------------------------------------*/
class CCustomDrawHelper
{
public:
	CCustomDrawHelper();
	virtual ~CCustomDrawHelper();
public:
	void DrawItemBackground(CDC* pDC, CRect rect, BYTE byAlpha = 255);
	void DrawFocus(CDC* pDC, CRect rect, BYTE byAlpha = 255);

#ifdef CUSTOMDRAW_GRADIENT
	void DrawThemeBackground(CDC* pDC, CRect rect, CDTBT type);
#endif // CUSTOMDRAW_GRADIENT

protected:
	inline bool ShouldFillBackground()
	{
#ifdef CUSTOMDRAW_GRADIENT
		bool bShouldFill = m_bSelected || m_bIsHotItem;
#else
		bool bShouldFill = m_bSelected;
#endif // CUSTOMDRAW_GRADIENT
		return bShouldFill;
	}
public:
	bool m_bWndHasFocus;
	bool m_bFocusItem;
	bool m_bSelected;
	bool m_bIsHotItem;
	bool m_bDrawBorderWhenFill;
	bool m_bIsPressed;
	bool m_bWndEnabled;

	CRect	m_rcBounds;
	HWND	m_hWnd;
};

/*----------------------------------------------------------------------------*/
/* CMemBitmap
/*----------------------------------------------------------------------------*/
class CMemBitmap
{
public:
	CMemBitmap(CDC* pDC = NULL, CBitmap* pBmp = NULL);
	virtual ~CMemBitmap();
public:
	BOOL			BeginDraw(int cx, int cy, BOOL bAutoCreate = TRUE);
	void			EndDraw();

	inline BOOL		BeginDraw(SIZE size)		{ return BeginDraw(size.cx, size.cy); }

	inline CDC&		GetDC()						{ return m_dcMemory; }
	inline const CDC&	GetDC() const			{ return m_dcMemory; }
	void			SetCompatibleDC(CDC* pDC);
	inline void		SetCompatibleDC(CDC& dc)	{ SetCompatibleDC(&dc); }

	inline CBitmap&	GetBitmap()					{ return m_pBmp ? *m_pBmp : m_bmp; }
	inline CBitmap*	GetBitmapPtr()				{ return m_pBmp ? m_pBmp : &m_bmp; }
	void			SetBitmap(CBitmap* pBmp);
	inline void		SetBitmap(CBitmap& bmp)		{ SetBitmap(&bmp); }

	inline const CSize& GetBitmapSize() const	{ return m_sizeBmp; }
	inline const int	GetBitmapWidth() const	{ return m_sizeBmp.cx; }
	inline const int	GetBitmapHeight() const	{ return m_sizeBmp.cy; }

	operator CDC&()								{ return GetDC(); }
	operator HDC()								{ return GetDC(); }
	CMemBitmap& operator =(CMemBitmap& src);
protected:
	CDC*		m_pDC;
	CBitmap*	m_pBmp;

	CDC			m_dcMemory;
	CBitmap		m_bmp;

	CBitmap*	m_pBmpOld;
	CSize		m_sizeBmp;
};

/*----------------------------------------------------------------------------*/
/* CSimpleMemDC
/*----------------------------------------------------------------------------*/
enum OUTPUTTYPE
{
	OUTPUTTYPE_NOP,
	OUTPUTTYPE_BITBLT,
	OUTPUTTYPE_TRANSPARENTBLT,
};

class CSimpleMemDC
{
public:
	CSimpleMemDC(CDC& dc, CWnd* pWnd, OUTPUTTYPE type = OUTPUTTYPE_BITBLT);
	CSimpleMemDC(CDC& dc, const CRect& rect, OUTPUTTYPE type = OUTPUTTYPE_BITBLT);
	
	virtual ~CSimpleMemDC();
public:
	inline CDC& GetDC()							{ return m_memBitmap.GetDC(); }
	inline CMemBitmap&	GetMemBitmap()			{ return m_memBitmap; }

	operator CDC&()								{ return GetDC(); }
	operator HDC()								{ return GetDC(); }
	CSimpleMemDC& operator =(CSimpleMemDC& src);
	CSimpleMemDC& operator =(CMemBitmap& src);

	inline void InitTransparentBackground(LPCRECT lpcRect = NULL)
	{
		GetDC().FillSolidRect(lpcRect ? lpcRect : m_rect, m_clrTransparent = RGB_TRANSPARENT);
	}
protected:
	inline BOOL PaintResult(const CRect& rectDraw, int nSrcX, int nSrcY)
	{
		switch (m_outputType)
		{
		case OUTPUTTYPE_BITBLT:
			return m_dc.BitBlt(rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(), &m_memBitmap.GetDC(), nSrcX, nSrcY, m_dwRop);
		case OUTPUTTYPE_TRANSPARENTBLT:
			return TransparentBlt(m_dc, rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(), 
				m_memBitmap.GetDC(), nSrcX, nSrcY, rectDraw.Width(), rectDraw.Height(), m_clrTransparent);
		case OUTPUTTYPE_NOP:
		default:
			return TRUE;
		}
		return FALSE;
	}
public:
	COLORREF		m_clrTransparent;	// note: this member won't be initialized, you must set it manually
	DWORD			m_dwRop;
protected:
	CDC&			m_dc;
	CRect			m_rect;
	CMemBitmap		m_memBitmap;
	CWnd*			m_pWnd;
	OUTPUTTYPE		m_outputType;

	CPoint			m_ptOldWndOrg;
};

/*----------------------------------------------------------------------------*/
/* ITextDrawer
/*----------------------------------------------------------------------------*/
class ITextDrawer
{
public:
	ITextDrawer(CWnd* pWnd = NULL)
		: m_pWnd(pWnd)
	{
		;
	}
	virtual ~ITextDrawer()
	{
		;
	}
public:
	virtual int Draw(CDC* pDC, const CString& strText, LPRECT lpRect, UINT nFormat = 0) = 0;
protected:
	CWnd*		m_pWnd;
};

#else
#error ### repeated include of CustomDrawUtils.h!!!
#endif // __CUSTOMDRAWUTILS_H__