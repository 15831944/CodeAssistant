#include "stdafx.h"
/*
	功能:拷贝屏幕矩形区域的像素
	参数:
		lpRect 指定需要拷贝的矩形区域
	返回值:
		指向保存屏幕像素的位图句柄
*/
HBITMAP CopyScreenToBitmap(LPRECT lpRect)
{
	//定义屏幕和内存设备描述表
	HDC hScreenDC, hMemDC;
	//定义位图句柄
	HBITMAP hBitmap, hOldBitmap; 
	//定义选择区域四个坐标点
	int x1, x2, y1, y2;
	//定义选择区域宽度,高度
	int nWidth, nHeight;
	int nScreenWidth, nScreenHeight;
	//如果矩形区域为空,则直接返回NULL
	if (IsRectEmpty(lpRect))
		return NULL;
	//获取选定区域坐标
	x1 = lpRect->left;
	x2 = lpRect->right;
	y1 = lpRect->top;
	y2 = lpRect->bottom;
	//获取屏幕宽度和高度
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	//确保选定区域是可见的
	if (x1 < 0)
		x1 = 0;
	if (x2 > nScreenWidth)
		x2 = nScreenWidth;
	if (y1 < 0)
		y1 = 0;
	if (y2 > nScreenHeight)
		y2 = nScreenHeight;
	//计算选择区域宽度和高度
	nWidth = x2 - x1;
	nHeight = y2 - y1;
	//创建屏幕设备描述表
	hScreenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScreenDC);
	// 创建一个与屏幕设备描述表兼容的位图
	hBitmap = CreateCompatibleBitmap(hScreenDC, nWidth, nHeight);
	//把新位图选进设备表述表当中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	//拷贝屏幕位图
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScreenDC, x1, y1, SRCCOPY);
	//得到屏幕位图句柄
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	//删除设备描述表
	DeleteDC(hMemDC);
	DeleteDC(hScreenDC);
	//返回位图句柄
	return hBitmap;
}
/*
	功能:拷贝设备描述表中指定的矩形区域像素到一个位图当中
	参数:
		lpRect 指定需要拷贝的矩形区域
		hSrcDC 设备描述表句柄
	返回值:
		指向保存位图像素的位图句柄
*/
HBITMAP CopyDCToBitmap(LPRECT lpRect, HDC hSrcDC)
{
	
	HDC hMemDC;
	//定义位图句柄
	HBITMAP hDestBitmap, hOldBitmap; 
	//定义选择区域四个坐标点
	int x1, x2, y1, y2;
	//定义选择区域宽度,高度
	int nWidth, nHeight;
	int nScreenWidth, nScreenHeight;
	//如果矩形区域为空,则直接返回NULL
	if (IsRectEmpty(lpRect))
		return NULL;
	//获取选定区域坐标
	x1 = lpRect->left;
	x2 = lpRect->right;
	y1 = lpRect->top;
	y2 = lpRect->bottom;
	//获取屏幕宽度和高度
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	//确保选定区域是可见的
	if (x1 < 0)
		x1 = 0;
	if (x2 > nScreenWidth)
		x2 = nScreenWidth;
	if (y1 < 0)
		y1 = 0;
	if (y2 > nScreenHeight)
		y2 = nScreenHeight;
	//计算选择区域宽度和高度
	nWidth = x2 - x1;
	nHeight = y2 - y1;
	//返回位图句柄
	hMemDC = CreateCompatibleDC(hSrcDC);
	hDestBitmap = CreateCompatibleBitmap(hSrcDC, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hDestBitmap);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hSrcDC, x1, y1, SRCCOPY);
	hDestBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
	return hDestBitmap;
}
