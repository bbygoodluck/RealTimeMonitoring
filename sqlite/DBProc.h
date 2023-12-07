#ifndef DBPROC_H_INCLUDED
#define DBPROC_H_INCLUDED

#include "MySQLite.h"
class CDBProc
{
private:
	explicit CDBProc() {};

public:
	~CDBProc();
	static CDBProc* Get();

	void Init();
	void ShowSQLiteErrorMessage();
	void CreateDB();
	void CreateDB(const wxString& strDBPath, const wxString& strDBName);

	void OpenDB();
	void OpenDB(const wxString& strDBPath, const wxString& strDBName);
	bool CreateTables();

	bool IsOpenDB() { return m_bOpenSuccess; }

	bool InsertProcessInfo(PROCESS_INFO* pProcessInfo, const wxString& strCPUUsage);
	void AllInitProcessList();

private:
	static std::unique_ptr<CDBProc> m_pInstance;
	CMySQLite m_sqlite;

	wxString strTableDDL = wxT("");
	wxString strExistTable = wxT("select count(*) from T_PROCESS");

	bool m_bOpenSuccess = false;

	wxString m_strToday = wxT("");
};

#endif // DBPROC_H_INCLUDED
