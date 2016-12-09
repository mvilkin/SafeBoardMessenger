#include "ClientInterface.h"
#include "Client.h"

static Client* its_me;

int EnterMessenger(char* login, char* password, char* server)
{
	its_me = new Client;
	return its_me->EnterMessenger(login, password, server);
}

void ExitMessenger()
{
	return its_me->ExitMessenger();
}

void SendMessage(char* to, char* msg, OnMessageSentCallback callback)
{
	its_me->SendMessage(to, msg);
	auto text = its_me->MessagesToText(to);
	callback(text.c_str());
}

void ReceiveNewMessages(char* from, OnMessageReceivedCallback callback)
{
	while (true)
	{
		auto text = its_me->MessagesToText(from);
		callback(text.c_str());
		its_me->ReadNewMessages(from);
	}
}

void GetOnlineUsersString(char* usersString, int* usersStringSize)
{
	messenger::UserList list = its_me->GetActiveUsers(!usersString);

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
