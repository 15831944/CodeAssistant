#pragma once


// CSreenWnd
//定义5个位置的枚举结构
enum MOUSEPOSITION{UPPER, INNER, DOWN, LEFT, RIGHT, OUTSIDE};
class CFullScreenWnd : public CWnd
{
public:
	CFullScreenWnd();
	virtual ~CFullScreenWnd();

	// 退出标识
	BOOL IsExit;

protected:
	DECLARE_MESSAGE_MAP()

	
protected:
	MOUSEPOSITION CalPointPosition(LPRECT lpRect, POINT point);
	void DrawRect(CDC *pDC, RECT *pRect, CBrush *pBrush);
	void TrackDraw();
	void ShowTip();
	void Save();

	BOOL m_bClip;
	RECT m_drawRect;
	CDC m_compatibleDC;
	POINT m_oriPoint;//原点
	BOOL m_catchMouse;
	CBrush m_brush;
	HBITMAP m_hScreenBmp;
	HCURSOR hCursor[5];
	MOUSEPOSITION m_mousePos;

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
