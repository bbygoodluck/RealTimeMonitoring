#include "../../ginc.h"
#include "ProcessCPUListCtrl.h"
#include "../PanelProcessTotal.h"

CProcessCPUListCtrl::CProcessCPUListCtrl(wxWindow* _parent)
	: CCommonProcessListCtrl(_parent, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxBORDER_SIMPLE)
{
	InitColumn();
	InitList();
	
	InitializeCriticalSection(&m_criSection);
}

CProcessCPUListCtrl::~CProcessCPUListCtrl()
{
	DeleteCriticalSection(&m_criSection);
	this->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CProcessCPUListCtrl::OnItemSelected), NULL, this);
}

void CProcessCPUListCtrl::InitColumn()
{
	sListColumnItem lstColumn[] = {
		{wxT("ProcessName")     , 280, wxLIST_FORMAT_LEFT},
		{wxT("PID")             , 70 , wxLIST_FORMAT_RIGHT},
		{wxT("CPU")             , 70 , wxLIST_FORMAT_RIGHT},
		{wxT("File Description"), 440, wxLIST_FORMAT_LEFT},
	};

	for (unsigned int i = 0; i < WXSIZEOF(lstColumn); i++)
	{
		wxListItem lstItemInfo;
		lstItemInfo.SetText(lstColumn[i]._strColumnName);
		lstItemInfo.SetAlign(lstColumn[i]._align);
		lstItemInfo.SetWidth(lstColumn[i]._width);

		this->InsertColumn(i, lstItemInfo);
	}
}

void CProcessCPUListCtrl::InitList()
{
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator iter = theSystemInfo->GetProcessIterBegin();
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator iterEnd = theSystemInfo->GetProcessIterEnd();
	
	int iIndex = 0;
	while (iter != iterEnd)
	{
		PROCESS_INFO* pProcessInfo = iter->second;

		this->InsertItem(iIndex, pProcessInfo->_strProcessName);
		this->SetItem(iIndex, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		this->SetItem(iIndex, 2, wxT("0.00%"));
		this->SetItem(iIndex, 3, pProcessInfo->_strFileDescription);
		SetItemIcon(iIndex, pProcessInfo->iIconIndex);
		iIndex++;
		++iter;
	}
	
	this->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CProcessCPUListCtrl::OnItemSelected), NULL, this);
}

void CProcessCPUListCtrl::UpdateData()
{
	EnterCriticalSection(&m_criSection);
	int iLstCnt = this->GetItemCount();

	wxString strCPUUsage(wxT(""));
	unsigned long ulTotalCPUUsage = theSystemInfo->GetCPUUsage();
	
	for (int i = 0; i < iLstCnt; i++)
	{
		wxString strProcessID = this->GetItemText(i, 1);
		unsigned long ulProcessID = 0;
		
		strProcessID.ToCULong(&ulProcessID);
		
		strCPUUsage = wxT("0.0");
		float fCPUUsage = 0.0f;
		if (theSystemInfo->GetExistProcess(ulProcessID))
		{
			PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo(ulProcessID);
			fCPUUsage = pProcessInfo->fCPUUsage;
			
			strCPUUsage = wxString::Format(wxT("%.2f%"), fCPUUsage);
		}
		
		if(ulProcessID == 0)
		{
			fCPUUsage = 100.0f - (ulTotalCPUUsage * 1.0f);
			strCPUUsage = wxString::Format(wxT("%.2f%"), fCPUUsage);
		}	
		else
			SetTextBackgroundColor(i, fCPUUsage);
			
		this->SetItem(i, 2, strCPUUsage);
		this->SetItemData(i, (LPARAM)(fCPUUsage * 100000.0f));
		
		SetItemState(i, 0, wxLIST_STATE_SELECTED);
	}
		
	this->SortItems(CCommonProcessListCtrl::ListCompareFunction, 0);
	LeaveCriticalSection(&m_criSection);
}

void CProcessCPUListCtrl::OnItemSelected(wxListEvent& event)
{
	int iSel = event.GetIndex();
	wxString strProcessID = GetItemText(iSel, 1);
	
	CPanelProcessTotal* pPanel = (CPanelProcessTotal *)this->GetParent();
	pPanel->SetSelectedItem(strProcessID);
}

void CProcessCPUListCtrl::AddNewProcess(unsigned long ulProcessID)
{
	EnterCriticalSection(&m_criSection);
	
	PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo(ulProcessID);
	if(pProcessInfo)
	{
		int iLstCnt = this->GetItemCount();
		
		this->InsertItem(iLstCnt, pProcessInfo->_strProcessName);
		this->SetItem(iLstCnt, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		this->SetItem(iLstCnt, 2, wxT("0.0%"));
		this->SetItem(iLstCnt, 3, pProcessInfo->_strFileDescription);
		SetItemIcon(iLstCnt, pProcessInfo->iIconIndex);
	}
	
	LeaveCriticalSection(&m_criSection);
}

void CProcessCPUListCtrl::DelProcess(unsigned long ulProcessID)
{
	EnterCriticalSection(&m_criSection);
	DeleteProcessItem(ulProcessID);
	LeaveCriticalSection(&m_criSection);
}
	