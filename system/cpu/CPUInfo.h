#ifndef CPUINFO_H_INCLUDED
#define CPUINFO_H_INCLUDED

#ifdef __WXMSW__
	#include <Pdh.h>
	#include <PdhMsg.h>
	#include <Iphlpapi.h>
#endif

#include "../ResourceCommon.h"
class CCPUInfo : public CResourceCommon
{
public:
	CCPUInfo();
	~CCPUInfo();

	unsigned long GetCPUUsage(int iCPUCoreIndex = -1);
	size_t GetCoreCount() const { return m_CPUCoreCount; }

private:
	void Initialize() override;
	void Update() override;

private:
	unsigned long  m_ulCPUTotalUage = 0;
	size_t m_CPUCoreCount = 0;

#ifdef __WXMSW__
	unsigned long* m_pArrayCore = nullptr;
	HQUERY m_hCpuQuery;

	HCOUNTER  m_hCounterCPUTotal;
	HCOUNTER* m_phCounterCPUCore = nullptr;

	bool m_bPhdOK = false;
#else
	FILETIME m_ftPrevSysIdle;
	FILETIME m_ftPrevSysKernel;
	FILETIME m_ftPrevSysUser;
#endif // __WXMSW__

private:
	void OnTimer(wxTimerEvent& event) override;
};

#endif // CPUINFO_H_INCLUDED
