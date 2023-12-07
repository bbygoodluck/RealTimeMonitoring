#include "../../ginc.h"
#include "MemoryCommon.h"

CMemoryCommon::CMemoryCommon()
{

}

CMemoryCommon::~CMemoryCommon()
{

}

void CMemoryCommon::Update()
{
	wxZeroMemory(_memroyStatus);
	_memroyStatus.dwLength = sizeof(_memroyStatus);
	::GlobalMemoryStatusEx(&_memroyStatus);
}
