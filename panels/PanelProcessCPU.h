#ifndef CPANELPROCESSCPU_H
#define CPANELPROCESSCPU_H

#include "CommonPanel.h"
class CPanelProcessCPU : public wxPanel, CCommonPanel
{
public:
	explicit CPanelProcessCPU(wxWindow* _parent);
	virtual ~CPanelProcessCPU();

public:
	virtual void UpdateData() override;

};

#endif // CPANELPROCESSCPU_H
