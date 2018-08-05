#pragma once
#define ID_HOTKEYPAUSE 100
#define ID_HOTKEYSTART 101

// CPickColorDlg 对话框

class CPickColorDlg : public CDialog
{
	

public:
	CPickColorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPickColorDlg();
	BOOL OnInitDialog();
	void ShowColorValue();
	void GetColorOfScreen();
	virtual void OnCancel();
// 对话框数据
	enum { IDD = IDD_PICKCOLOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HICON m_hIcon;
	COLORREF m_color;
	BOOL m_bPause;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
};
