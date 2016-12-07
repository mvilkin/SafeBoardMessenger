#include "Client.h"

Client::Client(const std::string& id) :
	m_ready(false)
{
	messenger::MessengerSettings settings;
	settings.serverUrl = "127.0.0.1";
	m_messenger = messenger::GetMessengerInstance(settings);

	messenger::SecurityPolicy securityPolicy;
	m_messenger->Login((id + "@localhost").c_str(), "", securityPolicy, this);

	std::unique_lock<std::mutex> lock(m_mutex);
	while (!m_ready)
	{
		m_cv.wait(lock);
	}

	m_messenger->RegisterObserver(this);
}

Client::~Client()
{
	m_messenger->UnregisterObserver(this);
	m_messenger->Disconnect();
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
		m_cv.wait(lock);
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
		m_cv.wait(lock);
	}
	return m_userList;
}

void Client::OnOperationResult(messenger::operation_result::Type result)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_ready = true;
	m_cv.notify_all();
}

void Client::OnOperationResult(messenger::operation_result::Type result, const messenger::UserList& users)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_userList = users;
	m_cv.notify_all();
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
	m_cv.notify_all();
}
