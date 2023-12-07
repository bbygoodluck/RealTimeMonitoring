#include "../../ginc.h"
#include "../../interface/Spectrum.h"
#include "MemoryUsageInfoView.h"

wxBEGIN_EVENT_TABLE(CMemoryUsageInfoView, CCommonViewImpl)
	EVT_PAINT(CMemoryUsageInfoView::OnPaint)
	EVT_SIZE(CMemoryUsageInfoView::OnSize)
	EVT_ERASE_BACKGROUND(CMemoryUsageInfoView::OnErase)
wxEND_EVENT_TABLE()

CMemoryUsageInfoView::CMemoryUsageInfoView(wxWindow* _parent, const wxSize& _size)
	: CCommonViewImpl(_parent, wxID_ANY, wxDefaultPosition, _size, VIEW_STYLE_SIMPLE)
{

}

CMemoryUsageInfoView::~CMemoryUsageInfoView()
{

}

void CMemoryUsageInfoView::OnPaint(wxPaintEvent& event)
{
	if (!m_pDoubleBuffer->IsOk())
		return;

	m_viewRect = GetClientRect();

	if (!m_bSetRectSepctrum)
	{
		m_Spectrum->SetSpectrumRect(m_viewRect);
		m_bSetRectSepctrum = true;
	}

	wxPaintDC dc(this);
	PrepareDC(dc);

	wxMemoryDC* pMemDC = CreateMemoryDC(&dc, m_viewRect);

	Render(pMemDC);

	dc.Blit(0, 0, m_pDoubleBuffer->GetWidth(), m_pDoubleBuffer->GetHeight(), pMemDC, 0, 0);
	ClearMemoryDC(pMemDC);
}

void CMemoryUsageInfoView::Render(wxDC* pDC)
{
	if (m_Spectrum->GetSize() > 0)
	{
		wxColour colPen(0, 128, 0);
		wxColour colBrush(0, 0, 0);

		for (int i = 0; i < 100; i++)
		{
			SPECTRUM_INFO* pSpectrumItem = m_Spectrum->GetItemPtr(i);
			wxPen pen(colPen);
			wxBrush brush(colBrush);

			pDC->SetPen(pen);
			pDC->SetBrush(brush);

			pDC->DrawRectangle(pSpectrumItem->m_rc);
		}

		if (m_iPercent > 0)
		{
			for (int i = 0; i < m_iPercent; i++)
			{
				SPECTRUM_INFO* pSpectrumItem = m_Spectrum->GetItemPtr(i);
				wxPen pen(pSpectrumItem->m_Colour);
				wxBrush brush(pSpectrumItem->m_Colour);

				pDC->SetPen(pen);
				pDC->SetBrush(brush);

				pDC->DrawRectangle(pSpectrumItem->m_rc);
			}
		}
	}

	wxString strMem = wxString::Format(wxT("%3.2fGB"), m_fAvaliableRAM) + wxT(" / ") + wxString::Format(wxT("%3.2fGB"), m_fPhysicalRAM);
	wxString strMemory = wxT("Memory : ") + strMem;

	wxRect viewRectShadow = m_viewRect;
	viewRectShadow.SetLeft(m_viewRect.GetLeft() + 1);
	viewRectShadow.SetTop(m_viewRect.GetTop() + 1);

	pDC->SetTextForeground(wxColour(128, 128, 128));
	pDC->DrawLabel(strMemory, viewRectShadow, wxALIGN_CENTER);


	pDC->SetTextForeground(wxColour(250, 250, 250));
	pDC->DrawLabel(strMemory, m_viewRect, wxALIGN_CENTER);
}

void CMemoryUsageInfoView::OnSize(wxSizeEvent& event)
{
	wxSize size = GetClientSize();
	if ((size.x == 0) || (size.y == 0))
		return;

	if (m_szChagned.x != size.x)
	{
		m_szChagned = size;
		if (m_pDoubleBuffer)
			delete m_pDoubleBuffer;

		m_pDoubleBuffer = new wxBitmap(m_szChagned.x, m_szChagned.y);
	}

	UpdateView(this, m_viewRect);
}

void CMemoryUsageInfoView::OnErase(wxEraseEvent& event)
{

}

void CMemoryUsageInfoView::UpdateData(unsigned long _val)
{

}

void CMemoryUsageInfoView::UpdateData(unsigned long long ullAvaliable, unsigned long long ullPhysical)
{
	m_fAvaliableRAM = (float)ullAvaliable / 1024.0;
	m_fPhysicalRAM = (float)ullPhysical / 1000.0;
	m_iPercent = (ullAvaliable * 100.0) / ullPhysical;

	UpdateView(this, m_viewRect);
}