#ifndef __SYTEMINFOIMPL_H_INCLUDED
#define __SYTEMINFOIMPL_H_INCLUDED

class CSystemInfoImpl
{
public:
	CSystemInfoImpl() {};
	~CSystemInfoImpl() {};
	
public:
	virtual void UpdateInfo() = 0;
};
#endif
