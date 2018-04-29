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

#ifndef CUSTOMDRAWCOMMON_H
#define CUSTOMDRAWCOMMON_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER >= 1600		// MS VC++ 10.0 (2010) or later
	#define MSVC_NEW_VER
#endif

enum
{
	// Type							// Gradient filling direction
	GFT_INVALID					= -1,
	GFT_FIRST					= 0,
	
	// Single direction
	GFT_TOP_BOTTOM = GFT_FIRST,		// Vertical, top-to-bottom direction
	GFT_BOTTOM_TOP,					// Vertical, bottom-to-top direction
	GFT_LEFT_RIGHT,					// Horizontal, left-to-right direction
	GFT_RIGHT_LEFT,					// Horizontal, right-to-left direction
	GFT_DIAG_TL_BR,					// Diagonal, from top-left to bottom-right
	GFT_DIAG_TR_BL,					// Diagonal, from top-right to bottom-left
	
	// Center out
	GFT_VERTICAL_CENTER_OUT,		// Vertical, from center to the two sides (top & bottom)
	GFT_VERTICAL_CENTER_IN,			// Vertical, from two sides (top & bottom) to the center
	GFT_HORIZONTAL_CENTER_OUT,		// Horizontal, from center to the two sides (left & right)
	GFT_HORIZONTAL_CENTER_IN,		// Horizontal, from two sides (left & right) to the center
	GFT_DIAG_UP_CENTER_OUT,			// Diagonal, from center to the two corners
	GFT_DIAG_DOWN_CENTER_OUT,		// Diagonal, from center to the two corners
	
	// From corner
	GFT_FROM_CORNER_TL,				// From top-left corner to the other 3 corners
	GFT_FROM_CORNER_TR,				// From top-right corner to the other 3 corners
	GFT_FROM_CORNER_BR,				// From bottom-right corner to the other 3 corners
	GFT_FROM_CORNER_BL,				// From bottom-right corner to the other 3 corners
	
	GFT_FROM_CENTER,				// From the center to the 4 corners outside.
	
	GFT_SIZE,
};

#define	IS_VALID_GRADIENT_TYPE(_nType)		( GFT_FIRST <= (_nType) && (_nType) < GFT_SIZE)

#define GFT_SINGLE_DIRECTION_FIRST			GFT_TOP_BOTTOM
#define GFT_SINGLE_DIRECTION_LAST			GFT_DIAG_TR_BL

#define GFT_CENTER_OUT_IN_FIRST				GFT_VERTICAL_CENTER_OUT
#define GFT_CENTER_OUT_IN_LAST				GFT_DIAG_DOWN_CENTER_OUT

#define GFT_FROM_CORNER_FIRST				GFT_FROM_CORNER_TL
#define GFT_FROM_CORNER_LAST				GFT_FROM_CORNER_BL

#define IS_GFT_BETWEEN(_nType, _t1, _t2)	( (_t1) <= (_nType) && (_nType) <= (_t2) )

#define IS_GFT_SINGLE_DIRECTION(_nType)		IS_GFT_BETWEEN((_nType), GFT_SINGLE_DIRECTION_FIRST, GFT_SINGLE_DIRECTION_LAST)
#define IS_GFT_CENTER_OUT_IN(_nType)		IS_GFT_BETWEEN((_nType), GFT_CENTER_OUT_IN_FIRST, GFT_CENTER_OUT_IN_LAST)
#define IS_GFT_FROM_CORNER(_nType)			IS_GFT_BETWEEN((_nType), GFT_FROM_CORNER_FIRST, GFT_FROM_CORNER_LAST)

#define IS_GFT_CENTER_OUT(_nType)			( GFT_VERTICAL_CENTER_OUT == (_nType) \
											|| GFT_HORIZONTAL_CENTER_OUT == (_nType) \
											|| GFT_DIAG_UP_CENTER_OUT == (_nType) \
											|| GFT_DIAG_DOWN_CENTER_OUT == (_nType) )

#define IS_GFT_LINEAR_DIRECTION(_nType)		IS_GFT_BETWEEN((_nType), GFT_SINGLE_DIRECTION_FIRST, GFT_CENTER_OUT_IN_LAST)

#define IS_GFT_FROM_CENTER(_nType)			( GFT_FROM_CENTER == (_nType) )

#define GET_GFT_CORNER_INDEX(_nType)		( (_nType) - GFT_FROM_CORNER_FIRST )

#include <string>

#ifdef _UNICODE
	typedef std::wstring	tstring;
#else
	typedef std::string		tstring;
#endif

#define TSTRINLINE

namespace str_utils
{
	TSTRINLINE tstring&		tolower(tstring& str);
	TSTRINLINE tstring		tolower(const tstring& str);
};

TSTRINLINE std::wstring	towstring(const tstring& str);
TSTRINLINE std::string	toastring(const tstring& str);
TSTRINLINE tstring		totstring(const std::wstring& str);

// Converts a wide character string to multi-byte character string
std::string WideToMultiByte(const std::wstring& szWide);

// Converts a multi-byte character string to wide character string
std::wstring MultiByteToWide(const std::string& szNarrow);


#define OGetComboBoxInfo			GetComboBoxInfo

#define COMBOBOX_DROPPED_ALL_ITEMS_VISIBLE		-1
void	SetComboboxDroppedVisibleItemCount(HWND hCombobox, int nItemCount = COMBOBOX_DROPPED_ALL_ITEMS_VISIBLE, BOOL bRepaint = TRUE);

// see http://blogs.msdn.com/b/oldnewthing/archive/2004/08/02/205624.aspx
void SetDialogFocus(HWND hdlg, HWND hwndControl);

HICON GetAssociatedFileIcon( LPCTSTR lpctszFilePath );

int GetStringResourceArray(UINT nID, CStringArray& arrArgs, TCHAR chSeparator = _T(' '));

UINT FileReadLines(LPCTSTR lpcszFile, CString& strText, UINT nLineIndex, UINT nNumber = 1);


#define		O_LOAD_CSTRING_OBJ(_str, _nResID)		_str.LoadString(_nResID)

// Comment out this line to use GDI for gradient drawing.
//#define __ENABLE_GDIPLUS__

#define CUSTOMDRAW_GRADIENT


#ifdef __ENABLE_GDIPLUS__

typedef unsigned long ULONG_PTR, *PULONG_PTR;

#pragma warning(disable: 4786)

#include "gdiplus.h"
#pragma comment(lib, "GdiPlus.lib")

#endif // __ENABLE_GDIPLUS__


#else
#error ### repeated include of CustomDrawCommon.h!!!
#endif // CUSTOMDRAWCOMMON_H
