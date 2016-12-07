#include "ClientInterface.h"
#include "Client.h"

static Client* its_me = nullptr;

void EnterMessenger(char* login, char* password, char* server)
{
	if (!its_me)
		its_me = new Client(login);
}

void SendMessage(char* to, char* text)
{
	its_me->SendMessage(to, text);
}

void RecvMessage(OnMessageReceivedCallback callback)
{
	while (true)
	{
		auto text = its_me->ReceiveMessage();
		callback(text.c_str());
	}
}

void GetOnlineUsersString(char* usersString, int* usersStringSize)
{
	if (!its_me)
		return;

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
