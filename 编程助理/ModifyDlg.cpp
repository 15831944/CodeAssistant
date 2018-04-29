// ModifyDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "ModifyDlg.h"
#include "afxdialogex.h"


// CModifyDlg �Ի���

IMPLEMENT_DYNAMIC(CModifyDlg, CDialogEx)

CModifyDlg::CModifyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CModifyDlg::IDD, pParent)
	, m_Name(_T(""))
	, m_Path(_T(""))
	, m_Version(_T(""))
{

}

CModifyDlg::~CModifyDlg()
{
}

void CModifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION_COMBO, m_ComboBox);
	DDX_Text(pDX, IDC_NAME_EDIT, m_Name);
	DDX_Text(pDX, IDC_PATH_EDIT, m_Path);
	DDX_Text(pDX, IDC_VERSION_EDIT, m_Version);
}


BEGIN_MESSAGE_MAP(CModifyDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CModifyDlg::OnOK)
	ON_BN_CLICKED(IDC_CHOOSE_BUTTON, &CModifyDlg::OnChoose)
	ON_BN_CLICKED(IDC_UNDO_BUTTON, &CModifyDlg::OnUndo)
	ON_BN_CLICKED(IDCANCEL, &CModifyDlg::OnCancel)
	ON_CBN_SELCHANGE(IDC_VERSION_COMBO, &CModifyDlg::OnSelchangeVersion)
END_MESSAGE_MAP()


// CModifyDlg ��Ϣ�������


BOOL CModifyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �������Ŀ
	if(IsProject)
	{
		if(IsNew)
		{
			SetWindowText(_T("��ӱ�����Ŀ"));
			m_ComboBox.EnableWindow(false);
			GetDlgItem(IDOK)->SetWindowText(_T("�����Ŀ"));
			GetDlgItem(IDC_UNDO_BUTTON)->EnableWindow(false);
			GetDlgItem(IDCANCEL)->SetWindowText(_T("ȡ�����"));
		}
		else
		{
			// ��ʾ����
			UpdateData(FALSE);
			GetDlgItem(IDC_VERSION_EDIT)->EnableWindow(false);

			// �õ��汾����
			m_ComboBox.ResetContent();

			// ѭ���������(�ж����� i �ͼӶ���)
			for (int i = 0 ;i < (int)this->pResult.size(); i += 5)
			{
				CString Version  = this->pResult[i + 3];
				m_ComboBox.AddString(Version);
			}

			// ����ѡ������
			int nIndex = m_ComboBox.FindStringExact(0, m_Version);
			if(nIndex != CB_ERR)
			{
				m_ComboBox.SetCurSel(nIndex);
			}
		}
	}
	else
	{
		if(IsNew)
		{
			SetWindowText(_T("�����Ŀ�汾"));
			GetDlgItem(IDC_NAME_STATIC)->SetWindowText(_T("�޸�˵��"));
			GetDlgItem(IDC_PATH_STATIC)->SetWindowText(_T("�޸�ʱ��"));

			GetDlgItem(IDC_PATH_EDIT)->EnableWindow(false);
			GetDlgItem(IDC_CHOOSE_BUTTON)->EnableWindow(false);
			GetDlgItem(IDC_UNDO_BUTTON)->EnableWindow(false);

			m_ComboBox.EnableWindow(false);

			GetDlgItem(IDOK)->SetWindowText(_T("��Ӱ汾"));
			GetDlgItem(IDCANCEL)->SetWindowText(_T("ȡ�����"));
			
		}
		else
		{
			// ��ʾ����
			UpdateData(FALSE);

			SetWindowText(_T("��Ŀ�汾�༭"));
			GetDlgItem(IDC_NAME_STATIC)->SetWindowText(_T("�޸�˵��"));
			GetDlgItem(IDC_PATH_STATIC)->SetWindowText(_T("�޸�ʱ��"));

			GetDlgItem(IDC_PATH_EDIT)->EnableWindow(false);
			GetDlgItem(IDC_CHOOSE_BUTTON)->EnableWindow(false);
			m_ComboBox.EnableWindow(false);

			// ����ѡ������
			m_ComboBox.ResetContent();
			m_ComboBox.AddString(m_Version);
			m_ComboBox.SetCurSel(0);
		}
	}

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CModifyDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CModifyDlg::OnSelchangeVersion()
{
	CString Version;
	m_ComboBox.GetWindowText(Version);

	GetDlgItem(IDC_VERSION_EDIT)->SetWindowText(Version);
	
}


void CModifyDlg::OnOK()
{
	// ��ȡ����
	UpdateData(TRUE);

	if(m_Name.IsEmpty())
	{
		if(IsProject)
			AfxMessageBox(_T("��Ŀ���Ʋ���Ϊ��!"));
		else
			AfxMessageBox(_T("�޸�˵������Ϊ��!"));
	}
	else if(m_Path.IsEmpty() && IsProject)
	{
		AfxMessageBox(_T("��Ŀ·������Ϊ��!"));
	}
	else if(m_Version.IsEmpty())
	{
		AfxMessageBox(_T("��Ŀ�汾����Ϊ��!"));
	}
	else
	{
		CDialogEx::OnOK();
	}
}


void CModifyDlg::OnChoose()
{
	TCHAR           szFolderPath[MAX_PATH] = {0};  
	CString         strFolderPath = TEXT("");  

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot  = 0;
	sInfo.lpszTitle = _T("��ѡ��һ��Ŀ¼��");
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	sInfo.lpfn      = NULL;

	// ��ʾ�ļ���ѡ��Ի���
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// ȡ���ļ�����
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath)) 
		{
			strFolderPath = szFolderPath;

			// ��ʾѡ��·��
			GetDlgItem(IDC_PATH_EDIT)->SetWindowText(strFolderPath);

			// ���ý���
			GetDlgItem(IDC_VERSION_EDIT)->SetFocus();
		}
	}
	if(lpidlBrowse != NULL)  
	{  
		::CoTaskMemFree(lpidlBrowse);  
	}
}


void CModifyDlg::OnUndo()
{
	if(IsNew)
	{
		AfxMessageBox(_T("�����������賷��!"));
	}
	else
	{
		// ��ʾ����
		UpdateData(FALSE);

		// ����ѡ������
		int nIndex = m_ComboBox.FindStringExact(0, m_Version);
		if(nIndex != CB_ERR)
		{
			m_ComboBox.SetCurSel(nIndex);
		}
	}
}


void CModifyDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
