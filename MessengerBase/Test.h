#ifndef _MSGBASE_TEST_H_
#define _MSGBASE_TEST_H_

#ifdef MESSENGERBASE_EXPORTS
#define MSGBASE_API __declspec(dllexport)
#else
#define MSGBASE_API __declspec(dllimport)
#endif

extern "C"
{
	MSGBASE_API int GetOnlineUsersStringSize();
	MSGBASE_API void GetOnlineUsersString(char* usersString);
}

#endif // _MSGBASE_TEST_H_