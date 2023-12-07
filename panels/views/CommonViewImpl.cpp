#include "../../ginc.h"
#include "CommonViewImpl.h"

CCommonViewImpl::CCommonViewImpl(wxWindow* _pParent, wxWindowID id, const wxPoint &pos, const wxSize& _size, long _style)
	: wxWindow(_pParent, id, pos, _size, _style)
	, m_viewRect(0, 0, 0, 0)
	, m_rcUsage(0, 0, 0, 0)
	, m_pDoubleBuffer(nullptr)
	, m_Spectrum(new CSpectrum())
{

}

CCommonViewImpl::~CCommonViewImpl()
{
	if (m_pDoubleBuffer)
		delete m_pDoubleBuffer;

	m_pDoubleBuffer = nullptr;
}

wxMemoryDC* CCommonViewImpl::CreateMemoryDC(wxDC* pDC, const wxRect& rc, const wxColour& col)
{
	m_pMemDC = std::make_unique<wxMemoryDC>(pDC);
	m_pMemDC->SelectObjectAsSource(*m_pDoubleBuffer);
	m_pMemDC->SetFont(wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT));

	wxPen pen(col);
	wxBrush brush(col);

	m_pMemDC->SetPen(pen);
	m_pMemDC->SetBrush(brush);
	m_pMemDC->DrawRectangle(0, 0, rc.GetWidth(), rc.GetHeight());

	return m_pMemDC.get();
}

void CCommonViewImpl::ClearMemoryDC(wxMemoryDC* pMemDC)
{
	pMemDC->SetFont(wxNullFont);
	pMemDC->SetPen(wxNullPen);
	pMemDC->SetBrush(wxNullBrush);
	pMemDC->SelectObjectAsSource(wxNullBitmap);
}

void CCommonViewImpl::UpdateView(wxWindow* pWindow, const wxRect& rect, bool bUpdate)
{
#ifdef __WXMSW__
	HWND hWnd = pWindow->GetHWND();
	if (rect.GetWidth() != 0)
	{
		RECT mswRect;
		const RECT* pRect;

		wxCopyRectToRECT(rect, mswRect);
		pRect = &mswRect;

		//	INT flags = RDW_INVALIDATE | RDW_NOCHILDREN | RDW_UPDATENOW | RDW_ERASE;
		//	::RedrawWindow(hWnd, pRect, NULL, flags);
		::InvalidateRect(hWnd, pRect, false);
	}
	else
		pWindow->Refresh(false);

	if (bUpdate) pWindow->Update();
	//	::UpdateWindow(hWnd);
#else
	pWindow->RefreshRect(rect, false);
	pWindow->Update();
#endif
}