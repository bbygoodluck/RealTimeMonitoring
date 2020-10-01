#include "../../ginc.h"
#include "ProcessMemListCtrl.h"
#include "../PanelProcessTotal.h"

CProcessMemListCtrl::CProcessMemListCtrl(wxWindow* _parent)
	: CCommonProcessListCtrl(_parent, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxBORDER_SIMPLE)
{
	InitColumn();
	InitList();
	
	InitializeCriticalSection(&m_criSection);
	
	this->Connect(wxEVT_LIST_ITEM_SELECTED, wxListEventHandler(CProcessMemListCtrl::OnItemSelected), NULL, this);
}

CProcessMemListCtrl::~CProcessMemListCtrl()
{
	DeleteCriticalSection(&m_criSection);
	this->Disconnect(wxEVT_LIST_ITEM_SELECTED, wxListEventHandler(CProcessMemListCtrl::OnItemSelected), NULL, this);
}

void CProcessMemListCtrl::InitColumn()
{
	sListColumnItem lstColumn[] = {
		{wxT("ProcessName")       , 190, wxLIST_FORMAT_LEFT},
		{wxT("PID")               , 70 , wxLIST_FORMAT_RIGHT},
		{wxT("Private KB")        , 160, wxLIST_FORMAT_RIGHT},
		{wxT("Working Set")       , 160, wxLIST_FORMAT_RIGHT},
		{wxT("File Description")  , 280, wxLIST_FORMAT_LEFT},
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

void CProcessMemListCtrl::InitList()
{
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator iter = theSystemInfo->GetProcessIterBegin();
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator iterEnd = theSystemInfo->GetProcessIterEnd();
	
	int iIndex = 0;
	while (iter != iterEnd)
	{
		PROCESS_INFO* pProcessInfo = iter->second;

		this->InsertItem(iIndex, pProcessInfo->_strProcessName);
		this->SetItem(iIndex, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		this->SetItem(iIndex, 2, wxT("0 KB"));
		this->SetItem(iIndex, 3, wxT("0 KB"));
		this->SetItem(iIndex, 4, pProcessInfo->_strFileDescription);
		SetItemIcon(iIndex, pProcessInfo->iIconIndex);
		iIndex++;
		++iter;
	}
}

void CProcessMemListCtrl::UpdateData()
{
	EnterCriticalSection(&m_criSection);
	int iLstCnt = this->GetItemCount();

	unsigned long long ullPrivateUsage = 0;
	unsigned long long ullWorkingSetSize = 0;
	
	wxString strPrivateSize(wxT(""));
	wxString strWorkingSet(wxT(""));
	
	for (int i = 0; i < iLstCnt; i++)
	{
		wxString strProcessID = this->GetItemText(i, 1);
		unsigned long ulProcessID = 0;
		
		strProcessID.ToCULong(&ulProcessID);
		
		strPrivateSize = wxT("0 KB");
		strWorkingSet = wxT("0 KB");
		
		ullPrivateUsage = 0;
		ullWorkingSetSize = 0;
		
		if (theSystemInfo->GetExistProcess(ulProcessID))
		{
			PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo(ulProcessID);
			
			ullPrivateUsage = pProcessInfo->_PrivateSize;
			ullWorkingSetSize = pProcessInfo->_WorkingSetSize;
		
			strPrivateSize = wxT("");
			SetComma(wxString::Format(wxT("%.0f"), (float)ullPrivateUsage / (1024.0f)), strPrivateSize);
			
			strWorkingSet = wxT("");
			SetComma(wxString::Format(wxT("%.0f"), (float)ullWorkingSetSize / (1024.0f)), strWorkingSet);
		}
		
		SetTextBackgroundColor(i, (ullPrivateUsage / 1024.0f), true);
		this->SetItemData(i, (LPARAM)ullPrivateUsage);
		this->SetItem(i, 2, strPrivateSize + wxT(" KB"));
		this->SetItem(i, 3, strWorkingSet + wxT(" KB"));
		
		SetItemState(i, 0, wxLIST_STATE_SELECTED);
	}
	
	this->SortItems(CCommonProcessListCtrl::ListCompareFunction, 0);
	LeaveCriticalSection(&m_criSection);
}

void CProcessMemListCtrl::OnItemSelected(wxListEvent& event)
{
	int iSel = event.GetIndex();
	wxString strProcessID = GetItemText(iSel, 1);
	
	CPanelProcessTotal* pPanel = (CPanelProcessTotal *)this->GetParent();
	pPanel->SetSelectedItem(strProcessID);
}

void CProcessMemListCtrl::AddNewProcess(unsigned long ulProcessID)
{
	EnterCriticalSection(&m_criSection);
	
	PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo(ulProcessID);
	if(pProcessInfo)
	{
		int iLstCnt = this->GetItemCount();
		
		this->InsertItem(iLstCnt, pProcessInfo->_strProcessName);
		this->SetItem(iLstCnt, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		this->SetItem(iLstCnt, 2, wxT("0 KB"));
		this->SetItem(iLstCnt, 3, wxT("0 KB"));
		this->SetItem(iLstCnt, 4, pProcessInfo->_strFileDescription);
		SetItemIcon(iLstCnt, pProcessInfo->iIconIndex);
	}
	
	LeaveCriticalSection(&m_criSection);
}

void CProcessMemListCtrl::DelProcess(unsigned long ulProcessID)
{
	EnterCriticalSection(&m_criSection);
	DeleteProcessItem(ulProcessID);
	LeaveCriticalSection(&m_criSection);
}