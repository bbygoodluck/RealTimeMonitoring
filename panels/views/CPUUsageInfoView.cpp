#include "../../ginc.h"
#include "CPUUsageInfoView.h"

wxBEGIN_EVENT_TABLE(CCPUUsageInfoView, CCommonViewImpl)
	EVT_PAINT(CCPUUsageInfoView::OnPaint)
	EVT_SIZE(CCPUUsageInfoView::OnSize)
	EVT_ERASE_BACKGROUND(CCPUUsageInfoView::OnErase)
wxEND_EVENT_TABLE()

CCPUUsageInfoView::CCPUUsageInfoView(wxWindow* _parent, const wxSize& _size)
	: CCommonViewImpl(_parent, wxID_ANY, wxDefaultPosition, _size, VIEW_STYLE_SIMPLE)
{
	m_bDefaultSpectrumShow = false;
}

CCPUUsageInfoView::~CCPUUsageInfoView()
{
}

void CCPUUsageInfoView::OnPaint(wxPaintEvent& event)
{
	if (!m_pDoubleBuffer->IsOk())
		return;

	m_viewRect = GetClientRect();
	m_rcUsage = GetClientRect();
	
	if(!m_bDefaultSpectrumShow)
	{
		if (!m_bSetRectSepctrum)
		{
			m_Spectrum->SetSpectrumRect(m_viewRect);
			m_bSetRectSepctrum = true;
		}
	}
	
	wxPaintDC dc(this);
	PrepareDC(dc);

	wxMemoryDC* pMemDC = CreateMemoryDC(&dc, m_viewRect);

	Render(pMemDC);

	dc.Blit(0, 0, m_pDoubleBuffer->GetWidth(), m_pDoubleBuffer->GetHeight(), pMemDC, 0, 0);
	ClearMemoryDC(pMemDC);
}

void CCPUUsageInfoView::Render(wxDC* pDC)
{
	//if(!m_bDefaultSpectrumShow)
	//{
	int iRed = (2 * m_iUsage) + 55;
	int iGreen = 255 - (m_iUsage * 3);
	iGreen = iGreen < 0 ? 0 : iGreen;
	wxColour col(iRed, iGreen, 0);
	
	int iRight = (m_iUsage * m_viewRect.GetRight()) / 100;
	m_rcUsage.SetRight(iRight);
	
	pDC->GradientFillLinear(m_rcUsage, wxColour(0, 255, 0), col);
	/*}
	else
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

			for (unsigned int i = 0; i < m_iUsage; i++)
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
	*/
	wxString strDisp = wxT("CPU : ") + wxString::Format(wxT("%02d%"), m_iUsage);
	
	wxRect viewRectShadow = m_viewRect;
	viewRectShadow.SetLeft(m_viewRect.GetLeft() + 1);
	viewRectShadow.SetTop(m_viewRect.GetTop() + 1);

	pDC->SetTextForeground(wxColour(128, 128, 128));
	pDC->DrawLabel(strDisp, viewRectShadow, wxALIGN_CENTER);

	pDC->SetTextForeground(wxColour(250, 250, 250));
	pDC->DrawLabel(strDisp, m_viewRect, wxALIGN_CENTER);
}

void CCPUUsageInfoView::OnSize(wxSizeEvent& event)
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

void CCPUUsageInfoView::OnErase(wxEraseEvent& event)
{

}

void CCPUUsageInfoView::UpdateData(unsigned long _val)
{
	if (m_viewRect.GetWidth() == 0)
		return;

	m_iUsage = wx_static_cast(int, _val);// rand() % 100;
	if (m_iUsage < 0)
		m_iUsage = 0;

	UpdateView(this, m_viewRect);
}