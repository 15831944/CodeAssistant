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


	//全局初始化
	static void Init();
	static void Uninit();

	//下载设置
	static void SetCurlDownConfig(const CurlDownConfig& downCfg);
	static CurlDownConfig GetCurlDownConfig();

	//开始下载
	bool Start(const std::string&strUrl, const std::string&strDownloadPath, 
		int* pThreadCount = NULL);

	//暂停
	bool Pause();

	//恢复下载，支持断点续传
	bool Resume();

	//终止
	bool Terminate(bool bDeleteFile = true);

	//阻塞等待下载完
	bool WaitForFinish();

	//获取下载进度
	void GetProgress(unsigned long* pTotalFileSize, unsigned long* pDownSize);

	//是否是多线程下载
	bool IsMutliDownload();
	bool IsRedirected();

	//检测是否完成下载
	bool CheckIsFinish(bool* pbThrdRunFinish);

	//获取当前状态
	CurlDownState GetCurlDownState();

	//设置代理
	void SetProxy(const std::string& strProxy);

	//获取错误码
	int GetLastHttpCode();

	//获取CURL错误码
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
#define GB2312 1 //发送和接收的网页编码
#define UTF8   2

	/**
	* @brief HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strPost 输入参数,使用如下格式para1=val1&para2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse, int SendCode = UTF8, int RecvCode = GB2312);

	/**
	* @brief HTTP GET请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Get(const std::string & strUrl, std::string & strResponse, int SendCode = UTF8, int RecvCode = GB2312);

	/**
	* @brief HTTPS POST请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strPost 输入参数,使用如下格式ppara1=val1&para2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL, int SendCode = UTF8, int RecvCode = GB2312);

	/**
	* @brief HTTPS GET请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL, int SendCode = UTF8, int RecvCode = GB2312);


protected:
	//下载初始化
	bool DownloadInit();

	//curl初始化
	static bool CurlInit(void* param);

	//下载函数
	static void DownloadFun(void* param);

	// 头信息 
	static size_t HeaderInfo(char *ptr, size_t size, size_t nmemb, void *userdata);

	//写数据
	static size_t WriteData(char *ptr, size_t size, size_t nmemb, void *userdata);

	//解析线程参数
	static void ParseThreadParam(void* param, CurlDownloaderImpl** ppDown, int* pIndex);
	static void FreeThreadParam(void* param);

	//通过临时文件进行断点续传
	bool ContinueDownloadByTmpFile();

	//最后处理
	bool FinalProc();

	//更新临时文件中指定线程块下载信息
	void UpdateDownloadInfoInTmpFile(int* pIndex);

	//清理线程信息
	void ClearThreadInfo();

	//检测SSL
	static bool CheckSSL(CURL* pCurl);

	//检测文件长度
	bool CheckFileLength();

	//检测是否支持分片传输
	bool CheckIsSupportRange();
	bool CheckIsSupportRangeEx();

	// 创建curl对象
	static CURL *Create_Share_Curl();

	// 时间相关
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
	//文件
	FILE* m_pFile;

	//用于控制写文件的互斥变量
	pthread_mutex_t m_mutexFile;

	//用于保存每个线程的信息的数组
	std::vector<ThreadInfo*> m_vecThrdInfo;

	//下载地址及本地保存路径
	std::string m_strUrl, m_strDownloadPath;

	//代理
	std::string m_strProxy;

	//是否暂停
	bool m_bPause;

	//是否中止下载
	bool m_bTerminate;

	//是否支持多线程下载
	bool m_bSupportMultiDown;

	//要下载的文件大小
	unsigned long m_ulFullFileSize;

	//已经下载的文件大小
	unsigned long m_ulDownFileSize;

	//HTTP错误码
	int m_iHttpCode;
	int m_curlCode;

	//是否重定向过，重定向之前的URL
	bool m_bRedirected;
	std::string m_strOriginalUrl;

	// 共享dns处理对象
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
