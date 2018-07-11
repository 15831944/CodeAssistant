#include "stdafx.h"
#include "CurlDownloadImpl.h"


CurlDownloader::~CurlDownloader()
{
}


//全局初始化
void  CurlDownloader::InitCurlDownloader()
{
	CurlDownloaderImpl::Init();
}


void  CurlDownloader::UninitCurlDownloader()
{
	CurlDownloaderImpl::Uninit();
}


//下载设置
void  CurlDownloader::SetCurlDownConfig(const CurlDownConfig& downCfg)
{
	CurlDownloaderImpl::SetCurlDownConfig(downCfg);
}


CurlDownloader*  CurlDownloader::CreateCurlDownloader()
{
	return new CurlDownloaderImpl;
}


void  CurlDownloader::DeleteCurlDownloader(CurlDownloader** downloader)
{
	if (downloader && *downloader)
	{
		delete *downloader;
		*downloader = NULL;
	}
}
