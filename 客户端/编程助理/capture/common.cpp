#include "stdafx.h"
/*
	����:������Ļ�������������
	����:
		lpRect ָ����Ҫ�����ľ�������
	����ֵ:
		ָ�򱣴���Ļ���ص�λͼ���
*/
HBITMAP CopyScreenToBitmap(LPRECT lpRect)
{
	//������Ļ���ڴ��豸������
	HDC hScreenDC, hMemDC;
	//����λͼ���
	HBITMAP hBitmap, hOldBitmap; 
	//����ѡ�������ĸ������
	int x1, x2, y1, y2;
	//����ѡ��������,�߶�
	int nWidth, nHeight;
	int nScreenWidth, nScreenHeight;
	//�����������Ϊ��,��ֱ�ӷ���NULL
	if (IsRectEmpty(lpRect))
		return NULL;
	//��ȡѡ����������
	x1 = lpRect->left;
	x2 = lpRect->right;
	y1 = lpRect->top;
	y2 = lpRect->bottom;
	//��ȡ��Ļ��Ⱥ͸߶�
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	//ȷ��ѡ�������ǿɼ���
	if (x1 < 0)
		x1 = 0;
	if (x2 > nScreenWidth)
		x2 = nScreenWidth;
	if (y1 < 0)
		y1 = 0;
	if (y2 > nScreenHeight)
		y2 = nScreenHeight;
	//����ѡ�������Ⱥ͸߶�
	nWidth = x2 - x1;
	nHeight = y2 - y1;
	//������Ļ�豸������
	hScreenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	//Ϊ��Ļ�豸�����������ݵ��ڴ��豸������
	hMemDC = CreateCompatibleDC(hScreenDC);
	// ����һ������Ļ�豸��������ݵ�λͼ
	hBitmap = CreateCompatibleBitmap(hScreenDC, nWidth, nHeight);
	//����λͼѡ���豸��������
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	//������Ļλͼ
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScreenDC, x1, y1, SRCCOPY);
	//�õ���Ļλͼ���
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	//ɾ���豸������
	DeleteDC(hMemDC);
	DeleteDC(hScreenDC);
	//����λͼ���
	return hBitmap;
}
/*
	����:�����豸��������ָ���ľ����������ص�һ��λͼ����
	����:
		lpRect ָ����Ҫ�����ľ�������
		hSrcDC �豸��������
	����ֵ:
		ָ�򱣴�λͼ���ص�λͼ���
*/
HBITMAP CopyDCToBitmap(LPRECT lpRect, HDC hSrcDC)
{
	
	HDC hMemDC;
	//����λͼ���
	HBITMAP hDestBitmap, hOldBitmap; 
	//����ѡ�������ĸ������
	int x1, x2, y1, y2;
	//����ѡ��������,�߶�
	int nWidth, nHeight;
	int nScreenWidth, nScreenHeight;
	//�����������Ϊ��,��ֱ�ӷ���NULL
	if (IsRectEmpty(lpRect))
		return NULL;
	//��ȡѡ����������
	x1 = lpRect->left;
	x2 = lpRect->right;
	y1 = lpRect->top;
	y2 = lpRect->bottom;
	//��ȡ��Ļ��Ⱥ͸߶�
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	//ȷ��ѡ�������ǿɼ���
	if (x1 < 0)
		x1 = 0;
	if (x2 > nScreenWidth)
		x2 = nScreenWidth;
	if (y1 < 0)
		y1 = 0;
	if (y2 > nScreenHeight)
		y2 = nScreenHeight;
	//����ѡ�������Ⱥ͸߶�
	nWidth = x2 - x1;
	nHeight = y2 - y1;
	//����λͼ���
	hMemDC = CreateCompatibleDC(hSrcDC);
	hDestBitmap = CreateCompatibleBitmap(hSrcDC, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hDestBitmap);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hSrcDC, x1, y1, SRCCOPY);
	hDestBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);
	return hDestBitmap;
}
