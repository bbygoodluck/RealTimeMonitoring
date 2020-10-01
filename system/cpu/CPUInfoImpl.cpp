#include "../../ginc.h"
#include "CPUInfoImpl.h"

CCPUInfoImpl::CCPUInfoImpl()
{
	Initialize();
}

CCPUInfoImpl::~CCPUInfoImpl()
{
#ifdef __WXMSW__
	PdhRemoveCounter(m_hCounterCPUTotal);

	for (size_t cnt = 0; cnt < m_CPUCoreCount; cnt++)
		PdhRemoveCounter(m_phCounterCPUCore[cnt]);

	delete(m_phCounterCPUCore);

	delete[] m_pCPUInfo->_pArrayCore;
	delete m_pCPUInfo;
#endif
}

void CCPUInfoImpl::Initialize()
{
#ifdef __WXMSW__
	m_pCPUInfo = new CPUINFO();
	
	PDH_STATUS status;
	status = PdhOpenQuery(NULL, 0, &m_hCpuQuery);
	assert(status == ERROR_SUCCESS);
	//CPU Total
	status = PdhAddCounter(m_hCpuQuery, TEXT("\\Processor(_Total)\\% Processor Time"), 1, &m_hCounterCPUTotal);
	assert(status == ERROR_SUCCESS);

	// CPU core 정보
	SYSTEM_INFO SystemInfo = { 0 };
	GetSystemInfo(&SystemInfo);
	if (SystemInfo.dwNumberOfProcessors > 0) 
	{
		m_CPUCoreCount = SystemInfo.dwNumberOfProcessors;
		m_pCPUInfo->_iCpuCoreCount = m_CPUCoreCount;
		
		m_phCounterCPUCore = new HCOUNTER[m_CPUCoreCount];
		//CPU코어 수만큼 생성하고 0으로 초기화
		m_pCPUInfo->_pArrayCore = new unsigned long[m_CPUCoreCount]();

		for (size_t cnt = 0; cnt < m_CPUCoreCount; cnt++) 
		{
			TCHAR szFullCounterPath[1024] = { 0 };
			wsprintf(szFullCounterPath, TEXT("\\Processor(%d)\\%% Processor Time"), cnt);
			PDH_STATUS status = PdhAddCounter(m_hCpuQuery, szFullCounterPath, 1, &m_phCounterCPUCore[cnt]);
			assert(status == ERROR_SUCCESS);
		}
	}
#endif
}

void CCPUInfoImpl::UpdateInfo()
{
#ifdef __WXMSW__
	PdhCollectQueryData(m_hCpuQuery);
	// 전체 CPU 사용량
	PDH_FMT_COUNTERVALUE PFC_Value = { 0 };
	PDH_STATUS status = PdhGetFormattedCounterValue(m_hCounterCPUTotal, PDH_FMT_DOUBLE, NULL, &PFC_Value);
	if (status != ERROR_SUCCESS)
	{
		m_pCPUInfo->_ulCPUTotalUage = 0;
		return;
	}

	m_pCPUInfo->_ulCPUTotalUage = (unsigned long)PFC_Value.doubleValue;
	
	// 개별 코어 사용량
	for (size_t c1 = 0; c1 < m_CPUCoreCount; c1++)
	{
		PDH_FMT_COUNTERVALUE PFC_Value = { 0 };
		PdhGetFormattedCounterValue(m_phCounterCPUCore[c1], PDH_FMT_DOUBLE, NULL, &PFC_Value);
		m_pCPUInfo->_pArrayCore[c1] = (unsigned long)PFC_Value.doubleValue;
	}
#endif
}