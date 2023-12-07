#include "../../ginc.h"
#include "WatchProcess.h"
#include "../system.h"

//#include "ProcessInfoImpl.h"
#include <strsafe.h>
void CALLBACK OnProcessExited(_In_ CWatchProcessImpl* const process, _In_ BOOLEAN TimerOrWaitFired);

class CWatchProcessImpl
{
public:
	explicit CWatchProcessImpl(CWatchProcess* pOwner, unsigned long ulProcessID)
		: m_ulProcessID(ulProcessID)
		, m_pOwner(pOwner)
	{
		Init();
	}

	~CWatchProcessImpl()
	{
		if (m_hWait)
		{
			CloseHandle(m_hProcess);
			::UnregisterWaitEx(m_hWait, NULL); // INVALID_HANDLE_VALUE means "Wait for pending callbacks"
			m_hWait = NULL;
		}
	}

	unsigned long GetProcessID() { return m_ulProcessID; }
	CWatchProcess* GetOwner() { return m_pOwner;}

public:
	bool AddRegisterWaitFor()
	{
		BOOL result = RegisterWaitForSingleObject(&m_hWait, m_hProcess, (WAITORTIMERCALLBACK)OnProcessExited, this, INFINITE, WT_EXECUTEONLYONCE);
		return result;
	}

private:
	void Init()
	{
		m_hProcess = OpenProcess(SYNCHRONIZE, FALSE, m_ulProcessID);
	}

private:
	HANDLE m_hProcess;
	HANDLE m_hWait;
	unsigned long m_ulProcessID;
	CWatchProcess* m_pOwner;
};

void CALLBACK OnProcessExited(
	_In_ CWatchProcessImpl* const process,
	_In_ BOOLEAN TimerOrWaitFired)
{
	EnterCriticalSection(&_gCoutAccess);
	unsigned long ulProcessID = process->GetProcessID();
	CWatchProcess* pWatcher = process->GetOwner();
	if(pWatcher)
	{
		if(!pWatcher->IsTerminate())
			pWatcher->DoDelProcess(ulProcessID);
	}

	LeaveCriticalSection(&_gCoutAccess);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/*wxBEGIN_EVENT_TABLE(CWatchProcess, wxEvtHandler)
	EVT_TIMER(CWatchProcess::PROCESS_WATCHID, CWatchProcess::OnTimer)
wxEND_EVENT_TABLE()*/

CWatchProcess::CWatchProcess()
//	: m_iTimerMillSec(1000)
{
	InitializeCriticalSection(&_gCoutAccess);
//	m_timer.SetOwner(this, PROCESS_WATCHID);
	 // while it's executed, so we use a thread to do it.
    if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
    {
        wxMessageBox(wxT("Could not create the worker thread!"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
    }
}

CWatchProcess::~CWatchProcess()
{

}

void CWatchProcess::StartWatch()
{
//	m_timer.Start(m_iTimerMillSec);
	GetThread()->Run();
}

void CWatchProcess::StopWatch()
{
//	theSystemInfo->SetPGTerminte(true);
	m_bStop = true;
	DeleteCriticalSection(&_gCoutAccess);
//	if (m_timer.IsRunning())
//		m_timer.Stop();
	if(GetThread() && GetThread()->IsRunning())
		GetThread()->Wait();

	std::unordered_map<unsigned long, CWatchProcessImpl *>::iterator iter;
	for (iter = m_processExitWait.begin(); iter != m_processExitWait.end(); ++iter)
	{
		CWatchProcessImpl* pProcess = iter->second;
		delete pProcess;

		pProcess = nullptr;
	}

	m_processExitWait.clear();
}

/*void CWatchProcess::OnTimer(wxTimerEvent & event)
{
	DoCheckNewProcess();
}*/

wxThread::ExitCode CWatchProcess::Entry()
{
	while(1)
	{
		DoCheckNewProcess();
		wxSleep(1.7);

		if(m_bStop)
			break;
	}

	return (wxThread::ExitCode)0;
}

void CWatchProcess::DoCheckNewProcess()
{
#ifdef __WXMSW__
	/*
	EnumProcesses()함수의 경우 읽어들일 프로세스의 수를 입력해야 하므로 프로세스의 갯수가 많은 시스템의 경우는 제한적일 수 있으므로 SnapShot을 이용하여 처리
	*/

	HANDLE hServer = WTS_CURRENT_SERVER_HANDLE;    // get local machine processes
    unsigned long ulLevel = 1;                     // get array of WTS_PROCESS_INFO_EX
    unsigned long ulCount = 0;                     // returns the number of processes
    PWTS_PROCESS_INFO pProcessInfo = nullptr;     // output data

	if(WTSEnumerateProcesses(hServer, 0, ulLevel, &pProcessInfo, &ulCount))
	{
		for(unsigned long i = 0; i < ulCount; i++)
		{
			PROCESS_INFO* processInfo = theSystemInfo->GetProcessInfo()->GetInfo(pProcessInfo[i].ProcessId);
			if(processInfo == nullptr)
			{
				EnterCriticalSection(&_gCoutAccess);
				theSystemInfo->GetProcessInfo()->AddProcessInfo(pProcessInfo[i], true);
				LeaveCriticalSection(&_gCoutAccess);
			}
		}
	}

	WTSFreeMemory(pProcessInfo);
	pProcessInfo = nullptr;

#else
#endif
}

bool CWatchProcess::CheckAndAddProcessWatch(unsigned long ulProcessID)
{
	if(m_bStop)
		return false;

	std::unordered_map<unsigned long, CWatchProcessImpl *>::const_iterator citer = m_processExitWait.find(ulProcessID);
	if(citer != m_processExitWait.end())
		return false;

	CWatchProcessImpl* pProcess = new CWatchProcessImpl(this, ulProcessID);
	if (!pProcess->AddRegisterWaitFor())
	{
		delete pProcess;
		return false;
	}

	std::unordered_map<unsigned long, CWatchProcessImpl *>::value_type val(ulProcessID, pProcess);
	m_processExitWait.insert(val);

	return true;
}

void CWatchProcess::DoDelProcess(unsigned long ulProcessID)
{
	std::unordered_map<unsigned long, CWatchProcessImpl *>::const_iterator citer = m_processExitWait.find(ulProcessID);
	if(citer == m_processExitWait.end())
		return;

	CWatchProcessImpl* pProcess = citer->second;
	delete pProcess;

	m_processExitWait.erase(citer);
	theSystemInfo->GetProcessInfo()->DelProcess(ulProcessID);
}
