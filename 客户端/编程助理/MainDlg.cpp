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
#include "ParagraphDlg.h"


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
	Type = TabCount = 0;
	m_hOperate = m_hUpDate = NULL;

	IsNew  = TRUE;
	IsEdit = IsSave = IsCancel = FALSE;

	CurClass = CurType = _T("");

	// ��������
	m_hCursor = AfxGetApp()->LoadCursor(IDC_TARGET_CURSOR);
	m_pAssisDlg = new CAssistantDlg;
	m_pAssisDlg->IsCreated = FALSE;
	m_preWnd = NULL;
	m_curWnd = NULL;
	m_bSnap = FALSE;

	IsExit = TRUE;
}


CMainDlg::~CMainDlg()
{
	m_Setting->DestroyWindow();
	delete m_Setting;

	if(m_pAssisDlg != NULL)
	{
		m_pAssisDlg->DestroyWindow();
		delete m_pAssisDlg;
	}

	for(int i=0; i<TabCount; i++)
	{
		m_pDocument[i]->DestroyWindow();
		delete m_pDocument[i];
	}
}


void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CODE_LIST, m_List);
	DDX_Control(pDX, IDC_CLASS_COMBO, m_Class);
	DDX_Control(pDX, IDC_TYPE_COMBO, m_Type);
	DDX_Control(pDX, IDC_DOCUMENT_TAB, m_Tab);
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
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CODE_LIST, &CMainDlg::OnItemchangedCodeList)

	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CMainDlg::OnEdit)
	ON_BN_CLICKED(IDC_SYNCHRONIZATION_BUTTON, &CMainDlg::OnSynchronization)
	ON_BN_CLICKED(IDC_SETTING_BUTTON, &CMainDlg::OnSetting)

	ON_MESSAGE(WM_CHILDMESSAGE, OnMessageChild)
	ON_BN_CLICKED(IDC_MANAGER_BUTTON, &CMainDlg::OnManager)

	ON_COMMAND(100, &CMainDlg::Complete)
	ON_COMMAND(101, &CMainDlg::OnError)
	ON_COMMAND(102, &CMainDlg::OnCancel)
	ON_COMMAND(103, &CMainDlg::OnSaveCode)
	ON_COMMAND(104, &CMainDlg::OnNewCode)
	ON_COMMAND(105, &CMainDlg::SetPic)
	ON_COMMAND(106, &CMainDlg::OnNewLabel)
	ON_COMMAND(107, &CMainDlg::SaveImage)

	ON_COMMAND(IDM_OPEN,   &CMainDlg::OnOpen)
	ON_COMMAND(IDM_SAVE,   &CMainDlg::OnOK)
	ON_COMMAND(IDM_NEW ,   &CMainDlg::OnNew)
	ON_COMMAND(IDM_EDIT,   &CMainDlg::OnEditFunction)
	ON_COMMAND(IDM_DELETE, &CMainDlg::OnRemove)
	ON_COMMAND(IDM_DIRECTORY, &CMainDlg::OnDirectory)

	ON_COMMAND(IDM_COPY,   &CMainDlg::OnCopy)
	ON_COMMAND(IDM_PASTE,  &CMainDlg::OnPaste)
	ON_COMMAND(IDM_CUT,    &CMainDlg::OnCut)
	ON_COMMAND(IDM_UNDO,   &CMainDlg::OnUndo)
	ON_COMMAND(IDM_REDO,   &CMainDlg::OnRedo)
	ON_COMMAND(IDM_CLEAR,  &CMainDlg::OnClearFormat)

	ON_COMMAND(IDM_IMAGE,   &CMainDlg::OnImage)
	ON_COMMAND(IDM_CAPTURE, &CMainDlg::OnScreenCapture)
	ON_COMMAND(IDM_SAVEIMG, &CMainDlg::SavePic)
	
	ON_COMMAND(IDM_FONT,   &CMainDlg::OnFont)

	ON_COMMAND(IDM_PARAGRAPH,   &CMainDlg::OnParagraph)
	ON_COMMAND(IDM_CENTER, &CMainDlg::OnCenter)
	ON_COMMAND(IDM_LEFT,   &CMainDlg::OnLeft)
	ON_COMMAND(IDM_RIGHT,  &CMainDlg::OnRight)

	ON_COMMAND(IDM_NONE,        &CMainDlg::OnNone)
	ON_COMMAND(IDM_SYMBOL,      &CMainDlg::OnSymbol)
	ON_COMMAND(IDM_NUMBER,      &CMainDlg::OnNumber)
	ON_COMMAND(IDM_LOWER_CASE,  &CMainDlg::OnLowerCase)
	ON_COMMAND(IDM_UPPER_CASE,  &CMainDlg::OnUpperCase)
	ON_COMMAND(IDM_LOWER_ROME,  &CMainDlg::OnLowerRome)
	ON_COMMAND(IDM_UPPER_ROME,  &CMainDlg::OnUpperRome)
	
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_STN_CLICKED(IDC_ICON_STATIC, &CMainDlg::OnIcon)
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
		}
	}

	// ���öԻ���
	m_Setting->Create(IDD_SETTING_DIALOG, this);
	m_Setting->ShowWindow(SW_HIDE);

	// ����ļ���Ŀ��
	if (m_hOperate == NULL)
	{
		Type = 0;
		m_hOperate = AfxBeginThread(Operate, this);
	}

	// ������
	SetWindowText(_T("��Ӱ��������� ") + theApp.GetApplicationVersion());
	OnCheck();

	// ��ʾ��
	m_toolTips.Create(this, TTS_ALWAYSTIP|WS_POPUP);
	m_toolTips.Activate(TRUE);

	// ��ʾ����
	m_toolTips.AddTool(GetDlgItem(IDC_CLASS_COMBO),            _T("ѡ������뷽�������͡�"));
	m_toolTips.AddTool(GetDlgItem(IDC_TYPE_COMBO),             _T("ѡ������뷽�������"));
	m_toolTips.AddTool(GetDlgItem(IDC_SAVE_BUTTON),            _T("������������������"));
	
	m_toolTips.AddTool(GetDlgItem(IDC_DELETE_BUTTON),          _T("ɾ����ǰѡ�е��������\n��ѡ������ɾ����ǰ���������\nѡ���ֻɾ�����\nȡ�����޲�����"));
	m_toolTips.AddTool(GetDlgItem(IDC_EDIT_BUTTON),            _T("�༭��ǰѡ�����������"));
	m_toolTips.AddTool(GetDlgItem(IDC_MANAGER_BUTTON),         _T("�򿪱��ص���Ŀ����ϵͳ��"));

	m_toolTips.AddTool(GetDlgItem(IDC_ICON_STATIC),            _T("ѡ����ٱ��������Ŀ�괰�ڡ�\nѡ�д��ں����������С����\n����Atl+����������ĸ������������"));
	m_toolTips.AddTool(GetDlgItem(IDC_CODE_LIST),              _T("��ʾ��ǰ�����µķ�����\n��������Ҽ�������Ŀ�귽����"));
	m_toolTips.AddTool(GetDlgItem(IDC_DOCUMENT_TAB),           _T("����ͬ�ķ�����ʾ�ڲ�ͬ�ı�ǩ�\n�����������л���ǩ��\n�Ҽ��������߰���Esc�رյ�ǰѡ�б�ǩ��\n�ڷ����ﵥ���Ҽ��ɵ����Ҽ��˵���"));
	m_toolTips.AddTool(GetDlgItem(IDOK),                       _T("���浱ǰ������\n�·����������Ϊ��\n��ݼ�: Ctrl + S��"));

	m_toolTips.AddTool(GetDlgItem(IDC_NEW_BUTTON),             _T("���±�ǩ���½�һ��������\n��ݼ�: Ctrl + N��"));
	m_toolTips.AddTool(GetDlgItem(IDC_FONT_BUTTON),            _T("���õ�ǰ���������塣\n��ݼ�: Ctrl + F��"));
	m_toolTips.AddTool(GetDlgItem(IDC_DIRECTORY_BUTTON),       _T("�򿪵�ǰ�����������ļ�Ŀ¼��\n��ݼ�: Ctrl + D��"));
	m_toolTips.AddTool(GetDlgItem(IDC_REMOVE_BUTTON),          _T("�Ƴ���ǰ�ķ�����\n�Ƴ����޷��ָ���\n��ݼ�: Ctrl + R��"));
	m_toolTips.AddTool(GetDlgItem(IDC_SYNCHRONIZATION_BUTTON), _T("���������ƶ��˻���½�����ƶˡ�\n��ݼ�: Ctrl + Y��"));
	m_toolTips.AddTool(GetDlgItem(IDC_SETTING_BUTTON),         _T("���������ò��޸��������á�\n��ݼ�: Ctrl + K��"));

	//������ɫ
	m_toolTips.SetTipTextColor(RGB(0,0,255));

	//���ָ���ú���ʾ��ʾ������
	m_toolTips.SetDelayTime(TTDT_INITIAL, 10); 

	//��걣��ָ����ʾ��ʾ��ã�����
	m_toolTips.SetDelayTime(TTDT_AUTOPOP, 9000000);

	//�趨��ʾ��ȣ����������Զ�����
	m_toolTips.SetMaxTipWidth(300);

	//vmprotect ��ǽ�����.
	VMPEND

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	VMPBEGIN

	// ������ʾ
	if(GetPrivateProfileInt(_T("Setting"), _T("Tip"), 0, _T("./Setting.ini")) == 1)
		m_toolTips.RelayEvent(pMsg); // ������Ϣ��Ӧ

	// ������Ϣ����
	UINT  nKeyCode = pMsg->wParam; // virtual key code of the key pressed
	if (pMsg->message == WM_KEYDOWN)
	{
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
		if ( (nKeyCode == _T('T') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnScreenCapture();
			return true;
		}

		// Ctrl + O (�򿪷���)
		if ( (nKeyCode == _T('O') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnOpen();
			return true;
		}

		// Ctrl + P (��������)
		if ( (nKeyCode == _T('P') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnParagraph();
			return true;
		}

		// Ctrl + <- (�������)
		if ( (nKeyCode == VK_LEFT && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnLeft();
			return true;
		}

		// Ctrl + -> (�������)
		if ( (nKeyCode == VK_RIGHT && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnRight();
			return true;
		}

		// Ctrl + M (�������)
		if ( (nKeyCode == _T('M') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnCenter();
			return true;
		}

		// Ctrl + L (����ѡ��ͼƬ)
		if ( (nKeyCode == _T('L') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			SavePic();
			return true;
		}

		// Ctrl + Y (�ƶ�ͬ��)
		if ( (nKeyCode == _T('Y') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnSynchronization();
			return true;
		}

		// Ctrl + K (��������)
		if ( (nKeyCode == _T('K') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnSetting();
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


// ��������
void CMainDlg::ClearScreen()
{
	HDC hScreenDC;

	//������Ļ�豸������
	hScreenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	//������ǰ���ڲ�ΪNULL ������ڴ����������Ƶľ��ο�
	HDC hMemDC;

	//��������Ļ���ݵ��ڴ��豸��������	
	hMemDC = CreateCompatibleDC(hScreenDC);	

	//����ǰ����Ĵ�������λͼѡ���豸��������
	SelectObject(hMemDC, m_rectOfWndBmp);

	//����ǰ����Ĵ�������λͼ��������Ļ
	BitBlt(hScreenDC, m_rectOfWnd.left, m_rectOfWnd.top, m_rectOfWnd.right - m_rectOfWnd.left, 
			m_rectOfWnd.bottom - m_rectOfWnd.top, hMemDC, 0, 0, SRCCOPY);		

	DeleteObject(m_rectOfWndBmp);
	m_rectOfWndBmp = NULL;
	DeleteDC(hMemDC);
	DeleteDC(hScreenDC);
}


void CMainDlg::SnapWindow(POINT point)
{
	ClientToScreen(&point);
	HDC hScreenDC;

	//�õ����λ�õĴ��ھ��
	m_curWnd = ::WindowFromPoint(point);

	if (m_curWnd == m_preWnd || m_curWnd == m_hWnd || ::GetParent(m_curWnd) == m_curWnd)
		return;

	if (m_rectOfWndBmp != NULL)
		ClearScreen();

	//�õ���ǰ���ھ�������
	::GetWindowRect(m_curWnd, &m_rectOfWnd);
	if (m_rectOfWnd.left < 0)
		m_rectOfWnd.left = 0;
	if (m_rectOfWnd.top <0)
		m_rectOfWnd.top = 0;

	//���浱ǰ���ھ������������
	m_rectOfWndBmp = CopyScreenToBitmap(&m_rectOfWnd);
	m_preWnd = m_curWnd;
	CBrush brush;

	//������ɫ��ˢ
	brush.CreateSolidBrush(RGB(255, 0, 0));

	//������Ļ�豸������
	hScreenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	//����Ļ����λ�û��ƾ���
	FrameRect(hScreenDC, &m_rectOfWnd, (HBRUSH)brush.m_hObject);
	brush.DeleteObject();
	DeleteDC(hScreenDC);
}


void CMainDlg::OnIcon()
{
	SetCursor(m_hCursor);
	SetCapture();
	m_bSnap = TRUE;
}


void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMainDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bSnap)
	{
		ReleaseCapture();
		ClearScreen();
		OnCodeAssistant();

		m_preWnd = NULL;
		m_curWnd = NULL;
		m_bSnap = FALSE;
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bSnap)
		SnapWindow(point);

	//CDialogEx::OnMouseMove(nFlags, point);
}


void CMainDlg::OnCodeAssistant()
{
	// �ҵ����ϲ�ĸ�����
	HWND m_hWnd = ::GetParent(m_curWnd);

	if(IsWindow(m_hWnd))
	{
		if(m_hWnd == AfxGetApp()->GetMainWnd()->GetSafeHwnd() || m_hWnd == m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->GetSafeHwnd() || m_hWnd == theApp.m_pMainWnd->GetParent()->GetSafeHwnd())
		{
			AfxMessageBox(_T("����ѡ�����������ΪĿ��!"));
			return;
		}

		if(m_pAssisDlg->TargetWnd == m_hWnd)
		{
			AfxMessageBox(_T("Ŀ����ѡ��, �����ٴ�ѡ����ͬĿ��!"));
			return;
		}

		// ��С��������
		ShowWindow(SW_MINIMIZE);

		// �ͷ���Դ
		if(m_pAssisDlg != NULL)
		{
			m_pAssisDlg->DestroyWindow();
			m_pAssisDlg->IsCreated = FALSE;
		}

		// ��Ŀ�괰����Ϊ�����󴴽�����Ի���
		m_pAssisDlg->Create(IDD_ASSISTANT_DIALOG, FromHandle(m_hWnd));
		m_pAssisDlg->CenterWindow();
		m_pAssisDlg->IsCreated = TRUE;
		m_pAssisDlg->TargetWnd = m_hWnd;
		m_pAssisDlg->FileClass = CurClass;
		m_pAssisDlg->FileType  = CurType;
		m_pAssisDlg->FilePath  = _T("Code\\") + CurClass + _T("\\") + CurType;
		m_pAssisDlg->OnSetCode();
		m_pAssisDlg->OnOversee();
	}
	else
	{
		// ��ֵΪ�Ӵ��ڶ���
		m_hWnd = m_curWnd;
		if(m_hWnd == AfxGetApp()->GetMainWnd()->GetSafeHwnd() || m_hWnd == m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->GetSafeHwnd())
		{
			AfxMessageBox(_T("����ѡ�����������ΪĿ��!"));
			return;
		}

		if(m_pAssisDlg->TargetWnd == m_hWnd)
		{
			AfxMessageBox(_T("Ŀ����ѡ��, �����ٴ�ѡ����ͬĿ��!"));
			return;
		}

		// ��С��������
		ShowWindow(SW_MINIMIZE);

		// �ͷ���Դ
		if(m_pAssisDlg != NULL)
		{
			m_pAssisDlg->DestroyWindow();
			m_pAssisDlg->IsCreated = FALSE;
		}

		// ��Ŀ�괰����Ϊ�����󴴽�����Ի���
		m_pAssisDlg->Create(IDD_ASSISTANT_DIALOG, FromHandle(m_hWnd));
		m_pAssisDlg->CenterWindow();
		m_pAssisDlg->IsCreated = TRUE;
		m_pAssisDlg->TargetWnd = m_hWnd;
		m_pAssisDlg->FileClass = CurClass;
		m_pAssisDlg->FileType  = CurType;
		m_pAssisDlg->FilePath  = _T("Code\\") + CurClass + _T("\\") + CurType;
		m_pAssisDlg->OnSetCode();
		m_pAssisDlg->OnOversee();
	}
}


void CMainDlg::OnChangeClassType(int Class, int Type)
{
	if(Class > 0)
	{
		int CurClass = m_Class.GetCurSel();
		if(CurClass >= 0)
		{
			m_Class.SetCurSel(CurClass + 1);

			m_Type.ResetContent();

			CString Class;
			m_Class.GetWindowText(Class);
			CString FilePath = _T("Code\\") + Class;

			this->CurClass = Class;

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

			CString pType;
			m_Type.GetWindowText(pType);
			FilePath = _T("Code\\") + Class + _T("\\") + pType;

			this->CurType = pType;

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
			::SendMessage(this->GetSafeHwnd(), WM_CHILDMESSAGE, 22, 0);
		}
	}
	else if(Class < 0)
	{
		int CurClass = m_Class.GetCurSel();
		if(CurClass > 0)
		{
			m_Class.SetCurSel(CurClass - 1);

			m_Type.ResetContent();

			CString Class;
			m_Class.GetWindowText(Class);
			CString FilePath = _T("Code\\") + Class;

			this->CurClass = Class;

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

			// ����ComboBox
			m_Type.SetCurSel(0);

			// ����б�
			m_List.DeleteAllItems();

			CString pType;
			m_Type.GetWindowText(pType);
			FilePath = _T("Code\\") + Class + _T("\\") + pType;

			this->CurType = pType;

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
			::SendMessage(this->GetSafeHwnd(), WM_CHILDMESSAGE, 22, 0);
		}
	}

	if(Type > 0)
	{
		int CurType = m_Type.GetCurSel();
		if(CurType >= 0)
		{
			m_Type.SetCurSel(CurType + 1);

			// ����б�
			m_List.DeleteAllItems();

			CString Class, Type;
			m_Class.GetWindowText(Class);
			m_Type.GetWindowText(Type);
			CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

			// ���õ�ǰ����
			this->CurClass = Class;
			this->CurType = Type;

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
			::SendMessage(this->GetSafeHwnd(), WM_CHILDMESSAGE, 22, 0);
		}
	}
	else if(Type < 0)
	{
		int CurType = m_Type.GetCurSel();
		if(CurType > 0)
		{
			m_Type.SetCurSel(CurType - 1);

			// ����б�
			m_List.DeleteAllItems();

			CString Class, Type;
			m_Class.GetWindowText(Class);
			m_Type.GetWindowText(Type);
			CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

			// ���õ�ǰ����
			this->CurClass = Class;
			this->CurType = Type;

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
			::SendMessage(this->GetSafeHwnd(), WM_CHILDMESSAGE, 22, 0);
		}
	}
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

	case 4:
		// �޸ı�ǩ
		if(m_Tab.m_selTabID >= 0)
		{
			if(!m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileText.IsEmpty())
			{
				if(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsChanged)
				{
					CString Name = _T(" ") + m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName + _T(" * ");
					TCHAR* NewName =  Name.GetBuffer(0);

					m_Tab.SetPageTitle(m_Tab.m_selTabID, NewName);
					m_Tab.Invalidate();
				}
			}
		}

		// �Զ�����
		if (m_hOperate == NULL)
		{
			Type = 7;
			m_hOperate = AfxBeginThread(Operate, this);
		}
		break;

	case 5:
		if(!IsEdit)
		{
			GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("��ӷ���"));
			IsNew = TRUE;
			IsEdit=FALSE;
		}
		else
			IsEdit=FALSE;
		break;

	case 6:
		OnOK();
		break;

	case 7:
		OnNew();
		break;

	case 8:
		OnDirectory();
		break;

	case 9:
		OnFont();
		break;

	case 10:
		OnRemove();
		break;

	case 11:
		OnClearFormat();
		break;

	case 12:
		OnEditFunction();
		break;

	case 13:
		OnImage();
		break;

	case 14:
		OnScreenCapture();
		break;

	case 15:
		OnOpen();
		break;

	case 16:
		OnParagraph();
		break;

	case 17:
		OnLeft();
		break;

	case 18:
		OnRight();
		break;

	case 19:
		OnCenter();
		break;

	case 20:
		m_Tab.OnRButtonDown(NULL, NULL);
		break;

	case 21:
		OnSetLabel();
		break;

	case 22:
		if(m_pAssisDlg->IsCreated)
		{
			m_pAssisDlg->FileClass= CurClass;
			m_pAssisDlg->FileType = CurType;
			m_pAssisDlg->FilePath = _T("Code\\") + CurClass + _T("\\") + CurType;
			m_pAssisDlg->OnSetCode();
		}
		break;

	case 23:
		{
			CString Target = (LPCTSTR)lParam;
			CString Code = theApp.CodePath = _T("Code\\") + CurClass + _T("\\") + CurType + _T("\\") + Target + _T(".code");

			// �½���ǩ
			FileClass = _T("");
			FileType  = _T("");
			FileName  = Target;
			FilePath  = Code;
			SendMessage(WM_COMMAND, 106);

			// ����ⲿ����
			m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsOutFunction = TRUE;

			// ��ѹ������
			Uncompress(Code, Code.Left(Code.GetLength() -5) + _T(".rtf"));

			// ��ȡrtf�ļ�
			m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->OnSetText(Code.Left(Code.GetLength() -5));

			// ɾ��ԭ�ļ�
			DeleteFile(Code.Left(Code.GetLength() -5) + _T(".rtf"));

			// ��������
			CString Source;
			m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetWindowText(Source);

			//put   your   text   in   source
			if(OpenClipboard())
			{
				HGLOBAL   clipbuffer;
				char   *   buffer;
				EmptyClipboard();
				clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   Source.GetLength() +1);
				buffer   =   (char*)GlobalLock(clipbuffer);
				strcpy_s(buffer, strlen(Source.GetBuffer())+1 ,LPCSTR((CStringA)Source));
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_TEXT,clipbuffer);
				CloseClipboard();
			}

			// �رձ�ǩ
			m_Tab.OnRButtonDown(NULL, NULL);
		}
		break;

	case 24:
		OnChangeClassType(-1, 0);
		break;

	case 25:
		OnChangeClassType(1, 0);
		break;

	case 26:
		OnChangeClassType(0, -1);
		break;

	case 27:
		OnChangeClassType(0, 1);
		break;

	case 28:
		for(int i=0; i<TabCount; i++)
		{
			m_Tab.m_DocumentTab.at(i)->m_Edit.SetTargetDevice(NULL, lParam);
		}break;

	case 29:
		SavePic();
		break;

	case 30:
		OnSynchronization();
		break;

	case 31:
		OnSetting();
		break;

	case 32:
		// ����
		IsSave = TRUE;
		OnOK();
		break;
	}

	return TRUE;
}


// �����߳�
UINT CMainDlg::Operate(LPVOID pParam)
{
	// ����ָ��
	CMainDlg * pWnd = (CMainDlg*)pParam;

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

				// ���õ�ǰ����
				pWnd->CurClass = Class;
				pWnd->CurType  = Type;

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

					// ��ǩ����
					CString Path = Code.Left(Code.GetLength() -5);
					Path.Replace(_T("\\"), _T("/"));
					CString Name = Path.Right(Path.GetLength() - Path.ReverseFind('/') -1);

					// �½���ǩ
					pWnd->FileName = Name;
					pWnd->FilePath = Code;
					pWnd->SendMessage(WM_COMMAND, 106);

					// �ⲿ�������
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->IsOutFunction = TRUE;

					// ��ѹ������
					pWnd->Uncompress(Code, Code.Left(Code.GetLength() -5) + _T(".rtf"));

					// ��ȡrtf�ļ�
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->OnSetText(Code.Left(Code.GetLength() -5));

					// ɾ��ԭ�ļ�
					DeleteFile(Code.Left(Code.GetLength() -5) + _T(".rtf"));

					// ����ȡһ��
					if(theApp.IsReadOnce)
						DeleteFile(Code);
				}

				// �Զ���
				else if(GetPrivateProfileInt(_T("Setting"), _T("Open"), 0, _T("./Setting.ini")) == 1)
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
				else
				{   // �½���ǩ
					pWnd->FileClass = _T("");
					pWnd->FileType  = _T("");;
					pWnd->FileName  = _T("�·���");
					pWnd->FilePath  = _T("");
					pWnd->SendMessage(WM_COMMAND, 106);
                }
			}break;

		case 1:
			{
read:
				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				pWnd->m_Type.GetWindowText(Type);

				CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");
				int i = pWnd->m_List.GetNextItem(-1, LVIS_SELECTED);
				CString Name = pWnd->m_List.GetItemText(i, 0);

				// ���İ�ť�ı�
				if(pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileName != _T("�·���"))
				{
					pWnd->IsEdit = TRUE;
					pWnd->GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("�༭����"));

					// �༭���
					pWnd->IsNew = FALSE;
				}

				// Ŀ��
				CString Target = FilePath + Name;

				// �ж��Ƿ��Ѵ�
				for(int i=0; i<(int)pWnd->m_Tab.m_DocumentTab.size(); i++)
				{
					// ��ǩ Path
					CString Path = pWnd->m_Tab.m_DocumentTab.at(i)->FilePath;
					CString Name = pWnd->m_Tab.m_DocumentTab.at(i)->FileName;

					// �����Ѵ򿪵ı�ǩ
					if(Target == Path + Name)
					{
						pWnd->m_Tab.SetCurSel(i);
						pWnd->m_Tab.OnLButtonDown(NULL, NULL);

						// ������Ϊ��
						pWnd->m_hOperate = NULL;
						return TRUE;
					}
				}

				if (!Name.IsEmpty() && i != -1)
				{
					// ��ʾ��Ϣ��ʾ
					//AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

					// �½���ǩ
					pWnd->FileClass = Class;
					pWnd->FileType  = Type;
					pWnd->FileName  = Name;
					pWnd->FilePath  = FilePath;
					pWnd->SendMessage(WM_COMMAND, 106);

					// ��ѹ������
					pWnd->Uncompress(Target + _T(".code"), Target + _T(".rtf"));

					// ��ȡrtf�ļ�
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->OnSetText(Target);

					// ɾ��ԭ�ļ�
					DeleteFile(FilePath + Name + _T(".rtf"));

					// ������ɱ�־
					//theApp.IsFinished = true;
				}
			}break;

		case 2:
			{
				// �½���ǩ
				pWnd->FileClass = _T("");
				pWnd->FileType  = _T("");;
				pWnd->FileName  = _T("ʹ��˵��");
				pWnd->FilePath  = _T("ʹ��˵��.code");
				pWnd->SendMessage(WM_COMMAND, 106);

				// ��ѹ������
				pWnd->Uncompress(_T("ʹ��˵��.code"), _T("ʹ��˵��.rtf"));

				// ��ʾ�û�ʹ��˵��
				pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->OnSetText(_T("ʹ��˵��"));

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

				// ���õ�ǰ����
				pWnd->CurClass = Class;
				pWnd->CurType  = _T("");

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

				// ����Ϣ
				pWnd->SendMessage(WM_CHILDMESSAGE, 22, 0);
			}break;

		case 4:
			{
				// ��ձ༭��
				//pWnd->m_Edit.SetWindowText(_T(""));

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
				//pWnd->m_Edit.SetWindowText(_T(""));

				// ����б�
				pWnd->m_List.DeleteAllItems();

				CString Class, Type;
				pWnd->m_Class.GetWindowText(Class);
				pWnd->m_Type.GetWindowText(Type);
				CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

				// ���õ�ǰ����
				pWnd->CurClass = Class;
				pWnd->CurType = Type;

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

				// ����Ϣ
				pWnd->SendMessage(WM_CHILDMESSAGE, 22, 0);
			}break;

		case 6:
			{
				// ��ձ༭��
				//pWnd->m_Edit.SetWindowText(_T(""));

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
					CString FilePath = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath, TargetFile = FilePath.Left(FilePath.GetLength() -5);

					// �ⲿ�ļ�
					if( pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->IsOutFunction && !FilePath.IsEmpty())
					{
						pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(TargetFile + _T(".rtf"), _T("SF_RTF"));

						// ѹ������
						pWnd->Compress(TargetFile + _T(".rtf"), TargetFile + _T(".code"));

						// ɾ��ԭ�ļ�
						DeleteFile(TargetFile + _T(".rtf"));
					}
					else if(!FilePath.IsEmpty())
					{
						CString Class = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileClass, Type = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileType, Text;
						pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.GetWindowText(Text);

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
							if(!Text.IsEmpty())
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
								CString Target = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath + pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileName;
								pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(Target + _T(".rtf"), _T("SF_RTF"));

								// ѹ������
								pWnd->Compress(Target + _T(".rtf"), Target + _T(".code"));

								// ɾ��ԭ�ļ�
								DeleteFile(Target + _T(".rtf"));
							}
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
					pWnd->ShowWindow(SW_MINIMIZE);

					// ��ʾ��Ϣ��ʾ
					//AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

					AfxGetApp()->BeginWaitCursor();
					WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
					AfxGetApp()->EndWaitCursor();
					
					// ��ԭ������
					pWnd->ShowWindow(SW_RESTORE);

					// ɾ��ԭ�ļ�
					DeleteFile(FilePath + Name + _T(".rtf"));

					// ������ɱ�־
					//theApp.IsFinished = true;
				}
			}break;

		case 9:
			{
				CString FilePath = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath, TargetFile = FilePath.Left(FilePath.GetLength() -5);

				// �ⲿ�ļ�
				if( pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->IsOutFunction && !FilePath.IsEmpty())
				{
					// ��ʾ��Ϣ��ʾ
					AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

					CString TargetFile = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath.Left(pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath.GetLength() -5);
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(TargetFile + _T(".rtf"), _T("SF_RTF"));

					// ѹ������
					pWnd->Compress(TargetFile + _T(".rtf"), TargetFile + _T(".code"));

					// ɾ��ԭ�ļ�
					DeleteFile(TargetFile + _T(".rtf"));

					// ������ɱ�־
					theApp.IsFinished = true;

					// ��Ϣ��ʾ
					pWnd->PostMessage(WM_COMMAND, 103);
				}
				else if(!FilePath.IsEmpty())
				{
					CString Class = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileClass, Type = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileType;

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

					// ��ʾ��Ϣ��ʾ
					AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

					CString TargetFile = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath + pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileName;
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(TargetFile + _T(".rtf"), _T("SF_RTF"));

					// ѹ������
					pWnd->Compress(TargetFile + _T(".rtf"), TargetFile + _T(".code"));

					// ɾ��ԭ�ļ�
					DeleteFile(TargetFile + _T(".rtf"));

					// ������ɱ�־
					theApp.IsFinished = true;

					// ��Ϣ��ʾ
					pWnd->PostMessage(WM_COMMAND, 103);

					// �ж��Ƿ����·���
					//if(!Path.IsEmpty() && pWnd->m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
					//{
					//	// ��ʾ��Ϣ��ʾ
					//	AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

					//	CString TargetFile = pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath + pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileName;
					//	pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(TargetFile + _T(".rtf"), _T("SF_RTF"));

					//	// ѹ������
					//	pWnd->Compress(TargetFile + _T(".rtf"), TargetFile + _T(".code"));

					//	// ɾ��ԭ�ļ�
					//	DeleteFile(TargetFile + _T(".rtf"));

					//	// ������ɱ�־
					//	theApp.IsFinished = true;

					//	// ��Ϣ��ʾ
					//	pWnd->PostMessage(WM_COMMAND, 103);
					//}
					//else
					//{
					//	// ѯ���Ƿ��½�
					//	pWnd->PostMessage(WM_COMMAND, 104);
					//}
				}
				else
					goto New;
			}break;

		case 10:
			{
				New:
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
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.StreamOutToFile(Target + _T(".rtf"), _T("SF_RTF"));

					// ѹ������
					pWnd->Compress(Target + _T(".rtf"), Target + _T(".code"));

					// ɾ��ԭ�ļ�
					DeleteFile(Target + _T(".rtf"));

					// ������ɱ�־
					theApp.IsFinished = true;

					// ��� & ���
					BOOL  isFind = FALSE;
					CString Name = FileDlg.GetFileName();

					// ���ı�ǩ����
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileClass = Class;
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileType  = Type;
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FilePath  = File;
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->FileName  = Name.Left(Name.GetLength() - 5);

					pWnd->m_Tab.SetPageTitle(pWnd->m_Tab.m_selTabID, (TCHAR*)(LPCTSTR)Name.Left(Name.GetLength() - 5));
					pWnd->m_Tab.Invalidate();

					// ȡ��ѡ��
					pWnd->m_List.SetItemState(pWnd->m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

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

					// ��Ϣ��ʾ
					pWnd->PostMessage(WM_COMMAND, 103);
				}
			}break;

			case 11:
			{
				int nItem = -1;
				POSITION pos;

				// ��ձ༭��
				pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.SetWindowText(_T(""));

				// ��ʾ��Ϣ��ʾ
				AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

				// �Ƴ�����
				while (pos = pWnd->m_List.GetFirstSelectedItemPosition())
				{
					nItem = -1;
					nItem = pWnd->m_List.GetNextSelectedItem(pos);
					if (nItem >= 0 && pWnd->m_List.GetSelectedCount() > 0)
					{
						CString Name = pWnd->m_List.GetItemText(nItem, 0);

						CString Class, Type;
						pWnd->m_Class.GetWindowText(Class);
						pWnd->m_Type.GetWindowText(Type);

						// ɾ���ļ�
						DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Name + _T(".code"));

						// �Ƴ��б�
						pWnd->m_List.DeleteItem(nItem);

						// ɾ������
						pWnd->DeleteDirectory(_T("File\\") + Class + _T("\\") + Type + _T("\\") + Name);

						// �ж��Ƿ��Ѵ�
						for(int i=0; i<(int)pWnd->m_Tab.m_DocumentTab.size(); i++)
						{
							// ��ǩ Path
							CString Code = _T("Code\\") + Class + _T("\\") + Type + _T("\\") + Name, Target = pWnd->m_Tab.m_DocumentTab.at(i)->FilePath + pWnd->m_Tab.m_DocumentTab.at(i)->FileName;

							// �ر��Ѵ򿪵ı�ǩ
							if( Code == Target )
							{
								pWnd->m_Tab.SetCurSel(i);
								pWnd->m_Tab.OnRButtonDown(NULL, NULL);
							}
						}

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
							pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.SetWindowText(_T(""));

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
				CString sFilter = _T("Picture Files(*.*)|*.bmp;*.jpg;*.jpeg;*.gif;*.png;*.ico;||");
				CFileDialog dlg(TRUE,0,0, OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING| OFN_FILEMUSTEXIST ,(LPCTSTR)sFilter, pWnd);

				OPENFILENAME &ofn = dlg.GetOFN();
				ofn.lpstrTitle = _T("ѡ��ͼƬ");

				if(dlg.DoModal()==IDOK)
				{
					pWnd->FilePath = dlg.GetPathName();

					// ����ͼƬ
					pWnd->PostMessage(WM_COMMAND, 105);
				}
				
			}break;

			case 13:
			{
				CString sFilter = _T("Code Files(*.code)|*.code||");
				CFileDialog dlg(TRUE, 0, 0, OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING| OFN_FILEMUSTEXIST ,(LPCTSTR)sFilter, pWnd);
				if(dlg.DoModal() == IDOK)
				{
					// ȡ��ѡ��
					pWnd->m_List.SetItemState(pWnd->m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

					// ������ֵ
					CString Code = theApp.CodePath = dlg.GetPathName();

					// �½���ǩ
					CString Name    = Code.Right(Code.GetLength() - Code.ReverseFind('\\') -1);
					        Name    = Name.Left(Name.GetLength() -5);

					pWnd->FileClass = _T("");
					pWnd->FileType  = _T("");
					pWnd->FileName  = Name;
					pWnd->FilePath  = Code;
					pWnd->SendMessage(WM_COMMAND, 106);

					// ����ⲿ����
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->IsOutFunction = TRUE;

					// ��ʾ��Ϣ��ʾ
					//AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

					// ��ѹ������
					pWnd->Uncompress(Code, Code.Left(Code.GetLength() -5) + _T(".rtf"));

					// ��ȡrtf�ļ�
					pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->OnSetText(Code.Left(Code.GetLength() -5));

					// ɾ��ԭ�ļ�
					DeleteFile(Code.Left(Code.GetLength() -5) + _T(".rtf"));

					// ������ɱ�־
					//theApp.IsFinished = true;
				}
			}break;

			case 14:
			{
				try
				{
					// ����������
					pWnd->ShowWindow(SW_HIDE);

					// ��ʾ����
					CScreenToolDlg dlg;
					if(dlg.DoModal() == IDOK)
					{
						// ճ������
						pWnd->m_Tab.m_DocumentTab.at(pWnd->m_Tab.m_selTabID)->m_Edit.Paste();
					}

					// ��ʾ������
					pWnd->ShowWindow(SW_SHOW);

					// �����ö�
					pWnd->SetForegroundWindow();
				}
				catch(...)
				{
					AfxMessageBox(_T("������δ֪�쳣��λ�ڵ�14���߳�!"));
				}
			}break;

		  case 15:
			{
				CString sFilter = _T("Picture Files(*.*)|*.bmp;*.jpg;*.jpeg;*.gif;*.png;*.ico;||");
				CFileDialog dlg(FALSE,0,0, OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING| OFN_FILEMUSTEXIST ,(LPCTSTR)sFilter, pWnd);

				OPENFILENAME &ofn = dlg.GetOFN();
				ofn.lpstrTitle = _T("����ͼƬ");

				if(dlg.DoModal()==IDOK)
				{
					pWnd->FilePath = dlg.GetPathName();

					// ����ͼƬ
					pWnd->PostMessage(WM_COMMAND, 107);
				}
			}break;
		}
	}
	catch(...)
	{
		AfxMessageBox(_T("�������쳣, λ��CMainDlg��Operate����."));
	}

	// ������Ϊ��
	pWnd->m_hOperate = NULL;
	pWnd = NULL;
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
	if(IsSave)
	{
		// �޸ı�ʶ
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsChanged = FALSE;
		m_Tab.OnRButtonDown(NULL, NULL);
		IsSave = FALSE;
	}
	else
	{
		if(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsChanged)
		{
			TCHAR* NewName =  m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName.GetBuffer(0);
			m_Tab.SetPageTitle(m_Tab.m_selTabID, NewName);
			m_Tab.Invalidate();

			// �޸ı�ʶ
			m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsChanged = FALSE;
		}
	}

	// ��Ϣ��ʾ
	MessageBox(_T("�����ѱ���ɹ�!"), _T("�������"), MB_ICONINFORMATION);

	// �˳�
	if(IsCancel)
	{
		IsCancel = FALSE;
		OnCancel();
	}

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
		// �½�����
		if (m_hOperate == NULL)
		{
			this->Type = 10;
			m_hOperate = AfxBeginThread(Operate, this);
		}
	}
}


void CMainDlg::OnCopy()
{
	//CString   source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();     
	////put   your   text   in   source   
	//if(OpenClipboard())   
	//{   
	//	HGLOBAL   clipbuffer;   
	//	char   *   buffer;   
	//	EmptyClipboard();   
	//	clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
	//	buffer   =   (char*)GlobalLock(clipbuffer);
	//	strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
	//	GlobalUnlock(clipbuffer);   
	//	SetClipboardData(CF_TEXT,clipbuffer);   
	//	CloseClipboard();   
	//}

	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.Copy();
}


void CMainDlg::OnPaste()
{
	if( GetPrivateProfileInt(_T("Setting"), _T("Clear"), 0, _T("./Setting.ini")) == 1)
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.PasteSpecial(CF_TEXT);
	else
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.Paste();
}


void CMainDlg::OnCut()
{
	//CString   source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();     
	////put   your   text   in   source   
	//if(OpenClipboard())   
	//{   
	//	HGLOBAL   clipbuffer;   
	//	char   *   buffer;   
	//	EmptyClipboard();   
	//	clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
	//	buffer   =   (char*)GlobalLock(clipbuffer);
	//	strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
	//	GlobalUnlock(clipbuffer);   
	//	SetClipboardData(CF_TEXT,clipbuffer);   
	//	CloseClipboard();   
	//}

	// ����

	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.Cut();
}


void CMainDlg::OnUndo()
{
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.Undo();
}


void CMainDlg::OnRedo()
{
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.Redo();
}


void CMainDlg::OnLeft()
{
	CString source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		PARAFORMAT pf;
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_LEFT; //PFA_CENTER or PFA_RIGHT
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf);
	}
	else
	{
		AfxMessageBox(_T("û����Ҫ���������!"));
	}
}


void CMainDlg::OnRight()
{
	CString source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		PARAFORMAT pf;
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_RIGHT; //PFA_CENTER or PFA_RIGHT
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf);
	}
	else
	{
		AfxMessageBox(_T("û����Ҫ���ҵ�����!"));
	}
}


void CMainDlg::OnCenter()
{
	CString source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		PARAFORMAT pf;
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
		pf.dwMask = PFM_ALIGNMENT;
		pf.wAlignment = PFA_CENTER; //PFA_CENTER or PFA_RIGHT
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf);
	}
	else
	{
		AfxMessageBox(_T("û����Ҫ���е�����!"));
	}
}


void CMainDlg::SetPic()     
{
	////����HBITMAP
 //   HBITMAP bmp = (HBITMAP)::LoadImage(NULL, FilePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_DEFAULTCOLOR|LR_DEFAULTSIZE);

	///*CImage image;
	//image.Load(FilePath);
	//HBITMAP bmp = image.Detach();*/
 //   
 //   STGMEDIUM stgm;
 //   stgm.tymed = TYMED_GDI;
 //   stgm.hBitmap = bmp;
 //   stgm.pUnkForRelease = NULL; 
 //   
 //   FORMATETC fm;
 //   fm.cfFormat = CF_BITMAP;
 //   fm.ptd = NULL;
 //   fm.dwAspect = DVASPECT_CONTENT; 
 //   fm.lindex = -1;
 //   fm.tymed = TYMED_GDI;
 //   
 //   //������������Դ
 //   IStorage *pStorage;
 //   
 //   //�����ڴ�
 //   LPLOCKBYTES lpLockBytes = NULL;
 //   SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes); 
 //   if (sc != S_OK)
 //       AfxThrowOleException(sc);
 //   ASSERT(lpLockBytes != NULL);

 //   sc = ::StgCreateDocfileOnILockBytes(lpLockBytes, STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
 //   if (sc != S_OK)
 //   {
 //       VERIFY(lpLockBytes->Release() == 0);
 //       lpLockBytes = NULL;
 //       AfxThrowOleException(sc);     
 //   }
 //   ASSERT(pStorage != NULL);
 //   
 //   COleDataSource *pDataSource = new COleDataSource;
 //   pDataSource->CacheData(CF_BITMAP, &stgm);
 //   LPDATAOBJECT lpDataObject  = (LPDATAOBJECT)pDataSource->GetInterface(&IID_IDataObject);
 //   
 //   //��ȡRichEdit��OLEClientSite
 //   LPOLECLIENTSITE lpClientSite;
 //   m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetIRichEditOle()->GetClientSite(&lpClientSite);
 //   
 //   //����OLE����
 //   IOleObject *pOleObject;
 //   sc = OleCreateStaticFromData(lpDataObject, IID_IOleObject, OLERENDER_FORMAT, &fm, lpClientSite, pStorage,(void **)&pOleObject);

 //   if(sc!=S_OK)
 //       AfxThrowOleException(sc);
 //   
 //   //����OLE����
 //   REOBJECT reobject;
 //   ZeroMemory(&reobject, sizeof(REOBJECT));
 //   reobject.cbStruct = sizeof(REOBJECT);
 //   
 //   CLSID clsid;
 //   sc = pOleObject->GetUserClassID(&clsid);
 //   if (sc != S_OK)
 //       AfxThrowOleException(sc);
 //   
 //   reobject.clsid = clsid;
 //   reobject.cp = REO_CP_SELECTION;
 //   reobject.dvaspect = DVASPECT_CONTENT; 
 //   reobject.poleobj = pOleObject;
 //   reobject.polesite = lpClientSite;
 //   reobject.pstg = pStorage;
 //   
 //   HRESULT hr = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetIRichEditOle()->InsertObject( &reobject );
 //   delete pDataSource;

	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.InsertImage(FilePath);
}


void CMainDlg::SavePic()
{
	// ����ͼƬ
	/*if (m_hOperate == NULL)
	{
		Type = 15;
		m_hOperate = AfxBeginThread(Operate, this);
		CloseHandle(m_hOperate->m_hThread);
	}*/
	AfxMessageBox(_T("�÷�������ȱ�ݣ���ʱ����ʹ�á�"));
}


void CMainDlg::OnImage()
{
	// ����ͼƬ
	if (m_hOperate == NULL)
	{
		Type = 12;
		m_hOperate = AfxBeginThread(Operate, this);
	}

	/*CFileDialog fd(TRUE);
	CString strFile;
	OPENFILENAME &ofn = fd.GetOFN();
	ofn.lpstrFilter = _T("ͼ���ļ�(*.bmp;*.jpg;*.jpeg;*.gif;*.png)\0*.bmp;*.jpg;*.jpeg;*.gif;*.png\0\0");
	ofn.lpstrTitle = _T("����ļ�");
	ofn.lpstrFile = strFile.GetBuffer(MAX_PATH);
	INT_PTR ret = fd.DoModal();
	strFile.ReleaseBuffer();
	if (ret == IDOK) {
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.InsertImage(strFile);
	}*/
}


BOOL SaveBmpDataToFile(HBITMAP hBitmap, CString szfilename)
{
	//HDC     hDC;         
	////��ǰ�ֱ�����ÿ������ռ�ֽ���          
	//int     iBits;         
	////λͼ��ÿ������ռ�ֽ���          
	//WORD     wBitCount;         
	////�����ɫ���С��     λͼ�������ֽڴ�С     ��λͼ�ļ���С     ��     д���ļ��ֽ���              
	//DWORD     dwPaletteSize=0,   dwBmBitsSize=0,   dwDIBSize=0,   dwWritten=0;             
	////λͼ���Խṹ              
	//BITMAP     Bitmap;                 
	////λͼ�ļ�ͷ�ṹ          
	//BITMAPFILEHEADER     bmfHdr;                 
	////λͼ��Ϣͷ�ṹ              
	//BITMAPINFOHEADER     bi;                 
	////ָ��λͼ��Ϣͷ�ṹ                  
	//LPBITMAPINFOHEADER     lpbi;                 
	////�����ļ��������ڴ�������ɫ����              
	//HANDLE     fh,   hDib,   hPal,hOldPal=NULL;             
 //
	////����λͼ�ļ�ÿ��������ռ�ֽ���              
	//hDC  = CreateDC("DISPLAY",   NULL,   NULL,   NULL);         
	//iBits  = GetDeviceCaps(hDC,   BITSPIXEL)     *     GetDeviceCaps(hDC,   PLANES);             
	//DeleteDC(hDC);             
	//if(iBits <=  1)                                                   
	//	wBitCount = 1;             
	//else  if(iBits <=  4)                               
	//	wBitCount  = 4;             
	//else if(iBits <=  8)                               
	//	wBitCount  = 8;             
	//else                                                                                                                               
	//	wBitCount  = 24;
 //
	//GetObject(hBitmap,   sizeof(Bitmap),   (LPSTR)&Bitmap);         
	//bi.biSize= sizeof(BITMAPINFOHEADER);         
	//bi.biWidth = Bitmap.bmWidth;         
	//bi.biHeight =  Bitmap.bmHeight;         
	//bi.biPlanes =  1;         
	//bi.biBitCount = wBitCount;         
	//bi.biCompression= BI_RGB;         
	//bi.biSizeImage=0;         
	//bi.biXPelsPerMeter = 0;         
	//bi.biYPelsPerMeter = 0;         
	//bi.biClrImportant = 0;         
	//bi.biClrUsed =  0;         
 //
	//dwBmBitsSize  = ((Bitmap.bmWidth *wBitCount+31) / 32)*4* Bitmap.bmHeight;         
 //
	////Ϊλͼ���ݷ����ڴ�              
	//hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));             
	//lpbi  = (LPBITMAPINFOHEADER)GlobalLock(hDib);             
	//*lpbi  = bi;             
 //
	////     �����ɫ��                  
	//hPal  = GetStockObject(DEFAULT_PALETTE);             
	//if (hPal)             
	//{             
	//	hDC  = ::GetDC(NULL);             
	//	hOldPal = ::SelectPalette(hDC,(HPALETTE)hPal, FALSE);             
	//	RealizePalette(hDC);             
	//}         
 //
	////     ��ȡ�õ�ɫ�����µ�����ֵ              
	//GetDIBits(hDC,hBitmap, 0,(UINT)Bitmap.bmHeight,  
	//	(LPSTR)lpbi+ sizeof(BITMAPINFOHEADER)+dwPaletteSize,   
	//	(BITMAPINFO *)lpbi, DIB_RGB_COLORS);             
 //
	////�ָ���ɫ��                  
	//if (hOldPal)             
	//{             
	//	::SelectPalette(hDC,   (HPALETTE)hOldPal,   TRUE);             
	//	RealizePalette(hDC);             
	//	::ReleaseDC(NULL,   hDC);             
	//}             
 //
	////����λͼ�ļ�                  
	//fh  = CreateFile(szfilename,   GENERIC_WRITE,0,   NULL,   CREATE_ALWAYS,           
	//	FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,   NULL);             
 //
	//if (fh     ==  INVALID_HANDLE_VALUE)         return     FALSE;             
 //
	////     ����λͼ�ļ�ͷ              
	//bmfHdr.bfType  = 0x4D42;     //     "BM"              
	//dwDIBSize  = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize;                 
	//bmfHdr.bfSize  = dwDIBSize;             
	//bmfHdr.bfReserved1  = 0;             
	//bmfHdr.bfReserved2  = 0;             
	//bmfHdr.bfOffBits  = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;             
	////     д��λͼ�ļ�ͷ              
	//WriteFile(fh,   (LPSTR)&bmfHdr,   sizeof(BITMAPFILEHEADER),   &dwWritten,   NULL);             
	////     д��λͼ�ļ���������              
	//WriteFile(fh,   (LPSTR)lpbi,   dwDIBSize,   &dwWritten,   NULL);             
	////���                  
	//GlobalUnlock(hDib);             
	//GlobalFree(hDib);             
	//CloseHandle(fh);             
 //
	//return     TRUE;         

	BITMAP bm;
    int width, height;
    GetObject(hBitmap, sizeof(BITMAP), &bm);    
    width = bm.bmWidth;
    height = bm.bmHeight;
    CDC memDC;
    memDC.CreateCompatibleDC(NULL);
    memDC.SelectObject(hBitmap);

    CImage image;
    image.Create(width, height, 32);
    BitBlt(image.GetDC(), 0, 0, width, height, memDC.m_hDC, 0, 0, SRCCOPY);

    HRESULT hResult = image.Save(szfilename);
    //image.Save(_T("new.bmp"), Gdiplus::ImageFormatBMP);   // �����bmp

    image.ReleaseDC();
    memDC.DeleteDC();
    return SUCCEEDED(hResult);
}


BOOL GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;

	GetImageEncodersSize(&num, &size);

	if(size == 0)
		return FALSE;

	ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)new BYTE[size];
	if(pImageCodecInfo == NULL)
		return FALSE;

	GetImageEncoders(num, size, pImageCodecInfo);
	for(UINT j = 0; j < num; ++j)
	{
		if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			delete pImageCodecInfo;
			return TRUE;
		}
	}

	delete pImageCodecInfo;
	return FALSE;
}


BOOL GetEncoderClsName(LPCTSTR lpszFileName, LPTSTR lpszBuf, DWORD dwBufSize)
{
	TCHAR* pFileExt = (TCHAR*)StrRChrW((LPCWSTR)lpszFileName, NULL, _T('.'));
	if(NULL == pFileExt)
		return FALSE;

	_stprintf_s(lpszBuf, dwBufSize, _T("image/%s"), pFileExt + 1);
	return TRUE;
}


// ֧�ֵı����ʽ:BMP/TIFF/GIF/JPEG/PNG
BOOL SaveBmpToJpeg(HBITMAP hBmp, LPCTSTR lpszJpegFileName, ULONG quality = 100)
{
	TCHAR sEncoderClsName[50] = {0};
	if(!GetEncoderClsName(lpszJpegFileName, sEncoderClsName, 50))
		return FALSE;

	CLSID jpgClsid;
	if(!GetEncoderClsid((const WCHAR *)sEncoderClsName, &jpgClsid))
		return FALSE;

	EncoderParameters encoderParameters;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &quality;

	Gdiplus::Bitmap bmp(hBmp, NULL);
	Status status = bmp.Save((const WCHAR *)lpszJpegFileName, &jpgClsid, &encoderParameters);

	return (status != Ok);
}


void CMainDlg::OnScreenCapture()
{
	// ����ͼƬ
	if (m_hOperate == NULL)
	{
		Type = 14;
		m_hOperate = AfxBeginThread(Operate, this);
	}

	// �ӳ�
	//Sleep(300);

	//CDC* pScreenDc = CDC::FromHandle(::GetDC(NULL));//��ĻDC
	//CDC memDc;// �ڴ�DC
	//CBitmap memBmp;
	//CBitmap* pOldBmp;
	//int cx = GetSystemMetrics(SM_CXSCREEN);
	//int cy = GetSystemMetrics(SM_CYSCREEN);
	//memDc.CreateCompatibleDC(pScreenDc);
	//memBmp.CreateCompatibleBitmap(pScreenDc, cx, cy);
	//pOldBmp = memDc.SelectObject(&memBmp);
	//memDc.BitBlt(0, 0, cx, cy, pScreenDc, 0, 0, SRCCOPY);

	////���Ƶ����а�
	//OpenClipboard();
	//EmptyClipboard();
	//SetClipboardData(CF_BITMAP, memBmp.Detach());
	//CloseClipboard();

	//// ��ʾ����ǰ����
	///*CClientDC clientDc(this);
	//CRect rcClient;
	//GetClientRect(rcClient);
	//clientDc.StretchBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDc, 0, 0, cx, cy, SRCCOPY);*/

	//memDc.SelectObject(pOldBmp);
	//memDc.DeleteDC();
	//memBmp.DeleteObject();

	//// ճ��
	//m_Edit.Paste();

	//m_Screen->CenterWindow();
	//m_Screen->ShowWindow(SW_SHOW);
}


void CMainDlg::SaveImage()
{
	//BOOL bSavePicSuccess = FALSE;
	//int nCount = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.m_pRichEditOle->GetObjectCount(); // ��ǰm_ChatEdit�����е�object����
	//for ( int i=0; i<nCount; i++ )
	//{
	//	REOBJECT reobject;
	//	memset( &reobject, 0 , sizeof(REOBJECT) );
	//	reobject.cbStruct = sizeof(REOBJECT);

	//	SCODE sc = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.m_pRichEditOle->GetObject( i, &reobject, REO_GETOBJ_ALL_INTERFACES ); // ��ȡ��reobject����
	//	if ( sc != S_OK )
	//	{
	//		continue;
	//	}

	//	if ( (reobject.dwFlags & REO_SELECTED) != REO_SELECTED ) // ���鵱ǰreobject�Ƿ���ѡ��״̬
	//	{
	//		continue; // ���ǵ�ǰѡ�еĶ��󣬼�������
	//	}

	//	IDataObject* pDataObject = NULL;
	//	sc = reobject.poleobj->QueryInterface( IID_IDataObject, (void**)&pDataObject ); // ��ȡIDataObject��Ϣ
	//	if ( sc != S_OK )
	//	{
	//		break; // �Ѿ��ҵ�ѡ�е�ͼƬ���󣬻�ȡ��Ϣʧ�ܣ�ֱ���˳�
	//	}

	//	// Ҫ��fm�����������ã�ָ��Ҫ��IStorage�л�ȡʲô������Ϣ������GetData�ӿڻ��ȡstg��Ϣʧ��
	//	STGMEDIUM stg;
	//	FORMATETC fm;
	//	fm.cfFormat = CF_BITMAP;        // Clipboard format = CF_BITMAP
	//	fm.ptd = NULL;                  // Target Device = Screen
	//	fm.dwAspect = DVASPECT_CONTENT; // Level of detail = Full content
	//	fm.lindex = -1;                 // Index = Not applicaple
	//	fm.tymed = TYMED_GDI;           // ��ӦCF_BITMAP

	//	HRESULT hr = pDataObject->GetData( &fm, &stg );
	//	if ( hr != S_OK || stg.hBitmap == NULL )
	//	{
	//		break; // �Ѿ��ҵ�ѡ�е�ͼƬ���󣬻�ȡ��Ϣʧ�ܣ�ֱ���˳�
	//	}

	//	BOOL bRet = SaveBmpDataToFile( stg.hBitmap, FilePath );
	//	if ( bRet )
	//	{
	//		bSavePicSuccess = TRUE;
	//	}
	//}

	//if ( !bSavePicSuccess )
	//{
	//	AfxMessageBox( _T("ͼƬ����ʧ�ܣ�") );
	//}

	AfxMessageBox(_T("�÷�������ȱ�ݣ���ʱ����ʹ�á�"));
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


void CMainDlg::OnOpen()
{
	// �򿪷���
	if (m_hOperate == NULL)
	{
		Type = 13;
		m_hOperate = AfxBeginThread(Operate, this);
	}
}


void CMainDlg::OnParagraph()
{
	CParagraphDlg dlg;

	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);

	//dlg.m_Left.Format(_T("%d"),    pf.dxOffset);
	//dlg.m_Tab.Format(_T("%d"),     pf.cTabCount);

	dlg.m_Index.Format(_T("%d"),   pf.dxStartIndent);
	dlg.m_Right.Format(_T("%d"),   pf.dxRightIndent);
	dlg.m_Spacing.Format(_T("%d"), pf.dyLineSpacing);
	dlg.m_Part.Format(_T("%d"),    pf.dySpaceAfter);
	dlg.m_Tab.Format(_T("%d"),     pf.dxOffset);

	// ���뷽ʽ
	if(pf.wAlignment == PFA_LEFT)
		dlg.Alignment = 0;
	else if(pf.wAlignment == PFA_RIGHT)
		dlg.Alignment = 1;
	else
		dlg.Alignment = 2;

	// �б���ʽ
	if(pf.wNumbering == PFN_BULLET)
		dlg.Tab = 1;
	else if(pf.wNumbering == PFN_ARABIC)
		dlg.Tab = 2;
	else if(pf.wNumbering == PFN_LCLETTER)
		dlg.Tab = 3;
	else if(pf.wNumbering == PFN_UCLETTER)
		dlg.Tab = 4;
	else if(pf.wNumbering == PFN_LCROMAN)
		dlg.Tab = 5;
	else if(pf.wNumbering == PFN_UCROMAN)
		dlg.Tab = 6;
	else
		dlg.Tab = 0;

	if(dlg.DoModal() == IDOK)
	{
		pf.cbSize = sizeof(PARAFORMAT2); // ���� ��������           �μ��           �м��            // �Ҽ��        // ���뷽ʽ     // �б���ʽ
		pf.dwMask = PFM_NUMBERING | PFM_OFFSET | PFM_STARTINDENT | PFM_SPACEAFTER | PFM_LINESPACING | PFM_RIGHTINDENT | PFM_ALIGNMENT | PFM_TABSTOPS;
		pf.dxStartIndent = atol(dlg.m_Index);
		pf.dxRightIndent = atol(dlg.m_Right);
		pf.dyLineSpacing = atol(dlg.m_Spacing);
		pf.dySpaceAfter  = atol(dlg.m_Part);
		//pf.cTabCount     = atoi(dlg.m_Tab);
		pf.dxOffset      = atoi(dlg.m_Tab);
		
		// ���뷽ʽ
		if(dlg.Alignment == 0)
			pf.wAlignment = PFA_LEFT;
		else if(dlg.Alignment == 1)
			pf.wAlignment = PFA_RIGHT;
		else
			pf.wAlignment = PFA_CENTER;

		// �б���ʽ
		if(dlg.Tab == 0)
			pf.wNumbering = NULL;
		else if(dlg.Tab == 1)
			pf.wNumbering = PFN_BULLET;
		else if(dlg.Tab == 2)
			pf.wNumbering = PFN_ARABIC;
		else if(dlg.Tab == 3)
			pf.wNumbering = PFN_LCLETTER;
		else if(dlg.Tab == 4)
			pf.wNumbering = PFN_UCLETTER;
		else if(dlg.Tab == 5)
			pf.wNumbering = PFN_LCROMAN;
		else if(dlg.Tab == 6)
			pf.wNumbering = PFN_UCROMAN;

		VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
	}
}


void CMainDlg::OnNone()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // �б���ʽ
	pf.wNumbering = NULL;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnSymbol()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // �б���ʽ
	pf.wNumbering = PFN_BULLET;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnNumber()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // �б���ʽ
	pf.wNumbering = PFN_ARABIC;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnLowerCase()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // �б���ʽ
	pf.wNumbering = PFN_LCLETTER;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnUpperCase()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // �б���ʽ
	pf.wNumbering = PFN_UCLETTER;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnLowerRome()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // �б���ʽ
	pf.wNumbering = PFN_LCROMAN;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnUpperRome()
{
	PARAFORMAT2 pf;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetParaFormat(pf);
	pf.cbSize     = sizeof(PARAFORMAT2);
	pf.dwMask     = PFM_NUMBERING;  // �б���ʽ
	pf.wNumbering = PFN_UCROMAN;
	VERIFY(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetParaFormat(pf));
}


void CMainDlg::OnNewLabel()
{
	// �½���ǩ
	m_pDocument[TabCount] = new CDocumentDlg();
	m_Tab.CreateTabPage(m_pDocument[TabCount], IDD_DOCUMENT_DIALOG, FileName);
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileClass = FileClass;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileType  = FileType;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName  = FileName;
	m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FilePath  = FilePath;
	TabCount++;
}


void CMainDlg::OnSetLabel()
{
	if(!m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsOutFunction)
	{
		CString CurClass, CurType, TargetClass, TargetType;
		m_Class.GetWindowText(CurClass);
		m_Type.GetWindowText (CurType);

		TargetClass = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileClass;
		TargetType  = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileType;

		if(!TargetClass.IsEmpty() && TargetClass != CurClass)
		{
			int nIndex = m_Class.FindStringExact(0, TargetClass);
			if(nIndex != CB_ERR)
			{
				m_Class.SetCurSel(nIndex);

				// ���ComboBox
				m_Type.ResetContent();

				CString FilePath = _T("Code\\") + TargetClass;

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
			}
		}

		if(!TargetType.IsEmpty() && TargetType != CurType)
		{
			int nIndex = m_Type.FindStringExact(0, TargetType);
			if(nIndex != CB_ERR)
			{
				m_Type.SetCurSel(nIndex);

				// ����б�
				m_List.DeleteAllItems();

				CString FilePath = _T("Code\\") + TargetClass + _T("\\") + TargetType;

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
			}
		}

		// ȡ��ѡ��
		m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

		// �ҵ���Ӧ�ķ���
		int Count = m_List.GetItemCount();
		for(int i=0; i<Count; i++)
		{
			CString Function = m_List.GetItemText(i, 0);

			if(Function == m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName)
			{
				//���ø�����ʾ  
				m_List.SetFocus();//���ý���  
				m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
				m_List.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ�
				break;
			}
		}

		// ����ת��
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetFocus();

		// ���İ�ť�ı�
		CString Text;
		GetDlgItem(IDC_NEW_BUTTON)->GetWindowText(Text);

		CString Title = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName;
		if(Title != _T("�·���"))
		{
			if(Text != _T("�༭����"))
			{
				// ���İ�ť�ı�
				GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("�༭����"));

				// �༭���
				IsNew = FALSE;
			}
		}
		else if(!Title.IsEmpty())
		{
			// ���İ�ť�ı�
			GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("��ӷ���"));

			// ��ӱ��
			IsNew  = TRUE;
		}

		// ���ý���
		m_List.SetFocus();
	}
	else
	{
		// ȡ��ѡ��
		m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

		// ���İ�ť�ı�
		CString Text;
		GetDlgItem(IDC_NEW_BUTTON)->GetWindowText(Text);

		if(Text != _T("�༭����"))
		{
			// ���İ�ť�ı�
			GetDlgItem(IDC_NEW_BUTTON)->SetWindowText(_T("�༭����"));

			// �༭���
			IsNew = FALSE;
		}
    }
}


void CMainDlg::OnClickCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
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
	}

	VMPEND

	*pResult = 0;
}


void CMainDlg::OnItemchangedCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN
		// ��ȡĿ���ļ�
		if (m_hOperate == NULL)
		{
			Type = 1;
			m_hOperate = AfxBeginThread(Operate, this);
		}
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
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetWindowText(_T(""));

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

				// ��ձ༭��
				m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetWindowText(_T(""));

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
				m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetWindowText(_T(""));

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
	}

	VMPEND
}


void CMainDlg::OnEditFunction()
{
	// �ж��ⲿ����
	if(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsOutFunction && !m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FilePath.IsEmpty())
	{
		CEditDlg dlg;
		dlg.Type  = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName;
		dlg.IsFunction = true;

		if(dlg.DoModal() == IDOK)
		{
			CString old_Path = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FilePath;
			CString new_Path = old_Path.Left(old_Path.GetLength() - m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName.GetLength() -5) + dlg.Type + _T(".code");

			// ������
			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				// �޸ı�ǩ����
				m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName   = dlg.Type;
				m_Tab.SetPageTitle(m_Tab.m_selTabID, (TCHAR*)(LPCTSTR)dlg.Type);

				// ˢ��
				m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->Invalidate();

				// ��ʾ��Ϣ
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
		CString Class = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileClass, 
			Type  = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileType, 
			Function = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName;

		// �ж�ѡ�з���
		if (!Function.IsEmpty() && !Class.IsEmpty() && !Type.IsEmpty())
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
					// �޸��б�
					int Count = m_List.GetItemCount();
					for(int i=0; i<Count; i++)
					{
						CString Name = m_List.GetItemText(i, 0);
						if(Function == Name)
						{
							m_List.SetItemText(i, 0, dlg.Type);

							// �޸ı�ǩ����
							m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName   = dlg.Type;
							m_Tab.SetPageTitle(m_Tab.m_selTabID, (TCHAR*)(LPCTSTR)dlg.Type);

							// ��ʾ��Ϣ
							MessageBox(_T("�����޸ĳɹ�!"), _T("�޸ĳɹ�"), MB_ICONINFORMATION | MB_OK);

							// ȡ��ѡ��
							m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

							//���ø�����ʾ
							m_List.SetFocus();//���ý���
							m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬
							m_List.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ�
							break;
						}
					}
				}
				else
				{
					AfxMessageBox(_T("�����޸�ʧ��!"));
				}
			}
		}
		else
		{
			AfxMessageBox(_T("�·�������༭, �뱣�淽��!"));
		}
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
	BOOL bSelect = (m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelectionType() != SEL_EMPTY) ? TRUE : FALSE; 
	if (bSelect)
	{
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelectionCharFormat(cf);
	}
	else
	{
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetDefaultCharFormat(cf);
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
			m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetSelectionCharFormat(cf);    //Ϊѡ���������趨��ѡ����
		else
			m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetWordCharFormat(cf);         //Ϊ��Ҫ����������趨����
	}

	VMPEND
}


void CMainDlg::OnNew()
{
	// ������ǩ
	if(IsNew)
	{
		// ȡ��ѡ��
		//m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

		// �½���ǩ
		m_pDocument[TabCount] = new CDocumentDlg();
		m_Tab.CreateTabPage(m_pDocument[TabCount], IDD_DOCUMENT_DIALOG, _T("�·���"));
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileName = _T("�·���");
		TabCount++;
	}
	else
	{
		// �༭����
		OnEditFunction();
	}
}


void CMainDlg::OnClearFormat()
{
	CString source = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelText();
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
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.GetSelectionCharFormat(cf);
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
		m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->m_Edit.SetSelectionCharFormat(cf);
	}
	else
	{
		AfxMessageBox(_T("û����Ҫ�����ʽ������!"));
	}
}


void CMainDlg::OnDirectory()
{
	VMPBEGIN

	if(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsOutFunction)
	{
		AfxMessageBox(_T("�ⲿ���������������ļ�!"));
		return;
	}

	CString Class = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileClass, Type = m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FileType, Dir;

	if(Class.IsEmpty() || Type.IsEmpty())
	{
		AfxMessageBox(_T("�·��������������ļ�, ���ȱ��淽��!"));
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

	// �ж��ⲿ����
	if(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->IsOutFunction)
	{
		if(MessageBox(_T("�Ƿ�Ҫ�Ƴ���ǰ����? "),  _T("�����Ƴ�ȷ��"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			// �Ƴ�����
			DeleteFile(m_Tab.m_DocumentTab.at(m_Tab.m_selTabID)->FilePath);

			// �رձ�ǩ
			m_Tab.OnRButtonDown(NULL, NULL);
		}
	}
	else
	{
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
		}
	}
	if(GetExitCodeThread(m_hUpDate, &Code))
	{
		if(Code == STILL_ACTIVE)
		{
			TerminateThread(m_hUpDate, 0);
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

	// ���ĵ������ж�
	for(int i=0; i<(int)m_Tab.m_DocumentTab.size(); i++)
	{
		if(m_Tab.m_DocumentTab.at(i)->IsChanged)
		{
			// ���õ�ǰ��ǩѡ��
			m_Tab.SetCurSel(i);
			m_Tab.OnLButtonDown(NULL, NULL);

			if( MessageBox(m_Tab.m_DocumentTab.at(i)->FileName + _T("�����ѱ��޸�, �Ƿ񱣴淽��?"), _T("��⵽�޸�"), MB_ICONQUESTION | MB_YESNO) == IDYES )
			{
				// ����
				IsCancel = TRUE;
				OnOK();
				return;
			}
		}
	}

	CDialogEx::OnCancel();
}
