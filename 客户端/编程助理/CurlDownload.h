#pragma once

#include <string>

//下载状态
enum CurlDownState{DOWN_PROGRESS, DOWN_PAUSE, DOWN_TERMINATE};

//日志函数
typedef void (*LPLOGFUN)(const std::string& strLog);

//下载设置
typedef struct _tCurlDownConfig{
	int iMaxThreadCount; //单个任务在支持多线程下载情况下最多开的线程数目
	int iMaxDownloadCount; //最多并行的下载个数
	int iTimeOut;        //下载过程中的超时时间(s)
	int iMaxTryTimes;    //下载线程失败最大重试次数
	int iMinBlockSize;   //多线程下载时，最小分块大小(B)
	LPLOGFUN pLogFun;
	std::string szSSLCrtName; //SSL证书名字
	std::string szSSLKeyName; //SSL证书密钥
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

	// 全局
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//全局初始化
	void  InitCurlDownloader();
	void UninitCurlDownloader();

	//下载设置
	void  SetCurlDownConfig(const CurlDownConfig& downCfg);
	CurlDownloader*  CreateCurlDownloader();
	void  DeleteCurlDownloader(CurlDownloader** downloader);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//开始下载
	virtual bool Start(const std::string&strUrl, const std::string&strDownloadPath, 
		int* pThreadCount = NULL) = 0;

	//暂停
	virtual bool Pause() = 0;

	//恢复下载，支持断点续传
	virtual bool Resume() = 0;

	//终止
	virtual bool Terminate(bool bDeleteFile = true) = 0;

	//阻塞等待下载完
	virtual bool WaitForFinish() = 0;

	//获取下载进度
	virtual void GetProgress(unsigned long* pTotalFileSize, unsigned long* pDownSize) = 0;

	//是否是多线程下载
	virtual bool IsMutliDownload() = 0;
	virtual bool IsRedirected() = 0;

	//检测是否完成下载
	virtual bool CheckIsFinish(bool* pbThrdRunFinish) = 0;

	//获取当前状态
	virtual CurlDownState GetCurlDownState() = 0;

	//设置代理
	virtual void SetProxy(const std::string& strProxy) = 0;

	//获取错误码
	virtual int GetLastHttpCode() = 0;

	//获取CURL错误码
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
#define GB2312 1 //发送和接收的网页编码
#define UTF8   2

	/**
	* @brief HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strPost 输入参数,使用如下格式para1=val1&para2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	virtual int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse, int SendCode = UTF8, int RecvCode = GB2312) = 0;

	/**
	* @brief HTTP GET请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	virtual int Get(const std::string & strUrl, std::string & strResponse, int SendCode = UTF8, int RecvCode = GB2312) = 0;

	/**
	* @brief HTTPS POST请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strPost 输入参数,使用如下格式ppara1=val1&para2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	virtual int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL, int SendCode = UTF8, int RecvCode = GB2312) = 0;

	/**
	* @brief HTTPS GET请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	virtual int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL, int SendCode = UTF8, int RecvCode = GB2312) = 0;
};