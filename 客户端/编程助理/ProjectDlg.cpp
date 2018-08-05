// ProjectDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "ProjectDlg.h"
#include "afxdialogex.h"

#include "ModifyDlg.h"
#include "NotifyDlg.h"
#include "SearchDlg.h"


// CProjectDlg �Ի���

IMPLEMENT_DYNAMIC(CProjectDlg, CDialogEx)

CProjectDlg::CProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProjectDlg::IDD, pParent)
{
	m_hOperate = NULL;
}

CProjectDlg::~CProjectDlg()
{
}

void CProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROJECT_LIST, m_Project);
	DDX_Control(pDX, IDC_VERSION_LIST, m_Version);
}


BEGIN_MESSAGE_MAP(CProjectDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CProjectDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CProjectDlg::OnCancel)
	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CProjectDlg::OnEdit)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, &CProjectDlg::OnDelete)
	ON_BN_CLICKED(IDC_ADD_BUTTON, &CProjectDlg::OnAdd)
	ON_BN_CLICKED(IDC_RESTORE_BUTTON, &CProjectDlg::OnRestore)
	ON_BN_CLICKED(IDC_MODIFY_BUTTON, &CProjectDlg::OnModify)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, &CProjectDlg::OnRemove)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PROJECT_LIST, &CProjectDlg::OnItemchangedProjectList)
	ON_NOTIFY(NM_CLICK, IDC_PROJECT_LIST, &CProjectDlg::OnClickProjectList)
	ON_NOTIFY(NM_DBLCLK, IDC_PROJECT_LIST, &CProjectDlg::OnDblclkProjectList)
	ON_NOTIFY(NM_RCLICK, IDC_PROJECT_LIST, &CProjectDlg::OnRclickProjectList)
	ON_NOTIFY(NM_DBLCLK, IDC_VERSION_LIST, &CProjectDlg::OnDblclkVersionList)
	ON_NOTIFY(NM_RCLICK, IDC_VERSION_LIST, &CProjectDlg::OnRclickVersionList)
END_MESSAGE_MAP()


// CProjectDlg ��Ϣ�������


BOOL CProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Project.SetExtendedStyle(LVS_EX_FULLROWSELECT  //��������ѡ��
		| LVS_EX_HEADERDRAGDROP				      //���������϶�
		| LVS_EX_SUBITEMIMAGES			          //����ѡ����
		| LVS_EX_GRIDLINES);				      //����������

	m_Project.SetHeadings(_T("��Ŀ����,147;��Ŀ�汾,120;��Ŀ·��,0;"));
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

	m_Version.SetHeadings(_T("�޸�˵��,161;�޸�ʱ��,150;��Ŀ�汾,120;"));
	m_Version.LoadColumnInfo();

	// ����
    pfont1   =   m_Version.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //��������޸�����ĸ߱���
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //��������޸�����Ŀ����
  
    font1.CreateFontIndirect(&logfont);
    m_Version.SetFont(&font1);  
    font1.Detach();

	// ˢ��
	Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CProjectDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CProjectDlg::OnItemchangedProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	OnClickProjectList(pNMHDR, pResult);

	*pResult = 0;
}


void CProjectDlg::OnClickProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if(m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		Name = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 0);
		Path = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 2);
	}

	if (!Name.IsEmpty() && m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		// ���
		this->pResult.clear();

		if (theApp.m_Sql.SelectData(_T("�汾����"), this->pResult, _T("Name = '") + Name + _T("'") ))
		{
			// ����б�
			m_Version.DeleteAllItems();

			// ѭ���������(�ж����� i �ͼӶ���)
			for (int i = 0 ;i < (int)this->pResult.size(); i += 5)
			{
				CString Name     = this->pResult[i],
					    Explain  = this->pResult[i + 1],
					    Time     = this->pResult[i + 2],
					    Version  = this->pResult[i + 3];

				m_Version.AddItem(Explain, Time, Version);
			}
		}
	}

	*pResult = 0;
}


void CProjectDlg::OnDblclkProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if(m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
		Path = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 2);

	if (!Path.IsEmpty() && m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		ShellExecute(NULL, _T("open"), Path, NULL, NULL, SW_SHOWNORMAL);
	}

	*pResult = 0;
}


void CProjectDlg::OnRclickProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CSearchDlg dlg;
	dlg.Title    = _T("������Ŀ");
	dlg.FilePath = "Project";
	dlg.FindFile = false;

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
				break;
			}
		}
	}

	*pResult = 0;
}


void CProjectDlg::OnDblclkVersionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CString Version = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2);
	if (!Version.IsEmpty() && m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString VersionPath = _T("Project\\") + Name + _T("\\") + Version;
		ShellExecute(NULL, _T("open"), VersionPath, NULL, NULL, SW_SHOWNORMAL);
	}

	*pResult = 0;
}


void CProjectDlg::OnRclickVersionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (!Name.IsEmpty())
	{
		CString VersionPath = _T("Project\\") + Name;

		CSearchDlg dlg;
		dlg.Title    = _T("���Ұ汾");
		dlg.FilePath = VersionPath;
		dlg.FindFile = false;

		if(dlg.DoModal() == IDOK)
		{
			int Count = m_Version.GetItemCount();
			for(int i=0; i<Count; i++)
			{
				CString Version = m_Version.GetItemText(i, 2);

				if(Version == dlg.Function)
				{
					//���ø�����ʾ  
					m_Version.SetFocus();//���ý���  
					m_Version.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
					m_Version.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 
					break;
				}
			}
		}
	}

	*pResult = 0;
}


void CProjectDlg::Refresh()
{
	// ���
	pResult.clear();

	if (theApp.m_Sql.SelectData(_T("��Ŀ����"), pResult, _T("Type = 'Local'") ))
	{
		// ����б�
		m_Project.DeleteAllItems();

		// ѭ���������(�ж����� i �ͼӶ���)
		for (int i = 0 ;i < (int)pResult.size(); i += 4)
		{
			CString Name     = pResult[i],
				    Path     = pResult[i + 1],
				    Type     = pResult[i + 2],
				    Version  = pResult[i + 3];

			m_Project.AddItem(Name, Version, Path);
		}
	}
}


void CProjectDlg::CopyDirectory(CString source, CString target)  
{
    CreateDirectory(target,NULL); //����Ŀ���ļ���
    //AfxMessageBox("�����ļ���"+target);
    CFileFind finder;
    CString path;
    path.Format(_T("%s/*.*"),source);
    //AfxMessageBox(path);
    BOOL bWorking =finder.FindFile(path);
    while(bWorking)
    {
        bWorking = finder.FindNextFile();
        //AfxMessageBox(finder.GetFileName());
        if(finder.IsDirectory() && !finder.IsDots())//���ļ��� ���� ���Ʋ��� . �� ..
		{
			// ���޵ݹ�bug����
			char szAppPath[MAX_PATH];
			GetModuleFileName(NULL, szAppPath, MAX_PATH);
			(strrchr(szAppPath, '\\'))[0] = 0;
			if(finder.GetFilePath().Replace(szAppPath, _T("")) && target.Replace(szAppPath, _T("")) )
			{
				AfxMessageBox(_T("���ܽ�����������ӽ���Ŀ����!"));
				break;
			}

			//�ݹ鴴���ļ���+"/"+finder.GetFileName()
			CopyDirectory(finder.GetFilePath(),target+"/"+finder.GetFileName());
		}
        else//���ļ� ��ֱ�Ӹ���
        {
            //AfxMessageBox("�����ļ�"+finder.GetFilePath());//+finder.GetFileName()
            CopyFile(finder.GetFilePath(),target+"/"+finder.GetFileName(),FALSE);
        }
    }
}


void CProjectDlg::DeleteDirectory(CString Directory)
{   
    CFileFind finder;
    CString path;
    path.Format(_T("%s/*.*"),Directory);
    BOOL bWorking = finder.FindFile(path);
    while(bWorking)
    {
        bWorking = finder.FindNextFile();
        if(finder.IsDirectory() && !finder.IsDots())//�����ļ���
        {
            DeleteDirectory(finder.GetFilePath()); //�ݹ�ɾ���ļ���
            RemoveDirectory(finder.GetFilePath());
        }
        else//�����ļ�
        {
            DeleteFile(finder.GetFilePath());
        }
    }

    RemoveDirectory(Directory);
}


// ���Ĺ����߳�
UINT CProjectDlg::Operate(LPVOID pParam)
{
	// ����ָ��
	CProjectDlg * pWnd = ((CProjectDlg*)pParam);

	// ִ�в���
	switch(pWnd->Type)
	{
	case 1:
		//������Ŀ
		pWnd->CopyDirectory(pWnd->TargetPath, pWnd->FilePath);
		break;

	case 2:
		//ɾ����Ŀ
		pWnd->DeleteDirectory(pWnd->TargetPath);
		break;
	}

	// ������ɱ�־
	theApp.IsFinished = true;

	// ������Ϊ��
	pWnd->m_hOperate = NULL;
	return false;
}


// ��Ϣ��ʾ�߳�
UINT CProjectDlg::Notify(LPVOID lpParameter)
{
	((CProjectDlg*)lpParameter)->OnShowNotify((LPCTSTR)lpParameter);
	return TRUE;
}


// ��ʾ��Ϣ����
void CProjectDlg::OnShowNotify(CString Notify)
{
	// ������ɱ�־
	theApp.IsFinished = false;

	CNotifyDlg m_pNotifyDlg;
	m_pNotifyDlg.DoModal();
}


void CProjectDlg::OnOK()
{
	CModifyDlg dlg;
	dlg.IsNew = true;
	dlg.IsProject = true;

	if(dlg.DoModal() == IDOK)
	{
		if (dlg.m_Path.Replace(_T("'"), _T("")))
		{
			AfxMessageBox(_T("��Ŀ·�����ܴ�������!"));
			return;
		}

		// ���޵ݹ�bug����
		char szAppPath[MAX_PATH];
		GetModuleFileName(NULL, szAppPath, MAX_PATH);
		(strrchr(szAppPath, '\\'))[0] = 0;
		if(dlg.m_Path.Replace(szAppPath, _T("")))
		{
			AfxMessageBox(_T("���ܽ�����������ӽ���Ŀ����!"));
			return;
		}
		

		// �ж����ݱ����Ƿ��������
		if (!theApp.m_Sql.CheckData(_T("��Ŀ����"), 1, _T("Path = '") + dlg.m_Path + _T("' and Type = 'Local'")))
		{
			// ��д�����ݿ�ʧ��
			if (!theApp.m_Sql.InsertData(_T("��Ŀ����"), _T("'") + dlg.m_Name + _T("','") + dlg.m_Path + _T("','Local','") + dlg.m_Version + _T("'")))
			{
				AfxMessageBox(_T("�޷������Ŀ����!"));
				return;
			}
			else
			{
				CString ModifyTime; //��ȡϵͳʱ�� ����
				CTime tm; tm = CTime::GetCurrentTime();
				ModifyTime   = tm.Format(_T("%Y-%m-%d %H:%M:%S"));

				// ��д��汾��ʧ��
				if (!theApp.m_Sql.InsertData(_T("�汾����"), _T("'") + dlg.m_Name + _T("','�������Ŀ�汾','") +  ModifyTime + _T("','") + dlg.m_Version + _T("','Local'")))
				{
					AfxMessageBox(_T("�޷������Ŀ�汾!"));
					return;
				}
				else
				{
					// Ŀ¼���
					FilePath = _T("Project/") + dlg.m_Name;
					if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //�ļ��в�����
					{
						CreateDirectory(FilePath, NULL);
					}

					FilePath = _T("Project/") + dlg.m_Name + _T("/") + dlg.m_Version;
					if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //�ļ��в�����
					{
						CreateDirectory(FilePath, NULL);
					}

					// ������Ŀ

					// ��ʾ��Ϣ��ʾ
					AfxBeginThread(Notify, _T("����ִ�в���..."));

					// �����������߳�
					TargetPath = dlg.m_Path;
					if (m_hOperate == NULL)
					{
						Type = 1;
						m_hOperate = AfxBeginThread(Operate, this);
					}

					// ����б�
					m_Project.AddItem(dlg.m_Name, dlg.m_Version, dlg.m_Path);
					m_Version.AddItem(_T("�������Ŀ�汾"), ModifyTime, dlg.m_Version);

					//���ø�����ʾ
					int i = m_Project.GetItemCount() -1;
					m_Project.SetItemState(m_Project.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

					m_Project.SetFocus();//���ý���  
					m_Project.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
					m_Project.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 
				}
			}
		}
		else
		{
			AfxMessageBox(_T("�Ѵ�����ͬ����Ŀ!"));
			return;
		}
	}
}


void CProjectDlg::OnEdit()
{
	int Row = m_Project.GetNextItem(-1, LVIS_SELECTED);
	CString Name    = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 0);
	CString Path    = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 2);
	CString Version = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 1);

	int Count = m_Project.GetSelectedCount();
	if(Count > 1)
	{
		AfxMessageBox(_T("һ��ֻ�ܱ༭һ����Ŀ!"));
		return;
	}

	if (!Name.IsEmpty() && m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CModifyDlg dlg;
		dlg.IsNew = false;
		dlg.IsProject = true;

		dlg.m_Name    = Name;
		dlg.m_Path    = Path;
		dlg.m_Version = Version;
		dlg.pResult   = pResult;

		if(dlg.DoModal() == IDOK)
		{
			CString ColumnParams = _T("Name = '") + dlg.m_Name + _T("', Path = '") + dlg.m_Path + _T("', Version = '") + dlg.m_Version + _T("'"),
				Params = _T("Path = '") + Path + _T("' And Type = 'Local'");

			if (!theApp.m_Sql.UpdataData(_T("��Ŀ����"), ColumnParams, Params))
			{
				AfxMessageBox(_T("�޷��޸���Ŀ����!"));
			}
			else
			{
				ColumnParams = _T("Name = '") + dlg.m_Name + _T("'");
				Params = _T("Name = '") + Name + _T("'");

				if (!theApp.m_Sql.UpdataData(_T("�汾����"), ColumnParams, Params))
				{
					AfxMessageBox(_T("�޷��޸���Ŀ�汾!"));
				}
				else
				{
					// ��������ĿĿ¼
					if(MoveFileEx(_T("Project/") + Name, _T("Project/") + dlg.m_Name, MOVEFILE_REPLACE_EXISTING))
					{
						// �����б�
						m_Project.SetItemText(Row, 0, dlg.m_Name);
						m_Project.SetItemText(Row, 1, dlg.m_Version);
						m_Project.SetItemText(Row, 2, dlg.m_Path);

						//���ø�����ʾ
						m_Project.SetFocus();//���ý���  
						m_Project.SetItemState(Row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
						m_Project.EnsureVisible(Row, FALSE);//���õ�ǰ��ͼ�ɼ� 
					}
					else
					{
						AfxMessageBox(_T("�޷���������Ŀ!"));
					}
				}
			}
		}
	}
	else
	{
		AfxMessageBox(_T("��ѡ��Ҫ�༭����Ŀ!"));
	}
}


void CProjectDlg::OnDelete()
{
	int Count = m_Project.GetSelectedCount();
	CString Name    = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 0);
	CString Path    = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 2);
	CString Version = m_Project.GetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 1);

	if (!Name.IsEmpty() && m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		if(Count == 1)
		{
			if (!Name.IsEmpty() && m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
			{
				if(MessageBox(_T("�Ƿ�Ҫ�Ƴ���ǰ��Ŀ? "), NULL, MB_YESNO) == IDYES)
				{
					// �����ݿ���ɾ������
					if (!theApp.m_Sql.DeleteData(_T("��Ŀ����"), _T(" Path = '") + Path + _T("' and Type = 'Local'")))
					{
						AfxMessageBox(_T("ɾ������ʧ��!"));
					}
					else
					{
						// �����ݿ���ɾ���汾
						if (!theApp.m_Sql.DeleteData(_T("�汾����"), _T(" Name = '") + Name + _T("'")))
						{
							AfxMessageBox(_T("ɾ������ʧ��!"));
						}
						else
						{
							// ɾ������Ŀ¼

							// ��ʾ��Ϣ��ʾ
							AfxBeginThread(Notify, _T("����ִ�в���..."));

							// �����������߳�
							TargetPath = _T("Project/") + Name;
							if (m_hOperate == NULL)
							{
								Type = 2;
								m_hOperate = AfxBeginThread(Operate, this);
							}

							// ���б���ɾ������
							m_Project.DeleteItem(m_Project.GetNextItem(-1, LVIS_SELECTED));

							// ��հ汾����
							m_Version.DeleteAllItems();
						}
					}
				}
			}
			else
			{
				AfxMessageBox(_T("��ѡ��Ҫ�Ƴ�����Ŀ!"));
			}
		}
		else if(Count > 1)
		{
			if(MessageBox(_T("�Ƿ�Ҫ�Ƴ�ѡ�е���Ŀ? "), NULL, MB_YESNO) == IDYES)
			{
				int nItem = -1;
				POSITION pos;
				while (pos = m_Project.GetFirstSelectedItemPosition())
				{
					nItem = -1;
					nItem = m_Project.GetNextSelectedItem(pos);
					if (nItem >= 0 && m_Project.GetSelectedCount() > 0)
					{
						Name    = m_Project.GetItemText(nItem, 0);
						Path    = m_Project.GetItemText(nItem, 2);
						Version = m_Project.GetItemText(nItem, 1);

						// �����ݿ���ɾ������
						if (!theApp.m_Sql.DeleteData(_T("��Ŀ����"), _T(" Path = '") + Path + _T("' and Type = 'Local'")))
						{
							AfxMessageBox(_T("ɾ������ʧ��!"));
						}
						else
						{
							// �����ݿ���ɾ���汾
							if (!theApp.m_Sql.DeleteData(_T("�汾����"), _T(" Name = '") + Name + _T("'")))
							{
								AfxMessageBox(_T("ɾ������ʧ��!"));
							}
							else
							{
								// ɾ������Ŀ¼

								// ��ʾ��Ϣ��ʾ
								AfxBeginThread(Notify, _T("����ִ�в���..."));

								// �����������߳�
								TargetPath = _T("Project/") + Name;
								if (m_hOperate == NULL)
								{
									Type = 2;
									m_hOperate = AfxBeginThread(Operate, this);
								}
							}
						}

						m_Project.DeleteItem(nItem);
					}
				}

				//for (int i = 0; i < Count; i++)
				//{
				//	int Row = m_Project.GetNextItem(i - 1, LVIS_SELECTED);
				//	Name    = m_Project.GetItemText(Row, 0);
				//	Path    = m_Project.GetItemText(Row, 2);
				//	Version = m_Project.GetItemText(Row, 1);

				//	// �����ݿ���ɾ������
				//	if (!theApp.m_Sql.DeleteData(_T("��Ŀ����"), _T(" Path = '") + Path + _T("' and Type = 'Local'")))
				//	{
				//		AfxMessageBox(_T("ɾ������ʧ��!"));
				//	}
				//	else
				//	{
				//		// �����ݿ���ɾ���汾
				//		if (!theApp.m_Sql.DeleteData(_T("�汾����"), _T(" Name = '") + Name + _T("'")))
				//		{
				//			AfxMessageBox(_T("ɾ������ʧ��!"));
				//		}
				//		else
				//		{
				//			// ɾ������Ŀ¼
				//			
				//			// ��ʾ��Ϣ��ʾ
				//			AfxBeginThread(Notify, _T("����ִ�в���..."));

				//			// �����������߳�
				//			TargetPath = _T("Project/") + Name;
				//			if (m_hOperate == NULL)
				//			{
				//				Type = 2;
				//				m_hOperate = AfxBeginThread(Operate, this);
				//			}
				//		}
				//	}
				//}

				// ��հ汾����
				//m_Version.DeleteAllItems();

				//Refresh();
			}
		}
		else
		{
			AfxMessageBox(_T("��ѡ��Ҫ�Ƴ�����Ŀ!"));
		}
	}
	else
	{
		AfxMessageBox(_T("��ѡ��Ҫ�Ƴ�����Ŀ!"));
	}
}


void CProjectDlg::OnAdd()
{
	if (!Name.IsEmpty() && m_Project.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CModifyDlg dlg;
		dlg.IsNew = true;
		dlg.IsProject = false;

		int Count = m_Project.GetSelectedCount();
		if(Count > 1)
		{
			AfxMessageBox(_T("һ��ֻ�����һ����Ŀ��һ���汾!"));
			return;
		}

		if(dlg.DoModal() == IDOK)
		{
			if (dlg.m_Name.Replace(_T("'"), _T("")))
			{
				AfxMessageBox(_T("�޸�˵�����ܴ�������!"));
				return;
			}

			// �ж����ݱ����Ƿ��������
			if (!theApp.m_Sql.CheckData(_T("�汾����"), 1, _T("Name = '") + Name + _T("' and Version = '") + dlg.m_Version + _T("' and Type = 'Local'")))
			{
				CString ModifyTime; //��ȡϵͳʱ��
				CTime tm; tm = CTime::GetCurrentTime();
				ModifyTime   = tm.Format(_T("%Y-%m-%d %H:%M:%S"));

				// ��д��汾��ʧ��
				if (!theApp.m_Sql.InsertData(_T("�汾����"), _T("'") + Name + _T("','") + dlg.m_Name + _T("','") +  ModifyTime + _T("','") + dlg.m_Version + _T("','Local'")))
				{
					AfxMessageBox(_T("�޷������Ŀ�汾!"));
					return;
				}
				else
				{
					// �޸ĵ�ǰ�汾
					CString ColumnParams = _T("Version = '") + dlg.m_Version + _T("'"), Params = _T("Path = '") + Path + _T("' And Type = 'Local'");
					if (!theApp.m_Sql.UpdataData(_T("��Ŀ����"), ColumnParams, Params))
					{
						AfxMessageBox(_T("�޷��޸���Ŀ�汾����!"));
						return;
					}
					else
					{
						int Count = m_Project.GetItemCount();
						for(int i=0; i<Count; i++)
						{
							CString Project = m_Project.GetItemText(i, 0);

							if(Project == Name)
							{
								// �޸İ汾��
								m_Project.SetItemText(i, 1, dlg.m_Version);
								break;
							}
						}
					}


					// Ŀ¼���
					FilePath = _T("Project/") + Name + _T("/") + dlg.m_Version;
					if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //�ļ��в�����
					{
						CreateDirectory(FilePath, NULL);
					}

					// ������Ŀ

					// ��ʾ��Ϣ��ʾ
					AfxBeginThread(Notify, _T("����ִ�в���..."));

					// �����������߳�
					TargetPath = Path;
					if (m_hOperate == NULL)
					{
						Type = 1;
						m_hOperate = AfxBeginThread(Operate, this);
					}

					// ����б�
					m_Version.AddItem(dlg.m_Name, ModifyTime, dlg.m_Version);

					//���ø�����ʾ
					int i = m_Version.GetItemCount() -1;
					m_Version.SetItemState(m_Version.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

					m_Version.SetFocus();//���ý���  
					m_Version.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
					m_Version.EnsureVisible(i, FALSE);//���õ�ǰ��ͼ�ɼ� 
				}
			}
			else
			{
				AfxMessageBox(_T("�Ѵ�����ͬ�İ汾!"));
			}
		}
	}
	else
	{
		AfxMessageBox(_T("��ѡ��Ҫ��Ӱ汾����Ŀ!"));
	}
}


void CProjectDlg::OnRestore()
{
	int Count = m_Version.GetSelectedCount();
	if(Count > 1)
	{
		AfxMessageBox(_T("һ��ֻ�ܻ�ԭһ���汾!"));
		return;
	}

	CString Version = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2);
	if (!Name.IsEmpty() && m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		if(MessageBox(_T("�Ƿ�Ҫ�ָ���ǰ�汾? "), NULL, MB_YESNO) == IDYES)
		{
			// �޸ĵ�ǰ�汾
			CString ColumnParams = _T("Version = '") + Version + _T("'"), Params = _T("Path = '") + Path + _T("' And Type = 'Local'");
			if (!theApp.m_Sql.UpdataData(_T("��Ŀ����"), ColumnParams, Params))
			{
				AfxMessageBox(_T("�޷��޸���Ŀ�汾����!"));
				return;
			}
			else
			{
				int Count = m_Project.GetItemCount();
				for(int i=0; i<Count; i++)
				{
					CString Project = m_Project.GetItemText(i, 0);

					if(Project == Name)
					{
						// �޸İ汾��
						m_Project.SetItemText(i, 1, Version);
						break;
					}
				}
			}

			// ��Ŀ·��
			FilePath = _T("Project/") + Name + _T("/") + Version;

			// ������Ŀ
			TargetPath = FilePath;
			FilePath   = Path;

			// ��ʾ��Ϣ��ʾ
			AfxBeginThread(Notify, _T("����ִ�в���..."));

			// �����������߳�
			if (m_hOperate == NULL)
			{
				Type = 1;
				m_hOperate = AfxBeginThread(Operate, this);
			}

			//MessageBox(_T("�汾�ָ����."));
		}
	}
	else
	{
		AfxMessageBox(_T("��ѡ��Ҫ�ָ��İ汾!"));
	}
}


void CProjectDlg::OnModify()
{
	int Row = m_Version.GetNextItem(-1, LVIS_SELECTED);
	CString Explain = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 0);
	CString Version = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2);

	int Count = m_Version.GetSelectedCount();
	if(Count > 1)
	{
		AfxMessageBox(_T("һ��ֻ�ܱ༭һ���汾!"));
		return;
	}

	if (!Explain.IsEmpty() && m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CModifyDlg dlg;
		dlg.IsNew = false;
		dlg.IsProject = false;

		dlg.m_Name    = Explain;
		dlg.m_Version = Version;

		if(dlg.DoModal() == IDOK)
		{
			CString ModifyTime; //��ȡϵͳʱ�� ����
			CTime tm; tm = CTime::GetCurrentTime();
			ModifyTime   = tm.Format(_T("%Y-%m-%d %H:%M:%S"));

			CString ColumnParams = _T("Explain = '") + dlg.m_Name + _T("', ModifyTime = '") + ModifyTime + _T("', Version = '") + dlg.m_Version + _T("'");
			CString Params = _T("Name = '") + Name + _T("' and Version = '") + Version + _T("'");

			if (!theApp.m_Sql.UpdataData(_T("�汾����"), ColumnParams, Params))
			{
				AfxMessageBox(_T("�޷��޸���Ŀ�汾!"));
			}
			else
			{
				// ��������ĿĿ¼
				if(MoveFileEx(_T("Project/") + Name + _T("/") + Version, _T("Project/") + Name + _T("/") + dlg.m_Version, MOVEFILE_REPLACE_EXISTING))
				{
					// �����б�
					m_Version.SetItemText(Row, 0, dlg.m_Name);
					m_Version.SetItemText(Row, 1, ModifyTime);
					m_Version.SetItemText(Row, 2, dlg.m_Version);

					//���ø�����ʾ
					m_Version.SetFocus();//���ý���  
					m_Version.SetItemState(Row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//����״̬  
					m_Version.EnsureVisible(Row, FALSE);//���õ�ǰ��ͼ�ɼ� 
				}
				else
				{
					AfxMessageBox(_T("�޷��������汾!"));
				}
			}
		}
	}
	else
	{
		AfxMessageBox(_T("��ѡ��Ҫ�༭�İ汾!"));
	}
}


void CProjectDlg::OnRemove()
{
	int Count = m_Version.GetSelectedCount();
	CString Explain = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 0);
	CString Version = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2);

	if (!Explain.IsEmpty() && m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		if(Count == 1)
		{
			if (!Name.IsEmpty() && m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
			{
				if(MessageBox(_T("�Ƿ�Ҫ�Ƴ���ǰ�汾? "), NULL, MB_YESNO) == IDYES)
				{
					// �����ݿ���ɾ���汾
					if (!theApp.m_Sql.DeleteData(_T("�汾����"), _T(" Name = '") + Name + _T("' and Version = '") +  Version + _T("'") ))
					{
						AfxMessageBox(_T("�Ƴ��汾����ʧ��!"));
					}
					else
					{
						// ɾ������Ŀ¼

						// ��ʾ��Ϣ��ʾ
						AfxBeginThread(Notify, _T("����ִ�в���..."));

						// �����������߳�
						TargetPath = _T("Project/") + Name + _T("/") + Version;
						if (m_hOperate == NULL)
						{
							Type = 2;
							m_hOperate = AfxBeginThread(Operate, this);
						}

						// ���б���ɾ������
						m_Version.DeleteItem(m_Version.GetNextItem(-1, LVIS_SELECTED));
					}
				}
			}
			else
			{
				AfxMessageBox(_T("��ѡ��Ҫ�Ƴ��İ汾!"));
			}
		}
		else if(Count > 1)
		{
			if(MessageBox(_T("�Ƿ�Ҫ�Ƴ�ѡ�еİ汾? "), NULL, MB_YESNO) == IDYES)
			{
				int nItem = -1;
				POSITION pos;
				while (pos = m_Version.GetFirstSelectedItemPosition())
				{
					nItem = -1;
					nItem = m_Version.GetNextSelectedItem(pos);
					if (nItem >= 0 && m_Version.GetSelectedCount() > 0)
					{
						Explain = m_Version.GetItemText(nItem, 0);
						Version = m_Version.GetItemText(nItem, 2);

						// �����ݿ���ɾ���汾
						if (!theApp.m_Sql.DeleteData(_T("�汾����"), _T(" Name = '") + Name + _T("' and Version = '") +  Version + _T("'") ))
						{
							AfxMessageBox(_T("�Ƴ��汾����ʧ��!"));
						}
						else
						{
							// ɾ������Ŀ¼

							// ��ʾ��Ϣ��ʾ
							AfxBeginThread(Notify, _T("����ִ�в���..."));

							// �����������߳�
							TargetPath = _T("Project/") + Name + _T("/") + Version;
							if (m_hOperate == NULL)
							{
								Type = 2;
								m_hOperate = AfxBeginThread(Operate, this);
							}
						}
						
						m_Version.DeleteItem(nItem);
					}
				}
			
				//for (int i = 0; i < Count; i++)
				//{
				//	int Row = m_Version.GetNextItem(i - 1, LVIS_SELECTED);
				//	if(Row == Count)
				//	{
				//		Row -= 1;
				//	}

				//	Explain = m_Version.GetItemText(Row, 0);
				//	Version = m_Version.GetItemText(Row, 2);

				//	// �����ݿ���ɾ���汾
				//	if (!theApp.m_Sql.DeleteData(_T("�汾����"), _T(" Name = '") + Name + _T("' and Version = '") +  Version + _T("'") ))
				//	{
				//		AfxMessageBox(_T("�Ƴ��汾����ʧ��!"));
				//	}
				//	else
				//	{
				//		// ɾ������Ŀ¼

				//		// ��ʾ��Ϣ��ʾ
				//		AfxBeginThread(Notify, _T("����ִ�в���..."));

				//		// �����������߳�
				//		TargetPath = _T("Project/") + Name + _T("/") + Version;
				//		if (m_hOperate == NULL)
				//		{
				//			Type = 2;
				//			m_hOperate = AfxBeginThread(Operate, this);
				//		}
				//	}
				//}

				//// ��������
				//OnItemchangedProjectList(NULL, NULL);
			}
		}
		else
		{
			AfxMessageBox(_T("��ѡ��Ҫ�Ƴ��İ汾!"));
		}
	}
	else
	{
		AfxMessageBox(_T("��ѡ��Ҫ�Ƴ��İ汾!"));
	}
}


void CProjectDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
