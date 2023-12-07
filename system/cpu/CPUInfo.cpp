#include "../../ginc.h"
#include "CPUInfo.h"

CCPUInfo::CCPUInfo()
{
	Initialize();
}

CCPUInfo::~CCPUInfo()
{
	CResourceCommon::Clear();
	CResourceCommon::Stop();

	this->Disconnect(wxEVT_TIMER, wxTimerEventHandler(CCPUInfo::OnTimer));
#ifdef __WXMSW__
	PdhCloseQuery(m_hCpuQuery);
	PdhRemoveCounter(m_hCounterCPUTotal);

	for (size_t cnt = 0; cnt < m_CPUCoreCount; cnt++)
		PdhRemoveCounter(m_phCounterCPUCore[cnt]);

	wxDELETE(m_phCounterCPUCore);
	delete[] m_pArrayCore;
#endif // __WXMSW__
}

void CCPUInfo::Initialize()
{
	this->Connect(wxEVT_TIMER, wxTimerEventHandler(CCPUInfo::OnTimer));
	m_timer.SetOwner(this, wxTIMER_ID++);

#ifdef __WXMSW__
	PDH_STATUS status = PdhOpenQuery(NULL, 0, &m_hCpuQuery);
	if(status != ERROR_SUCCESS)
		return;

	/*
	if( PdhAddEnglishCounter(m_hCpuQuery, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &m_hCounterCPUTotal) != ERROR_SUCCESS ||
		PdhCollectQueryData(m_hCpuQuery) != ERROR_SUCCESS)
	{
		PdhCloseQuery(m_hCpuQuery);
		return;
	}
	*/

	status = PdhAddCounter(m_hCpuQuery, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &m_hCounterCPUTotal);
	if(status != ERROR_SUCCESS)
		return;

	// CPU core 정보
	SYSTEM_INFO SystemInfo = { 0 };
	GetSystemInfo(&SystemInfo);

	if (SystemInfo.dwNumberOfProcessors > 0)
	{
		m_CPUCoreCount = SystemInfo.dwNumberOfProcessors;

		m_phCounterCPUCore = new HCOUNTER[m_CPUCoreCount];
		//CPU코어 수만큼 생성하고 0으로 초기화
		m_pArrayCore = new unsigned long[m_CPUCoreCount]();

		for (size_t cnt = 0; cnt < m_CPUCoreCount; cnt++)
		{
			TCHAR szFullCounterPath[1024] = { 0x00, };
			wsprintf(szFullCounterPath, TEXT("\\Processor(%d)\\%% Processor Time"), cnt);
			PDH_STATUS status = PdhAddCounter(m_hCpuQuery, szFullCounterPath, 1, &m_phCounterCPUCore[cnt]);
			assert(status == ERROR_SUCCESS);
		}
	}

	status = PdhCollectQueryData( m_hCpuQuery );
	if(status != ERROR_SUCCESS)
		return;


	m_bPhdOK = true;

#endif // __WXMSW__
}

void CCPUInfo::OnTimer(wxTimerEvent& event)
{
	Update();
	SendEvent(wxEVT_RESOURCE_MONITORING);

	event.Skip();
}

void CCPUInfo::Update()
{
#ifdef __WXMSW__
	if(!m_bPhdOK)
		return;

	PDH_STATUS status = PdhCollectQueryData(m_hCpuQuery);

	if (status == ERROR_SUCCESS)
	{
		DWORD dwTypeTotal;
		// 전체 CPU 사용량
		PDH_FMT_COUNTERVALUE PFC_Total = {0, };
		status = PdhGetFormattedCounterValue(m_hCounterCPUTotal, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, &dwTypeTotal, &PFC_Total);
		if (status == ERROR_SUCCESS)
			m_ulCPUTotalUage = (unsigned long)PFC_Total.doubleValue;
	}

	// 개별 코어 사용량
	for (size_t c1 = 0; c1 < m_CPUCoreCount; c1++)
	{
		DWORD dwTypeCore;
		PDH_FMT_COUNTERVALUE PFC_Value = {0, };
		status = PdhGetFormattedCounterValue(m_phCounterCPUCore[c1], PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, &dwTypeCore, &PFC_Value);
		if (status != ERROR_SUCCESS)
		{
			m_pArrayCore[c1] = 0;
			continue;
		}

		m_pArrayCore[c1] = (unsigned long)PFC_Value.doubleValue;
	}
#else
	FILETIME sysIdle, sysKernel, sysUser;
	// GetSystemTimes func FAILED return
	if (GetSystemTimes(&sysIdle, &sysKernel, &sysUser) == 0)
        return;

	//lamda function
	auto SubSysTimes = [](const FILETIME ft1, const FILETIME ft2) {
		ULONGLONG a,b = 0;

		memcpy( &a, &ft1, sizeof (ULONGLONG) );
		memcpy( &b, &ft2, sizeof (ULONGLONG) );

		return a - b;
	};

	ULONGLONG ullsysIdleDiff, ullsysKernelDiff, ullsysUserDiff;
	ullsysIdleDiff = SubSysTimes(sysIdle, m_ftPrevSysIdle);
	ullsysKernelDiff = SubSysTimes(sysKernel, m_ftPrevSysKernel);
	ullsysUserDiff = SubSysTimes(sysUser, m_ftPrevSysUser);

	ULONGLONG ullSysTotall = ullsysKernelDiff + ullsysUserDiff;
	// kernelTime - IdleTime = kernelTime, because sysKernel include IdleTime
	ULONGLONG ullkernelTotal = ullsysKernelDiff - ullsysIdleDiff;

	double dblCPUUsage = 0.0f;
	// sometimes kernelTime > idleTime
	if (ullSysTotall == 0)
		return;

	dblCPUUsage = (double)(((ullkernelTotal + ullsysUserDiff) * 100.0) / ullSysTotall);
	m_ulCPUTotalUage = (unsigned long)dblCPUUsage;

	m_ftPrevSysIdle = sysIdle;
    m_ftPrevSysKernel = sysKernel;
    m_ftPrevSysUser = sysUser;
#endif
}

unsigned long CCPUInfo::GetCPUUsage(int iCPUCoreIndex)
{
#ifdef __WXMSW__
	if(!m_bPhdOK)
		return 0;

	if(iCPUCoreIndex > m_CPUCoreCount)
		return 0;

	return iCPUCoreIndex == -1 ? m_ulCPUTotalUage : m_pArrayCore[iCPUCoreIndex];
#endif // __WXMSW__
}
