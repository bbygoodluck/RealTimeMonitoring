#include "../ginc.h"
#include "PanelSpectrum.h"

#include "./views/CPUUsageInfoView.h"
#include "./views/MemoryUsageInfoView.h"
#include "./views/PageFileUsageInfoView.h"

CPanelSpectrum::CPanelSpectrum(wxWindow* _parent)
	: wxPanel(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN)
{
	Init();
}

CPanelSpectrum::~CPanelSpectrum()
{
	
}

void CPanelSpectrum::Init()
{
	int iSpectrumWidth = 506;
	m_Mainsizer = new wxBoxSizer(wxHORIZONTAL);
	m_CPUUsageInfoView = std::make_unique<CCPUUsageInfoView>(this, wxDefaultSize);
	
	m_Mainsizer->Add( m_CPUUsageInfoView.get(), 1, wxEXPAND, 5 );
	
	m_MemoryUsageInfoView = std::make_unique<CMemoryUsageInfoView>(this, wxSize(iSpectrumWidth, -1));
	m_Mainsizer->Add( m_MemoryUsageInfoView.get(), 0, wxEXPAND | wxLEFT, 5 );
	
	m_PageFileUsageInfoView = std::make_unique<CPageFileUsageInfoView>(this, wxSize(iSpectrumWidth, -1));
	m_Mainsizer->Add( m_PageFileUsageInfoView.get(), 0, wxEXPAND | wxLEFT, 5 );
	
	SetSizer(m_Mainsizer);
	this->Layout();
}

void CPanelSpectrum::UpdateData()
{
	unsigned long ulCPUUsage = theSystemInfo->GetCPUUsage();
	
	unsigned long long ullTotalRam = 0;
	unsigned long long ullAvaRam   = 0;
	theSystemInfo->GetMemoryInfo(ullTotalRam, ullAvaRam);
	
	unsigned long long ullTotalPageFile = 0;
	unsigned long long ullAvaPageFile   = 0;
	theSystemInfo->GetPageFileInfo(ullTotalPageFile, ullAvaPageFile);
	
	m_CPUUsageInfoView->UpdateData(ulCPUUsage);
	m_MemoryUsageInfoView->UpdateData(ullAvaRam, ullTotalRam);
	m_PageFileUsageInfoView->UpdateData(ullAvaPageFile, ullTotalPageFile);
}