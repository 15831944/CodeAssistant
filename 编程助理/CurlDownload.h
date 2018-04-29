#pragma once

#include <string>

//����״̬
enum CurlDownState{DOWN_PROGRESS, DOWN_PAUSE, DOWN_TERMINATE};

//��־����
typedef void (*LPLOGFUN)(const std::string& strLog);

//��������
typedef struct _tCurlDownConfig{
	int iMaxThreadCount; //����������֧�ֶ��߳������������࿪���߳���Ŀ
	int iMaxDownloadCount; //��ಢ�е����ظ���
	int iTimeOut;        //���ع����еĳ�ʱʱ��(s)
	int iMaxTryTimes;    //�����߳�ʧ��������Դ���
	int iMinBlockSize;   //���߳�����ʱ����С�ֿ��С(B)
	LPLOGFUN pLogFun;
	std::string szSSLCrtName; //SSL֤������
	std::string szSSLKeyName; //SSL֤����Կ
	_tCurlDownConfig()
	{
		iMaxThreadCount = 5;
		iMaxDownloadCount = 5;
		iTimeOut = 60;
		iMaxTryTimes = 5;
		iMinBlockSize = 5*1024*1024;
		pLogFun = NULL;
	}
	_tCurlDownConfig& operator= (const _tCurlDownConfig& other)
	{
		if (this != &other)
		{
			iMaxThreadCount = other.iMaxThreadCount;
			iMaxDownloadCount = other.iMaxDownloadCount;
			iTimeOut = other.iTimeOut;
			iMaxTryTimes = other.iMaxTryTimes;
			iMinBlockSize = other.iMinBlockSize;
			pLogFun = other.pLogFun;
			szSSLKeyName = other.szSSLKeyName;
			szSSLCrtName = other.szSSLCrtName;
		}
		return *this;
	}
}CurlDownConfig;

class CurlDownloader
{
public:
	//
	virtual ~CurlDownloader() = 0;

	// ȫ��
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ȫ�ֳ�ʼ��
	void  InitCurlDownloader();
	void UninitCurlDownloader();

	//��������
	void  SetCurlDownConfig(const CurlDownConfig& downCfg);
	CurlDownloader*  CreateCurlDownloader();
	void  DeleteCurlDownloader(CurlDownloader** downloader);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//��ʼ����
	virtual bool Start(const std::string&strUrl, const std::string&strDownloadPath, 
		int* pThreadCount = NULL) = 0;

	//��ͣ
	virtual bool Pause() = 0;

	//�ָ����أ�֧�ֶϵ�����
	virtual bool Resume() = 0;

	//��ֹ
	virtual bool Terminate(bool bDeleteFile = true) = 0;

	//�����ȴ�������
	virtual bool WaitForFinish() = 0;

	//��ȡ���ؽ���
	virtual void GetProgress(unsigned long* pTotalFileSize, unsigned long* pDownSize) = 0;

	//�Ƿ��Ƕ��߳�����
	virtual bool IsMutliDownload() = 0;
	virtual bool IsRedirected() = 0;

	//����Ƿ��������
	virtual bool CheckIsFinish(bool* pbThrdRunFinish) = 0;

	//��ȡ��ǰ״̬
	virtual CurlDownState GetCurlDownState() = 0;

	//���ô���
	virtual void SetProxy(const std::string& strProxy) = 0;

	//��ȡ������
	virtual int GetLastHttpCode() = 0;

	//��ȡCURL������
	virtual int GetLastCurlCode() = 0;
	virtual std::string GetRealUrl() = 0;
	virtual int GetThreadCount() = 0;
	virtual bool IsDownloadInitOver() = 0;
	virtual bool IsDownloadInitInterrupt() = 0;
	virtual void SetPause(bool bPause) = 0;
	virtual void SetMaxRetryTimes(int iTimes) = 0;


	// FTP
	/////////////////////////////////////////////////////////////////////////
	//virtual int FTP_UpLoad  (CURL *curlhandle, const std::string remotepath, const std::string localpath, long timeout, long tries);
	//virtual int FTP_DownLoad(CURL *curlhandle, const std::string remotepath, const std::string localpath, long timeout, long tries);


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
	virtual int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse, int SendCode = UTF8, int RecvCode = GB2312) = 0;

	/**
	* @brief HTTP GET����
	* @param strUrl �������,�����Url��ַ,��:http://www.baidu.com
	* @param strResponse �������,���ص�����
	* @return �����Ƿ�Post�ɹ�
	*/
	virtual int Get(const std::string & strUrl, std::string & strResponse, int SendCode = UTF8, int RecvCode = GB2312) = 0;

	/**
	* @brief HTTPS POST����,��֤��汾
	* @param strUrl �������,�����Url��ַ,��:https://www.alipay.com
	* @param strPost �������,ʹ�����¸�ʽppara1=val1&para2=val2&��
	* @param strResponse �������,���ص�����
	* @param pCaPath �������,ΪCA֤���·��.�������ΪNULL,����֤��������֤�����Ч��.
	* @return �����Ƿ�Post�ɹ�
	*/
	virtual int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL, int SendCode = UTF8, int RecvCode = GB2312) = 0;

	/**
	* @brief HTTPS GET����,��֤��汾
	* @param strUrl �������,�����Url��ַ,��:https://www.alipay.com
	* @param strResponse �������,���ص�����
	* @param pCaPath �������,ΪCA֤���·��.�������ΪNULL,����֤��������֤�����Ч��.
	* @return �����Ƿ�Post�ɹ�
	*/
	virtual int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL, int SendCode = UTF8, int RecvCode = GB2312) = 0;
};