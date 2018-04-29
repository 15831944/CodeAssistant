// WebEditDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "WebEditDlg.h"
#include "afxdialogex.h"

#include "GroupDlg.h"


// json
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4005)
#include "json/json.h"
#pragma warning (pop)
#endif


// CWebEditDlg �Ի���

IMPLEMENT_DYNAMIC(CWebEditDlg, CDialogEx)

CWebEditDlg::CWebEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWebEditDlg::IDD, pParent)
	, m_Project(_T(""))
	, m_Version(_T(""))
	, m_Type(_T(""))
	, m_Path(_T(""))
	, m_Share(_T(""))
{
	m_hOperate = NULL;
}

CWebEditDlg::~CWebEditDlg()
{
}

void CWebEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHARE_LIST, m_Group);
	DDX_Text(pDX, IDC_PROJECT_EDIT, m_Project);
	DDX_Text(pDX, IDC_VERSION_EDIT, m_Version);
	DDX_CBString(pDX, IDC_TYPE_COMBO, m_Type);
	DDX_Text(pDX, IDC_PATH_EDIT, m_Path);
	DDX_CBString(pDX, IDC_SHARE_COMBO, m_Share);
}


BEGIN_MESSAGE_MAP(CWebEditDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CWebEditDlg::OnOK)
	ON_BN_CLICKED(IDC_UNDO_BUTTON, &CWebEditDlg::OnUndo)
	ON_BN_CLICKED(IDCANCEL, &CWebEditDlg::OnCancel)
	ON_BN_CLICKED(IDC_SELECT_BUTTON, &CWebEditDlg::OnSelect)
	ON_CBN_SELCHANGE(IDC_SHARE_COMBO, &CWebEditDlg::OnSelchangeShareCombo)
	ON_BN_CLICKED(IDC_ADD_BUTTON, &CWebEditDlg::OnAdd)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, &CWebEditDlg::OnRemove)
	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CWebEditDlg::OnEdit)

	ON_COMMAND(100, &CWebEditDlg::OnSuccess)
	ON_COMMAND(101, &CWebEditDlg::OnError)
END_MESSAGE_MAP()


// CWebEditDlg ��Ϣ�������


BOOL CWebEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// List ��ʼ��
	m_Group.SetExtendedStyle(LVS_EX_FULLROWSELECT  //��������ѡ��
		| LVS_EX_HEADERDRAGDROP				       //���������϶�
		| LVS_EX_SUBITEMIMAGES			           //����ѡ����
		| LVS_EX_GRIDLINES);				       //����������

	m_Group.SetHeadings(_T("�û�����,163;�û���ɫ,95;����汾,95;"));
	m_Group.LoadColumnInfo();

	// ����
	LOGFONT   logfont;//���Ū�����Ա,ȫ�ֱ���,��̬��Ա  
    CFont   *pfont1   =   m_Group.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //��������޸�����ĸ߱���
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //��������޸�����Ŀ����
    static   CFont   font1;
    font1.CreateFontIndirect(&logfont);
    m_Group.SetFont(&font1);  
    font1.Detach();

	// ComboBox ��ʼ��
	((CComboBox*)GetDlgItem(IDC_TYPE_COMBO))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_SHARE_COMBO))->SetCurSel(0);


	if(IsNew)
	{
		if(IsProject)
		{
			SetWindowText(_T("����ƶ���Ŀ"));
			GetDlgItem(IDC_UNDO_BUTTON)->EnableWindow(false);
			GetDlgItem(IDC_TYPE_COMBO)->EnableWindow(false);
			GetDlgItem(IDOK)->SetWindowText(_T("�����Ŀ"));
			GetDlgItem(IDCANCEL)->SetWindowText(_T("ȡ�����"));

			GetDlgItem(IDC_VERSION_STATIC)->SetWindowText(_T("��Ŀ˵��: "));
		}
		else
		{
			SetWindowText(_T("�����Ŀ�汾"));
			
			GetDlgItem(IDC_SHARE_COMBO)->EnableWindow(false);
			GetDlgItem(IDOK)->SetWindowText(_T("��Ӱ汾"));
			GetDlgItem(IDCANCEL)->SetWindowText(_T("ȡ�����"));

			GetDlgItem(IDC_PROJECT_STATIC)->SetWindowText(_T("�޸�˵��: "));
			GetDlgItem(IDC_PATH_STATIC)   ->SetWindowText(_T("�汾·��: "));
		}
	}
	else
	{
		if(IsProject)
		{
			SetWindowText(_T("�ƶ���Ŀ�༭"));
			GetDlgItem(IDC_TYPE_COMBO)->EnableWindow(false);
			GetDlgItem(IDC_VERSION_STATIC)->SetWindowText(_T("��Ŀ˵��: "));

			// �жϹ���״̬
			if(m_Share == _T("������"))
			{
				// �����������߳�
				if (m_hOperate == NULL)
				{
					Type = 0;
					GetDlgItem(IDOK)->EnableWindow(false);

					Parameter.Format(_T("id=%s"), ProjectId);
					m_hOperate = AfxBeginThread(Operate, this);
				}
			}
			else if(m_Share == _T("������") && OwnerId != UserId)
			{
				GetDlgItem(IDOK)->EnableWindow(false);
			}
		}
		else
		{
			SetWindowText(_T("��Ŀ�汾�༭"));
			GetDlgItem(IDC_SHARE_COMBO)->EnableWindow(false);

			GetDlgItem(IDC_PROJECT_STATIC)->SetWindowText(_T("�޸�˵��: "));
			GetDlgItem(IDC_PATH_STATIC)   ->SetWindowText(_T("�汾·��: "));
		}

		GetDlgItem(IDC_PATH_EDIT)->EnableWindow(false);
		GetDlgItem(IDC_SELECT_BUTTON)->EnableWindow(false);

		// ��ʾ����
		UpdateData(FALSE);
	}

	// ���� ComboBox ״̬
	OnSelchangeShareCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CWebEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CWebEditDlg::Split(CString source, CString divKey, CStringArray &dest)
{
	dest.RemoveAll();
	int pos = 0;
	int pre_pos = 0;
	while ( -1 != pos )
	{
		pre_pos = pos;
		pos     = source.Find(divKey, (pos +1));

		CString temp(source.Mid(pre_pos , (pos -pre_pos )));
		temp.Replace(divKey, _T(""));
		dest.Add(temp);
	}
}


// �����߳�
UINT CWebEditDlg::Operate(LPVOID pParam)
{
	// ����ָ��
	CWebEditDlg * pWnd = ((CWebEditDlg*)pParam);

	// �ֲ�����
	CString RecvData;
	BOOL IsSuccess;

	// ���ð�ť
	pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);

	// ��ȡ����������
	try
	{
		switch(pWnd->Type)
		{
		// ��ó�Ա
		case 0: 
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/GetGroup"), 80, pWnd->Parameter, IsSuccess);

			// ������
			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				pWnd->Error = _T("�޷����ӵ�������, �������硣");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
			else
			{
				if (IsSuccess)
				{
					Json::Reader reader;
					Json::Value root;

					// �����������
					CStringA m_RecvData(RecvData);

					// reader��Json�ַ���������root��root������Json��������Ԫ��
					if (reader.parse(m_RecvData.GetBuffer(), root))
					{
						if(!root["data"].isNull())
						{
							Json::Value Items = root["data"];
							Json::Value Total = root["total"];

							pWnd->m_Group.DeleteAllItems();

							int total = Total.asInt();
							for(int i=0; i < total; i++)
							{
								std::string Member = Items[i]["Member"].asString();
								std::string Role   = Items[i]["Role"].asString();
								std::string Type   = Items[i]["Type"].asString();

								CString m_Member   = (CString)Member.c_str();
								CString m_Role     = (CString)Role.c_str();
								CString m_Type     = (CString)Type.c_str();


								// �ָ�����
								CStringArray MemberArray, RoleArray, TypeArray;
								pWnd->Split(m_Member, _T(";"), MemberArray);
								pWnd->Split(m_Role, _T(";"),   RoleArray);
								pWnd->Split(m_Type, _T(";"),   TypeArray);

								// ѭ����������
								for(int k=0; k<MemberArray.GetSize() -1; k++)
								{
									m_Member = MemberArray.GetAt(k);
									m_Role   = RoleArray.GetAt(k);
									m_Type   = TypeArray.GetAt(k);

									if(m_Role == _T("0"))
										m_Role = _T("������");
									else
										m_Role = _T("������");
									
									if(m_Type == _T("all"))
										m_Type = _T("ȫ���汾");

									pWnd->m_Group.AddItem( m_Member, m_Role, m_Type );
								}
							}

							// Ȩ���ж�
							for(int p=0; p < pWnd->m_Group.GetItemCount(); p++)
							{
								CString Name = pWnd->m_Group.GetItemText(p, 0);
								CString Role = pWnd->m_Group.GetItemText(p, 1);

								// �����ִ�Сдƥ�� (strcmp ���ִ�Сд)
								if(_stricmp(Name, pWnd->UserName) == 0 && Role.Replace(_T("������"), _T("")))
									pWnd->GetDlgItem(IDOK)->EnableWindow();
							}

							// �ж�ӵ����
							if(pWnd->OwnerId == pWnd->UserId)
								pWnd->GetDlgItem(IDOK)->EnableWindow();
						}
					}
					else
					{
						pWnd->Error = _T("�޷��������ݡ�");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("�޷����ӵ�������, �������硣");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;


		// �½���Ŀ
		case 1: 
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/CreateProject"), 80, pWnd->Parameter, IsSuccess);

			// ������
			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				pWnd->Error = _T("�޷����ӵ�������, �������硣");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
			else
			{
				if (IsSuccess)
				{
					if( RecvData == RecvData.SpanIncluding( _T("0123456789") ) )
					{
						pWnd->ProjectId = RecvData;
						pWnd->PostMessage(WM_COMMAND, 100);
					}
					else if(RecvData == _T("used"))
					{
						pWnd->Error = _T("�Ѵ�����ͬ����Ŀ��");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else if(RecvData == _T("shared"))
					{
						pWnd->Error = _T("�Ѵ�����ͬ�Ĺ�����Ŀ��");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else
					{
						pWnd->Error = _T("���ݶ�ȡʧ�ܣ����Ժ����ԡ�");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("�޷����ӵ�������, �������硣");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;


		// �༭��Ŀ
		case 2: 
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/EditProject"), 80, pWnd->Parameter, IsSuccess);
			
			// ������
			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				pWnd->Error = _T("�޷����ӵ�������, �������硣");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
			else
			{
				if (IsSuccess)
				{
					if( RecvData.Replace(_T("success"), _T("")) )
					{
						pWnd->PostMessage(WM_COMMAND, 100);
					}
					else if( RecvData.Replace(_T("error"), _T("")) )
					{
						pWnd->Error = _T("û���޸��κ����ݡ�");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else
					{
						pWnd->Error = _T("����������Ӧ�����Ժ����ԡ�");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("�޷����ӵ�������, �������硣");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;


		// �½��汾
		case 3:
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/AddVersion"), 80, pWnd->Parameter, IsSuccess);
			
			// ������
			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				pWnd->Error = _T("�޷����ӵ�������, �������硣");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
			else
			{
				if (IsSuccess)
				{
					if( RecvData.Replace(_T("success"), _T("")) )
					{
						pWnd->PostMessage(WM_COMMAND, 100);
					}
					else if( RecvData.Replace(_T("used"), _T("")) )
					{
						pWnd->Error = _T("�Ѵ�����ͬ�İ汾��");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else
					{
						pWnd->Error = _T("����������Ӧ�����Ժ����ԡ�");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("�޷����ӵ�������, �������硣");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;


		// �༭�汾
		case 4:
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/ModifyVersion"), 80, pWnd->Parameter, IsSuccess);
			
			// ������
			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				pWnd->Error = _T("�޷����ӵ�������, �������硣");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
			else
			{
				if (IsSuccess)
				{
					if( RecvData.Replace(_T("success"), _T("")) )
					{
						pWnd->PostMessage(WM_COMMAND, 100);
					}
					else if( RecvData.Replace(_T("used"), _T("")) )
					{
						pWnd->Error = _T("�Ѵ�����ͬ�İ汾��");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else if( RecvData.Replace(_T("error"), _T("")) )
					{
						pWnd->Error = _T("û�н����κ��޸ġ�");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else
					{
						pWnd->Error = _T("����������Ӧ�����Ժ����ԡ�");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("�޷����ӵ�������, �������硣");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;
		}
	}
	catch (...)
	{
		pWnd->Error = _T("�������쳣��λ��Operate��OnGetWebInfo������");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	// ���ť
	//pWnd->GetDlgItem(IDOK)->EnableWindow();

	// ������Ϊ��
	pWnd->m_hOperate = NULL;
	return false;
}


void CWebEditDlg::OnError()
{
	AfxMessageBox(Error);
	GetDlgItem(IDOK)->EnableWindow();
}


void CWebEditDlg::OnSuccess()
{
	CDialogEx::OnOK();
}


void CWebEditDlg::OnOK()
{
	// ��ȡ����
	UpdateData();

	if(m_Project.IsEmpty())
	{
		AfxMessageBox(_T("��Ŀ���Ʋ���Ϊ��!"));
	}
	else if(m_Version.IsEmpty())
	{
		AfxMessageBox(_T("��Ŀ�汾����Ϊ��!"));
	}
	else if(m_Path.IsEmpty() && IsNew)
	{
		AfxMessageBox(_T("��Ŀ·������Ϊ��!"));
	}
	else
	{
		if(IsNew)
		{
			if(IsProject)
			{
				// �����������߳�
				if (m_hOperate == NULL)
				{
					if(m_Share == _T("������"))
					{
						m_Share = _T("0");
						Parameter.Format(_T("id=%s&project=%s&explain=%s&share=%s"), UserId, m_Project, m_Version, m_Share);
					}
					else if(m_Share == _T("������"))
					{
						m_Share = _T("1");
						Parameter.Format(_T("id=%s&project=%s&explain=%s&share=%s"), UserId, m_Project, m_Version, m_Share);
					}
					else
					{
						m_Share = _T("2");

						// �õ������Ա
						int Count = m_Group.GetItemCount();
						if( Count <= 0 )
						{
							AfxMessageBox(_T("����ĳ�Ա��������Ϊ0"));
							return;
						}

						CString User, Role, Type;
						for(int i=0; i < Count; i++)
						{
							User += m_Group.GetItemText(i, 0) + _T(";");

							if(m_Group.GetItemText(i, 1) == _T("������")) 
								Role += _T("1;");
							else
							    Role += _T("0;");
							
							if(m_Group.GetItemText(i, 2) == _T("ȫ���汾"))
								Type += _T("all;");
							else
								Type += m_Group.GetItemText(i, 2) + _T(";");
						}

						Parameter.Format(_T("id=%s&project=%s&explain=%s&share=%s&member=%s&role=%s&type=%s"), UserId, m_Project, m_Version, m_Share, User, Role, Type);
					}

					Type = 1;
					m_hOperate = AfxBeginThread(Operate, this);
				}
			}
			else
			{
				// �����������߳�
				if (m_hOperate == NULL)
				{
					Type = 3;
					Parameter.Format(_T("id=%s&project_id=%s&explain=%s&version=%s&branch=%s"), UserId, ProjectId, m_Project, m_Version, m_Type);
					m_hOperate = AfxBeginThread(Operate, this);
				}
			}
		}
		else
		{
			if(IsProject)
			{
				// �����������߳�
				if (m_hOperate == NULL)
				{
					if(m_Share == _T("������"))
					{
						m_Share = _T("0");
						Parameter.Format(_T("project_id=%s&project=%s&explain=%s&share=%s"), ProjectId, m_Project, m_Version, m_Share);
					}
					else if(m_Share == _T("������"))
					{
						m_Share = _T("1");
						Parameter.Format(_T("project_id=%s&project=%s&explain=%s&share=%s"), ProjectId, m_Project, m_Version, m_Share);
					}
					else
					{
						m_Share = _T("2");

						// �õ������Ա
						int Count = m_Group.GetItemCount();
						if( Count <= 0 )
						{
							AfxMessageBox(_T("����ĳ�Ա��������Ϊ0"));
							return;
						}

						CString User, Role, Type;
						for(int i=0; i < Count; i++)
						{
							User += m_Group.GetItemText(i, 0) + _T(";");

							if(m_Group.GetItemText(i, 1) == _T("������")) 
								Role += _T("1;");
							else
								Role += _T("0;");

							if(m_Group.GetItemText(i, 2) == _T("ȫ���汾"))
								Type += _T("all;");
							else
								Type += m_Group.GetItemText(i, 2) + _T(";");
						}

						Parameter.Format(_T("project_id=%s&project=%s&explain=%s&share=%s&member=%s&role=%s&type=%s"), ProjectId, m_Project, m_Version, m_Share, User, Role, Type);
					}

					Type = 2;
					m_hOperate = AfxBeginThread(Operate, this);
				}
			}
			else
			{
				// �����������߳�
				if (m_hOperate == NULL)
				{
					Type = 4;
					Parameter.Format(_T("id=%s&project_id=%s&explain=%s&version=%s&branch=%s&version_id=%s"), UserId, ProjectId, m_Project, m_Version, m_Type, VersionId);
					m_hOperate = AfxBeginThread(Operate, this);
				}
			}
		}
	}
}


void CWebEditDlg::OnUndo()
{
	// ��ʾ����
	UpdateData(FALSE);

	// ����״̬
	OnSelchangeShareCombo();

	// �жϹ���״̬
	if(m_Share == _T("������"))
	{
		// �����������߳�
		if (m_hOperate == NULL)
		{
			Type = 0;
			Parameter.Format(_T("id=%s"), ProjectId);
			m_hOperate = AfxBeginThread(Operate, this);
		}
	}
}


void CWebEditDlg::OnSelect()
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
			if(m_Version.IsEmpty())
				GetDlgItem(IDC_VERSION_EDIT)->SetFocus();
			else
			    GetDlgItem(IDC_SHARE_COMBO)->SetFocus();
		}
	}
	if(lpidlBrowse != NULL)  
	{  
		::CoTaskMemFree(lpidlBrowse);  
	}
}


void CWebEditDlg::OnSelchangeShareCombo()
{
	if( ((CComboBox*)GetDlgItem(IDC_SHARE_COMBO))->GetCurSel() == 0 || ((CComboBox*)GetDlgItem(IDC_SHARE_COMBO))->GetCurSel() == 1 )
	{
		m_Group.EnableWindow(false);

		GetDlgItem(IDC_ADD_BUTTON)   ->EnableWindow(false);
		GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_BUTTON)  ->EnableWindow(false);
	}
	else if( ((CComboBox*)GetDlgItem(IDC_SHARE_COMBO))->GetCurSel() == 2 )
	{
		m_Group.EnableWindow();

		GetDlgItem(IDC_ADD_BUTTON)   ->EnableWindow();
		GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow();
		GetDlgItem(IDC_EDIT_BUTTON)  ->EnableWindow();
	}
}


void CWebEditDlg::OnAdd()
{
	CGroupDlg dlg;
	dlg.IsNew = true;

	if(dlg.DoModal() == IDOK)
	{
		m_Group.AddItem(dlg.m_UserName, dlg.m_Role, dlg.m_Version);
	}
}


void CWebEditDlg::OnRemove()
{
	int Count = m_Group.GetSelectedCount();

	if (m_Group.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		if(Count == 1)
		{
			// ���б���ɾ������
			m_Group.DeleteItem(m_Group.GetNextItem(-1, LVIS_SELECTED));
		}
		else
		{
			int nItem = -1;
			POSITION pos;
			while (pos = m_Group.GetFirstSelectedItemPosition())
			{
				nItem = -1;
				nItem = m_Group.GetNextSelectedItem(pos);
				if (nItem >= 0 && m_Group.GetSelectedCount() > 0)
				{
					m_Group.DeleteItem(nItem);
				}
			}
		}
	}
}


void CWebEditDlg::OnEdit()
{
	if (m_Group.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString User = m_Group.GetItemText(m_Group.GetNextItem(-1, LVIS_SELECTED), 0);
		CString Role = m_Group.GetItemText(m_Group.GetNextItem(-1, LVIS_SELECTED), 1);
		CString Ver  = m_Group.GetItemText(m_Group.GetNextItem(-1, LVIS_SELECTED), 2);

		CGroupDlg dlg;
		dlg.IsNew = false;
		dlg.m_UserName = User;
		dlg.m_Role     = Role;
		dlg.m_Version  = Ver;

		if(dlg.DoModal() == IDOK)
		{
			// �����б�
			m_Group.SetItemText(m_Group.GetNextItem(-1, LVIS_SELECTED), 0, dlg.m_UserName);
			m_Group.SetItemText(m_Group.GetNextItem(-1, LVIS_SELECTED), 1, dlg.m_Role);
			m_Group.SetItemText(m_Group.GetNextItem(-1, LVIS_SELECTED), 2, dlg.m_Version);

			//���ø�����ʾ
			m_Group.SetFocus();//���ý���  
			m_Group.SetItemState(m_Group.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
			m_Group.EnsureVisible(m_Group.GetNextItem(-1, LVIS_SELECTED), FALSE);//���õ�ǰ��ͼ�ɼ� 
		}
	}
}


void CWebEditDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
