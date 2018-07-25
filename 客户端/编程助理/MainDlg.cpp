// MainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "MainDlg.h"
#include "afxdialogex.h"
#include <shlobj.h>

#include "atlimage.h"

#include "SearchDlg.h"
#include "EditDlg.h"
#include "LoginDlg.h"
#include "ProjectDlg.h"
#include "TransmissionDlg.h"
#include "SynchronizeDlg.h"

#include "lzma/LzmaLib.h"

#define  VMPBEGIN \
    __asm _emit 0xEB \
    __asm _emit 0x10 \
    __asm _emit 0x56 \
    __asm _emit 0x4D \
    __asm _emit 0x50 \
    __asm _emit 0x72 \
    __asm _emit 0x6F \
    __asm _emit 0x74 \
    __asm _emit 0x65 \
    __asm _emit 0x63 \
    __asm _emit 0x74 \
    __asm _emit 0x20 \
    __asm _emit 0x62 \
    __asm _emit 0x65 \
    __asm _emit 0x67 \
    __asm _emit 0x69 \
    __asm _emit 0x6E \
    __asm _emit 0x00

#define  VMPEND \
  __asm _emit 0xEB \
    __asm _emit 0x0E \
    __asm _emit 0x56 \
    __asm _emit 0x4D \
    __asm _emit 0x50 \
    __asm _emit 0x72 \
    __asm _emit 0x6F \
    __asm _emit 0x74 \
    __asm _emit 0x65 \
    __asm _emit 0x63 \
    __asm _emit 0x74 \
    __asm _emit 0x20 \
    __asm _emit 0x65 \
    __asm _emit 0x6E \
    __asm _emit 0x64 \
    __asm _emit 0x00

// CMainDlg �Ի���

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Setting = new CSettingDlg();

	// �����̳߳�ʼ��
	Type = 0;
	m_hOperate = m_hUpDate = NULL;

	IsNew = TRUE;
	IsSave= FALSE;
}


CMainDlg::~CMainDlg()
{
	m_Setting->DestroyWindow();
	delete m_Setting;
}


void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CODE_LIST, m_List);
	DDX_Control(pDX, IDC_CLASS_COMBO, m_Class);
	DDX_Control(pDX, IDC_TYPE_COMBO, m_Type);
	DDX_Control(pDX, IDC_CODE_RICHEDIT, m_Edit);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &CMainDlg::OnSave)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMainDlg::OnDelete)
	ON_BN_CLICKED(IDOK, &CMainDlg::OnOK)
	ON_BN_CLICKED(IDC_FONT_BUTTON, &CMainDlg::OnFont)
	ON_BN_CLICKED(IDC_NEW_BUTTON, &CMainDlg::OnNew)
	ON_BN_CLICKED(IDC_DIRECTORY_BUTTON, &CMainDlg::OnDirectory)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, &CMainDlg::OnRemove)

	ON_NOTIFY(NM_CLICK, IDC_CODE_LIST, &CMainDlg::OnClickCodeList)
	ON_NOTIFY(NM_RCLICK, IDC_CODE_LIST, &CMainDlg::OnRClickCodeList)

	ON_CBN_KILLFOCUS(IDC_CLASS_COMBO, &CMainDlg::OnKillfocusClassCombo)
	ON_CBN_KILLFOCUS(IDC_TYPE_COMBO, &CMainDlg::OnKillfocusTypeCombo)
	ON_CBN_DROPDOWN(IDC_CLASS_COMBO, &CMainDlg::OnDropdownClassCombo)
	ON_CBN_DROPDOWN(IDC_TYPE_COMBO, &CMainDlg::OnDropdownTypeCombo)

	ON_NOTIFY(NM_DBLCLK, IDC_CODE_LIST, &CMainDlg::OnDblclkCodeList)
	
	ON_NOTIFY(EN_LINK,IDC_CODE_RICHEDIT, OnRichEditLink) 
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CODE_LIST, &CMainDlg::OnItemchangedCodeList)

	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CMainDlg::OnEdit)
	ON_BN_CLICKED(IDC_SYNCHRONIZATION_BUTTON, &CMainDlg::OnSynchronization)
	ON_BN_CLICKED(IDC_SETTING_BUTTON, &CMainDlg::OnSetting)

	ON_MESSAGE(WM_CHILDMESSAGE, OnMessageChild)
	ON_EN_CHANGE(IDC_CODE_RICHEDIT, &CMainDlg::OnChangeCodeRichedit)
	ON_BN_CLICKED(IDC_MANAGER_BUTTON, &CMainDlg::OnManager)
	
	ON_EN_SETFOCUS(IDC_CODE_RICHEDIT, &CMainDlg::OnSetfocusCodeRichedit)
	ON_EN_KILLFOCUS(IDC_CODE_RICHEDIT, &CMainDlg::OnKillfocusCodeRichedit)

	ON_COMMAND(100, &CMainDlg::Complete)
	ON_COMMAND(101, &CMainDlg::OnError)
	ON_COMMAND(102, &CMainDlg::OnCancel)
	ON_COMMAND(103, &CMainDlg::OnSaveCode)
	ON_COMMAND(104, &CMainDlg::OnNewCode)
	ON_COMMAND(105, &CMainDlg::SetPic)

	ON_COMMAND(IDM_COPY,   &CMainDlg::OnCopy)
	ON_COMMAND(IDM_PASTE,  &CMainDlg::OnPaste)
	ON_COMMAND(IDM_CUT,    &CMainDlg::OnCut)
	ON_COMMAND(IDM_UNDO,    &CMainDlg::OnUndo)
	ON_COMMAND(IDM_REDO,    &CMainDlg::OnRedo)

	ON_COMMAND(IDM_IMAGE,  &CMainDlg::OnImage)
	ON_COMMAND(IDM_CAPTURE,    &CMainDlg::OnScreenCapture)

	ON_COMMAND(IDM_CLEAR,  &CMainDlg::OnClearFormat)
	ON_COMMAND(IDM_FONT,   &CMainDlg::OnFont)

	ON_COMMAND(IDM_CENTER, &CMainDlg::OnCenter)
	ON_COMMAND(IDM_LEFT,   &CMainDlg::OnLeft)
	ON_COMMAND(IDM_RIGHT,  &CMainDlg::OnRight)
	
END_MESSAGE_MAP()


// CMainDlg ��Ϣ�������


BOOL CMainDlg::OnInitDialog()
{
	//vmprotect ��ǿ�ʼ��.
	VMPBEGIN

	CDialogEx::OnInitDialog();

	// ����ͼ��
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// List
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT  //��������ѡ��
		| LVS_EX_HEADERDRAGDROP				      //���������϶�
		| LVS_EX_SUBITEMIMAGES			          //����ѡ����
		| LVS_EX_GRIDLINES);				      //����������

	m_List.SetHeadings(_T("��������,165;"));
	m_List.LoadColumnInfo();

	// ����
	LOGFONT   logfont;//���Ū�����Ա,ȫ�ֱ���,��̬��Ա  
    CFont   *pfont1   =   m_List.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //��������޸�����ĸ߱���
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //��������޸�����Ŀ����
    static   CFont   font1;
    font1.CreateFontIndirect(&logfont);
    m_List.SetFont(&font1);  
    font1.Detach();

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

	// ������
	DWORD mask =::SendMessage(m_Edit.m_hWnd,EM_GETEVENTMASK, 0, 0);  
    mask = mask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS |ENM_KEYEVENTS;  
    ::SendMessage(m_Edit.m_hWnd,EM_SETEVENTMASK, 0, mask);  
    ::SendMessage(m_Edit.m_hWnd,EM_AUTOURLDETECT, true, 0);

	// Ŀ¼���
	BOOL IsNew = false;
	CString FilePath = _T("File");
	DWORD Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //�ļ��в�����
	{
		IsNew = true;
		CreateDirectory(FilePath, NULL);
	}

	FilePath = _T("Project");
	Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //�ļ��в�����
	{
		IsNew = true;
		CreateDirectory(FilePath, NULL);
	}

	FilePath = _T("Code");
	Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //�ļ��в�����
	{
		IsNew = true;
		CreateDirectory(FilePath, NULL);
	}

	// ��ȡ����λ��
	if( GetPrivateProfileInt(_T("Setting"), _T("Position"), 0, _T("./Setting.ini")) == 1)
	{
		// ��ȡ���ر��ļ�
		CString Position;

		::GetPrivateProfileString(_T("Position"), _T("WindowPosition"), _T(""), Position.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));
		Position.ReleaseBuffer();

		// ��λ�ò�Ϊ��
		if(!Position.IsEmpty())
		{
			int Pos = Position.Find(","), m_Lift, m_Top, m_Width, m_Hight;
			CString Lift, Top, Width, Hight;

			if(Pos > 0)
			{
				Lift = Position.Left(Pos);
				Position = Position.Right(Position.GetLength() - Pos - 1);
				Pos  = Position.Find(",");

				Top  = Position.Left(Pos);
				Position = Position.Right(Position.GetLength() - Pos - 1);
				Pos  = Position.Find(",");

				Width = Position.Left(Pos);
				Position  = Position.Right(Position.GetLength() - Pos - 1);
				Pos   = Position.Find(",");

				Hight = Position.Left(Pos);
			}

			// ��ֵ
			m_Lift = atoi(Lift);
			m_Top  = atoi(Top);
			m_Width= atoi(Width);
			m_Hight= atoi(Hight);

			// ���ô���λ��
			SetWindowPos(NULL, m_Lift, m_Top, m_Width, m_Hight, SWP_NOSIZE);
		}
	}

	// �����Զ�����
	SetTimer(1,100,NULL);

	// �ж��Ƿ��һ������
	if(IsNew)
	{
		// ��ʾ�û�ʹ��˵��
		if (m_hOperate == NULL)
		{
			Type = 2;
			m_hOperate = AfxBeginThread(Operate, this);
			CloseHandle(m_hOperate->m_hThread);
		}
	}

	// ���öԻ���
	m_Setting->Create(IDD_SETTING_DIALOG, this);

	// ����ļ���Ŀ��
	if (m_hOperate == NULL)
	{
		Type = 0;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	// ������
	SetWindowText(_T("��Ӱ��������� ") + theApp.GetApplicationVersion());
	OnCheck();

	//vmprotect ��ǽ�����.
	VMPEND

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	VMPBEGIN

	//�Ҽ��˵�
	if (pMsg->message == WM_RBUTTONDOWN)
	{
		// �����ж�
		if (GetDlgItem(IDC_CODE_RICHEDIT) == GetFocus())
		{
			CMenu popMenu;
			popMenu.LoadMenu(IDR_FUNCTION_MENU);         //����˵�
			CMenu *pPopup = popMenu.GetSubMenu(0);     //����Ӳ˵�ָ��

			//pPopup->EnableMenuItem(ID_1,MF_BYCOMMAND|MF_ENABLED);                //����˵���ʹ��
			//pPopup->EnableMenuItem(ID_2,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);     //������˵���ʹ��
			//CPoint point;
			//ClientToScreen(&point);            //���ͻ�������ת������Ļ����

			POINT pt;
			::GetCursorPos(&pt);

			//pPopup->CheckMenuItem(ID_MEANING_SHOWDETAIL, m_bShowDetailedMeaning? MF_CHECKED : MF_UNCHECKED); 

			//��ʾ�����˵�����������Ϊ(����ڲ˵����|�����Ҽ���x��y��this)
			pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this);               
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
		if ( (nKeyCode == _T('C') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			CString   source = m_Edit.GetSelText();     
			//put   your   text   in   source   
			if(OpenClipboard())   
			{   
				HGLOBAL   clipbuffer;   
				char   *   buffer;   
				EmptyClipboard();   
				clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
				buffer   =   (char*)GlobalLock(clipbuffer);
				strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
				GlobalUnlock(clipbuffer);   
				SetClipboardData(CF_TEXT,clipbuffer);   
				CloseClipboard();   
			}

			return true;
		}

		// Ctrl + V (ճ��)
		if ( (nKeyCode == _T('V') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			if( GetPrivateProfileInt(_T("Setting"), _T("Clear"), 0, _T("./Setting.ini")) == 1)
			{
				m_Edit.PasteSpecial(CF_TEXT);
				return true;
			}
		}

		// Ctrl + X (����)
		if ( (nKeyCode == _T('X') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			CString   source = m_Edit.GetSelText();     
			//put   your   text   in   source   
			if(OpenClipboard())   
			{   
				HGLOBAL   clipbuffer;   
				char   *   buffer;   
				EmptyClipboard();   
				clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
				buffer   =   (char*)GlobalLock(clipbuffer);
				strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
				GlobalUnlock(clipbuffer);   
				SetClipboardData(CF_TEXT,clipbuffer);   
				CloseClipboard();   
			}

			// ����
			m_Edit.Cut();

			return true;
		}

		// Ctrl + S (���淽��)
		if ( (nKeyCode == _T('S') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnOK();
			return true;
		}

		// Ctrl + N (��ӷ���)
		if ( (nKeyCode == _T('N') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnNew();
			return true;
		}

		// Ctrl + D (�����ļ�)
		if ( (nKeyCode == _T('D') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnDirectory();
			return true;
		}

		// Ctrl + F (�༭����)
		if ( (nKeyCode == _T('F') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnFont();
			return true;
		}

		// Ctrl + R (�Ƴ�����)
		if ( (nKeyCode == _T('R') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnRemove();
			return true;
		}

		// Ctrl + Q (�����ʽ)
		if( (nKeyCode == _T('Q') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnClearFormat();
			return true;
		}

		// Ctrl + E (�༭����)
		if( (nKeyCode == _T('E') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnEditFunction();
			return true;
		}

		// Ctrl + I (����ͼƬ)
		if ( (nKeyCode == _T('I') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnImage();
			return true;
		}

		// Ctrl + P (��Ļ����)
		if ( (nKeyCode == _T('P') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnScreenCapture();
			return true;
		}

		// �ո� ȡ��ѡ��
		if(nKeyCode == VK_SPACE)
		{
			if (GetDlgItem(IDC_CODE_RICHEDIT) == GetFocus())
			{
				// �����ػس�����
			}
			else if (GetDlgItem(IDC_CODE_LIST) == GetFocus())
			{
				// ȡ��ѡ��
				m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);
				GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("��ӷ���"));
				return true;
			}
		}

		if(nKeyCode == VK_RETURN)
		{
			//�����Ƿ��ڿؼ���
			if (GetDlgItem(IDC_CODE_LIST) == GetFocus())
			{
				CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
				if (!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
				{
					// ʹ��д�ְ��
					if (m_hOperate == NULL)
					{
						Type = 8;
						m_hOperate = AfxBeginThread(Operate, this);
						CloseHandle(m_hOperate->m_hThread);
					}

					// ���λس���Ϣ
					return true;
				}
			}
			else if (GetDlgItem(IDC_CODE_RICHEDIT) == GetFocus())
			{
				// �����ػس�����
			}
			else
			{
				// ���λس���Ϣ
				return true;
			}
		}
	}

	VMPEND
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	CRect rc;
	CRect rect;
	GetWindowRect(&rect);//�����С
	rc.CopyRect(&rect);//���ƾ�����
	CPoint pOint;
	GetCursorPos(&pOint);//��ȡ���λ��

	//if(!Hide)
	//{
	//	rect.top = 0;
	//	MoveWindow(rect.left,rect.top,rc.Width(),rc.Height());//�ƶ�����
	//	return;
	//}

	if(rect.top < 0 && PtInRect(rect,pOint))//��ʾ����
	{
		rect.top = 0;
		MoveWindow(rect.left,rect.top,rc.Width(),rc.Height());//�ƶ�����
	}

	else if(rect.top > -3 && rect.top < 3 && !PtInRect(rect,pOint))//���ش���
	{
		rect.top = 3-rect.Height();
		MoveWindow(rect.left,rect.top,rc.Width(),rc.Height());
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CMainDlg::OnRichEditLink(NMHDR*in_pNotifyHeader, LRESULT* out_pResult )  
{
	VMPBEGIN

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

	VMPEND
}


// �Ӵ�����Ϣ
LRESULT CMainDlg::OnMessageChild(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case 1:
		Refresh();
		break;

	case 2:
		OnHelp();
		break;

	case 3:
		OnCheck();
		break;
	}

	return TRUE;
}


// �����߳�
UINT CMainDlg::Operate(LPVOID pParam)
{
	// ����ָ��
	CMainDlg * pWnd = ((CMainDlg*)pParam);

	// �쳣����
	try
	{
		switch(pWnd->Type)
		{
		case 0:
			{
				// ��ʼ��
				CString FilePath = _T("Code");

				// Class
				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						pWnd->m_Class.AddString(Finder.GetFileName());
				}

				pWnd->m_Class.SetCurSel(0);

				// Type
				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				FilePath = _T("Code\\") + Class;

				IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						pWnd->m_Type.AddString(Finder.GetFileName());
				}

				pWnd->m_Type.SetCurSel(0);


				// Code
				pWnd->m_Class.GetWindowText(Class);
				pWnd->m_Type.GetWindowText(Type);
				FilePath = _T("Code\\") + Class + _T("\\") + Type;

				IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						continue;
					else
					{
						CString Name = Finder.GetFileName();
						pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));
					}
				}

				// ��ȡ��������code�ļ�
				if(!theApp.CodePath.IsEmpty())
				{
					// ��ȡ����
					if(theApp.CodePath.Right(5) != _T(".code"))
						theApp.CodePath += _T(".code");

					// ������ֵ
					CString Code = theApp.CodePath;

					// ��ѹ������
					pWnd->Uncompress(Code, Code.Left(Code.GetLength() -5) + _T(".rtf"));

					// ��ȡrtf�ļ�
					pWnd->m_Edit.StreamInFromResource(Code.Left(Code.GetLength() -5) + _T(".rtf"), _T("SF_RTF"));

					// ɾ��ԭ�ļ�
					DeleteFile(Code.Left(Code.GetLength() -5) + _T(".rtf"));

					// ����ȡһ��
					if(theApp.IsReadOnce)
						DeleteFile(Code);
				}
				else
				{
					// �Զ���
					if( GetPrivateProfileInt(_T("Setting"), _T("Open"), 0, _T("./Setting.ini")) == 1)
					{
						// ��ȡ���ر��ļ�
						CString Class, Type, Name;

						::GetPrivateProfileString(_T("File"), _T("Class"), _T(""), Class.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));
						::GetPrivateProfileString(_T("File"), _T("Type"),  _T(""), Type.GetBuffer(MAX_PATH),  MAX_PATH, _T("./Setting.ini"));
						::GetPrivateProfileString(_T("File"), _T("Name"),  _T(""), Name.GetBuffer(MAX_PATH),  MAX_PATH, _T("./Setting.ini"));

						Class.ReleaseBuffer();
						Type.ReleaseBuffer();
						Name.ReleaseBuffer();

						// �Զ����ļ�
						int nIndex = pWnd->m_Class.FindStringExact(0, Class);
						if(nIndex != CB_ERR)
						{
							pWnd->m_Class.SetCurSel(nIndex);

							// ���ComboBox
							pWnd->m_Type.ResetContent();

							CString Class;
							pWnd->m_Class.GetWindowText(Class);
							CString FilePath = _T("Code\\") + Class;

							CFileFind Finder;
							BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
							while (IsFind)
							{
								IsFind = Finder.FindNextFile();

								if (Finder.IsDots())
									continue;
								if (Finder.IsDirectory())
									pWnd->m_Type.AddString(Finder.GetFileName());
							}

							// ����ComboBox
							pWnd->m_Type.SetCurSel(0);
						}
						else
						{
							// ������Ϣ��ʾ
							pWnd->Error = _T("�޷���ȡ�����ļ��� File �µ� Class ����, �Զ���ʧ�ܡ�");
							pWnd->PostMessage(WM_COMMAND, 101);

							// ������Ϊ��
							pWnd->m_hOperate = NULL;
							return FALSE;
						}

						nIndex = pWnd->m_Type.FindStringExact(0, Type);
						if(nIndex != CB_ERR)
						{
							pWnd->m_Type.SetCurSel(nIndex);
							// ��ձ༭��
							pWnd->m_Edit.SetWindowText(_T(""));

							// ����б�
							pWnd->m_List.DeleteAllItems();

							CString Class, Type;
							pWnd->m_Class.GetWindowText(Class);
							pWnd->m_Type.GetWindowText(Type);
							CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

							CFileFind Finder;
							BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
							while (IsFind)
							{
								IsFind = Finder.FindNextFile();

								if (Finder.IsDots())
									continue;
								if (Finder.IsDirectory())
									continue;
								else
								{
									CString Name = Finder.GetFileName();
									pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));
								}
							}
						}
						else
						{
							// ������Ϣ��ʾ
							pWnd->Error = _T("�޷���ȡ�����ļ��� File �µ� Type ����, �Զ���ʧ�ܡ�");
							pWnd->PostMessage(WM_COMMAND, 101);

							// ������Ϊ��
							pWnd->m_hOperate = NULL;
							return FALSE;
						}

						if(Name.IsEmpty())
						{
							// ������Ϣ��ʾ
							pWnd->Error = _T("�޷���ȡ�����ļ��� File �µ� Name ����, �Զ���ʧ�ܡ�");
							pWnd->PostMessage(WM_COMMAND, 101);

							// ������Ϊ��
							pWnd->m_hOperate = NULL;
							return FALSE;
						}

						int Count = pWnd->m_List.GetItemCount();
						for(int i=0; i<Count; i++)
						{
							CString Function = pWnd->m_List.GetItemText(i, 0);

							if(Function == Name)
							{
								//���ø�����ʾ  
								pWnd->m_List.SetFocus();//���ý���  
								pWnd->m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
								pWnd->m_List.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ�
								goto read;
								break;
							}
						}
					}
				}

			}break;

		case 1:
			{
read:
				int i = pWnd->m_List.GetNextItem(-1, LVIS_SELECTED);
				CString Name = pWnd->m_List.GetItemText(i, 0);
				if (!Name.IsEmpty() && i != -1)
				{
					CString Class, Type;
					pWnd->m_Class.GetWindowText(Class);
					pWnd->m_Type.GetWindowText(Type);
					CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

					// ��ʾ��Ϣ��ʾ
					//AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

					// ��ѹ������
					pWnd->Uncompress(FilePath + Name + _T(".code"), FilePath + Name + _T(".rtf"));

					// ��ȡrtf�ļ�
					pWnd->m_Edit.StreamInFromResource(FilePath + Name + _T(".rtf"), _T("SF_RTF"));

					// ɾ��ԭ�ļ�
					DeleteFile(FilePath + Name + _T(".rtf"));

					// ������ɱ�־
					//theApp.IsFinished = true;

					CString Text;
					pWnd->GetDlgItem(IDC_NEW_BUTTON)->GetWindowText(Text);
					if(Text != _T("�༭����"))
					{
						// ���İ�ť�ı�
						pWnd->GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("�༭����"));
					}

					// �·�����־
					pWnd->IsNew = FALSE;

					// �����־
					//pWnd->IsSave = FALSE;

					// ������Ϣ
					//Sleep(400);
					//pWnd->SendMessage(WM_COMMAND, 103);
				}
			}break;

		case 2:
			{
				// ��ѹ������
				pWnd->Uncompress(_T("ʹ��˵��.code"), _T("ʹ��˵��.rtf"));

				// ��ʾ�û�ʹ��˵��
				pWnd->m_Edit.StreamInFromResource(_T("ʹ��˵��.rtf"), _T("SF_RTF"));

				// ɾ��ԭ�ļ�
				DeleteFile(_T("ʹ��˵��.rtf"));
			}break;

		case 3:
			{
				// ���ComboBox
				pWnd->m_Type.ResetContent();

				CString Class;
				pWnd->m_Class.GetWindowText(Class);
				CString FilePath = _T("Code\\") + Class;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						pWnd->m_Type.AddString(Finder.GetFileName());
				}

				// ����ComboBox
				pWnd->m_Type.SetCurSel(0);
			}break;

		case 4:
			{
				// ��ձ༭��
				pWnd->m_Edit.SetWindowText(_T(""));

				// ����б�
				pWnd->m_List.DeleteAllItems();

				// ����ComboBox
				pWnd->m_Class.ResetContent();
				pWnd->m_Type.ResetContent();

				CString FilePath = _T("Code");
				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						pWnd->m_Class.AddString(Finder.GetFileName());
				}

				pWnd->m_Class.SetCurSel(0);
			}break;

		case 5:
			{
				// ��ձ༭��
				pWnd->m_Edit.SetWindowText(_T(""));

				// ����б�
				pWnd->m_List.DeleteAllItems();

				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				pWnd->m_Type.GetWindowText(Type);
				CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						continue;
					else
					{
						CString Name = Finder.GetFileName();
						pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));
					}
				}
			}break;

		case 6:
			{
				// ��ձ༭��
				pWnd->m_Edit.SetWindowText(_T(""));

				// ����б�
				pWnd->m_List.DeleteAllItems();

				// ����ComboBox
				pWnd->m_Type.ResetContent();

				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				CString FilePath = _T("Code\\") + Class;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						pWnd->m_Type.AddString(Finder.GetFileName());
				}

				pWnd->m_Type.SetCurSel(0);
			}break;

		case 7:
			{
				// ���������Զ�����
				if ( GetPrivateProfileInt(_T("Setting"), _T("Save"), 0, _T("./Setting.ini")) == 1)
				{
					CString Path = pWnd->m_List.GetItemText(pWnd->m_List.GetNextItem(-1, LVIS_SELECTED), 0);
					CString Class, Type, Text;
					pWnd->m_Class.GetWindowText(Class);
					pWnd->m_Type.GetWindowText(Type);
					pWnd->m_Edit.GetWindowText(Text);

					// ���������Ϣ������
					if(Class.IsEmpty() || Type.IsEmpty())
					{
						// ������Ϊ��
						pWnd->m_hOperate = NULL;
						return false;
					}
					else
					{
						// ����Դ��
						if(!Path.IsEmpty() && !Text.IsEmpty() && pWnd->m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
						{
							// Ŀ¼���
							DWORD DirPath = GetFileAttributes(_T("Code\\") + Class);
							if (DirPath == 0xFFFFFFFF)     //�ļ��в�����
							{
								CreateDirectory(_T("Code\\") + Class, NULL);
							}

							DirPath = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
							if (DirPath == 0xFFFFFFFF)     //�ļ��в�����
							{
								CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
							}

							// �Զ�����
							CString File = _T("Code\\") + Class + _T("\\") + Type + _T("\\");
							pWnd->m_Edit.StreamOutToFile(File + Path + _T(".rtf"), _T("SF_RTF"));

							// ѹ������
							pWnd->Compress(File + Path + _T(".rtf"), File + Path + _T(".code"));

							// ɾ��ԭ�ļ�
							DeleteFile(File + Path + _T(".rtf"));
						}
					}
				}
			}break;

		case 8:
			{
				CString Name = pWnd->m_List.GetItemText(pWnd->m_List.GetNextItem(-1, LVIS_SELECTED), 0);
				if (!Name.IsEmpty() && pWnd->m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
				{
					CString Class, Type;
					pWnd->m_Class.GetWindowText(Class);
					pWnd->m_Type.GetWindowText(Type);
					CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

					// ��ѹ������
					pWnd->Uncompress(FilePath + Name + _T(".code"), FilePath + Name + _T(".rtf"));

					// ���������� ִ��
					//ShellExecute(NULL, NULL, _T("write.exe"), FilePath + Name + _T(".rtf"), NULL, SW_HIDE);

					SHELLEXECUTEINFO ShExecInfo = {0};
					ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
					ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

					ShExecInfo.lpFile       = _T("write.exe");
					ShExecInfo.lpDirectory  = NULL;
					ShExecInfo.lpParameters = FilePath + Name + _T(".rtf");

					ShExecInfo.nShow = SW_SHOW;
					ShellExecuteEx(&ShExecInfo);

					// ��С��������
					//pWnd->ShowWindow(SW_MINIMIZE);

					// ��ʾ��Ϣ��ʾ
					AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

					AfxGetApp()->BeginWaitCursor();
					WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
					AfxGetApp()->EndWaitCursor();
					
					// ��ԭ������
					//pWnd->ShowWindow(SW_RESTORE);

					// ɾ��ԭ�ļ�
					DeleteFile(FilePath + Name + _T(".rtf"));

					// ������ɱ�־
					theApp.IsFinished = true;
				}
			}break;

		case 9:
			{
				CString Path = pWnd->m_List.GetItemText(pWnd->m_List.GetNextItem(-1, LVIS_SELECTED), 0);

				// �ⲿ�ļ�
				if( !theApp.CodePath.IsEmpty() && Path.IsEmpty() && pWnd->m_List.GetNextItem(-1, LVIS_SELECTED) == -1  )
				{
					// ��ʾ��Ϣ��ʾ
					AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

					CString TargetFile = theApp.CodePath.Left(theApp.CodePath.GetLength() -5);
					pWnd->m_Edit.StreamOutToFile(TargetFile + _T(".rtf"), _T("SF_RTF"));

					// ѹ������
					pWnd->Compress(TargetFile + _T(".rtf"), TargetFile + _T(".code"));

					// ɾ��ԭ�ļ�
					DeleteFile(TargetFile + _T(".rtf"));

					// ������ɱ�־
					theApp.IsFinished = true;

					// �����־
					pWnd->IsSave = TRUE;

					// ��Ϣ��ʾ
					pWnd->PostMessage(WM_COMMAND, 103);
				}
				else
				{
					CString Class, Type;
					pWnd->m_Class.GetWindowText(Class);
					pWnd->m_Type.GetWindowText(Type);

					if(Class.IsEmpty())
					{
						// ������Ϣ��ʾ
						pWnd->Error = _T("����������!");
						pWnd->PostMessage(WM_COMMAND, 101);

						// ������Ϊ��
						pWnd->m_hOperate = NULL;
						return false;
					}
					else if(Type.IsEmpty())
					{
						// ������Ϣ��ʾ
						pWnd->Error = _T("���������!");
						pWnd->PostMessage(WM_COMMAND, 101);

						// ������Ϊ��
						pWnd->m_hOperate = NULL;
						return false;
					}

					// Ŀ¼���
					DWORD DirPath = GetFileAttributes(_T("Code\\") + Class);
					if (DirPath == 0xFFFFFFFF)     //�ļ��в�����
					{
						CreateDirectory(_T("Code\\") + Class, NULL);
					}

					DirPath = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
					if (DirPath == 0xFFFFFFFF)     //�ļ��в�����
					{
						CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
					}

					// �ж��Ƿ����·���
					CString File = _T("Code\\") + Class + _T("\\") + Type + _T("\\");
					if(!Path.IsEmpty() && pWnd->m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
					{
						// ��ʾ��Ϣ��ʾ
						AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

						CString TargetFile = File + Path;
						pWnd->m_Edit.StreamOutToFile(TargetFile + _T(".rtf"), _T("SF_RTF"));

						// ѹ������
						pWnd->Compress(TargetFile + _T(".rtf"), TargetFile + _T(".code"));

						// ɾ��ԭ�ļ�
						DeleteFile(TargetFile + _T(".rtf"));

						// ������ɱ�־
						theApp.IsFinished = true;

						// �����־
						pWnd->IsSave = TRUE;

						// ��Ϣ��ʾ
						pWnd->PostMessage(WM_COMMAND, 103);
					}
					else
					{
						// ѯ���Ƿ��½�
						pWnd->PostMessage(WM_COMMAND, 104);
					}
				}
			}break;

		case 10:
			{
				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				pWnd->m_Type.GetWindowText(Type);

				if(Class.IsEmpty())
				{
					// ������Ϣ��ʾ
					pWnd->Error = _T("����������!");
					pWnd->PostMessage(WM_COMMAND, 101);

					// ������Ϊ��
					pWnd->m_hOperate = NULL;
					return false;
				}
				else if(Type.IsEmpty())
				{
					// ������Ϣ��ʾ
					pWnd->Error = _T("���������!");
					pWnd->PostMessage(WM_COMMAND, 101);

					// ������Ϊ��
					pWnd->m_hOperate = NULL;
					return false;
				}

				// Ŀ¼���
				DWORD DirPath = GetFileAttributes(_T("Code\\") + Class);
				if (DirPath == 0xFFFFFFFF)     //�ļ��в�����
				{
					CreateDirectory(_T("Code\\") + Class, NULL);
				}

				DirPath = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
				if (DirPath == 0xFFFFFFFF)     //�ļ��в�����
				{
					CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
				}

				CString File = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

				TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
				GetModuleFileName(NULL,exeFullPath,MAX_PATH);//�õ�����ģ�����ƣ�ȫ·��
				CString Path(exeFullPath), Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1), Dir = Path.Left(Path.GetLength() - Name.GetLength()) + File;

				CFileDialog FileDlg(TRUE, _T("code"), _T("Function"), OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, _T("Code File(*.code)|*.code;||"), pWnd);
				FileDlg.m_ofn.lpstrInitialDir = Dir;
				if(FileDlg.DoModal() != IDOK)
				{
					// ������Ϊ��
					pWnd->m_hOperate = NULL;
					return false;
				}
				else
				{
					// ��ʾ��Ϣ��ʾ
					AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

					CString strFile = FileDlg.GetPathName(); //�ļ������ھ��Զ�����
					CString Target  = strFile.Left(strFile.GetLength() -5);
					pWnd->m_Edit.StreamOutToFile(Target + _T(".rtf"), _T("SF_RTF"));

					// ѹ������
					pWnd->Compress(Target + _T(".rtf"), Target + _T(".code"));

					// ɾ��ԭ�ļ�
					DeleteFile(Target + _T(".rtf"));

					// ������ɱ�־
					theApp.IsFinished = true;

					// ��� & ���
					BOOL  isFind = FALSE;
					CString Name = FileDlg.GetFileName();

					int Count = pWnd->m_List.GetItemCount();
					for(int i=0; i<Count; i++)
					{
						CString Function = pWnd->m_List.GetItemText(i, 0);

						if(Function == Name.Left(Name.GetLength() - 5))
						{
							//��־
							isFind = TRUE;

							//���ø�����ʾ  
							pWnd->m_List.SetFocus();//���ý���  
							pWnd->m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
							pWnd->m_List.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 
							break;
						}
					}

					if(!isFind)
					{
						// �����б�
						int i = pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));

						//���ø�����ʾ  
						pWnd->m_List.SetFocus();//���ý���  
						pWnd->m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
						pWnd->m_List.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 
					}

					// �����־
					pWnd->IsSave = TRUE;

					// ��Ϣ��ʾ
					pWnd->PostMessage(WM_COMMAND, 103);
				}
			}break;

			case 11:
			{
				int nItem = -1;
				POSITION pos;

				// ��ձ༭��
				pWnd->m_Edit.SetWindowText(_T(""));

				// ��ʾ��Ϣ��ʾ
				AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

				// �Ƴ�����
				while (pos = pWnd->m_List.GetFirstSelectedItemPosition())
				{
					nItem = -1;
					nItem = pWnd->m_List.GetNextSelectedItem(pos);
					if (nItem >= 0 && pWnd->m_List.GetSelectedCount() > 0)
					{
						CString Path = pWnd->m_List.GetItemText(nItem, 0);

						CString Class, Type;
						pWnd->m_Class.GetWindowText(Class);
						pWnd->m_Type.GetWindowText(Type);

						// ɾ���ļ�
						DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".code"));

						// �Ƴ��б�
						pWnd->m_List.DeleteItem(nItem);

						// ɾ������
						pWnd->DeleteDirectory(_T("File\\") + Class + _T("\\") + Type + _T("\\") + Path);

						// �ж��Ƿ�Ϊ��
						if( pWnd->CountFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\")) <= 0 )
						{
							pWnd->DeleteDirectory( _T("Code\\") + Class + _T("\\") + Type + _T("\\") );

							// �������

							// ɾ��ComboBox ���ֵ
							pWnd->m_Type.DeleteString(pWnd->m_Type.GetCurSel());

							// ����ѡ��
							pWnd->m_Type.SetCurSel(0);

							// ���ݱ����Ϣ

							// ����б�
							pWnd->m_List.DeleteAllItems();

							pWnd->m_Class.GetWindowText(Class);
							pWnd->m_Type.GetWindowText(Type);
							CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

							CFileFind Finder;
							BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
							while (IsFind)
							{
								IsFind = Finder.FindNextFile();

								if (Finder.IsDots())
									continue;
								if (Finder.IsDirectory())
									continue;
								else
								{
									CString Name = Finder.GetFileName();
									pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));
								}
							}
						}

						if( pWnd->CountFile(_T("Code\\") + Class + _T("\\")) <= 0 )
						{
							pWnd->DeleteDirectory( _T("Code\\") + Class + _T("\\") );

							// �������������

							// ɾ��ComboBox ���ֵ
							pWnd->m_Class.DeleteString(pWnd->m_Class.GetCurSel());
							pWnd->m_Type.DeleteString(pWnd->m_Type.GetCurSel());

							// ����ѡ��
							pWnd->m_Class.SetCurSel(0);

							// ���ݱ����Ϣ

							// ��ձ༭��
							pWnd->m_Edit.SetWindowText(_T(""));

							// ���ComboBox
							pWnd->m_Type.ResetContent();

							CString Class;
							pWnd->m_Class.GetWindowText(Class);
							CString FilePath = _T("Code\\") + Class;

							CFileFind Finder;
							BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
							while (IsFind)
							{
								IsFind = Finder.FindNextFile();

								if (Finder.IsDots())
									continue;
								if (Finder.IsDirectory())
									pWnd->m_Type.AddString(Finder.GetFileName());
							}

							// ����ComboBox
							pWnd->m_Type.SetCurSel(0);


							// ����б�
							pWnd->m_List.DeleteAllItems();

							pWnd->m_Class.GetWindowText(Class);
							pWnd->m_Type.GetWindowText(Type);
							FilePath = _T("Code\\") + Class + _T("\\") + Type;

							IsFind = Finder.FindFile(FilePath + _T("./*.*"));
							while (IsFind)
							{
								IsFind = Finder.FindNextFile();

								if (Finder.IsDots())
									continue;
								if (Finder.IsDirectory())
									continue;
								else
								{
									CString Name = Finder.GetFileName();
									pWnd->m_List.AddItem(Name.Left(Name.GetLength() - 5));
								}
							}
						}
					}
				}

				// ������ɱ�־
				theApp.IsFinished = true;

			}break;

			case 12:
			{
				CString sFilter = _T("Bmp Files(*.bmp)|*.bmp||");
				CFileDialog dlg(TRUE,0,0, OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING| OFN_FILEMUSTEXIST ,(LPCTSTR)sFilter, pWnd);
				if(dlg.DoModal()==IDOK)
				{
					pWnd->FilePath = dlg.GetPathName();

					// ����ͼƬ
					pWnd->PostMessage(WM_COMMAND, 105);

					//CImage image;                      //����һ��CBitmap��  
					//image.Load(m_strFile);             //filenameΪҪ���ص��ļ���ַ  
					//HBITMAP hBitmap = image.Detach();  //���ر������ͼƬ�ľ��  
					//CBitmap bmp;                     // ����һ��bitmap  
					//bmp.Attach(hBitmap);             //���о���ĸ���  
				}

				pWnd->m_pPicObj = NULL;
			}break;
		}
	}
	catch(...)
	{
		AfxMessageBox(_T("�������쳣, λ��CMainDlg��Operate����."));
	}

	// ������Ϊ��
	pWnd->m_hOperate = NULL;
	return TRUE;
}


// ������
UINT CMainDlg::UpDate(LPVOID pParam)
{
	// ����ָ��
	CMainDlg * pWnd = ((CMainDlg*)pParam);
	BOOL IsSuccess  = false;

	// ��ȡ����������
	try
	{
		CString RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetUpDataInfo"), 80, NULL, IsSuccess);
		if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		{
			/*pWnd->Error = _T("�޷����ӵ�������, �������硣");
			pWnd->PostMessage(WM_COMMAND, 101);*/
		}
		else
		{
			if (IsSuccess)
			{
				if( RecvData.Replace(_T(";"), _T(";")) )
				{
					pWnd->UpDateInfo = RecvData;
					pWnd->PostMessage(WM_COMMAND, 100);
				}
				else
				{
					/*pWnd->Error = _T("���ݶ�ȡʧ�ܣ����Ժ����ԡ�");
					pWnd->PostMessage(WM_COMMAND, 101);*/
				}
			}
			else
			{
				/*pWnd->Error = _T("�޷����ӵ�������, �������硣");
				pWnd->PostMessage(WM_COMMAND, 101);*/
			}
		}
	}
	catch (...)
	{
		pWnd->Error = _T("�������쳣��λ��UpData��OnGetWebInfo������");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	pWnd->m_hUpDate = NULL;
	return true;
}


void CMainDlg::DeleteDirectory(CString Directory)   //ɾ��һ���ļ����µ���������  
{
	VMPBEGIN

	if(Directory.IsEmpty())   
	{ 
		RemoveDirectory(Directory); 
		return; 
	} 

	//����ɾ���ļ������ļ��� 
	CFileFind   ff; 

	BOOL bFound = ff.FindFile(Directory + _T("\\*"),0); 
	while(bFound) 
	{ 
		bFound = ff.FindNextFile(); 
		if(ff.GetFileName()== _T(".")||ff.GetFileName()== _T("..")) 
			continue; 

		//ȥ���ļ�(��)ֻ�������� 
		SetFileAttributes(ff.GetFilePath(),FILE_ATTRIBUTE_NORMAL); 

		if(ff.IsDirectory())  
		{   
			//�ݹ�ɾ�����ļ��� 
			DeleteDirectory(ff.GetFilePath()); 
			RemoveDirectory(ff.GetFilePath());
		} 
		else   
		{ 
			DeleteFile(ff.GetFilePath());   //ɾ���ļ� 
		} 
	} 

	ff.Close(); 

	//Ȼ��ɾ�����ļ��� 
	RemoveDirectory(Directory);

	VMPEND
} 


void CMainDlg::Refresh()
{
	// ˢ�´���
	Invalidate();

	//���ø�����ʾ  
	m_List.SetFocus();//���ý���  

	if(m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		m_List.SetItemState( m_List.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬
		m_List.EnsureVisible(m_List.GetItemCount() -1, FALSE);//���õ�ǰ��ͼ�ɼ�
		m_List.EnsureVisible(m_List.GetNextItem(-1, LVIS_SELECTED), FALSE);//���õ�ǰ��ͼ�ɼ�
	}
	else
	{
		m_List.EnsureVisible(m_List.GetItemCount() -1, FALSE);//���õ�ǰ��ͼ�ɼ�
		m_List.EnsureVisible(0, FALSE);//���õ�ǰ��ͼ�ɼ�
	}
}


void CMainDlg::OnHelp()
{
	// ��ʾ�����ĵ�
	if (m_hOperate == NULL)
	{
		Type = 2;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}
}


void CMainDlg::Split(CString source, CString divKey, CStringArray &dest)
{
	dest.RemoveAll();
	int pos = 0;
	int pre_pos = 0;
	while ( -1 != pos )
	{
		pre_pos = pos;
		pos     = source.Find(divKey, (pos +1));

		CString temp(source.Mid(pre_pos , (pos -pre_pos )));
		temp.Replace(divKey, _T(""));
		dest.Add(temp);
	}
}


bool CMainDlg::Compress  (const char* scrfilename, const char* desfilename)
{
	FILE *fin, *fout;
	fopen_s(&fin, scrfilename, "rb");
	if(fin == NULL)
	{
		printf("Open ScrFile ERR:%s\n",scrfilename);
		return false;
	}
	fopen_s(&fout, desfilename, "wb");
	if(fout == NULL)
	{
		printf("Open DesFile ERR:%s\n",desfilename);
		fclose(fin);
		return false;
	}

	fseek(fin,0,SEEK_END);
	size_t saveinsize=ftell(fin);
	fseek(fin,0,SEEK_SET);
	size_t saveoutsize = saveinsize * (size_t)1.1 + 1026 * 16;
	unsigned char* inbuff=(unsigned char*)malloc(saveinsize);
	unsigned char* outbuff=(unsigned char*)malloc(saveoutsize);
	unsigned char props[5]={0};
	size_t propsSize=5;
	size_t readlength=fread(inbuff,1,saveinsize,fin);
	if(readlength!=saveinsize){
		printf("read err\n");
		fclose(fin);
		fclose(fout);
		return false;
	}
	int	res = LzmaCompress(outbuff,&saveoutsize,inbuff,saveinsize,
			props,&propsSize,
			5,
			1 << 24,
			3,
			0,
			2,
			32,
			1);
	if(res!=0){
		printf("LzmaCompressErr:%d\n",res);
		fclose(fin);
		fclose(fout);
		return true;
	}
	int zero=0;
	fwrite(props,1,propsSize,fout);
	fwrite(&saveinsize,1,4,fout);
	fwrite(&zero,1,4,fout);
	fwrite(outbuff,1,saveoutsize,fout);
	
	fclose(fin);
	fclose(fout);
	return true;
}


bool CMainDlg::Uncompress(const char* scrfilename, const char* desfilename)
{
	FILE *fin, *fout;
	fopen_s(&fin, scrfilename, "rb");
	if(fin == NULL)
	{
		printf("Open ScrFile ERR:%s\n",scrfilename);
		return false;
	}
	fopen_s(&fout, desfilename, "wb");
	if(fout == NULL)
	{
		printf("Open DesFile ERR:%s\n",desfilename);
		fclose(fin);
		return false;
	}

	fseek(fin,0,SEEK_END);
	size_t saveinsize = ftell(fin);
	fseek(fin,0,SEEK_SET);
	size_t saveoutsize = saveinsize * (size_t)1.1 + 1026 * 16;
	unsigned char* inbuff=(unsigned char*)malloc(saveinsize);
	unsigned char props[5]={0};
	size_t propsSize=5;

	fread(props,1,5,fin);
	fread(&saveoutsize,1,4,fin);
	fseek(fin,4,SEEK_CUR);
	unsigned char* outbuff=(unsigned char*)malloc(saveoutsize);
	size_t readlength=fread(inbuff,1,saveinsize-13,fin);
	if(readlength!=(saveinsize-13)){
		printf("read err\n");
		fclose(fin);
		fclose(fout);
		return false;
	}
	int	res=LzmaUncompress(outbuff,&saveoutsize,inbuff,&readlength,
			props,propsSize);
	if(res!=0){
		printf("LzmaUncompress:%d\n",res);
		fclose(fin);
		fclose(fout);
		return true;
	}
	fwrite(outbuff,1,saveoutsize,fout);
	fclose(fin);
	fclose(fout);
	return true;
}


void CMainDlg::Complete()
{
	CStringArray TextArray;
	Split(UpDateInfo, _T(";"), TextArray);

	CString Version  = TextArray.GetAt(0);
	CString FileInfo = TextArray.GetAt(1);
	CString TipText  = Version;

	Version.Replace(_T("v"), _T(""));
	Version.Replace(_T("."), _T(""));

	CString OldVersion = theApp.GetApplicationVersion();
	OldVersion.Replace(_T("v"), _T(""));
	OldVersion.Replace(_T("."), _T(""));

	// �ȶ�
	int old_version = atoi(OldVersion);
	int new_version = atoi(Version);

	if(new_version  > old_version)
	{
		TipText.Format(_T("�°汾 %s �ѷ���, �Ƿ���Ҫ����?"), TipText);
		if( MessageBox(TipText, _T("��⵽����"), MB_ICONQUESTION | MB_YESNO) == IDYES )
		{
			// ��ʼ����
			CStringArray TargetList;
			TargetList.Add(FileInfo);

			CTransmissionDlg pDlg;
			pDlg.TargetList = &TargetList;
			pDlg.IsDownload = true;
			pDlg.IsCode     = false;
			pDlg.IsProject  = false;
			pDlg.IsUpDate   = true;
			pDlg.DoModal();
		}
	}
}


void CMainDlg::OnError()
{
	// ��ʾ����
	AfxMessageBox(Error);
}


void CMainDlg::OnCheck()
{
	// ������
	if(GetPrivateProfileInt(_T("Setting"), _T("Update"),1, _T("./Setting.ini")) == 1)
	{
		if(m_hUpDate == NULL)
			m_hUpDate = AfxBeginThread(UpDate, this);
	}
}


void CMainDlg::OnSaveCode()
{
	// ��Ϣ��ʾ
	if(IsSave)
		MessageBox(_T("�����ѱ���ɹ�!"), _T("�������"), MB_ICONINFORMATION);

	//���ø�����ʾ  
	m_List.SetFocus();//���ý���  
	m_List.SetItemState( m_List.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
	m_List.EnsureVisible(m_List.GetNextItem(-1, LVIS_SELECTED), FALSE);//���õ�ǰ��ͼ�ɼ� 
}


void CMainDlg::OnNewCode()
{
	// ѯ���Ƿ��½�
	if( MessageBox(_T("û��ѡ�еķ���, �Ƿ����Ϊ�·���?"), _T("�Ƿ���ӷ���"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		OnNew();
	}
}


void CMainDlg::OnCopy()
{
	CString   source = m_Edit.GetSelText();     
	//put   your   text   in   source   
	if(OpenClipboard())   
	{   
		HGLOBAL   clipbuffer;   
		char   *   buffer;   
		EmptyClipboard();   
		clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
		buffer   =   (char*)GlobalLock(clipbuffer);
		strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
		GlobalUnlock(clipbuffer);   
		SetClipboardData(CF_TEXT,clipbuffer);   
		CloseClipboard();   
	}
}


void CMainDlg::OnPaste()
{
	if( GetPrivateProfileInt(_T("Setting"), _T("Clear"), 0, _T("./Setting.ini")) == 1)
		m_Edit.PasteSpecial(CF_TEXT);
	else
		m_Edit.Paste();
}


void CMainDlg::OnCut()
{
	CString   source = m_Edit.GetSelText();     
	//put   your   text   in   source   
	if(OpenClipboard())   
	{   
		HGLOBAL   clipbuffer;   
		char   *   buffer;   
		EmptyClipboard();   
		clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
		buffer   =   (char*)GlobalLock(clipbuffer);
		strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
		GlobalUnlock(clipbuffer);   
		SetClipboardData(CF_TEXT,clipbuffer);   
		CloseClipboard();   
	}

	// ����
	m_Edit.Cut();
}


void CMainDlg::OnUndo()
{
	m_Edit.Undo();
}


void CMainDlg::OnRedo()
{
	m_Edit.Redo();
}


void CMainDlg::OnLeft()
{
	CString source = m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		CHARFORMAT cf;
		m_Edit.GetSelectionCharFormat(cf);

		PARAFORMAT pf;
		m_Edit.GetParaFormat(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_LEFT; //PFA_CENTER or PFA_RIGHT
		m_Edit.SetParaFormat(pf);
	}
	else
	{
		AfxMessageBox(_T("û����Ҫ���������!"));
	}
}


void CMainDlg::OnRight()
{
	CString source = m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		CHARFORMAT cf;
		m_Edit.GetSelectionCharFormat(cf);

		PARAFORMAT pf;
		m_Edit.GetParaFormat(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_RIGHT; //PFA_CENTER or PFA_RIGHT
		m_Edit.SetParaFormat(pf);
	}
	else
	{
		AfxMessageBox(_T("û����Ҫ���ҵ�����!"));
	}
}


void CMainDlg::OnCenter()
{
	CString source = m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		CHARFORMAT cf;
		m_Edit.GetSelectionCharFormat(cf);

		PARAFORMAT pf;
		m_Edit.GetParaFormat(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_CENTER; //PFA_CENTER or PFA_RIGHT
		m_Edit.SetParaFormat(pf);
	}
	else
	{
		AfxMessageBox(_T("û����Ҫ���е�����!"));
	}
}


BOOL CMainDlg::GetPic(REOBJECT FAR* pObject)
{
	CRichEditCtrl* pTextIn1=(CRichEditCtrl*)GetDlgItem(IDC_CODE_RICHEDIT);
	IRichEditOle* pITextIn1=pTextIn1->GetIRichEditOle();
	LONG lCount=pITextIn1->GetObjectCount();

	LRESULT lRet;
	for(int i=0;i<lCount;i++)
	{
		lRet=pITextIn1->GetObject(0,pObject,REO_GETOBJ_POLEOBJ);
		if(lRet==S_OK)
		{
			pObject->poleobj->Release();
			if(pObject->poleobj == m_pPicObj)
			{
				return TRUE;
			}
		}

	}
	return FALSE;
}


void CMainDlg::SetPic()
{
	HBITMAP hBitmap=(HBITMAP)::LoadImage(AfxGetInstanceHandle(), FilePath, IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_DEFAULTCOLOR);

	STGMEDIUM stgm;
	stgm.tymed   = TYMED_GDI;
	stgm.hBitmap = hBitmap;
	stgm.pUnkForRelease = NULL;

	FORMATETC fm;
	fm.cfFormat = CF_BITMAP;
	fm.ptd      = NULL;
	fm.dwAspect = DVASPECT_CONTENT;
	fm.lindex   = -1;
	fm.tymed    = TYMED_GDI;

	IStorage *pStorage;
	LPLOCKBYTES lpLockBytes=NULL;
	SCODE sc=::CreateILockBytesOnHGlobal(NULL,TRUE,&lpLockBytes);
	if(sc!=S_OK) AfxThrowOleException(sc);
	ASSERT(lpLockBytes!=NULL);
	sc=::StgCreateDocfileOnILockBytes(lpLockBytes,STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE,0,&pStorage);
	if(sc!=S_OK)
	{
		VERIFY(lpLockBytes->Release()==0);
		lpLockBytes=NULL;
		AfxThrowOleException(sc);

	}
	ASSERT(pStorage!=NULL);

	COleDataSource *pDataSource=new COleDataSource;
	pDataSource->CacheData(CF_BITMAP,&stgm);
	LPDATAOBJECT lpDataObject=(LPDATAOBJECT)pDataSource->GetInterface(&IID_IDataObject);

	LPOLECLIENTSITE  lpClientSite;
	CRichEditCtrl* pTextIn1=(CRichEditCtrl*)GetDlgItem(IDC_CODE_RICHEDIT);

	pTextIn1->GetIRichEditOle()->GetClientSite(&lpClientSite);

	//����OLE����
	IOleObject *pOleObject;
	sc=OleCreateStaticFromData(lpDataObject,IID_IOleObject,OLERENDER_FORMAT,
		&fm,lpClientSite,pStorage,(void **)&pOleObject);
	if(sc!=S_OK) AfxThrowOleException(sc);

	//����OLE����
	REOBJECT reobject;
	ZeroMemory(&reobject,sizeof(REOBJECT));
	reobject.cbStruct=sizeof(REOBJECT);


	CLSID clsid; 
	sc = pOleObject->GetUserClassID(&clsid); 
	if (sc != S_OK) 
		AfxThrowOleException(sc);

	reobject.clsid = clsid; 
	reobject.cp = REO_CP_SELECTION; 
	reobject.dvaspect = DVASPECT_CONTENT; 
	reobject.poleobj = pOleObject; 
	reobject.polesite = lpClientSite; 
	reobject.pstg = pStorage; 
	reobject.dwUser = 0;


	REOBJECT obj;
	obj.cbStruct=sizeof(obj);
	if( GetPic(&obj) )
	{
		long lStart,lEnd;
		pTextIn1->GetSel(lStart,lEnd);
		pTextIn1->SetSel(obj.cp,obj.cp+1);
		pTextIn1->Clear();

		pTextIn1->GetIRichEditOle()->InsertObject(&reobject);
		pTextIn1->SetSel(lStart,lEnd);
	}
	else
	{
		pTextIn1->GetIRichEditOle()->InsertObject(&reobject);
	}

	// �ͷŶ���
	if (hBitmap)
	{
		//DeleteObject(hBitmap);
	}
	//image.Destroy();

	delete pDataSource;
	m_pPicObj = pOleObject;

	::SendMessage(pTextIn1->m_hWnd,EM_SCROLLCARET,(WPARAM)0,(LPARAM)0);
	pTextIn1->SetFocus();
}


void CMainDlg::OnImage()
{
	// ����ͼƬ
	if (m_hOperate == NULL)
	{
		Type = 12;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}
}


void  CMainDlg::OnScreenCapture()
{
	// �ӳ�
	Sleep(300);

	CDC* pScreenDc = CDC::FromHandle(::GetDC(NULL));//��ĻDC
	CDC memDc;// �ڴ�DC
	CBitmap memBmp;
	CBitmap* pOldBmp;
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	memDc.CreateCompatibleDC(pScreenDc);
	memBmp.CreateCompatibleBitmap(pScreenDc, cx, cy);
	pOldBmp = memDc.SelectObject(&memBmp);
	memDc.BitBlt(0, 0, cx, cy, pScreenDc, 0, 0, SRCCOPY);

	//���Ƶ����а�
	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, memBmp.Detach());
	CloseClipboard();

	// ��ʾ����ǰ����
	/*CClientDC clientDc(this);
	CRect rcClient;
	GetClientRect(rcClient);
	clientDc.StretchBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDc, 0, 0, cx, cy, SRCCOPY);*/

	memDc.SelectObject(pOldBmp);
	memDc.DeleteDC();
	memBmp.DeleteObject();

	// ճ��
	m_Edit.Paste();
}


int CMainDlg::CountFile(CString DirPath)
{
	int count = 0;
	CFileFind finder;
	BOOL working = finder.FindFile(DirPath + "./*.*");

	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
			count += CountFile(finder.GetFilePath());
		else
		{
			count++;
		}
	}
	return count;
}


void CMainDlg::OnChangeCodeRichedit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	VMPBEGIN

	//IsChange = true;
	// �Զ�����
	if (m_hOperate == NULL)
	{
		Type = 7;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND
}


void CMainDlg::OnKillfocusCodeRichedit()
{
	//if(IsChange)
	//{
	//	// �Զ�����
	//	if (m_hOperate == NULL)
	//	{
	//		Type = 7;
	//		m_hOperate = AfxBeginThread(Operate, this);
	//		CloseHandle(m_hOperate->m_hThread);
	//	}

	//	// ��ʼ��
	//	IsChange = false;
	//}
}


void CMainDlg::OnSetfocusCodeRichedit()
{
	GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("��ӷ���"));
	IsNew = TRUE;
}


void CMainDlg::OnClickCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// ��ȡĿ���ļ�
	if (m_hOperate == NULL)
	{
		Type = 1;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	*pResult = 0;

	VMPEND
}


void CMainDlg::OnRClickCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	// ȡ��ѡ��
	m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);
	CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

	// ����Ŀ��
	CSearchDlg dlg;
	dlg.FilePath = FilePath;
	if(dlg.DoModal() == IDOK)
	{
		CString Function = dlg.Function;

		int Count = m_List.GetItemCount();
		for(int i=0; i<Count; i++)
		{
			CString Name = m_List.GetItemText(i, 0);

			if(Name == Function)
			{
				//���ø�����ʾ  
				m_List.SetFocus();//���ý���  
				m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
				m_List.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 
				break;
			}
		}
	}

	*pResult = 0;

	VMPEND
}


void CMainDlg::OnDblclkCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// ʹ��д�ְ��
	if (m_hOperate == NULL)
	{
		Type = 8;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND

	*pResult = 0;
}


void CMainDlg::OnItemchangedCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN
		OnClickCodeList(pNMHDR, pResult);
	VMPEND
}


void CMainDlg::OnSave()
{
	VMPBEGIN

	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	if(Class.IsEmpty())
	{
		AfxMessageBox(_T("����������!"));
		return;
	}
	else if(Type.IsEmpty())
	{
		AfxMessageBox(_T("���������!"));
		return;
	}

	DWORD Path = GetFileAttributes(_T("Code\\") + Class);
	if (Path == 0xFFFFFFFF)     //�ļ��в�����
	{
		CreateDirectory(_T("Code\\") + Class, NULL);
	}

	Path = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
	if (Path == 0xFFFFFFFF)     //�ļ��в�����
	{
		CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
	}

	MessageBox(_T("�ɹ���������"), _T("���ഴ���ɹ�"), MB_ICONINFORMATION | MB_OK);

	VMPEND
}


void CMainDlg::OnDelete()
{
	VMPBEGIN

	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	if(Class.IsEmpty())
	{
		AfxMessageBox(_T("����������!"));
		return;
	}
	else if(!Class.IsEmpty() && Type.IsEmpty())
	{
		// ɾ��Ŀ¼
		DeleteDirectory(_T("Code\\") + Class);
		DeleteDirectory(_T("File\\") + Class);

		// ɾ��ComboBox ���ֵ
		m_Class.DeleteString(m_Class.GetCurSel());
		m_Class.SetCurSel(0);

		// ���ݱ����Ϣ

		// ��ձ༭��
		m_Edit.SetWindowText(_T(""));

		// ���ComboBox
		m_Type.ResetContent();

		CString Class;
		m_Class.GetWindowText(Class);
		CString FilePath = _T("Code\\") + Class;

		CFileFind Finder;
		BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
		while (IsFind)
		{
			IsFind = Finder.FindNextFile();

			if (Finder.IsDots())
				continue;
			if (Finder.IsDirectory())
				m_Type.AddString(Finder.GetFileName());
		}

		// ����ComboBox
		m_Type.SetCurSel(0);


		// ����б�
		m_List.DeleteAllItems();

		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);
		FilePath = _T("Code\\") + Class + _T("\\") + Type;

		IsFind = Finder.FindFile(FilePath + _T("./*.*"));
		while (IsFind)
		{
			IsFind = Finder.FindNextFile();

			if (Finder.IsDots())
				continue;
			if (Finder.IsDirectory())
				continue;
			else
			{
				CString Name = Finder.GetFileName();
				m_List.AddItem(Name.Left(Name.GetLength() - 5));
			}
		}

		// ��Ϣ��ʾ
		MessageBox(_T("ɾ�����!"), _T("����ɾ���ɹ�"), MB_ICONINFORMATION | MB_OK);
	}
	else if(!Class.IsEmpty() && !Type.IsEmpty())
	{
		int code = MessageBox(_T("�Ƿ�ɾ������? ѡ��ֻɾ�������"), _T("ѡ���������"), MB_YESNOCANCEL);

		if(code != IDCANCEL)
		{
			if(code == IDYES)
			{
				// ɾ��Ŀ¼
				DeleteDirectory(_T("Code\\") + Class);
				DeleteDirectory(_T("File\\") + Class);

				// ɾ��ComboBox ���ֵ
				m_Class.DeleteString(m_Class.GetCurSel());
				m_Type.DeleteString(m_Type.GetCurSel());

				// ����ѡ��
				m_Class.SetCurSel(0);

				// ���ݱ����Ϣ

				// ��ձ༭��
				m_Edit.SetWindowText(_T(""));

				// ���ComboBox
				m_Type.ResetContent();

				CString Class;
				m_Class.GetWindowText(Class);
				CString FilePath = _T("Code\\") + Class;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						m_Type.AddString(Finder.GetFileName());
				}

				// ����ComboBox
				m_Type.SetCurSel(0);


				// ����б�
				m_List.DeleteAllItems();

				m_Class.GetWindowText(Class);
				m_Type.GetWindowText(Type);
				FilePath = _T("Code\\") + Class + _T("\\") + Type;

				IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						continue;
					else
					{
						CString Name = Finder.GetFileName();
						m_List.AddItem(Name.Left(Name.GetLength() - 5));
					}
				}

				// ��Ϣ��ʾ
				MessageBox(_T("ɾ�����!"), _T("����ɾ���ɹ�"), MB_ICONINFORMATION | MB_OK);
			}
			else
			{
				// ɾ��Ŀ¼
				DeleteDirectory(_T("Code\\") + Class + _T("\\") + Type);
				DeleteDirectory(_T("File\\") + Class + _T("\\") + Type);

				// ɾ��ComboBox ���ֵ
				m_Type.DeleteString(m_Type.GetCurSel());

				// ����ѡ��
				m_Type.SetCurSel(0);

				// ���ݱ����Ϣ

				// ����б�
				m_List.DeleteAllItems();

				m_Class.GetWindowText(Class);
				m_Type.GetWindowText(Type);
				CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

				CFileFind Finder;
				BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
				while (IsFind)
				{
					IsFind = Finder.FindNextFile();

					if (Finder.IsDots())
						continue;
					if (Finder.IsDirectory())
						continue;
					else
					{
						CString Name = Finder.GetFileName();
						m_List.AddItem(Name.Left(Name.GetLength() - 5));
					}
				}

				// ��ձ༭��
				m_Edit.SetWindowText(_T(""));

				// ��Ϣ��ʾ
				MessageBox(_T("ɾ�����!"), _T("����ɾ���ɹ�"), MB_ICONINFORMATION | MB_OK);
			}
		}
	}

	VMPEND
}


void CMainDlg::OnEdit()
{
	CString Class,Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	CEditDlg dlg;
	dlg.Class = Class;
	dlg.Type  = Type;
	dlg.IsFunction = false;

	if(dlg.DoModal() == IDOK)
	{
		// ���ֻ�޸������
		if(dlg.Class == Class && dlg.Type != Type)
		{
			CString old_Path = _T("Code\\") + Class + _T("\\") + Type;
			CString new_Path = _T("Code\\") + Class + _T("\\") + dlg.Type;

			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				m_Type.SetWindowText(dlg.Type);
				MessageBox(_T("�����޸ĳɹ�!"), _T("�޸ĳɹ�"), MB_ICONINFORMATION | MB_OK);
			}
			else
			{
				AfxMessageBox(_T("�����޸�ʧ��!"));
			}
		}

		// ���ֻ�޸�������
		else if(dlg.Class != Class && dlg.Type == Type)
		{
			CString old_Path = _T("Code\\") + Class;
			CString new_Path = _T("Code\\") + dlg.Class;

			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				m_Class.SetWindowText(dlg.Class);
				MessageBox(_T("�����޸ĳɹ�!"), _T("�޸ĳɹ�"), MB_ICONINFORMATION | MB_OK);
			}
			else
			{
				AfxMessageBox(_T("�����޸�ʧ��!"));
			}
		}

		// ���ֶ�����
		else
		{
			CString old_Class = _T("Code\\") + Class;
			CString new_Class = _T("Code\\") + dlg.Class;

			CString old_Type = _T("Code\\") + dlg.Class + _T("\\") + Type;
			CString new_Type = _T("Code\\") + dlg.Class + _T("\\") + dlg.Type;

			if(MoveFileEx(old_Class, new_Class, MOVEFILE_REPLACE_EXISTING) && MoveFileEx(old_Type, new_Type, MOVEFILE_REPLACE_EXISTING))
			{
				m_Class.SetWindowText(dlg.Class);
				m_Type.SetWindowText(dlg.Type);
				MessageBox(_T("�����޸ĳɹ�!"), _T("�޸ĳɹ�"), MB_ICONINFORMATION | MB_OK);
			}
			else
			{
				AfxMessageBox(_T("�����޸�ʧ��!"));
			}
		}
	}
}


void CMainDlg::OnManager()
{
	DWORD Path = GetFileAttributes(_T("Project"));
	if (Path == 0xFFFFFFFF)     //�ļ��в�����
	{
		CreateDirectory(_T("Project"), NULL);
	}

	CProjectDlg dlg;
	dlg.DoModal();
}


void CMainDlg::OnKillfocusClassCombo()
{
	VMPBEGIN

	// ���Ŀ��
	if (m_hOperate == NULL)
	{
		Type = 3;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND
}


void CMainDlg::OnDropdownClassCombo()
{
	VMPBEGIN

	// ���Ŀ��
	if (m_hOperate == NULL)
	{
		Type = 4;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND
}


void CMainDlg::OnKillfocusTypeCombo()
{
	VMPBEGIN

	// ���Ŀ��
	if (m_hOperate == NULL)
	{
		Type = 5;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND
}


void CMainDlg::OnDropdownTypeCombo()
{
	VMPBEGIN

	// ���Ŀ��
	if (m_hOperate == NULL)
	{
		Type = 6;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}

	VMPEND
}


void CMainDlg::OnEditFunction()
{
	CString Class, Type, Function;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);
	Function = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);

	// �ж�ѡ�з���
	if (!Function.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CEditDlg dlg;
		dlg.Type  = Function;
		dlg.IsFunction = true;

		if(dlg.DoModal() == IDOK)
		{
			CString old_Path = _T("Code\\") + Class + _T("\\") + Type + _T("\\") + Function + _T(".code");
			CString new_Path = _T("Code\\") + Class + _T("\\") + Type + _T("\\") + dlg.Type + _T(".code");

			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				int Row = m_List.GetNextItem(-1, LVIS_SELECTED);
				m_List.SetItemText(Row, 0, dlg.Type);

				//���ø�����ʾ
				m_List.SetFocus();//���ý���  
				m_List.SetItemState(Row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
				m_List.EnsureVisible(Row, FALSE);//���õ�ǰ��ͼ�ɼ� 

				MessageBox(_T("�����޸ĳɹ�!"), _T("�޸ĳɹ�"), MB_ICONINFORMATION | MB_OK);
			}
			else
			{
				AfxMessageBox(_T("�����޸�ʧ��!"));
			}
		}
	}
	else
	{
		AfxMessageBox(_T("��ѡ��Ҫ�༭�ķ���!"));
	}
}


void CMainDlg::OnOK()
{
	VMPBEGIN

	// ���淽��
	if (m_hOperate == NULL)
	{
		Type = 9;
		m_hOperate = AfxBeginThread(Operate, this);
	}

	VMPEND
}


void CMainDlg::OnFont()
{
	VMPBEGIN

	CHARFORMAT cf;
	LOGFONT lf;
	memset(&cf, 0, sizeof(CHARFORMAT));
	memset(&lf, 0, sizeof(LOGFONT)); 

	//�ж��Ƿ�ѡ��������
	BOOL bSelect = (m_Edit.GetSelectionType() != SEL_EMPTY) ? TRUE : FALSE; 
	if (bSelect)
	{
		m_Edit.GetSelectionCharFormat(cf);
	}
	else
	{
		m_Edit.GetDefaultCharFormat(cf);
	}

	//�õ������������
	BOOL bIsBold = cf.dwEffects & CFE_BOLD;
	BOOL bIsItalic = cf.dwEffects & CFE_ITALIC;
	BOOL bIsUnderline = cf.dwEffects & CFE_UNDERLINE;
	BOOL bIsStrickout = cf.dwEffects & CFE_STRIKEOUT;

	//��������
	lf.lfCharSet = cf.bCharSet;
	lf.lfHeight = cf.yHeight/15;
	lf.lfPitchAndFamily = cf.bPitchAndFamily;
	lf.lfItalic = bIsItalic;
	lf.lfWeight = (bIsBold ? FW_BOLD : FW_NORMAL);
	lf.lfUnderline = bIsUnderline;
	lf.lfStrikeOut = bIsStrickout;

	//sprintf_s(lf.lfFaceName, 65535, cf.szFaceName);
	/*asci��strcpy(m_NotifyData.szTip, "��֤ϵͳ�ͻ���");
	unicode��wcscpy(m_NotifyData.szTip, L"��֤ϵͳ�ͻ���");*/

	//wcscpy_s(lf.lfFaceName, cf.szFaceName);
	strcpy_s(lf.lfFaceName, cf.szFaceName);

	CFontDialog dlg(&lf);
	dlg.m_cf.rgbColors = cf.crTextColor;

	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCharFormat(cf);//�����ѡ���������
		if (bSelect)
			m_Edit.SetSelectionCharFormat(cf);    //Ϊѡ���������趨��ѡ����
		else
			m_Edit.SetWordCharFormat(cf);         //Ϊ��Ҫ����������趨����
	}

	VMPEND
}


void CMainDlg::OnNew()
{
	// ��ӷ���
	if(IsNew)
	{
		// ȡ��ѡ��
		m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

		CString Text;
		GetDlgItem(IDC_CODE_RICHEDIT)->GetWindowText(Text);

		if(!Text.IsEmpty())
		{
			// ѯ���Ƿ����
			if( MessageBox(_T("�Ƿ���յ�ǰ����?"), _T("�Ƿ����"), MB_ICONQUESTION | MB_YESNO) == IDYES )
			{
				// �������
				m_Edit.SetWindowText(_T(""));
			}

			this->Type = 10;
			m_hOperate = AfxBeginThread(Operate, this);
		}
		else
		{
			if (m_hOperate == NULL)
			{
				this->Type = 10;
				m_hOperate = AfxBeginThread(Operate, this);
			}
		}
	}
	else
	{
		// �༭����
		OnEditFunction();
	}
}


void CMainDlg::OnClearFormat()
{
	CString source = m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		//CHARFORMAT cf;  
		//m_Edit.GetSelectionCharFormat(cf);  
		//cf.dwMask|=CFM_BOLD;  
		//cf.dwEffects|=CFE_BOLD;//���ô��壬ȡ����cf.dwEffects&=~CFE_BOLD;  
		//cf.dwMask|=CFM_ITALIC;  
		//cf.dwEffects|=CFE_ITALIC;//����б�壬ȡ����cf.dwEffects&=~CFE_ITALIC;  
		//cf.dwMask|=CFM_UNDERLINE;  
		//cf.dwEffects|=CFE_UNDERLINE;//����б�壬ȡ����cf.dwEffects&=~CFE_UNDERLINE;  
		//cf.dwMask|=CFM_COLOR;  
		//cf.crTextColor = RGB(0,0,0);//������ɫ  
		//cf.dwMask|=CFM_SIZE;  
		//cf.yHeight =200;//���ø߶�  
		//cf.dwMask|=CFM_FACE;  
		//strcpy_s(cf.szFaceName ,_T("����"));//��������  
		//m_Edit.SetSelectionCharFormat(cf);  

		CHARFORMAT cf;
		m_Edit.GetSelectionCharFormat(cf);
		cf.dwMask|=CFM_BOLD;
		cf.dwEffects&=~CFE_BOLD;      //���ô��壬ȡ����cf.dwEffects&=~CFE_BOLD;
		cf.dwMask|=CFM_ITALIC;
		cf.dwEffects&=~CFE_ITALIC;    //����б�壬ȡ����cf.dwEffects&=~CFE_ITALIC;
		cf.dwMask|=CFM_UNDERLINE;
		cf.dwEffects&=~CFE_UNDERLINE; //����б�壬ȡ����cf.dwEffects&=~CFE_UNDERLINE;
		cf.dwMask|=CFM_COLOR;
		cf.crTextColor = RGB(0,0,0);  //������ɫ
		cf.dwMask|=CFM_SIZE;
		cf.yHeight = 14 * 14;         //���ø߶�
		cf.dwMask|=CFM_FACE;
		strcpy_s(cf.szFaceName ,_T("����"));//��������
		m_Edit.SetSelectionCharFormat(cf);
	}
	else
	{
		AfxMessageBox(_T("û����Ҫ�����ʽ������!"));
	}
}


void CMainDlg::OnDirectory()
{
	VMPBEGIN

	CString Class, Type, Dir;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	if(Class.IsEmpty())
	{
		AfxMessageBox(_T("����������!"));
		return;
	}
	else if(Type.IsEmpty())
	{
		AfxMessageBox(_T("���������!"));
		return;
	}

	CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
	if (!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		DWORD Path = GetFileAttributes(_T("File\\") + Class);
		if (Path == 0xFFFFFFFF)     //�ļ��в�����
		{
			CreateDirectory(_T("File\\") + Class, NULL);
		}

		Path = GetFileAttributes(_T("File\\") + Class + _T("\\") + Type);
		if (Path == 0xFFFFFFFF)     //�ļ��в�����
		{
			CreateDirectory(_T("File\\") + Class + _T("\\") + Type, NULL);
		}

		Dir  = _T("File\\") + Class + _T("\\") + Type + _T("\\") + Name;
		Path = GetFileAttributes(Dir);

		if (Path == 0xFFFFFFFF)     //�ļ��в�����
		{
			CreateDirectory(_T("File\\") + Class + _T("\\") + Type + _T("\\") + Name, NULL);
		}

		ShellExecute(NULL, _T("open"), Dir, NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		AfxMessageBox(_T("��ѡ�񷽷�����ִ�б�����!"));
	}

	VMPEND
}


void CMainDlg::OnRemove()
{
	VMPBEGIN

	int Count = m_List.GetSelectedCount();
	CString Path = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);

	if (!Path.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		if(Count == 1)
		{
			if (!Path.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
			{
				if(MessageBox(_T("�Ƿ�Ҫ�Ƴ���ǰ����? "),  _T("�����Ƴ�ȷ��"), MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					// �Ƴ�����
					if (m_hOperate == NULL)
					{
						Type = 11;
						m_hOperate = AfxBeginThread(Operate, this);
					}
					//CString Class, Type;
					//m_Class.GetWindowText(Class);
					//m_Type.GetWindowText(Type);

					//// ɾ���ļ�
					//DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".code"));

					//// �Ƴ��б�
					//m_List.DeleteItem(m_List.GetNextItem(-1, LVIS_SELECTED));

					//// ��ձ༭��
					//m_Edit.SetWindowText(_T(""));

					//// ɾ������
					//DeleteDirectory(_T("File\\") + Class + _T("\\") + Type + _T("\\") + Path);
				}
			}
			else
			{
				AfxMessageBox(_T("��ѡ��Ҫ�Ƴ��ķ���!"));
			}
		}
		else if(Count > 1)
		{
			if(MessageBox(_T("�Ƿ�Ҫ�Ƴ�ѡ�еķ���? "), _T("�����Ƴ�ȷ��"), MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				// �Ƴ�����
				if (m_hOperate == NULL)
				{
					Type = 11;
					m_hOperate = AfxBeginThread(Operate, this);
				}
				//int nItem = -1;
				//POSITION pos;

				//while (pos = m_List.GetFirstSelectedItemPosition())
				//{
				//	nItem = -1;
				//	nItem = m_List.GetNextSelectedItem(pos);
				//	if (nItem >= 0 && m_List.GetSelectedCount() > 0)
				//	{
				//		CString Path = m_List.GetItemText(nItem, 0);

				//		CString Class, Type;
				//		m_Class.GetWindowText(Class);
				//		m_Type.GetWindowText(Type);

				//		// ɾ���ļ�
				//		DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".code"));

				//		// �Ƴ��б�
				//		m_List.DeleteItem(nItem);

				//		// ɾ������
				//		DeleteDirectory(_T("File\\") + Class + _T("\\") + Type + _T("\\") + Path);
				//	}
				//}

				//for (int i = 0; i < Count; i++)
				//{
				//	int Row = m_List.GetNextItem(i - 1, LVIS_SELECTED);
				//	CString Path = m_List.GetItemText(Row, 0);

				//	CString Class, Type;
				//	m_Class.GetWindowText(Class);
				//	m_Type.GetWindowText(Type);

				//	DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".rtf"));
				//	//m_List.DeleteItem(m_List.GetNextItem(-1, LVIS_SELECTED));
				//}
			}
		}
		else
		{
			AfxMessageBox(_T("��ѡ��Ҫ�Ƴ��ķ���!"));
		}
	}
	else
	{
		AfxMessageBox(_T("��ѡ��Ҫ�Ƴ��ķ���!"));
	}

	VMPEND
}


void CMainDlg::OnSynchronization()
{
	CLoginDlg dlg;
	dlg.DoModal();
}


void CMainDlg::OnSetting()
{
	if(!m_Setting->IsWindowVisible())
	{
		m_Setting->CenterWindow();
		m_Setting->SetForegroundWindow();
		m_Setting->OnReadSetting();
		m_Setting->ShowWindow(SW_SHOW);
	}
	else
	{
		m_Setting->ShowWindow(SW_HIDE);
	}
}


void CMainDlg::OnCancel()
{
	// ����������
	KillTimer(1);

	// �����̴߳���
	DWORD Code = 0;
	if(GetExitCodeThread(m_hOperate, &Code))
	{
		if(Code == STILL_ACTIVE)
		{
			TerminateThread(m_hOperate, 0);
			CloseHandle(m_hOperate);
		}
	}
	if(GetExitCodeThread(m_hUpDate, &Code))
	{
		if(Code == STILL_ACTIVE)
		{
			TerminateThread(m_hUpDate, 0);
			CloseHandle(m_hUpDate);
		}
	}

	// �Ƿ��Զ���
	if( GetPrivateProfileInt(_T("Setting"), _T("Open"), 0, _T("./Setting.ini")) == 1)
	{
		CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0), Class, Type;
		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);

		// �ж��Ƿ�ѡ����Դ��
		if(!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
		{
			// ���������ļ�
			::WritePrivateProfileString(_T("File"), _T("Class"), Class, _T("./Setting.ini"));
			::WritePrivateProfileString(_T("File"), _T("Type"),  Type,  _T("./Setting.ini"));
			::WritePrivateProfileString(_T("File"), _T("Name"),  Name,  _T("./Setting.ini"));
		}
	}

	// �Ƿ��Զ�ͬ��
	if( GetPrivateProfileInt(_T("Setting"), _T("Synchronize"), 0, _T("./Setting.ini")) == 1)
	{
		CString UserName, Password;
		if (!theApp.m_Sql.SelectData(_T("�û��˻�"), UserName, 1, _T("Name Is Not Null")))
		{
			AfxMessageBox(_T("�޷���ȡ�û���!�Զ�ͬ��ʧ��!"));
		}

		if (!theApp.m_Sql.SelectData(_T("�û��˻�"), Password, 2, _T("Password Is Not Null")))
		{
			AfxMessageBox(_T("�޷���ȡ�û�����!�Զ�ͬ��ʧ��!"));
		}

		if(UserName.IsEmpty() || Password.IsEmpty())
		{
			AfxMessageBox(_T("û�н��й��Զ���½!�Զ�ͬ��ʧ��!"));
			CDialogEx::OnCancel();
			return;
		}

		try
		{
			CString Parameter;
			Parameter.Format(_T("username=%s&password=%s"), UserName, Password);
			BOOL IsSuccess = FALSE;

			CString RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/login"), 80, Parameter, IsSuccess);
			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				AfxMessageBox(_T("�޷����ӵ�������, �������硣�Զ�ͬ��ʧ��!"));
				return;
			}
			else
			{
				if (IsSuccess)
				{
					if( RecvData == RecvData.SpanIncluding( _T("0123456789") ) )
					{
						CSynchronizeDlg * dlg = new CSynchronizeDlg;
						dlg->UserId   = RecvData;
						dlg->UserName = UserName;
						dlg->IsAutoSynchronize=true;

						dlg->Create(IDD_SYNCHRONIZE_DIALOG, this);
						dlg->OnAutoSynchronize();

						dlg->DestroyWindow();
						delete dlg;
					}
					else
					{
						AfxMessageBox(_T("��¼ʧ�ܣ��û�������������Զ�ͬ��ʧ��!"));
					}
				}
				else
				{
					AfxMessageBox(_T("�޷����ӵ�������, �������硣�Զ�ͬ��ʧ��!"));
				}
			}
		}
		catch (...)
		{
			AfxMessageBox(_T("�������쳣��λ��CMainDlg��OnCancel������"));
		}
	}

	// �Ƿ񱣴洰��λ��
	if( GetPrivateProfileInt(_T("Setting"), _T("Position"), 0, _T("./Setting.ini")) == 1)
	{
		// �õ�����λ��
		CRect rc;
		CRect rect;
		GetWindowRect(&rect);  //�����С

		CString Position;
		Position.Format("%d,%d,%d,%d,",rect.left,rect.top,rc.Width(),rc.Height());

		// ���������ļ�
		::WritePrivateProfileString(_T("Position"), _T("WindowPosition"), Position, _T("./Setting.ini"));
	}

	CDialogEx::OnCancel();
}
