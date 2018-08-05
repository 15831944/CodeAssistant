// ProjectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "ProjectDlg.h"
#include "afxdialogex.h"

#include "ModifyDlg.h"
#include "NotifyDlg.h"
#include "SearchDlg.h"


// CProjectDlg 对话框

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


// CProjectDlg 消息处理程序


BOOL CProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Project.SetExtendedStyle(LVS_EX_FULLROWSELECT  //允许整行选中
		| LVS_EX_HEADERDRAGDROP				      //允许整列拖动
		| LVS_EX_SUBITEMIMAGES			          //单击选中项
		| LVS_EX_GRIDLINES);				      //画出网格线

	m_Project.SetHeadings(_T("项目名称,147;项目版本,120;项目路径,0;"));
	m_Project.LoadColumnInfo();

	// 字体
	LOGFONT   logfont;//最好弄成类成员,全局变量,静态成员  
    CFont   *pfont1   =   m_Project.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //这里可以修改字体的高比例
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //这里可以修改字体的宽比例
    static   CFont   font1;
    font1.CreateFontIndirect(&logfont);
    m_Project.SetFont(&font1);  
    font1.Detach();


	m_Version.SetExtendedStyle(LVS_EX_FULLROWSELECT  //允许整行选中
		| LVS_EX_HEADERDRAGDROP				      //允许整列拖动
		| LVS_EX_SUBITEMIMAGES			          //单击选中项
		| LVS_EX_GRIDLINES);				      //画出网格线

	m_Version.SetHeadings(_T("修改说明,161;修改时间,150;项目版本,120;"));
	m_Version.LoadColumnInfo();

	// 字体
    pfont1   =   m_Version.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //这里可以修改字体的高比例
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //这里可以修改字体的宽比例
  
    font1.CreateFontIndirect(&logfont);
    m_Version.SetFont(&font1);  
    font1.Detach();

	// 刷新
	Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CProjectDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

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
		// 清空
		this->pResult.clear();

		if (theApp.m_Sql.SelectData(_T("版本控制"), this->pResult, _T("Name = '") + Name + _T("'") ))
		{
			// 清空列表
			m_Version.DeleteAllItems();

			// 循环添加数据(有多少列 i 就加多少)
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
	dlg.Title    = _T("查找项目");
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
				//设置高亮显示  
				m_Project.SetFocus();//设置焦点  
				m_Project.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
				m_Project.EnsureVisible(i, FALSE);//设置当前视图可见 
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
		dlg.Title    = _T("查找版本");
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
					//设置高亮显示  
					m_Version.SetFocus();//设置焦点  
					m_Version.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
					m_Version.EnsureVisible(i, FALSE);//设置当前视图可见 
					break;
				}
			}
		}
	}

	*pResult = 0;
}


void CProjectDlg::Refresh()
{
	// 清空
	pResult.clear();

	if (theApp.m_Sql.SelectData(_T("项目管理"), pResult, _T("Type = 'Local'") ))
	{
		// 清空列表
		m_Project.DeleteAllItems();

		// 循环添加数据(有多少列 i 就加多少)
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
    CreateDirectory(target,NULL); //创建目标文件夹
    //AfxMessageBox("创建文件夹"+target);
    CFileFind finder;
    CString path;
    path.Format(_T("%s/*.*"),source);
    //AfxMessageBox(path);
    BOOL bWorking =finder.FindFile(path);
    while(bWorking)
    {
        bWorking = finder.FindNextFile();
        //AfxMessageBox(finder.GetFileName());
        if(finder.IsDirectory() && !finder.IsDots())//是文件夹 而且 名称不含 . 或 ..
		{
			// 无限递归bug打破
			char szAppPath[MAX_PATH];
			GetModuleFileName(NULL, szAppPath, MAX_PATH);
			(strrchr(szAppPath, '\\'))[0] = 0;
			if(finder.GetFilePath().Replace(szAppPath, _T("")) && target.Replace(szAppPath, _T("")) )
			{
				AfxMessageBox(_T("不能将程序自身添加进项目管理!"));
				break;
			}

			//递归创建文件夹+"/"+finder.GetFileName()
			CopyDirectory(finder.GetFilePath(),target+"/"+finder.GetFileName());
		}
        else//是文件 则直接复制
        {
            //AfxMessageBox("复制文件"+finder.GetFilePath());//+finder.GetFileName()
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
        if(finder.IsDirectory() && !finder.IsDots())//处理文件夹
        {
            DeleteDirectory(finder.GetFilePath()); //递归删除文件夹
            RemoveDirectory(finder.GetFilePath());
        }
        else//处理文件
        {
            DeleteFile(finder.GetFilePath());
        }
    }

    RemoveDirectory(Directory);
}


// 核心工作线程
UINT CProjectDlg::Operate(LPVOID pParam)
{
	// 窗口指针
	CProjectDlg * pWnd = ((CProjectDlg*)pParam);

	// 执行操作
	switch(pWnd->Type)
	{
	case 1:
		//复制项目
		pWnd->CopyDirectory(pWnd->TargetPath, pWnd->FilePath);
		break;

	case 2:
		//删除项目
		pWnd->DeleteDirectory(pWnd->TargetPath);
		break;
	}

	// 操作完成标志
	theApp.IsFinished = true;

	// 对象置为空
	pWnd->m_hOperate = NULL;
	return false;
}


// 消息提示线程
UINT CProjectDlg::Notify(LPVOID lpParameter)
{
	((CProjectDlg*)lpParameter)->OnShowNotify((LPCTSTR)lpParameter);
	return TRUE;
}


// 显示消息提醒
void CProjectDlg::OnShowNotify(CString Notify)
{
	// 操作完成标志
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
			AfxMessageBox(_T("项目路径不能带单引号!"));
			return;
		}

		// 无限递归bug打破
		char szAppPath[MAX_PATH];
		GetModuleFileName(NULL, szAppPath, MAX_PATH);
		(strrchr(szAppPath, '\\'))[0] = 0;
		if(dlg.m_Path.Replace(szAppPath, _T("")))
		{
			AfxMessageBox(_T("不能将程序自身添加进项目管理!"));
			return;
		}
		

		// 判断数据表中是否存在数据
		if (!theApp.m_Sql.CheckData(_T("项目管理"), 1, _T("Path = '") + dlg.m_Path + _T("' and Type = 'Local'")))
		{
			// 若写入数据库失败
			if (!theApp.m_Sql.InsertData(_T("项目管理"), _T("'") + dlg.m_Name + _T("','") + dlg.m_Path + _T("','Local','") + dlg.m_Version + _T("'")))
			{
				AfxMessageBox(_T("无法添加项目数据!"));
				return;
			}
			else
			{
				CString ModifyTime; //获取系统时间 　　
				CTime tm; tm = CTime::GetCurrentTime();
				ModifyTime   = tm.Format(_T("%Y-%m-%d %H:%M:%S"));

				// 若写入版本库失败
				if (!theApp.m_Sql.InsertData(_T("版本控制"), _T("'") + dlg.m_Name + _T("','最初的项目版本','") +  ModifyTime + _T("','") + dlg.m_Version + _T("','Local'")))
				{
					AfxMessageBox(_T("无法添加项目版本!"));
					return;
				}
				else
				{
					// 目录检测
					FilePath = _T("Project/") + dlg.m_Name;
					if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
					{
						CreateDirectory(FilePath, NULL);
					}

					FilePath = _T("Project/") + dlg.m_Name + _T("/") + dlg.m_Version;
					if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
					{
						CreateDirectory(FilePath, NULL);
					}

					// 复制项目

					// 显示消息提示
					AfxBeginThread(Notify, _T("正在执行操作..."));

					// 启动工作者线程
					TargetPath = dlg.m_Path;
					if (m_hOperate == NULL)
					{
						Type = 1;
						m_hOperate = AfxBeginThread(Operate, this);
					}

					// 添加列表
					m_Project.AddItem(dlg.m_Name, dlg.m_Version, dlg.m_Path);
					m_Version.AddItem(_T("最初的项目版本"), ModifyTime, dlg.m_Version);

					//设置高亮显示
					int i = m_Project.GetItemCount() -1;
					m_Project.SetItemState(m_Project.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

					m_Project.SetFocus();//设置焦点  
					m_Project.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
					m_Project.EnsureVisible(i, FALSE);//设置当前视图可见 
				}
			}
		}
		else
		{
			AfxMessageBox(_T("已存在相同的项目!"));
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
		AfxMessageBox(_T("一次只能编辑一个项目!"));
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

			if (!theApp.m_Sql.UpdataData(_T("项目管理"), ColumnParams, Params))
			{
				AfxMessageBox(_T("无法修改项目数据!"));
			}
			else
			{
				ColumnParams = _T("Name = '") + dlg.m_Name + _T("'");
				Params = _T("Name = '") + Name + _T("'");

				if (!theApp.m_Sql.UpdataData(_T("版本控制"), ColumnParams, Params))
				{
					AfxMessageBox(_T("无法修改项目版本!"));
				}
				else
				{
					// 重命名项目目录
					if(MoveFileEx(_T("Project/") + Name, _T("Project/") + dlg.m_Name, MOVEFILE_REPLACE_EXISTING))
					{
						// 更新列表
						m_Project.SetItemText(Row, 0, dlg.m_Name);
						m_Project.SetItemText(Row, 1, dlg.m_Version);
						m_Project.SetItemText(Row, 2, dlg.m_Path);

						//设置高亮显示
						m_Project.SetFocus();//设置焦点  
						m_Project.SetItemState(Row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
						m_Project.EnsureVisible(Row, FALSE);//设置当前视图可见 
					}
					else
					{
						AfxMessageBox(_T("无法重命名项目!"));
					}
				}
			}
		}
	}
	else
	{
		AfxMessageBox(_T("请选择要编辑的项目!"));
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
				if(MessageBox(_T("是否要移除当前项目? "), NULL, MB_YESNO) == IDYES)
				{
					// 从数据库中删除数据
					if (!theApp.m_Sql.DeleteData(_T("项目管理"), _T(" Path = '") + Path + _T("' and Type = 'Local'")))
					{
						AfxMessageBox(_T("删除数据失败!"));
					}
					else
					{
						// 从数据库中删除版本
						if (!theApp.m_Sql.DeleteData(_T("版本控制"), _T(" Name = '") + Name + _T("'")))
						{
							AfxMessageBox(_T("删除数据失败!"));
						}
						else
						{
							// 删除本地目录

							// 显示消息提示
							AfxBeginThread(Notify, _T("正在执行操作..."));

							// 启动工作者线程
							TargetPath = _T("Project/") + Name;
							if (m_hOperate == NULL)
							{
								Type = 2;
								m_hOperate = AfxBeginThread(Operate, this);
							}

							// 从列表中删除数据
							m_Project.DeleteItem(m_Project.GetNextItem(-1, LVIS_SELECTED));

							// 清空版本数据
							m_Version.DeleteAllItems();
						}
					}
				}
			}
			else
			{
				AfxMessageBox(_T("请选择要移除的项目!"));
			}
		}
		else if(Count > 1)
		{
			if(MessageBox(_T("是否要移除选中的项目? "), NULL, MB_YESNO) == IDYES)
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

						// 从数据库中删除数据
						if (!theApp.m_Sql.DeleteData(_T("项目管理"), _T(" Path = '") + Path + _T("' and Type = 'Local'")))
						{
							AfxMessageBox(_T("删除数据失败!"));
						}
						else
						{
							// 从数据库中删除版本
							if (!theApp.m_Sql.DeleteData(_T("版本控制"), _T(" Name = '") + Name + _T("'")))
							{
								AfxMessageBox(_T("删除数据失败!"));
							}
							else
							{
								// 删除本地目录

								// 显示消息提示
								AfxBeginThread(Notify, _T("正在执行操作..."));

								// 启动工作者线程
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

				//	// 从数据库中删除数据
				//	if (!theApp.m_Sql.DeleteData(_T("项目管理"), _T(" Path = '") + Path + _T("' and Type = 'Local'")))
				//	{
				//		AfxMessageBox(_T("删除数据失败!"));
				//	}
				//	else
				//	{
				//		// 从数据库中删除版本
				//		if (!theApp.m_Sql.DeleteData(_T("版本控制"), _T(" Name = '") + Name + _T("'")))
				//		{
				//			AfxMessageBox(_T("删除数据失败!"));
				//		}
				//		else
				//		{
				//			// 删除本地目录
				//			
				//			// 显示消息提示
				//			AfxBeginThread(Notify, _T("正在执行操作..."));

				//			// 启动工作者线程
				//			TargetPath = _T("Project/") + Name;
				//			if (m_hOperate == NULL)
				//			{
				//				Type = 2;
				//				m_hOperate = AfxBeginThread(Operate, this);
				//			}
				//		}
				//	}
				//}

				// 清空版本数据
				//m_Version.DeleteAllItems();

				//Refresh();
			}
		}
		else
		{
			AfxMessageBox(_T("请选择要移除的项目!"));
		}
	}
	else
	{
		AfxMessageBox(_T("请选择要移除的项目!"));
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
			AfxMessageBox(_T("一次只能添加一个项目的一个版本!"));
			return;
		}

		if(dlg.DoModal() == IDOK)
		{
			if (dlg.m_Name.Replace(_T("'"), _T("")))
			{
				AfxMessageBox(_T("修改说明不能带单引号!"));
				return;
			}

			// 判断数据表中是否存在数据
			if (!theApp.m_Sql.CheckData(_T("版本控制"), 1, _T("Name = '") + Name + _T("' and Version = '") + dlg.m_Version + _T("' and Type = 'Local'")))
			{
				CString ModifyTime; //获取系统时间
				CTime tm; tm = CTime::GetCurrentTime();
				ModifyTime   = tm.Format(_T("%Y-%m-%d %H:%M:%S"));

				// 若写入版本库失败
				if (!theApp.m_Sql.InsertData(_T("版本控制"), _T("'") + Name + _T("','") + dlg.m_Name + _T("','") +  ModifyTime + _T("','") + dlg.m_Version + _T("','Local'")))
				{
					AfxMessageBox(_T("无法添加项目版本!"));
					return;
				}
				else
				{
					// 修改当前版本
					CString ColumnParams = _T("Version = '") + dlg.m_Version + _T("'"), Params = _T("Path = '") + Path + _T("' And Type = 'Local'");
					if (!theApp.m_Sql.UpdataData(_T("项目管理"), ColumnParams, Params))
					{
						AfxMessageBox(_T("无法修改项目版本数据!"));
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
								// 修改版本号
								m_Project.SetItemText(i, 1, dlg.m_Version);
								break;
							}
						}
					}


					// 目录检测
					FilePath = _T("Project/") + Name + _T("/") + dlg.m_Version;
					if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
					{
						CreateDirectory(FilePath, NULL);
					}

					// 复制项目

					// 显示消息提示
					AfxBeginThread(Notify, _T("正在执行操作..."));

					// 启动工作者线程
					TargetPath = Path;
					if (m_hOperate == NULL)
					{
						Type = 1;
						m_hOperate = AfxBeginThread(Operate, this);
					}

					// 添加列表
					m_Version.AddItem(dlg.m_Name, ModifyTime, dlg.m_Version);

					//设置高亮显示
					int i = m_Version.GetItemCount() -1;
					m_Version.SetItemState(m_Version.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

					m_Version.SetFocus();//设置焦点  
					m_Version.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
					m_Version.EnsureVisible(i, FALSE);//设置当前视图可见 
				}
			}
			else
			{
				AfxMessageBox(_T("已存在相同的版本!"));
			}
		}
	}
	else
	{
		AfxMessageBox(_T("请选择要添加版本的项目!"));
	}
}


void CProjectDlg::OnRestore()
{
	int Count = m_Version.GetSelectedCount();
	if(Count > 1)
	{
		AfxMessageBox(_T("一次只能还原一个版本!"));
		return;
	}

	CString Version = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2);
	if (!Name.IsEmpty() && m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		if(MessageBox(_T("是否要恢复当前版本? "), NULL, MB_YESNO) == IDYES)
		{
			// 修改当前版本
			CString ColumnParams = _T("Version = '") + Version + _T("'"), Params = _T("Path = '") + Path + _T("' And Type = 'Local'");
			if (!theApp.m_Sql.UpdataData(_T("项目管理"), ColumnParams, Params))
			{
				AfxMessageBox(_T("无法修改项目版本数据!"));
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
						// 修改版本号
						m_Project.SetItemText(i, 1, Version);
						break;
					}
				}
			}

			// 项目路径
			FilePath = _T("Project/") + Name + _T("/") + Version;

			// 复制项目
			TargetPath = FilePath;
			FilePath   = Path;

			// 显示消息提示
			AfxBeginThread(Notify, _T("正在执行操作..."));

			// 启动工作者线程
			if (m_hOperate == NULL)
			{
				Type = 1;
				m_hOperate = AfxBeginThread(Operate, this);
			}

			//MessageBox(_T("版本恢复完成."));
		}
	}
	else
	{
		AfxMessageBox(_T("请选择要恢复的版本!"));
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
		AfxMessageBox(_T("一次只能编辑一个版本!"));
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
			CString ModifyTime; //获取系统时间 　　
			CTime tm; tm = CTime::GetCurrentTime();
			ModifyTime   = tm.Format(_T("%Y-%m-%d %H:%M:%S"));

			CString ColumnParams = _T("Explain = '") + dlg.m_Name + _T("', ModifyTime = '") + ModifyTime + _T("', Version = '") + dlg.m_Version + _T("'");
			CString Params = _T("Name = '") + Name + _T("' and Version = '") + Version + _T("'");

			if (!theApp.m_Sql.UpdataData(_T("版本控制"), ColumnParams, Params))
			{
				AfxMessageBox(_T("无法修改项目版本!"));
			}
			else
			{
				// 重命名项目目录
				if(MoveFileEx(_T("Project/") + Name + _T("/") + Version, _T("Project/") + Name + _T("/") + dlg.m_Version, MOVEFILE_REPLACE_EXISTING))
				{
					// 更新列表
					m_Version.SetItemText(Row, 0, dlg.m_Name);
					m_Version.SetItemText(Row, 1, ModifyTime);
					m_Version.SetItemText(Row, 2, dlg.m_Version);

					//设置高亮显示
					m_Version.SetFocus();//设置焦点  
					m_Version.SetItemState(Row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
					m_Version.EnsureVisible(Row, FALSE);//设置当前视图可见 
				}
				else
				{
					AfxMessageBox(_T("无法重命名版本!"));
				}
			}
		}
	}
	else
	{
		AfxMessageBox(_T("请选择要编辑的版本!"));
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
				if(MessageBox(_T("是否要移除当前版本? "), NULL, MB_YESNO) == IDYES)
				{
					// 从数据库中删除版本
					if (!theApp.m_Sql.DeleteData(_T("版本控制"), _T(" Name = '") + Name + _T("' and Version = '") +  Version + _T("'") ))
					{
						AfxMessageBox(_T("移除版本数据失败!"));
					}
					else
					{
						// 删除本地目录

						// 显示消息提示
						AfxBeginThread(Notify, _T("正在执行操作..."));

						// 启动工作者线程
						TargetPath = _T("Project/") + Name + _T("/") + Version;
						if (m_hOperate == NULL)
						{
							Type = 2;
							m_hOperate = AfxBeginThread(Operate, this);
						}

						// 从列表中删除数据
						m_Version.DeleteItem(m_Version.GetNextItem(-1, LVIS_SELECTED));
					}
				}
			}
			else
			{
				AfxMessageBox(_T("请选择要移除的版本!"));
			}
		}
		else if(Count > 1)
		{
			if(MessageBox(_T("是否要移除选中的版本? "), NULL, MB_YESNO) == IDYES)
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

						// 从数据库中删除版本
						if (!theApp.m_Sql.DeleteData(_T("版本控制"), _T(" Name = '") + Name + _T("' and Version = '") +  Version + _T("'") ))
						{
							AfxMessageBox(_T("移除版本数据失败!"));
						}
						else
						{
							// 删除本地目录

							// 显示消息提示
							AfxBeginThread(Notify, _T("正在执行操作..."));

							// 启动工作者线程
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

				//	// 从数据库中删除版本
				//	if (!theApp.m_Sql.DeleteData(_T("版本控制"), _T(" Name = '") + Name + _T("' and Version = '") +  Version + _T("'") ))
				//	{
				//		AfxMessageBox(_T("移除版本数据失败!"));
				//	}
				//	else
				//	{
				//		// 删除本地目录

				//		// 显示消息提示
				//		AfxBeginThread(Notify, _T("正在执行操作..."));

				//		// 启动工作者线程
				//		TargetPath = _T("Project/") + Name + _T("/") + Version;
				//		if (m_hOperate == NULL)
				//		{
				//			Type = 2;
				//			m_hOperate = AfxBeginThread(Operate, this);
				//		}
				//	}
				//}

				//// 更新数据
				//OnItemchangedProjectList(NULL, NULL);
			}
		}
		else
		{
			AfxMessageBox(_T("请选择要移除的版本!"));
		}
	}
	else
	{
		AfxMessageBox(_T("请选择要移除的版本!"));
	}
}


void CProjectDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
