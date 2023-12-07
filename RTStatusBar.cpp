#include "./ginc.h"
#include "RTStatusBar.h"
#include "./system/system.h"
#include "./views/UsageView.h"

static const char *numlockIndicators[] = { "OFF", "NUM" };
static const char *capslockIndicators[] = { "", "CAPS" };

wxBEGIN_EVENT_TABLE(CRTStatusBar, wxStatusBar)
	EVT_TIMER(wxID_ANY, CRTStatusBar::OnTimer)
	EVT_SIZE(CRTStatusBar::OnSize)
	EVT_IDLE(CRTStatusBar::OnIdle)
wxEND_EVENT_TABLE()

CRTStatusBar::CRTStatusBar(wxWindow *parent, long style)
	: wxStatusBar(parent, wxID_ANY, style, wxT("RTStatusBar"))
	, m_timer(this)
	, m_bSet(false)
{
	Setting();

	wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Fixedsys"));
	this->SetFont(font);

	m_timer.Start(1000);
	UpdateClock();
}

CRTStatusBar::~CRTStatusBar()
{
	wxDELETE(m_bmpProcessor);
	wxDELETE(m_pUsageCPUView);
	wxDELETE(m_pUsageMemoryPhysicsView);
}

void CRTStatusBar::OnIdle(wxIdleEvent& event)
{
	SetStatusText(numlockIndicators[wxGetKeyState(WXK_NUMLOCK)],Field_NumLockIndicator);
	SetStatusText(capslockIndicators[wxGetKeyState(WXK_CAPITAL)],Field_CapsLockIndicator);

	int fields = this->GetFieldsCount();

	wxClientDC dc(this);
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

    for ( int i = 0; i < fields; i++ )
    {
		wxRect r;
		if ( this->GetFieldRect(i, r) )
		{
			if(i < Field_NumLockIndicator)
				dc.SetPen(wxColour(220, 220, 220));
			else
				dc.SetPen(wxColour(157, 120, 98));

			dc.DrawRectangle(r);
		}
    }
}

void CRTStatusBar::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	UpdateClock();
}

void CRTStatusBar::OnSize(wxSizeEvent& event)
{
	wxRect rcBmp;
	wxRect rectCPU;
	wxRect rectMemPhysics;

	bool bBmp = GetFieldRect(Field_Processor_img, rcBmp);
	bool bCPU = GetFieldRect(Field_CPUTotal, rectCPU);
	bool bMemPhysics = GetFieldRect(Field_MemPhysics, rectMemPhysics);

	if(!bBmp || !bCPU || !bMemPhysics)
	{
		event.Skip();
		return;
	}

	wxSize sizeBmp = m_bmpProcessor->GetSize();
	m_bmpProcessor->Move((rcBmp.x + 2) + (rcBmp.width - sizeBmp.x) / 2, (rcBmp.y + 1) + (rcBmp.height - sizeBmp.y) / 2);

	wxSize sizeCPU = m_pUsageCPUView->GetSize();
	m_pUsageCPUView->Move(rectCPU.x + (rectCPU.width - sizeCPU.x) / 2, rectCPU.y + (rectCPU.height - sizeCPU.y) / 2);

	wxSize sizeMemPhysics = m_pUsageMemoryPhysicsView->GetSize();
	m_pUsageMemoryPhysicsView->Move(rectMemPhysics.x + (rectMemPhysics.width - sizeMemPhysics.x) / 2, rectMemPhysics.y + (rectMemPhysics.height - sizeMemPhysics.y) / 2);

	event.Skip();
}

void CRTStatusBar::Setting()
{
	wxClientDC dc(this);

	wxSize sz = GetClientSize();
	int iClientWidth = sz.GetWidth();
	if(iClientWidth == 0)
		return;

	int iFuncSize = (iClientWidth - 200) / Field_Max;

	SetBackgroundColour(wxColour(220, 220, 220));

	wxSize sizeNumLock = dc.GetTextExtent(numlockIndicators[0]);
	sizeNumLock.IncTo(dc.GetTextExtent(numlockIndicators[1]));

	int alpha = 100;
	int widths[Field_Max];

	widths[Field_Processor_img]     = 16;
	widths[Field_Processor]         = iFuncSize - 100;
	widths[Field_CPUTotal]          = m_iControlWidth + alpha;
	widths[Field_MemPhysics]        = m_iControlWidth + alpha;
	widths[Field_NumLockIndicator]  = sizeNumLock.x - 6;
	widths[Field_CapsLockIndicator] = dc.GetTextExtent(capslockIndicators[1]).x - 2;
	widths[Field_Clock]             = 170;

	SetFieldsCount(Field_Max);
	SetStatusWidths(Field_Max, widths);

	m_bSet = true;

	wxString strProcessorName(wxT(""));
	CProcessorInfo* pProcessor = theSystemInfo->GetProcessor();
	if(pProcessor != nullptr)
	{
		strProcessorName = pProcessor->GetProcessorName();
		int iCPUCoreCount = theSystemInfo->GetCPUInfo()->GetCoreCount();

		strProcessorName += wxString::Format(wxT(" %d Processor(Logical)"), iCPUCoreCount);
	}

	SetStatusText(strProcessorName, Field_Processor);

	m_pUsageCPUView = new CUsageView(this, wxID_ANY, wxSize(m_iControlWidth + alpha, 20), USAGE_CPU, VIEW_STYLE_SIMPLE);
	m_pUsageCPUView->SetProcessorCore(0);

	theSystemInfo->GetCPUInfo()->AddEventListener(m_pUsageCPUView);

	m_pUsageMemoryPhysicsView = new CUsageView(this, wxID_ANY, wxSize(m_iControlWidth + alpha, 20), USAGE_MEMORY_PHYSIC, VIEW_STYLE_SIMPLE);
	theSystemInfo->GetMemoryInfo()->AddEventListener(m_pUsageMemoryPhysicsView);

	m_bmpProcessor = new wxStaticBitmap( this, wxID_ANY, wxArtProvider::GetBitmap( wxART_HARDDISK, wxART_MENU ), wxDefaultPosition, wxSize(16, 16), 0 );
}

void CRTStatusBar::UpdateClock()
{
	if (!m_bSet) return;

	wxDateTime tm = wxDateTime::Now();
	wxString strTimeNow = wxT(" ") + tm.FormatISODate() + wxT(" ") + tm.FormatISOTime();
	SetStatusText(strTimeNow, Field_Clock);
}
