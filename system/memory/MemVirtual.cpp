#include "../../ginc.h"
#include "MemVirtual.h"

CMemVirtual::CMemVirtual()
{

}

CMemVirtual::~CMemVirtual()
{

}

void CMemVirtual::Initialize()
{
	UpdateMemInfo();
}

void CMemVirtual::UpdateMemInfo()
{
	Update();

		//가상메모리
	_ullTotalVirtual = _memroyStatus.ullTotalVirtual / MEGABYTE;
	_ullAvailVirtual = _memroyStatus.ullAvailVirtual / MEGABYTE;
	_ullUsedVirtual  = (_memroyStatus.ullTotalVirtual - _memroyStatus.ullAvailVirtual) / MEGABYTE;

}

unsigned long CMemVirtual::GetTotalMemory() const
{
	return _ullTotalVirtual;
}

unsigned long CMemVirtual::GetAvaliableMemory() const
{
	return _ullAvailVirtual;
}

unsigned long CMemVirtual::GetUsedMemory() const
{
	return _ullUsedVirtual;
}
