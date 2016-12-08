#include "Client.h"

Client::Client() :
	m_ready(false),
	m_enter_res(messenger::operation_result::Ok)
{
}

Client::~Client()
{
}

int Client::EnterMessenger(const std::string& login, const std::string& password, const std::string& server)
{
	messenger::MessengerSettings settings;
	settings.serverUrl = "127.0.0.1";
	m_messenger = messenger::GetMessengerInstance(settings);

	messenger::SecurityPolicy securityPolicy;
	m_messenger->Login((login + "@localhost").c_str(), "", securityPolicy, this);

	m_ready = false;
	std::unique_lock<std::mutex> lock(m_mutex);
	while (!m_ready)
	{
		m_cv.wait(lock);
	}

	m_messenger->RegisterObserver(this);

	return m_enter_res;
}

void Client::ExitMessenger()
{
	m_messenger->UnregisterObserver(this);
	m_messenger->Disconnect();
	m_messenger.reset();
}

void Client::SendMessage(std::string user, std::string msg)
{
	messenger::MessageContent message;
	message.type = messenger::message_content_type::Text;
	std::copy(msg.begin(), msg.end(), std::back_inserter(message.data));
	m_messenger->SendMessage(user, message);
}

std::string Client::ReceiveMessage()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_receivedMsg.empty())
	{
		m_cv_msg.wait(lock);
	}

	std::string res = m_receivedMsg;
	m_receivedMsg.clear();
	return res;
}

messenger::UserList Client::GetActiveUsers(bool update)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_userList.clear();
	m_messenger->RequestActiveUsers(this);
	while (m_userList.empty())
	{
		m_cv_usr.wait(lock);
	}
	return m_userList;
}

void Client::OnOperationResult(messenger::operation_result::Type result)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_enter_res = result;
	m_ready = true;
	m_cv.notify_all();
}

void Client::OnOperationResult(messenger::operation_result::Type result, const messenger::UserList& users)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_userList = users;
	m_cv_usr.notify_all();
}

void Client::OnMessageStatusChanged(const messenger::MessageId& msgId, messenger::message_status::Type status)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_receivedMsg = "<error>";
	m_cv.notify_all();
}

void Client::OnMessageReceived(const messenger::UserId& senderId, const messenger::Message& msg)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_receivedMsg.assign(reinterpret_cast<const char*>(&msg.content.data[0]), msg.content.data.size());
	m_cv_msg.notify_all();
}
