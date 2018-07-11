// WebProjectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "WebProjectDlg.h"
#include "afxdialogex.h"

#include "WebEditDlg.h"
#include "SearchDlg.h"
#include "TransmissionDlg.h"
#include "ProjectDlg.h"

// json
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4005)
#pragma warning (disable : 4482)
#pragma warning (disable : 4996)
#include "json/json.h"
#pragma warning (pop)
#endif


// CWebProjectDlg 对话框

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
	ON_BN_CLICKED(IDC_MERGE_BUTTON, &CWebProjectDlg::OnMerge)
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
	ON_COMMAND(102, &CWebProjectDlg::AddVersion)
	ON_COMMAND(103, &CWebProjectDlg::AddProject)
END_MESSAGE_MAP()


// CWebProjectDlg 消息处理程序


BOOL CWebProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Project.SetExtendedStyle(LVS_EX_FULLROWSELECT  //允许整行选中
		| LVS_EX_HEADERDRAGDROP				         //允许整列拖动
		| LVS_EX_SUBITEMIMAGES			             //单击选中项
		| LVS_EX_GRIDLINES);				         //画出网格线

	m_Project.SetHeadings(_T("项目名称,147;项目说明,120;共享状态,70;项目编号,0;用户编号,0;用户角色,0;"));
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

	m_Version.SetHeadings(_T("修改说明,161;修改时间,150;项目版本-分支,120;修改者,65;项目分支,0;版本编号,0;"));
	m_Version.LoadColumnInfo();

	// 字体
    pfont1   =   m_Version.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //这里可以修改字体的高比例
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //这里可以修改字体的宽比例
  
    font1.CreateFontIndirect(&logfont);
    m_Version.SetFont(&font1);  
    font1.Detach();


	// 默认选中
	((CButton*)GetDlgItem(IDC_ASSISTANT_RADIO))->SetCheck(1);
	OnAssistant();

	// 刷新数据
	OnOK();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CWebProjectDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CWebProjectDlg::CopyDirectory(CString source, CString target)  
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


// 工作线程
UINT CWebProjectDlg::Operate(LPVOID pParam)
{
	// 窗口指针
	CWebProjectDlg * pWnd = ((CWebProjectDlg*)pParam);

	// 局部变量
	CString RecvData;
	BOOL IsSuccess;

	// 禁用按钮
	pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);

	// 获取服务器数据
	try
	{
		switch(pWnd->OperateType)
		{
		case 1:
			{
				if(pWnd->ProjectType == 2)
					RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetProject"), 80, pWnd->Parameter, IsSuccess);
				else if(pWnd->ProjectType == 3)
					RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetShareProject"), 80, pWnd->Parameter, IsSuccess);
				else
					RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetOpenProject"), 80, pWnd->Parameter, IsSuccess);


				if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
				{
					pWnd->Error = _T("无法连接到服务器, 请检查网络。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
				else
				{
					if (IsSuccess)
					{
						Json::Reader reader;
						Json::Value root;

						// 定义接收数据
						CStringA m_RecvData(RecvData);

						// reader将Json字符串解析到root，root将包含Json里所有子元素
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
									

									CString m_UserId     = theApp.Convert(UserId);
									CString m_Project    = theApp.Convert(Project);
									CString m_Version    = theApp.Convert(Version);
									CString m_Status     = theApp.Convert(Status);
									CString m_Number     = theApp.Convert(Number);
									CString m_Role       = theApp.Convert(Role);

									// 处理数据
									if(m_Status == _T("0"))
										m_Status = _T("不共享");
									else if(m_Status == _T("1"))
										m_Status = _T("所有人");
									else
										m_Status = _T("工作组");

									if(!m_Project.IsEmpty() && !m_Version.IsEmpty() && !m_Status.IsEmpty())
										pWnd->m_Project.AddItem(m_Project, m_Version, m_Status, m_Number, m_UserId, m_Role);
								}
							}
						}
						else
						{
							pWnd->Error = _T("无法解析数据。");
							pWnd->PostMessage(WM_COMMAND, 101);
						}
					}
					else
					{
						pWnd->Error = _T("无法连接到服务器, 请检查网络。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
			} break;

		case 2:
			RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/DeleteProject"), 80, pWnd->Parameter, IsSuccess);

			// 读数据
			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				pWnd->Error = _T("无法连接到服务器, 请检查网络。");
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
						pWnd->Error = _T("无法删除项目，请稍后再试。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("无法连接到服务器, 请检查网络。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;

		case 3:
			Refresh:
			RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/GetVersion"), 80, pWnd->Parameter, IsSuccess);

			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				pWnd->Error = _T("无法连接到服务器, 请检查网络。");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
			else
			{
				if (IsSuccess)
				{
					Json::Reader reader;
					Json::Value root;

					// 定义接收数据
					CStringA m_RecvData(RecvData);

					// reader将Json字符串解析到root，root将包含Json里所有子元素
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
								std::string Editor   = Items[i]["Editor"].asString();
								std::string Branch   = Items[i]["Branch"].asString();
								std::string Number   = Items[i]["VersionId"].asString();

								CString m_Remark     = theApp.Convert(Remark);
								CString m_MTime      = theApp.Convert(MTime);
								CString m_Version    = theApp.Convert(Version);
								CString m_Editor     = theApp.Convert(Editor);
								CString m_Branch     = theApp.Convert(Branch);
								CString m_Number     = theApp.Convert(Number);

								if(!m_Remark.IsEmpty() && !m_Version.IsEmpty() && !m_MTime.IsEmpty())
									pWnd->m_Version.AddItem(m_Remark, m_MTime, m_Version + _T("-") + m_Branch, m_Editor, m_Branch, m_Number);
							}
						}
					}
					else
					{
						pWnd->Error = _T("无法解析数据。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("无法连接到服务器, 请检查网络。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;

		case 4:
			RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/RemoveVersion"), 80, pWnd->Parameter, IsSuccess);

			// 读数据
			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				pWnd->Error = _T("无法连接到服务器, 请检查网络。");
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
					else if(RecvData == _T("NotFind"))
					{
						pWnd->Error = _T("该版本不存在，无法移除。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else
					{
						pWnd->Error = _T("无法移除版本，请稍后再试。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("无法连接到服务器, 请检查网络。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;

			case 5:
			RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/MergeVersion"), 80, pWnd->Parameter, IsSuccess);

			// 读数据
			if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
			{
				pWnd->Error = _T("无法连接到服务器, 请检查网络。");
				pWnd->PostMessage(WM_COMMAND, 101);
			}
			else
			{
				if (IsSuccess)
				{
					if( RecvData == _T("success") )
					{
						// 刷新
						pWnd->Parameter.Format(_T("project_id=%s&user_id=%s"), pWnd->Number, pWnd->UserId);
						goto Refresh;

					}
					else if(RecvData == _T("NotFind"))
					{
						pWnd->Error = _T("该版本不存在，无法合并。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else if(RecvData == _T("top"))
					{
						pWnd->Error = _T("master或develop分支无法合并。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else
					{
						pWnd->Error = _T("无法合并版本，请稍后再试。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("无法连接到服务器, 请检查网络。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;

			case 6:
			{
				// 创建压缩文件
				CString CommandLine;
				CommandLine.Format(_T("a -t7z \"%s\" \"%s\" -r -mmt -mx=9 -ms=1024m -mf -mhc -mhcf -m1=LZMA2:a=2:d=25:fb=64"), pWnd->Version + _T(".7z"), pWnd->Path);

				SHELLEXECUTEINFO ShExecInfo = {0};
				ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
				ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

				ShExecInfo.lpFile       = "7z.exe";
				ShExecInfo.lpDirectory  = "./Application/";
				ShExecInfo.lpParameters = CommandLine;

				ShExecInfo.nShow = SW_HIDE;
				ShellExecuteEx(&ShExecInfo);

				AfxGetApp()->BeginWaitCursor();
				WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
				AfxGetApp()->EndWaitCursor();

				// 目录检测
				CString FilePath = _T("./Cache/") + pWnd->Project;
				if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
				{
					// 创建目录
					CreateDirectory(FilePath, NULL);
				}

				FilePath = _T("./Cache/") + pWnd->Project + _T("/") + pWnd->Type;
				if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
				{
					// 创建目录
					CreateDirectory(FilePath, NULL);
				}

				// 移动目标
				if(!MoveFileEx(_T("./Application/") + pWnd->Version + _T(".7z"), _T("./Cache/") + pWnd->Project + _T("/") + pWnd->Type + _T("/") + pWnd->Version + _T(".7z"), MOVEFILE_REPLACE_EXISTING))
				{
					AfxMessageBox(_T("移动目标失败!"));
				}
				else
				{
					// 后续处理
					pWnd->PostMessage(WM_COMMAND, 102);
				}

			}break;

			case 7:
			{
				// 创建压缩文件
				CString CommandLine;
				CommandLine.Format(_T("a -t7z \"%s\" \"%s\" -r -mmt -mx=9 -ms=1024m -mf -mhc -mhcf -m1=LZMA2:a=2:d=25:fb=64"), _T("Default.7z"), pWnd->Path);

				SHELLEXECUTEINFO ShExecInfo = {0};
				ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
				ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

				ShExecInfo.lpFile       = "7z.exe";
				ShExecInfo.lpDirectory  = "./Application/";
				ShExecInfo.lpParameters = CommandLine;

				ShExecInfo.nShow = SW_HIDE;
				ShellExecuteEx(&ShExecInfo);

				AfxGetApp()->BeginWaitCursor();
				WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
				AfxGetApp()->EndWaitCursor();

				// 目录检测
				CString FilePath = _T("./Cache/") + pWnd->Project;
				if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
				{
					// 创建目录
					CreateDirectory(FilePath, NULL);
				}

				FilePath = _T("./Cache/") + pWnd->Project + _T("/") + pWnd->Type;
				if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
				{
					// 创建目录
					CreateDirectory(FilePath, NULL);
				}

				// 移动目标
				if(!MoveFileEx(_T("./Application/Default.7z"), _T("./Cache/") + pWnd->Project + _T("/") + pWnd->Type + _T("/Default.7z"), MOVEFILE_REPLACE_EXISTING))
				{
					AfxMessageBox(_T("移动目标失败!"));
				}
				else
				{
					// 后续处理
					pWnd->PostMessage(WM_COMMAND, 103);
				}

			}break;
		}
	}
	catch (...)
	{
		pWnd->Error = _T("发生了异常，位于Operate的OnGetWebInfo方法。");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	// 激活按钮
	pWnd->GetDlgItem(IDOK)->EnableWindow();

	// 对象置为空
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
		// 启动工作者线程
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
 

void CWebProjectDlg::AddTarget(CString Path, CStringArray &TargetList)
{
	// 读取数据
	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(Path + "./*.*");
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			AddTarget(Finder.GetFilePath(), TargetList);
		else
		{
			CString Target = UserId + _T("/Cache/") + Finder.GetFilePath();
			Target.Replace(Project + _T("\\"), _T(""));
			Target.Replace(_T("\\"), _T("/"));
			TargetList.Add(Target);
		}
	}
}


void CWebProjectDlg::OnOK()
{
	switch(DataSource)
	{
	case 1:
		// 启动工作者线程
		if (m_hOperate == NULL)
		{
			OperateType=1;
			Parameter.Format(_T("id=%s"), UserId);

			m_hOperate = AfxBeginThread(Operate, this);
		}
		break;

	case 2:
		AfxMessageBox(_T("功能正在开发中,敬请期待!"));
		break;

	case 3:
		AfxMessageBox(_T("功能正在开发中,敬请期待!"));
		break;

	case 4:
		AfxMessageBox(_T("功能正在开发中,敬请期待!"));
		break;

	case 5:
		AfxMessageBox(_T("功能正在开发中,敬请期待!"));
		break;
	}
}


void CWebProjectDlg::OnAssistant()
{
	// 禁用编辑框
	GetDlgItem(IDC_USER_EDIT)    ->EnableWindow(false);
	GetDlgItem(IDC_PASSWORD_EDIT)->EnableWindow(false);
	GetDlgItem(IDC_USER_EDIT)->SetWindowText(UserName);

	// 数据源
	DataSource = 1;
}


void CWebProjectDlg::OnGithub()
{
	// 启用编辑框
	GetDlgItem(IDC_USER_EDIT)    ->EnableWindow();
	GetDlgItem(IDC_PASSWORD_EDIT)->EnableWindow();
	GetDlgItem(IDC_USER_EDIT)->SetWindowText(_T(""));

	// 数据源
	DataSource = 2;
}


void CWebProjectDlg::OnGitee()
{
	// 启用编辑框
	GetDlgItem(IDC_USER_EDIT)    ->EnableWindow();
	GetDlgItem(IDC_PASSWORD_EDIT)->EnableWindow();
	GetDlgItem(IDC_USER_EDIT)->SetWindowText(_T(""));

	// 数据源
	DataSource = 3;
}


void CWebProjectDlg::OnSourceforge()
{
	// 启用编辑框
	GetDlgItem(IDC_USER_EDIT)    ->EnableWindow();
	GetDlgItem(IDC_PASSWORD_EDIT)->EnableWindow();
	GetDlgItem(IDC_USER_EDIT)->SetWindowText(_T(""));

	// 数据源
	DataSource = 4;
}


void CWebProjectDlg::OnGoogle()
{
	// 启用编辑框
	GetDlgItem(IDC_USER_EDIT)    ->EnableWindow();
	GetDlgItem(IDC_PASSWORD_EDIT)->EnableWindow();
	GetDlgItem(IDC_USER_EDIT)->SetWindowText(_T(""));

	// 数据源
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

		// 权限判断
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

		// 分享类型判断
		if(ProjectType != 1)
			GetDlgItem(IDC_ADD_BUTTON)->EnableWindow();
		else if(OwnerId == UserId)
			GetDlgItem(IDC_ADD_BUTTON)->EnableWindow();

		// 得到项目版本

		// 启动工作者线程
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
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CWebProjectDlg::OnRclickProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CSearchDlg dlg;
	dlg.Title    = _T("查找项目");
	dlg.FilePath = _T("index/account/ProjectInfo");
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
				//设置高亮显示  
				m_Project.SetFocus();//设置焦点  
				m_Project.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
				m_Project.EnsureVisible(i, FALSE);//设置当前视图可见 

				// 模拟点击
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
	// TODO: 在此添加控件通知处理程序代码
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
			GetDlgItem(IDC_MERGE_BUTTON) ->EnableWindow();
		}
		else
		{
			if(_stricmp(Editor, UserName) == 0)
			{
				GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow();
				GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow();
				GetDlgItem(IDC_MERGE_BUTTON) ->EnableWindow();
			}
			else
			{
				GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
				GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(false);
				GetDlgItem(IDC_MERGE_BUTTON) ->EnableWindow(false);
			}
		}

		GetDlgItem(IDC_CLONE_BUTTON)->EnableWindow();
	}
	else
	{
		GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_MERGE_BUTTON) ->EnableWindow(false);
		GetDlgItem(IDC_CLONE_BUTTON) ->EnableWindow(false);
	}

	*pResult = 0;
}


void CWebProjectDlg::OnDblclkVersionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CWebProjectDlg::OnRclickVersionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CSearchDlg dlg;
	dlg.Title    = _T("查找版本");
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
			CString Branch  = m_Version.GetItemText(i, 4);
			Version.Replace(_T("-") + Branch, _T(""));

			if(Function == Version)
			{
				//设置高亮显示  
				m_Version.SetFocus();//设置焦点  
				m_Version.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
				m_Version.EnsureVisible(i, FALSE);//设置当前视图可见 

				// 模拟点击
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
		// 显示消息提示
		AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

		// 进行添加操作
		OperateType = 7;
		ProjectId   = dlg.ProjectId;
		Project     = dlg.m_Project;
		Share       = dlg.m_Share;
		Version     = dlg.m_Version;
		Path        = dlg.m_Path;
		Type        = dlg.m_Type;
		m_hOperate  = AfxBeginThread(Operate, this);
	}
}


void CWebProjectDlg::AddProject()
{
	// 上传压缩文件
	CStringArray TargetList;
	CString      Target = UserId + _T("./Cache/") + Project + _T("/") + Type + _T("/Default.7z");
	TargetList.Add(Target);

	CTransmissionDlg pDlg;
	pDlg.TargetList = &TargetList;
	pDlg.IsDownload = false;
	pDlg.IsCode     = false;
	pDlg.IsProject  = true;
	pDlg.DoModal();

	if(pDlg.IsFinished)
	{
		// 操作完成标志
		theApp.IsFinished = true;

		// 添加到列表
		m_Project.AddItem(Project, Version, Share, ProjectId, UserId, _T("1"));
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
			AfxMessageBox(_T("一次只能编辑一个项目!"));
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
			// 更新列表
			m_Project.SetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 0, dlg.m_Project);
			m_Project.SetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 1, dlg.m_Version);
			m_Project.SetItemText(m_Project.GetNextItem(-1, LVIS_SELECTED), 2, dlg.m_Share);

			//设置高亮显示
			m_Project.SetFocus();//设置焦点  
			m_Project.SetItemState(m_Project.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
			m_Project.EnsureVisible(m_Project.GetNextItem(-1, LVIS_SELECTED), FALSE);//设置当前视图可见 
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

			// 启动工作者线程
			if (m_hOperate == NULL)
			{
				OperateType = 2;
				Parameter.Format(_T("project_id=%s"), ProjectId);

				m_hOperate = AfxBeginThread(Operate, this);
			}

			// 从列表中删除数据
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

			//		// 启动工作者线程
			//		if (m_hOperate == NULL)
			//		{
			//			Total   = Count;
			//			OperateType = 2;
			//			Parameter.Format(_T("project_id=%s"), ProjectId);

			//			m_hOperate = AfxBeginThread(Operate, this);
			//		}

			//		// 删除项目
			//		//m_Project.DeleteItem(nItem);
			//	}
			//}
			AfxMessageBox(_T("一次只能删除一个项目!"));
		}

		// 清空版本列表
		m_Version.DeleteAllItems();
	}
}


void CWebProjectDlg::OnSwitch()
{
	switch(ProjectType)
	{
	case 1:
		ProjectType = 2;
		GetDlgItem(IDC_PROJECT_STATIC)->SetWindowText(_T("项目管理 (创建的项目)"));
		GetDlgItem(IDC_NEW_BUTTON)->EnableWindow();
		break;

	case 2:
		ProjectType = 3;
		GetDlgItem(IDC_PROJECT_STATIC)->SetWindowText(_T("项目管理 (参与的项目)"));
		GetDlgItem(IDC_NEW_BUTTON)->EnableWindow(false);
		break;

	case 3:
		ProjectType = 1;
		GetDlgItem(IDC_PROJECT_STATIC)->SetWindowText(_T("项目管理 (共享的项目)"));
		GetDlgItem(IDC_NEW_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_ADD_BUTTON)->EnableWindow(false);
		break;
	}

	// 初始化
	GetDlgItem(IDC_EDIT_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(false);

	m_Version.DeleteAllItems();
	GetDlgItem(IDC_ADD_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_REMOVE_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_MERGE_BUTTON) ->EnableWindow(false);
	GetDlgItem(IDC_CLONE_BUTTON) ->EnableWindow(false);

	// 启动工作者线程
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
		dlg.UserId    = UserId;

		if(dlg.DoModal() == IDOK)
		{
			// 显示消息提示
			AfxBeginThread(CProjectDlg::Notify, _T("正在执行操作..."));

			// 进行添加操作
			OperateType = 6;
			VersionId   = dlg.VersionId;
			Remark      = dlg.m_Project;
			Version     = dlg.m_Version;
			Path        = dlg.m_Path;
			Type        = dlg.m_Type;
			m_hOperate  = AfxBeginThread(Operate, this);
		}
	}
	else
	{
		AfxMessageBox(_T("请选择目标项目!"));
	}
}


void CWebProjectDlg::AddVersion()
{
	// 上传压缩文件
	CStringArray TargetList;
	CString      Target = UserId + _T("./Cache/") + Project + _T("/") + Type + _T("/") + Version + _T(".7z");
	TargetList.Add(Target);

	CTransmissionDlg pDlg;
	pDlg.TargetList = &TargetList;
	pDlg.IsDownload = false;
	pDlg.IsCode     = false;
	pDlg.IsProject  = true;
	pDlg.DoModal();

	// 得到修改时间
	CString ModifyTime; //获取系统时间
	CTime tm; tm = CTime::GetCurrentTime();
	ModifyTime   = tm.Format(_T("%Y-%m-%d %H:%M:%S"));

	if(pDlg.IsFinished)
	{
		// 操作完成标志
		theApp.IsFinished = true;

		// 添加到列表
		m_Version.AddItem(Remark, ModifyTime, Version + _T("-") + Type, UserName, Type, VersionId);
	}
}


void CWebProjectDlg::OnMerge()
{
	int Count = m_Version.GetSelectedCount();

	if (m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		if(Count == 1)
		{
			CString VersionId = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 5);
			CString Branch    = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 4);
			CString Version   = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2);
			Version.Replace(_T("-") + Branch, _T(""));

			// 启动工作者线程
			if (m_hOperate == NULL)
			{
				OperateType = 5;
				Parameter.Format(_T("version_id=%s&branch=%s&project=%s&version=%s&user=%s"), VersionId, Branch, Project, Version, UserId);

				m_hOperate = AfxBeginThread(Operate, this);
			}
		}
		else
		{
			AfxMessageBox(_T("一次只能合并一个版本!"));
		}
	}
}


void CWebProjectDlg::OnModify()
{
	int Count = m_Version.GetSelectedCount();
	if(Count > 1)
	{
		AfxMessageBox(_T("一次只能编辑一个版本!"));
		return;
	}

	if (Project != _T("") && m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString Explain = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 0);
		CString MTime   = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 1);
		CString Version = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2);
		CString Type    = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 4);
		CString Ver_Id  = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 5);

		// 处理数据
		Version.Replace(_T("-") + Type, _T(""));

		int Count = m_Project.GetSelectedCount();
		if(Count > 1)
		{
			AfxMessageBox(_T("一次只能编辑一个项目!"));
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
			CString ModifyTime; //获取系统时间
			CTime tm; tm = CTime::GetCurrentTime();
			ModifyTime   = tm.Format(_T("%Y-%m-%d %H:%M:%S"));

			// 更新列表
			m_Version.SetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 0, dlg.m_Project);
			m_Version.SetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 1, ModifyTime);
			m_Version.SetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2, dlg.m_Version + _T("-") + dlg.m_Type);
			m_Version.SetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 3, UserName);
			m_Version.SetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 4, dlg.m_Type);

			//设置高亮显示
			m_Version.SetFocus();//设置焦点  
			m_Version.SetItemState( m_Version.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
			m_Version.EnsureVisible(m_Version.GetNextItem(-1, LVIS_SELECTED), FALSE);//设置当前视图可见 
		}
	}
	else if(m_Version.GetNextItem(-1, LVIS_SELECTED) == -1)
	{
		AfxMessageBox(_T("请选择目标版本!"));
	}
	else
	{
		AfxMessageBox(_T("请选择目标项目!"));
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
			CString Branch    = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 4);
			CString Version   = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2);
			Version.Replace(_T("-") + Branch, _T(""));

			// 不可移除master分支
			if(Branch == _T("master"))
			{
				AfxMessageBox(_T("不可移除master分支!"));
				return;
			}

			// 启动工作者线程
			if (m_hOperate == NULL)
			{
				OperateType = 4;
				Parameter.Format(_T("version_id=%s&branch=%s&project=%s&version=%s&user=%s"), VersionId, Branch, Project, Version, UserId);

				m_hOperate = AfxBeginThread(Operate, this);
			}

			// 从列表中删除数据
			m_Version.DeleteItem(m_Version.GetNextItem(-1, LVIS_SELECTED));
		}
		else
		{
			AfxMessageBox(_T("一次只能删除一个版本!"));
		}
	}
}


void CWebProjectDlg::OnClone()
{
	TCHAR           szFolderPath[MAX_PATH] = {0};  
	CString         strFolderPath = TEXT("");  

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot  = 0;
	sInfo.lpszTitle = _T("请选择一个目录：");
	sInfo.ulFlags   = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	sInfo.lpfn      = NULL;

	// 显示文件夹选择对话框
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath)) 
		{
			strFolderPath = szFolderPath;
		}
	}
	if(lpidlBrowse != NULL)  
	{  
		::CoTaskMemFree(lpidlBrowse);  
	}

	// 返回
	if(strFolderPath.IsEmpty())
		return;


	if (m_Version.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString Branch    = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 4);
		CString Version   = m_Version.GetItemText(m_Version.GetNextItem(-1, LVIS_SELECTED), 2);
		Version.Replace(_T("-") + Branch, _T(""));

		CStringArray TargetList;
		CString      Target = UserId + _T("/Project/") + Project + _T("/") + Branch + _T("/") + Version + _T(".7z");
		TargetList.Add(Target);

		CTransmissionDlg pDlg;
		pDlg.TargetList = &TargetList;
		pDlg.TargetPath = strFolderPath;
		pDlg.IsDownload = true;
		pDlg.IsCode     = false;
		pDlg.IsProject  = true;
		pDlg.DoModal();

		if(pDlg.IsFinished)
		{
			// 打开项目目录
			ShellExecute(NULL, _T("open"), strFolderPath, NULL, NULL, SW_SHOW);
		}
	}
}


void CWebProjectDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
