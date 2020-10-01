#include "../../ginc.h"
#include "MemoryInfoImpl.h"

CMemoryInfoImpl::CMemoryInfoImpl()
	: m_pMemInfo(nullptr)
	, m_pPageFileInfo(nullptr)
{
	Initialize();
}

CMemoryInfoImpl::~CMemoryInfoImpl()
{
	if(m_pMemInfo)
		delete m_pMemInfo;
		
	m_pMemInfo = nullptr;
	
	if(m_pPageFileInfo)
		delete m_pPageFileInfo;
		
	m_pPageFileInfo = nullptr;
}

void CMemoryInfoImpl::Initialize()
{
	m_pMemInfo      = new MEMINFO();
	m_pPageFileInfo = new PAGEFILEINFO();
}

void CMemoryInfoImpl::UpdateInfo()
{
	MEMORYSTATUSEX MemoryStatus = { 0 };
	MemoryStatus.dwLength = sizeof(MemoryStatus);
	::GlobalMemoryStatusEx(&MemoryStatus);

	m_pMemInfo->ullPhyscialAvaRam = ((MemoryStatus.ullTotalPhys - MemoryStatus.ullAvailPhys) / (1024 * 1024));
	m_pMemInfo->ullPhysicalTotalRam = ((MemoryStatus.ullTotalPhys) / (1024 * 1024));

	m_pMemInfo->ullVirtualAvaRam = ((MemoryStatus.ullTotalVirtual - MemoryStatus.ullAvailVirtual) / (1024 * 1024));
	m_pMemInfo->ullVirtualTotalRam = ((MemoryStatus.ullTotalVirtual) / (1024 * 1024));
	
	m_pPageFileInfo->ullPageFileAva   = ((MemoryStatus.ullTotalPageFile - MemoryStatus.ullAvailPageFile) / (1024 * 1024));
	m_pPageFileInfo->ullPageFileTotal = ((MemoryStatus.ullTotalPageFile) / (1024 * 1024));
}