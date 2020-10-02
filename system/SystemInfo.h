#ifndef CSYSTEMINFO_HPP
#define CSYSTEMINFO_HPP

class CCPUInfoImpl;
class CMemoryInfoImpl;
class CProcessInfoImpl;

class CSystemInfo
{
private:
	explicit CSystemInfo();

public:
	~CSystemInfo();
	static CSystemInfo* Get();
	
	unsigned long GetCPUUsage();
	unsigned long GetCPUCoreUsage(int iIndex);
	unsigned int GetCPUCoreCount();
	void GetMemoryInfo(unsigned long long &ullTotalRam, unsigned long long &ullAvaRam);
	void GetPageFileInfo(unsigned long long &ullTotalPage, unsigned long long &ullAvaPage);
	void UpdateInfo(_PANEL_TYPE _type);
	void UpdateInfo();
	
	//프로세스 존재여부 확인
	bool GetExistProcess(unsigned long ulProcessID);
	//프로세스 저장맵 iterator
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator GetProcessIterBegin();
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator GetProcessIterEnd();
	PROCESS_INFO* GetProcessInfo(unsigned long ulProcessID);
	
	CProcessInfoImpl* GetProcessObj();
	int KillProcess(unsigned long ulProcessID);
	
	void SetPGTerminte(bool bTerminate) {m_bProgramTerminated = bTerminate; }
	bool GetPGTerminate() const { return m_bProgramTerminated; }
	bool GetCommandLineOfProcess(unsigned long ulProcessID, wxString& strCommandLine);
	wxString GetProcessorName() const;
	
private:
	static std::unique_ptr<CSystemInfo> m_pInstance;
	
	std::unique_ptr<CCPUInfoImpl> m_pCPUInfoImpl;
	std::unique_ptr<CMemoryInfoImpl> m_pMemInfoImpl;
	std::unique_ptr<CProcessInfoImpl> m_pProcessImpl;
	
	bool m_bProgramTerminated = false;
};

#endif // CSYSTEMINFO_HPP
