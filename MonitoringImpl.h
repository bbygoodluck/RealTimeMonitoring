#ifndef CMONITORINGIMPL_H
#define CMONITORINGIMPL_H


class CMonitoringImpl : public wxEvtHandler
{
	enum timerID {
		TIMER_CPU_MEMORY_DATA = 10000
	};
	
private:
	CMonitoringImpl();
	
public:
	virtual ~CMonitoringImpl();
	static CMonitoringImpl* Get();
	
public:
	void StartMonitoring();
	void StopMonitoring();

private:
	static std::unique_ptr<CMonitoringImpl> m_pInstance;
	wxTimer m_timer;
	int m_iTimerMillSec;

private:
	void OnTimer(wxTimerEvent& event);
	wxDECLARE_EVENT_TABLE();

};

#endif // CMONITORINGIMPL_H
