#include "Client.h"
#include <codecvt>
#include <fstream>
#include "utils.h"

Client::Client() :
	m_is_inited(false),
	m_init_result(messenger::operation_result::Ok),
	m_status_changed(false),
	m_recv_process(false),
	m_update_process(false)
{
	setlocale(LC_ALL, "UTF-8");
}

Client::~Client()
{
	if (m_messenger)
		ExitMessenger();
}

int Client::EnterMessenger(std::string login, std::string password, std::string server, int port)
{
	messenger::MessengerSettings settings;
	settings.serverUrl = server;
	settings.serverPort = port;
	m_messenger = messenger::GetMessengerInstance(settings);

	messenger::SecurityPolicy securityPolicy;
	m_messenger->Login(login, password, securityPolicy, this);

	m_is_inited = false;
	std::unique_lock<std::mutex> lock(m_mutex_init);
	while (!m_is_inited)
	{
		m_cv_init.wait(lock);
	}

	m_messenger->RegisterObserver(this);
	return m_init_result;
}

void Client::ExitMessenger()
{
	m_messenger->UnregisterObserver(this);
	m_messenger->Disconnect();
	m_messenger.reset();
}

void Client::SendNewMessage(messenger::UserId user_id, std::string message)
{
	messenger::MessageContent message_content;
	message_content.type = messenger::message_content_type::Text;
	std::copy(message.begin(), message.end(), std::back_inserter(message_content.data));

	messenger::Message msg = m_messenger->SendMessage(user_id, message_content);

	m_map_chat[user_id].push_back(msg);
}

void Client::SendNewFile(messenger::UserId user_id, std::string path)
{
	messenger::MessageContent message_content;
	message_content.type = messenger::message_content_type::Image;
	message_content.data = readFileBinary(path);

	messenger::Message msg = m_messenger->SendMessage(user_id, message_content);

	msg.content.data.clear();
	std::string info = "You sent file: ";
	std::copy(info.begin(), info.end(), std::back_inserter(msg.content.data));
	std::copy(path.begin(), path.end(), std::back_inserter(msg.content.data));
	m_map_chat[user_id].push_back(msg);
}

void Client::StartReceivingProcess()
{
	m_recv_process = true;
}

void Client::StopReceivingProcess()
{
	std::unique_lock<std::mutex> lock(m_mutex_msg);
	m_recv_process = false;
	m_cv_msg.notify_all();
}

bool Client::ReadNewMessages(messenger::UserId user_id)
{
	std::unique_lock<std::mutex> lock(m_mutex_msg);
	m_status_changed = false;
	while (m_map_new_msg[user_id].empty() && !m_status_changed && m_recv_process)
	{
		m_cv_msg.wait(lock);
	}

	for (auto& msg : m_map_new_msg[user_id])
	{
		m_messenger->SendMessageSeen(user_id, msg.identifier);
		m_map_msg_statuses[msg.identifier] = messenger::message_status::Seen;

		if (msg.content.type != messenger::message_content_type::Text)
		{
			std::string path_file = writeFileBinary(msg.content.data, msg.time);
			msg.content.data.clear();
			std::string info = "You received file: ";
			std::copy(info.begin(), info.end(), std::back_inserter(msg.content.data));
			std::copy(path_file.begin(), path_file.end(), std::back_inserter(msg.content.data));
			msg.content.type = messenger::message_content_type::Text;
		}
	}

	m_map_chat[user_id].insert(m_map_chat[user_id].end(),
		std::make_move_iterator(m_map_new_msg[user_id].begin()),
		std::make_move_iterator(m_map_new_msg[user_id].end()));

	m_map_new_msg[user_id].clear();

	return m_recv_process;
}

std::string Client::MessagesToText(messenger::UserId user_id)
{
	std::string result;
	std::unique_lock<std::mutex> lock(m_mutex_msg);

	for (auto& msg : m_map_chat[user_id])
	{
		std::string text;
		text.assign(reinterpret_cast<const char*>(&msg.content.data[0]), msg.content.data.size());
		if (m_map_msg_statuses[msg.identifier] == messenger::message_status::FailedToSend)
			text += " *** Failed to send ***";
		else if (m_map_msg_statuses[msg.identifier] != messenger::message_status::Seen)
			text += " *** Not seen yet ***";
		result += text + "\n";
	}

	return result;
}

void Client::StartUpdatingProcess()
{
	m_update_process = true;
}

void Client::StopUpdatingProcess()
{
	std::unique_lock<std::mutex> lock(m_mutex_usr);
	m_update_process = false;
	m_cv_usr.notify_all();
}

messenger::UserList Client::GetActiveUsers()
{
	std::unique_lock<std::mutex> lock(m_mutex_usr);
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

std::string Client::GetActiveUsersString()
{
	messenger::UserList list = GetActiveUsers();
	if (list.empty())
		return std::string();

	std::string listString;
	for (auto& user : list)
	{
		if (CheckUserNewMessages(user.identifier))
			listString += "** ";
		listString += user.identifier + ';';
	}
	listString.pop_back();

	return listString;
}

bool Client::CheckUserNewMessages(messenger::UserId user)
{
	return !m_map_new_msg[user].empty();
}

void Client::OnOperationResult(messenger::operation_result::Type result)
{
	std::unique_lock<std::mutex> lock(m_mutex_init);
	m_init_result = result;
	m_is_inited = true;
	m_cv_init.notify_all();
}

void Client::OnOperationResult(messenger::operation_result::Type result, const messenger::UserList& users)
{
	std::unique_lock<std::mutex> lock(m_mutex_usr);
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

void Client::OnMessageStatusChanged(const messenger::MessageId& message_id, messenger::message_status::Type status)
{
	std::unique_lock<std::mutex> lock(m_mutex_msg);
	m_map_msg_statuses[message_id] = status;
	if (status == messenger::message_status::Seen || status == messenger::message_status::FailedToSend)
		m_status_changed = true;
	m_cv_msg.notify_all();
}

void Client::OnMessageReceived(const messenger::UserId& user_id, const messenger::Message& message)
{
	std::unique_lock<std::mutex> lock(m_mutex_msg);

	messenger::Message msg;
	msg.identifier = message.identifier;
	msg.time = message.time;
	msg.content.type = message.content.type;
	msg.content.encrypted = message.content.encrypted;
	msg.content.data = message.content.data;
	m_map_new_msg[user_id].push_back(msg);

	m_cv_msg.notify_all();
}

messenger::Data Client::readFileBinary(std::string path)
{
	std::ifstream file(path, std::ios::binary);
	file.unsetf(std::ios::skipws);

	std::streampos size;
	file.seekg(0, std::ios::end);
	size = file.tellg();
	file.seekg(0, std::ios::beg);

	messenger::Data result;
	result.reserve(size);

	result.insert(result.begin(),
		std::istream_iterator<unsigned char>(file),
		std::istream_iterator<unsigned char>());

	return result;
}

std::string Client::writeFileBinary(const messenger::Data& data, time_t time)
{
	std::string dir_path = GetCurrentDir() + "\\images";
	std::string file_name = dir_path + "\\image_" + std::to_string(time) + ".png";

	CreateDir(dir_path);
	std::ofstream file(file_name, std::ios::binary);
	file.write(reinterpret_cast<const char*>(&data[0]), data.size());

	return file_name;
}
