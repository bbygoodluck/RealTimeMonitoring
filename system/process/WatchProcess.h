#ifndef __WATCH_PROCESS_H_INCLUDED
#define __WATCH_PROCESS_H_INCLUDED

class CWatchProcessImpl;
class CWatchProcess : public wxThreadHelper //wxEvtHandler
{
	enum {
		PROCESS_WATCHID = 100
	};
public:
	CWatchProcess();
	~CWatchProcess();

public:
	void StartWatch();
	void StopWatch();
	bool CheckAndAddProcessWatch(unsigned long ulProcessID);
	void DoCheckNewProcess();
	void DoDelProcess(unsigned long ulProcessID);
	bool IsTerminate() { return m_bStop;}
private:
	virtual wxThread::ExitCode Entry() override;

private:
//	wxTimer m_timer;
//	int m_iTimerMillSec;
	bool m_bStop = false;
	std::unordered_map<unsigned long, CWatchProcessImpl *> m_processExitWait;

private:
//	void OnTimer(wxTimerEvent& event);
//	wxDECLARE_EVENT_TABLE();

};
#endif
