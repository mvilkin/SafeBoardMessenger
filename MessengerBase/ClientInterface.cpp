#include "ClientInterface.h"
#include "Client.h"
#include "compatibility.h"

static Client* current_client = nullptr;

int EnterMessenger(wchar_t* login, wchar_t* password, wchar_t* server)
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

void SendNewMessage(wchar_t* user, wchar_t* message, OnMessageSentCallback callback)
{
	if (!current_client)
		return;

	current_client->SendNewMessage(user, message);
	auto text = current_client->MessagesToText(user);
	callback(text.c_str());
}

void StartReceiveNewMessages(wchar_t* user, OnMessageReceivedCallback callback)
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
		std::wstring listString = current_client->GetActiveUsersString();
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
