#include "Client.h"
#include <codecvt>
#include <fstream>
#include "utils.h"
#include "LocalHistory.h"

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
	std::string full_login = login;
	if (full_login.find('@') == std::string::npos)
		full_login += "@defualt";
	m_messenger->Login(full_login, password, securityPolicy, this);

	m_is_inited = false;
	std::unique_lock<std::mutex> lock(m_mutex_init);
	while (!m_is_inited)
	{
		m_cv_init.wait(lock);
	}

	if (m_init_result != messenger::operation_result::Ok)
	{
		ExitMessenger();
		return m_init_result;
	}

	loadLocalHistory();
	m_messenger->RegisterObserver(this);
	return m_init_result;
}

void Client::ExitMessenger()
{
	saveLocalHistory();
	m_messenger->UnregisterObserver(this);
	m_messenger->Disconnect();
	m_messenger.reset();
}

void Client::saveLocalHistory() const
{
	std::ofstream file(m_local_history_path);
	SaveMessageStatuses(file, m_map_msg_statuses);
	SaveCurrentChats(file, m_map_chat);
	SaveNewMessages(file, m_map_new_msg);
}

void Client::loadLocalHistory()
{
	std::ifstream file(m_local_history_path);
	LoadMessageStatuses(file, m_map_msg_statuses);
	LoadCurrentChats(file, m_map_chat);
	LoadNewMessages(file, m_map_new_msg);
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

messenger::message_content_type::Type Client::detectFileContentType(const messenger::Data& data)
{
	ExtensionType data_type = detectFileExtensionType(data);
	return data_type.type;
}

std::string Client::detectFileExtension(const messenger::Data& data)
{
	ExtensionType data_type = detectFileExtensionType(data);
	std::string result;

	switch (data_type.type)
	{
	case messenger::message_content_type::Image:
		{
			switch (data_type.image_type)
			{
			case ImageType::bmp: result = ".bmp"; break;
			case ImageType::jpg: result = ".jpg"; break;
			case ImageType::png: result = ".png"; break;
			case ImageType::undef:
			default:
				result = ".image_undef"; break;
			}
			break;
		}
	case messenger::message_content_type::Video:
		{
			switch (data_type.video_type)
			{
			case VideoType::avi: result = ".avi"; break;
			case VideoType::mkv: result = ".mkv"; break;
			case VideoType::undef:
			default:
				result = ".video_undef"; break;
			}
			break;
		}
	default:
		result = ".undef_undef";
		break;
	}

	return result;
}

static bool is_begining_equal(const messenger::Data& data, const std::vector<unsigned char>& base)
{
	for (unsigned i = 0; i < base.size(); ++i)
		if (data[i] != base[i])
			return false;
	return true;
}

ExtensionType Client::detectFileExtensionType(const messenger::Data& data)
{
	std::vector<unsigned char> base_bmp = { 0x42, 0x4D };
	std::vector<unsigned char> base_png = { 0x89, 0x50, 0x4E, 0x47 };
	std::vector<unsigned char> base_jpg1 = { 0xFF, 0xD8, 0xFF, 0xDB };
	std::vector<unsigned char> base_jpg2 = { 0xFF, 0xD8, 0xFF, 0xE0 };
	std::vector<unsigned char> base_jpg3 = { 0xFF, 0xD8, 0xFF, 0xE1 };
	std::vector<unsigned char> base_avi = { 0x52, 0x49, 0x46, 0x46 };
	std::vector<unsigned char> base_mkv = { 0x1A, 0x45, 0xDF, 0xA3 };

	if (is_begining_equal(data, base_bmp))
		return{ messenger::message_content_type::Image, { ImageType::bmp } };

	if (is_begining_equal(data, base_png))
		return{ messenger::message_content_type::Image, { ImageType::png } };

	if (is_begining_equal(data, base_jpg1))
		return{ messenger::message_content_type::Image, { ImageType::jpg } };

	if (is_begining_equal(data, base_jpg2))
		return{ messenger::message_content_type::Image, { ImageType::jpg } };

	if (is_begining_equal(data, base_jpg3))
		return{ messenger::message_content_type::Image, { ImageType::jpg } };

	if (is_begining_equal(data, base_avi))
		return{ messenger::message_content_type::Video, {}, { VideoType::avi } };

	if (is_begining_equal(data, base_mkv))
		return{ messenger::message_content_type::Video, {}, { VideoType::mkv } };

	return { messenger::message_content_type::Text, { ImageType::undef }, { VideoType::undef }};
}

messenger::Data Client::readFileBinary(std::string path)
{
	std::ifstream file(ConvertUTF8_UTF16(path), std::ios::binary);
	file.unsetf(std::ios::skipws);

	std::streampos size;
	file.seekg(0, std::ios::end);
	size = file.tellg();
	file.seekg(0, std::ios::beg);

	messenger::Data result;
	result.reserve(static_cast<size_t>(size));

	result.insert(result.begin(),
		std::istream_iterator<unsigned char>(file),
		std::istream_iterator<unsigned char>());

	return result;
}

std::string Client::writeFileBinary(const messenger::Data& data, time_t time)
{
	std::string extension = detectFileExtension(data);
	messenger::message_content_type::Type data_type = detectFileContentType(data);
	std::string type = data_type == messenger::message_content_type::Video ? "video" : "image";

	std::string dir_path = GetCurrentDir() + "\\" + type + "s";
	std::string file_name = dir_path + "\\" + type + "_" + std::to_string(time) + extension;

	CreateDir(ConvertUTF8_UTF16(dir_path));
	std::ofstream file(ConvertUTF8_UTF16(file_name), std::ios::binary);
	file.write(reinterpret_cast<const char*>(&data[0]), data.size());

	return file_name;
}
