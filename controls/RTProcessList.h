#ifndef ILISTCTRLCOMMON_H_INCLUDED
#define ILISTCTRLCOMMON_H_INCLUDED

#include "../common/IUpdate.h"
class CRTProcessList : public wxListCtrl, public IUpdate
{
	typedef struct _stCONTEXT_MENU {
		int iId = -1;
		wxString strMenuName = wxT("");
	} CONTEXT_MENU;

	struct sListColumnItem {
		wxString _strColumnName = wxT("");
		unsigned int _width = 0;
		wxListColumnFormat _align = wxLIST_FORMAT_LEFT;
	};

	typedef struct stListFunction {
		int _id;
		std::function<void()> funcPtr;
	} FUNCTION_LIST;

	typedef struct stListFunctionNewProcess {
		int _id;
		std::function<void(unsigned long)> funcPtr;
	} NEWPROCESS_FUNCTION_LIST;

public:
	explicit CRTProcessList(wxWindow* _parent, long style, PROCESSLIST_PRIORITY_TYPE type = PROCESSLIST_PRIORITY_MAX);
	virtual ~CRTProcessList();

public:
	bool IsAscending() const     { return m_bAscending; }
	int GetSortingColumn() const { return m_iSortingColumn; }
	void SetItemIcon(int iItemIndex, int iIconIndex);

	void SetProcessListType(PROCESSLIST_PRIORITY_TYPE type)
	{
		m_enumProcessListPriority = type;
	}

protected:
	static int wxCALLBACK ListCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);
	void SetTextBackgroundColor(int iIndex, float fValue, bool IsMem = false);

protected:
	int m_iSelIndex      = -1;
	bool m_bAscending    = true;
	int m_iSortingColumn = -1;

	PROCESSLIST_PRIORITY_TYPE m_enumProcessListPriority = PROCESSLIST_PRIORITY_MAX;

private:
	wxColour colBack = wxColour(240, 240, 240);
	wxColour colFont = wxColour(60, 60, 60);

	std::unordered_map< int, std::function<void()> > m_mapFuncPtrInit;
	std::unordered_map< int, std::function<void()> > m_mapFuncPtrUpdate;
	std::unordered_map< int, std::function<void(unsigned long)> > m_mapFuncPtrNew;

	unsigned long m_ulCurrentProcessId = 0;
	bool m_bSorted = false;

	wxMenu* m_pContextMenu = nullptr;
	int m_iSelItemRightClick = -1;

	wxBitmap m_bitmapKillProcess;
	wxBitmap m_bitmapDetail;

#ifdef __WXMSW__
	std::unique_ptr<SYNC> m_pSync = nullptr;
#else
#endif

	unsigned int m_TimerCount = 0;
protected:
	void Initialize();
	void UpdateData(wxCommandEvent& event) override; //IUPDate override function
//	void Update();
	void CreateContextMenu();

private:
	void InitColumn(struct sListColumnItem* columnItems, int columns);
	void InitMain();
	void InitCPU();
	void InitMEM();

	void UpdateMain();
	void UpdateCPU();
	void UpdateMEM();

	void NewProcessMain(unsigned long ulProcessId);
	void NewProcessCPU(unsigned long ulProcessId);
	void NewProcessMEM(unsigned long ulProcessId);

private:
	void OnListContextMenu(wxCommandEvent& event);
	void OnListItemRightClick( wxListEvent& event );
	void OnCreateNewProcess(wxCommandEvent& event);
	void OnTerminateAndDelProcess(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif // ILISTCTRLCOMMON_H_INCLUDED
