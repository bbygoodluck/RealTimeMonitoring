#ifndef MEMPAGEFILE_H_INCLUDED
#define MEMPAGEFILE_H_INCLUDED

#include "../ISystemCommon.h"
#include "MemoryCommon.h"

class CMemPageFile : public ISystemCommon, public CMemoryCommon
{
public:
	CMemPageFile();
	~CMemPageFile();

public:
	void UpdateMemInfo() override;

	unsigned long GetTotalMemory() const override;
	unsigned long GetAvaliableMemory() const override;
	unsigned long GetUsedMemory() const override;

private:
	void Initialize() override;

};

#endif // MEMPAGEFILE_H_INCLUDED
