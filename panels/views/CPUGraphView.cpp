#include "../../ginc.h"
#include "CPUGraphView.h"

wxBEGIN_EVENT_TABLE(CCPUGraphView, CCommonViewImpl)
EVT_PAINT(CCPUGraphView::OnPaint)
EVT_SIZE(CCPUGraphView::OnSize)
EVT_ERASE_BACKGROUND(CCPUGraphView::OnErase)
wxEND_EVENT_TABLE()

CCPUGraphView::CCPUGraphView(wxWindow* _parent, const wxSize& _size)
	: CCommonViewImpl(_parent, wxID_ANY, wxDefaultPosition, _size, VIEW_STYLE)
	, m_colRuler(140, 140, 140)
{
}

CCPUGraphView::~CCPUGraphView()
{
	ClearData();
}

void CCPUGraphView::ClearData()
{
	GRAPH_DATA_NODE* pCurrNode = m_headNode;
	while (pCurrNode != nullptr)
	{
		GRAPH_DATA_NODE* tmpTarget = pCurrNode;
		pCurrNode = pCurrNode->_next;
		delete tmpTarget;
	}
}

void CCPUGraphView::OnPaint(wxPaintEvent& event)
{
	if(!m_pDoubleBuffer)
		return;
		
	if (!m_pDoubleBuffer->IsOk())
		return;

	m_viewRect = GetClientRect();

	wxPaintDC dc(this);
	PrepareDC(dc);

	wxMemoryDC* pMemDC = CreateMemoryDC(&dc, m_viewRect);

	::SetViewportOrgEx(pMemDC->GetHDC(), m_viewRect.GetRight(), m_viewRect.GetBottom(), nullptr);
	m_iViewportMaxX = -1 * m_viewRect.GetRight();
	m_iViewportMaxY = -1 * m_viewRect.GetBottom();

	Render(pMemDC);

	dc.Blit(0, 0, m_pDoubleBuffer->GetWidth(), m_pDoubleBuffer->GetHeight(), pMemDC, m_iViewportMaxX, m_iViewportMaxY);

	ClearMemoryDC(pMemDC);
}

void CCPUGraphView::Render(wxDC* pDC)
{
	int iXPos = 0;
	int iCurXPos = iXPos;
	int iCurYPos = CONST_ONE_SECOND_Y_HEIGHT;

	wxPen pen(m_colRuler, 1, wxPENSTYLE_DOT);
	pDC->SetPen(pen);

	int iRowPos = m_iViewportMaxY / 4;
	for (int i = 0; i < 3; i++)
		pDC->DrawLine(0, (i + 1) * iRowPos, m_iViewportMaxX, (i + 1) * iRowPos);
	
	pen.SetColour(m_colRuler);
	pen.SetStyle(wxPENSTYLE_SOLID);
	pDC->SetPen(pen);

	int iTenSecCnt = 0;
	while (iCurXPos > m_iViewportMaxX)
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
		pDC->DrawLine(iCurXPos + 1, m_iViewportMaxY, iCurXPos + 1, m_iViewportMaxY + iCurYPos);
		
		iCurXPos -= CONST_ONE_SECOND;
		iTenSecCnt++;
	}

	DrawGraph(pDC);
	
	if (!m_strDispText.IsEmpty())
	{
		wxString strDisp = m_strDispText + wxString::Format(wxT(" : %02d"), m_iPercent) + wxT("%");

		pDC->SetTextForeground(wxColour(150, 150, 150));
		pDC->DrawText(strDisp, m_iViewportMaxX + 5, m_iViewportMaxY + 2);
		
		pDC->SetTextForeground(wxColour(225, 225, 225));
		pDC->DrawText(strDisp, m_iViewportMaxX + 4, m_iViewportMaxY + 1);
	}
}

void CCPUGraphView::OnSize(wxSizeEvent& event)
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

void CCPUGraphView::OnErase(wxEraseEvent& event)
{

}

void CCPUGraphView::DrawGraph(wxDC* pDC)
{
	int iYTop = m_iViewportMaxY;
	int iYBottom = 0;

	if (m_headNode == nullptr || m_headNode->_next == nullptr)
		return;

	int iXPos = -1;
	int x1;
	int y1;
	int x2;
	int y2;

	GRAPH_DATA_NODE* pCurrNode = m_headNode->_next;
	
	while (pCurrNode != nullptr)
	{
		iXPos = pCurrNode->_iXPos;

		if (pCurrNode->_bLineDraw)
		{
			wxPen pen(pCurrNode->_col, 1, pCurrNode->_penStyle);
			pDC->SetPen(pen);
			pDC->DrawLine(iXPos, iYTop, iXPos, iYBottom);
		}

		x1 = pCurrNode->_iXPos;
		y1 = pCurrNode->_iYPos;
		if (pCurrNode->_next)
		{
			x2 = pCurrNode->_next->_iXPos;
			y2 = pCurrNode->_next->_iYPos;

			wxPen pen(wxColour(0, 255, 0), 1, wxPENSTYLE_SOLID);
			pDC->SetPen(pen);
			pDC->DrawLine(x1, y1, x2, y2);
			/*
			int xTmp = x1;
			int yTmp = y1;
			while(xTmp < x2)
			{
				pDC->DrawLine(xTmp, yTmp, xTmp, iYBottom);
				
				if(yTmp < y2)
					yTmp++;
				else
					yTmp--;
					
				xTmp++;
			}*/
		}
		
		pCurrNode = pCurrNode->_next;
	}
}

void CCPUGraphView::UpdateData(unsigned long _val)
{
	if (m_viewRect.GetWidth() == 0)
		return;
	
	int iYPos = (int)(((_val * m_viewRect.GetHeight()) / 100) * -1);
	if (iYPos < m_iViewportMaxY)
		iYPos = m_iViewportMaxY;

	m_iPercent = wx_static_cast(int, _val);

	GRAPH_DATA_NODE* _dataNode = new GRAPH_DATA_NODE();
	_dataNode->_next = nullptr;
	_dataNode->_iXPos = 1;
	_dataNode->_ulData = _val;
	_dataNode->_iYPos = iYPos;

	m_iAddCount++;
	if ((m_iAddCount % 10) != 0)
		_dataNode->_bLineDraw = false;

	if (m_headNode == nullptr)
	{
		m_headNode = new GRAPH_DATA_NODE;
		m_headNode->_next = _dataNode;
	}
	else
	{
		GRAPH_DATA_NODE* pLastNode = m_headNode;
		GRAPH_DATA_NODE* pTmpNode = m_headNode->_next;
		while (pTmpNode != nullptr)
		{
			if (pLastNode->_next != nullptr)
				pLastNode = pLastNode->_next;

			pTmpNode->_iXPos -= CONST_ONE_SECOND;
			pTmpNode = pTmpNode->_next;
		}

		pLastNode->_next = _dataNode;

		GRAPH_DATA_NODE* firstNode = m_headNode->_next;
		if (firstNode->_iXPos <= m_iViewportMaxX)
		{
			m_headNode->_next = firstNode->_next;
			delete firstNode;
		}
	}

	if (m_iAddCount == 10)
		m_iAddCount = 0;

	UpdateView(this, m_viewRect);
}