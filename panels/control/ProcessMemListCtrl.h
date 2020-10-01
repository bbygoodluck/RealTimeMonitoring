#ifndef CPROCESSMEMLISTCTRL_H
#define CPROCESSMEMLISTCTRL_H

#include "CommonProcessListCtrl.h"
class CProcessMemListCtrl : public CCommonProcessListCtrl
{
public:
	CProcessMemListCtrl(wxWindow* _parent);
	~CProcessMemListCtrl();

public:
	virtual void UpdateData() override;
	virtual void AddNewProcess(unsigned long ulProcessID) override;
	virtual void DelProcess(unsigned long ulProcessID)  override;
	
protected:
	virtual void InitColumn() override;
	virtual void InitList() override;

private:
	void OnItemSelected(wxListEvent& event);
};

#endif // CPROCESSMEMLISTCTRL_H
