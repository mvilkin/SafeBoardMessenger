#include "Client.h"
#include <codecvt>

Client::Client() :
	m_inited(false),
	m_status_changed(false),
	m_recv_process(false),
	m_update_process(false),
	m_enter_res(messenger::operation_result::Ok)
{
	setlocale(LC_ALL, "UTF-8");
}

Client::~Client()
{
	if (m_messenger)
		ExitMessenger();
}

int Client::EnterMessenger(std::wstring login_utf16, std::wstring password_utf16, std::wstring server_utf16)
{
	std::string login = WideToNormal(login_utf16);
	std::string password = WideToNormal(password_utf16);
	std::string server = WideToNormal(server_utf16);

	messenger::MessengerSettings settings;
	settings.serverUrl = server;
	settings.serverPort = 5222; // xero
	m_messenger = messenger::GetMessengerInstance(settings);

	messenger::SecurityPolicy securityPolicy;
	std::string full_login = login;
	if (login.find_first_of('@') == std::string::npos)
		full_login += "@localhost";
	m_messenger->Login(full_login, password, securityPolicy, this);

	m_inited = false;
	std::unique_lock<std::mutex> lock(m_mutex);
	while (!m_inited)
	{
		m_cv_init.wait(lock);
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

void Client::SendNewMessage(std::wstring user_utf16, std::wstring message_utf16)
{
	std::string user = WideToNormal(user_utf16);
	std::string message = WideToNormal(message_utf16);

	messenger::MessageContent message_content;
	message_content.type = messenger::message_content_type::Text;
	std::copy(message.begin(), message.end(), std::back_inserter(message_content.data));

	messenger::Message msg = m_messenger->SendMessage(user, message_content);

	m_map_chat[user].push_back(msg);
}

void Client::StartUpdatingProcess()
{
	m_update_process = true;
}

void Client::StopUpdatingProcess()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_update_process = false;
	m_cv_usr.notify_all();
}

messenger::UserList Client::GetActiveUsers()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_userList.clear();
	while (m_userList.empty() && m_update_process)
	{
		m_messenger->RequestActiveUsers(this);
		m_cv_usr.wait(lock);
	}
	
	if (!m_update_process)
		m_userList.clear();

	return m_userList;
}

std::wstring Client::GetActiveUsersString()
{
	messenger::UserList list = GetActiveUsers();
	if (list.empty())
		return std::wstring();

	std::string listString;
	for (auto& user : list)
	{
		if (CheckUserNewMessages(user.identifier))
			listString += "** ";
		listString += user.identifier + ';';
	}
	listString.pop_back();

	return NormalToWide(listString);
}

bool Client::CheckUserNewMessages(messenger::UserId user)
{
	return !m_map_new_msg[user].empty();
}

void Client::StartReceivingProcess()
{
	m_recv_process = true;
}

void Client::StopReceivingProcess()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_recv_process = false;
	m_cv_msg.notify_all();
}

bool Client::ReadNewMessages(std::wstring user_utf16)
{
	std::string user = WideToNormal(user_utf16);

	std::unique_lock<std::mutex> lock(m_mutex);
	m_status_changed = false;
	while (m_map_new_msg[user].empty() && !m_status_changed && m_recv_process)
	{
		m_cv_msg.wait(lock);
	}

	for (auto& msg : m_map_new_msg[user])
	{
		m_messenger->SendMessageSeen(user, msg.identifier);
		m_map_msg_statuses[msg.identifier] = messenger::message_status::Seen;
	}

	m_map_chat[user].insert(m_map_chat[user].end(),
		std::make_move_iterator(m_map_new_msg[user].begin()),
		std::make_move_iterator(m_map_new_msg[user].end()));

	m_map_new_msg[user].clear();

	return m_recv_process;
}

std::wstring Client::MessagesToText(std::wstring user_utf16)
{
	std::wstring result;
	std::string user = WideToNormal(user_utf16);

	for (auto& msg : m_map_chat[user])
	{
		std::string text_utf8;
		text_utf8.assign(reinterpret_cast<const char*>(&msg.content.data[0]), msg.content.data.size());
		std::wstring text = NormalToWide(text_utf8);
		if (m_map_msg_statuses[msg.identifier] == messenger::message_status::FailedToSend)
			text += L" *** Failed to send ***";
		else if (m_map_msg_statuses[msg.identifier] != messenger::message_status::Seen)
			text += L" *** Not seen yet ***";
		result += text + L"\n";
	}

	return result;
}

void Client::OnOperationResult(messenger::operation_result::Type result)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_enter_res = result;
	m_inited = true;
	m_cv_init.notify_all();
}

void Client::OnOperationResult(messenger::operation_result::Type result, const messenger::UserList& users)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	if (result == messenger::operation_result::Ok)
	{
		m_userList = users;
	}
	else
	{
		m_userList.push_back(messenger::User());
	}
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

std::wstring Client::NormalToWide(std::string string_to_convert)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(string_to_convert.c_str());
}

std::string Client::WideToNormal(std::wstring string_to_convert)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(string_to_convert);
}