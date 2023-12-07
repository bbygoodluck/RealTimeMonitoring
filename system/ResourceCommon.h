#ifndef RESOURCECOMMON_H_INCLUDED
#define RESOURCECOMMON_H_INCLUDED

class CResourceCommon : public wxEvtHandler
{
public:
	CResourceCommon() {
		m_evtListeners.reserve(100);
	}

	~CResourceCommon() {}

	void Start(int millisecond) {
		m_bStarted = true;
		m_timer.Start(millisecond);
	}

	void Stop() {
		m_bStarted = false;
		if(m_timer.IsRunning())
			m_timer.Stop();
	}

	void AddEventListener(wxWindow* pWindow) {
		if(pWindow == nullptr)
			return;

		m_evtListeners.push_back(pWindow);
	}

	virtual void Clear() {
		m_evtListeners.clear();

		wxVector<wxWindow *>().swap(m_evtListeners);
		m_evtListeners.reserve(0);
	}

protected:
	void SendEvent(const wxEventType& evtType) {
		wxVector<wxWindow *>::const_iterator iter = nullptr;
		for(iter = m_evtListeners.begin(); iter != m_evtListeners.end(); ++iter)
		{
			wxWindow* pWindow = *iter;
			wxCommandEvent evt(evtType);

			if(!m_bStarted)
				return;

			if(pWindow && m_bStarted)
			{
				if(pWindow->GetScrollThumb(wxHORIZONTAL) == 0)
					wxPostEvent(pWindow, evt);
			}
		}
	}

	void SendEvent(const wxEventType& evtType, long lValue) {
		wxVector<wxWindow *>::const_iterator iter = nullptr;
		for(iter = m_evtListeners.begin(); iter != m_evtListeners.end(); ++iter)
		{
			wxWindow* pWindow = *iter;
			wxCommandEvent evt(evtType);
			evt.SetExtraLong(lValue);

			if(!m_bStarted)
				return;

			if(pWindow && m_bStarted)
			{
				if(pWindow->GetScrollThumb(wxHORIZONTAL) == 0)
					wxPostEvent(pWindow, evt);
			}
		}
	}

protected:
	wxTimer m_timer;
	bool m_bStarted = false;

	wxVector<wxWindow *> m_evtListeners;

protected:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void OnTimer(wxTimerEvent& event) = 0;

};

#endif // RESOURCECOMMON_H_INCLUDED
