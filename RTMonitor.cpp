#include "ginc.h"
#include "RTMonitor.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(CRealTimeMonitor);

CRealTimeMonitor::CRealTimeMonitor()
{

}

CRealTimeMonitor::~CRealTimeMonitor()
{

}

bool CRealTimeMonitor::OnInit()
{
	theCommonUtil->LoadImageList();
#ifdef NDEBUG
	m_pSingleInstance = new wxSingleInstanceChecker;
	if (m_pSingleInstance->IsAnotherRunning())
	{
		delete m_pSingleInstance;
		m_pSingleInstance = NULL;

		wxMessageBox(wxT("이미 실행중입니다."), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return false;
	}
#endif

	if (!wxApp::OnInit())
	{
		wxMessageBox(wxT("윈도우즈 초기화 실패"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	wxString strPGTitle = PROGRAM_FULL_NAME + ENV_OS_VER;

#ifndef NDEBUG
	strPGTitle += wxT(" - Debug Version");
#endif

	strPGTitle += wxT(" (Made by BaeByoungYong)");
	CMainFrame* pMainFrame = new CMainFrame(strPGTitle);
	pMainFrame->Show();

	_gMainWindow = (wxWindow *)pMainFrame;
	this->SetTopWindow(pMainFrame);

//	theSystemInfo->WatcherStart();
	return true;
}

int CRealTimeMonitor::OnExit()
{
	return 0;
}