// SynchronizeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "SynchronizeDlg.h"
#include "afxdialogex.h"
#include "MainDlg.h"

#include "TransmissionDlg.h"
#include "WebProjectDlg.h"
#include "ProjectDlg.h"

// CSynchronizeDlg �Ի���

IMPLEMENT_DYNAMIC(CSynchronizeDlg, CDialogEx)

CSynchronizeDlg::CSynchronizeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSynchronizeDlg::IDD, pParent)
{
	IsCode = TRUE;
	m_hOperate = m_hDelete = NULL;
	Type = 1;

	IsAutoSynchronize = FALSE;
}

CSynchronizeDlg::~CSynchronizeDlg()
{
}

void CSynchronizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOCAL_TREE, m_Local);
	DDX_Control(pDX, IDC_SERVER_TREE, m_Server);
}


BEGIN_MESSAGE_MAP(CSynchronizeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSynchronizeDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CSynchronizeDlg::OnCancel)
	ON_NOTIFY(NM_CLICK, IDC_LOCAL_TREE, &CSynchronizeDlg::OnClickLocalTree)
	ON_NOTIFY(NM_CLICK, IDC_SERVER_TREE, &CSynchronizeDlg::OnClickServerTree)
	ON_NOTIFY(NM_DBLCLK, IDC_LOCAL_TREE, &CSynchronizeDlg::OnDblclkLocalTree)
	ON_NOTIFY(NM_DBLCLK, IDC_SERVER_TREE, &CSynchronizeDlg::OnDblclkServerTree)
	ON_BN_CLICKED(IDC_UPLOAD_BUTTON, &CSynchronizeDlg::OnUpload)
	ON_BN_CLICKED(IDC_DOWNLOAD_BUTTON, &CSynchronizeDlg::OnDownload)
	ON_BN_CLICKED(IDC_SWITCH_BUTTON, &CSynchronizeDlg::OnSwitch)
	ON_BN_CLICKED(IDC_VERSION_BUTTON, &CSynchronizeDlg::OnVersion)

	ON_COMMAND(100, &CSynchronizeDlg::OnSuccess)
	ON_COMMAND(101, &CSynchronizeDlg::OnError)
	ON_COMMAND(102, &CSynchronizeDlg::OnMessage)
END_MESSAGE_MAP()


// CSynchronizeDlg ��Ϣ�������


BOOL CSynchronizeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ˢ������
	HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("����Դ��� (�� delete ɾ��Դ��)"));//������ڵ�
	ShowFile(_T("Code"), m_TreeRoot);//��Ŀ¼���б���


	// ��ȡ����������
	if(!IsAutoSynchronize)
		GetServerInfo();

	// ��ʾ��
	m_toolTips.Create(this, TTS_ALWAYSTIP|WS_POPUP);
	m_toolTips.Activate(TRUE);

	m_toolTips.AddTool(GetDlgItem(IDC_LOCAL_TREE),      _T("��ʾ���ؿ����ݡ�\n˫��Ŀ����ϴ����ƶˡ�\n����delete�����˸��ɾ��ѡ��Ŀ�ꡣ"));
	m_toolTips.AddTool(GetDlgItem(IDC_SERVER_TREE),     _T("��ʾ�ƶ˿����ݡ�\n˫��Ŀ������������ء�\n����delete�����˸��ɾ��ѡ��Ŀ�ꡣ"));
	m_toolTips.AddTool(GetDlgItem(IDC_UPLOAD_BUTTON),   _T("�ϴ�ѡ��Ŀ�����ƶˡ�"));
	m_toolTips.AddTool(GetDlgItem(IDC_DOWNLOAD_BUTTON), _T("����ѡ��Ŀ�������ء�"));
	m_toolTips.AddTool(GetDlgItem(IDOK),                _T("�Ա������ƶ˿��Զ�������ͬ�����ݡ�\n�ɰ��»س�����ͬ����"));
	m_toolTips.AddTool(GetDlgItem(IDC_SWITCH_BUTTON),   _T("�л�����/�ƶ˵ķ���/�ļ��⡣"));
	m_toolTips.AddTool(GetDlgItem(IDC_VERSION_BUTTON),  _T("�����ƶ���Ŀ����ϵͳ��"));

	//������ɫ
	m_toolTips.SetTipTextColor(RGB(0,0,255));

	//���ָ���ú���ʾ��ʾ������
	m_toolTips.SetDelayTime(TTDT_INITIAL, 10); 

	//��걣��ָ����ʾ��ʾ��ã�����
	m_toolTips.SetDelayTime(TTDT_AUTOPOP, 9000000);

	//�趨��ʾ��ȣ����������Զ�����
	m_toolTips.SetMaxTipWidth(300);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CSynchronizeDlg::PreTranslateMessage(MSG* pMsg)
{
	UINT  nKeyCode = pMsg->wParam; // virtual key code of the key pressed
	if (pMsg->message == WM_KEYDOWN)
	{
		// ����Delete �� BACK
		if ( nKeyCode == VK_DELETE || nKeyCode == VK_BACK )
		{
			OnDelete();
			return true;
		}
	}

	// ������ʾ
	if(GetPrivateProfileInt(_T("Setting"), _T("Tip"), 0, _T("./Setting.ini")) == 1)
		m_toolTips.RelayEvent(pMsg); // ������Ϣ��Ӧ

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSynchronizeDlg::ShowFile(CString str_Dir, HTREEITEM tree_Root)
{
    CFileFind FileFind;

    //��ʱ���������Լ�¼���ص����ڵ�
    HTREEITEM tree_Temp;

    //�ж�����Ŀ¼����Ƿ����'\'���������򲹳�
    if (str_Dir.Right(1) != "\\")
        str_Dir += "\\";
    str_Dir += "*.*";
    BOOL res = FileFind.FindFile(str_Dir);
    while (res)
    {
        tree_Temp = tree_Root;
        res = FileFind.FindNextFile();
        if (FileFind.IsDirectory() && !FileFind.IsDots())//Ŀ¼���ļ���
        {
            CString strPath = FileFind.GetFilePath(); //�õ�·������Ϊ�ݹ���õĿ�ʼ
            CString strTitle = FileFind.GetFileName();//�õ�Ŀ¼������Ϊ���صĽ��
            tree_Temp = m_Local.InsertItem(strTitle, 0, 0, tree_Root);
            ShowFile(strPath, tree_Temp);
        }
        else if (!FileFind.IsDirectory() && !FileFind.IsDots())//������ļ�
        {
            CString strPath = FileFind.GetFilePath(); //�õ�·������Ϊ�ݹ���õĿ�ʼ
            CString strTitle = FileFind.GetFileName();//�õ��ļ�������Ϊ���صĽ��
            m_Local.InsertItem(strTitle, 0, 0, tree_Temp);
        }
    }

    FileFind.Close();
}


void CSynchronizeDlg::ConsistentParentCheck(CTreeCtrl * pTreeCtrl, HTREEITEM hTreeItem)
{
	// ��ȡ���ڵ㣬��Ϊ���򷵻أ�������״̬
	HTREEITEM hParentItem = pTreeCtrl->GetParentItem(hTreeItem);
	if(hParentItem != NULL)
	{
		// �����ж�ѡ����ڵ���ӽڵ�״̬
		HTREEITEM hChildItem = pTreeCtrl->GetChildItem(hParentItem);
		while(hChildItem != NULL)
		{
			// ����һ���ӽڵ�δѡ�У��򸸽ڵ�Ҳδѡ�У�ͬʱ�ݹ鴦���ڵ�ĸ��ڵ�
			if(pTreeCtrl->GetCheck(hChildItem) == FALSE)
			{
				pTreeCtrl->SetCheck(hParentItem, FALSE);
				return ConsistentParentCheck(pTreeCtrl, hParentItem);
			}

			// ��ȡ���ڵ����һ���ӽڵ�
			hChildItem = pTreeCtrl->GetNextItem(hChildItem, TVGN_NEXT);
		}

		// ���ӽڵ�ȫ��ѡ�У��򸸽ڵ�Ҳѡ�У�ͬʱ�ݹ鴦���ڵ�ĸ��ڵ�
		pTreeCtrl->SetCheck(hParentItem, TRUE);
		return ConsistentParentCheck(pTreeCtrl, hParentItem);
	}
}


void CSynchronizeDlg::ConsistentChildCheck(CTreeCtrl  * pTreeCtrl, HTREEITEM hTreeItem)
{
	// ��ȡ��ǰ��ѡ��ѡ��״̬
	BOOL bCheck = pTreeCtrl->GetCheck(hTreeItem);

	// ����ǰ�ڵ�����ӽڵ㣬��һ�»��ӽڵ�״̬
	if(pTreeCtrl->ItemHasChildren(hTreeItem))
	{
		// ����һ�»��ӽڵ���ӽڵ�״̬
		HTREEITEM hChildItem = pTreeCtrl->GetChildItem(hTreeItem);
		while(hChildItem != NULL)
		{
			pTreeCtrl->SetCheck(hChildItem, bCheck);
			ConsistentChildCheck(pTreeCtrl, hChildItem);

			hChildItem = pTreeCtrl->GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
}


void CSynchronizeDlg::Split(CString source, CString divKey, CStringArray &dest)
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


void CSynchronizeDlg::GetServerInfo()
{
	// �����������߳�
	if (m_hOperate == NULL)
	{
		Parameter.Format(_T("id=%s&type=%d"), UserId, Type);
		m_hOperate = AfxBeginThread(Operate, this);
	}
}


void CSynchronizeDlg::OnSuccess()
{
	switch(Type)
	{
	case 1:
		{
			CStringArray TextArray;
			Split(ServerInfo, _T(";"), TextArray);

			HTREEITEM rootItem = m_Server.InsertItem(_T("�ƶ�Դ��� (�� delete ɾ��Դ��)"), 0, 0, NULL), parentItem, subItem;

			CString Parent, Sub;
			for(int i=0; i<TextArray.GetSize() -1; i++)
			{
				CString FileInfo = TextArray.GetAt(i);

				// ���ݴ���
				FileInfo.Replace(_T("cloud/") + UserId + _T("/Code/"), _T(""));
				FileInfo.Replace(_T("cloud/") + UserId + _T("/Code"), _T(""));

				if (FileInfo.Replace(_T(".."), _T("..")) || FileInfo.Replace(_T("/."), _T("/.")) || FileInfo == _T(".") || FileInfo == _T(""))
				{
					//Ŀ¼
					continue;
				}
				else if(FileInfo.Replace(_T("/"), _T("/")) && !FileInfo.Replace(_T("."), _T(".")))
				{
					// ���ļ���
					FileInfo.Replace(Parent + _T("/"), _T(""));
					subItem = m_Server.InsertItem(FileInfo, 2, 2, parentItem);
					Sub = Parent + _T("/") + FileInfo;
				}
				else if(FileInfo.Replace(_T("."), _T(".")))
				{
					// �ļ�
					FileInfo.Replace(Sub + _T("/"), _T(""));
					m_Server.InsertItem(FileInfo, 3, 3, subItem);
				}
				else 
				{
					// �ļ���
					Parent = FileInfo;
					parentItem = m_Server.InsertItem(FileInfo, 1, 1, rootItem);
				}
			}
		} break;

	case 2:
		{
			CStringArray TextArray;
			Split(ServerInfo, _T(";"), TextArray);

			HTREEITEM rootItem = m_Server.InsertItem(_T("�ƶ��ļ��� (�� delete ɾ���ļ�)"), 0, 0, NULL), parentItem, subItem, subFile;

			CString Parent, Sub, Item, Function;
			for(int i=0; i<TextArray.GetSize() -1; i++)
			{
				CString FileInfo = TextArray.GetAt(i);

				// ���ݴ���
				FileInfo.Replace(_T("cloud/") + UserId + _T("/File/"), _T(""));
				FileInfo.Replace(_T("cloud/") + UserId + _T("/File"), _T(""));

				if (FileInfo.Replace(_T(".."), _T("..")) || FileInfo.Replace(_T("/."), _T("/.")) || FileInfo == _T(".") || FileInfo == _T(""))
				{
					//Ŀ¼
					continue;
				}

				else if(FileInfo.Replace(_T("/"), _T("/")) && !FileInfo.Replace(_T("."), _T(".")))
				{
					// ����
					if(!Item.IsEmpty() && FileInfo.Replace(Item + _T("/"), Item + _T("/")))
					{
						FileInfo.Replace(Sub + _T("/"), _T(""));
						subFile = m_Server.InsertItem(FileInfo, 3, 3, subItem);
						Function= FileInfo;
					}
					else
					{
						// ���ļ���
						FileInfo.Replace(Parent + _T("/"), _T(""));
						subItem = m_Server.InsertItem(FileInfo, 2, 2, parentItem);
						Sub = Parent + _T("/") + FileInfo;
						Item= FileInfo;
					}
				}

				else if(FileInfo.Replace(_T("."), _T(".")))
				{
					// �ļ�
					FileInfo.Replace(Sub + _T("/") + Function + _T("/"), _T(""));
					m_Server.InsertItem(FileInfo, 4, 4, subFile);
				}

				else 
				{
					// �ļ���
					Parent = FileInfo;
					parentItem = m_Server.InsertItem(FileInfo, 1, 1, rootItem);
				}
			}
		} break;
	}

	
}


void CSynchronizeDlg::OnError()
{
	AfxMessageBox(Error);
}


void CSynchronizeDlg::OnMessage()
{
	MessageBox(Message, _T("��ʾ��Ϣ"), MB_ICONINFORMATION);
}


void CSynchronizeDlg::OnDelete()
{
	// �����������߳�
	if (m_hDelete == NULL)
	{
		m_hDelete = AfxBeginThread(DeleteOperate, this);
	}
}


void CSynchronizeDlg::GetTreeData(CTreeCtrl * pTreeCtrl, HTREEITEM hitem, BOOL IsCheck)
{
	if(hitem != NULL)
	{
		hitem = pTreeCtrl->GetChildItem(hitem);
		while(hitem)
		{
			if(IsCheck)
			{
				// ���ѡ��
				if(pTreeCtrl->GetCheck(hitem))
				{
					CString text = pTreeCtrl->GetItemText(hitem);

					// ���ļ�
					if(text.Replace(_T("."), _T(".")))
					{
						IsFile  = true;

						CString CodeTarget = UserId + _T("/code") + TreeData + _T("/") + text;
						CString FileTarget = UserId + _T("/file") + TreeData + _T("/") + text;

						if(IsCode)
							TargetList.Add( UserId + _T("/code") + TreeData + _T("/") + text);
						else
							TargetList.Add( UserId + _T("/file") + TreeData + _T("/") + text);
					}
					else
					{
						IsFile = false;
						TreeData += _T("/") + text;
					}

					// ��¼��ǰ����
					if(IsFile)
					{
						OldTree = _T("");
					}
					else
					{
						OldTree = TreeData;
					}
				}
			}

			// ͬ��
			else
			{
				CString text = pTreeCtrl->GetItemText(hitem);

				// ���ļ�
				if(text.Replace(_T("."), _T(".")))
				{
					IsFile  = true;

					CString CodeTarget = UserId + _T("/code") + TreeData + _T("/") + text;
					CString FileTarget = UserId + _T("/file") + TreeData + _T("/") + text;

					if(IsCode)
						TargetList.Add( UserId + _T("/code") + TreeData + _T("/") + text);
					else
						TargetList.Add( UserId + _T("/file") + TreeData + _T("/") + text);
				}
				else
				{
					IsFile = false;
					TreeData += _T("/") + text;
				}

				// ��¼��ǰ����
				if(IsFile)
				{
					OldTree = _T("");
				}
				else
				{
					OldTree = TreeData;
				}
			}

			// �ݹ�
			GetTreeData(pTreeCtrl, hitem, IsCheck);
			hitem = pTreeCtrl->GetNextItem(hitem, TVGN_NEXT);

			// �������·��
			if(IsCode)
			{
				if(!IsFile && OldTree.IsEmpty())
				{
					TreeData = _T("");
				}
				else
				{
					IsFile = false;
				}
			}
			else
			{
				if(!IsFile && OldTree != TreeData)
				{
					CString Final = TreeData.Right(TreeData.GetLength() - TreeData.ReverseFind('/') -1);
					TreeData = TreeData.Left(TreeData.GetLength() - Final.GetLength() -1);
				}
				else
				{
					IsFile = false;
				}
			}
		}
	}
}


void CSynchronizeDlg::OnAutoSynchronize()
{
	// ��ȡ����������
	Parameter.Format(_T("id=%s&type=%d"), UserId, Type);

	// �ֲ�����
	CString RecvData;
	BOOL IsSuccess;

	// ��ȡ����������
	try
	{
		RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetFileInfo"), 80, Parameter, IsSuccess);
		if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		{
			//AfxMessageBox(_T("�޷����ӵ�������, �������硣"));
		}
		else
		{
			if (IsSuccess)
			{
				if( RecvData.Replace(_T(";"), _T(";")) )
				{
					ServerInfo = RecvData;
					OnSuccess();

					// ��ʼ������
					IsFile = false;
					TreeData = _T("");
					TargetList.RemoveAll();
					ModifyList.RemoveAll();

					// �������б�
					CStringArray Local_TargetList, Server_TargetList;

					// �õ�����������
					HTREEITEM root = m_Local.GetRootItem();
					GetTreeData(&m_Local, root, false);
					Local_TargetList.Append(TargetList);

					// ��ʼ������
					IsFile = false;
					TreeData = _T("");
					TargetList.RemoveAll();

					// �õ��ƶ�������
					root = m_Server.GetRootItem();
					GetTreeData(&m_Server, root, false);
					Server_TargetList.Append(TargetList);
					
					// �Ա��ƶ��뱾����������
					// �ƶ�û�б�������
					if(Server_TargetList.GetSize() < Local_TargetList.GetSize())
					{
						
						// �õ��ƶ�ȱʧ�ı��������б�
						for(int i=0; i<Server_TargetList.GetSize(); i++)
						{
							CString LocalData  = Local_TargetList .GetAt(i);
							CString ServerData = Server_TargetList.GetAt(i);
							
							// ��ȥ�ƶ���������
							if(LocalData == ServerData)
							{
								Local_TargetList.RemoveAt(i);
							}
						}
						
						// ��Ҫ�ϴ�����
						if(Local_TargetList.GetSize() > 0)
						{
							CTransmissionDlg dlg;
							dlg.TargetList = &Local_TargetList;
							dlg.IsDownload = false;
							dlg.IsCode     = IsCode;
							dlg.IsSynchronize = true;
							dlg.DoModal();
						}
						else
						{
							AfxMessageBox(_T("û����Ҫͬ�����ļ�!"));
						}
					}

					// ����û���ƶ�����
					else if(Server_TargetList.GetSize() > Local_TargetList.GetSize())
					{
						// �õ�����ȱʧ���ƶ������б�
						for(int i=0; i<Local_TargetList.GetSize(); i++)
						{
							CString LocalData  = Local_TargetList .GetAt(i);
							CString ServerData = Server_TargetList.GetAt(i);

							// ��ȥ������������
							if(LocalData == ServerData)
							{
								Server_TargetList.RemoveAt(i);
							}
						}

						// ��Ҫ�����ƶ�����
						if(Server_TargetList.GetSize() > 0)
						{
							CTransmissionDlg dlg;
							dlg.TargetList = &Server_TargetList;
							dlg.IsDownload = true;
							dlg.IsCode     = IsCode;
							dlg.IsSynchronize = true;
							dlg.DoModal();
						}
						else
						{
							AfxMessageBox(_T("û����Ҫͬ�����ļ�!"));
						}
					}

					// �ж��ƶ��뱾�����ݵ��޸�ʱ��
					else
					{
						CStringArray Local_Time, Server_Time, UpLoad, DownLoad;

						// �õ������ļ��޸�ʱ��
						for(int i=0; i<Local_TargetList.GetSize(); i++)
						{
							// ��������
							CString LocalPath = Local_TargetList.GetAt(i);
							LocalPath = LocalPath.Right(LocalPath.GetLength() - LocalPath.Find('/') -1);

							CFileStatus Status;
							if(CFile::GetStatus(LocalPath, Status))
							{
								CString ModifyTime = Status.m_mtime.Format("%Y-%m-%d %H:%M:%S");
								Local_Time.Add(ModifyTime);
							}
						}
						
						for(int i=0; i<Server_TargetList.GetSize(); i++)
						{
							// ��ȡ�ƶ��ļ��޸�ʱ��
							try
							{
								Parameter.Format(_T("path=%s"), Server_TargetList.GetAt(i));
								RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetFileModifyTime"), 80, Parameter, IsSuccess);
								
								if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
								{
									//AfxMessageBox(_T("�޷����ӵ�������, �������硣"));
								}
								else
								{
									if (IsSuccess)
									{
										if( RecvData.Replace(_T(":"), _T(":")) )
										{
											// ����ƶ��ļ��޸�ʱ��
											Server_Time.Add(RecvData);
										}
										else
										{
											//AfxMessageBox(_T("���ݶ�ȡʧ�ܣ����Ժ����ԡ�"));
										}
									}
									else
									{
										//AfxMessageBox(_T("�޷����ӵ�������, �������硣"));
									}
								}
							}
							catch (...)
							{
								AfxMessageBox(_T("�������쳣��λ��CSynchronizeDlg��OnAutoSynchronize������"));
							}
						}
						
						// �ȶ��ļ��޸�ʱ��
						for(int i=0; i<Local_Time.GetSize(); i++)
						{
							CString LocalData  = Local_Time .GetAt(i);
							CString ServerData = Server_Time.GetAt(i);

							// ������ݲ�ͬ
							if(LocalData != ServerData)
							{
								if(LocalData > ServerData)
								{
									UpLoad.Add(Local_TargetList.GetAt(i));
								}
								else
								{
									DownLoad.Add(Server_TargetList.GetAt(i));

									// ����ʱ�����
									ModifyList.Add(ServerData);
								}
							}
						}

						// ���������
						if(UpLoad.GetSize() > 0)
						{
							CTransmissionDlg dlg;
							dlg.TargetList = &UpLoad;
							dlg.IsDownload = false;
							dlg.IsCode     = IsCode;
							dlg.IsSynchronize = true;
							dlg.DoModal();
						}
						else if(DownLoad.GetSize() > 0)
						{
							CTransmissionDlg dlg;
							dlg.TargetList = &DownLoad;
							dlg.ModifyList = &ModifyList;
							dlg.ModifyTime = true;
							dlg.IsDownload = true;
							dlg.IsCode     = IsCode;
							dlg.IsSynchronize = true;
							dlg.DoModal();
						}
					}
				}
			}
		}
	}
	catch (...)
	{
		AfxMessageBox(_T("�������쳣��λ��CSynchronizeDlg��OnAutoSynchronize������"));
	}
}


// �����߳�
UINT CSynchronizeDlg::Operate(LPVOID pParam)
{
	// ����ָ��
	CSynchronizeDlg * pWnd = ((CSynchronizeDlg*)pParam);

	// �ֲ�����
	CString RecvData;
	BOOL IsSuccess;

	// ���ð�ť
	pWnd->GetDlgItem(IDC_UPLOAD_BUTTON)->EnableWindow(FALSE);
	pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);
	pWnd->GetDlgItem(IDC_DOWNLOAD_BUTTON)->EnableWindow(FALSE);
	pWnd->GetDlgItem(IDC_SWITCH_BUTTON)->EnableWindow(FALSE);
	pWnd->GetDlgItem(IDC_VERSION_BUTTON)->EnableWindow(FALSE);

	// ��ȡ����������
	try
	{
		RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetFileInfo"), 80, pWnd->Parameter, IsSuccess);
		if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		{
			pWnd->Error = _T("�޷����ӵ�������, �������硣");
			pWnd->PostMessage(WM_COMMAND, 101);
		}
		else
		{
			if (IsSuccess)
			{
				if( RecvData.Replace(_T(";"), _T(";")) )
				{
					pWnd->ServerInfo = RecvData;
					pWnd->PostMessage(WM_COMMAND, 100);
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
		}
	}
	catch (...)
	{
		pWnd->Error = _T("�������쳣��λ��Operate��OnGetWebInfo������");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	// ���ť
	pWnd->GetDlgItem(IDC_UPLOAD_BUTTON)->EnableWindow();
	pWnd->GetDlgItem(IDOK)->EnableWindow();
	pWnd->GetDlgItem(IDC_DOWNLOAD_BUTTON)->EnableWindow();
	pWnd->GetDlgItem(IDC_SWITCH_BUTTON)->EnableWindow();
	pWnd->GetDlgItem(IDC_VERSION_BUTTON)->EnableWindow();

	// ������Ϊ��
	pWnd->m_hOperate = NULL;
	return false;
}


// ɾ���ļ�
UINT CSynchronizeDlg::DeleteOperate(LPVOID pParam)
{
	// ����ָ��
	CSynchronizeDlg * pWnd = ((CSynchronizeDlg*)pParam);

	// ��ʼ������
	pWnd->IsFile = false;
	pWnd->TreeData = _T("");
	pWnd->TargetList.RemoveAll();

	// �õ�������
	HTREEITEM root = pWnd->m_Local.GetRootItem();
	pWnd->GetTreeData(&pWnd->m_Local, root, true);

	if(pWnd->TargetList.GetSize() > 0)
	{
		// ��ʾ��Ϣ��ʾ
		AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

		for(int i=0; i < pWnd->TargetList.GetSize(); i++)
		{
			CString Target = pWnd->TargetList.GetAt(i);
			Target = Target.Right(Target.GetLength() -1);
			Target.Replace(_T("\\"), _T("/"));
			Target = _T(".") + Target;

			// ɾ���ļ�
			DeleteFile(Target);

			// Ŀ¼
			CString File = Target.Right(Target.GetLength() - Target.ReverseFind('/') );
			CString Path = Target.Left( Target.GetLength() - File.GetLength());

			// �ж��Ƿ�Ϊ��
			if(CMainDlg::CountFile(Path) <= 0)
			{
				// ɾ������Ŀ¼
				CMainDlg::DeleteDirectory(Path);

				File = Path.Right(Path.GetLength() - Path.ReverseFind('/') );
				Path = Path.Left( Path.GetLength() - File.GetLength());

				if(CMainDlg::CountFile(Path) <= 0)
				{
					// ɾ������Ŀ¼
					CMainDlg::DeleteDirectory(Path);

					// �ļ���һ��Ŀ¼
					if(!pWnd->IsCode)
					{
						File = Path.Right(Path.GetLength() - Path.ReverseFind('/') );
						Path = Path.Left( Path.GetLength() - File.GetLength());

						if(CMainDlg::CountFile(Path) <= 0)
							CMainDlg::DeleteDirectory(Path);
					}
				}
			}
		}

		// ���
		pWnd->m_Local.DeleteAllItems();

		// �ж�����
		if(pWnd->IsCode)
		{
			// ˢ�±�������
			HTREEITEM m_TreeRoot = pWnd->m_Local.InsertItem(_T("����Դ���"));//������ڵ�
			pWnd->ShowFile(_T("Code"), m_TreeRoot);//��Ŀ¼���б���
			pWnd->m_Local.Invalidate();

			// ������ɱ�־
			theApp.IsFinished = true;

			// ��ʾ��Ϣ
			pWnd->Message = _T("��ѡ�ı���Դ����ȫ��ɾ��!");
			pWnd->PostMessage(WM_COMMAND, 102);
		}
		else
		{
			// ˢ�±�������
			HTREEITEM m_TreeRoot = pWnd->m_Local.InsertItem(_T("�����ļ���"));//������ڵ�
			pWnd->ShowFile(_T("File"), m_TreeRoot);//��Ŀ¼���б���
			pWnd->m_Local.Invalidate();

			// ������ɱ�־
			theApp.IsFinished = true;

			// ��ʾ��Ϣ
			pWnd->Message = _T("��ѡ�ı����ļ���ȫ��ɾ��!");
			pWnd->PostMessage(WM_COMMAND, 102);
		}
	}

	// ��ʼ������
	pWnd->IsFile = false;
	pWnd->TreeData = _T("");
	pWnd->TargetList.RemoveAll();

	// �õ�������
	root = pWnd->m_Server.GetRootItem();
	pWnd->GetTreeData(&pWnd->m_Server, root, true);

	if(pWnd->TargetList.GetSize() > 0)
	{
		// ���ð�ť
		pWnd->GetDlgItem(IDC_UPLOAD_BUTTON)->EnableWindow(FALSE);
		pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);
		pWnd->GetDlgItem(IDC_DOWNLOAD_BUTTON)->EnableWindow(FALSE);
		pWnd->GetDlgItem(IDC_SWITCH_BUTTON)->EnableWindow(FALSE);
		pWnd->GetDlgItem(IDC_VERSION_BUTTON)->EnableWindow(FALSE);

		// ��ʾ��Ϣ��ʾ
		AfxBeginThread(CProjectDlg::Notify, _T("����ִ�в���..."));

		for(int i=0; i < pWnd->TargetList.GetSize(); i++)
		{
			CString Target = pWnd->TargetList.GetAt(i), Parameter;
			Parameter.Format(_T("Path=%s"), Target);
			
			// �������ͨѶ
			// �ֲ�����
			CString RecvData;
			BOOL IsSuccess;

			// ��ȡ����������
			try
			{
				RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/DeleteFile"), 80, Parameter, IsSuccess);
				if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
				{
					pWnd->Error = _T("�޷����ӵ�������, �������硣");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
				else
				{
					if (IsSuccess)
					{
						if( RecvData == _T("error") )
						{
							pWnd->Error = _T("Ŀ���ļ������ڣ�ɾ��ʧ�ܡ�");
							pWnd->PostMessage(WM_COMMAND, 101);
						}
						else if(RecvData != _T("success"))
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
				}
			}
			catch (...)
			{
				pWnd->Error = _T("�������쳣��λ��Operate��OnGetWebInfo������");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
		}

		// ���
		pWnd->m_Server.DeleteAllItems();

		// ˢ���ƶ�����
		pWnd->GetServerInfo();

		// ������ɱ�־
		theApp.IsFinished = true;

		// �ж�����
		if(pWnd->IsCode)
		{
			// ��ʾ��Ϣ
			pWnd->Message = _T("��ѡ���ƶ�Դ����ȫ��ɾ��!");
			pWnd->PostMessage(WM_COMMAND, 102);
		}
		else
		{
			// ��ʾ��Ϣ
			pWnd->Message = _T("��ѡ���ƶ��ļ���ȫ��ɾ��!");
			pWnd->PostMessage(WM_COMMAND, 102);
		}

		// ���ť
		pWnd->GetDlgItem(IDC_UPLOAD_BUTTON)->EnableWindow();
		pWnd->GetDlgItem(IDOK)->EnableWindow();
		pWnd->GetDlgItem(IDC_DOWNLOAD_BUTTON)->EnableWindow();
		pWnd->GetDlgItem(IDC_SWITCH_BUTTON)->EnableWindow();
		pWnd->GetDlgItem(IDC_VERSION_BUTTON)->EnableWindow();
	}
	
	// ������Ϊ��
	pWnd->m_hDelete = NULL;
	return true;
}


void CSynchronizeDlg::OnClickLocalTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint oPoint;
	UINT   nFlag;
	GetCursorPos(&oPoint);
	m_Local.ScreenToClient(&oPoint);

	HTREEITEM oSelectItem = m_Local.HitTest(oPoint, &nFlag);
	if(oSelectItem == NULL)
	{
		return;
	}

	m_Local.SelectItem(oSelectItem);
	if(nFlag & TVHT_ONITEMSTATEICON)
	{
		BOOL bCheck = !m_Local.GetCheck(oSelectItem);

		// Ϊ��һ�»�ѡ��״̬�������ø�ѡ��Ϊ�ı���״̬
		m_Local.SetCheck(oSelectItem, bCheck);

		// һ�»�ѡ��״̬
		ConsistentParentCheck(&m_Local, oSelectItem);
		ConsistentChildCheck(&m_Local,oSelectItem);

		// ��ѡ��״̬��ԭ��MFC�Զ���Ӧ�ı����
		m_Local.SetCheck(oSelectItem, !bCheck);
	}

	*pResult = 0;
}


void CSynchronizeDlg::OnDblclkLocalTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnUpload();
	*pResult = 0;
}


void CSynchronizeDlg::OnClickServerTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint oPoint;
	UINT   nFlag;
	GetCursorPos(&oPoint);
	m_Server.ScreenToClient(&oPoint);

	HTREEITEM oSelectItem = m_Server.HitTest(oPoint, &nFlag);
	if(oSelectItem == NULL)
	{
		return;
	}

	m_Server.SelectItem(oSelectItem);
	if(nFlag & TVHT_ONITEMSTATEICON)
	{
		BOOL bCheck = !m_Server.GetCheck(oSelectItem);

		// Ϊ��һ�»�ѡ��״̬�������ø�ѡ��Ϊ�ı���״̬
		m_Server.SetCheck(oSelectItem, bCheck);

		// һ�»�ѡ��״̬
		ConsistentParentCheck(&m_Server, oSelectItem);
		ConsistentChildCheck(&m_Server, oSelectItem);

		// ��ѡ��״̬��ԭ��MFC�Զ���Ӧ�ı����
		m_Server.SetCheck(oSelectItem, !bCheck);
	}
	*pResult = 0;
}


void CSynchronizeDlg::OnDblclkServerTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnDownload();
	*pResult = 0;
}


void CSynchronizeDlg::OnOK()
{
	// ��ʼ������
	IsFile = false;
	TreeData = _T("");
	TargetList.RemoveAll();
	ModifyList.RemoveAll();

	// �������б�
	CStringArray Local_TargetList, Server_TargetList;

	// �õ�����������
	HTREEITEM root = m_Local.GetRootItem();
	GetTreeData(&m_Local, root, false);
	Local_TargetList.Append(TargetList);

	// ��ʼ������
	IsFile = false;
	TreeData = _T("");
	TargetList.RemoveAll();

	// �õ��ƶ�������
	root = m_Server.GetRootItem();
	GetTreeData(&m_Server, root, false);
	Server_TargetList.Append(TargetList);

	// �Ա��ƶ��뱾����������
	// �ƶ�û�б�������
	if(Server_TargetList.GetSize() < Local_TargetList.GetSize())
	{
		// �õ��ƶ�ȱʧ�ı��������б�
		for(int i=0; i<Server_TargetList.GetSize(); i++)
		{
			CString LocalData  = Local_TargetList .GetAt(i);
			CString ServerData = Server_TargetList.GetAt(i);

			// ��ȥ�ƶ���������
			if(LocalData == ServerData)
			{
				Local_TargetList.RemoveAt(i);
			}
		}

		// ��Ҫ�ϴ�����
		if(Local_TargetList.GetSize() > 0)
		{
			CTransmissionDlg dlg;
			dlg.TargetList = &Local_TargetList;
			dlg.IsDownload = false;
			dlg.IsCode     = IsCode;
			dlg.DoModal();

			if(dlg.IsFinished)
			{
				// ����ƶ��б�
				m_Server.DeleteAllItems();

				// �����ж�
				if(IsCode)
				{
					Type = 1;
				}
				else
				{
					Type = 2;
				}

				// ˢ���ƶ�����
				GetServerInfo();
			}
		}
		else
		{
			AfxMessageBox(_T("û����Ҫͬ�����ļ�!"));
		}
	}

	// ����û���ƶ�����
	else if(Server_TargetList.GetSize() > Local_TargetList.GetSize())
	{
		// �õ�����ȱʧ���ƶ������б�
		for(int i=0; i<Local_TargetList.GetSize(); i++)
		{
			CString LocalData  = Local_TargetList .GetAt(i);
			CString ServerData = Server_TargetList.GetAt(i);

			// ��ȥ������������
			if(LocalData == ServerData)
			{
				Server_TargetList.RemoveAt(i);
			}
		}

		// ��Ҫ�����ƶ�����
		if(Server_TargetList.GetSize() > 0)
		{
			CTransmissionDlg dlg;
			dlg.TargetList = &Server_TargetList;
			dlg.IsDownload = true;
			dlg.IsCode     = IsCode;
			dlg.DoModal();

			if(dlg.IsFinished)
			{
				// ��ձ����б�
				m_Local.DeleteAllItems();

				// �ж�����
				if(IsCode)
				{
					// ˢ�±�������
					HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("����Դ��� (�� delete ɾ��Դ��)"));//������ڵ�
					ShowFile(_T("Code"), m_TreeRoot);//��Ŀ¼���б���
				}
				else
				{
					// ˢ�±�������
					HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("�����ļ��� (�� delete ɾ���ļ�)"));//������ڵ�
					ShowFile(_T("File"), m_TreeRoot);//��Ŀ¼���б���
				}
			}
		}
		else
		{
			AfxMessageBox(_T("û����Ҫͬ�����ļ�!"));
		}
	}

	// �ж��ƶ��뱾�����ݵ��޸�ʱ��
	else
	{
		CStringArray Local_Time, Server_Time, UpLoad, DownLoad;

		// �õ������ļ��޸�ʱ��
		for(int i=0; i<Local_TargetList.GetSize(); i++)
		{
			// ��������
			CString LocalPath = Local_TargetList.GetAt(i);
			LocalPath = LocalPath.Right(LocalPath.GetLength() - LocalPath.Find('/') -1);

			CFileStatus Status;
			if(CFile::GetStatus(LocalPath, Status))
			{
				CString ModifyTime = Status.m_mtime.Format("%Y-%m-%d %H:%M:%S");
				Local_Time.Add(ModifyTime);
			}
		}

		for(int i=0; i<Server_TargetList.GetSize(); i++)
		{
			// ��ȡ�ƶ��ļ��޸�ʱ��
			try
			{
				// �ֲ�����
				CString RecvData;
				BOOL IsSuccess;
				Parameter.Format(_T("path=%s"), Server_TargetList.GetAt(i));

				RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetFileModifyTime"), 80, Parameter, IsSuccess);
				if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
				{
					AfxMessageBox(_T("�޷����ӵ�������, �������硣"));
				}
				else
				{
					if (IsSuccess)
					{
						if( RecvData.Replace(_T(":"), _T(":")) )
						{
							// ����ƶ��ļ��޸�ʱ��
							Server_Time.Add(RecvData);
						}
						else
						{
							AfxMessageBox(_T("���ݶ�ȡʧ�ܣ����Ժ����ԡ�"));
						}
					}
					else
					{
						AfxMessageBox(_T("�޷����ӵ�������, �������硣"));
					}
				}
			}
			catch (...)
			{
				AfxMessageBox(_T("�������쳣��λ��OK��OnGetWebInfo������"));
			}
		}

		// �ȶ��ļ��޸�ʱ��
		for(int i=0; i<Local_Time.GetSize(); i++)
		{
			CString LocalData  = Local_Time .GetAt(i);
			CString ServerData = Server_Time.GetAt(i);

			// ������ݲ�ͬ
			if(LocalData != ServerData)
			{
				if(LocalData > ServerData)
				{
					// �����ϴ�����
					UpLoad.Add(Local_TargetList.GetAt(i));
				}
				else
				{
					// �������ض���
					DownLoad.Add(Server_TargetList.GetAt(i));

					// ����ʱ�����
					ModifyList.Add(ServerData);
				}
			}
		}

		// ���������
		if(UpLoad.GetSize() > 0)
		{
			CTransmissionDlg dlg;
			dlg.TargetList = &UpLoad;
			dlg.IsDownload = false;
			dlg.IsCode     = IsCode;
			dlg.DoModal();

			if(dlg.IsFinished)
			{
				// ����ƶ��б�
				m_Server.DeleteAllItems();

				// �����ж�
				if(IsCode)
				{
					Type = 1;
				}
				else
				{
					Type = 2;
				}

				// ˢ���ƶ�����
				GetServerInfo();
			}

			MessageBox(_T("�ƶ����뱾��������ȫͬ��!"), _T("ͬ�����"), MB_ICONINFORMATION);
		}
		else if(DownLoad.GetSize() > 0)
		{
			CTransmissionDlg dlg;
			dlg.TargetList = &DownLoad;
			dlg.ModifyList = &ModifyList;
			dlg.ModifyTime = true;
			dlg.IsDownload = true;
			dlg.IsCode     = IsCode;
			dlg.DoModal();

			if(dlg.IsFinished)
			{
				// ��ձ����б�
				m_Local.DeleteAllItems();

				// �ж�����
				if(IsCode)
				{
					// ˢ�±�������
					HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("����Դ��� (�� delete ɾ��Դ��)"));//������ڵ�
					ShowFile(_T("Code"), m_TreeRoot);//��Ŀ¼���б���
				}
				else
				{
					// ˢ�±�������
					HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("�����ļ��� (�� delete ɾ���ļ�)"));//������ڵ�
					ShowFile(_T("File"), m_TreeRoot);//��Ŀ¼���б���
				}

				MessageBox(_T("�ƶ����뱾��������ȫͬ��!"), _T("ͬ�����"), MB_ICONINFORMATION);
			}
		}
		else
		{
			MessageBox(_T("�ƶ����뱾��������ȫͬ��!"), _T("ͬ�����"), MB_ICONINFORMATION);
		}
	}
}


void CSynchronizeDlg::OnUpload()
{
	// ��ʼ������
	IsFile = false;
	TreeData = _T("");
	TargetList.RemoveAll();

	// �õ�������
	HTREEITEM root = m_Local.GetRootItem();
	GetTreeData(&m_Local, root, true);

	if(TargetList.GetSize() > 0)
	{
		CTransmissionDlg dlg;
		dlg.TargetList = &TargetList;
		dlg.IsDownload = false;
		dlg.IsCode     = IsCode;
		dlg.DoModal();

		if(dlg.IsFinished)
		{
			// ȥ������
			HTREEITEM hitem = NULL;
			m_Local.GetChildItem(hitem);
			m_Local.SetCheck(hitem, 0);

			// ����ƶ��б�
			m_Server.DeleteAllItems();

			// �����ж�
			if(IsCode)
			{
				Type = 1;
			}
			else
			{
				Type = 2;
			}

			// ˢ���ƶ�����
			GetServerInfo();
		}
	}
	else
	{
		AfxMessageBox(_T("�빴ѡ��Ҫ�ϴ����ļ�!"));
	}
}


void CSynchronizeDlg::OnDownload()
{
	// ��ʼ������
	IsFile = false;
	TreeData = _T("");
	TargetList.RemoveAll();

	// �õ�������
	HTREEITEM root = m_Server.GetRootItem();
	GetTreeData(&m_Server, root, true);

	if(TargetList.GetSize() > 0)
	{
		CTransmissionDlg dlg;
		dlg.TargetList = &TargetList;
		dlg.IsDownload = true;
		dlg.IsCode     = IsCode;
		dlg.DoModal();

		if(dlg.IsFinished)
		{
			// ȥ������
			HTREEITEM hitem = NULL;
			m_Server.GetChildItem(hitem);
			m_Server.SetCheck(hitem, 0);

			// ��ձ����б�
			m_Local.DeleteAllItems();

			// �ж�����
			if(IsCode)
			{
				// ˢ�±�������
				HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("����Դ��� (�� delete ɾ��Դ��)"));//������ڵ�
				ShowFile(_T("Code"), m_TreeRoot);//��Ŀ¼���б���
			}
			else
			{
				// ˢ�±�������
				HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("�����ļ��� (�� delete ɾ���ļ�)"));//������ڵ�
				ShowFile(_T("File"), m_TreeRoot);//��Ŀ¼���б���
			}
		}
	}
	else
	{
		AfxMessageBox(_T("�빴ѡ��Ҫ���ص��ļ�!"));
	}
}


void CSynchronizeDlg::OnSwitch()
{
	// ���
	m_Local.DeleteAllItems();
	m_Server.DeleteAllItems();

	if(!IsCode)
	{
		// ����
		HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("����Դ��� (�� delete ɾ��Դ��)"));//������ڵ�
		ShowFile(_T("Code"), m_TreeRoot);//��Ŀ¼���б���

		// �ƶ�
		Type = 1;
		GetServerInfo();

		IsCode = TRUE;
	}
	else
	{
		HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("�����ļ��� (�� delete ɾ���ļ�)"));//������ڵ�
		ShowFile(_T("File"), m_TreeRoot);//��Ŀ¼���б���

		// �ƶ�
		Type = 2;
		GetServerInfo();

		IsCode = FALSE;
	}
}


void CSynchronizeDlg::OnVersion()
{
	CWebProjectDlg dlg;
	dlg.UserId   = UserId;
	dlg.UserName = UserName;
	dlg.DoModal();
}


void CSynchronizeDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
