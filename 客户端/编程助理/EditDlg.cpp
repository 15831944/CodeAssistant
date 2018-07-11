// EditDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "EditDlg.h"
#include "afxdialogex.h"


// CEditDlg �Ի���

IMPLEMENT_DYNAMIC(CEditDlg, CDialogEx)

CEditDlg::CEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditDlg::IDD, pParent)
{

}

CEditDlg::~CEditDlg()
{
}

void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLASS_EDIT, m_Class);
	DDX_Control(pDX, IDC_TYPE_EDIT, m_Type);
}


BEGIN_MESSAGE_MAP(CEditDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CEditDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CEditDlg::OnCancel)
END_MESSAGE_MAP()


// CEditDlg ��Ϣ�������


BOOL CEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if(IsFunction)
	{
		SetWindowText(_T("�༭����"));

		m_Class.EnableWindow(false);
		GetDlgItem(IDC_TYPE_STATIC)->SetWindowText(_T("����: "));
		m_Type.SetWindowText(Type);
	}
	else
	{
		m_Class.SetWindowText(Class);
		m_Type.SetWindowText(Type);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CEditDlg::OnOK()
{
	CString new_Class, new_Type;
	m_Class.GetWindowText(new_Class);
	m_Type.GetWindowText(new_Type);

	// �޸ķ���
	if(!IsFunction)
	{
		// û���޸�
		if(new_Class == Class && new_Type == Type)
			CDialogEx::OnCancel();
		else
		{
			Class = new_Class;
			Type  = new_Type;

			CDialogEx::OnOK();
		}
	}
	else
	{
		// û���޸�
		if(new_Type == Type)
			CDialogEx::OnCancel();
		else
		{
			Type  = new_Type;
			CDialogEx::OnOK();
		}
	}
}


void CEditDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
