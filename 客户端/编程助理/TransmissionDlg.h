#pragma once

#include "CurlDownload.h"   // ������
#include "afxcmn.h"

// TransmissionDlg �Ի���

class CTransmissionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTransmissionDlg)

public:
	CTransmissionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTransmissionDlg();

	// ����ģʽ��ʶ&��������ʶ
	BOOL IsDownload, IsCode, IsProject, IsUpDate, IsSynchronize;

	// �����������
	CurlDownloader * m_pCurlDownloader;
	CurlDownloader * m_Downloader;

	// ȫ�ֱ���
	CWinThread* m_Thread;
	BOOL IsPause, IsFinished, IsTerminate;
	CProgressCtrl m_Progress;
	CString DownloadPath, DownloadName, Error, UploadPath, UploadName, TargetPath;
	CStringArray *TargetList;

	// ����Ŀ��
	int Total, Target;

	//�ٽ��� �̼߳���  CCriticalSection�Ƕ��ٽ����ķ�װ��  
	CCriticalSection  m_cs;

	// �������߳�
	static UINT DownloadFile(LPVOID pParam);
	static UINT UploadFile(LPVOID pParam);
	static UINT Decompression(LPVOID pParam);

// �Ի�������
	enum { IDD = IDD_TRANSMISSION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
