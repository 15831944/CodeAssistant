// TransmissionDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "�������.h"
#include "TransmissionDlg.h"
#include "afxdialogex.h"

#include <afxinet.h> 
#include "ProjectDlg.h"

// �����С
#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)

// TransmissionDlg �Ի���

IMPLEMENT_DYNAMIC(CTransmissionDlg, CDialogEx)

CTransmissionDlg::CTransmissionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTransmissionDlg::IDD, pParent)
{
	IsPause = IsFinished = IsTerminate = false;
	m_Thread= NULL;

	Target  = 0;
	IsDownload = IsCode = IsProject = IsUpDate = IsSynchronize = FALSE;
}

CTransmissionDlg::~CTransmissionDlg()
{
	// �ͷŶ���
	m_Downloader->DeleteCurlDownloader(&m_pCurlDownloader);

	// �ͷ�ȫ��
	m_Downloader->UninitCurlDownloader();

	// ��Ϊ��
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
END_MESSAGE_MAP()


// TransmissionDlg ��Ϣ�������


BOOL CTransmissionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �������ض���
	m_Downloader = NULL;

	// ��ʼ�����ض���
	////////////////////////////////////////////////////////////////////////////////////////////////
	m_Downloader->InitCurlDownloader();

	// �������ض���
	////////////////////////////////////////////////////////////////////////////////////////////////
	m_pCurlDownloader = m_Downloader->CreateCurlDownloader();

	// ����Ŀ¼���
	CString FilePath = _T("Cache");
	DWORD Path = GetFileAttributes(FilePath);
	if (Path == 0xFFFFFFFF)     //�ļ��в�����
	{
		CreateDirectory(FilePath, NULL);
	}

	// �õ������������
	Total = TargetList->GetSize();

	// ��ʼ����
	Prepare();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CTransmissionDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

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


void CTransmissionDlg::HttpPostFile( CString ServerName, CString ServerPath, int ServerPort, CString File, CString ParamName, CString ContentType )
{
     CString boundary = "---------------------------1a2b3c4d5e6f";//�ָ�����ע�⣺���б����"--"��β���������"--"������ʶ�𲻳���
     CString boundaryTemplate;
     boundaryTemplate.Format(_T("\r\n--%s\r\n") , boundary);

     byte* boundarybytes = (byte*)boundaryTemplate.GetBuffer(boundaryTemplate.GetLength());
     CInternetSession pSession("ic_PostWav");
     CHttpConnection* pConnect;
     CHttpFile*         pFile;

	 // ��������
     INTERNET_PORT wPort = ServerPort;
     pConnect= pSession.GetHttpConnection(ServerName , wPort);
     pFile= pConnect->OpenRequest(CHttpConnection::HTTP_VERB_POST, ServerPath, NULL,0,NULL,NULL,INTERNET_FLAG_DONT_CACHE);

     //���ݰ�ͷ
     CString pPostHeader , pPostHeaderTemplate;
     pPostHeaderTemplate = 
		 _T("Accept:audio/x-wav,text/html,application/xhtml+xml,application/xml,*/*;q=0.9,*/*;q=0.8\r\n")
         _T("Content-Type: multipart/form-data;boundary=%s\r\n")
         _T("Connection: keep-alive\r\n");  
  
     pPostHeader.Format(pPostHeaderTemplate, boundary);  
     pFile->AddRequestHeaders(pPostHeader);  
  

     //����֡ͷ  
     CString pPostTop , pPostTopTemplate;  
     pPostTopTemplate= _T("%sContent-Disposition:form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n");  
     pPostTop.Format(pPostTopTemplate  , boundaryTemplate, _T("file"), File, ContentType);  
     byte* pPostTopbytes = (byte*)pPostTop.GetBuffer(pPostTop.GetLength());  


     //���ݰ�β
     CString ender,enderTemplate;  
     enderTemplate= _T("\r\n--%s\r\nContent-Disposition:form-data; name=\"%s\"\r\n\r\n%s");
     ender.Format(enderTemplate, boundary , _T("Ender") , _T("ender"));
     ender+= _T("\r\n--");
     ender+= boundary;
     ender+= _T("--\r\n");
     byte* enderbyte = (byte*)ender.GetBuffer(ender.GetLength());

	 // �ļ�����
     CFile cfile;
     cfile.Open(File, CFile::modeRead|CFile::shareDenyRead, NULL);
     DWORD dwSize = pPostTop.GetLength() + ender.GetLength() + (DWORD)cfile.GetLength();
     pFile->SendRequestEx(dwSize);
     pFile->Write(pPostTopbytes, pPostTop.GetLength());
	 

     //��������
     int bufflength = 4 * 1024;
     byte* buffer = new byte[bufflength];
     int byteRead = 0;


	 // �ϴ�����
	 int pos = 0;          // ��ǰ��������λ��
	 CString ProgressText; // ����������ʾ
	 SetDlgItemText(IDC_STATUS_STATIC, _T("���ڽ��д������..."));

     while((byteRead = cfile.Read(buffer , bufflength)) != 0)
     {
		 // �����µĽ�����λ��
		 pos = pos + byteRead;                
         pFile->Write(buffer , byteRead);

		 //���½���
		 m_Progress.SetRange32(0, byteRead);
		 m_Progress.SetPos(pos);

		 ProgressText.Format(_T("������ (����: %s / %s)"), OnGetSize(pos), OnGetSize(byteRead));
		 SetDlgItemText(IDC_STATUS_STATIC, ProgressText);
	 }
     cfile.Close();


     //дβ
     pFile->Write(enderbyte, ender.GetLength());
     pFile->EndRequest();

     CString strSentence = _T(""), strGetSentence = _T("");
     DWORD dwRet;

     pFile->QueryInfoStatusCode(dwRet);
     if(HTTP_STATUS_OK == dwRet)
     {
		 // ��ȡ�ύ���ݺ�ķ��ؽ��
         while(pFile->ReadString(strSentence))
         {
              strGetSentence = strGetSentence+ strSentence;
         }
     }

     pFile->Close();
     pConnect->Close();

	 // ����ϴ�
	 // �õ��ϴ�·��
	 UploadPath = strGetSentence;

	 // �ֲ�����
	 CString RecvData, Parameter, UserId, Parent, Sub, Item, Temp;
	 BOOL IsSuccess;

	 if(IsProject)
	 {
		  // �滻
		 TargetPath.Replace(_T("./Cache/"), _T("/Project/"));
	 }

	 Temp   = TargetPath.Right(TargetPath.GetLength() - TargetPath.Find('/') -1);
	 UserId = TargetPath.Left(TargetPath.GetLength() - Temp.GetLength() -1);

	 // ����
	 if(IsCode)
	 {
		 Parent = Temp.Left(Temp.GetLength() - Temp.Find('/') );
		 Sub    = Temp.Right(Temp.GetLength() - Parent.GetLength() -1);
	 }
	 else if(!IsCode && !IsProject)
	 {
		 // �ļ�
		 Temp   = Temp.Right(Temp.GetLength() - Temp.Find('/') -1);
		 Item   = Temp.Right(Temp.GetLength() - Temp.ReverseFind('/') -1);

		 Temp   = Temp.Left(Temp.GetLength() - Item.GetLength() -1);
		 Sub    = Temp.Right(Temp.GetLength() - Temp.ReverseFind('/') -1);
		 Parent = _T("File/") + Temp.Left(Temp.GetLength() - Sub.GetLength() -1);
	 }
	 else
	 {
		 if(Temp.Replace(_T("master"), _T("master")) || Temp.Replace(_T("hotfix"), _T("hotfix")) )
		 {
			 // ��Ŀ
			 Parent = Temp.Left(Temp.GetLength()  - Temp.Find('/'));
			 Sub    = Temp.Right(Temp.GetLength() - Parent.GetLength() -1);
		 }
		 else
		 {
			 // ��Ŀ
			 Parent = Temp.Left(Temp.GetLength()  - Temp.Find('/') -1);
			 Sub    = Temp.Right(Temp.GetLength() - Parent.GetLength() -1);
		 }
	 }
	 
	 // ������ֵ
	 if(IsCode || IsProject)
		 Parameter.Format(_T("path=%s&target=%s&name=%s&id=%s&parent=%s&sub=%s"), UploadPath, TargetPath, UploadName, UserId, Parent, Sub);
	 else
		 Parameter.Format(_T("path=%s&target=%s&name=%s&id=%s&parent=%s&sub=%s&item=%s"), UploadPath, TargetPath, UploadName, UserId, Parent, Sub, Item);

	 // ��ȡ����������
	 try
	 {
		 RecvData = theApp.OnGetWebInfo(_T("www.shadowviolet.cn"), _T("index/account/ReMoveFile"), 80, Parameter, IsSuccess);
		 if (RecvData == _T("") || RecvData.IsEmpty() || !IsSuccess)
		 {
			 AfxMessageBox(_T("�޷����ӵ�������, �������硣"));
		 }
		 else
		 {
			 if (IsSuccess)
			 {
				 if( RecvData.Replace(_T("Success"), _T("Success")) )
				 {
					 // ���½�����Ϣ
					 SetDlgItemText(IDOK, _T("��ʼ"));
					 SetDlgItemText(IDCANCEL, _T("�ر�"));

					 SetDlgItemText(IDC_STATUS_STATIC, _T("������������."));
					 GetDlgItem(IDOK)->EnableWindow(false);
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
		 AfxMessageBox(_T("�������쳣��λ��CTransmissionDlg��HttpPostFile������"));
	 }
}


UINT CTransmissionDlg::DownloadFile(LPVOID pParam)
{
	// ����ָ��
	CTransmissionDlg * pWnd = (CTransmissionDlg*)pParam;

	// �����ٽ���
	EnterCriticalSection(pWnd->m_cs);

	// �ֲ�����
	int Percentage = 0;
	bool isfinished = false;
	CStringA Path = (CStringA)pWnd->DownloadPath, Name = (CStringA)pWnd->DownloadName;
	CString ProgressText;
	
	// ��ʾ����Ŀ��
	pWnd->SetDlgItemText(IDC_TARGET_STATIC, pWnd->DownloadName);

	// ��ʾ����״̬
	pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("����У�����ز���..."));

	// ��ʼ����Ŀ��
	pWnd->GetDlgItem(IDOK)->EnableWindow();
	if(!pWnd->m_pCurlDownloader->Start(Path.GetBuffer(), Name.GetBuffer()))
	{
		// ���´������
		pWnd->Target++;

		// ���½�����Ϣ
		pWnd->SetDlgItemText(IDOK, _T("��ʼ"));
		pWnd->SetDlgItemText(IDCANCEL, _T("�ر�"));

		pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("������������."));
		pWnd->GetDlgItem(IDOK)->EnableWindow(false);

		// �̶߳�������Ϊ��
		pWnd->m_Thread   = NULL;
		pWnd->IsFinished = true;

		// ��ʾ����
		pWnd->Error = _T("���ز���У��ʧ�ܣ�������Ŀ���СΪ����ļ���Ϣ����");
		pWnd->PostMessage(WM_COMMAND, 101);

		// �뿪�ٽ���
		LeaveCriticalSection(pWnd->m_cs);
		return false;
	}

	// ���Ŀ������
	unsigned long  x = 0, y = 0;
	pWnd->m_pCurlDownloader->GetProgress(&x, &y);
	CString Ext = PathFindExtension(pWnd->DownloadName);

	// ���Ŀ����Ϣ����
	if(x <= 0)
	{
		// ���´������
		pWnd->Target++;

		// ���½�����Ϣ
		pWnd->SetDlgItemText(IDOK, _T("��ʼ"));
		pWnd->SetDlgItemText(IDCANCEL, _T("�ر�"));

		pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("������������."));
		pWnd->GetDlgItem(IDOK)->EnableWindow(false);

		// �̶߳�������Ϊ��
		pWnd->m_Thread   = NULL;
		pWnd->IsFinished = true;

		// ��ʾ����
		pWnd->Error = _T("Ŀ���ļ�Ϊ�գ��������ء�");
		pWnd->PostMessage(WM_COMMAND, 101);

		// �뿪�ٽ���
		LeaveCriticalSection(pWnd->m_cs);
		return false;
	}
	if(Ext == _T(".") || Ext.IsEmpty())
	{
		// ���´������
		pWnd->Target++;

		// ���½�����Ϣ
		pWnd->SetDlgItemText(IDOK, _T("��ʼ"));
		pWnd->SetDlgItemText(IDCANCEL, _T("�ر�"));

		pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("������������."));
		pWnd->GetDlgItem(IDOK)->EnableWindow(false);

		// �̶߳�������Ϊ��
		pWnd->m_Thread   = NULL;
		pWnd->IsFinished = true;

		// ��ʾ����
		pWnd->Error = _T("Ŀ���ļ���չ������ȷ���������ء�");
		pWnd->PostMessage(WM_COMMAND, 101);

		// �뿪�ٽ���
		LeaveCriticalSection(pWnd->m_cs);
		return false;
	}

	// ���ý�����
	pWnd->m_Progress.SetRange32(0, x);
	pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("����ִ�д������..."));

	while (!isfinished)
	{
		if (!pWnd->IsPause)
		{
			pWnd->m_pCurlDownloader->CheckIsFinish(&isfinished);
			pWnd->m_pCurlDownloader->GetCurlDownState();

			pWnd->m_pCurlDownloader->GetProgress(&x, &y);
			ProgressText.Format(_T("������ (����: %s / %s)"), pWnd->OnGetSize(y), pWnd->OnGetSize(x));
			pWnd->SetDlgItemText(IDC_STATUS_STATIC, ProgressText);

			// ������
			pWnd->m_Progress.SetPos(y);
			Sleep(30);
		}
		else
		{
			CString Status;
			pWnd->GetDlgItemText(IDC_STATUS_STATIC, Status);
			if (pWnd->IsPause && Status != _T("�������û���ͣ."))
			{
				pWnd->SetDlgItemText(IDOK, _T("����"));
				pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("�������û���ͣ."));
			}
		}

		if (pWnd->IsTerminate)
		{
			// ����/��ֹ�߳�
			if (pWnd->m_pCurlDownloader->Pause() && pWnd->m_pCurlDownloader->Terminate())
			{
				// �ع�������
				pWnd->m_Progress.SetPos(0);

				// �޸İ�ť����
				pWnd->SetDlgItemText(IDCANCEL, _T("�ر�"));
				pWnd->SetDlgItemText(IDOK, _T("��ʼ"));

				// �޸İ�ť״̬
				pWnd->GetDlgItem(IDOK)->EnableWindow();

				// �޸Ĳ���״̬
				pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("�������û�ȡ��."));

				// �̶߳�������Ϊ��
				pWnd->m_Thread   = NULL;
				pWnd->IsFinished = false;

				// ��ֹ��������Ϊ��
				pWnd->IsTerminate = false;

				// ��ֹѭ��
				break;
			}
		}

		// ���������
		if (isfinished)
		{
			// �ȴ����
			pWnd->m_pCurlDownloader->WaitForFinish();

			// ���½�����Ϣ
			pWnd->SetDlgItemText(IDOK, _T("��ʼ"));
			pWnd->SetDlgItemText(IDCANCEL, _T("�ر�"));

			pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("������������."));
			pWnd->GetDlgItem(IDOK)->EnableWindow(false);
		}
	}
	
	// ���´������
	pWnd->Target++;
	pWnd->PostMessage(WM_COMMAND, 100);

	// �̶߳�������Ϊ��
	pWnd->m_Thread   = NULL;
	pWnd->IsFinished = true;

	// �뿪�ٽ���
	LeaveCriticalSection(pWnd->m_cs);
	return TRUE;
}


UINT CTransmissionDlg::UploadFile(LPVOID pParam)
{
	// ����ָ��
	CTransmissionDlg * pWnd = (CTransmissionDlg*)pParam;

	// ��ȡ����������
	try
	{
		pWnd->IsFinished = false;

		// ��ʾ����Ŀ��
		pWnd->SetDlgItemText(IDC_TARGET_STATIC, pWnd->UploadPath);

		// ��ʾ����״̬
		pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("����У���ϴ�����..."));

		// �ж��ϴ�����
		pWnd->HttpPostFile("www.shadowviolet.cn", _T("/index/account/UpLoadFile"), 80, pWnd->UploadPath, "file", "multipart/form-data");
		/*if(pWnd->IsCode)
		{
			pWnd->HttpPostFile("www.shadowviolet.cn", _T("/index/account/UpLoadFile"), 80, pWnd->UploadPath, "file", "application/rtf");
		}
		else
		{
			pWnd->HttpPostFile("www.shadowviolet.cn", _T("/index/account/UpLoadFile"), 80, pWnd->UploadPath, "file", "multipart/form-data");
		}*/

		// �����ٽ���
		EnterCriticalSection(pWnd->m_cs);
	}
	catch (...)
	{
		pWnd->Error = _T("�������쳣��λ��CTransmissionDlg��UploadFile������");
		pWnd->PostMessage(WM_COMMAND, 101);
	}

	// �̶߳�������Ϊ��
	pWnd->m_Thread   = NULL;
	pWnd->IsFinished = true;

	// ���´������
	pWnd->Target++;
	pWnd->PostMessage(WM_COMMAND, 100);

	// �뿪�ٽ���
	LeaveCriticalSection(pWnd->m_cs);
	return true;
}


UINT CTransmissionDlg::Decompression(LPVOID pParam)
{
	// ����ָ��
	CTransmissionDlg * pWnd = (CTransmissionDlg*)pParam;

	// ��ʾ����״̬
	pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("���ڽ�ѹ��Ŀ..."));
	pWnd->GetDlgItem(IDCANCEL)->EnableWindow(false);

	// ��ѹѹ���ļ�
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

	// ��ʾ����״̬
	pWnd->SetDlgItemText(IDC_STATUS_STATIC, _T("������������."));
	pWnd->GetDlgItem(IDCANCEL)->EnableWindow();

	return 1;
}


void CTransmissionDlg::Prepare()
{
	// ���·��
	LPCTSTR str = TargetList->GetAt(Target); //��CString����ת��Ϊchar* ����
	for (int i=0;str[i];i++)
	{
		if (str[i] < 0) //һ������ռ�����ֽڣ���ÿ���ֽڶ���С��0��
		{
			CString tmp;
			tmp.Format("%c%c",str[i],str[i+1]);//������������ٸ�����
			i++;

			AfxMessageBox(_T("Ŀ��·�����ܺ�������!"));
			CDialog::OnCancel();
			return;
		}
	}

	CString Check(TargetList->GetAt(Target));
	if(Check.Replace(_T(" "), _T("")) || Check.Replace(_T(" "), _T("")))
	{
		AfxMessageBox(_T("Ŀ��·�����ܺ��пո�!"));

		CDialog::OnCancel();
		return;
	}

	if(Check.Replace(_T("!"), _T("")) || Check.Replace(_T("@"), _T("")) || Check.Replace(_T("#"), _T("")) || Check.Replace(_T("$"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("%"), _T("")) || Check.Replace(_T("^"), _T("")) || Check.Replace(_T("����"), _T("")) || Check.Replace(_T("&"), _T("")) || Check.Replace(_T("*"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("("), _T("")) || Check.Replace(_T(")"), _T("")) || Check.Replace(_T("_"), _T("")) || Check.Replace(_T("--"), _T("")) || Check.Replace(_T("+"), _T("")) || Check.Replace(_T("="), _T("")) || Check.Replace(_T(","), _T("")) || Check.Replace(_T(".."), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("//"), _T("")) || Check.Replace(_T("?"), _T("")) || Check.Replace(_T(";"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("'"), _T("")) || Check.Replace(_T(":"), _T("")) || Check.Replace(_T("\""), _T("")) || Check.Replace(_T("|"), _T("")) || Check.Replace(_T("\\"), _T("")) || Check.Replace(_T("["), _T("")) || Check.Replace(_T("{"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("}"), _T("")) || Check.Replace(_T("]"), _T("")) || Check.Replace(_T("`"), _T("")) || Check.Replace(_T("��"), _T("")) || Check.Replace(_T("~"), _T("")) || Check.Replace(_T("\r"), _T("")) || Check.Replace(_T("\n"), _T("")))
	{
		AfxMessageBox(_T("Ŀ��·�����ܺ��з���!"));

		CDialog::OnCancel();
		return;
	}


	// ����
	if(IsDownload)
	{
		// ��������
		DownloadPath = _T("http://www.shadowviolet.cn/cloud/") + TargetList->GetAt(Target);
		DownloadName = _T("./Cache/") + DownloadPath.Right(DownloadPath.GetLength() - DownloadPath.ReverseFind('/') -1);

		if(IsUpDate)
		{
			DownloadPath = _T("http://www.shadowviolet.cn/update/") + TargetList->GetAt(Target);
			DownloadName = _T("./Cache/") + TargetList->GetAt(Target);
		}

		// ��ʼ����
		if (m_Thread == NULL)
		{
			m_Thread = AfxBeginThread(DownloadFile, this);
		}
	}
	else // �ϴ�
	{
		// ��������
		UploadPath = TargetPath = TargetList->GetAt(Target);
		UploadPath = UploadPath.Right(UploadPath.GetLength() - UploadPath.Find('/') -1);
		UploadName = UploadPath.Right(UploadPath.GetLength() - UploadPath.ReverseFind('/') -1);
		TargetPath = TargetPath.Left(TargetPath.GetLength() - UploadName.GetLength() -1);

		// ��ʼ����
		if (m_Thread == NULL)
		{
			m_Thread = AfxBeginThread(UploadFile, this);
		}
	}
}


void CTransmissionDlg::Complete()
{
	// ��ɴ���
	CString Path(DownloadPath);
	Path.Replace(_T("http://www.shadowviolet.cn/cloud/"), _T(""));
	
	// �ֲ�����
	CStringArray TextArray;
	Split(Path, _T("/"), TextArray);

	// ����ģʽ
	if(IsDownload)
	{
		// ����Ǵ���
		if(IsCode)
		{
			CString Parent = TextArray.GetAt(2), Sub = TextArray.GetAt(3);

			// Ŀ¼���
			CString FilePath = _T("./Code/") + Parent;
			if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //�ļ��в�����
			{
				CreateDirectory(FilePath, NULL);
			}

			FilePath = _T("./Code/") + Parent + _T("/") + Sub;
			if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //�ļ��в�����
			{
				CreateDirectory(FilePath, NULL);
			}

			// �ƶ��ļ�
			CString Name(DownloadName);
			Name.Replace(_T("./Cache/"), _T(""));
			FilePath = _T("./Code/") + Parent + _T("/") + Sub + _T("/") + Name;

			if(!MoveFileEx(DownloadName, FilePath, MOVEFILE_REPLACE_EXISTING))
			{
				AfxMessageBox(_T("�ƶ�Ŀ��ʧ��!"));
			}
		}
		else if(IsProject)
		{
			// ��ѹ��
			AfxBeginThread(Decompression, this);
		}
		else if(IsUpDate)
		{
			// �õ��ļ���
			CString FileName = DownloadName;
			FileName.Replace(_T("./Cache/"), _T(""));

			// �ƶ��ļ�
			if(!MoveFileEx(DownloadName, FileName, MOVEFILE_REPLACE_EXISTING))
			{
				AfxMessageBox(_T("�ƶ�Ŀ��ʧ��!"));
				return;
			}

			// ��ѹ����
			CString Parament = _T("Application\\7z x -t7z -y ") + FileName;

			// �ȴ�������ر�֮��ִ�н�ѹ
			CString strParam = _T("/C choice /t 0.5 /d y /n > nul & tasklist | find /i \"�������.exe\" && taskkill /im �������.exe /f || echo NotFind & ");
			strParam += Parament;
			strParam += _T(" & del ") + FileName;
			strParam += _T(" & Start �������.exe & pause");

			// ���������� ִ�и���
			ShellExecute(NULL, NULL, _T("cmd.exe"), strParam, NULL, SW_HIDE);

			// ������������
			CDialogEx::OnCancel();
			::SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_COMMAND, 102, 0);
		}
		else
		{
			CString Parent = TextArray.GetAt(2), Sub = TextArray.GetAt(3), Item = TextArray.GetAt(4);

			// Ŀ¼���
			CString FilePath = _T("./File/") + Parent;
			if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //�ļ��в�����
			{
				CreateDirectory(FilePath, NULL);
			}

			FilePath = _T("./File/") + Parent + _T("/") + Sub;
			if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //�ļ��в�����
			{
				CreateDirectory(FilePath, NULL);
			}

			FilePath = _T("./File/") + Parent + _T("/") + Sub + _T("/") + Item;
			if (GetFileAttributes(FilePath) == 0xFFFFFFFF)     //�ļ��в�����
			{
				CreateDirectory(FilePath, NULL);
			}

			// �ƶ��ļ�
			CString Name(DownloadName);
			Name.Replace(_T("./Cache/"), _T(""));
			FilePath = _T("./File/") + Parent + _T("/") + Sub + _T("/") + Item + _T("/") + Name;

			if(!MoveFileEx(DownloadName, FilePath, MOVEFILE_REPLACE_EXISTING))
			{
				AfxMessageBox(_T("�ƶ�Ŀ��ʧ��!"));
			}
		}
	}
	else
	{
		// ������
		if(IsProject)
		{
			CString Temp = TargetPath.Right(TargetPath.GetLength() - TargetPath.Find('/') -1), Parent = Temp.Left(Temp.GetLength() - Temp.Find('/'));
			Parent.Replace(_T("Project"), _T("Cache"));

			CProjectDlg::DeleteDirectory(_T("./") + Parent);
		}
	}
	

	// �ж��Ƿ���Ҫ����
	if(Total -1 >= Target)
	{
		// ׼������
		Prepare();
	}
	else
	{
		// �Զ��ر�
		if(IsSynchronize)
		{
			CDialogEx::OnCancel();
		}
	}
}


void CTransmissionDlg::OnError()
{
	// ��ʾ����
	AfxMessageBox(Error);

	// �ж��Ƿ���Ҫ����
	if(Total -1 >= Target)
	{
		// ׼������
		Prepare();
	}
}


void CTransmissionDlg::OnOK()
{
	// û����ͣ
	if(!IsPause)
	{
		if (m_pCurlDownloader->Pause())
		{
			IsPause = true;
			GetDlgItem(IDOK)->SetWindowText(_T("����"));
		}
	}
	else
	{
		if (m_pCurlDownloader->Resume())
		{
			IsPause = false;
			GetDlgItem(IDOK)->SetWindowText(_T("��ͣ"));
		}
	}
}


void CTransmissionDlg::OnCancel()
{
	if (m_Thread != NULL && !IsFinished && !IsTerminate)
	{
		SetDlgItemText(IDC_STATUS_STATIC, _T("����ȡ������."));
		IsTerminate = true;
		IsFinished = false;
	}
	else if (IsTerminate)
	{
		IsFinished = false;
		return;
	}
	else
	{
		if (IsFinished)
		{
			// ��ֹ�߳�
			m_pCurlDownloader->Terminate();
		}

		CDialogEx::OnCancel();
	}
}
