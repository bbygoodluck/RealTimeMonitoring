#include "ginc.h"
#include "./panels/PanelSpectrum.h"
#include "./panels/PanelCPU.h"
#include "./panels/PanelProcessTotal.h"
#include "PanelManagerImpl.h"

std::unique_ptr<CPanelManagerImpl> CPanelManagerImpl::m_pInstance = nullptr;

CPanelManagerImpl* CPanelManagerImpl::Get()
{
	if (m_pInstance.get() == nullptr)
		m_pInstance.reset(new CPanelManagerImpl());

	return m_pInstance.get();
}

CPanelManagerImpl::CPanelManagerImpl()
{
}

CPanelManagerImpl::~CPanelManagerImpl()
{
}

bool CPanelManagerImpl::CreatePanel(wxWindow* pOwner, _PANEL_TYPE _panelType)
{
	PANEL_MAP::iterator findIter = m_panelMap.find(_panelType);
	if (findIter != m_panelMap.end())
		return true;

	PANEL_MAP::value_type val(_panelType, nullptr);
	if(_panelType == PANEL_SPECTRUM)
	{
		m_spectrumPanel = new CPanelSpectrum(pOwner);
		val.second = m_spectrumPanel;
	}
	
	if(_panelType == PANEL_CPU_GRAPH)
	{
		m_cpuPanel = new CPanelCPU(pOwner);
		val.second = m_cpuPanel;
		
	}
	
	if(_panelType == PANEL_PROCESS_TOTAL)
	{
		m_processTotal = new CPanelProcessTotal(pOwner);
		val.second = m_processTotal;
	}
	
	m_panelMap.insert(val);
	return true;
}

wxPanel* CPanelManagerImpl::GetPanel(_PANEL_TYPE _panelType)
{
	std::unordered_map<_PANEL_TYPE, wxPanel *>::iterator findIter = m_panelMap.find(_panelType);
	if (findIter == m_panelMap.end())
		return nullptr;

	return findIter->second;
}

void CPanelManagerImpl::UpdateInfo()
{
	m_spectrumPanel->UpdateData();
	m_cpuPanel->UpdateData();
}