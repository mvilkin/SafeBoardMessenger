#ifndef _MSGBASE_TEST_H_
#define _MSGBASE_TEST_H_

#ifdef MESSENGERBASE_EXPORTS
#define MSGBASE_API __declspec(dllexport)
#else
#define MSGBASE_API __declspec(dllimport)
#endif

extern "C"
{
	MSGBASE_API int GetInteger();
	MSGBASE_API void GetIntegerArg(int* value);
	MSGBASE_API void GetString(char* buffer);
}

#endif // _MSGBASE_TEST_H_