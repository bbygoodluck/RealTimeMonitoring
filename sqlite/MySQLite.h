#ifndef MYSQLITE_H_INCLUDED
#define MYSQLITE_H_INCLUDED

#include "sqlite3.h"


//for Multithreading
typedef struct _SyncDB
{
	CRITICAL_SECTION _cs;
	_SyncDB()			 { ::InitializeCriticalSection(&_cs); }
	void LockDB()        { ::EnterCriticalSection(&_cs);      }
	void UnLockDB()      { ::LeaveCriticalSection(&_cs);      }
	~_SyncDB()           { ::DeleteCriticalSection(&_cs);     }
} SyncDB;

/*Interface class for Result data of query */
class IResult
{
public:
	/*This function return of count of column
	  present in result set of last excueted query*/
	virtual int	GetColumnCount() = 0;

	/*Get the next coloumn name*/
	virtual wxString NextColomnName(int iColumnIndex) = 0;

	/* Get the column type */
	virtual int GetColumnType(int iColumnIndex) = 0;
	/*This function returns TRUE if still rows are
	der in result set of last excueted query FALSE
	if no row present*/
	virtual bool Next() = 0;

	/*Get the next coloumn data*/
	virtual int      GetColumnInt(int iColumnIndex) = 0;
 	virtual int64_t  GetColumnInt64(int iColumnIndex) = 0;
	virtual wxString GetColumnString(int iColumnIndex) = 0;
	virtual wxString GetColumnBlob(int iColumnIndex) = 0;
	virtual double   GetColumnDouble(int iColumnIndex) = 0;

	/*RELEASE all result set as well as RESET all data*/
	virtual void Release() = 0;
};

class CMySQLite : public IResult
{
public:
	CMySQLite();
	~CMySQLite();

	bool CreateDB(const wxString& strDBName, const wxString& strPath);

	bool Begin();
	bool Commit();
	bool Vacuum();

	bool Open(const wxString& strDBName, const wxString& strPath);
	bool Close();

	const bool IsOpened() const { return m_bOpened;	}
	bool IsTableExist(const wxString& strQuery);

	inline int GetSQLiteErrorCode() const {
		return _sqlite_err;
	}

	inline wxString GetSQLiteErrorMsg() const {
		return _strSQLite_ErrorMsg;
	}

	bool     Execute(const std::vector<wxString>& vecQuery);
	int      Execute(const wxString& strQuery);
	IResult* ExecuteSelect(const wxString& strQuery);

	wxString GetDBPathName() {
		if(m_pConnection != nullptr)
			return (m_pConnection->_strPath + SLASH + m_pConnection->_strDBName);

		return wxT("");
	}

	sqlite3_stmt* GetStmt(const wxString& strQuery);
	bool ExecuteBind();

protected:
	/* SQLite Connection Object */
	typedef struct _SQLITEConnection
	{
		wxString _strDBName = wxT("");
		wxString _strPath   = wxT("");
		sqlite3* _sqlite_db = nullptr;
		sqlite3_stmt* _stmt = nullptr;
	} SQLiteConnection;

	std::unique_ptr<SQLiteConnection> m_pConnection = nullptr;
	std::unique_ptr<SyncDB> m_pSync = nullptr;

	bool m_bOpened = false;

	int _sqlite_err = SQLITE_OK;
	wxString _strSQLite_ErrorMsg = wxT("");

private:
	void Init();
	bool Rollback();

	bool SQLiteStatement(const wxString& strQuery);

	// interface functions
	bool     Next();
	int      GetColumnInt(int iColumnIndex);
 	int64_t  GetColumnInt64(int iColumnIndex);
	wxString GetColumnString(int iColumnIndex);
	wxString GetColumnBlob(int iColumnIndex);
	double   GetColumnDouble(int iColumnIndex);

	int	 GetColumnCount();
	int GetColumnType(int iColumnIndex);
	wxString NextColomnName(int iColumnIndex);

	void Release();

	bool CheckColumn(int iColumnIndex);
	void SetLastSQLiteError(const wxString& strFuncName, int errLine);

private:

	int m_iColumnCount = 0;

	wxString _strBEGIN      = wxT("BEGIN TRANSACTION;");
	wxString _strCOMMIT     = wxT("COMMIT TRANSACTION;");
	wxString _strROLLBACK   = wxT("ROLLBACK;");
	wxString _strVACCUM     = wxT("VACUUM;");
};

#endif // MYSQLITE_H_INCLUDED
