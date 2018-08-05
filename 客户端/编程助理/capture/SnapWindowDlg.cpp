// SnapWindowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../�������.h"
#include "SnapWindowDlg.h"
#include "common.h"
#include <atlimage.h>
// CSnapWindowDlg �Ի���
CSnapWindowDlg::CSnapWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSnapWindowDlg::IDD, pParent)
{
	m_hCursor = AfxGetApp()->LoadCursor(IDC_TARGET_CURSOR);
	m_rectOfWndBmp = NULL;
	m_preWnd = NULL;
	m_curWnd = NULL;
	m_bSnap = FALSE;

	IsExit = TRUE;
}

CSnapWindowDlg::~CSnapWindowDlg()
{
}

BOOL CSnapWindowDlg::OnInitDialog()
{
	return TRUE;
}

void CSnapWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CSnapWindowDlg::ClearScreen()
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

void CSnapWindowDlg::SnapWindow(POINT point)
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

void CSnapWindowDlg::SaveBmpToFile()
{
	//����Ӧ�ó��򴰿�
	ShowWindow(SW_HIDE);
	Sleep(300);

	HBITMAP hBitmap = CopyScreenToBitmap(&m_rectOfWnd);

	//�����ļ�·��
	CString strPathName;

	if( MessageBox(_T("�Ƿ񱣴����Ϊ�ļ�?"), _T("���Ϊѡ��"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		//�����ļ��Ի������
		CFileDialog fd(FALSE, _T("jpg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, NULL);

		//�����ļ�������
		fd.m_ofn.lpstrFilter = _T("PNG �ļ�\0*.png\0JPEG �ļ�\0*.jpg\0λͼ�ļ�\0*.bmp\0All Files\0*.*\0\0");

		//���ļ��Ի���
		fd.DoModal();

		//��ȡѡ����ļ�·��
		strPathName = fd.GetPathName();

		//�ж��ļ�·���Ƿ�Ϊ��
		if(strPathName.IsEmpty())
		{
			//����hBitmap����
			DeleteObject(hBitmap);

			ShowWindow(SW_SHOW);
		}

		//����CImage����,������λͼ���浽һ���ļ���
		CImage image;

		//��λͼ���������image����
		image.Attach(hBitmap);

		//��λͼ���浽һ���ļ���
		image.Save(strPathName);

		//����image����
		image.Destroy();

		//����hBitmap����
		DeleteObject(hBitmap);

		// ��ʾ����
		ShowWindow(SW_SHOW);
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

		//����hBitmap����
		DeleteObject(hBitmap);

		if(IsExit)
		{
			CDialog::OnOK();

			// ������Ϣ��������
			::SendMessage(this->GetParent()->GetSafeHwnd(), WM_COMMAND, 100, 0);
		}
		else
			ShowWindow(SW_SHOW);
	}
}

BEGIN_MESSAGE_MAP(CSnapWindowDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CSnapWindowDlg ��Ϣ�������

void CSnapWindowDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	SetCursor(m_hCursor);
	SetCapture();
	m_bSnap = TRUE;
	CDialog::OnLButtonDown(nFlags, point);
}

void CSnapWindowDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	ReleaseCapture();
	ClearScreen();
	SaveBmpToFile();
	m_preWnd = NULL;
	m_curWnd = NULL;
	m_bSnap = FALSE;
	
}

void CSnapWindowDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_bSnap)
		SnapWindow(point);
}
