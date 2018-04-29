// WebEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
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


// CWebEditDlg 对话框

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


// CWebEditDlg 消息处理程序


BOOL CWebEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// List 初始化
	m_Group.SetExtendedStyle(LVS_EX_FULLROWSELECT  //允许整行选中
		| LVS_EX_HEADERDRAGDROP				       //允许整列拖动
		| LVS_EX_SUBITEMIMAGES			           //单击选中项
		| LVS_EX_GRIDLINES);				       //画出网格线

	m_Group.SetHeadings(_T("用户名称,163;用户角色,95;参与版本,95;"));
	m_Group.LoadColumnInfo();

	// 字体
	LOGFONT   logfont;//最好弄成类成员,全局变量,静态成员  
    CFont   *pfont1   =   m_Group.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //这里可以修改字体的高比例
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //这里可以修改字体的宽比例
    static   CFont   font1;
    font1.CreateFontIndirect(&logfont);
    m_Group.SetFont(&font1);  
    font1.Detach();

	// ComboBox 初始化
	((CComboBox*)GetDlgItem(IDC_TYPE_COMBO))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_SHARE_COMBO))->SetCurSel(0);


	if(IsNew)
	{
		if(IsProject)
		{
			SetWindowText(_T("添加云端项目"));
			GetDlgItem(IDC_UNDO_BUTTON)->EnableWindow(false);
			GetDlgItem(IDC_TYPE_COMBO)->EnableWindow(false);
			GetDlgItem(IDOK)->SetWindowText(_T("添加项目"));
			GetDlgItem(IDCANCEL)->SetWindowText(_T("取消添加"));

			GetDlgItem(IDC_VERSION_STATIC)->SetWindowText(_T("项目说明: "));
		}
		else
		{
			SetWindowText(_T("添加项目版本"));
			
			GetDlgItem(IDC_SHARE_COMBO)->EnableWindow(false);
			GetDlgItem(IDOK)->SetWindowText(_T("添加版本"));
			GetDlgItem(IDCANCEL)->SetWindowText(_T("取消添加"));

			GetDlgItem(IDC_PROJECT_STATIC)->SetWindowText(_T("修改说明: "));
			GetDlgItem(IDC_PATH_STATIC)   ->SetWindowText(_T("版本路径: "));
		}
	}
	else
	{
		if(IsProject)
		{
			SetWindowText(_T("云端项目编辑"));
			GetDlgItem(IDC_TYPE_COMBO)->EnableWindow(false);
			GetDlgItem(IDC_VERSION_STATIC)->SetWindowText(_T("项目说明: "));

			// 判断共享状态
			if(m_Share == _T("工作组"))
			{
				// 启动工作者线程
				if (m_hOperate == NULL)
				{
					Type = 0;
					GetDlgItem(IDOK)->EnableWindow(false);

					Parameter.Format(_T("id=%s"), ProjectId);
					m_hOperate = AfxBeginThread(Operate, this);
				}
			}
			else if(m_Share == _T("所有人") && OwnerId != UserId)
			{
				GetDlgItem(IDOK)->EnableWindow(false);
			}
		}
		else
		{
			SetWindowText(_T("项目版本编辑"));
			GetDlgItem(IDC_SHARE_COMBO)->EnableWindow(false);

			GetDlgItem(IDC_PROJECT_STATIC)->SetWindowText(_T("修改说明: "));
			GetDlgItem(IDC_PATH_STATIC)   ->SetWindowText(_T("版本路径: "));
		}

		GetDlgItem(IDC_PATH_EDIT)->EnableWindow(false);
		GetDlgItem(IDC_SELECT_BUTTON)->EnableWindow(false);

		// 显示数据
		UpdateData(FALSE);
	}

	// 设置 ComboBox 状态
	OnSelchangeShareCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CWebEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

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


// 工作线程
UINT CWebEditDlg::Operate(LPVOID pParam)
{
	// 窗口指针
	CWebEditDlg * pWnd = ((CWebEditDlg*)pParam);

	// 局部变量
	CString RecvData;
	BOOL IsSuccess;

	// 禁用按钮
	pWnd->GetDlgItem(IDOK)->EnableWindow(FALSE);

	// 获取服务器数据
	try
	{
		switch(pWnd->Type)
		{
		// 获得成员
		case 0: 
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/GetGroup"), 80, pWnd->Parameter, IsSuccess);

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


								// 分割数据
								CStringArray MemberArray, RoleArray, TypeArray;
								pWnd->Split(m_Member, _T(";"), MemberArray);
								pWnd->Split(m_Role, _T(";"),   RoleArray);
								pWnd->Split(m_Type, _T(";"),   TypeArray);

								// 循环插入数据
								for(int k=0; k<MemberArray.GetSize() -1; k++)
								{
									m_Member = MemberArray.GetAt(k);
									m_Role   = RoleArray.GetAt(k);
									m_Type   = TypeArray.GetAt(k);

									if(m_Role == _T("0"))
										m_Role = _T("参与者");
									else
										m_Role = _T("管理者");
									
									if(m_Type == _T("all"))
										m_Type = _T("全部版本");

									pWnd->m_Group.AddItem( m_Member, m_Role, m_Type );
								}
							}

							// 权限判断
							for(int p=0; p < pWnd->m_Group.GetItemCount(); p++)
							{
								CString Name = pWnd->m_Group.GetItemText(p, 0);
								CString Role = pWnd->m_Group.GetItemText(p, 1);

								// 不区分大小写匹配 (strcmp 区分大小写)
								if(_stricmp(Name, pWnd->UserName) == 0 && Role.Replace(_T("管理者"), _T("")))
									pWnd->GetDlgItem(IDOK)->EnableWindow();
							}

							// 判断拥有者
							if(pWnd->OwnerId == pWnd->UserId)
								pWnd->GetDlgItem(IDOK)->EnableWindow();
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


		// 新建项目
		case 1: 
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/CreateProject"), 80, pWnd->Parameter, IsSuccess);

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
					if( RecvData == RecvData.SpanIncluding( _T("0123456789") ) )
					{
						pWnd->ProjectId = RecvData;
						pWnd->PostMessage(WM_COMMAND, 100);
					}
					else if(RecvData == _T("used"))
					{
						pWnd->Error = _T("已存在相同的项目。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else if(RecvData == _T("shared"))
					{
						pWnd->Error = _T("已存在相同的共享项目。");
						pWnd->PostMessage(WM_COMMAND, 101);
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
			} break;


		// 编辑项目
		case 2: 
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/EditProject"), 80, pWnd->Parameter, IsSuccess);
			
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
					if( RecvData.Replace(_T("success"), _T("")) )
					{
						pWnd->PostMessage(WM_COMMAND, 100);
					}
					else if( RecvData.Replace(_T("error"), _T("")) )
					{
						pWnd->Error = _T("没有修改任何内容。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else
					{
						pWnd->Error = _T("服务器无响应，请稍后再试。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("无法连接到服务器, 请检查网络。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;


		// 新建版本
		case 3:
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/AddVersion"), 80, pWnd->Parameter, IsSuccess);
			
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
					if( RecvData.Replace(_T("success"), _T("")) )
					{
						pWnd->PostMessage(WM_COMMAND, 100);
					}
					else if( RecvData.Replace(_T("used"), _T("")) )
					{
						pWnd->Error = _T("已存在相同的版本。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else
					{
						pWnd->Error = _T("服务器无响应，请稍后再试。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("无法连接到服务器, 请检查网络。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;


		// 编辑版本
		case 4:
			RecvData = theApp.OnGetWebInfo(_T("Localhost"), _T("index/account/ModifyVersion"), 80, pWnd->Parameter, IsSuccess);
			
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
					if( RecvData.Replace(_T("success"), _T("")) )
					{
						pWnd->PostMessage(WM_COMMAND, 100);
					}
					else if( RecvData.Replace(_T("used"), _T("")) )
					{
						pWnd->Error = _T("已存在相同的版本。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else if( RecvData.Replace(_T("error"), _T("")) )
					{
						pWnd->Error = _T("没有进行任何修改。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
					else
					{
						pWnd->Error = _T("服务器无响应，请稍后再试。");
						pWnd->PostMessage(WM_COMMAND, 101);
					}
				}
				else
				{
					pWnd->Error = _T("无法连接到服务器, 请检查网络。");
					pWnd->PostMessage(WM_COMMAND, 101);
				}
			} break;
		}
	}
	catch (...)
	{
		pWnd->Error = _T("发生了异常，位于Operate的OnGetWebInfo方法。");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	// 激活按钮
	//pWnd->GetDlgItem(IDOK)->EnableWindow();

	// 对象置为空
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
	// 读取数据
	UpdateData();

	if(m_Project.IsEmpty())
	{
		AfxMessageBox(_T("项目名称不能为空!"));
	}
	else if(m_Version.IsEmpty())
	{
		AfxMessageBox(_T("项目版本不能为空!"));
	}
	else if(m_Path.IsEmpty() && IsNew)
	{
		AfxMessageBox(_T("项目路径不能为空!"));
	}
	else
	{
		if(IsNew)
		{
			if(IsProject)
			{
				// 启动工作者线程
				if (m_hOperate == NULL)
				{
					if(m_Share == _T("不共享"))
					{
						m_Share = _T("0");
						Parameter.Format(_T("id=%s&project=%s&explain=%s&share=%s"), UserId, m_Project, m_Version, m_Share);
					}
					else if(m_Share == _T("所有人"))
					{
						m_Share = _T("1");
						Parameter.Format(_T("id=%s&project=%s&explain=%s&share=%s"), UserId, m_Project, m_Version, m_Share);
					}
					else
					{
						m_Share = _T("2");

						// 得到共享成员
						int Count = m_Group.GetItemCount();
						if( Count <= 0 )
						{
							AfxMessageBox(_T("共享的成员数量不能为0"));
							return;
						}

						CString User, Role, Type;
						for(int i=0; i < Count; i++)
						{
							User += m_Group.GetItemText(i, 0) + _T(";");

							if(m_Group.GetItemText(i, 1) == _T("管理者")) 
								Role += _T("1;");
							else
							    Role += _T("0;");
							
							if(m_Group.GetItemText(i, 2) == _T("全部版本"))
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
				// 启动工作者线程
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
				// 启动工作者线程
				if (m_hOperate == NULL)
				{
					if(m_Share == _T("不共享"))
					{
						m_Share = _T("0");
						Parameter.Format(_T("project_id=%s&project=%s&explain=%s&share=%s"), ProjectId, m_Project, m_Version, m_Share);
					}
					else if(m_Share == _T("所有人"))
					{
						m_Share = _T("1");
						Parameter.Format(_T("project_id=%s&project=%s&explain=%s&share=%s"), ProjectId, m_Project, m_Version, m_Share);
					}
					else
					{
						m_Share = _T("2");

						// 得到共享成员
						int Count = m_Group.GetItemCount();
						if( Count <= 0 )
						{
							AfxMessageBox(_T("共享的成员数量不能为0"));
							return;
						}

						CString User, Role, Type;
						for(int i=0; i < Count; i++)
						{
							User += m_Group.GetItemText(i, 0) + _T(";");

							if(m_Group.GetItemText(i, 1) == _T("管理者")) 
								Role += _T("1;");
							else
								Role += _T("0;");

							if(m_Group.GetItemText(i, 2) == _T("全部版本"))
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
				// 启动工作者线程
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
	// 显示数据
	UpdateData(FALSE);

	// 设置状态
	OnSelchangeShareCombo();

	// 判断共享状态
	if(m_Share == _T("工作组"))
	{
		// 启动工作者线程
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

			// 显示选择路径
			GetDlgItem(IDC_PATH_EDIT)->SetWindowText(strFolderPath);

			// 设置焦点
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
			// 从列表中删除数据
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
			// 更新列表
			m_Group.SetItemText(m_Group.GetNextItem(-1, LVIS_SELECTED), 0, dlg.m_UserName);
			m_Group.SetItemText(m_Group.GetNextItem(-1, LVIS_SELECTED), 1, dlg.m_Role);
			m_Group.SetItemText(m_Group.GetNextItem(-1, LVIS_SELECTED), 2, dlg.m_Version);

			//设置高亮显示
			m_Group.SetFocus();//设置焦点  
			m_Group.SetItemState(m_Group.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
			m_Group.EnsureVisible(m_Group.GetNextItem(-1, LVIS_SELECTED), FALSE);//设置当前视图可见 
		}
	}
}


void CWebEditDlg::OnCancel()
{
	CDialogEx::OnCancel();
}
