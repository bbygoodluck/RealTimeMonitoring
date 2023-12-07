#include "../ginc.h"
#include "MySQLite.h"

CMySQLite::CMySQLite()
{
	Init();
}

CMySQLite::~CMySQLite()
{
	Close();
}

void CMySQLite::Init()
{
	m_pConnection = std::make_unique<SQLiteConnection>();
	m_pSync = std::make_unique<SyncDB>();
}

bool CMySQLite::CreateDB(const wxString& strDBName, const wxString& strPath)
{
	if(!Open(strDBName, strPath))
		return false;

	return true;
}

bool CMySQLite::Open(const wxString& strDBName, const wxString& strPath)
{
	m_pConnection->_strDBName = strDBName;
	m_pConnection->_strPath = strPath;

	wxString strDBPathName = strPath + SLASH + strDBName;
	int rc = SQLITE_OK;
#if defined(_UNICODE) || defined(UNICODE)
	rc = sqlite3_open16(strDBPathName.wchar_str(), &m_pConnection->_sqlite_db);
#else
	rc = sqlite3_open(strDBPathName.mb_str(), &m_pConnection->_sqlite_db);
#endif // _UNICODE

	if(rc != SQLITE_OK)
	{
		Close();

		_sqlite_err = sqlite3_errcode(m_pConnection->_sqlite_db);
		SetLastSQLiteError(wxT("CMySQLite::Open"), __LINE__);

		return false;
	}

	m_bOpened = true;
	_sqlite_err = SQLITE_OK;

	return true;
}

bool CMySQLite::Close()
{
	if(m_pConnection->_sqlite_db)
	{
		int rc = sqlite3_close(m_pConnection->_sqlite_db);
		if(rc != SQLITE_OK)
		{
			_sqlite_err = sqlite3_errcode(m_pConnection->_sqlite_db);
			SetLastSQLiteError(wxT("CMySQLite::Close"), __LINE__);

			return false;
		}
	}

	_sqlite_err = SQLITE_OK;
	return true;
}

bool CMySQLite::SQLiteStatement(const wxString& strQuery)
{
#if defined(_UNICODE) || defined(UNICODE)
	_sqlite_err = sqlite3_prepare16_v2(m_pConnection->_sqlite_db, strQuery.wchar_str(), -1, &m_pConnection->_stmt, nullptr);
#else
	_sqlite_err = sqlite3_prepare_v2(m_pConnection->_sqlite_db, strQuery.c_str(), -1, &m_pConnection->_stmt, nullptr);
#endif // defined

	if(_sqlite_err != SQLITE_OK)
	{
		_sqlite_err = sqlite3_errcode(m_pConnection->_sqlite_db);
		SetLastSQLiteError(wxT("CMySQLite::SQLiteStatement"), __LINE__);

		return false;
	}

	_sqlite_err = SQLITE_OK;
	return true;
}

bool CMySQLite::Execute(const std::vector<wxString>& vecQuery)
{
	int rc = SQLITE_OK;
	wxString strQuery(wxT(""));

	auto it = vecQuery.begin();
	while(it != vecQuery.end())
	{
		strQuery = *it;
		rc = Execute(strQuery);
		if(rc < 0)
			return false;
	}

	return true;
}

int CMySQLite::Execute(const wxString& strQuery)
{
	int rc = SQLITE_OK;
#if defined(_UNICODE) || defined(UNICODE)
	rc = sqlite3_exec16(m_pConnection->_sqlite_db, strQuery.wchar_str(), nullptr, 0, nullptr);
#else
	rc = sqlite3_exec(m_pConnection->_sqlite_db, strQuery.c_str(), nullptr, 0, nullptr);
#endif // defined

	if(rc != SQLITE_OK)
	{
		_sqlite_err = sqlite3_errcode(m_pConnection->_sqlite_db);
		SetLastSQLiteError(wxT("CMySQLite::Execute"), __LINE__);

		return -1;
	}

	return sqlite3_total_changes(m_pConnection->_sqlite_db);

}

IResult* CMySQLite::ExecuteSelect(const wxString& strQuery)
{
	if(!IsOpened())
		return nullptr;

	m_pSync->LockDB();

	if(!SQLiteStatement(strQuery))
	{
		_sqlite_err = sqlite3_errcode(m_pConnection->_sqlite_db);
		SetLastSQLiteError(wxT("CMySQLite::ExecuteSelect"), __LINE__);

		sqlite3_reset(m_pConnection->_stmt);
		sqlite3_finalize(m_pConnection->_stmt);

		m_pSync->UnLockDB();
		return nullptr;
	}

	m_iColumnCount = sqlite3_column_count(m_pConnection->_stmt);
	IResult *ires = this;
	return ires;
}

bool CMySQLite::IsTableExist(const wxString& strQuery)
{
	if(!SQLiteStatement(strQuery))
		return false;

	bool bReturn = true;
	int rc = sqlite3_step(m_pConnection->_stmt);

	if(rc != SQLITE_ROW)
	{
		_sqlite_err = sqlite3_errcode(m_pConnection->_sqlite_db);
		SetLastSQLiteError(wxT("CMySQLite::IsTableExist"), __LINE__);

		bReturn = false;
	}

	sqlite3_reset(m_pConnection->_stmt);
	sqlite3_finalize(m_pConnection->_stmt);

	_sqlite_err = SQLITE_OK;
	return bReturn;
}

sqlite3_stmt* CMySQLite::GetStmt(const wxString& strQuery)
{
	if(!IsOpened())
		return nullptr;

	if(!SQLiteStatement(strQuery))
		return nullptr;

	return m_pConnection->_stmt;
}

bool CMySQLite::ExecuteBind()
{
	if(!IsOpened())
		return false;

	if(m_pConnection->_stmt == nullptr)
		return false;


	bool bRet = true;

	this->Begin();

	if(sqlite3_step(m_pConnection->_stmt) != SQLITE_DONE)
	{
		_sqlite_err = sqlite3_errcode(m_pConnection->_sqlite_db);
		SetLastSQLiteError(wxT("CMySQLite::IsTableExist"), __LINE__);

		bRet = false;
	}

	this->Commit();

	sqlite3_reset(m_pConnection->_stmt);
	sqlite3_finalize(m_pConnection->_stmt);

	return bRet;
}


bool CMySQLite::Begin()
{
#if defined(_UNICODE) || defined(UNICODE)
	sqlite3_exec16(m_pConnection->_sqlite_db, _strBEGIN.wchar_str(), nullptr, nullptr, nullptr);
#else
	sqlite3_exec(m_pConnection->_sqlite_db, _strBEGIN.mb_str(), nullptr, nullptr, nullptr);
#endif // defined

	return true;
}

bool CMySQLite::Commit()
{
#if defined(_UNICODE) || defined(UNICODE)
	sqlite3_exec16(m_pConnection->_sqlite_db, _strCOMMIT.wchar_str(), nullptr, nullptr, nullptr);
#else
	sqlite3_exec(m_pConnection->_sqlite_db, _strCOMMIT.mb_str(), nullptr, nullptr, nullptr);
#endif // defined

	return true;
}

bool CMySQLite::Vacuum()
{
#if defined(_UNICODE) || defined(UNICODE)
	sqlite3_exec16(m_pConnection->_sqlite_db, _strVACCUM.wchar_str(), nullptr, nullptr, nullptr);
#else
	sqlite3_exec(m_pConnection->_sqlite_db, _strCOMMIT.mb_str(), nullptr, nullptr, nullptr);
#endif // defined

	return true;
}

bool CMySQLite::Rollback()
{
#if defined(_UNICODE) || defined(UNICODE)
	sqlite3_exec16(m_pConnection->_sqlite_db, _strROLLBACK.wchar_str(), nullptr, nullptr, nullptr);
#else
	sqlite3_exec(m_pConnection->_sqlite_db, _strROLLBACK.mb_str(), nullptr, nullptr, nullptr);
#endif // defined

	return true;
}

int	CMySQLite::GetColumnCount()
{
	return m_iColumnCount;
}

int CMySQLite::GetColumnType(int iColumnIndex)
{
	return sqlite3_column_type(m_pConnection->_stmt, iColumnIndex);
}

bool CMySQLite::Next()
{
	return (sqlite3_step(m_pConnection->_stmt) == SQLITE_ROW) ? true : false;
}

wxString CMySQLite::NextColomnName(int iColumnIndex)
{
	if(!CheckColumn(iColumnIndex))
		return wxT("");

	wxString strColName(wxT(""));
#if defined(_UNICODE) || defined(UNICODE)
	strColName = wxString::Format(wxT("%s"), (wchar_t *)sqlite3_column_name16(m_pConnection->_stmt, iColumnIndex));
#else
	strColName = wxString::Format(wxT("%s"), sqlite3_column_name(m_pConnection->_stmt, iColumnIndex))
#endif // defined
	return strColName;
}

bool CMySQLite::CheckColumn(int iColumnIndex)
{
	return iColumnIndex < m_iColumnCount;
}

int CMySQLite::GetColumnInt(int iColumnIndex)
{
	if(!CheckColumn(iColumnIndex))
		return -1;

	return sqlite3_column_int(m_pConnection->_stmt, iColumnIndex);
}

int64_t CMySQLite::GetColumnInt64(int iColumnIndex)
{
	if(!CheckColumn(iColumnIndex))
		return -1;

	return sqlite3_column_int64(m_pConnection->_stmt, iColumnIndex);
}

wxString CMySQLite::GetColumnString(int iColumnIndex)
{
	if(!CheckColumn(iColumnIndex))
		return wxT("");

	wxString strReturn(wxT(""));

#if defined(_UNICODE) || defined(UNICODE)
	strReturn = wxString::Format(wxT("%s"), (wchar_t *)sqlite3_column_text16(m_pConnection->_stmt, iColumnIndex));
#else
	strReturn = wxString::Format(wxT("%s"), sqlite3_column_text(m_pConnection->_stmt, iColumnIndex));
#endif // defined

	return strReturn;
}

wxString CMySQLite::GetColumnBlob(int iColumnIndex)
{
	if(!CheckColumn(iColumnIndex))
		return wxT("");

	wxString strReturn(wxT(""));
	strReturn = wxString::Format(wxT("%s"), sqlite3_column_blob(m_pConnection->_stmt, iColumnIndex));

	return strReturn;
}

double CMySQLite::GetColumnDouble(int iColumnIndex)
{
	if(!CheckColumn(iColumnIndex))
		return -1.0;

	return sqlite3_column_double(m_pConnection->_stmt, iColumnIndex);
}

void CMySQLite::Release()
{
	sqlite3_reset(m_pConnection->_stmt);
	sqlite3_finalize(m_pConnection->_stmt);
	m_iColumnCount = 0;

	m_pSync->UnLockDB();
}

void CMySQLite::SetLastSQLiteError(const wxString& strFuncName, int errLine)
{
#if defined(_UNICODE) || defined(UNICODE)
	_strSQLite_ErrorMsg = strFuncName + wxString::Format(wxT(" : error line : %d / msg : %s"), errLine, (wchar_t *)sqlite3_errmsg16(m_pConnection->_sqlite_db));
#else
	_strSQLite_ErrorMsg = strFuncName + wxString::Format(wxT(" : error line : %d / msg : %s"), errLine, sqlite3_errmsg(m_pConnection->_sqlite_db));
#endif // defined

}
