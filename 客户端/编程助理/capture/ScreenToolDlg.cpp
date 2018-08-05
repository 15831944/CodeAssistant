// ScreenToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../�������.h"
#include "ScreenToolDlg.h"
#include "PickColorDlg.h"
#include "MagnifierDlg.h"
#include "SnapWindowDlg.h"
#include "common.h"
#include <atlimage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CScreenToolDlg �Ի���
CScreenToolDlg::CScreenToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	IsExit  = TRUE;
}

CScreenToolDlg::~CScreenToolDlg()
{
	//ɾ���ȼ�
	UnregisterHotKey(m_hWnd, IDHOTKEYCFS);
	UnregisterHotKey(m_hWnd, IDHOTKEYCWS);
	UnregisterHotKey(m_hWnd, IDHOTKEYCRS);
	UnregisterHotKey(m_hWnd, IDHOTKEYCFWS);
	UnregisterHotKey(m_hWnd, IDHOTKEYSC);
	UnregisterHotKey(m_hWnd, IDHOTKEYSM);
}

void CScreenToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NEWCUT_BTN, m_imageCutBtn);
	DDX_Control(pDX, IDC_PICKCOLOR_BTN, m_imageColorBtn);
	DDX_Control(pDX, IDC_MAGNIFIER_BTN, m_imageMagnifierBtn);
	DDX_Control(pDX, IDC_DROPCUT_BTN, m_imageDropCutBtn);
}

BEGIN_MESSAGE_MAP(CScreenToolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_DROPCUT_BTN, &CScreenToolDlg::OnBnClickedDropcutBtn)

	ON_COMMAND(IDM_RECTCUT,    &CScreenToolDlg::OnRectcut)
	ON_COMMAND(IDM_WINDOWCUT,  &CScreenToolDlg::OnWindowcut)
	ON_COMMAND(IDM_SCREENCUT,  &CScreenToolDlg::OnScreencut)
	ON_COMMAND(IDM_CURRENTCUT, &CScreenToolDlg::OnCurrentcut)

	ON_BN_CLICKED(IDC_NEWCUT_BTN, &CScreenToolDlg::OnBnClickedNewcutBtn)
	ON_BN_CLICKED(IDC_PICKCOLOR_BTN, &CScreenToolDlg::OnBnClickedPickcolorBtn)
	ON_BN_CLICKED(IDC_MAGNIFIER_BTN, &CScreenToolDlg::OnBnClickedMagnifierBtn)
	ON_WM_HOTKEY()
	
	ON_MESSAGE(WM_TRAY_MSG, &CScreenToolDlg::OnTrayCallBackMsg)
	ON_WM_SYSCOMMAND()

	ON_COMMAND(100, &CScreenToolDlg::OnExit)
END_MESSAGE_MAP()


// CScreenToolDlg ��Ϣ�������

BOOL CScreenToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//���ز˵���Դ
	m_cutPopupMenu.LoadMenu(IDR_CAPTURE_MENU);
	CMenu *pMenu = m_cutPopupMenu.GetSubMenu(0);
	pMenu->CheckMenuRadioItem(0, 2, 0, MF_BYPOSITION);

	//m_trayPopupMenu.LoadMenu(IDR_MENU2);
	m_clipType = RECTCLIP;

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_images.Create(32, 32, ILC_COLOR32 | ILC_MASK, 1, 0);
	m_images.Add(AfxGetApp()->LoadIcon(IDI_CLIP_ICON)); //����ͼ��
	m_images.Add(AfxGetApp()->LoadIcon(IDI_SNAP_ICON));
	m_images.Add(AfxGetApp()->LoadIcon(IDI_MAGNIFIER_ICON));
	
	m_imageCutBtn.SetImageList(&m_images);				//����ͼ���б�
	m_imageMagnifierBtn.SetImageList(&m_images);
	m_imageColorBtn.SetImageList(&m_images);
	//m_imageHelpBtn.SetImageList(&m_images);

	//���ð�ť��ʾ��ͼ������
	m_imageCutBtn.SetImageIndex(0);
	m_imageColorBtn.SetImageIndex(1);
	m_imageMagnifierBtn.SetImageIndex(2);
	//m_imageHelpBtn.SetImageIndex(3);
	
	//���ð�ť����ɫ
	m_imageCutBtn.SetNormalBkColor(RGB(0xd2, 0xd2, 0xe3));
	m_imageCutBtn.SetActiveBkColor(RGB(0xd8, 0xf6, 0xfc));
	m_imageCutBtn.SetPressedBkColor(RGB(0xbb, 0xec, 0xf9));

	m_imageColorBtn.SetNormalBkColor(RGB(0xd2, 0xd2, 0xe3));
	m_imageColorBtn.SetActiveBkColor(RGB(0xd8, 0xf6, 0xfc));
	m_imageColorBtn.SetPressedBkColor(RGB(0xbb, 0xec, 0xf9));

	m_imageMagnifierBtn.SetNormalBkColor(RGB(0xd2, 0xd2, 0xe3));
	m_imageMagnifierBtn.SetActiveBkColor(RGB(0xd8, 0xf6, 0xfc));
	m_imageMagnifierBtn.SetPressedBkColor(RGB(0xbb, 0xec, 0xf9));

	m_imageDropCutBtn.SetNormalBkColor(RGB(0xd2, 0xd2, 0xe3));
	m_imageDropCutBtn.SetActiveBkColor(RGB(0xd8, 0xf6, 0xfc));
	m_imageDropCutBtn.SetPressedBkColor(RGB(0xbb, 0xec, 0xf9));

	/*m_imageHelpBtn.SetNormalBkColor(RGB(0xd2, 0xd2, 0xe3));
	m_imageHelpBtn.SetActiveBkColor(RGB(0xd8, 0xf6, 0xfc));
	m_imageHelpBtn.SetPressedBkColor(RGB(0xbb, 0xec, 0xf9));*/

	//ע�ᴰ����
	m_pszClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, 0, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	
	//ע���ȼ�
	RegisterHotKey(m_hWnd, IDHOTKEYCFS, MOD_CONTROL, 'C');
	RegisterHotKey(m_hWnd, IDHOTKEYCWS, MOD_CONTROL, 'X');
	RegisterHotKey(m_hWnd, IDHOTKEYCRS, MOD_CONTROL, 'Z');
	RegisterHotKey(m_hWnd, IDHOTKEYCFWS, MOD_CONTROL, 'A');
	RegisterHotKey(m_hWnd, IDHOTKEYSC, MOD_CONTROL, 'S');
	RegisterHotKey(m_hWnd, IDHOTKEYSM, MOD_CONTROL, 'D');
	m_bTray = FALSE;
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CScreenToolDlg::OnCancel()
{
	CDialog::OnCancel();
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CScreenToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CScreenToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CScreenToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	if (nCtlColor == CTLCOLOR_DLG)
		hbr = CreateSolidBrush(RGB(0xd2, 0xd2, 0xe3));
	return hbr;
}

BOOL CScreenToolDlg::SetTray(BOOL bAdd)
{
	BOOL bRet = FALSE;
	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = m_hWnd;
	tnd.uID = IDR_MAINFRAME;
	if ( bAdd == TRUE ) {
		tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		tnd.uCallbackMessage = WM_TRAY_MSG;
		tnd.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
		_tcscpy_s(tnd.szTip, sizeof(tnd.szTip), _T("ScreenTool v1.0"));
		bRet = Shell_NotifyIcon(NIM_ADD, &tnd);
	}else{
		bRet = Shell_NotifyIcon(NIM_DELETE, &tnd);
	}
	return bRet;
}

LRESULT CScreenToolDlg::OnTrayCallBackMsg(WPARAM wparam, LPARAM lparam)
{
	switch (lparam)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		ShowWindow(SW_RESTORE);
		break;
	case WM_RBUTTONDOWN:
		CMenu *pMenu = NULL;
		CPoint pt;
		pMenu = m_trayPopupMenu.GetSubMenu(0);
		GetCursorPos(&pt);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
		break;
	}
	return 0;
}

void CScreenToolDlg::OnBnClickedDropcutBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMenu *menu;
	menu = m_cutPopupMenu.GetSubMenu(0);
	CRect rect;
	GetDlgItem(IDC_DROPCUT_BTN)->GetWindowRect(&rect);
	menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON , rect.left, rect.bottom + 1, this);
}

void CScreenToolDlg::OnRectcut()
{
	// TODO: �ڴ���������������
	CMenu *pMenu = m_cutPopupMenu.GetSubMenu(0);
	pMenu->CheckMenuRadioItem(0, 6, 0, MF_BYPOSITION);
	m_clipType = RECTCLIP;
}

void CScreenToolDlg::OnWindowcut()
{
	// TODO: �ڴ���������������
	CMenu *pMenu = m_cutPopupMenu.GetSubMenu(0);
	pMenu->CheckMenuRadioItem(0, 6, 2, MF_BYPOSITION);
	m_clipType = WINDOWCLIP;
}

void CScreenToolDlg::OnScreencut()
{
	// TODO: �ڴ���������������
	CMenu *pMenu = m_cutPopupMenu.GetSubMenu(0);
	pMenu->CheckMenuRadioItem(0, 6, 4, MF_BYPOSITION);
	m_clipType = SCREENCLIP;
}

void CScreenToolDlg::OnCurrentcut()
{
	CMenu *pMenu = m_cutPopupMenu.GetSubMenu(0);
	pMenu->CheckMenuRadioItem(0, 6, 6, MF_BYPOSITION);
	m_clipType = CURRENTCLIP;
}

void CScreenToolDlg::OnBnClickedNewcutBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	//
	
	switch (m_clipType)
	{
	case RECTCLIP:
		ClipRectOfScreen();
		break;
	case WINDOWCLIP:
		ClipWindowOfScreen();
		break;
	case SCREENCLIP:
		ClipFullScreenToFile();
		break;
	case CURRENTCLIP:
		ClipForegroundWindow();
		break;
	default:
		break;
	}
}

void CScreenToolDlg::ClipWindowOfScreen()
{
	CSnapWindowDlg dlg;
	ShowWindow(SW_HIDE);
	dlg.IsExit = IsExit;
	dlg.DoModal();
	ShowWindow(SW_SHOW);
}

//���������ͼ
void CScreenToolDlg::ClipRectOfScreen()
{
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = GetSystemMetrics(SM_CXSCREEN);
	rect.bottom = GetSystemMetrics(SM_CYSCREEN);

	//����Ӧ�ó��򴰿�
	ShowWindow(SW_HIDE);
	Sleep(300);

	//����ȫ����ͼ����
	if( !m_fullScreenWnd.CreateEx(0, m_pszClassName, _T("FULLSCREENWND"), /*WS_CHILD |*/ WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, this->GetSafeHwnd(), NULL) )
		AfxMessageBox(_T("���ڴ���ʧ��!"));
	else
	{
		m_fullScreenWnd.IsExit = IsExit;
		m_fullScreenWnd.ShowWindow(SW_SHOW);
	}
}

//��ȡ��ǰǰ������
void CScreenToolDlg::ClipForegroundWindow()
{
	//�����ļ�·��
	CString strPathName;

	//������Ļ��������
	RECT rect;

	//����λͼ���
	HBITMAP hBitmap;

	//����Ӧ�ó��򴰿�
	ShowWindow(SW_HIDE);
	Sleep(300);

	//��ȡ��ǰǰ������
	HWND hWnd = ::GetForegroundWindow();

	//��ȡ��Ļ��������
	::GetWindowRect(hWnd, &rect);

	//��ȡ��Ļ��������λͼ���
	hBitmap = CopyScreenToBitmap(&rect);

	//��ʾӦ�ó��򴰿�
	ShowWindow(SW_SHOW);

	if( MessageBox(_T("�Ƿ񱣴����Ϊ�ļ�?"), _T("���Ϊѡ��"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		//�����ļ��Ի������
		CFileDialog fd(FALSE, _T("png"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, NULL);

		//�����ļ�������
		fd.m_ofn.lpstrFilter = _T("PNG �ļ�\0*.png\0JPEG �ļ�\0*.jpg\0λͼ�ļ�\0*.bmp\0All Files\0*.*\0\0");

		//���ļ��Ի���
		fd.DoModal();

		//��ȡѡ����ļ�·��
		strPathName = fd.GetPathName();

		//�ж��ļ�·���Ƿ�Ϊ��
		if(strPathName.IsEmpty())
		{
			// ����hBitmap����
			DeleteObject(hBitmap);
			return;
		}

		//����CImage����,������λͼ���浽һ���ļ���
		CImage image;

		//��λͼ���������image����
		image.Attach(hBitmap);

		//��λͼ���浽һ���ļ���
		image.Save(strPathName);

		//����image����
		image.Destroy();

		// ����hBitmap����
		DeleteObject(hBitmap);
	}
	else
	{
		// ���浽������
		if(::OpenClipboard(m_hWnd))
		{
			::EmptyClipboard();
			::SetClipboardData(CF_BITMAP, hBitmap);
			::CloseClipboard();
		}

		// ����hBitmap����
		DeleteObject(hBitmap);

		// �˳�����
		if(IsExit)
			OnExit();
	}
}

//ȫ����ͼ
void CScreenToolDlg::ClipFullScreenToFile()
{
	//�����ļ�·��
	CString strPathName;

	//������Ļ��������
	RECT rect;

	//����λͼ���
	HBITMAP hBitmap;

	//��ȡ��Ļ��������
	rect.left = 0;
	rect.top = 0;
	rect.right = GetSystemMetrics(SM_CXSCREEN);
	rect.bottom = GetSystemMetrics(SM_CYSCREEN);

	//���س���������
	ShowWindow(SW_HIDE);
	Sleep(300);

	//��ȡ��Ļ��������λͼ���
	hBitmap = CopyScreenToBitmap(&rect);
	ShowWindow(SW_SHOW);

	if( MessageBox(_T("�Ƿ񱣴����Ϊ�ļ�?"), _T("���Ϊѡ��"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		//�����ļ��Ի������
		CFileDialog fd(FALSE, _T("png"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, NULL);

		//�����ļ�������
		fd.m_ofn.lpstrFilter = _T("PNG �ļ�\0*.png\0JPEG �ļ�\0*.jpg\0λͼ�ļ�\0*.bmp\0All Files\0*.*\0\0");

		//���ļ��Ի���
		fd.DoModal();

		//��ȡѡ����ļ�·��
		strPathName = fd.GetPathName();

		//�ж��ļ�·���Ƿ�Ϊ��
		if(strPathName.IsEmpty())
		{
			// ����hBitmap����
			DeleteObject(hBitmap);
			return;
		}

		//����CImage����,������λͼ���浽һ���ļ���
		CImage image;

		//��λͼ���������image����
		image.Attach(hBitmap);

		//��λͼ���浽һ���ļ���
		image.Save(strPathName);

		//����image����
		image.Destroy();

		// ����hBitmap����
		DeleteObject(hBitmap);
	}
	else
	{
		// ���浽������
		if(::OpenClipboard(m_hWnd))
		{
			::EmptyClipboard();
			::SetClipboardData(CF_BITMAP, hBitmap);
			::CloseClipboard();
		}

		// ����hBitmap����
		DeleteObject(hBitmap);

		// �˳�����
		if(IsExit)
			OnExit();
	}
}

void CScreenToolDlg::OnBnClickedPickcolorBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//������ɫʰɫ���Ի���
	CPickColorDlg dlg;
	dlg.DoModal();
}

void CScreenToolDlg::OnBnClickedMagnifierBtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//����Ŵ󾵶Ի���
	CMagnifierDlg dlg;
	dlg.DoModal();
}

void CScreenToolDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	CDialog *pDlg;
	switch (nHotKeyId)
	{
	case IDHOTKEYCFS:
		ClipFullScreenToFile();
		break;
	case IDHOTKEYCWS:
		ClipWindowOfScreen();
		break;
	case IDHOTKEYCRS:
		ClipRectOfScreen();
		break;
	case IDHOTKEYCFWS:
		ClipForegroundWindow();
		break;
	case IDHOTKEYSC:
		pDlg = new CPickColorDlg;
		pDlg->DoModal();
		delete pDlg;
		break;
	case IDHOTKEYSM:
		pDlg = new CMagnifierDlg;
		pDlg->DoModal();
		delete pDlg;
		break;
	default:
		break;
	}
}

void CScreenToolDlg::OnBnClickedHelpBtn()
{
}

void CScreenToolDlg::OnShowMenu()
{
	// TODO: �ڴ���������������
	ShowWindow(SW_RESTORE);
}

void CScreenToolDlg::OnHelpMenu()
{
}

void CScreenToolDlg::OnExitMenu()
{
	// TODO: �ڴ���������������
	SendMessage(WM_CLOSE, 0, 0);
}

void CScreenToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialog::OnSysCommand(nID, lParam);
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
		if (!m_bTray)
		{
			SetTray(TRUE);
			m_bTray = TRUE;
		}
	}
}

void CScreenToolDlg::OnExit()
{
	CDialog::OnOK();
}
