#include "ginc.h"
#include "MenuOperation.h"
#include "MenuEvent.h"
#include "MainFrame.h"
#include "./dialog/DlgNewJob.h"

#include <wx/filedlg.h>

CMenuOperation::CMenuOperation(CMenuEvent* pMenuEvent)
	: m_pMenuEvent(pMenuEvent)
{
	Init();
}

CMenuOperation::~CMenuOperation()
{
	m_mapMenuFunctionPtr.clear();
}

void CMenuOperation::Init()
{
	typedef struct stMenuList {
		wxString strMenuID;
		std::function<void()> funcPtr;
	} MENU_FNPTRLIST;

	MENU_FNPTRLIST stMenufnList[] = {
		{ wxT("m_fMenu_Exec"),     std::bind(&CMenuOperation::FileMenu_NewJobExec  , this) },
		{ wxT("m_fMenu_Exit"),	   std::bind(&CMenuOperation::FileMenu_Terminate   , this) },
		{ wxT("m_sMenu_SetEnv"),   std::bind(&CMenuOperation::ToolMenu_SetEnv      , this) },
		{ wxT("m_sMenu_SetSave"),  std::bind(&CMenuOperation::ToolMenu_CurrentSave , this) },
	};

	for(unsigned int i = 0; i < WXSIZEOF(stMenufnList); i++)
	{
		int iMenuID = XRCID(stMenufnList[i].strMenuID);
		m_mapMenuFunctionPtr.insert(std::make_pair(iMenuID, stMenufnList[i].funcPtr));
	}
}

void CMenuOperation::ExecuteMenuOperation(int iMenuID, _MENU_EVENT_TYPE _mType)
{
	std::unordered_map< int, std::function<void()> >::const_iterator fIter = m_mapMenuFunctionPtr.find(iMenuID);
	if(fIter == m_mapMenuFunctionPtr.end())
		return;

	auto functionPtr = fIter->second;
	(functionPtr)();
}

void CMenuOperation::FileMenu_NewJobExec()
{
	CMainFrame* pMainFrame = (CMainFrame *)m_pMenuEvent->GetMainFrame();
	CDlgNewJob dlg(pMainFrame);

	dlg.ShowModal();
}

void CMenuOperation::FileMenu_Terminate()
{
	wxTheApp->GetTopWindow()->Close();
}

void CMenuOperation::ToolMenu_SetEnv()
{

}

void CMenuOperation::ToolMenu_CurrentSave()
{
	CMainFrame* pMainFrame = (CMainFrame *)m_pMenuEvent->GetMainFrame();

	int xPos = 0;
	int yPos = 0;

	int nCurrDisp = wxDisplay::GetFromWindow(pMainFrame);

	wxDisplay disp(nCurrDisp);
	wxRect rcMonitor = disp.GetGeometry();

	wxTopLevelWindow* pTopWindow = (wxTopLevelWindow *)wxTheApp->GetTopWindow();
	pTopWindow->GetScreenPosition(&xPos, &yPos);
	wxRect rcScreen = pTopWindow->GetScreenRect();

	int iMaximized = pMainFrame->IsMaximized() ? 1 : 0;

	if (rcMonitor.GetLeft() != 0)
		xPos = rcMonitor.GetLeft();

	int iWidth = rcScreen.GetWidth();
	int iHeight = rcScreen.GetHeight();

	theSetting->SetChangeGlobalVal(wxT("Maximized"), iMaximized);
	theSetting->SetChangeGlobalVal(wxT("DispMonitor"), nCurrDisp);
	theSetting->SetChangeGlobalVal(wxT("XPos"), xPos);
	theSetting->SetChangeGlobalVal(wxT("YPos"), yPos);
	theSetting->SetChangeGlobalVal(wxT("Width"), iWidth);
	theSetting->SetChangeGlobalVal(wxT("Height"), iHeight);

	theSetting->SaveConfig();
	wxMessageBox(wxT("저장되었습니다."), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
}
