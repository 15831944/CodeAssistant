// PickColorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../�������.h"
#include "PickColorDlg.h"
// CPickColorDlg �Ի���
CPickColorDlg::CPickColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPickColorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_SNAP_ICON);
	m_bPause = FALSE;
}

CPickColorDlg::~CPickColorDlg()
{
	
}
BOOL CPickColorDlg::OnInitDialog()
{
	SetIcon(m_hIcon, FALSE);
	RegisterHotKey(m_hWnd, ID_HOTKEYPAUSE, MOD_CONTROL, 'Q');
	RegisterHotKey(m_hWnd, ID_HOTKEYSTART, MOD_CONTROL, 'W');
	SetTimer(222, 100, NULL);
	return TRUE;
}
void CPickColorDlg::OnCancel()
{
	KillTimer(222);
	DestroyIcon(m_hIcon);
	UnregisterHotKey(m_hWnd, ID_HOTKEYSTART);
	UnregisterHotKey(m_hWnd, ID_HOTKEYPAUSE);
	CDialog::OnCancel();
}
void CPickColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPickColorDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_HOTKEY()
END_MESSAGE_MAP()


// CPickColorDlg ��Ϣ�������
void CPickColorDlg::ShowColorValue()
{
	//�����豸��������
	CDC *pDC;
	RECT rt;
	RECT clientRect;
	//�õ��ͻ����ľ�������
	GetClientRect(&clientRect);
	rt.left = clientRect.left + 20;
	rt.top = clientRect.top + 10;
	rt.right = clientRect.right - 20;
	rt.bottom = 50;
	//���建����
	TCHAR buff[50];
	CBrush brush;
	//������ɫ��ˢ
	brush.CreateSolidBrush(m_color);
	//��ȡ�ͻ����豸��������
	pDC = GetDC();
	//�û�ˢ����������
	pDC->FillRect(&rt, &brush);
	brush.DeleteObject();
	
	wsprintf(buff, _T("%d,%d,%d"), GetRValue(m_color), GetGValue(m_color), GetBValue(m_color));
	GetDlgItem(IDC_RGBCOLOR_EDIT)->SetWindowText(buff);
	wsprintf(buff, _T("#%08X"), m_color);
	GetDlgItem(IDC_PAGECOLOR_EDIT)->SetWindowText(buff);
	wsprintf(buff, _T("0x%08X"), m_color);
	GetDlgItem(IDC_HEXCOLOR_EDIT)->SetWindowText(buff);
	ReleaseDC(pDC);
}
void CPickColorDlg::GetColorOfScreen()
{
	//������Ļ�豸���
	HDC hScreenDC;
	POINT pt;
	//��ȡ���λ��
	GetCursorPos(&pt);
	//�õ���Ļ�豸���
	hScreenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	//�õ���Ļָ�������ɫֵ
	m_color = GetPixel(hScreenDC, pt.x, pt.y);
	DeleteDC(hScreenDC);
}
void CPickColorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (!m_bPause)
	{
		//�õ����λ�õ���Ļ��ɫֵ
		GetColorOfScreen();
		//��ʾ��ȡ������ɫֵ
		ShowColorValue();
		
	}	
}

void CPickColorDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nHotKeyId == ID_HOTKEYPAUSE)
		m_bPause = TRUE;
	else if (nHotKeyId == ID_HOTKEYSTART)
		m_bPause = FALSE;
	CDialog::OnHotKey(nHotKeyId, nKey1, nKey2);
}
