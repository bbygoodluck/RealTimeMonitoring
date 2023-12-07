#include "common.h"
#include "MyEventDef.h"
//CPU /memory / pagefile 모니터링
wxDEFINE_EVENT(wxEVT_RESOURCE_MONITORING, wxCommandEvent);
//프로세스 종료
wxDEFINE_EVENT(wxEVT_TERMINATE_AND_DEL_PROCESS, wxCommandEvent);
//새로운 프로세스 생성
wxDEFINE_EVENT(wxEVT_CREATE_NEW_PROCESS, wxCommandEvent);
