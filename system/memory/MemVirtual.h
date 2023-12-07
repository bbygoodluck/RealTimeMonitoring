#ifndef MEMVRITUAL_H_INCLUDED
#define MEMVRITUAL_H_INCLUDED

#include "../ISystemCommon.h"
#include "MemoryCommon.h"

class CMemVirtual : public ISystemCommon, public CMemoryCommon
{
public:
	CMemVirtual();
	~CMemVirtual();

public:
	void UpdateMemInfo() override;

	unsigned long GetTotalMemory() const override;
	unsigned long GetAvaliableMemory()const override;
	unsigned long GetUsedMemory()const override;

private:
	void Initialize() override;

};

#endif // MEMVRITUAL_H_INCLUDED
