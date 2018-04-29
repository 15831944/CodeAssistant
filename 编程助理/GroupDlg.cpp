// GroupDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "GroupDlg.h"
#include "afxdialogex.h"

#include "SearchDlg.h"

// CGroupDlg �Ի���

IMPLEMENT_DYNAMIC(CGroupDlg, CDialogEx)

CGroupDlg::CGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGroupDlg::IDD, pParent)
	, m_UserName(_T(""))
	, m_Role(_T(""))
	, m_Version(_T(""))
{

}

CGroupDlg::~CGroupDlg()
{
}

void CGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USER_EDIT, m_UserName);
	DDX_CBString(pDX, IDC_ROLE_COMBO, m_Role);
	DDX_CBString(pDX, IDC_VERSION_COMBO, m_Version);
}


BEGIN_MESSAGE_MAP(CGroupDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGroupDlg::OnOK)
	ON_BN_CLICKED(IDC_SEARCH_BUTTON, &CGroupDlg::OnSearch)
	ON_CBN_SELCHANGE(IDC_ROLE_COMBO, &CGroupDlg::OnSelchangeRoleCombo)
	ON_BN_CLICKED(IDC_UNDO_BUTTON, &CGroupDlg::OnUndo)
	ON_BN_CLICKED(IDCANCEL, &CGroupDlg::OnCancel)
END_MESSAGE_MAP()


// CGroupDlg ��Ϣ�������


BOOL CGroupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ComboBox ��ʼ��
	((CComboBox*)GetDlgItem(IDC_ROLE_COMBO))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_VERSION_COMBO))->SetCurSel(0);

	if(IsNew)
	{
		SetWindowText(_T("��ӹ����Ա"));
		GetDlgItem(IDC_UNDO_BUTTON)->EnableWindow(false);
	}
	else
	{
		// ��ʾ����
		UpdateData(FALSE);
	}

	// ���� ComboBox ״̬
	OnSelchangeRoleCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CGroupDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CGroupDlg::OnOK()
{
	// ��ȡ����
	UpdateData();

	if(m_UserName.IsEmpty())
	{
		AfxMessageBox(_T("�û����Ʋ���Ϊ��!"));
	}
	else
	{
		CDialogEx::OnOK();
	}
}


void CGroupDlg::OnSearch()
{
	CSearchDlg dlg;
	dlg.Title    = _T("�����û�");
	dlg.FilePath = _T("index/account/GetUserInfo");
	dlg.NeedFind = false;
	dlg.Web_Data = true;
	dlg.Resolve  = _T("UserName");

	if(dlg.DoModal() == IDOK)
	{
		// ��ֵ
		m_UserName = dlg.Function;

		// ��ʾ����
		UpdateData(FALSE);
	}
}


void CGroupDlg::OnSelchangeRoleCombo()
{
	if( ((CComboBox*)GetDlgItem(IDC_ROLE_COMBO))->GetCurSel() == 0 )
	{
		((CComboBox*)GetDlgItem(IDC_VERSION_COMBO))->SetCurSel(0);
		((CComboBox*)GetDlgItem(IDC_VERSION_COMBO))->EnableWindow(false);
	}
	else
	{
		((CComboBox*)GetDlgItem(IDC_VERSION_COMBO))->EnableWindow();
	}
}


void CGroupDlg::OnUndo()
{
	// ��ʾ����
	UpdateData(FALSE);
}


void CGroupDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
