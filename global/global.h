#pragma once

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/aui/auibook.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/display.h>
#include <unordered_map>
#include <memory>
#include <deque>

//using namespace std;

#ifdef __WXMSW__
	#include <wx/msw/private.h>
	#include <windows.h>	
	#include <TlHelp32.h>
	#include <Psapi.h>
	#include <CommCtrl.h>
	#include <winver.h>
	#include <shellapi.h>
	#include <shlobj.h>
	#include <Propkey.h>
	#include <wtsapi32.h>
	#include <aclapi.h>
#endif

#include "Version.h"
#include "genum.h"
#include "defs.h"
#include "gvalue.h"
//공통함수 모음
#include "CommonUtil.h"

//이벤트 정의
#include "MyEventDef.h"

#include "../MonitoringImpl.h"
#include "../system/SystemInfo.h"
#include "../PanelManagerImpl.h"

#define theMonitoring   (CMonitoringImpl::Get())
#define theSystemInfo   (CSystemInfo::Get())
#define thePanelManager (CPanelManagerImpl::Get())
//공용함수
#define theCommonUtil	    (CCommonUtil::Get())