#ifndef _COMMON_UTIL_H_INCLUDED
#define _COMMON_UTIL_H_INCLUDED

class CUtility
{
private:
	explicit CUtility() {};

public:
	~CUtility() {};
	static CUtility* Get();

	//이미지리스트 로드
	void LoadImageList();
	void GetIconIndexFromImageList(const wxString& strFullPath, int &iIconIndex, int& iOverlayIndex);

	//작업디렉토리 가져오기
	wxString GetWorkDir() const { return m_strWorkingDirectory; }
	wxString SetComma(const wxString& strSize);
	wxString SetComma(unsigned long val);

private:
	void SetWorkingDirectory();

private:
	static std::unique_ptr<CUtility> m_pInstance;

	wxString m_strWorkingDirectory = wxT("");
};

#endif
