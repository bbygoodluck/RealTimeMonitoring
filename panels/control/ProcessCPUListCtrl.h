#ifndef CPROCESSCPULISTCTRL_H
#define CPROCESSCPULISTCTRL_H

#include "CommonProcessListCtrl.h"
class CProcessCPUListCtrl : public CCommonProcessListCtrl
{
public:
	CProcessCPUListCtrl(wxWindow* _parent);
	~CProcessCPUListCtrl();
	
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

#endif // CPROCESSCPULISTCTRL_H
