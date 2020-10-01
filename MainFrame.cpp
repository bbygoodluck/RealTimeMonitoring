#include "ginc.h"
#include "MainFrame.h"

wxBEGIN_EVENT_TABLE(CMainFrame, wxFrame)
	EVT_CLOSE(CMainFrame::OnClose)
	EVT_MAXIMIZE(CMainFrame::OnMaximized)
	EVT_ICONIZE(CMainFrame::OnIconized)
wxEND_EVENT_TABLE()

CMainFrame::CMainFrame(const wxString& strTitle)
	: wxFrame(nullptr, wxID_ANY, strTitle, wxDefaultPosition, wxSize(1700, 900))
{
	SetIcon(wxIcon("wxwin"));
	
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* m_SizeMain;
	m_SizeMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	bSizer10->SetMinSize( wxSize( -1,210 ) );
	m_auiNBSpectrum = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxSize( -1,50 ), 0 );
	
	thePanelManager->CreatePanel(m_auiNBSpectrum, PANEL_SPECTRUM);
	m_auiNBSpectrum->AddPage(thePanelManager->GetPanel(PANEL_SPECTRUM), wxT("Spectrum View - CPU / Memory / PageFiles"), false, wxArtProvider::GetBitmap(wxART_NEW, wxART_BUTTON) );

	bSizer10->Add( m_auiNBSpectrum, 0, wxEXPAND, 5 );
	bSizer10->Add( 0, 0, 0, wxBOTTOM|wxEXPAND, 5 );
	
	m_auiNBCPUGraph = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	thePanelManager->CreatePanel(m_auiNBCPUGraph, PANEL_CPU_GRAPH);
	m_auiNBCPUGraph->AddPage(thePanelManager->GetPanel(PANEL_CPU_GRAPH), wxT("CPU Graph View"), false, wxArtProvider::GetBitmap(wxART_PLUS, wxART_BUTTON) );
	
	bSizer10->Add( m_auiNBCPUGraph, 1, wxEXPAND, 5 );
	m_SizeMain->Add( bSizer10, 0, wxEXPAND, 5 );
	m_SizeMain->Add( 0, 2, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizerProcessTotal;
	sizerProcessTotal = new wxBoxSizer( wxVERTICAL );
	
	m_auiNBProcessTotal = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	thePanelManager->CreatePanel(m_auiNBProcessTotal, PANEL_PROCESS_TOTAL);
	m_auiNBProcessTotal->AddPage(thePanelManager->GetPanel(PANEL_PROCESS_TOTAL), wxT("Process List"), false, wxArtProvider::GetBitmap( wxART_REPORT_VIEW, wxART_BUTTON ) );
	
	sizerProcessTotal->Add( m_auiNBProcessTotal, 1, wxEXPAND, 5 );
	
	m_SizeMain->Add( sizerProcessTotal, 1, wxEXPAND, 5 );
	
	wxDisplay screen;
	wxRect rcDisp = screen.GetClientArea();

	this->SetSize(rcDisp.GetWidth() - 130, rcDisp.GetHeight() - 20);
	
	this->SetSizer( m_SizeMain );
	this->Layout();

	this->Centre( wxBOTH );
	
	theMonitoring->StartMonitoring();	
}

CMainFrame::~CMainFrame()
{

}

void CMainFrame::OnClose(wxCloseEvent& event)
{
	theMonitoring->StopMonitoring();
	Destroy();
}

void CMainFrame::OnMaximized(wxMaximizeEvent& event)
{

}

void CMainFrame::OnIconized(wxIconizeEvent& event)
{

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