#include "common.h"
#include "JsonConfig.h"

#include <iostream>
#include <fstream>
#include <streambuf>

std::unique_ptr<CJsonConfig> CJsonConfig::m_pInstance(nullptr);

CJsonConfig::CJsonConfig()
	: m_iWidth(1480)
	, m_iHeight(800)
	, m_iXPos(0)
	, m_iYPos(0)
	, m_iCurrentMonitor(0)
{

};

CJsonConfig::~CJsonConfig()
{
	_jsonDoc.RemoveAllMembers();
}

CJsonConfig* CJsonConfig::Get()
{
	if (m_pInstance.get() == NULL)
		m_pInstance.reset(new CJsonConfig());

	return m_pInstance.get();
}

bool CJsonConfig::Load()
{
	wxString strJson(theUtility->GetWorkDir() + SLASH + wxT("settings") + SLASH + wxT("settings.json"));
	if (!wxFileName::FileExists(strJson))
	{
		wxMessageBox(strJson + wxT(" file is not exist"), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return false;
	}

	bool bReturn = DoLoad(strJson);
	if (bReturn)
		SetGlobalConfig();

	return bReturn;
}

bool CJsonConfig::IsExistMember(const wxString& strGroup, const wxString& strMember)
{
	assert(_jsonDoc.IsObject());

	wxString _strKey(strGroup);
	wxString _strMember(strMember);

	if (_strKey.IsEmpty()) return HasMember(_strMember);
	else                   return HasMember(_strMember, _strKey);

	return true;
}

void CJsonConfig::SetGlobalConfig()
{
	assert(_jsonDoc.IsObject());
	assert(HasMember(wxT("settings")));

	m_iMaximized      = _jsonDoc["settings"]["Maximized"].GetInt();
	m_iCurrentMonitor = _jsonDoc["settings"]["DispMonitor"].GetInt();

	m_iWidth          = _jsonDoc["settings"]["Width"].GetInt();
	m_iHeight         = _jsonDoc["settings"]["Height"].GetInt();
	m_iXPos           = _jsonDoc["settings"]["XPos"].GetInt();
	m_iYPos           = _jsonDoc["settings"]["YPos"].GetInt();
	m_iListPosition   = _jsonDoc["settings"]["ListPostion"].GetInt();
	m_iTaskBarShow    = _jsonDoc["settings"]["TaskBarShow"].GetInt();
	m_iCPUMaxCheckVal = _jsonDoc["settings"]["CPUMaxCheck"].GetInt();
	m_iMEMMaxCheckVal = _jsonDoc["settings"]["MEMMaxCheck"].GetInt();

	m_strDBName       = _jsonDoc["settings"]["DBName"].GetString();
//	unsigned long ulGroupColor    = ConvertStringToColor(strColGroup);
//	unsigned long ulPriorityColor = ConvertStringToColor(strColPriority);
//	unsigned long ulDateLateColor = ConvertStringToColor(strDateLate);

}

void CJsonConfig::SetChangeGlobalVal(const wxString& strMember, int iVal)
{
	wxString _strMember(strMember);

	Value _Key(_strMember.c_str(), _jsonDoc.GetAllocator());
	_jsonDoc["settings"][_Key] = iVal;
}

void CJsonConfig::SetChangeGlobalVal(const wxString& strMember, const wxString& strVal)
{
	wxString _strMember(strMember);
	wxString _strVal(strVal);

	Value _Key(_strMember.c_str(), _jsonDoc.GetAllocator());
	Value _Val(_strVal.c_str(), _jsonDoc.GetAllocator());

	_jsonDoc["settings"][_Key] = _Val;
}

void CJsonConfig::SetChangeColorVal(const wxString& strKey, const wxString& strMember, const wxString& strVal)
{
	if (!HasMember(strKey))
		return;

	if (!HasMember(strMember, strKey))
		return;

	Value _Key(strKey.c_str(), _jsonDoc.GetAllocator());
	Value _Member(strMember.c_str(), _jsonDoc.GetAllocator());
	Value _Val(strVal.c_str(), _jsonDoc.GetAllocator());

	_jsonDoc[_Key][_Member] = _Val;
}

void CJsonConfig::SaveConfig()
{
	wxString strOutJson(m_strJsonPath);
	std::ofstream ofs;

	ofs.open(strOutJson.char_str());

	OStreamWrapper osw(ofs);
	PrettyWriter<OStreamWrapper> writer(osw);
	_jsonDoc.Accept(writer);

	SetGlobalConfig();
}

unsigned long CJsonConfig::ConvertStringToColor(wxString& strColor)
{
	unsigned long lvalue = 0;
	strColor.ToULong(&lvalue, 16);

	return lvalue;
}
