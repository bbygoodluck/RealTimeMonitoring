#ifndef _COMMON_PROCESS_LIST_CTRL_H_INCLUDED
#define _COMMON_PROCESS_LIST_CTRL_H_INCLUDED

class CCommonProcessListCtrl : public wxListCtrl
{
	typedef struct _stCONTEXT_MENU {
		int iId = -1;
		wxString strMenuName = wxT("");
	} CONTEXT_MENU;
	
public:
	explicit CCommonProcessListCtrl(wxWindow* _parent, long style);
	virtual ~CCommonProcessListCtrl();

public:
	bool IsAscending() { return m_bAscending; }
	int GetSortingColumn() { return m_iSortingColumn; }
	void SetItemIcon(int iItemIndex, int iIconIndex);
protected:
	virtual void InitColumn() = 0;
	virtual void InitList() = 0;
	virtual void UpdateData() = 0;
	void SetTextBackgroundColor(int iIndex, float fValue, bool IsMem = false);
	void SetComma(const wxString& strSize, wxString& strRet);
	void DeleteProcessItem(unsigned long ulProcessID);
	
	static int wxCALLBACK ListCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);

	virtual void AddNewProcess(unsigned long ulProcessID) = 0;
	virtual void DelProcess(unsigned long ulProcessID) = 0;
	
private:
	void CreateContextMenu();
	
protected:
	struct sListColumnItem {
		wxString _strColumnName = wxT("");
		unsigned int _width = 0;
		wxListColumnFormat _align = wxLIST_FORMAT_LEFT;
	};
	
	wxColour colBack;
	wxString m_strSelectedPID = wxT("");
	bool m_bSelected = false;
	int m_iSelectedTime = 0;
	
	unsigned long m_ulSelProcessID = 0;
	int m_iSelIndex = -1;
	bool m_bAscending = true;
	int m_iSortingColumn = -1;
	
	wxMenu* m_pContextMenu = nullptr;
	int m_iSelItemRightClick = -1;
	
	wxBitmap m_bitmapKillProcess;
	wxBitmap m_bitmapDetail;
	
	CRITICAL_SECTION m_criSection;
private:
	void OnListContextMenu(wxCommandEvent& event);
	void OnListItemRightClick( wxListEvent& event );
	wxDECLARE_EVENT_TABLE();
};
#endif
