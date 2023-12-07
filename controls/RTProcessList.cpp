#include "../ginc.h"
#include "RTProcessList.h"
#include "../system/system.h"
#include "../dialog/DlgProcessDetail.h"

#define CONTEXT_MENU_KILL_PROCESS        CONTEXT_MENU_START
#define CONTEXT_MENU_VIEW_PROCESS_DETAIL CONTEXT_MENU_START + 1

wxBEGIN_EVENT_TABLE(CRTProcessList, wxListCtrl)
	EVT_MY_CUSTOM_COMMAND(wxEVT_CREATE_NEW_PROCESS, wxID_ANY, CRTProcessList::OnCreateNewProcess)
	EVT_MY_CUSTOM_COMMAND(wxEVT_TERMINATE_AND_DEL_PROCESS, wxID_ANY, CRTProcessList::OnTerminateAndDelProcess)
	EVT_MY_CUSTOM_COMMAND(wxEVT_RESOURCE_MONITORING, wxID_ANY, CRTProcessList::UpdateData)
	EVT_MENU_RANGE(CONTEXT_MENU_START, CONTEXT_MENU_END, CRTProcessList::OnListContextMenu)
wxEND_EVENT_TABLE()


CRTProcessList::CRTProcessList(wxWindow* _parent, long style, PROCESSLIST_PRIORITY_TYPE type)
	: wxListCtrl(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style)
{
	wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Fixedsys"));
	this->SetFont(font);
	SetBackgroundColour(colBack);

	m_bitmapKillProcess = wxArtProvider::GetBitmap(wxART_QUIT, wxART_MENU, wxSize(16, 16));
	m_bitmapDetail = wxArtProvider::GetBitmap(wxART_PLUS, wxART_MENU, wxSize(16, 16));

#ifdef __WXMSW__
	ListView_SetImageList(GetHWND(), _gImageList, LVSIL_SMALL);
#else
	this->SetImageList(_gImageList);
#endif

	m_enumProcessListPriority = type;

	{
		FUNCTION_LIST stFunctionInit[] = {
			{ PROCESSLIST_PRIORITY_ALL, std::bind(&CRTProcessList::InitMain, this) },
			{ PROCESSLIST_PRIORITY_CPU, std::bind(&CRTProcessList::InitCPU , this) },
			{ PROCESSLIST_PRIORITY_MEM, std::bind(&CRTProcessList::InitMEM , this) },
		};

		for(unsigned int i = 0; i < WXSIZEOF(stFunctionInit); i++)
			m_mapFuncPtrInit.insert(std::make_pair(stFunctionInit[i]._id, stFunctionInit[i].funcPtr));
	}

	{
		FUNCTION_LIST stFunctionUpdate[] = {
			{ PROCESSLIST_PRIORITY_ALL, std::bind(&CRTProcessList::UpdateMain, this) },
			{ PROCESSLIST_PRIORITY_CPU, std::bind(&CRTProcessList::UpdateCPU , this) },
			{ PROCESSLIST_PRIORITY_MEM, std::bind(&CRTProcessList::UpdateMEM , this) },
		};

		//프로세스 갱신에 필요한 함수
		for(unsigned int i = 0; i < WXSIZEOF(stFunctionUpdate); i++)
			m_mapFuncPtrUpdate.insert(std::make_pair(stFunctionUpdate[i]._id, stFunctionUpdate[i].funcPtr));
	}

	{
		NEWPROCESS_FUNCTION_LIST stNewProcess[] = {
			{ PROCESSLIST_PRIORITY_ALL, std::bind(&CRTProcessList::NewProcessMain, this, std::placeholders::_1) },
			{ PROCESSLIST_PRIORITY_CPU, std::bind(&CRTProcessList::NewProcessCPU , this, std::placeholders::_1) },
			{ PROCESSLIST_PRIORITY_MEM, std::bind(&CRTProcessList::NewProcessMEM , this, std::placeholders::_1) },
		};

		//프로세스 갱신에 필요한 함수
		for(unsigned int i = 0; i < WXSIZEOF(stNewProcess); i++)
			m_mapFuncPtrNew.insert(std::make_pair(stNewProcess[i]._id, stNewProcess[i].funcPtr));
	}

	Initialize();
	this->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( CRTProcessList::OnListItemRightClick ), NULL, this );
	CreateContextMenu();

#ifdef __WXMSW__
	m_pSync = std::make_unique<SYNC>();
#else

#endif
}

CRTProcessList::~CRTProcessList()
{
	m_mapFuncPtrInit.clear();
	m_mapFuncPtrUpdate.clear();
	m_mapFuncPtrNew.clear();

	if(m_pContextMenu)
		delete m_pContextMenu;

	this->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( CRTProcessList::OnListItemRightClick ), NULL, this );
}

void CRTProcessList::SetItemIcon(int iItemIndex, int iIconIndex)
{
	SetItemImage(iItemIndex, iIconIndex);
}

int wxCALLBACK CRTProcessList::ListCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
	CRTProcessList* pListCtrl = (CRTProcessList *)sortData;
	if(pListCtrl == nullptr)
	{
		if (item1 < item2)
			return 1;

		if (item1 > item2)
			return -1;

		return 0;
	}

	int iSortingColumn = pListCtrl->GetSortingColumn();
	bool bAscending = pListCtrl->IsAscending();

	wxString str1 = pListCtrl->GetItemText(item1, iSortingColumn);
	wxString str2 = pListCtrl->GetItemText(item2, iSortingColumn);

	int iRet = 0;
	if(str1.CmpNoCase(str2) > 0)
	{
		if(bAscending)
			iRet = 1;
		else
			iRet = -1;
	}

	if(str1.CmpNoCase(str2) < 0)
	{
		if(bAscending)
			iRet = -1;
		else
			iRet = 1;
	}

	return iRet;
}

void CRTProcessList::Initialize()
{
	m_ulCurrentProcessId = GetCurrentProcessId();

	std::unordered_map< int, std::function<void()> >::const_iterator fIter = m_mapFuncPtrInit.find(m_enumProcessListPriority);
	if(fIter == m_mapFuncPtrInit.end())
		return;

	auto functionPtr = fIter->second;
	(functionPtr)();
}

void CRTProcessList::InitMain()
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

	InitColumn(lstColumn, WXSIZEOF(lstColumn));

	auto itStart = theSystemInfo->GetProcessInfo()->Begin();
	auto itEnd = theSystemInfo->GetProcessInfo()->End();

	wxString strSize(wxT(""));
	int iIndex = 0;
	while(itStart != itEnd)
	{
		PROCESS_INFO* pProcessInfo = itStart->second;

		InsertItem(iIndex, pProcessInfo->_strProcessName);
		SetItem(iIndex, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		SetItem(iIndex, 2, pProcessInfo->_strDomainAndUser);
		SetItem(iIndex, 3, wxT("0.00%"));
		SetItem(iIndex, 4, wxT("0 KB"));
		SetItem(iIndex, 5, wxT("0 KB"));
		SetItem(iIndex, 6, pProcessInfo->_strFileDescription);
		SetItem(iIndex, 7, pProcessInfo->_strCompanyName);
		SetItem(iIndex, 8, pProcessInfo->_strProcessFullPath);

		this->SetItemImage(iIndex, pProcessInfo->iIconIndex);
		this->SetItemData(iIndex, iIndex);

		if(m_ulCurrentProcessId == pProcessInfo->_ulProcessID)
			this->SetItemBackgroundColour(iIndex, wxColour(255, 255, 0));

		iIndex++;
		++itStart;
	}

	this->SortItems(CRTProcessList::ListCompareFunction, (LPARAM)this);
	m_bSorted = true;
}

void CRTProcessList::InitCPU()
{
	sListColumnItem lstColumn[] = {
		{wxT("ProcessName")     , 340, wxLIST_FORMAT_LEFT},
		{wxT("PID")             , 100 , wxLIST_FORMAT_RIGHT},
		{wxT("CPU")             , 100 , wxLIST_FORMAT_RIGHT},
	};

	InitColumn(lstColumn, WXSIZEOF(lstColumn));

	auto itStart = theSystemInfo->GetProcessInfo()->Begin();
	auto itEnd = theSystemInfo->GetProcessInfo()->End();

	wxString strSize(wxT(""));
	int iIndex = 0;
	while(itStart != itEnd)
	{
		PROCESS_INFO* pProcessInfo = itStart->second;

		InsertItem(iIndex, pProcessInfo->_strProcessName);
		SetItem(iIndex, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		SetItem(iIndex, 2, wxT("0.00%"));

		this->SetItemImage(iIndex, pProcessInfo->iIconIndex);

		if(m_ulCurrentProcessId == pProcessInfo->_ulProcessID)
			this->SetItemBackgroundColour(iIndex, wxColour(255, 255, 0));

		iIndex++;
		++itStart;
	}
}

void CRTProcessList::InitMEM()
{
	sListColumnItem lstColumn[] = {
		{wxT("ProcessName")     , 220, wxLIST_FORMAT_LEFT},
		{wxT("PID")             , 60 , wxLIST_FORMAT_RIGHT},
		{wxT("Private KBytes")  , 140, wxLIST_FORMAT_RIGHT},
		{wxT("WorkingSet")      , 140, wxLIST_FORMAT_RIGHT},
	};

	InitColumn(lstColumn, WXSIZEOF(lstColumn));

	auto itStart = theSystemInfo->GetProcessInfo()->Begin();
	auto itEnd = theSystemInfo->GetProcessInfo()->End();

	wxString strSize(wxT(""));
	int iIndex = 0;
	while(itStart != itEnd)
	{
		PROCESS_INFO* pProcessInfo = itStart->second;

		InsertItem(iIndex, pProcessInfo->_strProcessName);
		SetItem(iIndex, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		SetItem(iIndex, 2, wxT("0 KB"));
		SetItem(iIndex, 3, wxT("0 KB"));

		this->SetItemImage(iIndex, pProcessInfo->iIconIndex);
		this->SetItemData(iIndex, iIndex);

		if(m_ulCurrentProcessId == pProcessInfo->_ulProcessID)
			this->SetItemBackgroundColour(iIndex, wxColour(255, 255, 0));

		iIndex++;
		++itStart;
	}
}

void CRTProcessList::InitColumn(struct sListColumnItem* columnItems, int columns)
{
	for (int i = 0; i < columns; i++)
	{
		wxListItem lstItemInfo;
		lstItemInfo.SetText((columnItems + i)->_strColumnName);
		lstItemInfo.SetAlign((columnItems + i)->_align);
		lstItemInfo.SetWidth((columnItems + i)->_width);

		this->InsertColumn(i, lstItemInfo);
	}
}
/*
void CRTProcessList::Update()
{
	std::unordered_map< int, std::function<void()> >::const_iterator fIter = m_mapFuncPtrUpdate.find(m_enumProcessListPriority);
	if(fIter == m_mapFuncPtrUpdate.end())
		return;

	auto functionPtr = fIter->second;
	(functionPtr)();
}
*/
void CRTProcessList::UpdateMain()
{
	wxString strCPUUsage(wxT(""));
	wxString strPrivateSize(wxT(""));
	wxString strWorkingSet(wxT(""));

	float fCPUUsage = 0.0f;
	int nProcessCounts = this->GetItemCount();
	for (int index = 0; index < nProcessCounts; index++)
	{
		wxString strProcessID = this->GetItemText(index, 1);
		unsigned long ulProcessID = 0;

		strProcessID.ToCULong(&ulProcessID);

		PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo()->GetInfo(ulProcessID);
		if(pProcessInfo == nullptr)
			continue;

		fCPUUsage = 0.0f;
		fCPUUsage = pProcessInfo->fCPUUsage;
		strCPUUsage = wxString::Format(wxT("%.2f%"), fCPUUsage);

		strPrivateSize = theUtility->SetComma(pProcessInfo->_PrivateSize / 1024);//theUtility->SetComma(wxString::Format(wxT("%.0f"), (float)pProcessInfo->_PrivateSize / (1024.0f)));
		strWorkingSet = theUtility->SetComma(pProcessInfo->_WorkingSetSize / 1024);//theUtility->SetComma(wxString::Format(wxT("%.0f"), (float)pProcessInfo->_WorkingSetSize / (1024.0f)));

		this->SetItem(index, 3, strCPUUsage);
		this->SetItem(index, 4, strPrivateSize + wxT(" KB"));
		this->SetItem(index, 5, strWorkingSet + wxT(" KB"));

		if(!m_bSorted)
			this->SetItemData(index, index);

//		if(m_ulCurrentProcessId != pProcessInfo->_ulProcessID)
//			UpdateBackgroundColour(index, fCPUUsage);
	}

	if(!m_bSorted)
	{
		this->SortItems(CRTProcessList::ListCompareFunction, (LPARAM)this);
		m_bSorted = true;
	}
}

void CRTProcessList::UpdateCPU()
{
	wxString strCPUUsage(wxT(""));
	int nProcessCounts = this->GetItemCount();

	float fUsedCPUTotal = 0.0f;
	unsigned long ulProcessIDZeroIndex = -1;
	for (int index = 0; index < nProcessCounts; index++)
	{
		strCPUUsage = wxT("");
		wxString strProcessID = this->GetItemText(index, 1);
		unsigned long ulProcessID = 0;

		strProcessID.ToCULong(&ulProcessID);

		PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo()->GetInfo(ulProcessID);
		if(pProcessInfo == nullptr)
			continue;

		float fCPUUsage = 0.0f;
		if (theSystemInfo->GetProcessInfo()->GetExistProcess(ulProcessID))
		{
			fCPUUsage = pProcessInfo->fCPUUsage;
			strCPUUsage = wxString::Format(wxT("%.2f%"), fCPUUsage);

			if(ulProcessID == 0)
				ulProcessIDZeroIndex = index;
			else
				SetTextBackgroundColor(index, fCPUUsage);

			//CPU사용량이 4.5% 이상인경우 DB저장
			if(ulProcessID != 0 && ulProcessID != 4 && fCPUUsage >= 4.5f)
			{
				if(m_TimerCount == 7200)
				{	//2시간에 한번씩 프로세스리스트를 모두 지운다.
					theSQLite->AllInitProcessList();
					m_TimerCount = 0;
				}

				theSQLite->InsertProcessInfo(pProcessInfo, strCPUUsage);
			}
		}

		fUsedCPUTotal += fCPUUsage;
		this->SetItem(index, 2, strCPUUsage);
		this->SetItemData(index, (LPARAM)(fCPUUsage * 100000.0f));
		SetItemState(index, 0, wxLIST_STATE_SELECTED);
	}

	float fCPUIdle = 100.0f - fUsedCPUTotal;
	if(fCPUIdle < 0.0f)
		fCPUIdle = 0.0f;

	strCPUUsage = wxString::Format(wxT("%.2f%"), fCPUIdle);

	this->SetItem(ulProcessIDZeroIndex, 2, strCPUUsage);
	this->SetItemData(ulProcessIDZeroIndex, (LPARAM)(fCPUIdle * 100000.0f));

	this->SortItems(CRTProcessList::ListCompareFunction, 0);

	m_TimerCount++;
}

void CRTProcessList::UpdateMEM()
{
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

		strPrivateSize = wxT("0");
		strWorkingSet = wxT("0");

		ullPrivateUsage = 0;
		ullWorkingSetSize = 0;

		if (theSystemInfo->GetProcessInfo()->GetExistProcess(ulProcessID))
		{
			PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo()->GetInfo(ulProcessID);

			ullPrivateUsage = pProcessInfo->_PrivateSize;
			ullWorkingSetSize = pProcessInfo->_WorkingSetSize;

			strPrivateSize = theUtility->SetComma(ullPrivateUsage / 1024);
			strWorkingSet = theUtility->SetComma(ullWorkingSetSize / 1024);
		}

		SetTextBackgroundColor(i, (ullPrivateUsage / 1024), true);
		this->SetItemData(i, (LPARAM)(ullPrivateUsage / 1024));
		this->SetItem(i, 2, strPrivateSize + wxT(" KB"));
		this->SetItem(i, 3, strWorkingSet + wxT(" KB"));

		SetItemState(i, 0, wxLIST_STATE_SELECTED);
	}

	this->SortItems(CRTProcessList::ListCompareFunction, 0);
}

void CRTProcessList::UpdateData(wxCommandEvent& event)
{
	m_pSync->Lock();
	std::unordered_map< int, std::function<void()> >::const_iterator fIter = m_mapFuncPtrUpdate.find(m_enumProcessListPriority);
	if(fIter == m_mapFuncPtrUpdate.end())
		return;

	auto functionPtr = fIter->second;
	(functionPtr)();

	event.Skip();
	m_pSync->UnLock();
}

void CRTProcessList::NewProcessMain(unsigned long ulProcessId)
{
	PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo()->GetInfo(ulProcessId);
	if(pProcessInfo)
	{
		int iLstCnt = this->GetItemCount();

		this->InsertItem(iLstCnt, pProcessInfo->_strProcessName);
		this->SetItem(iLstCnt, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		this->SetItem(iLstCnt, 2, pProcessInfo->_strDomainAndUser);
		this->SetItem(iLstCnt, 3, wxT("0.00%"));
		this->SetItem(iLstCnt, 4, wxT("0 KB"));
		this->SetItem(iLstCnt, 5, wxT("0 KB"));
		this->SetItem(iLstCnt, 6, pProcessInfo->_strFileDescription);
		this->SetItem(iLstCnt, 7, pProcessInfo->_strCompanyName);
		this->SetItem(iLstCnt, 8, pProcessInfo->_strProcessFullPath);

		this->SetItemImage(iLstCnt, pProcessInfo->iIconIndex);

		m_bSorted = false;
	}
}

void CRTProcessList::NewProcessCPU(unsigned long ulProcessId)
{
	PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo()->GetInfo(ulProcessId);
	if(pProcessInfo)
	{
		int iLstCnt = this->GetItemCount();

		this->InsertItem(iLstCnt, pProcessInfo->_strProcessName);
		this->SetItem(iLstCnt, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		this->SetItem(iLstCnt, 2, wxT("0.00%"));

		this->SetItemImage(iLstCnt, pProcessInfo->iIconIndex);

		m_bSorted = false;
	}
}

void CRTProcessList::NewProcessMEM(unsigned long ulProcessId)
{
	PROCESS_INFO* pProcessInfo = theSystemInfo->GetProcessInfo()->GetInfo(ulProcessId);
	if(pProcessInfo)
	{
		int iLstCnt = this->GetItemCount();

		this->InsertItem(iLstCnt, pProcessInfo->_strProcessName);
		this->SetItem(iLstCnt, 1, wxString::Format(wxT("%u"), pProcessInfo->_ulProcessID));
		this->SetItem(iLstCnt, 2, wxT("0 KB"));
		this->SetItem(iLstCnt, 3, wxT("0 KB"));

		this->SetItemImage(iLstCnt, pProcessInfo->iIconIndex);

		m_bSorted = false;
	}
}

void CRTProcessList::OnCreateNewProcess(wxCommandEvent& event)
{
	m_pSync->Lock();
	std::unordered_map< int, std::function<void(unsigned long)> >::const_iterator fIter = m_mapFuncPtrNew.find(m_enumProcessListPriority);
	if(fIter == m_mapFuncPtrNew.end())
		return;

	unsigned long ulProcessId = event.GetExtraLong();

	auto functionPtr = fIter->second;
	(functionPtr)(ulProcessId);

	event.Skip();
	m_pSync->UnLock();
}

void CRTProcessList::OnTerminateAndDelProcess(wxCommandEvent& event)
{
	m_pSync->Lock();
	unsigned long ulProcessId = event.GetExtraLong();
	wxString strDelPID = wxString::Format(wxT("%u"), ulProcessId);
	int iLstCnt = this->GetItemCount();

	wxString strProcessID(wxT(""));
	int iDelItem = -1;
	for (int i = 0; i < iLstCnt; i++)
	{
		strProcessID = this->GetItemText(i, 1);
		if(strDelPID.CmpNoCase(strProcessID) == 0)
		{
			iDelItem = i;
			break;
		}
	}

	this->DeleteItem(iDelItem);
	m_pSync->UnLock();
}

void CRTProcessList::SetTextBackgroundColor(int iIndex, float fValue, bool IsMem)
{
	int iCPUMaxCheckVal = theSetting->GetCPUMaxCheckVal();
	int iMEMMaxCheckVal = theSetting->GetMEMMaxCheckVal();

	this->SetItemTextColour(iIndex, wxColour(60, 60, 60));
	unsigned long ulMemTotal = theSystemInfo->GetMemoryInfo()->GetTotalMemory(USAGE_MEMORY_PHYSIC);

	unsigned long ulPercent = 0;
	int r = 0;
	if(IsMem)
	{
		unsigned long ulUsedMem = (unsigned long)(fValue / 1024.0f); // GB로 변경
		ulPercent = ulUsedMem * 100 / ulMemTotal;

		//메모리 사용량이 20% 이상이면 빨간색으로 표시
		r = ulPercent >= iMEMMaxCheckVal ? 255 : (ulPercent * 255 / 100);
		this->SetItemTextColour(iIndex, wxColour(r, 0, 0));
	}
	else
	{
		unsigned long ulUsedCPU = (unsigned long)fValue;
		//CPU사용량이 30% 이상이면 빨간색으로 표시
		r = ulUsedCPU >= iCPUMaxCheckVal ? 255 : (ulUsedCPU * 255 / 100);
		this->SetItemTextColour(iIndex, wxColour(r, 0, 0));
	}
}

void CRTProcessList::OnListItemRightClick( wxListEvent& event )
{
	if(m_pContextMenu == nullptr)
		return;

	m_iSelItemRightClick = event.GetIndex();
	wxPoint pt = event.GetPoint();

	this->PopupMenu(m_pContextMenu, pt.x, pt.y);
}

void CRTProcessList::CreateContextMenu()
{
	m_pContextMenu = new wxMenu();

	CONTEXT_MENU cMenu[] = {
		{ CONTEXT_MENU_KILL_PROCESS,        wxT("kill process") },
		{ CONTEXT_MENU_VIEW_PROCESS_DETAIL, wxT("View Detail")  },
	};

	int iMenuCount = WXSIZEOF(cMenu);
	for(int i = 0; i < iMenuCount; i++)
	{
		wxMenuItem* pItem = m_pContextMenu->Append(cMenu[i].iId, cMenu[i].strMenuName);

		if(cMenu[i].iId == CONTEXT_MENU_KILL_PROCESS)
			pItem->SetBitmap(m_bitmapKillProcess);

		if(cMenu[i].iId == CONTEXT_MENU_VIEW_PROCESS_DETAIL)
			pItem->SetBitmap(m_bitmapDetail);

		this->Bind(wxEVT_COMMAND_MENU_SELECTED, &CRTProcessList::OnListContextMenu, this, cMenu[i].iId);
	}
}

void CRTProcessList::OnListContextMenu(wxCommandEvent& event)
{
	int iColumnCount = GetColumnCount();

	int iID = event.GetId();

	wxString strProcessID = this->GetItemText(m_iSelItemRightClick, 1);
	unsigned long ulProcessID = 0;
	strProcessID.ToULong(&ulProcessID);

	if(iID == CONTEXT_MENU_KILL_PROCESS)
	{
		wxString strMsgProcessKill(wxT(""));

		strMsgProcessKill = wxT("프로세스명 : ");
		strMsgProcessKill += this->GetItemText(m_iSelItemRightClick, 0);  //프로세스명
		strMsgProcessKill += wxT("\n");
		strMsgProcessKill += wxT("ProcessID : ");

		strMsgProcessKill += strProcessID;  //PID
		strMsgProcessKill += wxT("\n");

		if(iColumnCount == 8)
		{
			strMsgProcessKill += wxT("File Description : ");
			strMsgProcessKill += this->GetItemText(m_iSelItemRightClick, 5);  //설명
			strMsgProcessKill += wxT("\n");
			strMsgProcessKill += wxT("Company Name : ");
			strMsgProcessKill += this->GetItemText(m_iSelItemRightClick, 6);  //회사명
			strMsgProcessKill += wxT("\n");
			strMsgProcessKill += wxT("FullPath : ");
			strMsgProcessKill += this->GetItemText(m_iSelItemRightClick, 7);  //패스
			strMsgProcessKill += wxT("\n");
		}
		else
		{
			int iIndex = -1;
			if(iColumnCount == 5)
				iIndex = 4;
			else
				iIndex = 3;

			strMsgProcessKill += wxT("File Description : ");
			strMsgProcessKill += this->GetItemText(m_iSelItemRightClick, iIndex);  //설명
			strMsgProcessKill += wxT("\n");
		}

		strMsgProcessKill += wxT("\n위 프로세스를 정말로 삭제하시겠습니까?");
		int iRetID = wxMessageBox(strMsgProcessKill, PROGRAM_FULL_NAME, wxICON_QUESTION | wxYES_NO);
		if(iRetID == wxYES)
		{
			int iRet = theSystemInfo->GetProcessInfo()->KillProcess(ulProcessID);
			if(iRet != KILL_PROCESS_MSG_SUCCESS)
			{
				wxString strErrMsg(wxT(""));
				switch(iRet)
				{
					case KILL_PROCESS_MSG_NOT_PROCESSID:
						strErrMsg = wxT("프로세스 ID가 존재하지 않습니다.");
						break;

					case KILL_PROCESS_MSG_PROCESS_NULL:
						strErrMsg = wxT("프로세스가 존재 않습니다.");
						break;

					case KILL_PROCESS_MSG_PROCESS_TERMINATE_FAIL:
						strErrMsg = wxT("프로세스 종료에 실패하였습니다.");
						break;

					case KILL_PROCESS_MSG_PROCESS_ALIVE:
						strErrMsg = wxT("프로세스가 아직 살아 있습니다.");
						break;

					default:
						break;
				}

				wxMessageBox(strErrMsg, PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
				return;
			}
		}
	}

	if(iID == CONTEXT_MENU_VIEW_PROCESS_DETAIL)
	{
		DlgProcessDetail dlg(_gMainWindow);
		dlg.SetProcessID(ulProcessID);

		dlg.ShowModal();
		dlg.Destroy();
	}
}
