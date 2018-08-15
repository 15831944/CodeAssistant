// SynchronizeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "SynchronizeDlg.h"
#include "afxdialogex.h"
#include "MainDlg.h"

#include "TransmissionDlg.h"
#include "WebProjectDlg.h"
#include "ProjectDlg.h"

// CSynchronizeDlg 对话框

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


// CSynchronizeDlg 消息处理程序


BOOL CSynchronizeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 刷新数据
	HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("本地源码库 (按 delete 删除源码)"));//插入根节点
	ShowFile(_T("Code"), m_TreeRoot);//根目录进行遍历


	// 获取服务器数据
	if(!IsAutoSynchronize)
		GetServerInfo();

	// 提示框
	m_toolTips.Create(this, TTS_ALWAYSTIP|WS_POPUP);
	m_toolTips.Activate(TRUE);

	m_toolTips.AddTool(GetDlgItem(IDC_LOCAL_TREE),      _T("显示本地库数据。\n双击目标后上传至云端。\n按下delete或者退格键删除选中目标。"));
	m_toolTips.AddTool(GetDlgItem(IDC_SERVER_TREE),     _T("显示云端库数据。\n双击目标后下载至本地。\n按下delete或者退格键删除选中目标。"));
	m_toolTips.AddTool(GetDlgItem(IDC_UPLOAD_BUTTON),   _T("上传选中目标至云端。"));
	m_toolTips.AddTool(GetDlgItem(IDC_DOWNLOAD_BUTTON), _T("下载选中目标至本地。"));
	m_toolTips.AddTool(GetDlgItem(IDOK),                _T("对本地与云端库自动分析并同步数据。\n可按下回车快速同步。"));
	m_toolTips.AddTool(GetDlgItem(IDC_SWITCH_BUTTON),   _T("切换本地/云端的方法/文件库。"));
	m_toolTips.AddTool(GetDlgItem(IDC_VERSION_BUTTON),  _T("进入云端项目管理系统。"));

	//文字颜色
	m_toolTips.SetTipTextColor(RGB(0,0,255));

	//鼠标指向多久后显示提示，毫秒
	m_toolTips.SetDelayTime(TTDT_INITIAL, 10); 

	//鼠标保持指向，提示显示多久，毫秒
	m_toolTips.SetDelayTime(TTDT_AUTOPOP, 9000000);

	//设定显示宽度，超长内容自动换行
	m_toolTips.SetMaxTipWidth(300);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CSynchronizeDlg::PreTranslateMessage(MSG* pMsg)
{
	UINT  nKeyCode = pMsg->wParam; // virtual key code of the key pressed
	if (pMsg->message == WM_KEYDOWN)
	{
		// 按下Delete 或 BACK
		if ( nKeyCode == VK_DELETE || nKeyCode == VK_BACK )
		{
			OnDelete();
			return true;
		}
	}

	// 功能提示
	if(GetPrivateProfileInt(_T("Setting"), _T("Tip"), 0, _T("./Setting.ini")) == 1)
		m_toolTips.RelayEvent(pMsg); // 接受消息响应

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSynchronizeDlg::ShowFile(CString str_Dir, HTREEITEM tree_Root)
{
    CFileFind FileFind;

    //临时变量，用以记录返回的树节点
    HTREEITEM tree_Temp;

    //判断输入目录最后是否存在'\'，不存在则补充
    if (str_Dir.Right(1) != "\\")
        str_Dir += "\\";
    str_Dir += "*.*";
    BOOL res = FileFind.FindFile(str_Dir);
    while (res)
    {
        tree_Temp = tree_Root;
        res = FileFind.FindNextFile();
        if (FileFind.IsDirectory() && !FileFind.IsDots())//目录是文件夹
        {
            CString strPath = FileFind.GetFilePath(); //得到路径，做为递归调用的开始
            CString strTitle = FileFind.GetFileName();//得到目录名，做为树控的结点
            tree_Temp = m_Local.InsertItem(strTitle, 0, 0, tree_Root);
            ShowFile(strPath, tree_Temp);
        }
        else if (!FileFind.IsDirectory() && !FileFind.IsDots())//如果是文件
        {
            CString strPath = FileFind.GetFilePath(); //得到路径，做为递归调用的开始
            CString strTitle = FileFind.GetFileName();//得到文件名，做为树控的结点
            m_Local.InsertItem(strTitle, 0, 0, tree_Temp);
        }
    }

    FileFind.Close();
}


void CSynchronizeDlg::ConsistentParentCheck(CTreeCtrl * pTreeCtrl, HTREEITEM hTreeItem)
{
	// 获取父节点，若为空则返回，否则处理状态
	HTREEITEM hParentItem = pTreeCtrl->GetParentItem(hTreeItem);
	if(hParentItem != NULL)
	{
		// 依次判断选中项父节点各子节点状态
		HTREEITEM hChildItem = pTreeCtrl->GetChildItem(hParentItem);
		while(hChildItem != NULL)
		{
			// 若有一个子节点未选中，则父节点也未选中，同时递归处理父节点的父节点
			if(pTreeCtrl->GetCheck(hChildItem) == FALSE)
			{
				pTreeCtrl->SetCheck(hParentItem, FALSE);
				return ConsistentParentCheck(pTreeCtrl, hParentItem);
			}

			// 获取父节点的下一个子节点
			hChildItem = pTreeCtrl->GetNextItem(hChildItem, TVGN_NEXT);
		}

		// 若子节点全部选中，则父节点也选中，同时递归处理父节点的父节点
		pTreeCtrl->SetCheck(hParentItem, TRUE);
		return ConsistentParentCheck(pTreeCtrl, hParentItem);
	}
}


void CSynchronizeDlg::ConsistentChildCheck(CTreeCtrl  * pTreeCtrl, HTREEITEM hTreeItem)
{
	// 获取当前复选框选中状态
	BOOL bCheck = pTreeCtrl->GetCheck(hTreeItem);

	// 若当前节点存在子节点，则一致化子节点状态
	if(pTreeCtrl->ItemHasChildren(hTreeItem))
	{
		// 依次一致化子节点的子节点状态
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
	// 启动工作者线程
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

			HTREEITEM rootItem = m_Server.InsertItem(_T("云端源码库 (按 delete 删除源码)"), 0, 0, NULL), parentItem, subItem;

			CString Parent, Sub;
			for(int i=0; i<TextArray.GetSize() -1; i++)
			{
				CString FileInfo = TextArray.GetAt(i);

				// 数据处理
				FileInfo.Replace(_T("cloud/") + UserId + _T("/Code/"), _T(""));
				FileInfo.Replace(_T("cloud/") + UserId + _T("/Code"), _T(""));

				if (FileInfo.Replace(_T(".."), _T("..")) || FileInfo.Replace(_T("/."), _T("/.")) || FileInfo == _T(".") || FileInfo == _T(""))
				{
					//目录
					continue;
				}
				else if(FileInfo.Replace(_T("/"), _T("/")) && !FileInfo.Replace(_T("."), _T(".")))
				{
					// 子文件夹
					FileInfo.Replace(Parent + _T("/"), _T(""));
					subItem = m_Server.InsertItem(FileInfo, 2, 2, parentItem);
					Sub = Parent + _T("/") + FileInfo;
				}
				else if(FileInfo.Replace(_T("."), _T(".")))
				{
					// 文件
					FileInfo.Replace(Sub + _T("/"), _T(""));
					m_Server.InsertItem(FileInfo, 3, 3, subItem);
				}
				else 
				{
					// 文件夹
					Parent = FileInfo;
					parentItem = m_Server.InsertItem(FileInfo, 1, 1, rootItem);
				}
			}
		} break;

	case 2:
		{
			CStringArray TextArray;
			Split(ServerInfo, _T(";"), TextArray);

			HTREEITEM rootItem = m_Server.InsertItem(_T("云端文件库 (按 delete 删除文件)"), 0, 0, NULL), parentItem, subItem, subFile;

			CString Parent, Sub, Item, Function;
			for(int i=0; i<TextArray.GetSize() -1; i++)
			{
				CString FileInfo = TextArray.GetAt(i);

				// 数据处理
				FileInfo.Replace(_T("cloud/") + UserId + _T("/File/"), _T(""));
				FileInfo.Replace(_T("cloud/") + UserId + _T("/File"), _T(""));

				if (FileInfo.Replace(_T(".."), _T("..")) || FileInfo.Replace(_T("/."), _T("/.")) || FileInfo == _T(".") || FileInfo == _T(""))
				{
					//目录
					continue;
				}

				else if(FileInfo.Replace(_T("/"), _T("/")) && !FileInfo.Replace(_T("."), _T(".")))
				{
					// 方法
					if(!Item.IsEmpty() && FileInfo.Replace(Item + _T("/"), Item + _T("/")))
					{
						FileInfo.Replace(Sub + _T("/"), _T(""));
						subFile = m_Server.InsertItem(FileInfo, 3, 3, subItem);
						Function= FileInfo;
					}
					else
					{
						// 子文件夹
						FileInfo.Replace(Parent + _T("/"), _T(""));
						subItem = m_Server.InsertItem(FileInfo, 2, 2, parentItem);
						Sub = Parent + _T("/") + FileInfo;
						Item= FileInfo;
					}
				}

				else if(FileInfo.Replace(_T("."), _T(".")))
				{
					// 文件
					FileInfo.Replace(Sub + _T("/") + Function + _T("/"), _T(""));
					m_Server.InsertItem(FileInfo, 4, 4, subFile);
				}

				else 
				{
					// 文件夹
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
	MessageBox(Message, _T("提示消息"), MB_ICONINFORMATION);
}


void CSynchronizeDlg::OnDelete()
{
	// 启动工作者线程
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
				// 如果选中
				if(pTreeCtrl->GetCheck(hitem))
				{
					CString text = pTreeCtrl->GetItemText(hitem);

					// 是文件
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

					// 记录当前数据
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

			// 同步
			else
			{
				CString text = pTreeCtrl->GetItemText(hitem);

				// 是文件
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

				// 记录当前数据
				if(IsFile)
				{
					OldTree = _T("");
				}
				else
				{
					OldTree = TreeData;
				}
			}

			// 递归
			GetTreeData(pTreeCtrl, hitem, IsCheck);
			hitem = pTreeCtrl->GetNextItem(hitem, TVGN_NEXT);

			// 清除多余路径
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
	// 获取服务器数据
	Parameter.Format(_T("id=%s&type=%d"), UserId, Type);

	// 局部变量
	CString RecvData;
	BOOL IsSuccess;

	// 获取服务器数据
	try
	{
		RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetFileInfo"), 80, Parameter, IsSuccess);
		if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		{
			//AfxMessageBox(_T("无法连接到服务器, 请检查网络。"));
		}
		else
		{
			if (IsSuccess)
			{
				if( RecvData.Replace(_T(";"), _T(";")) )
				{
					ServerInfo = RecvData;
					OnSuccess();

					// 初始化数据
					IsFile = false;
					TreeData = _T("");
					TargetList.RemoveAll();
					ModifyList.RemoveAll();

					// 树数据列表
					CStringArray Local_TargetList, Server_TargetList;

					// 得到本地树数据
					HTREEITEM root = m_Local.GetRootItem();
					GetTreeData(&m_Local, root, false);
					Local_TargetList.Append(TargetList);

					// 初始化数据
					IsFile = false;
					TreeData = _T("");
					TargetList.RemoveAll();

					// 得到云端树数据
					root = m_Server.GetRootItem();
					GetTreeData(&m_Server, root, false);
					Server_TargetList.Append(TargetList);
					
					// 对比云端与本地数据数量
					// 云端没有本地数据
					if(Server_TargetList.GetSize() < Local_TargetList.GetSize())
					{
						
						// 得到云端缺失的本地数据列表
						for(int i=0; i<Server_TargetList.GetSize(); i++)
						{
							CString LocalData  = Local_TargetList .GetAt(i);
							CString ServerData = Server_TargetList.GetAt(i);
							
							// 除去云端已有数据
							if(LocalData == ServerData)
							{
								Local_TargetList.RemoveAt(i);
							}
						}
						
						// 需要上传数据
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
							AfxMessageBox(_T("没有需要同步的文件!"));
						}
					}

					// 本地没有云端数据
					else if(Server_TargetList.GetSize() > Local_TargetList.GetSize())
					{
						// 得到本地缺失的云端数据列表
						for(int i=0; i<Local_TargetList.GetSize(); i++)
						{
							CString LocalData  = Local_TargetList .GetAt(i);
							CString ServerData = Server_TargetList.GetAt(i);

							// 除去本地已有数据
							if(LocalData == ServerData)
							{
								Server_TargetList.RemoveAt(i);
							}
						}

						// 需要下载云端数据
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
							AfxMessageBox(_T("没有需要同步的文件!"));
						}
					}

					// 判断云端与本地数据的修改时间
					else
					{
						CStringArray Local_Time, Server_Time, UpLoad, DownLoad;

						// 得到本地文件修改时间
						for(int i=0; i<Local_TargetList.GetSize(); i++)
						{
							// 处理数据
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
							// 获取云端文件修改时间
							try
							{
								Parameter.Format(_T("path=%s"), Server_TargetList.GetAt(i));
								RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetFileModifyTime"), 80, Parameter, IsSuccess);
								
								if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
								{
									//AfxMessageBox(_T("无法连接到服务器, 请检查网络。"));
								}
								else
								{
									if (IsSuccess)
									{
										if( RecvData.Replace(_T(":"), _T(":")) )
										{
											// 添加云端文件修改时间
											Server_Time.Add(RecvData);
										}
										else
										{
											//AfxMessageBox(_T("数据读取失败，请稍后再试。"));
										}
									}
									else
									{
										//AfxMessageBox(_T("无法连接到服务器, 请检查网络。"));
									}
								}
							}
							catch (...)
							{
								AfxMessageBox(_T("发生了异常，位于CSynchronizeDlg的OnAutoSynchronize方法。"));
							}
						}
						
						// 比对文件修改时间
						for(int i=0; i<Local_Time.GetSize(); i++)
						{
							CString LocalData  = Local_Time .GetAt(i);
							CString ServerData = Server_Time.GetAt(i);

							// 如果数据不同
							if(LocalData != ServerData)
							{
								if(LocalData > ServerData)
								{
									UpLoad.Add(Local_TargetList.GetAt(i));
								}
								else
								{
									DownLoad.Add(Server_TargetList.GetAt(i));

									// 加入时间队列
									ModifyList.Add(ServerData);
								}
							}
						}

						// 处理传输队列
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
		AfxMessageBox(_T("发生了异常，位于CSynchronizeDlg的OnAutoSynchronize方法。"));
	}
}


// 工作线程
UINT CSynchronizeDlg::Operate(LPVOID pParam)
{
	// 窗口指针
	CSynchronizeDlg * pWnd = ((CSynchronizeDlg*)pParam);

	// 局部变量
	CString RecvData;
	BOOL IsSuccess;

	// 禁用按钮
	pWnd->GetDlgItem(IDC_UPLOAD_BUTTON)->EnableWindow(FALSE);
	pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);
	pWnd->GetDlgItem(IDC_DOWNLOAD_BUTTON)->EnableWindow(FALSE);
	pWnd->GetDlgItem(IDC_SWITCH_BUTTON)->EnableWindow(FALSE);
	pWnd->GetDlgItem(IDC_VERSION_BUTTON)->EnableWindow(FALSE);

	// 获取服务器数据
	try
	{
		RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetFileInfo"), 80, pWnd->Parameter, IsSuccess);
		if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		{
			pWnd->Error = _T("无法连接到服务器, 请检查网络。");
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
					pWnd->Error = _T("数据读取失败，请稍后再试。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			}
			else
			{
				pWnd->Error = _T("无法连接到服务器, 请检查网络。");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
		}
	}
	catch (...)
	{
		pWnd->Error = _T("发生了异常，位于Operate的OnGetWebInfo方法。");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	// 激活按钮
	pWnd->GetDlgItem(IDC_UPLOAD_BUTTON)->EnableWindow();
	pWnd->GetDlgItem(IDOK)->EnableWindow();
	pWnd->GetDlgItem(IDC_DOWNLOAD_BUTTON)->EnableWindow();
	pWnd->GetDlgItem(IDC_SWITCH_BUTTON)->EnableWindow();
	pWnd->GetDlgItem(IDC_VERSION_BUTTON)->EnableWindow();

	// 对象置为空
	pWnd->m_hOperate = NULL;
	return false;
}


// 删除文件
UINT CSynchronizeDlg::DeleteOperate(LPVOID pParam)
{
	// 窗口指针
	CSynchronizeDlg * pWnd = ((CSynchronizeDlg*)pParam);

	// 初始化数据
	pWnd->IsFile = false;
	pWnd->TreeData = _T("");
	pWnd->TargetList.RemoveAll();

	// 得到树数据
	HTREEITEM root = pWnd->m_Local.GetRootItem();
	pWnd->GetTreeData(&pWnd->m_Local, root, true);

	if(pWnd->TargetList.GetSize() > 0)
	{
		// 显示消息提示
		AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

		for(int i=0; i < pWnd->TargetList.GetSize(); i++)
		{
			CString Target = pWnd->TargetList.GetAt(i);
			Target = Target.Right(Target.GetLength() -1);
			Target.Replace(_T("\\"), _T("/"));
			Target = _T(".") + Target;

			// 删除文件
			DeleteFile(Target);

			// 目录
			CString File = Target.Right(Target.GetLength() - Target.ReverseFind('/') );
			CString Path = Target.Left( Target.GetLength() - File.GetLength());

			// 判断是否为空
			if(CMainDlg::CountFile(Path) <= 0)
			{
				// 删除本地目录
				CMainDlg::DeleteDirectory(Path);

				File = Path.Right(Path.GetLength() - Path.ReverseFind('/') );
				Path = Path.Left( Path.GetLength() - File.GetLength());

				if(CMainDlg::CountFile(Path) <= 0)
				{
					// 删除本地目录
					CMainDlg::DeleteDirectory(Path);

					// 文件多一级目录
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

		// 清空
		pWnd->m_Local.DeleteAllItems();

		// 判断类型
		if(pWnd->IsCode)
		{
			// 刷新本地数据
			HTREEITEM m_TreeRoot = pWnd->m_Local.InsertItem(_T("本地源码库"));//插入根节点
			pWnd->ShowFile(_T("Code"), m_TreeRoot);//根目录进行遍历
			pWnd->m_Local.Invalidate();

			// 操作完成标志
			theApp.IsFinished = true;

			// 提示消息
			pWnd->Message = _T("勾选的本地源码已全部删除!");
			pWnd->PostMessage(WM_COMMAND, 102);
		}
		else
		{
			// 刷新本地数据
			HTREEITEM m_TreeRoot = pWnd->m_Local.InsertItem(_T("本地文件库"));//插入根节点
			pWnd->ShowFile(_T("File"), m_TreeRoot);//根目录进行遍历
			pWnd->m_Local.Invalidate();

			// 操作完成标志
			theApp.IsFinished = true;

			// 提示消息
			pWnd->Message = _T("勾选的本地文件已全部删除!");
			pWnd->PostMessage(WM_COMMAND, 102);
		}
	}

	// 初始化数据
	pWnd->IsFile = false;
	pWnd->TreeData = _T("");
	pWnd->TargetList.RemoveAll();

	// 得到树数据
	root = pWnd->m_Server.GetRootItem();
	pWnd->GetTreeData(&pWnd->m_Server, root, true);

	if(pWnd->TargetList.GetSize() > 0)
	{
		// 禁用按钮
		pWnd->GetDlgItem(IDC_UPLOAD_BUTTON)->EnableWindow(FALSE);
		pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);
		pWnd->GetDlgItem(IDC_DOWNLOAD_BUTTON)->EnableWindow(FALSE);
		pWnd->GetDlgItem(IDC_SWITCH_BUTTON)->EnableWindow(FALSE);
		pWnd->GetDlgItem(IDC_VERSION_BUTTON)->EnableWindow(FALSE);

		// 显示消息提示
		AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

		for(int i=0; i < pWnd->TargetList.GetSize(); i++)
		{
			CString Target = pWnd->TargetList.GetAt(i), Parameter;
			Parameter.Format(_T("Path=%s"), Target);
			
			// 与服务器通讯
			// 局部变量
			CString RecvData;
			BOOL IsSuccess;

			// 获取服务器数据
			try
			{
				RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/DeleteFile"), 80, Parameter, IsSuccess);
				if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
				{
					pWnd->Error = _T("无法连接到服务器, 请检查网络。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
				else
				{
					if (IsSuccess)
					{
						if( RecvData == _T("error") )
						{
							pWnd->Error = _T("目标文件不存在，删除失败。");
							pWnd->PostMessage(WM_COMMAND, 101);
						}
						else if(RecvData != _T("success"))
						{
							pWnd->Error = _T("数据读取失败，请稍后再试。");
							pWnd->PostMessage(WM_COMMAND, 101);
						}
					}
					else
					{
						pWnd->Error = _T("无法连接到服务器, 请检查网络。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
			}
			catch (...)
			{
				pWnd->Error = _T("发生了异常，位于Operate的OnGetWebInfo方法。");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
		}

		// 清空
		pWnd->m_Server.DeleteAllItems();

		// 刷新云端数据
		pWnd->GetServerInfo();

		// 操作完成标志
		theApp.IsFinished = true;

		// 判断类型
		if(pWnd->IsCode)
		{
			// 提示消息
			pWnd->Message = _T("勾选的云端源码已全部删除!");
			pWnd->PostMessage(WM_COMMAND, 102);
		}
		else
		{
			// 提示消息
			pWnd->Message = _T("勾选的云端文件已全部删除!");
			pWnd->PostMessage(WM_COMMAND, 102);
		}

		// 激活按钮
		pWnd->GetDlgItem(IDC_UPLOAD_BUTTON)->EnableWindow();
		pWnd->GetDlgItem(IDOK)->EnableWindow();
		pWnd->GetDlgItem(IDC_DOWNLOAD_BUTTON)->EnableWindow();
		pWnd->GetDlgItem(IDC_SWITCH_BUTTON)->EnableWindow();
		pWnd->GetDlgItem(IDC_VERSION_BUTTON)->EnableWindow();
	}
	
	// 对象置为空
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

		// 为了一致化选中状态，需设置复选框为改变后的状态
		m_Local.SetCheck(oSelectItem, bCheck);

		// 一致化选中状态
		ConsistentParentCheck(&m_Local, oSelectItem);
		ConsistentChildCheck(&m_Local,oSelectItem);

		// 复选框状态复原，MFC自动响应改变绘制
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

		// 为了一致化选中状态，需设置复选框为改变后的状态
		m_Server.SetCheck(oSelectItem, bCheck);

		// 一致化选中状态
		ConsistentParentCheck(&m_Server, oSelectItem);
		ConsistentChildCheck(&m_Server, oSelectItem);

		// 复选框状态复原，MFC自动响应改变绘制
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
	// 初始化数据
	IsFile = false;
	TreeData = _T("");
	TargetList.RemoveAll();
	ModifyList.RemoveAll();

	// 树数据列表
	CStringArray Local_TargetList, Server_TargetList;

	// 得到本地树数据
	HTREEITEM root = m_Local.GetRootItem();
	GetTreeData(&m_Local, root, false);
	Local_TargetList.Append(TargetList);

	// 初始化数据
	IsFile = false;
	TreeData = _T("");
	TargetList.RemoveAll();

	// 得到云端树数据
	root = m_Server.GetRootItem();
	GetTreeData(&m_Server, root, false);
	Server_TargetList.Append(TargetList);

	// 对比云端与本地数据数量
	// 云端没有本地数据
	if(Server_TargetList.GetSize() < Local_TargetList.GetSize())
	{
		// 得到云端缺失的本地数据列表
		for(int i=0; i<Server_TargetList.GetSize(); i++)
		{
			CString LocalData  = Local_TargetList .GetAt(i);
			CString ServerData = Server_TargetList.GetAt(i);

			// 除去云端已有数据
			if(LocalData == ServerData)
			{
				Local_TargetList.RemoveAt(i);
			}
		}

		// 需要上传数据
		if(Local_TargetList.GetSize() > 0)
		{
			CTransmissionDlg dlg;
			dlg.TargetList = &Local_TargetList;
			dlg.IsDownload = false;
			dlg.IsCode     = IsCode;
			dlg.DoModal();

			if(dlg.IsFinished)
			{
				// 清空云端列表
				m_Server.DeleteAllItems();

				// 类型判断
				if(IsCode)
				{
					Type = 1;
				}
				else
				{
					Type = 2;
				}

				// 刷新云端数据
				GetServerInfo();
			}
		}
		else
		{
			AfxMessageBox(_T("没有需要同步的文件!"));
		}
	}

	// 本地没有云端数据
	else if(Server_TargetList.GetSize() > Local_TargetList.GetSize())
	{
		// 得到本地缺失的云端数据列表
		for(int i=0; i<Local_TargetList.GetSize(); i++)
		{
			CString LocalData  = Local_TargetList .GetAt(i);
			CString ServerData = Server_TargetList.GetAt(i);

			// 除去本地已有数据
			if(LocalData == ServerData)
			{
				Server_TargetList.RemoveAt(i);
			}
		}

		// 需要下载云端数据
		if(Server_TargetList.GetSize() > 0)
		{
			CTransmissionDlg dlg;
			dlg.TargetList = &Server_TargetList;
			dlg.IsDownload = true;
			dlg.IsCode     = IsCode;
			dlg.DoModal();

			if(dlg.IsFinished)
			{
				// 清空本地列表
				m_Local.DeleteAllItems();

				// 判断类型
				if(IsCode)
				{
					// 刷新本地数据
					HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("本地源码库 (按 delete 删除源码)"));//插入根节点
					ShowFile(_T("Code"), m_TreeRoot);//根目录进行遍历
				}
				else
				{
					// 刷新本地数据
					HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("本地文件库 (按 delete 删除文件)"));//插入根节点
					ShowFile(_T("File"), m_TreeRoot);//根目录进行遍历
				}
			}
		}
		else
		{
			AfxMessageBox(_T("没有需要同步的文件!"));
		}
	}

	// 判断云端与本地数据的修改时间
	else
	{
		CStringArray Local_Time, Server_Time, UpLoad, DownLoad;

		// 得到本地文件修改时间
		for(int i=0; i<Local_TargetList.GetSize(); i++)
		{
			// 处理数据
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
			// 获取云端文件修改时间
			try
			{
				// 局部变量
				CString RecvData;
				BOOL IsSuccess;
				Parameter.Format(_T("path=%s"), Server_TargetList.GetAt(i));

				RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetFileModifyTime"), 80, Parameter, IsSuccess);
				if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
				{
					AfxMessageBox(_T("无法连接到服务器, 请检查网络。"));
				}
				else
				{
					if (IsSuccess)
					{
						if( RecvData.Replace(_T(":"), _T(":")) )
						{
							// 添加云端文件修改时间
							Server_Time.Add(RecvData);
						}
						else
						{
							AfxMessageBox(_T("数据读取失败，请稍后再试。"));
						}
					}
					else
					{
						AfxMessageBox(_T("无法连接到服务器, 请检查网络。"));
					}
				}
			}
			catch (...)
			{
				AfxMessageBox(_T("发生了异常，位于OK的OnGetWebInfo方法。"));
			}
		}

		// 比对文件修改时间
		for(int i=0; i<Local_Time.GetSize(); i++)
		{
			CString LocalData  = Local_Time .GetAt(i);
			CString ServerData = Server_Time.GetAt(i);

			// 如果数据不同
			if(LocalData != ServerData)
			{
				if(LocalData > ServerData)
				{
					// 加入上传队列
					UpLoad.Add(Local_TargetList.GetAt(i));
				}
				else
				{
					// 加入下载队列
					DownLoad.Add(Server_TargetList.GetAt(i));

					// 加入时间队列
					ModifyList.Add(ServerData);
				}
			}
		}

		// 处理传输队列
		if(UpLoad.GetSize() > 0)
		{
			CTransmissionDlg dlg;
			dlg.TargetList = &UpLoad;
			dlg.IsDownload = false;
			dlg.IsCode     = IsCode;
			dlg.DoModal();

			if(dlg.IsFinished)
			{
				// 清空云端列表
				m_Server.DeleteAllItems();

				// 类型判断
				if(IsCode)
				{
					Type = 1;
				}
				else
				{
					Type = 2;
				}

				// 刷新云端数据
				GetServerInfo();
			}

			MessageBox(_T("云端已与本地数据完全同步!"), _T("同步完成"), MB_ICONINFORMATION);
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
				// 清空本地列表
				m_Local.DeleteAllItems();

				// 判断类型
				if(IsCode)
				{
					// 刷新本地数据
					HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("本地源码库 (按 delete 删除源码)"));//插入根节点
					ShowFile(_T("Code"), m_TreeRoot);//根目录进行遍历
				}
				else
				{
					// 刷新本地数据
					HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("本地文件库 (按 delete 删除文件)"));//插入根节点
					ShowFile(_T("File"), m_TreeRoot);//根目录进行遍历
				}

				MessageBox(_T("云端已与本地数据完全同步!"), _T("同步完成"), MB_ICONINFORMATION);
			}
		}
		else
		{
			MessageBox(_T("云端已与本地数据完全同步!"), _T("同步完成"), MB_ICONINFORMATION);
		}
	}
}


void CSynchronizeDlg::OnUpload()
{
	// 初始化数据
	IsFile = false;
	TreeData = _T("");
	TargetList.RemoveAll();

	// 得到树数据
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
			// 去掉勾勾
			HTREEITEM hitem = NULL;
			m_Local.GetChildItem(hitem);
			m_Local.SetCheck(hitem, 0);

			// 清空云端列表
			m_Server.DeleteAllItems();

			// 类型判断
			if(IsCode)
			{
				Type = 1;
			}
			else
			{
				Type = 2;
			}

			// 刷新云端数据
			GetServerInfo();
		}
	}
	else
	{
		AfxMessageBox(_T("请勾选需要上传的文件!"));
	}
}


void CSynchronizeDlg::OnDownload()
{
	// 初始化数据
	IsFile = false;
	TreeData = _T("");
	TargetList.RemoveAll();

	// 得到树数据
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
			// 去掉勾勾
			HTREEITEM hitem = NULL;
			m_Server.GetChildItem(hitem);
			m_Server.SetCheck(hitem, 0);

			// 清空本地列表
			m_Local.DeleteAllItems();

			// 判断类型
			if(IsCode)
			{
				// 刷新本地数据
				HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("本地源码库 (按 delete 删除源码)"));//插入根节点
				ShowFile(_T("Code"), m_TreeRoot);//根目录进行遍历
			}
			else
			{
				// 刷新本地数据
				HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("本地文件库 (按 delete 删除文件)"));//插入根节点
				ShowFile(_T("File"), m_TreeRoot);//根目录进行遍历
			}
		}
	}
	else
	{
		AfxMessageBox(_T("请勾选需要下载的文件!"));
	}
}


void CSynchronizeDlg::OnSwitch()
{
	// 清空
	m_Local.DeleteAllItems();
	m_Server.DeleteAllItems();

	if(!IsCode)
	{
		// 本地
		HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("本地源码库 (按 delete 删除源码)"));//插入根节点
		ShowFile(_T("Code"), m_TreeRoot);//根目录进行遍历

		// 云端
		Type = 1;
		GetServerInfo();

		IsCode = TRUE;
	}
	else
	{
		HTREEITEM m_TreeRoot = m_Local.InsertItem(_T("本地文件库 (按 delete 删除文件)"));//插入根节点
		ShowFile(_T("File"), m_TreeRoot);//根目录进行遍历

		// 云端
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
