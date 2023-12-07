#include "ginc.h"
#include "MainFrame.h"
#include "MenuEvent.h"
#include "MenuOperation.h"

wxBEGIN_EVENT_TABLE(CMenuEvent, wxEvtHandler)
	EVT_MENU(wxID_ANY, CMenuEvent::OnMenuEvent)
	EVT_UPDATE_UI(wxID_ANY, CMenuEvent::OnMenuEventUpdate)
wxEND_EVENT_TABLE()

CMenuEvent::CMenuEvent(CMainFrame* _pMainFrame)
	: m_pMainFrame(_pMainFrame)
{
	m_pMenuOperation = std::make_unique<CMenuOperation>(this);
	Init();
}

CMenuEvent::~CMenuEvent()
{
	m_mapMenu.clear();
}

void CMenuEvent::Init()
{
	typedef struct stMenuList {
		wxString strMenuID;
		_MENU_EVENT_TYPE mType = _MENU_EVENT_NONE;
	} MENU_LIST;

	MENU_LIST mList[] = {
		{ wxT("m_fMenu_Exec"),      _MENU_FILE_NEW_JOB_EXEC },
		{ wxT("m_fMenu_Exit"),	    _MENU_FILE_PROGRAM_TERMINATE },
		{ wxT("m_sMenu_SetEnv"),	_MENU_TOOL_SETENV },
		{ wxT("m_sMenu_SetSave"),	_MENU_TOOL_SAVE },

	};

	for(unsigned int i = 0; i < WXSIZEOF(mList); i++)
	{
		int iMenuID = XRCID(mList[i].strMenuID);
		_MENU_EVENT_TYPE mType = mList[i].mType;

		m_mapMenu.insert(std::make_pair(iMenuID, mType));
	}
}

void CMenuEvent::SetMainFrame(CMainFrame* pMainFrame)
{
	if(m_pMainFrame != nullptr)
		return;

	m_pMainFrame = pMainFrame;
}

void CMenuEvent::OnMenuEvent(wxCommandEvent& event)
{
	int _iMenuID = event.GetId();
	DoMenuOperation(_iMenuID);
}

void CMenuEvent::OnMenuEventUpdate(wxUpdateUIEvent& event)
{
	event.Skip();
}

void CMenuEvent::DoMenuOperation(int nMenuID)
{
	std::unordered_map<int, _MENU_EVENT_TYPE>::const_iterator fIter = m_mapMenu.find(nMenuID);
	if(fIter == m_mapMenu.end())
		return;

	m_pMenuOperation->ExecuteMenuOperation(nMenuID, fIter->second);
}
