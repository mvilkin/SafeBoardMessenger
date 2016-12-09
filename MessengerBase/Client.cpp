#include "Client.h"

Client::Client() :
	m_ready(false),
	m_status_changed(false),
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

	m_map_chat[user].push_back(msg_info);
}

messenger::UserList Client::GetActiveUsers(bool update)
{
	if (update)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_userList.clear();
		m_messenger->RequestActiveUsers(this);
		while (m_userList.empty())
		{
			m_cv_usr.wait(lock);
		}
	}
	return m_userList;
}

void Client::ReadNewMessages(std::string fromUserId)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_status_changed = false;
	while (m_map_new_msg[fromUserId].empty() && !m_status_changed)
	{
		m_cv_msg.wait(lock);
	}

	for (auto& msg : m_map_new_msg[fromUserId])
	{
		m_messenger->SendMessageSeen(fromUserId, msg.identifier);
		m_map_msg_statuses[msg.identifier] = messenger::message_status::Seen;
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
		text.assign(reinterpret_cast<const char*>(&msg.content.data[0]), msg.content.data.size());
		if (m_map_msg_statuses[msg.identifier] == messenger::message_status::FailedToSend)
			text += " *** Fail sending ***";
		else if (m_map_msg_statuses[msg.identifier] != messenger::message_status::Seen)
			text += " *** Not seen yet ***";
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
	m_map_msg_statuses[msgId] = status;
	if (status == messenger::message_status::Seen || status == messenger::message_status::FailedToSend)
		m_status_changed = true;
	m_cv_msg.notify_all();
}

void Client::OnMessageReceived(const messenger::UserId& senderId, const messenger::Message& msg)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	messenger::Message message;
	message.identifier = msg.identifier;
	message.time = msg.time;
	message.content.type = msg.content.type;
	message.content.encrypted = msg.content.encrypted;
	message.content.data = msg.content.data;
	m_map_new_msg[senderId].push_back(message);

	m_cv_msg.notify_all();
}
