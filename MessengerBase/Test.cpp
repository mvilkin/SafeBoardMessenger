#include "Test.h"
#include <string>

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
