#include "common.h"
#include "gvalue.h"

wxWindow* _gMainWindow = nullptr;
CRITICAL_SECTION _gCoutAccess;
//CRITICAL_SECTION _gListCtrlDelSection;

// 이미지리스트
SYSTEM_IMAGE_LIST _gImageList;

// 전역뷰폰트
wxFont* _gViewFont = nullptr;

//메뉴 이벤트
CMenuEvent* _gMenuEvent = nullptr;
