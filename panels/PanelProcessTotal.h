#ifndef CPANELPROCESSTOTAL_H
#define CPANELPROCESSTOTAL_H

#include "CommonPanel.h"
class CProcessTotalListCtrl;
class CProcessCPUListCtrl;
class CProcessMemListCtrl;

class CPanelProcessTotal : public wxPanel, CCommonPanel
{
	enum {
		TIMER_PROCESS = 10
	};
	
public:
	explicit CPanelProcessTotal(wxWindow* _parent);
	virtual ~CPanelProcessTotal();

public:
	virtual void UpdateData() override;
	void SetSelectedItem(const wxString& strProcessID);
	
private:
	void Init();
	
private:
	wxBoxSizer* m_Mainsizer;
	std::unique_ptr<CProcessTotalListCtrl> m_lstProcessTotal;
	std::unique_ptr<CProcessCPUListCtrl> m_lstProcessCPU;
	std::unique_ptr<CProcessMemListCtrl> m_lstProcessMem;
	
	wxStaticText* m_staticText1;
	wxStaticText* m_staticText2;
		
private:
	wxTimer m_timer;
	int m_iTimerMillSec;

private:
	void OnTimer(wxTimerEvent& event);
	void OnCreateNewProcess(wxCommandEvent& event);
	void OnTerminateAndDelProcess(wxCommandEvent& event);
	
	wxDECLARE_EVENT_TABLE();
};

#endif // CPANELPROCESSTOTAL_H
