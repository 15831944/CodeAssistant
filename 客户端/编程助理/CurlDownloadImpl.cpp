#include "stdafx.h"
#include <io.h>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <Sensapi.h>
#include "curl/curl.h"
#include "pthreads/pthread.h"
#include "threadpool.h"
#include "CurlDownloadImpl.h"

#include <sys/stat.h>
#include <string.h>
using namespace std;//�����������ռ�

//������ʱ�ļ���׺
#define DOWNLOAD_TMP_FILE_EXT ".dltmp"

//����Ƿ�֧�ֶ��̷߳�Ƭ������ַ���
#define RANGE_TEST_FLAG "RangeTest"

//���ڼ���Ƿ�֧�ֶ��߳����ؽ��մ�С
#define RANGE_TEST_RECV_SIZE 1024

// 
#define CURLD_FORMAT_LOG MyLog(g_curlDownCfg.pLogFun, __FILE__, __FUNCTION__, __LINE__)<<Log_Format

// 
/////////////////////////////////////////////////////////////////////////////////////////////////////
CURLSH* CurlDownloaderImpl::sharedns_handle = NULL; 
CurlDownConfig CurlDownloaderImpl::g_curlDownCfg;


//�߳���Ϣ
typedef struct _tThreadInfo
{
	unsigned long ulBegPos;    //������ʼλ��
	unsigned long ulBlockSize; //���̸߳������ص����ݴ�С
	unsigned long ulRecvSize;  //���߳��Ѿ����յ������ݴ�С
	CURL* pCurl;
	pthread_mutex_t thrdMutex;
	int iTryTimes;  //ʧ���Ѿ����Դ���
	_tThreadInfo()
	{
		ulBegPos = 0;
		ulBlockSize = 0;
		ulRecvSize = 0;
		pCurl = NULL;
		pthread_mutex_init(&thrdMutex, NULL);
		iTryTimes = 0;
	}
	~_tThreadInfo(){
		pthread_mutex_destroy(&thrdMutex);
	}
}ThreadInfo;

class PthreadMutex
{
public:
	PthreadMutex(){ 
		pthread_mutex_init(&m_mutex, NULL);
	}
	~PthreadMutex(){ 
		pthread_mutex_destroy(&m_mutex);
	}
	void Lock() { 
		pthread_mutex_lock(&m_mutex);
	}
	void UnLock() {
		pthread_mutex_unlock(&m_mutex);
	}
private:
	pthread_mutex_t m_mutex;
};

class PThreadPoolSingleton{
public:
	PThreadPoolSingleton(){
		m_pool = NULL;
	}
	void InitThreadPool(){
		if (!m_pool)
		{
			m_mutex.Lock();
			if (!m_pool)
			{
				CurlDownConfig cfg = CurlDownloaderImpl::GetCurlDownConfig();
				int thread_count = cfg.iMaxDownloadCount*(cfg.iMaxThreadCount + 1);
				int queue_size = thread_count;
				m_pool = threadpool_create(thread_count, queue_size, 0);
			}
			m_mutex.UnLock();
		}
	}
	threadpool_t* GetThreadPool(){
		return m_pool;
	}
    ~PThreadPoolSingleton(){
		threadpool_destroy(m_pool, 0);
		m_pool = NULL;
	}
private:
	threadpool_t* m_pool;
	PthreadMutex m_mutex;
};

static PThreadPoolSingleton g_poolInstance;

//////////////////////////////////////////////////////////////////////////////////////////////////////


void FFlushEx(FILE* stream)
{
	int duphandle;
	fflush(stream);
	duphandle=_dup(_fileno(stream));
	_close(duphandle);
}

void ReplaceStr(std::string& src, const std::string& target, const std::string& replacement)
{
	if (target == replacement) return;
	std::string::size_type startpos = 0;  
	while (startpos != std::string::npos)  
	{  
		startpos = src.find(target, startpos);      
		if( startpos != std::string::npos )  
		{  
			src.replace(startpos,target.length(),replacement); 
			startpos += replacement.length();
		}  
	}  
}

void SplitStr(const std::string& s, const std::string& delim, std::vector<std::string>* ret) 
{  
	if (!ret) return;
	ret->clear();
	size_t last = 0;  
	size_t index=s.find_first_of(delim,last);  
	while (index!=std::string::npos)  
	{  
		ret->push_back(s.substr(last,index-last));  
		last=index+delim.length();  
		index=s.find_first_of(delim,last);  
	}  
	if (index-last>0)  
	{  
		ret->push_back(s.substr(last,index-last));  
	}  
}  

bool IsHttpsUrl(const std::string& strUrl)
{
	std::string strTmp = strUrl.substr(0, 5);
	transform(strTmp.begin(), strTmp.end(), strTmp.begin(), tolower);
	return strTmp == "https";
}

bool IsFtpUrl(const std::string& strUrl)
{
	std::string strTmp = strUrl.substr(0, 3);
	transform(strTmp.begin(), strTmp.end(), strTmp.begin(), tolower);
	return strTmp == "ftp";
}

std::string Log_Format(LPCSTR lpszFormat, ...)
{
	char szLog[1024]={0};
	va_list ap;
	va_start(ap, lpszFormat);
	vsprintf_s(szLog, lpszFormat, ap);
	va_end(ap);
	return szLog;
}


// FTP
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* parse headers for Content-Length */  
size_t CurlDownloaderImpl::GetContentLengthFunc(void *ptr, size_t size, size_t nmemb, void *stream)   
{  
    int r;  
    long len = 0;  
    /* _snscanf() is Win32 specific */  
    //r = _snscanf(ptr, size * nmemb, "Content-Length: %ld\n", &len);  
    r = sscanf_s((const char*)ptr, "Content-Length: %ld\n", &len);  
    if (r) /* Microsoft: we don't read the specs */  
        *((long *) stream) = len;

    return size * nmemb;  
}

/* discard downloaded data */  
size_t CurlDownloaderImpl::DisCardFunc(void *ptr, size_t size, size_t nmemb, void *stream)   
{
    return size * nmemb;  
}

//write data to upload  
size_t CurlDownloaderImpl::writefunc(void *ptr, size_t size, size_t nmemb, void *stream)  
{
	size_t writeLen   = fwrite(ptr, size, nmemb, (FILE*)stream);

	CurlDownloaderImpl* pGauge = (CurlDownloaderImpl*) ptr; 
	if(pGauge)
	{
		pGauge->m_ulDownFileSize += writeLen;
	}

    return size*nmemb;
}

/* read data to upload */  
size_t CurlDownloaderImpl::readfunc(void *ptr, size_t size, size_t nmemb, void *stream)  
{  
    FILE *f = (FILE*)stream;  
    size_t n;  
    if (ferror(f))  
        return CURL_READFUNC_ABORT;  
    n = fread(ptr, size, nmemb, f) * size;  
    return n;  
}

//��������ʾ���� 
int CurlDownloaderImpl::progress_func(void *ptr, double rDlTotal, double rDlNow, double rUlTotal, double rUlNow) 
{
	/*CDownload* pGauge = (CDownload*) ptr; 
	if(pGauge)
	{
		pGauge->m_Progress.SetRange(0,100);
		//���ý�������ֵ 
		pGauge->m_Progress.SetPos(100 *((int)rDlNow/(int)rDlTotal));
	}*/

	CurlDownloaderImpl* pGauge = (CurlDownloaderImpl*) ptr; 
	if(pGauge)
	{
		pGauge->m_ulFullFileSize = (unsigned long)rDlTotal;
	}

	return 0;
}

// �ϴ�
int CurlDownloaderImpl::FTP_UpLoad(CURL *curlhandle, const std::string remotepath, const std::string localpath, long timeout, long tries)  
{
    FILE *f;
    long uploaded_len = 0;
    CURLcode r = CURLE_GOT_NOTHING;
    int c;

	const char * m_Local  = localpath.c_str();
	const char * m_Remote = remotepath.c_str();
	CStringA UserPswd(UserPwd);

    fopen_s(&f, m_Local, "rb");
    if (f == NULL)
	{
        perror(NULL);
        return 0;
    }

    curl_easy_setopt(curlhandle, CURLOPT_UPLOAD, 1L);  
    curl_easy_setopt(curlhandle, CURLOPT_URL, m_Remote);  
    curl_easy_setopt(curlhandle, CURLOPT_USERPWD, UserPswd);

    if (timeout)
        curl_easy_setopt(curlhandle, CURLOPT_FTP_RESPONSE_TIMEOUT, timeout);

    curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, GetContentLengthFunc);
    curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &uploaded_len);
    curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, DisCardFunc);
    curl_easy_setopt(curlhandle, CURLOPT_READFUNCTION, readfunc);
    curl_easy_setopt(curlhandle, CURLOPT_READDATA, f);
    curl_easy_setopt(curlhandle, CURLOPT_FTPPORT, "-"); /* disable passive mode */
    curl_easy_setopt(curlhandle, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
    curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);

    for (c = 0; (r != CURLE_OK) && (c < tries); c++) 
	{
        /* are we resuming? */
        if (c) 
		{ /* yes */  
            /* determine the length of the file already written */  
            /* 
            * With NOBODY and NOHEADER, libcurl will issue a SIZE 
            * command, but the only way to retrieve the result is 
            * to parse the returned Content-Length header. Thus, 
            * getcontentlengthfunc(). We need discardfunc() above 
            * because HEADER will dump the headers to stdout 
            * without it. 
            */  
            curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 1L);  
            curl_easy_setopt(curlhandle, CURLOPT_HEADER, 1L);  
            r = curl_easy_perform(curlhandle);  
            if (r != CURLE_OK)  
                continue;  
            curl_easy_setopt(curlhandle, CURLOPT_NOBODY, 0L);  
            curl_easy_setopt(curlhandle, CURLOPT_HEADER, 0L);  
            fseek(f, uploaded_len, SEEK_SET);  
            curl_easy_setopt(curlhandle, CURLOPT_APPEND, 1L);  
        }
        else 
		{ /* no */  
            curl_easy_setopt(curlhandle, CURLOPT_APPEND, 0L);  
        }  
        r = curl_easy_perform(curlhandle);  
    }

    fclose(f);
    if (r == CURLE_OK)
        return 1;
    else
	{  
        fprintf(stderr, "%s\n", curl_easy_strerror(r));
        return 0;  
    }
}

// ����
int CurlDownloaderImpl::FTP_DownLoad(CURL *curlhandle, const std::string remotepath, const std::string localpath, long timeout, long tries)
{
    FILE *f;
    curl_off_t local_file_len = -1 ;
    long filesize =0 ;
    CURLcode r = CURLE_GOT_NOTHING;
    struct stat file_info;
    int use_resume = 0;

	const char * m_Local  = localpath.c_str();
	const char * m_Remote = remotepath.c_str();
	CStringA UserPswd(UserPwd);

    //��ȡ�����ļ���С��Ϣ
    if(stat(m_Local, &file_info) == 0)
    {
        local_file_len = file_info.st_size;
        use_resume = 1;
    }

    //׷�ӷ�ʽ���ļ���ʵ�ֶϵ�����
    fopen_s(&f, localpath.c_str(), "ab+");
    if (f == NULL) 
	{
        perror(NULL);
        return 0;
    }

    curl_easy_setopt(curlhandle, CURLOPT_URL, m_Remote);
    curl_easy_setopt(curlhandle, CURLOPT_USERPWD, UserPswd);

    //���ӳ�ʱ����
    curl_easy_setopt(curlhandle, CURLOPT_CONNECTTIMEOUT, timeout);

    //����ͷ������
    curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, GetContentLengthFunc);
    curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &filesize);

    // ���öϵ�����
	curl_easy_setopt(curlhandle, CURLOPT_RESUME_FROM_LARGE, use_resume?local_file_len:0);
	curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, writefunc);
	curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, f);

	// ���������
	curl_easy_setopt(curlhandle, CURLOPT_NOPROGRESS, FALSE);
	curl_easy_setopt(curlhandle, CURLOPT_PROGRESSFUNCTION, progress_func);

	curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);
	r = curl_easy_perform(curlhandle);

    fclose(f);
    if (r == CURLE_OK)
        return 1;
    else
	{
        fprintf(stderr, "%s\n", curl_easy_strerror(r));
        return 0;  
    }
}


// ���ֱ�����ز���
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// UTF-8 ת Unicode
void UTF_8ToUnicode(WCHAR* pOut, char *pText)
{
    char* uchar = (char *)pOut;
    uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
    uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

    return;  
}

// Unicode ת GB2312
void UnicodeToGB2312(char* pOut, WCHAR uData)
{
    WideCharToMultiByte(CP_ACP, NULL, &uData, 1, pOut, sizeof(WCHAR), NULL, NULL);
    return;
}

//UTF_8 תgb2312
void UTF_8ToGB2312(string &pOut, char *pText, int pLen)  
{
    char buf[4];
    char* rst = new char[pLen + (pLen >> 2) + 2];
    memset(buf, 0, 4);
    memset(rst, 0, pLen + (pLen >> 2) + 2);

    int i = 0;
    int j = 0;

    while (i < pLen)
    {
        if (*(pText + i) >= 0)
        {

            rst[j++] = pText[i++];
        }
        else
        {
            WCHAR Wtemp;

            UTF_8ToUnicode(&Wtemp, pText + i);
            UnicodeToGB2312(buf, Wtemp);

            unsigned short int tmp = 0;
            tmp = rst[j] = buf[0];
            tmp = rst[j + 1] = buf[1];
            tmp = rst[j + 2] = buf[2];

            i += 3;
            j += 2;
        }
    }

    rst[j] = '\0';
    pOut = rst;
    delete[]rst;
}

// Gb2312 ת Unicode
void Gb2312ToUnicode(WCHAR* pOut, char *gbBuffer)
{
    ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gbBuffer, 2, pOut, 1);
    return;
}

// UnicodeתUTF-8
void UnicodeToUTF_8(char* pOut, WCHAR* pText)
{
    // ע�� WCHAR�ߵ��ֵ�˳��,���ֽ���ǰ�����ֽ��ں�
    char* pchar = (char *)pText;

    pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
    pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
    pOut[2] = (0x80 | (pchar[0] & 0x3F));

    return;
}

//GB2312 תΪ UTF-8
void GB2312ToUTF_8(string& pOut, char *pText, int pLen)
{
    char buf[4];
    memset(buf, 0, 4);
    pOut.clear();

    int i = 0;
    while (i < pLen)
    {
        //�����Ӣ��ֱ�Ӹ��ƾͿ���
        if (pText[i] >= 0)
        {
            char asciistr[2] = { 0 };
            asciistr[0] = (pText[i++]);
            pOut.append(asciistr);
        }
        else
        {
            WCHAR pbuffer;
            Gb2312ToUnicode(&pbuffer, pText + i);

            UnicodeToUTF_8(buf, &pbuffer);
            pOut.append(buf);
            i += 2;
        }
    }

    return;
}

// �ַ�ת����, ��Ϊ��Urlʹ��
char CharToInt(char ch)
{
    if (ch >= '0' && ch <= '9')return (char)(ch - '0');
    if (ch >= 'a' && ch <= 'f')return (char)(ch - 'a' + 10);
    if (ch >= 'A' && ch <= 'F')return (char)(ch - 'A' + 10);
    return -1;
}

// �ַ�ת������, ��Ϊ��Urlʹ��
char StrToBin(char *str)
{
    char tempWord[2];
    char chn;

    tempWord[0] = CharToInt(str[0]);           // make the B to 11 -- 00001011
    tempWord[1] = CharToInt(str[1]);           // make the 0 to 0  -- 00000000

    chn = (tempWord[0] << 4) | tempWord[1];    // to change the BO to 10110000
    return chn;
}

//��str����Ϊ��ҳ�е� GB2312 url encode ,Ӣ�Ĳ��䣬����˫�ֽ�  ��%3D%AE%88
string UrlGB2312(char * str) 
{
    string dd;
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        if (isalnum((BYTE)str[i]))
        {
            char tempbuff[2];
            sprintf_s(tempbuff,sizeof(tempbuff), "%c", str[i]);
            dd.append(tempbuff);
        }
        else if (isspace((BYTE)str[i]))
        {
            dd.append("+");
        }
        else
        {  
            char tempbuff[4];
            sprintf_s(tempbuff, sizeof(tempbuff),"%%%X%X", ((BYTE*)str)[i] >> 4, ((BYTE*)str)[i] % 16);
            dd.append(tempbuff);
        }
    }
    return dd;
}

//��str����Ϊ��ҳ�е� UTF-8 url encode ,Ӣ�Ĳ��䣬�������ֽ�  ��%3D%AE%88
string UrlUTF8(char * str)
{
    string tt;
    string dd;
    GB2312ToUTF_8(tt, str, (int)strlen(str));

    size_t len = tt.length();
    for (size_t i = 0; i < len; i++)
    {
        if (isalnum((BYTE)tt.at(i)))
        {
            char tempbuff[2] = { 0 };
            sprintf_s(tempbuff,sizeof(tempbuff), "%c", (BYTE)tt.at(i));
            dd.append(tempbuff);
        }
        else if (isspace((BYTE)tt.at(i)))
        {
            dd.append("+");
        }
        else
        {
            char tempbuff[4];
            sprintf_s(tempbuff, sizeof(tempbuff), "%%%X%X", ((BYTE)tt.at(i)) >> 4, ((BYTE)tt.at(i)) % 16);
            dd.append(tempbuff);
        }
    }
    return dd;
}

//��url GB2312����
string UrlGB2312Decode(string str)  
{  
    string output = "";  
    char tmp[2];  
    int i = 0, idx = 0, len = str.length();  
  
    while (i < len) {  
        if (str[i] == '%') {  
            tmp[0] = str[i + 1];  
            tmp[1] = str[i + 2];  
            output += StrToBin(tmp);  
            i = i + 3;  
        }  
        else if (str[i] == '+') {  
            output += ' ';  
            i++;  
        }  
        else {  
            output += str[i];  
            i++;  
        }  
    }  
  
    return output;  
}

//��url utf8����
string UrlUTF8Decode(string str)  
{  
    string output = "";  
  
    string temp = UrlGB2312Decode(str);//    
  
    UTF_8ToGB2312(output, (char *)temp.data(), strlen(temp.data()));  
  
    return output;  
  
}


// Post
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PostFile(int argc, char *argv[])  
{  
  CURL *curl;  
  CURLcode res;  
  
  struct curl_httppost *formpost=NULL;  
  struct curl_httppost *lastptr=NULL;  
  struct curl_slist *headerlist=NULL;  
  static const char buf[] = "Expect:";  
  
  curl_global_init(CURL_GLOBAL_ALL);  
  
  /* Fill in the file upload field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "sendfile",  
               CURLFORM_FILE, "D:\\sign.txt",  
               CURLFORM_END);  
  
  /* Fill in the filename field */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "filename",  
               CURLFORM_COPYCONTENTS, "sign.txt",  
               CURLFORM_END);  
  
  /* Fill in the submit field too, even if this is rarely needed */  
  curl_formadd(&formpost,  
               &lastptr,  
               CURLFORM_COPYNAME, "submit",  
               CURLFORM_COPYCONTENTS, "Submit",  
               CURLFORM_END);  
  
  curl = curl_easy_init();  
  /* initalize custom header list (stating that Expect: 100-continue is not 
     wanted */  
  headerlist = curl_slist_append(headerlist, buf);  
  if(curl) {  
    /* what URL that receives this POST */  

     curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/fileUpload.action");  
    if ( (argc == 2) && (!strcmp(argv[1], "noexpectheader")) )  
      /* only disable 100-continue header if explicitly requested */  
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);  
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);  
  
    /* Perform the request, res will get the return code */  
    res = curl_easy_perform(curl);  
    /* Check for errors */  
    if(res != CURLE_OK)  
      fprintf(stderr, "curl_easy_perform() failed: %s\n",  
              curl_easy_strerror(res));  
  
    /* always cleanup */  
    curl_easy_cleanup(curl);  
  
    /* then cleanup the formpost chain */  
    curl_formfree(formpost);  
    /* free slist */  
    curl_slist_free_all (headerlist);  
  }  
  return 0;  
}  

static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)
{
	if(itype == CURLINFO_TEXT)
	{
		//printf("[TEXT]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_IN)
	{
		printf("[HEADER_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_HEADER_OUT)
	{
		printf("[HEADER_OUT]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_IN)
	{
		printf("[DATA_IN]%s\n", pData);
	}
	else if(itype == CURLINFO_DATA_OUT)
	{
		printf("[DATA_OUT]%s\n", pData);
	}
	return 0;
}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if( NULL == str || NULL == buffer )
	{
		return -1;
	}

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);
	return nmemb;
}

int CurlDownloaderImpl::Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse, int SendCode,int RecvCode)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	// ����� Debug ģʽ
    #ifdef _DEBUG
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    #endif

		/*if (SendCode == GB2312)
			UTF_8ToGB2312((const std::string)strPost, (char*)strPost.c_str(), strPost.size());
			;*/

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10); //���ӳ�ʱ�������ֵ�������̫�̿��ܵ����������󲻵��ͶϿ���
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);        //��������ʱ��ʱ���ã����10��������δ�����ֱ꣬���˳�
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	/*if (RecvCode == GB2312)
		UTF_8ToGB2312(strResponse, (char*)strResponse.c_str(), strResponse.size());
		;*/
	
	int lCode = 0;
	if (res == CURLE_OK)  
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &lCode);

	return lCode;
}

int CurlDownloaderImpl::Get(const std::string & strUrl, std::string & strResponse, int SendCode,int RecvCode)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	// ����� Debug ģʽ
    #ifdef _DEBUG
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    #endif

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	/**
	* ������̶߳�ʹ�ó�ʱ�����ʱ��ͬʱ���߳�����sleep����wait�Ȳ�����
	* ������������ѡ�libcurl���ᷢ�źŴ�����wait�Ӷ����³����˳���
	*/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10); //���ӳ�ʱ�������ֵ�������̫�̿��ܵ����������󲻵��ͶϿ���
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);        //��������ʱ��ʱ���ã����10��������δ�����ֱ꣬���˳�
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if(RecvCode == GB2312)
		UTF_8ToGB2312(strResponse, (char*)strResponse.c_str(), strResponse.size());

	int lCode = 0;
	if (res == CURLE_OK)  
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &lCode);

	return lCode;
}

int CurlDownloaderImpl::Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath, int SendCode,int RecvCode)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	// ����� Debug ģʽ
    #ifdef _DEBUG
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    #endif

	if(SendCode == GB2312)
		//UTF_8ToGB2312((const std::string)strPost, (char*)strPost.c_str(), strPost.size());

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//ȱʡ�������PEM�������������ã�����֧��DER
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}

	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10); //���ӳ�ʱ�������ֵ�������̫�̿��ܵ����������󲻵��ͶϿ���
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);        //��������ʱ��ʱ���ã����10��������δ�����ֱ꣬���˳�
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if(RecvCode == GB2312)
		UTF_8ToGB2312(strResponse, (char*)strResponse.c_str(), strResponse.size());

	int lCode = 0;
	if (res == CURLE_OK)  
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &lCode);

	return lCode;
}

int CurlDownloaderImpl::Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath, int SendCode,int RecvCode)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if(NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}

	// ����� Debug ģʽ
    #ifdef _DEBUG
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
    #endif

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if(NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}

	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10); //���ӳ�ʱ�������ֵ�������̫�̿��ܵ����������󲻵��ͶϿ���
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);        //��������ʱ��ʱ���ã����10��������δ�����ֱ꣬���˳�
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if(RecvCode == GB2312)
		UTF_8ToGB2312(strResponse, (char*)strResponse.c_str(), strResponse.size());

	int lCode = 0;
	if (res == CURLE_OK)  
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &lCode);

	return lCode;
}


// CurlDownloaderImpl
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CurlDownloaderImpl::CurlDownloaderImpl(void)
{
	m_pFile = NULL;
	m_bPause = false;
	m_bSupportMultiDown = false;
	m_bTerminate = false;
	m_ulFullFileSize = 0;
	m_ulDownFileSize = 0;
	m_strUrl = m_strDownloadPath = "";
	pthread_mutex_init(&m_mutexFile,NULL);
	m_strProxy = "";
	m_iHttpCode = 200;
	m_bRedirected = false;
	m_strOriginalUrl = "";
	m_curlCode = CURLE_OK;
	m_bInitOver = false;
	m_bInitInterrupt = false;
	m_iMaxRetryTimes = -1;
	g_poolInstance.InitThreadPool();
}

CurlDownloaderImpl::~CurlDownloaderImpl(void)
{
	Pause();
	pthread_mutex_lock(&m_mutexFile);
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	pthread_mutex_unlock(&m_mutexFile);
    ClearThreadInfo();
	//�ļ����ʱ����ͷ�
	pthread_mutex_destroy(&m_mutexFile);
}

void CurlDownloaderImpl::ClearThreadInfo()
{
	std::vector<ThreadInfo*>::const_iterator ita = m_vecThrdInfo.begin();
	while (ita != m_vecThrdInfo.end())
	{
		ThreadInfo* pInfo = *ita++;
		if (pInfo)
		{
			if (pInfo->pCurl)
			{
				curl_easy_cleanup(pInfo->pCurl);
				pInfo->pCurl = NULL;
			}
			delete pInfo;
			pInfo = NULL;
		}
	}
	m_vecThrdInfo.clear();
}

bool CurlDownloaderImpl::Start(const std::string&strUrl, const std::string&strDownloadPath, 
	int* pThreadCount)
{
	int iThreadCountX = 0;
	if (pThreadCount)
	{
	    assert(*pThreadCount);
		iThreadCountX = *pThreadCount;
	}
	else
	{
		iThreadCountX = g_curlDownCfg.iMaxThreadCount;
	}

	m_bPause = false;
	m_bTerminate = false;
	m_strUrl = strUrl;

	// FTP
	if(IsFtpUrl(strUrl))
	{
		CString UserPswd = (CString)strUrl.c_str();
		UserPswd.Replace(UserPswd.Left(6), _T(""));

		int Pos    = UserPswd.Find(_T("@"));
		UserPwd    = UserPswd.Left(Pos);

		CString RemotePath = _T("ftp://") + UserPswd.Right(UserPswd.GetLength() -Pos -1);
		CStringA m_Remote(RemotePath);

		const std::string remotepath = m_Remote.GetBuffer(0);

		//����ԭʼURL
		m_strOriginalUrl = remotepath;
	}
	else
	{
		//����ԭʼURL
		m_strOriginalUrl = strUrl;
	}

	m_strDownloadPath = strDownloadPath;

	//��ֹ���¿�ʼ������Ҫ���³�ʼ����������
	m_ulFullFileSize = 0;
	m_ulDownFileSize = 0;
	m_bSupportMultiDown = false;
    m_bRedirected = false;
	m_curlCode = CURLE_OK;
	m_iHttpCode = 200;

	// ����߳���Ϣ
	ClearThreadInfo();

	//ɾ�������ļ�
	DeleteFileA(strDownloadPath.c_str());
	std::string strTmpFile = strDownloadPath + DOWNLOAD_TMP_FILE_EXT;

	//ͨ����ʱ�ļ��ж��Ƿ��Ѿ����ع�
	if (_access(strTmpFile.c_str(), 0) == -1)
	{
		//�ļ������ڣ��µ����� 
        if (!DownloadInit())
        {
			return false;
        }

		//���ż��̸߳���
		int iCount = (int)ceil(1.0*m_ulFullFileSize/(g_curlDownCfg.iMinBlockSize));

		// �̸߳������ܳ���iThreadCountX 
		const int iThreadCount = (m_bSupportMultiDown ? min(iCount, iThreadCountX) : 1);
		m_ulDownFileSize = 0;
		unsigned long ulFileSize = m_ulFullFileSize;

		//��ʱ��Ϣ����
		const int iTmpInfoLen = iThreadCount*3*sizeof(unsigned long) + sizeof(unsigned long);

		//д����ʱ����
		const unsigned long ulTmpFileLen = ulFileSize + iTmpInfoLen;
		unsigned long ulHasWritten = 0;

		// ���ڴ�ӳ�䴴����ʱ�ļ� 
		bool bFlag = false;
		HANDLE hFile = CreateFileA(strTmpFile.c_str(), GENERIC_READ|GENERIC_WRITE,
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			HANDLE hFileMapping = CreateFileMapping(hFile,NULL,PAGE_READWRITE,0,
				ulTmpFileLen,NULL);
			if (hFileMapping != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hFileMapping);
				bFlag = true;
			}
			CloseHandle(hFile);
		}

		if (!bFlag)
		{
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ʱ�ļ�����ʧ��, lastErrorCode=%d"
				, this, GetLastError());
			return false;
		}

		//������ʱ�ļ�	
		errno_t err = fopen_s(&m_pFile, strTmpFile.c_str(), "rb+");
		if (!m_pFile) 
		{
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ʱ�ļ���ʧ��, lastErrorCode=%d"
				, this, GetLastError());
			return false;
		}

		//�ļ���λ��δβ�������ֽ���ʼλ��, ���ļ���Сд�����4�ֽ���
		if (_fseeki64(m_pFile, sizeof(unsigned long), SEEK_END)
			|| 0 == fwrite(&ulFileSize, sizeof(unsigned long), 1, m_pFile)) 
		{
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ʱ��Ϣ��ʼ��д��ʧ��, lastErrorCode=%d"
				, this, GetLastError());
			goto Loop;
		}

		//ÿ���̸߳������ص����ݿ��С
		const unsigned long ulBlockSize = ulFileSize/iThreadCount;
		int off = - iTmpInfoLen;
		_fseeki64(m_pFile, off, SEEK_END);

		//�����ļ���С���̸߳���������ÿ���̸߳������ص���������
		for (int i = 0; i < iThreadCount; ++i)
		{
			ThreadInfo* pInfo = new ThreadInfo;
			pInfo->ulBegPos = i * ulBlockSize;
			if (i == iThreadCount - 1)
			{
				pInfo->ulBlockSize = ulFileSize - i * ulBlockSize;
			}
			else
			{
				pInfo->ulBlockSize = ulBlockSize;
			}
			m_vecThrdInfo.push_back(pInfo);

			//���ļ�β��д��ÿ���̵߳�����������Ϣ����ʼλ��(4B)���������ص����ݴ�С(4B)��
			//�Ѿ����صĴ�С(4B)
			fwrite(&pInfo->ulBegPos, sizeof(unsigned long), 1, m_pFile);
			fwrite(&pInfo->ulBlockSize, sizeof(unsigned long), 1, m_pFile);
			fwrite(&pInfo->ulRecvSize, sizeof(unsigned long), 1, m_pFile);
		}
		FFlushEx(m_pFile);

		for (int j = 0; j < iThreadCount; ++j)
		{
			DownThreadParam* param = new DownThreadParam;
			param->pDownloader = this;
			param->index = j;
			if (!CurlInit(param)
				|| threadpool_add(g_poolInstance.GetThreadPool(), DownloadFun, param, 0))
			{
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ���߳�%d����ʧ��", this, j);
				delete param;
				param = NULL;
				goto Loop;
			}
			else
			{
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ���߳�%d����!", this, j);
			}
		}
		return true;
	}
	else
	{
		//�ļ����ڣ�������ʱ�ļ�����ȡ��һ��δ��ɵ���Ϣ����������
		CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �����ϴε���ʱ�ļ������жϵ�����", this);
		return ContinueDownloadByTmpFile();
	}

Loop:
	//��ͣ�Ѿ���ʼ���߳�
	Pause();
	FinalProc();
	return false;
}

bool CurlDownloaderImpl::Pause()
{
	m_bPause = true;
	bool bRet = true;
	std::vector<ThreadInfo*>::const_iterator ita = m_vecThrdInfo.begin();
	while (ita != m_vecThrdInfo.end())
	{
		ThreadInfo* pInfo = *ita++;
		if (pInfo && pInfo->ulRecvSize < pInfo->ulBlockSize)
		{
			//�߳��Ѿ�ִ�����������ͣ	
			if (pthread_mutex_lock(&pInfo->thrdMutex))
			{
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �߳�%d��ͣʧ��", this, 
					(int)(ita - m_vecThrdInfo.begin()) - 1);
				bRet = false;
				//��������ͣʧ�ܾ�������
			}
			pthread_mutex_unlock(&pInfo->thrdMutex);
		}
	}
	return bRet;
}

bool CurlDownloaderImpl::Resume()
{
	m_bPause = false;
	bool bRunFinish = true;
	//���߳�����ִ�л��ļ��Ѿ�������ϣ���ֱ���˳�
	if (CheckIsFinish(&bRunFinish) || !bRunFinish) return false;
	//δ��ɵ��߳��ٴο���
	std::vector<ThreadInfo*>::const_iterator ita = m_vecThrdInfo.begin();
	//
	while (ita != m_vecThrdInfo.end())
	{
		ThreadInfo* pInfo = *ita++;
		if (pInfo && pInfo->ulBlockSize > pInfo->ulRecvSize)
		{
			DownThreadParam* param = new DownThreadParam;
			param->pDownloader = this;
			param->index = int(ita - m_vecThrdInfo.begin()) - 1;
			//û�����꣬��������Ϊ��ͣ��Ҳ��������Ϊ����ʧ��
			if ((!pInfo->pCurl && !CurlInit(param)) 
				|| threadpool_add(g_poolInstance.GetThreadPool(), DownloadFun, param, 0))
			{
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ���߳�%d��ԭʧ��", this, 
					(int)(ita - m_vecThrdInfo.begin()) - 1);
				//�ָ�ԭ��״̬
				Pause();
				delete param;
				param = NULL;
				return false;
			}
			else
			{
				//�̻߳�ԭ�ɹ�
			    CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ���߳�%d����!", this, param->index);	
			}
		}
	}
	return true;
}

bool CurlDownloaderImpl::Terminate(bool bDeleteFile/* = true*/)
{
	m_bTerminate = true;
	if (Pause())
	{
		pthread_mutex_lock(&m_mutexFile);
		if (m_pFile)
		{
			fclose(m_pFile);
			m_pFile = NULL;
		}
		pthread_mutex_unlock(&m_mutexFile);
		if (bDeleteFile)
		{
			//ɾ����ʱ�ļ��������ļ�
			DeleteFileA(m_strDownloadPath.c_str());
			std::string strTmpPath = m_strDownloadPath + DOWNLOAD_TMP_FILE_EXT;
			DeleteFileA(strTmpPath.c_str());
		}
		//
		m_ulDownFileSize = 0;
		ClearThreadInfo();
		return true;
	}
	return false;
}

bool CurlDownloaderImpl::CurlInit(void* param)
{
	CurlDownloaderImpl* pDown = NULL;
	ThreadInfo* pInfo = NULL;
	int index = -1;
	ParseThreadParam(param, &pDown, &index);
	if (pDown && index != -1) pInfo = pDown->m_vecThrdInfo[index];
	if (pInfo)
	{
		//CURL* _curl = curl_easy_init(); 
		CURL* _curl = Create_Share_Curl();
		if (!_curl)
		{
			AfxMessageBox(_T("_curl_ False"));
			return false;
		}

		// ���Ӵ������� 
		CURLcode code;
		bool bHttps, bFtp = bHttps = false;
        if (!pDown->m_strProxy.empty())
        {
			code = (CURLcode)curl_easy_setopt(_curl, CURLOPT_PROXY, pDown->m_strProxy.c_str());
			if (code != CURLE_OK) goto Loop;
        }

		//��ʼ������URL
		code = (CURLcode)curl_easy_setopt(_curl, CURLOPT_URL, pDown->m_strUrl.c_str());
		if (code != CURLE_OK) goto Loop;
		
		// ֧��SSL 
		bHttps = IsHttpsUrl(pDown->m_strUrl);
		if (bHttps && !CheckSSL(_curl)) goto Loop;

		// FTP
		if(IsFtpUrl(pDown->m_strUrl))
		{
			long filesize = 0;

			//����ͷ������
			curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, GetContentLengthFunc);
			curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &filesize);

			CStringA UserPswd(pDown->UserPwd);
			curl_easy_setopt(_curl, CURLOPT_USERPWD, UserPswd);

			curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
		}

		// �������غ���
		code = (CURLcode)curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, 
			&CurlDownloaderImpl::WriteData);  
		if (code != CURLE_OK) goto Loop;

		code = (CURLcode)curl_easy_setopt(_curl, CURLOPT_WRITEDATA, param); 
		if (code != CURLE_OK) goto Loop;

		//�ض���
		code = (CURLcode)curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L); 
		if (code != CURLE_OK) goto Loop;

		// �Ż����ܣ���ֹ��ʱ���� 
		code = (CURLcode)curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);  
		if (code != CURLE_OK) goto Loop;

		// �������ٶ�< 1 �ֽ�/�� ���� 5 ��ʱ,�����ӻ���ֹ����������ƽ��ճ�ʱ 
		code = (CURLcode)curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_LIMIT, 1L);  
		if (code != CURLE_OK) goto Loop;

		code = (CURLcode)curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_TIME, 30L); 
		if (code != CURLE_OK) goto Loop;

		// ֻ���������ӳ�ʱ����Ϊ����ʱ�䲻�ɿ�
		code = (CURLcode)curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, 
			bHttps ? 2*g_curlDownCfg.iTimeOut : g_curlDownCfg.iTimeOut);
		if (code != CURLE_OK) goto Loop;


		pInfo->pCurl = _curl;
		pInfo->iTryTimes = 0;

		//�̳߳�ʼ�����!
		return true;
Loop:
		curl_easy_cleanup(_curl);
		return false;
	}

	return false;
}

void CurlDownloaderImpl::DownloadFun(void* param)
{
	CurlDownloaderImpl* pDown = NULL;
	ThreadInfo* pInfo = NULL;
	int index = -1;
	long lCode = 0;
	ParseThreadParam(param, &pDown, &index);
	if (pDown && index != -1) pInfo = pDown->m_vecThrdInfo[index];
	if (pInfo)
	{
		if (pthread_mutex_trylock(&pInfo->thrdMutex))
		{
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �߳�%d�쳣!",
				pDown, index);
			return;
		}
		else
		{
			pthread_mutex_unlock(&pInfo->thrdMutex);
		}
	    pthread_mutex_lock(&pInfo->thrdMutex);
		CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �߳�%d��ʼִ��!",
			pDown, index);
		unsigned long ulBegPos, ulUnRecvSize, ulCurRecvSize;
		CURLcode code;
		bool bRet;
		const int iOnceMaxRecvSize = g_curlDownCfg.iMinBlockSize;
		while (!pDown->m_bPause)
		{
			// ���Ի��� 
	    ReTry:
			//�ֶ�����
			if (pInfo->ulRecvSize < pInfo->ulBlockSize)
			{
				//δ���յ����ݴ�С
				ulUnRecvSize = pInfo->ulBlockSize - pInfo->ulRecvSize;
				//����Ҫ���յ����ݴ�С
				ulCurRecvSize = ulUnRecvSize;				
                //ȷ����ʼλ��
				ulBegPos = pInfo->ulBegPos + pInfo->ulRecvSize;
				bRet = false;
				// ���ڲ�֧�ֶ��߳����صģ�������Range����ʱ����ղ������� 
				if (pDown->m_bSupportMultiDown)
				{
					char range[32] = {0};
					// ������һ�ֽڣ���ֹ����ʱ�����һ�ֽ����� 
					sprintf_s(range, "%lu-%lu", ulBegPos, ulBegPos + ulCurRecvSize/* - 1*/);
					code = (CURLcode)curl_easy_setopt(pInfo->pCurl, CURLOPT_RANGE, range);
				}
				else
				{
					code = (CURLcode)CURLE_OK;
				}
				if (code == CURLE_OK)
				{
					code = (CURLcode)curl_easy_perform(pInfo->pCurl);
					if (code == CURLE_OK)
					{
						// ��ʱ����CURLE_OK����ȴû���յ�����
						bRet = (ulUnRecvSize > pInfo->ulBlockSize - pInfo->ulRecvSize);
					}
					// ����CURLE_OK��û���յ����ݣ�Ҳ��Ҫ����
					//else
					if (!bRet)
					{
						DWORD dwFlags;
						// ������ʧ����������ͣ����ֹ��������򲻱����� 
						if (!pDown->m_bPause && !pDown->m_bTerminate
							&& IsNetworkAlive(&dwFlags))
						{
							// ��ȡ������ 
							curl_easy_getinfo(pInfo->pCurl, CURLINFO_RESPONSE_CODE, &lCode);
							pDown->m_iHttpCode = lCode;
							pDown->m_curlCode = code;
							if (pInfo->iTryTimes < pDown->GetMaxRetryTimes())
							{
								// ���³�ʼ�� ���Դ���Ҫ����
								int tryTimes = pInfo->iTryTimes;
								curl_easy_cleanup(pInfo->pCurl);
								pInfo->pCurl = NULL;
								if (pDown->CurlInit(param)/*1*/)
								{
									// �����Ѿ����ԵĴ��� 
									pInfo->iTryTimes = tryTimes;
									// ����ʱ�䣺һ�룬���룬���� 
									float iVal = (float)pow(2.0, pInfo->iTryTimes);
									pDown->SleepEx((unsigned long)iVal*1000);
									pInfo->iTryTimes++;
									// ���ڵ��߳����أ�����Ҫ��0��ʼ 
									if (!pDown->m_bSupportMultiDown)
									{
										pInfo->ulRecvSize = 0;
										pDown->m_ulDownFileSize = 0;
									}
									//����ʱ�����ؽ���
									float fPercent = 
										100*((float)pDown->m_ulDownFileSize)/((float)pDown->m_ulFullFileSize);
										CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ���ؽ���:%f%%, �߳�%d��������ʧ�ܣ�lastHttpCode: %d, CURLcode: %d, ���Ե�%d������", 
											pDown, fPercent, index, lCode, code, 
											pInfo->iTryTimes);
									goto ReTry;
								}
								else
								{
									CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �߳�%d��ʼ��ʧ�ܣ��޷���������", pDown, index);
								}
							}
							else
							{
								CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �߳�%d��������ʧ��",
									pDown, index);
							}
						}
					}
				}   
				//
				if (!bRet)
				{
					CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �߳�%d��������ʧ�ܣ������룺CURLCode=%d", pDown, index, code);
					break;
				}
			}
			else
			{
				//�����м䷢�����ִ���ֻҪ���ճɹ���������͸�λ
				pDown->m_iHttpCode = 200;
				pDown->m_curlCode = CURLE_OK;
				//���߳������Ѿ��������
				if (pInfo->iTryTimes)
				{
					CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �߳�%d����������ϣ����Դ�����%d", pDown, index, pInfo->iTryTimes);
				}
				break;
			}
		}

		if (1)
		{
			curl_easy_cleanup(pInfo->pCurl);
			pInfo->pCurl = NULL;
		}

		pthread_mutex_unlock(&pInfo->thrdMutex);
	}
	FreeThreadParam(param);
}

bool CurlDownloaderImpl::ContinueDownloadByTmpFile()
{
	std::string strTmpPath = m_strDownloadPath + DOWNLOAD_TMP_FILE_EXT;
	//�Զ�д�����Ʒ�ʽ���ļ�
	errno_t err = fopen_s(&m_pFile, strTmpPath.c_str(), "rb+");
    //���س�ʼ��������������ļ��Ƿ��и���
	if (err == 0)
	{
		if (!DownloadInit())
		{
			//˵�������س�ʼ��ʧ�ܣ��˳�
			pthread_mutex_lock(&m_mutexFile);
			if (m_pFile) 
			{
				fclose(m_pFile);
				m_pFile = NULL;
			}
			pthread_mutex_unlock(&m_mutexFile);
			return false;
		}
		//��λ���ļ�δβ���ȶ�ȡ��ʱ�ļ��ܳ���
		_fseeki64(m_pFile, 0, SEEK_END);
		long long ulTmpFileSize = _ftelli64(m_pFile);
		//�ٻ�ȡҪ���ص��ļ����ȣ�����ʼ����ȡ���ĳ��ȱȽϣ�����ͬ����������Դ�и��£���Ҫ��������
		unsigned long ulFileSize = 0;
		if (_fseeki64(m_pFile, sizeof(unsigned long), SEEK_END) == 0
			&& fread(&ulFileSize, sizeof(unsigned long), 1, m_pFile)
			&& ulFileSize == m_ulFullFileSize
			&& ulFileSize < ulTmpFileSize)
		{
			__int64 ulTmp = ulTmpFileSize - ulFileSize - sizeof(unsigned long);
			//������һ�ε��̸߳���
			int iThreadCount = (int)ulTmp/(sizeof(unsigned long)*3);
			m_ulDownFileSize = 0;
			//��λ��������Ϣ������ȡ��һ�ε�������Ϣ
			if (0 == _fseeki64(m_pFile, ulFileSize, SEEK_SET))
			{
				unsigned long ulBegPos, ulBlockSize, ulRecvSize;
				// ��ǰ�ֿ���Ϣ�Ƿ���Ч 
				bool bInfoValid;
				for (int i = 0; i < iThreadCount; ++i)
				{
					bInfoValid = true;
					if (m_bSupportMultiDown)
					{
						//֧�ֶ��߳����أ��ſ��ܶϵ�����
						fread(&ulBegPos, sizeof(unsigned long), 1, m_pFile);
						fread(&ulBlockSize, sizeof(unsigned long), 1, m_pFile);
						fread(&ulRecvSize, sizeof(unsigned long), 1, m_pFile);
						if (ulBegPos == -1 || ulBlockSize == -1 || ulRecvSize == -1 
							|| ulBegPos >= ulFileSize || ulBlockSize > ulFileSize 
							|| ulRecvSize > ulFileSize || ulRecvSize > ulBlockSize
							|| ulBlockSize == 0)
						{
							bInfoValid = false;
						}

						if (i == 0 && ulBegPos != 0)
						{
							bInfoValid = false;
						}
						
					}
					else
					{
						//���̲߳�֧�ֶϵ�������ֱ�����¿�ʼ
						CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ���̲߳�֧�ֶϵ�������ֱ�����¿�ʼ", this);
						ulBegPos = 0;
						ulBlockSize = ulFileSize;
						ulRecvSize = 0;
					}
					//
					// ���ϵ���Ϣ�Ƿ���Ч
					if (i)
					{
						ThreadInfo* pLastInfo = m_vecThrdInfo[i - 1];
						if (pLastInfo->ulBegPos + pLastInfo->ulBlockSize != ulBegPos)
						{
							bInfoValid = false;
						}
					}

					ThreadInfo* pInfo = new ThreadInfo;
					if (bInfoValid)
					{
						//�˷ֿ���Ϣ��Ч
						pInfo->ulBegPos = ulBegPos;
						pInfo->ulBlockSize = ulBlockSize;
						pInfo->ulRecvSize = ulRecvSize;
						m_ulDownFileSize += ulRecvSize;
					}
					else
					{
						//�˷ֿ���Ϣ��Ч����֮���������ش˿�
						CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �ϵ�������%d����Ϣ���󣬴������ش˿�����", this, i);
						pInfo->ulBegPos = i * (ulFileSize/iThreadCount);
						if (i == iThreadCount - 1)
						{
							pInfo->ulBlockSize = ulFileSize - i * (ulFileSize/iThreadCount);
						}
						else
						{
							pInfo->ulBlockSize = ulFileSize/iThreadCount;
						}
					}
					m_vecThrdInfo.push_back(pInfo);
				}
				
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �ϵ�������ʼ����һ�������ذٷֱ�%f%%", this, m_ulDownFileSize*100.0/m_ulFullFileSize);
				//
				for (int j = 0; j < iThreadCount; ++j)
				{
					ThreadInfo* pInfo = m_vecThrdInfo[j];
					if (pInfo->ulRecvSize < pInfo->ulBlockSize)
					{
						//�����߳���һ��δ�����꣬���¿�ʼ����
						DownThreadParam* param = new DownThreadParam;
						param->pDownloader = this;
						param->index = j;
						if (!CurlInit(param)
							|| threadpool_add(g_poolInstance.GetThreadPool(), DownloadFun, param, 0))
						{
							//��ͣ�Ѿ���ʼ������
							Pause();
							fclose(m_pFile);
							m_pFile = NULL;
							delete param;
							param = NULL;
							return false;
						}
						else
						{
							CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ���߳�%d����!", this, j);
						}
					}		     
				}
				//
				return true;
			}
		}
	}

	CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ʱ�ļ������⣬���¿�ʼ����", this);
	//��ʱ�ļ������⣬ɾ�������¿�ʼ����
	pthread_mutex_lock(&m_mutexFile);
	if (m_pFile) 
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	pthread_mutex_unlock(&m_mutexFile);
	//��ɾ������������ ���ɾ��������ʱ�ļ������
	if (_access(strTmpPath.c_str(), 0) != -1 && !DeleteFileA(strTmpPath.c_str()))
	{
		char ch[16]={0};
		int flag = 0;
		std::string strPath;
		do 
		{
			memset(ch, 0, sizeof(ch));
			sprintf_s(ch, "~bk%d", flag++);
			strPath = m_strDownloadPath + std::string(ch);
		} while (_access(strPath.c_str(), 0) != -1);
		m_strDownloadPath = strPath;
		CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ʱ�ļ���ռ���޷�ɾ������������Ŀ���ļ���Ϊ%s", this, strPath.c_str());
	}
	return Start(m_strUrl, m_strDownloadPath);
}

CURL *CurlDownloaderImpl::Create_Share_Curl()
{  
	CURL *curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_SHARE, sharedns_handle);  
	curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);
	return curl_handle;
}

bool CurlDownloaderImpl::DownloadInit()
{
	m_bInitOver = false;
	m_bInitInterrupt = false;
	DWORD dwTick = GetTickCount();
	CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p DownloadInit begin", this);
    bool bInitSuccess = false;
	//ȷ���ļ�����
	if (CheckFileLength())
	{
		//ȷ���Ƿ�֧�ַ�Ƭ����
		if (CheckIsSupportRange())
		{
			bInitSuccess = true;
			if (m_bSupportMultiDown)
			{
				//��һ��ȷ���Ƿ�֧��
				if (!CheckIsSupportRangeEx())
				{
					bInitSuccess = false;
				}
			}
		}
	}
	m_bInitOver = true;
	if (!bInitSuccess)
	{
		CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p DownloadInit Fail,Take time=%d", this, GetTickCount() - dwTick);
		return false;
	}
	//
	CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p DownloadInit end, IsMutilDonwload: %d, FileSize: %dB, Take time=%d", this, (m_bSupportMultiDown ? 1 : 0), m_ulFullFileSize, GetTickCount() - dwTick);
	//
	// 0KB�ļ�����֧�����أ���404�� 
	if (m_ulFullFileSize == 0)
	{
		m_iHttpCode = 404;
	}
	else
	{
		//�����м䷢�����ִ���ֻҪ���ճɹ���������͸�λ
		m_iHttpCode = 200;
		m_curlCode = CURLE_OK;
	}

	return m_ulFullFileSize != 0;
}

//��������
void CurlDownloaderImpl::ParseThreadParam(void* param, CurlDownloaderImpl** ppDown, int* pIndex)
{
	ParamBase* bparam = (ParamBase*)param;
	if (bparam && bparam->type == DOWNLOAD_THREAD)
	{
		DownThreadParam* downParam = (DownThreadParam*)bparam;
		if (downParam)
		{
			if (ppDown) *ppDown = downParam->pDownloader;
			if (downParam->pDownloader 
				&& downParam->index < downParam->pDownloader->m_vecThrdInfo.size())
			{
				if (pIndex) *pIndex = downParam->index;
			}
		}
	}
}

void CurlDownloaderImpl::FreeThreadParam(void* param)
{
	DownThreadParam* downParam = (DownThreadParam*)param;
	if (downParam)
	{
		delete downParam;
		downParam = NULL;
	}
}

size_t CurlDownloaderImpl::HeaderInfo(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::string* pHead = (std::string*)userdata;
	if (pHead)
	{
		pHead->append(std::string((char*)ptr, nmemb));
	}
	return size*nmemb;
}

size_t CurlDownloaderImpl::WriteData(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	//�ж��Ƿ�֧�ֶ��̷߳�Ƭ�ϴ�
	ParamBase* bparam = (ParamBase*)userdata;
	if (bparam && bparam->type == RANGE_CHECK)
	{
		RangeParam* rparam = (RangeParam*)bparam;
		rparam->nRecvSize += size*nmemb;
		if (rparam->nRecvSize > rparam->nRangeTestSize)
		{
			//��ʱ�Ѿ�ȷ����֧�ֶ��߳������ˣ�ֱ�ӷ���
			return size*nmemb - 1;
		}
		return size*nmemb;	
	}
	//
	CurlDownloaderImpl* pDown = NULL;
	ThreadInfo* pInfo = NULL;
	int index = -1;
	ParseThreadParam(userdata, &pDown, &index);
	if (pDown && index != -1) pInfo = pDown->m_vecThrdInfo[index];
	//
	size_t writeLen = 0;  
	if (pInfo)
	{
		//����ֹ����ֱ�ӷ���
		if (pDown->m_bTerminate)
		{
			return size*nmemb - 1;
		}
		pthread_mutex_lock(&pDown->m_mutexFile);
		if (pInfo->ulRecvSize < pInfo->ulBlockSize)
		{
			if (0 == _fseeki64(pDown->m_pFile, pInfo->ulBegPos + pInfo->ulRecvSize, SEEK_SET))
			{
				unsigned long ulUnRecvSize = pInfo->ulBlockSize - pInfo->ulRecvSize;
				size_t toWriteCount = (ulUnRecvSize > size * nmemb ? nmemb : ulUnRecvSize/size);
				writeLen = fwrite(ptr, size, toWriteCount, pDown->m_pFile);
				pInfo->ulRecvSize += writeLen*size;
			} 
			else
			{
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, _fseeki64 error! GetLastError:%d",
					pDown, GetLastError());
			}
		}
		else
		{
			//�߳����ݽ�����
		}
		pthread_mutex_unlock(&pDown->m_mutexFile);
		//
		pDown->m_ulDownFileSize += writeLen;

		// Ϊ�Ż��ϵ�������Ϊ��ʱ������ʱ��Ϣ 
		pDown->UpdateDownloadInfoInTmpFile(&index);

		//������߳���ͣ���������أ����̲߳�������ͣ
		if (pDown->m_bPause/* && pDown->m_bSupportMultiDown*/)
		{
			// ���߳̾�������ͣ���ָ�ʱ�����¿�ʼ���� 
			if (!pDown->m_bSupportMultiDown)
			{
				pInfo->ulRecvSize = 0;
				pDown->m_ulDownFileSize = 0;
			}
			//ͨ������ֵʹcurl_easy_peform����
			return size*nmemb - 1;
		}
	}  
	return /*writeLen*/size*nmemb; 
}

bool CurlDownloaderImpl::CheckIsFinish(bool* pbThrdRunFinish)
{
	//�������߳��Ƿ�ִ����
	bool bRunFinish = true;
	//�ļ��Ƿ�������
	bool bDownFinish = true;
	pthread_mutex_lock(&m_mutexFile);
	std::vector<ThreadInfo*>::const_iterator ita = m_vecThrdInfo.begin();
	while (ita != m_vecThrdInfo.end())
	{
		ThreadInfo* pInfo = *ita++;
		if (pInfo)
		{
			if (bDownFinish)		
				bDownFinish = (pInfo->ulBlockSize == pInfo->ulRecvSize);
			// ��ʱpCurl�Ѿ�Ϊ�գ���pthread_killȴ����0 
			if (bRunFinish)
			{
				//�жϵ�ǰ�߳��Ƿ��Ѿ����н���
				if (pthread_mutex_trylock(&pInfo->thrdMutex) == 0)
				{
					bRunFinish = true;
					pthread_mutex_unlock(&pInfo->thrdMutex);
				}
				else
				{
					bRunFinish = false;
				}
			}
			if (!bDownFinish && !bRunFinish) break;
		}
	}
	pthread_mutex_unlock(&m_mutexFile);
	// ��������͵�������Ҳ�� 
	if (pbThrdRunFinish) *pbThrdRunFinish = (bDownFinish ? true : bRunFinish);
	return bDownFinish && !m_vecThrdInfo.empty();
}

void CurlDownloaderImpl::UpdateDownloadInfoInTmpFile(int* pIndex)
{
	pthread_mutex_lock(&m_mutexFile);
	if (m_pFile)
	{
		int off = sizeof(unsigned long) - m_vecThrdInfo.size()*sizeof(unsigned long)*3;
		int iBeg, iEnd;
		if (pIndex)
		{
			//ֻ���´�ָ���߳̿����Ϣ
			iBeg = *pIndex;
			iEnd = iBeg + 1;
			off += (*pIndex)*sizeof(unsigned long)*3;
		}
		else
		{
			//��������
			iBeg = 0;
			iEnd = m_vecThrdInfo.size();
		}
		if (0 == _fseeki64(m_pFile, off, SEEK_END))
		{
			ThreadInfo* pInfo = NULL;
			for (int i = iBeg; i < iEnd; ++i)
			{
				pInfo = m_vecThrdInfo[i];
				fwrite(&pInfo->ulBegPos, sizeof(unsigned long), 1, m_pFile);
				fwrite(&pInfo->ulBlockSize, sizeof(unsigned long), 1, m_pFile);
				fwrite(&pInfo->ulRecvSize, sizeof(unsigned long), 1, m_pFile);
			}
		}
	}
	FFlushEx(m_pFile);
	pthread_mutex_unlock(&m_mutexFile);
}

bool CurlDownloaderImpl::FinalProc()
{
	bool bRet = false;
	//�ж��Ƿ��������
	bool bRunFinish = false;
	bool bFileDownFinish = CheckIsFinish(&bRunFinish);
	if (bRunFinish)
	{
		//���߳�ִ���꣬��ر���ʱ�ļ�
		pthread_mutex_lock(&m_mutexFile);
		if (m_pFile)
		{
			int iTryTimes = 0;
			//��û�йرճɹ���������ļ���ռ�ã��ȴ�ռ�����
			while (iTryTimes++ < 3)
			{
				if (fclose(m_pFile) == 0)
				{
					m_pFile = NULL;
					break;
				}
				else
				{
					if (iTryTimes == 3)
					{
						CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, fclose Fail, GetLastError:%d",
							this, GetLastError());
						FFlushEx(m_pFile);
						break;
					}
					Sleep(500);
				}
			}
		}
		pthread_mutex_unlock(&m_mutexFile);
		//���ļ�������ɣ����޸���ʱ�ļ�����
		if (bFileDownFinish)
		{
			std::string strTmpPath = m_strDownloadPath + DOWNLOAD_TMP_FILE_EXT;
			HANDLE hFile = CreateFileA(strTmpPath.c_str(), GENERIC_READ|GENERIC_WRITE, 
				0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			// ���ļ���ռ�ã�����һ�ݣ��ñ��ݵ��ļ������� 
			if (hFile == INVALID_HANDLE_VALUE && GetLastError() == ERROR_SHARING_VIOLATION)
			{
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ʱ�ļ���ռ�ã�����֮!",
					this);
				std::string strTmpPathX = strTmpPath + ".bk";
				if (CopyFileA(strTmpPath.c_str(), strTmpPathX.c_str(), FALSE))
				{
					CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ʱ�ļ������ɹ�!",
						this);
					strTmpPath = strTmpPathX;
					hFile = CreateFileA(strTmpPath.c_str(), GENERIC_READ|GENERIC_WRITE, 
						0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				}
				else
				{
					CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ʱ�ļ���ռ�ã�����ʧ��, GetLastError:%d", this, GetLastError());
				}
			}
			//
			if (hFile != INVALID_HANDLE_VALUE)
			{
				int off = sizeof(unsigned long) - m_vecThrdInfo.size()*sizeof(unsigned long)*3;
				DWORD dwPtr = SetFilePointer(hFile, off, NULL, FILE_END);
				if (dwPtr != INVALID_SET_FILE_POINTER)
				{
					bRet = (SetEndOfFile(hFile) == TRUE);
					if (!bRet)
					{
						CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, SetEndOfFile Fail, GetLastError:%d", this, GetLastError());
					}
				}
				else
				{
					CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, SetFilePointer Fail, GetLastError:%d", this, GetLastError());
				}
				CloseHandle(hFile);
				if (bRet)
				{
					int pos = m_strDownloadPath.rfind("~bk");
					if (pos != std::string::npos)
					{
						m_strDownloadPath = m_strDownloadPath.substr(0, pos);
						CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ����Ŀ���ļ�������ϵ�����ʱ��ʱ�ļ���ռ�ö����Ĺ������ڻ�ԭΪ%s", this, m_strDownloadPath.c_str());
					}
					//���ļ�������ɣ���������
					bRet = (0 == rename(strTmpPath.c_str(), m_strDownloadPath.c_str()));
					if (!bRet)
					{
						CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, rename Fail, GetLastError:%d",
							this, GetLastError());
					}
				}
			}
			else
			{
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, CreateFileA Fail, GetLastError:%d",
					this, GetLastError());
			}
		}
	}
	return bRet;
}

void CurlDownloaderImpl::GetProgress(unsigned long* pTotalFileSize, unsigned long* pDownSize)
{
	if (pTotalFileSize) *pTotalFileSize = m_ulFullFileSize;
	if (pDownSize) *pDownSize = m_ulDownFileSize;
}

bool CurlDownloaderImpl::IsMutliDownload()
{
	return m_bSupportMultiDown;
}

bool CurlDownloaderImpl::IsRedirected()
{
	return m_bRedirected;
}

bool CurlDownloaderImpl::WaitForFinish()
{
	std::vector<ThreadInfo*>::const_iterator ita = m_vecThrdInfo.begin();
	while (ita != m_vecThrdInfo.end())
	{
		ThreadInfo* pInfo = *ita++;
		if (pInfo && pInfo->pCurl) 
		{
			//�ȴ��˳�
			pthread_mutex_lock(&pInfo->thrdMutex);
			pthread_mutex_unlock(&pInfo->thrdMutex);
		}
	}
	bool bRet = FinalProc();
	return bRet;
}

CurlDownState CurlDownloaderImpl::GetCurlDownState()
{
	if (m_bPause)
	{
		return DOWN_PAUSE;
	}
	else if (m_bTerminate)
	{
		return DOWN_TERMINATE;
	}
	return DOWN_PROGRESS;
}

void CurlDownloaderImpl::SetProxy(const std::string& strProxy)
{
	m_strProxy = strProxy;
}

int CurlDownloaderImpl::GetLastHttpCode()
{
	return m_iHttpCode;
}

int CurlDownloaderImpl::GetLastCurlCode()
{
	return m_curlCode;
}

void CurlDownloaderImpl::Init()
{
	CURLcode code = (CURLcode)curl_global_init(CURL_GLOBAL_ALL);
	if (!sharedns_handle)  
	{  
		sharedns_handle = curl_share_init();  
		curl_share_setopt(sharedns_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);  
	}
}

void CurlDownloaderImpl::Uninit()
{
	if (sharedns_handle)
	{
		curl_share_cleanup(sharedns_handle);
		sharedns_handle = NULL;
	}
	curl_global_cleanup();
}

void CurlDownloaderImpl::SetCurlDownConfig(const CurlDownConfig& downCfg)
{
	g_curlDownCfg = downCfg;
}

CurlDownConfig CurlDownloaderImpl::GetCurlDownConfig()
{
	return g_curlDownCfg;
}

bool CurlDownloaderImpl::CheckSSL(CURL* pCurl)
{
	if (pCurl)
	{
		static std::string strRootDir;
		if (strRootDir.empty())
		{
			char path[MAX_PATH]={0};
			GetModuleFileNameA(NULL, path, MAX_PATH);
		    std::string strPath = std::string(path);
		    ReplaceStr(strPath, "/", "\\");
		    int pos = strPath.rfind('\\');
		    strRootDir = strPath.substr(0, pos);
		}
		std::string strCertPath = strRootDir + std::string("\\") 
			+ std::string(g_curlDownCfg.szSSLCrtName);
		std::string strKeyPath = strRootDir + std::string("\\") 
			+ std::string(g_curlDownCfg.szSSLKeyName);
		if (_access(strCertPath.c_str(), 0) == -1)
		{
			CURLD_FORMAT_LOG("%s is not exist!", g_curlDownCfg.szSSLCrtName);
			return false;
		}
		if (_access(strKeyPath.c_str(), 0) == -1)
		{
			CURLD_FORMAT_LOG("%s is not exist!", g_curlDownCfg.szSSLKeyName);
			return false;
		}
		curl_easy_setopt(pCurl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
		curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 1);
		curl_easy_setopt(pCurl, CURLOPT_SSLCERT, strCertPath.c_str());
		curl_easy_setopt(pCurl, CURLOPT_SSLKEY, strKeyPath.c_str());

		return true;
	}
	return false;
}

bool CurlDownloaderImpl::CheckFileLength()
{
	DWORD dwTick = GetTickCount();
	double downloadFileLenth = -1; 
	///////////////////////////////////////////
	// ��ȡ�����ļ�����Ϊ0Ҫ���� 
	int iTryTimes = 0;
	std::string strTmp;
	bool bFtp, bHttps = bFtp = false;

ReTry0:
	std::string strUrl = m_strUrl;
	CURL *curl = Create_Share_Curl(); 

ReTry1:
	if (m_bPause)
	{
		CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �⵽��ͣ", this);
		m_bInitInterrupt = true;
		return false;
	}

	if (!m_strProxy.empty())
	{
		curl_easy_setopt(curl, CURLOPT_PROXY, m_strProxy.c_str());
	}

	// ֧��SSL 
	bHttps = IsHttpsUrl(strUrl);
	if (bHttps && !CheckSSL(curl)) 
	{
		return false;
	}

	//����ض���-ǿ����GET����
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");		
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_HEADER, 1); 
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

	// ��������ʱ����
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, bHttps ? 2*g_curlDownCfg.iTimeOut : g_curlDownCfg.iTimeOut);

	// �Ż����ܣ���ֹ��ʱ���� 
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	if(IsFtpUrl(strUrl))
	{
		long filesize =0 ;

		//����ͷ������
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, GetContentLengthFunc);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &filesize);

		CStringA UserPswd(UserPwd);
		curl_easy_setopt(curl, CURLOPT_USERPWD, UserPswd);

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}

	long lCode = 0;
	if (m_bPause)
	{
		CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �⵽��ͣ", this);
		m_bInitInterrupt = true;
		return false;
	}

	CURLcode code = (CURLcode)curl_easy_perform(curl);
	if (code == CURLE_OK)  
	{  	
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &lCode);
		if (lCode == 301 || lCode == 302)
		{
			//�����ض��򣬻�ȡ�ض�����url
			char *redirecturl = {0};
			curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &redirecturl);

			//myLog(coutx<<"�����ض���RedirectUrl: "<<redicturl<<"\n";)
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �����ض���RedirectUrl: %s",
				this, redirecturl);
			strUrl = redirecturl;
			m_bRedirected = true;
			goto ReTry1;
		}
		else if(IsFtpUrl(strUrl))
		{
			curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);
		}
		else if (lCode >= 200 && lCode < 300)
		{
			if (lCode != 200)
			{
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, HttpCode: %d", this, lCode);
			}
			curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth); 
		}
	}
	else
	{
		CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ȡ�ļ�����ʧ��, CURLcode=%d", 
			this, code);
	}

	if (curl)
	{
		curl_easy_cleanup(curl);
		curl = NULL;
	}	
	//
	m_curlCode = code;
	m_iHttpCode = lCode;
	//
	// 0�ֽ��ļ���СҲҪ����
	if (downloadFileLenth == -1 || downloadFileLenth == 0)
	{
		if (lCode == 404)
		{
			//��Դ�����ڣ��Ͳ�������
			return false;
		}
		// ���Ի�ȡ�ļ�����
		if (iTryTimes < GetMaxRetryTimes())
		{
			iTryTimes++;
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ȡ�ļ�����ʧ��, lastHttpCode:%d, CURLcode:%d, ���Ե�%d������", this, lCode, code, iTryTimes);
			DWORD dwFlags;
			// ����ͣ����ֹ��������򲻱����� 
			if (!m_bPause && !m_bTerminate && IsNetworkAlive(&dwFlags))
			{
				// ����ʱ�䣺һ�룬���룬���� 
				float iVal = (float)pow(2.0, iTryTimes - 1);
				SleepEx((unsigned long)iVal*1000);
			}
			goto ReTry0;
		}
		else
		{
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ���Ի�ȡ�ļ�����ʧ��, LastHttpCode:%d", this, lCode);
			//RICHLOG<<"���Ի�ȡ�ļ�����ʧ��";
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, end, Take time=%d", this, 
				GetTickCount() - dwTick);
			return false;
		}
	}

	if (iTryTimes)
	{
		CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, ��ȡ�ļ����ȳɹ������Դ�����%d", 
			this, iTryTimes);
	}

	// url����
	m_strUrl = strUrl;
	/////////////////////////////////////////
	//��ȡҪ���ص��ļ���С
	m_ulFullFileSize = (unsigned long)downloadFileLenth; 

	return true;
}

bool CurlDownloaderImpl::CheckIsSupportRange()
{
	int iTryTimes = 0;
	std::string strUrl = m_strUrl;
	bool bHttps = false;
	// �ж��Ƿ�֧�ֶϵ����� 
	// �ļ�С����С�ֿ飬��ʹ�ж�֧�ֶ��̣߳�����ǿ�һ���̣߳���û��Ҫ�ж�
	// ��Ϊ���߳�Ӱ�쵽��ͣ���ָ���������Ҫ�������ж�
	if (m_ulFullFileSize/* > MMBLOCK_SIZE*/)
	{
		// �ж��Ƿ�֧�ֶ��߳�Ҳ���� 
		iTryTimes = 0;
ReTry2:
		if (m_bPause)
		{
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �⵽��ͣ", this);
			m_bInitInterrupt = true;
			return false;
		}
		//curl = curl_easy_init(); 
		CURL* curl = Create_Share_Curl();
		if (!m_strProxy.empty())
		{
			curl_easy_setopt(curl, CURLOPT_PROXY, m_strProxy.c_str());
		}	
		// ֧��SSL 
		bHttps = IsHttpsUrl(strUrl);
		if (bHttps && !CheckSSL(curl)) 
		{
			return false;
		}
		curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());  
		curl_easy_setopt(curl, CURLOPT_HEADER, 1); 
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

		// ͨ��ͷ����Ϣ�ж��Ƿ�֧�ֶϵ�����
		std::string strHeader;
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &strHeader);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &CurlDownloaderImpl::HeaderInfo);
		curl_easy_setopt(curl, CURLOPT_RANGE, "0-");

		// ��������ʱ����
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, bHttps ? 2*g_curlDownCfg.iTimeOut : g_curlDownCfg.iTimeOut);

		// �Ż����ܣ���ֹ��ʱ���� 
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

		// FTP
		if(IsFtpUrl(strUrl))
		{
			long filesize =0 ;

			//����ͷ������
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, GetContentLengthFunc);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &filesize);

			CStringA UserPswd(UserPwd);
			curl_easy_setopt(curl, CURLOPT_USERPWD, UserPswd);

			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		}

		CURLcode code = (CURLcode)curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		curl = NULL;

		if (code == CURLE_OK)
		{
			m_bSupportMultiDown = (/*strHeader.find("Accept-Ranges: bytes") != std::string::npos
				|| */strHeader.find("Content-Range: bytes") != std::string::npos);
		}
		else
		{
			// �����ж��Ƿ�֧�ֶ��߳� 
			if (iTryTimes < GetMaxRetryTimes())
			{
				iTryTimes++;
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �ж��Ƿ�֧�ֶ��߳�ʧ��, CURLcode:%d, ���Ե�%d������", this, code, iTryTimes);
				DWORD dwFlags;
				// ����ͣ����ֹ��������򲻱����� 
				if (!m_bPause && !m_bTerminate && IsNetworkAlive(&dwFlags))
				{
					// ����ʱ�䣺һ�룬���룬���� 
					float iVal = (float)pow(2.0, iTryTimes - 1);
					SleepEx((unsigned long)iVal*1000);
				}
				goto ReTry2;
			}
			else
			{
				CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �ж��Ƿ�֧�ֶ��߳�ʧ�ܣ�Ĭ�ϲ��õ��߳�����", this);
			}
		}
	}
	else
	{
		//CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �ļ���С<��С�ֿ�5M, �����ж��Ƿ�֧�ֶ��̣߳�ֱ�Ӳ��õ��߳�����", this);
	}
	return true;
}

bool CurlDownloaderImpl::CheckIsSupportRangeEx()
{
	if (m_ulFullFileSize)
	{
		//�ж��Ƿ�֧�ֶ��̷߳�Ƭ����
		if (m_bPause)
		{
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �⵽��ͣ", this);
			m_bInitInterrupt = true;
			return false;
		}
		CURL* curl = Create_Share_Curl(); 
		bool bHttps = false;
		if (!m_strProxy.empty())
		{
			curl_easy_setopt(curl, CURLOPT_PROXY, m_strProxy.c_str());
		}
		// ֧��SSL 
		bHttps = IsHttpsUrl(m_strUrl);
		if (bHttps && !CheckSSL(curl)) 
		{
			return false;
		}
		curl_easy_setopt(curl, CURLOPT_URL, m_strUrl.c_str()); 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlDownloaderImpl::WriteData); 
		//
		const int iRangeTestSize = 
			m_ulFullFileSize > RANGE_TEST_RECV_SIZE ? RANGE_TEST_RECV_SIZE : m_ulFullFileSize/2;
		RangeParam param;
		param.nRangeTestSize = iRangeTestSize;
		//
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &param); 
		//�ض���
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); 
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
		//��������һС���ֽڵ�����
		char range[32] = {0};
		sprintf_s(range, "1-%d", iRangeTestSize);
		curl_easy_setopt(curl, CURLOPT_RANGE, range); 
		// �������ٶ�< 1 �ֽ�/�� ���� 5 ��ʱ,�����ӻ���ֹ����������ƽ��ճ�ʱ 
		curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);  
		curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 5L); 
		// ֻ���������ӳ�ʱ����Ϊ����ʱ�䲻�ɿ�
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, bHttps ? 2*g_curlDownCfg.iTimeOut : g_curlDownCfg.iTimeOut);
		CURLcode code = (CURLcode)curl_easy_perform(curl);
		if (CURLE_OK == code)
		{
			m_bSupportMultiDown = true;
		}
		else
		{
			m_bSupportMultiDown = false;
		}
		curl_easy_cleanup(curl);
		curl = NULL;
	}
	else
	{
		//���ļ�̫С��ֱ���õ��߳�
		m_bSupportMultiDown = false;
	}
	return true;
}

std::string CurlDownloaderImpl::GetRealUrl()
{
	return m_strUrl;
}

int CurlDownloaderImpl::GetThreadCount()
{
	return m_vecThrdInfo.size();
}

void CurlDownloaderImpl::SleepEx(unsigned long ulMilliseconds)
{
	unsigned long ulSleeped = 0;
	while (ulSleeped < ulMilliseconds)
	{
		if (m_bPause)
		{
			CURLD_FORMAT_LOG("CurlDownloader Obj Ptr: 0X%p, �⵽��ͣ", this);
			break;
		}
		Sleep(100);
		ulSleeped += 100;
	}
}

void CurlDownloaderImpl::SetPause(bool bPause)
{
	m_bPause = bPause;
}

bool CurlDownloaderImpl::IsDownloadInitOver()
{
	return m_bInitOver;
}

bool CurlDownloaderImpl::IsDownloadInitInterrupt()
{
	return m_bInitInterrupt;
}

void CurlDownloaderImpl::SetMaxRetryTimes(int iTimes)
{
	m_iMaxRetryTimes = iTimes;
}

int CurlDownloaderImpl::GetMaxRetryTimes()
{
	// ���û��Լ������ã������ȫ������
	return m_iMaxRetryTimes == -1 ? g_curlDownCfg.iMaxTryTimes : m_iMaxRetryTimes;
}

/*
int main(int c, char **argv)
{  
    CURL *curlhandle = NULL;
    CURL *curldwn = NULL;
    curl_global_init(CURL_GLOBAL_ALL);
    curlhandle = curl_easy_init();  
    curldwn = curl_easy_init();  
    FTP_UpLoad(curlhandle, "ftp://192.168.0.185/a/success", "D:/abc.jpg", 1, 3);
    FTP_DownLoad(curldwn, "ftp://192.168.0.185/a/success", "D:/abc1.jpg", 1, 3);
    curl_easy_cleanup(curlhandle);
    curl_easy_cleanup(curldwn);
    curl_global_cleanup();
    return 0;
}
*/