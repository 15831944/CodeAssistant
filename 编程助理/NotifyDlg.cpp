// NotifyDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "NotifyDlg.h"
#include "afxdialogex.h"


// CNotifyDlg �Ի���

IMPLEMENT_DYNAMIC(CNotifyDlg, CDialogEx)

CNotifyDlg::CNotifyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNotifyDlg::IDD, pParent)
{
}

CNotifyDlg::~CNotifyDlg()
{
}

void CNotifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNotifyDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CNotifyDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CNotifyDlg::OnCancel)
END_MESSAGE_MAP()


// CNotifyDlg ��Ϣ�������


BOOL CNotifyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AfxBeginThread(NotifyThread, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


// ��Ϣ��ʾ�߳�
UINT CNotifyDlg::NotifyThread(LPVOID lpParameter)
{
	CNotifyDlg * pDlg = (CNotifyDlg*)lpParameter;

	// �ȴ��������
	while (!theApp.IsFinished)
	{
		Sleep(50);
	}
	
	pDlg->EndDialog(TRUE);
	return TRUE;
}


void CNotifyDlg::OnOK()
{
}


void CNotifyDlg::OnCancel()
{
}
