#ifndef _COMMON_H_INCLUDED
#define _COMMON_H_INCLUDED

#pragma once

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/artprov.h>
#include <wx/aui/auibook.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/display.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>

#include <unordered_map>
#include <memory>
#include <cstring>
#include <fstream>
#include <iostream>
#include <streambuf>

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
	#include <iphlpapi.h>
#endif

//버전관리
#include "Version.h"

//열거형 정의
#include "genum.h"

//메뉴 이벤트
#include "../MenuEvent.h"

//define
#include "defs.h"

//전역변수
#include "gvalue.h"

//공통함수 모음
#include "Utility.h"

//멀티쓰레딩 동기화
#include "ISync.h"

//이벤트 정의
#include "MyEventDef.h"

//시스템
#include "../system/SystemInfo.h"

//환경설정
#include "JsonConfig.h"

//SQLite
#include "../sqlite/DBProc.h"

//시스템 정보
#define theSystemInfo   (CSystemInfo::Get())

//공용함수
#define theUtility	    (CUtility::Get())

//환경설정
#define theSetting      (CJsonConfig::Get())

//SQLite Wrapper
#define theSQLite       (CDBProc::Get())
#endif // header guard

