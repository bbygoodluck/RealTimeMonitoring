#include "../ginc.h"
#include "../system/system.h"
#include "NetworkUsageGraphView.h"

wxBEGIN_EVENT_TABLE(CNetworkUsageGraphView, wxWindow)
	EVT_PAINT(CNetworkUsageGraphView::OnPaint)
	EVT_SIZE(CNetworkUsageGraphView::OnSize)
	EVT_ERASE_BACKGROUND(CNetworkUsageGraphView::OnErase)
	EVT_MY_CUSTOM_COMMAND(wxEVT_RESOURCE_MONITORING, wxID_ANY, CNetworkUsageGraphView::UpdateData)
wxEND_EVENT_TABLE()

CNetworkUsageGraphView::CNetworkUsageGraphView(wxWindow* parent, const int nID, const wxSize& sz, long lStyle)
	: wxWindow(parent, nID, wxDefaultPosition, sz, lStyle)
	, m_colRuler(128, 128, 128)
{
//	this->Connect(wxEVT_TIMER, wxTimerEventHandler(CNetworkUsageGraphView::OnTimer));
//	m_timer.SetOwner(this, wxTIMER_ID++);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

#ifdef __WXMSW__
	font_graphview_.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Consolas"));
#else
	font_graphview_ = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
#endif
}

CNetworkUsageGraphView::~CNetworkUsageGraphView()
{
	Clear(ruler_headNode_);
	Clear(headNode_in_);
	Clear(headNode_out_);
}

void CNetworkUsageGraphView::Clear(GRAPH_DATA_NODE* _node)
{
	GRAPH_DATA_NODE* pCurrNode = _node;
	while (pCurrNode != nullptr)
	{
		_node = pCurrNode->_next;
		wxDELETE(pCurrNode);

		pCurrNode = _node;
	}

	wxDELETE(_node);
}

void CNetworkUsageGraphView::OnErase(wxEraseEvent& event)
{

}

void CNetworkUsageGraphView::OnSize(wxSizeEvent& event)
{
	wxSize size = event.GetSize();
	m_memDC.ChangeViewSize(size);

    RefreshWindow(this, m_viewRect);
}

void CNetworkUsageGraphView::CreateHeadTailNode()
{
	GRAPH_DATA_NODE* pDummyIn = new GRAPH_DATA_NODE();
	pDummyIn->_next = nullptr;
	pDummyIn->_iXPos = nusageview_x_;
	pDummyIn->_iYPos = 0;

	headNode_in_ = pDummyIn;
	tailNode_in_ = pDummyIn;

	GRAPH_DATA_NODE* pDummyOut = new GRAPH_DATA_NODE();
	pDummyOut->_next = nullptr;
	pDummyOut->_iXPos = nusageview_x_;
	pDummyOut->_iYPos = 0;

	headNode_out_ = pDummyOut;
	tailNode_out_ = pDummyOut;
}

void CNetworkUsageGraphView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	PrepareDC(dc);

    m_viewRect = GetClientRect();

	wxMemoryDC* pMemDC = m_memDC.CreateMemoryDC(&dc, m_viewRect);
	pMemDC->SetFont(font_graphview_);
	::SetViewportOrgEx(pMemDC->GetHDC(), m_viewRect.GetRight(), m_viewRect.GetBottom(), nullptr);

	nviewport_maxx_ = -1 * m_viewRect.GetRight();
	nviewport_maxy_ = -1 * m_viewRect.GetBottom();

	nusageview_x_ = -1 * ((m_viewRect.GetWidth() / 6) - 50);
	nusageview_y_ = nviewport_maxy_;

	if(!bCreatedHeadTailNode_)
	{
		CreateHeadTailNode();
		bCreatedHeadTailNode_ = true;
	}

	Render(pMemDC);
	pMemDC->SetFont(wxNullFont);

	::SetViewportOrgEx(pMemDC->GetHDC(), m_viewRect.GetLeft(), m_viewRect.GetTop(), nullptr);
	dc.Blit(0, 0, m_viewRect.GetWidth(), m_viewRect.GetHeight(), pMemDC, 0, 0);
}

void CNetworkUsageGraphView::Render(wxDC* pDC)
{
	wxPen pen(wxColour(0, 0, 0));
	wxBrush brush(wxColour(50, 50, 50));

	m_viewUsageRect.SetLeft(nviewport_maxx_);
	m_viewUsageRect.SetTop(nviewport_maxy_);
	m_viewUsageRect.SetRight(nusageview_x_ + 1);
	m_viewUsageRect.SetBottom(0);

	pDC->SetPen(pen);
	pDC->SetBrush(brush);
	pDC->DrawRectangle(m_viewUsageRect);

	int iXPos = nusageview_x_;
	int iCurXPos = iXPos;
	int iCurYPos = CONST_ONE_SECOND_Y_HEIGHT;

	wxPen penRow(m_colRuler, 1, wxPENSTYLE_DOT);
	pDC->SetPen(penRow);

	int iRowPos = nviewport_maxy_ / 4;
	for (int i = 0; i < 3; i++)
		pDC->DrawLine(nusageview_x_, (i + 1) * iRowPos, nviewport_maxx_, (i + 1) * iRowPos);

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

	DrawTenSecondRuler(pDC);
	DrawNetworkUsage(pDC);

	DrawData(pDC, headNode_in_, tailNode_in_);
	DrawData(pDC, headNode_out_, tailNode_out_);
}

void CNetworkUsageGraphView::DrawTenSecondRuler(wxDC* pDC)
{
	if(ruler_headNode_ == ruler_tailNode_)
		return;

	int iYTop = nviewport_maxy_;
	int iYBottom = 0;

	int iXPos = -1;

	GRAPH_DATA_NODE* pCurrNode = ruler_headNode_;
	while (pCurrNode != nullptr)
	{
		iXPos = pCurrNode->_iXPos;

		if (pCurrNode->_bLineDraw)
		{
			wxPen pen(m_colRuler, 1, pCurrNode->_penStyle);
			pDC->SetPen(pen);
			pDC->DrawLine(iXPos, iYTop, iXPos, iYBottom);
		}

		pCurrNode = pCurrNode->_next;
	}
}

void CNetworkUsageGraphView::DrawNetworkUsage(wxDC* pDC)
{
	wxPen penIn(m_colIn, 1);
	wxPen penOut(m_colOut, 1);

	wxString strInMaxBps(wxT(""));
	wxString strOutMaxBps(wxT(""));

	if(ulMaxbps_ <= 100)
	{
		strInMaxBps  = GetBandWidth(ulMaxbps_, false);//wxString::Format(wxT("%uKbps"), ulMaxbps_);
		strOutMaxBps = GetBandWidth(ulMaxbps_, false);//wxString::Format(wxT("%uKbps"), ulMaxbps_);

		pDC->SetTextForeground(m_colOut);
		pDC->DrawText(strOutMaxBps, m_viewUsageRect.GetLeft() + 2, m_viewUsageRect.GetTop() + 1);

		wxSize szBps = pDC->GetTextExtent(strInMaxBps);
		pDC->SetTextForeground(m_colIn);
		pDC->DrawText(strInMaxBps, m_viewUsageRect.GetRight() - (szBps.GetWidth() + 1), m_viewUsageRect.GetTop() + 1);
	}
	else
	{
		int nYPos = 0;
		int nNewYPos = 0;
		wxSize szBps;

		if(IsMaxIn)
		{
			strInMaxBps  = GetBandWidth(ulMaxbps_, false);//wxString::Format(wxT("%uKbps"), ulMaxbps_);
			strOutMaxBps = GetBandWidth(ulOutMaxbps_, false);//wxString::Format(wxT("%uKbps"), ulOutMaxbps_);

			szBps = pDC->GetTextExtent(strInMaxBps);

			pDC->SetTextForeground(m_colIn);
			pDC->DrawText(strInMaxBps, m_viewUsageRect.GetRight() - (szBps.GetWidth() + 1), m_viewUsageRect.GetTop() + 1);

			nYPos = (int)(((ulOutMaxbps_ * m_viewRect.GetHeight()) / ulMaxbps_) * -1);

			nNewYPos = nYPos - (szBps.GetHeight() + 1);

			if(nNewYPos < m_viewUsageRect.GetTop())
				nNewYPos = m_viewUsageRect.GetTop() + 2;
			else if(nNewYPos > 0)
				nNewYPos = m_viewUsageRect.GetBottom() - (szBps.GetHeight() + 1);

			pDC->SetPen(penOut);
			pDC->DrawLine(m_viewUsageRect.GetLeft(), nYPos, m_viewUsageRect.GetRight(), nYPos);
			pDC->SetTextForeground(m_colOut);
			pDC->DrawText(strOutMaxBps, m_viewUsageRect.GetLeft() + 2, nNewYPos);
		}
		else
		{
			strInMaxBps  = GetBandWidth(ulInMaxbps_, false);//wxString::Format(wxT("%uKbps"), ulInMaxbps_);
			strOutMaxBps = GetBandWidth(ulMaxbps_, false);//wxString::Format(wxT("%uKbps"), ulMaxbps_);

			szBps = pDC->GetTextExtent(strInMaxBps);

			pDC->SetTextForeground(m_colOut);
			pDC->DrawText(strOutMaxBps, m_viewUsageRect.GetLeft() + 1, m_viewUsageRect.GetTop() + 1);

			nYPos = (int)(((ulInMaxbps_ * m_viewRect.GetHeight()) / ulMaxbps_) * -1);

			nNewYPos = nYPos - (szBps.GetHeight() + 1);

			if(nNewYPos < m_viewUsageRect.GetTop())
				nNewYPos = m_viewUsageRect.GetTop() + 2;
			else if(nNewYPos > 0)
				nNewYPos = m_viewUsageRect.GetBottom() - (szBps.GetHeight() + 1);

			pDC->SetPen(penIn);
			pDC->DrawLine(m_viewUsageRect.GetLeft(), nYPos, m_viewUsageRect.GetRight(), nYPos);
			pDC->SetTextForeground(m_colIn);
			pDC->DrawText(strInMaxBps, m_viewUsageRect.GetRight() - (szBps.GetWidth() + 1), nNewYPos);
		}
	}

	//************************************************************************************************************
	//                                           In Out 정보, 속도
	//************************************************************************************************************
	wxRect rcPacketInfo;
	rcPacketInfo.SetLeft(m_viewUsageRect.GetRight());
	rcPacketInfo.SetTop(m_viewUsageRect.GetTop());
	rcPacketInfo.SetRight(0);
	rcPacketInfo.SetBottom(0);

	int nLeft   = rcPacketInfo.GetLeft();
	int nTop    = rcPacketInfo.GetTop();
	int nRight  = rcPacketInfo.GetRight();
	int nBottom = rcPacketInfo.GetBottom();

	int nXCenter = rcPacketInfo.GetWidth() / 2;
	int nYCenter = rcPacketInfo.GetHeight() / 2;

	int nPadding = 3;
	//************************************************************************************************************
	//                                           Received
	//************************************************************************************************************

	auto fnSpeed = [](double _dblSpeed, bool _bDown) {
		double dblGB = 1000.0f * 1000.0f;
		double dblMB = 1000.0f;

		double dblspdG = _dblSpeed / dblGB;
		double dblspdM = _dblSpeed / dblMB;

		wxString strRet(wxT(""));
		if(_dblSpeed > dblGB)
			strRet = _bDown ? wxString::Format(wxT("Recv Spd : %.2f GiB/sec"), dblspdG) : wxString::Format(wxT("Send Spd : %.2f GiB/sec"), dblspdG);
		else if(_dblSpeed > dblMB)
			strRet = _bDown ? wxString::Format(wxT("Recv Spd : %.2f MiB/sec"), dblspdM) : wxString::Format(wxT("Send Spd : %.2f MiB/sec"), dblspdM);
		else
			strRet = _bDown ? wxString::Format(wxT("Recv Spd : %.2f KiB/sec"), _dblSpeed) : wxString::Format(wxT("Send Spd : %.2f KiB/sec"), _dblSpeed);

		return strRet;
	};

	pDC->SetPen(penIn);
	pDC->DrawLine(nLeft + nPadding + 5, nTop + 2, nLeft + nPadding + 5, (nTop + nYCenter) - 2);

	double dblDnSpd       = dblDownSpeed_;// / 1024.0f * 8;
	wxString strDownSpeed = fnSpeed(dblDnSpd, true);//wxString::Format(wxT("Recv Spd : %.2f KiB/sec"), dblDownSpeed_ / 1024.0f * 8);
	wxString strInMax     = GetBandWidth(ulInMaxbps_, true);//wxString::Format(wxT("Mx : %uKbps"), ulInMaxbps_);
	wxSize szInData       = pDC->GetTextExtent(strInData_);

	pDC->SetTextForeground(m_colIn);
	pDC->DrawText(strDownSpeed, nLeft + nPadding + 10, nTop + 3);
	pDC->DrawText(strInData_, nLeft + nPadding + 10, (nTop + nYCenter) - (szInData.GetHeight() + 2));
	pDC->DrawText(strInMax, nLeft + nXCenter - 10, (nTop + nYCenter) - (szInData.GetHeight() + 2));

	//************************************************************************************************************
	//                                           Sent
	//************************************************************************************************************
	pDC->SetPen(penOut);
	pDC->DrawLine(nLeft + nPadding + 5, nTop + nYCenter + 2, nLeft + nPadding + 5, -2);

	double dblUpSpd     = dblUpSpeed_;// / 1024.0f * 8;
	wxString strUpSpeed = fnSpeed(dblUpSpd, false);//wxString::Format(wxT("Send Spd : %.2f KiB/sec"), dblUpSpeed_ / 1024.0f * 8);
	wxString strOutMax  = GetBandWidth(ulOutMaxbps_, true);//wxString::Format(wxT("Mx : %uKbps"), ulOutMaxbps_);
	wxSize szOutData    = pDC->GetTextExtent(strOutData_);

	pDC->SetTextForeground(m_colOut);
	pDC->DrawText(strUpSpeed, nLeft + nPadding + 10, nTop + nYCenter + 3);
	pDC->DrawText(strOutData_, nLeft + nPadding + 10, -1 * (szOutData.GetHeight() + 2));
	pDC->DrawText(strOutMax, nLeft + nXCenter - 10, -1 * (szOutData.GetHeight() + 2));
}

void CNetworkUsageGraphView::DrawData(wxDC* pDC, GRAPH_DATA_NODE* _head, GRAPH_DATA_NODE* _tail)
{
	if(_head == _tail)
		return;

	int iYTop = nviewport_maxy_;
	int iYBottom = 0;

	int iXPos = -1;

	int x1;
	int y1;
	int x2;
	int y2;

	GRAPH_DATA_NODE* pCurrNode = _head;
	while (pCurrNode != nullptr)
	{
		x1 = pCurrNode->_iXPos;
		y1 = pCurrNode->_iYPos;

		if (pCurrNode->_next)
		{
			x2 = pCurrNode->_next->_iXPos;
			y2 = pCurrNode->_next->_iYPos;

			wxPen pen(pCurrNode->_col, 1, wxPENSTYLE_SOLID);
			pDC->SetPen(pen);
			pDC->DrawLine(x1, y1, x2, y2);
		}

		pCurrNode = pCurrNode->_next;
	}
}
/*
void CNetworkUsageGraphView::OnTimer(wxTimerEvent& event)
{
	UpdateTenSecondRuler();
	theUtility->RefreshWindow(this, m_viewRect);
}
*/
void CNetworkUsageGraphView::UpdateTenSecondRuler()
{
	GRAPH_DATA_NODE* _dataNodeRuler = new GRAPH_DATA_NODE();
	_dataNodeRuler->_next = nullptr;
	_dataNodeRuler->_iXPos = nusageview_x_;
	_dataNodeRuler->_iYPos = nviewport_maxy_;

	addcount_++;
	if ((addcount_ % ten_secound_) != 0)
		_dataNodeRuler->_bLineDraw = false;

	if (ruler_headNode_ == nullptr)
	{
		ruler_headNode_ = _dataNodeRuler;
		ruler_tailNode_ = _dataNodeRuler;
	}
	else
	{
		if(_dataNodeRuler != nullptr)
		{
			ruler_tailNode_->_next = _dataNodeRuler;
			ruler_tailNode_ = _dataNodeRuler;
		}

		GRAPH_DATA_NODE* pCurrNode = ruler_headNode_;
		while (pCurrNode != nullptr)
		{
			pCurrNode->_iXPos -= CONST_ONE_SECOND;
			pCurrNode = pCurrNode->_next;

			if(pCurrNode == ruler_tailNode_)
				break;
		}

		GRAPH_DATA_NODE* firstNode = ruler_headNode_;
		if (firstNode->_iXPos <= nviewport_maxx_)
		{
			ruler_headNode_ = firstNode->_next;
			wxDELETE(firstNode);
		}
	}

	if (addcount_ == ten_secound_)
		addcount_ = 0;
}

void CNetworkUsageGraphView::UpdateData(wxCommandEvent& event)
{
	UpdateTenSecondRuler();
	Update();
}

wxString CNetworkUsageGraphView::GetBandWidth(unsigned long _ulbps, bool _bMaxDisp)
{
	unsigned int iMega = 1000;
	unsigned int iGiga = 1000 * 1000;

	unsigned long ulM = _ulbps / iMega;
	unsigned long ulG = _ulbps / iGiga;

	wxString strRet(wxT(""));
	if(_ulbps > iGiga)
	{
		strRet = _bMaxDisp ? wxString::Format(wxT("Mx : %ldGbps") , ulG) :
							 wxString::Format(wxT("%ldGbps"), ulG);
	}
	else if(_ulbps > iMega)
	{
		strRet = _bMaxDisp ? wxString::Format(wxT("Mx : %ldMbps") , ulM) :
							 wxString::Format(wxT("%ldMbps"), ulM);
	}
	else
	{
		strRet = _bMaxDisp ? wxString::Format(wxT("Mx : %ldKbps") , _ulbps) :
							 wxString::Format(wxT("%ldKbps"), _ulbps);
	}

	return strRet;
}

void CNetworkUsageGraphView::Update()
{
	ulInData_     = theSystemInfo->GetNetInfo()->GetInBytes();
	ulOutData_    = theSystemInfo->GetNetInfo()->GetOutBytes();

	dblDownSpeed_ = theSystemInfo->GetNetInfo()->GetDownSpeed();
	dblUpSpeed_   = theSystemInfo->GetNetInfo()->GetUpSpeed();

	if(ulInData_ > 0)
		ulInData_ = ulInData_ / 1000;//1024 * 8;

	if(ulInMaxbps_ < ulInData_)
		ulInMaxbps_ = ulInData_;

	strInData_ = GetBandWidth(ulInData_, false);

	if(ulOutData_ > 0)
		ulOutData_ = ulOutData_ / 1000;//1024 * 8 ;

	if(ulOutMaxbps_ < ulOutData_)
		ulOutMaxbps_ = ulOutData_;

	strOutData_ = GetBandWidth(ulOutData_, false);

	if(initMaxbpstime_ == -1)
	{
		bool bChecked1000 = false;
		bool bChecked500  = false;

		bChecked1000 = CheckAndMaxBPS(1000);
		if(!bChecked1000)
			bChecked500 = CheckAndMaxBPS(500);

		IsChangedMaxbps = true;
		if(!bChecked1000 && !bChecked500)
			IsChangedMaxbps = false;
	}

	if(initMaxbpstime_ > 60)
	{
		IsChangedMaxbps = false;

		initMaxbpstime_ = -1;

		ulInMaxbps_  = 0;
		ulOutMaxbps_ = 0;
		ulMaxbps_    = 100;

		UpdateNodePosition(headNode_in_, tailNode_in_, true);
		UpdateNodePosition(headNode_out_, tailNode_out_, false);
	}

	if(IsChangedMaxbps)
		initMaxbpstime_++;

	AddNode(true, ulInData_);
	AddNode(false, ulOutData_);


	RefreshWindow(this, m_viewRect);
}

bool CNetworkUsageGraphView::CheckAndMaxBPS(unsigned long _value)
{
	if(ulInMaxbps_ >= _value || ulOutMaxbps_ >= _value)
	{
		ulMaxbps_ = _value;

		IsMaxIn = true;
		if(ulInMaxbps_ < ulOutMaxbps_)
			IsMaxIn = false;

		UpdateNodePosition(headNode_in_, tailNode_in_, true);
		UpdateNodePosition(headNode_out_, tailNode_out_, false);

		return true;
	}

	return false;
}

void CNetworkUsageGraphView::UpdateNodePosition(GRAPH_DATA_NODE* _head, GRAPH_DATA_NODE* _tail, bool _isIn)
{
	GRAPH_DATA_NODE* pCurrNode = _head;
	while (pCurrNode != nullptr)
	{
		int iYPos = (int)(((pCurrNode->_ulData * m_viewRect.GetHeight()) / ulMaxbps_) * -1);
		pCurrNode->_iYPos = iYPos;
		pCurrNode = pCurrNode->_next;

		if(pCurrNode == _tail)
			break;
	}
}

void CNetworkUsageGraphView::AddNode(bool IsIn, unsigned int ulData)
{
	GRAPH_DATA_NODE* _head = nullptr;
	GRAPH_DATA_NODE* _tail = nullptr;

	if(IsIn)
	{
		_head = headNode_in_;
		_tail = tailNode_in_;
	}
	else
	{
		_head = headNode_out_;
		_tail = tailNode_out_;
	}

	int iYPos = (int)(((ulData * m_viewRect.GetHeight()) / ulMaxbps_) * -1);

	GRAPH_DATA_NODE* _data = new GRAPH_DATA_NODE();

	_data->_next = nullptr;
	_data->_ulData = ulData;
	_data->_bInData = IsIn;
	_data->_col = IsIn ? m_colIn : m_colOut;
	_data->_iXPos = nusageview_x_;
	_data->_iYPos = iYPos;

	_tail->_next = _data;
	_tail = _data;

	GRAPH_DATA_NODE* pCurrNode = _head;
	while (pCurrNode != nullptr)
	{
		pCurrNode->_iXPos -= CONST_ONE_SECOND;
		pCurrNode = pCurrNode->_next;

		if(pCurrNode == _tail)
			break;
	}

	GRAPH_DATA_NODE* firstNode = _head;
	if (firstNode->_iXPos <= nviewport_maxx_)
	{
		_head = firstNode->_next;
		wxDELETE(firstNode);
	}

	if(IsIn)
	{
		headNode_in_ = _head;
		tailNode_in_ = _tail;
	}
	else
	{
		headNode_out_ = _head;
		tailNode_out_ = _tail;
	}
}

/*
void CNetworkUsageGraphView::ShadowStart()
{
	m_timer.Start(1000);
}

void CNetworkUsageGraphView::ShadowStop()
{
	m_timer.Stop();
}
*/
