#pragma once
#include "afxwin.h"
#include "autocombox.h"


// CSearchDlg 对话框

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(CWnd* pParent = NULL);   // 标准构造方法
	virtual ~CSearchDlg();

	BOOL NeedFind, FindFile, Web_Data;
	CString Title, FilePath, Function;
	CAutoCombox m_ComboBox;

	// 错误提示
	CString Error;
	void OnError();

	// 工作者线程
	static UINT Operate(LPVOID pParam);
	CWinThread * m_hOperate;
	CString Resolve, Parameter;

// 对话框数据
	enum { IDD = IDD_SEARCH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
