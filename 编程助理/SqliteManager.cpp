#include "stdafx.h"
#include "SqliteManager.h"


///////////////////////////////////�����۹�����///////////////////////////////////


// Ĭ�Ϲ��캯��
CSqliteManager::CSqliteManager()
{
	// Ĭ������ʧ��
	IsConnect = FALSE;
}


// �����ݿ����ƺ�·�������Ĺ��캯��
CSqliteManager::CSqliteManager(CString Name, CString Path)
{
	// Ĭ������ʧ��
	IsConnect = FALSE;


	// ��Ŀ�����ݿ�����
	if (!OpenDataBase(Name, Path))
	{
		// ����������Ϣ
		AfxMessageBox(_T("Ŀ�����ݿ���󴴽�ʧ��!"));

		// �������ݿ���󴴽�ʧ�ܵ���Ϣ
		return;

	}
}


// Ĭ���۹�����
CSqliteManager::~CSqliteManager()
{
	// �ر����ݿ�����
	CloseDataBase();
}


///////////////////////////////////������������///////////////////////////////////


// �����������������
int CSqliteManager::FindCharCount(CString csStr, char c)
{
	// ��ʼ����������Ϊ0
	int iCount = 0;

	// ѭ�����Ҳ���
	for (int i = 0; i<csStr.GetLength(); i++)
	{
		// ѭ������
		i = csStr.Find(c, i + 1);

		// ���������Լ�1
		iCount++;
	}

	// ���ز�������
	return iCount;
}


//���ڼ����ļ����ڵ��ļ�����
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

////////////////////////////////////���ﴦ�����//////////////////////////////////

bool CSqliteManager::transaction(sqlite3 *p)
{
	bool result = true;
	char *zErrorMsg = NULL;
	int ret = sqlite3_exec(p, "begin transaction", 0, 0, &zErrorMsg); // ��ʼһ������  
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
	int ret = sqlite3_exec(p, "commit transaction", 0, 0, &zErrorMsg); // �ύ����  
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

////////////////////////////////////���ݴ������///////////////////////////////////

Result *CSqliteManager::Query(sqlite3 *p, const string &sql)
{
	Result *pRe = NULL;
	char *errmsg = NULL;
	char **dbResult;
	int nRow = 0;
	int nColumn = 0;

	//��ʼ��ѯ���ݿ�
	int result = sqlite3_get_table(p, sql.c_str(), &dbResult, &nRow, &nColumn, &errmsg);
	if (result == SQLITE_OK)
	{
		pRe = new Result(nRow, nColumn, dbResult);
	}
	else
	{
		//LOGI("[sqlite] SqliteBaseV Query error:dbName=%s,msg=%s sql:%s", dbName.c_str(), errmsg, sql.c_str());
	}

	// �������ݿ��ѯ�Ƿ�ɹ������ͷ� char** ��ѯ���
	//sqlite3_free_table(dbResult);

	//�ͷ��ڴ�
	sqlite3_free(errmsg);
	return pRe;
}


///////////////////////////////////���ݿ��������//////////////////////////////////


// ��Ŀ�����ݿ�����
BOOL CSqliteManager::OpenDataBase(CString Name, CString Path)
{
	// ���ݿ����Ĭ��Ϊ��
	db = NULL;

	// �����ݿ������ӳɹ�
	if (IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("�����ݿ������ӣ�����Ҫ�ٴ�����!"));

		// ��������ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ��ʼ��Ŀ�����ݿ����
		CString DatabasePath = Path;

		// ��Ŀ��·����Ϊ��
		if (!Path.IsEmpty() || atoi((CStringA)Path) != NULL)
		{
			// ���Ŀ��·�������� \ �ͼ���												   
			if (DatabasePath.Right(1) != _T("\\"))
				DatabasePath += _T("\\");

			// ����Ŀ�����ݿ��ֵΪĿ��·������Ŀ���ļ���
			DatabasePath += Name;
		}
		else
		{
			// ����Ŀ�����ݿ��ֵΪ��ǰ·������Ŀ���ļ���
			DatabasePath = _T("./") + Name;
		}
		

		// ����Ŀ�����ݿ��Ƿ����
		CFileFind Finder;
		BOOL Status = Finder.FindFile(DatabasePath);

		// ��������Ŀ�����ݿ����Դ���������Ŀ�����ݿ�
		if (!Status)
		{
			// ��Ŀ��·����Ϊ��
			if (!Path.IsEmpty())
			{
				// ���Ŀ��·���Ƿ����
				DWORD DataPath = GetFileAttributes(Path);

				// ��Ŀ��·��������
				if (DataPath == 0xFFFFFFFF)
				{
					// ����Ŀ��·��
					CreateDirectory(Path, NULL);
				}
			}

			// ��������Ŀ�����ݿ�
			try
			{
				// ��������Ŀ�����ݿ�
				if (sqlite3_open((CStringA)DatabasePath, &db) != SQLITE_OK) // ���޷��������ݿ�
				{
					// ����������Ϣ
					::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

					// �ر����ݿ�����
					CloseDataBase();

					// �������ݿ�����ʧ�ܵ���Ϣ
					return FALSE;
				}

				// �����ݿ�ɹ�����
				else
				{
					// ��ʾ���ݿ�������
					IsConnect = TRUE;

					// �������ݿ����ӳɹ�����Ϣ
					return TRUE;
				}
			}
			// ��׽����
			catch (...)
			{
				// ����������Ϣ
				::MessageBox(NULL, _T("�޷����ӵ����ݿ�!"), NULL, NULL);

				// �������ݿ�����ʧ�ܵ���Ϣ
				return FALSE;
			}
		}
		// ������Ŀ�����ݿ���������Ŀ�����ݿ�
		else
		{
			// �����������ݿ�
			try
			{
				// ����Ŀ�����ݿ�
				if (sqlite3_open((CStringA)DatabasePath, &db) != SQLITE_OK) // ���޷��������ݿ�
				{
					// ����������Ϣ
					::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

					// �ر����ݿ�����
					CloseDataBase();

					// �������ݿ�����ʧ�ܵ���Ϣ
					return FALSE;
				}

				// �����ݿ�ɹ�����
				else
				{
					// ��ʾ���ݿ�������
					IsConnect = TRUE;

					// �������ݿ����ӳɹ�����Ϣ
					return TRUE;
				}
			}
			// ��׽����
			catch (...)
			{
				// ����������Ϣ
				::MessageBox(NULL, _T("�޷����ӵ����ݿ�!"), NULL, NULL);

				// �������ݿ�����ʧ�ܵ���Ϣ
				return FALSE;
			}
		}
	}

	// Ĭ�Ϸ������ݿ�����ʧ�ܵ���Ϣ
	return FALSE;
}


// �ر�Ŀ�����ݿ�����
BOOL CSqliteManager::CloseDataBase()
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("�����ݿ��ѹرգ�����Ҫ�ٴιر�!"));

		// ���عر�ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ���Թر����ݿ�����
		try
		{
			// ���ر����ݿ�ʧ��
			if (sqlite3_close(db) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���عر�ʧ����Ϣ
				return FALSE;
			}
			else
			{
				// ��ʾ���ݿ��ѶϿ�����
				IsConnect = FALSE;

				// ���ݿ��������Ϊ��
				db = NULL;

				// ���عرճɹ���Ϣ
				return TRUE;
			}
		}
		catch (...)
		{
			// ��ʾ������Ϣ
			AfxMessageBox(_T("���ݿ�ر�ʧ��!"));

			// ���عر�ʧ����Ϣ
			return FALSE;
		}
	}

	// Ĭ�Ϸ��عر�ʧ����Ϣ
	return FALSE;
}


// ����һ��Ŀ�����ݿ�
BOOL CSqliteManager::CreateDataBase(CString Name, CString Path)
{
	// ��ʼ��Ŀ�����ݿ����
	CString DatabasePath = Path;

	// ��Ŀ��·����Ϊ��
	if (!Path.IsEmpty() || atoi((CStringA)Path) != NULL)
	{
		// ���Ŀ��·�������� \ �ͼ���												   
		if (DatabasePath.Right(1) != _T("\\"))
			DatabasePath += _T("\\");

		// ����Ŀ�����ݿ��ֵΪĿ��·������Ŀ���ļ���
		DatabasePath += Name;
	}
	else
	{
		// ����Ŀ�����ݿ��ֵΪ��ǰ·������Ŀ���ļ���
		DatabasePath = _T("./") + Name;
	}

	// ����Ŀ�����ݿ��Ƿ����
	CFileFind Finder;
	BOOL Status = Finder.FindFile(DatabasePath);

	// ��������Ŀ�����ݿ����Դ���������Ŀ�����ݿ�
	if (!Status)
	{
		// ��Ŀ��·����Ϊ��
		if (!Path.IsEmpty())
		{
			// ���Ŀ��·���Ƿ����
			DWORD DataPath = GetFileAttributes(Path);

			// ��Ŀ��·��������
			if (DataPath == 0xFFFFFFFF)
			{
				// ����Ŀ��·��
				CreateDirectory(Path, NULL);
			}
		}

		// ���Դ���Ŀ�����ݿ�
		try
		{
			// ���ݿ����
			sqlite3 *Create_db;

			// ��������Ŀ�����ݿ�
			if (sqlite3_open((CStringA)DatabasePath, &Create_db) != SQLITE_OK) // ���޷��������ݿ�
			{
				// ����������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(Create_db), NULL, NULL);

				// ���ر����ݿ�ʧ��
				if (sqlite3_close(Create_db) != SQLITE_OK)
				{
					// ��ʾ������Ϣ
					::MessageBox(NULL, (CString)sqlite3_errmsg(Create_db), NULL, NULL);

					// ���عر�ʧ����Ϣ
					return FALSE;
				}

				// �������ݿⴴ��ʧ�ܵ���Ϣ
				return FALSE;
			}

			// �����ݿ�ɹ�����
			else
			{
				// ���ر����ݿ�ʧ��
				if (sqlite3_close(Create_db) != SQLITE_OK)
				{
					// ��ʾ������Ϣ
					::MessageBox(NULL, (CString)sqlite3_errmsg(Create_db), NULL, NULL);

					// ���عر�ʧ����Ϣ
					return FALSE;
				}

				// �������ݿⴴ���ɹ�����Ϣ
				return TRUE;
			}
		}
		// ��׽����
		catch (...)
		{
			// ����������Ϣ
			::MessageBox(NULL, _T("�޷�����Ŀ�����ݿ�!"), NULL, NULL);

			// �������ݿⴴ��ʧ�ܵ���Ϣ
			return FALSE;
		}
	}
	else
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("�Ѵ���Ŀ�����ݿ�,���δ�������ʧ��!"));

		// �������ݿⴴ��ʧ�ܵ���Ϣ
		return FALSE;
	}
	
	// Ĭ�Ϸ������ݿⴴ��ʧ�ܵ���Ϣ
	return FALSE;
}


// ɾ��һ��Ŀ�����ݿ�
BOOL CSqliteManager::DeleteDataBase(CString Name, CString Path)
{
	// ��ʼ��Ŀ�����ݿ����
	CString DatabasePath = Path;

	// ��Ŀ��·����Ϊ��
	if (!Path.IsEmpty() || atoi((CStringA)Path) != NULL)
	{
		// ���Ŀ��·�������� \ �ͼ���												   
		if (DatabasePath.Right(1) != _T("\\"))
			DatabasePath += _T("\\");

		// ����Ŀ�����ݿ��ֵΪĿ��·������Ŀ���ļ���
		DatabasePath += Name;
	}
	else
	{
		// ����Ŀ�����ݿ��ֵΪ��ǰ·������Ŀ���ļ���
		DatabasePath = _T("./") + Name;
	}

	// ����Ŀ�����ݿ��Ƿ����
	CFileFind Finder;
	BOOL Status = Finder.FindFile(DatabasePath);

	// ��������Ŀ�����ݿ�����ʾ������Ϣ
	if (!Status)
	{
		// ����������Ϣ
		AfxMessageBox(_T("������Ŀ�����ݿ�,����ɾ������ʧ��!"));

		// ����ɾ��ʧ��
		return FALSE;
	}
	else
	{
		// ����ɾ�����ݿ�
		try
		{
			if (DeleteFile(DatabasePath))
			{
				// ����ɾ���ɹ�
				return TRUE;
			}
			else
			{
				// ����������Ϣ
				AfxMessageBox(_T("����ɾ��Ŀ�����ݿ�ʧ��!"));

				// ����ɾ��ʧ��
				return FALSE;
			}
		}
		catch (...)
		{
			// ����������Ϣ
			AfxMessageBox(_T("����ɾ��Ŀ�����ݿ�ʧ��!"));

			// ����ɾ��ʧ��
			return FALSE;
		}
	}

	// Ĭ�Ϸ���ɾ��ʧ��
	return FALSE;
}


// �޸�Ŀ�����ݿ�����
BOOL CSqliteManager::ReNameDataBase(CString OldName, CString OldPath, CString NewName, CString NewPath)
{
	// ��ʼ��Ŀ�����ݿ����
	CString OldDatabasePath = OldPath, NewDatabasePath = NewPath;

	// ����Ŀ��·����Ϊ��
	if (!OldPath.IsEmpty() || atoi((CStringA)OldPath) != NULL)
	{
		// ���Ŀ��·�������� \ �ͼ���												   
		if (OldDatabasePath.Right(1) != _T("\\"))
			OldDatabasePath += _T("\\");

		// ����Ŀ�����ݿ��ֵΪĿ��·������Ŀ���ļ���
		OldDatabasePath += OldName;
	}
	else
	{
		// ����Ŀ�����ݿ��ֵΪ��ǰ·������Ŀ���ļ���
		OldDatabasePath = _T("./") + OldName;
	}

	// ����Ŀ��·����Ϊ��
	if (!NewPath.IsEmpty() || atoi((CStringA)NewPath) != NULL)
	{
		// ���Ŀ��·�������� \ �ͼ���												   
		if (NewDatabasePath.Right(1) != _T("\\"))
			NewDatabasePath += _T("\\");

		// ����Ŀ�����ݿ��ֵΪĿ��·������Ŀ���ļ���
		NewDatabasePath += NewName;
	}
	else
	{
		// ����Ŀ�����ݿ��ֵΪ��ǰ·������Ŀ���ļ���
		NewDatabasePath = _T("./") + NewName;
	}


	// ���Ҿ�Ŀ�����ݿ��Ƿ����
	CFileFind Finder;
	BOOL Status = Finder.FindFile(OldDatabasePath);

	// �������ھ�Ŀ�����ݿ�����ʾ������Ϣ
	if (!Status)
	{
		// ����������Ϣ
		AfxMessageBox(_T("�����ھ�Ŀ�����ݿ�,�����޸Ĳ���ʧ��!"));

		// �����޸�ʧ��
		return FALSE;
	}
	else
	{
		// �����޸����ݿ�
		try
		{
			// ����Ŀ��·����Ϊ��
			if (!NewPath.IsEmpty())
			{
				// �����Ŀ��·���Ƿ����
				DWORD DataPath = GetFileAttributes(NewPath);

				// ����Ŀ��·��������
				if (DataPath == 0xFFFFFFFF)
				{
					// ������Ŀ��·��
					CreateDirectory(NewPath, NULL);
				}
			}
			else
			{
				// ����������Ϣ
				AfxMessageBox(_T("��Ŀ�����ݿⲻ��Ϊ��,�ƶ�Ŀ�����ݿ�ʧ��!"));

				// �����ƶ�ʧ��
				return FALSE;
			}

			if (MoveFileEx(OldDatabasePath, NewDatabasePath, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING))
			{
				// �����޸ĳɹ�
				return TRUE;
			}
			else
			{
				// ����������Ϣ
				AfxMessageBox(_T("�޸�Ŀ�����ݿ�ʧ��!"));

				// �����޸�ʧ��
				return FALSE;
			}
		}
		catch (...)
		{
			// ����������Ϣ
			AfxMessageBox(_T("�޸�Ŀ�����ݿ�ʧ��!"));

			// �����޸�ʧ��
			return FALSE;
		}
	}

	// Ĭ�Ϸ����޸�ʧ��
	return FALSE;
}


// �ƶ�һ��Ŀ�����ݿ�
BOOL CSqliteManager::ReMoveDataBase(CString OldName, CString OldPath, CString NewName, CString NewPath)
{
	// ��ʼ��Ŀ�����ݿ����
	CString OldDatabasePath = OldPath, NewDatabasePath = NewPath;

	// ����Ŀ��·����Ϊ��
	if (!OldPath.IsEmpty() || atoi((CStringA)OldPath) != NULL)
	{
		// ���Ŀ��·�������� \ �ͼ���												   
		if (OldDatabasePath.Right(1) != _T("\\"))
			OldDatabasePath += _T("\\");

		// ����Ŀ�����ݿ��ֵΪĿ��·������Ŀ���ļ���
		OldDatabasePath += OldName;
	}
	else
	{
		// ����Ŀ�����ݿ��ֵΪ��ǰ·������Ŀ���ļ���
		OldDatabasePath = _T("./") + OldName;
	}

	// ����Ŀ��·����Ϊ��
	if (!NewPath.IsEmpty() || atoi((CStringA)NewPath) != NULL)
	{
		// ���Ŀ��·�������� \ �ͼ���												   
		if (NewDatabasePath.Right(1) != _T("\\"))
			NewDatabasePath += _T("\\");

		// ����Ŀ�����ݿ��ֵΪĿ��·������Ŀ���ļ���
		NewDatabasePath += NewName;
	}
	else
	{
		// ����Ŀ�����ݿ��ֵΪ��ǰ·������Ŀ���ļ���
		NewDatabasePath = _T("./") + NewName;
	}


	// ���Ҿ�Ŀ�����ݿ��Ƿ����
	CFileFind Finder;
	BOOL Status = Finder.FindFile(OldDatabasePath);

	// �������ھ�Ŀ�����ݿ�����ʾ������Ϣ
	if (!Status)
	{
		// ����������Ϣ
		AfxMessageBox(_T("�����ھ�Ŀ�����ݿ�,�����ƶ�����ʧ��!"));

		// �����ƶ�ʧ��
		return FALSE;
	}
	else
	{
		// �����޸����ݿ�
		try
		{
			// ����Ŀ��·����Ϊ��
			if (!NewPath.IsEmpty())
			{
				// �����Ŀ��·���Ƿ����
				DWORD DataPath = GetFileAttributes(NewPath);

				// ����Ŀ��·��������
				if (DataPath == 0xFFFFFFFF)
				{
					// ������Ŀ��·��
					CreateDirectory(NewPath, NULL);
				}
			}
			else
			{
				// ����������Ϣ
				AfxMessageBox(_T("��Ŀ�����ݿⲻ��Ϊ��,�ƶ�Ŀ�����ݿ�ʧ��!"));

				// �����ƶ�ʧ��
				return FALSE;
			}
			if (MoveFileEx(OldDatabasePath, NewDatabasePath, MOVEFILE_COPY_ALLOWED| MOVEFILE_REPLACE_EXISTING))
			{
				// �����ƶ��ɹ�
				return TRUE;
			}
			else
			{
				// ����������Ϣ
				AfxMessageBox(_T("�ƶ�Ŀ�����ݿ�ʧ��!"));

				// �����ƶ�ʧ��
				return FALSE;
			}
		}
		catch (...)
		{
			// ����������Ϣ
			AfxMessageBox(_T("�ƶ�Ŀ�����ݿ�ʧ��!"));

			// �����ƶ�ʧ��
			return FALSE;
		}
	}

	// Ĭ�Ϸ����ƶ�ʧ��
	return FALSE;
}


// ���Ŀ�����ݿ����
BOOL CSqliteManager::CheckDataBase(CString Name, CString Path)
{
	// ��ʼ��Ŀ�����ݿ����
	CString DatabasePath = Path;

	// ��Ŀ��·����Ϊ��
	if (!Path.IsEmpty() || atoi((CStringA)Path) != NULL)
	{
		// ���Ŀ��·�������� \ �ͼ���												   
		if (DatabasePath.Right(1) != _T("\\"))
			DatabasePath += _T("\\");

		// ����Ŀ�����ݿ��ֵΪĿ��·������Ŀ���ļ���
		DatabasePath += Name;
	}
	else
	{
		// ����Ŀ�����ݿ��ֵΪ��ǰ·������Ŀ���ļ���
		DatabasePath = _T("./") + Name;
	}


	// ���Դ����ݿ�
	try
	{
		sqlite3 *Check_db;
		if (sqlite3_open_v2((CStringA)DatabasePath, &Check_db, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK)
		{
			// ���ؼ��ʧ��
			return FALSE;
		}
		else
		{
			// �ر����ݿ�����
			if (sqlite3_close(Check_db) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(Check_db), NULL, NULL);

				// ���عر�ʧ����Ϣ
				return FALSE;
			}

			// ���ؼ��ɹ�
			return TRUE;
		}
	}
	catch (...)
	{
		// ����������Ϣ
		AfxMessageBox(_T("�޷������ݿ�!"));

		// ���ؼ��ʧ��
		return FALSE;
	}
	
	// Ĭ�Ϸ��ؼ��ʧ��
	return FALSE;
}


// ͳ��Ŀ�����ݿ�����
BOOL CSqliteManager::CountDataBase(CString DataBasePath, int &Count)
{
	Count = CountFile(DataBasePath);
	return Count;
}


// ����Ŀ�����ݿ�����
BOOL CSqliteManager::BackupDataBase(CString DataBasePath, CString BackupPath)
{
	return CopyFile(DataBasePath, BackupPath, FALSE);
}


// ��ԭĿ�����ݿ�����
BOOL CSqliteManager::RestoreDataBase(CString DataBasePath, CString RestorePath)
{
	return CopyFile(RestorePath, DataBasePath, FALSE);
}


// Զ������Ŀ�����ݿ�
BOOL CSqliteManager::RemoteDataBase(CString Name, CString Path)
{
	return true;
}


// ����Ŀ�����ݿ�����
BOOL CSqliteManager::EncryptionDataBase(CString DataBaseName, CString Password)
{
	return true;
}


// ����Ŀ�����ݿ�����
BOOL CSqliteManager::DecryptionDataBase(CString DataBaseName, CString Password)
{
	return true;
}


// �ϴ�Զ��Ŀ�����ݿ�
BOOL CSqliteManager::UploadDataBase(CString DataBasePath, CString UploadPath)
{
	return true;
}


// ����Զ��Ŀ�����ݿ�
BOOL CSqliteManager::DownloadDataBase(CString DataBasePath, CString DownloadPath)
{
	return true;
}


///////////////////////////////////���ݱ��������///////////////////////////////////


// ��ȡ���ݿ������б������
BOOL CSqliteManager::GetTableName(CString &TableName)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ش���ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// �����жϲ����Ƿ�ɹ�
		BOOL IsSuccess;

		// ����ִ�л�ȡ���ݱ�Ĳ���
		try
		{
			// ��ʼ��SQL���
			CStringA GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name";

			// ����ȡ����ʧ��
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���ػ�ȡʧ��
				IsSuccess = FALSE;
			}

			// ����ȡ�ɹ�
			else
			{
				// ������Ϊ��
				if (!stmt)
				{
					// ���ؿյı���
					TableName = _T("");

					// ���ػ�ȡʧ��
					IsSuccess = FALSE;
				}

				// �����ݲ�Ϊ��
				else
				{
					// ѭ����ñ���
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// �õ��ñ�������Լ�
						TableName += (CStringA)sqlite3_column_text(stmt, 0);

						// ���Ϸָ��������ֱ�
						TableName += _T(";");
					}

					// ���ػ�ȡ�ɹ�
					IsSuccess = TRUE;
				}
			}

			//�ͷŲ�ѯ���(���۲�ѯ�Ƿ�ɹ�)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// ����������Ϣ
			AfxMessageBox(_T("�޷������ݿ��л�ñ���"));

			// ���ؿյı���
			TableName = _T("");

			// ���ػ�ȡʧ��
			return FALSE;
		}

		// ���ز������
		return IsSuccess;
	}

	// Ĭ�Ϸ��ػ�ȡʧ��
	return FALSE;
}


// ��ȡ���ݿ������б������
BOOL CSqliteManager::GetDataTable(CString &DataTable)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ش���ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// �����жϲ����Ƿ�ɹ�
		BOOL IsSuccess;

		// ����ִ�л�ȡ���ݱ�Ĳ���
		try
		{
			// ��ʼ��SQL���
			CStringA GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name";

			// ����ȡ����ʧ��
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���ػ�ȡʧ��
				IsSuccess = FALSE;
			}

			// ����ȡ�ɹ�
			else
			{
				// ������Ϊ��
				if (!stmt)
				{
					// ���ؿյ�����
					DataTable = _T("");

					// ���ػ�ȡʧ��
					IsSuccess = FALSE;
				}

				// �����ݲ�Ϊ��
				else
				{
					// ѭ����ñ���
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// �õ��ñ����ݲ��Լ�
						DataTable += (CStringA)sqlite3_column_text(stmt, 1);

						// ���Ϸָ��������ֱ�
						DataTable += _T(";");
					}

					// ���ػ�ȡ�ɹ�
					IsSuccess = TRUE;
				}
			}

			//�ͷŲ�ѯ���(���۲�ѯ�Ƿ�ɹ�)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// ����������Ϣ
			AfxMessageBox(_T("�޷������ݿ��л�ñ���"));

			// ���ؿյ�����
			DataTable = _T("");

			// ���ػ�ȡʧ��
			return FALSE;
		}

		// ���ز������
		return IsSuccess;
	}

	// Ĭ�Ϸ��ػ�ȡʧ��
	return FALSE;
}


// ��ȡ���ݿ������ݱ����Ϣ
BOOL CSqliteManager::GetTableData(CString TableName, CString &SQL_Data)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ش���ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// �����жϲ����Ƿ�ɹ�
		BOOL IsSuccess;

		// ����ִ�л�ȡ���ݱ�Ĳ���
		try
		{
			// ��ʼ��SQL���
			CStringA GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name;";

			// ����ȡ����ʧ��
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���ػ�ȡʧ��
				IsSuccess = FALSE;
			}

			// ����ȡ�ɹ�
			else
			{
				// ������Ϊ��
				if (!stmt)
				{
					// ���ؿյ�����
					SQL_Data = _T("");

					// ���ػ�ȡʧ��
					IsSuccess = FALSE;
				}

				// �����ݲ�Ϊ��
				else
				{
					// ͨ�������õ�����
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						CStringA Name = (char *)sqlite3_column_text(stmt, 0);

						if (Name == (CStringA)TableName)
						{
							// �õ��ñ���䲢�Լ�
							SQL_Data = (char *)sqlite3_column_text(stmt, 1);

							// ���ػ�ȡ�ɹ�
							IsSuccess = TRUE;
						}
						else
						{
							// ���ػ�ȡʧ��
							IsSuccess = FALSE;
						}
					}
				}
			}

			//�ͷŲ�ѯ���(���۲�ѯ�Ƿ�ɹ�)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// ����������Ϣ
			AfxMessageBox(_T("�޷������ݱ��л������"));

			// ���ؿյ����
			SQL_Data = _T("");

			// ���ػ�ȡʧ��
			return FALSE;
		}

		// ���ز������
		return IsSuccess;
	}

	// Ĭ�Ϸ��ػ�ȡʧ��
	return FALSE;
}


// ������ݱ��������е�����
BOOL CSqliteManager::GetColName(CString TableName, CString &ColName)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ش���ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// �����жϲ����Ƿ�ɹ�
		BOOL IsSuccess = FALSE;

		// ����ִ�л�ȡ���ݱ��������Ĳ���
		try
		{
			// ��ʼ��SQL���
			CStringA GetSQL = "SELECT * FROM " + (CStringA)TableName + " limit 0,1";

			// ����ȡ����ʧ��
			if (sqlite3_get_table(db, GetSQL, &pRes, &nRow, &nCol, &errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���ػ�ȡʧ��
				IsSuccess = FALSE;
			}

			// ����ȡ�ɹ�
			else
			{
				// ������Ϊ��
				if (!nRow)
				{
					// ���ؿյ�����
					ColName = _T("");

					// ���ػ�ȡʧ��
					IsSuccess = FALSE;
				}

				// �����ݲ�Ϊ��
				else
				{
					// ѭ���������
					for (int i = 0; i < nRow; i++)
					{
						for (int j = 0; j < nCol; j++)
						{
							char *pv = *(pRes + nCol*i + j);

							// �����Լ�
							ColName += pv;

							// ���Ϸָ�����������
							ColName += _T(";");
						}
						break;
					}

					// �ͷ���Դ
					if (errMsg != NULL)
					{
						sqlite3_free(errMsg);
					}

					// ���ػ�ȡ�ɹ�
					IsSuccess = TRUE;
				}
			}

			//�ͷŲ�ѯ���(���۲�ѯ�Ƿ�ɹ�)
			sqlite3_free_table(pRes);
			pRes = NULL;
		}
		catch (...)
		{
			// ����������Ϣ
			AfxMessageBox(_T("�޷������ݿ��л�ñ���"));

			// ���ؿյ�����
			ColName = _T("");

			// ���ػ�ȡʧ��
			return FALSE;
		}

		// ���ز������
		return IsSuccess;
	}

	// Ĭ�Ϸ��ػ�ȡʧ��
	return FALSE;
}


// ������ݱ��������е�����
BOOL CSqliteManager::GetColType(CString TableName, CString &ColType)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ش���ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// �����жϲ����Ƿ�ɹ�
		BOOL IsSuccess = FALSE;

		// ����ִ�л�ȡ���ݱ�Ĳ���
		try
		{
			// ��ʼ��SQL���
			CStringA GetSQL = "SELECT * FROM " + (CStringA)TableName + " limit 0,1";

			// ����ȡ����ʧ��
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���ػ�ȡʧ��
				IsSuccess = FALSE;
			}

			// ����ȡ�ɹ�
			else
			{
				// ������Ϊ��
				if (!stmt)
				{
					// ���ؿյ�����
					ColType = _T("");

					// ���ػ�ȡʧ��
					IsSuccess = FALSE;
				}

				// �����ݲ�Ϊ��
				else
				{
					// ѭ���������
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// �������
						int nCount = sqlite3_column_count(stmt);

						for (int i = 0; i < nCount; i++)
						{
							int nValue = sqlite3_column_int(stmt, 0);
							int nType = sqlite3_column_type(stmt, i);

							// �ж�����
							switch (nType)
							{
							case 1:
								// INTEGER ����
								ColType += _T("SQLITE_INTEGER");

								// ���Ϸָ�������������
								ColType += _T(";");
								break;
							case 2:
								// FLOAT ����
								ColType += _T("SQLITE_FLOAT");

								// ���Ϸָ�������������
								ColType += _T(";");
								break;
							case 3:
								// TEXT ����
								ColType += _T("SQLITE_TEXT");

								// ���Ϸָ�������������
								ColType += _T(";");
								break;
							case 4:
								// BLOB ����
								ColType += _T("SQLITE_BLOB");

								// ���Ϸָ�������������
								ColType += _T(";");
								break;
							case 5:
								// NULL ����
								ColType += _T("SQLITE_NULL");

								// ���Ϸָ�������������
								ColType += _T(";");
								break;
							}
						}
						break;
					}

					// ���ػ�ȡ�ɹ�
					IsSuccess = TRUE;
				}
			}

			//�ͷŲ�ѯ���(���۲�ѯ�Ƿ�ɹ�)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// ����������Ϣ
			AfxMessageBox(_T("�޷������ݱ��л������"));

			// ���ؿյ�������
			TableName = _T("");

			// ���ػ�ȡʧ��
			return FALSE;
		}

		// ���ز������
		return IsSuccess;
	}

	// Ĭ�Ϸ��ػ�ȡʧ��
	return FALSE;
}


// ��Ŀ�����ݿ��д������ݱ�
BOOL CSqliteManager::CreateDataTable(CString TableName, CString Params)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ش���ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ����ִ�д������ݱ�Ĳ���
		try
		{
			// ��ʼ��SQL���
			CStringA CreateSQL = "CREATE TABLE IF Not Exists " + (CStringA)TableName + " ( " + (CStringA)Params + " );";

			//���ݿ�����һ������
			if (!transaction(db))
			{
				AfxMessageBox(_T("�޷�����������!"));
				return false;
			}

			// ����������ʧ��
			if (sqlite3_exec(db, CreateSQL, NULL, NULL, NULL) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���ش���ʧ��
				return FALSE;
			}

			//�ύ����
			if (commitTransaction(db) == false)
			{
				// �ع�����
				rollbackTransaction(db);

				// ��ʾ������Ϣ
				AfxMessageBox(_T("�������ݱ�ʧ��: ") + (CString)sqlite3_errmsg(db));

				// ���ش���ʧ��
				return FALSE;
			}

			// ���ش����ɹ�
			else
				return TRUE;
		}
		catch (...)
		{
			// ��ʾ������Ϣ
			AfxMessageBox(_T("�������ݱ�ʧ��!"));

			// ���ش���ʧ��
			return FALSE;
		}
	}

	// Ĭ�Ϸ��ش���ʧ��
	return FALSE;
}


// ��Ŀ�����ݿ���ɾ�����ݱ�
BOOL CSqliteManager::DeleteDataTable(CString TableName)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ����ɾ��ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ����ִ��ɾ�����ݱ�Ĳ���
		try
		{
			// ��ʼ��SQL���
			CStringA DeleteSQL = "Drop TABLE " + (CStringA)TableName + ";";

			//���ݿ�����һ������
			if (!transaction(db))
			{
				AfxMessageBox(_T("�޷�����������!"));
				return false;
			}

			// ��ɾ������ʧ��
			if (sqlite3_exec(db, DeleteSQL, NULL, NULL, NULL) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ����ɾ��ʧ����Ϣ
				return FALSE;
			}

			//�ύ����
			if (commitTransaction(db) == false)
			{
				// �ع�����
				rollbackTransaction(db);

				// ��ʾ������Ϣ
				AfxMessageBox(_T("ɾ�����ݱ�ʧ��: ") + (CString)sqlite3_errmsg(db));

				// ����ɾ��ʧ����Ϣ
				return FALSE;
			}

			// ����ɾ���ɹ���Ϣ
			else
				return TRUE;
		}
		catch (...)
		{
			// ��ʾ������Ϣ��Ϣ
			AfxMessageBox(_T("ɾ�����ݱ�ʧ��!"));

			// ����ɾ��ʧ����Ϣ
			return FALSE;
		}
	}

	// Ĭ�Ϸ���ɾ��ʧ����Ϣ
	return FALSE;
}


// ��Ŀ�����ݿ����޸����ݱ�
BOOL CSqliteManager::UpdataDataTable(CString TableName, int Operation, CString Params, CString NewParams)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// �����޸�ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ���ݲ������Բ�ͬSQL����
		switch (Operation)
		{
		// ���������ݱ�
		case REN_TABLE:
		{
			try
			{
				// ��ʼ��SQL���
				CStringA ReNameTableSQL = "ALTER TABLE " + (CStringA)TableName + " RENAME TO " + (CStringA)Params + ";";

				//���ݿ�����һ������
				if (!transaction(db))
				{
					AfxMessageBox(_T("�޷�����������!"));
					return false;
				}

				// ����Ӳ���ʧ��
				if (sqlite3_exec(db, ReNameTableSQL, NULL, NULL, NULL) != SQLITE_OK)
				{
					// ��ʾ������Ϣ
					::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

					// ����������ʧ��
					return FALSE;
				}

				//�ύ����
				if (commitTransaction(db) == false)
				{
					// �ع�����
					rollbackTransaction(db);

					// ��ʾ������Ϣ
					AfxMessageBox(_T("���������ݱ�ʧ��: ") + (CString)sqlite3_errmsg(db));

					// ����������ʧ��
					return FALSE;
				}

				// �����������ɹ�
				else
					return TRUE;
			}
			catch (...)
			{
				// ��ʾ������Ϣ
				AfxMessageBox(_T("���������ݱ�ʧ��!"));

				// ����������ʧ��
				return FALSE;
			}

		}break;

		// �����
		case ADD_COLUMN:
		{
			try
			{
				// ��ʼ��SQL���
				CStringA Add_Column_SQL = "ALTER TABLE " + (CStringA)TableName + " ADD COLUMN " + (CStringA)Params + ";";

				//���ݿ�����һ������
				if (!transaction(db))
				{
					AfxMessageBox(_T("�޷�����������!"));
					return false;
				}

				// ����Ӳ���ʧ��
				if (sqlite3_exec(db, Add_Column_SQL, NULL, NULL, NULL) != SQLITE_OK)
				{
					// ��ʾ������Ϣ
					::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

					// �������ʧ��
					return FALSE;
				}

				//�ύ����
				if (commitTransaction(db) == false)
				{
					// �ع�����
					rollbackTransaction(db);

					// ��ʾ������Ϣ
					AfxMessageBox(_T("�����ݱ��������ʧ��: ") + (CString)sqlite3_errmsg(db));

					// �������ʧ��
					return FALSE;
				}

				// ������ӳɹ�
				else
					return TRUE;
			}
			catch (...)
			{
				// ��ʾ������Ϣ
				AfxMessageBox(_T("�����ݱ��������ʧ��!"));

				// �������ʧ��
				return FALSE;
			}

		}break;

		// ɾ����
		case DEL_COLUMN:
		{
			try
			{
				// �õ���������
				CString ColumnName;
				GetColName(TableName, ColumnName);

				// ���Ҳ���Ŀ����
				if (!ColumnName.Replace(Params + _T(";"), _T("")))
				{
					// ��ʾ������Ϣ
					AfxMessageBox(_T("�Ҳ���Ŀ���У�ɾ��ʧ��!"));

					// ����ɾ��ʧ��
					return FALSE;

				}
				else
				{
					// ���޷�����Ŀ���е�����
					if (!ColumnName.Replace(_T(";"), _T(",")))
					{
						// ��ʾ������Ϣ
						AfxMessageBox(_T("�޷�����Ŀ���У�ɾ��ʧ��!"));

						// ����ɾ��ʧ��
						return FALSE;
					}
					else
					{
						// �ж�ĩβ�Ƿ�Ϊ , 
						if (ColumnName.Right(1) == _T(","))
						{
							// ɾ��ĩβ�� ,
							ColumnName = ColumnName.Left(ColumnName.GetLength() - 1);
						}

						// ��ʼ��SQL���
						CStringA Cre_Column_SQL = "Create Table Temp As Select "+ (CStringA)ColumnName +" From " + (CStringA)TableName + ";";

						//���ݿ�����һ������
						if (!transaction(db))
						{
							AfxMessageBox(_T("�޷�����������!"));
							return false;
						}

						// ��ɾ������ʧ��
						if (sqlite3_exec(db, Cre_Column_SQL, NULL, NULL, NULL) != SQLITE_OK)
						{
							// ��ʾ������Ϣ
							::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

							// ����ɾ��ʧ��
							return FALSE;
						}

						//�ύ����
						if (commitTransaction(db) == false)
						{
							// �ع�����
							rollbackTransaction(db);

							// ��ʾ������Ϣ
							AfxMessageBox(_T("�����ݱ���ɾ����ʧ��: ") + (CString)sqlite3_errmsg(db));

							// ����ɾ��ʧ��
							return FALSE;
						}
						else
						{
							// ɾ��ԭ���ݱ�
							if (!DeleteDataTable(TableName))
							{
								// ��ʾ������Ϣ
								::MessageBox(NULL, _T("�޷�ɾ��ԭ���ݱ�!"), NULL, NULL);

								// ����ɾ��ʧ��
								return FALSE;
							}
							else
							{
								// �������� Temp ��ΪĿ�����ݱ�ʧ��
								if (!UpdataDataTable(_T("Temp"), REN_TABLE, TableName))
								{
									// ��ʾ������Ϣ
									::MessageBox(NULL, _T("�޷�ɾ��ԭ���ݱ�!"), NULL, NULL);

									// ����ɾ��ʧ��
									return FALSE;
								}
								else
								{
									// ����ɾ��Ŀ���гɹ�
									return TRUE;
								}
							}
						}
					}
				}
			}
			catch (...)
			{
				// ��ʾ������Ϣ
				AfxMessageBox(_T("�����ݱ���ɾ����ʧ��!"));

				// ����ɾ��ʧ��
				return FALSE;
			}

		}break;

		// ��������
		case REN_COLUMN:
		{
			try
			{
				// �õ�Ŀ�����ݱ��SQL����
				CString Sql_Data;
				GetTableData(TableName, Sql_Data);

				// ���Ҳ���Ŀ���
				if (!Sql_Data.Replace(TableName, _T("Temp")))
				{
					// ��ʾ������Ϣ
					AfxMessageBox(_T("�Ҳ���Ŀ����޸�ʧ��!"));

					// �����޸�ʧ��
					return FALSE;
				}
				else
				{
					// ���Ҳ���Ŀ����
					if (!Sql_Data.Replace(Params, NewParams))
					{
						// ��ʾ������Ϣ
						AfxMessageBox(_T("�Ҳ���Ŀ���У��޸�ʧ��!"));

						// �����޸�ʧ��
						return FALSE;
					}

					//���ݿ�����һ������
					if (!transaction(db))
					{
						AfxMessageBox(_T("�޷�����������!"));
						return false;
					}

					// ��������ʱ�� Temp ʧ��
					if (sqlite3_exec(db, (CStringA)Sql_Data, NULL, NULL, NULL) != SQLITE_OK)
					{
						// ��ʾ������Ϣ
						::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

						// �����޸�ʧ��
						return FALSE;
					}

					//�ύ����
					if (commitTransaction(db) == false)
					{
						// �ع�����
						rollbackTransaction(db);

						// ��ʾ������Ϣ
						AfxMessageBox(_T("�޷�������ԭ���ݱ�: ") + (CString)sqlite3_errmsg(db));

						// �����޸�ʧ��
						return FALSE;
					}
					else
					{
						// ����ԭ������
						CStringA Insert_Sql = "INSERT INTO Temp SELECT * FROM " + (CStringA)TableName + ";";

						//���ݿ�����һ������
						if (!transaction(db))
						{
							AfxMessageBox(_T("�޷�����������!"));
							return false;
						}

						// ���������ʧ��
						if (sqlite3_exec(db, Insert_Sql, NULL, NULL, NULL) != SQLITE_OK)
						{
							// ��ʾ������Ϣ
							::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

							// �����޸�ʧ��
							return FALSE;
						}
						else
						{
							// ɾ��ԭ���ݱ�
							if (!DeleteDataTable(TableName))
							{
								// ��ʾ������Ϣ
								::MessageBox(NULL, _T("�޷�ɾ��ԭ���ݱ�!"), NULL, NULL);

								// �����޸�ʧ��
								return FALSE;
							}
							else
							{
								// �������� Temp ��ΪĿ�����ݱ�ʧ��
								if (!UpdataDataTable(_T("Temp"), REN_TABLE, TableName))
								{
									// ��ʾ������Ϣ
									::MessageBox(NULL, _T("�޷�������ԭ���ݱ�!"), NULL, NULL);

									// �����޸�ʧ��
									return FALSE;
								}
								else
								{
									// �����޸��гɹ�
									return TRUE;
								}
							}
						}
					}
				}
			}
			catch (...)
			{
				// ��ʾ������Ϣ
				AfxMessageBox(_T("�����ݱ����޸���ʧ��!"));

				// �����޸�ʧ��
				return FALSE;
			}

		}break;

		// Ĭ��ʲôҲ����
		default:
		{
			AfxMessageBox(_T("��Ч���������SQL�޲���!"));

		}break;
		}
	}

	// Ĭ�Ϸ����޸�ʧ����Ϣ
	return FALSE;
}


// ��Ŀ�����ݿ��м�����ݱ�     ( ������ݱ��Ƿ���� )
BOOL CSqliteManager::CheckDataTable (CString TableName)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ؼ��ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ����ִ�м�����ݱ�Ĳ���
		try
		{
			// ����ȶԱ���
			CString CheckName;
			if (!GetTableName(CheckName))
			{
				// ����������Ϣ
				AfxMessageBox(_T("�޷������ݿ��л�ñ���"));

				// ���ؼ��ʧ��
				return FALSE;
			}
			else
			{
				// �ȶ��Ƿ����Ŀ�����ݱ�
				if (CheckName.Replace(TableName, _T("")))
				{
					// ���ش���Ŀ�����ݱ�
					return TRUE;
				}
				else
				{
					// ���ز�����Ŀ�����ݱ�
					return FALSE;
				}
			}
		}
		catch (...)
		{
			AfxMessageBox(_T("�޷������ݿ��л�ñ���"));

			// ���ؼ��ʧ��
			return FALSE;
		}
	}

	// Ĭ�Ϸ��ؼ��ʧ����Ϣ
	return FALSE;
}


// ��Ŀ�����ݿ���ͳ�����ݱ�     ( ͳ�����ݱ��ܹ����� )
BOOL CSqliteManager::CountDataTable (int &Count)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ش���ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// �����жϲ����Ƿ�ɹ�
		BOOL IsSuccess;

		// ��ʼ������
		Count = 0;


		// ����ִ�л�ȡ���ݱ�Ĳ���
		try
		{
			// ��ʼ��SQL���
			CStringA GetSQL = "Select name,sql From sqlite_master Where type = 'table' Order By name";

			// ����ȡ����ʧ��
			if (sqlite3_prepare(db, GetSQL, -1, &stmt, (const char**)errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���ػ�ȡʧ��
				IsSuccess = FALSE;
			}

			// ����ȡ�ɹ�
			else
			{
				// ������Ϊ��
				if (!stmt)
				{
					// ���� 0 ������
					Count = 0;

					// ���ػ�ȡʧ��
					IsSuccess = FALSE;
				}

				// �����ݲ�Ϊ��
				else
				{
					// ѭ��������ݱ�
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						// ���������Լ�1
						Count ++;
					}

					// ���ػ�ȡ�ɹ�
					IsSuccess = TRUE;
				}
			}

			//�ͷŲ�ѯ���(���۲�ѯ�Ƿ�ɹ�)
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		catch (...)
		{
			// ����������Ϣ
			AfxMessageBox(_T("�޷������ݿ��л�ñ���"));

			// ���� 0 ������
			Count = 0;

			// ���ػ�ȡʧ��
			return FALSE;
		}

		// ���ز������
		return IsSuccess;
	}

	// Ĭ�Ϸ��ػ�ȡʧ��
	return FALSE;
}


// ��Ŀ�����ݿ��е������ݱ�
BOOL CSqliteManager::ImportDataTable(CString TableName, CString TargetDataBase)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ش���ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// �����жϲ����Ƿ�ɹ�
		//BOOL IsSuccess;

		sqlite3 *pTarget;

		// ����Ŀ�����ݿ��Ƿ����
		CFileFind Finder;
		BOOL Status = Finder.FindFile(TargetDataBase);

		// ������Ŀ�����ݿ���������Ŀ�����ݿ�
		if (Status)
		{
			// ��������Ŀ�����ݿ�
			try
			{
				// ��������Ŀ�����ݿ�
				if (sqlite3_open((CStringA)TargetDataBase, &pTarget) != SQLITE_OK) // ���޷��������ݿ�
				{
					// ����������Ϣ
					::MessageBox(NULL, (CString)sqlite3_errmsg(pTarget), NULL, NULL);

					// ���ر����ݿ�ʧ��
					if (sqlite3_close(pTarget) != SQLITE_OK)
					{
						// ��ʾ������Ϣ
						::MessageBox(NULL, (CString)sqlite3_errmsg(pTarget), NULL, NULL);

						// ���عر�ʧ����Ϣ
						return FALSE;
					}

					// �������ݿ�����ʧ�ܵ���Ϣ
					return FALSE;
				}

				// �����ݿ�ɹ�����
				else
				{
					try
					{
						// ������ݱ��Ƿ����
						if (CheckDataTable(TableName))
						{
							//���ݿ�����һ������
							if (!transaction(db))
							{
								AfxMessageBox(_T("�޷�����������!"));
								return false;
							}

							// ��ʼ��SQL���
							CStringA InsertSQL;//"INSERT INTO " + (CStringA)TableName + " VALUES( " + (CStringA)Params + " );";

							// ����Ӳ���ʧ��
							if (sqlite3_exec(db, InsertSQL, NULL, NULL, &errMsg) != SQLITE_OK)
							{
								// ��ʾ������Ϣ
								::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

								// �������ʧ����Ϣ
								return FALSE;
							}

							//�ύ����
							if (commitTransaction(db) == false)
							{
								// �ع�����
								rollbackTransaction(db);

								// ��ʾ������Ϣ
								AfxMessageBox(_T("������ݲ���ʧ��: ") + (CString)sqlite3_errmsg(db));

								// �������ʧ����Ϣ
								return FALSE;
							}

						}
						else
						{
							// ����������Ϣ
							::MessageBox(NULL, _T("Ŀ�����ݱ�����,�޷���������!"), NULL, NULL);

							// �������ݵ���ʧ�ܵ���Ϣ
							return FALSE;
						}
					}
					// ��׽����
					catch (...)
					{
						// ����������Ϣ
						::MessageBox(NULL, _T("�޷��������ݵ�Ŀ�����ݿ�!"), NULL, NULL);

						// �������ݵ���ʧ�ܵ���Ϣ
						return FALSE;
					}
				}
			}
			// ��׽����
			catch (...)
			{
				// ����������Ϣ
				::MessageBox(NULL, _T("�޷����ӵ�Ŀ�����ݿ�!"), NULL, NULL);

				// �������ݿ�����ʧ�ܵ���Ϣ
				return FALSE;
			}
		}
		else
		{
			// ����������Ϣ
			::MessageBox(NULL, _T("�޷����ӵ�Ŀ�����ݿ�, ��ΪĿ�����ݿⲻ����!"), NULL, NULL);

			// �������ݿ�����ʧ�ܵ���Ϣ
			return FALSE;
		}

		// Ĭ�Ϸ��ص���ʧ��
		return FALSE;
	}
}


// ��Ŀ�����ݿ��е������ݱ�
BOOL CSqliteManager::ExportDataTable(CString TableName, CString TargetDataBase)
{
	return true;
}


///////////////////////////////////�������������/////////////////////////////////////


// ��Ŀ�����ݱ������������
BOOL CSqliteManager::InsertData(CString TableName, CString Params)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// �������ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ����ִ��������ݵĲ���
		try
		{
			// ��ʼ��SQL���
			CStringA InsertSQL = "INSERT INTO " + (CStringA)TableName + " VALUES( " + (CStringA)Params + " );";

			//���ݿ�����һ������
			if (!transaction(db))
			{
				AfxMessageBox(_T("�޷�����������!"));
				return false;
			}

			// ����Ӳ���ʧ��
			if (sqlite3_exec(db, InsertSQL, NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// �������ʧ����Ϣ
				return FALSE;
			}

			//�ύ����
			if (commitTransaction(db) == false)
			{
				// �ع�����
				rollbackTransaction(db);

				// ��ʾ������Ϣ
				AfxMessageBox(_T("������ݲ���ʧ��: ") + (CString)sqlite3_errmsg(db));

				// �������ʧ����Ϣ
				return FALSE;
			}

			// ������ӳɹ���Ϣ
			else
				return TRUE;
		}
		catch (...)
		{
			// ��ʾ������Ϣ
			AfxMessageBox(_T("������ݲ���ʧ��!"));

			// �������ʧ����Ϣ
			return FALSE;
		}
	}

	// Ĭ�Ϸ������ʧ����Ϣ
	return FALSE;
}


// ��Ŀ�����ݱ���ɾ��������
BOOL CSqliteManager::DeleteData(CString TableName, CString Params)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ����ɾ��ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ����ִ�д����ݱ���ɾ�����ݵĲ���
		try
		{
			// ��ʼ��SQL���
			CStringA DeleteSQL;
			if (!Params.IsEmpty())
				DeleteSQL = "Delete From " + (CStringA)TableName + " Where " + (CStringA)Params + ";";
			else
				DeleteSQL = "Delete From " + (CStringA)TableName + ";";

			//���ݿ�����һ������
			if (!transaction(db))
			{
				AfxMessageBox(_T("�޷�����������!"));
				return false;
			}

			// ��ɾ������ʧ��
			if (sqlite3_exec(db, DeleteSQL, NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ����ɾ��ʧ��
				return FALSE;
			}

			//�ύ����
			if (commitTransaction(db) == false)
			{
				// �ع�����
				rollbackTransaction(db);

				// ��ʾ������Ϣ
				AfxMessageBox(_T("������ݲ���ʧ��: ") + (CString)sqlite3_errmsg(db));

				// ����ɾ��ʧ��
				return FALSE;
			}

			// ����ɾ���ɹ�
			else
				return TRUE;
		}
		catch (...)
		{
			// ��ʾ������Ϣ
			AfxMessageBox(_T("ɾ������ʧ��!"));

			// ����ɾ��ʧ��
			return FALSE;
		}
	}
}


// ��Ŀ�����ݱ����޸�������
BOOL CSqliteManager::UpdataData(CString TableName, CString Column, CString NewData, CString Params)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// �����޸�ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ����ִ���޸����ݵĲ���
		try
		{
			// ��ʼ��SQL���
			CStringA UpData_SQL;

			// ������Ϊ��
			if (Params.IsEmpty())
			{
				UpData_SQL = _T("UPDATE ") + TableName + _T(" SET ") + Column + _T(" = ") + _T("'") + NewData + _T("'") + _T(";");
			}
			else
			{
				UpData_SQL = _T("UPDATE ") + TableName + _T(" SET ") + Column + _T(" = ") + _T("'") + NewData + _T("'") + _T(" Where ") + Params + _T(";");
			}
			
			//���ݿ�����һ������
			if (!transaction(db))
			{
				AfxMessageBox(_T("�޷�����������!"));
				return false;
			}

			// ���޸Ĳ���ʧ��
			if (sqlite3_exec(db, UpData_SQL, NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// �����޸�ʧ����Ϣ
				return FALSE;
			}

			//�ύ����
			if (commitTransaction(db) == false)
			{
				// �ع�����
				rollbackTransaction(db);

				// ��ʾ������Ϣ
				AfxMessageBox(_T("�޸����ݲ���ʧ��: ") + (CString)sqlite3_errmsg(db));

				// �����޸�ʧ����Ϣ
				return FALSE;
			}

			// �����޸ĳɹ���Ϣ
			else
				return TRUE;
		}
		catch (...)
		{
			// ��ʾ������Ϣ
			AfxMessageBox(_T("�޸����ݲ���ʧ��!"));

			// �����޸�ʧ����Ϣ
			return FALSE;
		}
	}

	// Ĭ�Ϸ����޸�ʧ����Ϣ
	return FALSE;
}


// ��Ŀ�����ݱ����޸�������
BOOL CSqliteManager::UpdataData(CString TableName, CString ColumnParams, CString Params)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// �����޸�ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ����ִ���޸����ݵĲ���
		try
		{
			// ��ʼ��SQL���
			CStringA UpData_SQL;

			// ������Ϊ��
			if (Params.IsEmpty())
			{
				UpData_SQL = _T("UPDATE ") + TableName + _T(" SET ") + ColumnParams + _T(";");
			}
			else
			{
				UpData_SQL = _T("UPDATE ") + TableName + _T(" SET ") + ColumnParams + _T(" Where ") + Params + _T(";");
			}

			//���ݿ�����һ������
			if (!transaction(db))
			{
				AfxMessageBox(_T("�޷�����������!"));
				return false;
			}

			// ���޸Ĳ���ʧ��
			if (sqlite3_exec(db, UpData_SQL, NULL, NULL, &errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// �����޸�ʧ����Ϣ
				return FALSE;
			}

			//�ύ����
			if (commitTransaction(db) == false)
			{
				// �ع�����
				rollbackTransaction(db);

				// ��ʾ������Ϣ
				AfxMessageBox(_T("�޸����ݲ���ʧ��: ") + (CString)sqlite3_errmsg(db));

				// �����޸�ʧ����Ϣ
				return FALSE;
			}

			// �����޸ĳɹ���Ϣ
			else
				return TRUE;
		}
		catch (...)
		{
			// ��ʾ������Ϣ
			AfxMessageBox(_T("�޸����ݲ���ʧ��!"));

			// �����޸�ʧ����Ϣ
			return FALSE;
		}
	}

	// Ĭ�Ϸ����޸�ʧ����Ϣ
	return FALSE;
}


// ��Ŀ�����ݱ��в�ѯ������
BOOL CSqliteManager::SelectData(CString TableName, vector<CString> &pResult,  BOOL DISTINCT, CString COUNT, CString COLUMN, CString Params, CString GROUP, CString HAVING, CString Order, CString Limit, int SortMode)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ز�ѯʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ����ִ�в�ѯ���ݵĲ���
		try
		{
			// ���ԭ�ȵĽ����
			pResult.clear();

			// �����ѯ����
			int rc, i, ncols;
			const char *tail;

			// ��ʼ��SQL���
			CStringA Select_SQL;
			CString  DISTINCT_Text = _T("");

			CString  SortText = _T("ASC");

			// ��������Ϊ��
			if (COLUMN.IsEmpty())
			{
				if (DISTINCT)
				{
					// ��ʾ����
					AfxMessageBox(_T("��Ϊ�յ������, DISTINCT ������Ϊ��"));
					return FALSE;
				}
				if (!COUNT.IsEmpty())
				{
					// ��ʾ����
					AfxMessageBox(_T("��Ϊ�յ������, COUNT �����Բ�Ϊ��"));
					return FALSE;
				}

				COLUMN = _T("*");
			}
			else
			{
				// ��ȥ���ظ�Ϊ��
				if (DISTINCT)
				{
					DISTINCT_Text = _T("DISTINCT ");
				}

				// ��������Ϊ��
				if (!COUNT.IsEmpty())
				{
					COUNT = _T(", ") + COUNT;
				}
			}

			// ������Ϊ��
			if (Params.IsEmpty())
			{
				if (!GROUP.IsEmpty())
				{
					// ��ʾ����
					AfxMessageBox(_T("����Ϊ�յ������, ���鲻���Բ�Ϊ��"));
					return FALSE;
				}

				if (!HAVING.IsEmpty())
				{
					// ��ʾ����
					AfxMessageBox(_T("����Ϊ�յ������, ɸѡ�����Բ�Ϊ��"));
					return FALSE;
				}

				if (!Order.IsEmpty())
				{
					// ��ʾ����
					AfxMessageBox(_T("����Ϊ�յ������, ���򲻿��Բ�Ϊ��"));
					return FALSE;
				}

				if (!Limit.IsEmpty())
				{
					// ��ʾ����
					AfxMessageBox(_T("����Ϊ�յ������, ƫ���������Բ�Ϊ��"));
					return FALSE;
				}
			}
			else
			{
				// �����鲻Ϊ��
				if (!GROUP.IsEmpty())
				{
					GROUP = _T(" GROUP By ") + GROUP;
				}

				// ��ɸѡ��Ϊ��
				if (!HAVING.IsEmpty())
				{
					HAVING = _T(" HAVING ") + HAVING;
				}

				// ������Ϊ��
				if (!Order.IsEmpty())
				{
					Order = _T(" Order by ") + Order;

					// ���ݴ����������
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

				// ��ƫ������Ϊ��
				if (!Limit.IsEmpty())
				{
					Limit = _T(" Limit ") + Limit;
				}
			}
			
			
			// �����ѯSQL
			Select_SQL = _T("SELECT ") + DISTINCT_Text + COLUMN + COUNT + _T(" FROM ") + TableName + _T(" Where ") + Params + GROUP + HAVING + Order + SortText + Limit + _T(";");

			// ��Ԥ��������ɹ�
			if (sqlite3_prepare(db, Select_SQL, (int)strlen(Select_SQL), &stmt, &tail) == SQLITE_OK)
			{
				rc    = sqlite3_step(stmt);
				ncols = sqlite3_column_count(stmt);
				while (rc == SQLITE_ROW)
				{
					for (i = 0; i < ncols; i++)
					{
						// ѭ������ѯ�����������
						pResult.push_back((CString)sqlite3_column_text(stmt, i));
					}
					
					rc = sqlite3_step(stmt);
				}

				//�ͷ�statement
				sqlite3_finalize(stmt);

				// ���ز�ѯ�ɹ���Ϣ
				return TRUE;
			}
			else
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���ز�ѯʧ����Ϣ
				return FALSE;
			}

			// �ͷű���
			sqlite3_finalize(stmt);
		}
		catch (...)
		{
			// ��ʾ������Ϣ
			AfxMessageBox(_T("��ѯ���ݲ���ʧ��!"));

			// ���ز�ѯʧ����Ϣ
			return FALSE;
		}
	}

	// Ĭ�Ϸ��ز�ѯʧ����Ϣ
	return FALSE;
}


// ��Ŀ�����ݱ��в�ѯ������
BOOL CSqliteManager::SelectData(CString TableName, CString &pResult, int Col, BOOL DISTINCT, CString COUNT, CString COLUMN, CString Params, CString GROUP, CString HAVING, CString Order, CString Limit, int SortMode)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ز�ѯʧ����Ϣ
		return FALSE;
	}
	else
	{
		// ����ִ�в�ѯ���ݵĲ���
		try
		{
			// ���ԭ�ȵĽ��
			pResult = _T("");

			// �����ѯ����
			int rc, i, ncols;
			const char *tail;

			// ��ʼ��SQL���
			CStringA Select_SQL;
			CString  DISTINCT_Text = _T("");

			CString  SortText = _T("ASC");

			// ��������Ϊ��
			if (COLUMN.IsEmpty())
			{
				if (DISTINCT)
				{
					// ��ʾ����
					AfxMessageBox(_T("��Ϊ�յ������, DISTINCT ������Ϊ��"));
					return FALSE;
				}
				if (!COUNT.IsEmpty())
				{
					// ��ʾ����
					AfxMessageBox(_T("��Ϊ�յ������, COUNT �����Բ�Ϊ��"));
					return FALSE;
				}

				COLUMN = _T("*");
			}
			else
			{
				// ��ȥ���ظ�Ϊ��
				if (DISTINCT)
				{
					DISTINCT_Text = _T("DISTINCT ");
				}

				// ��������Ϊ��
				if (!COUNT.IsEmpty())
				{
					COUNT = _T(", ") + COUNT;
				}
			}

			// ������Ϊ��
			if (Params.IsEmpty())
			{
				if (!GROUP.IsEmpty())
				{
					// ��ʾ����
					AfxMessageBox(_T("����Ϊ�յ������, ���鲻���Բ�Ϊ��"));
					return FALSE;
				}

				if (!HAVING.IsEmpty())
				{
					// ��ʾ����
					AfxMessageBox(_T("����Ϊ�յ������, ɸѡ�����Բ�Ϊ��"));
					return FALSE;
				}

				if (!Order.IsEmpty())
				{
					// ��ʾ����
					AfxMessageBox(_T("����Ϊ�յ������, ���򲻿��Բ�Ϊ��"));
					return FALSE;
				}

				if (!Limit.IsEmpty())
				{
					// ��ʾ����
					AfxMessageBox(_T("����Ϊ�յ������, ƫ���������Բ�Ϊ��"));
					return FALSE;
				}
			}
			else
			{
				// �����鲻Ϊ��
				if (!GROUP.IsEmpty())
				{
					GROUP = _T(" GROUP By ") + GROUP;
				}

				// ��ɸѡ��Ϊ��
				if (!HAVING.IsEmpty())
				{
					HAVING = _T(" HAVING ") + HAVING;
				}

				// ������Ϊ��
				if (!Order.IsEmpty())
				{
					Order = _T(" Order by ") + Order;

					// ���ݴ����������
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

				// ��ƫ������Ϊ��
				if (!Limit.IsEmpty())
				{
					Limit = _T(" Limit ") + Limit;
				}
			}

			// �����ѯSQL
			Select_SQL = _T("SELECT ") + DISTINCT_Text + COLUMN + COUNT + _T(" FROM ") + TableName + _T(" Where ") + Params + GROUP + HAVING + Order + SortText + Limit + _T(";");

			// ��Ԥ��������ɹ�
			if (sqlite3_prepare(db, Select_SQL, (int)strlen(Select_SQL), &stmt, &tail) == SQLITE_OK)
			{
				// r��Ŀ����Ϊ0
				if (Col == 0)
					Col = 1;

				// ִ��Ԥ����
				rc = sqlite3_step(stmt);
				ncols = sqlite3_column_count(stmt);
				while (rc == SQLITE_ROW)
				{
					// ѭ���õ���ѯ���
					for (i = 0; i < ncols; i++)
					{
						// ��Ŀ������i���
						if (i == Col)
						{
							// �õ�Ŀ������
							pResult = (CString)sqlite3_column_text(stmt, i);
						}
					}

					rc = sqlite3_step(stmt);
				}

				//�ͷ�statement
				sqlite3_finalize(stmt);

				// ���ز�ѯ�ɹ���Ϣ
				return TRUE;
			}
			else
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���ز�ѯʧ����Ϣ
				return FALSE;
			}

			// �ͷű���
			sqlite3_finalize(stmt);
		}
		catch (...)
		{
			// ��ʾ������Ϣ
			AfxMessageBox(_T("��ѯ���ݲ���ʧ��!"));

			// ���ز�ѯʧ����Ϣ
			return FALSE;
		}
	}

	// Ĭ�Ϸ��ز�ѯʧ����Ϣ
	return FALSE;
}


// ��Ŀ�����ݱ��м��������
BOOL CSqliteManager::CheckData (CString TableName, int Col, BOOL DISTINCT, CString COUNT, CString COLUMN, CString Params, CString GROUP, CString HAVING, CString Order, CString Limit, int SortMode)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ز�ѯʧ����Ϣ
		return FALSE;
	}
	else
	{
		CString CheckData = _T("");

		if (SelectData(TableName, CheckData, Col, DISTINCT, COUNT, COLUMN, Params, GROUP, HAVING, Order, Limit, SortMode))
		{
			if (!CheckData.IsEmpty() && CheckData != _T(""))
			{
				// ����Ŀ�����ݴ���
				return TRUE;
			}
			else
			{
				// ����Ŀ�����ݲ�����
				return FALSE;
			}
		}
		else
		{
			AfxMessageBox(_T("���ݿ��ѯʧ��!"));

			// ���ز�ѯʧ����Ϣ
			return FALSE;
		}
	}

	// Ĭ�Ϸ��ز�ѯʧ����Ϣ
	return FALSE;
}


// ͳ��Ŀ�����ݱ�����������
BOOL CSqliteManager::CountNumber(CString TableName, CString Params, int &Count)
{
	// �����ݿ�δ���ӳɹ�
	if (!IsConnect)
	{
		// ��ʾ������Ϣ
		AfxMessageBox(_T("���ݿ�δ���ӣ������������ݿ�!"));

		// ���ش���ʧ����Ϣ
		return FALSE;
	}
	else
	{
		// �����жϲ����Ƿ�ɹ�
		BOOL IsSuccess;

		// ����ִ�л�ȡ���ݱ��������Ĳ���
		try
		{
			// ��ʼ��SQL���
			CStringA GetSQL;

			// ������Ϊ��
			if (Params.IsEmpty())
			{
				GetSQL = "SELECT * FROM " + (CStringA)TableName + ";";
			}

			// ��������Ϊ��
			else
			{
				GetSQL = "SELECT * FROM " + (CStringA)TableName + " Where " + (CStringA)Params + ";";
			}
			

			// ����ȡ����ʧ��
			if (sqlite3_get_table(db, GetSQL, &pRes, &nRow, &nCol, &errMsg) != SQLITE_OK)
			{
				// ��ʾ������Ϣ
				::MessageBox(NULL, (CString)sqlite3_errmsg(db), NULL, NULL);

				// ���ػ�ȡʧ��
				IsSuccess = FALSE;
			}

			// ����ȡ�ɹ�
			else
			{
				// ������Ϊ��
				if (!nRow)
				{
					// ����0������
					Count = 0;

					// ���ػ�ȡʧ��
					IsSuccess = FALSE;
				}

				// �����ݲ�Ϊ��
				else
				{
					// �õ����ݸ���
					Count = nRow;

					// �ͷ���Դ
					if (errMsg != NULL)
					{
						sqlite3_free(errMsg);
					}

					// ���ػ�ȡ�ɹ�
					IsSuccess = TRUE;
				}
			}

			//�ͷŲ�ѯ���(���۲�ѯ�Ƿ�ɹ�)
			sqlite3_free_table(pRes);
		}
		catch (...)
		{
			// ����������Ϣ
			AfxMessageBox(_T("�޷������ݿ��л�ñ���"));

			// ����0������
			Count = 0;

			// ���ػ�ȡʧ��
			return FALSE;
		}

		// ���ز������
		return IsSuccess;
	}

	// Ĭ�Ϸ��ػ�ȡʧ��
	return FALSE;
}


// ��Ŀ�����ݱ��е���������
BOOL CSqliteManager::ImportData (CString TableName, CString TargetTableName, CString Params)
{
	return true;
}


// ��Ŀ�����ݱ��е���������
BOOL CSqliteManager::ExportData (CString TableName, CString TargetTableName, CString Params)
{
	return true;
}


// ɾ��Ŀ�����ݱ����о�����
BOOL CSqliteManager::DeleteRowData(CString TableName, int Row)
{
	return true;
}


// ɾ��Ŀ�����ݱ����о�����
BOOL CSqliteManager::DeleteColData(CString TableName, int Col)
{
	return true;
}


// ɾ��Ŀ�����ݱ��г�������
BOOL CSqliteManager::DeleteMinData(CString TableName)
{
	return true;
}


// ɾ��Ŀ�����ݱ��м������
BOOL CSqliteManager::DeleteMidData(CString TableName)
{
	return true;
}


// ɾ��Ŀ�����ݱ���ĩ������
BOOL CSqliteManager::DeleteMaxData(CString TableName)
{
	return true;
}


// �޸�Ŀ�����ݱ����о�����
BOOL CSqliteManager::UpdataRowData(CString TableName, int Row, CString UpdataData)
{
	return true;
}


// �޸�Ŀ�����ݱ����о�����
BOOL CSqliteManager::UpdataColData(CString TableName, int Col, CString UpdataData)
{
	return true;
}


// �޸�Ŀ�����ݱ��г�������
BOOL CSqliteManager::UpdataMinData(CString TableName, CString UpdataData)
{
	return true;
}


// �޸�Ŀ�����ݱ��м������
BOOL CSqliteManager::UpdataMidData(CString TableName, CString UpdataData)
{
	return true;
}


// �޸�Ŀ�����ݱ���ĩ������
BOOL CSqliteManager::UpdataMaxData(CString TableName, CString UpdataData)
{
	return true;
}


// ����Ŀ�����ݱ����о�����
BOOL CSqliteManager::SelectRowData(CString TableName, int Row, CString &RowData)
{
	return true;
}


// ����Ŀ�����ݱ����о�����
BOOL CSqliteManager::SelectColData(CString TableName, int Col, CString &ColData)
{
	return true;
}


// ����Ŀ�����ݱ��г�������
BOOL CSqliteManager::SelectMinData(CString TableName, CString &MinData)
{
	return true;
}


// �޸�Ŀ�����ݱ��м������
BOOL CSqliteManager::SelectMidData(CString TableName, CString &MidData)
{
	return true;
}


// ����Ŀ�����ݱ���ĩ������
BOOL CSqliteManager::SelectMaxData(CString TableName, CString &MaxData)
{
	return true;
}


// ΪĿ�����ݱ��о���������
BOOL CSqliteManager::SelectSortData(CString TableName, int Sort, CString &SortData)
{
	return true;
}


// �����������ݱ������˳��
BOOL CSqliteManager::ChangeSortData(CString TableName, int Sort)
{
	return true;
}


// ��Ŀ�����ݱ���ɸѡ������
BOOL CSqliteManager::FilterData(CString TableName, CString Params, CString &FilterData)
{
	return true;
}


// ��Ŀ�����ݱ��еõ����������
BOOL CSqliteManager::RanData(CString TableName, CString Params, CString &RanData)
{
	return true;
}


///////////////////////////////////������������////////////////////////////////////////////////////


// ��Ŀ�����ݱ������������
BOOL CSqliteManager::SumData(CString TableName, CString Params, int &SumData)
{
	return true;
}


// ��Ŀ�����ݱ������������
BOOL CSqliteManager::SubData(CString TableName, CString Params, int &SubData)
{
	return true;
}


// ��Ŀ�����ݱ������������
BOOL CSqliteManager::ProductData(CString TableName, CString Params, int &ProductData)
{
	return true;
}


// ��Ŀ�����ݱ�������������
BOOL CSqliteManager::QuotientData(CString TableName, CString Params, int &QuotientData)
{
	return true;
}


// ��Ŀ�����ݱ�������������
BOOL CSqliteManager::ModData(CString TableName, CString Params, int &ModData)
{
	return true;
}


// ��Ŀ�����ݱ�������������
BOOL CSqliteManager::PowerData(CString TableName, CString Params, int &PowerData, int &Power)
{
	return true;
}


// ��Ŀ�����ݱ��������������
BOOL CSqliteManager::LogData(CString TableName, CString Params, int &LogData, int &Log)
{
	return true;
}


// ��Ŀ�����ݱ�����������׳�
BOOL CSqliteManager::FactData(CString TableName, CString Params, int &FactData)
{
	return true;
}


// ��Ŀ�����ݱ�����������˷�
BOOL CSqliteManager::ExpData(CString TableName, CString Params, int &ExpData, int &Exp)
{
	return true;
}


// ��Ŀ�����ݱ���������������
BOOL CSqliteManager::PluData(CString TableName, CString Params, int &PluData)
{
	return true;
}


// ��Ŀ�����ݱ������������
BOOL CSqliteManager::LimData(CString TableName, CString Params, int &LimData)
{
	return true;
}


// ��Ŀ�����ݱ���������������
BOOL CSqliteManager::SinData(CString TableName, CString Params, int &SinData)
{
	return true;
}


// ��Ŀ�����ݱ���������������
BOOL CSqliteManager::CosData(CString TableName, CString Params, int &CosData)
{
	return true;
}


// ��Ŀ�����ݱ���������������
BOOL CSqliteManager::TanData(CString TableName, CString Params, int &TanData)
{
	return true;
}


// ��Ŀ�����ݱ�����������ƽ����
BOOL CSqliteManager::AvgData(CString TableName, CString Params, int &AvgData)
{
	return true;
}


// ��Ŀ�����ݱ�������������λ��
BOOL CSqliteManager::MidData(CString TableName, CString Params, int &MidData)
{
	return true;
}


// ��Ŀ�����ݱ�����������ٷֱ�
BOOL CSqliteManager::PerData(CString TableName, CString Params, int &PerData)
{
	return true;
}


// ��Ŀ�����ݱ�����������ƽ����
BOOL CSqliteManager::SqrData(CString TableName, CString Params, int &SqrData)
{
	return true;
}


// ��Ŀ�����ݱ�����������������
BOOL CSqliteManager::CubData(CString TableName, CString Params, int &CubData)
{
	return true;
}


// ��Ŀ�����ݱ��������������ֵ
BOOL CSqliteManager::AbsData(CString TableName, CString Params, int &AbsData)
{
	return true;
}


// ��Ŀ�����ݱ��������������ֵ
BOOL CSqliteManager::MaxData(CString TableName, CString Params, int &AbsData)
{
	return true;
}


// ��Ŀ�����ݱ�������������Сֵ
BOOL CSqliteManager::MinData(CString TableName, CString Params, int &AbsData)
{
	return true;
}


// ��Ŀ�����ݱ��������������Լ��
BOOL CSqliteManager::GcdData(CString TableName, CString Params, int &GcdData)
{
	return true;
}


// ��Ŀ�����ݱ�������������С������
BOOL CSqliteManager::LcmData(CString TableName, CString Params, int &LcmData)
{
	return true;
}


// ��Ŀ�����ݱ���ȡ�����������
BOOL RanData(CString TableName, CString Params, int &RanData)
{
	return true;
}


///////////////////////////////////��������ת������///////////////////////////////////


// ACSII   ����ת Unicode ����  
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


// ACSII   ����ת UTF8    ����
string  CSqliteManager::AcsiiToUtf8(   const string  &acsii_string)
{
	wstring unicode_string = AcsiiToUnicode(acsii_string);  //�� ACSII   ����ת��Ϊ Unicode  ����
	string  utf8_string = UnicodeToUtf8(unicode_string);    //�� Unicode ����ת��Ϊ UTF8     ����

	// ���ر���ת�����
	return  utf8_string;
}


// Unicode ����ת ACSII   ����
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


// Unicode ����ת UTF8    ����
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


// UTF8    ����ת ACSII   ����
string  CSqliteManager::Utf8ToAcsii(   const string  &utf8_string)
{
	wstring unicode_string = Utf8ToUnicode(utf8_string);        //��UTF8ת��ΪUnicode  
	string acsii_string = UnicodeToAcsii(unicode_string);   //��Unicodeת��ΪACSII  
	return acsii_string;
}


// UTF8    ����ת Unicode ����
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
