#ifndef __JSON_CONFIG_H__
#define __JSON_CONFIG_H__

#include "JsonBase.h"
class CJsonConfig : public CJsonBase
{
private:
	CJsonConfig();

public:
	~CJsonConfig();
	static CJsonConfig* Get();

public:
	virtual bool Load();
	bool IsExistMember(const wxString& strGroup, const wxString& strMember);

	const int GetWidth()  const { return m_iWidth;}
	const int GetHeight() const { return m_iHeight;}
	const int GetXPos()	  const { return m_iXPos;}
	const int GetYPos()	  const { return m_iYPos;}

	const int GetMonitorPos() const { return m_iCurrentMonitor; }
	const int IsMaximized()   const { return m_iMaximized;  }
	const int GetListPosition() const { return m_iListPosition; }
	const int GetTaskBarShow()  const { return m_iTaskBarShow; }
	void SetChangeGlobalVal(const wxString& strMember, int iVal);
	void SetChangeGlobalVal(const wxString& strMember, const wxString& strVal);
	void SetChangeColorVal(const wxString& strKey, const wxString& strMember, const wxString& strVal);
	void SaveConfig();

	const int GetCPUMaxCheckVal() { return m_iCPUMaxCheckVal; }
	const int GetMEMMaxCheckVal() { return m_iMEMMaxCheckVal; }

	wxString GetDBName() { return m_strDBName; }
private:
	void SetGlobalConfig();
	unsigned long ConvertStringToColor(wxString& strColor);

private:
	static std::unique_ptr<CJsonConfig> m_pInstance;

private:
	int m_iWidth;
	int m_iHeight;
	int m_iXPos;
	int m_iYPos;
	int m_iCurrentMonitor;
	int m_iMaximized;
	int m_iListPosition;
	int m_iTaskBarShow;
	int m_iCPUMaxCheckVal = 20;
	int m_iMEMMaxCheckVal = 20;
	wxString m_strDBName = wxT("");
};
#endif
