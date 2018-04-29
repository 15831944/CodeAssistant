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
#include "CustomDrawUtils.h"

#include <shlwapi.h>	// IsThemeEnabled

#pragma comment(lib, "msimg32.lib")		// Import GDI GradientFill() and AlphaBlend()

void DrawSimpleRoundRectBorder( CDC* pDC, RECT rect, COLORREF clrLine, bool bForClipping /*= false*/ )
{
	const int nOffset = 2;
	
	CPen penLine(PS_SOLID, 1, clrLine);
	CPen* pOldPen = pDC->SelectObject(&penLine);
	
	int nExtraOffset;
	if ( bForClipping )
	{
		nExtraOffset = 1;
		++rect.right;
		++rect.bottom;
	}
	else
	{
		nExtraOffset = 0;
	}
	
	// top border
	pDC->MoveTo(rect.left + nOffset, rect.top);
	pDC->LineTo(rect.right - nOffset - 1, rect.top);
	
	// top-right corner
	pDC->LineTo(rect.right - 1, rect.top + nOffset);
	
	// right border
	pDC->LineTo(rect.right - 1, rect.bottom - 1 - nOffset - nExtraOffset);
	
	// bottom-right corner
	pDC->LineTo(rect.right - nOffset - 1, rect.bottom - 1 - nExtraOffset);
	
	// bottom border
	pDC->LineTo(rect.left + nOffset + nExtraOffset, rect.bottom - 1);
	
	// bottom-left corner
	pDC->LineTo(rect.left, rect.bottom - 1 - nOffset - nExtraOffset);
	
	// left border
	pDC->LineTo(rect.left, rect.top + nOffset);
	
	// top-left corner
	pDC->LineTo(rect.left + nOffset, rect.top);
	
	pDC->SelectObject(pOldPen);
}

BOOL OPrintWindow( HWND hwnd, HDC hdcBlt, UINT nFlags /*= 0*/ )
{
	typedef BOOL (WINAPI *PFNPrintWindow)( HWND hwnd, HDC hdcBlt, UINT nFlags );
	static PFNPrintWindow pfn = NULL;
	if ( NULL == pfn )
	{
		HMODULE hModule = GetModuleHandle(_T("User32.dll"));
		ASSERT(hModule);	// can't fail!
		pfn = (PFNPrintWindow)GetProcAddress(hModule, _T("PrintWindow"));
	}
	if (pfn)
		return pfn(hwnd, hdcBlt, nFlags);
	return FALSE;
}

#if _MSC_VER < 1400
BOOL OAnimateWindow(HWND hwnd, DWORD dwTime, DWORD dwFlags)
{
	typedef     BOOL (WINAPI *PFNAnimateWindow)(HWND hwnd, DWORD dwTime, DWORD dwFlags);
	static      PFNAnimateWindow      l_pfn = NULL;
	if ( NULL == l_pfn )
	{
		HMODULE           hModule = ::GetModuleHandle(_T("User32.dll"));
		ASSERT( hModule );            // cannot fail
		
		l_pfn = (PFNAnimateWindow)GetProcAddress(hModule, _T("AnimateWindow"));
		if ( NULL == l_pfn )
		{
			ASSERT(FALSE);          // something wrong?
			return FALSE;
		}
	}
	return l_pfn(hwnd, dwTime, dwFlags);
}
#endif	 // _MSC_VER < 1400

BOOL ORandomAnimateWindow( HWND hwnd, DWORD dwTIme )
{
	srand( (unsigned)time( NULL ) );
	int rnd = rand();
	DWORD dwAnimateFlags = AW_ACTIVATE;
	if ( (rnd % 6) == 0 )
		dwAnimateFlags |= AW_BLEND;
	else
	{
		rnd = 1 << (rnd % 5);
		dwAnimateFlags |= AW_SLIDE|rnd;
	}

	return ::OAnimateWindow(hwnd, dwTIme, dwAnimateFlags);
}

/*----------------------------------------------------------------------------*/
/* GradientDrawParams
/*----------------------------------------------------------------------------*/
#ifdef CUSTOMDRAW_GRADIENT

#ifdef __ENABLE_GDIPLUS__
	#include <math.h>	// for atan2()
	#ifndef PI
		#define PI 3.14159f
	#endif

Gdiplus::REAL getAngleForLinearGradient( int nGradientType, const Gdiplus::Rect& rect )
{
	switch (nGradientType)
	{
	case GFT_TOP_BOTTOM:
	case GFT_VERTICAL_CENTER_OUT:
		return 90.0f;
	case GFT_BOTTOM_TOP:
	case GFT_VERTICAL_CENTER_IN:
		return -90.0f;
	case GFT_LEFT_RIGHT:
	case GFT_HORIZONTAL_CENTER_OUT:
		return 0.0f;
	case GFT_RIGHT_LEFT:
	case GFT_HORIZONTAL_CENTER_IN:
		return 180.0f;
	case GFT_DIAG_TL_BR:
	case GFT_DIAG_UP_CENTER_OUT:
		return (Gdiplus::REAL)atan2( (float)rect.Width, (float)rect.Height ) * 180.0f / PI;
	case GFT_DIAG_TR_BL:
	case GFT_DIAG_DOWN_CENTER_OUT:
		return (Gdiplus::REAL)(180.0f - atan2( (float)rect.Width, (float)rect.Height ) * 180.0f / PI);
	default:
		ASSERT(FALSE);
		break;
	}
	return 0.0f;
}

void RectToPoints( const Gdiplus::Rect& rect, Gdiplus::Point (&pts)[4] )
{
	pts[0].X = pts[3].X = rect.X;
	pts[0].Y = pts[1].Y = rect.Y;
	pts[1].X = pts[2].X = rect.X + rect.Width;
	pts[2].Y = pts[3].Y = rect.Y + rect.Height;
}

struct GradientDrawParams
{
	HDC			m_hDC;
	int			m_nGradientType;
	COLORREF	m_c1;
	COLORREF	m_c2;
};

#define GDIPLUSCOLOR_FROM_CRF_ALPHA_ARGUMENT_LIST(_clr, _byAlpha)		(_byAlpha), GetRValue(_clr), GetGValue(_clr), GetBValue(_clr)

Gdiplus::Brush*	GetBrushForRect(GradientDrawParams& params, const Gdiplus::Rect* pRect, BYTE byAlpha = 255)
{
	Gdiplus::Brush* pBrush = NULL;
	Gdiplus::Color color1( GDIPLUSCOLOR_FROM_CRF_ALPHA_ARGUMENT_LIST(params.m_c1, byAlpha) );
	Gdiplus::Color color2( GDIPLUSCOLOR_FROM_CRF_ALPHA_ARGUMENT_LIST(params.m_c2, byAlpha) );
	if ( IS_GFT_LINEAR_DIRECTION(params.m_nGradientType) )
	{
		BOOL bNeedReverseColors	= IS_GFT_CENTER_OUT(params.m_nGradientType);
		Gdiplus::Color* pColor1	= bNeedReverseColors ? &color2 : &color1;
		Gdiplus::Color* pColor2	= bNeedReverseColors ? &color1 : &color2;
		Gdiplus::REAL rAngle	= getAngleForLinearGradient(params.m_nGradientType, *pRect);
		
		BOOL bIsCenterOutIn		= IS_GFT_CENTER_OUT_IN(params.m_nGradientType);
		
		Gdiplus::LinearGradientBrush* pLinGrBrush = new Gdiplus::LinearGradientBrush(*pRect, *pColor1, *pColor2, rAngle);
		
		if ( pLinGrBrush )
		{
			Gdiplus::REAL rFocus = bIsCenterOutIn ? 0.5f : 1.0f;
			pLinGrBrush->SetBlendBellShape(rFocus);
			pLinGrBrush->SetGammaCorrection(TRUE);
			pLinGrBrush->SetWrapMode(Gdiplus::WrapModeTileFlipXY);
		}
		
		return pLinGrBrush;
	}
	else
	{
		Gdiplus::Point pts[4];
		RectToPoints(*pRect, pts);
		Gdiplus::PathGradientBrush* pPathGrBrush = new Gdiplus::PathGradientBrush(pts, 4);
		if ( pPathGrBrush )
		{
			pPathGrBrush->SetCenterColor(color1);
			INT nCount = 1;
			pPathGrBrush->SetSurroundColors(&color2, &nCount);
			if ( !IS_GFT_FROM_CENTER(params.m_nGradientType) )
			{
				const INT nPtIndex = GET_GFT_CORNER_INDEX(params.m_nGradientType);
				ASSERT(nPtIndex >= 0 && nPtIndex < 4);
				pPathGrBrush->SetCenterPoint( pts[nPtIndex] );
			}
			
			pPathGrBrush->SetBlendBellShape(1.0f);
			
			pPathGrBrush->SetGammaCorrection(TRUE);
			pPathGrBrush->SetWrapMode(Gdiplus::WrapModeTileFlipXY);
			pBrush = pPathGrBrush;
		}
	}
	return pBrush;
}

BOOL FillGradient(HDC hDC, PRECT pRect, COLORREF c1, COLORREF c2, int nGradientType /*= GFT_TOP_BOTTOM*/, BYTE byAlpha /*= 255*/)
{
	if ( !pRect || !hDC || !IS_VALID_GRADIENT_TYPE(nGradientType) )
	{
		ASSERT(FALSE);
		return FALSE;
	}

	GradientDrawParams params;
	params.m_hDC	= hDC;
	params.m_c1		= c1;
	params.m_c2		= c2;
	params.m_nGradientType	= nGradientType;
	
	Gdiplus::Rect brushRect(pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top);
	
	Gdiplus::Brush* pBrush = GetBrushForRect(params, &brushRect, byAlpha);
	
	if ( !pBrush )
	{
		ASSERT(FALSE);
		return FALSE;
	}
	
	Gdiplus::Graphics gg(hDC);
	Gdiplus::Status status = gg.FillRectangle(pBrush, brushRect);
	
	delete pBrush;
	
	return status == Gdiplus::Ok;
}
#else	// __ENABLE_GDIPLUS__

// Reference: <Windows Graphics Programming Win32 GDI and DirectDraw> by FengYuan
// Chapter 9.7 GRADIENT FILLS

#define IMPLEMENT_COLORREF_TO_16(cName)	\
	inline COLOR16 Get##cName##16Value(COLORREF cc)\
	{\
		return MAKEWORD(0x00, Get##cName##Value(cc));\
	}\
\
	inline COLOR16 Get##cName##16Value(COLORREF c1, COLORREF c2)\
	{\
		return MAKEWORD(0x00, (Get##cName##Value(c1) + Get##cName##Value(c2))/2 );\
	}

IMPLEMENT_COLORREF_TO_16(R)
IMPLEMENT_COLORREF_TO_16(G)
IMPLEMENT_COLORREF_TO_16(B)

#define DECLARE_COLOR16(cName, cRef)	\
	const COLOR16 cName##_R = GetR16Value(cRef);\
	const COLOR16 cName##_G = GetG16Value(cRef);\
	const COLOR16 cName##_B = GetB16Value(cRef);

#define DECLARE_COLOR16_MID(cName, cRef1, cRef2)	\
	const COLOR16 cName##_R = GetR16Value(cRef1, cRef2);\
	const COLOR16 cName##_G = GetG16Value(cRef1, cRef2);\
	const COLOR16 cName##_B = GetB16Value(cRef1, cRef2);

static BOOL _gradient_fill_rect_single_direction(HDC hDC, PRECT pRect, COLORREF c0, COLORREF c1, int nGradientType)
{
	BOOL bRet = FALSE;
	DECLARE_COLOR16(c0, c0)
	DECLARE_COLOR16(c1, c1)
	DECLARE_COLOR16_MID(cMid, c0, c1)
	TRIVERTEX vtx[4] = {
		{pRect->left,	pRect->top,		c0_R,	c0_G,	c0_B,	0},
		{pRect->right,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0},
		{pRect->left,	pRect->bottom,	cMid_R,	cMid_G,	cMid_B,	0}, 
		{pRect->right,	pRect->top,		cMid_R,	cMid_G,	cMid_B,	0}
	};
	ULONG meshIndices[] = {0, 1, 2, 0, 1, 3};
	
	switch (nGradientType)
	{
	case GFT_TOP_BOTTOM:
		bRet = GradientFill(hDC, vtx, 2, meshIndices, 1, GRADIENT_FILL_RECT_V);
		break;
	case GFT_LEFT_RIGHT:
		bRet = GradientFill(hDC, vtx, 2, meshIndices, 1, GRADIENT_FILL_RECT_H);
		break;
	case GFT_DIAG_TL_BR:
		bRet = GradientFill(hDC, vtx, 4, meshIndices, 2, GRADIENT_FILL_TRIANGLE);
		break;
	case GFT_DIAG_TR_BL:
		vtx[0].x = pRect->right;
		vtx[3].x = pRect->left;
		vtx[1].x = pRect->left;
		vtx[2].x = pRect->right;
		bRet = GradientFill(hDC, vtx, 4, meshIndices, 2, GRADIENT_FILL_TRIANGLE);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return bRet;
}

static BOOL _gradient_fill_rect_from_center(HDC hDC, PRECT pRect, COLORREF c0, COLORREF c1, int nGradientType)
{
	const int nMidX = pRect->left + (pRect->right - pRect->left)/2;
	const int nMidY = pRect->top + (pRect->bottom - pRect->top)/2;
	DECLARE_COLOR16(c0, c0)
	DECLARE_COLOR16(c1, c1)
	TRIVERTEX vtx[5] = {
		{pRect->left,	pRect->top,		c1_R,	c1_G,	c1_B,	0},
		{pRect->right,	pRect->top,		c1_R,	c1_G,	c1_B,	0},
		{pRect->right,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0}, 
		{pRect->left,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0},
		{nMidX,			nMidY,			c0_R,	c0_G,	c0_B,	0},
	};
	
	ULONG meshIndices[] = {0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4};
	return GradientFill(hDC, vtx, 5, meshIndices, 4, GRADIENT_FILL_TRIANGLE);
}

static BOOL _gradient_fill_rect_from_corner(HDC hDC, PRECT pRect, COLORREF c0, COLORREF c1, int nGradientType)
{
	DECLARE_COLOR16(c0, c0)
	DECLARE_COLOR16(c1, c1)
	TRIVERTEX vtx[4] = {
		{pRect->left,	pRect->top,		c1_R,	c1_G,	c1_B,	0},
		{pRect->right,	pRect->top,		c1_R,	c1_G,	c1_B,	0},
		{pRect->right,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0}, 
		{pRect->left,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0}
	};
	
	int nCorner = GET_GFT_CORNER_INDEX(nGradientType);
	vtx[nCorner].Red   = c0_R;
	vtx[nCorner].Green = c0_G;
	vtx[nCorner].Blue  = c0_B;

	ULONG meshIndices[] = {nCorner, (nCorner+1)%4, (nCorner+2)%4, nCorner, (nCorner+3)%4, (nCorner+2)%4};
	return GradientFill(hDC, vtx, 4, meshIndices, 2, GRADIENT_FILL_TRIANGLE);
}

static BOOL _gradient_fill_rect_center_out(HDC hDC, PRECT pRect, COLORREF c0, COLORREF c1, int nGradientType)
{
	BOOL bRet = FALSE;
	RECT rect1 = *pRect;
	RECT rect2 = *pRect;;
	switch (nGradientType)
	{
	case GFT_VERTICAL_CENTER_OUT:
	case GFT_HORIZONTAL_CENTER_OUT:
		{
			const bool bHorizontal = GFT_VERTICAL_CENTER_OUT == nGradientType;
			nGradientType = bHorizontal ? GFT_TOP_BOTTOM : GFT_LEFT_RIGHT;
			if ( bHorizontal )
				rect1.bottom = rect2.top = pRect->top + (pRect->bottom - pRect->top)/2;
			else
				rect1.right = rect2.left = pRect->left + (pRect->right - pRect->left)/2;
			// Fill the two parts
			bRet = _gradient_fill_rect_single_direction(hDC, &rect1, c1, c0, nGradientType);
			bRet &= _gradient_fill_rect_single_direction(hDC, &rect2, c0, c1, nGradientType);
		}
		break;
	case GFT_DIAG_UP_CENTER_OUT:
	case GFT_DIAG_DOWN_CENTER_OUT:
		{
			const bool bDiagonalUp = GFT_DIAG_UP_CENTER_OUT == nGradientType;
			DECLARE_COLOR16(c0, c0)
			DECLARE_COLOR16(c1, c1)
			TRIVERTEX vtx[4] = {
				{pRect->left,	pRect->top,		c1_R,	c1_G,	c1_B,	0},
				{pRect->right,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0},
				{pRect->left,	pRect->bottom,	c1_R,	c1_G,	c1_B,	0}, 
				{pRect->right,	pRect->top,		c1_R,	c1_G,	c1_B,	0}
			};
			const int nC1IndexBegin = bDiagonalUp ? 2 : 0;	// the index of vertices that should be filled in color c1
			for (int ii = nC1IndexBegin; ii < nC1IndexBegin + 2; ++ii)
			{
				vtx[ii].Red		= c0_R;
				vtx[ii].Green	= c0_G;
				vtx[ii].Blue	= c0_B;
			}
			ULONG meshIndices[] = {0, 2, 3, 2, 1, 3};
			if ( !bDiagonalUp )
			{
				meshIndices[2] = 1;	// change 3 to 1
				meshIndices[3] = 0;	// change 2 to 0
			}
			bRet = GradientFill(hDC, vtx, 4, meshIndices, 2, GRADIENT_FILL_TRIANGLE);
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return bRet;
}

#define CHECK_CORRECT_GRADIENT_COLOR(_nGrad1, _nGrad2, _nGradientType, _c0, _c1)	\
		{\
			if ( _nGrad1 == _nGradientType || _nGrad2 == _nGradientType )\
			{\
				--_nGradientType;\
				COLORREF ctmp = _c0;\
				_c0 = _c1;\
				_c1 = ctmp;\
			}\
		}

#ifndef AC_SRC_ALPHA
	#define AC_SRC_ALPHA 0x01
#endif

BOOL FillGradient(HDC hDC, PRECT pRect, COLORREF c1, COLORREF c2, int nGradientType /*= GFT_TOP_BOTTOM*/, BYTE byAlpha /*= 255*/)
{
#ifdef _DEBUG
	if ( !pRect || !IS_VALID_GRADIENT_TYPE(nGradientType) )
	{
		ASSERT(FALSE);
		return FALSE;
	}
#endif // _DEBUG
	if (0 == byAlpha)
		return TRUE;

	BOOL bAlphaBlend = byAlpha < 255;
	BOOL bRet = FALSE;
	HDC hDrawDC = hDC;
	CRect rectDraw(pRect);

	CMemBitmap bitmapDrawer(bAlphaBlend ? CDC::FromHandle(hDC) : NULL);
	int cx = rectDraw.Width();
	int cy = rectDraw.Height();

	CRect rectGradient(pRect);
	if ( bAlphaBlend )
	{
		if (bitmapDrawer.BeginDraw(cx*2, cy*2))
		{
			hDrawDC = bitmapDrawer.GetDC().GetSafeHdc();
			rectGradient.left = 0;
			rectGradient.top = 0;
			rectGradient.right = cx;
			rectGradient.bottom = cy;
		}
		else
		{
			bAlphaBlend = FALSE;
		}
	}

	if ( IS_GFT_FROM_CENTER(nGradientType) )
	{
		bRet = _gradient_fill_rect_from_center(hDrawDC, rectGradient, c1, c2, nGradientType);
	}
	else if ( IS_GFT_FROM_CORNER(nGradientType) )
	{
		bRet = _gradient_fill_rect_from_corner(hDrawDC, rectGradient, c1, c2, nGradientType);
	}
	else if ( IS_GFT_CENTER_OUT_IN(nGradientType) )
	{
		CHECK_CORRECT_GRADIENT_COLOR ( GFT_VERTICAL_CENTER_IN, GFT_HORIZONTAL_CENTER_IN, nGradientType, c1, c2);
		bRet = _gradient_fill_rect_center_out(hDrawDC, rectGradient, c1, c2, nGradientType);
	}
	else
	{
		// must be single direction now.
		ASSERT( IS_GFT_SINGLE_DIRECTION(nGradientType) );
		CHECK_CORRECT_GRADIENT_COLOR ( GFT_BOTTOM_TOP, GFT_RIGHT_LEFT, nGradientType, c1, c2);
		bRet = _gradient_fill_rect_single_direction(hDrawDC, rectGradient, c1, c2, nGradientType);
	}

	if (bAlphaBlend)
	{
		BLENDFUNCTION bf;
		bf.BlendFlags			= 0;
		bf.AlphaFormat			= 0;
		bf.BlendOp				= AC_SRC_OVER;
		bf.SourceConstantAlpha	= byAlpha;
		bRet = ::AlphaBlend(hDC, rectDraw.left, rectDraw.top, cx, cy, hDrawDC, 0, 0, cx, cy, bf);
		bitmapDrawer.EndDraw();
	}

	return bRet;
}
#endif // __ENABLE_GDIPLUS__

BOOL SimpleGradientRoundRect(HDC hDC, 
							 PRECT pRect, 
							 COLORREF c1, 
							 COLORREF c2, 
							 COLORREF clrBorder /*= CLR_INVALID*/, 
							 int nGradientType /*= GFT_TOP_BOTTOM*/,
							 BYTE byAlpha /*= 255*/
							 )
{
	if ( !pRect || !hDC || !IS_VALID_GRADIENT_TYPE(nGradientType) )
	{
		ASSERT(FALSE);
		return FALSE;
	}
	
	// Set the clip region.
	CDC* pDC = CDC::FromHandle(hDC);
	
	VERIFY( ::BeginPath(hDC) );
	
	DrawSimpleRoundRectBorder(pDC, *pRect, clrBorder, true);
	
	VERIFY( ::EndPath(hDC) );
	
	::SelectClipPath(hDC, RGN_COPY);
	
	BOOL bRet = FillGradient(hDC, pRect, c1, c2, nGradientType, byAlpha);
	
	if ( clrBorder != CLR_INVALID )
	{
		DrawSimpleRoundRectBorder(pDC, *pRect, clrBorder);
	}

	// Restore the clipping region
	::SelectClipRgn(hDC, NULL);
	
	return bRet;
}

void DrawRoundRectItem(CDC* pDC, CRect rect, 
					   COLORREF clrFillColor1, COLORREF clrFillColor2, 
					   COLORREF clrInnerBorder, COLORREF clrOuterBorder, 
					   BYTE byAlpha /*= 255*/
					   )
{
	CRect rectInner(rect);
	rectInner.DeflateRect(1, 1);
	SimpleGradientRoundRect(pDC->GetSafeHdc(), rectInner, clrFillColor1, clrFillColor2, clrInnerBorder, GFT_TOP_BOTTOM, byAlpha);
	DrawSimpleRoundRectBorder(pDC, rect, clrOuterBorder);
}

void Fill4ColorsGradient( HDC hDC, CRect rect, 
						 COLORREF colorStart1, COLORREF colorFinish1, 
						 COLORREF colorStart2, COLORREF colorFinish2, 
						 BOOL bHorz /*= TRUE*/, 
						 int nPercentage /*= 50*/,
						 BYTE byAlpha /*= 255*/
						 )
{
	ASSERT(nPercentage >= 0);
	ASSERT(nPercentage <= 100);
	
	CRect rectFirst = rect;
	CRect rectSecond = rect;
	
	if (!bHorz)
	{
		rectFirst.right = rectFirst.left + rectFirst.Width() * nPercentage / 100;
		rectSecond.left = rectFirst.right;
	}
	else
	{
		rectFirst.bottom = rectFirst.top + rectFirst.Height() * nPercentage / 100;
		rectSecond.top = rectFirst.bottom;
	}
	int nGradientType = bHorz ? GFT_TOP_BOTTOM : GFT_LEFT_RIGHT;
	FillGradient(hDC, rectFirst, colorStart1, colorFinish1, nGradientType, byAlpha);
	FillGradient(hDC, rectSecond, colorStart2, colorFinish2, nGradientType, byAlpha);
}

void Fill4ColorsGradient( HDC hDC, CRect rect, PCCOLORS4 pColors, BOOL bHorz /*= TRUE*/, int nPercentage /*= 50*/, BYTE byAlpha /*= 255*/ )
{
	ASSERT(pColors);
	Fill4ColorsGradient(hDC, rect, pColors->c1, pColors->c2, pColors->c3, pColors->c4, bHorz, nPercentage, byAlpha);
}

#ifdef CUSTOMDRAW_THEME_BACKGROUND
const static COLORS4 s_ButtonClrsNormal		= {RGB_BUTTON_NORMAL_FILL_COLOR1,	RGB_BUTTON_NORMAL_FILL_COLOR2,	
												RGB_BUTTON_NORMAL_FILL_COLOR3,	RGB_BUTTON_NORMAL_FILL_COLOR4};

const static COLORS4 s_ButtonClrsHot		= {RGB_BUTTON_HOT_FILL_COLOR1,		RGB_BUTTON_HOT_FILL_COLOR2,	
												RGB_BUTTON_HOT_FILL_COLOR3,		RGB_BUTTON_HOT_FILL_COLOR4};

const static COLORS4 s_ButtonClrsPressed	= {RGB_BUTTON_PUSHED_FILL_COLOR1,	RGB_BUTTON_PUSHED_FILL_COLOR2,	
												RGB_BUTTON_PUSHED_FILL_COLOR3,	RGB_BUTTON_PUSHED_FILL_COLOR4};

const static COLORS4 s_HeaderClrsNormal		= {RGB_HEADER_NORMAL_FILL_COLOR1,	RGB_HEADER_NORMAL_FILL_COLOR2,	
												RGB_HEADER_NORMAL_FILL_COLOR3,	RGB_HEADER_NORMAL_FILL_COLOR4};

const static COLORS4 s_HeaderClrsHot		= {RGB_HEADER_HOT_FILL_COLOR1,		RGB_HEADER_HOT_FILL_COLOR2,	
												RGB_HEADER_HOT_FILL_COLOR3,		RGB_HEADER_HOT_FILL_COLOR4};

const static COLORS4 s_HeaderClrsPressed	= {RGB_HEADER_PUSHED_FILL_COLOR1,	RGB_HEADER_PUSHED_FILL_COLOR2,	
												RGB_HEADER_PUSHED_FILL_COLOR3,	RGB_HEADER_PUSHED_FILL_COLOR4};

typedef struct tagItem3DBackgrnd 
{
	PCCOLORS4	clrs[CDSTATEID_MAX];
	int			nPercentage;
}Item3DBackgrnd, *PItem3DBackgrnd;

const static Item3DBackgrnd s_itemButton = {
	NULL, &s_ButtonClrsHot, &s_ButtonClrsNormal, &s_ButtonClrsPressed, BUTTON_FIRST_HALF_PERCENTAGE
};

const static Item3DBackgrnd s_itemHeader = {
	NULL, &s_HeaderClrsHot, &s_HeaderClrsNormal, &s_HeaderClrsPressed, HEADER_FIRST_HALF_PERCENTAGE
};

const static Item3DBackgrnd s_itemComboBtn = {
	NULL, &s_ButtonClrsHot, NULL, &s_ButtonClrsPressed, BUTTON_FIRST_HALF_PERCENTAGE
};

const static void DrawBorderLines( CDC* pDC, CRect rect, COLORREF clrLine, UINT nOffset)
{
	CPen penLine(PS_SOLID, 1, clrLine);
	CPen* pOldPen = pDC->SelectObject(&penLine);
	
	// top border
	pDC->MoveTo(rect.left+nOffset, rect.top);
	pDC->LineTo(rect.right-nOffset+1, rect.top);
	
	// bottom border
	pDC->MoveTo(rect.left+nOffset, rect.bottom);
	pDC->LineTo(rect.right-nOffset+1, rect.bottom);
	
	// left border
	pDC->MoveTo(rect.left, rect.top+nOffset);
	pDC->LineTo(rect.left, rect.bottom-nOffset+1);
	
	// right border
	pDC->MoveTo(rect.right, rect.top+nOffset);
	pDC->LineTo(rect.right, rect.bottom-nOffset+1);
	
	pDC->SelectObject(pOldPen);
}

static void DrawButtonBorder( CDC* pDC, CRect rect, CDSTATEID iStateId, 
							 bool bDrawShadow = true, 
							 int nOuterBorderOffset = 3, 
							 int nInnerBorderOffset = 1
							 )
{
	COLORREF clrOuterBorder, clrInnerBorder, clrShadow;
	switch (iStateId)
	{
	case CDSTATEID_PRESSED:
		clrOuterBorder	= RGB(44, 98, 139);
		clrInnerBorder	= RGB(158, 176, 186);
		clrShadow		= RGB(46, 96, 134);
		break;
	case CDSTATEID_HOT:
		clrOuterBorder	= RGB(60, 127, 177);
		clrInnerBorder	= RGB(250, 253, 254);
		clrShadow		= RGB(66, 131, 179);
		break;
	case CDSTATEID_DISABLED:
		clrOuterBorder	= ::GetSysColor(COLOR_BTNSHADOW);
		clrInnerBorder	= RGB(250, 253, 254);
		clrShadow		= clrOuterBorder;
		break;
	default:
		clrOuterBorder	= RGB(112, 112, 112);
		clrInnerBorder	= RGB(252, 252, 252);
		clrShadow		= RGB(144, 144, 144);
		break;
	}
	
	DrawBorderLines(pDC, rect, clrOuterBorder, nOuterBorderOffset);
	
	if ( bDrawShadow )
	{
		// Top-left
		pDC->SetPixel(rect.left+1, rect.top, clrShadow);
		pDC->SetPixel(rect.left+2, rect.top, clrShadow);
		pDC->SetPixel(rect.left, rect.top+1, clrShadow);
		pDC->SetPixel(rect.left+1, rect.top+1, clrShadow);
		pDC->SetPixel(rect.left, rect.top+2, clrShadow);
		
		// Top-right
		pDC->SetPixel(rect.right-2, rect.top, clrShadow);
		pDC->SetPixel(rect.right-1, rect.top, clrShadow);
		pDC->SetPixel(rect.right-1, rect.top+1, clrShadow);
		pDC->SetPixel(rect.right, rect.top+1, clrShadow);
		pDC->SetPixel(rect.right, rect.top+2, clrShadow);
		
		// Bottom-left
		pDC->SetPixel(rect.left, rect.bottom-2, clrShadow);
		pDC->SetPixel(rect.left, rect.bottom-1, clrShadow);
		pDC->SetPixel(rect.left+1, rect.bottom-1, clrShadow);
		pDC->SetPixel(rect.left+1, rect.bottom, clrShadow);
		pDC->SetPixel(rect.left+2, rect.bottom, clrShadow);
		
		// Bottom-right
		pDC->SetPixel(rect.right, rect.bottom-2, clrShadow);
		pDC->SetPixel(rect.right-1, rect.bottom-1, clrShadow);
		pDC->SetPixel(rect.right, rect.bottom-1, clrShadow);
		pDC->SetPixel(rect.right-2, rect.bottom, clrShadow);
		pDC->SetPixel(rect.right-1, rect.bottom, clrShadow);
	}
	
	rect.DeflateRect(1, 1);
	DrawBorderLines(pDC, rect, clrInnerBorder, nInnerBorderOffset);
}
#endif // CUSTOMDRAW_THEME_BACKGROUND

void DrawDropDownArrow( HDC hDC, const RECT* pRect )
{
	CDC* pDC = CDC::FromHandle(hDC);
	ASSERT(pRect);
	CRect rcFill(*pRect);

	// Draw the down arrow
	CPen penLine(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&penLine);
	
	CPoint ptTopCenter( rcFill.CenterPoint().x, (rcFill.top + rcFill.bottom-1) / 2 - 1);
	for (int ii = 0, yy = ptTopCenter.y; ii < 4; ++ii, ++yy)
	{
		int cx = 7 - ii * 2;
		int xx = ptTopCenter.x - cx / 2;
		pDC->MoveTo(xx, yy);
		pDC->LineTo(xx+cx, yy);
	}
	pDC->SelectObject(pOldPen);
}

HRESULT CustomDrawThemeBackground( HWND hwnd, HDC hDC, CDTBT type, CDSTATEID state, const RECT* pRect, const RECT* pClipRect /*= NULL*/ )
{
	HRESULT hresult = S_OK;
#ifdef CUSTOMDRAW_THEME_BACKGROUND
	CDC* pDC = CDC::FromHandle(hDC);
	ASSERT(pRect);
	CRect rcFill(*pRect);

	const Item3DBackgrnd* pItem	= NULL;
	switch (type)
	{
	case CDTBT_PUSHBUTTON:
	case CDTBT_COMBOBUTTON:
		pItem = &s_itemButton;
		rcFill.DeflateRect(2, 2);
		break;
	case CDTBT_COMBODOWNBUTTON:
		pItem = &s_itemComboBtn;
		rcFill.DeflateRect(2, 2);
		break;
	case CDTBT_HEADER:
		pItem = &s_itemHeader;
		break;
	default:
		ASSERT(0);
		return hresult;
		break;
	}

	ASSERT(pItem);
	PCCOLORS4 pFillColors = pItem->clrs[state];
	if (pFillColors)
		Fill4ColorsGradient(*pDC, rcFill, pFillColors, TRUE, pItem->nPercentage);

	bool bDrawShadow = true;
	int nOuterBorderOffset = 3;
	int nInnerBorderOffset = 1;
	bool bDrawBtnBorder = false;
	bool bDisableFill = false;
	if ( CDSTATEID_DISABLED == state && hwnd )
	{
		if ( CDTBT_PUSHBUTTON == type || CDTBT_COMBOBUTTON == type )
		{
			bDisableFill = true;
		}
	}

	switch (type)
	{
	case CDTBT_PUSHBUTTON:
	case CDTBT_COMBOBUTTON:
		{
			bDrawBtnBorder = true;
		}
		break;
	case CDTBT_COMBODOWNBUTTON:
		{
			if (pFillColors)
			{
				bDrawBtnBorder		= true;
				bDrawShadow			= false;
				nOuterBorderOffset	= nInnerBorderOffset = 0;
			}

			DrawDropDownArrow(hDC, rcFill);
		}
		break;
	case CDTBT_HEADER:
		break;
	}

	if ( bDisableFill )
	{
		pDC->FillSolidRect(rcFill, ::GetSysColor(COLOR_3DFACE));
	}

	if ( bDrawBtnBorder )
	{
		CRect rect(*pRect);
		--rect.right;
		--rect.bottom;
		DrawButtonBorder(pDC, rect, state, bDrawShadow, nOuterBorderOffset, nInnerBorderOffset);
	}
#else
	ASSERT( IsWindow(hwnd) );
	LPCWSTR pszClassList = NULL;
	int iPartId = 0, iStateId = 0;
	switch (type)
	{
	case CDTBT_PUSHBUTTON:
	case CDTBT_COMBOBUTTON:
		pszClassList	= VSCLASS_BUTTON;
		iPartId			= BP_PUSHBUTTON;
		switch (state)
		{
		case CDSTATEID_DISABLED:
			iStateId = PBS_DISABLED;
			break;
		case CDSTATEID_HOT:
			iStateId = PBS_HOT;
			break;
		case CDSTATEID_PRESSED:
			iStateId = PBS_PRESSED;
			break;
		case CDSTATEID_NORMAL:
		default:
			iStateId = PBS_NORMAL;
			break;
		}
		break;
	case CDTBT_COMBODOWNBUTTON:
		pszClassList	= VSCLASS_COMBOBOX;
		iPartId			= CP_DROPDOWNBUTTON;
		switch (state)
		{
		case CDSTATEID_DISABLED:
			iStateId = CBXS_DISABLED;
			break;
		case CDSTATEID_HOT:
			iStateId = CBXS_HOT;
			break;
		case CDSTATEID_PRESSED:
			iStateId = CBXS_PRESSED;
			break;
		case CDSTATEID_NORMAL:
		default:
			iStateId = CBXS_NORMAL;
			break;
		}
		break;
	case CDTBT_HEADER:
		pszClassList	= VSCLASS_HEADER;
		iPartId			= HP_HEADERITEM;
		switch (state)
		{
		case CDSTATEID_DISABLED:
			iStateId = HIS_NORMAL;	// disabled state is not applicable for a header control
			break;
		case CDSTATEID_HOT:
			iStateId = HIS_HOT;
			break;
		case CDSTATEID_PRESSED:
			iStateId = HIS_PRESSED;
			break;
		case CDSTATEID_NORMAL:
		default:
			iStateId = HIS_NORMAL;
			break;
		}
		break;
	default:
		ASSERT(0);
		return hresult;
		break;
	}
	HTHEME hTheme = OOpenThemeData(hwnd, pszClassList);
	if ( hTheme )
	{
		if ( OIsThemeBackgroundPartiallyTransparent(hTheme, iPartId, iStateId) )
		{
			ODrawThemeParentBackground(hwnd, hDC, pRect);
		}
		ODrawThemeBackground(hTheme, hDC, iPartId, iStateId, pRect, pClipRect);
		OCloseThemeData(hTheme);
	}
#endif // CUSTOMDRAW_THEME_BACKGROUND
	return hresult;
}
#endif // CUSTOMDRAW_GRADIENT

void DrawSortArrow( CDC* pDC, CRect rectArrow, BOOL bIsUp )
{
	COLORREF clrLight	= RGB(154,199,224);//GetSysColor(COLOR_3DHILIGHT);
	COLORREF clrDark	= RGB(67,95,111);//GetSysColor(COLOR_3DSHADOW);
	CPen penLight(PS_SOLID, 1, clrLight);
	CPen penDark(PS_SOLID, 1, clrDark);
	
	CPen* pPenOld = pDC->SelectObject(&penLight);
	ASSERT_VALID(pPenOld);
	
	if (!bIsUp)
	{
		pDC->MoveTo(rectArrow.right, rectArrow.top);
		pDC->LineTo(rectArrow.CenterPoint().x, rectArrow.bottom);
		
		pDC->SelectObject(&penDark);
		pDC->LineTo(rectArrow.left, rectArrow.top);
		pDC->LineTo(rectArrow.right, rectArrow.top);
	}
	else
	{
		pDC->MoveTo(rectArrow.left, rectArrow.bottom);
		pDC->LineTo(rectArrow.right, rectArrow.bottom);
		pDC->LineTo(rectArrow.CenterPoint().x, rectArrow.top);
		
		pDC->SelectObject(&penDark);
		pDC->LineTo(rectArrow.left, rectArrow.bottom);
	}
	
	pDC->SelectObject(pPenOld);
}

// http://www.codeproject.com/KB/list/CGridListCtrlEx.aspx

bool CheckOSVersion(WORD requestOS)
{
	static WORD fullver = 0;
	if (fullver==0)
	{
		OSVERSIONINFO osver = {0};
		osver.dwOSVersionInfoSize = sizeof(osver);
		GetVersionEx(&osver);
		fullver = MAKEWORD(osver.dwMinorVersion, osver.dwMajorVersion);
	}
	return requestOS <= fullver;
}

bool IsCommonControlsEnabled()
{
	bool commoncontrols = false;
	
	// Test if application has access to common controls
	HMODULE hinstDll = ::LoadLibrary(_T("comctl32.dll"));
	if (hinstDll)
	{
		DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)::GetProcAddress(hinstDll, "DllGetVersion");
		if (pDllGetVersion != NULL)
		{
			DLLVERSIONINFO dvi = {0};
			dvi.cbSize = sizeof(dvi);
			HRESULT hRes = pDllGetVersion ((DLLVERSIONINFO *) &dvi);
			if (SUCCEEDED(hRes))
				commoncontrols = dvi.dwMajorVersion >= 6;
		}
		::FreeLibrary(hinstDll);
	}
	return commoncontrols;
}

bool IsThemeEnabled()
{
	bool XPStyle = false;
	bool (__stdcall *pIsAppThemed)();
	bool (__stdcall *pIsThemeActive)();
	
	// Test if operating system has themes enabled
	HMODULE hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
	if (hinstDll)
	{
		(FARPROC&)pIsAppThemed = ::GetProcAddress(hinstDll, "IsAppThemed");
		(FARPROC&)pIsThemeActive = ::GetProcAddress(hinstDll,"IsThemeActive");
		if (pIsAppThemed != NULL && pIsThemeActive != NULL)
		{
			if (pIsAppThemed() && pIsThemeActive())
			{
				// Test if application has themes enabled by loading the proper DLL
				XPStyle = IsCommonControlsEnabled();
			}
		}
		::FreeLibrary(hinstDll);
	}
	return XPStyle;
}

#ifndef LINK_TO_UXTHEME
HTHEME OOpenThemeData( HWND hwnd, LPCWSTR pszClassList )
{
	HTHEME hTheme = NULL;
	HMODULE hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
	if (hinstDll)
	{
		HTHEME (__stdcall *pfn)(HWND hwnd, LPCWSTR pszClassList);
		(FARPROC&)pfn = ::GetProcAddress(hinstDll, _T("OpenThemeData"));
		if ( pfn )
		{
			hTheme = pfn(hwnd, pszClassList);
		}
		::FreeLibrary(hinstDll);
	}
	return hTheme;
}

HRESULT OCloseThemeData( HTHEME hTheme )
{
	HRESULT hResult = S_FALSE;
	HMODULE hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
	if (hinstDll)
	{
		HRESULT (__stdcall *pfn)(HTHEME hTheme);
		(FARPROC&)pfn = ::GetProcAddress(hinstDll, _T("CloseThemeData"));
		if ( pfn )
		{
			hResult = pfn(hTheme);
		}
		::FreeLibrary(hinstDll);
	}
	return hResult;
}

HRESULT OSetWindowTheme( HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList )
{
	HRESULT hResult = S_FALSE;
	HMODULE hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
	if (hinstDll)
	{
		HRESULT (__stdcall *pfn)(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
		(FARPROC&)pfn = ::GetProcAddress(hinstDll, _T("SetWindowTheme"));
		if ( pfn )
		{
			hResult = pfn(hwnd, pszSubAppName, pszSubIdList);
		}
		::FreeLibrary(hinstDll);
	}
	return hResult;
}

HRESULT ODrawThemeBackground( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect )
{
	HRESULT hResult = S_FALSE;
	HMODULE hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
	if (hinstDll)
	{
		HRESULT (__stdcall *pfn)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect);
		(FARPROC&)pfn = ::GetProcAddress(hinstDll, _T("DrawThemeBackground"));
		if ( pfn )
		{
			hResult = pfn(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
		}
		::FreeLibrary(hinstDll);
	}
	return hResult;
}

BOOL OIsThemeBackgroundPartiallyTransparent( HTHEME hTheme, int iPartId, int iStateId )
{
	BOOL bRet = FALSE;
	HMODULE hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
	if (hinstDll)
	{
		HRESULT (__stdcall *pfn)(HTHEME hTheme, int iPartId, int iStateId);
		(FARPROC&)pfn = ::GetProcAddress(hinstDll, _T("IsThemeBackgroundPartiallyTransparent"));
		if ( pfn )
		{
			bRet = pfn(hTheme, iPartId, iStateId);
		}
		::FreeLibrary(hinstDll);
	}
	return bRet;
}

HRESULT ODrawThemeParentBackground( HWND hwnd, HDC hdc, const RECT* prc )
{
	HRESULT hResult = S_FALSE;
	HMODULE hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
	if (hinstDll)
	{
		HRESULT (__stdcall *pfn)(HWND hwnd, HDC hdc, const RECT* prc);
		(FARPROC&)pfn = ::GetProcAddress(hinstDll, _T("DrawThemeParentBackground"));
		if ( pfn )
		{
			hResult = pfn(hwnd, hdc, prc);
		}
		::FreeLibrary(hinstDll);
	}
	return hResult;
}
#endif // LINK_TO_UXTHEME

LRESULT EnableWindowTheme(HWND hwnd, LPCWSTR classList, LPCWSTR subApp, LPCWSTR idlist)
{
	LRESULT lResult = S_FALSE;
	
	HRESULT (__stdcall *pSetWindowTheme)(HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList);
	HANDLE (__stdcall *pOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
	HRESULT (__stdcall *pCloseThemeData)(HANDLE hTheme);
	
	HMODULE hinstDll = ::LoadLibrary(_T("UxTheme.dll"));
	if (hinstDll)
	{
		(FARPROC&)pOpenThemeData = ::GetProcAddress(hinstDll, _T("OpenThemeData"));
		(FARPROC&)pCloseThemeData = ::GetProcAddress(hinstDll, _T("CloseThemeData"));
		(FARPROC&)pSetWindowTheme = ::GetProcAddress(hinstDll, _T("SetWindowTheme"));
		if (pSetWindowTheme && pOpenThemeData && pCloseThemeData)
		{
			HANDLE theme = pOpenThemeData(hwnd,classList);
			if (theme != NULL)
			{
				VERIFY(pCloseThemeData(theme)==S_OK);
				lResult = pSetWindowTheme(hwnd, subApp, idlist);
			}
		}
		::FreeLibrary(hinstDll);
	}
	return lResult;
}

SIZE GetBmpSize( HBITMAP hBmp )
{
	BITMAPINFO bmp = {0};
	GetObject(hBmp, sizeof(BITMAPINFO), &bmp);
	return CSize(bmp.bmiHeader.biWidth, bmp.bmiHeader.biHeight);
}


SIZE GetIconSize( HICON hIcon )
{
	ICONINFO iconInfo = {0};
	GetIconInfo(hIcon, &iconInfo);
	BITMAPINFO bmp = {0};
	HBITMAP hBitmap = iconInfo.hbmColor ? iconInfo.hbmColor : iconInfo.hbmMask;
	SIZE size = GetBmpSize(hBitmap);
	if ( iconInfo.hbmColor != NULL )
		DeleteObject(iconInfo.hbmColor);
	if ( iconInfo.hbmMask != NULL )
		DeleteObject(iconInfo.hbmMask);
	return size;
}

void DrawAlphaBlendedSelection( CDC& dc, CRect rcPaint, CRect rcSelBox, COLORREF clrSelFill, COLORREF clrSelBorder )
{
	CRect rcSelUpdate;
	rcPaint.NormalizeRect();
	rcSelBox.NormalizeRect();
	if ( rcSelUpdate.IntersectRect(rcSelBox, rcPaint) && !rcSelUpdate.IsRectEmpty() )
	{
		CMemBitmap bmpFill(&dc);
		if ( bmpFill.BeginDraw(rcSelUpdate.Width(), rcSelUpdate.Height()) )
		{
			CDC& dcSel = bmpFill.GetDC();
			CRect rcFill;
			rcFill.left		= 0;
			rcFill.top		= 0;
			rcFill.right	= rcSelUpdate.Width();
			rcFill.bottom	= rcSelUpdate.Height();
			
			dcSel.FillSolidRect(rcFill, clrSelFill);
			
			BLENDFUNCTION bf;
			bf.BlendFlags			= 0;
			bf.AlphaFormat			= 0;
			bf.BlendOp				= AC_SRC_OVER;
			bf.SourceConstantAlpha	= ALPHA_SELECTION_FILL;
			::AlphaBlend(dc, rcSelUpdate.left, rcSelUpdate.top, rcSelUpdate.Width(), rcSelUpdate.Height(), 
				dcSel, 0, 0, rcFill.right, rcFill.bottom, bf);
			
			CBrush brush(clrSelBorder);
			dc.FrameRect(rcSelBox, &brush);
		}
	}
}


/*----------------------------------------------------------------------------*/
/* CCustomDrawHelper
/*----------------------------------------------------------------------------*/
CCustomDrawHelper::CCustomDrawHelper()
	: m_bWndHasFocus(false)
	, m_bFocusItem(false)
	, m_bSelected(false)
	, m_bIsHotItem(false)
	, m_bDrawBorderWhenFill(true)
	, m_bIsPressed(false)
	, m_bWndEnabled(true)
	, m_rcBounds(0,0,0,0)
	, m_hWnd(NULL)
{

}

CCustomDrawHelper::~CCustomDrawHelper()
{
	
}

/*
	Different cases with different colors:
					wnd has focus							 				  wnd has no focus
           +--------------------------------+                          +-------------------------+
		selected					 	unselected					selected				 	unselected
     +---------------+               +------------+              +-------------+            +---------+
	hot		      not hot			hot		   not hot			hot		 	not hot		   hot	   not hot
     1           +--------+          3            x              1             4            3         x
	 		  focus   no focus
				1		2
*/

// TODO: The colors can be generated by some kind of algorithm so that the the caller can control the main color.
// e.g. With given color (like sky-blue), all the other colors can be generated by reducing/increasing its luminosity,
// so RGB2HSL and HSL2RGB will be needed in that case. However, the difficult part is how to design the rule, which I
// do not have time by now, so I just use hard coded value here.
void CCustomDrawHelper::DrawItemBackground( CDC* pDC, CRect rect, BYTE byAlpha /*= 255*/ )
{
	if ( ShouldFillBackground() )
	{
#ifdef CUSTOMDRAW_GRADIENT
		COLORREF clrFillColor1;
		COLORREF clrFillColor2;
		if ( m_bSelected )
		{
			if ( m_bIsHotItem || (m_bFocusItem && m_bWndHasFocus) )
			{
				clrFillColor1	= RGB_ITEM_SELFOCUS_FILL_COLOR1;
				clrFillColor2	= RGB_ITEM_SELFOCUS_FILL_COLOR2;
			}
			else if ( m_bWndHasFocus )
			{
				ASSERT( !m_bFocusItem );
				clrFillColor1	= RGB_ITEM_SELECTED_FILL_COLOR1;
				clrFillColor2	= RGB_ITEM_SELECTED_FILL_COLOR2;
			}
			else
			{
				clrFillColor1	= RGB_ITEM_SELNOFOCUS_FILL_COLOR1;
				clrFillColor2	= RGB_ITEM_SELNOFOCUS_FILL_COLOR2;
			}
		}
		else
		{
			// not selected, hot item
			clrFillColor1 = RGB_ITEM_HOT_FILL_COLOR1;
			clrFillColor2 = RGB_ITEM_HOT_FILL_COLOR2;
		}
		if ( m_bDrawBorderWhenFill )
		{
			COLORREF clrInnerBorder;
			COLORREF clrOuterBorder;
			if ( m_bSelected )
			{
				clrInnerBorder = (m_bWndHasFocus || m_bIsHotItem) ? RGB_ITEM_SELFOCUS_INNER_BORDER_COLOR : RGB_ITEM_SELNOFOCUS_INNER_BORDER_COLOR;
				clrOuterBorder = (m_bWndHasFocus || m_bIsHotItem) ? RGB_ITEM_SELFOCUS_OUTER_BORDER_COLOR : RGB_ITEM_SELNOFOCUS_OUTER_BORDER_COLOR;
			}
			else
			{
				clrInnerBorder = RGB_ITEM_HOT_INNER_BORDER_COLOR;
				clrOuterBorder = RGB_ITEM_HOT_OUTER_BORDER_COLOR;
			}
			DrawRoundRectItem(pDC, rect, clrFillColor1, clrFillColor2, clrInnerBorder, clrOuterBorder, byAlpha);
		}
		else
		{
			rect.DeflateRect(2, 2);
			FillGradient(pDC->GetSafeHdc(), rect, clrFillColor1, clrFillColor2, GFT_TOP_BOTTOM, byAlpha);
		}
#else
		pDC->FillSolidRect(rect, ::GetSysColor( m_bWndHasFocus ? COLOR_HIGHLIGHT : COLOR_BTNFACE));
#endif // CUSTOMDRAW_GRADIENT
		//TRACE("====> hilight %d %d [%d, %d]\n", nItem, nSubItem, rect.left, rect.right);
	}
	else if ( m_bFocusItem )
	{
		CRect& rcItem = m_rcBounds.IsRectEmpty() ? rect : m_rcBounds;
		DrawFocus(pDC, rcItem, byAlpha);
	}
}

void CCustomDrawHelper::DrawFocus( CDC* pDC, CRect rect, BYTE byAlpha /*= 255*/ )
{
	ASSERT( !ShouldFillBackground() && m_bFocusItem );
#ifdef CUSTOMDRAW_GRADIENT
	DrawSimpleRoundRectBorder(pDC, rect, RGB_ITEM_SELFOCUS_OUTER_BORDER_COLOR);
#else
	pDC->DrawFocusRect(rect);
#endif // CUSTOMDRAW_GRADIENT	
}

#ifdef CUSTOMDRAW_GRADIENT
void CCustomDrawHelper::DrawThemeBackground( CDC* pDC, CRect rect, CDTBT type )
{
	CDSTATEID state;
	if ( !m_bWndEnabled )
		state = CDSTATEID_DISABLED;
	else if ( m_bIsPressed )
		state = CDSTATEID_PRESSED;
	else if ( m_bIsHotItem )
		state = CDSTATEID_HOT;
	else
		state = CDSTATEID_NORMAL;
	CustomDrawThemeBackground(m_hWnd, pDC->GetSafeHdc(), type, state, rect);
}
#endif // CUSTOMDRAW_GRADIENT

/*----------------------------------------------------------------------------*/
/* CMemBitmap
/*----------------------------------------------------------------------------*/
CMemBitmap::CMemBitmap(CDC* pDC /*= NULL*/, CBitmap* pBmp /*= NULL*/)
	: m_pBmpOld(NULL)
	, m_pDC(pDC)
	, m_sizeBmp(0, 0)
	, m_pBmp(pBmp)
{
	
}

CMemBitmap::~CMemBitmap()
{
	if ( m_pBmpOld )
	{
		EndDraw();
	}
}

void CMemBitmap::SetBitmap( CBitmap* pBmp )
{
	ASSERT(pBmp);
	ASSERT(NULL == m_pBmpOld);	// you've called BeginDraw() ?
	m_pBmp = pBmp;
}

void CMemBitmap::SetCompatibleDC( CDC* pDC )
{
	ASSERT(pDC);
	ASSERT(NULL == m_pBmpOld);	// you've called BeginDraw() ?
	m_pDC = pDC;
}

BOOL CMemBitmap::BeginDraw( int cx, int cy, BOOL bAutoCreate /*= TRUE*/ )
{
	ASSERT(NULL == m_dcMemory.GetSafeHdc());
	cx = abs(cx);
	cy = abs(cy);
	CWindowDC	dctemp(NULL);
	CDC* pDC = m_pDC ? m_pDC : &dctemp;
	
	if( !m_dcMemory.GetSafeHdc() && !m_dcMemory.CreateCompatibleDC(pDC) )
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CBitmap* pBmp = m_pBmp ? m_pBmp : &m_bmp;
	ASSERT( pBmp );	// can't fail!
	if ( bAutoCreate )
	{
		if ( pBmp->GetSafeHandle() )
		{
			pBmp->DeleteObject();
		}
		if( !pBmp->CreateCompatibleBitmap(pDC, cx, cy) )
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}
	else
	{
		ASSERT( pBmp->GetSafeHandle() );
	}
	
	m_sizeBmp.cx = cx;
	m_sizeBmp.cy = cy;

	m_pBmpOld = (CBitmap*)m_dcMemory.SelectObject( GetBitmapPtr() );
	return TRUE;
}

void CMemBitmap::EndDraw()
{
#ifdef _DEBUG
	static bool s_bTestDraw = false;
	bool& bTestDraw = s_bTestDraw;
	if (bTestDraw)
	{
		::BitBlt( ::GetDC(NULL), 10, 10, m_sizeBmp.cx, m_sizeBmp.cy, m_dcMemory.GetSafeHdc(), 0, 0, SRCCOPY);
		bTestDraw = false;
	}
#endif // _DEBUG

	// Finish the bitmap
	m_dcMemory.SelectObject(m_pBmpOld);
	m_pBmpOld = NULL;
}

CMemBitmap& CMemBitmap::operator=( CMemBitmap& src )
{
	ASSERT( GetBitmapSize() == src.GetBitmapSize() );
	GetDC().BitBlt(0, 0, GetBitmapWidth(), GetBitmapHeight(), &src.GetDC(), 0, 0, SRCCOPY);
	return *this;
}

/*----------------------------------------------------------------------------*/
/* CSimpleMemDC
/*----------------------------------------------------------------------------*/
CSimpleMemDC::CSimpleMemDC( CDC& dc, CWnd* pWnd, OUTPUTTYPE type /*= OUTPUTTYPE_BITBLT*/ )
	: m_dc(dc)
	, m_memBitmap(&dc)
	, m_pWnd(pWnd)
	, m_ptOldWndOrg(0, 0)
	, m_outputType(type)
	, m_dwRop(SRCCOPY)
{
	ASSERT_VALID(m_pWnd);
	
	m_pWnd->GetClientRect(m_rect);
	
	m_rect.right += m_pWnd->GetScrollPos(SB_HORZ);
	m_rect.bottom += m_pWnd->GetScrollPos(SB_VERT);

	if ( m_memBitmap.BeginDraw(m_rect.Width(), m_rect.Height()) )
	{
		// additional stuff.
	}
}

CSimpleMemDC::CSimpleMemDC( CDC& dc, const CRect& rect, OUTPUTTYPE type /*= OUTPUTTYPE_BITBLT*/ )
	: m_dc(dc)
	, m_rect(rect)
	, m_memBitmap(&dc)
	, m_pWnd(NULL)
	, m_ptOldWndOrg(0, 0)
	, m_outputType(type)
	, m_dwRop(SRCCOPY)
{
	//ASSERT(!m_rect.IsRectEmpty());
	if ( m_memBitmap.BeginDraw(m_rect.Width(), m_rect.Height()) )
	{
		// make the drawing area starts from the top-left of the bitmap
		m_ptOldWndOrg = GetDC().SetWindowOrg(m_rect.left, m_rect.top);
	}
}

CSimpleMemDC::~CSimpleMemDC()
{
	// Copy the results to the on-screen DC
	CRect rectClip;
	int nClipType = m_dc.GetClipBox(rectClip);
	
	if ( m_pWnd )
	{
		if (nClipType != NULLREGION)
		{
			if (nClipType != SIMPLEREGION)
			{
				rectClip = m_rect;
			}
			
			//m_dc.BitBlt(rectClip.left, rectClip.top, rectClip.Width(), rectClip.Height(), &m_memBitmap.GetDC(), rectClip.left, rectClip.top, m_dwRop);
			PaintResult(rectClip, rectClip.left, rectClip.top);
		}
		else
		{
			// nothing is drawn
		}
	}
	else
	{
		//ASSERT( !m_rect.IsRectEmpty() && !m_rect.IsRectNull() );

		// restore the window origin
		GetDC().SetWindowOrg(m_ptOldWndOrg);

		CRect rectDraw;
		if ( rectDraw.IntersectRect(rectClip, m_rect) )
		{
			int nBmpX = rectDraw.left - m_rect.left;
			int nBmpY = rectDraw.top - m_rect.top;
			ASSERT(nBmpX >= 0 && nBmpY >= 0);
			//m_dc.BitBlt(rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(), &m_memBitmap.GetDC(), nBmpX, nBmpY, m_dwRop);
			PaintResult(rectDraw, nBmpX, nBmpY);
		}
	}
}

CSimpleMemDC& CSimpleMemDC::operator=( CSimpleMemDC& src )
{
	m_memBitmap = src.m_memBitmap;
	return *this;
}

CSimpleMemDC& CSimpleMemDC::operator=( CMemBitmap& src )
{
	m_memBitmap = src;
	return *this;
}

