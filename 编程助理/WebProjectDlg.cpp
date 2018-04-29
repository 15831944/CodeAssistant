// WebProjectDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "WebProjectDlg.h"
#include "afxdialogex.h"

#include "WebEditDlg.h"
#include "SearchDlg.h"

// json
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4005)
#include "json/json.h"
#pragma warning (pop)
#endif

// CWebProjectDlg �Ի���

IMPLEMENT_DYNAMIC(CWebProjectDlg, CDialogEx)

CWebProjectDlg::CWebProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWebProjectDlg::IDD, pParent)
{
	m_hOperate = NULL;

	ProjectType=2;
}

CWebProjectDlg::~CWebProjectDlg()
{
}

void CWebProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROJECT_LIST, m_Project);
	DDX_Control(pDX, IDC_VERSION_LIST, m_Version);
}


BEGIN_MESSAGE_MAP(CWebProjectDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CWebProjectDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CWebProjectDlg::OnCancel)
	ON_BN_CLICKED(IDC_ASSISTANT_RADIO, &CWebProjectDlg::OnAssistant)
	ON_BN_CLICKED(IDC_GITHUB_RADIO, &CWebProjectDlg::OnGithub)
	ON_BN_CLICKED(IDC_GITEE_RADIO, &CWebProjectDlg::OnGitee)
	ON_BN_CLICKED(IDC_SOURCEFORGE_RADIO, &CWebProjectDlg::OnSourceforge)
	ON_BN_CLICKED(IDC_GOOGLE_RADIO, &CWebProjectDlg::OnGoogle)
	ON_BN_CLICKED(IDC_NEW_BUTTON, &CWebProjectDlg::OnNew)
	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CWebProjectDlg::OnEdit)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, &CWebProjectDlg::OnDelete)
	ON_BN_CLICKED(IDC_SWITCH__BUTTON, &CWebProjectDlg::OnSwitch)
	ON_BN_CLICKED(IDC_ADD_BUTTON, &CWebProjectDlg::OnAdd)
	ON_BN_CLICKED(IDC_RESTORE_BUTTON, &CWebProjectDlg::OnRestore)
	ON_BN_CLICKED(IDC_MODIFY_BUTTON, &CWebProjectDlg::OnModify)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, &CWebProjectDlg::OnRemove)
	ON_BN_CLICKED(IDC_CLONE_BUTTON, &CWebProjectDlg::OnClone)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PROJECT_LIST, &CWebProjectDlg::OnItemchangedProjectList)
	ON_NOTIFY(NM_CLICK, IDC_PROJECT_LIST, &CWebProjectDlg::OnClickProjectList)
	ON_NOTIFY(NM_DBLCLK, IDC_PROJECT_LIST, &CWebProjectDlg::OnDblclkProjectList)
	ON_NOTIFY(NM_RCLICK, IDC_PROJECT_LIST, &CWebProjectDlg::OnRclickProjectList)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_VERSION_LIST, &CWebProjectDlg::OnItemchangedVersionList)
	ON_NOTIFY(NM_CLICK, IDC_VERSION_LIST, &CWebProjectDlg::OnClickVersionList)
	ON_NOTIFY(NM_DBLCLK, IDC_VERSION_LIST, &CWebProjectDlg::OnDblclkVersionList)
	ON_NOTIFY(NM_RCLICK, IDC_VERSION_LIST, &CWebProjectDlg::OnRclickVersionList)

	ON_COMMAND(100, &CWebProjectDlg::OnSuccess)
	ON_COMMAND(101, &CWebProjectDlg::OnError)
END_MESSAGE_MAP()


// CWebProjectDlg ��Ϣ�������


BOOL CWebProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Project.SetExtendedStyle(LVS_EX_FULLROWSELECT  //��������ѡ��
		| LVS_EX_HEADERDRAGDROP				         //���������϶�
		| LVS_EX_SUBITEMIMAGES			             //����ѡ����
		| LVS_EX_GRIDLINES);				         //����������

	m_Project.SetHeadings(_T("��Ŀ����,147;��Ŀ˵��,120;����״̬,70;��Ŀ���,0;�û����,0;�û���ɫ,0;"));
	m_Project.LoadColumnInfo();

	// ����
	LOGFONT   logfont;//���Ū�����Ա,ȫ�ֱ���,��̬��Ա  
    CFont   *pfont1   =   m_Project.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //��������޸�����ĸ߱���
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //��������޸�����Ŀ����
    static   CFont   font1;
    font1.CreateFontIndirect(&logfont);
    m_Project.SetFont(&font1);  
    font1.Detach();


	m_Version.SetExtendedStyle(LVS_EX_FULLROWSELECT  //��������ѡ��
		| LVS_EX_HEADERDRAGDROP				      //���������϶�
		| LVS_EX_SUBITEMIMAGES			          //����ѡ����
		| LVS_EX_GRIDLINES);				      //����������

	m_Version.SetHeadings(_T("�޸�˵��,161;�޸�ʱ��,150;��Ŀ�汾-��֧,120;�޸���,65;��Ŀ��֧,0;��Ŀ���,0;"));
	m_Version.LoadColumnInfo();

	// ����
    pfont1   =   m_Version.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //��������޸�����ĸ߱���
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //��������޸�����Ŀ����
  
    font1.CreateFontIndirect(&logfont);
    m_Version.SetFont(&font1);  
    font1.Detach();


	// Ĭ��ѡ��
	((CButton*)GetDlgItem(IDC_ASSISTANT_RADIO))->SetCheck(1);
	OnAssistant();

	// ˢ������
	OnOK();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CWebProjectDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialogEx::PreTranslateMessage(pMsg);
}


// �����߳�
UINT CWebProjectDlg::Operate(LPVOID pParam)
{
	// ����ָ��
	CWebProjectDlg * pWnd = ((CWebProjectDlg*)pParam);

	// �ֲ�����
	CString RecvData;
	BOOL IsSuccess;

	// ���ð�ť
	pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);

	// ��ȡ����������
	try
	{
		switch(pWnd->OperateType)
		{
		case 1:
			{
				if(pWnd->ProjectType == 2)
					RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/GetProject"), 80, pWnd->Parameter, IsSuccess);
				else if(pWnd->ProjectType == 3)
					RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/GetShareProject"), 80, pWnd->Parameter, IsSuccess);
				else
					RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/GetOpenProject"), 80, pWnd->Parameter, IsSuccess);


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

								int total = Total.asInt();
								pWnd->m_Project.DeleteAllItems();

								for(int i=0; i < total; i++)
								{
									std::string UserId   = Items[i]["UserId"].asString();
									std::string Project  = Items[i]["ProjectName"].asString();
									std::string Version  = Items[i]["ProjectExplain"].asString();
									std::string Status   = Items[i]["ShareStatus"].asString();
									std::string Number   = Items[i]["ProjectId"].asString();
									std::string Role     = Items[i]["Role"].asString();
									

									CString m_UserId     = (CString)UserId.c_str();
									CString m_Project    = (CString)Project.c_str();
									CString m_Version    = (CString)Version.c_str();
									CString m_Status     = (CString)Status.c_str();
									CString m_Number     = (CString)Number.c_str();
									CString m_Role       = (CString)Role.c_str();

									// ��������
									if(m_Status == _T("0"))
										m_Status = _T("������");
									else if(m_Status == _T("1"))
										m_Status = _T("������");
									else
										m_Status = _T("������");

									if(!m_Project.IsEmpty() && !m_Version.IsEmpty() && !m_Status.IsEmpty())
										pWnd->m_Project.AddItem(m_Project, m_Version, m_Status, m_Number, m_UserId, m_Role);
								}
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
				}
			} break;

		case 2:
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/DeleteProject"), 80, pWnd->Parameter, IsSuccess);

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
					if( RecvData == _T("success") )
					{
						;
					}
					else
					{
						pWnd->Error = _T("�޷�ɾ����Ŀ�����Ժ����ԡ�");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("�޷����ӵ�������, �������硣");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;

		case 3:
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/GetVersion"), 80, pWnd->Parameter, IsSuccess);

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

							int total = Total.asInt();
							pWnd->m_Version.DeleteAllItems();

							for(int i=0; i < total; i++)
							{
								std::string Remark   = Items[i]["Remark"].asString();
								std::string MTime    = Items[i]["ModifyTime"].asString();
								std::string Version  = Items[i]["Version"].asString();
								std::string Editor   = Items[i]["UserName"].asString();
								std::string Branch   = Items[i]["Branch"].asString();
								std::string Number   = Items[i]["VersionId"].asString();

								//�������ת������  
								int len = strlen(Editor.c_str())+1;  
								char outch[MAX_PATH];  
								WCHAR * wChar = new WCHAR[len];  
								wChar[0] = 0;  
								MultiByteToWideChar(CP_UTF8, 0, Editor.c_str(), len, wChar, len);  
								WideCharToMultiByte(CP_ACP, 0, wChar, len, outch , len, 0, 0);  
								delete [] wChar;  
								char* pchar = (char*)outch;  

								len=strlen(pchar)+1;  
								WCHAR outName[MAX_PATH];  
								MultiByteToWideChar(CP_ACP, 0, pchar, len, outName, len);

								CString m_Remark     = (CString)Remark.c_str();
								CString m_MTime      = (CString)MTime.c_str();
								CString m_Version    = (CString)Version.c_str();
								CString m_Editor     = (CString)outName;
								CString m_Branch     = (CString)Branch.c_str();
								CString m_Number     = (CString)Number.c_str();

								if(!m_Remark.IsEmpty() && !m_Version.IsEmpty() && !m_MTime.IsEmpty())
									pWnd->m_Version.AddItem(m_Remark, m_MTime, m_Version + _T("-") + m_Branch, m_Editor, m_Branch, m_Number);
							}
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

		case 4:
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/RemoveVersion"), 80, pWnd->Parameter, IsSuccess);

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
					if( RecvData == _T("success") )
					{
						;
					}
					else
					{
						pWnd->Error = _T("�޷��Ƴ��汾�����Ժ����ԡ�");
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
	pWnd->GetDlgItem(IDOK)->EnableWindow();

	// ������Ϊ��
	pWnd->m_hOperate = NULL;
	return false;
}


void CWebProjectDlg::OnError()
{
	AfxMessageBox(Error);
}


void CWebProjectDlg::OnSuccess()
{
	Total--;
	if(Total > 0)
	{
		// �����������߳�
		if (m_hOperate == NULL)
		{
			m_hOperate = AfxBeginThread(Operate, this);
		}
	}
}


void CWebProjectDlg::Split(CString source, CString divKey, CStringArray &dest)
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



void CWebProjectDlg::OnOK()
{
	switch(DataSource)
	{
	case 1:
		// �����������߳�
		if (m_hOperate == NULL)
		{
			OperateType=1;
			Parameter.Format(_T("id=%s"), UserId);

			m_hOperate = AfxBeginThread(Operate, this);
		}
		break;

	case 2:
		AfxMessageBox(_T("�������ڿ�����,�����ڴ�!"));
		break;

	case 3:
		AfxMessageBox(_T("�������ڿ�����,�����ڴ�!"));
		break;

	case 4:
		AfxMessageBox(_T("�������ڿ�����,�����ڴ�!"));
		break;

	case 5:
		AfxMessageBox(_T("�������ڿ�����,�����ڴ�!"));
		break;
	}
}


void CWebProjectDlg::OnAssistant()
{
	// ���ñ༭��
	GetDlgItem(IDC_USER_EDIT)    ->EnableWindow(false);
	GetDlgItem(IDC_PASSWORD_EDIT)->EnableWindow(false);
	GetDlgItem(IDC_USER_EDIT)->SetWindowText(UserName);

	// ����Դ
	DataSource = 1;
}


void CWebProjectDlg::OnGithub()
{
	// ���ñ༭��
	GetDlgItem(IDC_USER_EDIT)    ->EnableWindow();
	GetDlgItem(IDC_PASSWORD_EDIT)->EnableWindow();
	GetDlgItem(IDC_USER_EDIT)->SetWindowText(_T(""));

	// ����Դ
	DataSource = 2;
}


void CWebProjectDlg::OnGitee()
{
	// ���ñ༭��
	GetDlgItem(IDC_USER_EDIT)    ->EnableWindow();
	GetDlgItem(IDC_PASSWORD_EDIT)->EnableWindow();
	GetDlgItem(IDC_USER_EDIT)->SetWindowText(_T(""));

	// ����Դ
	DataSource = 3;
}


void CWebProjectDlg::OnSourceforge()
{
	// ���ñ༭��
	GetDlgItem(IDC_USER_EDIT)    ->EnableWindow();
	GetDlgItem(IDC_PASSWORD_EDIT)->EnableWindow();
	GetDlgItem(IDC_USER_EDIT)->SetWindowText(_T(""));

	// ����Դ
	DataSource = 4;
}


void CWebProjectDlg::OnGoogle()
{
	// ���ñ༭��
	GetDlgItem(IDC_USER_EDIT)    ->EnableWindow();
	GetDlgItem(IDC_PASSWORD_EDIT)->EnableWindow();
	GetDlgItem(IDC_USER_EDIT)->SetWindowText(_T(""));

	// ����Դ
	DataSource = 5;
}


void CWebProjectDlg::OnItemchangedProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	int Count = m_Project.GetSelectedCount();
	if(Count == 1)
	{
		//OnClickProjectList(pNMHDR, pResult);
	}

	*pResult = 0;
}


void CWebProjectDlg::OnClickProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
	            Project = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 0);
		CString Version = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 1);
		CString Share   = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 2);
		        Number  = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 3);
		        OwnerId = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 4);
				Role    = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 5);

		// Ȩ���ж�
		if(OwnerId != UserId)
		{
			GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(false);
		}
		else
		{
			GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow();
		}

		if(OwnerId == UserId || Role == _T("1"))
		{
			GetDlgItem(IDC_EDIT_BUTTON)->EnableWindow();
		}
		else
		{
			GetDlgItem(IDC_EDIT_BUTTON)->EnableWindow(false);
		}

		// ���������ж�
		if(ProjectType != 1)
			GetDlgItem(IDC_ADD_BUTTON)->EnableWindow();
		else if(OwnerId == UserId)
			GetDlgItem(IDC_ADD_BUTTON)->EnableWindow();

		// �õ���Ŀ�汾

		// �����������߳�
		if (m_hOperate == NULL)
		{
			OperateType = 3;
			Parameter.Format(_T("project_id=%s&user_id=%s"), Number, UserId);

			m_hOperate = AfxBeginThread(Operate, this);
		}
	}
	else
	{
		GetDlgItem(IDC_EDIT_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(false);
	}

	*pResult = 0;
}


void CWebProjectDlg::OnDblclkProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}


void CWebProjectDlg::OnRclickProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CSearchDlg dlg;
	dlg.Title    = _T("������Ŀ");
	dlg.FilePath = _T("index/account/GetProjectInfo");
	dlg.NeedFind = false;
	dlg.Web_Data = true;
	dlg.Resolve  = _T("ProjectName");
	dlg.Parameter.Format(_T("id=%s&share=%d"), UserId, ProjectType);

	if(dlg.DoModal() == IDOK)
	{
		CString Function = dlg.Function;

		int Count = m_Project.GetItemCount();
		for(int i=0; i<Count; i++)
		{
			CString Name = m_Project.GetItemText(i, 0);

			if(Name == Function)
			{
				//���ø�����ʾ  
				m_Project.SetFocus();//���ý���  
				m_Project.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
				m_Project.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 

				// ģ����
				OnClickProjectList(pNMHDR, pResult);
				break;
			}
		}
	}

	*pResult = 0;
}


void CWebProjectDlg::OnItemchangedVersionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}


void CWebProjectDlg::OnClickVersionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString Editor = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 3);
		if(OwnerId == UserId || Role == _T("1"))
		{
			GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow();
			GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow();
			GetDlgItem(IDC_RESTORE_BUTTON)->EnableWindow();
		}
		else
		{
			if(_stricmp(Editor, UserName) == 0)
			{
				GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow();
				GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow();
				GetDlgItem(IDC_RESTORE_BUTTON)->EnableWindow();
			}
			else
			{
				GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
				GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(false);
				GetDlgItem(IDC_RESTORE_BUTTON)->EnableWindow(false);
			}
		}

		GetDlgItem(IDC_CLONE_BUTTON)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_RESTORE_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_CLONE_BUTTON)->EnableWindow(false);
	}

	*pResult = 0;
}


void CWebProjectDlg::OnDblclkVersionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}


void CWebProjectDlg::OnRclickVersionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CSearchDlg dlg;
	dlg.Title    = _T("���Ұ汾");
	dlg.FilePath = _T("index/account/GetVersionInfo");
	dlg.NeedFind = false;
	dlg.Web_Data = true;
	dlg.Resolve  = _T("Version");
	dlg.Parameter.Format(_T("project_id=%s&user_id=%s"), Number, UserId);

	if(dlg.DoModal() == IDOK)
	{
		CString Function = dlg.Function;

		int Count = m_Version.GetItemCount();
		for(int i=0; i<Count; i++)
		{
			CString Version = m_Version.GetItemText(i, 2);

			if(Version == Function)
			{
				//���ø�����ʾ  
				m_Version.SetFocus();//���ý���  
				m_Version.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
				m_Version.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 

				// ģ����
				OnClickVersionList(pNMHDR, pResult);
				break;
			}
		}
	}

	*pResult = 0;
}


void CWebProjectDlg::OnNew()
{
	CWebEditDlg dlg;
	dlg.IsNew     = true;
	dlg.IsProject = true;
	dlg.UserId    = UserId;

	if(dlg.DoModal() == IDOK)
	{
		m_Project.AddItem(dlg.m_Project, dlg.m_Version, dlg.m_Share, dlg.ProjectId);
	}
}


void CWebProjectDlg::OnEdit()
{
	if (m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString Project = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 0);
		CString Version = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 1);
		CString Share   = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 2);
		CString Number  = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 3);
		CString OwnerId = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 4);

		int Count = m_Project.GetSelectedCount();
		if(Count > 1)
		{
			AfxMessageBox(_T("һ��ֻ�ܱ༭һ����Ŀ!"));
			return;
		}

		CWebEditDlg dlg;
		dlg.IsNew     = false;
		dlg.IsProject = true;
		dlg.m_Project = Project;
		dlg.m_Version = Version;
		dlg.m_Share   = Share;
		dlg.ProjectId = Number;
		dlg.UserName  = UserName;
		dlg.OwnerId   = OwnerId;
		dlg.UserId    = UserId;

		if(dlg.DoModal() == IDOK)
		{
			// �����б�
			m_Project.SetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 0, dlg.m_Project);
			m_Project.SetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 1, dlg.m_Version);
			m_Project.SetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 2, dlg.m_Share);

			//���ø�����ʾ
			m_Project.SetFocus();//���ý���  
			m_Project.SetItemState(m_Project.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
			m_Project.EnsureVisible(m_Project.GetNextItem(-1, LVIS_SELECTED), FALSE);//���õ�ǰ��ͼ�ɼ� 
		}
	}
}


void CWebProjectDlg::OnDelete()
{
	int Count = m_Project.GetSelectedCount();

	if (m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		if(Count == 1)
		{
			CString ProjectId = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 3);

			// �����������߳�
			if (m_hOperate == NULL)
			{
				OperateType = 2;
				Parameter.Format(_T("project_id=%s"), ProjectId);

				m_hOperate = AfxBeginThread(Operate, this);
			}

			// ���б���ɾ������
			m_Project.DeleteItem(m_Project.GetNextItem(-1, LVIS_SELECTED));
		}
		else
		{
			//int nItem = -1;
			//POSITION pos;
			//while (pos = m_Project.GetFirstSelectedItemPosition())
			//{
			//	nItem = -1;
			//	nItem = m_Project.GetNextSelectedItem(pos);
			//	if (nItem >= 0 && m_Project.GetSelectedCount() > 0)
			//	{
			//		CString ProjectId = m_Project.GetItemText(nItem, 3);

			//		// �����������߳�
			//		if (m_hOperate == NULL)
			//		{
			//			Total   = Count;
			//			OperateType = 2;
			//			Parameter.Format(_T("project_id=%s"), ProjectId);

			//			m_hOperate = AfxBeginThread(Operate, this);
			//		}

			//		// ɾ����Ŀ
			//		//m_Project.DeleteItem(nItem);
			//	}
			//}
			AfxMessageBox(_T("һ��ֻ��ɾ��һ����Ŀ!"));
		}
	}
}


void CWebProjectDlg::OnSwitch()
{
	switch(ProjectType)
	{
	case 1:
		ProjectType = 2;
		GetDlgItem(IDC_PROJECT_STATIC)->SetWindowText(_T("��Ŀ���� (��������Ŀ)"));
		GetDlgItem(IDC_NEW_BUTTON)->EnableWindow();
		break;

	case 2:
		ProjectType = 3;
		GetDlgItem(IDC_PROJECT_STATIC)->SetWindowText(_T("��Ŀ���� (�������Ŀ)"));
		GetDlgItem(IDC_NEW_BUTTON)->EnableWindow(false);
		break;

	case 3:
		ProjectType = 1;
		GetDlgItem(IDC_PROJECT_STATIC)->SetWindowText(_T("��Ŀ���� (�������Ŀ)"));
		GetDlgItem(IDC_NEW_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_ADD_BUTTON)->EnableWindow(false);
		break;
	}

	// ��ʼ��
	GetDlgItem(IDC_EDIT_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(false);

	m_Version.DeleteAllItems();
	GetDlgItem(IDC_ADD_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_RESTORE_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_CLONE_BUTTON)->EnableWindow(false);

	// �����������߳�
	if (m_hOperate == NULL)
	{
		OperateType=1;
		Parameter.Format(_T("id=%s"), UserId);

		m_hOperate = AfxBeginThread(Operate, this);
	}
}	


void CWebProjectDlg::OnAdd()
{
	if (Project != _T("") /*m_Project.GetNextItem(-1, LVIS_SELECTED) != -1*/)
	{
		CWebEditDlg dlg;
		dlg.IsNew     = true;
		dlg.IsProject = false;
		dlg.ProjectId = Number;
		dlg.UserId  = UserId;

		if(dlg.DoModal() == IDOK)
		{
			CString ModifyTime; //��ȡϵͳʱ��
			CTime tm; tm = CTime::GetCurrentTime();
			ModifyTime   = tm.Format(_T("%Y-%m-%d %H:%M:%S"));

			m_Version.AddItem(dlg.m_Project, ModifyTime, dlg.m_Version + _T("-") + dlg.m_Type, UserName, dlg.m_Type);
		}
	}
	else
	{
		AfxMessageBox(_T("��ѡ��Ŀ����Ŀ!"));
	}
}


void CWebProjectDlg::OnRestore()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CWebProjectDlg::OnModify()
{
	int Count = m_Version.GetSelectedCount();
	if(Count > 1)
	{
		AfxMessageBox(_T("һ��ֻ�ܱ༭һ���汾!"));
		return;
	}

	if (Project != _T("") && m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString Explain = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 0);
		CString MTime   = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 1);
		CString Version = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2);
		CString Type    = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 4);
		CString Ver_Id  = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 5);

		// ��������
		Version.Replace(_T("-") + Type, _T(""));

		int Count = m_Project.GetSelectedCount();
		if(Count > 1)
		{
			AfxMessageBox(_T("һ��ֻ�ܱ༭һ����Ŀ!"));
			return;
		}

		CWebEditDlg dlg;
		dlg.IsNew     = false;
		dlg.IsProject = false;
		dlg.ProjectId = Number;
		dlg.UserId    = UserId;
		dlg.VersionId = Ver_Id;
		dlg.m_Project = Explain;
		dlg.m_Version = Version;
		dlg.m_Type    = Type;

		if(dlg.DoModal() == IDOK)
		{
			CString ModifyTime; //��ȡϵͳʱ��
			CTime tm; tm = CTime::GetCurrentTime();
			ModifyTime   = tm.Format(_T("%Y-%m-%d %H:%M:%S"));

			// �����б�
			m_Version.SetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 0, dlg.m_Project);
			m_Version.SetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 1, ModifyTime);
			m_Version.SetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2, dlg.m_Version + _T("-") + dlg.m_Type);
			m_Version.SetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 3, UserName);
			m_Version.SetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 4, dlg.m_Type);

			//���ø�����ʾ
			m_Version.SetFocus();//���ý���  
			m_Version.SetItemState( m_Version.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
			m_Version.EnsureVisible(m_Version.GetNextItem(-1, LVIS_SELECTED), FALSE);//���õ�ǰ��ͼ�ɼ� 
		}
	}
	else if(m_Version.GetNextItem(-1, LVIS_SELECTED) == -1)
	{
		AfxMessageBox(_T("��ѡ��Ŀ��汾!"));
	}
	else
	{
		AfxMessageBox(_T("��ѡ��Ŀ����Ŀ!"));
	}
}


void CWebProjectDlg::OnRemove()
{
	int Count = m_Version.GetSelectedCount();

	if (m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		if(Count == 1)
		{
			CString VersionId = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 5);

			// �����������߳�
			if (m_hOperate == NULL)
			{
				OperateType = 4;
				Parameter.Format(_T("version_id=%s"), VersionId);

				m_hOperate = AfxBeginThread(Operate, this);
			}

			// ���б���ɾ������
			m_Version.DeleteItem(m_Version.GetNextItem(-1, LVIS_SELECTED));
		}
		else
		{
			AfxMessageBox(_T("һ��ֻ��ɾ��һ���汾!"));
		}
	}
}


void CWebProjectDlg::OnClone()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CWebProjectDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
