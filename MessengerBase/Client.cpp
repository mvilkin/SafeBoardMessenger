#include "Client.h"

Client::Client() :
	m_ready(false),
	m_enter_res(messenger::operation_result::Ok)
{
}

Client::~Client()
{
	if (m_messenger)
		ExitMessenger();
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

	messenger::Message msg_info = m_messenger->SendMessage(user, message);


	MyMessageInfo info;
	info.message.identifier = msg_info.identifier;
	info.message.time = msg_info.time;
	info.message.content.type = msg_info.content.type;
	info.message.content.encrypted = msg_info.content.encrypted;
	info.message.content.data = msg_info.content.data;
	info.status = messenger::message_status::Sending;
	m_map_chat[user].push_back(info);
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

void Client::ReadNewMessages(std::string fromUserId)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_map_new_msg[fromUserId].empty())
	{
		m_cv_msg.wait(lock);
	}

	for (auto& msg : m_map_new_msg[fromUserId])
	{
		m_messenger->SendMessageSeen(fromUserId, msg.message.identifier);
		msg.status = messenger::message_status::Seen;
	}

	m_map_chat[fromUserId].insert(m_map_chat[fromUserId].end(), 
		std::make_move_iterator(m_map_new_msg[fromUserId].begin()), 
		std::make_move_iterator(m_map_new_msg[fromUserId].end()));

	m_map_new_msg[fromUserId].clear();
}

std::string Client::MessagesToText(std::string fromUserId)
{
	std::string result;

	for (auto& msg : m_map_chat[fromUserId])
	{
		std::string text;
		text.assign(reinterpret_cast<const char*>(&msg.message.content.data[0]), msg.message.content.data.size());
		result += text + "\n";
	}

	return result;
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

	MyMessageInfo info;
	info.message.identifier = msg.identifier;
	info.message.time = msg.time;
	info.message.content.type = msg.content.type;
	info.message.content.encrypted = msg.content.encrypted;
	info.message.content.data = msg.content.data;
	info.status = messenger::message_status::Delivered;
	m_map_new_msg[senderId].push_back(info);

	m_cv_msg.notify_all();
}
