#include "../../ginc.h"
#include "WatchProcess.h"
#include "ProcessInfoImpl.h"
#include "FileVersionInfo.h"
#include "../../panels/PanelProcessTotal.h"

/*
 * SE_xxxx 선언은 <ddk/wdm.h> 헤더에 적용되어 있으나
 * wdm.h 헤더를 include하게되면 컴파일 에러가 발생함
 * 하여 SE_xxxx 선언을 직접 가져와 처리함
 */
#define SE_MIN_WELL_KNOWN_PRIVILEGE         2
#define SE_CREATE_TOKEN_PRIVILEGE           2
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE     3
#define SE_LOCK_MEMORY_PRIVILEGE            4
#define SE_INCREASE_QUOTA_PRIVILEGE         5
#define SE_MACHINE_ACCOUNT_PRIVILEGE        6
#define SE_TCB_PRIVILEGE                    7
#define SE_SECURITY_PRIVILEGE               8
#define SE_TAKE_OWNERSHIP_PRIVILEGE         9
#define SE_LOAD_DRIVER_PRIVILEGE            10
#define SE_SYSTEM_PROFILE_PRIVILEGE         11
#define SE_SYSTEMTIME_PRIVILEGE             12
#define SE_PROF_SINGLE_PROCESS_PRIVILEGE    13
#define SE_INC_BASE_PRIORITY_PRIVILEGE      14
#define SE_CREATE_PAGEFILE_PRIVILEGE        15
#define SE_CREATE_PERMANENT_PRIVILEGE       16
#define SE_BACKUP_PRIVILEGE                 17
#define SE_RESTORE_PRIVILEGE                18
#define SE_SHUTDOWN_PRIVILEGE               19
#define SE_DEBUG_PRIVILEGE                  20
#define SE_AUDIT_PRIVILEGE                  21
#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE     22
#define SE_CHANGE_NOTIFY_PRIVILEGE          23
#define SE_REMOTE_SHUTDOWN_PRIVILEGE        24
#define SE_UNDOCK_PRIVILEGE                 25
#define SE_SYNC_AGENT_PRIVILEGE             26
#define SE_ENABLE_DELEGATION_PRIVILEGE      27
#define SE_MANAGE_VOLUME_PRIVILEGE          28
#define SE_IMPERSONATE_PRIVILEGE            29
#define SE_CREATE_GLOBAL_PRIVILEGE          30
#define SE_TRUSTED_CREDMAN_ACCESS_PRIVILEGE 31
#define SE_RELABEL_PRIVILEGE                32
#define SE_INC_WORKING_SET_PRIVILEGE        33
#define SE_TIME_ZONE_PRIVILEGE              34
#define SE_CREATE_SYMBOLIC_LINK_PRIVILEGE   35
#define SE_MAX_WELL_KNOWN_PRIVILEGE         SE_CREATE_SYMBOLIC_LINK_PRIVILEGE

CProcessInfoImpl::CProcessInfoImpl()
	: m_pWatchProcess(nullptr)
{
	m_pWatchProcess = std::make_unique<CWatchProcess>();
	Initialize();
}

CProcessInfoImpl::~CProcessInfoImpl()
{
	m_pWatchProcess->StopWatch();
	std::unordered_map<unsigned long, PROCESS_INFO *>::iterator fIter;
	for (fIter = m_mapProcesses.begin(); fIter != m_mapProcesses.end(); ++fIter)
	{
		PROCESS_INFO* pProcessInfo = fIter->second;
		CloseHandle(pProcessInfo->_hProcess);
		delete pProcessInfo;
	}
}

void CProcessInfoImpl::Initialize()
{
#ifdef __WXMSW__
	/* 
	EnumProcesses()함수의 경우 읽어들일 프로세스의 수를 입력해야 하므로 프로세스의 갯수가 많은 시스템의 경우는 제한적일 수 있으므로 SnapShot을 이용하여 처리
	*/
	/*HANDLE hProcessSnap;
	PROCESSENTRY pe32;
	
	// 시스템에 있는 모든 프로세스의 스냅샷을 가져옴.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return;
	
	// PROCESSENTRY32 사이즈
	pe32.dwSize = sizeof(PROCESSENTRY);
	// 첫번째 프로세스에 관한 정보를 가져옴
	// 실패하면 return
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return;
	}
	
	do
	{
		AddNewProcess(pe32);
	} while (Process32Next(hProcessSnap, &pe32));
	
	
	CloseHandle(hProcessSnap);*/
	SetEnableTokenPrivilege(SE_DEBUG_NAME);
	HANDLE hServer = WTS_CURRENT_SERVER_HANDLE;    // get local machine processes
    unsigned long ulLevel = 1;                     // get array of WTS_PROCESS_INFO_EX
    unsigned long ulCount = 0;                     // returns the number of processes
    PWTS_PROCESS_INFO pProcessInfo = nullptr;     // output data

	if(WTSEnumerateProcesses(hServer, 0, ulLevel, &pProcessInfo, &ulCount))
	{
		for(unsigned long i = 0; i < ulCount; i++)
			AddNewProcess(pProcessInfo[i]);
	}
	
	WTSFreeMemory(pProcessInfo);
	pProcessInfo = nullptr;
	
	m_pWatchProcess->StartWatch();
#else
#endif
}

bool CProcessInfoImpl::SetEnableTokenPrivilege(LPCTSTR pszPrivilege)
{
	HANDLE hToken		 = 0;
//	TOKEN_PRIVILEGES tkp = {0}; 
//	LUID luid;
	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(),
						  TOKEN_ADJUST_PRIVILEGES |
						  TOKEN_QUERY, &hToken))
	{
        return false;
	}
	
	int iArrayPrivileges[] = {
		SE_DEBUG_PRIVILEGE,
		SE_INC_BASE_PRIORITY_PRIVILEGE,
		SE_INC_WORKING_SET_PRIVILEGE,
		SE_LOAD_DRIVER_PRIVILEGE,
		SE_PROF_SINGLE_PROCESS_PRIVILEGE,
		SE_BACKUP_PRIVILEGE,
		SE_RESTORE_PRIVILEGE,
		SE_SHUTDOWN_PRIVILEGE,
		SE_TAKE_OWNERSHIP_PRIVILEGE,
		SE_ASSIGNPRIMARYTOKEN_PRIVILEGE,
		SE_TCB_PRIVILEGE,
		SE_MACHINE_ACCOUNT_PRIVILEGE,
		SE_INCREASE_QUOTA_PRIVILEGE,
		SE_TRUSTED_CREDMAN_ACCESS_PRIVILEGE,
		SE_SYSTEM_PROFILE_PRIVILEGE,
		SE_SYSTEMTIME_PRIVILEGE
	};
	
	constexpr int iPrivilegeCount = WXSIZEOF(iArrayPrivileges);
	CHAR privilegesBuffer[FIELD_OFFSET(TOKEN_PRIVILEGES, Privileges) + sizeof(LUID_AND_ATTRIBUTES) * iPrivilegeCount];
	PTOKEN_PRIVILEGES privileges;
	ULONG i;
	
	privileges = (PTOKEN_PRIVILEGES)privilegesBuffer;
	privileges->PrivilegeCount = iPrivilegeCount;

	for (i = 0; i < privileges->PrivilegeCount; i++)
	{
		privileges->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
		privileges->Privileges[i].Luid.HighPart = 0;
		privileges->Privileges[i].Luid.LowPart = iArrayPrivileges[i];
	}
	
	AdjustTokenPrivileges(hToken,                  //HANDLE
						  FALSE,                   //BOOL(DisableAllPrivileges
						  privileges,              //NewState 
						  0,                       //BufferLength
						  (PTOKEN_PRIVILEGES)NULL, //PreviousState
						  0);                      //ReturnLength

	if (GetLastError() != ERROR_SUCCESS)
		return false;
	
	return true;
		
/*	// Get the LUID for the privilege. 
	if(LookupPrivilegeValue(NULL, pszPrivilege, &luid))//&tkp.Privileges[0].Luid)) 
	{
        tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		tkp.Privileges[0].Luid = luid;
		// Set the privilege for this process. 
		AdjustTokenPrivileges(hToken,                      //HANDLE
							  FALSE,                       //BOOL(DisableAllPrivileges
							  &tkp,                        //NewState 
							  0,                           //BufferLength
							  (PTOKEN_PRIVILEGES)NULL,     //PreviousState
							  0);                          //ReturnLength

		if (GetLastError() != ERROR_SUCCESS)
			return false;
		
		return true;
	}

	return false;*/
}
//void CProcessInfoImpl::AddNewProcess(const PROCESSENTRY& pe32, bool bCreateNew)
void CProcessInfoImpl::AddNewProcess(const WTS_PROCESS_INFO& wtsProcessInfo, bool bCreateNew)
{
	unsigned long ulProcessID = wtsProcessInfo.ProcessId;//pe32.th32ProcessID;//;
	
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator fIter = m_mapProcesses.find(ulProcessID);
	if (fIter != m_mapProcesses.end())
		return;
	
	PROCESS_INFO *pProcessInfo = new PROCESS_INFO;
//	pProcessInfo->_piEntry = pEntry;
	pProcessInfo->_ulProcessID = ulProcessID;
	
	wxString strProcessName(wtsProcessInfo.pProcessName);//pEntry.szExeFile);
	if(ulProcessID == 0)
		strProcessName = wxT("System Idle Process");
	/*	
	if(strProcessName.CmpNoCase(wxT("[System Process]")) == 0)
		strProcessName = wxT("System Idle Process");
	*/
	
	pProcessInfo->_strProcessName = strProcessName;
	pProcessInfo->_hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_READ, FALSE, ulProcessID);//OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, ulProcessID);
	
	GetProcessFullPathName(ulProcessID, pProcessInfo->_strProcessFullPath);
	SetUserNameAndDomainNameFromSid(pProcessInfo, pProcessInfo->_strProcessFullPath, wtsProcessInfo.pUserSid);
	
	wxString strExeFile(pProcessInfo->_strProcessFullPath);
	theCommonUtil->GetIconIndexFromImageList(strExeFile, pProcessInfo->iIconIndex, pProcessInfo->iOvelayIndex);
	
	CFileVersionInfo data;
	if(data.Create(strExeFile))
	{	
		pProcessInfo->_strFileDescription = data.GetFileDescription();
		pProcessInfo->_strCompanyName = data.GetCompanyName();
	}
	
	InitCPUFromProcess(pProcessInfo);
	//신규 프로세스 등록
	std::unordered_map<unsigned long, PROCESS_INFO *>::value_type val(ulProcessID, pProcessInfo);
	m_mapProcesses.insert(val);
	
	if(m_pWatchProcess->CheckAndAddProcessWatch(ulProcessID))
	{
		if(bCreateNew)
		{
			//이벤트 전송
			CPanelProcessTotal* pProcessPanel = (CPanelProcessTotal *)thePanelManager->GetPanel(PANEL_PROCESS_TOTAL);
			if(pProcessPanel)
			{
				wxCommandEvent evt(wxEVT_CREATE_NEW_PROCESS);
				
				evt.SetExtraLong((long)ulProcessID);
				wxPostEvent(pProcessPanel, evt);
			}
		}
	}
}

void CProcessInfoImpl::GetUserFromProcess(PROCESS_INFO* pInfo)
{
	HANDLE hToken = NULL;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pInfo->_ulProcessID); 
    if(hProcess == NULL)
        return;
    
    if( !OpenProcessToken( hProcess, TOKEN_QUERY, &hToken ) )
    {
        CloseHandle( hProcess );
        return;
    }

	if (NULL == hToken)
		return;
	
	PTOKEN_USER ptu = NULL;
	DWORD dwSize = MAX_PATH;
	DWORD dwLength = 0;
	
	if (!GetTokenInformation(
         hToken,         // handle to the access token
         TokenUser,      // get information about the token's groups 
         (LPVOID) ptu,   // pointer to PTOKEN_USER buffer
         0,              // size of buffer
         &dwLength       // receives required buffer size
      )) 
	
	ptu = (PTOKEN_USER)HeapAlloc(GetProcessHeap(),	HEAP_ZERO_MEMORY, dwLength);

	if (ptu == NULL)
		return;
   
    if (!GetTokenInformation(
         hToken,         // handle to the access token
         TokenUser,      // get information about the token's groups 
         (LPVOID) ptu,   // pointer to PTOKEN_USER buffer
         dwLength,       // size of buffer
         &dwLength       // receives required buffer size
         )) 
		return;
		
	SID_NAME_USE SidType;
    TCHAR lpName[256] = {0x00, };
    TCHAR lpDomain[256] = {0x00, };

    if( !LookupAccountSid( NULL , ptu->User.Sid, lpName, &dwSize, lpDomain, &dwSize, &SidType ) )                                    
    {
        DWORD dwResult = GetLastError();
        if( dwResult == ERROR_NONE_MAPPED )
			wcscpy(lpName, _T("NONE_MAPPED"));
    }
    else
    {
		pInfo->_strUserName = lpName;
		pInfo->_strDomainName = lpDomain;
		pInfo->_strDomainAndUser = wxString::Format(TEXT("%s\\%s"), lpDomain, lpName);
    }

	if (ptu != NULL)
      HeapFree(GetProcessHeap(), 0, (LPVOID)ptu);

    CloseHandle( hToken );
    CloseHandle( hProcess );
}

void CProcessInfoImpl::GetProcessName(unsigned long ulProcessID, wxString& strProcessName)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ulProcessID);
	if(hProcess == INVALID_HANDLE_VALUE)
		return;
		
	TCHAR buffer[MAX_PATH] = {0x00, };
	DWORD buffer_size = MAX_PATH;
	
	if(QueryFullProcessImageName(hProcess, 0, buffer, &buffer_size))
		strProcessName = wxString::Format(wxT("%s"), buffer);
		
	CloseHandle(hProcess);
}

//int CProcessInfoImpl::SetUserNameAndDomainNameFromSid(PROCESS_INFO* pInfo, const wxString& strFullPath)
int CProcessInfoImpl::SetUserNameAndDomainNameFromSid(PROCESS_INFO* pInfo, const wxString& strFullPath, PSID pUserSid)
{
	DWORD dwErrorCode = 0;
	BOOL bRtnBool = TRUE;
	DWORD dwAcctName = 1, dwDomainName = 1;
	SID_NAME_USE eUse = SidTypeUnknown;
	
	TCHAR AcctName[255] = {0x00, };
	TCHAR DomainName[255] = {0x00, };
	// First call to LookupAccountSid to get the buffer sizes.
	bRtnBool = LookupAccountSid(
                  NULL,           // local computer
                  pUserSid,
                  AcctName,
                  (LPDWORD)&dwAcctName,
                  DomainName,
                  (LPDWORD)&dwDomainName,
                  &eUse);
	
	
    // Second call to LookupAccountSid to get the account name.
    bRtnBool = LookupAccountSid(
          NULL,                   // name of local or remote computer
          pUserSid,              // security identifier
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
		
	//	if(!SetUserNameAndDomainNameFromSid(pInfo))
	//		return 0;
			
		return -1;
	} 
	
	pInfo->_strUserName = wxString::Format(TEXT("%s"), AcctName);
	pInfo->_strDomainName = wxString::Format(TEXT("%s"), DomainName);
	pInfo->_strDomainAndUser = wxString::Format(TEXT("%s\\%s"), DomainName, AcctName);
	
	return 0;
}

int CProcessInfoImpl::SetUserNameAndDomainNameFromSid(PROCESS_INFO* pInfo)
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

void CProcessInfoImpl::GetUserNameFromToken(PROCESS_INFO* pInfo)
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

void CProcessInfoImpl::DelProcess(unsigned long ulProcessID)
{
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator fIter = m_mapProcesses.find(ulProcessID);
	if (fIter == m_mapProcesses.end())
		return;
	
	PROCESS_INFO* pProcessInfo = fIter->second;
	if(pProcessInfo->_hProcess != nullptr)
		CloseHandle(pProcessInfo->_hProcess);
	
	delete pProcessInfo;
	
	m_mapProcesses.erase(fIter);
	
	if(theSystemInfo->GetPGTerminate())
		return;
		
	//이벤트 전송
	CPanelProcessTotal* pProcessPanel = (CPanelProcessTotal *)thePanelManager->GetPanel(PANEL_PROCESS_TOTAL);
	if(pProcessPanel)
	{
		wxCommandEvent evt(wxEVT_TERMINATE_AND_DEL_PROCESS);
		evt.SetExtraLong((long)ulProcessID);
		wxPostEvent(pProcessPanel, evt);
	}
}

void CProcessInfoImpl::GetProcessFullPathName(unsigned long ulProcessID, wxString& strProcessPath)
{
#ifdef __WXMSW__
	DWORD dwLen = 0;
	TCHAR szImagePath[MAX_PATH] = { 0, };

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, ulProcessID);
	if (hProcess == nullptr)
	{
		strProcessPath = wxT("");
		return;
	}

	strProcessPath = wxT("<Unknown>");
	ZeroMemory(szImagePath, sizeof(szImagePath));
	dwLen = sizeof(szImagePath) / sizeof(TCHAR);

	if (QueryFullProcessImageName(hProcess, 0, szImagePath, &dwLen))
		strProcessPath = szImagePath;

	CloseHandle(hProcess);
#else

#endif
}

void CProcessInfoImpl::InitCPUFromProcess(PROCESS_INFO* pProcessInfo)
{
	//cpu 사용시간
	FILETIME ftime, fsys, fuser;
	//System시간을 읽어옴
	GetSystemTimeAsFileTime(&ftime);
	memcpy(&pProcessInfo->_lastCPU, &ftime, sizeof(FILETIME));
	//오픈한 프로세스에 대한 사용시간 읽어옴
	GetProcessTimes(pProcessInfo->_hProcess, &ftime, &ftime, &fsys, &fuser);
	memcpy(&pProcessInfo->_lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&pProcessInfo->_lastUserCPU, &fuser, sizeof(FILETIME));

/*	__int64  nCreationTime = 0;
	__int64  nExitTime = 0;
	__int64  nKernelTimeOld = 0;
	__int64  nUserTimeOld = 0;

	__int64  nTickCountOld = 0;

	nTickCountOld = GetTickCount64();
	if (!GetProcessTimes(pProcessInfo->_hProcess, (LPFILETIME)&nCreationTime, (LPFILETIME)&nExitTime, (LPFILETIME)&nKernelTimeOld, (LPFILETIME)&nUserTimeOld))
		return;
	
	pProcessInfo->nKernelTimeOld = nKernelTimeOld;
	pProcessInfo->nUserTimeOld = nUserTimeOld;
	pProcessInfo->nTickCountOld = nTickCountOld;
*/
}

void CProcessInfoImpl::SetProcessCPUUsage(PROCESS_INFO* pProcessInfo)
{
	if (pProcessInfo->_hProcess == nullptr)
		return;

	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	float percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));
	
	GetProcessTimes(pProcessInfo->_hProcess, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = (sys.QuadPart - pProcessInfo->_lastSysCPU.QuadPart) + (user.QuadPart - pProcessInfo->_lastUserCPU.QuadPart);
	percent /= (now.QuadPart - pProcessInfo->_lastCPU.QuadPart);
	percent /= theSystemInfo->GetCPUCoreCount();
	pProcessInfo->_lastCPU = now;
	pProcessInfo->_lastUserCPU = user;
	pProcessInfo->_lastSysCPU = sys;
	pProcessInfo->fCPUUsage = percent * 100.0f;
	
//	return percent * 100.0f;
/*
	__int64  nCreationTime = 0;
	__int64  nExitTime = 0;
	__int64  nKernelTimeNew = 0;
	__int64  nUserTimeNew = 0;

	__int64  nTickCountNew = 0;
	__int32  nCpuUsagePercent = 0;
	
	nTickCountNew = GetTickCount64();
	if (!GetProcessTimes(pProcessInfo->_hProcess,  (LPFILETIME)&nCreationTime, (LPFILETIME)&nExitTime, (LPFILETIME)&nKernelTimeNew, (LPFILETIME)&nUserTimeNew))
		return -1;

	nCpuUsagePercent = (__int32)((nKernelTimeNew - pProcessInfo->nKernelTimeOld + nUserTimeNew - pProcessInfo->nUserTimeOld) / (100 * (theSystemInfo->GetCPUCoreCount() * (nTickCountNew - pProcessInfo->nTickCountOld))));
	
	pProcessInfo->nKernelTimeOld = nKernelTimeNew;
	pProcessInfo->nUserTimeOld = nUserTimeNew;
	pProcessInfo->nTickCountOld = nTickCountNew;
	
	return nCpuUsagePercent * 1.0;
*/
}

bool CProcessInfoImpl::GetExistProcess(unsigned long ulProcessID)
{
	bool bReturn = true;

#ifdef __WXMSW__
	if (ulProcessID == 0 || ulProcessID == 4)
		return true;
			
	PROCESS_INFO* pProcessInfo = GetProcessInfo(ulProcessID);
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

void CProcessInfoImpl::SetProcessMemorySize(PROCESS_INFO* pProcessInfo)
{
#ifdef __WXMSW__
	PROCESS_MEMORY_COUNTERS_EX pmc;
	BOOL bInfo = GetProcessMemoryInfo(pProcessInfo->_hProcess, (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));

	if (bInfo)
	{
		pProcessInfo->_PrivateSize = pmc.PrivateUsage;
		pProcessInfo->_WorkingSetSize = pmc.WorkingSetSize;
	}
#else

#endif
}

std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator CProcessInfoImpl::GetIteratorBegin() const
{
	return m_mapProcesses.begin();
}

std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator CProcessInfoImpl::GetIteratorEnd() const
{
	return m_mapProcesses.end();
}

PROCESS_INFO* CProcessInfoImpl::GetProcessInfo(unsigned long ulProcessID)
{
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator fIter = m_mapProcesses.find(ulProcessID);
	if (fIter == m_mapProcesses.end())
		return nullptr;
	
	PROCESS_INFO* pProcessInfo = fIter->second;
	if (pProcessInfo == nullptr)
		return nullptr;
		
	return pProcessInfo;
}

void CProcessInfoImpl::UpdateInfo()
{
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator iter = GetIteratorBegin();
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator iterEnd = GetIteratorEnd();
	
	while (iter != iterEnd)
	{
		PROCESS_INFO* pProcessInfo = iter->second;
		
		SetProcessCPUUsage(pProcessInfo);
		SetProcessMemorySize(pProcessInfo);
		
		++iter;
	}
}

int CProcessInfoImpl::KillProcess(unsigned long ulProcessID)
{
	//프로세스 항목이 존재하는지 검사만 한다.
	//프로세스가 종료되고 나면 DelProcess() 함수를 CWatchProcess 에서 호출하게 되므로
	//PROCESS_INFO의 _hProcess Handle의 Close는 DelProcess에서 처리
	PROCESS_INFO* pProcess = GetProcessInfo(ulProcessID);
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

#define _WIN32_DCOM

#include <tchar.h>
#include <wbemidl.h>
#include <comutil.h>
#include <oleauto.h>

bool CProcessInfoImpl::GetCommandLineOfProcess(unsigned long ulProcessID, wxString& strCommandLine)
{
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
	return true;
}