#include "../ginc.h"
#include "DBProc.h"
#include "../system/process/ProcessInfo.h"
#include "../system/process/FileVersionInfo.h"

std::unique_ptr<CDBProc> CDBProc::m_pInstance(nullptr);

CDBProc* CDBProc::Get()
{
	if (m_pInstance.get() == nullptr)
	{
		m_pInstance.reset(new CDBProc());
		m_pInstance->Init();
	}

	return m_pInstance.get();
}

CDBProc::~CDBProc()
{

}


void CDBProc::Init()
{
	strTableDDL = wxT("CREATE TABLE 'T_PROCESS'(");
    strTableDDL += wxT("'id'  INTEGER NOT NULL,");
    strTableDDL += wxT("'ProcessName' TEXT NOT NULL,");
    strTableDDL += wxT("'CPUUsage' TEXT NOT NULL,");
    strTableDDL += wxT("'FullPath' TEXT NOT NULL,");
    strTableDDL += wxT("'Description' TEXT,");
    strTableDDL += wxT("'CompanyName' TEXT,");
    strTableDDL += wxT("'CommandLine' TEXT,");
    strTableDDL += wxT("'InputTime' TEXT NOT NULL,");
	strTableDDL += wxT("PRIMARY KEY('id' AUTOINCREMENT));");
}

void CDBProc::ShowSQLiteErrorMessage()
{
	int iRet = m_sqlite.GetSQLiteErrorCode();
	wxString strMsg(wxString::Format(wxT("sqlite error code : %d error msg : "), iRet) + m_sqlite.GetSQLiteErrorMsg());

	wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_ERROR | wxOK, _gMenuEvent->GetMainFrame());
}

void CDBProc::CreateDB()
{
	wxString strDBPath = wxT("");
	wxString strDBName = wxT("");

	wxFileDialog dialog
	(
		_gMainWindow,
		wxT("신규 작업파일 생성"),
		wxEmptyString,
		wxEmptyString,
		wxString::Format(wxT("SQLiteDB files (*.db)|*.db|All files (%s)|%s"), wxFileSelectorDefaultWildcardStr, wxFileSelectorDefaultWildcardStr)
	);

	if (dialog.ShowModal() == wxID_OK)
	{
		strDBPath = dialog.GetDirectory();
		strDBName = dialog.GetFilename();

		CreateDB(strDBPath, strDBName);
	}
}

void CDBProc::CreateDB(const wxString& strDBPath, const wxString& strDBName)
{
	if(!m_sqlite.CreateDB(strDBName, strDBPath))
	{
		ShowSQLiteErrorMessage();
		return;
	}

	if(!CreateTables())
	{
		ShowSQLiteErrorMessage();
		return;
	}

	m_bOpenSuccess = true;
}

void CDBProc::OpenDB()
{
	wxString strDBPath;
	wxString strDBName;

	wxFileDialog dialog
	(
		_gMainWindow,
		wxT("작업 파일 열기"),
		wxEmptyString,
		wxEmptyString,
		wxString::Format(wxT("SQLiteDB files (*.db)|*.db|All files (%s)|%s"), wxFileSelectorDefaultWildcardStr, wxFileSelectorDefaultWildcardStr)
	);

	if (dialog.ShowModal() != wxID_OK)
		return;

	strDBPath = dialog.GetDirectory();
	strDBName = dialog.GetFilename();

	OpenDB(strDBPath, strDBName);
}

void CDBProc::OpenDB(const wxString& strDBPath, const wxString& strDBName)
{
	if(!m_sqlite.Open(strDBName, strDBPath))
	{
		ShowSQLiteErrorMessage();
		return;
	}

	if(!m_sqlite.IsTableExist(strExistTable))
	{
		if(!CreateTables())
		{
			ShowSQLiteErrorMessage();
			return;
		}
	}

	m_bOpenSuccess = true;
	AllInitProcessList();
}

bool CDBProc::CreateTables()
{
	int rc = m_sqlite.Execute(strTableDDL);
	if( rc < 0)
		return false;

	return true;
}

void CDBProc::AllInitProcessList()
{
	//Process list 초기화
	wxString strInitProcessQuery = wxT("delete from T_PROCESS;");
	int rc = m_sqlite.Execute(strInitProcessQuery);
	if( rc < 0)
		ShowSQLiteErrorMessage();

	wxString strInitAutoIncrement = wxT("update sqlite_sequence set seq = 0 where name = 'T_PROCESS'");
	rc = m_sqlite.Execute(strInitAutoIncrement);
	if( rc < 0)
		ShowSQLiteErrorMessage();

	m_sqlite.Vacuum();
}

bool CDBProc::InsertProcessInfo(PROCESS_INFO* pProcessInfo, const wxString& strCPUUsage)
{
	wxDateTime dtNow = wxDateTime::Now();
	wxString strToday = dtNow.FormatISODate();
	wxString strInsertTime = dtNow.FormatISODate() + wxT(" ") + dtNow.FormatISOTime();

	if(m_strToday.IsEmpty())
		m_strToday = strToday;
	else if(m_strToday.CmpNoCase(strToday) != 0) //오늘과 같지 않으면..
	{
		AllInitProcessList();
		m_strToday = strToday;
	}

	wxString strTmpQuery = wxT("insert into T_PROCESS (ProcessName, CPUUsage, FullPath, Description, CompanyName, CommandLine, InputTime) values (");
	 	     strTmpQuery += wxT("'%s', '%s', '%s', '%s', '%s', '%s', '%s')");

	wxString strQuery = wxString::Format(strTmpQuery,
										pProcessInfo->_strProcessName,
										strCPUUsage,
										pProcessInfo->_strProcessFullPath,
										pProcessInfo->_strFileDescription,
										pProcessInfo->_strCompanyName,
										pProcessInfo->_strCommandLine,
										strInsertTime);

	int rc = m_sqlite.Execute(strQuery);
	if( rc < 0)
	{
		//프로세스 감시는 계속되어야 하므로 메세지창은 보이지 않음
#ifndef NDEBUG
		ShowSQLiteErrorMessage();
#endif
		return false;
	}

	return true;
}
