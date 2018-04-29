#pragma once


// CRegisterDlg 对话框

class CRegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegisterDlg)

public:
	CRegisterDlg(CWnd* pParent = NULL);   // 标准构造方法
	virtual ~CRegisterDlg();

	// 方法
	void OnSuccess();
	void OnError();

	// 工作者线程
	static UINT Register(LPVOID pParam);
	CWinThread * m_hOperate;

	// 全局变量
	CString Parameter, Error;
	CString UserName, Password;

// 对话框数据
	enum { IDD = IDD_REGISTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
