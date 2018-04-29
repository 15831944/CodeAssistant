#include "stdafx.h"
#include "SqliteManager.h"


///////////////////////////////////构造折构代码///////////////////////////////////


// 默认构造函数
CSqliteManager::CSqliteManager()
{
	// 默认连接失败
	IsConnect = FALSE;
}


// 带数据库名称和路径参数的构造函数
CSqliteManager::CSqliteManager(CString Name, CString Path)
{
	// 默认连接失败
	IsConnect = FALSE;


	// 打开目标数据库连接
	if (!OpenDataBase(Name, Path))
	{
		// 弹出错误信息
		AfxMessageBox(_T("目标数据库对象创建失败!"));

		// 返回数据库对象创建失败的信息
		return;

	}
}


// 默认折构函数
CSqliteManager::~CSqliteManager()
{
	// 关闭数据库连接
	CloseDataBase();
}


///////////////////////////////////参数解析代码///////////////////////////////////


// 解析传入参数的数量
int CSqliteManager::FindCharCount(CString csStr, char c)
{
	// 初始化参数个数为0
	int iCount = 0;

	// 循环查找参数
	for (int i = 0; i<csStr.GetLength(); i++)
	{
		// 循环遍历
		i = csStr.Find(c, i + 1);

		// 参数个数自加1
		iCount++;
	}

	// 返回参数个数
	return iCount;
}


//用于计算文件夹内的文件数量
int CSqliteManager::CountFile(CString Path)
{
	int count = 0;
	CFileFind finder;
	BOOL working = finder.FindFile(Path + "./*.*");

	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
			continue;
		if (finder.IsDirectory())
			count += CountFile(finder.GetFilePath());
		else
		{
			count++;
		}
	}
	return count;
}

////////////////////////////////////事物处理代码//////////////////////////////////

bool CSqliteManager::transaction(sqlite3 *p)
{
	bool result = true;
	char *zErrorMsg = NULL;
	int ret = sqlite3_exec(p, "begin transaction", 0, 0, &zErrorMsg); // 开始一个事务  
	if (ret != SQLITE_OK)
	{
		//LOGI("start transaction failed:%s", zErrorMsg);
		result = false;
	}
	sqlite3_free(zErrorMsg);
	return result;
}


bool CSqliteManager::commitTransaction(sqlite3 *p)
{
	bool result = true;
	char *zErrorMsg = NULL;
	int ret = sqlite3_exec(p, "commit transaction", 0, 0, &zErrorMsg); // 提交事务  
	if (ret != SQLITE_OK)
	{
		//LOGI("commit transaction failed:%s", zErrorMsg);
		result = false;
	}
	sqlite3_free(zErrorMsg);
	return result;
}


bool CSqliteManager::rollbackTransaction(sqlite3 *p)
{
	bool result = true;
	char *zErrorMsg = NULL;
	int     ret = sqlite3_exec(p, "rollback transaction", 0, 0, &zErrorMsg);
	if (ret != SQLITE_OK)
	{
		//LOGI("rollback transaction failed:%s", zErrorMsg);
		result = false;
	}
	sqlite3_free(zErrorMsg);
	return result;
}

////////////////////////////////////数据处理代码///////////////////////////////////

Result *CSqliteManager::Query(sqlite3 *p, const string &sql)
{
	Result *pRe = NULL;
	char *errmsg = NULL;
	char **dbResult;
	int nRow = 0;
	int nColumn = 0;

	//开始查询数据库
	int result = sqlite3_get_table(p, sql.c_str(), &dbResult, &nRow, &nColumn, &errmsg);
	if (result == SQLITE_OK)
	{
		pRe = new Result(nRow, nColumn, dbResult);
	}
	else
	{
		//LOGI("[sqlite] SqliteBaseV Query error:dbName=%s,msg=%s sql:%s", dbName.c_str(), errmsg, sql.c_str());
	}

	// 不论数据库查询是否成功，都释放 char** 查询结果
	//sqlite3_free_table(dbResult);

	//释放内存
	sqlite3_free(errmsg);
	return pRe;
}


///////////////////////////////////数据库操作代码//////////////////////////////////


// 打开目标数据库连接
BOOL CSqliteManager::OpenDataBase(CString Name, CString Path)
{
	// 数据库对象默认为空
	db = NULL;

	// 若数据库已连接成功
	if (IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("该数据库已连接，不需要再次连接!"));

		// 返回连接失败信息
		return FALSE;
	}
	else
	{
		// 初始化目标数据库变量
		CString DatabasePath = Path;

		// 若目标路径不为空
		if (!Path.IsEmpty() || atoi((CStringA)Path) != NULL)
		{
			// 如果目标路径不包含 \ 就加上												   
			if (DatabasePath.Right(1) != _T("\\"))
				DatabasePath += _T("\\");

			// 赋予目标数据库的值为目标路径加上目标文件名
			DatabasePath += Name;
		}
		else
		{
			// 赋予目标数据库的值为当前路径加上目标文件名
			DatabasePath = _T("./") + Name;
		}
		

		// 查找目标数据库是否存在
		CFileFind Finder;
		BOOL Status = Finder.FindFile(DatabasePath);

		// 若不存在目标数据库则尝试创建并连接目标数据库
		if (!Status)
		{
			// 若目标路径不为空
			if (!Path.IsEmpty())
			{
				// 检查目标路径是否存在
				DWORD DataPath = GetFileAttributes(Path);

				// 若目标路径不存在
				if (DataPath == 0xFFFFFFFF)
				{
					// 创建目标路径
					CreateDirectory(Path, NULL);
				}
			}

			// 尝试连接目标数据库
			try
			{
				// 创建并打开目标数据库
				if (sqlite3_open((CStringA)DatabasePath, &db) != SQLITE_OK) // 若无法连接数据库
				{
					// 弹出错误信息
					::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

					// 关闭数据库连接
					CloseDataBase();

					// 返回数据库连接失败的信息
					return FALSE;
				}

				// 若数据库成功连接
				else
				{
					// 标示数据库已连接
					IsConnect = TRUE;

					// 返回数据库连接成功的信息
					return TRUE;
				}
			}
			// 捕捉错误
			catch (...)
			{
				// 弹出错误信息
				::MessageBox(NULL, _T("无法连接到数据库!"), NULL, NULL);

				// 返回数据库连接失败的信息
				return FALSE;
			}
		}
		// 若存在目标数据库则尝试连接目标数据库
		else
		{
			// 尝试连接数据库
			try
			{
				// 连接目标数据库
				if (sqlite3_open((CStringA)DatabasePath, &db) != SQLITE_OK) // 若无法连接数据库
				{
					// 弹出错误信息
					::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

					// 关闭数据库连接
					CloseDataBase();

					// 返回数据库连接失败的信息
					return FALSE;
				}

				// 若数据库成功连接
				else
				{
					// 标示数据库已连接
					IsConnect = TRUE;

					// 返回数据库连接成功的信息
					return TRUE;
				}
			}
			// 捕捉错误
			catch (...)
			{
				// 弹出错误信息
				::MessageBox(NULL, _T("无法连接到数据库!"), NULL, NULL);

				// 返回数据库连接失败的信息
				return FALSE;
			}
		}
	}

	// 默认返回数据库连接失败的信息
	return FALSE;
}


// 关闭目标数据库连接
BOOL CSqliteManager::CloseDataBase()
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("该数据库已关闭，不需要再次关闭!"));

		// 返回关闭失败信息
		return FALSE;
	}
	else
	{
		// 尝试关闭数据库连接
		try
		{
			// 若关闭数据库失败
			if (sqlite3_close(db) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回关闭失败信息
				return FALSE;
			}
			else
			{
				// 标示数据库已断开连接
				IsConnect = FALSE;

				// 数据库对象重置为空
				db = NULL;

				// 返回关闭成功信息
				return TRUE;
			}
		}
		catch (...)
		{
			// 提示错误信息
			AfxMessageBox(_T("数据库关闭失败!"));

			// 返回关闭失败信息
			return FALSE;
		}
	}

	// 默认返回关闭失败信息
	return FALSE;
}


// 创建一个目标数据库
BOOL CSqliteManager::CreateDataBase(CString Name, CString Path)
{
	// 初始化目标数据库变量
	CString DatabasePath = Path;

	// 若目标路径不为空
	if (!Path.IsEmpty() || atoi((CStringA)Path) != NULL)
	{
		// 如果目标路径不包含 \ 就加上												   
		if (DatabasePath.Right(1) != _T("\\"))
			DatabasePath += _T("\\");

		// 赋予目标数据库的值为目标路径加上目标文件名
		DatabasePath += Name;
	}
	else
	{
		// 赋予目标数据库的值为当前路径加上目标文件名
		DatabasePath = _T("./") + Name;
	}

	// 查找目标数据库是否存在
	CFileFind Finder;
	BOOL Status = Finder.FindFile(DatabasePath);

	// 若不存在目标数据库则尝试创建并连接目标数据库
	if (!Status)
	{
		// 若目标路径不为空
		if (!Path.IsEmpty())
		{
			// 检查目标路径是否存在
			DWORD DataPath = GetFileAttributes(Path);

			// 若目标路径不存在
			if (DataPath == 0xFFFFFFFF)
			{
				// 创建目标路径
				CreateDirectory(Path, NULL);
			}
		}

		// 尝试创建目标数据库
		try
		{
			// 数据库对象
			sqlite3 *Create_db;

			// 创建并打开目标数据库
			if (sqlite3_open((CStringA)DatabasePath, &Create_db) != SQLITE_OK) // 若无法连接数据库
			{
				// 弹出错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(Create_db), NULL, NULL);

				// 若关闭数据库失败
				if (sqlite3_close(Create_db) != SQLITE_OK)
				{
					// 提示错误信息
					::MessageBox(NULL, (CString)sqlite3_errmsg(Create_db), NULL, NULL);

					// 返回关闭失败信息
					return FALSE;
				}

				// 返回数据库创建失败的信息
				return FALSE;
			}

			// 若数据库成功创建
			else
			{
				// 若关闭数据库失败
				if (sqlite3_close(Create_db) != SQLITE_OK)
				{
					// 提示错误信息
					::MessageBox(NULL, (CString)sqlite3_errmsg(Create_db), NULL, NULL);

					// 返回关闭失败信息
					return FALSE;
				}

				// 返回数据库创建成功的信息
				return TRUE;
			}
		}
		// 捕捉错误
		catch (...)
		{
			// 弹出错误信息
			::MessageBox(NULL, _T("无法创建目标数据库!"), NULL, NULL);

			// 返回数据库创建失败的信息
			return FALSE;
		}
	}
	else
	{
		// 提示错误信息
		AfxMessageBox(_T("已存在目标数据库,本次创建操作失败!"));

		// 返回数据库创建失败的信息
		return FALSE;
	}
	
	// 默认返回数据库创建失败的信息
	return FALSE;
}


// 删除一个目标数据库
BOOL CSqliteManager::DeleteDataBase(CString Name, CString Path)
{
	// 初始化目标数据库变量
	CString DatabasePath = Path;

	// 若目标路径不为空
	if (!Path.IsEmpty() || atoi((CStringA)Path) != NULL)
	{
		// 如果目标路径不包含 \ 就加上												   
		if (DatabasePath.Right(1) != _T("\\"))
			DatabasePath += _T("\\");

		// 赋予目标数据库的值为目标路径加上目标文件名
		DatabasePath += Name;
	}
	else
	{
		// 赋予目标数据库的值为当前路径加上目标文件名
		DatabasePath = _T("./") + Name;
	}

	// 查找目标数据库是否存在
	CFileFind Finder;
	BOOL Status = Finder.FindFile(DatabasePath);

	// 若不存在目标数据库则提示错误信息
	if (!Status)
	{
		// 弹出错误信息
		AfxMessageBox(_T("不存在目标数据库,本次删除操作失败!"));

		// 返回删除失败
		return FALSE;
	}
	else
	{
		// 尝试删除数据库
		try
		{
			if (DeleteFile(DatabasePath))
			{
				// 返回删除成功
				return TRUE;
			}
			else
			{
				// 弹出错误信息
				AfxMessageBox(_T("尝试删除目标数据库失败!"));

				// 返回删除失败
				return FALSE;
			}
		}
		catch (...)
		{
			// 弹出错误信息
			AfxMessageBox(_T("尝试删除目标数据库失败!"));

			// 返回删除失败
			return FALSE;
		}
	}

	// 默认返回删除失败
	return FALSE;
}


// 修改目标数据库名称
BOOL CSqliteManager::ReNameDataBase(CString OldName, CString OldPath, CString NewName, CString NewPath)
{
	// 初始化目标数据库变量
	CString OldDatabasePath = OldPath, NewDatabasePath = NewPath;

	// 若旧目标路径不为空
	if (!OldPath.IsEmpty() || atoi((CStringA)OldPath) != NULL)
	{
		// 如果目标路径不包含 \ 就加上												   
		if (OldDatabasePath.Right(1) != _T("\\"))
			OldDatabasePath += _T("\\");

		// 赋予目标数据库的值为目标路径加上目标文件名
		OldDatabasePath += OldName;
	}
	else
	{
		// 赋予目标数据库的值为当前路径加上目标文件名
		OldDatabasePath = _T("./") + OldName;
	}

	// 若新目标路径不为空
	if (!NewPath.IsEmpty() || atoi((CStringA)NewPath) != NULL)
	{
		// 如果目标路径不包含 \ 就加上												   
		if (NewDatabasePath.Right(1) != _T("\\"))
			NewDatabasePath += _T("\\");

		// 赋予目标数据库的值为目标路径加上目标文件名
		NewDatabasePath += NewName;
	}
	else
	{
		// 赋予目标数据库的值为当前路径加上目标文件名
		NewDatabasePath = _T("./") + NewName;
	}


	// 查找旧目标数据库是否存在
	CFileFind Finder;
	BOOL Status = Finder.FindFile(OldDatabasePath);

	// 若不存在旧目标数据库则提示错误信息
	if (!Status)
	{
		// 弹出错误信息
		AfxMessageBox(_T("不存在旧目标数据库,本次修改操作失败!"));

		// 返回修改失败
		return FALSE;
	}
	else
	{
		// 尝试修改数据库
		try
		{
			// 若新目标路径不为空
			if (!NewPath.IsEmpty())
			{
				// 检查新目标路径是否存在
				DWORD DataPath = GetFileAttributes(NewPath);

				// 若新目标路径不存在
				if (DataPath == 0xFFFFFFFF)
				{
					// 创建新目标路径
					CreateDirectory(NewPath, NULL);
				}
			}
			else
			{
				// 弹出错误信息
				AfxMessageBox(_T("新目标数据库不能为空,移动目标数据库失败!"));

				// 返回移动失败
				return FALSE;
			}

			if (MoveFileEx(OldDatabasePath, NewDatabasePath, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))
			{
				// 返回修改成功
				return TRUE;
			}
			else
			{
				// 弹出错误信息
				AfxMessageBox(_T("修改目标数据库失败!"));

				// 返回修改失败
				return FALSE;
			}
		}
		catch (...)
		{
			// 弹出错误信息
			AfxMessageBox(_T("修改目标数据库失败!"));

			// 返回修改失败
			return FALSE;
		}
	}

	// 默认返回修改失败
	return FALSE;
}


// 移动一个目标数据库
BOOL CSqliteManager::ReMoveDataBase(CString OldName, CString OldPath, CString NewName, CString NewPath)
{
	// 初始化目标数据库变量
	CString OldDatabasePath = OldPath, NewDatabasePath = NewPath;

	// 若旧目标路径不为空
	if (!OldPath.IsEmpty() || atoi((CStringA)OldPath) != NULL)
	{
		// 如果目标路径不包含 \ 就加上												   
		if (OldDatabasePath.Right(1) != _T("\\"))
			OldDatabasePath += _T("\\");

		// 赋予目标数据库的值为目标路径加上目标文件名
		OldDatabasePath += OldName;
	}
	else
	{
		// 赋予目标数据库的值为当前路径加上目标文件名
		OldDatabasePath = _T("./") + OldName;
	}

	// 若新目标路径不为空
	if (!NewPath.IsEmpty() || atoi((CStringA)NewPath) != NULL)
	{
		// 如果目标路径不包含 \ 就加上												   
		if (NewDatabasePath.Right(1) != _T("\\"))
			NewDatabasePath += _T("\\");

		// 赋予目标数据库的值为目标路径加上目标文件名
		NewDatabasePath += NewName;
	}
	else
	{
		// 赋予目标数据库的值为当前路径加上目标文件名
		NewDatabasePath = _T("./") + NewName;
	}


	// 查找旧目标数据库是否存在
	CFileFind Finder;
	BOOL Status = Finder.FindFile(OldDatabasePath);

	// 若不存在旧目标数据库则提示错误信息
	if (!Status)
	{
		// 弹出错误信息
		AfxMessageBox(_T("不存在旧目标数据库,本次移动操作失败!"));

		// 返回移动失败
		return FALSE;
	}
	else
	{
		// 尝试修改数据库
		try
		{
			// 若新目标路径不为空
			if (!NewPath.IsEmpty())
			{
				// 检查新目标路径是否存在
				DWORD DataPath = GetFileAttributes(NewPath);

				// 若新目标路径不存在
				if (DataPath == 0xFFFFFFFF)
				{
					// 创建新目标路径
					CreateDirectory(NewPath, NULL);
				}
			}
			else
			{
				// 弹出错误信息
				AfxMessageBox(_T("新目标数据库不能为空,移动目标数据库失败!"));

				// 返回移动失败
				return FALSE;
			}
			if (MoveFileEx(OldDatabasePath, NewDatabasePath, MOVEFILE_COPY_ALLOWED| MOVEFILE_REPLACE_EXISTING))
			{
				// 返回移动成功
				return TRUE;
			}
			else
			{
				// 弹出错误信息
				AfxMessageBox(_T("移动目标数据库失败!"));

				// 返回移动失败
				return FALSE;
			}
		}
		catch (...)
		{
			// 弹出错误信息
			AfxMessageBox(_T("移动目标数据库失败!"));

			// 返回移动失败
			return FALSE;
		}
	}

	// 默认返回移动失败
	return FALSE;
}


// 检查目标数据库存在
BOOL CSqliteManager::CheckDataBase(CString Name, CString Path)
{
	// 初始化目标数据库变量
	CString DatabasePath = Path;

	// 若目标路径不为空
	if (!Path.IsEmpty() || atoi((CStringA)Path) != NULL)
	{
		// 如果目标路径不包含 \ 就加上												   
		if (DatabasePath.Right(1) != _T("\\"))
			DatabasePath += _T("\\");

		// 赋予目标数据库的值为目标路径加上目标文件名
		DatabasePath += Name;
	}
	else
	{
		// 赋予目标数据库的值为当前路径加上目标文件名
		DatabasePath = _T("./") + Name;
	}


	// 尝试打开数据库
	try
	{
		sqlite3 *Check_db;
		if (sqlite3_open_v2((CStringA)DatabasePath, &Check_db, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK)
		{
			// 返回检查失败
			return FALSE;
		}
		else
		{
			// 关闭数据库连接
			if (sqlite3_close(Check_db) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(Check_db), NULL, NULL);

				// 返回关闭失败信息
				return FALSE;
			}

			// 返回检查成功
			return TRUE;
		}
	}
	catch (...)
	{
		// 弹出错误信息
		AfxMessageBox(_T("无法打开数据库!"));

		// 返回检查失败
		return FALSE;
	}
	
	// 默认返回检查失败
	return FALSE;
}


// 统计目标数据库数量
BOOL CSqliteManager::CountDataBase(CString DataBasePath, int &Count)
{
	Count = CountFile(DataBasePath);
	return Count;
}


// 备份目标数据库数据
BOOL CSqliteManager::BackupDataBase(CString DataBasePath, CString BackupPath)
{
	return CopyFile(DataBasePath, BackupPath, FALSE);
}


// 还原目标数据库数据
BOOL CSqliteManager::RestoreDataBase(CString DataBasePath, CString RestorePath)
{
	return CopyFile(RestorePath, DataBasePath, FALSE);
}


// 远程连接目标数据库
BOOL CSqliteManager::RemoteDataBase(CString Name, CString Path)
{
	return true;
}


// 加密目标数据库数据
BOOL CSqliteManager::EncryptionDataBase(CString DataBaseName, CString Password)
{
	return true;
}


// 解密目标数据库数据
BOOL CSqliteManager::DecryptionDataBase(CString DataBaseName, CString Password)
{
	return true;
}


// 上传远程目标数据库
BOOL CSqliteManager::UploadDataBase(CString DataBasePath, CString UploadPath)
{
	return true;
}


// 下载远程目标数据库
BOOL CSqliteManager::DownloadDataBase(CString DataBasePath, CString DownloadPath)
{
	return true;
}


///////////////////////////////////数据表操作代码///////////////////////////////////


// 获取数据库中所有表的名称
BOOL CSqliteManager::GetTableName(CString &TableName)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回创建失败信息
		return FALSE;
	}
	else
	{
		// 用于判断操作是否成功
		BOOL IsSuccess;

		// 尝试执行获取数据表的操作
		try
		{
			// 初始化SQL语句
			CStringA GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name";

			// 若获取操作失败
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回获取失败
				IsSuccess = FALSE;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!stmt)
				{
					// 返回空的表名
					TableName = _T("");

					// 返回获取失败
					IsSuccess = FALSE;
				}

				// 若数据不为空
				else
				{
					// 循环获得表名
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// 得到该表表名并自加
						TableName += (CStringA)sqlite3_column_text(stmt, 0);

						// 加上分隔符以区分表
						TableName += _T(";");
					}

					// 返回获取成功
					IsSuccess = TRUE;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			AfxMessageBox(_T("无法从数据库中获得表名"));

			// 返回空的表名
			TableName = _T("");

			// 返回获取失败
			return FALSE;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return FALSE;
}


// 获取数据库中所有表的数据
BOOL CSqliteManager::GetDataTable(CString &DataTable)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回创建失败信息
		return FALSE;
	}
	else
	{
		// 用于判断操作是否成功
		BOOL IsSuccess;

		// 尝试执行获取数据表的操作
		try
		{
			// 初始化SQL语句
			CStringA GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name";

			// 若获取操作失败
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回获取失败
				IsSuccess = FALSE;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!stmt)
				{
					// 返回空的数据
					DataTable = _T("");

					// 返回获取失败
					IsSuccess = FALSE;
				}

				// 若数据不为空
				else
				{
					// 循环获得表名
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// 得到该表数据并自加
						DataTable += (CStringA)sqlite3_column_text(stmt, 1);

						// 加上分隔符以区分表
						DataTable += _T(";");
					}

					// 返回获取成功
					IsSuccess = TRUE;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			AfxMessageBox(_T("无法从数据库中获得表名"));

			// 返回空的数据
			DataTable = _T("");

			// 返回获取失败
			return FALSE;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return FALSE;
}


// 获取数据库中数据表的信息
BOOL CSqliteManager::GetTableData(CString TableName, CString &SQL_Data)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回创建失败信息
		return FALSE;
	}
	else
	{
		// 用于判断操作是否成功
		BOOL IsSuccess;

		// 尝试执行获取数据表的操作
		try
		{
			// 初始化SQL语句
			CStringA GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name;";

			// 若获取操作失败
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回获取失败
				IsSuccess = FALSE;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!stmt)
				{
					// 返回空的数据
					SQL_Data = _T("");

					// 返回获取失败
					IsSuccess = FALSE;
				}

				// 若数据不为空
				else
				{
					// 通过表名得到数据
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						CStringA Name = (char *)sqlite3_column_text(stmt, 0);

						if (Name == (CStringA)TableName)
						{
							// 得到该表语句并自加
							SQL_Data = (char *)sqlite3_column_text(stmt, 1);

							// 返回获取成功
							IsSuccess = TRUE;
						}
						else
						{
							// 返回获取失败
							IsSuccess = FALSE;
						}
					}
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			AfxMessageBox(_T("无法从数据表中获得数据"));

			// 返回空的语句
			SQL_Data = _T("");

			// 返回获取失败
			return FALSE;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return FALSE;
}


// 获得数据表中所有列的名称
BOOL CSqliteManager::GetColName(CString TableName, CString &ColName)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回创建失败信息
		return FALSE;
	}
	else
	{
		// 用于判断操作是否成功
		BOOL IsSuccess = FALSE;

		// 尝试执行获取数据表中列名的操作
		try
		{
			// 初始化SQL语句
			CStringA GetSQL = "SELECT * FROM " + (CStringA)TableName + " limit 0,1";

			// 若获取操作失败
			if (sqlite3_get_table(db, GetSQL, &pRes, &nRow, &nCol, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回获取失败
				IsSuccess = FALSE;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!nRow)
				{
					// 返回空的列名
					ColName = _T("");

					// 返回获取失败
					IsSuccess = FALSE;
				}

				// 若数据不为空
				else
				{
					// 循环获得列名
					for (int i = 0; i < nRow; i++)
					{
						for (int j = 0; j < nCol; j++)
						{
							char *pv = *(pRes + nCol*i + j);

							// 列名自加
							ColName += pv;

							// 加上分隔符以区分列
							ColName += _T(";");
						}
						break;
					}

					// 释放资源
					if (errMsg != NULL)
					{
						sqlite3_free(errMsg);
					}

					// 返回获取成功
					IsSuccess = TRUE;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_free_table(pRes);
			pRes = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			AfxMessageBox(_T("无法从数据库中获得表名"));

			// 返回空的列名
			ColName = _T("");

			// 返回获取失败
			return FALSE;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return FALSE;
}


// 获得数据表中所有列的类型
BOOL CSqliteManager::GetColType(CString TableName, CString &ColType)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回创建失败信息
		return FALSE;
	}
	else
	{
		// 用于判断操作是否成功
		BOOL IsSuccess = FALSE;

		// 尝试执行获取数据表的操作
		try
		{
			// 初始化SQL语句
			CStringA GetSQL = "SELECT * FROM " + (CStringA)TableName + " limit 0,1";

			// 若获取操作失败
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回获取失败
				IsSuccess = FALSE;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!stmt)
				{
					// 返回空的类型
					ColType = _T("");

					// 返回获取失败
					IsSuccess = FALSE;
				}

				// 若数据不为空
				else
				{
					// 循环获得类型
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// 获得列数
						int nCount = sqlite3_column_count(stmt);

						for (int i = 0; i < nCount; i++)
						{
							int nValue = sqlite3_column_int(stmt, 0);
							int nType = sqlite3_column_type(stmt, i);

							// 判断类型
							switch (nType)
							{
							case 1:
								// INTEGER 类型
								ColType += _T("SQLITE_INTEGER");

								// 加上分隔符以区分类型
								ColType += _T(";");
								break;
							case 2:
								// FLOAT 类型
								ColType += _T("SQLITE_FLOAT");

								// 加上分隔符以区分类型
								ColType += _T(";");
								break;
							case 3:
								// TEXT 类型
								ColType += _T("SQLITE_TEXT");

								// 加上分隔符以区分类型
								ColType += _T(";");
								break;
							case 4:
								// BLOB 类型
								ColType += _T("SQLITE_BLOB");

								// 加上分隔符以区分类型
								ColType += _T(";");
								break;
							case 5:
								// NULL 类型
								ColType += _T("SQLITE_NULL");

								// 加上分隔符以区分类型
								ColType += _T(";");
								break;
							}
						}
						break;
					}

					// 返回获取成功
					IsSuccess = TRUE;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			AfxMessageBox(_T("无法从数据表中获得类型"));

			// 返回空的类型名
			TableName = _T("");

			// 返回获取失败
			return FALSE;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return FALSE;
}


// 在目标数据库中创建数据表
BOOL CSqliteManager::CreateDataTable(CString TableName, CString Params)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回创建失败信息
		return FALSE;
	}
	else
	{
		// 尝试执行创建数据表的操作
		try
		{
			// 初始化SQL语句
			CStringA CreateSQL = "CREATE TABLE IF Not Exists " + (CStringA)TableName + " ( " + (CStringA)Params + " );";

			//数据库启动一个事物
			if (!transaction(db))
			{
				AfxMessageBox(_T("无法启动事务处理!"));
				return false;
			}

			// 若创建操作失败
			if (sqlite3_exec(db, CreateSQL, NULL, NULL, NULL) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回创建失败
				return FALSE;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				AfxMessageBox(_T("创建数据表失败: ") + (CString)sqlite3_errmsg(db));

				// 返回创建失败
				return FALSE;
			}

			// 返回创建成功
			else
				return TRUE;
		}
		catch (...)
		{
			// 提示错误信息
			AfxMessageBox(_T("创建数据表失败!"));

			// 返回创建失败
			return FALSE;
		}
	}

	// 默认返回创建失败
	return FALSE;
}


// 在目标数据库中删除数据表
BOOL CSqliteManager::DeleteDataTable(CString TableName)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回删除失败信息
		return FALSE;
	}
	else
	{
		// 尝试执行删除数据表的操作
		try
		{
			// 初始化SQL语句
			CStringA DeleteSQL = "Drop TABLE " + (CStringA)TableName + ";";

			//数据库启动一个事物
			if (!transaction(db))
			{
				AfxMessageBox(_T("无法启动事务处理!"));
				return false;
			}

			// 若删除操作失败
			if (sqlite3_exec(db, DeleteSQL, NULL, NULL, NULL) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回删除失败信息
				return FALSE;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				AfxMessageBox(_T("删除数据表失败: ") + (CString)sqlite3_errmsg(db));

				// 返回删除失败信息
				return FALSE;
			}

			// 返回删除成功信息
			else
				return TRUE;
		}
		catch (...)
		{
			// 提示错误信息信息
			AfxMessageBox(_T("删除数据表失败!"));

			// 返回删除失败信息
			return FALSE;
		}
	}

	// 默认返回删除失败信息
	return FALSE;
}


// 在目标数据库中修改数据表
BOOL CSqliteManager::UpdataDataTable(CString TableName, int Operation, CString Params, CString NewParams)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回修改失败信息
		return FALSE;
	}
	else
	{
		// 根据参数尝试不同SQL操作
		switch (Operation)
		{
		// 重命名数据表
		case REN_TABLE:
		{
			try
			{
				// 初始化SQL语句
				CStringA ReNameTableSQL = "ALTER TABLE " + (CStringA)TableName + " RENAME TO " + (CStringA)Params + ";";

				//数据库启动一个事物
				if (!transaction(db))
				{
					AfxMessageBox(_T("无法启动事务处理!"));
					return false;
				}

				// 若添加操作失败
				if (sqlite3_exec(db, ReNameTableSQL, NULL, NULL, NULL) != SQLITE_OK)
				{
					// 提示错误信息
					::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

					// 返回重命名失败
					return FALSE;
				}

				//提交事务
				if (commitTransaction(db) == false)
				{
					// 回滚事务
					rollbackTransaction(db);

					// 提示错误信息
					AfxMessageBox(_T("重命名数据表失败: ") + (CString)sqlite3_errmsg(db));

					// 返回重命名失败
					return FALSE;
				}

				// 返回重命名成功
				else
					return TRUE;
			}
			catch (...)
			{
				// 提示错误信息
				AfxMessageBox(_T("重命名数据表失败!"));

				// 返回重命名失败
				return FALSE;
			}

		}break;

		// 添加列
		case ADD_COLUMN:
		{
			try
			{
				// 初始化SQL语句
				CStringA Add_Column_SQL = "ALTER TABLE " + (CStringA)TableName + " ADD COLUMN " + (CStringA)Params + ";";

				//数据库启动一个事物
				if (!transaction(db))
				{
					AfxMessageBox(_T("无法启动事务处理!"));
					return false;
				}

				// 若添加操作失败
				if (sqlite3_exec(db, Add_Column_SQL, NULL, NULL, NULL) != SQLITE_OK)
				{
					// 提示错误信息
					::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

					// 返回添加失败
					return FALSE;
				}

				//提交事务
				if (commitTransaction(db) == false)
				{
					// 回滚事务
					rollbackTransaction(db);

					// 提示错误信息
					AfxMessageBox(_T("向数据表中添加列失败: ") + (CString)sqlite3_errmsg(db));

					// 返回添加失败
					return FALSE;
				}

				// 返回添加成功
				else
					return TRUE;
			}
			catch (...)
			{
				// 提示错误信息
				AfxMessageBox(_T("向数据表中添加列失败!"));

				// 返回添加失败
				return FALSE;
			}

		}break;

		// 删除列
		case DEL_COLUMN:
		{
			try
			{
				// 得到所有列名
				CString ColumnName;
				GetColName(TableName, ColumnName);

				// 若找不到目标列
				if (!ColumnName.Replace(Params + _T(";"), _T("")))
				{
					// 提示错误信息
					AfxMessageBox(_T("找不到目标列，删除失败!"));

					// 返回删除失败
					return FALSE;

				}
				else
				{
					// 若无法分析目标列的数据
					if (!ColumnName.Replace(_T(";"), _T(",")))
					{
						// 提示错误信息
						AfxMessageBox(_T("无法分析目标列，删除失败!"));

						// 返回删除失败
						return FALSE;
					}
					else
					{
						// 判断末尾是否为 , 
						if (ColumnName.Right(1) == _T(","))
						{
							// 删掉末尾的 ,
							ColumnName = ColumnName.Left(ColumnName.GetLength() - 1);
						}

						// 初始化SQL语句
						CStringA Cre_Column_SQL = "Create Table Temp As Select "+ (CStringA)ColumnName +" From " + (CStringA)TableName + ";";

						//数据库启动一个事物
						if (!transaction(db))
						{
							AfxMessageBox(_T("无法启动事务处理!"));
							return false;
						}

						// 若删除操作失败
						if (sqlite3_exec(db, Cre_Column_SQL, NULL, NULL, NULL) != SQLITE_OK)
						{
							// 提示错误信息
							::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

							// 返回删除失败
							return FALSE;
						}

						//提交事务
						if (commitTransaction(db) == false)
						{
							// 回滚事务
							rollbackTransaction(db);

							// 提示错误信息
							AfxMessageBox(_T("向数据表中删除列失败: ") + (CString)sqlite3_errmsg(db));

							// 返回删除失败
							return FALSE;
						}
						else
						{
							// 删除原数据表
							if (!DeleteDataTable(TableName))
							{
								// 提示错误信息
								::MessageBox(NULL, _T("无法删除原数据表!"), NULL, NULL);

								// 返回删除失败
								return FALSE;
							}
							else
							{
								// 若重命名 Temp 表为目标数据表失败
								if (!UpdataDataTable(_T("Temp"), REN_TABLE, TableName))
								{
									// 提示错误信息
									::MessageBox(NULL, _T("无法删除原数据表!"), NULL, NULL);

									// 返回删除失败
									return FALSE;
								}
								else
								{
									// 返回删除目标列成功
									return TRUE;
								}
							}
						}
					}
				}
			}
			catch (...)
			{
				// 提示错误信息
				AfxMessageBox(_T("向数据表中删除列失败!"));

				// 返回删除失败
				return FALSE;
			}

		}break;

		// 重命名列
		case REN_COLUMN:
		{
			try
			{
				// 得到目标数据表的SQL数据
				CString Sql_Data;
				GetTableData(TableName, Sql_Data);

				// 若找不到目标表
				if (!Sql_Data.Replace(TableName, _T("Temp")))
				{
					// 提示错误信息
					AfxMessageBox(_T("找不到目标表，修改失败!"));

					// 返回修改失败
					return FALSE;
				}
				else
				{
					// 若找不到目标列
					if (!Sql_Data.Replace(Params, NewParams))
					{
						// 提示错误信息
						AfxMessageBox(_T("找不到目标列，修改失败!"));

						// 返回修改失败
						return FALSE;
					}

					//数据库启动一个事物
					if (!transaction(db))
					{
						AfxMessageBox(_T("无法启动事务处理!"));
						return false;
					}

					// 若创建临时表 Temp 失败
					if (sqlite3_exec(db, (CStringA)Sql_Data, NULL, NULL, NULL) != SQLITE_OK)
					{
						// 提示错误信息
						::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

						// 返回修改失败
						return FALSE;
					}

					//提交事务
					if (commitTransaction(db) == false)
					{
						// 回滚事务
						rollbackTransaction(db);

						// 提示错误信息
						AfxMessageBox(_T("无法重命名原数据表: ") + (CString)sqlite3_errmsg(db));

						// 返回修改失败
						return FALSE;
					}
					else
					{
						// 导入原表数据
						CStringA Insert_Sql = "INSERT INTO Temp SELECT * FROM " + (CStringA)TableName + ";";

						//数据库启动一个事物
						if (!transaction(db))
						{
							AfxMessageBox(_T("无法启动事务处理!"));
							return false;
						}

						// 若导入操作失败
						if (sqlite3_exec(db, Insert_Sql, NULL, NULL, NULL) != SQLITE_OK)
						{
							// 提示错误信息
							::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

							// 返回修改失败
							return FALSE;
						}
						else
						{
							// 删除原数据表
							if (!DeleteDataTable(TableName))
							{
								// 提示错误信息
								::MessageBox(NULL, _T("无法删除原数据表!"), NULL, NULL);

								// 返回修改失败
								return FALSE;
							}
							else
							{
								// 若重命名 Temp 表为目标数据表失败
								if (!UpdataDataTable(_T("Temp"), REN_TABLE, TableName))
								{
									// 提示错误信息
									::MessageBox(NULL, _T("无法重命名原数据表!"), NULL, NULL);

									// 返回修改失败
									return FALSE;
								}
								else
								{
									// 返回修改列成功
									return TRUE;
								}
							}
						}
					}
				}
			}
			catch (...)
			{
				// 提示错误信息
				AfxMessageBox(_T("在数据表中修改列失败!"));

				// 返回修改失败
				return FALSE;
			}

		}break;

		// 默认什么也不做
		default:
		{
			AfxMessageBox(_T("无效的命令，本次SQL无操作!"));

		}break;
		}
	}

	// 默认返回修改失败信息
	return FALSE;
}


// 在目标数据库中检查数据表     ( 检查数据表是否存在 )
BOOL CSqliteManager::CheckDataTable (CString TableName)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回检查失败信息
		return FALSE;
	}
	else
	{
		// 尝试执行检查数据表的操作
		try
		{
			// 定义比对变量
			CString CheckName;
			if (!GetTableName(CheckName))
			{
				// 弹出错误信息
				AfxMessageBox(_T("无法从数据库中获得表名"));

				// 返回检查失败
				return FALSE;
			}
			else
			{
				// 比对是否存在目标数据表
				if (CheckName.Replace(TableName, _T("")))
				{
					// 返回存在目标数据表
					return TRUE;
				}
				else
				{
					// 返回不存在目标数据表
					return FALSE;
				}
			}
		}
		catch (...)
		{
			AfxMessageBox(_T("无法从数据库中获得表名"));

			// 返回检查失败
			return FALSE;
		}
	}

	// 默认返回检查失败信息
	return FALSE;
}


// 在目标数据库中统计数据表     ( 统计数据表总共数量 )
BOOL CSqliteManager::CountDataTable (int &Count)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回创建失败信息
		return FALSE;
	}
	else
	{
		// 用于判断操作是否成功
		BOOL IsSuccess;

		// 初始化数量
		Count = 0;


		// 尝试执行获取数据表的操作
		try
		{
			// 初始化SQL语句
			CStringA GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name";

			// 若获取操作失败
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回获取失败
				IsSuccess = FALSE;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!stmt)
				{
					// 返回 0 个数量
					Count = 0;

					// 返回获取失败
					IsSuccess = FALSE;
				}

				// 若数据不为空
				else
				{
					// 循环获得数据表
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// 数据数量自加1
						Count ++;
					}

					// 返回获取成功
					IsSuccess = TRUE;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// 弹出错误信息
			AfxMessageBox(_T("无法从数据库中获得表名"));

			// 返回 0 个数量
			Count = 0;

			// 返回获取失败
			return FALSE;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return FALSE;
}


// 向目标数据库中导入数据表
BOOL CSqliteManager::ImportDataTable(CString TableName, CString TargetDataBase)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回创建失败信息
		return FALSE;
	}
	else
	{
		// 用于判断操作是否成功
		//BOOL IsSuccess;

		sqlite3 *pTarget;

		// 查找目标数据库是否存在
		CFileFind Finder;
		BOOL Status = Finder.FindFile(TargetDataBase);

		// 若存在目标数据库则尝试连接目标数据库
		if (Status)
		{
			// 尝试连接目标数据库
			try
			{
				// 创建并打开目标数据库
				if (sqlite3_open((CStringA)TargetDataBase, &pTarget) != SQLITE_OK) // 若无法连接数据库
				{
					// 弹出错误信息
					::MessageBox(NULL, (CString)sqlite3_errmsg(pTarget), NULL, NULL);

					// 若关闭数据库失败
					if (sqlite3_close(pTarget) != SQLITE_OK)
					{
						// 提示错误信息
						::MessageBox(NULL, (CString)sqlite3_errmsg(pTarget), NULL, NULL);

						// 返回关闭失败信息
						return FALSE;
					}

					// 返回数据库连接失败的信息
					return FALSE;
				}

				// 若数据库成功连接
				else
				{
					try
					{
						// 检查数据表是否存在
						if (CheckDataTable(TableName))
						{
							//数据库启动一个事物
							if (!transaction(db))
							{
								AfxMessageBox(_T("无法启动事务处理!"));
								return false;
							}

							// 初始化SQL语句
							CStringA InsertSQL;//"INSERT INTO " + (CStringA)TableName + " VALUES( " + (CStringA)Params + " );";

							// 若添加操作失败
							if (sqlite3_exec(db, InsertSQL, NULL, NULL, &errMsg) != SQLITE_OK)
							{
								// 提示错误信息
								::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

								// 返回添加失败信息
								return FALSE;
							}

							//提交事务
							if (commitTransaction(db) == false)
							{
								// 回滚事务
								rollbackTransaction(db);

								// 提示错误信息
								AfxMessageBox(_T("添加数据操作失败: ") + (CString)sqlite3_errmsg(db));

								// 返回添加失败信息
								return FALSE;
							}

						}
						else
						{
							// 弹出错误信息
							::MessageBox(NULL, _T("目标数据表不存在,无法导入数据!"), NULL, NULL);

							// 返回数据导入失败的信息
							return FALSE;
						}
					}
					// 捕捉错误
					catch (...)
					{
						// 弹出错误信息
						::MessageBox(NULL, _T("无法导入数据到目标数据库!"), NULL, NULL);

						// 返回数据导入失败的信息
						return FALSE;
					}
				}
			}
			// 捕捉错误
			catch (...)
			{
				// 弹出错误信息
				::MessageBox(NULL, _T("无法连接到目标数据库!"), NULL, NULL);

				// 返回数据库连接失败的信息
				return FALSE;
			}
		}
		else
		{
			// 弹出错误信息
			::MessageBox(NULL, _T("无法连接到目标数据库, 因为目标数据库不存在!"), NULL, NULL);

			// 返回数据库连接失败的信息
			return FALSE;
		}

		// 默认返回导入失败
		return FALSE;
	}
}


// 从目标数据库中导出数据表
BOOL CSqliteManager::ExportDataTable(CString TableName, CString TargetDataBase)
{
	return true;
}


///////////////////////////////////数据项操作代码/////////////////////////////////////


// 向目标数据表中添加数据项
BOOL CSqliteManager::InsertData(CString TableName, CString Params)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回添加失败信息
		return FALSE;
	}
	else
	{
		// 尝试执行添加数据的操作
		try
		{
			// 初始化SQL语句
			CStringA InsertSQL = "INSERT INTO " + (CStringA)TableName + " VALUES( " + (CStringA)Params + " );";

			//数据库启动一个事物
			if (!transaction(db))
			{
				AfxMessageBox(_T("无法启动事务处理!"));
				return false;
			}

			// 若添加操作失败
			if (sqlite3_exec(db, InsertSQL, NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回添加失败信息
				return FALSE;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				AfxMessageBox(_T("添加数据操作失败: ") + (CString)sqlite3_errmsg(db));

				// 返回添加失败信息
				return FALSE;
			}

			// 返回添加成功信息
			else
				return TRUE;
		}
		catch (...)
		{
			// 提示错误信息
			AfxMessageBox(_T("添加数据操作失败!"));

			// 返回添加失败信息
			return FALSE;
		}
	}

	// 默认返回添加失败信息
	return FALSE;
}


// 从目标数据表中删除数据项
BOOL CSqliteManager::DeleteData(CString TableName, CString Params)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回删除失败信息
		return FALSE;
	}
	else
	{
		// 尝试执行从数据表中删除数据的操作
		try
		{
			// 初始化SQL语句
			CStringA DeleteSQL;
			if (!Params.IsEmpty())
				DeleteSQL = "Delete From " + (CStringA)TableName + " Where " + (CStringA)Params + ";";
			else
				DeleteSQL = "Delete From " + (CStringA)TableName + ";";

			//数据库启动一个事物
			if (!transaction(db))
			{
				AfxMessageBox(_T("无法启动事务处理!"));
				return false;
			}

			// 若删除操作失败
			if (sqlite3_exec(db, DeleteSQL, NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回删除失败
				return FALSE;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				AfxMessageBox(_T("添加数据操作失败: ") + (CString)sqlite3_errmsg(db));

				// 返回删除失败
				return FALSE;
			}

			// 返回删除成功
			else
				return TRUE;
		}
		catch (...)
		{
			// 提示错误信息
			AfxMessageBox(_T("删除数据失败!"));

			// 返回删除失败
			return FALSE;
		}
	}
}


// 从目标数据表中修改数据项
BOOL CSqliteManager::UpdataData(CString TableName, CString Column, CString NewData, CString Params)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回修改失败信息
		return FALSE;
	}
	else
	{
		// 尝试执行修改数据的操作
		try
		{
			// 初始化SQL语句
			CStringA UpData_SQL;

			// 若参数为空
			if (Params.IsEmpty())
			{
				UpData_SQL = _T("UPDATE ") + TableName + _T(" SET ") + Column + _T(" = ") + _T("'") + NewData + _T("'") + _T(";");
			}
			else
			{
				UpData_SQL = _T("UPDATE ") + TableName + _T(" SET ") + Column + _T(" = ") + _T("'") + NewData + _T("'") + _T(" Where ") + Params + _T(";");
			}
			
			//数据库启动一个事物
			if (!transaction(db))
			{
				AfxMessageBox(_T("无法启动事务处理!"));
				return false;
			}

			// 若修改操作失败
			if (sqlite3_exec(db, UpData_SQL, NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回修改失败信息
				return FALSE;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				AfxMessageBox(_T("修改数据操作失败: ") + (CString)sqlite3_errmsg(db));

				// 返回修改失败信息
				return FALSE;
			}

			// 返回修改成功信息
			else
				return TRUE;
		}
		catch (...)
		{
			// 提示错误信息
			AfxMessageBox(_T("修改数据操作失败!"));

			// 返回修改失败信息
			return FALSE;
		}
	}

	// 默认返回修改失败信息
	return FALSE;
}


// 从目标数据表中修改数据项
BOOL CSqliteManager::UpdataData(CString TableName, CString ColumnParams, CString Params)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回修改失败信息
		return FALSE;
	}
	else
	{
		// 尝试执行修改数据的操作
		try
		{
			// 初始化SQL语句
			CStringA UpData_SQL;

			// 若参数为空
			if (Params.IsEmpty())
			{
				UpData_SQL = _T("UPDATE ") + TableName + _T(" SET ") + ColumnParams + _T(";");
			}
			else
			{
				UpData_SQL = _T("UPDATE ") + TableName + _T(" SET ") + ColumnParams + _T(" Where ") + Params + _T(";");
			}

			//数据库启动一个事物
			if (!transaction(db))
			{
				AfxMessageBox(_T("无法启动事务处理!"));
				return false;
			}

			// 若修改操作失败
			if (sqlite3_exec(db, UpData_SQL, NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回修改失败信息
				return FALSE;
			}

			//提交事务
			if (commitTransaction(db) == false)
			{
				// 回滚事务
				rollbackTransaction(db);

				// 提示错误信息
				AfxMessageBox(_T("修改数据操作失败: ") + (CString)sqlite3_errmsg(db));

				// 返回修改失败信息
				return FALSE;
			}

			// 返回修改成功信息
			else
				return TRUE;
		}
		catch (...)
		{
			// 提示错误信息
			AfxMessageBox(_T("修改数据操作失败!"));

			// 返回修改失败信息
			return FALSE;
		}
	}

	// 默认返回修改失败信息
	return FALSE;
}


// 从目标数据表中查询数据项
BOOL CSqliteManager::SelectData(CString TableName, vector<CString> &pResult,  BOOL DISTINCT, CString COUNT, CString COLUMN, CString Params, CString GROUP, CString HAVING, CString Order, CString Limit, int SortMode)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回查询失败信息
		return FALSE;
	}
	else
	{
		// 尝试执行查询数据的操作
		try
		{
			// 清空原先的结果集
			pResult.clear();

			// 定义查询参数
			int rc, i, ncols;
			const char *tail;

			// 初始化SQL语句
			CStringA Select_SQL;
			CString  DISTINCT_Text = _T("");

			CString  SortText = _T("ASC");

			// 若数据列为空
			if (COLUMN.IsEmpty())
			{
				if (DISTINCT)
				{
					// 提示错误
					AfxMessageBox(_T("列为空的情况下, DISTINCT 不可以为真"));
					return FALSE;
				}
				if (!COUNT.IsEmpty())
				{
					// 提示错误
					AfxMessageBox(_T("列为空的情况下, COUNT 不可以不为空"));
					return FALSE;
				}

				COLUMN = _T("*");
			}
			else
			{
				// 若去除重复为真
				if (DISTINCT)
				{
					DISTINCT_Text = _T("DISTINCT ");
				}

				// 若计数不为空
				if (!COUNT.IsEmpty())
				{
					COUNT = _T(", ") + COUNT;
				}
			}

			// 若参数为空
			if (Params.IsEmpty())
			{
				if (!GROUP.IsEmpty())
				{
					// 提示错误
					AfxMessageBox(_T("参数为空的情况下, 分组不可以不为空"));
					return FALSE;
				}

				if (!HAVING.IsEmpty())
				{
					// 提示错误
					AfxMessageBox(_T("参数为空的情况下, 筛选不可以不为空"));
					return FALSE;
				}

				if (!Order.IsEmpty())
				{
					// 提示错误
					AfxMessageBox(_T("参数为空的情况下, 排序不可以不为空"));
					return FALSE;
				}

				if (!Limit.IsEmpty())
				{
					// 提示错误
					AfxMessageBox(_T("参数为空的情况下, 偏移量不可以不为空"));
					return FALSE;
				}
			}
			else
			{
				// 若分组不为空
				if (!GROUP.IsEmpty())
				{
					GROUP = _T(" GROUP By ") + GROUP;
				}

				// 若筛选不为空
				if (!HAVING.IsEmpty())
				{
					HAVING = _T(" HAVING ") + HAVING;
				}

				// 若排序不为空
				if (!Order.IsEmpty())
				{
					Order = _T(" Order by ") + Order;

					// 根据代码进行排序
					if (SortMode == MOD_ASC)
					{
						SortText = _T(" ASC ");
					}
					else
					{
						SortText = _T(" DESC ");
					}
				}
				else
				{
					SortText = _T("");
				}

				// 若偏移量不为空
				if (!Limit.IsEmpty())
				{
					Limit = _T(" Limit ") + Limit;
				}
			}
			
			
			// 构造查询SQL
			Select_SQL = _T("SELECT ") + DISTINCT_Text + COLUMN + COUNT + _T(" FROM ") + TableName + _T(" Where ") + Params + GROUP + HAVING + Order + SortText + Limit + _T(";");

			// 若预处理操作成功
			if (sqlite3_prepare(db, Select_SQL, (int)strlen(Select_SQL), &stmt, &tail) == SQLITE_OK)
			{
				rc    = sqlite3_step(stmt);
				ncols = sqlite3_column_count(stmt);
				while (rc == SQLITE_ROW)
				{
					for (i = 0; i < ncols; i++)
					{
						// 循环将查询结果放入结果集
						pResult.push_back((CString)sqlite3_column_text(stmt, i));
					}
					
					rc = sqlite3_step(stmt);
				}

				//释放statement
				sqlite3_finalize(stmt);

				// 返回查询成功信息
				return TRUE;
			}
			else
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回查询失败信息
				return FALSE;
			}

			// 释放变量
			sqlite3_finalize(stmt);
		}
		catch (...)
		{
			// 提示错误信息
			AfxMessageBox(_T("查询数据操作失败!"));

			// 返回查询失败信息
			return FALSE;
		}
	}

	// 默认返回查询失败信息
	return FALSE;
}


// 从目标数据表中查询单数据
BOOL CSqliteManager::SelectData(CString TableName, CString &pResult, int Col, BOOL DISTINCT, CString COUNT, CString COLUMN, CString Params, CString GROUP, CString HAVING, CString Order, CString Limit, int SortMode)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回查询失败信息
		return FALSE;
	}
	else
	{
		// 尝试执行查询数据的操作
		try
		{
			// 清空原先的结果
			pResult = _T("");

			// 定义查询参数
			int rc, i, ncols;
			const char *tail;

			// 初始化SQL语句
			CStringA Select_SQL;
			CString  DISTINCT_Text = _T("");

			CString  SortText = _T("ASC");

			// 若数据列为空
			if (COLUMN.IsEmpty())
			{
				if (DISTINCT)
				{
					// 提示错误
					AfxMessageBox(_T("列为空的情况下, DISTINCT 不可以为真"));
					return FALSE;
				}
				if (!COUNT.IsEmpty())
				{
					// 提示错误
					AfxMessageBox(_T("列为空的情况下, COUNT 不可以不为空"));
					return FALSE;
				}

				COLUMN = _T("*");
			}
			else
			{
				// 若去除重复为真
				if (DISTINCT)
				{
					DISTINCT_Text = _T("DISTINCT ");
				}

				// 若计数不为空
				if (!COUNT.IsEmpty())
				{
					COUNT = _T(", ") + COUNT;
				}
			}

			// 若参数为空
			if (Params.IsEmpty())
			{
				if (!GROUP.IsEmpty())
				{
					// 提示错误
					AfxMessageBox(_T("参数为空的情况下, 分组不可以不为空"));
					return FALSE;
				}

				if (!HAVING.IsEmpty())
				{
					// 提示错误
					AfxMessageBox(_T("参数为空的情况下, 筛选不可以不为空"));
					return FALSE;
				}

				if (!Order.IsEmpty())
				{
					// 提示错误
					AfxMessageBox(_T("参数为空的情况下, 排序不可以不为空"));
					return FALSE;
				}

				if (!Limit.IsEmpty())
				{
					// 提示错误
					AfxMessageBox(_T("参数为空的情况下, 偏移量不可以不为空"));
					return FALSE;
				}
			}
			else
			{
				// 若分组不为空
				if (!GROUP.IsEmpty())
				{
					GROUP = _T(" GROUP By ") + GROUP;
				}

				// 若筛选不为空
				if (!HAVING.IsEmpty())
				{
					HAVING = _T(" HAVING ") + HAVING;
				}

				// 若排序不为空
				if (!Order.IsEmpty())
				{
					Order = _T(" Order by ") + Order;

					// 根据代码进行排序
					if (SortMode == MOD_ASC)
					{
						SortText = _T(" ASC ");
					}
					else
					{
						SortText = _T(" DESC ");
					}
				}
				else
				{
					SortText = _T("");
				}

				// 若偏移量不为空
				if (!Limit.IsEmpty())
				{
					Limit = _T(" Limit ") + Limit;
				}
			}

			// 构造查询SQL
			Select_SQL = _T("SELECT ") + DISTINCT_Text + COLUMN + COUNT + _T(" FROM ") + TableName + _T(" Where ") + Params + GROUP + HAVING + Order + SortText + Limit + _T(";");

			// 若预处理操作成功
			if (sqlite3_prepare(db, Select_SQL, (int)strlen(Select_SQL), &stmt, &tail) == SQLITE_OK)
			{
				// r若目标列为0
				if (Col == 0)
					Col = 1;

				// 执行预处理
				rc = sqlite3_step(stmt);
				ncols = sqlite3_column_count(stmt);
				while (rc == SQLITE_ROW)
				{
					// 循环得到查询结果
					for (i = 0; i < ncols; i++)
					{
						// 若目标列与i相等
						if (i == Col)
						{
							// 得到目标数据
							pResult = (CString)sqlite3_column_text(stmt, i);
						}
					}

					rc = sqlite3_step(stmt);
				}

				//释放statement
				sqlite3_finalize(stmt);

				// 返回查询成功信息
				return TRUE;
			}
			else
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回查询失败信息
				return FALSE;
			}

			// 释放变量
			sqlite3_finalize(stmt);
		}
		catch (...)
		{
			// 提示错误信息
			AfxMessageBox(_T("查询数据操作失败!"));

			// 返回查询失败信息
			return FALSE;
		}
	}

	// 默认返回查询失败信息
	return FALSE;
}


// 在目标数据表中检查数据项
BOOL CSqliteManager::CheckData (CString TableName, int Col, BOOL DISTINCT, CString COUNT, CString COLUMN, CString Params, CString GROUP, CString HAVING, CString Order, CString Limit, int SortMode)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回查询失败信息
		return FALSE;
	}
	else
	{
		CString CheckData = _T("");

		if (SelectData(TableName, CheckData, Col, DISTINCT, COUNT, COLUMN, Params, GROUP, HAVING, Order, Limit, SortMode))
		{
			if (!CheckData.IsEmpty() && CheckData != _T(""))
			{
				// 返回目标数据存在
				return TRUE;
			}
			else
			{
				// 返回目标数据不存在
				return FALSE;
			}
		}
		else
		{
			AfxMessageBox(_T("数据库查询失败!"));

			// 返回查询失败信息
			return FALSE;
		}
	}

	// 默认返回查询失败信息
	return FALSE;
}


// 统计目标数据表中数据数项
BOOL CSqliteManager::CountNumber(CString TableName, CString Params, int &Count)
{
	// 若数据库未连接成功
	if (!IsConnect)
	{
		// 提示错误信息
		AfxMessageBox(_T("数据库未连接，请先连接数据库!"));

		// 返回创建失败信息
		return FALSE;
	}
	else
	{
		// 用于判断操作是否成功
		BOOL IsSuccess;

		// 尝试执行获取数据表中列名的操作
		try
		{
			// 初始化SQL语句
			CStringA GetSQL;

			// 若参数为空
			if (Params.IsEmpty())
			{
				GetSQL = "SELECT * FROM " + (CStringA)TableName + ";";
			}

			// 若参数不为空
			else
			{
				GetSQL = "SELECT * FROM " + (CStringA)TableName + " Where " + (CStringA)Params + ";";
			}
			

			// 若获取操作失败
			if (sqlite3_get_table(db, GetSQL, &pRes, &nRow, &nCol, &errMsg) != SQLITE_OK)
			{
				// 提示错误信息
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// 返回获取失败
				IsSuccess = FALSE;
			}

			// 若获取成功
			else
			{
				// 若数据为空
				if (!nRow)
				{
					// 返回0个数据
					Count = 0;

					// 返回获取失败
					IsSuccess = FALSE;
				}

				// 若数据不为空
				else
				{
					// 得到数据个数
					Count = nRow;

					// 释放资源
					if (errMsg != NULL)
					{
						sqlite3_free(errMsg);
					}

					// 返回获取成功
					IsSuccess = TRUE;
				}
			}

			//释放查询结果(无论查询是否成功)
			sqlite3_free_table(pRes);
		}
		catch (...)
		{
			// 弹出错误信息
			AfxMessageBox(_T("无法从数据库中获得表名"));

			// 返回0个数据
			Count = 0;

			// 返回获取失败
			return FALSE;
		}

		// 返回操作结果
		return IsSuccess;
	}

	// 默认返回获取失败
	return FALSE;
}


// 向目标数据表中导入数据项
BOOL CSqliteManager::ImportData (CString TableName, CString TargetTableName, CString Params)
{
	return true;
}


// 从目标数据表中导出数据项
BOOL CSqliteManager::ExportData (CString TableName, CString TargetTableName, CString Params)
{
	return true;
}


// 删除目标数据表中行据数项
BOOL CSqliteManager::DeleteRowData(CString TableName, int Row)
{
	return true;
}


// 删除目标数据表中列据数项
BOOL CSqliteManager::DeleteColData(CString TableName, int Col)
{
	return true;
}


// 删除目标数据表中初据数项
BOOL CSqliteManager::DeleteMinData(CString TableName)
{
	return true;
}


// 删除目标数据表中间据数项
BOOL CSqliteManager::DeleteMidData(CString TableName)
{
	return true;
}


// 删除目标数据表中末据数项
BOOL CSqliteManager::DeleteMaxData(CString TableName)
{
	return true;
}


// 修改目标数据表中行据数项
BOOL CSqliteManager::UpdataRowData(CString TableName, int Row, CString UpdataData)
{
	return true;
}


// 修改目标数据表中列据数项
BOOL CSqliteManager::UpdataColData(CString TableName, int Col, CString UpdataData)
{
	return true;
}


// 修改目标数据表中初据数项
BOOL CSqliteManager::UpdataMinData(CString TableName, CString UpdataData)
{
	return true;
}


// 修改目标数据表中间据数项
BOOL CSqliteManager::UpdataMidData(CString TableName, CString UpdataData)
{
	return true;
}


// 修改目标数据表中末据数项
BOOL CSqliteManager::UpdataMaxData(CString TableName, CString UpdataData)
{
	return true;
}


// 检索目标数据表中行据数项
BOOL CSqliteManager::SelectRowData(CString TableName, int Row, CString &RowData)
{
	return true;
}


// 检索目标数据表中列据数项
BOOL CSqliteManager::SelectColData(CString TableName, int Col, CString &ColData)
{
	return true;
}


// 检索目标数据表中初据数项
BOOL CSqliteManager::SelectMinData(CString TableName, CString &MinData)
{
	return true;
}


// 修改目标数据表中间据数项
BOOL CSqliteManager::SelectMidData(CString TableName, CString &MidData)
{
	return true;
}


// 检索目标数据表中末据数项
BOOL CSqliteManager::SelectMaxData(CString TableName, CString &MaxData)
{
	return true;
}


// 为目标数据表中据数项排序
BOOL CSqliteManager::SelectSortData(CString TableName, int Sort, CString &SortData)
{
	return true;
}


// 重新排列数据表据数项顺序
BOOL CSqliteManager::ChangeSortData(CString TableName, int Sort)
{
	return true;
}


// 从目标数据表中筛选数据项
BOOL CSqliteManager::FilterData(CString TableName, CString Params, CString &FilterData)
{
	return true;
}


// 从目标数据表中得到随机数据项
BOOL CSqliteManager::RanData(CString TableName, CString Params, CString &RanData)
{
	return true;
}


///////////////////////////////////数据项计算代码////////////////////////////////////////////////////


// 从目标数据表中求数据项和
BOOL CSqliteManager::SumData(CString TableName, CString Params, int &SumData)
{
	return true;
}


// 从目标数据表中求数据项差
BOOL CSqliteManager::SubData(CString TableName, CString Params, int &SubData)
{
	return true;
}


// 从目标数据表中求数据项积
BOOL CSqliteManager::ProductData(CString TableName, CString Params, int &ProductData)
{
	return true;
}


// 从目标数据表中求数据项商
BOOL CSqliteManager::QuotientData(CString TableName, CString Params, int &QuotientData)
{
	return true;
}


// 从目标数据表中求数据项余
BOOL CSqliteManager::ModData(CString TableName, CString Params, int &ModData)
{
	return true;
}


// 从目标数据表中求数据项幂
BOOL CSqliteManager::PowerData(CString TableName, CString Params, int &PowerData, int &Power)
{
	return true;
}


// 从目标数据表中求数据项对数
BOOL CSqliteManager::LogData(CString TableName, CString Params, int &LogData, int &Log)
{
	return true;
}


// 从目标数据表中求数据项阶乘
BOOL CSqliteManager::FactData(CString TableName, CString Params, int &FactData)
{
	return true;
}


// 从目标数据表中求数据项乘方
BOOL CSqliteManager::ExpData(CString TableName, CString Params, int &ExpData, int &Exp)
{
	return true;
}


// 从目标数据表中求数据项众数
BOOL CSqliteManager::PluData(CString TableName, CString Params, int &PluData)
{
	return true;
}


// 从目标数据表中求数据项极限
BOOL CSqliteManager::LimData(CString TableName, CString Params, int &LimData)
{
	return true;
}


// 从目标数据表中求数据项正弦
BOOL CSqliteManager::SinData(CString TableName, CString Params, int &SinData)
{
	return true;
}


// 从目标数据表中求数据项余弦
BOOL CSqliteManager::CosData(CString TableName, CString Params, int &CosData)
{
	return true;
}


// 从目标数据表中求数据项正切
BOOL CSqliteManager::TanData(CString TableName, CString Params, int &TanData)
{
	return true;
}


// 从目标数据表中求数据项平均数
BOOL CSqliteManager::AvgData(CString TableName, CString Params, int &AvgData)
{
	return true;
}


// 从目标数据表中求数据项中位数
BOOL CSqliteManager::MidData(CString TableName, CString Params, int &MidData)
{
	return true;
}


// 从目标数据表中求数据项百分比
BOOL CSqliteManager::PerData(CString TableName, CString Params, int &PerData)
{
	return true;
}


// 从目标数据表中求数据项平方根
BOOL CSqliteManager::SqrData(CString TableName, CString Params, int &SqrData)
{
	return true;
}


// 从目标数据表中求数据项立方根
BOOL CSqliteManager::CubData(CString TableName, CString Params, int &CubData)
{
	return true;
}


// 从目标数据表中求数据项绝对值
BOOL CSqliteManager::AbsData(CString TableName, CString Params, int &AbsData)
{
	return true;
}


// 从目标数据表中求数据项最大值
BOOL CSqliteManager::MaxData(CString TableName, CString Params, int &AbsData)
{
	return true;
}


// 从目标数据表中求数据项最小值
BOOL CSqliteManager::MinData(CString TableName, CString Params, int &AbsData)
{
	return true;
}


// 从目标数据表中求数据项最大公约数
BOOL CSqliteManager::GcdData(CString TableName, CString Params, int &GcdData)
{
	return true;
}


// 从目标数据表中求数据项最小公倍数
BOOL CSqliteManager::LcmData(CString TableName, CString Params, int &LcmData)
{
	return true;
}


// 从目标数据表中取数据项随机数
BOOL RanData(CString TableName, CString Params, int &RanData)
{
	return true;
}


///////////////////////////////////编码类型转换代码///////////////////////////////////


// ACSII   编码转 Unicode 编码  
wstring CSqliteManager::AcsiiToUnicode(const string  &acsii_string)
{
	wstring unicode_string;

	//CP_ACP - default to ANSI code page  
	int len = MultiByteToWideChar(CP_ACP, 0, acsii_string.c_str(), -1, NULL, 0);
	if (ERROR_NO_UNICODE_TRANSLATION == len || 0 == len)
	{
		//return empty wstring  
		return unicode_string;
	}

	vector<wchar_t> vec_result(len);
	int result_len = MultiByteToWideChar(CP_ACP, 0, acsii_string.c_str(), -1, &vec_result[0], len);
	if (result_len != len)
	{
		//return empty wstring  
		return unicode_string;
	}

	unicode_string = wstring(&vec_result[0]);
	return unicode_string;
}


// ACSII   编码转 UTF8    编码
string  CSqliteManager::AcsiiToUtf8(   const string  &acsii_string)
{
	wstring unicode_string = AcsiiToUnicode(acsii_string);  //将 ACSII   编码转换为 Unicode  编码
	string  utf8_string = UnicodeToUtf8(unicode_string);    //将 Unicode 编码转换为 UTF8     编码

	// 返回编码转换结果
	return  utf8_string;
}


// Unicode 编码转 ACSII   编码
string  CSqliteManager::UnicodeToAcsii(const wstring &unicode_string)
{
	string acsii_string;

	//CP_OEMCP - default to OEM  code page  
	int len = WideCharToMultiByte(CP_OEMCP, 0, unicode_string.c_str(), -1, NULL, 0, NULL, NULL);
	if (ERROR_NO_UNICODE_TRANSLATION == len || 0 == len)
	{
		//return empty wstring  
		return acsii_string;
	}

	vector<char> vec_result(len);
	int result_len = WideCharToMultiByte(CP_OEMCP, 0, unicode_string.c_str(), -1, &vec_result[0], len, NULL, NULL);;
	if (result_len != len)
	{
		//return empty wstring  
		return acsii_string;
	}

	acsii_string = string(&vec_result[0]);
	return acsii_string;
}


// Unicode 编码转 UTF8    编码
string  CSqliteManager::UnicodeToUtf8( const wstring &unicode_string)
{
	string utf8_string;

	//CP_UTF8 - UTF-8 translation  
	int len = WideCharToMultiByte(CP_UTF8, 0, unicode_string.c_str(), -1, NULL, 0, NULL, NULL);
	if (0 == len)
	{
		//return empty wstring  
		return utf8_string;
	}

	vector<char> vec_result(len);
	int result_len = WideCharToMultiByte(CP_UTF8, 0, unicode_string.c_str(), -1, &vec_result[0], len, NULL, NULL);;
	if (result_len != len)
	{
		//return empty wstring  
		return utf8_string;
	}

	utf8_string = string(&vec_result[0]);
	return utf8_string;
}


// UTF8    编码转 ACSII   编码
string  CSqliteManager::Utf8ToAcsii(   const string  &utf8_string)
{
	wstring unicode_string = Utf8ToUnicode(utf8_string);        //将UTF8转换为Unicode  
	string acsii_string = UnicodeToAcsii(unicode_string);   //将Unicode转换为ACSII  
	return acsii_string;
}


// UTF8    编码转 Unicode 编码
wstring CSqliteManager::Utf8ToUnicode( const string  &utf8_string)
{
	wstring unicode_string;

	//CP_UTF8 - UTF-8 translation  
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, NULL, 0);
	if (ERROR_NO_UNICODE_TRANSLATION == len || 0 == len)
	{
		//return empty wstring  
		return unicode_string;
	}

	vector<wchar_t> vec_result(len);
	int result_len = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, &vec_result[0], len);
	if (result_len != len)
	{
		//return empty wstring  
		return unicode_string;
	}

	unicode_string = wstring(&vec_result[0]);
	return unicode_string;
}
