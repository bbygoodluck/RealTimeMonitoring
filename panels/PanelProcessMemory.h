#ifndef CPANELPROCESSMEMORY_H
#define CPANELPROCESSMEMORY_H

#include "CommonPanel.h"
class CPanelProcessMemory : public wxPanel, CCommonPanel
{
public:
	explicit CPanelProcessMemory(wxWindow* _parent);
	virtual ~CPanelProcessMemory();

public:
	virtual void UpdateData() override;
};

#endif // CPANELPROCESSMEMORY_H
