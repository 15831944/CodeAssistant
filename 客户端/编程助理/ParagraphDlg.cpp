// ParagraphDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "ParagraphDlg.h"
#include "afxdialogex.h"


// CParagraphDlg �Ի���

IMPLEMENT_DYNAMIC(CParagraphDlg, CDialogEx)

CParagraphDlg::CParagraphDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CParagraphDlg::IDD, pParent)
	, m_Index(_T(""))
	, m_Right(_T(""))
	, m_Spacing(_T(""))
	, m_Part(_T(""))
	, m_Tab(_T(""))
{
}

CParagraphDlg::~CParagraphDlg()
{
}

void CParagraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FIRST_EDIT,   m_Index);
	DDX_Text(pDX, IDC_RIGHT_EDIT,   m_Right);
	DDX_Text(pDX, IDC_SPACING_EDIT, m_Spacing);
	DDX_Text(pDX, IDC_PARAGRAPH_EDIT, m_Part);
	DDX_Control(pDX, IDC_ALIGN_COMBO, m_Align);
	DDX_Control(pDX, IDC_STYLE_COMBO, m_Style);
	DDX_Text(pDX, IDC_OFFSET_EDIT,    m_Tab);
}


BEGIN_MESSAGE_MAP(CParagraphDlg, CDialogEx)
	ON_BN_CLICKED(IDC_UNDO_BUTTON, &CParagraphDlg::OnUndo)
END_MESSAGE_MAP()


// CParagraphDlg ��Ϣ�������


BOOL CParagraphDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������
	UpdateData(false);

	// ���뷽ʽ
	m_Align.SetCurSel(Alignment);

	// �б���ʽ
	m_Style.SetCurSel(Tab);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CParagraphDlg::OnOK()
{
	// ��������
	UpdateData();

	// ���뷽ʽ
	Alignment = m_Align.GetCurSel();

	// �б���ʽ
	Tab = m_Style.GetCurSel();

	CDialogEx::OnOK();
}


void CParagraphDlg::OnUndo()
{
	// ��������
	UpdateData(false);

	// ���뷽ʽ
	m_Align.SetCurSel(Alignment);

	// �б���ʽ
	m_Style.SetCurSel(Tab);
}


void CParagraphDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialogEx::OnCancel();
}
