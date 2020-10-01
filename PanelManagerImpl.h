#ifndef CPANELMANAGERIMPL_H
#define CPANELMANAGERIMPL_H

class CPanelSpectrum;
class CPanelCPU;
class CPanelProcessTotal;

class CPanelManagerImpl
{
	typedef std::unordered_map<_PANEL_TYPE, wxPanel *> PANEL_MAP;
	
private:
	explicit CPanelManagerImpl();

public:
	~CPanelManagerImpl();
	static CPanelManagerImpl* Get();
	
public:
	bool CreatePanel(wxWindow* pOwner, _PANEL_TYPE _panelType);
	wxPanel* GetPanel(_PANEL_TYPE _panelType);
	
public:
	void UpdateInfo();
	
private:
	static std::unique_ptr<CPanelManagerImpl> m_pInstance;

	PANEL_MAP m_panelMap;
	CPanelSpectrum* m_spectrumPanel;
	CPanelCPU* m_cpuPanel;
	CPanelProcessTotal* m_processTotal;
};

#endif // CPANELMANAGERIMPL_H
