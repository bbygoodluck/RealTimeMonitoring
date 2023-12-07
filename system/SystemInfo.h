#ifndef SYSTEMINFO_H_INCLUDED
#define SYSTEMINFO_H_INCLUDED

class CProcessorInfo;
class CCPUInfo;
class CMemoryInfo;
class CProcessInfo;
class CNetworkInfo;

class CSystemInfo
{
private:
	explicit CSystemInfo() {}

public:
	~CSystemInfo();
	static CSystemInfo* Get();

	void Initialize();

	CProcessorInfo* GetProcessor() {
		if(m_Processor == nullptr)
			return nullptr;

		return m_Processor;
	}

	CCPUInfo* GetCPUInfo() {
		if(m_cpu == nullptr)
			return nullptr;

		return m_cpu;
	}

	CMemoryInfo* GetMemoryInfo() {
		if(m_memory == nullptr)
			return nullptr;

		return m_memory;
	}

	CProcessInfo* GetProcessInfo() {
		if(m_process == nullptr)
			return nullptr;

		return m_process;
	}

	CNetworkInfo* GetNetInfo() {
		if(m_netInfo == nullptr)
			return nullptr;

		return m_netInfo;
	}

protected:
	CProcessorInfo* m_Processor = nullptr;
	CCPUInfo*       m_cpu       = nullptr;
	CMemoryInfo*    m_memory    = nullptr;
	CProcessInfo*   m_process   = nullptr;
	CNetworkInfo*   m_netInfo   = nullptr;

private:
	static std::unique_ptr<CSystemInfo> m_pInstance;
	bool m_bProgramTerminated = false;
};

#endif

