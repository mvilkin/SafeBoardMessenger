#include "Test.h"

int GetInteger()
{
	return 123;
}

void GetIntegerArg(int* value)
{
	if (value)
		*value = 12;
}

void GetString(char* buffer)
{
	if (buffer)
		*buffer = 'a';
}
