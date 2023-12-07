#ifndef MEMPHYSICS_H_INCLUDED
#define MEMPHYSICS_H_INCLUDED

#include "../ISystemCommon.h"
#include "MemoryCommon.h"
class CMemPhysics : public ISystemCommon, public CMemoryCommon
{
public:
	CMemPhysics();
	~CMemPhysics();

public:
	void UpdateMemInfo() override;

	unsigned long GetTotalMemory() const override;
	unsigned long GetAvaliableMemory()const override;
	unsigned long GetUsedMemory()const override;

private:
	void Initialize() override;

};
#endif // MEMPHYSICS_H_INCLUDED
