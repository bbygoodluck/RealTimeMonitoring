#include "../../ginc.h"
#include "MemPhysics.h"

CMemPhysics::CMemPhysics()
{
	Initialize();
}

CMemPhysics::~CMemPhysics()
{

}

void CMemPhysics::UpdateMemInfo()
{
	Update();

	//물리메모리
	_ullTotalPhys  = _memroyStatus.ullTotalPhys / MEGABYTE;
	_ullAvailPhys  = _memroyStatus.ullAvailPhys / MEGABYTE;
	_ullUsedPhys   = (_memroyStatus.ullTotalPhys - _memroyStatus.ullAvailPhys) / MEGABYTE;
}

void CMemPhysics::Initialize()
{
	UpdateMemInfo();
}

unsigned long CMemPhysics::GetTotalMemory() const
{
	return _ullTotalPhys;
}
unsigned long CMemPhysics::GetAvaliableMemory() const
{
	return _ullAvailPhys;
}

unsigned long CMemPhysics::GetUsedMemory() const
{
	return _ullUsedPhys;
}

