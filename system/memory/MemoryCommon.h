#ifndef MEMORYINFO_H_INCLUDED
#define MEMORYINFO_H_INCLUDED

class CMemoryCommon
{
public:
	CMemoryCommon() {}
	~CMemoryCommon() {}

public:
	virtual void UpdateMemInfo() = 0;

protected:
	void Update() {
		wxZeroMemory(_memroyStatus);
		_memroyStatus.dwLength = sizeof(_memroyStatus);
		::GlobalMemoryStatusEx(&_memroyStatus);
	}

	virtual unsigned long GetTotalMemory() const = 0;
	virtual unsigned long GetAvaliableMemory()const = 0;
	virtual unsigned long GetUsedMemory()const = 0;

protected:
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
};

#endif // MEMORYINFO_H_INCLUDED
