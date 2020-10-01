#ifndef CPROCESSINFOIMPL_H
#define CPROCESSINFOIMPL_H

#include "../SystemInfoImpl.h"

class CWatchProcess;
class CProcessInfoImpl : public CSystemInfoImpl
{
	typedef std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator CONSTITER;
	typedef std::unordered_map<unsigned long, PROCESS_INFO *>::iterator ITER;
public:
	CProcessInfoImpl();
	~CProcessInfoImpl();

public:
	virtual void UpdateInfo() override;
	
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator GetIteratorBegin() const;
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator GetIteratorEnd() const;
	
	//프로세스 존재여부 확인
	bool GetExistProcess(unsigned long ulProcessID);
	
	PROCESS_INFO* GetProcessInfo(unsigned long ulProcessID);
	void AddNewProcess(const WTS_PROCESS_INFO& wtsProcessInfo, bool bCreateNew = false);
//	void AddNewProcess(const PROCESSENTRY& pe32, bool bCreateNew = false);
	void DelProcess(unsigned long ulProcessID);
	int KillProcess(unsigned long ulProcessID);
	
	int SetUserNameAndDomainNameFromSid(PROCESS_INFO* pInfo);
	int SetUserNameAndDomainNameFromSid(PROCESS_INFO* pInfo, const wxString& strFullPath, PSID pUserSid);
//	int SetUserNameAndDomainNameFromSid(PROCESS_INFO* pInfo, const wxString& strFullPath);
	void GetUserFromProcess(PROCESS_INFO* pInfo);
	void GetUserNameFromToken(PROCESS_INFO* pInfo);
	void GetProcessName(unsigned long ulProcessID, wxString& strProcessName);
	bool GetCommandLineOfProcess(unsigned long ulProcessID, wxString& strCommandLine);
	
private:
	void Initialize();
	
	void InitCPUFromProcess(PROCESS_INFO* pProcessInfo);
	void GetProcessFullPathName(unsigned long ulProcessID, wxString& strProcessPath);
	//프로세서 CPU사용량
	void SetProcessCPUUsage(PROCESS_INFO* pProcessInfo);
	//프로세스 메모리
	void SetProcessMemorySize(PROCESS_INFO* pProcessInfo);
	bool SetEnableTokenPrivilege(LPCTSTR pszPrivilege);
	
	
private:
	std::unordered_map<unsigned long, PROCESS_INFO *> m_mapProcesses;
	std::unique_ptr<CWatchProcess> m_pWatchProcess;
};

#endif // CPROCESSINFOIMPL_H
