#ifndef __VIRTUAL_MEMORY_USAGE_INFO_H__
#define __VIRTUAL_MEMORY_USAGE_INFO_H__

#include "CommonViewImpl.h"
class CPageFileUsageInfoView : public CCommonViewImpl
{
public:
	explicit CPageFileUsageInfoView(wxWindow* _parent, const wxSize& _size);
	virtual ~CPageFileUsageInfoView();

	virtual void UpdateData(unsigned long _val = 0) wxOVERRIDE;
	void UpdateData(unsigned long long ullAvaliable, unsigned long long ullTotal);

protected:
	virtual void Render(wxDC* pDC) wxOVERRIDE;

private:
	int m_iPercent = 0;
	float m_fAvaliableRAM = 0;
	float m_fPhysicalRAM = 0;

private:
	virtual void OnPaint(wxPaintEvent& event) wxOVERRIDE;
	virtual void OnSize(wxSizeEvent& event) wxOVERRIDE;
	virtual void OnErase(wxEraseEvent& event) wxOVERRIDE;
	wxDECLARE_EVENT_TABLE();
};
#endif
