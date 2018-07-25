// TransmissionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "编程助理.h"
#include "TransmissionDlg.h"
#include "afxdialogex.h"

#include <assert.h>
#include <afxinet.h> 
#include "ProjectDlg.h"


// 定义大小
#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)

// TransmissionDlg 对话框

IMPLEMENT_DYNAMIC(CTransmissionDlg, CDialogEx)

CTransmissionDlg::CTransmissionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTransmissionDlg::IDD, pParent)
{
	IsPause = IsFinished = IsTerminate = false;
	m_Thread= NULL;

	Target  = 0;
	IsDownload = IsCode = IsProject = IsUpDate = IsSynchronize = ModifyTime = FALSE;
}

CTransmissionDlg::~CTransmissionDlg()
{
	// 释放对象
	m_Downloader->DeleteCurlDownloader(&m_pCurlDownloader);

	// 释放全局
	m_Downloader->UninitCurlDownloader();

	// 置为空
	m_pCurlDownloader = NULL;
	m_Downloader      = NULL;
}

void CTransmissionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}


BEGIN_MESSAGE_MAP(CTransmissionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CTransmissionDlg::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CTransmissionDlg::OnCancel)

	ON_COMMAND(100, &CTransmissionDlg::Complete)
	ON_COMMAND(101, &CTransmissionDlg::OnError)
	ON_COMMAND(102, &CTransmissionDlg::OnCancel)
END_MESSAGE_MAP()


// TransmissionDlg 消息处理程序


BOOL CTransmissionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 声明下载对象
	m_Downloader = NULL;

	// 初始化下载对象
	////////////////////////////////////////////////////////////////////////////////////////////////
	m_Downloader->InitCurlDownloader();

	// 创建下载对象
	////////////////////////////////////////////////////////////////////////////////////////////////
	m_pCurlDownloader = m_Downloader->CreateCurlDownloader();

	// 缓存目录检测
	CString FilePath = _T("Cache");
	DWORD Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //文件夹不存在
	{
		CreateDirectory(FilePath, NULL);
	}

	// 得到传输队列总数
	Total = TargetList->GetSize();

	// 开始传输
	Prepare();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL CTransmissionDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


CString CTransmissionDlg::OnGetSize(int nSize)
{
	CString Size;
	if (nSize > GB)
	{
		Size.Format(_T("%0.2f GB"), (long double)nSize / GB);
	}
	else if (nSize > MB)
	{
		Size.Format(_T("%0.2f MB"), (double)nSize / MB);
	}
	else if (nSize > KB)
	{
		Size.Format(_T("%0.2f KB"), (double)nSize / KB);
	}
	else
	{
		Size.Format(_T("%d B"), nSize);
	}

	return Size;
}


void CTransmissionDlg::Split(CString source, CString divKey, CStringArray &dest)
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


unsigned char ToHex(unsigned char x) 
{ 
    return  x > 9 ? x + 55 : x + 48; 
}
 

unsigned char FromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}
 

std::string UrlEncode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) || 
            (str[i] == '-') ||
            (str[i] == '_') || 
            (str[i] == '.') || 
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}
 

std::string UrlDecode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '+') strTemp += ' ';
        else if (str[i] == '%')
        {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high*16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}


CString urlEncode(CString s)
{
	int len = s.GetLength();
	char *out = new char[len*9+1];
	memset(out , 0 , len*9+1);
	int i , j;
	int ch = 0 ;

	static char  myhex[0xFF+1][4];  //add by zhouzd 2008-10-06
	static bool isinital = false;

	if ( !isinital )
	{
		for ( i = 0 ; i <= 0xFF ; ++i )
		{
			myhex[i][0] = '%';
			sprintf_s( myhex[i]+1, strlen(myhex[i]+1), "%02X" , i );
		}
		isinital = true;
	}

	for (i = 0 , j = 0; i < len ; ++i )
	{
		ch = s.GetAt(i);

		//printf("%c\n" , s.GetAt(i) );

		if ('A' <= ch && ch <= 'Z')         // 'A'..'Z'
		{
			out[j++] = ch;
		}
		else if ('a' <= ch && ch <= 'z')    // 'a'..'z'
		{
			out[j++] = ch;
		}
		else if ('0' <= ch && ch <= '9')    // '0'..'9'
		{
			out[j++] = ch;
		}
		else if (ch == ' ')           // space
		{
			out[j++] = '+';
		}
		else if (ch == '-' || ch == '_'        // 不需要转化
			|| ch == '.' || ch == '!'
			|| ch == '~' || ch == '*'
			|| ch == '\'' || ch == '('
			|| ch == ')')
		{
			out[j++] = ch;
		}
		else if (ch <= 0x007f)     // ASCII控制字符
		{    
			strcat_s(out, strlen(out), myhex[ch]);
			j += 3;
		}
		else if (ch <= 0x07FF)         // 非ASCII <= 0x7FF
		{
			strcat_s(out, strlen(out), myhex[0xc0 | (ch >> 6)]);
			strcat_s(out, strlen(out), myhex[0x80 | (ch & 0x3F)]);
			j += 6;
		}
		else                       // 0x7FF < ch <= 0xFFFF
		{
			strcat_s(out, strlen(out), myhex[0xe0 | (ch >> 12)]);
			strcat_s(out, strlen(out), myhex[0x80 | ((ch >> 6) & 0x3F)]);
			strcat_s(out, strlen(out), myhex[0x80 | (ch & 0x3F)]);
			j += 9;
		}
	}
	out[j] = '\0';
	USES_CONVERSION;
	CString result = (CStringA)A2W(out);

	delete out;
	out = NULL;

	return result;
}


// 获取URL重定向后的文件名,如果没有重定向,也返回下载文件名
CString GetFileNameFromRedirectUrl(CString strUrl)
{
	CInternetSession iSession; 
	CStdioFile* pFileDown = NULL; 
	CString sFileName; 
 
	pFileDown = iSession.OpenURL(strUrl, 1, INTERNET_FLAG_TRANSFER_BINARY); 
 
	CHttpFile* pHttpFile = (CHttpFile *)pFileDown; 
	HINTERNET hHttpFile = HINTERNET(*pHttpFile); 
 
	//获得重定向文件名 
	//BOOL bResult = pHttpFile->QueryOption(INTERNET_OPTION_URL, sFileName);
	DWORD File   = (DWORD)(LPCTSTR)sFileName;
	BOOL bResult = pHttpFile->QueryOption(INTERNET_OPTION_URL, File);
	int flag     = sFileName.ReverseFind('/');
	CString strFileName = sFileName.Mid(flag + 1);
 
	return strFileName;
}


int CTransmissionDlg::GetFileSize(CString Path)
{
	CFileStatus Status;

	if(CFile::GetStatus(Path, Status))
	{
		int nSize = (int)Status.m_size;
		return nSize;
	}

	return 0;
}


time_t CTransmissionDlg::FormatTime(char * szTime)  
{  
    struct tm tm1;  
    time_t time1;  
  
	// 格式化
    sscanf_s(szTime, "%4d%2d%2d%2d%2d%2d",      
          &tm1.tm_year,   
          &tm1.tm_mon,   
          &tm1.tm_mday,   
          &tm1.tm_hour,   
          &tm1.tm_min,  
          &tm1.tm_sec);  
          
    tm1.tm_year -= 1900;  
     tm1.tm_mon --;  
  
  
    tm1.tm_isdst=-1;  
    
    time1 = mktime(&tm1);  
    return time1;  
}


//指定YYYYMMDDHH24MISS型的时间，格式化为time_t型的时间  
SYSTEMTIME CTransmissionDlg::FormatTime2(TCHAR * szTime)  
{  
  SYSTEMTIME tm1;  
  _stscanf_s( szTime, _T("%4d-%2d-%2d %2d:%2d:%2d"),     
    &tm1.wYear,   
    &tm1.wMonth,   
    &tm1.wDay,   
    &tm1.wHour,   
    &tm1.wMinute,  
    &tm1.wSecond );  
    return tm1;  
}


void CTransmissionDlg::HttpPostFile( CString ServerName, CString ServerPath, int ServerPort, CString File, CString ParamName, CString ContentType )
{
     CString boundary = "---------------------------1a2b3c4d5e6f";//分隔符，注意：首行必须多"--"，尾部最后必须加"--"，否则识别不出。
     CString boundaryTemplate;
     boundaryTemplate.Format(_T("\r\n--%s\r\n") , boundary);

     byte* boundarybytes = (byte*)boundaryTemplate.GetBuffer(boundaryTemplate.GetLength());
     CInternetSession pSession("ic_PostWav");
     CHttpConnection* pConnect;
     CHttpFile*         pFile;

	 // 数据连接
     INTERNET_PORT wPort = ServerPort;
     pConnect= pSession.GetHttpConnection(ServerName , wPort);
     pFile= pConnect->OpenRequest(CHttpConnection::HTTP_VERB_POST, ServerPath, NULL,0,NULL,NULL,INTERNET_FLAG_DONT_CACHE);

     //数据包头
     CString pPostHeader , pPostHeaderTemplate;
     pPostHeaderTemplate = 
		 _T("Accept:audio/x-wav,text/html,application/xhtml+xml,application/xml,*/*;q=0.9,*/*;q=0.8\r\n")
         _T("Content-Type: multipart/form-data;boundary=%s\r\n")
         _T("Connection: keep-alive\r\n");  
  
     pPostHeader.Format(pPostHeaderTemplate, boundary);  
     pFile->AddRequestHeaders(pPostHeader);  
  

     //数据帧头  
     CString pPostTop , pPostTopTemplate;  
     pPostTopTemplate= _T("%sContent-Disposition:form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n");  
     pPostTop.Format(pPostTopTemplate  , boundaryTemplate, _T("file"), File, ContentType);  
     byte* pPostTopbytes = (byte*)pPostTop.GetBuffer(pPostTop.GetLength());  


     //数据包尾
     CString ender,enderTemplate;  
     enderTemplate= _T("\r\n--%s\r\nContent-Disposition:form-data; name=\"%s\"\r\n\r\n%s");
     ender.Format(enderTemplate, boundary , _T("Ender") , _T("ender"));
     ender+= _T("\r\n--");
     ender+= boundary;
     ender+= _T("--\r\n");
     byte* enderbyte = (byte*)ender.GetBuffer(ender.GetLength());

	 // 文件对象
     CFile cfile;
     cfile.Open(File, CFile::modeRead|CFile::shareDenyRead, NULL);
     DWORD dwSize = pPostTop.GetLength() + ender.GetLength() + (DWORD)cfile.GetLength();
     pFile->SendRequestEx(dwSize);
     pFile->Write(pPostTopbytes, pPostTop.GetLength());
	 

     //数据主体
     int bufflength = 4 * 1024;
     byte* buffer = new byte[bufflength];
     int byteRead = 0;


	 // 上传进度
	 int pos = 0;          // 当前进度条的位置
	 CString ProgressText; // 进度文字提示
	 SetDlgItemText(IDC_STATUS_STATIC, _T("正在进行传输操作..."));

	 // 获取目标文件大小
	 int Size = GetFileSize(File);

	 // 上传文件
     while((byteRead = cfile.Read(buffer , bufflength)) != 0)
     {
		 // 设置新的进度条位置
		 pos = pos + byteRead;                
         pFile->Write(buffer , byteRead);

		 //更新进度
		 m_Progress.SetRange32(0, Size);
		 m_Progress.SetPos(pos);

		 ProgressText.Format(_T("传输中 (进度: %s / %s)"), OnGetSize(pos), OnGetSize(Size));
		 SetDlgItemText(IDC_STATUS_STATIC, ProgressText);
	 }
     cfile.Close();


     //写尾
     pFile->Write(enderbyte, ender.GetLength());
     pFile->EndRequest();

     CString strSentence = _T(""), strGetSentence = _T("");
     DWORD dwRet;

     pFile->QueryInfoStatusCode(dwRet);
     if(HTTP_STATUS_OK == dwRet)
     {
		 // 读取提交数据后的返回结果
         while(pFile->ReadString(strSentence))
         {
              strGetSentence = strGetSentence+ strSentence;
         }
     }

     pFile->Close();
     pConnect->Close();

	 // 完成上传
	 // 得到上传路径
	 UploadPath = strGetSentence;

	 // 局部变量
	 CString RecvData, Parameter, UserId, Parent, Sub, Item, Temp;
	 BOOL IsSuccess;

	 if(IsProject)
	 {
		  // 替换
		 TargetPath.Replace(_T("./Cache/"), _T("/Project/"));
	 }

	 Temp   = TargetPath.Right(TargetPath.GetLength() - TargetPath.Find('/') -1);
	 UserId = TargetPath.Left(TargetPath.GetLength() - Temp.GetLength() -1);

	 // 方法
	 if(IsCode || IsProject)
	 {
		 Sub    = Temp.Right(Temp.GetLength() - Temp.ReverseFind('/') -1);
		 Parent = Temp.Left(Temp.GetLength()  - Sub.GetLength() -1);
	 }
	 else if(!IsCode && !IsProject)
	 {
		 // 文件
		 Temp   = Temp.Right(Temp.GetLength() - Temp.Find('/') -1);
		 Item   = Temp.Right(Temp.GetLength() - Temp.ReverseFind('/') -1);

		 Temp   = Temp.Left(Temp.GetLength() - Item.GetLength() -1);
		 Sub    = Temp.Right(Temp.GetLength() - Temp.ReverseFind('/') -1);
		 Parent = _T("File/") + Temp.Left(Temp.GetLength() - Sub.GetLength() -1);
	 }
	 
	 // 参数赋值
	 if(IsCode || IsProject)
		 Parameter.Format(_T("path=%s&target=%s&name=%s&id=%s&parent=%s&sub=%s"), UploadPath, TargetPath, UploadName, UserId, Parent, Sub);
	 else
		 Parameter.Format(_T("path=%s&target=%s&name=%s&id=%s&parent=%s&sub=%s&item=%s"), UploadPath, TargetPath, UploadName, UserId, Parent, Sub, Item);

	 // 获取服务器数据
	 try
	 {
		 RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/ReMoveFile"), 80, Parameter, IsSuccess);
		 if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		 {
			 AfxMessageBox(_T("无法连接到服务器, 请检查网络。"));
		 }
		 else
		 {
			 if (IsSuccess)
			 {
				 if( RecvData.Replace(_T("Success"), _T("Success")) )
				 {
					 // 更新界面信息
					 SetDlgItemText(IDOK, _T("开始"));
					 SetDlgItemText(IDCANCEL, _T("关闭"));

					 SetDlgItemText(IDC_STATUS_STATIC, _T("传输操作已完成."));
					 GetDlgItem(IDOK)->EnableWindow(false);
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
		 AfxMessageBox(_T("发生了异常，位于CTransmissionDlg的HttpPostFile方法。"));
	 }
}


UINT CTransmissionDlg::DownloadFile(LPVOID pParam)
{
	// 窗口指针
	CTransmissionDlg * pWnd = (CTransmissionDlg*)pParam;

	// 进入临界区
	EnterCriticalSection(pWnd->m_cs);

	// 局部变量
	int Percentage = 0;
	bool isfinished = false;
	CStringA Path = (CStringA)pWnd->DownloadPath, Name = (CStringA)pWnd->DownloadName;
	CString ProgressText;
	
	// 显示传输目标
	pWnd->SetDlgItemText(IDC_TARGET_STATIC, pWnd->DownloadName);

	// 显示传输状态
	pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("正在校验下载参数..."));

	// 开始传输目标
	pWnd->GetDlgItem(IDOK)->EnableWindow();
	if(!pWnd->m_pCurlDownloader->Start(Path.GetBuffer(), Name.GetBuffer()))
	{
		// 更新传输队列
		pWnd->Target++;

		// 更新界面信息
		pWnd->SetDlgItemText(IDOK, _T("开始"));
		pWnd->SetDlgItemText(IDCANCEL, _T("关闭"));

		pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("传输操作已完成."));
		pWnd->GetDlgItem(IDOK)->EnableWindow(false);

		// 线程对象重置为空
		pWnd->m_Thread   = NULL;
		pWnd->IsFinished = true;

		// 提示错误
		pWnd->Error = _T("下载参数校验失败，可能是目标大小为零或文件信息错误。");
		pWnd->PostMessage(WM_COMMAND, 101);

		// 离开临界区
		LeaveCriticalSection(pWnd->m_cs);
		return false;
	}

	// 获得目标数据
	unsigned long  x = 0, y = 0;
	pWnd->m_pCurlDownloader->GetProgress(&x, &y);
	CString Ext = PathFindExtension(pWnd->DownloadName);

	// 如果目标信息错误
	if(x <= 0)
	{
		// 更新传输队列
		pWnd->Target++;

		// 更新界面信息
		pWnd->SetDlgItemText(IDOK, _T("开始"));
		pWnd->SetDlgItemText(IDCANCEL, _T("关闭"));

		pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("传输操作已完成."));
		pWnd->GetDlgItem(IDOK)->EnableWindow(false);

		// 线程对象重置为空
		pWnd->m_Thread   = NULL;
		pWnd->IsFinished = true;

		// 提示错误
		pWnd->Error = _T("目标文件为空，不能下载。");
		pWnd->PostMessage(WM_COMMAND, 101);

		// 离开临界区
		LeaveCriticalSection(pWnd->m_cs);
		return false;
	}
	if(Ext == _T(".") || Ext.IsEmpty())
	{
		// 更新传输队列
		pWnd->Target++;

		// 更新界面信息
		pWnd->SetDlgItemText(IDOK, _T("开始"));
		pWnd->SetDlgItemText(IDCANCEL, _T("关闭"));

		pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("传输操作已完成."));
		pWnd->GetDlgItem(IDOK)->EnableWindow(false);

		// 线程对象重置为空
		pWnd->m_Thread   = NULL;
		pWnd->IsFinished = true;

		// 提示错误
		pWnd->Error = _T("目标文件扩展名不正确，不能下载。");
		pWnd->PostMessage(WM_COMMAND, 101);

		// 离开临界区
		LeaveCriticalSection(pWnd->m_cs);
		return false;
	}

	// 设置进度条
	pWnd->m_Progress.SetRange32(0, x);
	pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("正在执行传输操作..."));

	while (!isfinished)
	{
		if (!pWnd->IsPause)
		{
			pWnd->m_pCurlDownloader->CheckIsFinish(&isfinished);
			pWnd->m_pCurlDownloader->GetCurlDownState();

			pWnd->m_pCurlDownloader->GetProgress(&x, &y);
			ProgressText.Format(_T("传输中 (进度: %s / %s)"), pWnd->OnGetSize(y), pWnd->OnGetSize(x));
			pWnd->SetDlgItemText(IDC_STATUS_STATIC, ProgressText);

			// 进度条
			pWnd->m_Progress.SetPos(y);
			Sleep(30);
		}
		else
		{
			CString Status;
			pWnd->GetDlgItemText(IDC_STATUS_STATIC, Status);
			if (pWnd->IsPause && Status != _T("操作被用户暂停."))
			{
				pWnd->SetDlgItemText(IDOK, _T("继续"));
				pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("操作被用户暂停."));
			}
		}

		if (pWnd->IsTerminate)
		{
			// 挂起/终止线程
			if (pWnd->m_pCurlDownloader->Pause() && pWnd->m_pCurlDownloader->Terminate())
			{
				// 回滚进度条
				pWnd->m_Progress.SetPos(0);

				// 修改按钮标题
				pWnd->SetDlgItemText(IDCANCEL, _T("关闭"));
				pWnd->SetDlgItemText(IDOK, _T("开始"));

				// 修改按钮状态
				pWnd->GetDlgItem(IDOK)->EnableWindow();

				// 修改操作状态
				pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("操作被用户取消."));

				// 线程对象重置为空
				pWnd->m_Thread   = NULL;
				pWnd->IsFinished = false;

				// 终止对象重置为假
				pWnd->IsTerminate = false;

				// 终止循环
				break;
			}
		}

		// 下载已完成
		if (isfinished)
		{
			// 等待完成
			pWnd->m_pCurlDownloader->WaitForFinish();

			// 更新界面信息
			pWnd->SetDlgItemText(IDOK, _T("开始"));
			pWnd->SetDlgItemText(IDCANCEL, _T("关闭"));

			pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("传输操作已完成."));
			pWnd->GetDlgItem(IDOK)->EnableWindow(false);
		}
	}
	
	// 更新传输队列
	pWnd->Target++;
	pWnd->PostMessage(WM_COMMAND, 100);

	// 线程对象重置为空
	pWnd->m_Thread   = NULL;
	pWnd->IsFinished = true;

	// 离开临界区
	LeaveCriticalSection(pWnd->m_cs);
	return TRUE;
}


UINT CTransmissionDlg::UploadFile(LPVOID pParam)
{
	// 窗口指针
	CTransmissionDlg * pWnd = (CTransmissionDlg*)pParam;

	// 获取服务器数据
	try
	{
		pWnd->IsFinished = false;

		// 显示传输目标
		pWnd->SetDlgItemText(IDC_TARGET_STATIC, pWnd->UploadPath);

		// 显示传输状态
		pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("正在校验上传参数..."));

		// 判断上传类型
		pWnd->HttpPostFile("www.shadowviolet.cn", _T("/index/account/UpLoadFile"), 80, pWnd->UploadPath, "file", "multipart/form-data");
		/*if(pWnd->IsCode)
		{
			pWnd->HttpPostFile("www.shadowviolet.cn", _T("/index/account/UpLoadFile"), 80, pWnd->UploadPath, "file", "application/rtf");
		}
		else
		{
			pWnd->HttpPostFile("www.shadowviolet.cn", _T("/index/account/UpLoadFile"), 80, pWnd->UploadPath, "file", "multipart/form-data");
		}*/

		// 进入临界区
		EnterCriticalSection(pWnd->m_cs);
	}
	catch (...)
	{
		pWnd->Error = _T("发生了异常，位于CTransmissionDlg的UploadFile方法。");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	// 线程对象重置为空
	pWnd->m_Thread   = NULL;
	pWnd->IsFinished = true;

	// 更新传输队列
	pWnd->Target++;
	pWnd->PostMessage(WM_COMMAND, 100);

	// 离开临界区
	LeaveCriticalSection(pWnd->m_cs);
	return true;
}


UINT CTransmissionDlg::Decompression(LPVOID pParam)
{
	// 窗口指针
	CTransmissionDlg * pWnd = (CTransmissionDlg*)pParam;

	// 显示传输状态
	pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("正在解压项目..."));
	pWnd->GetDlgItem(IDCANCEL)->EnableWindow(false);

	// 解压压缩文件
	CString CommandLine;
	CommandLine.Format(_T("x -t7z \"%s\" -o\"%s\" -y"), pWnd->DownloadName, pWnd->TargetPath);

	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;

	ShExecInfo.lpFile       = "Application\\7z.exe";
	ShExecInfo.lpDirectory  = NULL;
	ShExecInfo.lpParameters = CommandLine;

	ShExecInfo.nShow = SW_HIDE;
	ShellExecuteEx(&ShExecInfo);

	AfxGetApp()->BeginWaitCursor();
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	AfxGetApp()->EndWaitCursor();

	// 显示传输状态
	pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("传输操作已完成."));
	pWnd->GetDlgItem(IDCANCEL)->EnableWindow();

	return 1;
}


void CTransmissionDlg::Prepare()
{
	// 检查路径
	LPCTSTR str = TargetList->GetAt(Target); //把CString类型转换为char* 类型
	for (int i=0;str[i];i++)
	{
		if (str[i] < 0) //一个中文占两个字节，且每个字节都是小于0的
		{
			CString tmp;
			tmp.Format("%c%c",str[i],str[i+1]);//把中文输出，举个例子
			i++;

			AfxMessageBox(_T("目标路径不能含有中文!"));
			CDialog::OnCancel();
			return;
		}
	}

	CString Check(TargetList->GetAt(Target));
	if(Check.Replace(_T(" "), _T("")) || Check.Replace(_T(" "), _T("")))
	{
		AfxMessageBox(_T("目标路径不能含有空格!"));

		CDialog::OnCancel();
		return;
	}

	if(Check.Replace(_T("!"), _T("")) || Check.Replace(_T("@"), _T("")) || Check.Replace(_T("#"), _T("")) || Check.Replace(_T("$"), _T("")) || Check.Replace(_T("￥"), _T("")) || Check.Replace(_T("%"), _T("")) || Check.Replace(_T("^"), _T("")) || Check.Replace(_T("……"), _T("")) || Check.Replace(_T("&"), _T("")) || Check.Replace(_T("*"), _T("")) || Check.Replace(_T("（"), _T("")) || Check.Replace(_T("）"), _T("")) || Check.Replace(_T("("), _T("")) || Check.Replace(_T(")"), _T("")) || Check.Replace(_T("_"), _T("")) || Check.Replace(_T("--"), _T("")) || Check.Replace(_T("+"), _T("")) || Check.Replace(_T("="), _T("")) || Check.Replace(_T(","), _T("")) || Check.Replace(_T(".."), _T("")) || Check.Replace(_T("，"), _T("")) || Check.Replace(_T("。"), _T("")) || Check.Replace(_T("、"), _T("")) || Check.Replace(_T("//"), _T("")) || Check.Replace(_T("?"), _T("")) || Check.Replace(_T(";"), _T("")) || Check.Replace(_T("；"), _T("")) || Check.Replace(_T("‘"), _T("")) || Check.Replace(_T("’"), _T("")) || Check.Replace(_T("'"), _T("")) || Check.Replace(_T(":"), _T("")) || Check.Replace(_T("\""), _T("")) || Check.Replace(_T("|"), _T("")) || Check.Replace(_T("\\"), _T("")) || Check.Replace(_T("["), _T("")) || Check.Replace(_T("{"), _T("")) || Check.Replace(_T("【"), _T("")) || Check.Replace(_T("】"), _T("")) || Check.Replace(_T("}"), _T("")) || Check.Replace(_T("]"), _T("")) || Check.Replace(_T("`"), _T("")) || Check.Replace(_T("·"), _T("")) || Check.Replace(_T("~"), _T("")) || Check.Replace(_T("\r"), _T("")) || Check.Replace(_T("\n"), _T("")))
	{
		AfxMessageBox(_T("目标路径不能含有符号!"));

		CDialog::OnCancel();
		return;
	}


	// 下载
	if(IsDownload)
	{
		// 传输配置
		DownloadPath = _T("http://www.shadowviolet.cn/cloud/") + TargetList->GetAt(Target);
		DownloadName = _T("./Cache/") + DownloadPath.Right(DownloadPath.GetLength() - DownloadPath.ReverseFind('/') -1);

		if(IsUpDate)
		{
			DownloadPath = _T("http://www.shadowviolet.cn/update/") + TargetList->GetAt(Target);
			DownloadName = _T("./Cache/") + TargetList->GetAt(Target);
		}

		// 开始传输
		if (m_Thread == NULL)
		{
			m_Thread = AfxBeginThread(DownloadFile, this);
		}
	}
	else // 上传
	{
		// 传输配置
		UploadPath = TargetPath = TargetList->GetAt(Target);
		UploadPath = UploadPath.Right(UploadPath.GetLength() - UploadPath.Find('/') -1);
		UploadName = UploadPath.Right(UploadPath.GetLength() - UploadPath.ReverseFind('/') -1);
		TargetPath = TargetPath.Left(TargetPath.GetLength() - UploadName.GetLength() -1);

		// 开始传输
		if (m_Thread == NULL)
		{
			m_Thread = AfxBeginThread(UploadFile, this);
		}
	}
}


void CTransmissionDlg::Complete()
{
	// 完成传输
	CString Path(DownloadPath);
	Path.Replace(_T("http://www.shadowviolet.cn/cloud/"), _T(""));
	
	// 局部变量
	CStringArray TextArray;
	Split(Path, _T("/"), TextArray);

	// 传输模式
	if(IsDownload)
	{
		// 如果是代码
		if(IsCode)
		{
			CString Parent = TextArray.GetAt(2), Sub = TextArray.GetAt(3);

			// 目录检测
			CString FilePath = _T("./Code/") + Parent;
			if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
			{
				CreateDirectory(FilePath, NULL);
			}

			FilePath = _T("./Code/") + Parent + _T("/") + Sub;
			if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
			{
				CreateDirectory(FilePath, NULL);
			}

			// 移动文件
			CString Name(DownloadName);
			Name.Replace(_T("./Cache/"), _T(""));
			FilePath = _T("./Code/") + Parent + _T("/") + Sub + _T("/") + Name;

			if(!MoveFileEx(DownloadName, FilePath, MOVEFILE_REPLACE_EXISTING))
			{
				AfxMessageBox(_T("移动目标失败!"));
			}

			// 修改本地文件时间
			if(ModifyTime)
			{
				ModifyList;

				// 修改本地文件修改时间
				CFileStatus Status;
				CString ModifyTime = ModifyList->GetAt(Target -1);

				Status.m_mtime = FormatTime2(ModifyTime.GetBuffer());
				CFile::SetStatus(FilePath, Status);
			}
		}
		else if(IsProject)
		{
			// 解压缩
			AfxBeginThread(Decompression, this);
		}
		else if(IsUpDate)
		{
			// 得到文件名
			CString FileName = DownloadName;
			FileName.Replace(_T("./Cache/"), _T(""));

			// 移动文件
			if(!MoveFileEx(DownloadName, FileName, MOVEFILE_REPLACE_EXISTING))
			{
				AfxMessageBox(_T("移动目标失败!"));
				return;
			}

			// 解压参数
			CString Parament = _T("Application\\7z x -t7z -y ") + FileName;

			// 等待主程序关闭之后执行解压
			CString strParam = _T("/C choice /t 1 /d y /n > nul & tasklist | find /i \"编程助理.exe\" && taskkill /im 编程助理.exe /f || echo NotFind & ");
			strParam += Parament;
			strParam += _T(" & del ") + FileName;
			strParam += _T(" & Start 编程助理 -readonce 更新说明");

			// 调用命令行 执行更新
			ShellExecute(NULL, NULL, _T("cmd.exe"), strParam, NULL, SW_HIDE);

			// 发出更新命令
			CDialogEx::OnCancel();
			::SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_COMMAND, 102, 0);
		}
		else
		{
			CString Parent = TextArray.GetAt(2), Sub = TextArray.GetAt(3), Item = TextArray.GetAt(4);

			// 目录检测
			CString FilePath = _T("./File/") + Parent;
			if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
			{
				CreateDirectory(FilePath, NULL);
			}

			FilePath = _T("./File/") + Parent + _T("/") + Sub;
			if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
			{
				CreateDirectory(FilePath, NULL);
			}

			FilePath = _T("./File/") + Parent + _T("/") + Sub + _T("/") + Item;
			if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //文件夹不存在
			{
				CreateDirectory(FilePath, NULL);
			}

			// 移动文件
			CString Name(DownloadName);
			Name.Replace(_T("./Cache/"), _T(""));
			FilePath = _T("./File/") + Parent + _T("/") + Sub + _T("/") + Item + _T("/") + Name;

			if(!MoveFileEx(DownloadName, FilePath, MOVEFILE_REPLACE_EXISTING))
			{
				AfxMessageBox(_T("移动目标失败!"));
			}

			// 修改本地文件时间
			if(ModifyTime)
			{
				ModifyList;

				// 修改本地文件修改时间
				CFileStatus Status;
				CString ModifyTime = ModifyList->GetAt(Target);

				Status.m_mtime = FormatTime2(ModifyTime.GetBuffer());
				CFile::SetStatus(FilePath, Status);
			}
		}
	}
	else
	{
		// 清理缓存
		if(IsProject)
		{
			CString Temp = TargetPath.Right(TargetPath.GetLength() - TargetPath.Find('/') -1), Parent = Temp.Left(Temp.GetLength() - Temp.Find('/'));
			Parent.Replace(_T("Project"), _T("Cache"));

			CProjectDlg::DeleteDirectory(_T("./") + Parent);
		}
	}
	

	// 判断是否需要传输
	if(Total -1 >= Target)
	{
		// 准备传输
		Prepare();
	}
	else
	{
		// 自动关闭
		if(IsSynchronize)
		{
			PostMessage(WM_COMMAND, 102);
		}

		// 变量初始化
		IsDownload = IsCode = IsProject = IsUpDate = IsSynchronize = ModifyTime = FALSE;
	}
}


void CTransmissionDlg::OnError()
{
	// 提示错误
	AfxMessageBox(Error);

	// 判断是否需要传输
	if(Total -1 >= Target)
	{
		// 准备传输
		Prepare();
	}
}


void CTransmissionDlg::OnOK()
{
	// 没有暂停
	if(!IsPause)
	{
		if (m_pCurlDownloader->Pause())
		{
			IsPause = true;
			GetDlgItem(IDOK)->SetWindowText(_T("继续"));
		}
	}
	else
	{
		if (m_pCurlDownloader->Resume())
		{
			IsPause = false;
			GetDlgItem(IDOK)->SetWindowText(_T("暂停"));
		}
	}
}


void CTransmissionDlg::OnCancel()
{
	if (m_Thread != NULL && !IsFinished && !IsTerminate)
	{
		SetDlgItemText(IDC_STATUS_STATIC, _T("正在取消操作."));
		IsTerminate = true;
		IsFinished = false;
	}
	else if (IsTerminate)
	{
		IsFinished = false;

		if( MessageBox(_T("确定要终止操作吗?"), _T("终止操作确认"), MB_ICONQUESTION | MB_YESNO) != IDYES )
			return;
		else
		{
			// 终止线程
			m_pCurlDownloader->Terminate();
			CDialogEx::OnCancel();
		}
	}
	else
	{
		if (IsFinished)
		{
			// 终止线程
			m_pCurlDownloader->Terminate();
		}

		CDialogEx::OnCancel();
	}
}
