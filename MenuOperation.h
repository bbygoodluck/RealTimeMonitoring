#ifndef MENUOPERATION_H_INCLUDED
#define MENUOPERATION_H_INCLUDED

class CMenuEvent;
class CMenuOperation final
{
public:
	CMenuOperation(CMenuEvent* pMenuEvent);
	~CMenuOperation();

public:
	void ExecuteMenuOperation(int iMenuID, _MENU_EVENT_TYPE _mType);

private:
	void Init();

	//파일메뉴
	void FileMenu_NewJobExec();
	void FileMenu_Terminate();

	//도구메뉴
	void ToolMenu_SetEnv();
	void ToolMenu_CurrentSave();
private:
	std::unordered_map< int, std::function<void()> > m_mapMenuFunctionPtr;
	CMenuEvent* m_pMenuEvent = nullptr;
};

#endif // MENUOPERATION_H_INCLUDED
