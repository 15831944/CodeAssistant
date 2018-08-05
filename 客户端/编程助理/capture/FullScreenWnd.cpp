// SreenWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FullScreenWnd.h"
#include <atlimage.h>
#include "common.h"
// CSreenWnd



CFullScreenWnd::CFullScreenWnd()
{
	m_brush.CreateSolidBrush(RGB(0, 0, 255));
	//��ȡ4�ֱ�׼�����Դ���
	hCursor[0] = LoadCursor(NULL, IDC_CROSS);
	hCursor[1] = LoadCursor(NULL, IDC_SIZENS);
	hCursor[2] = LoadCursor(NULL, IDC_SIZEWE);
	hCursor[3] = LoadCursor(NULL, IDC_SIZEALL);
	hCursor[4] = LoadCursor(NULL, IDC_ARROW);
	m_catchMouse = TRUE;

	IsExit = TRUE;
}

CFullScreenWnd::~CFullScreenWnd()
{
}

BEGIN_MESSAGE_MAP(CFullScreenWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
// CSreenWnd ��Ϣ�������
BOOL CFullScreenWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//������Ļ���,�߶�
	int nWidth, nHeight;
	//��ȡ��Ļ��Ⱥ͸߶�
	nWidth = GetSystemMetrics(SM_CXSCREEN);
	nHeight = GetSystemMetrics(SM_CYSCREEN);
	
	pDC->BitBlt(0, 0, nWidth, nHeight, &m_compatibleDC, 0, 0, SRCCOPY);
	return TRUE;
}


void CFullScreenWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_bClip)
		SetCursor(hCursor[0]);
	else
	{
		if (m_catchMouse)
		{
		m_mousePos = CalPointPosition(&m_drawRect, point);
		if (m_mousePos == UPPER || m_mousePos == DOWN)
			SetCursor(hCursor[1]);
		else if(m_mousePos == INNER)
			SetCursor(hCursor[3]);
		else if(m_mousePos == LEFT || m_mousePos == RIGHT)
			SetCursor(hCursor[2]);
		else if(m_mousePos == OUTSIDE)
			SetCursor(hCursor[4]);
		}
		
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CFullScreenWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CWnd::OnRButtonDown(nFlags, point);
}

void CFullScreenWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//this->ShowWindow(SW_HIDE);
	this->GetParent()->ShowWindow(SW_SHOW);
	m_bClip = TRUE;
	this->DestroyWindow();

	CWnd::OnRButtonUp(nFlags, point);
}

void CFullScreenWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//���ö�ʱ��

	SetTimer(888, 30, NULL);
	m_oriPoint.x = point.x;
	m_oriPoint.y = point.y;
	m_catchMouse = FALSE;
	CWnd::OnLButtonDown(nFlags, point);
}

void CFullScreenWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	KillTimer(888);
	if (m_bClip)
		m_bClip = FALSE;
	m_catchMouse = TRUE;
	ShowTip();
	CWnd::OnLButtonUp(nFlags, point);
}

//����:��ʾ��ʾ��Ϣ
void CFullScreenWnd::ShowTip()
{
	CDC *pDC;
	pDC = GetDC();
	RECT rt;
	CSize size;
	int x, y;
	//������Ļ���,�߶�
	int nWidth, nHeight;
	//��ȡ��Ļ��Ⱥ͸߶�
	nWidth = GetSystemMetrics(SM_CXSCREEN);
	nHeight = GetSystemMetrics(SM_CYSCREEN);
	//���ñ���Ϊ͸��
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 0, 0));
	CString str(_T("��ܰ��ʾ: ˫�����Ϊ����ͼƬ,�����Ҽ�Ϊȡ��"));
	size = pDC->GetTextExtent(str);
	if (m_drawRect.bottom + size.cy > nHeight)
		y = m_drawRect.top - size.cy - 4;
	else
		y = m_drawRect.bottom + 4;
	x = m_drawRect.right - size.cx;
	if (m_drawRect.right - size.cx < 0)
		x = 0;
	if (m_drawRect.right > nWidth)
		x = nWidth - size.cx;
	rt.left = x;
	rt.top = y;
	rt.right = x + size.cx;
	rt.bottom = y + size.cy;
	pDC->FrameRect(&rt, &m_brush);
	pDC->TextOut(x, y, str, str.GetLength());
	ReleaseDC(pDC);
}

int CFullScreenWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
	{
		AfxMessageBox(_T("���ڴ���ʧ��!"));
		return -1;
	}

	// TODO:  �ڴ������ר�õĴ�������
	m_bClip = TRUE;	
	CRect rect;
	if (m_compatibleDC.m_hDC == NULL)
	{
		CDC *theDC;
		theDC = GetDC();
		m_compatibleDC.CreateCompatibleDC(theDC);
		ReleaseDC(theDC);
	}
	rect.left = 0;
	rect.top = 0;
	rect.right = GetSystemMetrics(SM_CXSCREEN);
	rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	m_hScreenBmp = CopyScreenToBitmap(&rect);
	CBitmap bitmap, *hOldBitmap;
	bitmap.Attach(m_hScreenBmp);
	hOldBitmap = m_compatibleDC.SelectObject(&bitmap);
	hOldBitmap->DeleteObject();
	return 0;
}

void CFullScreenWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
}

/*
	����:
		������������ڻ��Ƶľ��������е�λ��
	����:
		lpRect ָ����Ƶľ�������
		point ���λ������
	����ֵ:
		����ڻ��Ƶľ��������е�λ��
*/
MOUSEPOSITION CFullScreenWnd::CalPointPosition(LPRECT lpRect, POINT point)
{
	//���巵�ؽ��
	MOUSEPOSITION retPos;
	//�������������,�߶�
	int nWidth, nHeight;
	//����������������λ��
	int x1, x2, y1, y2;
	//��ȡ�������������λ��
	x1 = lpRect->left;
	x2 = lpRect->right;
	y1 = lpRect->top;
	y2 = lpRect->bottom;	
	//�������������, �߶�
	nWidth = x2 - x1;
	nHeight = y2 - y1;
	//��������Ƿ��ھ���������ڲ�
	if ((point.x > x1 && point.x < x2) &&(point.y > y1 && point.y < y2))
		retPos = INNER;
	//��������Ƿ��ھ��������������������λ�ÿ�ʼ������2�����ط�Χ��λ��
	else if ((point.x >= x1 + nWidth/2 - 2 && point.x <= x1 + nWidth/2 + 2)
			&&(point.y >= y1 - 2 && point.y <= y1 + 2))
		retPos = UPPER;
	//��������Ƿ��ھ��������������������λ�ÿ�ʼ������2�����ط�Χ��λ��
	else if ((point.x >= x1 + nWidth/2 - 2 && point.x <= x1 + nWidth/2 + 2)
		&&(point.y >= y2 - 2 && point.y <= y2 + 2))
		retPos = DOWN;
	//��������Ƿ��ھ��������������������λ�ÿ�ʼ������2�����ط�Χ��λ��
	else if ((point.x >= x1 - 2 && point.x <= x1 + 2)
		&&(point.y >= y1 + nHeight/2 - 2 && point.y <= y1 + nHeight/2 + 2))
		retPos = LEFT;
	//��������Ƿ��ھ��������������������λ�ÿ�ʼ������2�����ط�Χ��λ��
	else if ((point.x >= x2 - 2 && point.x <= x2 + 2)
		&&(point.y >= y1 + nHeight/2 - 2 && point.y <= y1 + nHeight/2 + 2))
		retPos = RIGHT;
	else
		retPos = OUTSIDE;
	return retPos;
}

void CFullScreenWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	TrackDraw();
}

//��̬����
void CFullScreenWnd::TrackDraw()
{
	CClientDC clientDC(this);
	int nWidth, nHeight;
	//��ȡ��Ļ��Ⱥ͸߶�
	nWidth = GetSystemMetrics(SM_CXSCREEN);
	nHeight = GetSystemMetrics(SM_CYSCREEN);
	POINT point;
	//�õ���ǰ����λ��
	GetCursorPos(&point);
	//�ж��Ƿ��ǽ�ͼ��־
	if (m_bClip)
	{
		//��ȡ��������
		m_drawRect.left = m_oriPoint.x<point.x?m_oriPoint.x:point.x;
		m_drawRect.top  = m_oriPoint.y<point.y?m_oriPoint.y:point.y;
		m_drawRect.right = m_oriPoint.x<point.x?point.x:m_oriPoint.x;
		m_drawRect.bottom  = m_oriPoint.y<point.y?point.y:m_oriPoint.y;
		//����
		clientDC.BitBlt(0, 0, nWidth, nHeight, &m_compatibleDC, 0, 0, SRCCOPY);
		//���ƾ���
		DrawRect(&clientDC, &m_drawRect, &m_brush);
	}
	else 
	{
		int moveWidth, width, moveHeight, height;
		int temp;
		if (m_mousePos == OUTSIDE)
			return;
		//��������ƶ��ľ���
		moveWidth = point.x - m_oriPoint.x;
		moveHeight = point.y - m_oriPoint.y;
		//�ж�����λ��
		switch (m_mousePos)
		{
			//�ھ���������ڲ�
		case INNER:
			//��ȡ���εĿ�Ⱥ͸߶�
			width = m_drawRect.right - m_drawRect.left;
			height = m_drawRect.bottom - m_drawRect.top;
			//���¼�����������λ��
			m_drawRect.left += moveWidth;
			m_drawRect.top += moveHeight;
			m_drawRect.right = m_drawRect.left + width;
			m_drawRect.bottom = m_drawRect.top + height;	
			break;
			//�ھ���������ϲ�
		case UPPER:
			if (m_drawRect.top > m_drawRect.bottom)
			{
				
				temp = m_drawRect.top;
				m_drawRect.top = m_drawRect.bottom;
				m_drawRect.bottom = temp;
				m_mousePos = DOWN;
			}
			else
				m_drawRect.top += moveHeight;
			break;
			//�ھ���������²�
		case DOWN:
			if (m_drawRect.bottom < m_drawRect.top)
			{

				temp = m_drawRect.top;
				m_drawRect.top = m_drawRect.bottom;
				m_drawRect.bottom = temp;
				m_mousePos = UPPER;
			}
			else
				m_drawRect.bottom += moveHeight;
			break;
			//�ھ����������
		case LEFT:
			if (m_drawRect.left > m_drawRect.right)
			{

				temp = m_drawRect.left;
				m_drawRect.left = m_drawRect.right;
				m_drawRect.right = temp;
				m_mousePos = RIGHT;
			}
			else
				m_drawRect.left += moveWidth;
			break;
			//�ھ���������Ҳ�
		case RIGHT:
			if (m_drawRect.right < m_drawRect.left)
			{

				temp = m_drawRect.left;
				m_drawRect.left = m_drawRect.right;
				m_drawRect.right = temp;
				m_mousePos = LEFT;
			}
			m_drawRect.right += moveWidth;
			break;
		default:
			break;
		}	
		//����
		clientDC.BitBlt(0, 0, nWidth, nHeight, &m_compatibleDC, 0, 0, SRCCOPY);
		//���ƾ���
		DrawRect(&clientDC, &m_drawRect, &m_brush);
		//��������ԭ��
		m_oriPoint = point;	
	}
	ReleaseDC(&clientDC);
}

/*
	����:
		��ָ�����豸�������л�����,�Լ��ھ����������ϵ����Ļ�С���ο�
	����:
		pDC ָ���豸������ľ��
		pRect ָ�����ʦ�ľ��ε�����
		pBrush ָ�����ʱʹ�õĻ�ˢ
	����ֵ:
		void 
*/
void CFullScreenWnd::DrawRect(CDC *pDC, RECT *pRect, CBrush *pBrush)
{
	//������εĿ��,�߶�
	int width, height;
	//���廭���ο���Ҫ����ʱ����
	RECT rt;
	CBrush *oldBrush;
	CPen pen, *oldPen;
	//������εĿ�Ⱥ͸߶�
	width = pRect->right - pRect->left;
	height = pRect->bottom - pRect->top;
	//�����ο�
	pDC->FrameRect(pRect, pBrush);
	
	//��������
	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(pBrush);

	//�ھ��ε������ߵ�����λ�û���4���صľ��ο�
	//�ھ��ο�������ߵ�����λ�û��ƾ���
	rt.left = pRect->left + width/2 - 2;
	rt.right = pRect->left + width/2 + 2;
	rt.top = pRect->top - 2;
	rt.bottom = pRect->top + 2;
	pDC->Rectangle(&rt);

	//�ھ��ο�������ߵ�����λ�û��ƾ���
	rt.left = pRect->left + width/2 - 2;
	rt.right = pRect->left + width/2 + 2;
	rt.top = pRect->bottom - 2;
	rt.bottom = pRect->bottom + 2;
	pDC->Rectangle(&rt);

	//�ھ��ο�������ߵ�����λ�û��ƾ���
	rt.left = pRect->left - 2;
	rt.right = pRect->left + 2;
	rt.top = pRect->top + height/2 - 2;
	rt.bottom = pRect->top + height/2 + 2;
	pDC->Rectangle(&rt);

	//�ھ��ο�������ߵ�����λ�û��ƾ���
	rt.left = pRect->right - 2;
	rt.right = pRect->right + 2;
	rt.top = pRect->top + height/2 - 2;
	rt.bottom = pRect->top + height/2 + 2;
	pDC->Rectangle(&rt);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pen.DeleteObject();	
}

//������Ļ��������һ���ļ�
void CFullScreenWnd::Save()
{
	//�õ���������λͼ���
	HBITMAP hBitmap = CopyDCToBitmap(&m_drawRect, m_compatibleDC.m_hDC);

	if( MessageBox(_T("�Ƿ񱣴����Ϊ�ļ�?"), _T("���Ϊѡ��"), MB_ICONQUESTION | MB_YESNO) == IDYES )
	{
		//���ļ��Ի���
		CFileDialog fd(FALSE, _T("png"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, NULL);
		fd.m_ofn.lpstrFilter = _T("PNG �ļ�\0*.png\0JPEG �ļ�\0*.jpg\0λͼ�ļ�\0*.bmp\0All Files\0*.*\0\0");
		fd.DoModal();

		//��ȡ�ļ�·��
		CString strPathName = fd.GetPathName();
		if(strPathName.IsEmpty())
		{
			// ����hBitmap����
			DeleteObject(hBitmap);

			//�򴰿ڷ�������Ҽ�������Ϣ
			SendMessage(WM_RBUTTONUP, 0, 0);
			return;
		}

		//����CImage����,������λͼ���浽һ���ļ���
		CImage image;

		//��λͼ�����CImgae�������
		image.Attach(hBitmap);

		//���浽�ļ�
		image.Save(strPathName);

		//����image����
		image.Destroy();

		// ����hBitmap����
		DeleteObject(hBitmap);

		//�򴰿ڷ�������Ҽ�������Ϣ
		SendMessage(WM_RBUTTONUP, 0, 0);
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

		// ������ʶ
		m_bClip = TRUE;
		if(IsExit)
		{
			// ������Ϣ��������
			::SendMessage(this->GetParent()->GetSafeHwnd(), WM_COMMAND, 100, 0);
			this->DestroyWindow();
		}
		else
			//�򴰿ڷ�������Ҽ�������Ϣ
			SendMessage(WM_RBUTTONUP, 0, 0);
	}
}

void CFullScreenWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//����λͼ
	Save();
}
