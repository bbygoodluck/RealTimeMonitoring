#include "../ginc.h"
#include "system.h"
#include "SystemInfo.h"

std::unique_ptr<CSystemInfo> CSystemInfo::m_pInstance = nullptr;

CSystemInfo* CSystemInfo::Get()
{
	if (m_pInstance.get() == nullptr)
		m_pInstance.reset(new CSystemInfo());

	return m_pInstance.get();
}

CSystemInfo::~CSystemInfo()
{
	wxDELETE(m_Processor);
	wxDELETE(m_cpu);
	wxDELETE(m_memory);
	wxDELETE(m_process);
	wxDELETE(m_netInfo);

}

void CSystemInfo::Initialize()
{
	m_Processor = new CProcessorInfo();
	m_cpu       = new CCPUInfo();
	m_memory    = new CMemoryInfo();
	m_process   = new CProcessInfo();
	m_netInfo   = new CNetworkInfo();
}
