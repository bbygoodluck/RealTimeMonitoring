#include "../ginc.h"
#include "UsageView.h"
#include "../system/system.h"

wxBEGIN_EVENT_TABLE(CUsageView, wxWindow)
	EVT_PAINT(CUsageView::OnPaint)
	EVT_SIZE(CUsageView::OnSize)
	EVT_ERASE_BACKGROUND(CUsageView::OnErase)
	EVT_MY_CUSTOM_COMMAND(wxEVT_RESOURCE_MONITORING, wxID_ANY, CUsageView::UpdateData)
wxEND_EVENT_TABLE()


CUsageView::CUsageView(wxWindow* parent, const int nID, const wxSize& sz, USAGE_TYPE usage_type, long lStyle)
	: wxWindow(parent, nID, wxDefaultPosition, sz, lStyle)
	, e_usage_type(usage_type)
{
	m_Datas.reserve(100);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	typedef struct stUsageList {
		int _id;
		std::function<void()> funcPtr;
	} USAGE_LIST;

	USAGE_LIST stUsageList[] = {
		{ USAGE_CPU,             std::bind(&CUsageView::CPUUsage           , this) },
		{ USAGE_MEMORY_PHYSIC,   std::bind(&CUsageView::MemoryPhysicsUsage , this) },
		{ USAGE_MEMORY_VIRTUAL,  std::bind(&CUsageView::MemoryVirtualUsage , this) },
		{ USAGE_MEMORY_PAGEFILE, std::bind(&CUsageView::PageFileUsage      , this) },
	};

	for(unsigned int i = 0; i < WXSIZEOF(stUsageList); i++)
		m_mapFuncPtr.insert(std::make_pair(stUsageList[i]._id, stUsageList[i].funcPtr));
}

CUsageView::~CUsageView()
{
	ClearData();

	wxVector<SPECTRUM_DATA *>().swap(m_Datas);
	m_Datas.reserve(0);
}

void CUsageView::OnErase(wxEraseEvent& event)
{

}

void CUsageView::OnSize(wxSizeEvent& event)
{
	wxSize size = event.GetSize();
	m_memDC.ChangeViewSize(size);

    RefreshWindow(this, m_viewRect);
}

void CUsageView::OnPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	PrepareDC(dc);

    m_viewRect = GetClientRect();
    m_viewRectSpectrum = m_viewRect;
	wxMemoryDC* pMemDC = m_memDC.CreateMemoryDC(&dc, m_viewRect);

	if(!_bCreatedSpectrum)
		CreateSpectrumData(pMemDC);

	Render(pMemDC);
	dc.Blit(0, 0, m_viewRect.GetWidth(), m_viewRect.GetHeight(), pMemDC, 0, 0);
}

void CUsageView::ClearData()
{
	for (auto item : m_Datas)
	{
		SPECTRUM_DATA* pData = item;
		wxDELETE(pData);
	}

	m_Datas.clear();
	m_Datas.reserve(100);
}

void CUsageView::Render(wxDC* pDC)
{
	if(m_Datas.size() == 0)
		return;

	pDC->SetFont(*_gViewFont);

	DrawSpectrum(pDC);
	DrawUsageSpectrum(pDC);
}

void CUsageView::CreateSpectrumData(wxDC* pDC)
{
	ClearData();

	int iXStartPos = 0;
	int iXEndPos = _gap;
	int iWidth = 3;
	int iHeight = m_viewRectSpectrum.GetHeight() - _gap;
	int iRed = 0;
	int iGreen = 0;

	m_SpectrumDatas = 0;

	int nRightPos = m_viewRectSpectrum.GetRight() - _gap;
	while (iXStartPos < nRightPos)
	{
		iXStartPos = iXEndPos;
		wxRect rc(iXStartPos, 2, iWidth, iHeight);

		if(rc.GetRight() > nRightPos + _gap)
			break;

		SPECTRUM_DATA* pSpectrumInfo = new SPECTRUM_DATA();
		pSpectrumInfo->_rect = rc;

		m_Datas.push_back(pSpectrumInfo);
		iXEndPos = rc.GetRight() + _gap;
	}

	m_SpectrumDatas = m_Datas.size();
	int nIndex = 1;

	wxVector<SPECTRUM_DATA *>::const_iterator iter = nullptr;
	for(iter = m_Datas.begin(); iter != m_Datas.end(); ++iter)
	{
		SPECTRUM_DATA* pData = *iter;

		iRed = 1 + ((255 * nIndex) / m_SpectrumDatas);
		iRed = iRed > 255 ? 255 : iRed;

		iGreen = 255 - ((255 * nIndex) / m_SpectrumDatas);
		iGreen = iGreen < 0 ? 0 : iGreen;

		wxColour col(iRed, iGreen, 0);
		pData->_col = col;

		nIndex++;
	}

	_bCreatedSpectrum = true;
}

void CUsageView::DrawSpectrum(wxDC* pDC)
{
	wxVector<SPECTRUM_DATA *>::const_iterator iter = nullptr;
	for(iter = m_Datas.begin(); iter != m_Datas.end(); ++iter)
	{
		SPECTRUM_DATA* pData = *iter;
		wxPen pen(wxColour(_redBack - 10, _greenBack + 10, _blueBack - 10));
		wxBrush brush(wxColour(_redBack, _greenBack, _blueBack));

	//	wxPen pen(wxColour(30, 30, 30));
	//	wxBrush brush(wxColour(30, 30, 30));

		pDC->SetPen(pen);
		pDC->SetBrush(brush);

		pDC->DrawRectangle(pData->_rect);
	}
}
void CUsageView::DrawUsageSpectrum(wxDC* pDC)
{
	if(m_SpectrumDatas == 0)
		return;

	wxColour colCur;
	for(unsigned int index = 0; index < m_UsageCount; index++)
	{
		SPECTRUM_DATA* pData = m_Datas.at(index);
		wxPen pen(pData->_col);
		wxBrush brush(pData->_col);

		pDC->SetPen(pen);
		pDC->SetBrush(brush);

		pDC->DrawRectangle(pData->_rect);

		colCur = pData->_col;
	}

	wxRect viewRectShadow = m_viewRectSpectrum;
	viewRectShadow.SetLeft(m_viewRectSpectrum.GetLeft() + 1);
	viewRectShadow.SetTop(m_viewRectSpectrum.GetTop() + 1);

	pDC->SetTextForeground(wxColour(90, 90, 90));
	pDC->DrawLabel(_strUsage, viewRectShadow, wxALIGN_CENTER);

//	pDC->SetTextForeground(wxColour(250, 250, 250));
	pDC->SetTextForeground(wxColour(250, 250, 250));
	pDC->DrawLabel(_strUsage, m_viewRectSpectrum, wxALIGN_CENTER);
}

void CUsageView::UpdateData(wxCommandEvent& event)
{
	if(e_usage_type == USAGE_TYPE::USAGE_NONE)
		return;

	std::unordered_map< int, std::function<void()> >::const_iterator fIter = m_mapFuncPtr.find(e_usage_type);
	if(fIter == m_mapFuncPtr.end())
		return;

	auto functionPtr = fIter->second;
	(functionPtr)();

	RefreshWindow(this, m_viewRect);

	event.Skip();
}

void CUsageView::CPUUsage()
{
	unsigned long ulUsageData = theSystemInfo->GetCPUInfo()->GetCPUUsage(_ProcessorCoreIndex);
	m_UsagePercent = ulUsageData;

	_strUsage = wxString::Format(wxT("CPU : %u%"), m_UsagePercent);
	CalcUsage();
}

void CUsageView::MemoryPhysicsUsage()
{
	CMemoryInfo* pMemInfo = theSystemInfo->GetMemoryInfo();

	unsigned long ulTotalMem = pMemInfo->GetTotalMemory(USAGE_MEMORY_PHYSIC);
	unsigned long ulUsedMem = pMemInfo->GetUsedMemory(USAGE_MEMORY_PHYSIC);

	wxString strFormat = wxT("Memory(%u%) : %.2f GB / %.2f GB");
	MemoryCalc(strFormat, ulTotalMem, ulUsedMem);
}

void CUsageView::MemoryVirtualUsage()
{
	CMemoryInfo* pMemInfo = theSystemInfo->GetMemoryInfo();

	unsigned long ulTotalMem = pMemInfo->GetTotalMemory(USAGE_MEMORY_VIRTUAL);
	unsigned long ulUsedMem = pMemInfo->GetUsedMemory(USAGE_MEMORY_VIRTUAL);

	wxString strFormat = wxT("Memory(%u%) : %.2f GB / %.2f GB");
	MemoryCalc(strFormat, ulTotalMem, ulUsedMem);
}

void CUsageView::PageFileUsage()
{
	CMemoryInfo* pMemInfo = theSystemInfo->GetMemoryInfo();

	unsigned long ulTotalMem = pMemInfo->GetTotalMemory(USAGE_MEMORY_PAGEFILE);
	unsigned long ulUsedMem = pMemInfo->GetUsedMemory(USAGE_MEMORY_PAGEFILE);

	wxString strFormat = wxT("Memory(%u%) : %.2f GB / %.2f GB");
	MemoryCalc(strFormat, ulTotalMem, ulUsedMem);
}

void CUsageView::MemoryCalc(const wxString& strFormat, unsigned long _ulTotal, unsigned long _ulUsed)
{
	m_UsagePercent = wx_static_cast(unsigned int, (_ulUsed  * 100) / _ulTotal);

	float fTotalGB = (float)_ulTotal / 1024.0f;
	float fUsedeGB = (float)_ulUsed / 1024.0f;

	_strUsage = wxString::Format(strFormat, m_UsagePercent, fUsedeGB, fTotalGB);
	CalcUsage();
}

void CUsageView::CalcUsage()
{
	m_UsageCount = (unsigned int)(m_UsagePercent * m_SpectrumDatas) / 100;
}

void CUsageView::SetProcessorCore(const int coreIndex)
{
	_ProcessorCoreIndex = coreIndex;
}
