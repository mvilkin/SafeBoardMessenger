#ifndef _MSGBASE_TEST_H_
#define _MSGBASE_TEST_H_

#ifdef MESSENGERBASE_EXPORTS
#define MSGBASE_API __declspec(dllexport)
#else
#define MSGBASE_API __declspec(dllimport)
#endif

#include <string>

extern "C"
{
	MSGBASE_API int GetOnlineUsersStringSize();
	MSGBASE_API void GetOnlineUsersString(char* usersString);
}

int GetOnlineUsersStringSize()
{
	return std::string("User1;User2;User3").length() + 1;
}

void GetOnlineUsersString(char* usersString)
{
	int len = std::string("User1;User2;User3").length();
	memset(usersString, 0, len + 1);
	memcpy(usersString, "User1;User2;User3", len);
}

#endif // _MSGBASE_TEST_H_