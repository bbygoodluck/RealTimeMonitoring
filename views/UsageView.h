#ifndef USAGEVIEW_H_INCLUDED
#define USAGEVIEW_H_INCLUDED

#include "../interface/MemoryDC.h"
#include "../common/IUpdate.h"

class CUsageView : public wxWindow, public IUpdate
{
	typedef struct _SPECTRUM_DATA {
		wxRect _rect;
		wxColor _col;
	} SPECTRUM_DATA;

public:
	explicit CUsageView(wxWindow* parent, const int nID, const wxSize& sz, USAGE_TYPE usage_type, long lStyle = CHILD_WINDOW_STYLE);
	~CUsageView();

	void SetProcessorCore(const int coreIndex);

private:
	void Render(wxDC* pDC);
	void CreateSpectrumData(wxDC* pDC);
	void DrawSpectrum(wxDC* pDC);
	void DrawUsageSpectrum(wxDC* pDC);

	void ClearData();

	void CPUUsage();
	void MemoryPhysicsUsage();
	void MemoryVirtualUsage();
	void PageFileUsage();
	void MemoryCalc(const wxString& strFormat, unsigned long _ulTotal, unsigned long _ulUsed);
	void CalcUsage();

private:
	//화면 사이즈
    wxRect m_viewRect = wxRect(0, 0, 0, 0);
    wxRect m_viewRectSpectrum;

    //MemoryDC
	CMemoryDC m_memDC;

	//Spectrum create flag
	bool _bCreatedSpectrum = false;
	wxVector<SPECTRUM_DATA *> m_Datas;

	const int _gap = 3;
	unsigned int m_SpectrumDatas = 0;
	unsigned int m_UsageCount = 0;
	unsigned long m_UsagePercent = 0;
	wxString _strUsage = wxT("");

	const unsigned int _redBack   = 30;
	const unsigned int _greenBack = 30;
	const unsigned int _blueBack  = 30;

	enum USAGE_TYPE e_usage_type = USAGE_NONE;

	int _ProcessorCoreIndex = 0;  //CPU 전체

	std::unordered_map< int, std::function<void()> > m_mapFuncPtr;
private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnSize(wxSizeEvent& event);
	void UpdateData(wxCommandEvent& event) override;

	wxDECLARE_EVENT_TABLE();
};

#endif // USAGEVIEW_H_INCLUDED
