#ifndef __PANEL_SPECTRUM_H_INCLUDED
#define __PANEL_SPECTRUM_H_INCLUDED

#include "CommonPanel.h"
class CCPUUsageInfoView;
class CMemoryUsageInfoView;
class CPageFileUsageInfoView;

class CPanelSpectrum : public wxPanel, CCommonPanel
{
public:
	explicit CPanelSpectrum(wxWindow* _parent);
	virtual ~CPanelSpectrum();

public:
	virtual void UpdateData() override;
private:
	void Init();
	
private:
	std::unique_ptr<CCPUUsageInfoView> m_CPUUsageInfoView;
	std::unique_ptr<CMemoryUsageInfoView> m_MemoryUsageInfoView;
	std::unique_ptr<CPageFileUsageInfoView> m_PageFileUsageInfoView;
	wxBoxSizer* m_Mainsizer = nullptr;
}; 
#endif