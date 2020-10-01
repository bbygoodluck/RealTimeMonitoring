#ifndef _MAINFRAME_H_INCLUDED
#define _MAINFRAME_H_INCLUDED

class CMainFrame : public wxFrame
{
public:
	explicit CMainFrame(const wxString& strTitle);
	virtual ~CMainFrame();
	
#ifdef __WXMSW__
	virtual WXLRESULT MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) wxOVERRIDE;
#endif

private:
	wxAuiNotebook* m_auiNBSpectrum;
	wxAuiNotebook* m_auiNBCPUGraph;
	wxAuiNotebook* m_auiNBProcessTotal;

private:
	void OnClose(wxCloseEvent& event);
	void OnMaximized(wxMaximizeEvent& event);
	void OnIconized(wxIconizeEvent& event);

	wxDECLARE_EVENT_TABLE();

};
#endif