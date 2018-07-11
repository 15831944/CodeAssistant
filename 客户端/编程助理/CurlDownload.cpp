#include "stdafx.h"
#include "CurlDownloadImpl.h"


CurlDownloader::~CurlDownloader()
{
}


//ȫ�ֳ�ʼ��
void  CurlDownloader::InitCurlDownloader()
{
	CurlDownloaderImpl::Init();
}


void  CurlDownloader::UninitCurlDownloader()
{
	CurlDownloaderImpl::Uninit();
}


//��������
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
