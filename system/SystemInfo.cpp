#include "../ginc.h"
#include "SystemInfo.h"
#include "./cpu/CPUInfoImpl.h"
#include "./memory/MemoryInfoImpl.h"
#include "./process/ProcessInfoImpl.h"

std::unique_ptr<CSystemInfo> CSystemInfo::m_pInstance = nullptr;

CSystemInfo* CSystemInfo::Get()
{
	if (m_pInstance.get() == nullptr)
		m_pInstance.reset(new CSystemInfo());

	return m_pInstance.get();
}

CSystemInfo::CSystemInfo()
	: m_pCPUInfoImpl(nullptr)
	, m_pMemInfoImpl(nullptr)
	, m_pProcessImpl(nullptr)
{
	m_pCPUInfoImpl = std::make_unique<CCPUInfoImpl>();
	m_pMemInfoImpl = std::make_unique<CMemoryInfoImpl>();
	m_pProcessImpl = std::make_unique<CProcessInfoImpl>();
}

CSystemInfo::~CSystemInfo()
{
}

unsigned int CSystemInfo::GetCPUCoreCount()
{
	return m_pCPUInfoImpl->GetCPUInfo()->_iCpuCoreCount;
}

void CSystemInfo::UpdateInfo(_PANEL_TYPE _type)
{
	switch(_type)
	{
		case PANEL_CPU_GRAPH:
			m_pCPUInfoImpl->UpdateInfo();
			break;
			
		case PANEL_SPECTRUM_MEMORY:
			m_pMemInfoImpl->UpdateInfo();
			break;
			
		case PANEL_PROCESS_TOTAL:
			m_pProcessImpl->UpdateInfo();
			break;
			
		default:
			break;
	}
}

void CSystemInfo::UpdateInfo()
{
	m_pCPUInfoImpl->UpdateInfo();
	m_pMemInfoImpl->UpdateInfo();
}

unsigned long CSystemInfo::GetCPUUsage()
{
	return m_pCPUInfoImpl->GetCPUInfo()->_ulCPUTotalUage;
}

unsigned long CSystemInfo::GetCPUCoreUsage(int iIndex)
{
	return m_pCPUInfoImpl->GetCPUInfo()->_pArrayCore[iIndex];
}

wxString CSystemInfo::GetProcessorName() const
{
	return m_pCPUInfoImpl->GetProcessorName();
}

void CSystemInfo::GetMemoryInfo(unsigned long long &ullTotalRam, unsigned long long &ullAvaRam)
{
	ullTotalRam = m_pMemInfoImpl->GetMemInfo()->ullPhysicalTotalRam;
	ullAvaRam   = m_pMemInfoImpl->GetMemInfo()->ullPhyscialAvaRam;
}

void CSystemInfo::GetPageFileInfo(unsigned long long &ullTotalPage, unsigned long long &ullAvaPage)
{
	ullTotalPage = m_pMemInfoImpl->GetPageFileInfo()->ullPageFileTotal;
	ullAvaPage   = m_pMemInfoImpl->GetPageFileInfo()->ullPageFileAva;
}

std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator CSystemInfo::GetProcessIterBegin()
{
	return m_pProcessImpl->GetIteratorBegin();
}

std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator CSystemInfo::GetProcessIterEnd()
{
	return m_pProcessImpl->GetIteratorEnd();
}

bool CSystemInfo::GetExistProcess(unsigned long ulProcessID)
{
	return m_pProcessImpl->GetExistProcess(ulProcessID);
}

PROCESS_INFO* CSystemInfo::GetProcessInfo(unsigned long ulProcessID)
{
	return m_pProcessImpl->GetProcessInfo(ulProcessID);
}

CProcessInfoImpl* CSystemInfo::GetProcessObj()
{
	return m_pProcessImpl.get();
}

int CSystemInfo::KillProcess(unsigned long ulProcessID)
{
	return m_pProcessImpl->KillProcess(ulProcessID);
}

bool CSystemInfo::GetCommandLineOfProcess(unsigned long ulProcessID, wxString& strCommandLine)
{
	return m_pProcessImpl->GetCommandLineOfProcess(ulProcessID, strCommandLine);
}