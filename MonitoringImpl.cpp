#include "./ginc.h"
#include "MonitoringImpl.h"

std::unique_ptr<CMonitoringImpl> CMonitoringImpl::m_pInstance = nullptr;

wxBEGIN_EVENT_TABLE(CMonitoringImpl, wxEvtHandler)
	EVT_TIMER(TIMER_CPU_MEMORY_DATA, CMonitoringImpl::OnTimer)
wxEND_EVENT_TABLE()


CMonitoringImpl::CMonitoringImpl()
	: m_iTimerMillSec(1000)
{
	m_timer.SetOwner(this, TIMER_CPU_MEMORY_DATA);
}

CMonitoringImpl::~CMonitoringImpl()
{
}

CMonitoringImpl* CMonitoringImpl::Get()
{
	if (m_pInstance.get() == nullptr)
		m_pInstance.reset(new CMonitoringImpl());

	return m_pInstance.get();
}

void CMonitoringImpl::OnTimer(wxTimerEvent & event)
{
	theSystemInfo->UpdateInfo();
	thePanelManager->UpdateInfo();
}

void CMonitoringImpl::StartMonitoring()
{
	m_timer.Start(m_iTimerMillSec);
}

void CMonitoringImpl::StopMonitoring()
{
	if (m_timer.IsRunning())
		m_timer.Stop();
}
