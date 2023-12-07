#ifndef MEMORYINFO_H_INCLUDED
#define MEMORYINFO_H_INCLUDED

#include "../ResourceCommon.h"
class CMemoryInfo : public CResourceCommon
{
public:
	CMemoryInfo();
	~CMemoryInfo();

public:
	unsigned long GetTotalMemory(USAGE_TYPE type);
	unsigned long GetAvaliableMemory(USAGE_TYPE type);
	unsigned long GetUsedMemory(USAGE_TYPE type);

private:
	void Initialize() override;
	void Update() override;

private:
#ifdef __WXMSW__
	MEMORYSTATUSEX _memroyStatus = {0x00, };
#else
#endif // __WXMSW__

	//물리메모리
	unsigned long _ullTotalPhys  = 0;
	unsigned long _ullUsedPhys   = 0;
	unsigned long _ullAvailPhys  = 0;
	//가상메모리
	unsigned long _ullTotalVirtual = 0;
	unsigned long _ullUsedVirtual  = 0;
	unsigned long _ullAvailVirtual = 0;
	//페이지파일
	unsigned long _ullTotalPageFile = 0;
	unsigned long _ullUsedPageFile  = 0;
	unsigned long _ullAvailPageFile = 0;

	unsigned long arrTotalMem[USAGE_MAX] = {0,};
	unsigned long arrAvaliableMem[USAGE_MAX] = {0,};
	unsigned long arrUsedMem[USAGE_MAX] = {0,};

private:
	void OnTimer(wxTimerEvent& event) override;
};

#endif // MEMORYINFO_H_INCLUDED
