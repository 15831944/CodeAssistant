// AssistantDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "AssistantDlg.h"
#include "afxdialogex.h"

// CAssistantDlg �Ի���

IMPLEMENT_DYNAMIC(CAssistantDlg, CDialogEx)

CAssistantDlg::CAssistantDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAssistantDlg::IDD, pParent)
{
	IsCreated = FALSE;
}

CAssistantDlg::~CAssistantDlg()
{
}

void CAssistantDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CODE_COMBO, m_ComboBox);
	DDX_Control(pDX, IDC_HIDE_CHECK, m_Hide);
	DDX_Control(pDX, IDC_FOLLOW_CHECK, m_Follow);
}


BEGIN_MESSAGE_MAP(CAssistantDlg, CDialogEx)
	ON_WM_HOTKEY()
	ON_CBN_DROPDOWN(IDC_CODE_COMBO, &CAssistantDlg::OnDropdownCodeCombo)
	ON_BN_CLICKED(IDC_HIDE_CHECK, &CAssistantDlg::OnHideCheck)
	ON_BN_CLICKED(IDC_FOLLOW_CHECK, &CAssistantDlg::OnFollowCheck)
END_MESSAGE_MAP()


// CAssistantDlg ��Ϣ�������


BOOL CAssistantDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��ȡ�Զ�����
	if( GetPrivateProfileInt(_T("Setting"), _T("AutoHide"), 1, _T("./Setting.ini")) == 1)
	{
		m_Hide.SetCheck(1);
		ShowWindow(SW_HIDE);
	}
	else
	{
		m_Hide.SetCheck(0);
		ShowWindow(SW_SHOW);
	}

	// ��ȡ�������
	if( GetPrivateProfileInt(_T("Setting"), _T("Follow"), 1, _T("./Setting.ini")) == 1)
	{
		m_Follow.SetCheck(1);
	}
	else
	{
		m_Follow.SetCheck(0);
	}

	// ע���ȼ�
	::RegisterHotKey(m_hWnd, ID_HOTKEY_A, MOD_ALT, 'A');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_B, MOD_ALT, 'B');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_C, MOD_ALT, 'C');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_D, MOD_ALT, 'D');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_E, MOD_ALT, 'E');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_F, MOD_ALT, 'F');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_G, MOD_ALT, 'G');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_H, MOD_ALT, 'H');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_I, MOD_ALT, 'I');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_J, MOD_ALT, 'J');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_K, MOD_ALT, 'K');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_L, MOD_ALT, 'L');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_M, MOD_ALT, 'M');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_N, MOD_ALT, 'N');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_O, MOD_ALT, 'O');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_P, MOD_ALT, 'P');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_Q, MOD_ALT, 'Q');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_R, MOD_ALT, 'R');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_S, MOD_ALT, 'S');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_T, MOD_ALT, 'T');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_U, MOD_ALT, 'U');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_V, MOD_ALT, 'V');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_W, MOD_ALT, 'W');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_X, MOD_ALT, 'X');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_Y, MOD_ALT, 'Y');
	::RegisterHotKey(m_hWnd, ID_HOTKEY_Z, MOD_ALT, 'Z');

	::RegisterHotKey(m_hWnd, ID_HOTKEY_LEFT,  MOD_ALT, VK_LEFT);
	::RegisterHotKey(m_hWnd, ID_HOTKEY_RIGHT, MOD_ALT, VK_RIGHT);
	::RegisterHotKey(m_hWnd, ID_HOTKEY_UP,    MOD_ALT, VK_UP);
	::RegisterHotKey(m_hWnd, ID_HOTKEY_DOWN,  MOD_ALT, VK_DOWN);

	// ��ʾ��
	m_toolTips.Create(this, TTS_ALWAYSTIP|WS_POPUP);
	m_toolTips.Activate(TRUE);

	m_toolTips.AddTool(GetDlgItem(IDC_CODE_COMBO),   _T("��ʾ��ǰ�������з�����\nAlt + ��������ĸ���ٲ��ҷ�����\nAtl + ���·�����л���ǰ���\nAtl + ���ҷ�����л���ǰ���͡�"));
	m_toolTips.AddTool(GetDlgItem(IDC_HIDE_CHECK),   _T("ѡ�д������������Զ����ء�\n��Atl+��������ĸ������"));
	m_toolTips.AddTool(GetDlgItem(IDC_FOLLOW_CHECK), _T("ѡ�д�������صı���������������λ�ó��֡�\n��û���Զ�����, ������ʹ�ô˹��ܡ�"));
	m_toolTips.AddTool(GetDlgItem(IDOK),             _T("��ȡĿ�귽�����ڹ��λ�ò��롣"));
	m_toolTips.AddTool(GetDlgItem(IDCANCEL),         _T("���ؿ��ٱ�������\n���غ����Atl+��������ĸ������"));

	//������ɫ
	m_toolTips.SetTipTextColor(RGB(0,0,255));

	//���ָ���ú���ʾ��ʾ������
	m_toolTips.SetDelayTime(TTDT_INITIAL, 10); 

	//��걣��ָ����ʾ��ʾ��ã�����
	m_toolTips.SetDelayTime(TTDT_AUTOPOP, 9000000);

	//�趨��ʾ��ȣ����������Զ�����
	m_toolTips.SetMaxTipWidth(300);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CAssistantDlg::PreTranslateMessage(MSG* pMsg)
{
	// ������ʾ
	if(GetPrivateProfileInt(_T("Setting"), _T("Tip"), 0, _T("./Setting.ini")) == 1)
		m_toolTips.RelayEvent(pMsg); // ������Ϣ��Ӧ

	return CDialogEx::PreTranslateMessage(pMsg);
}


UINT CAssistantDlg::Operate(LPVOID pParam)
{
	// ����ָ��
	CAssistantDlg * pWnd = ((CAssistantDlg*)pParam);

	while(::IsWindow(pWnd->TargetWnd))
	{
		Sleep(100);
	}

	pWnd->ShowWindow(SW_HIDE);
	AfxGetApp()->GetMainWnd()->ShowWindow(SW_RESTORE);
	return TRUE;
}


void CAssistantDlg::OnOK()
{
	// �õ�Ŀ������
	CString Target;
	m_ComboBox.GetWindowText(Target);
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 23, (LPARAM)(LPCTSTR)Target);

	// ��ȡ����λ��
	if( GetPrivateProfileInt(_T("Setting"), _T("AutoHide"), 1, _T("./Setting.ini")) == 1)
	{
		// ���ش���
		ShowWindow(SW_HIDE);
	}

	// ǰ�ô���
	::SetForegroundWindow(TargetWnd);

	char * buffer = NULL;
	//�򿪼�����
	CString fromClipboard;
	if ( OpenClipboard() )
	{
		HANDLE hData = GetClipboardData(CF_TEXT);
		char * buffer = (char*)GlobalLock(hData);
		fromClipboard = buffer;
		GlobalUnlock(hData);
		CloseClipboard();
	}

	// ճ������
	keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), 0 ,0);
	keybd_event(86, MapVirtualKey(L'V',0),0,0);

	Sleep(50);
	keybd_event(86,  MapVirtualKey(L'V',0), KEYEVENTF_KEYUP,0);
	keybd_event(VK_CONTROL, MapVirtualKey(VK_CONTROL,0), KEYEVENTF_KEYUP,0);
}


void CAssistantDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	CRect rc;
	CRect rect;
	GetWindowRect(&rect);//�����С
	rc.CopyRect(&rect);//���ƾ�����
	CPoint pOint;
	GetCursorPos(&pOint);//��ȡ���λ��

	// ��ȡ�������
	if( GetPrivateProfileInt(_T("Setting"), _T("Follow"), 1, _T("./Setting.ini")) == 1)
	{
		// �ƶ�����
		MoveWindow(pOint.x,pOint.y,rc.Width(),rc.Height());//�ƶ�����
	}

	// ��ʾ����
	ShowWindow(SW_SHOW);

	// ���ý���
	m_ComboBox.SetFocus();

	switch (nHotKeyId)
	{
	case ID_HOTKEY_A:
		m_ComboBox.SetWindowText(_T("A"));
		break;

	case ID_HOTKEY_B:
		m_ComboBox.SetWindowText(_T("B"));
		break;

	case ID_HOTKEY_C:
		m_ComboBox.SetWindowText(_T("C"));
		break;

	case ID_HOTKEY_D:
		m_ComboBox.SetWindowText(_T("D"));
		break;

	case ID_HOTKEY_E:
		m_ComboBox.SetWindowText(_T("E"));
		break;

	case ID_HOTKEY_F:
		m_ComboBox.SetWindowText(_T("F"));
		break;

	case ID_HOTKEY_G:
		m_ComboBox.SetWindowText(_T("G"));
		break;

	case ID_HOTKEY_H:
		m_ComboBox.SetWindowText(_T("H"));
		break;

	case ID_HOTKEY_I:
		m_ComboBox.SetWindowText(_T("I"));
		break;

	case ID_HOTKEY_J:
		m_ComboBox.SetWindowText(_T("J"));
		break;

	case ID_HOTKEY_K:
		m_ComboBox.SetWindowText(_T("K"));
		break;

	case ID_HOTKEY_L:
		m_ComboBox.SetWindowText(_T("L"));
		break;

	case ID_HOTKEY_M:
		m_ComboBox.SetWindowText(_T("M"));
		break;

	case ID_HOTKEY_N:
		m_ComboBox.SetWindowText(_T("N"));
		break;

	case ID_HOTKEY_O:
		m_ComboBox.SetWindowText(_T("O"));
		break;

	case ID_HOTKEY_P:
		m_ComboBox.SetWindowText(_T("P"));
		break;

	case ID_HOTKEY_Q:
		m_ComboBox.SetWindowText(_T("Q"));
		break;

	case ID_HOTKEY_R:
		m_ComboBox.SetWindowText(_T("R"));
		break;

	case ID_HOTKEY_S:
		m_ComboBox.SetWindowText(_T("S"));
		break;

	case ID_HOTKEY_T:
		m_ComboBox.SetWindowText(_T("T"));
		break;

	case ID_HOTKEY_U:
		m_ComboBox.SetWindowText(_T("U"));
		break;

	case ID_HOTKEY_V:
		m_ComboBox.SetWindowText(_T("V"));
		break;

	case ID_HOTKEY_W:
		m_ComboBox.SetWindowText(_T("W"));
		break;

	case ID_HOTKEY_X:
		m_ComboBox.SetWindowText(_T("X"));
		break;

	case ID_HOTKEY_Y:
		m_ComboBox.SetWindowText(_T("Y"));
		break;

	case ID_HOTKEY_Z:
		m_ComboBox.SetWindowText(_T("Z"));
		break;

	case ID_HOTKEY_LEFT:
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 24, 0);
		break;

	case ID_HOTKEY_RIGHT:
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 25, 0);
		break;

	case ID_HOTKEY_UP:
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 26, 0);
		break;

	case ID_HOTKEY_DOWN:
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 27, 0);
		break;

	default:
		break;
	}

	// �Զ�ƥ��
	m_ComboBox.OnMatch();

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


void CAssistantDlg::OnSetCode()
{
	GetDlgItem(IDC_TIP_STATIC)->SetWindowText(_T("����: ") + FileClass + _T("      ���: ") + FileType);

	// ���
	m_ComboBox.ResetContent();

	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
		{
			continue;
		}
		else
		{
			CString Name = Finder.GetFileName();
			m_ComboBox.AddString(Name.Left(Name.GetLength() - 5));
		}
	}
}


void CAssistantDlg::OnOversee()
{
	AfxBeginThread(Operate, this);
}


void CAssistantDlg::OnDropdownCodeCombo()
{
	// ֪ͨ������
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHILDMESSAGE, 22, 0);
}


void CAssistantDlg::OnHideCheck()
{
	if(m_Hide.GetCheck())
	{
		::WritePrivateProfileString(_T("Setting"), _T("AutoHide"), _T("1"), _T("./Setting.ini"));
	}
	else
	{
		::WritePrivateProfileString(_T("Setting"), _T("AutoHide"), _T("0"), _T("./Setting.ini"));
	}
}


void CAssistantDlg::OnFollowCheck()
{
	if(m_Follow.GetCheck())
	{
		::WritePrivateProfileString(_T("Setting"), _T("Follow"), _T("1"), _T("./Setting.ini"));
	}
	else
	{
		::WritePrivateProfileString(_T("Setting"), _T("Follow"), _T("0"), _T("./Setting.ini"));
	}
}


BOOL CAssistantDlg::DestroyWindow()
{
	//ɾ���ȼ�
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_A);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_B);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_C);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_D);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_E);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_F);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_G);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_H);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_I);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_J);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_K);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_L);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_M);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_N);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_O);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_P);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_Q);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_R);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_S);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_T);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_U);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_V);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_W);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_X);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_Y);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_Z);

	::UnregisterHotKey(m_hWnd, ID_HOTKEY_LEFT);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_RIGHT);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_UP);
	::UnregisterHotKey(m_hWnd, ID_HOTKEY_DOWN);

	return CDialogEx::DestroyWindow();
}


void CAssistantDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}
