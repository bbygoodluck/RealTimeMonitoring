#include "../ginc.h"
#include "PanelCPU.h"
#include "./views/CPUGraphView.h"

CPanelCPU::CPanelCPU(wxWindow* _parent)
	: wxPanel(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
	, m_Mainsizer(nullptr)
{
	Init();
}

CPanelCPU::~CPanelCPU()
{
	wxVector<CCPUGraphView *>::iterator iter = m_vecViewCPUCore.begin();
	while(iter != m_vecViewCPUCore.end())
	{
		CCPUGraphView* pView = *iter;
		delete pView;
		
		pView = nullptr;
		
		++iter;
	}
	
	m_vecViewCPUCore.clear();
}

void CPanelCPU::Init()
{
	m_Mainsizer = new wxBoxSizer(wxVERTICAL);
	
	wxBoxSizer* sizerCPUTotal;
	sizerCPUTotal = new wxBoxSizer( wxVERTICAL );
	
	m_viewCPUTotal = std::make_unique<CCPUGraphView>(this, wxDefaultSize);
	m_viewCPUTotal->SetDispText(wxT("Total"));
	
	sizerCPUTotal->Add( m_viewCPUTotal.get(), 1, wxEXPAND | wxALL, 5 );
	
	m_Mainsizer->Add( sizerCPUTotal, 1, wxEXPAND, 5 );
		
	wxBoxSizer* sizerCPUCore;
	sizerCPUCore = new wxBoxSizer( wxHORIZONTAL );
	
	int iCpuCoreCount = theSystemInfo->GetCPUCoreCount();
	for (int i = 0; i < iCpuCoreCount; i++)
	{
		wxString strDisp = wxT("Core ") + wxString::Format(wxT("%d"), i + 1);

		CCPUGraphView* pGraphView = new CCPUGraphView(this, wxDefaultSize);
		pGraphView->SetDispText(strDisp);
		
		if(i == iCpuCoreCount - 1)
			sizerCPUCore->Add(pGraphView, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
		else
			sizerCPUCore->Add(pGraphView, 1, wxEXPAND | wxLEFT, 5);
	
		m_vecViewCPUCore.push_back(pGraphView);
	}
	
	m_Mainsizer->Add( sizerCPUCore, 1, wxEXPAND, 5 );
	this->SetSizer( m_Mainsizer );
	this->Layout();
}

void CPanelCPU::UpdateData()
{
	unsigned long ulCPUUsage = theSystemInfo->GetCPUUsage();
	m_viewCPUTotal->UpdateData(ulCPUUsage);

	int iCoreCount = theSystemInfo->GetCPUCoreCount();
	for(int i = 0; i < iCoreCount; i++)
	{
		CCPUGraphView* pGraphView = m_vecViewCPUCore.at(i);
		unsigned long iCoreCpu = theSystemInfo->GetCPUCoreUsage(i);
		pGraphView->UpdateData(iCoreCpu);
	}
}