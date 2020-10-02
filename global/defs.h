#ifndef __DEFINED_H__
#define __DEFINED_H__

#define EVT_MY_CUSTOM_COMMAND(custom_cmd, id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( \
        custom_cmd, id, wxID_ANY, \
        wxCommandEventHandler(fn), \
        (wxObject *) NULL \
    ),

#ifdef __WXMSW__
	#define WM_USER_MSG_PROCESS_TERMINATED WM_USER + 100
	#define WM_USER_MSG_PROCESS_NEW WM_USER + 101
#else
#endif

constexpr long VIEW_STYLE = wxBORDER_NONE | wxCLIP_CHILDREN | /* wxTRANSPARENT_WINDOW  |*/ wxCLIP_SIBLINGS | wxNO_FULL_REPAINT_ON_RESIZE; 
constexpr long VIEW_STYLE_SIMPLE = wxBORDER_SIMPLE | wxCLIP_CHILDREN | /* wxTRANSPARENT_WINDOW  |*/ wxCLIP_SIBLINGS | wxNO_FULL_REPAINT_ON_RESIZE;
constexpr long IMAGELIST_FLAG = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_OVERLAYINDEX; //|SHGFI_ADDOVERLAYS

typedef struct _CPUINFO {
	unsigned int   _iCpuCoreCount  = 0;         // CPU 코어갯수
	unsigned long  _ulCPUTotalUage = 0;         // 전체 CPU 사용량
	unsigned long* _pArrayCore     = nullptr;   // CPU 코어별 사용량
	wxString _strProcessorName     = wxT("");
} CPUINFO;

typedef struct _MEMINFO {
	unsigned long long ullPhyscialAvaRam = 0;   // 사용가능 물리 메모리
	unsigned long long ullPhysicalTotalRam = 0; // 전체 물리 메모리
	unsigned long long ullVirtualAvaRam = 0;    // 사용가능 가상메모리
	unsigned long long ullVirtualTotalRam = 0;  // 전체 가상메모리	
} MEMINFO;	

typedef struct _PAGEFILEINFO {
	unsigned long long ullPageFileAva = 0;      // 사용가능 페이지파일사이즈
	unsigned long long ullPageFileTotal = 0;    // 전체 페이지 파일사이즈	
} PAGEFILEINFO;

typedef struct _SPECTRUM_INFO {
	wxRect m_rc;
	wxColour m_Colour;
} SPECTRUM_INFO;

#ifdef __WXMSW__
	typedef PROCESSENTRY32 PROCESSENTRY;

	typedef struct _PROCESS_INFO {
		HANDLE _hProcess = nullptr;
		unsigned long _ulProcessID = 0x00;
		wxString _strProcessName = wxT("");
		wxString _strProcessFullPath = wxT("");
		wxString _strFileDescription = wxT("");
		wxString _strCompanyName     = wxT("");
		wxString _strUserName        = wxT("");
		wxString _strDomainName      = wxT("");
		wxString _strDomainAndUser   = wxT("");
	//	PROCESSENTRY _piEntry;
		unsigned long long _PrivateSize = 0;
		unsigned long long _WorkingSetSize = 0;
		float fCPUUsage = 0.0f;
		
		ULARGE_INTEGER _lastCPU;
		ULARGE_INTEGER _lastSysCPU;
		ULARGE_INTEGER _lastUserCPU;
		
		__int64 nKernelTimeOld = 0;
		__int64 nUserTimeOld = 0;
		__int64  nTickCountOld = 0;
		int iIconIndex = -1;
		int iOvelayIndex = -1;
	} PROCESS_INFO;
#else
#endif

//컨텍스트 메뉴의 갯수는 30개
constexpr int CONTEXT_MENU_START = wxID_HIGHEST + 1001;
constexpr int CONTEXT_MENU_END   = CONTEXT_MENU_START + 30;

constexpr int KILL_PROCESS_MSG_NOT_PROCESSID          = -100;
constexpr int KILL_PROCESS_MSG_PROCESS_NULL           = -101;
constexpr int KILL_PROCESS_MSG_PROCESS_TERMINATE_FAIL = -102;
constexpr int KILL_PROCESS_MSG_PROCESS_ALIVE          = -103;
constexpr int KILL_PROCESS_MSG_SUCCESS = 0;

#endif
