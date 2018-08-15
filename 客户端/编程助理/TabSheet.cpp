// TabSheet.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "TabSheet.h"


// CTabSheet

IMPLEMENT_DYNAMIC(CTabSheet, CTabCtrl)

CTabSheet::CTabSheet()
{
	m_curTabNumber = m_selTabID = 0;
}

CTabSheet::~CTabSheet()
{
}


BEGIN_MESSAGE_MAP(CTabSheet, CTabCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CTabSheet 消息处理程序

BOOL CTabSheet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CTabCtrl::PreTranslateMessage(pMsg);
}


//创建并且增加一个标签页
void CTabSheet::CreateTabPage(CWnd *insWnd, int wndID,CString pageText)
{
     /*if (m_curTabNumber >= MAXTABPAGE)
     {
         AfxMessageBox(_T("标签页己经达到最大!"));
         return;
     }*/
     
	 //首先new一个对话框的指针,但是不要调用create函数,再将些指针当成参数传进来即可,创建己由此函数做完
     if (NULL == insWnd)
     {
         AfxMessageBox(_T("标签页为空, 创建出错"));
         return;
     }

     //创建对话框,并且增加标签页
     //CDialog* curDlg = (CDialog*)insWnd;
	 CDocumentDlg * curDlg = (CDocumentDlg*)insWnd;
     curDlg->Create(wndID, this);
     int suc = InsertItem(m_curTabNumber, pageText);
     if (-1 == suc)
     {
         AfxMessageBox(_T("插入标签页失败"));
         return;
     }

	 //得到当前标签页的位置以便设置对话框显示的位置
	 CRect curRect;
	 GetClientRect(curRect);
	 curDlg->SetWindowPos(NULL,0,20,curRect.Width(),curRect.bottom,SWP_SHOWWINDOW);
     curDlg->ShowWindow(SW_SHOW);

     //将这个对应的窗体指针存放起来
     m_dlgWnd[m_curTabNumber] = curDlg;
	 m_DocumentTab.push_back(curDlg);

	  // 隐藏标签
	 for (int i = 0; i < m_curTabNumber; i ++)
	 {
		 m_dlgWnd[i]->SetWindowPos(NULL,0,20,curRect.Width(),curRect.bottom,SWP_HIDEWINDOW);
	 }

	 // 显示标签
	 curDlg->SetWindowPos(NULL,0,20,curRect.Width(),curRect.bottom,SWP_SHOWWINDOW);

     //此时选择当前页面
     SetCurSel(GetItemCount() -1);
     m_selTabID = m_DocumentTab.size() -1;
     m_curTabNumber ++;
}


void CTabSheet::OnLButtonDown(UINT nFlags, CPoint point)
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	//得到当前用户点击的标签页的ID
	int curSelect = GetCurSel();

	CRect curRect;
	GetClientRect(curRect);
	if (-1 == curSelect)
	{
		return;
	}

	// 赋值
	m_selTabID = curSelect;

	// 隐藏标签
	for(int i=0; i<m_curTabNumber; i++)
	{
		m_dlgWnd[i]->SetWindowPos(NULL,0,20,curRect.Width(),curRect.bottom,SWP_HIDEWINDOW);
	}

	// 显示标签
	m_DocumentTab.at(curSelect)->SetWindowPos(NULL,0,20,curRect.Width(),curRect.bottom,SWP_SHOWWINDOW);

	// 刷新界面
	Invalidate();

	// 设置焦点
	SetFocus();

	// 通知主窗口
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 21, 0);
}


void CTabSheet::OnRButtonDown(UINT nFlags, CPoint point)
{
	CTabCtrl::OnRButtonDown(nFlags, point);

	//得到当前用户点击的标签页的ID
	int curSelect = GetCurSel();

	// 赋值
	m_selTabID = curSelect;

	// 判断值
	if (-1 == curSelect)
	{
		return;
	}

	// 修改判断
	if(m_DocumentTab.at(curSelect)->IsChanged)
	{
		if( MessageBox(m_DocumentTab.at(curSelect)->FileName + _T("方法已被修改, 是否保存方法?"), _T("检测到修改"), MB_ICONQUESTION | MB_YESNO) == IDYES )
		{
			// 重置值
			m_DocumentTab.at(curSelect)->IsChanged = false;

			// 通知主窗口
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 32, 0);
			return;
		}

		// 重置值
		m_DocumentTab.at(curSelect)->IsChanged = false;
	}

	//得到当前标签页的位置以便设置对话框显示的位置
	CRect curRect;
	GetClientRect(curRect);
	if (-1 == curSelect)
	{
		return;
	}

	// 赋值
	m_selTabID = curSelect -1;
	if(m_selTabID == -1)
	{
		m_dlgWnd[0]->m_Edit.SetWindowText(_T(""));
		SetPageTitle(0, _T("新方法"));

		// 重置数据
		m_dlgWnd[0]->FileClass = _T("");
		m_dlgWnd[0]->FileType  = _T("");
		m_dlgWnd[0]->FilePath  = _T("");
		m_dlgWnd[0]->FileName  = _T("新方法");
		m_dlgWnd[0]->IsChanged = FALSE;
	}
	else
	{
		// 移除链表
		m_DocumentTab.erase(m_DocumentTab.begin() + curSelect);

		// 删除标签
		DeleteItem(curSelect);

		// 设置当前标签
		SetCurSel(m_selTabID);
	}

	// 刷新界面
	Invalidate();

	// 执行左键按下消息
	OnLButtonDown(nFlags, point);
}


void CTabSheet::SetPageTitle( int index, TCHAR *caption )
{
    TC_ITEM ti;
    ti.mask = TCIF_TEXT;
    ti.pszText = caption;
    SetItem( index, &ti );
}
