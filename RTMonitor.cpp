#include "ginc.h"
#include "RTMonitor.h"
#include "MainFrame.h"

#include <wx/fontenum.h>
#include <wx/busyinfo.h>

wxIMPLEMENT_APP(CRealTimeMonitor);

CRealTimeMonitor::CRealTimeMonitor()
{

}

CRealTimeMonitor::~CRealTimeMonitor()
{

}

bool CRealTimeMonitor::OnInit()
{
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

	wxBusyInfo info
    (
        wxBusyInfoFlags()
            .Parent(nullptr)
            .Icon(wxArtProvider::GetIcon(wxART_INFORMATION,
                                         wxART_OTHER, wxSize(64, 64)))
            .Title("<b>Initialize System resource...</b>")
            .Text("please wait...")
            .Foreground(*wxWHITE)
            .Background(wxColour(100, 100, 100))
            .Transparency(5 * (wxALPHA_OPAQUE / 5))
    );


	//이미지 리스트 로딩
	theUtility->LoadImageList();

	//시스템 정보 초기화
	theSystemInfo->Initialize();

	if(!theSetting->Load())
	{
		wxMessageBox(wxT("Config file load failed"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

#ifdef __WXMSW__
	//확장자 명령 설정 셋팅
	wxString strCurrentDir(theUtility->GetWorkDir());

	//폰트 설정
	wxString strFixedsysFontPath(wxT(""));
	strFixedsysFontPath = strCurrentDir + SLASH + wxT("rc") + SLASH + wxT("hvgafix.fon");// wxT("FSEX300.ttf");

	//등록하고자 하는 폰트가 존재하는지 체크
	wxArrayString arrFontList = wxFontEnumerator::GetFacenames();
	bool bFontExist = false;
	for (auto item : arrFontList)
	{
		if (item.Cmp(wxT("Fixedsys")) == 0)
		{
			bFontExist = true;
			break;
		}
	}

	if (!bFontExist)
	{
		int iErr = AddFontResource(CONVSTR(strFixedsysFontPath));
		if (iErr != 0)
			m_bAddFontRC = true;
	}

	_gViewFont = new wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Fixedsys"));
#else
	_gViewFont = new wxFont(wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT));
#endif // __WXMSW__
	wxString strPGTitle = PROGRAM_FULL_NAME + ENV_OS_VER;

#ifndef NDEBUG
	strPGTitle += wxT(" - Debug Version");
#endif

	long style = wxDEFAULT_FRAME_STYLE;
	int iTaskbarShow = theSetting->GetTaskBarShow();
	if(!iTaskbarShow)
		style = style | wxFRAME_NO_TASKBAR;

	strPGTitle += wxT(" (Made by BaeByoungYong)");

	wxString strDBPath = strCurrentDir + SLASH + wxT("db");

	wxFileName fn;
	if(!fn.DirExists(strDBPath))
		fn.Mkdir(strDBPath);

	wxString strDBName = theSetting->GetDBName();
	if(strDBName.IsEmpty())
		strDBName = wxT("ProcessList.db");

	wxString strDBPathName = strDBPath + SLASH + strDBName;
	if(!fn.FileExists(strDBPathName))
		theSQLite->CreateDB(strDBPath, strDBName);

	theSQLite->OpenDB(strDBPath, strDBName);

	CMainFrame* pMainFrame = new CMainFrame(strPGTitle, style);
	pMainFrame->Show();

	_gMainWindow = (wxWindow *)pMainFrame;
	this->SetTopWindow(pMainFrame);
	return true;
}

int CRealTimeMonitor::OnExit()
{
	return 0;
}
