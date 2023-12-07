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

#if defined(_UNICODE) || defined(UNICODE)
	#define CONVSTR(x)        (x.wchar_str())
    #define CONVSTR_TO_STD(x) (x.ToStdWstring())
#else
	#define CONVSTR(x)        (x.mb_str())
    #define CONVSTR_TO_STD(x) (x.ToStdString())
#endif

#define CONVSTR_TO_C_STR(x)   (x.c_str())

constexpr long CHILD_WINDOW_STYLE = wxBORDER_NONE | wxCLIP_CHILDREN | /* wxTRANSPARENT_WINDOW  |*/ wxCLIP_SIBLINGS | wxNO_FULL_REPAINT_ON_RESIZE;
constexpr long VIEW_STYLE = wxBORDER_NONE | wxCLIP_CHILDREN | /* wxTRANSPARENT_WINDOW  |*/ wxCLIP_SIBLINGS | wxNO_FULL_REPAINT_ON_RESIZE;
constexpr long VIEW_STYLE_SIMPLE = wxBORDER_SIMPLE | wxCLIP_CHILDREN | /* wxTRANSPARENT_WINDOW  |*/ wxCLIP_SIBLINGS | wxNO_FULL_REPAINT_ON_RESIZE;
constexpr long IMAGELIST_FLAG = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_OVERLAYINDEX; //|SHGFI_ADDOVERLAYS

const wxString SLASH = wxString(wxFILE_SEP_PATH);

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
typedef struct _PROCESS_INFO {
	HANDLE _hProcess = nullptr;
	unsigned long _ulProcessID = 0x00;
	wxString _strProcessName = wxT("");
	wxString _strProcessFullPath = wxT("");
	wxString _strFileDescription = wxT("");
	wxString _strCommandLine     = wxT("");
	wxString _strCompanyName     = wxT("");
	wxString _strUserName        = wxT("");
	wxString _strDomainName      = wxT("");
	wxString _strDomainAndUser   = wxT("");

	unsigned long long _PrivateSize = 0;
	unsigned long long _WorkingSetSize = 0;

	float fCPUUsage = 0.0f;

	int iIconIndex = -1;
	int iOvelayIndex = -1;

	FILETIME _ftSysKernel;
	FILETIME _ftSysUser;
	FILETIME _ftProcKernel;
	FILETIME _ftProcUser;

	FILETIME _ftLastCPU;
	FILETIME _ftLastSysCPU;
	FILETIME _ftLastUserCPU;

	ULARGE_INTEGER _lastCPU;
	ULARGE_INTEGER _lastSysCPU;
	ULARGE_INTEGER _lastUserCPU;

} PROCESS_INFO, *PPROCESS_INFO;
#else
#endif

typedef struct _PROCESS_CPU_USAGE_TO_DB {
	float    _cpuUsage        = 0.0f;
	wxString _processName     = wxT("");
	wxString _fullPath        = wxT("");
	wxString _fileDescription = wxT("");
	wxString _companyName     = wxT("");
	wxString _commandLine     = wxT("");
} PROCESS_CPU_USAGE_TO_DB, *PPROCESS_CPU_USAGE_TO_DB;

typedef struct _NET_ADAPTER_INFO {
	wxString _adapterName = wxT("");
	wxString _description = wxT("");
	wxString _ip          = wxT("");
	int _adapterIndex     = 0;
	int _dhcpEnable       = 0;
} NET_ADAPTER_INFO, *PNET_ADAPTER_INFO;

//컨텍스트 메뉴의 갯수는 30개
constexpr int CONTEXT_MENU_START = wxID_HIGHEST + 1001;
constexpr int CONTEXT_MENU_END   = CONTEXT_MENU_START + 30;

constexpr int KILL_PROCESS_MSG_NOT_PROCESSID          = -100;
constexpr int KILL_PROCESS_MSG_PROCESS_NULL           = -101;
constexpr int KILL_PROCESS_MSG_PROCESS_TERMINATE_FAIL = -102;
constexpr int KILL_PROCESS_MSG_PROCESS_ALIVE          = -103;
constexpr int KILL_PROCESS_MSG_SUCCESS = 0;

static int wxTIMER_ID           = 100;

const double KILOBYTE = 1024.0;
const double MEGABYTE = (1024.0 * 1024.0);
const double GIGABYTE = (1024.0 * 1024.0 * 1024.0);
const double TERABYTE = (1024.0 * 1024.0 * 1024.0 * 1024.0);
#endif
