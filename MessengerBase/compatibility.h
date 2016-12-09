#ifndef _MESSENGERBASE_COMPATIBILITY_H_
#define _MESSENGERBASE_COMPATIBILITY_H_

#ifdef _WIN32
#include <Windows.h>
inline void sleep(int msec)
{
	Sleep(msec);
}
#endif

#endif // _MESSENGERBASE_COMPATIBILITY_H_
