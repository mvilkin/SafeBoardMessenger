#include "ClientInterface.h"
#include "Client.h"
#include "compatibility.h"

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
	current_client = nullptr;
}

void SendNewMessage(char* user, char* message, OnMessageSentCallback callback)
{
	if (!current_client)
		return;

	current_client->SendNewMessage(user, message);
	auto text = current_client->MessagesToText(user);
	callback(text.c_str());
}

void StartReceiveNewMessages(char* user, OnMessageReceivedCallback callback)
{
	if (!current_client)
		return;

	current_client->StartReceivingProcess();
	bool recv_process = true;
	while (recv_process)
	{
		auto text = current_client->MessagesToText(user);
		callback(text.c_str());
		recv_process = current_client->ReadNewMessages(user);
	}
}

void StopReceiveNewMessages()
{
	if (!current_client)
		return;

	current_client->StopReceivingProcess();
}

void StartGetOnlineUsers(OnUserUpdate callback)
{
	if (!current_client)
		return;

	current_client->StartUpdatingProcess();
	while (true)
	{
		if (!current_client)
			break;
		messenger::UserList list = current_client->GetActiveUsers();
		if (list.empty())
			break;

		std::string listString;
		for (auto& user : list)
		{
			if (current_client->CheckUserNewMessages(user.identifier))
				listString += "** ";
			listString += user.identifier + ';';
		}
		listString.pop_back();

		callback(listString.c_str());
		sleep(1000);
	}
}

void StopGetOnlineUsers()
{
	if (!current_client)
		return;

	current_client->StopUpdatingProcess();
}

/*
void GetOnlineUsersString(char* users, int* users_size)
{
	if (!current_client)
		return;

	messenger::UserList list = current_client->GetActiveUsers(!users);

	std::string listString;
	for (auto& user : list)
	{
		if (current_client->CheckUserNewMessages(user.identifier))
			listString += '** ';
		listString += user.identifier + ';';
	}
	listString.pop_back();
	
	if (users_size)
		*users_size = listString.length() + 1;

	if (users)
	{
		memset(users, 0, listString.length() + 1);
		memcpy(users, listString.c_str(), listString.length());
	}
}
*/
