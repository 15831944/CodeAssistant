#pragma once


// CSnapWindowDlg 对话框

class CSnapWindowDlg : public CDialog
{

public:
	CSnapWindowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSnapWindowDlg();
	BOOL OnInitDialog();

	// 退出标识
	BOOL IsExit;

// 对话框数据
	enum { IDD = IDD_SNAPWINDOW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void SnapWindow(POINT point);
	void DrawRectInWnd(LPRECT pRect);
	void SaveBmpToFile();
	void ClearScreen();
	HCURSOR m_hCursor;
	//定义当前窗口句柄
	HWND m_curWnd;
	//定义前一个窗口句柄
	HWND m_preWnd;
	RECT m_rectOfWnd;
	HBITMAP m_rectOfWndBmp;
	BOOL m_bSnap;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
