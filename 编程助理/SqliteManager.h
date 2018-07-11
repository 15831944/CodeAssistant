#pragma once
/* Sqlite ���ݿ����
 * ��;: ����Sqlite���ݿ�
 * ����: �ϻ�(��Ӱ��)
 * ����: 2018-6-22
 * ��Ȩ���� 2016 - 2018 ��Ӱ��������
 * https://www.shadowviolet.com
 */

// ֧��XPϵͳ
//#define _WIN32_WINNT 0x0502

// �������ݿ�ͷ�ļ�֧�����ݿ����
#include "sqlite3/sqlite3.h"
using namespace SqliteSpace;


// �����ַ�����ͷ�ļ������ֿռ�
#include "iostream"
#include <algorithm>
using namespace std;

#include<cstdlib>
#include<ctime>

#include <string>  
using std::string;
using std::wstring;

// ���������ͷ�ļ������ֿռ�
#include <afxtempl.h>
#include <vector>
using std::vector;



// ���� ModifyDataTable �Ĳ���

// --------- ���ݱ���� ---------|
                              // |
// ��������                      |
#define REN_TABLE  0x3000     // |
// ------------------------------|
// �����                        |
#define ADD_COLUMN 0x3001     // |
// ------------------------------|
// ɾ����                        |
#define DEL_COLUMN 0x3002     // |
// ------------------------------|
// �޸���                        |
#define REN_COLUMN 0x3003     // |
                              // |
// ------------------------------|


// ���� SelectData �Ĳ���

// ---------- ����ģʽ ----------|
                              // |
// ��������                      |
#define MOD_ASC    0x4000     // |
// ------------------------------|
// ��������                      |
#define MOD_DESC   0x4001     // |
                              // |
// ------------------------------|


// �����ѯ����
typedef struct qureyResult
{
	int nRow;
	int nColumn;
	int nIndex;
	char** pResult;
	qureyResult(int row, int column, char**p)
	{
		nRow = row;
		nColumn = column;
		pResult = NULL;
		nIndex = 0;
		if (nRow > 0)
		{
			pResult = p;
		}
	}
	~qureyResult()
	{
		if (pResult)
		{
			sqlite3_free_table(pResult);
		}
	}
	bool next()
	{
		if (nIndex == 0)
		{
			if (nRow > 0)
			{
				++nIndex;
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if ((nRow*nColumn) >= ((nIndex + 1)*nColumn))
			{
				++nIndex;
				return true;
			}
			else {
				return false;
			}
		}
	}
	char* value(char *pColumnName)
	{
		int index = nIndex*nColumn;
		for (int i = 0; i < nColumn; ++i)
		{
			if (strcmp(pColumnName, pResult[i]) == 0)
			{
				return pResult[index];
			}
			++index;
		}
		return NULL;
	}
} Result;


// CSqliteManager ���ݿ������
class CSqliteManager
{
private: bool Init;

public:
	/////////////////////////////////////�������۹�//////////////////////////////////////////////////////////////////////////

	// Ĭ�Ϲ��캯��
	CSqliteManager(bool Console = FALSE);

	// �����ݿ����ƺ�·�������Ĺ��캯��
	CSqliteManager(CString Name, CString Path);

	// Ĭ���۹�����
	virtual ~CSqliteManager();

	/////////////////////////////////////���Ա����//////////////////////////////////////////////////////////////////////////

	// ���ݿ�������
	static sqlite3 *db;

	// ���ݿ��¼������
	static sqlite3_stmt * stmt;

	// ���ݿ������Ϣ����
	static char * errMsg;

	// ��¼��
	static char **pRes;

	// �ж���
	static int nRow, nCol;

	// ��ʾ���ݿ������Ƿ�ɹ�
	static BOOL IsConnect;

	// ����̨��ʶ
	static bool IsConsole;

	// ���ݿ�汾��
	static int version;

	// ��ѯ�������
	static CList <CString, CString&> pResult;

	/////////////////////////////////////���Ա����//////////////////////////////////////////////////////////////////////////
	

	////////////////////////////////////������������/////////////////////////////////////////////////////////////////////////


	// ���Ҳ�������
	static int FindCharCount(CString csStr, char c);

	// �ָ��ַ���
	static void Split(CString source, CString divKey, CStringArray &dest);

	//���ڼ����ļ����ڵ��ļ�����
	static int CountFile(CString Path);

	////////////////////////////////////���ﴦ�����/////////////////////////////////////////////////////////////////////////

	//��������  
	static bool transaction(sqlite3 *p);

	//�ύ����  
	static bool commitTransaction(sqlite3 *p);

	//�ع�����  
	static bool rollbackTransaction(sqlite3 *p);

	////////////////////////////////////���ݴ������/////////////////////////////////////////////////////////////////////////

	//bool Update(sqlite3 *p, const string &sql);

	static Result *Query(sqlite3 *p, const string &sql);


	///////////////////////////////////���ݿ��������////////////////////////////////////////////////////////////////////////


	// ��Ŀ�����ݿ�����
	static BOOL OpenDataBase(CString Name, CString Path = _T(""));


	// �ر�Ŀ�����ݿ�����
	static BOOL CloseDataBase();


	// ����һ��Ŀ�����ݿ�
	static BOOL CreateDataBase(CString Name, CString Path = _T(""));


	// ɾ��һ��Ŀ�����ݿ�
	static BOOL DeleteDataBase(CString Name, CString Path = _T(""));


	// �޸�Ŀ�����ݿ�����
	static BOOL ReNameDataBase(CString OldName, CString OldPath = _T(""), CString NewName = _T(""), CString NewPath = _T(""));


	// �ƶ�һ��Ŀ�����ݿ�
	static BOOL ReMoveDataBase(CString OldName, CString OldPath = _T(""), CString NewName = _T(""), CString NewPath = _T(""));


	// ���Ŀ�����ݿ����
	static BOOL CheckDataBase(CString Name, CString Path = _T(""));


	// ͳ��Ŀ�����ݿ�����
	static BOOL CountDataBase(CString DataBasePath, int &Count);


	// ����Ŀ�����ݿ�����
	static BOOL BackupDataBase(CString DataBasePath, CString BackupPath);


	// ��ԭĿ�����ݿ�����
	static BOOL RestoreDataBase(CString DataBasePath, CString RestorePath);


	// Զ������Ŀ�����ݿ�
	static BOOL RemoteDataBase(CString Name, CString Path);


	// ����Ŀ�����ݿ�����
	static BOOL EncryptionDataBase(CString DataBaseName, CString Password);


	// ����Ŀ�����ݿ�����
	static BOOL DecryptionDataBase(CString DataBaseName, CString Password);


	// �ϴ�Զ��Ŀ�����ݿ�
	static BOOL UploadDataBase(CString DataBasePath, CString UploadPath);


	// ����Զ��Ŀ�����ݿ�
	static BOOL DownloadDataBase(CString DataBasePath, CString DownloadPath);


	///////////////////////////////////���ݱ��������////////////////////////////////////////////////////////////////////////


	// ��ȡ���ݿ������б������
	static BOOL GetTableName(CString &TableName);


	// ��ȡ���ݿ������б������
	static BOOL GetDataTable(CString &DataTable);


	// ��ȡ���ݿ������ݱ����Ϣ
	static BOOL GetTableData(CString TableName, CString &SQL_Data);


	// ������ݱ��������е�����
	static BOOL GetColName(CString TableName, CString &ColName);


	// ������ݱ��������е�����
	static BOOL GetColType(CString TableName, CString &ColType);


	// ��Ŀ�����ݿ��д������ݱ�
	static BOOL CreateDataTable(CString TableName, CString Params);


	// ��Ŀ�����ݿ���ɾ�����ݱ�
	static BOOL DeleteDataTable(CString TableName);


	// ��Ŀ�����ݿ����޸����ݱ�
	static BOOL UpdataDataTable(CString TableName, int Operation, CString Params, CString NewParams = _T(""));


	// ��Ŀ�����ݿ��м�����ݱ�     ( ������ݱ��Ƿ���� )
	static BOOL CheckDataTable (CString TableName);


	// ��Ŀ�����ݿ���ͳ�����ݱ�     ( ͳ�����ݱ��ܹ����� )
	static BOOL CountDataTable (int &Count);


	// ��Ŀ�����ݿ��е������ݱ�
	static BOOL ImportDataTable(CString TableName, CString TargetName);


	// ��Ŀ�����ݿ��е������ݱ�
	static BOOL ExportDataTable(CString TableName, CString TargetName);


	///////////////////////////////////�������������////////////////////////////////////////////////////////////////////////


	// ��Ŀ�����ݱ������������
	static BOOL InsertData(CString TableName, CString Params);


	// ��Ŀ�����ݱ���ɾ��������
	static BOOL DeleteData(CString TableName, CString Params);


	// ��Ŀ�����ݱ����޸ĵ�����
	static BOOL UpdataData(CString TableName, CString Column, CString NewData, CString Params);


	// ��Ŀ�����ݱ����޸�������
	static BOOL UpdataData(CString TableName, CString ColumnParams, CString Params);


	// ��Ŀ�����ݱ��в�ѯ������
	static BOOL SelectData(CString TableName, vector<CString> &pResult, CString Params = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString GROUP = _T(""), CString HAVING = _T(""));


	// ��Ŀ�����ݱ��в�ѯ������
	static BOOL SelectData(CString TableName, CString &pResult, int Col, CString Params = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString GROUP = _T(""), CString HAVING = _T(""));
	
	
	// ��Ŀ�����ݱ��в�ѯ����ֵ
	static BOOL SelectData(CString TableName, CString &pResult, CString Column, CString Params = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString GROUP = _T(""), CString HAVING = _T(""));


	// ��Ŀ�����ݱ��м��������
	static BOOL CheckData (CString TableName, int Col, CString Params = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString GROUP = _T(""), CString HAVING = _T(""));


	// ��Ŀ�����ݱ��м��������
	static BOOL CheckData (CString TableName, CString Column, CString Params = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString GROUP = _T(""), CString HAVING = _T(""));


	// ͳ��Ŀ�����ݱ�����������
	static BOOL CountNumber(CString TableName, CString Params, int &Count);


	///////////////////////////////////������������////////////////////////////////////////////////////////////////////////


	// ��Ŀ�����ݱ������������
	static BOOL SumData(CString TableName, CString Column, CString Params, double &SumData);


	// ��Ŀ�����ݱ������������
	static BOOL ProductData(CString TableName, CString Column, CString Params, double &ProductData);


	// ��Ŀ�����ݱ�����������ƽ����
	static BOOL AvgData(CString TableName, CString Column ,CString Params, double &AvgData);


	// ��Ŀ�����ݱ����������������
	static BOOL PluData(CString TableName, CString Column, CString Params, double &PluData);


	// ��Ŀ�����ݱ�������������λ��
	static BOOL MidData(CString TableName, CString Column, CString Params, double &MidData);


	// ��Ŀ�����ݱ��������������ֵ
	static BOOL MaxData(CString TableName, CString Column, CString Params, double &MaxData);


	// ��Ŀ�����ݱ�������������Сֵ
	static BOOL MinData(CString TableName, CString Column, CString Params, double &MinData);


	// ��Ŀ�����ݱ���ȡ�����������
	static BOOL RandData(CString TableName, CString Column, CString Params, int &RanData);


	///////////////////////////////////���ݱ���ת������////////////////////////////////////////////////////////////////////////


	//ACSIIתUnicode
	static wstring AcsiiToUnicode( const string  & acsii_string);

	//ACSIIתUTF8  
	static string  AcsiiToUtf8(    const string  & acsii_string);

	//UnicodeתACSII  
	static string  UnicodeToAcsii( const wstring & unicode_string);

	//UnicodeתUTF8  
	static string  UnicodeToUtf8(  const wstring & unicode_string);

	//UTF8תACSII 
	static string  Utf8ToAcsii(    const string  & utf8_string);

	//UTF8תUnicode  
	static wstring Utf8ToUnicode(  const string  & utf8_string);
};

