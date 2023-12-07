#ifndef __CPU_USAGE_INFO_VIEW_H__
#define __CPU_USAGE_INFO_VIEW_H__

#include "CommonViewImpl.h"
class CCPUUsageInfoView : public CCommonViewImpl
{
public:
	explicit CCPUUsageInfoView(wxWindow* _parent, const wxSize& _size);
	virtual ~CCPUUsageInfoView();

	virtual void UpdateData(unsigned long _val = 0) wxOVERRIDE;

protected:
	virtual void Render(wxDC* pDC) wxOVERRIDE;

private:
	unsigned int m_iUsage = 0;
	
private:
	virtual void OnPaint(wxPaintEvent& event) wxOVERRIDE;
	virtual void OnSize(wxSizeEvent& event) wxOVERRIDE;
	virtual void OnErase(wxEraseEvent& event) wxOVERRIDE;
	wxDECLARE_EVENT_TABLE();
};
#endif 
