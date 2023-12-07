#include "ginc.h"
#include "NetworkPanel.h"
#include "./views/NetworkUsageGraphView.h"
#include "./system/system.h"

CNetworkPanel::CNetworkPanel(wxWindow* _parent)
	: wxPanel(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
	wxBoxSizer* bxAdapterMain;
	bxAdapterMain = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bxAdapter;
	bxAdapter = new wxBoxSizer( wxHORIZONTAL );

	m_bcomAdapter = new wxBitmapComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(700, -1), 0, NULL, wxCB_READONLY );
	bxAdapter->Add( m_bcomAdapter, 0, 0, 5 );

	bxAdapterMain->Add( bxAdapter, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxTOP, 5 );

	CNetworkUsageGraphView* pNetView = new CNetworkUsageGraphView(this, wxID_ANY, wxSize(-1, 70));
	bxAdapterMain->Add( pNetView, 1, wxEXPAND | wxALL, 5 );

	theSystemInfo->GetNetInfo()->AddEventListener(pNetView);

	wxVector<NET_ADAPTER_INFO>* pAdapterInfo = theSystemInfo->GetNetInfo()->GetNetAdaptersInfo();
	wxVector<NET_ADAPTER_INFO>::const_iterator cIt = pAdapterInfo->begin();

	wxString strNetInfo = wxT("");
	for(cIt ; cIt != pAdapterInfo->end(); ++cIt)
	{
		NET_ADAPTER_INFO netInfo = *cIt;

		strNetInfo = wxT("Adapter - ") + netInfo._description + wxT("    IP : ") + netInfo._ip + wxT(" ") + (netInfo._dhcpEnable == 1 ? wxT(" - DHCP") : wxT(" - TCP"));
		m_bcomAdapter->Append(strNetInfo, wxArtProvider::GetBitmap( wxART_FLOPPY, wxART_MENU ));
	}

	int itemIndex = theSystemInfo->GetNetInfo()->GetAdapterIndex();
	m_bcomAdapter->SetSelection(itemIndex);

	this->SetSizer( bxAdapterMain );
	this->Layout();

	m_bcomAdapter->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( CNetworkPanel::OnComboItemSelection ), NULL, this );
}

CNetworkPanel::~CNetworkPanel()
{
	m_bcomAdapter->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( CNetworkPanel::OnComboItemSelection ), NULL, this );
}

void CNetworkPanel::OnComboItemSelection( wxCommandEvent& event )
{
	int iSel = event.GetSelection();
	theSystemInfo->GetNetInfo()->SetAdapterIndex(iSel);
}
