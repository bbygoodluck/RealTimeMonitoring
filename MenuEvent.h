#ifndef MENUEVENT_H_INCLUDED
#define MENUEVENT_H_INCLUDED

class CMainFrame;
class CMenuOperation;

class CMenuEvent : public wxEvtHandler
{
public:
	CMenuEvent(CMainFrame* _pMainFrame);
	~CMenuEvent();

	void SetMainFrame(CMainFrame* pMainFrame);
	void DoMenuOperation(int nMenuID);

	wxWindow* GetMainFrame() { return (wxWindow *)m_pMainFrame; }
private:
	void Init();

private:
	CMainFrame* m_pMainFrame;
	std::unordered_map<int, _MENU_EVENT_TYPE> m_mapMenu;

	std::unique_ptr<CMenuOperation> m_pMenuOperation;

private:
	void OnMenuEvent(wxCommandEvent& event);
	void OnMenuEventUpdate(wxUpdateUIEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif // MENUEVENT_H_INCLUDED
