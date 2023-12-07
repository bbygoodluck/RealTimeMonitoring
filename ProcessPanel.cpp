#include "ginc.h"
#include "ProcessPanel.h"
#include "./system/system.h"
#include "./controls/RTProcessList.h"

CProcessPanel::CProcessPanel(wxWindow* _parent)
	: wxPanel(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
#ifdef __WXMSW__
	m_pSync = std::make_unique<SYNC>();
#else

#endif
	Initialize();
}

CProcessPanel::~CProcessPanel()
{
	if(m_timer.IsRunning())
		m_timer.Stop();

	m_mgr.UnInit();

	wxDELETE(m_processListMain);
	wxDELETE(m_processListCPU);
	wxDELETE(m_processListMEM);
}

void CProcessPanel::Initialize()
{
	m_mgr.SetManagedWindow(this);

	m_processListMain = new CRTProcessList( this, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxBORDER_SIMPLE, PROCESSLIST_PRIORITY_ALL );
	m_processListCPU = new CRTProcessList( this, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxBORDER_SIMPLE, PROCESSLIST_PRIORITY_CPU );
	m_processListMEM = new CRTProcessList( this, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxBORDER_SIMPLE, PROCESSLIST_PRIORITY_MEM );

	theSystemInfo->GetProcessInfo()->AddEventListener(m_processListMain);
	theSystemInfo->GetProcessInfo()->AddEventListener(m_processListCPU);
	theSystemInfo->GetProcessInfo()->AddEventListener(m_processListMEM);

	wxAuiPaneInfo cpuPane = wxAuiPaneInfo().
	                        Name("ProcessList_CPU").
	                        Caption("ProcessList - CPU").
	                        MinSize(wxSize(-1, 250)).
				            CloseButton(false).
				            MaximizeButton(false).
				            Icon(wxArtProvider::GetBitmapBundle(wxART_REMOVABLE, wxART_OTHER, wxSize(16, 16)));

	wxAuiPaneInfo memPane = wxAuiPaneInfo().
	                        Name("ProcessList_Mem").
	                        Caption("ProcessList - Memory").
	                        MinSize(wxSize(-1, 250)).
							CloseButton(false).
							MaximizeButton(false).
				            Icon(wxArtProvider::GetBitmapBundle(wxART_ADD_BOOKMARK, wxART_OTHER, wxSize(16, 16)));

	switch(theSetting->GetListPosition())
	{
		case POS_LEFT:
			cpuPane = cpuPane.Left().Layer(1).Position(1);
			memPane = memPane.Left().Layer(1).Position(2);
			break;

		case POS_RIGHT:
			cpuPane = cpuPane.Right().Layer(1).Position(1);
			memPane = memPane.Right().Layer(1).Position(2);
			break;

		case POS_TOP:
			cpuPane = cpuPane.Top().Layer(1).Position(1);
			memPane = memPane.Top().Layer(1).Position(2);
			break;

		case POS_BOTTOM:
			cpuPane = cpuPane.Bottom().Layer(1).Position(1);
			memPane = memPane.Bottom().Layer(1).Position(2);
			break;

		default:
			cpuPane = cpuPane.Bottom().Layer(1).Position(1);
			memPane = memPane.Bottom().Layer(1).Position(2);
			break;

	}

	m_mgr.AddPane(m_processListMain, wxAuiPaneInfo().Name("ProcessList").CenterPane().PaneBorder(false));
	m_mgr.AddPane(m_processListCPU, cpuPane);
	m_mgr.AddPane(m_processListMEM, memPane);

	m_mgr.Update();

	m_timer.SetOwner(this, wxTIMER_ID++);
	this->Connect(wxEVT_TIMER, wxTimerEventHandler(CProcessPanel::OnTimer));

	m_timer.Start(1500);
}

void CProcessPanel::OnTimer(wxTimerEvent& event)
{
	m_pSync->Lock();
	theSystemInfo->GetProcessInfo()->UpdateProcesses();
	m_pSync->UnLock();
}
