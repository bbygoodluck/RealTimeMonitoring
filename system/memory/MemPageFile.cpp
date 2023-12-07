#include "../../ginc.h"
#include "MemPageFile.h"

CMemPageFile::CMemPageFile()
{

}

CMemPageFile::~CMemPageFile()
{

}

void CMemPageFile::Initialize()
{
	UpdateMemInfo();
}

void CMemPageFile::UpdateMemInfo()
{
	Update();

	//페이지파일
	_ullTotalPageFile = _memroyStatus.ullTotalPageFile / MEGABYTE;
	_ullAvailPageFile = _memroyStatus.ullAvailPageFile / MEGABYTE;
	_ullUsedPageFile  = (_memroyStatus.ullTotalPageFile - _memroyStatus.ullAvailPageFile) / MEGABYTE;

}

unsigned long CMemPageFile::GetTotalMemory() const
{
	return _ullTotalPageFile;
}

unsigned long CMemPageFile::GetAvaliableMemory() const
{
	return _ullAvailPageFile;
}

unsigned long CMemPageFile::GetUsedMemory() const
{
	return _ullUsedPageFile;
}
