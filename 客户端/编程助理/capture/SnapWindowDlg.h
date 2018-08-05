#pragma once


// CSnapWindowDlg �Ի���

class CSnapWindowDlg : public CDialog
{

public:
	CSnapWindowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSnapWindowDlg();
	BOOL OnInitDialog();

	// �˳���ʶ
	BOOL IsExit;

// �Ի�������
	enum { IDD = IDD_SNAPWINDOW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void SnapWindow(POINT point);
	void DrawRectInWnd(LPRECT pRect);
	void SaveBmpToFile();
	void ClearScreen();
	HCURSOR m_hCursor;
	//���嵱ǰ���ھ��
	HWND m_curWnd;
	//����ǰһ�����ھ��
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
