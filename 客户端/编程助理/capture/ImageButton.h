#pragma once


// CImageButton

class CImageButton : public CButton
{

public:
	CImageButton();
	virtual ~CImageButton();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void SetImageList(CImageList* pImage);
	void SetImageIndex(UINT index);
	void SetTextColor(COLORREF);
	void SetNormalBkColor(COLORREF);
	void SetActiveBkColor(COLORREF);
	void SetPressedBkColor(COLORREF);
private:
	CImageList* m_pImagelist;  //图像列表指针
	int m_ImageIndex; //图标索引
	BOOL m_bActive;
	COLORREF m_normalBkColor, m_activeBkColor, m_pressedBkColor;
	COLORREF m_textColor;
protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};


