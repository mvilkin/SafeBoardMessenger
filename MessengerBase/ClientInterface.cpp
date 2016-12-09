#include "ClientInterface.h"
#include "Client.h"

static Client* current_client = nullptr;

int EnterMessenger(char* login, char* password, char* server)
{
	if (!!current_client)
		return messenger::operation_result::InternalError;

	current_client = new Client;
	return current_client->EnterMessenger(login, password, server);
}

void ExitMessenger()
{
	if (!current_client)
		return;

	current_client->ExitMessenger();
	delete current_client;
}

void SendMessage(char* to, char* msg, OnMessageSentCallback callback)
{
	if (!current_client)
		return;

	current_client->SendMessage(to, msg);
	auto text = current_client->MessagesToText(to);
	callback(text.c_str());
}

void StartReceiveNewMessages(char* from, OnMessageReceivedCallback callback)
{
	if (!current_client)
		return;

	current_client->StartReceivingProcess();
	bool recv_process = true;
	while (recv_process)
	{
		auto text = current_client->MessagesToText(from);
		callback(text.c_str());
		recv_process = current_client->ReadNewMessages(from);
	}
}

void StopReceiveNewMessages()
{
	if (!current_client)
		return;

	current_client->StopReceivingProcess();
}

void GetOnlineUsersString(char* usersString, int* usersStringSize)
{
	if (!current_client)
		return;

	messenger::UserList list = current_client->GetActiveUsers(!usersString);

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
