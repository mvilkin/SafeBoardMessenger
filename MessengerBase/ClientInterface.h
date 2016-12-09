#ifndef _MESSENGERBASE_CLIENT_INTERFACE_H_
#define _MESSENGERBASE_CLIENT_INTERFACE_H_

#ifdef MESSENGERBASE_EXPORTS
# define MSGBASE_API __declspec(dllexport)
#else
# define MSGBASE_API __declspec(dllimport)
#endif

#ifndef IN
# define IN
# define OUT
#endif

extern "C"
{
	typedef char* (__stdcall * OnMessageReceivedCallback)(const char*);
	typedef char* (__stdcall * OnMessageSentCallback)(const char*);

	MSGBASE_API int EnterMessenger(IN char* login, IN char* password, IN char* server);
	MSGBASE_API void ExitMessenger();
	MSGBASE_API void SendMessage(IN char* user, IN char* message, IN OnMessageSentCallback callback);
	MSGBASE_API void StartReceiveNewMessages(IN char* user, IN OnMessageReceivedCallback callback);
	MSGBASE_API void StopReceiveNewMessages();
	MSGBASE_API void GetOnlineUsersString(OUT char* users, OUT int* usersStringSize);
}

#endif // _MESSENGERBASE_CLIENT_INTERFACE_H_
