#ifndef _MAINFRAME_H_INCLUDED
#define _MAINFRAME_H_INCLUDED

class CRTStatusBar;
class CCPUPanel;
class CProcessPanel;
class CNetworkPanel;

class CMainFrame : public wxFrame
{
public:
	explicit CMainFrame(const wxString& strTitle, long style);
	virtual ~CMainFrame();

#ifdef __WXMSW__
	virtual WXLRESULT MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) wxOVERRIDE;
#endif

private:
	wxAuiNotebook* m_auiCPU;
	CCPUPanel* m_panelCPUMain;
	wxAuiNotebook* m_auiProcess;
	CProcessPanel* m_panelProcess;

	wxAuiNotebook* m_auiNetwork;
	CNetworkPanel* m_panelNetwork;

	std::unique_ptr<CRTStatusBar> m_pStatusBar;

private:
	void RegisterMenuEvent();

private:
	void OnClose(wxCloseEvent& event);
	void OnMaximized(wxMaximizeEvent& event);
	void OnIconized(wxIconizeEvent& event);

	wxDECLARE_EVENT_TABLE();

};
#endif
