#ifndef CPROCESSTOTALLISTCTRL_H
#define CPROCESSTOTALLISTCTRL_H

#include "CommonProcessListCtrl.h"
class CProcessTotalListCtrl : public CCommonProcessListCtrl
{
public:
	CProcessTotalListCtrl(wxWindow* _parent);
	~CProcessTotalListCtrl();

public:
	virtual void UpdateData() override;
	void SetSelectedItem(const wxString& strProcessID);
	
	virtual void AddNewProcess(unsigned long ulProcessID) override;
	virtual void DelProcess(unsigned long ulProcessID)  override;
	
protected:
	virtual void InitColumn() override;
	virtual void InitList() override;
	
private:
	void SelectedItem(int iSelIndex);
	
private:
	unsigned long m_processForegroundWndID = 0;
	bool m_bSorted = false;
};

#endif // CPROCESSTOTALLISTCTRL_H
