#ifndef _MEMORY_INFO_IMPL_H_INCLUDED
#define _MEMORY_INFO_IMPL_H_INCLUDED

#include "../SystemInfoImpl.h"
class CMemoryInfoImpl : public CSystemInfoImpl
{
public:
	CMemoryInfoImpl();
	~CMemoryInfoImpl();
	
public:
	virtual void UpdateInfo() override;
	MEMINFO* GetMemInfo() { return m_pMemInfo; }
	PAGEFILEINFO* GetPageFileInfo() { return m_pPageFileInfo; }
private:
	void Initialize();
	
private:
	MEMINFO* m_pMemInfo;
	PAGEFILEINFO* m_pPageFileInfo;
	
};
#endif