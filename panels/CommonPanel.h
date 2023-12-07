#ifndef _COMMON_PALEN_H_INCLUDED
#define _COMMON_PALEN_H_INCLUDED

class CCommonPanel
{
public:
	CCommonPanel() {}
	~CCommonPanel() {}
	
public:
	virtual void UpdateData() = 0; 
};
#endif