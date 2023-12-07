#include "../../ginc.h"
#include "CommonProcessListCtrl.h"
#include "../../dialog/DlgProcessDetail.h"

#define CONTEXT_MENU_KILL_PROCESS        CONTEXT_MENU_START
#define CONTEXT_MENU_VIEW_PROCESS_DETAIL CONTEXT_MENU_START + 1

wxBEGIN_EVENT_TABLE(CCommonProcessListCtrl, wxListCtrl)
	EVT_MENU_RANGE(CONTEXT_MENU_START, CONTEXT_MENU_END, CCommonProcessListCtrl::OnListContextMenu)
wxEND_EVENT_TABLE()

CCommonProcessListCtrl::CCommonProcessListCtrl(wxWindow* _parent, long style)
	: wxListCtrl(_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style)
	, colBack(240, 240, 240)
{
	m_bitmapKillProcess = wxArtProvider::GetBitmap(wxART_QUIT, wxART_MENU, wxSize(16, 16));
	m_bitmapDetail = wxArtProvider::GetBitmap(wxART_PLUS, wxART_MENU, wxSize(16, 16));
	
	wxFont font(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Fixedsys"));
	this->SetFont(font);//wxSystemSettings::GetFont(wxSYS_SYSTEM_FIXED_FONT));
	SetBackgroundColour(colBack);
	SetTextColour(wxColour(60, 60, 60));
	
	this->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( CCommonProcessListCtrl::OnListItemRightClick ), NULL, this );
	
	CreateContextMenu();
	
#ifdef __WXMSW__	
	ListView_SetImageList(GetHWND(), _gImageList, LVSIL_SMALL);
#else
	this->SetImageList(_gImageList);
#endif
}

CCommonProcessListCtrl::~CCommonProcessListCtrl()
{
	if(m_pContextMenu)
		delete m_pContextMenu;
		
	this->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( CCommonProcessListCtrl::OnListItemRightClick ), NULL, this );
}

void CCommonProcessListCtrl::SetItemIcon(int iItemIndex, int iIconIndex)
{
	SetItemImage(iItemIndex, iIconIndex);
}

void CCommonProcessListCtrl::CreateContextMenu()
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
			
		this->Bind(wxEVT_COMMAND_MENU_SELECTED, &CCommonProcessListCtrl::OnListContextMenu, this, cMenu[i].iId);
	}
}
	
int wxCALLBACK CCommonProcessListCtrl::ListCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
	CCommonProcessListCtrl* pListCtrl = (CCommonProcessListCtrl *)sortData;
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
/**
 * @brief 이름순 정렬(오름차순) 
 * @param 리스트컨트롤 인덱스1
 * @param 리스트컨트롤 인덱스2
 * @param 리스트컨트롤 포인터
 */

void CCommonProcessListCtrl::SetTextBackgroundColor(int iIndex, float fValue, bool IsMem)
{
	this->SetItemTextColour(iIndex, wxColour(60, 60, 60));

	if(IsMem)
	{
		if(fValue >= 1000000.0f)
		{
			this->SetItemBackgroundColour(iIndex, wxColour(255, 0, 0));
			this->SetItemTextColour(iIndex, wxColour(255, 220, 220));
		//	this->SetItemFont(iIndex, wxSystemSettings::GetFont(wxSYS_SYSTEM_FIXED_FONT));
		}
		else if(fValue >= 900000.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 66, 66));
		else if(fValue >= 800000.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 74, 74));
		else if(fValue >= 700000.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 104, 104));
		else if(fValue >= 600000.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 121, 121));
		else if(fValue >= 500000.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 147, 147));
		else if(fValue >= 400000.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 166, 166));
		else if(fValue >= 300000.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 189, 189));
		else if(fValue >= 200000.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 218, 218));
		else if(fValue >= 100000.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 230, 230));
		else                         this->SetItemBackgroundColour(iIndex, colBack);	
	}
	else
	{
		if(fValue >= 50.0f)
		{
			this->SetItemBackgroundColour(iIndex, wxColour(255, 30, 30));
			this->SetItemTextColour(iIndex, wxColour(255, 220, 220));
		}
		else if(fValue >= 40.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 77, 77));
		else if(fValue >= 30.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 113, 113));
		else if(fValue >= 20.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 176, 176));
		else if(fValue >= 10.0f) this->SetItemBackgroundColour(iIndex, wxColour(255, 210, 210));
		else if(fValue >= 1.0f)  this->SetItemBackgroundColour(iIndex, wxColour(192, 255, 192));
		else                     this->SetItemBackgroundColour(iIndex, colBack);

	}
}

void CCommonProcessListCtrl::SetComma(const wxString& strSize, wxString& strRet)
{
	wxString strTmp(strSize);
	wxString strReturn = wxT("");
	int nLen = strTmp.Len();

	if (nLen > 3)
	{
		int nRestItemCount = nLen % 3;
		int nCommaCount = (nLen / 3);

		if (nRestItemCount == 0)
			nCommaCount -= 1;

		if (nCommaCount == 0)
			strReturn = strTmp;
		else
		{
			if (nRestItemCount != 0)
				strReturn = strTmp.Left(nRestItemCount) + wxT(",");

			wxString strNext = strTmp.Mid(nRestItemCount, nLen);
			nLen = strNext.Len();

			for (int nCnt = 0; nCnt < nLen; nCnt = nCnt + 3)
			{
				strReturn += strNext.Mid(nCnt, 3);
				if ((nCnt + 3) < (nLen - 1))
					strReturn += wxT(",");
			}
		}
	}
	else
		strReturn = strTmp;

	strRet = strReturn;
}

void CCommonProcessListCtrl::DeleteProcessItem(unsigned long ulProcessID)
{
	EnterCriticalSection(&_gListCtrlDelSection);
	int iLstCnt = this->GetItemCount();

	wxString strProcessID(wxT(""));
	int iDelItem = -1;
	for (int i = 0; i < iLstCnt; i++)
	{
		strProcessID = this->GetItemText(i, 1);
		unsigned long _ulProcessID = 0;
		
		strProcessID.ToCULong(&_ulProcessID);
		if(ulProcessID == _ulProcessID)
		{
			iDelItem = i;
			break;
		}
	}
	
	this->DeleteItem(iDelItem);
	
	LeaveCriticalSection(&_gListCtrlDelSection);
}

void CCommonProcessListCtrl::OnListContextMenu(wxCommandEvent& event)
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
			int iRet = theSystemInfo->KillProcess(ulProcessID);
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

void CCommonProcessListCtrl::OnListItemRightClick( wxListEvent& event )
{
	m_iSelItemRightClick = event.GetIndex();
	wxPoint pt = event.GetPoint();
	
	this->PopupMenu(m_pContextMenu, pt.x, pt.y);
}