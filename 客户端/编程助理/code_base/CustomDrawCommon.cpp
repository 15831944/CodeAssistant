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

#include "stdafx.h"
#include "CustomDrawCommon.h"
#include <algorithm>	// for transform

namespace str_utils
{
	tstring& tolower(tstring& str)
	{
	#ifdef _UNICODE
		std::transform(str.begin(), str.end(), str.begin(), (int (*)(int))::towlower);	
	#else
		std::transform(str.begin(), str.end(), str.begin(), (int (*)(int))::tolower);	
	#endif // _UNICODE
		return str;
	}

	tstring tolower( const tstring& str )
	{
		tstring strCpy(str);
		return tolower(strCpy);
	}
}

std::wstring towstring(const tstring& str)
{
#ifdef _UNICODE
	return str;	
#else
	return MultiByteToWide(str);
#endif // _UNICODE
}

std::string toastring( const tstring& str )
{
#ifdef _UNICODE
	return WideToMultiByte(str);
#else
	return str;
#endif // _UNICODE	
}

tstring totstring(const std::wstring& str)
{
#ifdef _UNICODE
	return str;	
#else
	return WideToMultiByte(str);
#endif // _UNICODE
}

std::wstring MultiByteToWide(const std::string& szNarrow)
{
	int size = MultiByteToWideChar(
		CP_ACP,				// code page
		0,					// character-type options
		szNarrow.c_str(),	// string to map
		szNarrow.size(),	// number of bytes in string
		NULL,				// wide-character buffer
		0					// size of buffer
		);
	
	size++;
	
	WCHAR* pBuf = new WCHAR[size];
	memset(pBuf,0,size*sizeof(WCHAR));
	
	int size2 = MultiByteToWideChar(
		CP_ACP,           // code page
		0,    // character-type options
		szNarrow.c_str(), // string to map
		szNarrow.size(),  // number of bytes in string
		pBuf,  // wide-character buffer
		size      // size of buffer
		);
	
	std::wstring szWide;
	if(size2>0)
		szWide = pBuf;
	
	memset(pBuf,0,size*sizeof(WCHAR));
	delete [] pBuf;
	pBuf = NULL;
	
	return szWide;
}

std::string WideToMultiByte(const std::wstring& szWide)
{
	BOOL bUsedDefaultChar=FALSE;
	int size = WideCharToMultiByte(
		CP_ACP,           // code page
		0,    // character-type options
		szWide.c_str(), // string to map
		szWide.size(),  // number of bytes in string
		NULL,  // character buffer
		0,      // size of buffer
		NULL,
		&bUsedDefaultChar);
	
	size++;
	
	CHAR* pBuf = new CHAR[size];
	memset(pBuf,0,size*sizeof(CHAR));
	
	bUsedDefaultChar=FALSE;
	int size2 = WideCharToMultiByte(
		CP_ACP,           // code page
		0,    // character-type options
		szWide.c_str(), // string to map
		szWide.size(),  // number of bytes in string
		pBuf,  // character buffer
		size,      // size of buffer
		NULL,
		&bUsedDefaultChar);
	
	std::string szNarrow;
	if(size2>0)
		szNarrow = pBuf;
	
	memset(pBuf,0,size*sizeof(CHAR));
	delete [] pBuf;
	pBuf = NULL;
	
	return szNarrow;
}

void	SetComboboxDroppedVisibleItemCount(HWND hCombobox, int nItemCount /*= COMBOBOX_DROPPED_ALL_ITEMS_VISIBLE*/, BOOL bRepaint /*= TRUE*/)
{
	int nItemHeight = SendMessage(hCombobox, CB_GETITEMHEIGHT, 0, 0L);
	if ( COMBOBOX_DROPPED_ALL_ITEMS_VISIBLE == nItemCount )
	{
		nItemCount = SendMessage(hCombobox, CB_GETCOUNT, 0, 0);
	}
	int nTotalHeight = nItemCount * nItemHeight;
	
	COMBOBOXINFO cbi = { sizeof(COMBOBOXINFO) };
	BOOL bRet = OGetComboBoxInfo(hCombobox, &cbi);
	if ( !bRet || NULL == cbi.hwndList )
		return;
	
	RECT rtClient, rtWnd;
	::GetClientRect(cbi.hwndList, &rtClient);
	::GetWindowRect(cbi.hwndList, &rtWnd);
	
	int nEdgeHeight = (rtWnd.bottom-rtWnd.top) - (rtClient.bottom-rtClient.top);
	nTotalHeight += nEdgeHeight;
	
	::MoveWindow(cbi.hwndList, rtWnd.left, rtWnd.top, rtWnd.right-rtWnd.left, nTotalHeight, bRepaint);
}

void SetDialogFocus( HWND hdlg, HWND hwndControl )
{
	SendMessage(hdlg, WM_NEXTDLGCTL, (WPARAM)hwndControl, TRUE);
}

HICON GetAssociatedFileIcon( LPCTSTR lpctszFilePath )
{
	SHFILEINFO shFileInfo = { 0 };
	
    // For retrieving icon
    VERIFY( SHGetFileInfo( lpctszFilePath, 
		FILE_ATTRIBUTE_NORMAL, 
		&shFileInfo, 
		sizeof( shFileInfo ), 
		SHGFI_SMALLICON | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES ));
	
    // Icon to return
    return shFileInfo.hIcon;
}

int GetStringResourceArray(UINT nID, CStringArray& arrArgs, TCHAR chSeparator /*= _T(' ')*/)
{
	CString strText;
	O_LOAD_CSTRING_OBJ(strText, nID);
	if ( !strText.IsEmpty() )
	{
		int nBegin	= 0;
		int nCurSep	= -1;
		while ( (nCurSep = strText.Find(chSeparator, nBegin)) >= 0 )
		{
			int nLen = nCurSep - nBegin;
			if (nLen > 0)
			{
				CString strItem = strText.Mid(nBegin, nLen);
				arrArgs.Add(strItem);
			}
			nBegin = nCurSep+1;
		}
		if (nBegin < strText.GetLength())
		{
			CString strItem = strText.Mid(nBegin);
			arrArgs.Add(strItem);
		}
		return arrArgs.GetSize();
	}
	return 0;
}

#define READ_FILE_LINE_SIZE		(64 * 1024)

UINT FileReadLines(LPCTSTR lpcszFile, CString& strText, UINT nLineBegin, UINT nNumber /*= 1*/)
{
	if ( 0 == nNumber )
	{
		ASSERT(0);	// incorrect parameter!
		return 0;
	}
#ifdef MSVC_NEW_VER
	FILE* fp = NULL;
	_tfopen_s(&fp, lpcszFile, _T("r"));
#else
	FILE* fp = _tfopen(lpcszFile, _T("r"));
#endif // MSVC_NEW_VER
	if ( !fp )
		return 0;
	
	UINT ii = 0;
	TCHAR* buf = NULL;
	try
	{
		buf = new TCHAR[READ_FILE_LINE_SIZE];
		for ( ; ii < nLineBegin+nNumber; ++ii)
		{
			if (_fgetts(buf, READ_FILE_LINE_SIZE, fp) == NULL) // end-of-file or error
			{
				break;
			}
			if ( ii >= nLineBegin )
			{
				strText += buf;
			}
		}
		delete [] buf;
	}
	catch (std::bad_alloc& )
	{
		delete [] buf;
	}
	
	fclose(fp);
	
	strText.TrimRight(_T("\r\n"));
	return ii-nLineBegin;	// returns the number of lines that read into strText
}
