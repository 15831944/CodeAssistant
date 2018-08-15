#pragma once
#include "afxwin.h"


// CLoginDlg 对话框

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // 标准构造方法
	virtual ~CLoginDlg();

	// 方法
	void OnSuccess();
	void OnError();

	// 工作者线程
	static UINT Login(LPVOID pParam);
	CWinThread * m_hOperate;

	// 全局变量
	CString Parameter, UserId, Error;
	CToolTipCtrl m_toolTips;


// 对话框数据
	enum { IDD = IDD_LOGIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnRegister();
	afx_msg void OnCancel();
	afx_msg void OnRemember();
	afx_msg void OnAuto();
	CButton m_Remember;
	CButton m_Auto;
};
