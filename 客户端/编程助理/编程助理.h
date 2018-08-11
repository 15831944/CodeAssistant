
// �������.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "HttpFileEx.h"     // �������ݴ���
#include "SqliteManager.h"  // ���ݿ�

#include <GdiPlus.h>
using namespace Gdiplus;

#define WM_CHILDMESSAGE  WM_USER + 100 //�Զ����Ӵ�����Ϣ

// CMainApp:
// �йش����ʵ�֣������ �������.cpp
//

class CMainApp : public CWinApp
{
public:
	CMainApp();

	// ������ɱ�־ & ��ȡһ��
	BOOL IsFinished, IsReadOnce;

	// ���ݿ����
	CSqliteManager m_Sql; 
	CString DataBase_Name, DataBase_Path, CodePath;

	// ���ݿ��ʼ��
	BOOL InitDataBase();

	// ��ȡ����������
	CString OnGetWebInfo(CString ServerName, CString ServerPath, int ServerPort, CString Param, BOOL &IsSuccess);
	
	// ����ת��
	CString Convert(string Source);

	// �õ�����汾��
	CString GetApplicationVersion();

	// �õ���������Ŀ¼
	CString GetModuleDir();

	// ��ȡ������·��
	CString GetWorkDir();

	// ʹ�ð���
	void OnHelp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMainApp theApp;