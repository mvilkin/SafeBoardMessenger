#include "ClientInterface.h"
#include "Client.h"
#include "utils.h"

static Client* current_client = nullptr;

int EnterMessenger(wchar_t* login, wchar_t* password, wchar_t* server)
{
	if (!!current_client)
		return messenger::operation_result::InternalError;

	current_client = new Client;
	return current_client->EnterMessenger(ConvertUTF16_UTF8(login), ConvertUTF16_UTF8(password), ConvertUTF16_UTF8(server), 5222);
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

	current_client->SendNewMessage(ConvertUTF16_UTF8(user), ConvertUTF16_UTF8(message));
	auto text = current_client->MessagesToText(ConvertUTF16_UTF8(user));
	callback(ConvertUTF8_UTF16(text).c_str());
}

void SendNewFile(wchar_t* user, wchar_t* path, OnMessageSentCallback callback)
{
	if (!current_client)
		return;

	current_client->SendNewFile(ConvertUTF16_UTF8(user), ConvertUTF16_UTF8(path));
	auto text = current_client->MessagesToText(ConvertUTF16_UTF8(user));
	callback(ConvertUTF8_UTF16(text).c_str());
}

void StartReceiveNewMessages(wchar_t* user, OnMessageReceivedCallback callback)
{
	if (!current_client)
		return;

	current_client->StartReceivingProcess();
	bool recv_process = true;
	while (recv_process)
	{
		auto text = current_client->MessagesToText(ConvertUTF16_UTF8(user));
		callback(ConvertUTF8_UTF16(text).c_str());
		recv_process = current_client->ReadNewMessages(ConvertUTF16_UTF8(user));
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
		std::string listString = current_client->GetActiveUsersString();
		callback(ConvertUTF8_UTF16(listString).c_str());
		sleep(1000);
	}
}

void StopGetOnlineUsers()
{
	if (!current_client)
		return;

	current_client->StopUpdatingProcess();
}
