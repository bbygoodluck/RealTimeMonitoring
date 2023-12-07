#ifndef __COMMON_VIEW_IMPL_H__
#define __COMMON_VIEW_IMPL_H__

#include "../../interface/Spectrum.h"
class CCommonViewImpl : public wxWindow
{
public:
	explicit CCommonViewImpl(wxWindow* _pParent, wxWindowID id, const wxPoint &pos, const wxSize& _size, long _style);
	virtual ~CCommonViewImpl();

public:
	void UpdateView(wxWindow* pWindow, const wxRect& rect, bool bUpdate = false);
	virtual void UpdateData(unsigned long _val = 0) = 0;
	void SetDispText(const wxString& strDisp) {
		m_strDispText = strDisp;
	}

protected:
	wxMemoryDC* CreateMemoryDC(wxDC* pDC, const wxRect& rc, const wxColour& col = wxColour(0, 0, 0));
	void ClearMemoryDC(wxMemoryDC* pMemDC);

	virtual void OnPaint(wxPaintEvent& event) = 0;
	virtual void OnSize(wxSizeEvent& event) = 0;
	virtual void OnErase(wxEraseEvent& event) = 0;
	virtual void Render(wxDC* pDC) = 0;

protected:
	wxRect m_viewRect;
	wxRect m_rcUsage;
	wxBitmap* m_pDoubleBuffer;
	wxSize m_szChagned;
	wxString m_strDispText = wxT("");
	
	bool m_bDefaultSpectrumShow = false;
	bool m_bSetRectSepctrum = false;
	
	std::unique_ptr<CSpectrum> m_Spectrum;
private:
	std::unique_ptr<wxMemoryDC> m_pMemDC = nullptr;
};
#endif
