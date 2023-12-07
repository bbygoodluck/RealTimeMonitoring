#ifndef NETWORKPANEL_H_INCLUDED
#define NETWORKPANEL_H_INCLUDED

#include <wx/statbmp.h>
#include <wx/bmpcbox.h>

class CNetworkPanel : public wxPanel
{
public:
	CNetworkPanel(wxWindow* _parent);
	~CNetworkPanel();

private:
	wxBitmapComboBox* m_bcomAdapter;


private:
	virtual void OnComboItemSelection( wxCommandEvent& event );
};

#endif // NETWORKPANEL_H_INCLUDED
