// TabSheet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
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



// CTabSheet ��Ϣ�������

BOOL CTabSheet::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CTabCtrl::PreTranslateMessage(pMsg);
}


//������������һ����ǩҳ
void CTabSheet::CreateTabPage(CWnd *insWnd, int wndID,CString pageText)
{
     /*if (m_curTabNumber >= MAXTABPAGE)
     {
         AfxMessageBox(_T("��ǩҳ�����ﵽ���!"));
         return;
     }*/
     
	 //����newһ���Ի����ָ��,���ǲ�Ҫ����create����,�ٽ�Щָ�뵱�ɲ�������������,�������ɴ˺�������
     if (NULL == insWnd)
     {
         AfxMessageBox(_T("��ǩҳΪ��, ��������"));
         return;
     }

     //�����Ի���,�������ӱ�ǩҳ
     //CDialog* curDlg = (CDialog*)insWnd;
	 CDocumentDlg * curDlg = (CDocumentDlg*)insWnd;
     curDlg->Create(wndID, this);
     int suc = InsertItem(m_curTabNumber, pageText);
     if (-1 == suc)
     {
         AfxMessageBox(_T("�����ǩҳʧ��"));
         return;
     }

	 //�õ���ǰ��ǩҳ��λ���Ա����öԻ�����ʾ��λ��
	 CRect curRect;
	 GetClientRect(curRect);
	 curDlg->SetWindowPos(NULL,0,20,curRect.Width(),curRect.bottom,SWP_SHOWWINDOW);
     curDlg->ShowWindow(SW_SHOW);

     //�������Ӧ�Ĵ���ָ��������
     m_dlgWnd[m_curTabNumber] = curDlg;
	 m_DocumentTab.push_back(curDlg);

	  // ���ر�ǩ
	 for (int i = 0; i < m_curTabNumber; i ++)
	 {
		 m_dlgWnd[i]->SetWindowPos(NULL,0,20,curRect.Width(),curRect.bottom,SWP_HIDEWINDOW);
	 }

	 // ��ʾ��ǩ
	 curDlg->SetWindowPos(NULL,0,20,curRect.Width(),curRect.bottom,SWP_SHOWWINDOW);

     //��ʱѡ��ǰҳ��
     SetCurSel(GetItemCount() -1);
     m_selTabID = m_DocumentTab.size() -1;
     m_curTabNumber ++;
}


void CTabSheet::OnLButtonDown(UINT nFlags, CPoint point)
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	//�õ���ǰ�û�����ı�ǩҳ��ID
	int curSelect = GetCurSel();

	CRect curRect;
	GetClientRect(curRect);
	if (-1 == curSelect)
	{
		return;
	}

	// ��ֵ
	m_selTabID = curSelect;

	// ���ر�ǩ
	for(int i=0; i<m_curTabNumber; i++)
	{
		m_dlgWnd[i]->SetWindowPos(NULL,0,20,curRect.Width(),curRect.bottom,SWP_HIDEWINDOW);
	}

	// ��ʾ��ǩ
	m_DocumentTab.at(curSelect)->SetWindowPos(NULL,0,20,curRect.Width(),curRect.bottom,SWP_SHOWWINDOW);

	// ˢ�½���
	Invalidate();

	// ���ý���
	SetFocus();

	// ֪ͨ������
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 21, 0);
}


void CTabSheet::OnRButtonDown(UINT nFlags, CPoint point)
{
	CTabCtrl::OnRButtonDown(nFlags, point);

	//�õ���ǰ�û�����ı�ǩҳ��ID
	int curSelect = GetCurSel();

	// ��ֵ
	m_selTabID = curSelect;

	// �ж�ֵ
	if (-1 == curSelect)
	{
		return;
	}

	// �޸��ж�
	if(m_DocumentTab.at(curSelect)->IsChanged)
	{
		if( MessageBox(m_DocumentTab.at(curSelect)->FileName + _T("�����ѱ��޸�, �Ƿ񱣴淽��?"), _T("��⵽�޸�"), MB_ICONQUESTION | MB_YESNO) == IDYES )
		{
			// ����ֵ
			m_DocumentTab.at(curSelect)->IsChanged = false;

			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 32, 0);
			return;
		}

		// ����ֵ
		m_DocumentTab.at(curSelect)->IsChanged = false;
	}

	//�õ���ǰ��ǩҳ��λ���Ա����öԻ�����ʾ��λ��
	CRect curRect;
	GetClientRect(curRect);
	if (-1 == curSelect)
	{
		return;
	}

	// ��ֵ
	m_selTabID = curSelect -1;
	if(m_selTabID == -1)
	{
		m_dlgWnd[0]->m_Edit.SetWindowText(_T(""));
		SetPageTitle(0, _T("�·���"));

		// ��������
		m_dlgWnd[0]->FileClass = _T("");
		m_dlgWnd[0]->FileType  = _T("");
		m_dlgWnd[0]->FilePath  = _T("");
		m_dlgWnd[0]->FileName  = _T("�·���");
		m_dlgWnd[0]->IsChanged = FALSE;
	}
	else
	{
		// �Ƴ�����
		m_DocumentTab.erase(m_DocumentTab.begin() + curSelect);

		// ɾ����ǩ
		DeleteItem(curSelect);

		// ���õ�ǰ��ǩ
		SetCurSel(m_selTabID);
	}

	// ˢ�½���
	Invalidate();

	// ִ�����������Ϣ
	OnLButtonDown(nFlags, point);
}


void CTabSheet::SetPageTitle( int index, TCHAR *caption )
{
    TC_ITEM ti;
    ti.mask = TCIF_TEXT;
    ti.pszText = caption;
    SetItem( index, &ti );
}
