#include "global.h"
#include "CommonUtil.h"

std::unique_ptr<CCommonUtil> CCommonUtil::m_pInstance = nullptr;

CCommonUtil* CCommonUtil::Get()
{
	if (m_pInstance.get() == NULL)
		m_pInstance.reset(new CCommonUtil());

	return m_pInstance.get();
}

void CCommonUtil::LoadImageList()
{
#ifdef __WXMSW__
	// IID_IImageList {46EB5926-582E-4017-9FDF-E8998DAA0950}
	static const GUID IID_IImageList = { 0x46EB5926, 0x582E, 0x4017, { 0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x9, 0x50 } };
	// IID_IImageList2 {192B9D83-50FC-457B-90A0-2B82A8B5DAE1}
	static const GUID IID_IImageList2 = { 0x192B9D83, 0x50FC, 0x457B, { 0x90, 0xA0, 0x2B, 0x82, 0xA8, 0xB5, 0xDA, 0xE1 } };

	_gImageList = nullptr;
	HRESULT res = SHGetImageList(SHIL_SMALL, IID_IImageList2, (void**)& _gImageList);
	if (FAILED(res))
		res = SHGetImageList(SHIL_SMALL, IID_IImageList, (void**)& _gImageList);
#else
	_gImageList = wxTheFileIconsTable->GetSmallImageList();
#endif
}

void CCommonUtil::GetIconIndexFromImageList(const wxString& strFullPath, int &iIconIndex, int& iOverlayIndex)
{
	#ifdef __WXMSW__
	SHFILEINFO sfi;
	wxZeroMemory(sfi);

	DWORD dwNum = GetFileAttributes(strFullPath);

	SHGetFileInfo(strFullPath, dwNum, &sfi, sizeof(sfi), IMAGELIST_FLAG);// | SHGFI_ADDOVERLAYS);

	iIconIndex = (sfi.iIcon & 0x00FFFFFF);
	iOverlayIndex = (sfi.iIcon >> 24) - 1;

	DestroyIcon(sfi.hIcon);
#else
#endif
}