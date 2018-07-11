// MainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "MainDlg.h"
#include "afxdialogex.h"

#include "SearchDlg.h"
#include "EditDlg.h"
#include "LoginDlg.h"
#include "ProjectDlg.h"


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

	// �ж��Ƿ��һ������
	if(IsNew)
	{
		// ��ʾ�û�ʹ��˵��
		m_Edit.StreamInFromResource(_T("ʹ��˵��.rtf"), _T("SF_RTF"));
	}

	// Class
	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			m_Class.AddString(Finder.GetFileName());
	}

	m_Class.SetCurSel(0);

	// Type
	CString Class, Type;
	m_Class.GetWindowText(Class);
	FilePath = _T("Code\\") + Class;

	IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			m_Type.AddString(Finder.GetFileName());
	}

	m_Type.SetCurSel(0);


	// Code
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
			m_List.AddItem(Name.Left(Name.GetLength() - 4));
		}
	}

	// ���öԻ���
	m_Setting->Create(IDD_SETTING_DIALOG, this);

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
		int nIndex = m_Class.FindStringExact(0, Class);
		if(nIndex != CB_ERR)
		{
			m_Class.SetCurSel(nIndex);
			OnKillfocusClassCombo();
		}
		else
		{
			AfxMessageBox(_T("�Ҳ���") + Class + _T("����"));
		}

		nIndex = m_Type.FindStringExact(0, Type);
		if(nIndex != CB_ERR)
		{
			m_Type.SetCurSel(nIndex);
			OnKillfocusTypeCombo();
		}
		else
		{
			AfxMessageBox(_T("�Ҳ���") + Type + _T("���"));
		}

		int Count = m_List.GetItemCount();
		for(int i=0; i<Count; i++)
		{
			CString Function = m_List.GetItemText(i, 0);

			if(Function == Name)
			{
				//���ø�����ʾ  
				m_List.SetFocus();//���ý���  
				m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
				m_List.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 
				break;
			}
		}
	}

	//vmprotect ��ǽ�����.
	VMPEND

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	VMPBEGIN

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

		// Ctrl + W (�༭����)
		if( (nKeyCode == _T('W') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnEditFunction();
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
					CString Class, Type;
					m_Class.GetWindowText(Class);
					m_Type.GetWindowText(Type);
					CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

					ShellExecute(NULL, _T("open"), FilePath + Name + _T(".rtf"), NULL, NULL, SW_SHOWNORMAL);

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
	}

	return TRUE;
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
	// ��ʾ�û�ʹ��˵��
	m_Edit.StreamInFromResource(_T("ʹ��˵��.rtf"), _T("SF_RTF"));
}


void CMainDlg::OnChangeCodeRichedit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	VMPBEGIN

	// ���������Զ�����
	if ( GetPrivateProfileInt(_T("Setting"), _T("Save"), 0, _T("./Setting.ini")) == 1)
	{
		CString Path = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
		CString Class, Type;
		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);

		// ���������Ϣ������
		if(Class.IsEmpty() || Type.IsEmpty())
		{
			return;
		}
		else
		{
			// ����Դ��
			if(!Path.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
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
				m_Edit.StreamOutToFile(File + Path + _T(".rtf"), _T("SF_RTF"));
			}
		}
	}

	VMPEND
}


void CMainDlg::OnClickCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	//���ø�����ʾ  
	m_List.SetFocus();//���ý���  
	m_List.SetItemState( m_List.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
	m_List.EnsureVisible(m_List.GetNextItem(-1, LVIS_SELECTED), FALSE);//���õ�ǰ��ͼ�ɼ� 

	CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
	if (!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString Class, Type;
		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);
		CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

		m_Edit.StreamInFromResource(FilePath + Name + _T(".rtf"), _T("SF_RTF"));
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

		//std::map<CString, MetaData>::iterator iter;  
		//if ((iter = m_idBufferMap.find(Function)) != m_idBufferMap.end())  
		//{
		//	int currentIndex = iter->second.m_index;  

		//	//���ø�����ʾ  
		//	m_List.SetFocus();//���ý���  
		//	m_List.SetItemState(currentIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
		//	m_List.EnsureVisible(currentIndex, FALSE);//���õ�ǰ��ͼ�ɼ�  
		//}
		//else  
		//{  
		//	AfxMessageBox("δ�ҵ�");  
		//}  
	}

	*pResult = 0;

	VMPEND
}


void CMainDlg::OnDblclkCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
	if (!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString Class, Type;
		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);
		CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

		ShellExecute(NULL, _T("open"), FilePath + Name + _T(".rtf"), NULL, NULL, SW_SHOWNORMAL);
	}

	VMPEND

	*pResult = 0;
}


void CMainDlg::OnItemchangedCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN
	//OnClickCodeList(pNMHDR, pResult);
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

	MessageBox(_T("�ɹ���������"));

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
		OnKillfocusClassCombo();
		OnKillfocusTypeCombo();

		// ��ձ༭��
		m_Edit.SetWindowText(_T(""));

		// ��Ϣ��ʾ
		MessageBox(_T("ɾ�����!"));
	}
	else if(!Class.IsEmpty() && !Type.IsEmpty())
	{
		int code = MessageBox(_T("�Ƿ�ɾ������? ѡ��ֻɾ�������"), NULL, MB_YESNOCANCEL);

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
				OnKillfocusClassCombo();
				OnKillfocusTypeCombo();

				// ��ձ༭��
				m_Edit.SetWindowText(_T(""));

				// ��Ϣ��ʾ
				MessageBox(_T("ɾ�����!"));
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
				OnKillfocusTypeCombo();

				// ��ձ༭��
				m_Edit.SetWindowText(_T(""));

				// ��Ϣ��ʾ
				MessageBox(_T("ɾ�����!"));
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
				MessageBox(_T("�����޸ĳɹ�!"));
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
				MessageBox(_T("�����޸ĳɹ�!"));
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
				MessageBox(_T("�����޸ĳɹ�!"));
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

	VMPEND
}


void CMainDlg::OnDropdownClassCombo()
{
	VMPBEGIN

	// ��ձ༭��
	m_Edit.SetWindowText(_T(""));

	// ����б�
	m_List.DeleteAllItems();

	// ����ComboBox
	m_Class.ResetContent();
	m_Type.ResetContent();

	CString FilePath = _T("Code");
	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			m_Class.AddString(Finder.GetFileName());
	}

	m_Class.SetCurSel(0);

	VMPEND
}


void CMainDlg::OnKillfocusTypeCombo()
{
	VMPBEGIN

	// ��ձ༭��
	//m_Edit.SetWindowText(_T(""));

	// ����б�
	m_List.DeleteAllItems();

	CString Class, Type;
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
			m_List.AddItem(Name.Left(Name.GetLength() - 4));
		}
	}

	VMPEND
}


void CMainDlg::OnDropdownTypeCombo()
{
	VMPBEGIN

	// ��ձ༭��
	m_Edit.SetWindowText(_T(""));

	// ����б�
	m_List.DeleteAllItems();

	// ����ComboBox
	m_Type.ResetContent();
	
	CString Class, Type;
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

	m_Type.SetCurSel(0);

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
			CString old_Path = _T("Code\\") + Class + _T("\\") + Type + _T("\\") + Function + _T(".rtf");
			CString new_Path = _T("Code\\") + Class + _T("\\") + Type + _T("\\") + dlg.Type + _T(".rtf");

			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				int Row = m_List.GetNextItem(-1, LVIS_SELECTED);
				m_List.SetItemText(Row, 0, dlg.Type);

				//���ø�����ʾ
				m_List.SetFocus();//���ý���  
				m_List.SetItemState(Row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
				m_List.EnsureVisible(Row, FALSE);//���õ�ǰ��ͼ�ɼ� 

				MessageBox(_T("�����޸ĳɹ�!"));
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

	CString Path = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
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
	if(!Path.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		m_Edit.StreamOutToFile(File + Path + _T(".rtf"), _T("SF_RTF"));
		MessageBox(_T("����ɹ�!"));

		//���ø�����ʾ  
		m_List.SetFocus();//���ý���  
		m_List.SetItemState( m_List.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
		m_List.EnsureVisible(m_List.GetNextItem(-1, LVIS_SELECTED), FALSE);//���õ�ǰ��ͼ�ɼ� 
	}
	else
	{
		TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
		GetModuleFileName(NULL,exeFullPath,MAX_PATH);//�õ�����ģ�����ƣ�ȫ·��
		CString Path(exeFullPath), Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1), Dir = Path.Left(Path.GetLength() - Name.GetLength()) + File;

		CFileDialog FileDlg(TRUE, _T("rtf"), _T("��������"), OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, _T("�����ļ�(*.rtf)|*.rtf;||"), this);
		FileDlg.m_ofn.lpstrInitialDir = Dir;
		if(FileDlg.DoModal() == IDOK)
		{
			CString strFile = FileDlg.GetPathName(); //�ļ������ھ��Զ�����
			m_Edit.StreamOutToFile(strFile, _T("SF_RTF"));
			MessageBox(_T("����ɹ�!"));

			BOOL  isFind = FALSE;
			CString Name = FileDlg.GetFileName();

			int Count = m_List.GetItemCount();
			for(int i=0; i<Count; i++)
			{
				CString Function = m_List.GetItemText(i, 0);

				if(Function == Name.Left(Name.GetLength() - 4))
				{
					//��־
					isFind = TRUE;

					//���ø�����ʾ  
					m_List.SetFocus();//���ý���  
					m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
					m_List.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 
					break;
				}
			}

			if(!isFind)
			{
				// �����б�
				int i = m_List.AddItem(Name.Left(Name.GetLength() - 4));

				//���ø�����ʾ  
				m_List.SetFocus();//���ý���  
				m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
				m_List.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 
			}
		}
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
	// ȡ��ѡ��
	m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

	// �������
	m_Edit.SetWindowText(_T(""));

	// ��ӷ���
	OnOK();
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
				if(MessageBox(_T("�Ƿ�Ҫ�Ƴ���ǰ����? "), NULL, MB_YESNO) == IDYES)
				{
					CString Class, Type;
					m_Class.GetWindowText(Class);
					m_Type.GetWindowText(Type);

					DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".rtf"));

					m_List.DeleteItem(m_List.GetNextItem(-1, LVIS_SELECTED));
					m_Edit.SetWindowText(_T(""));

					MessageBox(_T("�����Ƴ����!"));
				}
			}
			else
			{
				AfxMessageBox(_T("��ѡ��Ҫ�Ƴ��ķ���!"));
			}
		}
		else if(Count > 1)
		{
			if(MessageBox(_T("�Ƿ�Ҫ�Ƴ�ѡ�еķ���? "), NULL, MB_YESNO) == IDYES)
			{
				int nItem = -1;
				POSITION pos;
				while (pos = m_List.GetFirstSelectedItemPosition())
				{
					nItem = -1;
					nItem = m_List.GetNextSelectedItem(pos);
					if (nItem >= 0 && m_List.GetSelectedCount() > 0)
					{
						CString Path = m_List.GetItemText(nItem, 0);

						CString Class, Type;
						m_Class.GetWindowText(Class);
						m_Type.GetWindowText(Type);

						DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".rtf"));

						m_List.DeleteItem(nItem);
					}
				}

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

				m_Edit.SetWindowText(_T(""));
				OnKillfocusTypeCombo();

				MessageBox(_T("�����Ƴ����!"));
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
	if(dlg.DoModal() == IDOK)
	{

	}
}


void CMainDlg::OnSetting()
{
	m_Setting->CenterWindow();
	m_Setting->SetForegroundWindow();
	m_Setting->OnReadSetting();
	m_Setting->ShowWindow(SW_SHOW);
}


void CMainDlg::OnCancel()
{
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

	CDialogEx::OnCancel();
}
