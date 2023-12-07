#include "../../ginc.h"
#include "ProcessTotalListCtrl.h"

bool comp(std::pair<unsigned long, PROCESS_INFO *> a, std::pair<unsigned long, PROCESS_INFO *> b) 
{
	PROCESS_INFO* a1 = a.second;
	PROCESS_INFO* b1 = b.second;
	
	return a1->_strProcessName.CmpNoCase(b1->_strProcessName) < 0;
}

CProcessTotalListCtrl::CProcessTotalListCtrl(wxWindow* _parent)
	: CCommonProcessListCtrl(_parent, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES|wxBORDER_NONE)
{
	InitColumn();
	InitList();
	
	InitializeCriticalSection(&m_criSection);
}

CProcessTotalListCtrl::~CProcessTotalListCtrl()
{
	DeleteCriticalSection(&m_criSection);
}

void CProcessTotalListCtrl::InitColumn()
{
	sListColumnItem lstColumn[] = {
		{wxT("ProcessName")     , 250, wxLIST_FORMAT_LEFT},
		{wxT("PID")             , 70 , wxLIST_FORMAT_RIGHT},
		{wxT("UserName")        , 230, wxLIST_FORMAT_RIGHT},
		{wxT("CPU")             , 70 , wxLIST_FORMAT_RIGHT},
		{wxT("Private KBytes")  , 140, wxLIST_FORMAT_RIGHT},
		{wxT("WorkingSet")      , 140, wxLIST_FORMAT_RIGHT},
		{wxT("File Description"), 270, wxLIST_FORMAT_LEFT},
		{wxT("Company Name")    , 180, wxLIST_FORMAT_LEFT},
		{wxT("FullPath")        , 440, wxLIST_FORMAT_LEFT},
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

void CProcessTotalListCtrl::InitList()
{
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator iter = theSystemInfo->GetProcessIterBegin();
	std::unordered_map<unsigned long, PROCESS_INFO *>::const_iterator iterEnd = theSystemInfo->GetProcessIterEnd();
	//unordered_map sorting
/*	std::vector<std::pair<unsigned long, PROCESS_INFO *>> elems(iter, iterEnd);
	std::sort(elems.begin(), elems.end(), comp);

	std::vector<std::pair<unsigned long, PROCESS_INFO *>>::const_iterator vecIterS = elems.begin();
	std::vector<std::pair<unsigned long, PROCESS_INFO *>>::const_iterator vecIterE = elems.end();*/

	wxString strSize(wxT(""));
	int iIndex = 0;
	while(iter != iterEnd)
	{
		/*std::pair<unsigned long, PROCESS_INFO *> item = *vecIterS;*/
		PROCESS_INFO* pProcessInfo = iter->second;
		
		InsertItem(iIndex, pProcessInfo->_strProcessName);
		SetItem(iIndex, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		SetItem(iIndex, 2, pProcessInfo->_strDomainAndUser);
		SetItem(iIndex, 3, wxT("0.00%"));
		SetItem(iIndex, 4, wxT("0 KB"));
		SetItem(iIndex, 5, wxT("0 KB"));
		SetItem(iIndex, 6, pProcessInfo->_strFileDescription);
		SetItem(iIndex, 7, pProcessInfo->_strCompanyName);
		SetItem(iIndex, 8, pProcessInfo->_strProcessFullPath);
		SetItemIcon(iIndex, pProcessInfo->iIconIndex);
		iIndex++;
		++iter;
	}	
}

void CProcessTotalListCtrl::UpdateData()
{
	EnterCriticalSection(&m_criSection);
	unsigned long _ulForegroundProcessID = 0;
	if(!m_bSelected)
	{
#ifdef __WXMSW__
		HWND hForeWindow = ::GetForegroundWindow();
		if(hForeWindow != NULL)
		{
			TCHAR tzText[MAX_PATH] = {0, };
			::GetWindowText(hForeWindow, tzText, MAX_PATH);
			::GetWindowThreadProcessId(hForeWindow, &_ulForegroundProcessID);
		}
#else
#endif
	}
	else
		_ulForegroundProcessID = m_ulSelProcessID;
	
	int iLstCnt = this->GetItemCount();
	int iSelectedItemIndex = -1;
	int iSystemIdleIndex = -1;
	
	wxString strCPUUsage(wxT(""));
	wxString strPrivateSize(wxT(""));
	wxString strWorkingSet(wxT(""));

	for (int i = 0; i < iLstCnt; i++)
	{
		wxString strProcessID = this->GetItemText(i, 1);
		unsigned long ulProcessID = 0;
		
		strProcessID.ToCULong(&ulProcessID);
		
		if(_ulForegroundProcessID == ulProcessID)
			iSelectedItemIndex = i;
		
		if(ulProcessID == 0)
			iSystemIdleIndex = i;
			
		strCPUUsage = wxT("0.00");
		
		strPrivateSize = wxT("0 KB");
		strWorkingSet = wxT("0 KB");
		
		float fCPUUsage = 0.0f;
		if (theSystemInfo->GetExistProcess(ulProcessID))
		{
			PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo(ulProcessID);
			fCPUUsage = pProcessInfo->fCPUUsage;
			
			strCPUUsage = wxString::Format(wxT("%.2f"), fCPUUsage);
			strPrivateSize = wxT("");
			SetComma(wxString::Format(wxT("%.0f"), (float)pProcessInfo->_PrivateSize / (1024.0f)), strPrivateSize);
			
			strWorkingSet = wxT("");
			SetComma(wxString::Format(wxT("%.0f"), (float)pProcessInfo->_WorkingSetSize / (1024.0f)), strWorkingSet);
		}
		
		SetTextBackgroundColor(i, 0);
		
		this->SetItem(i, 3, strCPUUsage + wxT("%"));
		this->SetItem(i, 4, strPrivateSize + wxT(" KB"));
		this->SetItem(i, 5, strWorkingSet + wxT(" KB"));
		
		if(!m_bSorted)
			this->SetItemData(i, i);
	}
	
	this->m_bAscending = true;
	this->m_iSortingColumn = 0;
	
	if(m_bSorted)
	{
		wxColour col;
		if(m_bSelected && m_iSelectedTime < 4)
		{
		//	SelectedItem(iSelectedItemIndex);
			col = wxColour(0, 255, 255);
			m_iSelectedTime++;
		}
		else
		{
			col = wxColour(255, 255, 0);
			m_bSelected = false;
			m_ulSelProcessID = 0;
			if(m_processForegroundWndID != _ulForegroundProcessID)
			{
			//	SelectedItem(iSelectedItemIndex);
				m_processForegroundWndID = _ulForegroundProcessID;
			}
		}
		
		if(iSelectedItemIndex != -1)
			SetItemBackgroundColour(iSelectedItemIndex, col);
		
		unsigned long ulTotalCPUUsage = theSystemInfo->GetCPUUsage();
		strCPUUsage = wxString::Format(wxT("%.1f"), (float)(100 - ulTotalCPUUsage));
		
		this->SetItem(iSystemIdleIndex, 3, strCPUUsage + wxT("%"));
	}
	
	if(!m_bSorted)
	{
		this->SortItems(CCommonProcessListCtrl::ListCompareFunction, (LPARAM)this);
		m_bSorted = true;
	}
	
	LeaveCriticalSection(&m_criSection);
}

void CProcessTotalListCtrl::SelectedItem(int iSelIndex)
{
	SetItemState(iSelIndex, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
//	long sel = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
//	if ( sel != -1 )
//		SetItemState(sel, 0, wxLIST_STATE_SELECTED);

	SetItemState(iSelIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	EnsureVisible(iSelIndex);
	
	SetItemState(iSelIndex, 0, wxLIST_STATE_SELECTED);
}

void CProcessTotalListCtrl::SetSelectedItem(const wxString& strProcessID)
{
	m_iSelectedTime = 0;
	m_bSelected = true;
	strProcessID.ToCULong(&m_ulSelProcessID);
}

void CProcessTotalListCtrl::AddNewProcess(unsigned long ulProcessID)
{
	EnterCriticalSection(&m_criSection);
	
	PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo(ulProcessID);
	if(pProcessInfo)
	{
		int iLstCnt = this->GetItemCount();
		
		this->InsertItem(iLstCnt, pProcessInfo->_strProcessName);
		this->SetItem(iLstCnt, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		this->SetItem(iLstCnt, 2, pProcessInfo->_strDomainAndUser);
		this->SetItem(iLstCnt, 3, wxT("0.0%"));
		this->SetItem(iLstCnt, 4, wxT("0 KB"));
		this->SetItem(iLstCnt, 5, wxT("0 KB"));
		this->SetItem(iLstCnt, 6, pProcessInfo->_strFileDescription);
		this->SetItem(iLstCnt, 7, pProcessInfo->_strCompanyName);
		this->SetItem(iLstCnt, 8, pProcessInfo->_strProcessFullPath);
		SetItemIcon(iLstCnt, pProcessInfo->iIconIndex);
		
		m_bSorted = false;
	}
	
	LeaveCriticalSection(&m_criSection);
}

void CProcessTotalListCtrl::DelProcess(unsigned long ulProcessID)
{
	EnterCriticalSection(&m_criSection);
	DeleteProcessItem(ulProcessID);
	LeaveCriticalSection(&m_criSection);
}