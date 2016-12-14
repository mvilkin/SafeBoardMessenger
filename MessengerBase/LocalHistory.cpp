#include "LocalHistory.h"
#include "utils.h"

static std::string msg_statuses_end = "End msg statuses";
static std::string current_chat_end = "End current chat";
static std::string new_messages_end = "End new messages";

static void save_message(std::ofstream& file, const messenger::Message& message)
{
	file << message.identifier << "\n";
	file << message.time << "\n";
	file << Base64_encode(message.content.data) << "\n";
}

static messenger::Message load_message(std::ifstream& file)
{
	messenger::Message message;

	std::getline(file, message.identifier);

	std::string time_str;
	std::getline(file, time_str);
	message.time = std::stoll(time_str);

	std::string data_base64;
	std::getline(file, data_base64);
	message.content.data = Base64_decode(data_base64);

	return message;
}

void SaveMessageStatuses(std::ofstream& file, const std::unordered_map<messenger::MessageId, messenger::message_status::Type>& statuses)
{
	for (auto& status : statuses)
	{
		file << status.first << "\n";
		file << status.second << "\n";
	}
	file << msg_statuses_end << "\n";
}

void SaveCurrentChats(std::ofstream& file, const std::unordered_map<messenger::UserId, std::vector<MessageInfo> >& chat)
{
	for (auto& record : chat)
	{
		file << record.first << "\n";
		file << record.second.size() << "\n";
		for (auto& msg : record.second)
		{
			file << msg.type << "\n";
			save_message(file, msg.message);
		}
	}
	file << current_chat_end << "\n";
}

void SaveNewMessages(std::ofstream& file, const std::unordered_map<messenger::UserId, std::vector<MessageInfo> >& new_messages)
{
	for (auto& record : new_messages)
	{
		file << record.first << "\n";
		file << record.second.size() << "\n";
		for (auto& msg : record.second)
		{
			file << msg.type << "\n";
			save_message(file, msg.message);
		}
	}
	file << new_messages_end << "\n";
}

void LoadMessageStatuses(std::ifstream& file, std::unordered_map<messenger::MessageId, messenger::message_status::Type>& statuses)
{
	std::string message_id;
	std::getline(file, message_id);
	while (message_id != msg_statuses_end)
	{
		std::string type_str;
		std::getline(file, type_str);
		statuses[message_id] = static_cast<messenger::message_status::Type>(std::stoi(type_str));

		std::getline(file, message_id);
	}
}

void LoadCurrentChats(std::ifstream& file, std::unordered_map<messenger::UserId, std::vector<MessageInfo> >& chat)
{
	std::string user_id;
	std::getline(file, user_id);
	while (user_id != current_chat_end)
	{
		std::string num_records_str;
		std::getline(file, num_records_str);
		size_t num_records = std::stoi(num_records_str);

		std::vector<MessageInfo> records;
		while (num_records--)
		{
			MessageInfo info;
			std::string type_str;
			std::getline(file, type_str);
			info.type = static_cast<MessageDirection::Type>(std::stoi(type_str));
			info.message = load_message(file);
			records.push_back(std::move(info));
		}
		chat[user_id] = records;

		std::getline(file, user_id);
	}
}

void LoadNewMessages(std::ifstream& file, std::unordered_map<messenger::UserId, std::vector<MessageInfo> >& new_messages)
{
	std::string user_id;
	std::getline(file, user_id);
	while (user_id != new_messages_end)
	{
		std::string num_records_str;
		std::getline(file, num_records_str);
		size_t num_records = std::stoi(num_records_str);

		std::vector<MessageInfo> records;
		while (num_records--)
		{
			MessageInfo info;
			std::string type_str;
			std::getline(file, type_str);
			info.type = static_cast<MessageDirection::Type>(std::stoi(type_str));
			info.message = load_message(file);
			records.push_back(std::move(info));
		}
		new_messages[user_id] = records;

		std::getline(file, user_id);
	}
}
