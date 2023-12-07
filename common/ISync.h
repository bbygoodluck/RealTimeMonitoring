#ifndef ISYNC_H_INCLUDED
#define ISYNC_H_INCLUDED

#ifdef __WXMSW__
typedef struct Sync
{
	CRITICAL_SECTION _cs;
	Sync()		   { ::InitializeCriticalSection(&_cs); }
	void Lock()    { ::EnterCriticalSection(&_cs);      }
	void UnLock()  { ::LeaveCriticalSection(&_cs);      }
	~Sync()        { ::DeleteCriticalSection(&_cs);     }
} SYNC;
#else
#endif

#endif // ISYNC_H_INCLUDED
