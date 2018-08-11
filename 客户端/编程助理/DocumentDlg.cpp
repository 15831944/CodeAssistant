// DocumentDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "DocumentDlg.h"
#include "afxdialogex.h"


// CDocumentDlg �Ի���

IMPLEMENT_DYNAMIC(CDocumentDlg, CDialogEx)

CDocumentDlg::CDocumentDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDocumentDlg::IDD, pParent)
{
	IsOutFunction = FALSE;
	FileClass = FileType = FilePath = FileName = _T("");
}

CDocumentDlg::~CDocumentDlg()
{
}

void CDocumentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CODE_RICHEDIT, m_Edit);
}


BEGIN_MESSAGE_MAP(CDocumentDlg, CDialogEx)
	ON_EN_SETFOCUS(IDC_CODE_RICHEDIT, &CDocumentDlg::OnSetfocusCodeRichedit)
	ON_EN_CHANGE(IDC_CODE_RICHEDIT, &CDocumentDlg::OnChangeCodeRichedit)
	ON_NOTIFY(EN_LINK,IDC_CODE_RICHEDIT, OnRichEditLink) 
END_MESSAGE_MAP()


// CDocumentDlg ��Ϣ�������


BOOL CDocumentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Edit
	m_Edit.SubclassDlgItem(IDC_CODE_RICHEDIT, this);

	// ���� ENM_CHANGE (��ӦChange�¼�)
	//EM_SETEVENTMASK
	long lMask = m_Edit.GetEventMask();
	lMask |= ENM_CHANGE;
	lMask &= ~ENM_PROTECTED;
	m_Edit.SetEventMask(lMask);


	// ��ʽ
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;

	cf.dwEffects&=~CFE_BOLD;      //���ô��壬ȡ����cf.dwEffects&=~CFE_BOLD;
	cf.dwEffects&=~CFE_ITALIC;    //����б�壬ȡ����cf.dwEffects&=~CFE_ITALIC;
	cf.dwEffects&=~CFE_UNDERLINE; //����б�壬ȡ����cf.dwEffects&=~CFE_UNDERLINE;
	cf.crTextColor = RGB(0,0,0);  //������ɫ
	cf.yHeight = 14 * 14;         //���ø߶�
	strcpy_s(cf.szFaceName, 1024 ,_T("����"));//��������
	m_Edit.SetDefaultCharFormat(cf);

	::SendMessage(m_Edit, EM_SETLANGOPTIONS, 0, 0);

	//������
	DWORD mask =::SendMessage(m_Edit.m_hWnd,EM_GETEVENTMASK, 0, 0);  
    mask = mask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS |ENM_KEYEVENTS;  
    ::SendMessage(m_Edit.m_hWnd,EM_SETEVENTMASK, 0, mask);  
    ::SendMessage(m_Edit.m_hWnd,EM_AUTOURLDETECT, true, 0);

	// �Զ�����
	if(GetPrivateProfileInt(_T("Setting"), _T("Line"), 0, _T("./Setting.ini")) == 1)
		m_Edit.SetTargetDevice(NULL,0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CDocumentDlg::PreTranslateMessage(MSG* pMsg)
{
	//�Ҽ��˵�
	if (pMsg->message == WM_RBUTTONDOWN)
	{
		// �����ж�
		if (GetDlgItem(IDC_CODE_RICHEDIT) == GetFocus())
		{
			CMenu popMenu;
			popMenu.LoadMenu(IDR_FUNCTION_MENU);         //����˵�
			CMenu *pPopup = popMenu.GetSubMenu(0);     //����Ӳ˵�ָ��

			pPopup->EnableMenuItem(IDM_SAVEIMG, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);     //������˵���ʹ��

			//pPopup->EnableMenuItem(ID_1,MF_BYCOMMAND|MF_ENABLED);                //����˵���ʹ��
			//pPopup->EnableMenuItem(ID_2,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);     //������˵���ʹ��
			//CPoint point;
			//ClientToScreen(&point);            //���ͻ�������ת������Ļ����

			POINT pt;
			::GetCursorPos(&pt);

			//pPopup->CheckMenuItem(ID_MEANING_SHOWDETAIL, m_bShowDetailedMeaning? MF_CHECKED : MF_UNCHECKED); 

			//��ʾ�����˵�����������Ϊ(����ڲ˵����|�����Ҽ���x��y��this)
			pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetApp()->GetMainWnd());
			pPopup->Detach();
			popMenu.DestroyMenu();

			// ����
			return true;
		}
	}

	// ������Ϣ����
	UINT  nKeyCode = pMsg->wParam; // virtual key code of the key pressed
	if (pMsg->message == WM_KEYDOWN)
	{
		// Ctrl + C (����)
		//if ( (nKeyCode == _T('C') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		//{
		//	CString   source = m_Edit.GetSelText();     
		//	//put   your   text   in   source   
		//	if(OpenClipboard())   
		//	{   
		//		HGLOBAL   clipbuffer;   
		//		char   *   buffer;   
		//		EmptyClipboard();   
		//		clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
		//		buffer   =   (char*)GlobalLock(clipbuffer);
		//		strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
		//		GlobalUnlock(clipbuffer);   
		//		SetClipboardData(CF_TEXT,clipbuffer);   
		//		CloseClipboard();   
		//	}

		//	return true;
		//}

		//// Ctrl + V (ճ��)
		//if ( (nKeyCode == _T('V') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		//{
		//	if( GetPrivateProfileInt(_T("Setting"), _T("Clear"), 0, _T("./Setting.ini")) == 1)
		//	{
		//		m_Edit.PasteSpecial(CF_TEXT);
		//		return true;
		//	}
		//}

		//// Ctrl + X (����)
		//if ( (nKeyCode == _T('X') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		//{
		//	CString   source = m_Edit.GetSelText();     
		//	//put   your   text   in   source   
		//	if(OpenClipboard())   
		//	{   
		//		HGLOBAL   clipbuffer;   
		//		char   *   buffer;   
		//		EmptyClipboard();   
		//		clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
		//		buffer   =   (char*)GlobalLock(clipbuffer);
		//		strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
		//		GlobalUnlock(clipbuffer);   
		//		SetClipboardData(CF_TEXT,clipbuffer);   
		//		CloseClipboard();   
		//	}

		//	// ����
		//	m_Edit.Cut();
		//	return true;
		//}

		// Ctrl + S (���淽��)
		if ( (nKeyCode == _T('S') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 6, 0);
			return true;
		}

		// Ctrl + N (��ӷ���)
		if ( (nKeyCode == _T('N') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 7, 0);
			return true;
		}

		// Ctrl + D (�����ļ�)
		if ( (nKeyCode == _T('D') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 8, 0);
			return true;
		}

		// Ctrl + F (�༭����)
		if ( (nKeyCode == _T('F') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 9, 0);
			return true;
		}

		// Ctrl + R (�Ƴ�����)
		if ( (nKeyCode == _T('R') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 10, 0);
			return true;
		}

		// Ctrl + Q (�����ʽ)
		if( (nKeyCode == _T('Q') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 11, 0);
			return true;
		}

		// Ctrl + E (�༭����)
		if( (nKeyCode == _T('E') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 12, 0);
			return true;
		}

		// Ctrl + I (����ͼƬ)
		if ( (nKeyCode == _T('I') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 13, 0);
			return true;
		}

		// Ctrl + P (��Ļ����)
		if ( (nKeyCode == _T('T') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 14, 0);
			return true;
		}

		// Ctrl + O (�򿪷���)
		if ( (nKeyCode == _T('O') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 15, 0);
			return true;
		}

		// Ctrl + P (��������)
		if ( (nKeyCode == _T('P') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 16, 0);
			return true;
		}

		// Ctrl + <- (�������)
		if ( (nKeyCode == VK_LEFT && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 17, 0);
			return true;
		}

		// Ctrl + -> (�������)
		if ( (nKeyCode == VK_RIGHT && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 18, 0);
			return true;
		}

		// Ctrl + M (�������)
		if ( (nKeyCode == _T('M') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 19, 0);
			return true;
		}

		// Ctrl + L (����ѡ��ͼƬ)
		if ( (nKeyCode == _T('L') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 29, 0);
			return true;
		}

		// Ctrl + Y (�ƶ�ͬ��)
		if ( (nKeyCode == _T('Y') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 30, 0);
			return true;
		}

		// Ctrl + K (��������)
		if ( (nKeyCode == _T('K') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			// ֪ͨ������
			::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 31, 0);
			return true;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDocumentDlg::OnOK()
{
}


void CDocumentDlg::OnRichEditLink(NMHDR*in_pNotifyHeader, LRESULT* out_pResult )  
{
    ENLINK* l_pENLink =(ENLINK*)in_pNotifyHeader;
    *out_pResult = 0;
    switch(l_pENLink->msg)
    {
    case WM_LBUTTONDOWN:
        {
            CString strURL;
            CHARRANGE crCharRange ;
            CRichEditCtrl * pTempEdit;
            pTempEdit =(CRichEditCtrl*)CRichEditCtrl::FromHandle(l_pENLink->nmhdr.hwndFrom);
            pTempEdit->GetSel(crCharRange );
            pTempEdit->SetSel(l_pENLink->chrg);

            strURL = pTempEdit->GetSelText();
            pTempEdit->SetSel(crCharRange);
            CWaitCursor l_WaitCursor;
            ShellExecute( this->GetSafeHwnd(), _T( "open"),strURL, NULL, NULL, SW_SHOWNORMAL );
            *out_pResult = 1;
        }
        break;
    case WM_LBUTTONUP:
        {
            *out_pResult = 1;
        }
        break ;
    }
}


void CDocumentDlg::OnChangeCodeRichedit()
{
	// ֪ͨ������
	::SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_CHILDMESSAGE, 4, 0);
}


void CDocumentDlg::OnSetfocusCodeRichedit()
{
	// ֪ͨ������
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 5, 0);
}


void CDocumentDlg::OnCancel()
{
	// ֪ͨ������
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 20, 0);
}
