#ifndef PROCESSPANEL_H_INCLUDED
#define PROCESSPANEL_H_INCLUDED

class CRTProcessList;
class CProcessPanel : public wxPanel
{
public:
	CProcessPanel(wxWindow* _parent);
	~CProcessPanel();

private:
	void Initialize();
	void OnTimer(wxTimerEvent& event);

private:
	wxAuiManager m_mgr;
	CRTProcessList* m_processListMain;
	CRTProcessList* m_processListCPU;
	CRTProcessList* m_processListMEM;

	wxTimer m_timer;

#ifdef __WXMSW__
	std::unique_ptr<SYNC> m_pSync = nullptr;
#else
#endif
};

#endif // PROCESSPANEL_H_INCLUDED
