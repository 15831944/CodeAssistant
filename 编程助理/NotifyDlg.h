#pragma once


// CNotifyDlg 对话框

class CNotifyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNotifyDlg)

public:
	CNotifyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNotifyDlg();

	static UINT NotifyThread(LPVOID lpParameter);

// 对话框数据
	enum { IDD = IDD_NOTIFY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
