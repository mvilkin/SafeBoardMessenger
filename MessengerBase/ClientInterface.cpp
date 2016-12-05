#include "ClientInterface.h"
#include "Client.h"

static Client* its_me = NULL;
static Client* add = NULL;

void AddCli()
{
	add = new Client("add");
}

void SendMessage(char* to, char* text)
{
	its_me->SendMessage(to, text);
}

void RecvMessage(char* text)
{
	auto txt = add->ReceiveMessage();
	int size = txt.length();
	memset(text, 0, size + 1);
	memcpy(text, txt.c_str(), size);
}

void GetOnlineUsersString(char* usersString, int* usersStringSize)
{
	if (!its_me)
		its_me = new Client("me");

	static messenger::UserList list;
	if (!usersString)
		list = its_me->GetActiveUsers(true);

	std::string listString;
	for (auto& user : list)
	{
		listString += user.identifier + ';';
	}
	listString.pop_back();
	
	if (usersStringSize)
		*usersStringSize = listString.length() + 1;

	if (usersString)
	{
		memset(usersString, 0, listString.length() + 1);
		memcpy(usersString, listString.c_str(), listString.length());
	}
}
