#pragma once

// 引用数据库头文件支持数据库操作
#include "sqlite3\sqlite3.h"
using namespace SqliteSpace;


// 引用字符串库头文件和名字空间
#include <string>  
#include <xstring>  
using std::string;
using std::wstring;

// 引用链表库头文件和名字空间
#include <vector>
using std::vector;




// 定义 ModifyDataTable 的参数

// --------- 数据表操作 ---------|
                              // |
// 重命名表                      |
#define REN_TABLE  0x3000     // |
// ------------------------------|
// 添加列                        |
#define ADD_COLUMN 0x3001     // |
// ------------------------------|
// 删除列                        |
#define DEL_COLUMN 0x3002     // |
// ------------------------------|
// 修改列                        |
#define REN_COLUMN 0x3003     // |
                              // |
// ------------------------------|


// 定义 SelectData 的参数

// ---------- 排序模式 ----------|
                              // |
// 升序排列                      |
#define MOD_ASC    0x4000     // |
// ------------------------------|
// 降序排列                      |
#define MOD_DESC   0x4001     // |
                              // |
// ------------------------------|


// 定义查询对象
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


// CSqliteManager 数据库操作类
class CSqliteManager
{
public:
	/////////////////////////////////////构造与折构//////////////////////////////////////////////////////////////////////////

	// 默认构造函数
	CSqliteManager();

	// 带数据库名称和路径参数的构造函数
	CSqliteManager(CString Name, CString Path);

	// 默认折构函数
	virtual ~CSqliteManager();


	/////////////////////////////////////类成员变量//////////////////////////////////////////////////////////////////////////

	// 数据库对象变量
	sqlite3 *db;

	// 数据库记录集变量
	sqlite3_stmt * stmt;

	// 数据库错误信息变量
	char * errMsg;

	// 记录集
	char **pRes;

	// 列对象
	int nRow, nCol;

	// 标示数据库连接是否成功
	BOOL IsConnect;

	// 数据库版本号
	int version;

	// 查询结果链表
	CList <CString, CString&> pResult;

	/////////////////////////////////////类成员方法//////////////////////////////////////////////////////////////////////////
	

	////////////////////////////////////参数解析代码/////////////////////////////////////////////////////////////////////////


	// 查找参数个数
	int FindCharCount(CString csStr, char c);

	//用于计算文件夹内的文件数量
	int CountFile(CString Path);

	////////////////////////////////////事物处理代码/////////////////////////////////////////////////////////////////////////

	//开启事务  
	bool transaction(sqlite3 *p);

	//提交事务  
	bool commitTransaction(sqlite3 *p);

	//回滚事物  
	bool rollbackTransaction(sqlite3 *p);

	////////////////////////////////////数据处理代码/////////////////////////////////////////////////////////////////////////

	//bool Update(sqlite3 *p, const string &sql);

	Result *Query(sqlite3 *p, const string &sql);


	///////////////////////////////////数据库操作代码////////////////////////////////////////////////////////////////////////


	// 打开目标数据库连接
	BOOL OpenDataBase(CString Name, CString Path = _T(""));


	// 关闭目标数据库连接
	BOOL CloseDataBase();


	// 创建一个目标数据库
	BOOL CreateDataBase(CString Name, CString Path = _T(""));


	// 删除一个目标数据库
	BOOL DeleteDataBase(CString Name, CString Path = _T(""));


	// 修改目标数据库名称
	BOOL ReNameDataBase(CString OldName, CString OldPath = _T(""), CString NewName = _T(""), CString NewPath = _T(""));


	// 移动一个目标数据库
	BOOL ReMoveDataBase(CString OldName, CString OldPath = _T(""), CString NewName = _T(""), CString NewPath = _T(""));


	// 检查目标数据库存在
	BOOL CheckDataBase(CString Name, CString Path = _T(""));


	// 统计目标数据库数量
	BOOL CountDataBase(CString DataBasePath, int &Count);


	// 备份目标数据库数据
	BOOL BackupDataBase(CString DataBasePath, CString BackupPath);


	// 还原目标数据库数据
	BOOL RestoreDataBase(CString DataBasePath, CString RestorePath);


	// 远程连接目标数据库
	BOOL RemoteDataBase(CString Name, CString Path);


	// 加密目标数据库数据
	BOOL EncryptionDataBase(CString DataBaseName, CString Password);


	// 解密目标数据库数据
	BOOL DecryptionDataBase(CString DataBaseName, CString Password);


	// 上传远程目标数据库
	BOOL UploadDataBase(CString DataBasePath, CString UploadPath);


	// 下载远程目标数据库
	BOOL DownloadDataBase(CString DataBasePath, CString DownloadPath);


	///////////////////////////////////数据表操作代码////////////////////////////////////////////////////////////////////////


	// 获取数据库中所有表的名称
	BOOL GetTableName(CString &TableName);


	// 获取数据库中所有表的数据
	BOOL GetDataTable(CString &DataTable);


	// 获取数据库中数据表的信息
	BOOL GetTableData(CString TableName, CString &SQL_Data);


	// 获得数据表中所有列的名称
	BOOL GetColName(CString TableName, CString &ColName);


	// 获得数据表中所有列的类型
	BOOL GetColType(CString TableName, CString &ColType);


	// 在目标数据库中创建数据表
	BOOL CreateDataTable(CString TableName, CString Params);


	// 在目标数据库中删除数据表
	BOOL DeleteDataTable(CString TableName);


	// 在目标数据库中修改数据表
	BOOL UpdataDataTable(CString TableName, int Operation, CString Params, CString NewParams = _T(""));


	// 在目标数据库中检查数据表     ( 检查数据表是否存在 )
	BOOL CheckDataTable (CString TableName);


	// 在目标数据库中统计数据表     ( 统计数据表总共数量 )
	BOOL CountDataTable (int &Count);


	// 向目标数据库中导入数据表
	BOOL ImportDataTable(CString TableName, CString TargetName);


	// 从目标数据库中导出数据表
	BOOL ExportDataTable(CString TableName, CString TargetName);


	///////////////////////////////////数据项操作代码////////////////////////////////////////////////////////////////////////


	// 向目标数据表中添加数据项
	BOOL InsertData(CString TableName, CString Params);


	// 从目标数据表中删除数据项
	BOOL DeleteData(CString TableName, CString Params);


	// 从目标数据表中修改单数据
	BOOL UpdataData(CString TableName, CString Column, CString NewData, CString Params);


	// 从目标数据表中修改数据项
	BOOL UpdataData(CString TableName, CString ColumnParams, CString Params);


	// 从目标数据表中查询数据项
	BOOL SelectData(CString TableName, vector<CString> &pResult,  BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString Params = _T(""), CString GROUP = _T(""), CString HAVING = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC);


	// 从目标数据表中查询单数据
	BOOL SelectData(CString TableName, CString &pResult, int Col, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString Params = _T(""), CString GROUP = _T(""), CString HAVING = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC);


	// 在目标数据表中检查数据项
	BOOL CheckData (CString TableName, int Col, BOOL DISTINCT = FALSE, CString COUNT = _T(""), CString COLUMN = _T(""), CString Params = _T(""), CString GROUP = _T(""), CString HAVING = _T(""), CString Order = _T(""), CString Limit = _T(""), int SortMode = MOD_ASC);


	// 统计目标数据表中数据数项
	BOOL CountNumber(CString TableName, CString Params, int &Count);


	// 向目标数据表中导入数据项
	BOOL ImportData (CString TableName, CString TargetTableName, CString Params);


	// 从目标数据表中导出数据项
	BOOL ExportData (CString TableName, CString TargetTableName, CString Params);


	// 删除目标数据表中行据数项
	BOOL DeleteRowData(CString TableName, int Row);


	// 删除目标数据表中列据数项
	BOOL DeleteColData(CString TableName, int Col);


	// 删除目标数据表中初据数项
	BOOL DeleteMinData(CString TableName);


	// 删除目标数据表中间据数项
	BOOL DeleteMidData(CString TableName);


	// 删除目标数据表中末据数项
	BOOL DeleteMaxData(CString TableName);


	// 修改目标数据表中行据数项
	BOOL UpdataRowData(CString TableName, int Row, CString UpdataData);


	// 修改目标数据表中列据数项
	BOOL UpdataColData(CString TableName, int Col, CString UpdataData);


	// 修改目标数据表中初据数项
	BOOL UpdataMinData(CString TableName, CString UpdataData);


	// 修改目标数据表中间据数项
	BOOL UpdataMidData(CString TableName, CString UpdataData);


	// 修改目标数据表中末据数项
	BOOL UpdataMaxData(CString TableName, CString UpdataData);


	// 检索目标数据表中行据数项
	BOOL SelectRowData(CString TableName, int Row, CString &RowData);


	// 检索目标数据表中列据数项
	BOOL SelectColData(CString TableName, int Col, CString &ColData);


	// 检索目标数据表中初据数项
	BOOL SelectMinData(CString TableName, CString &MinData);


	// 修改目标数据表中间据数项
	BOOL SelectMidData(CString TableName, CString &MidData);


	// 检索目标数据表中末据数项
	BOOL SelectMaxData(CString TableName, CString &MaxData);


	// 为目标数据表中据数项排序
	BOOL SelectSortData(CString TableName, int Sort, CString &SortData);


	// 重新排列数据表据数项顺序
	BOOL ChangeSortData(CString TableName, int Sort);


	// 从目标数据表中筛选数据项
	BOOL FilterData(CString TableName, CString Params, CString &FilterData);


	// 从目标数据表中得到随机数据项
	BOOL RanData(CString TableName, CString Params, CString &RanData);


	///////////////////////////////////数据项计算代码////////////////////////////////////////////////////////////////////////


	// 从目标数据表中求数据项和
	BOOL SumData(CString TableName, CString Params, int &SumData);


	// 从目标数据表中求数据项差
	BOOL SubData(CString TableName, CString Params, int &SubData);


	// 从目标数据表中求数据项积
	BOOL ProductData(CString TableName, CString Params, int &ProductData);


	// 从目标数据表中求数据项商
	BOOL QuotientData(CString TableName, CString Params, int &QuotientData);


	// 从目标数据表中求数据项余
	BOOL ModData(CString TableName, CString Params, int &ModData);


	// 从目标数据表中求数据项幂
	BOOL PowerData(CString TableName, CString Params, int &PowerData, int &Power);


	// 从目标数据表中求数据项对数
	BOOL LogData(CString TableName, CString Params, int &LogData, int &Log);


	// 从目标数据表中求数据项阶乘
	BOOL FactData(CString TableName, CString Params, int &FactData);


	// 从目标数据表中求数据项乘方
	BOOL ExpData(CString TableName, CString Params, int &ExpData, int &Exp);


	// 从目标数据表中求数据项众数
	BOOL PluData(CString TableName, CString Params, int &PluData);


	// 从目标数据表中求数据项极限
	BOOL LimData(CString TableName, CString Params, int &LimData);


	// 从目标数据表中求数据项正弦
	BOOL SinData(CString TableName, CString Params, int &SinData);


	// 从目标数据表中求数据项余弦
	BOOL CosData(CString TableName, CString Params, int &CosData);


	// 从目标数据表中求数据项正切
	BOOL TanData(CString TableName, CString Params, int &TanData);


	// 从目标数据表中求数据项平均数
	BOOL AvgData(CString TableName, CString Params, int &AvgData);


	// 从目标数据表中求数据项中位数
	BOOL MidData(CString TableName, CString Params, int &MidData);


	// 从目标数据表中求数据项百分比
	BOOL PerData(CString TableName, CString Params, int &PerData);


	// 从目标数据表中求数据项平方根
	BOOL SqrData(CString TableName, CString Params, int &SqrData);


	// 从目标数据表中求数据项立方根
	BOOL CubData(CString TableName, CString Params, int &CubData);


	// 从目标数据表中求数据项绝对值
	BOOL AbsData(CString TableName, CString Params, int &AbsData);


	// 从目标数据表中求数据项最大值
	BOOL MaxData(CString TableName, CString Params, int &AbsData);


	// 从目标数据表中求数据项最小值
	BOOL MinData(CString TableName, CString Params, int &AbsData);


	// 从目标数据表中求数据项最大公约数
	BOOL GcdData(CString TableName, CString Params, int &GcdData);


	// 从目标数据表中求数据项最小公倍数
	BOOL LcmData(CString TableName, CString Params, int &LcmData);


	// 从目标数据表中取数据项随机数
	BOOL RanData(CString TableName, CString Params, int &RanData);


	///////////////////////////////////数据编码转换代码////////////////////////////////////////////////////////////////////////


	//ACSII转Unicode
	static wstring AcsiiToUnicode( const string  & acsii_string);

	//ACSII转UTF8  
	static string  AcsiiToUtf8(    const string  & acsii_string);

	//Unicode转ACSII  
	static string  UnicodeToAcsii( const wstring & unicode_string);

	//Unicode转UTF8  
	static string  UnicodeToUtf8(  const wstring & unicode_string);

	//UTF8转ACSII 
	static string  Utf8ToAcsii(    const string  & utf8_string);

	//UTF8转Unicode  
	static wstring Utf8ToUnicode(  const string  & utf8_string);
};

