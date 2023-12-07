#include "ginc.h"
#include "CPUPanel.h"
#include "./views/GraphView.h"
#include "./system/system.h"

CCPUPanel::CCPUPanel(wxWindow* _parent)
	: wxPanel(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
	wxBoxSizer* bxCPUMain = new wxBoxSizer( wxVERTICAL );
	CreateCPUCoreView(bxCPUMain);

	this->SetSizer( bxCPUMain );
	this->Layout();
}

CCPUPanel::~CCPUPanel()
{

}

void CCPUPanel::CreateCPUCoreView(wxBoxSizer* pSizer)
{
	unsigned int nCoreCount = theSystemInfo->GetCPUInfo()->GetCoreCount();
	unsigned int nMaxDispCore = 8;

	unsigned int nDisp = nCoreCount / nMaxDispCore;
	unsigned int nCPUCoreRow = nDisp == 0 ? 1 : nDisp;

	unsigned int corenum = 0;
	//한번에 표시할 수 있는 Core수는 8개
	if(nCoreCount > nMaxDispCore)
		nCoreCount = nMaxDispCore;

	for(unsigned int index = 0; index < nCPUCoreRow; index++)
	{
		wxBoxSizer* coreSizer = new wxBoxSizer( wxHORIZONTAL );
		for(unsigned int coreindex = 0; coreindex < nCoreCount; coreindex++ )
		{
			CGraphView* pGraphView = new CGraphView(this, wxID_ANY, wxSize(-1, 50));
			pGraphView->SetCoreIndex(corenum++);
			coreSizer->Add( pGraphView, 1, wxEXPAND | wxALL, 5 );

			theSystemInfo->GetCPUInfo()->AddEventListener(pGraphView);
		}

		pSizer->Add( coreSizer, 1, wxEXPAND, 5 );
	}
}
