#ifndef __VERSION_H__
#define __VERSION_H__

/********************************************************************************
	class Name :
	설      명 : 프로그램 버전
*********************************************************************************/
const wxString PROGRAM_NAME = wxT("RealTime Monitoring");
const wxString PROGRAM_MAJOR_VER = wxT("1");
const wxString PROGRAM_MINOR_HI_VER = wxT("0");
const wxString PROGRAM_MINOR_LO_VER = wxT("0");
const wxString PROGRAM_VERSION = wxT(" Ver ") + PROGRAM_MAJOR_VER + wxT(".") + PROGRAM_MINOR_HI_VER + wxT(".") + PROGRAM_MINOR_LO_VER;
const wxString PROGRAM_FULL_NAME = PROGRAM_NAME + PROGRAM_VERSION;

#ifdef __WXMSW__
	#ifdef _WIN64
		const wxString ENV_OS_VER = wxT(" - x64");
	#else
		const wxString ENV_OS_VER = wxT(" - x86");
	#endif
#endif
#endif
