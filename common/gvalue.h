#ifndef __GLOBAL_VALUES_H__
#define __GLOBAL_VALUES_H__

extern wxWindow* _gMainWindow;
extern CRITICAL_SECTION _gCoutAccess;
//extern CRITICAL_SECTION _gListCtrlDelSection;

#ifdef __WXMSW__
	typedef HIMAGELIST SYSTEM_IMAGE_LIST;
#else
	typedef wxImageList* SYSTEM_IMAGE_LIST;
#endif

// 이미지리스트
extern SYSTEM_IMAGE_LIST _gImageList;

// 전역뷰폰트
extern wxFont* _gViewFont;

//메뉴 이벤트
extern CMenuEvent* _gMenuEvent;
#endif
