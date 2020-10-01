#ifndef _COMMON_UTIL_H_INCLUDED
#define _COMMON_UTIL_H_INCLUDED

class CCommonUtil
{
private:
	explicit CCommonUtil() {};

public:
	~CCommonUtil() {};
	static CCommonUtil* Get();
	
	//이미지리스트 로드
	void LoadImageList();
	void GetIconIndexFromImageList(const wxString& strFullPath, int &iIconIndex, int& iOverlayIndex);
	
private:
	static std::unique_ptr<CCommonUtil> m_pInstance;
};

#endif