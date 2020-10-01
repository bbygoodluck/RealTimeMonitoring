#ifndef CCPUINFOIMPL_H
#define CCPUINFOIMPL_H

#ifdef __WXMSW__
	#include <Pdh.h>
	#include <PdhMsg.h>
	#include <Iphlpapi.h>
#endif

#include "../SystemInfoImpl.h"
class CCPUInfoImpl : public CSystemInfoImpl
{
public:
	CCPUInfoImpl();
	~CCPUInfoImpl();
	
public:
	virtual void UpdateInfo() override;
	
	CPUINFO* GetCPUInfo() {
		return m_pCPUInfo;
	}

private:
	void Initialize();
	
private:
	CPUINFO* m_pCPUInfo;
	
	size_t    m_CPUCoreCount;
	unsigned long* m_pArrayCore = nullptr;
	
	HQUERY m_hCpuQuery;

	HCOUNTER  m_hCounterCPUTotal;
	HCOUNTER* m_phCounterCPUCore = nullptr;
};

#endif // CCPUINFOIMPL_H
