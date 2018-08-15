#pragma once
/* CVSTab

���ߣ�����
ʱ�䣺2009.8.26
�汾��1.1
���£�2009.8.31
	+ ToolTips֧�֣���Ҫ�ڸ�����ʵ��
	+ ��������Ӵ���
���ܣ�

��������TAB�������棨�ɲο�FireFox��Tab��

****************************************************************************
*(TAB1) | (TAB2) | (TAB3) | (TAB4) |(+)|                          |(��)|��X��
****************************************************************************

������
	��ͼ��+Ϊ�½�����Ϊ�����˵���XΪ�رհ�ť�������κ�һ��������ȥ������
	�����½��������˵���ǩ�⣬����ÿ��ҳ���ǩ�����ͬ��
	��ǩ��һ��Ĭ�Ͽ�ȣ����б�ǩ����ܺͳ����ñ�ǩ���Զ�ƽ����С��ȡ������½���ť
�������˵���ť���رտ�ȷֱ�ΪA��B��C��Ԥ���հ�������ΪW����ǩĬ�Ͽ��ΪX,�ؼ���
���ΪY����ǩ����ΪN,�����������£�
	if (N*X+A+B+C+W <= Y)
		X����
	else
		X = (Y-A-B-C-2)/N;

ע�⣺
	VSTAB_LBUTTONDOWN��VSTAB_RBUTTONDOWN
	�����ڱ��������������Ϣ�Ķ�Ӧ����Ϣ��������������Ҫdelete��lParam�����������ڴ�й©
	
*/
#include <map>

class CMyMemDC : public CDC 
{
private:	
	CBitmap		m_bitmap;		// Offscreen bitmap
	CBitmap*	m_oldBitmap;	// bitmap originally found in CMyMemDC
	CDC*		m_pDC;			// Saves CDC passed in constructor
	CRect		m_rect;			// Rectangle of drawing area.
	BOOL		m_bMemDC;		// TRUE if CDC really is a Memory DC.
public:
	
	CMyMemDC(CDC* pDC, const CRect* pRect = NULL) : CDC()
	{
		ASSERT(pDC != NULL); 

		// Some initialization
		m_pDC = pDC;
		m_oldBitmap = NULL;
		m_bMemDC = !pDC->IsPrinting();

		// Get the rectangle to draw
		if (pRect == NULL) {
			pDC->GetClipBox(&m_rect);
		} else {
			m_rect = *pRect;
		}
		
		if (m_bMemDC) {
			// Create a Memory DC
			CreateCompatibleDC(pDC);
			//pDC->LPtoDP(&m_rect);

			m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
			m_oldBitmap = SelectObject(&m_bitmap);
			
			SetMapMode(pDC->GetMapMode());
			//pDC->DPtoLP(&m_rect);
			SetWindowOrg(m_rect.left, m_rect.top);
		} else {
			// Make a copy of the relevent parts of the current DC for printing
			m_bPrinting = pDC->m_bPrinting;
			m_hDC       = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}

		// Fill background 
		FillSolidRect(m_rect,RGB(255,255,255));

	}

	
	~CMyMemDC()	
	{		
		if (m_bMemDC) {
			// Copy the offscreen bitmap onto the screen.
			m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
				this, m_rect.left, m_rect.top, SRCCOPY);			
			
			//Swap back the original bitmap.
			SelectObject(m_oldBitmap);
		} else {
			// All we need to do is replace the DC with an illegal value,
			// this keeps us from accidently deleting the handles associated with
			// the CDC that was passed to the constructor.			
			m_hDC = m_hAttribDC = NULL;
		}	
	}
	
	// Allow usage as a pointer	
	CMyMemDC* operator->() 
	{
		return this;
	}	

	// Allow usage as a pointer	
	operator CMyMemDC*() 
	{
		return this;
	}
};

//�궨��
#define CLOSE_ITEM_WIDTH		16		//�رձ�ǩ���
#define NEW_ITEM_WIDTH			20		//�½���ǩ���
#define DROP_MENU_WIDTH			16		//�����˵����
#define DEFAULT_ITEM_WIDHT		100		//Ĭ�ϱ�ǩ���
#define DEFAULT_ITEM_HEIGHT		25		//Ĭ�ϱ�ǩ�߶�
#define HMARGIN					2		//Ĭ��ˮƽ������
#define VMARGIN					0		

#define BTN_NEW_ITEM			0		
#define BTN_DROP_MENU			1
#define BTN_CLOSE_ITEM			2

//���а�ť����Ϊ��Ϣ���뷢�͸������ڣ�������б�ǩҳ��Ϊ�Ǹ�����
#define HIT_NEW_ITEM			-1
#define HIT_DROP_MENU			-2
#define HIT_CLOSE_ITEM			-3
#define HIT_OUT_OF_RANGE		-99		//���������������ʱ����

#define BTNST_NORMAL			0		//�½����������ñ�ǩΪ��ͨ״̬
#define BTNST_HOT				1		//����״̬������������ʱ��

#define COLOR_BORDER			RGB(51,153,255)
#define COLOR_BK_HOT			RGB(192,221,252)
#define COLOR_BK_NORMAL			GetSysColor(COLOR_BTNFACE)
#define COLOR_ITEM_SELECTED		RGB(51,153,255)//RGB(245,190,63)

#define TIMER_MOUSE_HOVER		0	//���������ͣ���೤ʱ��ŷ���mouse move ��Ϣ
#define MOUSE_HOVER_ELAPSE		500	//����

//��Ϣ����
#define VSTAB_LBUTTONDOWN		WM_USER+3333
#define VSTAB_RBUTTONDOWN		WM_USER+3334
#define VSTAB_MOUSEMOVE			WM_USER+3345

//�û���������������ṹ�ж���ÿ����ǩҳ������
typedef struct TabItem{
	TabItem(){
		nImage = 5;
		nCloseBtn = 0;
		nItem = 0;
		nData = 0;
		strItem = "";
		pWnd = NULL;
	}
	int nImage;			//Tabǰ�Ǹ�ͼƬ����,����Ǹ���������
	int nCloseBtn;		
	int nItem;			//��������
	int nData;			//�������������
	CString strItem;	//��������
	CWnd *pWnd;
}TabItem, *PTabItem;

struct NM_DATA{
	NMHDR hdr;
	TabItem* pItem;
	int hitArea;		//�¼�����������
	CPoint hitPoint;	//����������꣨��Ļ����ϵ��
};
class CVSTab : public CTabCtrl
{
	DECLARE_DYNAMIC(CVSTab)

public:
	CVSTab();
	virtual ~CVSTab();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();


private:
	int m_nControlID;

	int m_nItemCount;
	int m_nCurItem;
	int m_nItemHeight;	//��ʱ���ݿؼ��߶ȼ���
	int m_nItemWidth;
	
	int m_btnState[3];	//0Ϊ�½���1Ϊ�����˵�,2�ر�

	//
	int m_nCurMouseHoverItem;	
	int m_nOldMouseHoverItem;
	bool m_bTimerAlive;
	CPoint m_curPoint;

	CFont m_fontText;
	COLORREF m_clrText;

	CImageList m_ImageList;
	CImageList* m_pImageList;

	CList<PTabItem, PTabItem> m_TabItemList;
protected:
	void DrawTabItem(CDC *pDC, CRect rcItem, int nItem);
	void CVSTab::DrawArrow(CDC *pDC, CPoint ptArrow, COLORREF crCol=RGB(0,0,0));
	int HitTest(CPoint pt);
	int CheckItem(int nItem);
	
	int DeleteItem(POSITION rpos);
	int CalculateItemWidth(void);
	NM_DATA* NewNotifyMsg(int nCode, int nArea, CPoint pt, TabItem* pItem);
public:
	void SetImageList(CImageList *pImageList){ m_pImageList = pImageList; }
	CImageList* GetImageList(){ return m_pImageList; }

	//Item's function
	TabItem*  FindItem(int nItem, bool itemOrData=true);
	int GetItemCount(){ return m_nItemCount; }
	int GetCurSel() { return m_nCurItem; }
	int SetCurSel(int nItem);
	int SetItemText(int nItem, CString szText);
	CString GetItemText(int nItem);
	BOOL SetItemTextFont(CString szFont, UINT nHeight);
	CFont* GetItemTextFont(){ return &m_fontText; }
	void SetItemTextColor(COLORREF clrText){ m_clrText = clrText; }
	COLORREF GetItemTextColor(){ return m_clrText; }
	int SetItemImage(int nItem, int nImage);
	int GetItemImage(int nItem);
	void SetItemWidth(int wd){ m_nItemWidth = wd; }
	int GetItemWidth(){ return m_nItemWidth; }
	//void SetItemHeight(int ht){ m_nItemHeight = ht; }
	//int SetItemHeight(){ return m_nItemHeight; }

	int InsertItem(TabItem* pItem);
	int InsertItem(CString szText, CWnd *pWnd=NULL, int nImage=-1,int nData=0);
	int DeleteItem(int nInfo, bool itemOrData);
	CRect GetItemRect(int nItem);
	
	//create vstab
	BOOL Create(CWnd *pParent, CRect rc, UINT nID);

	//msg function
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	BOOL DrawIcon(CDC* pDC, int nImage, CPoint point);
	void DrawLine(CDC* pDC, CPoint ptStart, CPoint ptEnd, COLORREF clrPen);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


