#pragma once


// CMagnifierDlg 对话框

class CMagnifierDlg : public CDialog
{
public:
	CMagnifierDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMagnifierDlg();
	BOOL OnInitDialog();
	void OnCancel();
	void MagnifyScreen();
// 对话框数据
	enum { IDD = IDD_MAGNIFIER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
