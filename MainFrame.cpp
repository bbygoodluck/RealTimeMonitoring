#include "ginc.h"
#include "MainFrame.h"
#include "RTStatusBar.h"
#include "CPUPanel.h"
#include "ProcessPanel.h"
#include "NetworkPanel.h"

#include "./system/system.h"
#include "MenuEvent.h"

wxBEGIN_EVENT_TABLE(CMainFrame, wxFrame)
	EVT_CLOSE(CMainFrame::OnClose)
	EVT_MAXIMIZE(CMainFrame::OnMaximized)
	EVT_ICONIZE(CMainFrame::OnIconized)
wxEND_EVENT_TABLE()

CMainFrame::CMainFrame(const wxString& strTitle , long style)
	: wxFrame(nullptr, wxID_ANY, strTitle, wxPoint(theSetting->GetXPos(), theSetting->GetYPos()), wxSize(theSetting->GetWidth(), theSetting->GetHeight()), style)
{
	SetIcon(wxIcon("wxwin"));

	//메뉴읽기
	wxXmlResource::Get()->InitAllHandlers();
	wxString strMenuXRC = wxT("menus_ko.xrc");

	wxString strXrcPath = theUtility->GetWorkDir() + SLASH + wxT("xrc") + SLASH + strMenuXRC;
	bool bXRCLoaded = wxXmlResource::Get()->Load(strXrcPath);
	if(bXRCLoaded)
	{
		wxMenuBar* pMenuBar = wxXmlResource::Get()->LoadMenuBar(this, wxT("RTMenu"));
		if (pMenuBar != nullptr)
			SetMenuBar(pMenuBar);
	}

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bxMainFrame;
	bxMainFrame = new wxBoxSizer( wxVERTICAL );

	m_auiCPU = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_auiCPU->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT ) );

	wxString strCPUCoreTitle = wxString::Format(wxT("CPU Usage Graph - %d Processor(Logical)"), (int)theSystemInfo->GetCPUInfo()->GetCoreCount());
	m_panelCPUMain = new CCPUPanel( m_auiCPU);
	m_auiCPU->AddPage( m_panelCPUMain, strCPUCoreTitle, false, wxArtProvider::GetBitmap( wxART_REMOVABLE, wxART_MENU ) );

	bxMainFrame->Add( m_auiCPU, 0, wxEXPAND | wxALL , 5 );

	m_auiProcess = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_panelProcess = new CProcessPanel( m_auiProcess);//, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_auiProcess->AddPage( m_panelProcess, wxT("Process"), false, wxArtProvider::GetBitmap( wxART_REPORT_VIEW, wxART_MENU ) );

	bxMainFrame->Add( m_auiProcess, 1, wxEXPAND | wxALL, 5 );

	m_auiNetwork = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_panelNetwork = new CNetworkPanel( m_auiNetwork);//, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_auiNetwork->AddPage( m_panelNetwork, wxT("Network"), false, wxArtProvider::GetBitmap( wxART_FLOPPY, wxART_MENU ) );

	bxMainFrame->Add( m_auiNetwork, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( bxMainFrame );
	this->Layout();

	m_pStatusBar = std::make_unique<CRTStatusBar>(this);
	SetStatusBar(m_pStatusBar.get());

	//서비스 시작
	theSystemInfo->GetCPUInfo()->Start(1000);
	theSystemInfo->GetMemoryInfo()->Start(1000);
	theSystemInfo->GetNetInfo()->Start(1000);

	RegisterMenuEvent();
	CenterOnScreen();

	if (theSetting->IsMaximized())
		this->Maximize();
}

CMainFrame::~CMainFrame()
{

}

void CMainFrame::OnClose(wxCloseEvent& event)
{
	PopEventHandler(true);
	Destroy();
}

void CMainFrame::OnMaximized(wxMaximizeEvent& event)
{

}

void CMainFrame::OnIconized(wxIconizeEvent& event)
{

}

void CMainFrame::RegisterMenuEvent()
{
	_gMenuEvent = new CMenuEvent(this);
	PushEventHandler(_gMenuEvent);
}

#ifdef __WXMSW__
WXLRESULT CMainFrame::MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
	WXLRESULT rc;
	switch(nMsg)
	{
		default:
			break;
	}

	rc = ::DefWindowProc(this->GetHWND(), nMsg, wParam, lParam);
	return rc;
}
#endif
