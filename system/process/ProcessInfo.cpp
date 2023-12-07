#include "../../ginc.h"
#include "FileVersionInfo.h"
#include "WatchProcess.h"
#include "../system.h"

CProcessInfo::CProcessInfo()
{
	m_pWatchProcess = std::make_unique<CWatchProcess>();
	Initialize();
}

CProcessInfo::~CProcessInfo()
{
	m_pWatchProcess->StopWatch();

	std::unordered_map<unsigned long, PROCESS_INFO *>::iterator fIter;
	for (fIter = m_mapProcessList.begin(); fIter != m_mapProcessList.end(); ++fIter)
	{
		PROCESS_INFO* pProcessInfo = fIter->second;
		CloseHandle(pProcessInfo->_hProcess);
		wxDELETE(pProcessInfo);
	}

	m_mapProcessList.clear();
	CResourceCommon::Stop();
	CResourceCommon::Clear();
}

void CProcessInfo::Initialize()
{
#ifdef __WXMSW__
	SetPrivilige(SE_DEBUG_NAME);

	HANDLE hServer = WTS_CURRENT_SERVER_HANDLE;    // get local machine processes
    unsigned long ulLevel = 1;                     // get array of WTS_PROCESS_INFO_EX
    unsigned long ulCount = 0;                     // returns the number of processes
    PWTS_PROCESS_INFO pProcessInfo = nullptr;      // output data

	if(WTSEnumerateProcesses(hServer, 0, ulLevel, &pProcessInfo, &ulCount))
	{
		for(unsigned long i = 0; i < ulCount; i++)
			AddProcessInfo(pProcessInfo[i]);
	}

    WTSFreeMemory(pProcessInfo);
	pProcessInfo = nullptr;

	m_pWatchProcess->StartWatch();
	m_bStarted = true;

	numProcessors = theSystemInfo->GetCPUInfo()->GetCoreCount();
#endif
}

#ifdef __WXMSW__
void CProcessInfo::AddProcessInfo(const WTS_PROCESS_INFO& wtsProcessInfo, bool bNewProcess)
{
	wxString strProcessName(wtsProcessInfo.pProcessName);
	AddProcessInfo(wtsProcessInfo.ProcessId, strProcessName);

	if(m_pWatchProcess->CheckAndAddProcessWatch(wtsProcessInfo.ProcessId))
	{
		if(bNewProcess)
			Update(wxEVT_CREATE_NEW_PROCESS, (long)wtsProcessInfo.ProcessId);
	}
}

void CProcessInfo::AddProcessInfo(unsigned long ulProcessID, const wxString& _strProcessName)
{
	wxString strProcessName(_strProcessName);
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator fIter = m_mapProcessList.find(ulProcessID);
	if (fIter != m_mapProcessList.end())
		return;

	PROCESS_INFO *pPSInfo = new PROCESS_INFO();
	pPSInfo->_ulProcessID = ulProcessID;

	if(ulProcessID == 0)
		strProcessName = wxT("System Idle Process");

	pPSInfo->_strProcessName = strProcessName;
	pPSInfo->_hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_READ, FALSE, ulProcessID);
	pPSInfo->_strProcessFullPath = wxT("");

	DWORD dwLen = 0;
	TCHAR szImagePath[MAX_PATH] = { 0, };

	ZeroMemory(szImagePath, sizeof(szImagePath));
	dwLen = sizeof(szImagePath) / sizeof(TCHAR);
	if(QueryFullProcessImageName(pPSInfo->_hProcess, 0, szImagePath, &dwLen))
		pPSInfo->_strProcessFullPath = szImagePath;

	SetUserNameAndDomainNameFromSid(pPSInfo);
	theUtility->GetIconIndexFromImageList(pPSInfo->_strProcessFullPath, pPSInfo->iIconIndex, pPSInfo->iOvelayIndex);

	wxString strExeFile(pPSInfo->_strProcessFullPath);
	CFileVersionInfo data;
	if(data.Create(strExeFile))
	{
		pPSInfo->_strFileDescription = data.GetFileDescription();
		pPSInfo->_strCompanyName = data.GetCompanyName();
	}

	//CommandLine Setting : 속도가 느림
	GetCommandLineOfProcess(ulProcessID, pPSInfo->_strCommandLine);
	InitProcessCPUTime(pPSInfo);

	std::unordered_map<unsigned long, PROCESS_INFO *>::value_type val(ulProcessID, pPSInfo);
	m_mapProcessList.insert(val);
}

int CProcessInfo::SetUserNameAndDomainNameFromSid(PROCESS_INFO* pInfo)
{
	DWORD dwRtnCode = 0;
	DWORD dwErrorCode = 0;
	BOOL bRtnBool = TRUE;
	PSID pSidOwner = NULL;
	DWORD dwAcctName = 1, dwDomainName = 1;
	SID_NAME_USE eUse = SidTypeUnknown;
	HANDLE hFile;
	PSECURITY_DESCRIPTOR pSD = NULL;

	// Get the handle of the file object.
	hFile = CreateFile(
                  pInfo->_strProcessFullPath,
                  GENERIC_READ,
                  FILE_SHARE_READ,
                  NULL,
                  OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL,
                  NULL);

	// Check GetLastError for CreateFile error code.
	if (hFile == INVALID_HANDLE_VALUE)
	{
		dwErrorCode = GetLastError();
		return -1;
	}

	// Get the owner SID of the file.
	dwRtnCode = GetSecurityInfo(
                  hFile,
                  SE_FILE_OBJECT,
                  OWNER_SECURITY_INFORMATION,
                  &pSidOwner,
                  NULL,
                  NULL,
                  NULL,
                  &pSD);

	CloseHandle(hFile);
	// Check GetLastError for GetSecurityInfo error condition.
	if (dwRtnCode != ERROR_SUCCESS)
	{
		dwErrorCode = GetLastError();
		return -1;
	}

	TCHAR AcctName[255] = {0x00, };
	TCHAR DomainName[255] = {0x00, };
	// First call to LookupAccountSid to get the buffer sizes.
	bRtnBool = LookupAccountSid(
                  NULL,           // local computer
                  pSidOwner,
                  AcctName,
                  (LPDWORD)&dwAcctName,
                  DomainName,
                  (LPDWORD)&dwDomainName,
                  &eUse);


    // Second call to LookupAccountSid to get the account name.
    bRtnBool = LookupAccountSid(
          NULL,                   // name of local or remote computer
          pSidOwner,              // security identifier
          AcctName,               // account name buffer
          (LPDWORD)&dwAcctName,   // size of account name buffer
          DomainName,             // domain name
          (LPDWORD)&dwDomainName, // size of domain name buffer
          &eUse);                 // SID type

    // Check GetLastError for LookupAccountSid error condition.
    if (bRtnBool == FALSE)
	{
		dwErrorCode = GetLastError();
		if( dwErrorCode == ERROR_NONE_MAPPED )
			pInfo->_strDomainAndUser = wxT("NONE_MAPPED");

		GetUserNameFromToken(pInfo);
		return -1;
	}

	pInfo->_strUserName = wxString::Format(TEXT("%s"), AcctName);
	pInfo->_strDomainName = wxString::Format(TEXT("%s"), DomainName);
	pInfo->_strDomainAndUser = wxString::Format(TEXT("%s\\%s"), DomainName, AcctName);

	return 0;
}

void CProcessInfo::GetUserNameFromToken(PROCESS_INFO* pInfo)
{
	HANDLE tok = 0;
	TOKEN_USER* ptu;
	wchar_t name[300], dom[300], tubuf[300];
	DWORD nlen, dlen;
	int iUse;
	if(!OpenProcessToken(pInfo->_hProcess, TOKEN_QUERY, &tok))
		return;

	ptu = (TOKEN_USER *)tubuf;
	if(!GetTokenInformation(tok, (TOKEN_INFORMATION_CLASS)1, ptu, 300, &nlen))
		return;

	dlen = 300;
	nlen = 300;
	if(!LookupAccountSid(0, ptu->User.Sid, name, &nlen, dom, &dlen, (PSID_NAME_USE)&iUse))
		return;

	pInfo->_strUserName = wxString::Format(TEXT("%s"), name);
	pInfo->_strDomainName = wxString::Format(TEXT("%s"), dom);
	pInfo->_strDomainAndUser = wxString::Format(TEXT("%s\\%s"), dom, name);
}

bool CProcessInfo::SetPrivilige(LPCTSTR lpszPrivilege, bool bEnablePrivilege)
{
	HANDLE hToken = 0;
	if (!OpenProcessToken(GetCurrentProcess(),
						  TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
						  &hToken))
	{
		wxMessageBox(wxString::Format(wxT("OpenProcessToken function error=%0x"), GetLastError()), PROGRAM_FULL_NAME,  wxOK | wxICON_ERROR);
        return false;
	}

	TOKEN_PRIVILEGES tp;
	// Used by local system to identify the privilege
	LUID luid;

	if(!LookupPrivilegeValue(NULL,          // lookup privilege on local system
							lpszPrivilege,  // privilege to lookup
							&luid))         // receives LUID of privilege

	{
		wxMessageBox(wxString::Format(wxT("LLookupPrivilegeValue() failed, error : %0x\n"), GetLastError()), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;

	// If TRUE, enable
	if(bEnablePrivilege) tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else tp.Privileges[0].Attributes = 0; // else, disable

	if(!AdjustTokenPrivileges(hToken,
							  FALSE,
							  &tp,
							  sizeof(TOKEN_PRIVILEGES),
							  (PTOKEN_PRIVILEGES) NULL,
							  (PDWORD) NULL))
	{
		wxMessageBox(wxString::Format(wxT("AdjustTokenPrivileges() failed, error : %0x\n"), GetLastError()), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	return true;
}
#endif // __WXMSW__

void CProcessInfo::InitProcessCPUTime(PROCESS_INFO* _pPsInfo)
{
	//cpu 사용시간
	FILETIME ftime, fsys, fuser;
	//System시간을 읽어옴
	GetSystemTimeAsFileTime(&ftime);
	memcpy(&_pPsInfo->_ftLastCPU, &ftime, sizeof(FILETIME));
//	memcpy(&_pPsInfo->_lastCPU, &ftime, sizeof(FILETIME));

	//오픈한 프로세스에 대한 사용시간 읽어옴
	GetProcessTimes(_pPsInfo->_hProcess, &ftime, &ftime, &fsys, &fuser);

	memcpy(&_pPsInfo->_ftLastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&_pPsInfo->_ftLastUserCPU, &fuser, sizeof(FILETIME));

//	memcpy(&_pPsInfo->_lastSysCPU, &fsys, sizeof(FILETIME));
//  memcpy(&_pPsInfo->_lastUserCPU, &fuser, sizeof(FILETIME));
}

void CProcessInfo::UpdateProcessCPUTime(PROCESS_INFO* _pPsInfo)
{
	//lamda function
	auto SubSysTimes = [](const FILETIME ft1, const FILETIME ft2) {
		ULONGLONG a,b = 0;

		memcpy( &a, &ft1, sizeof (ULONGLONG) );
		memcpy( &b, &ft2, sizeof (ULONGLONG) );

		return a - b;
	};

	FILETIME ftime, fsys, fuser;
	FILETIME ftNow;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&ftNow, &ftime, sizeof(FILETIME));

	GetProcessTimes(_pPsInfo->_hProcess, &ftime, &ftime, &fsys, &fuser);

	ULONGLONG ullSysDiff = SubSysTimes(fsys, _pPsInfo->_ftLastSysCPU);
	ULONGLONG ullUsrDiff = SubSysTimes(fuser, _pPsInfo->_ftLastUserCPU);
	ULONGLONG ullNowDiff = SubSysTimes(ftNow, _pPsInfo->_ftLastCPU);
	ULONGLONG ullTotal   = ullSysDiff + ullUsrDiff;

	float fPercent = float( ullTotal ) / float( ullNowDiff );
	fPercent /= numProcessors;
	fPercent *= 100.0f;

	_pPsInfo->_ftLastCPU     = ftNow;
	_pPsInfo->_ftLastUserCPU = fuser;
	_pPsInfo->_ftLastSysCPU  = fsys;
	_pPsInfo->fCPUUsage      = fPercent;

/*
	FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    float percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(_pPsInfo->_hProcess, &ftime, &ftime, &fsys, &fuser);

    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));

    percent = (sys.QuadPart - _pPsInfo->_lastSysCPU.QuadPart) + (user.QuadPart - _pPsInfo->_lastUserCPU.QuadPart);
    percent /= (now.QuadPart - _pPsInfo->_lastCPU.QuadPart);
    percent /= numProcessors;
	percent *= 100.0f;

    _pPsInfo->_lastCPU = now;
    _pPsInfo->_lastUserCPU = user;
    _pPsInfo->_lastSysCPU = sys;
	_pPsInfo->fCPUUsage = percent;
*/
}

PROCESS_INFO* CProcessInfo::UpdateProcessInfo(unsigned long _ulProcessId)
{
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator fIter = m_mapProcessList.find(_ulProcessId);
	if (fIter == m_mapProcessList.end())
		return nullptr;

	PROCESS_INFO* pProcessInfo = fIter->second;
	UpdateProcessCPUTime(pProcessInfo);
	UpdateProcessMemory(pProcessInfo);

	return pProcessInfo;
}

void CProcessInfo::UpdateProcessMemory(PROCESS_INFO* _pPsInfo)
{
#ifdef __WXMSW__
	PROCESS_MEMORY_COUNTERS_EX pmc;
	BOOL bInfo = GetProcessMemoryInfo(_pPsInfo->_hProcess, (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));

	if (bInfo)
	{
		_pPsInfo->_PrivateSize = pmc.PrivateUsage;
		_pPsInfo->_WorkingSetSize = pmc.WorkingSetSize;
	}
#else

#endif
}

void CProcessInfo::DelProcess(unsigned long ulProcessID)
{
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator fIter = m_mapProcessList.find(ulProcessID);
	if (fIter == m_mapProcessList.end())
		return;

	PROCESS_INFO* pProcessInfo = fIter->second;
	if(pProcessInfo->_hProcess != nullptr)
		CloseHandle(pProcessInfo->_hProcess);

	wxDELETE(pProcessInfo);

	m_mapProcessList.erase(fIter);

	Update(wxEVT_TERMINATE_AND_DEL_PROCESS, (long)ulProcessID);
}

int CProcessInfo::KillProcess(unsigned long ulProcessID)
{
	//프로세스 항목이 존재하는지 검사만 한다.
	//프로세스가 종료되고 나면 DelProcess() 함수를 CWatchProcess 에서 호출하게 되므로
	//PROCESS_INFO의 _hProcess Handle의 Close는 DelProcess에서 처리
	PROCESS_INFO* pProcess = GetInfo(ulProcessID);
	if(pProcess == nullptr)
		return KILL_PROCESS_MSG_NOT_PROCESSID;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, ulProcessID);

	bool bTerminate = ::TerminateProcess(hProcess, 0);
	if(!bTerminate)
		return KILL_PROCESS_MSG_PROCESS_TERMINATE_FAIL;

	unsigned long ulRetCode = 0;
	GetExitCodeProcess(hProcess, &ulRetCode);

	if (ulRetCode == STILL_ACTIVE)
		return KILL_PROCESS_MSG_PROCESS_ALIVE;

	CloseHandle(hProcess);
	return KILL_PROCESS_MSG_SUCCESS;
}

bool CProcessInfo::GetExistProcess(unsigned long ulProcessID)
{
	bool bReturn = true;

#ifdef __WXMSW__
	if (ulProcessID == 0 || ulProcessID == 4)
		return true;

	PROCESS_INFO* pProcessInfo = GetInfo(ulProcessID);
	if (pProcessInfo == nullptr)
		return false;

	if (pProcessInfo->_hProcess == nullptr)
		bReturn = false;

	DWORD dwExitCode = 0;
	bool bSuccess = GetExitCodeProcess(pProcessInfo->_hProcess, &dwExitCode);

	if(!bSuccess)
		bReturn = false;

	if (dwExitCode != STILL_ACTIVE)
		bReturn = false;

#else

#endif

	return bReturn;
}

PROCESS_INFO* CProcessInfo::GetInfo(unsigned long ulProcessID)
{
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator fIter = m_mapProcessList.find(ulProcessID);
	if (fIter == m_mapProcessList.end())
		return nullptr;

	PROCESS_INFO* pProcessInfo = fIter->second;
	if (pProcessInfo == nullptr)
		return nullptr;

	return pProcessInfo;
}

void CProcessInfo::OnTimer(wxTimerEvent& event)
{
	Update();
}

void CProcessInfo::Update()
{

}

void CProcessInfo::Update(const wxEventType& evtType, long lValue)
{
	SendEvent(evtType, lValue);
}

std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator CProcessInfo::Begin()
{
	return m_mapProcessList.begin();
}

std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator CProcessInfo::End()
{
	return m_mapProcessList.end();
}

void CProcessInfo::UpdateProcesses()
{
	for(auto& elem : m_mapProcessList)
	{
		unsigned long ulProcessId = elem.first;
		PROCESS_INFO* _psInfo = UpdateProcessInfo(ulProcessId);

		m_mapProcessList[ulProcessId]= _psInfo;
	}

	SendEvent(wxEVT_RESOURCE_MONITORING);
}

#ifdef __WXMSW__
	#define _WIN32_DCOM
	#include <tchar.h>
	#include <wbemidl.h>
	#include <comutil.h>
	#include <oleauto.h>
#endif


bool CProcessInfo::GetCommandLineOfProcess(unsigned long ulProcessID, wxString& strCommandLine)
{
#ifdef __WXMSW__
	HRESULT hr = 0;
    IWbemLocator         *WbemLocator  = NULL;
    IWbemServices        *WbemServices = NULL;
    IEnumWbemClassObject *EnumWbem  = NULL;

    CoInitialize(NULL);

	//initializate the Windows security
	hr = CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0);
    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &WbemLocator);

	//connect to the WMI
	hr = WbemLocator->ConnectServer(_T("ROOT\\CIMV2"), NULL, NULL, NULL, 0, NULL, NULL, &WbemServices);

	wchar_t strSql[255] = {0x00, };
	wsprintf(strSql, _T("SELECT ProcessId,CommandLine,ExecutablePath FROM Win32_Process where ProcessId=%u"), ulProcessID);
	hr = WbemServices->ExecQuery(_T("WQL"), strSql, WBEM_FLAG_FORWARD_ONLY, NULL, &EnumWbem);

	// Iterate over the enumerator
    if (EnumWbem != NULL)
	{
		IWbemClassObject *result = NULL;
        ULONG returnedCount = 0;

        while((hr = EnumWbem->Next(WBEM_INFINITE, 1, &result, &returnedCount)) == S_OK)
		{
			VARIANT ProcessId;
            VARIANT CommandLine;
            VARIANT ExecutablePath;

            // access the properties
            hr = result->Get(L"ProcessId", 0, &ProcessId, 0, 0);
            hr = result->Get(L"CommandLine", 0, &CommandLine, 0, 0);
            hr = result->Get(L"ExecutablePath", 0, &ExecutablePath, 0, 0);

            if (ProcessId.uintVal == ulProcessID)
            {
				wchar_t* _strCommandLine = (wchar_t *)_bstr_t(V_BSTR(&CommandLine));
				wxString strCmdLine(_strCommandLine);

				strCommandLine = strCmdLine;
			}

			result->Release();
		}
	}

	// Release the resources
    EnumWbem->Release();
    WbemServices->Release();
    WbemLocator->Release();

    CoUninitialize();
#endif
	return true;
}
