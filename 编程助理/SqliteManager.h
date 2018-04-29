#pragma once

// �������ݿ�ͷ�ļ�֧�����ݿ����
#include "sqlite3\sqlite3.h"
using namespace SqliteSpace;


// �����ַ�����ͷ�ļ������ֿռ�
#include <string>  
#include <xstring>  
using std::string;
using std::wstring;

// ���������ͷ�ļ������ֿռ�
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
public:
	/////////////////////////////////////�������۹�//////////////////////////////////////////////////////////////////////////

	// Ĭ�Ϲ��캯��
	CSqliteManager();

	// �����ݿ����ƺ�·�������Ĺ��캯��
	CSqliteManager(CString Name, CString Path);

	// Ĭ���۹�����
	virtual ~CSqliteManager();


	/////////////////////////////////////���Ա����//////////////////////////////////////////////////////////////////////////

	// ���ݿ�������
	sqlite3 *db;

	// ���ݿ��¼������
	sqlite3_stmt * stmt;

	// ���ݿ������Ϣ����
	char * errMsg;

	// ��¼��
	char **pRes;

	// �ж���
	int nRow, nCol;

	// ��ʾ���ݿ������Ƿ�ɹ�
	BOOL IsConnect;

	// ���ݿ�汾��
	int version;

	// ��ѯ�������
	CList <CString, CString&> pResult;

	/////////////////////////////////////���Ա����//////////////////////////////////////////////////////////////////////////
	

	////////////////////////////////////������������/////////////////////////////////////////////////////////////////////////


	// ���Ҳ�������
	int FindCharCount(CString csStr, char c);

	//���ڼ����ļ����ڵ��ļ�����
	int CountFile(CString Path);

	////////////////////////////////////���ﴦ�����/////////////////////////////////////////////////////////////////////////

	//��������  
	bool transaction(sqlite3 *p);

	//�ύ����  
	bool commitTransaction(sqlite3 *p);

	//�ع�����  
	bool rollbackTransaction(sqlite3 *p);

	////////////////////////////////////���ݴ������/////////////////////////////////////////////////////////////////////////

	//bool Update(sqlite3 *p, const string &sql);

	Result *Query(sqlite3 *p, const string &sql);


	///////////////////////////////////���ݿ��������////////////////////////////////////////////////////////////////////////


	// ��Ŀ�����ݿ�����
	BOOL OpenDataBase(CString Name, CString Path = _T(""));


	// �ر�Ŀ�����ݿ�����
	BOOL CloseDataBase();


	// ����һ��Ŀ�����ݿ�
	BOOL CreateDataBase(CString Name, CString Path = _T(""));


	// ɾ��һ��Ŀ�����ݿ�
	BOOL DeleteDataBase(CString Name, CString Path = _T(""));


	// �޸�Ŀ�����ݿ�����
	BOOL ReNameDataBase(CString OldName, CString OldPath = _T(""), CString NewName = _T(""), CString NewPath = _T(""));


	// �ƶ�һ��Ŀ�����ݿ�
	BOOL ReMoveDataBase(CString OldName, CString OldPath = _T(""), CString NewName = _T(""), CString NewPath = _T(""));


	// ���Ŀ�����ݿ����
	BOOL CheckDataBase(CString Name, CString Path = _T(""));


	// ͳ��Ŀ�����ݿ�����
	BOOL CountDataBase(CString DataBasePath, int &Count);


	// ����Ŀ�����ݿ�����
	BOOL BackupDataBase(CString DataBasePath, CString BackupPath);


	// ��ԭĿ�����ݿ�����
	BOOL RestoreDataBase(CString DataBasePath, CString RestorePath);


	// Զ������Ŀ�����ݿ�
	BOOL RemoteDataBase(CString Name, CString Path);


	// ����Ŀ�����ݿ�����
	BOOL EncryptionDataBase(CString DataBaseName, CString Password);


	// ����Ŀ�����ݿ�����
	BOOL DecryptionDataBase(CString DataBaseName, CString Password);


	// �ϴ�Զ��Ŀ�����ݿ�
	BOOL UploadDataBase(CString DataBasePath, CString UploadPath);


	// ����Զ��Ŀ�����ݿ�
	BOOL DownloadDataBase(CString DataBasePath, CString DownloadPath);


	///////////////////////////////////���ݱ��������////////////////////////////////////////////////////////////////////////


	// ��ȡ���ݿ������б������
	BOOL GetTableName(CString &TableName);


	// ��ȡ���ݿ������б������
	BOOL GetDataTable(CString &DataTable);


	// ��ȡ���ݿ������ݱ����Ϣ
	BOOL GetTableData(CString TableName, CString &SQL_Data);


	// ������ݱ��������е�����
	BOOL GetColName(CString TableName, CString &ColName);


	// ������ݱ��������е�����
	BOOL GetColType(CString TableName, CString &ColType);


	// ��Ŀ�����ݿ��д������ݱ�
	BOOL CreateDataTable(CString TableName, CString Params);


	// ��Ŀ�����ݿ���ɾ�����ݱ�
	BOOL DeleteDataTable(CString TableName);


	// ��Ŀ�����ݿ����޸����ݱ�
	BOOL UpdataDataTable(CString TableName, int Operation, CString Params, CString NewParams = _T(""));


	// ��Ŀ�����ݿ��м�����ݱ�     ( ������ݱ��Ƿ���� )
	BOOL CheckDataTable (CString TableName);


	// ��Ŀ�����ݿ���ͳ�����ݱ�     ( ͳ�����ݱ��ܹ����� )
	BOOL CountDataTable (int &Count);


	// ��Ŀ�����ݿ��е������ݱ�
	BOOL ImportDataTable(CString TableName, CString TargetName);


	// ��Ŀ�����ݿ��е������ݱ�
	BOOL ExportDataTable(CString TableName, CString TargetName);


	///////////////////////////////////�������������////////////////////////////////////////////////////////////////////////


	// ��Ŀ�����ݱ������������
	BOOL InsertData(CString TableName, CString Params);


	// ��Ŀ�����ݱ���ɾ��������
	BOOL DeleteData(CString TableName, CString Params);


	// ��Ŀ�����ݱ����޸ĵ�����
	BOOL UpdataData(CString TableName, CString Column, CString NewData, CString Params);


	// ��Ŀ�����ݱ����޸�������
	BOOL UpdataData(CString TableName, CString ColumnParams, CString Params);


	// ��Ŀ�����ݱ��в�ѯ������
	BOOL SelectData(CString TableName, vector<CString> &pResult,  BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString Params = _T(""), CString GROUP = _T(""), CString HAVING = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC);


	// ��Ŀ�����ݱ��в�ѯ������
	BOOL SelectData(CString TableName, CString &pResult, int Col, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString Params = _T(""), CString GROUP = _T(""), CString HAVING = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC);


	// ��Ŀ�����ݱ��м��������
	BOOL CheckData (CString TableName, int Col, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString Params = _T(""), CString GROUP = _T(""), CString HAVING = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC);


	// ͳ��Ŀ�����ݱ�����������
	BOOL CountNumber(CString TableName, CString Params, int &Count);


	// ��Ŀ�����ݱ��е���������
	BOOL ImportData (CString TableName, CString TargetTableName, CString Params);


	// ��Ŀ�����ݱ��е���������
	BOOL ExportData (CString TableName, CString TargetTableName, CString Params);


	// ɾ��Ŀ�����ݱ����о�����
	BOOL DeleteRowData(CString TableName, int Row);


	// ɾ��Ŀ�����ݱ����о�����
	BOOL DeleteColData(CString TableName, int Col);


	// ɾ��Ŀ�����ݱ��г�������
	BOOL DeleteMinData(CString TableName);


	// ɾ��Ŀ�����ݱ��м������
	BOOL DeleteMidData(CString TableName);


	// ɾ��Ŀ�����ݱ���ĩ������
	BOOL DeleteMaxData(CString TableName);


	// �޸�Ŀ�����ݱ����о�����
	BOOL UpdataRowData(CString TableName, int Row, CString UpdataData);


	// �޸�Ŀ�����ݱ����о�����
	BOOL UpdataColData(CString TableName, int Col, CString UpdataData);


	// �޸�Ŀ�����ݱ��г�������
	BOOL UpdataMinData(CString TableName, CString UpdataData);


	// �޸�Ŀ�����ݱ��м������
	BOOL UpdataMidData(CString TableName, CString UpdataData);


	// �޸�Ŀ�����ݱ���ĩ������
	BOOL UpdataMaxData(CString TableName, CString UpdataData);


	// ����Ŀ�����ݱ����о�����
	BOOL SelectRowData(CString TableName, int Row, CString &RowData);


	// ����Ŀ�����ݱ����о�����
	BOOL SelectColData(CString TableName, int Col, CString &ColData);


	// ����Ŀ�����ݱ��г�������
	BOOL SelectMinData(CString TableName, CString &MinData);


	// �޸�Ŀ�����ݱ��м������
	BOOL SelectMidData(CString TableName, CString &MidData);


	// ����Ŀ�����ݱ���ĩ������
	BOOL SelectMaxData(CString TableName, CString &MaxData);


	// ΪĿ�����ݱ��о���������
	BOOL SelectSortData(CString TableName, int Sort, CString &SortData);


	// �����������ݱ������˳��
	BOOL ChangeSortData(CString TableName, int Sort);


	// ��Ŀ�����ݱ���ɸѡ������
	BOOL FilterData(CString TableName, CString Params, CString &FilterData);


	// ��Ŀ�����ݱ��еõ����������
	BOOL RanData(CString TableName, CString Params, CString &RanData);


	///////////////////////////////////������������////////////////////////////////////////////////////////////////////////


	// ��Ŀ�����ݱ������������
	BOOL SumData(CString TableName, CString Params, int &SumData);


	// ��Ŀ�����ݱ������������
	BOOL SubData(CString TableName, CString Params, int &SubData);


	// ��Ŀ�����ݱ������������
	BOOL ProductData(CString TableName, CString Params, int &ProductData);


	// ��Ŀ�����ݱ�������������
	BOOL QuotientData(CString TableName, CString Params, int &QuotientData);


	// ��Ŀ�����ݱ�������������
	BOOL ModData(CString TableName, CString Params, int &ModData);


	// ��Ŀ�����ݱ�������������
	BOOL PowerData(CString TableName, CString Params, int &PowerData, int &Power);


	// ��Ŀ�����ݱ��������������
	BOOL LogData(CString TableName, CString Params, int &LogData, int &Log);


	// ��Ŀ�����ݱ�����������׳�
	BOOL FactData(CString TableName, CString Params, int &FactData);


	// ��Ŀ�����ݱ�����������˷�
	BOOL ExpData(CString TableName, CString Params, int &ExpData, int &Exp);


	// ��Ŀ�����ݱ���������������
	BOOL PluData(CString TableName, CString Params, int &PluData);


	// ��Ŀ�����ݱ������������
	BOOL LimData(CString TableName, CString Params, int &LimData);


	// ��Ŀ�����ݱ���������������
	BOOL SinData(CString TableName, CString Params, int &SinData);


	// ��Ŀ�����ݱ���������������
	BOOL CosData(CString TableName, CString Params, int &CosData);


	// ��Ŀ�����ݱ���������������
	BOOL TanData(CString TableName, CString Params, int &TanData);


	// ��Ŀ�����ݱ�����������ƽ����
	BOOL AvgData(CString TableName, CString Params, int &AvgData);


	// ��Ŀ�����ݱ�������������λ��
	BOOL MidData(CString TableName, CString Params, int &MidData);


	// ��Ŀ�����ݱ�����������ٷֱ�
	BOOL PerData(CString TableName, CString Params, int &PerData);


	// ��Ŀ�����ݱ�����������ƽ����
	BOOL SqrData(CString TableName, CString Params, int &SqrData);


	// ��Ŀ�����ݱ�����������������
	BOOL CubData(CString TableName, CString Params, int &CubData);


	// ��Ŀ�����ݱ��������������ֵ
	BOOL AbsData(CString TableName, CString Params, int &AbsData);


	// ��Ŀ�����ݱ��������������ֵ
	BOOL MaxData(CString TableName, CString Params, int &AbsData);


	// ��Ŀ�����ݱ�������������Сֵ
	BOOL MinData(CString TableName, CString Params, int &AbsData);


	// ��Ŀ�����ݱ��������������Լ��
	BOOL GcdData(CString TableName, CString Params, int &GcdData);


	// ��Ŀ�����ݱ�������������С������
	BOOL LcmData(CString TableName, CString Params, int &LcmData);


	// ��Ŀ�����ݱ���ȡ�����������
	BOOL RanData(CString TableName, CString Params, int &RanData);


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

