#ifndef CPUPANEL_H_INCLUDED
#define CPUPANEL_H_INCLUDED

class CCPUPanel : public wxPanel
{
public:
	CCPUPanel(wxWindow* _parent);
	~CCPUPanel();

private:
	void CreateCPUCoreView(wxBoxSizer* pSizer);
};

#endif // CPUPANEL_H_INCLUDED
