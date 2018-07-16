#pragma once

#include "CurlDownload.h"   // 下载类
#include "afxcmn.h"

// TransmissionDlg 对话框

class CTransmissionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTransmissionDlg)

public:
	CTransmissionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTransmissionDlg();

	// 传输模式标识&传输对象标识
	BOOL IsDownload, IsCode, IsProject, IsUpDate, IsSynchronize;

	// 下载组件对象
	CurlDownloader * m_pCurlDownloader;
	CurlDownloader * m_Downloader;

	// 全局变量
	CWinThread* m_Thread;
	BOOL IsPause, IsFinished, IsTerminate;
	CProgressCtrl m_Progress;
	CString DownloadPath, DownloadName, Error, UploadPath, UploadName, TargetPath;
	CStringArray *TargetList;

	// 传输目标
	int Total, Target;

	//临界区 线程加锁  CCriticalSection是对临界区的封装类  
	CCriticalSection  m_cs;

	// 工作者线程
	static UINT DownloadFile(LPVOID pParam);
	static UINT UploadFile(LPVOID pParam);
	static UINT Decompression(LPVOID pParam);

// 对话框数据
	enum { IDD = IDD_TRANSMISSION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CString OnGetSize(int nSize);
	void Prepare();
	void Complete();
	void Split(CString source, CString divKey, CStringArray &dest);

	void OnError();
	void HttpPostFile( CString ServerName, CString ServerPath, int ServerPort, CString File, CString ParamName, CString ContentType );

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
