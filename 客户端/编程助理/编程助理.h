
// 编程助理.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "HttpFileEx.h"     // 网络数据传送
#include "SqliteManager.h"  // 数据库

#include <GdiPlus.h>
using namespace Gdiplus;

#define WM_CHILDMESSAGE  WM_USER + 100 //自定义子窗口消息

// CMainApp:
// 有关此类的实现，请参阅 编程助理.cpp
//

class CMainApp : public CWinApp
{
public:
	CMainApp();

	// 操作完成标志 & 读取一次
	BOOL IsFinished, IsReadOnce;

	// 数据库对象
	CSqliteManager m_Sql; 
	CString DataBase_Name, DataBase_Path, CodePath;

	// 数据库初始化
	BOOL InitDataBase();

	// 获取服务器数据
	CString OnGetWebInfo(CString ServerName, CString ServerPath, int ServerPort, CString Param, BOOL &IsSuccess);
	
	// 中文转换
	CString Convert(string Source);

	// 得到程序版本号
	CString GetApplicationVersion();

	// 得到程序运行目录
	CString GetModuleDir();

	// 获取程序工作路径
	CString GetWorkDir();

	// 使用帮助
	void OnHelp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMainApp theApp;