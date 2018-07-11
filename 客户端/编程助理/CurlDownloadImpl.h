#pragma once

#include "CurlDownload.h"
#include <vector>

typedef void CURL;
typedef void CURLSH;
struct _tThreadInfo;
typedef struct _tThreadInfo ThreadInfo;
struct pthread_mutex_t_;
typedef struct pthread_mutex_t_ * pthread_mutex_t;

#pragma warning(disable:4251)


class MyLog{
public:
	MyLog(LPLOGFUN pLogFun, const char* szFileName, 
		const char* szFuncName, long lLine)
		: m_pLogFun(pLogFun) 
	{
		std::string strFileName(szFileName ? szFileName : "");
		int pos = strFileName.rfind('\\');
		if (pos != -1) strFileName = strFileName.substr(pos + 1, strFileName.size() - pos - 1);
		m_szFileName = strFileName;
		m_szFuncName = std::string(szFuncName ? szFuncName : "");
		m_lLine = lLine;
	};
	~MyLog(){};
	void operator<<(const std::string& strLog)
	{
		if (m_pLogFun) 
		{
			char ch[512]={0};
			sprintf_s(ch, "%s:%d %s(): ", m_szFileName.c_str(), m_lLine,
				m_szFuncName.c_str());
			std::string strInfo = std::string(ch) + strLog;
			m_pLogFun(strInfo);
		}
	}
private:
	LPLOGFUN m_pLogFun;
	std::string m_szFileName, m_szFuncName;
	long m_lLine;
};


class CurlDownloaderImpl : public CurlDownloader
{
public:
	CurlDownloaderImpl(void);
	~CurlDownloaderImpl(void);


	//ȫ�ֳ�ʼ��
	static void Init();
	static void Uninit();

	//��������
	static void SetCurlDownConfig(const CurlDownConfig& downCfg);
	static CurlDownConfig GetCurlDownConfig();

	//��ʼ����
	bool Start(const std::string&strUrl, const std::string&strDownloadPath, 
		int* pThreadCount = NULL);

	//��ͣ
	bool Pause();

	//�ָ����أ�֧�ֶϵ�����
	bool Resume();

	//��ֹ
	bool Terminate(bool bDeleteFile = true);

	//�����ȴ�������
	bool WaitForFinish();

	//��ȡ���ؽ���
	void GetProgress(unsigned long* pTotalFileSize, unsigned long* pDownSize);

	//�Ƿ��Ƕ��߳�����
	bool IsMutliDownload();
	bool IsRedirected();

	//����Ƿ��������
	bool CheckIsFinish(bool* pbThrdRunFinish);

	//��ȡ��ǰ״̬
	CurlDownState GetCurlDownState();

	//���ô���
	void SetProxy(const std::string& strProxy);

	//��ȡ������
	int GetLastHttpCode();

	//��ȡCURL������
	int GetLastCurlCode();
	std::string GetRealUrl();
	int GetThreadCount();
	bool IsDownloadInitOver();
	bool IsDownloadInitInterrupt();
	void SetPause(bool bPause);
	void SetMaxRetryTimes(int iTimes);


	// FTP
	/////////////////////////////////////////////////////////////////////////
	int FTP_UpLoad  (CURL *curlhandle, const std::string remotepath, const std::string localpath, long timeout, long tries);
	int FTP_DownLoad(CURL *curlhandle, const std::string remotepath, const std::string localpath, long timeout, long tries);


	// Post
	/////////////////////////////////////////////////////////////////////////
#define GB2312 1 //���ͺͽ��յ���ҳ����
#define UTF8   2

	/**
	* @brief HTTP POST����
	* @param strUrl �������,�����Url��ַ,��:http://www.baidu.com
	* @param strPost �������,ʹ�����¸�ʽpara1=val1&para2=val2&��
	* @param strResponse �������,���ص�����
	* @return �����Ƿ�Post�ɹ�
	*/
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse, int SendCode = UTF8, int RecvCode = GB2312);

	/**
	* @brief HTTP GET����
	* @param strUrl �������,�����Url��ַ,��:http://www.baidu.com
	* @param strResponse �������,���ص�����
	* @return �����Ƿ�Post�ɹ�
	*/
	int Get(const std::string & strUrl, std::string & strResponse, int SendCode = UTF8, int RecvCode = GB2312);

	/**
	* @brief HTTPS POST����,��֤��汾
	* @param strUrl �������,�����Url��ַ,��:https://www.alipay.com
	* @param strPost �������,ʹ�����¸�ʽppara1=val1&para2=val2&��
	* @param strResponse �������,���ص�����
	* @param pCaPath �������,ΪCA֤���·��.�������ΪNULL,����֤��������֤�����Ч��.
	* @return �����Ƿ�Post�ɹ�
	*/
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL, int SendCode = UTF8, int RecvCode = GB2312);

	/**
	* @brief HTTPS GET����,��֤��汾
	* @param strUrl �������,�����Url��ַ,��:https://www.alipay.com
	* @param strResponse �������,���ص�����
	* @param pCaPath �������,ΪCA֤���·��.�������ΪNULL,����֤��������֤�����Ч��.
	* @return �����Ƿ�Post�ɹ�
	*/
	int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL, int SendCode = UTF8, int RecvCode = GB2312);


protected:
	//���س�ʼ��
	bool DownloadInit();

	//curl��ʼ��
	static bool CurlInit(void* param);

	//���غ���
	static void DownloadFun(void* param);

	// ͷ��Ϣ 
	static size_t HeaderInfo(char *ptr, size_t size, size_t nmemb, void *userdata);

	//д����
	static size_t WriteData(char *ptr, size_t size, size_t nmemb, void *userdata);

	//�����̲߳���
	static void ParseThreadParam(void* param, CurlDownloaderImpl** ppDown, int* pIndex);
	static void FreeThreadParam(void* param);

	//ͨ����ʱ�ļ����жϵ�����
	bool ContinueDownloadByTmpFile();

	//�����
	bool FinalProc();

	//������ʱ�ļ���ָ���߳̿�������Ϣ
	void UpdateDownloadInfoInTmpFile(int* pIndex);

	//�����߳���Ϣ
	void ClearThreadInfo();

	//���SSL
	static bool CheckSSL(CURL* pCurl);

	//����ļ�����
	bool CheckFileLength();

	//����Ƿ�֧�ַ�Ƭ����
	bool CheckIsSupportRange();
	bool CheckIsSupportRangeEx();

	// ����curl����
	static CURL *Create_Share_Curl();

	// ʱ�����
	void SleepEx(unsigned long ulMilliseconds);
	int GetMaxRetryTimes();


	// FTP
	CString UserPwd;
	static int progress_func(void *ptr, double rDlTotal, double rDlNow, double rUlTotal, double rUlNow);

	static size_t GetContentLengthFunc(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t CurlDownloaderImpl::DisCardFunc(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t CurlDownloaderImpl::writefunc(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t CurlDownloaderImpl::readfunc(void *ptr, size_t size, size_t nmemb, void *stream);

private:
	//�ļ�
	FILE* m_pFile;

	//���ڿ���д�ļ��Ļ������
	pthread_mutex_t m_mutexFile;

	//���ڱ���ÿ���̵߳���Ϣ������
	std::vector<ThreadInfo*> m_vecThrdInfo;

	//���ص�ַ�����ر���·��
	std::string m_strUrl, m_strDownloadPath;

	//����
	std::string m_strProxy;

	//�Ƿ���ͣ
	bool m_bPause;

	//�Ƿ���ֹ����
	bool m_bTerminate;

	//�Ƿ�֧�ֶ��߳�����
	bool m_bSupportMultiDown;

	//Ҫ���ص��ļ���С
	unsigned long m_ulFullFileSize;

	//�Ѿ����ص��ļ���С
	unsigned long m_ulDownFileSize;

	//HTTP������
	int m_iHttpCode;
	int m_curlCode;

	//�Ƿ��ض�������ض���֮ǰ��URL
	bool m_bRedirected;
	std::string m_strOriginalUrl;

	// ����dns�������
	static CURLSH* sharedns_handle;
	static CurlDownConfig g_curlDownCfg;
	bool m_bInitOver;
	bool m_bInitInterrupt;
	int m_iMaxRetryTimes;
};


enum ParamType{RANGE_CHECK, DOWNLOAD_THREAD};


struct ParamBase{ParamType type;};


struct RangeParam : public ParamBase{
	int nRangeTestSize;
	int nRecvSize;
	RangeParam(){
		type = RANGE_CHECK;
		nRecvSize = nRangeTestSize = 0;
	}
};


struct DownThreadParam : public ParamBase{
	CurlDownloaderImpl* pDownloader;
	unsigned int index;
	DownThreadParam(){
		type = DOWNLOAD_THREAD;
		pDownloader = NULL;
		index = -1;
	}
};
