#include "../../ginc.h"
#include "MemoryInfo.h"

CMemoryInfo::CMemoryInfo()
{
	Initialize();
}

CMemoryInfo::~CMemoryInfo()
{
	CResourceCommon::Clear();
	CResourceCommon::Stop();

	this->Disconnect(wxEVT_TIMER, wxTimerEventHandler(CMemoryInfo::OnTimer));
}

void CMemoryInfo::Initialize()
{
	this->Connect(wxEVT_TIMER, wxTimerEventHandler(CMemoryInfo::OnTimer));
	m_timer.SetOwner(this, wxTIMER_ID++);

	Update();
}

void CMemoryInfo::Update()
{
	wxZeroMemory(_memroyStatus);
	_memroyStatus.dwLength = sizeof(_memroyStatus);
	::GlobalMemoryStatusEx(&_memroyStatus);

	//물리메모리
	_ullTotalPhys  = _memroyStatus.ullTotalPhys / MEGABYTE;
	_ullAvailPhys  = _memroyStatus.ullAvailPhys / MEGABYTE;
	_ullUsedPhys   = (_memroyStatus.ullTotalPhys - _memroyStatus.ullAvailPhys) / MEGABYTE;

	//가상메모리
	_ullTotalVirtual = _memroyStatus.ullTotalVirtual / MEGABYTE;
	_ullAvailVirtual = _memroyStatus.ullAvailVirtual / MEGABYTE;
	_ullUsedVirtual  = (_memroyStatus.ullTotalVirtual - _memroyStatus.ullAvailVirtual) / MEGABYTE;

	//페이지파일
	_ullTotalPageFile = _memroyStatus.ullTotalPageFile / MEGABYTE;
	_ullAvailPageFile = _memroyStatus.ullAvailPageFile / MEGABYTE;
	_ullUsedPageFile  = (_memroyStatus.ullTotalPageFile - _memroyStatus.ullAvailPageFile) / MEGABYTE;

	arrTotalMem[USAGE_MEMORY_PHYSIC] = _ullTotalPhys;
	arrTotalMem[USAGE_MEMORY_VIRTUAL] = _ullTotalVirtual;
	arrTotalMem[USAGE_MEMORY_PAGEFILE] = _ullTotalPageFile;


	arrAvaliableMem[USAGE_MEMORY_PHYSIC] = _ullAvailPhys;
	arrAvaliableMem[USAGE_MEMORY_VIRTUAL] = _ullAvailVirtual;
	arrAvaliableMem[USAGE_MEMORY_PAGEFILE] = _ullAvailPageFile;

	arrUsedMem[USAGE_MEMORY_PHYSIC] = _ullUsedPhys;
	arrUsedMem[USAGE_MEMORY_VIRTUAL] = _ullUsedVirtual;
	arrUsedMem[USAGE_MEMORY_PAGEFILE] = _ullUsedPageFile;
}

void CMemoryInfo::OnTimer(wxTimerEvent& event)
{
	Update();
	SendEvent(wxEVT_RESOURCE_MONITORING);

	event.Skip();
}

unsigned long CMemoryInfo::GetTotalMemory(USAGE_TYPE type)
{
	return arrTotalMem[type];
}

unsigned long CMemoryInfo::GetAvaliableMemory(USAGE_TYPE type)
{
	return arrAvaliableMem[type];
}

unsigned long CMemoryInfo::GetUsedMemory(USAGE_TYPE type)
{
	return arrUsedMem[type];
}
