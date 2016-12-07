#ifndef _MESSENGERBASE_CLIENT_INTERFACE_H_
#define _MESSENGERBASE_CLIENT_INTERFACE_H_

#ifdef MESSENGERBASE_EXPORTS
#define MSGBASE_API __declspec(dllexport)
#else
#define MSGBASE_API __declspec(dllimport)
#endif

#include <string>

extern "C"
{
	MSGBASE_API void EnterMessenger(char* login, char* password, char* server);
	MSGBASE_API void GetOnlineUsersString(char* usersString, int* usersStringSize);
	MSGBASE_API void SendMessage(char* to, char* text);
	MSGBASE_API void RecvMessage(char* text);
	MSGBASE_API void AddCli();
}

#endif // _MESSENGERBASE_CLIENT_INTERFACE_H_