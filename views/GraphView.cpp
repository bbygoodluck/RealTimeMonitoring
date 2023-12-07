#include "../ginc.h"
#include "GraphView.h"
#include "../system/system.h"

wxBEGIN_EVENT_TABLE(CGraphView, wxWindow)
	EVT_PAINT(CGraphView::OnPaint)
	EVT_SIZE(CGraphView::OnSize)
	EVT_ERASE_BACKGROUND(CGraphView::OnErase)
	EVT_MY_CUSTOM_COMMAND(wxEVT_RESOURCE_MONITORING, wxID_ANY, CGraphView::UpdateData)
wxEND_EVENT_TABLE()

CGraphView::CGraphView(wxWindow* parent, const int nID, const wxSize& sz, long lStyle)
	: wxWindow(parent, nID, wxDefaultPosition, sz, lStyle)
	, m_colRuler(90, 90, 90)
{
#ifdef __WXMSW__
	font_graphview_.Create(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Consolas"));
#else
	font_graphview_ = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
#endif

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

CGraphView::~CGraphView()
{
	ClearData();
}

void CGraphView::ClearData()
{
	GRAPH_DATA_NODE* pCurrNode = headNode_;
	while (pCurrNode != nullptr)
	{
		headNode_ = pCurrNode->_next;
		wxDELETE(pCurrNode);
		pCurrNode = headNode_;
	}

	wxDELETE(headNode_);
}

void CGraphView::OnErase(wxEraseEvent& event)
{

}

void CGraphView::OnSize(wxSizeEvent& event)
{
	wxSize size = event.GetSize();
	m_memDC.ChangeViewSize(size);

    RefreshWindow(this, m_viewRect);
}

void CGraphView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	PrepareDC(dc);

    m_viewRect = GetClientRect();
	wxMemoryDC* pMemDC = m_memDC.CreateMemoryDC(&dc, m_viewRect, wxColour(50, 50, 50), wxColour(10, 10, 10));

	pMemDC->SetFont(font_graphview_);
	::SetViewportOrgEx(pMemDC->GetHDC(), m_viewRect.GetRight(), m_viewRect.GetBottom(), nullptr);

	nviewport_maxx_ = -1 * m_viewRect.GetRight();
	nviewport_maxy_ = -1 * m_viewRect.GetBottom();

	Render(pMemDC);

	::SetViewportOrgEx(pMemDC->GetHDC(), m_viewRect.GetLeft(), m_viewRect.GetTop(), nullptr);
	DisplayCorePercent(pMemDC);
	pMemDC->SetFont(wxNullFont);

	dc.Blit(0, 0, m_viewRect.GetWidth(), m_viewRect.GetHeight(), pMemDC, 0, 0);
}

void CGraphView::Render(wxDC* pDC)
{
	int iXPos = 0;
	int iCurXPos = iXPos;
	int iCurYPos = CONST_ONE_SECOND_Y_HEIGHT;

	wxPen penRow(m_colRuler, 1, wxPENSTYLE_DOT);
	pDC->SetPen(penRow);

	int iRowPos = nviewport_maxy_ / 4;
	for (int i = 0; i < 3; i++)
		pDC->DrawLine(0, (i + 1) * iRowPos, nviewport_maxx_, (i + 1) * iRowPos);

	wxPen penCol(m_colRuler, 1, wxPENSTYLE_SOLID);
	pDC->SetPen(penCol);

	int iTenSecCnt = 0;
	while (iCurXPos > nviewport_maxx_)
	{
		if ((iTenSecCnt % 10) == 0)
		{
			iTenSecCnt = 0;
			iCurYPos = CONST_TEN_SECOND_Y_HEIGHT;
		}
		else
		{
			iCurYPos = CONST_ONE_SECOND_Y_HEIGHT;
		}

		pDC->DrawLine(iCurXPos + 1, 0, iCurXPos + 1, -1 * iCurYPos);
		pDC->DrawLine(iCurXPos + 1, nviewport_maxy_, iCurXPos + 1, nviewport_maxy_ + iCurYPos);

		iCurXPos -= CONST_ONE_SECOND;
		iTenSecCnt++;
	}

	DrawGraph(pDC);
}

void CGraphView::DrawGraph(wxDC* pDC)
{
	if(headNode_ == tailNode_)
		return;

	int iYTop = nviewport_maxy_;
	int iYBottom = 0;

	int iXPos = -1;
	int x1;
	int y1;
	int x2;
	int y2;

	GRAPH_DATA_NODE* pCurrNode = headNode_;
	while (pCurrNode != nullptr)
	{
		iXPos = pCurrNode->_iXPos;

		if (pCurrNode->_bLineDraw)
		{
			wxPen pen(m_colRuler, 1, pCurrNode->_penStyle);
			pDC->SetPen(pen);
			pDC->DrawLine(iXPos, iYTop, iXPos, iYBottom);
		}

		x1 = pCurrNode->_iXPos;
		y1 = pCurrNode->_iYPos;
		if (pCurrNode->_next)
		{
			x2 = pCurrNode->_next->_iXPos;
			y2 = pCurrNode->_next->_iYPos;

			wxPen pen(pCurrNode->_col /*wxColour(0, 0, 255)*/, 1, wxPENSTYLE_SOLID);
			pDC->SetPen(pen);
			pDC->DrawLine(x1, y1, x2, y2);
		//	DrawPixelLine(pDC, x1, y1, x2, y2, pCurrNode->_col);
		}

		pCurrNode = pCurrNode->_next;
	}
}

void CGraphView::DrawPixelLine(wxDC* pDC, int x1, int y1, int x2, int y2, const wxColour& col)
{
	wxPen pen(col, 1, wxPENSTYLE_SOLID);
	pDC->SetPen(pen);

	const bool steep = (abs(y2 - y1) > abs(x2 - x1));
	if(steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if(x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const int dx = x2 - x1;
	const int dy = abs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;
	for(int x = x1; x <= maxX; x++)
	{
		if(steep)
			pDC->DrawPoint(y, x);
		else
			pDC->DrawPoint(x, y);

		error -= dy;
		if(error < 0)
		{
			y += ystep;
			error += dx;
		}
	}
}

void CGraphView::DisplayCorePercent(wxDC* pDC)
{
	wxString strDisp = wxString::Format(wxT("Core #%02d - %02d%"), coreindex_ + 1, percent_usage_);

	pDC->SetTextForeground(wxColour(20, 20, 20));
	pDC->DrawText(strDisp, 5, 2);

	int nRed = 1 + ((255 * percent_usage_) / 100);
	nRed = nRed > 255 ? 255 : nRed;

	int nGreen = 255 - ((255 * percent_usage_) / 100);
	nGreen = nGreen < 0 ? 0 : nGreen;

	wxColour col(nRed, nGreen, 0);

	pDC->SetTextForeground(col);
	pDC->DrawText(strDisp, 4, 1);
}

void CGraphView::UpdateData(wxCommandEvent& event)
{
	UpdateGraphData();
	RefreshWindow(this, m_viewRect);
}

void CGraphView::UpdateGraphData()
{
	if (m_viewRect.GetWidth() == 0)
		return;

	unsigned long ulCPUCoreData = theSystemInfo->GetCPUInfo()->GetCPUUsage(coreindex_);

	int iYPos = (int)(((ulCPUCoreData * m_viewRect.GetHeight()) / 100) * -1);
	if (iYPos < nviewport_maxy_)
		iYPos = nviewport_maxy_;

	percent_usage_ = ulCPUCoreData;

	GRAPH_DATA_NODE* _dataNode = new GRAPH_DATA_NODE();
	_dataNode->_next = nullptr;
	_dataNode->_iXPos = 1;
	_dataNode->_ulData = percent_usage_;
	_dataNode->_iYPos = iYPos;

	addcount_++;
	if ((addcount_ % 10) != 0)
		_dataNode->_bLineDraw = false;

	if (headNode_ == nullptr)
	{
		headNode_ = _dataNode;
		tailNode_ = _dataNode;
	}
	else
	{
		tailNode_->_next = _dataNode;
		tailNode_ = _dataNode;

		GRAPH_DATA_NODE* pCurrNode = headNode_;
		while (pCurrNode != nullptr)
		{
			pCurrNode->_iXPos -= CONST_ONE_SECOND;
			pCurrNode = pCurrNode->_next;

			if(pCurrNode == tailNode_)
				break;
		}

		GRAPH_DATA_NODE* firstNode = headNode_;
		if (firstNode->_iXPos <= nviewport_maxx_)
		{
			headNode_ = firstNode->_next;
			wxDELETE(firstNode);
		}
	}

	if (addcount_ == ten_secound_)
		addcount_ = 0;
}
