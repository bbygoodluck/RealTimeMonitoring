#include "global.h"
#include "MyEventDef.h"
//새로운 프로세스 생성
wxDEFINE_EVENT(wxEVT_CREATE_NEW_PROCESS, wxCommandEvent);
//프로세스 종료
wxDEFINE_EVENT(wxEVT_TERMINATE_AND_DEL_PROCESS, wxCommandEvent);
