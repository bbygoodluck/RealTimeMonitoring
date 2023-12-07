#ifndef _MY_EVENT_DEFINE_H_INCLUDED
#define _MY_EVENT_DEFINE_H_INCLUDED

/********************************************************************************
// 설      명 : 사용자 정의 이벤트들
*********************************************************************************/
//CPU /memory / pagefile 모니터링
wxDECLARE_EVENT(wxEVT_RESOURCE_MONITORING, wxCommandEvent);
//프로세스 종료
wxDECLARE_EVENT(wxEVT_TERMINATE_AND_DEL_PROCESS, wxCommandEvent);
//새로운 프로세스 생성
wxDECLARE_EVENT(wxEVT_CREATE_NEW_PROCESS, wxCommandEvent);

#endif
