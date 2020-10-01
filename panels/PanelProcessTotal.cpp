#include "../ginc.h"
#include "./control/ProcessTotalListCtrl.h"
#include "./control/ProcessCPUListCtrl.h"
#include "./control/ProcessMemListCtrl.h"

#include "PanelProcessTotal.h"

wxBEGIN_EVENT_TABLE(CPanelProcessTotal, wxPanel)
	EVT_TIMER(CPanelProcessTotal::TIMER_PROCESS, CPanelProcessTotal::OnTimer)
	EVT_MY_CUSTOM_COMMAND(wxEVT_CREATE_NEW_PROCESS, wxID_ANY, CPanelProcessTotal::OnCreateNewProcess)
	EVT_MY_CUSTOM_COMMAND(wxEVT_TERMINATE_AND_DEL_PROCESS, wxID_ANY, CPanelProcessTotal::OnTerminateAndDelProcess)
wxEND_EVENT_TABLE()


CPanelProcessTotal::CPanelProcessTotal(wxWindow* _parent)
	: wxPanel(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
	, m_iTimerMillSec(1000)
{
	InitializeCriticalSection(&_gListCtrlDelSection);
	
	m_timer.SetOwner(this, TIMER_PROCESS);
	Init();
	
	m_timer.Start(m_iTimerMillSec);
}

CPanelProcessTotal::~CPanelProcessTotal()
{
	if (m_timer.IsRunning())
		m_timer.Stop();
		
	DeleteCriticalSection(&_gListCtrlDelSection);
}

void CPanelProcessTotal::Init()
{
	m_Mainsizer = new wxBoxSizer(wxVERTICAL);
	m_lstProcessTotal = std::make_unique<CProcessTotalListCtrl>(this);
	m_lstProcessCPU = std::make_unique<CProcessCPUListCtrl>(this);
	m_lstProcessMem = std::make_unique<CProcessMemListCtrl>(this);

	m_Mainsizer->Add(m_lstProcessTotal.get(), 1, wxEXPAND, 5);
	m_Mainsizer->Add( 0, 3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	bSizer12->SetMinSize( wxSize( -1,240 ) );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("ProcessList - CPU"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL|wxBORDER_SIMPLE );
	m_staticText1->Wrap( -1 );
	m_staticText1->SetBackgroundColour( wxColour( 155, 240, 221 ) );
	
	bSizer13->Add( m_staticText1, 0, wxEXPAND, 5 );
	bSizer13->Add( 0, 1, 0, wxEXPAND, 5 );
	bSizer13->Add( m_lstProcessCPU.get(), 1, wxEXPAND, 5 );
	
	bSizer12->Add( bSizer13, 1, wxEXPAND, 5 );
	bSizer12->Add( 4, 0, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer141;
	bSizer141 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("ProcessList - Memory"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL|wxBORDER_SIMPLE );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetBackgroundColour( wxColour( 157, 223, 255 ) );
	
	bSizer141->Add( m_staticText2, 0, wxEXPAND, 5 );
	bSizer141->Add( 0, 1, 0, wxEXPAND, 5 );
	bSizer141->Add( m_lstProcessMem.get(), 1, wxEXPAND, 5 );
	
	bSizer12->Add( bSizer141, 1, wxEXPAND, 5 );
	
	m_Mainsizer->Add( bSizer12, 0, wxEXPAND, 5 );

	SetSizer(m_Mainsizer);
	m_Mainsizer->SetSizeHints(this);
}

void CPanelProcessTotal::UpdateData()
{
//	m_lstProcessTotal->UpdateData();
}

void CPanelProcessTotal::OnTimer(wxTimerEvent & event)
{
	theSystemInfo->UpdateInfo(PANEL_PROCESS_TOTAL);
	m_lstProcessTotal->UpdateData();
	m_lstProcessCPU->UpdateData();
	m_lstProcessMem->UpdateData();
}

void CPanelProcessTotal::SetSelectedItem(const wxString& strProcessID)
{
	m_lstProcessTotal->SetSelectedItem(strProcessID);
}

void CPanelProcessTotal::OnCreateNewProcess(wxCommandEvent& event)
{
	unsigned long ulProcessID = (unsigned long)event.GetExtraLong();
	m_lstProcessTotal->AddNewProcess(ulProcessID);
	m_lstProcessCPU->AddNewProcess(ulProcessID);
	m_lstProcessMem->AddNewProcess(ulProcessID);
}

void CPanelProcessTotal::OnTerminateAndDelProcess(wxCommandEvent& event)
{
	unsigned long ulProcessID = (unsigned long)event.GetExtraLong();
	m_lstProcessTotal->DelProcess(ulProcessID);
	m_lstProcessCPU->DelProcess(ulProcessID);
	m_lstProcessMem->DelProcess(ulProcessID);
}