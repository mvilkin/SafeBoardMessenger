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
	typedef char* (__stdcall * OnMessageReceivedCallback)(const wchar_t*);
	typedef char* (__stdcall * OnMessageSentCallback)(const wchar_t*);
	typedef char* (__stdcall * OnUserUpdate)(const wchar_t*);

	MSGBASE_API int EnterMessenger(IN wchar_t* login, IN wchar_t* password, IN wchar_t* server);
	MSGBASE_API void ExitMessenger();
	MSGBASE_API void SendNewMessage(IN wchar_t* user, IN wchar_t* message, IN OnMessageSentCallback callback);
	MSGBASE_API void SendNewFile(IN wchar_t* user, IN wchar_t* path, IN OnMessageSentCallback callback);
	MSGBASE_API void StartReceiveNewMessages(IN wchar_t* user, IN OnMessageReceivedCallback callback);
	MSGBASE_API void StopReceiveNewMessages();
	MSGBASE_API void StartGetOnlineUsers(IN OnUserUpdate callback);
	MSGBASE_API void StopGetOnlineUsers();
}

#endif // _MESSENGERBASE_CLIENT_INTERFACE_H_
