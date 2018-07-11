// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "MainDlg.h"
#include "afxdialogex.h"

#include "SearchDlg.h"
#include "EditDlg.h"
#include "LoginDlg.h"
#include "ProjectDlg.h"


#define  VMPBEGIN \
    __asm _emit 0xEB \
    __asm _emit 0x10 \
    __asm _emit 0x56 \
    __asm _emit 0x4D \
    __asm _emit 0x50 \
    __asm _emit 0x72 \
    __asm _emit 0x6F \
    __asm _emit 0x74 \
    __asm _emit 0x65 \
    __asm _emit 0x63 \
    __asm _emit 0x74 \
    __asm _emit 0x20 \
    __asm _emit 0x62 \
    __asm _emit 0x65 \
    __asm _emit 0x67 \
    __asm _emit 0x69 \
    __asm _emit 0x6E \
    __asm _emit 0x00

#define  VMPEND \
  __asm _emit 0xEB \
    __asm _emit 0x0E \
    __asm _emit 0x56 \
    __asm _emit 0x4D \
    __asm _emit 0x50 \
    __asm _emit 0x72 \
    __asm _emit 0x6F \
    __asm _emit 0x74 \
    __asm _emit 0x65 \
    __asm _emit 0x63 \
    __asm _emit 0x74 \
    __asm _emit 0x20 \
    __asm _emit 0x65 \
    __asm _emit 0x6E \
    __asm _emit 0x64 \
    __asm _emit 0x00

// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Setting = new CSettingDlg();
}


CMainDlg::~CMainDlg()
{
	m_Setting->DestroyWindow();
	delete m_Setting;
}


void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CODE_LIST, m_List);
	DDX_Control(pDX, IDC_CLASS_COMBO, m_Class);
	DDX_Control(pDX, IDC_TYPE_COMBO, m_Type);
	DDX_Control(pDX, IDC_CODE_RICHEDIT, m_Edit);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &CMainDlg::OnSave)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, &CMainDlg::OnDelete)
	ON_BN_CLICKED(IDOK, &CMainDlg::OnOK)
	ON_BN_CLICKED(IDC_FONT_BUTTON, &CMainDlg::OnFont)
	ON_BN_CLICKED(IDC_NEW_BUTTON, &CMainDlg::OnNew)
	ON_BN_CLICKED(IDC_DIRECTORY_BUTTON, &CMainDlg::OnDirectory)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, &CMainDlg::OnRemove)

	ON_NOTIFY(NM_CLICK, IDC_CODE_LIST, &CMainDlg::OnClickCodeList)
	ON_NOTIFY(NM_RCLICK, IDC_CODE_LIST, &CMainDlg::OnRClickCodeList)

	ON_CBN_KILLFOCUS(IDC_CLASS_COMBO, &CMainDlg::OnKillfocusClassCombo)
	ON_CBN_KILLFOCUS(IDC_TYPE_COMBO, &CMainDlg::OnKillfocusTypeCombo)
	ON_CBN_DROPDOWN(IDC_CLASS_COMBO, &CMainDlg::OnDropdownClassCombo)
	ON_CBN_DROPDOWN(IDC_TYPE_COMBO, &CMainDlg::OnDropdownTypeCombo)

	ON_NOTIFY(NM_DBLCLK, IDC_CODE_LIST, &CMainDlg::OnDblclkCodeList)
	
	ON_NOTIFY(EN_LINK,IDC_CODE_RICHEDIT, OnRichEditLink) 
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CODE_LIST, &CMainDlg::OnItemchangedCodeList)

	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CMainDlg::OnEdit)
	ON_BN_CLICKED(IDC_SYNCHRONIZATION_BUTTON, &CMainDlg::OnSynchronization)
	ON_BN_CLICKED(IDC_SETTING_BUTTON, &CMainDlg::OnSetting)

	ON_MESSAGE(WM_CHILDMESSAGE, OnMessageChild)
	ON_EN_CHANGE(IDC_CODE_RICHEDIT, &CMainDlg::OnChangeCodeRichedit)
	ON_BN_CLICKED(IDC_MANAGER_BUTTON, &CMainDlg::OnManager)
	
END_MESSAGE_MAP()


// CMainDlg 消息处理程序


BOOL CMainDlg::OnInitDialog()
{
	//vmprotect 标记开始处.
	VMPBEGIN

	CDialogEx::OnInitDialog();

	// 设置图标
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// List
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT  //允许整行选中
		| LVS_EX_HEADERDRAGDROP				      //允许整列拖动
		| LVS_EX_SUBITEMIMAGES			          //单击选中项
		| LVS_EX_GRIDLINES);				      //画出网格线

	m_List.SetHeadings(_T("方法名称,165;"));
	m_List.LoadColumnInfo();

	// 字体
	LOGFONT   logfont;//最好弄成类成员,全局变量,静态成员  
    CFont   *pfont1   =   m_List.GetFont();  
    pfont1->GetLogFont(   &logfont   );  
    logfont.lfHeight  = (LONG)(logfont.lfHeight * 1.3);   //这里可以修改字体的高比例
    logfont.lfWidth   = (LONG)(logfont.lfWidth  * 1.3);   //这里可以修改字体的宽比例
    static   CFont   font1;
    font1.CreateFontIndirect(&logfont);
    m_List.SetFont(&font1);  
    font1.Detach();

	// Edit
	m_Edit.SubclassDlgItem(IDC_CODE_RICHEDIT, this);

	// 设置 ENM_CHANGE (响应Change事件)
	//EM_SETEVENTMASK
	long lMask = m_Edit.GetEventMask();
	lMask |= ENM_CHANGE;
	lMask &= ~ENM_PROTECTED;
	m_Edit.SetEventMask(lMask);


	// 格式
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;

	cf.dwEffects&=~CFE_BOLD;      //设置粗体，取消用cf.dwEffects&=~CFE_BOLD;
	cf.dwEffects&=~CFE_ITALIC;    //设置斜体，取消用cf.dwEffects&=~CFE_ITALIC;
	cf.dwEffects&=~CFE_UNDERLINE; //设置斜体，取消用cf.dwEffects&=~CFE_UNDERLINE;
	cf.crTextColor = RGB(0,0,0);  //设置颜色
	cf.yHeight = 14 * 14;         //设置高度
	strcpy_s(cf.szFaceName, 1024 ,_T("宋体"));//设置字体
	m_Edit.SetDefaultCharFormat(cf);

	::SendMessage(m_Edit, EM_SETLANGOPTIONS, 0, 0);

	// 超链接
	DWORD mask =::SendMessage(m_Edit.m_hWnd,EM_GETEVENTMASK, 0, 0);  
    mask = mask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS |ENM_KEYEVENTS;  
    ::SendMessage(m_Edit.m_hWnd,EM_SETEVENTMASK, 0, mask);  
    ::SendMessage(m_Edit.m_hWnd,EM_AUTOURLDETECT, true, 0);

	// 目录检测
	BOOL IsNew = false;
	CString FilePath = _T("File");
	DWORD Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		IsNew = true;
		CreateDirectory(FilePath, NULL);
	}

	FilePath = _T("Project");
	Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		IsNew = true;
		CreateDirectory(FilePath, NULL);
	}

	FilePath = _T("Code");
	Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		IsNew = true;
		CreateDirectory(FilePath, NULL);
	}

	// 判断是否第一次运行
	if(IsNew)
	{
		// 显示用户使用说明
		m_Edit.StreamInFromResource(_T("使用说明.rtf"), _T("SF_RTF"));
	}

	// Class
	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			m_Class.AddString(Finder.GetFileName());
	}

	m_Class.SetCurSel(0);

	// Type
	CString Class, Type;
	m_Class.GetWindowText(Class);
	FilePath = _T("Code\\") + Class;

	IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			m_Type.AddString(Finder.GetFileName());
	}

	m_Type.SetCurSel(0);


	// Code
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);
	FilePath = _T("Code\\") + Class + _T("\\") + Type;

	IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			continue;
		else
		{
			CString Name = Finder.GetFileName();
			m_List.AddItem(Name.Left(Name.GetLength() - 4));
		}
	}

	// 设置对话框
	m_Setting->Create(IDD_SETTING_DIALOG, this);

	// 自动打开
	if( GetPrivateProfileInt(_T("Setting"), _T("Open"), 0, _T("./Setting.ini")) == 1)
	{
		// 读取最后关闭文件
		CString Class, Type, Name;

		::GetPrivateProfileString(_T("File"), _T("Class"), _T(""), Class.GetBuffer(MAX_PATH), MAX_PATH, _T("./Setting.ini"));
		::GetPrivateProfileString(_T("File"), _T("Type"),  _T(""), Type.GetBuffer(MAX_PATH),  MAX_PATH, _T("./Setting.ini"));
		::GetPrivateProfileString(_T("File"), _T("Name"),  _T(""), Name.GetBuffer(MAX_PATH),  MAX_PATH, _T("./Setting.ini"));

		Class.ReleaseBuffer();
		Type.ReleaseBuffer();
		Name.ReleaseBuffer();

		// 自动打开文件
		int nIndex = m_Class.FindStringExact(0, Class);
		if(nIndex != CB_ERR)
		{
			m_Class.SetCurSel(nIndex);
			OnKillfocusClassCombo();
		}
		else
		{
			AfxMessageBox(_T("找不到") + Class + _T("类型"));
		}

		nIndex = m_Type.FindStringExact(0, Type);
		if(nIndex != CB_ERR)
		{
			m_Type.SetCurSel(nIndex);
			OnKillfocusTypeCombo();
		}
		else
		{
			AfxMessageBox(_T("找不到") + Type + _T("类别"));
		}

		int Count = m_List.GetItemCount();
		for(int i=0; i<Count; i++)
		{
			CString Function = m_List.GetItemText(i, 0);

			if(Function == Name)
			{
				//设置高亮显示  
				m_List.SetFocus();//设置焦点  
				m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
				m_List.EnsureVisible(i, FALSE);//设置当前视图可见 
				break;
			}
		}
	}

	//vmprotect 标记结束处.
	VMPEND

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	VMPBEGIN

	UINT  nKeyCode = pMsg->wParam; // virtual key code of the key pressed
	if (pMsg->message == WM_KEYDOWN)
	{
		// Ctrl + C (复制)
		if ( (nKeyCode == _T('C') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			CString   source = m_Edit.GetSelText();     
			//put   your   text   in   source   
			if(OpenClipboard())   
			{   
				HGLOBAL   clipbuffer;   
				char   *   buffer;   
				EmptyClipboard();   
				clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
				buffer   =   (char*)GlobalLock(clipbuffer);
				strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
				GlobalUnlock(clipbuffer);   
				SetClipboardData(CF_TEXT,clipbuffer);   
				CloseClipboard();   
			}

			return true;
		}

		// Ctrl + V (粘贴)
		if ( (nKeyCode == _T('V') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			if( GetPrivateProfileInt(_T("Setting"), _T("Clear"), 0, _T("./Setting.ini")) == 1)
			{
				m_Edit.PasteSpecial(CF_TEXT);
				return true;
			}
		}

		// Ctrl + X (剪切)
		if ( (nKeyCode == _T('X') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			CString   source = m_Edit.GetSelText();     
			//put   your   text   in   source   
			if(OpenClipboard())   
			{   
				HGLOBAL   clipbuffer;   
				char   *   buffer;   
				EmptyClipboard();   
				clipbuffer   =   GlobalAlloc(GMEM_DDESHARE,   source.GetLength() +1);
				buffer   =   (char*)GlobalLock(clipbuffer);
				strcpy_s(buffer, 65535 ,LPCSTR((CStringA)source));
				GlobalUnlock(clipbuffer);   
				SetClipboardData(CF_TEXT,clipbuffer);   
				CloseClipboard();   
			}

			// 剪切
			m_Edit.Cut();

			return true;
		}

		// Ctrl + S (保存方法)
		if ( (nKeyCode == _T('S') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnOK();
			return true;
		}

		// Ctrl + N (添加方法)
		if ( (nKeyCode == _T('N') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnNew();
			return true;
		}

		// Ctrl + D (依赖文件)
		if ( (nKeyCode == _T('D') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnDirectory();
			return true;
		}

		// Ctrl + F (编辑字体)
		if ( (nKeyCode == _T('F') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnFont();
			return true;
		}

		// Ctrl + R (移除方法)
		if ( (nKeyCode == _T('R') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnRemove();
			return true;
		}

		// Ctrl + Q (清除格式)
		if( (nKeyCode == _T('Q') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnClearFormat();
			return true;
		}

		// Ctrl + W (编辑方法)
		if( (nKeyCode == _T('W') && (::GetKeyState(VK_CONTROL) & 0x8000) ) )
		{
			OnEditFunction();
			return true;
		}

		// 空格 取消选择
		if(nKeyCode == VK_SPACE)
		{
			if (GetDlgItem(IDC_CODE_RICHEDIT) == GetFocus())
			{
				// 不拦截回车换行
			}
			else if (GetDlgItem(IDC_CODE_LIST) == GetFocus())
			{
				// 取消选择
				m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);
				return true;
			}
		}

		if(nKeyCode == VK_RETURN)
		{
			//焦点是否在控件上
			if (GetDlgItem(IDC_CODE_LIST) == GetFocus())
			{
				CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
				if (!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
				{
					CString Class, Type;
					m_Class.GetWindowText(Class);
					m_Type.GetWindowText(Type);
					CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

					ShellExecute(NULL, _T("open"), FilePath + Name + _T(".rtf"), NULL, NULL, SW_SHOWNORMAL);

					// 屏蔽回车消息
					return true;
				}
			}
			else if (GetDlgItem(IDC_CODE_RICHEDIT) == GetFocus())
			{
				// 不拦截回车换行
			}
			else
			{
				// 屏蔽回车消息
				return true;
			}
		}
	}

	VMPEND
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMainDlg::OnRichEditLink(NMHDR*in_pNotifyHeader, LRESULT* out_pResult )  
{
	VMPBEGIN

    ENLINK* l_pENLink =(ENLINK*)in_pNotifyHeader;
    *out_pResult = 0;
    switch(l_pENLink->msg)
    {
    case WM_LBUTTONDOWN:
        {
            CString strURL;
            CHARRANGE crCharRange ;
            CRichEditCtrl * pTempEdit;
            pTempEdit =(CRichEditCtrl*)CRichEditCtrl::FromHandle(l_pENLink->nmhdr.hwndFrom);
            pTempEdit->GetSel(crCharRange );
            pTempEdit->SetSel(l_pENLink->chrg);

            strURL = pTempEdit->GetSelText();
            pTempEdit->SetSel(crCharRange);
            CWaitCursor l_WaitCursor;
            ShellExecute( this->GetSafeHwnd(), _T( "open"),strURL, NULL, NULL, SW_SHOWNORMAL );
            *out_pResult = 1;
        }
        break;
    case WM_LBUTTONUP:
        {
            *out_pResult = 1;
        }
        break ;
    }

	VMPEND
}


// 子窗口消息
LRESULT CMainDlg::OnMessageChild(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case 1:
		Refresh();
		break;

	case 2:
		OnHelp();
		break;
	}

	return TRUE;
}


void CMainDlg::DeleteDirectory(CString Directory)   //删除一个文件夹下的所有内容  
{
	VMPBEGIN

	if(Directory.IsEmpty())   
	{ 
		RemoveDirectory(Directory); 
		return; 
	} 

	//首先删除文件及子文件夹 
	CFileFind   ff; 

	BOOL bFound = ff.FindFile(Directory + _T("\\*"),0); 
	while(bFound) 
	{ 
		bFound = ff.FindNextFile(); 
		if(ff.GetFileName()== _T(".")||ff.GetFileName()== _T("..")) 
			continue; 

		//去掉文件(夹)只读等属性 
		SetFileAttributes(ff.GetFilePath(),FILE_ATTRIBUTE_NORMAL); 

		if(ff.IsDirectory())  
		{   
			//递归删除子文件夹 
			DeleteDirectory(ff.GetFilePath()); 
			RemoveDirectory(ff.GetFilePath());
		} 
		else   
		{ 
			DeleteFile(ff.GetFilePath());   //删除文件 
		} 
	} 

	ff.Close(); 

	//然后删除该文件夹 
	RemoveDirectory(Directory);

	VMPEND
} 


void CMainDlg::Refresh()
{
	// 刷新窗口
	Invalidate();

	//设置高亮显示  
	m_List.SetFocus();//设置焦点  

	if(m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		m_List.SetItemState( m_List.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态
		m_List.EnsureVisible(m_List.GetItemCount() -1, FALSE);//设置当前视图可见
		m_List.EnsureVisible(m_List.GetNextItem(-1, LVIS_SELECTED), FALSE);//设置当前视图可见
	}
	else
	{
		m_List.EnsureVisible(m_List.GetItemCount() -1, FALSE);//设置当前视图可见
		m_List.EnsureVisible(0, FALSE);//设置当前视图可见
	}
}


void CMainDlg::OnHelp()
{
	// 显示用户使用说明
	m_Edit.StreamInFromResource(_T("使用说明.rtf"), _T("SF_RTF"));
}


void CMainDlg::OnChangeCodeRichedit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 方法并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	VMPBEGIN

	// 若设置了自动保存
	if ( GetPrivateProfileInt(_T("Setting"), _T("Save"), 0, _T("./Setting.ini")) == 1)
	{
		CString Path = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
		CString Class, Type;
		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);

		// 如果分类信息不完整
		if(Class.IsEmpty() || Type.IsEmpty())
		{
			return;
		}
		else
		{
			// 已有源码
			if(!Path.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
			{
				// 目录检测
				DWORD DirPath = GetFileAttributes(_T("Code\\") + Class);
				if (DirPath == 0xFFFFFFFF)     //文件夹不存在
				{
					CreateDirectory(_T("Code\\") + Class, NULL);
				}

				DirPath = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
				if (DirPath == 0xFFFFFFFF)     //文件夹不存在
				{
					CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
				}

				// 自动保存
				CString File = _T("Code\\") + Class + _T("\\") + Type + _T("\\");
				m_Edit.StreamOutToFile(File + Path + _T(".rtf"), _T("SF_RTF"));
			}
		}
	}

	VMPEND
}


void CMainDlg::OnClickCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	//设置高亮显示  
	m_List.SetFocus();//设置焦点  
	m_List.SetItemState( m_List.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
	m_List.EnsureVisible(m_List.GetNextItem(-1, LVIS_SELECTED), FALSE);//设置当前视图可见 

	CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
	if (!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString Class, Type;
		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);
		CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

		m_Edit.StreamInFromResource(FilePath + Name + _T(".rtf"), _T("SF_RTF"));
	}

	*pResult = 0;

	VMPEND
}


void CMainDlg::OnRClickCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	// 取消选择
	m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);
	CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

	CSearchDlg dlg;
	dlg.FilePath = FilePath;
	if(dlg.DoModal() == IDOK)
	{
		CString Function = dlg.Function;

		int Count = m_List.GetItemCount();
		for(int i=0; i<Count; i++)
		{
			CString Name = m_List.GetItemText(i, 0);

			if(Name == Function)
			{
				//设置高亮显示  
				m_List.SetFocus();//设置焦点  
				m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
				m_List.EnsureVisible(i, FALSE);//设置当前视图可见 
				break;
			}
		}

		//std::map<CString, MetaData>::iterator iter;  
		//if ((iter = m_idBufferMap.find(Function)) != m_idBufferMap.end())  
		//{
		//	int currentIndex = iter->second.m_index;  

		//	//设置高亮显示  
		//	m_List.SetFocus();//设置焦点  
		//	m_List.SetItemState(currentIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
		//	m_List.EnsureVisible(currentIndex, FALSE);//设置当前视图可见  
		//}
		//else  
		//{  
		//	AfxMessageBox("未找到");  
		//}  
	}

	*pResult = 0;

	VMPEND
}


void CMainDlg::OnDblclkCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
	if (!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CString Class, Type;
		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);
		CString FilePath = _T("Code\\") + Class + _T("\\") + Type + _T("\\");

		ShellExecute(NULL, _T("open"), FilePath + Name + _T(".rtf"), NULL, NULL, SW_SHOWNORMAL);
	}

	VMPEND

	*pResult = 0;
}


void CMainDlg::OnItemchangedCodeList(NMHDR *pNMHDR, LRESULT *pResult)
{
	VMPBEGIN
	//OnClickCodeList(pNMHDR, pResult);
	VMPEND
}


void CMainDlg::OnSave()
{
	VMPBEGIN

	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	if(Class.IsEmpty())
	{
		AfxMessageBox(_T("请输入类型!"));
		return;
	}
	else if(Type.IsEmpty())
	{
		AfxMessageBox(_T("请输入类别!"));
		return;
	}

	DWORD Path = GetFileAttributes(_T("Code\\") + Class);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		CreateDirectory(_T("Code\\") + Class, NULL);
	}

	Path = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
	}

	MessageBox(_T("成功创建分类"));

	VMPEND
}


void CMainDlg::OnDelete()
{
	VMPBEGIN

	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	if(Class.IsEmpty())
	{
		AfxMessageBox(_T("请输入类型!"));
		return;
	}
	else if(!Class.IsEmpty() && Type.IsEmpty())
	{
		// 删除目录
		DeleteDirectory(_T("Code\\") + Class);
		DeleteDirectory(_T("File\\") + Class);

		// 删除ComboBox 里的值
		m_Class.DeleteString(m_Class.GetCurSel());
		m_Class.SetCurSel(0);

		// 内容变更消息
		OnKillfocusClassCombo();
		OnKillfocusTypeCombo();

		// 清空编辑框
		m_Edit.SetWindowText(_T(""));

		// 消息提示
		MessageBox(_T("删除完毕!"));
	}
	else if(!Class.IsEmpty() && !Type.IsEmpty())
	{
		int code = MessageBox(_T("是否删除类型? 选否只删除该类别。"), NULL, MB_YESNOCANCEL);

		if(code != IDCANCEL)
		{
			if(code == IDYES)
			{
				// 删除目录
				DeleteDirectory(_T("Code\\") + Class);
				DeleteDirectory(_T("File\\") + Class);

				// 删除ComboBox 里的值
				m_Class.DeleteString(m_Class.GetCurSel());
				m_Type.DeleteString(m_Type.GetCurSel());

				// 设置选项
				m_Class.SetCurSel(0);

				// 内容变更消息
				OnKillfocusClassCombo();
				OnKillfocusTypeCombo();

				// 清空编辑框
				m_Edit.SetWindowText(_T(""));

				// 消息提示
				MessageBox(_T("删除完毕!"));
			}
			else
			{
				// 删除目录
				DeleteDirectory(_T("Code\\") + Class + _T("\\") + Type);
				DeleteDirectory(_T("File\\") + Class + _T("\\") + Type);

				// 删除ComboBox 里的值
				m_Type.DeleteString(m_Type.GetCurSel());

				// 设置选项
				m_Type.SetCurSel(0);

				// 内容变更消息
				OnKillfocusTypeCombo();

				// 清空编辑框
				m_Edit.SetWindowText(_T(""));

				// 消息提示
				MessageBox(_T("删除完毕!"));
			}
		}
	}

	VMPEND
}


void CMainDlg::OnEdit()
{
	CString Class,Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	CEditDlg dlg;
	dlg.Class = Class;
	dlg.Type  = Type;
	dlg.IsFunction = false;

	if(dlg.DoModal() == IDOK)
	{
		// 如果只修改了类别
		if(dlg.Class == Class && dlg.Type != Type)
		{
			CString old_Path = _T("Code\\") + Class + _T("\\") + Type;
			CString new_Path = _T("Code\\") + Class + _T("\\") + dlg.Type;

			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				m_Type.SetWindowText(dlg.Type);
				MessageBox(_T("分类修改成功!"));
			}
			else
			{
				AfxMessageBox(_T("分类修改失败!"));
			}
		}

		// 如果只修改了类型
		else if(dlg.Class != Class && dlg.Type == Type)
		{
			CString old_Path = _T("Code\\") + Class;
			CString new_Path = _T("Code\\") + dlg.Class;

			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				m_Class.SetWindowText(dlg.Class);
				MessageBox(_T("分类修改成功!"));
			}
			else
			{
				AfxMessageBox(_T("分类修改失败!"));
			}
		}

		// 两种都改了
		else
		{
			CString old_Class = _T("Code\\") + Class;
			CString new_Class = _T("Code\\") + dlg.Class;

			CString old_Type = _T("Code\\") + dlg.Class + _T("\\") + Type;
			CString new_Type = _T("Code\\") + dlg.Class + _T("\\") + dlg.Type;

			if(MoveFileEx(old_Class, new_Class, MOVEFILE_REPLACE_EXISTING) && MoveFileEx(old_Type, new_Type, MOVEFILE_REPLACE_EXISTING))
			{
				m_Class.SetWindowText(dlg.Class);
				m_Type.SetWindowText(dlg.Type);
				MessageBox(_T("分类修改成功!"));
			}
			else
			{
				AfxMessageBox(_T("分类修改失败!"));
			}
		}
	}
}


void CMainDlg::OnManager()
{
	DWORD Path = GetFileAttributes(_T("Project"));
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		CreateDirectory(_T("Project"), NULL);
	}

	CProjectDlg dlg;
	dlg.DoModal();
}


void CMainDlg::OnKillfocusClassCombo()
{
	VMPBEGIN

	// 清空ComboBox
	m_Type.ResetContent();
	
	CString Class;
	m_Class.GetWindowText(Class);
	CString FilePath = _T("Code\\") + Class;

	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			m_Type.AddString(Finder.GetFileName());
	}

	// 设置ComboBox
	m_Type.SetCurSel(0);

	VMPEND
}


void CMainDlg::OnDropdownClassCombo()
{
	VMPBEGIN

	// 清空编辑框
	m_Edit.SetWindowText(_T(""));

	// 清空列表
	m_List.DeleteAllItems();

	// 重置ComboBox
	m_Class.ResetContent();
	m_Type.ResetContent();

	CString FilePath = _T("Code");
	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			m_Class.AddString(Finder.GetFileName());
	}

	m_Class.SetCurSel(0);

	VMPEND
}


void CMainDlg::OnKillfocusTypeCombo()
{
	VMPBEGIN

	// 清空编辑框
	//m_Edit.SetWindowText(_T(""));

	// 清空列表
	m_List.DeleteAllItems();

	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);
	CString FilePath = _T("Code\\") + Class + _T("\\") + Type;

	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			continue;
		else
		{
			CString Name = Finder.GetFileName();
			m_List.AddItem(Name.Left(Name.GetLength() - 4));
		}
	}

	VMPEND
}


void CMainDlg::OnDropdownTypeCombo()
{
	VMPBEGIN

	// 清空编辑框
	m_Edit.SetWindowText(_T(""));

	// 清空列表
	m_List.DeleteAllItems();

	// 重置ComboBox
	m_Type.ResetContent();
	
	CString Class, Type;
	m_Class.GetWindowText(Class);
	CString FilePath = _T("Code\\") + Class;

	CFileFind Finder;
	BOOL IsFind = Finder.FindFile(FilePath + _T("./*.*"));
	while (IsFind)
	{
		IsFind = Finder.FindNextFile();

		if (Finder.IsDots())
			continue;
		if (Finder.IsDirectory())
			m_Type.AddString(Finder.GetFileName());
	}

	m_Type.SetCurSel(0);

	VMPEND
}


void CMainDlg::OnEditFunction()
{
	CString Class, Type, Function;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);
	Function = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);

	// 判断选中方法
	if (!Function.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		CEditDlg dlg;
		dlg.Type  = Function;
		dlg.IsFunction = true;

		if(dlg.DoModal() == IDOK)
		{
			CString old_Path = _T("Code\\") + Class + _T("\\") + Type + _T("\\") + Function + _T(".rtf");
			CString new_Path = _T("Code\\") + Class + _T("\\") + Type + _T("\\") + dlg.Type + _T(".rtf");

			if(MoveFileEx(old_Path, new_Path, MOVEFILE_REPLACE_EXISTING))
			{
				int Row = m_List.GetNextItem(-1, LVIS_SELECTED);
				m_List.SetItemText(Row, 0, dlg.Type);

				//设置高亮显示
				m_List.SetFocus();//设置焦点  
				m_List.SetItemState(Row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
				m_List.EnsureVisible(Row, FALSE);//设置当前视图可见 

				MessageBox(_T("方法修改成功!"));
			}
			else
			{
				AfxMessageBox(_T("方法修改失败!"));
			}
		}
	}
	else
	{
		AfxMessageBox(_T("请选择要编辑的方法!"));
	}
}


void CMainDlg::OnOK()
{
	VMPBEGIN

	CString Path = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
	CString Class, Type;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	if(Class.IsEmpty())
	{
		AfxMessageBox(_T("请输入类型!"));
		return;
	}
	else if(Type.IsEmpty())
	{
		AfxMessageBox(_T("请输入类别!"));
		return;
	}

	// 目录检测
	DWORD DirPath = GetFileAttributes(_T("Code\\") + Class);
	if (DirPath == 0xFFFFFFFF)     //文件夹不存在
	{
		CreateDirectory(_T("Code\\") + Class, NULL);
	}

	DirPath = GetFileAttributes(_T("Code\\") + Class + _T("\\") + Type);
	if (DirPath == 0xFFFFFFFF)     //文件夹不存在
	{
		CreateDirectory(_T("Code\\") + Class + _T("\\") + Type, NULL);
	}

	// 判断是否是新方法
	CString File = _T("Code\\") + Class + _T("\\") + Type + _T("\\");
	if(!Path.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		m_Edit.StreamOutToFile(File + Path + _T(".rtf"), _T("SF_RTF"));
		MessageBox(_T("保存成功!"));

		//设置高亮显示  
		m_List.SetFocus();//设置焦点  
		m_List.SetItemState( m_List.GetNextItem(-1, LVIS_SELECTED), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
		m_List.EnsureVisible(m_List.GetNextItem(-1, LVIS_SELECTED), FALSE);//设置当前视图可见 
	}
	else
	{
		TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
		GetModuleFileName(NULL,exeFullPath,MAX_PATH);//得到程序模块名称，全路径
		CString Path(exeFullPath), Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1), Dir = Path.Left(Path.GetLength() - Name.GetLength()) + File;

		CFileDialog FileDlg(TRUE, _T("rtf"), _T("方法名称"), OFN_NOCHANGEDIR | OFN_ENABLEHOOK | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, _T("方法文件(*.rtf)|*.rtf;||"), this);
		FileDlg.m_ofn.lpstrInitialDir = Dir;
		if(FileDlg.DoModal() == IDOK)
		{
			CString strFile = FileDlg.GetPathName(); //文件不存在就自动创建
			m_Edit.StreamOutToFile(strFile, _T("SF_RTF"));
			MessageBox(_T("保存成功!"));

			BOOL  isFind = FALSE;
			CString Name = FileDlg.GetFileName();

			int Count = m_List.GetItemCount();
			for(int i=0; i<Count; i++)
			{
				CString Function = m_List.GetItemText(i, 0);

				if(Function == Name.Left(Name.GetLength() - 4))
				{
					//标志
					isFind = TRUE;

					//设置高亮显示  
					m_List.SetFocus();//设置焦点  
					m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
					m_List.EnsureVisible(i, FALSE);//设置当前视图可见 
					break;
				}
			}

			if(!isFind)
			{
				// 加入列表
				int i = m_List.AddItem(Name.Left(Name.GetLength() - 4));

				//设置高亮显示  
				m_List.SetFocus();//设置焦点  
				m_List.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//设置状态  
				m_List.EnsureVisible(i, FALSE);//设置当前视图可见 
			}
		}
	}

	VMPEND
}


void CMainDlg::OnFont()
{
	VMPBEGIN

	CHARFORMAT cf;
	LOGFONT lf;
	memset(&cf, 0, sizeof(CHARFORMAT));
	memset(&lf, 0, sizeof(LOGFONT)); 

	//判断是否选择了内容
	BOOL bSelect = (m_Edit.GetSelectionType() != SEL_EMPTY) ? TRUE : FALSE; 
	if (bSelect)
	{
		m_Edit.GetSelectionCharFormat(cf);
	}
	else
	{
		m_Edit.GetDefaultCharFormat(cf);
	}

	//得到相关字体属性
	BOOL bIsBold = cf.dwEffects & CFE_BOLD;
	BOOL bIsItalic = cf.dwEffects & CFE_ITALIC;
	BOOL bIsUnderline = cf.dwEffects & CFE_UNDERLINE;
	BOOL bIsStrickout = cf.dwEffects & CFE_STRIKEOUT;

	//设置属性
	lf.lfCharSet = cf.bCharSet;
	lf.lfHeight = cf.yHeight/15;
	lf.lfPitchAndFamily = cf.bPitchAndFamily;
	lf.lfItalic = bIsItalic;
	lf.lfWeight = (bIsBold ? FW_BOLD : FW_NORMAL);
	lf.lfUnderline = bIsUnderline;
	lf.lfStrikeOut = bIsStrickout;

	//sprintf_s(lf.lfFaceName, 65535, cf.szFaceName);
	/*asci：strcpy(m_NotifyData.szTip, "认证系统客户端");
	unicode：wcscpy(m_NotifyData.szTip, L"认证系统客户端");*/

	//wcscpy_s(lf.lfFaceName, cf.szFaceName);
	strcpy_s(lf.lfFaceName, cf.szFaceName);

	CFontDialog dlg(&lf);
	dlg.m_cf.rgbColors = cf.crTextColor;

	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCharFormat(cf);//获得所选字体的属性
		if (bSelect)
			m_Edit.SetSelectionCharFormat(cf);    //为选定的内容设定所选字体
		else
			m_Edit.SetWordCharFormat(cf);         //为将要输入的内容设定字体
	}

	VMPEND
}


void CMainDlg::OnNew()
{
	// 取消选择
	m_List.SetItemState(m_List.GetNextItem(-1, LVIS_SELECTED), 0, LVIS_SELECTED | LVIS_FOCUSED);

	// 清空内容
	m_Edit.SetWindowText(_T(""));

	// 添加方法
	OnOK();
}


void CMainDlg::OnClearFormat()
{
	CString source = m_Edit.GetSelText();
	if(!source.IsEmpty())
	{
		//CHARFORMAT cf;  
		//m_Edit.GetSelectionCharFormat(cf);  
		//cf.dwMask|=CFM_BOLD;  
		//cf.dwEffects|=CFE_BOLD;//设置粗体，取消用cf.dwEffects&=~CFE_BOLD;  
		//cf.dwMask|=CFM_ITALIC;  
		//cf.dwEffects|=CFE_ITALIC;//设置斜体，取消用cf.dwEffects&=~CFE_ITALIC;  
		//cf.dwMask|=CFM_UNDERLINE;  
		//cf.dwEffects|=CFE_UNDERLINE;//设置斜体，取消用cf.dwEffects&=~CFE_UNDERLINE;  
		//cf.dwMask|=CFM_COLOR;  
		//cf.crTextColor = RGB(0,0,0);//设置颜色  
		//cf.dwMask|=CFM_SIZE;  
		//cf.yHeight =200;//设置高度  
		//cf.dwMask|=CFM_FACE;  
		//strcpy_s(cf.szFaceName ,_T("宋体"));//设置字体  
		//m_Edit.SetSelectionCharFormat(cf);  

		CHARFORMAT cf;
		m_Edit.GetSelectionCharFormat(cf);
		cf.dwMask|=CFM_BOLD;
		cf.dwEffects&=~CFE_BOLD;      //设置粗体，取消用cf.dwEffects&=~CFE_BOLD;
		cf.dwMask|=CFM_ITALIC;
		cf.dwEffects&=~CFE_ITALIC;    //设置斜体，取消用cf.dwEffects&=~CFE_ITALIC;
		cf.dwMask|=CFM_UNDERLINE;
		cf.dwEffects&=~CFE_UNDERLINE; //设置斜体，取消用cf.dwEffects&=~CFE_UNDERLINE;
		cf.dwMask|=CFM_COLOR;
		cf.crTextColor = RGB(0,0,0);  //设置颜色
		cf.dwMask|=CFM_SIZE;
		cf.yHeight = 14 * 14;         //设置高度
		cf.dwMask|=CFM_FACE;
		strcpy_s(cf.szFaceName ,_T("宋体"));//设置字体
		m_Edit.SetSelectionCharFormat(cf);
	}
	else
	{
		AfxMessageBox(_T("没有需要清除格式的内容!"));
	}
	
}


void CMainDlg::OnDirectory()
{
	VMPBEGIN

	CString Class, Type, Dir;
	m_Class.GetWindowText(Class);
	m_Type.GetWindowText(Type);

	if(Class.IsEmpty())
	{
		AfxMessageBox(_T("请输入类型!"));
		return;
	}
	else if(Type.IsEmpty())
	{
		AfxMessageBox(_T("请输入类别!"));
		return;
	}

	CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);
	if (!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		DWORD Path = GetFileAttributes(_T("File\\") + Class);
		if (Path == 0xFFFFFFFF)     //文件夹不存在
		{
			CreateDirectory(_T("File\\") + Class, NULL);
		}

		Path = GetFileAttributes(_T("File\\") + Class + _T("\\") + Type);
		if (Path == 0xFFFFFFFF)     //文件夹不存在
		{
			CreateDirectory(_T("File\\") + Class + _T("\\") + Type, NULL);
		}

		Dir  = _T("File\\") + Class + _T("\\") + Type + _T("\\") + Name;
		Path = GetFileAttributes(Dir);

		if (Path == 0xFFFFFFFF)     //文件夹不存在
		{
			CreateDirectory(_T("File\\") + Class + _T("\\") + Type + _T("\\") + Name, NULL);
		}

		ShellExecute(NULL, _T("open"), Dir, NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		AfxMessageBox(_T("请选择方法后再执行本操作!"));
	}

	VMPEND
}


void CMainDlg::OnRemove()
{
	VMPBEGIN

	int Count = m_List.GetSelectedCount();
	CString Path = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0);

	if (!Path.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
	{
		if(Count == 1)
		{
			if (!Path.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
			{
				if(MessageBox(_T("是否要移除当前方法? "), NULL, MB_YESNO) == IDYES)
				{
					CString Class, Type;
					m_Class.GetWindowText(Class);
					m_Type.GetWindowText(Type);

					DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".rtf"));

					m_List.DeleteItem(m_List.GetNextItem(-1, LVIS_SELECTED));
					m_Edit.SetWindowText(_T(""));

					MessageBox(_T("方法移除完毕!"));
				}
			}
			else
			{
				AfxMessageBox(_T("请选择要移除的方法!"));
			}
		}
		else if(Count > 1)
		{
			if(MessageBox(_T("是否要移除选中的方法? "), NULL, MB_YESNO) == IDYES)
			{
				int nItem = -1;
				POSITION pos;
				while (pos = m_List.GetFirstSelectedItemPosition())
				{
					nItem = -1;
					nItem = m_List.GetNextSelectedItem(pos);
					if (nItem >= 0 && m_List.GetSelectedCount() > 0)
					{
						CString Path = m_List.GetItemText(nItem, 0);

						CString Class, Type;
						m_Class.GetWindowText(Class);
						m_Type.GetWindowText(Type);

						DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".rtf"));

						m_List.DeleteItem(nItem);
					}
				}

				//for (int i = 0; i < Count; i++)
				//{
				//	int Row = m_List.GetNextItem(i - 1, LVIS_SELECTED);
				//	CString Path = m_List.GetItemText(Row, 0);

				//	CString Class, Type;
				//	m_Class.GetWindowText(Class);
				//	m_Type.GetWindowText(Type);

				//	DeleteFile(_T("Code\\") + Class + _T("\\") + Type + _T("\\") + Path + _T(".rtf"));
				//	//m_List.DeleteItem(m_List.GetNextItem(-1, LVIS_SELECTED));
				//}

				m_Edit.SetWindowText(_T(""));
				OnKillfocusTypeCombo();

				MessageBox(_T("方法移除完毕!"));
			}
		}
		else
		{
			AfxMessageBox(_T("请选择要移除的方法!"));
		}
	}
	else
	{
		AfxMessageBox(_T("请选择要移除的方法!"));
	}

	VMPEND
}


void CMainDlg::OnSynchronization()
{
	CLoginDlg dlg;
	if(dlg.DoModal() == IDOK)
	{

	}
}


void CMainDlg::OnSetting()
{
	m_Setting->CenterWindow();
	m_Setting->SetForegroundWindow();
	m_Setting->OnReadSetting();
	m_Setting->ShowWindow(SW_SHOW);
}


void CMainDlg::OnCancel()
{
	// 是否自动打开
	if( GetPrivateProfileInt(_T("Setting"), _T("Open"), 0, _T("./Setting.ini")) == 1)
	{
		CString Name = m_List.GetItemText(m_List.GetNextItem(-1, LVIS_SELECTED), 0), Class, Type;
		m_Class.GetWindowText(Class);
		m_Type.GetWindowText(Type);

		// 判断是否选择了源码
		if(!Name.IsEmpty() && m_List.GetNextItem(-1, LVIS_SELECTED) != -1)
		{
			// 保存配置文件
			::WritePrivateProfileString(_T("File"), _T("Class"), Class, _T("./Setting.ini"));
			::WritePrivateProfileString(_T("File"), _T("Type"),  Type,  _T("./Setting.ini"));
			::WritePrivateProfileString(_T("File"), _T("Name"),  Name,  _T("./Setting.ini"));
		}
	}

	CDialogEx::OnCancel();
}
