#ifndef PROCESSINFO_H_INCLUDED
#define PROCESSINFO_H_INCLUDED

#include "../ResourceCommon.h"
class CWatchProcess;
class CProcessInfo: public CResourceCommon
{
public:
	CProcessInfo();
	~CProcessInfo();

	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator Begin();
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator End();

public:
	void UpdateProcesses();
	PROCESS_INFO* UpdateProcessInfo(unsigned long _ulProcessId);
	PROCESS_INFO* GetInfo(unsigned long ulProcessID);
	void DelProcess(unsigned long ulProcessID);
	int KillProcess(unsigned long ulProcessID);

	//프로세스 존재여부 확인
	bool GetExistProcess(unsigned long ulProcessID);
	bool GetCommandLineOfProcess(unsigned long ulProcessID, wxString& strCommandLine);

#ifdef __WXMSW__
	void AddProcessInfo(const WTS_PROCESS_INFO& wtsProcessInfo, bool bNewProcess = false);
#endif
private:
#ifdef __WXMSW__
	void AddProcessInfo(unsigned long ulProcessID, const wxString& _strProcessName);
	bool SetPrivilige(LPCTSTR lpszPrivilege, bool bEnablePrivilege = true);
	int SetUserNameAndDomainNameFromSid(PROCESS_INFO* pInfo);
	void GetUserNameFromToken(PROCESS_INFO* pInfo);
#else
#endif

	//Process CPU
	void InitProcessCPUTime(PROCESS_INFO* _pPsInfo);
	void UpdateProcessCPUTime(PROCESS_INFO* _pPsInfo);
	//Process Memory
	void UpdateProcessMemory(PROCESS_INFO* _pPsInfo);

private:
	std::unordered_map<unsigned long, PROCESS_INFO *> m_mapProcessList;
	std::unique_ptr<CWatchProcess> m_pWatchProcess;

	unsigned int numProcessors = 0;
private:
	void Initialize() override;
	void Update() override;
	void Update(const wxEventType& evtType, long lValue);
	void OnTimer(wxTimerEvent& event) override;
};

#endif // PROCESSINFO_H_INCLUDED
