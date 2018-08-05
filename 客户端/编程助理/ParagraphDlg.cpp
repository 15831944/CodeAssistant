// ParagraphDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "ParagraphDlg.h"
#include "afxdialogex.h"


// CParagraphDlg 对话框

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


// CParagraphDlg 消息处理程序


BOOL CParagraphDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 更新数据
	UpdateData(false);

	// 对齐方式
	m_Align.SetCurSel(Alignment);

	// 列表样式
	m_Style.SetCurSel(Tab);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CParagraphDlg::OnOK()
{
	// 更新数据
	UpdateData();

	// 对齐方式
	Alignment = m_Align.GetCurSel();

	// 列表样式
	Tab = m_Style.GetCurSel();

	CDialogEx::OnOK();
}


void CParagraphDlg::OnUndo()
{
	// 更新数据
	UpdateData(false);

	// 对齐方式
	m_Align.SetCurSel(Alignment);

	// 列表样式
	m_Style.SetCurSel(Tab);
}


void CParagraphDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnCancel();
}
