#ifndef _REAL_TIME_MONITORING_H_INCLUDED_
#define _REAL_TIME_MONITORING_H_INCLUDED_

//***************************************************************************************
// Program : RealTimeMonitor
// Make by : Bae Byoung Yong
// Date    : 2020.09.04 ~
//***************************************************************************************
#include <wx/snglinst.h>
class CRealTimeMonitor : public wxApp
{
public:
	explicit CRealTimeMonitor();
	virtual ~CRealTimeMonitor();

	virtual bool OnInit() wxOVERRIDE;
	virtual int OnExit() wxOVERRIDE;

private:
	wxSingleInstanceChecker* m_pSingleInstance;
	bool m_bAddFontRC = false;
};
#endif
