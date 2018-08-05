#pragma once

#include "DocumentDlg.h"

// 最多7个标签
#define MAXTABPAGE 1000


// CTabSheet

class CTabSheet : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabSheet)

public:
	CTabSheet();
	virtual ~CTabSheet();

	//CDialog* m_dlgWnd[MAXTABPAGE]; //这个是存放对话框指针的指针数组
	CDocumentDlg* m_dlgWnd[MAXTABPAGE]; //这个是存放对话框指针的指针数组
	int m_curTabNumber; //记录当前用户添加了几个标签页
	int m_selTabID;  //当前用户点击的标签页的ID

	// 文档链表
	vector<CDocumentDlg*> m_DocumentTab;

	//通过这个函数,可以将一个对话框指针与添加的标签页关联起来,insWnd是创建的非模式对话框的指针,wndID是对话框的ID,pageText是标签页的标题
	void CreateTabPage(CWnd *insWnd, int wndID,CString pageText);

	void SetPageTitle( int index, TCHAR *caption );

protected:
	DECLARE_MESSAGE_MAP()

public:
	//添加控件的点击事件的处理,当点击后得到当前点击的标签页的ID,然后将与此标签页相关的对话框显示,其它的隐藏即可
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
