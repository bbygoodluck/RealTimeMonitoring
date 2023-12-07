#ifndef _PALEN_CPU_H_INCLUDED
#define _PALEN_CPU_H_INCLUDED

#include "CommonPanel.h"
class CCPUGraphView;
class CPanelCPU : public wxPanel, CCommonPanel
{
public:
	explicit CPanelCPU(wxWindow* _parent);
	virtual ~CPanelCPU();

public:
	virtual void UpdateData() override;
	
private:
	void Init();
	
private:
	wxBoxSizer* m_Mainsizer;
	std::unique_ptr<CCPUGraphView> m_viewCPUTotal;
	wxVector<CCPUGraphView *> m_vecViewCPUCore;

//	int m_iCpuCoreCount = 0;
};
#endif