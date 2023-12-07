#ifndef RTSTATUSBAR_H_INCLUDED
#define RTSTATUSBAR_H_INCLUDED

class CUsageView;
class CRTStatusBar : public wxStatusBar
{
	enum _DEFAULT
	{
		Field_Processor_img = 0,
		Field_Processor,
		Field_CPUTotal,
		Field_MemPhysics,
		Field_NumLockIndicator,
		Field_CapsLockIndicator,
		Field_Clock,
		Field_Max
	};

public:
	CRTStatusBar(wxWindow *parent, long style = wxSTB_DEFAULT_STYLE);
	virtual ~CRTStatusBar();

private:
	void Setting();
	void UpdateClock();

private:
	wxTimer m_timer;
	bool m_bSet;
	const int m_iControlWidth = 250;
	CUsageView* m_pUsageCPUView = nullptr;
	CUsageView* m_pUsageMemoryPhysicsView = nullptr;

	wxStaticBitmap* m_bmpProcessor;
private:
	void OnIdle(wxIdleEvent& event);
	void OnTimer(wxTimerEvent& WXUNUSED(event));
	void OnSize(wxSizeEvent& event);
	wxDECLARE_EVENT_TABLE();

};

#endif // RTSTATUSBAR_H_INCLUDED
